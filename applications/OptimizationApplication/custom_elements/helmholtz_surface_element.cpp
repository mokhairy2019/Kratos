//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   license: OptimizationApplication/license.txt
//
//  Main authors:    Reza Najian Asl
//

// System includes


// External includes


// Project includes
#include "optimization_application_variables.h"
#include "includes/checks.h"
#include "includes/define.h"
#include "utilities/math_utils.h"

// Application incldues
#include "custom_elements/helmholtz_surface_element.h"
#include "custom_utilities/entity_calculation_utils.h"

namespace Kratos
{

//************************************************************************************
//************************************************************************************

HelmholtzSurfaceElement::HelmholtzSurfaceElement(IndexType NewId, GeometryType::Pointer pGeometry)
    : Element(NewId, pGeometry)
{
    //DO NOT ADD DOFS HERE!!!
    mpSolidGeometry = EntityCalculationUtils::CreateSolidGeometry(this->GetGeometry());
}

//************************************************************************************
//************************************************************************************

HelmholtzSurfaceElement::HelmholtzSurfaceElement(IndexType NewId, GeometryType::Pointer pGeometry,  PropertiesType::Pointer pProperties)
    : Element(NewId, pGeometry, pProperties)
{
    //DO NOT ADD DOFS HERE!!!
    mpSolidGeometry = EntityCalculationUtils::CreateSolidGeometry(this->GetGeometry());
}

Element::Pointer HelmholtzSurfaceElement::Create(IndexType NewId, NodesArrayType const& ThisNodes,  PropertiesType::Pointer pProperties) const
{
    return Kratos::make_intrusive<HelmholtzSurfaceElement>(NewId, GetGeometry().Create(ThisNodes), pProperties);
}

/***********************************************************************************/
/***********************************************************************************/

Element::Pointer HelmholtzSurfaceElement::Create(IndexType NewId, GeometryType::Pointer pGeom,  PropertiesType::Pointer pProperties) const
{
    return Kratos::make_intrusive<HelmholtzSurfaceElement>(NewId, pGeom, pProperties);
}

/***********************************************************************************/
/***********************************************************************************/

HelmholtzSurfaceElement::~HelmholtzSurfaceElement()
{
}

/***********************************************************************************/
/***********************************************************************************/

Element::Pointer HelmholtzSurfaceElement::Clone (
    IndexType NewId,
    NodesArrayType const& rThisNodes
    ) const
{
    KRATOS_TRY

    HelmholtzSurfaceElement::Pointer p_new_elem = Kratos::make_intrusive<HelmholtzSurfaceElement>(NewId, GetGeometry().Create(rThisNodes), pGetProperties());
    p_new_elem->SetData(this->GetData());
    p_new_elem->Set(Flags(*this));

    return p_new_elem;

    KRATOS_CATCH("");
}

//************************************************************************************
//************************************************************************************
void HelmholtzSurfaceElement::CalculateLocalSystem(MatrixType& rLeftHandSideMatrix,
                                            VectorType& rRightHandSideVector,
                                            const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    KRATOS_ERROR_IF_NOT(rCurrentProcessInfo.Has(COMPUTE_HELMHOLTZ_INVERSE))
    << "COMPUTE_HELMHOLTZ_INVERSE not defined in the ProcessInfo!" << std::endl;

    KRATOS_ERROR_IF_NOT(rCurrentProcessInfo.Has(HELMHOLTZ_INTEGRATED_FIELD))
    << "HELMHOLTZ_INTEGRATED_FIELD not defined in the ProcessInfo!" << std::endl;

    KRATOS_ERROR_IF_NOT(rCurrentProcessInfo.Has(HELMHOLTZ_RADIUS))
    << "HELMHOLTZ_RADIUS not defined in the ProcessInfo!" << std::endl;

    auto& r_geometry = this->GetGeometry();
    const SizeType number_of_nodes = r_geometry.size();

    // Resizing as needed the LHS
    const SizeType mat_size = number_of_nodes;

    if ( rLeftHandSideMatrix.size1() != mat_size )
        rLeftHandSideMatrix.resize( mat_size, mat_size, false );

    noalias( rLeftHandSideMatrix ) = ZeroMatrix( mat_size, mat_size ); //resetting LHS

    // Resizing as needed the RHS
    if ( rRightHandSideVector.size() != mat_size )
        rRightHandSideVector.resize( mat_size, false );

    rRightHandSideVector = ZeroVector( mat_size ); //resetting RHS

    MatrixType M;
    CalculateMassMatrix(M,rCurrentProcessInfo);

    MatrixType K;
    CalculateStiffnessMatrix(K,rCurrentProcessInfo);

    const bool is_inversed = rCurrentProcessInfo[COMPUTE_HELMHOLTZ_INVERSE];

    noalias(rLeftHandSideMatrix) += M;
    if(!is_inversed)
        noalias(rLeftHandSideMatrix) += K;

    const unsigned int number_of_points = r_geometry.size();
    Vector nodal_vals(number_of_points);
    const bool is_integrated_field = rCurrentProcessInfo[HELMHOLTZ_INTEGRATED_FIELD];
    for(unsigned int node_element = 0; node_element<number_of_points; node_element++)
    {
        const auto &source = r_geometry[node_element].GetValue(HELMHOLTZ_SCALAR_SOURCE);
        auto node_weight = r_geometry[node_element].GetValue(NUMBER_OF_NEIGHBOUR_ELEMENTS);
        nodal_vals[node_element] = source;
        if(is_integrated_field)
            nodal_vals[node_element] /= node_weight;
    }

    if(is_integrated_field)
        noalias(rRightHandSideVector) += nodal_vals;
    else if (is_inversed)
        noalias(rRightHandSideVector) += prod(K+M,nodal_vals);
    else
        noalias(rRightHandSideVector) += prod(M,nodal_vals);

    //apply drichlet BC
    Vector temp;
    GetValuesVector(temp,0);
    noalias(rRightHandSideVector) -= prod(rLeftHandSideMatrix,temp);

    KRATOS_CATCH("")
}

//******************************************************************************
//******************************************************************************
void HelmholtzSurfaceElement::GetValuesVector(VectorType &rValues,
                                            int Step) const {
  const GeometryType &rgeom = this->GetGeometry();
  const SizeType num_nodes = rgeom.PointsNumber();
  const unsigned int local_size = num_nodes;

  if(rValues.size() != local_size)
    rValues.resize(local_size, false);

  SizeType index = 0;
  for (SizeType i_node = 0; i_node < num_nodes; ++i_node)
    rValues[index++] = rgeom[i_node].FastGetSolutionStepValue(HELMHOLTZ_SCALAR, Step);
}

//************************************************************************************
//************************************************************************************
void HelmholtzSurfaceElement::CalculateLeftHandSide(MatrixType& rLeftHandSideMatrix, const ProcessInfo& rCurrentProcessInfo)
{
    VectorType temp(0);
    CalculateLocalSystem(rLeftHandSideMatrix, temp, rCurrentProcessInfo);
}

//************************************************************************************
//************************************************************************************
void HelmholtzSurfaceElement::CalculateRightHandSide(VectorType& rRightHandSideVector, const ProcessInfo& rCurrentProcessInfo)
{
    MatrixType temp(0,0);
    CalculateLocalSystem(temp, rRightHandSideVector, rCurrentProcessInfo);
}

//************************************************************************************
//************************************************************************************
void HelmholtzSurfaceElement::EquationIdVector(EquationIdVectorType& rResult, const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY;

    unsigned int number_of_nodes = GetGeometry().PointsNumber();
    if (rResult.size() != number_of_nodes)
        rResult.resize(number_of_nodes, false);

    for (unsigned int i = 0; i < number_of_nodes; i++)
        rResult[i] = GetGeometry()[i].GetDof(HELMHOLTZ_SCALAR).EquationId();

    KRATOS_CATCH("")

}

//************************************************************************************
//************************************************************************************
void HelmholtzSurfaceElement::GetDofList(DofsVectorType& rElementalDofList,const ProcessInfo& rCurrentProcessInfo) const
{

    KRATOS_TRY;

    unsigned int number_of_nodes = GetGeometry().PointsNumber();

    if (rElementalDofList.size() != number_of_nodes)
        rElementalDofList.resize(number_of_nodes);

    for (unsigned int i = 0; i < number_of_nodes; i++)
        rElementalDofList[i] = GetGeometry()[i].pGetDof(HELMHOLTZ_SCALAR);

    KRATOS_CATCH("")

}

//************************************************************************************
//************************************************************************************
int HelmholtzSurfaceElement::Check(const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY;

    int check = Element::Check(rCurrentProcessInfo);

    const auto& r_geometry = GetGeometry();
    const SizeType number_of_nodes = r_geometry.size();

    // Check that the element's nodes contain all required SolutionStepData and Degrees of freedom
    for ( IndexType i = 0; i < number_of_nodes; i++ ) {
        const NodeType &rnode = r_geometry[i];
        KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(HELMHOLTZ_SCALAR,rnode)
        KRATOS_CHECK_DOF_IN_NODE(HELMHOLTZ_SCALAR, rnode)
    }

    return check;

    KRATOS_CATCH( "" );
}
/***********************************************************************************/
/***********************************************************************************/

void HelmholtzSurfaceElement::CalculateMassMatrix(
    MatrixType& rMassMatrix,
    const ProcessInfo& rCurrentProcessInfo
    )
{

    KRATOS_TRY;

    const auto& r_geom = GetGeometry();
    SizeType number_of_nodes = r_geom.size();
    SizeType mat_size = number_of_nodes;

    // Clear matrix
    if (rMassMatrix.size1() != mat_size || rMassMatrix.size2() != mat_size)
        rMassMatrix.resize( mat_size, mat_size, false );
    rMassMatrix = ZeroMatrix( mat_size, mat_size );

    const IntegrationMethod& integration_method = r_geom.GetDefaultIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& integration_points = r_geom.IntegrationPoints( integration_method );

    MatrixType Ncontainer;
    GetPseudoBulkSurfaceShapeFunctionsValues(Ncontainer,integration_method,rCurrentProcessInfo);


    for ( IndexType point_number = 0; point_number < integration_points.size(); ++point_number ) {
        const double detJ0 = r_geom.DeterminantOfJacobian(point_number,integration_method);
        const double integration_weight = integration_points[point_number].Weight() * detJ0;
        const Vector& rN = row(Ncontainer,point_number);

        noalias(rMassMatrix) += integration_weight * outer_prod(rN,rN);
    }

    KRATOS_CATCH("");

}


/***********************************************************************************/
/***********************************************************************************/

void HelmholtzSurfaceElement::GetPseudoBulkSurfaceShapeFunctionsValues(
    MatrixType& rNMatrix,
    const IntegrationMethod& rIntegrationMethod,
    const ProcessInfo& rCurrentProcessInfo
    ) const
{
    KRATOS_TRY;

    const auto& r_geom = GetGeometry();
    SizeType number_of_nodes = r_geom.size();
    const GeometryType::IntegrationPointsArrayType& integration_points = r_geom.IntegrationPoints( rIntegrationMethod );
    SizeType mat_size1 = integration_points.size();
    SizeType mat_size2 = number_of_nodes;

    rNMatrix.resize( mat_size1, mat_size2, false );
    rNMatrix = ZeroMatrix( mat_size1, mat_size2 );

    // create a pseudo bulk element here
    double height = r_geom.Length();
    Point geom_center = r_geom.Center();
    const auto surf_points = r_geom.Points();
    VectorType n_surf;
    CalculateAvgSurfUnitNormal(n_surf);
    PointPtrType p0 = new PointType(surf_points[0].Id(),surf_points[0]);
    PointPtrType p1 = new PointType(surf_points[1].Id(),surf_points[1]);
    PointPtrType p2 = new PointType(surf_points[2].Id(),surf_points[2]);

    if(number_of_nodes==3){
        PointPtrType p3 = new PointType(surf_points[0].Id()+3, geom_center.Coordinates() + height * n_surf);
        TetrahedraGeometryType pseudo_tetrahedra(p0,p1,p2,p3);

        for ( IndexType point_number = 0; point_number < integration_points.size(); ++point_number ) {
            Point gp_local_pt = Point(integration_points[point_number].Coordinates());
            Point gp_global_pt;
            r_geom.GlobalCoordinates(gp_global_pt,gp_local_pt);
            Point elem_gp_local_pt;
            pseudo_tetrahedra.PointLocalCoordinates(elem_gp_local_pt,gp_global_pt);

            for(IndexType i = 0; i < number_of_nodes; ++i )
                rNMatrix(point_number,i) = pseudo_tetrahedra.ShapeFunctionValue(i,elem_gp_local_pt);

        }

    }else if(number_of_nodes==4) {
        PointPtrType p3 = new PointType(surf_points[3].Id(),surf_points[3]);
        PointPtrType p4 = new PointType(surf_points[0].Id()+4, geom_center.Coordinates() + height * n_surf);
        PyramidGeometryType pseudo_pyramid(p0,p1,p2,p3,p4);

        for ( IndexType point_number = 0; point_number < integration_points.size(); ++point_number ) {
            Point gp_local_pt = Point(integration_points[point_number].Coordinates());
            Point gp_global_pt;
            r_geom.GlobalCoordinates(gp_global_pt,gp_local_pt);
            Point elem_gp_local_pt;
            pseudo_pyramid.PointLocalCoordinates(elem_gp_local_pt,gp_global_pt);

            for(IndexType i = 0; i < number_of_nodes; ++i )
                rNMatrix(point_number,i) = pseudo_pyramid.ShapeFunctionValue(i,elem_gp_local_pt);
        }
    }
    else
        KRATOS_ERROR<<"HelmholtzSurfShapeElement: this element only supports 3 and 4 noded surface elements"<<std::endl;

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void HelmholtzSurfaceElement::CalculateStiffnessMatrix(
    MatrixType& rStiffnessMatrix,
    const ProcessInfo& rCurrentProcessInfo
    ) const
{
    KRATOS_TRY;

    const auto& r_geom = GetGeometry();
    SizeType dimension = r_geom.WorkingSpaceDimension();
    SizeType number_of_nodes = r_geom.size();
    SizeType mat_size = number_of_nodes;

    // Clear matrix
    if (rStiffnessMatrix.size1() != mat_size || rStiffnessMatrix.size2() != mat_size)
        rStiffnessMatrix.resize( mat_size, mat_size, false );
    rStiffnessMatrix = ZeroMatrix( mat_size, mat_size );


    //reading integration points and local gradients
    const IntegrationMethod& integration_method = r_geom.GetDefaultIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& integration_points = r_geom.IntegrationPoints(integration_method);
    const unsigned int NumGauss = integration_points.size();
    Vector GaussPtsJDet = ZeroVector(NumGauss);
    r_geom.DeterminantOfJacobian(GaussPtsJDet, integration_method);

    VectorType n_surf;
    CalculateAvgSurfUnitNormal(n_surf);
    MatrixType id_matrix = IdentityMatrix(dimension,dimension);
    MatrixType tangent_projection_matrix = id_matrix - outer_prod(n_surf, n_surf);


    for(std::size_t i_point = 0; i_point<integration_points.size(); ++i_point)
    {

        Matrix DN_DX;
        EntityCalculationUtils::CalculateSurfaceElementShapeDerivatives(
            DN_DX, *mpSolidGeometry, this->GetGeometry(), integration_method, i_point);
        const double IntToReferenceWeight = integration_points[i_point].Weight() * GaussPtsJDet[i_point];

        MatrixType DN_DX_t = prod(DN_DX,tangent_projection_matrix);

        const double r_helmholtz = rCurrentProcessInfo[HELMHOLTZ_RADIUS];
        noalias(rStiffnessMatrix) += IntToReferenceWeight * r_helmholtz * r_helmholtz * prod(DN_DX_t, trans(DN_DX_t));
    }

    KRATOS_CATCH("");
}

void HelmholtzSurfaceElement::CalculateAvgSurfUnitNormal(VectorType & rNormal) const
{
    const auto& r_geom = GetGeometry();
    const IntegrationMethod& integration_method = r_geom.GetDefaultIntegrationMethod();
    const GeometryType::IntegrationPointsArrayType& integration_points = r_geom.IntegrationPoints(integration_method);

    rNormal.resize(3);
    rNormal = ZeroVector(3);
    for ( IndexType point_number = 0; point_number < integration_points.size(); ++point_number )
        rNormal += r_geom.UnitNormal(point_number,integration_method);

    rNormal /= integration_points.size();
    rNormal /= MathUtils<double>::Norm3(rNormal);
}


} // Namespace Kratos
