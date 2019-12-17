//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Saransh Saxena
//

// System includes

// External includes

// Project includes
#include "custom_processes/simple_error_calculator_process.h"
#include "convection_diffusion_application_variables.h"
#include "utilities/variable_utils.h"

namespace Kratos
{

template <std::size_t TDim>
SimpleErrorCalculatorProcess<TDim>::SimpleErrorCalculatorProcess(ModelPart &rThisModelPart, Parameters ThisParameters) : mrThisModelPart(rThisModelPart)
{
    //WIP
    //mrThisModelPart.AddNodalSolutionStepVariable(NODAL_TEMP_GRADIENT);
    //mrThisModelPart.AddNodalSolutionStepVariable(NODAL_ERROR_PROJ);

    Parameters default_parameters = Parameters(R"(
    {
        "minimal_size"                        : 0.01,
        "maximal_size"                        : 10.0,
        "refinement_strategy"                 : "Simple_Error_Calculator",
        "reference_variable_name"             : "ERROR_RATIO",
        "echo_level"                          : 0
    })");

    ThisParameters.ValidateAndAssignDefaults(default_parameters);
    mMinSize = ThisParameters["minimal_size"].GetDouble();
    mMaxSize = ThisParameters["maximal_size"].GetDouble();
    mEchoLevel = ThisParameters["echo_level"].GetInt();
    mReferenceVariable = ThisParameters["reference_variable_name"].GetString();
}

template <std::size_t TDim>
void SimpleErrorCalculatorProcess<TDim>::Execute()
{
    // Initialize the metric
    // a) Check for Metric Scalar in Meshing Application
    KRATOS_ERROR_IF_NOT(KratosComponents<Variable<double>>::Has("METRIC_SCALAR")) << "Import Meshing Application" << std::endl;
    const Variable<double> &scalar_variable = KratosComponents<Variable<double>>::Get("METRIC_SCALAR");

    // c) Initialize Metric Scalar and Nodal Temperature Gradient for all nodes to 0
    VariableUtils().SetNonHistoricalVariableToZero(scalar_variable, mrThisModelPart.Nodes());
    VariableUtils()
        .SetNonHistoricalVariableToZero(NODAL_TEMP_GRADIENT, mrThisModelPart.Nodes());
    VariableUtils()
        .SetNonHistoricalVariableToZero(NODAL_ERROR_PROJ, mrThisModelPart.Nodes());

    Vector nodal_area;
    KRATOS_INFO(this->Info()) << "Calculating Nodal Area for " << mrThisModelPart.NumberOfNodes() << " nodes in " << mrThisModelPart.Name() << "\n";
    this->CalculateNodalArea(nodal_area);

    KRATOS_INFO(this->Info()) << "Calculating Nodal Temperature Gradient for " << mrThisModelPart.NumberOfNodes() << " nodes in " << mrThisModelPart.Name() << "\n";

    // d) Call the Nodal Temperature Gradient Calculator Function
    const int number_nodes = mrThisModelPart.NumberOfNodes();
    const int number_elements = mrThisModelPart.NumberOfElements();

    KRATOS_WATCH(number_nodes);
    Matrix nodal_grad = ZeroMatrix(number_nodes, 3);
    KRATOS_WATCH(nodal_grad);

    // Loop over the elements
    for (unsigned int i_elem = 0; i_elem < number_elements; i_elem++)
    {
        ModelPart::ElementType &r_element = *(mrThisModelPart.ElementsBegin() + i_elem);
        ModelPart::ElementType::GeometryType &r_geometry = r_element.GetGeometry();
        const auto n_nodes = r_geometry.PointsNumber();

        Vector GaussWeights;
        Vector DetJ;
        Matrix ShapeFunctions;
        ShapeFunctionDerivativesArrayType ShapeDerivatives;
        unsigned int NumGPoints = 0;
        
        const GeometryType::IntegrationPointsArrayType &integration_points = r_geometry.IntegrationPoints(GeometryData::GI_GAUSS_1);
        NumGPoints = integration_points.size();
        const auto n_nodes = r_geometry.size();

        r_geometry.ShapeFunctionsIntegrationPointsGradients(ShapeDerivatives, DetJ, GeometryData::GI_GAUSS_1);

        if (ShapeFunctions.size1() != NumGPoints || ShapeFunctions.size2() != n_nodes)
        {
            ShapeFunctions.resize(NumGPoints, n_nodes, false);
        }
        ShapeFunctions = r_geometry.ShapeFunctionsValues(GeometryData::GI_GAUSS_1);

        if (GaussWeights.size() != NumGPoints)
        {
            GaussWeights.resize(NumGPoints, false);
        }

        for (unsigned int g = 0; g < NumGPoints; g++)
        {
            GaussWeights[g] = DetJ[g] * integration_points[g].Weight();
        }

        for (unsigned int g = 0; g < NumGPoints; g++)
        {
            const Matrix &rDN_DX = ShapeDerivatives[g];
            const Vector &Ncontainer = row(ShapeFunctions, g);

            Vector GaussPointTGrad(3, 0.0);

            for (unsigned int j = 0; j < TDim; j++)
            {
                for (unsigned int i_node = 0; i_node < n_nodes; i_node++)
                {
                    GaussPointTGrad[j] += rDN_DX(i_node, j) * r_geometry[i_node].FastGetSolutionStepValue(TEMPERATURE);
                }
                GaussPointTGrad[j] *= GaussWeights[g];
            }

            for (int i_node = 0; i_node < n_nodes; i_node++)
            {
                for (int j = 0; j < TDim; j++)
                {
                    nodal_grad(r_geometry[i_node].Id() - 1, j) += Ncontainer[i_node] * GaussPointTGrad[j] / nodal_area[r_geometry[i_node].Id() - 1];
                }
            }
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < number_nodes; i++)
    {
        ModelPart::NodeType &r_node = *(mrThisModelPart.NodesBegin() + i);
        const Vector &nodal_grad_row = row(nodal_grad, i);
        r_node.SetValue(NODAL_TEMP_GRADIENT, nodal_grad_row);
    }

    // e) Call the Nodal Error Function
    KRATOS_INFO(this->Info()) << "Calculating Nodal Error for " << number_nodes << " nodes in " << mrThisModelPart.Name() << "\n";
    double global_gw = 0.0;
    double global_del_sigma = 0.0;

    Vector element_del_sigma = ZeroVector(number_elements);
    Vector element_error_norm_squared = ZeroVector(number_elements);
    Vector element_gw = ZeroVector(number_elements);
    Vector nodal_error_proj = ZeroVector(number_nodes);
    Matrix element_error = ZeroMatrix(number_elements, 3);

    // c) Loop over the elements to calculate Element Error
    for (int i_elem = 0; i_elem < number_elements; i_elem++)
    {
        ModelPart::ElementType &r_element = *(mrThisModelPart.ElementsBegin() + i_elem);
        ModelPart::ElementType::GeometryType &r_geometry = r_element.GetGeometry();
        const auto n_nodes = r_geometry.PointsNumber();

        Vector GaussWeights;
        Vector DetJ;
        Matrix ShapeFunctions;
        ShapeFunctionDerivativesArrayType ShapeDerivatives;
        unsigned int NumGPoints = 0;
        const GeometryType::IntegrationPointsArrayType &integration_points = r_geometry.IntegrationPoints(GeometryData::GI_GAUSS_1);
        NumGPoints = integration_points.size();
        const auto n_nodes = r_geometry.size();

        r_geometry.ShapeFunctionsIntegrationPointsGradients(ShapeDerivatives, DetJ, GeometryData::GI_GAUSS_1);

        if (ShapeFunctions.size1() != NumGPoints || ShapeFunctions.size2() != n_nodes)
        {
            ShapeFunctions.resize(NumGPoints, n_nodes, false);
        }
        ShapeFunctions = r_geometry.ShapeFunctionsValues(GeometryData::GI_GAUSS_1);

        if (GaussWeights.size() != NumGPoints)
        {
            GaussWeights.resize(NumGPoints, false);
        }

        for (unsigned int g = 0; g < NumGPoints; g++)
        {
            GaussWeights[g] = DetJ[g] * integration_points[g].Weight();
        }

        for (unsigned int g = 0; g < NumGPoints; g++)
        {
            element_gw[i_elem] += GaussWeights[g];
            global_gw += GaussWeights[g];
            const Matrix &rDN_DX = ShapeDerivatives[g];
            const Vector &Ncontainer = row(ShapeFunctions, g);

            Vector GaussPointTGrad(3, 0.0);
            Vector NodalTempGrad(3, 0.0);

            for (unsigned int j = 0; j < TDim; j++)
            {
                for (unsigned int i_node = 0; i_node < n_nodes; i_node++)
                {
                    GaussPointTGrad[j] += rDN_DX(i_node, j) * r_geometry[i_node].FastGetSolutionStepValue(TEMPERATURE);
                }
                GaussPointTGrad[j] *= GaussWeights[g];
            }

            for (unsigned int i_node = 0; i_node < n_nodes; i_node++)
            {
                NodalTempGrad[0] += Ncontainer[i_node] * r_geometry[i_node].GetValue(NODAL_TEMP_GRADIENT_X);
                NodalTempGrad[1] += Ncontainer[i_node] * r_geometry[i_node].GetValue(NODAL_TEMP_GRADIENT_Y);
                if (TDim == 3)
                {
                    NodalTempGrad[2] += Ncontainer[i_node] * r_geometry[i_node].GetValue(NODAL_TEMP_GRADIENT_Z);
                }
            }

            for (unsigned int j = 0; j < TDim; j++)
            {
                NodalTempGrad[j] *= GaussWeights[g];
                element_error(i_elem, j) += (NodalTempGrad[j] - GaussPointTGrad[j]);
            }
        }

        for (unsigned int j = 0; j < TDim; j++)
        {
            element_error_norm_squared[i_elem] += element_error(i_elem, j) * element_error(i_elem, j);
        }
        element_del_sigma[i_elem] = element_error_norm_squared[i_elem] / element_gw[i_elem];
        global_del_sigma += element_error_norm_squared[i_elem];
    }
    global_del_sigma = global_del_sigma / global_gw;

    // d) Loop over the elements to calculate Nodal Error Projections
    for (int i_elem = 0; i_elem < number_elements; i_elem++)
    {
        ModelPart::ElementType &r_element = *(mrThisModelPart.ElementsBegin() + i_elem);
        ModelPart::ElementType::GeometryType &r_geometry = r_element.GetGeometry();
        const auto n_nodes = r_geometry.PointsNumber();

        Vector GaussWeights;
        Vector DetJ;
        Matrix ShapeFunctions;
        ShapeFunctionDerivativesArrayType ShapeDerivatives;
        unsigned int NumGPoints = 0;
        const GeometryType::IntegrationPointsArrayType &integration_points = r_geometry.IntegrationPoints(GeometryData::GI_GAUSS_1);
        NumGPoints = integration_points.size();
        const auto n_nodes = r_geometry.size();

        r_geometry.ShapeFunctionsIntegrationPointsGradients(ShapeDerivatives, DetJ, GeometryData::GI_GAUSS_1);

        if (ShapeFunctions.size1() != NumGPoints || ShapeFunctions.size2() != n_nodes)
        {
            ShapeFunctions.resize(NumGPoints, n_nodes, false);
        }
        ShapeFunctions = r_geometry.ShapeFunctionsValues(GeometryData::GI_GAUSS_1);

        if (GaussWeights.size() != NumGPoints)
        {
            GaussWeights.resize(NumGPoints, false);
        }

        for (unsigned int g = 0; g < NumGPoints; g++)
        {
            GaussWeights[g] = DetJ[g] * integration_points[g].Weight();
        }

        const Vector &Ncontainer = row(ShapeFunctions, 0);

        for (unsigned int i_node = 0; i_node < n_nodes; i_node++)
        {
            r_geometry[i_node].GetValue(NODAL_ERROR_PROJ) += Ncontainer[i_node] * element_del_sigma[i_elem] / (global_del_sigma * nodal_area[r_geometry[i_node].Id() - 1]);
        }
    }

    // e) Call the Metric Scalar Function
    //CalculateMetricScalar();
}

template <std::size_t TDim>
void SimpleErrorCalculatorProcess<TDim>::CalculateNodalArea(Vector &nodal_area) const
{
    const int number_nodes = mrThisModelPart.NumberOfNodes();
    const int number_elements = mrThisModelPart.NumberOfElements();

    if (nodal_area.size() != number_nodes)
    {
        nodal_area.resize(number_nodes);
        nodal_area.clear();
    }

    // b) Loop over Elements and calculate nodal area
    for (int i_elem = 0; i_elem < number_elements; ++i_elem)
    {
        const ModelPart::ElementType &r_element = *(mrThisModelPart.ElementsBegin() + i_elem);
        const ModelPart::ElementType::GeometryType &r_geometry = r_element.GetGeometry();
        const int n_nodes = r_geometry.PointsNumber();

        for (int i_node = 0; i_node < n_nodes; i_node++)
        {
            nodal_area[r_geometry[i_node].Id() - 1] += r_geometry.Area() / n_nodes;
        }
    }
}

/***********************************************************************************/
/***********************************************************************************/

template class SimpleErrorCalculatorProcess<2>;
template class SimpleErrorCalculatorProcess<3>;

}; // namespace Kratos