// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:		 BSD License
//					 license: structural_mechanics_application/license.txt
//
//  Main authors:    Ruben Zorrilla
//                   Riccardo Rossi
//                   Guglielmo Scovazzi
//

// System includes

// External includes

// Project includes
#include "utilities/element_size_calculator.h"
#include "utilities/geometry_utilities.h"
#include "utilities/math_utils.h"
#include "includes/checks.h"

// Application includes
#include "custom_elements/total_lagrangian_mixed_detJ_element.h"
#include "custom_utilities/structural_mechanics_element_utilities.h"

namespace Kratos
{

template<std::size_t TDim>
Element::Pointer TotalLagrangianMixedDetJElement<TDim>::Create(
    IndexType NewId,
    NodesArrayType const& ThisNodes,
    PropertiesType::Pointer pProperties) const
{
    return Kratos::make_intrusive<TotalLagrangianMixedDetJElement<TDim>>(NewId, GetGeometry().Create(ThisNodes), pProperties );
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
Element::Pointer TotalLagrangianMixedDetJElement<TDim>::Create(
    IndexType NewId,
    GeometryType::Pointer pGeom,
    PropertiesType::Pointer pProperties) const
{
    return Kratos::make_intrusive<TotalLagrangianMixedDetJElement<TDim>>(NewId, pGeom, pProperties);
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
Element::Pointer TotalLagrangianMixedDetJElement<TDim>::Clone(
    IndexType NewId,
    NodesArrayType const& rThisNodes) const
{
    KRATOS_TRY

    TotalLagrangianMixedDetJElement::Pointer p_new_elem = Kratos::make_intrusive<TotalLagrangianMixedDetJElement>(NewId, GetGeometry().Create(rThisNodes), pGetProperties());
    p_new_elem->SetData(this->GetData());
    p_new_elem->Set(Flags(*this));

    // Currently selected integration methods
    p_new_elem->SetIntegrationMethod(mThisIntegrationMethod);

    // The vector containing the constitutive laws
    p_new_elem->SetConstitutiveLawVector(mConstitutiveLawVector);

    return p_new_elem;

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::EquationIdVector(
    EquationIdVectorType& rResult,
    const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY

    if (rResult.size() != LocalSize){
        rResult.resize(LocalSize);
    }

    const auto& r_geometry = GetGeometry();
    const IndexType disp_pos = r_geometry[0].GetDofPosition(DISPLACEMENT_X);
    const IndexType det_F_pos = r_geometry[0].GetDofPosition(DETERMINANT_F);

    IndexType aux_index = 0;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        rResult[aux_index++] = r_geometry[i_node].GetDof(DISPLACEMENT_X, disp_pos).EquationId();
        rResult[aux_index++] = r_geometry[i_node].GetDof(DISPLACEMENT_Y, disp_pos + 1).EquationId();
        rResult[aux_index++] = r_geometry[i_node].GetDof(DETERMINANT_F, det_F_pos).EquationId();
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::EquationIdVector(
    EquationIdVectorType& rResult,
    const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY

    if (rResult.size() != LocalSize){
        rResult.resize(LocalSize);
    }

    const auto& r_geometry = GetGeometry();
    const IndexType disp_pos = r_geometry[0].GetDofPosition(DISPLACEMENT_X);
    const IndexType det_F_pos = r_geometry[0].GetDofPosition(DETERMINANT_F);

    IndexType aux_index = 0;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        rResult[aux_index++] = r_geometry[i_node].GetDof(DISPLACEMENT_X, disp_pos).EquationId();
        rResult[aux_index++] = r_geometry[i_node].GetDof(DISPLACEMENT_Y, disp_pos + 1).EquationId();
        rResult[aux_index++] = r_geometry[i_node].GetDof(DISPLACEMENT_Z, disp_pos + 2).EquationId();
        rResult[aux_index++] = r_geometry[i_node].GetDof(DETERMINANT_F, det_F_pos).EquationId();
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::GetDofList(
    DofsVectorType& rElementalDofList,
    const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY

    if (rElementalDofList.size() != LocalSize){
        rElementalDofList.resize(LocalSize);
    }

    const auto& r_geometry = GetGeometry();
    for(IndexType i = 0; i < NumNodes; ++i) {
        rElementalDofList[i * BlockSize] = r_geometry[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[i * BlockSize + 1] = r_geometry[i].pGetDof(DISPLACEMENT_Y);
        rElementalDofList[i * BlockSize + 2] = r_geometry[i].pGetDof(DETERMINANT_F);
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::GetDofList(
    DofsVectorType& rElementalDofList,
    const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY

    if (rElementalDofList.size() != LocalSize){
        rElementalDofList.resize(LocalSize);
    }

    const auto& r_geometry = GetGeometry();
    for(IndexType i = 0; i < NumNodes; ++i){
        rElementalDofList[i * BlockSize] = r_geometry[i].pGetDof(DISPLACEMENT_X);
        rElementalDofList[i * BlockSize + 1] = r_geometry[i].pGetDof(DISPLACEMENT_Y);
        rElementalDofList[i * BlockSize + 2] = r_geometry[i].pGetDof(DISPLACEMENT_Z);
        rElementalDofList[i * BlockSize + 3] = r_geometry[i].pGetDof(DETERMINANT_F);
    }

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::Initialize(const ProcessInfo &rCurrentProcessInfo)
{
    KRATOS_TRY

    // Initialization should not be done again in a restart!
    if (!rCurrentProcessInfo[IS_RESTARTED]) {
        // Integration method initialization
        mThisIntegrationMethod = GeometryData::IntegrationMethod::GI_GAUSS_2;
        const auto& r_integration_points = GetGeometry().IntegrationPoints(this->GetIntegrationMethod());

        // Constitutive Law Vector initialisation
        if (mConstitutiveLawVector.size() != r_integration_points.size()) {
            mConstitutiveLawVector.resize(r_integration_points.size());
        }

        // Initialize material
        InitializeMaterial();
    }

    KRATOS_CATCH( "" )
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::InitializeSolutionStep(const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    const auto& r_geometry = GetGeometry();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node, d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Set te constitutive law values
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(GetGeometry(), GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Call the initialize material response
    for (IndexType i_gauss = 0; i_gauss < mConstitutiveLawVector.size(); ++i_gauss) {
        // Recompute the kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());

        // Set the constitutive variables
        SetConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_integration_points);

        // Call the constitutive law to update material variables
        mConstitutiveLawVector[i_gauss]->InitializeMaterialResponseCauchy(cons_law_values);
    }

    KRATOS_CATCH( "" )
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::FinalizeSolutionStep(const ProcessInfo& rCurrentProcessInfo)
{
    KRATOS_TRY

    const auto& r_geometry = GetGeometry();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node, d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Set te constitutive law values
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Call the initialize material response
    for (IndexType i_gauss = 0; i_gauss < mConstitutiveLawVector.size(); ++i_gauss) {
        // Recompute the kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());

        // Set the constitutive variables
        SetConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_integration_points);

        // Call the constitutive law to update material variables
        mConstitutiveLawVector[i_gauss]->FinalizeMaterialResponseCauchy(cons_law_values);
    }

    KRATOS_CATCH( "" )
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::CalculateLocalSystem(
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check RHS size
    if (rRightHandSideVector.size() != matrix_size) {
        rRightHandSideVector.resize(matrix_size, false);
    }

    // Check LHS size
    if (rLeftHandSideMatrix.size1() != matrix_size || rLeftHandSideMatrix.size2() != matrix_size) {
        rLeftHandSideMatrix.resize(matrix_size, matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rLeftHandSideMatrix.clear();
    rRightHandSideVector.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<2,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 2> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 2; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedVector<double,LocalSize> rhs;
    BoundedMatrix<double,LocalSize,LocalSize> lhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the LHS Gauss point contributions
        const double clhs0 = DN(0,1)*u(0,0);
const double clhs1 = DN(1,1)*u(1,0);
const double clhs2 = DN(2,1)*u(2,0);
const double clhs3 = clhs0 + clhs1 + clhs2;
const double clhs4 = DN(0,0)*u(0,0);
const double clhs5 = DN(1,1)*u(1,1);
const double clhs6 = DN(2,1)*u(2,1);
const double clhs7 = DN(0,1)*u(0,1);
const double clhs8 = DN(1,0)*u(1,0);
const double clhs9 = DN(2,0)*u(2,0);
const double clhs10 = DN(0,0)*u(0,1);
const double clhs11 = DN(1,0)*u(1,1);
const double clhs12 = DN(2,0)*u(2,1);
const double clhs13 = clhs4 + clhs8 + clhs9 + 1;
const double clhs14 = clhs5 + clhs6 + clhs7;
const double clhs15 = -clhs0*clhs11 - clhs0*clhs12 - clhs1*clhs10 - clhs1*clhs12 - clhs10*clhs2 - clhs11*clhs2 + clhs13 + clhs14 + clhs4*clhs5 + clhs4*clhs6 + clhs5*clhs9 + clhs6*clhs8 + clhs7*clhs8 + clhs7*clhs9;
const double clhs16 = 1.0/clhs15;
const double clhs17 = pow(clhs3, 2);
const double clhs18 = DN(0,0)*clhs5 + DN(0,0)*clhs6 + DN(0,0) - DN(0,1)*clhs11 - DN(0,1)*clhs12;
const double clhs19 = pow(clhs15, -2.0);
const double clhs20 = clhs18*clhs19;
const double clhs21 = 0.5*clhs20;
const double clhs22 = clhs14 + 1;
const double clhs23 = pow(clhs22, 2);
const double clhs24 = 1.0*DN(0,1)*clhs16*clhs3 - clhs17*clhs21 - clhs21*clhs23;
const double clhs25 = clhs10 + clhs11 + clhs12;
const double clhs26 = pow(clhs25, 2);
const double clhs27 = pow(clhs13, 2);
const double clhs28 = 1.0*DN(0,0)*clhs13*clhs16 - clhs21*clhs26 - clhs21*clhs27;
const double clhs29 = DN(0,0)*clhs3;
const double clhs30 = DN(0,1)*clhs13;
const double clhs31 = clhs22*clhs25;
const double clhs32 = clhs13*clhs3;
const double clhs33 = clhs16*clhs29 + clhs16*clhs30 - clhs20*clhs31 - clhs20*clhs32;
const double clhs34 = C(0,0)*clhs28 + C(0,1)*clhs24 + C(0,2)*clhs33;
const double clhs35 = N[0]*th[0] + N[1]*th[1] + N[2]*th[2];
const double clhs36 = pow(clhs35, 2);
const double clhs37 = clhs13*clhs36;
const double clhs38 = C(0,2)*clhs28 + C(1,2)*clhs24 + C(2,2)*clhs33;
const double clhs39 = clhs3*clhs36;
const double clhs40 = clhs31 + clhs32;
const double clhs41 = clhs16*clhs36;
const double clhs42 = clhs40*clhs41;
const double clhs43 = 0.5*clhs26*clhs41 + 0.5*clhs27*clhs41 - 0.5;
const double clhs44 = 0.5*clhs17*clhs41 + 0.5*clhs23*clhs41 - 0.5;
const double clhs45 = C(0,0)*clhs43 + C(0,1)*clhs44 + C(0,2)*clhs42;
const double clhs46 = C(0,2)*clhs43 + C(1,2)*clhs44 + C(2,2)*clhs42;
const double clhs47 = DN(0,0)*clhs45 + DN(0,1)*clhs46;
const double clhs48 = clhs34*clhs37 + clhs38*clhs39 + clhs47;
const double clhs49 = C(0,1)*clhs28 + C(1,1)*clhs24 + C(1,2)*clhs33;
const double clhs50 = C(0,1)*clhs43 + C(1,1)*clhs44 + C(1,2)*clhs42;
const double clhs51 = DN(0,0)*clhs46 + DN(0,1)*clhs50;
const double clhs52 = clhs37*clhs38 + clhs39*clhs49 + clhs51;
const double clhs53 = -DN(0,0)*clhs1 - DN(0,0)*clhs2 + DN(0,1)*clhs8 + DN(0,1)*clhs9 + DN(0,1);
const double clhs54 = clhs19*clhs53;
const double clhs55 = 0.5*clhs54;
const double clhs56 = 1.0*DN(0,0)*clhs16*clhs25 - clhs26*clhs55 - clhs27*clhs55;
const double clhs57 = 1.0*DN(0,1)*clhs16*clhs22 - clhs17*clhs55 - clhs23*clhs55;
const double clhs58 = DN(0,1)*clhs25;
const double clhs59 = DN(0,0)*clhs22;
const double clhs60 = clhs16*clhs58 + clhs16*clhs59 - clhs31*clhs54 - clhs32*clhs54;
const double clhs61 = C(0,2)*clhs56 + C(1,2)*clhs57 + C(2,2)*clhs60;
const double clhs62 = C(0,0)*clhs56 + C(0,1)*clhs57 + C(0,2)*clhs60;
const double clhs63 = clhs13*clhs62 + clhs3*clhs61;
const double clhs64 = C(0,1)*clhs56 + C(1,1)*clhs57 + C(1,2)*clhs60;
const double clhs65 = clhs13*clhs61 + clhs3*clhs64;
const double clhs66 = clhs26 + clhs27;
const double clhs67 = C(0,2)*clhs66;
const double clhs68 = clhs17 + clhs23;
const double clhs69 = C(1,2)*clhs68;
const double clhs70 = 2.0*clhs40;
const double clhs71 = C(2,2)*clhs70 + 1.0*clhs67 + 1.0*clhs69;
const double clhs72 = C(0,0)*clhs66;
const double clhs73 = C(0,1)*clhs68;
const double clhs74 = C(0,2)*clhs70 + 1.0*clhs72 + 1.0*clhs73;
const double clhs75 = clhs13*clhs74 + clhs3*clhs71;
const double clhs76 = C(0,1)*clhs66;
const double clhs77 = C(1,1)*clhs68;
const double clhs78 = C(1,2)*clhs70 + 1.0*clhs76 + 1.0*clhs77;
const double clhs79 = clhs13*clhs71 + clhs3*clhs78;
const double clhs80 = clhs16*clhs35;
const double clhs81 = clhs80*(DN(0,0)*clhs75 + DN(0,1)*clhs79);
const double clhs82 = DN(1,0)*clhs6 + DN(1,0)*clhs7 + DN(1,0) - DN(1,1)*clhs10 - DN(1,1)*clhs12;
const double clhs83 = clhs19*clhs82;
const double clhs84 = 0.5*clhs83;
const double clhs85 = 1.0*DN(1,1)*clhs16*clhs3 - clhs17*clhs84 - clhs23*clhs84;
const double clhs86 = 1.0*DN(1,0)*clhs13*clhs16 - clhs26*clhs84 - clhs27*clhs84;
const double clhs87 = DN(1,0)*clhs3;
const double clhs88 = DN(1,1)*clhs13;
const double clhs89 = clhs16*clhs87 + clhs16*clhs88 - clhs31*clhs83 - clhs32*clhs83;
const double clhs90 = C(0,0)*clhs86 + C(0,1)*clhs85 + C(0,2)*clhs89;
const double clhs91 = C(0,2)*clhs86 + C(1,2)*clhs85 + C(2,2)*clhs89;
const double clhs92 = DN(1,0)*clhs45 + DN(1,1)*clhs46;
const double clhs93 = clhs37*clhs90 + clhs39*clhs91 + clhs92;
const double clhs94 = C(0,1)*clhs86 + C(1,1)*clhs85 + C(1,2)*clhs89;
const double clhs95 = DN(1,0)*clhs46 + DN(1,1)*clhs50;
const double clhs96 = clhs37*clhs91 + clhs39*clhs94 + clhs95;
const double clhs97 = -DN(1,0)*clhs0 - DN(1,0)*clhs2 + DN(1,1)*clhs4 + DN(1,1)*clhs9 + DN(1,1);
const double clhs98 = clhs19*clhs97;
const double clhs99 = 0.5*clhs98;
const double clhs100 = 1.0*DN(1,0)*clhs16*clhs25 - clhs26*clhs99 - clhs27*clhs99;
const double clhs101 = 1.0*DN(1,1)*clhs16*clhs22 - clhs17*clhs99 - clhs23*clhs99;
const double clhs102 = DN(1,1)*clhs25;
const double clhs103 = DN(1,0)*clhs22;
const double clhs104 = clhs102*clhs16 + clhs103*clhs16 - clhs31*clhs98 - clhs32*clhs98;
const double clhs105 = C(0,2)*clhs100 + C(1,2)*clhs101 + C(2,2)*clhs104;
const double clhs106 = C(0,0)*clhs100 + C(0,1)*clhs101 + C(0,2)*clhs104;
const double clhs107 = clhs105*clhs3 + clhs106*clhs13;
const double clhs108 = C(0,1)*clhs100 + C(1,1)*clhs101 + C(1,2)*clhs104;
const double clhs109 = clhs105*clhs13 + clhs108*clhs3;
const double clhs110 = DN(2,0)*clhs5 + DN(2,0)*clhs7 + DN(2,0) - DN(2,1)*clhs10 - DN(2,1)*clhs11;
const double clhs111 = clhs110*clhs19;
const double clhs112 = 0.5*clhs111;
const double clhs113 = 1.0*DN(2,1)*clhs16*clhs3 - clhs112*clhs17 - clhs112*clhs23;
const double clhs114 = 1.0*DN(2,0)*clhs13*clhs16 - clhs112*clhs26 - clhs112*clhs27;
const double clhs115 = DN(2,0)*clhs3;
const double clhs116 = DN(2,1)*clhs13;
const double clhs117 = -clhs111*clhs31 - clhs111*clhs32 + clhs115*clhs16 + clhs116*clhs16;
const double clhs118 = C(0,0)*clhs114 + C(0,1)*clhs113 + C(0,2)*clhs117;
const double clhs119 = C(0,2)*clhs114 + C(1,2)*clhs113 + C(2,2)*clhs117;
const double clhs120 = DN(2,0)*clhs45 + DN(2,1)*clhs46;
const double clhs121 = clhs118*clhs37 + clhs119*clhs39 + clhs120;
const double clhs122 = C(0,1)*clhs114 + C(1,1)*clhs113 + C(1,2)*clhs117;
const double clhs123 = DN(2,0)*clhs46 + DN(2,1)*clhs50;
const double clhs124 = clhs119*clhs37 + clhs122*clhs39 + clhs123;
const double clhs125 = -DN(2,0)*clhs0 - DN(2,0)*clhs1 + DN(2,1)*clhs4 + DN(2,1)*clhs8 + DN(2,1);
const double clhs126 = clhs125*clhs19;
const double clhs127 = 0.5*clhs126;
const double clhs128 = 1.0*DN(2,0)*clhs16*clhs25 - clhs127*clhs26 - clhs127*clhs27;
const double clhs129 = 1.0*DN(2,1)*clhs16*clhs22 - clhs127*clhs17 - clhs127*clhs23;
const double clhs130 = DN(2,1)*clhs25;
const double clhs131 = DN(2,0)*clhs22;
const double clhs132 = -clhs126*clhs31 - clhs126*clhs32 + clhs130*clhs16 + clhs131*clhs16;
const double clhs133 = C(0,2)*clhs128 + C(1,2)*clhs129 + C(2,2)*clhs132;
const double clhs134 = C(0,0)*clhs128 + C(0,1)*clhs129 + C(0,2)*clhs132;
const double clhs135 = clhs13*clhs134 + clhs133*clhs3;
const double clhs136 = C(0,1)*clhs128 + C(1,1)*clhs129 + C(1,2)*clhs132;
const double clhs137 = clhs13*clhs133 + clhs136*clhs3;
const double clhs138 = clhs22*clhs38 + clhs25*clhs34;
const double clhs139 = clhs22*clhs49 + clhs25*clhs38;
const double clhs140 = clhs25*clhs36;
const double clhs141 = clhs22*clhs36;
const double clhs142 = clhs140*clhs62 + clhs141*clhs61 + clhs47;
const double clhs143 = clhs140*clhs61 + clhs141*clhs64 + clhs51;
const double clhs144 = clhs22*clhs71 + clhs25*clhs74;
const double clhs145 = clhs22*clhs78 + clhs25*clhs71;
const double clhs146 = clhs80*(DN(0,0)*clhs144 + DN(0,1)*clhs145);
const double clhs147 = clhs22*clhs91 + clhs25*clhs90;
const double clhs148 = clhs22*clhs94 + clhs25*clhs91;
const double clhs149 = clhs105*clhs141 + clhs106*clhs140 + clhs92;
const double clhs150 = clhs105*clhs140 + clhs108*clhs141 + clhs95;
const double clhs151 = clhs118*clhs25 + clhs119*clhs22;
const double clhs152 = clhs119*clhs25 + clhs122*clhs22;
const double clhs153 = clhs120 + clhs133*clhs141 + clhs134*clhs140;
const double clhs154 = clhs123 + clhs133*clhs140 + clhs136*clhs141;
const double clhs155 = DN(0,1)*clhs3;
const double clhs156 = DN(0,0)*clhs13;
const double clhs157 = clhs29 + clhs30;
const double clhs158 = C(0,0)*clhs156 + C(0,1)*clhs155 + C(0,2)*clhs157;
const double clhs159 = C(0,2)*clhs156 + C(1,2)*clhs155 + C(2,2)*clhs157;
const double clhs160 = 2*clhs40;
const double clhs161 = C(0,2)*clhs160 + clhs72 + clhs73;
const double clhs162 = DN(0,0)*clhs161;
const double clhs163 = 0.25/clhs15;
const double clhs164 = clhs163*clhs18;
const double clhs165 = C(2,2)*clhs160 + clhs67 + clhs69;
const double clhs166 = DN(0,1)*clhs165;
const double clhs167 = DN(0,0)*th[0] + DN(1,0)*th[1] + DN(2,0)*th[2];
const double clhs168 = 1.0/clhs35;
const double clhs169 = sqrt(clhs15*clhs168);
const double clhs170 = clhs169*tau;
const double clhs171 = clhs167*clhs170;
const double clhs172 = C(0,1)*clhs156 + C(1,1)*clhs155 + C(1,2)*clhs157;
const double clhs173 = DN(0,0)*clhs165;
const double clhs174 = C(1,2)*clhs160 + clhs76 + clhs77;
const double clhs175 = DN(0,1)*clhs174;
const double clhs176 = DN(0,1)*th[0] + DN(1,1)*th[1] + DN(2,1)*th[2];
const double clhs177 = clhs170*clhs176;
const double clhs178 = DN(0,0)*clhs25;
const double clhs179 = DN(0,1)*clhs22;
const double clhs180 = clhs58 + clhs59;
const double clhs181 = C(0,0)*clhs178 + C(0,1)*clhs179 + C(0,2)*clhs180;
const double clhs182 = C(0,2)*clhs178 + C(1,2)*clhs179 + C(2,2)*clhs180;
const double clhs183 = clhs163*clhs53;
const double clhs184 = C(0,1)*clhs178 + C(1,1)*clhs179 + C(1,2)*clhs180;
const double clhs185 = clhs162 + clhs166;
const double clhs186 = (1.0/2.0)*clhs170;
const double clhs187 = clhs185*clhs186;
const double clhs188 = clhs173 + clhs175;
const double clhs189 = clhs186*clhs188;
const double clhs190 = 0.25*clhs168;
const double clhs191 = N[0]*clhs190;
const double clhs192 = clhs171*clhs185;
const double clhs193 = clhs177*clhs188;
const double clhs194 = N[0]*b(0,1) + N[1]*b(1,1) + N[2]*b(2,1);
const double clhs195 = rho0*tau;
const double clhs196 = clhs195*(DN(0,0)*DN(1,1) - DN(0,1)*DN(1,0));
const double clhs197 = clhs194*clhs196;
const double clhs198 = DN(1,1)*clhs3;
const double clhs199 = DN(1,0)*clhs13;
const double clhs200 = clhs87 + clhs88;
const double clhs201 = C(0,0)*clhs199 + C(0,1)*clhs198 + C(0,2)*clhs200;
const double clhs202 = C(0,2)*clhs199 + C(1,2)*clhs198 + C(2,2)*clhs200;
const double clhs203 = clhs163*clhs82;
const double clhs204 = C(0,1)*clhs199 + C(1,1)*clhs198 + C(1,2)*clhs200;
const double clhs205 = N[0]*b(0,0) + N[1]*b(1,0) + N[2]*b(2,0);
const double clhs206 = clhs196*clhs205;
const double clhs207 = DN(1,0)*clhs25;
const double clhs208 = DN(1,1)*clhs22;
const double clhs209 = clhs102 + clhs103;
const double clhs210 = C(0,0)*clhs207 + C(0,1)*clhs208 + C(0,2)*clhs209;
const double clhs211 = C(0,2)*clhs207 + C(1,2)*clhs208 + C(2,2)*clhs209;
const double clhs212 = clhs163*clhs97;
const double clhs213 = C(0,1)*clhs207 + C(1,1)*clhs208 + C(1,2)*clhs209;
const double clhs214 = N[0]*N[1];
const double clhs215 = N[1]*clhs190;
const double clhs216 = clhs195*(DN(0,0)*DN(2,1) - DN(0,1)*DN(2,0));
const double clhs217 = clhs194*clhs216;
const double clhs218 = DN(2,1)*clhs3;
const double clhs219 = DN(2,0)*clhs13;
const double clhs220 = clhs115 + clhs116;
const double clhs221 = C(0,0)*clhs219 + C(0,1)*clhs218 + C(0,2)*clhs220;
const double clhs222 = C(0,2)*clhs219 + C(1,2)*clhs218 + C(2,2)*clhs220;
const double clhs223 = clhs110*clhs163;
const double clhs224 = C(0,1)*clhs219 + C(1,1)*clhs218 + C(1,2)*clhs220;
const double clhs225 = clhs205*clhs216;
const double clhs226 = DN(2,0)*clhs25;
const double clhs227 = DN(2,1)*clhs22;
const double clhs228 = clhs130 + clhs131;
const double clhs229 = C(0,0)*clhs226 + C(0,1)*clhs227 + C(0,2)*clhs228;
const double clhs230 = C(0,2)*clhs226 + C(1,2)*clhs227 + C(2,2)*clhs228;
const double clhs231 = clhs125*clhs163;
const double clhs232 = C(0,1)*clhs226 + C(1,1)*clhs227 + C(1,2)*clhs228;
const double clhs233 = N[0]*N[2];
const double clhs234 = N[2]*clhs190;
const double clhs235 = clhs80*(DN(1,0)*clhs75 + DN(1,1)*clhs79);
const double clhs236 = clhs80*(DN(1,0)*clhs144 + DN(1,1)*clhs145);
const double clhs237 = DN(1,0)*clhs161;
const double clhs238 = DN(1,1)*clhs165;
const double clhs239 = DN(1,0)*clhs165;
const double clhs240 = DN(1,1)*clhs174;
const double clhs241 = clhs237 + clhs238;
const double clhs242 = clhs186*clhs241;
const double clhs243 = clhs239 + clhs240;
const double clhs244 = clhs186*clhs243;
const double clhs245 = clhs171*clhs241;
const double clhs246 = clhs177*clhs243;
const double clhs247 = clhs195*(DN(1,0)*DN(2,1) - DN(1,1)*DN(2,0));
const double clhs248 = clhs194*clhs247;
const double clhs249 = clhs205*clhs247;
const double clhs250 = N[1]*N[2];
const double clhs251 = clhs80*(DN(2,0)*clhs75 + DN(2,1)*clhs79);
const double clhs252 = clhs80*(DN(2,0)*clhs144 + DN(2,1)*clhs145);
const double clhs253 = DN(2,0)*clhs161;
const double clhs254 = DN(2,1)*clhs165;
const double clhs255 = DN(2,0)*clhs165;
const double clhs256 = DN(2,1)*clhs174;
const double clhs257 = clhs253 + clhs254;
const double clhs258 = clhs186*clhs257;
const double clhs259 = clhs255 + clhs256;
const double clhs260 = clhs186*clhs259;
const double clhs261 = clhs171*clhs257;
const double clhs262 = clhs177*clhs259;
lhs(0,0)=-DN(0,0)*clhs48 - DN(0,1)*clhs52;
lhs(0,1)=-clhs36*(DN(0,0)*clhs63 + DN(0,1)*clhs65);
lhs(0,2)=-N[0]*clhs81;
lhs(0,3)=-DN(0,0)*clhs93 - DN(0,1)*clhs96;
lhs(0,4)=-clhs36*(DN(0,0)*clhs107 + DN(0,1)*clhs109);
lhs(0,5)=-N[1]*clhs81;
lhs(0,6)=-DN(0,0)*clhs121 - DN(0,1)*clhs124;
lhs(0,7)=-clhs36*(DN(0,0)*clhs135 + DN(0,1)*clhs137);
lhs(0,8)=-N[2]*clhs81;
lhs(1,0)=-clhs36*(DN(0,0)*clhs138 + DN(0,1)*clhs139);
lhs(1,1)=-DN(0,0)*clhs142 - DN(0,1)*clhs143;
lhs(1,2)=-N[0]*clhs146;
lhs(1,3)=-clhs36*(DN(0,0)*clhs147 + DN(0,1)*clhs148);
lhs(1,4)=-DN(0,0)*clhs149 - DN(0,1)*clhs150;
lhs(1,5)=-N[1]*clhs146;
lhs(1,6)=-clhs36*(DN(0,0)*clhs151 + DN(0,1)*clhs152);
lhs(1,7)=-DN(0,0)*clhs153 - DN(0,1)*clhs154;
lhs(1,8)=-N[2]*clhs146;
lhs(2,0)=-N[0]*clhs18 + clhs171*(DN(0,0)*clhs158 + DN(0,1)*clhs159 + clhs162*clhs164 + clhs164*clhs166) + clhs177*(DN(0,0)*clhs159 + DN(0,1)*clhs172 + clhs164*clhs173 + clhs164*clhs175);
lhs(2,1)=-N[0]*clhs53 + clhs171*(DN(0,0)*clhs181 + DN(0,1)*clhs182 + clhs162*clhs183 + clhs166*clhs183) + clhs177*(DN(0,0)*clhs182 + DN(0,1)*clhs184 + clhs173*clhs183 + clhs175*clhs183);
lhs(2,2)=DN(0,0)*clhs187 + DN(0,1)*clhs189 + pow(N[0], 2) - clhs191*clhs192 - clhs191*clhs193;
lhs(2,3)=-N[0]*clhs82 + clhs167*clhs169*tau*(DN(0,0)*clhs201 + DN(0,1)*clhs202 + clhs162*clhs203 + clhs166*clhs203) + clhs169*clhs176*tau*(DN(0,0)*clhs202 + DN(0,1)*clhs204 + clhs173*clhs203 + clhs175*clhs203) - clhs197;
lhs(2,4)=-N[0]*clhs97 + clhs171*(DN(0,0)*clhs210 + DN(0,1)*clhs211 + clhs162*clhs212 + clhs166*clhs212) + clhs177*(DN(0,0)*clhs211 + DN(0,1)*clhs213 + clhs173*clhs212 + clhs175*clhs212) + clhs206;
lhs(2,5)=DN(1,0)*clhs187 + DN(1,1)*clhs189 - clhs192*clhs215 - clhs193*clhs215 + clhs214;
lhs(2,6)=-N[0]*clhs110 + clhs167*clhs169*tau*(DN(0,0)*clhs221 + DN(0,1)*clhs222 + clhs162*clhs223 + clhs166*clhs223) + clhs169*clhs176*tau*(DN(0,0)*clhs222 + DN(0,1)*clhs224 + clhs173*clhs223 + clhs175*clhs223) - clhs217;
lhs(2,7)=-N[0]*clhs125 + clhs171*(DN(0,0)*clhs229 + DN(0,1)*clhs230 + clhs162*clhs231 + clhs166*clhs231) + clhs177*(DN(0,0)*clhs230 + DN(0,1)*clhs232 + clhs173*clhs231 + clhs175*clhs231) + clhs225;
lhs(2,8)=DN(2,0)*clhs187 + DN(2,1)*clhs189 - clhs192*clhs234 - clhs193*clhs234 + clhs233;
lhs(3,0)=-DN(1,0)*clhs48 - DN(1,1)*clhs52;
lhs(3,1)=-clhs36*(DN(1,0)*clhs63 + DN(1,1)*clhs65);
lhs(3,2)=-N[0]*clhs235;
lhs(3,3)=-DN(1,0)*clhs93 - DN(1,1)*clhs96;
lhs(3,4)=-clhs36*(DN(1,0)*clhs107 + DN(1,1)*clhs109);
lhs(3,5)=-N[1]*clhs235;
lhs(3,6)=-DN(1,0)*clhs121 - DN(1,1)*clhs124;
lhs(3,7)=-clhs36*(DN(1,0)*clhs135 + DN(1,1)*clhs137);
lhs(3,8)=-N[2]*clhs235;
lhs(4,0)=-clhs36*(DN(1,0)*clhs138 + DN(1,1)*clhs139);
lhs(4,1)=-DN(1,0)*clhs142 - DN(1,1)*clhs143;
lhs(4,2)=-N[0]*clhs236;
lhs(4,3)=-clhs36*(DN(1,0)*clhs147 + DN(1,1)*clhs148);
lhs(4,4)=-DN(1,0)*clhs149 - DN(1,1)*clhs150;
lhs(4,5)=-N[1]*clhs236;
lhs(4,6)=-clhs36*(DN(1,0)*clhs151 + DN(1,1)*clhs152);
lhs(4,7)=-DN(1,0)*clhs153 - DN(1,1)*clhs154;
lhs(4,8)=-N[2]*clhs236;
lhs(5,0)=-N[1]*clhs18 + clhs171*(DN(1,0)*clhs158 + DN(1,1)*clhs159 + clhs164*clhs237 + clhs164*clhs238) + clhs177*(DN(1,0)*clhs159 + DN(1,1)*clhs172 + clhs164*clhs239 + clhs164*clhs240) + clhs197;
lhs(5,1)=-N[1]*clhs53 + clhs167*clhs169*tau*(DN(1,0)*clhs181 + DN(1,1)*clhs182 + clhs183*clhs237 + clhs183*clhs238) + clhs169*clhs176*tau*(DN(1,0)*clhs182 + DN(1,1)*clhs184 + clhs183*clhs239 + clhs183*clhs240) - clhs206;
lhs(5,2)=DN(0,0)*clhs242 + DN(0,1)*clhs244 - clhs191*clhs245 - clhs191*clhs246 + clhs214;
lhs(5,3)=-N[1]*clhs82 + clhs171*(DN(1,0)*clhs201 + DN(1,1)*clhs202 + clhs203*clhs237 + clhs203*clhs238) + clhs177*(DN(1,0)*clhs202 + DN(1,1)*clhs204 + clhs203*clhs239 + clhs203*clhs240);
lhs(5,4)=-N[1]*clhs97 + clhs171*(DN(1,0)*clhs210 + DN(1,1)*clhs211 + clhs212*clhs237 + clhs212*clhs238) + clhs177*(DN(1,0)*clhs211 + DN(1,1)*clhs213 + clhs212*clhs239 + clhs212*clhs240);
lhs(5,5)=DN(1,0)*clhs242 + DN(1,1)*clhs244 + pow(N[1], 2) - clhs215*clhs245 - clhs215*clhs246;
lhs(5,6)=-N[1]*clhs110 + clhs167*clhs169*tau*(DN(1,0)*clhs221 + DN(1,1)*clhs222 + clhs223*clhs237 + clhs223*clhs238) + clhs169*clhs176*tau*(DN(1,0)*clhs222 + DN(1,1)*clhs224 + clhs223*clhs239 + clhs223*clhs240) - clhs248;
lhs(5,7)=-N[1]*clhs125 + clhs171*(DN(1,0)*clhs229 + DN(1,1)*clhs230 + clhs231*clhs237 + clhs231*clhs238) + clhs177*(DN(1,0)*clhs230 + DN(1,1)*clhs232 + clhs231*clhs239 + clhs231*clhs240) + clhs249;
lhs(5,8)=DN(2,0)*clhs242 + DN(2,1)*clhs244 - clhs234*clhs245 - clhs234*clhs246 + clhs250;
lhs(6,0)=-DN(2,0)*clhs48 - DN(2,1)*clhs52;
lhs(6,1)=-clhs36*(DN(2,0)*clhs63 + DN(2,1)*clhs65);
lhs(6,2)=-N[0]*clhs251;
lhs(6,3)=-DN(2,0)*clhs93 - DN(2,1)*clhs96;
lhs(6,4)=-clhs36*(DN(2,0)*clhs107 + DN(2,1)*clhs109);
lhs(6,5)=-N[1]*clhs251;
lhs(6,6)=-DN(2,0)*clhs121 - DN(2,1)*clhs124;
lhs(6,7)=-clhs36*(DN(2,0)*clhs135 + DN(2,1)*clhs137);
lhs(6,8)=-N[2]*clhs251;
lhs(7,0)=-clhs36*(DN(2,0)*clhs138 + DN(2,1)*clhs139);
lhs(7,1)=-DN(2,0)*clhs142 - DN(2,1)*clhs143;
lhs(7,2)=-N[0]*clhs252;
lhs(7,3)=-clhs36*(DN(2,0)*clhs147 + DN(2,1)*clhs148);
lhs(7,4)=-DN(2,0)*clhs149 - DN(2,1)*clhs150;
lhs(7,5)=-N[1]*clhs252;
lhs(7,6)=-clhs36*(DN(2,0)*clhs151 + DN(2,1)*clhs152);
lhs(7,7)=-DN(2,0)*clhs153 - DN(2,1)*clhs154;
lhs(7,8)=-N[2]*clhs252;
lhs(8,0)=-N[2]*clhs18 + clhs171*(DN(2,0)*clhs158 + DN(2,1)*clhs159 + clhs164*clhs253 + clhs164*clhs254) + clhs177*(DN(2,0)*clhs159 + DN(2,1)*clhs172 + clhs164*clhs255 + clhs164*clhs256) + clhs217;
lhs(8,1)=-N[2]*clhs53 + clhs167*clhs169*tau*(DN(2,0)*clhs181 + DN(2,1)*clhs182 + clhs183*clhs253 + clhs183*clhs254) + clhs169*clhs176*tau*(DN(2,0)*clhs182 + DN(2,1)*clhs184 + clhs183*clhs255 + clhs183*clhs256) - clhs225;
lhs(8,2)=DN(0,0)*clhs258 + DN(0,1)*clhs260 - clhs191*clhs261 - clhs191*clhs262 + clhs233;
lhs(8,3)=-N[2]*clhs82 + clhs171*(DN(2,0)*clhs201 + DN(2,1)*clhs202 + clhs203*clhs253 + clhs203*clhs254) + clhs177*(DN(2,0)*clhs202 + DN(2,1)*clhs204 + clhs203*clhs255 + clhs203*clhs256) + clhs248;
lhs(8,4)=-N[2]*clhs97 + clhs167*clhs169*tau*(DN(2,0)*clhs210 + DN(2,1)*clhs211 + clhs212*clhs253 + clhs212*clhs254) + clhs169*clhs176*tau*(DN(2,0)*clhs211 + DN(2,1)*clhs213 + clhs212*clhs255 + clhs212*clhs256) - clhs249;
lhs(8,5)=DN(1,0)*clhs258 + DN(1,1)*clhs260 - clhs215*clhs261 - clhs215*clhs262 + clhs250;
lhs(8,6)=-N[2]*clhs110 + clhs171*(DN(2,0)*clhs221 + DN(2,1)*clhs222 + clhs223*clhs253 + clhs223*clhs254) + clhs177*(DN(2,0)*clhs222 + DN(2,1)*clhs224 + clhs223*clhs255 + clhs223*clhs256);
lhs(8,7)=-N[2]*clhs125 + clhs171*(DN(2,0)*clhs229 + DN(2,1)*clhs230 + clhs231*clhs253 + clhs231*clhs254) + clhs177*(DN(2,0)*clhs230 + DN(2,1)*clhs232 + clhs231*clhs255 + clhs231*clhs256);
lhs(8,8)=DN(2,0)*clhs258 + DN(2,1)*clhs260 + pow(N[2], 2) - clhs234*clhs261 - clhs234*clhs262;

        // Calculate and add the RHS Gauss point contribution
        const double crhs0 = N[0]*b(0,0) + N[1]*b(1,0) + N[2]*b(2,0);
const double crhs1 = N[0]*rho0;
const double crhs2 = DN(0,1)*u(0,0);
const double crhs3 = DN(1,1)*u(1,0);
const double crhs4 = DN(2,1)*u(2,0);
const double crhs5 = crhs2 + crhs3 + crhs4;
const double crhs6 = DN(0,0)*u(0,0);
const double crhs7 = DN(1,0)*u(1,0);
const double crhs8 = DN(2,0)*u(2,0);
const double crhs9 = crhs6 + crhs7 + crhs8 + 1;
const double crhs10 = S[0]*crhs9 + S[2]*crhs5;
const double crhs11 = S[1]*crhs5 + S[2]*crhs9;
const double crhs12 = N[0]*b(0,1) + N[1]*b(1,1) + N[2]*b(2,1);
const double crhs13 = DN(0,0)*u(0,1);
const double crhs14 = DN(1,0)*u(1,1);
const double crhs15 = DN(2,0)*u(2,1);
const double crhs16 = crhs13 + crhs14 + crhs15;
const double crhs17 = DN(0,1)*u(0,1);
const double crhs18 = DN(1,1)*u(1,1);
const double crhs19 = DN(2,1)*u(2,1);
const double crhs20 = crhs17 + crhs18 + crhs19;
const double crhs21 = crhs20 + 1;
const double crhs22 = S[0]*crhs16 + S[2]*crhs21;
const double crhs23 = S[1]*crhs21 + S[2]*crhs16;
const double crhs24 = rho0*tau;
const double crhs25 = crhs12*crhs24;
const double crhs26 = crhs0*crhs24;
const double crhs27 = N[0]*th[0];
const double crhs28 = N[1]*th[1];
const double crhs29 = N[2]*th[2];
const double crhs30 = -crhs13*crhs3 - crhs13*crhs4 - crhs14*crhs2 - crhs14*crhs4 - crhs15*crhs2 - crhs15*crhs3 + crhs17*crhs7 + crhs17*crhs8 + crhs18*crhs6 + crhs18*crhs8 + crhs19*crhs6 + crhs19*crhs7 + crhs20 + crhs9;
const double crhs31 = -crhs27 - crhs28 - crhs29 + crhs30;
const double crhs32 = pow(crhs16, 2) + pow(crhs9, 2);
const double crhs33 = pow(crhs21, 2) + pow(crhs5, 2);
const double crhs34 = 2*crhs16*crhs21 + 2*crhs5*crhs9;
const double crhs35 = C(0,0)*crhs32 + C(0,1)*crhs33 + C(0,2)*crhs34;
const double crhs36 = C(0,2)*crhs32 + C(1,2)*crhs33 + C(2,2)*crhs34;
const double crhs37 = (1.0/2.0)*tau*sqrt(crhs30/(crhs27 + crhs28 + crhs29));
const double crhs38 = crhs37*(DN(0,0)*th[0] + DN(1,0)*th[1] + DN(2,0)*th[2]);
const double crhs39 = C(0,1)*crhs32 + C(1,1)*crhs33 + C(1,2)*crhs34;
const double crhs40 = crhs37*(DN(0,1)*th[0] + DN(1,1)*th[1] + DN(2,1)*th[2]);
const double crhs41 = N[1]*rho0;
const double crhs42 = N[2]*rho0;
rhs[0]=DN(0,0)*crhs10 + DN(0,1)*crhs11 - crhs0*crhs1;
rhs[1]=DN(0,0)*crhs22 + DN(0,1)*crhs23 - crhs1*crhs12;
rhs[2]=N[0]*crhs31 - crhs25*(-DN(0,0)*crhs5 + DN(0,1)*crhs9) - crhs26*(DN(0,0)*crhs21 - DN(0,1)*crhs16) - crhs38*(DN(0,0)*crhs35 + DN(0,1)*crhs36) - crhs40*(DN(0,0)*crhs36 + DN(0,1)*crhs39);
rhs[3]=DN(1,0)*crhs10 + DN(1,1)*crhs11 - crhs0*crhs41;
rhs[4]=DN(1,0)*crhs22 + DN(1,1)*crhs23 - crhs12*crhs41;
rhs[5]=N[1]*crhs31 - crhs25*(-DN(1,0)*crhs5 + DN(1,1)*crhs9) - crhs26*(DN(1,0)*crhs21 - DN(1,1)*crhs16) - crhs38*(DN(1,0)*crhs35 + DN(1,1)*crhs36) - crhs40*(DN(1,0)*crhs36 + DN(1,1)*crhs39);
rhs[6]=DN(2,0)*crhs10 + DN(2,1)*crhs11 - crhs0*crhs42;
rhs[7]=DN(2,0)*crhs22 + DN(2,1)*crhs23 - crhs12*crhs42;
rhs[8]=N[2]*crhs31 - crhs25*(-DN(2,0)*crhs5 + DN(2,1)*crhs9) - crhs26*(DN(2,0)*crhs21 - DN(2,1)*crhs16) - crhs38*(DN(2,0)*crhs35 + DN(2,1)*crhs36) - crhs40*(DN(2,0)*crhs36 + DN(2,1)*crhs39);

        //TODO: Amend this once the assembly is done in the input arrays
        rLeftHandSideMatrix += w_gauss * lhs;
        rRightHandSideVector += w_gauss * rhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::CalculateLocalSystem(
    MatrixType& rLeftHandSideMatrix,
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check RHS size
    if (rRightHandSideVector.size() != matrix_size) {
        rRightHandSideVector.resize(matrix_size, false);
    }

    // Check LHS size
    if (rLeftHandSideMatrix.size1() != matrix_size || rLeftHandSideMatrix.size2() != matrix_size) {
        rLeftHandSideMatrix.resize(matrix_size, matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rLeftHandSideMatrix.clear();
    rRightHandSideVector.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<3,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 3> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 3; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedVector<double,LocalSize> rhs;
    BoundedMatrix<double,LocalSize,LocalSize> lhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the LHS Gauss point contributions
        //substitute_lhs_3D_4N
        // Calculate and add the RHS Gauss point contribution
        //substitute_rhs_3D_4N
        //TODO: Amend this once the assembly is done in the input arrays
        rLeftHandSideMatrix += w_gauss * lhs;
        rRightHandSideVector += w_gauss * rhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::CalculateLeftHandSide(
    MatrixType& rLeftHandSideMatrix,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check LHS size
    if (rLeftHandSideMatrix.size1() != matrix_size || rLeftHandSideMatrix.size2() != matrix_size) {
        rLeftHandSideMatrix.resize(matrix_size, matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rLeftHandSideMatrix.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<2,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 2> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 2; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedMatrix<double,LocalSize,LocalSize> lhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the LHS Gauss point contributions
        const double clhs0 = DN(0,1)*u(0,0);
const double clhs1 = DN(1,1)*u(1,0);
const double clhs2 = DN(2,1)*u(2,0);
const double clhs3 = clhs0 + clhs1 + clhs2;
const double clhs4 = DN(0,0)*u(0,0);
const double clhs5 = DN(1,1)*u(1,1);
const double clhs6 = DN(2,1)*u(2,1);
const double clhs7 = DN(0,1)*u(0,1);
const double clhs8 = DN(1,0)*u(1,0);
const double clhs9 = DN(2,0)*u(2,0);
const double clhs10 = DN(0,0)*u(0,1);
const double clhs11 = DN(1,0)*u(1,1);
const double clhs12 = DN(2,0)*u(2,1);
const double clhs13 = clhs4 + clhs8 + clhs9 + 1;
const double clhs14 = clhs5 + clhs6 + clhs7;
const double clhs15 = -clhs0*clhs11 - clhs0*clhs12 - clhs1*clhs10 - clhs1*clhs12 - clhs10*clhs2 - clhs11*clhs2 + clhs13 + clhs14 + clhs4*clhs5 + clhs4*clhs6 + clhs5*clhs9 + clhs6*clhs8 + clhs7*clhs8 + clhs7*clhs9;
const double clhs16 = 1.0/clhs15;
const double clhs17 = pow(clhs3, 2);
const double clhs18 = DN(0,0)*clhs5 + DN(0,0)*clhs6 + DN(0,0) - DN(0,1)*clhs11 - DN(0,1)*clhs12;
const double clhs19 = pow(clhs15, -2.0);
const double clhs20 = clhs18*clhs19;
const double clhs21 = 0.5*clhs20;
const double clhs22 = clhs14 + 1;
const double clhs23 = pow(clhs22, 2);
const double clhs24 = 1.0*DN(0,1)*clhs16*clhs3 - clhs17*clhs21 - clhs21*clhs23;
const double clhs25 = clhs10 + clhs11 + clhs12;
const double clhs26 = pow(clhs25, 2);
const double clhs27 = pow(clhs13, 2);
const double clhs28 = 1.0*DN(0,0)*clhs13*clhs16 - clhs21*clhs26 - clhs21*clhs27;
const double clhs29 = DN(0,0)*clhs3;
const double clhs30 = DN(0,1)*clhs13;
const double clhs31 = clhs22*clhs25;
const double clhs32 = clhs13*clhs3;
const double clhs33 = clhs16*clhs29 + clhs16*clhs30 - clhs20*clhs31 - clhs20*clhs32;
const double clhs34 = C(0,0)*clhs28 + C(0,1)*clhs24 + C(0,2)*clhs33;
const double clhs35 = N[0]*th[0] + N[1]*th[1] + N[2]*th[2];
const double clhs36 = pow(clhs35, 2);
const double clhs37 = clhs13*clhs36;
const double clhs38 = C(0,2)*clhs28 + C(1,2)*clhs24 + C(2,2)*clhs33;
const double clhs39 = clhs3*clhs36;
const double clhs40 = clhs31 + clhs32;
const double clhs41 = clhs16*clhs36;
const double clhs42 = clhs40*clhs41;
const double clhs43 = 0.5*clhs26*clhs41 + 0.5*clhs27*clhs41 - 0.5;
const double clhs44 = 0.5*clhs17*clhs41 + 0.5*clhs23*clhs41 - 0.5;
const double clhs45 = C(0,0)*clhs43 + C(0,1)*clhs44 + C(0,2)*clhs42;
const double clhs46 = C(0,2)*clhs43 + C(1,2)*clhs44 + C(2,2)*clhs42;
const double clhs47 = DN(0,0)*clhs45 + DN(0,1)*clhs46;
const double clhs48 = clhs34*clhs37 + clhs38*clhs39 + clhs47;
const double clhs49 = C(0,1)*clhs28 + C(1,1)*clhs24 + C(1,2)*clhs33;
const double clhs50 = C(0,1)*clhs43 + C(1,1)*clhs44 + C(1,2)*clhs42;
const double clhs51 = DN(0,0)*clhs46 + DN(0,1)*clhs50;
const double clhs52 = clhs37*clhs38 + clhs39*clhs49 + clhs51;
const double clhs53 = -DN(0,0)*clhs1 - DN(0,0)*clhs2 + DN(0,1)*clhs8 + DN(0,1)*clhs9 + DN(0,1);
const double clhs54 = clhs19*clhs53;
const double clhs55 = 0.5*clhs54;
const double clhs56 = 1.0*DN(0,0)*clhs16*clhs25 - clhs26*clhs55 - clhs27*clhs55;
const double clhs57 = 1.0*DN(0,1)*clhs16*clhs22 - clhs17*clhs55 - clhs23*clhs55;
const double clhs58 = DN(0,1)*clhs25;
const double clhs59 = DN(0,0)*clhs22;
const double clhs60 = clhs16*clhs58 + clhs16*clhs59 - clhs31*clhs54 - clhs32*clhs54;
const double clhs61 = C(0,2)*clhs56 + C(1,2)*clhs57 + C(2,2)*clhs60;
const double clhs62 = C(0,0)*clhs56 + C(0,1)*clhs57 + C(0,2)*clhs60;
const double clhs63 = clhs13*clhs62 + clhs3*clhs61;
const double clhs64 = C(0,1)*clhs56 + C(1,1)*clhs57 + C(1,2)*clhs60;
const double clhs65 = clhs13*clhs61 + clhs3*clhs64;
const double clhs66 = clhs26 + clhs27;
const double clhs67 = C(0,2)*clhs66;
const double clhs68 = clhs17 + clhs23;
const double clhs69 = C(1,2)*clhs68;
const double clhs70 = 2.0*clhs40;
const double clhs71 = C(2,2)*clhs70 + 1.0*clhs67 + 1.0*clhs69;
const double clhs72 = C(0,0)*clhs66;
const double clhs73 = C(0,1)*clhs68;
const double clhs74 = C(0,2)*clhs70 + 1.0*clhs72 + 1.0*clhs73;
const double clhs75 = clhs13*clhs74 + clhs3*clhs71;
const double clhs76 = C(0,1)*clhs66;
const double clhs77 = C(1,1)*clhs68;
const double clhs78 = C(1,2)*clhs70 + 1.0*clhs76 + 1.0*clhs77;
const double clhs79 = clhs13*clhs71 + clhs3*clhs78;
const double clhs80 = clhs16*clhs35;
const double clhs81 = clhs80*(DN(0,0)*clhs75 + DN(0,1)*clhs79);
const double clhs82 = DN(1,0)*clhs6 + DN(1,0)*clhs7 + DN(1,0) - DN(1,1)*clhs10 - DN(1,1)*clhs12;
const double clhs83 = clhs19*clhs82;
const double clhs84 = 0.5*clhs83;
const double clhs85 = 1.0*DN(1,1)*clhs16*clhs3 - clhs17*clhs84 - clhs23*clhs84;
const double clhs86 = 1.0*DN(1,0)*clhs13*clhs16 - clhs26*clhs84 - clhs27*clhs84;
const double clhs87 = DN(1,0)*clhs3;
const double clhs88 = DN(1,1)*clhs13;
const double clhs89 = clhs16*clhs87 + clhs16*clhs88 - clhs31*clhs83 - clhs32*clhs83;
const double clhs90 = C(0,0)*clhs86 + C(0,1)*clhs85 + C(0,2)*clhs89;
const double clhs91 = C(0,2)*clhs86 + C(1,2)*clhs85 + C(2,2)*clhs89;
const double clhs92 = DN(1,0)*clhs45 + DN(1,1)*clhs46;
const double clhs93 = clhs37*clhs90 + clhs39*clhs91 + clhs92;
const double clhs94 = C(0,1)*clhs86 + C(1,1)*clhs85 + C(1,2)*clhs89;
const double clhs95 = DN(1,0)*clhs46 + DN(1,1)*clhs50;
const double clhs96 = clhs37*clhs91 + clhs39*clhs94 + clhs95;
const double clhs97 = -DN(1,0)*clhs0 - DN(1,0)*clhs2 + DN(1,1)*clhs4 + DN(1,1)*clhs9 + DN(1,1);
const double clhs98 = clhs19*clhs97;
const double clhs99 = 0.5*clhs98;
const double clhs100 = 1.0*DN(1,0)*clhs16*clhs25 - clhs26*clhs99 - clhs27*clhs99;
const double clhs101 = 1.0*DN(1,1)*clhs16*clhs22 - clhs17*clhs99 - clhs23*clhs99;
const double clhs102 = DN(1,1)*clhs25;
const double clhs103 = DN(1,0)*clhs22;
const double clhs104 = clhs102*clhs16 + clhs103*clhs16 - clhs31*clhs98 - clhs32*clhs98;
const double clhs105 = C(0,2)*clhs100 + C(1,2)*clhs101 + C(2,2)*clhs104;
const double clhs106 = C(0,0)*clhs100 + C(0,1)*clhs101 + C(0,2)*clhs104;
const double clhs107 = clhs105*clhs3 + clhs106*clhs13;
const double clhs108 = C(0,1)*clhs100 + C(1,1)*clhs101 + C(1,2)*clhs104;
const double clhs109 = clhs105*clhs13 + clhs108*clhs3;
const double clhs110 = DN(2,0)*clhs5 + DN(2,0)*clhs7 + DN(2,0) - DN(2,1)*clhs10 - DN(2,1)*clhs11;
const double clhs111 = clhs110*clhs19;
const double clhs112 = 0.5*clhs111;
const double clhs113 = 1.0*DN(2,1)*clhs16*clhs3 - clhs112*clhs17 - clhs112*clhs23;
const double clhs114 = 1.0*DN(2,0)*clhs13*clhs16 - clhs112*clhs26 - clhs112*clhs27;
const double clhs115 = DN(2,0)*clhs3;
const double clhs116 = DN(2,1)*clhs13;
const double clhs117 = -clhs111*clhs31 - clhs111*clhs32 + clhs115*clhs16 + clhs116*clhs16;
const double clhs118 = C(0,0)*clhs114 + C(0,1)*clhs113 + C(0,2)*clhs117;
const double clhs119 = C(0,2)*clhs114 + C(1,2)*clhs113 + C(2,2)*clhs117;
const double clhs120 = DN(2,0)*clhs45 + DN(2,1)*clhs46;
const double clhs121 = clhs118*clhs37 + clhs119*clhs39 + clhs120;
const double clhs122 = C(0,1)*clhs114 + C(1,1)*clhs113 + C(1,2)*clhs117;
const double clhs123 = DN(2,0)*clhs46 + DN(2,1)*clhs50;
const double clhs124 = clhs119*clhs37 + clhs122*clhs39 + clhs123;
const double clhs125 = -DN(2,0)*clhs0 - DN(2,0)*clhs1 + DN(2,1)*clhs4 + DN(2,1)*clhs8 + DN(2,1);
const double clhs126 = clhs125*clhs19;
const double clhs127 = 0.5*clhs126;
const double clhs128 = 1.0*DN(2,0)*clhs16*clhs25 - clhs127*clhs26 - clhs127*clhs27;
const double clhs129 = 1.0*DN(2,1)*clhs16*clhs22 - clhs127*clhs17 - clhs127*clhs23;
const double clhs130 = DN(2,1)*clhs25;
const double clhs131 = DN(2,0)*clhs22;
const double clhs132 = -clhs126*clhs31 - clhs126*clhs32 + clhs130*clhs16 + clhs131*clhs16;
const double clhs133 = C(0,2)*clhs128 + C(1,2)*clhs129 + C(2,2)*clhs132;
const double clhs134 = C(0,0)*clhs128 + C(0,1)*clhs129 + C(0,2)*clhs132;
const double clhs135 = clhs13*clhs134 + clhs133*clhs3;
const double clhs136 = C(0,1)*clhs128 + C(1,1)*clhs129 + C(1,2)*clhs132;
const double clhs137 = clhs13*clhs133 + clhs136*clhs3;
const double clhs138 = clhs22*clhs38 + clhs25*clhs34;
const double clhs139 = clhs22*clhs49 + clhs25*clhs38;
const double clhs140 = clhs25*clhs36;
const double clhs141 = clhs22*clhs36;
const double clhs142 = clhs140*clhs62 + clhs141*clhs61 + clhs47;
const double clhs143 = clhs140*clhs61 + clhs141*clhs64 + clhs51;
const double clhs144 = clhs22*clhs71 + clhs25*clhs74;
const double clhs145 = clhs22*clhs78 + clhs25*clhs71;
const double clhs146 = clhs80*(DN(0,0)*clhs144 + DN(0,1)*clhs145);
const double clhs147 = clhs22*clhs91 + clhs25*clhs90;
const double clhs148 = clhs22*clhs94 + clhs25*clhs91;
const double clhs149 = clhs105*clhs141 + clhs106*clhs140 + clhs92;
const double clhs150 = clhs105*clhs140 + clhs108*clhs141 + clhs95;
const double clhs151 = clhs118*clhs25 + clhs119*clhs22;
const double clhs152 = clhs119*clhs25 + clhs122*clhs22;
const double clhs153 = clhs120 + clhs133*clhs141 + clhs134*clhs140;
const double clhs154 = clhs123 + clhs133*clhs140 + clhs136*clhs141;
const double clhs155 = DN(0,1)*clhs3;
const double clhs156 = DN(0,0)*clhs13;
const double clhs157 = clhs29 + clhs30;
const double clhs158 = C(0,0)*clhs156 + C(0,1)*clhs155 + C(0,2)*clhs157;
const double clhs159 = C(0,2)*clhs156 + C(1,2)*clhs155 + C(2,2)*clhs157;
const double clhs160 = 2*clhs40;
const double clhs161 = C(0,2)*clhs160 + clhs72 + clhs73;
const double clhs162 = DN(0,0)*clhs161;
const double clhs163 = 0.25/clhs15;
const double clhs164 = clhs163*clhs18;
const double clhs165 = C(2,2)*clhs160 + clhs67 + clhs69;
const double clhs166 = DN(0,1)*clhs165;
const double clhs167 = DN(0,0)*th[0] + DN(1,0)*th[1] + DN(2,0)*th[2];
const double clhs168 = 1.0/clhs35;
const double clhs169 = sqrt(clhs15*clhs168);
const double clhs170 = clhs169*tau;
const double clhs171 = clhs167*clhs170;
const double clhs172 = C(0,1)*clhs156 + C(1,1)*clhs155 + C(1,2)*clhs157;
const double clhs173 = DN(0,0)*clhs165;
const double clhs174 = C(1,2)*clhs160 + clhs76 + clhs77;
const double clhs175 = DN(0,1)*clhs174;
const double clhs176 = DN(0,1)*th[0] + DN(1,1)*th[1] + DN(2,1)*th[2];
const double clhs177 = clhs170*clhs176;
const double clhs178 = DN(0,0)*clhs25;
const double clhs179 = DN(0,1)*clhs22;
const double clhs180 = clhs58 + clhs59;
const double clhs181 = C(0,0)*clhs178 + C(0,1)*clhs179 + C(0,2)*clhs180;
const double clhs182 = C(0,2)*clhs178 + C(1,2)*clhs179 + C(2,2)*clhs180;
const double clhs183 = clhs163*clhs53;
const double clhs184 = C(0,1)*clhs178 + C(1,1)*clhs179 + C(1,2)*clhs180;
const double clhs185 = clhs162 + clhs166;
const double clhs186 = (1.0/2.0)*clhs170;
const double clhs187 = clhs185*clhs186;
const double clhs188 = clhs173 + clhs175;
const double clhs189 = clhs186*clhs188;
const double clhs190 = 0.25*clhs168;
const double clhs191 = N[0]*clhs190;
const double clhs192 = clhs171*clhs185;
const double clhs193 = clhs177*clhs188;
const double clhs194 = N[0]*b(0,1) + N[1]*b(1,1) + N[2]*b(2,1);
const double clhs195 = rho0*tau;
const double clhs196 = clhs195*(DN(0,0)*DN(1,1) - DN(0,1)*DN(1,0));
const double clhs197 = clhs194*clhs196;
const double clhs198 = DN(1,1)*clhs3;
const double clhs199 = DN(1,0)*clhs13;
const double clhs200 = clhs87 + clhs88;
const double clhs201 = C(0,0)*clhs199 + C(0,1)*clhs198 + C(0,2)*clhs200;
const double clhs202 = C(0,2)*clhs199 + C(1,2)*clhs198 + C(2,2)*clhs200;
const double clhs203 = clhs163*clhs82;
const double clhs204 = C(0,1)*clhs199 + C(1,1)*clhs198 + C(1,2)*clhs200;
const double clhs205 = N[0]*b(0,0) + N[1]*b(1,0) + N[2]*b(2,0);
const double clhs206 = clhs196*clhs205;
const double clhs207 = DN(1,0)*clhs25;
const double clhs208 = DN(1,1)*clhs22;
const double clhs209 = clhs102 + clhs103;
const double clhs210 = C(0,0)*clhs207 + C(0,1)*clhs208 + C(0,2)*clhs209;
const double clhs211 = C(0,2)*clhs207 + C(1,2)*clhs208 + C(2,2)*clhs209;
const double clhs212 = clhs163*clhs97;
const double clhs213 = C(0,1)*clhs207 + C(1,1)*clhs208 + C(1,2)*clhs209;
const double clhs214 = N[0]*N[1];
const double clhs215 = N[1]*clhs190;
const double clhs216 = clhs195*(DN(0,0)*DN(2,1) - DN(0,1)*DN(2,0));
const double clhs217 = clhs194*clhs216;
const double clhs218 = DN(2,1)*clhs3;
const double clhs219 = DN(2,0)*clhs13;
const double clhs220 = clhs115 + clhs116;
const double clhs221 = C(0,0)*clhs219 + C(0,1)*clhs218 + C(0,2)*clhs220;
const double clhs222 = C(0,2)*clhs219 + C(1,2)*clhs218 + C(2,2)*clhs220;
const double clhs223 = clhs110*clhs163;
const double clhs224 = C(0,1)*clhs219 + C(1,1)*clhs218 + C(1,2)*clhs220;
const double clhs225 = clhs205*clhs216;
const double clhs226 = DN(2,0)*clhs25;
const double clhs227 = DN(2,1)*clhs22;
const double clhs228 = clhs130 + clhs131;
const double clhs229 = C(0,0)*clhs226 + C(0,1)*clhs227 + C(0,2)*clhs228;
const double clhs230 = C(0,2)*clhs226 + C(1,2)*clhs227 + C(2,2)*clhs228;
const double clhs231 = clhs125*clhs163;
const double clhs232 = C(0,1)*clhs226 + C(1,1)*clhs227 + C(1,2)*clhs228;
const double clhs233 = N[0]*N[2];
const double clhs234 = N[2]*clhs190;
const double clhs235 = clhs80*(DN(1,0)*clhs75 + DN(1,1)*clhs79);
const double clhs236 = clhs80*(DN(1,0)*clhs144 + DN(1,1)*clhs145);
const double clhs237 = DN(1,0)*clhs161;
const double clhs238 = DN(1,1)*clhs165;
const double clhs239 = DN(1,0)*clhs165;
const double clhs240 = DN(1,1)*clhs174;
const double clhs241 = clhs237 + clhs238;
const double clhs242 = clhs186*clhs241;
const double clhs243 = clhs239 + clhs240;
const double clhs244 = clhs186*clhs243;
const double clhs245 = clhs171*clhs241;
const double clhs246 = clhs177*clhs243;
const double clhs247 = clhs195*(DN(1,0)*DN(2,1) - DN(1,1)*DN(2,0));
const double clhs248 = clhs194*clhs247;
const double clhs249 = clhs205*clhs247;
const double clhs250 = N[1]*N[2];
const double clhs251 = clhs80*(DN(2,0)*clhs75 + DN(2,1)*clhs79);
const double clhs252 = clhs80*(DN(2,0)*clhs144 + DN(2,1)*clhs145);
const double clhs253 = DN(2,0)*clhs161;
const double clhs254 = DN(2,1)*clhs165;
const double clhs255 = DN(2,0)*clhs165;
const double clhs256 = DN(2,1)*clhs174;
const double clhs257 = clhs253 + clhs254;
const double clhs258 = clhs186*clhs257;
const double clhs259 = clhs255 + clhs256;
const double clhs260 = clhs186*clhs259;
const double clhs261 = clhs171*clhs257;
const double clhs262 = clhs177*clhs259;
lhs(0,0)=-DN(0,0)*clhs48 - DN(0,1)*clhs52;
lhs(0,1)=-clhs36*(DN(0,0)*clhs63 + DN(0,1)*clhs65);
lhs(0,2)=-N[0]*clhs81;
lhs(0,3)=-DN(0,0)*clhs93 - DN(0,1)*clhs96;
lhs(0,4)=-clhs36*(DN(0,0)*clhs107 + DN(0,1)*clhs109);
lhs(0,5)=-N[1]*clhs81;
lhs(0,6)=-DN(0,0)*clhs121 - DN(0,1)*clhs124;
lhs(0,7)=-clhs36*(DN(0,0)*clhs135 + DN(0,1)*clhs137);
lhs(0,8)=-N[2]*clhs81;
lhs(1,0)=-clhs36*(DN(0,0)*clhs138 + DN(0,1)*clhs139);
lhs(1,1)=-DN(0,0)*clhs142 - DN(0,1)*clhs143;
lhs(1,2)=-N[0]*clhs146;
lhs(1,3)=-clhs36*(DN(0,0)*clhs147 + DN(0,1)*clhs148);
lhs(1,4)=-DN(0,0)*clhs149 - DN(0,1)*clhs150;
lhs(1,5)=-N[1]*clhs146;
lhs(1,6)=-clhs36*(DN(0,0)*clhs151 + DN(0,1)*clhs152);
lhs(1,7)=-DN(0,0)*clhs153 - DN(0,1)*clhs154;
lhs(1,8)=-N[2]*clhs146;
lhs(2,0)=-N[0]*clhs18 + clhs171*(DN(0,0)*clhs158 + DN(0,1)*clhs159 + clhs162*clhs164 + clhs164*clhs166) + clhs177*(DN(0,0)*clhs159 + DN(0,1)*clhs172 + clhs164*clhs173 + clhs164*clhs175);
lhs(2,1)=-N[0]*clhs53 + clhs171*(DN(0,0)*clhs181 + DN(0,1)*clhs182 + clhs162*clhs183 + clhs166*clhs183) + clhs177*(DN(0,0)*clhs182 + DN(0,1)*clhs184 + clhs173*clhs183 + clhs175*clhs183);
lhs(2,2)=DN(0,0)*clhs187 + DN(0,1)*clhs189 + pow(N[0], 2) - clhs191*clhs192 - clhs191*clhs193;
lhs(2,3)=-N[0]*clhs82 + clhs167*clhs169*tau*(DN(0,0)*clhs201 + DN(0,1)*clhs202 + clhs162*clhs203 + clhs166*clhs203) + clhs169*clhs176*tau*(DN(0,0)*clhs202 + DN(0,1)*clhs204 + clhs173*clhs203 + clhs175*clhs203) - clhs197;
lhs(2,4)=-N[0]*clhs97 + clhs171*(DN(0,0)*clhs210 + DN(0,1)*clhs211 + clhs162*clhs212 + clhs166*clhs212) + clhs177*(DN(0,0)*clhs211 + DN(0,1)*clhs213 + clhs173*clhs212 + clhs175*clhs212) + clhs206;
lhs(2,5)=DN(1,0)*clhs187 + DN(1,1)*clhs189 - clhs192*clhs215 - clhs193*clhs215 + clhs214;
lhs(2,6)=-N[0]*clhs110 + clhs167*clhs169*tau*(DN(0,0)*clhs221 + DN(0,1)*clhs222 + clhs162*clhs223 + clhs166*clhs223) + clhs169*clhs176*tau*(DN(0,0)*clhs222 + DN(0,1)*clhs224 + clhs173*clhs223 + clhs175*clhs223) - clhs217;
lhs(2,7)=-N[0]*clhs125 + clhs171*(DN(0,0)*clhs229 + DN(0,1)*clhs230 + clhs162*clhs231 + clhs166*clhs231) + clhs177*(DN(0,0)*clhs230 + DN(0,1)*clhs232 + clhs173*clhs231 + clhs175*clhs231) + clhs225;
lhs(2,8)=DN(2,0)*clhs187 + DN(2,1)*clhs189 - clhs192*clhs234 - clhs193*clhs234 + clhs233;
lhs(3,0)=-DN(1,0)*clhs48 - DN(1,1)*clhs52;
lhs(3,1)=-clhs36*(DN(1,0)*clhs63 + DN(1,1)*clhs65);
lhs(3,2)=-N[0]*clhs235;
lhs(3,3)=-DN(1,0)*clhs93 - DN(1,1)*clhs96;
lhs(3,4)=-clhs36*(DN(1,0)*clhs107 + DN(1,1)*clhs109);
lhs(3,5)=-N[1]*clhs235;
lhs(3,6)=-DN(1,0)*clhs121 - DN(1,1)*clhs124;
lhs(3,7)=-clhs36*(DN(1,0)*clhs135 + DN(1,1)*clhs137);
lhs(3,8)=-N[2]*clhs235;
lhs(4,0)=-clhs36*(DN(1,0)*clhs138 + DN(1,1)*clhs139);
lhs(4,1)=-DN(1,0)*clhs142 - DN(1,1)*clhs143;
lhs(4,2)=-N[0]*clhs236;
lhs(4,3)=-clhs36*(DN(1,0)*clhs147 + DN(1,1)*clhs148);
lhs(4,4)=-DN(1,0)*clhs149 - DN(1,1)*clhs150;
lhs(4,5)=-N[1]*clhs236;
lhs(4,6)=-clhs36*(DN(1,0)*clhs151 + DN(1,1)*clhs152);
lhs(4,7)=-DN(1,0)*clhs153 - DN(1,1)*clhs154;
lhs(4,8)=-N[2]*clhs236;
lhs(5,0)=-N[1]*clhs18 + clhs171*(DN(1,0)*clhs158 + DN(1,1)*clhs159 + clhs164*clhs237 + clhs164*clhs238) + clhs177*(DN(1,0)*clhs159 + DN(1,1)*clhs172 + clhs164*clhs239 + clhs164*clhs240) + clhs197;
lhs(5,1)=-N[1]*clhs53 + clhs167*clhs169*tau*(DN(1,0)*clhs181 + DN(1,1)*clhs182 + clhs183*clhs237 + clhs183*clhs238) + clhs169*clhs176*tau*(DN(1,0)*clhs182 + DN(1,1)*clhs184 + clhs183*clhs239 + clhs183*clhs240) - clhs206;
lhs(5,2)=DN(0,0)*clhs242 + DN(0,1)*clhs244 - clhs191*clhs245 - clhs191*clhs246 + clhs214;
lhs(5,3)=-N[1]*clhs82 + clhs171*(DN(1,0)*clhs201 + DN(1,1)*clhs202 + clhs203*clhs237 + clhs203*clhs238) + clhs177*(DN(1,0)*clhs202 + DN(1,1)*clhs204 + clhs203*clhs239 + clhs203*clhs240);
lhs(5,4)=-N[1]*clhs97 + clhs171*(DN(1,0)*clhs210 + DN(1,1)*clhs211 + clhs212*clhs237 + clhs212*clhs238) + clhs177*(DN(1,0)*clhs211 + DN(1,1)*clhs213 + clhs212*clhs239 + clhs212*clhs240);
lhs(5,5)=DN(1,0)*clhs242 + DN(1,1)*clhs244 + pow(N[1], 2) - clhs215*clhs245 - clhs215*clhs246;
lhs(5,6)=-N[1]*clhs110 + clhs167*clhs169*tau*(DN(1,0)*clhs221 + DN(1,1)*clhs222 + clhs223*clhs237 + clhs223*clhs238) + clhs169*clhs176*tau*(DN(1,0)*clhs222 + DN(1,1)*clhs224 + clhs223*clhs239 + clhs223*clhs240) - clhs248;
lhs(5,7)=-N[1]*clhs125 + clhs171*(DN(1,0)*clhs229 + DN(1,1)*clhs230 + clhs231*clhs237 + clhs231*clhs238) + clhs177*(DN(1,0)*clhs230 + DN(1,1)*clhs232 + clhs231*clhs239 + clhs231*clhs240) + clhs249;
lhs(5,8)=DN(2,0)*clhs242 + DN(2,1)*clhs244 - clhs234*clhs245 - clhs234*clhs246 + clhs250;
lhs(6,0)=-DN(2,0)*clhs48 - DN(2,1)*clhs52;
lhs(6,1)=-clhs36*(DN(2,0)*clhs63 + DN(2,1)*clhs65);
lhs(6,2)=-N[0]*clhs251;
lhs(6,3)=-DN(2,0)*clhs93 - DN(2,1)*clhs96;
lhs(6,4)=-clhs36*(DN(2,0)*clhs107 + DN(2,1)*clhs109);
lhs(6,5)=-N[1]*clhs251;
lhs(6,6)=-DN(2,0)*clhs121 - DN(2,1)*clhs124;
lhs(6,7)=-clhs36*(DN(2,0)*clhs135 + DN(2,1)*clhs137);
lhs(6,8)=-N[2]*clhs251;
lhs(7,0)=-clhs36*(DN(2,0)*clhs138 + DN(2,1)*clhs139);
lhs(7,1)=-DN(2,0)*clhs142 - DN(2,1)*clhs143;
lhs(7,2)=-N[0]*clhs252;
lhs(7,3)=-clhs36*(DN(2,0)*clhs147 + DN(2,1)*clhs148);
lhs(7,4)=-DN(2,0)*clhs149 - DN(2,1)*clhs150;
lhs(7,5)=-N[1]*clhs252;
lhs(7,6)=-clhs36*(DN(2,0)*clhs151 + DN(2,1)*clhs152);
lhs(7,7)=-DN(2,0)*clhs153 - DN(2,1)*clhs154;
lhs(7,8)=-N[2]*clhs252;
lhs(8,0)=-N[2]*clhs18 + clhs171*(DN(2,0)*clhs158 + DN(2,1)*clhs159 + clhs164*clhs253 + clhs164*clhs254) + clhs177*(DN(2,0)*clhs159 + DN(2,1)*clhs172 + clhs164*clhs255 + clhs164*clhs256) + clhs217;
lhs(8,1)=-N[2]*clhs53 + clhs167*clhs169*tau*(DN(2,0)*clhs181 + DN(2,1)*clhs182 + clhs183*clhs253 + clhs183*clhs254) + clhs169*clhs176*tau*(DN(2,0)*clhs182 + DN(2,1)*clhs184 + clhs183*clhs255 + clhs183*clhs256) - clhs225;
lhs(8,2)=DN(0,0)*clhs258 + DN(0,1)*clhs260 - clhs191*clhs261 - clhs191*clhs262 + clhs233;
lhs(8,3)=-N[2]*clhs82 + clhs171*(DN(2,0)*clhs201 + DN(2,1)*clhs202 + clhs203*clhs253 + clhs203*clhs254) + clhs177*(DN(2,0)*clhs202 + DN(2,1)*clhs204 + clhs203*clhs255 + clhs203*clhs256) + clhs248;
lhs(8,4)=-N[2]*clhs97 + clhs167*clhs169*tau*(DN(2,0)*clhs210 + DN(2,1)*clhs211 + clhs212*clhs253 + clhs212*clhs254) + clhs169*clhs176*tau*(DN(2,0)*clhs211 + DN(2,1)*clhs213 + clhs212*clhs255 + clhs212*clhs256) - clhs249;
lhs(8,5)=DN(1,0)*clhs258 + DN(1,1)*clhs260 - clhs215*clhs261 - clhs215*clhs262 + clhs250;
lhs(8,6)=-N[2]*clhs110 + clhs171*(DN(2,0)*clhs221 + DN(2,1)*clhs222 + clhs223*clhs253 + clhs223*clhs254) + clhs177*(DN(2,0)*clhs222 + DN(2,1)*clhs224 + clhs223*clhs255 + clhs223*clhs256);
lhs(8,7)=-N[2]*clhs125 + clhs171*(DN(2,0)*clhs229 + DN(2,1)*clhs230 + clhs231*clhs253 + clhs231*clhs254) + clhs177*(DN(2,0)*clhs230 + DN(2,1)*clhs232 + clhs231*clhs255 + clhs231*clhs256);
lhs(8,8)=DN(2,0)*clhs258 + DN(2,1)*clhs260 + pow(N[2], 2) - clhs234*clhs261 - clhs234*clhs262;

        //TODO: Amend this once the assembly is done in the input arrays
        rLeftHandSideMatrix += w_gauss * lhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::CalculateLeftHandSide(
    MatrixType& rLeftHandSideMatrix,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check LHS size
    if (rLeftHandSideMatrix.size1() != matrix_size || rLeftHandSideMatrix.size2() != matrix_size) {
        rLeftHandSideMatrix.resize(matrix_size, matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rLeftHandSideMatrix.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<3,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 3> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 3; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedMatrix<double,LocalSize,LocalSize> lhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the LHS Gauss point contributions
        //substitute_lhs_3D_4N
        //TODO: Amend this once the assembly is done in the input arrays
        rLeftHandSideMatrix += w_gauss * lhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::CalculateRightHandSide(
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check RHS size
    if (rRightHandSideVector.size() != matrix_size) {
        rRightHandSideVector.resize(matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rRightHandSideVector.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<2,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 2> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 2; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedVector<double,LocalSize> rhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the RHS Gauss point contribution
        const double crhs0 = N[0]*b(0,0) + N[1]*b(1,0) + N[2]*b(2,0);
const double crhs1 = N[0]*rho0;
const double crhs2 = DN(0,1)*u(0,0);
const double crhs3 = DN(1,1)*u(1,0);
const double crhs4 = DN(2,1)*u(2,0);
const double crhs5 = crhs2 + crhs3 + crhs4;
const double crhs6 = DN(0,0)*u(0,0);
const double crhs7 = DN(1,0)*u(1,0);
const double crhs8 = DN(2,0)*u(2,0);
const double crhs9 = crhs6 + crhs7 + crhs8 + 1;
const double crhs10 = S[0]*crhs9 + S[2]*crhs5;
const double crhs11 = S[1]*crhs5 + S[2]*crhs9;
const double crhs12 = N[0]*b(0,1) + N[1]*b(1,1) + N[2]*b(2,1);
const double crhs13 = DN(0,0)*u(0,1);
const double crhs14 = DN(1,0)*u(1,1);
const double crhs15 = DN(2,0)*u(2,1);
const double crhs16 = crhs13 + crhs14 + crhs15;
const double crhs17 = DN(0,1)*u(0,1);
const double crhs18 = DN(1,1)*u(1,1);
const double crhs19 = DN(2,1)*u(2,1);
const double crhs20 = crhs17 + crhs18 + crhs19;
const double crhs21 = crhs20 + 1;
const double crhs22 = S[0]*crhs16 + S[2]*crhs21;
const double crhs23 = S[1]*crhs21 + S[2]*crhs16;
const double crhs24 = rho0*tau;
const double crhs25 = crhs12*crhs24;
const double crhs26 = crhs0*crhs24;
const double crhs27 = N[0]*th[0];
const double crhs28 = N[1]*th[1];
const double crhs29 = N[2]*th[2];
const double crhs30 = -crhs13*crhs3 - crhs13*crhs4 - crhs14*crhs2 - crhs14*crhs4 - crhs15*crhs2 - crhs15*crhs3 + crhs17*crhs7 + crhs17*crhs8 + crhs18*crhs6 + crhs18*crhs8 + crhs19*crhs6 + crhs19*crhs7 + crhs20 + crhs9;
const double crhs31 = -crhs27 - crhs28 - crhs29 + crhs30;
const double crhs32 = pow(crhs16, 2) + pow(crhs9, 2);
const double crhs33 = pow(crhs21, 2) + pow(crhs5, 2);
const double crhs34 = 2*crhs16*crhs21 + 2*crhs5*crhs9;
const double crhs35 = C(0,0)*crhs32 + C(0,1)*crhs33 + C(0,2)*crhs34;
const double crhs36 = C(0,2)*crhs32 + C(1,2)*crhs33 + C(2,2)*crhs34;
const double crhs37 = (1.0/2.0)*tau*sqrt(crhs30/(crhs27 + crhs28 + crhs29));
const double crhs38 = crhs37*(DN(0,0)*th[0] + DN(1,0)*th[1] + DN(2,0)*th[2]);
const double crhs39 = C(0,1)*crhs32 + C(1,1)*crhs33 + C(1,2)*crhs34;
const double crhs40 = crhs37*(DN(0,1)*th[0] + DN(1,1)*th[1] + DN(2,1)*th[2]);
const double crhs41 = N[1]*rho0;
const double crhs42 = N[2]*rho0;
rhs[0]=DN(0,0)*crhs10 + DN(0,1)*crhs11 - crhs0*crhs1;
rhs[1]=DN(0,0)*crhs22 + DN(0,1)*crhs23 - crhs1*crhs12;
rhs[2]=N[0]*crhs31 - crhs25*(-DN(0,0)*crhs5 + DN(0,1)*crhs9) - crhs26*(DN(0,0)*crhs21 - DN(0,1)*crhs16) - crhs38*(DN(0,0)*crhs35 + DN(0,1)*crhs36) - crhs40*(DN(0,0)*crhs36 + DN(0,1)*crhs39);
rhs[3]=DN(1,0)*crhs10 + DN(1,1)*crhs11 - crhs0*crhs41;
rhs[4]=DN(1,0)*crhs22 + DN(1,1)*crhs23 - crhs12*crhs41;
rhs[5]=N[1]*crhs31 - crhs25*(-DN(1,0)*crhs5 + DN(1,1)*crhs9) - crhs26*(DN(1,0)*crhs21 - DN(1,1)*crhs16) - crhs38*(DN(1,0)*crhs35 + DN(1,1)*crhs36) - crhs40*(DN(1,0)*crhs36 + DN(1,1)*crhs39);
rhs[6]=DN(2,0)*crhs10 + DN(2,1)*crhs11 - crhs0*crhs42;
rhs[7]=DN(2,0)*crhs22 + DN(2,1)*crhs23 - crhs12*crhs42;
rhs[8]=N[2]*crhs31 - crhs25*(-DN(2,0)*crhs5 + DN(2,1)*crhs9) - crhs26*(DN(2,0)*crhs21 - DN(2,1)*crhs16) - crhs38*(DN(2,0)*crhs35 + DN(2,1)*crhs36) - crhs40*(DN(2,0)*crhs36 + DN(2,1)*crhs39);

        //TODO: Amend this once the assembly is done in the input arrays
        rRightHandSideVector += w_gauss * rhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::CalculateRightHandSide(
    VectorType& rRightHandSideVector,
    const ProcessInfo& rCurrentProcessInfo)
{
    const auto& r_geometry = GetGeometry();
    const SizeType dim = r_geometry.WorkingSpaceDimension();
    const SizeType n_nodes = r_geometry.PointsNumber();
    const SizeType block_size = dim + 1;
    const SizeType matrix_size = block_size * n_nodes;
    const SizeType strain_size = GetProperties().GetValue(CONSTITUTIVE_LAW)->GetStrainSize();

    // Check RHS size
    if (rRightHandSideVector.size() != matrix_size) {
        rRightHandSideVector.resize(matrix_size, false);
    }

    // Create the kinematics container and fill the nodal data
    KinematicVariables kinematic_variables;
    for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
        const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
        for (IndexType d = 0; d < dim; ++d) {
            kinematic_variables.Displacements(i_node * dim + d) = r_disp[d];
        }
        kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
    }

    // Create the constitutive variables and values containers
    ConstitutiveVariables constitutive_variables;
    ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
    auto& r_cons_law_options = cons_law_values.GetOptions();
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
    r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
    r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, true);

    // Calculate the RHS and LHS contributions
    rRightHandSideVector.clear();

    // Calculate stabilization constant
    const double c_tau = 2.0;
    const double h = ElementSizeCalculator<3,NumNodes>::AverageElementSize(r_geometry);
    const double mu = 1.0; //FIXME: This is the Lame constant. Compute it.
    const double tau = c_tau * std::pow(h,2) / (2.0 * mu);

    // Set data for the body force calculation
    BoundedMatrix<double, NumNodes, 3> b;
    for (IndexType i_node = 0; i_node < NumNodes; ++i_node) {
        const array_1d<double,3>& r_b_i = r_geometry[i_node].FastGetSolutionStepValue(BODY_FORCE);
        for (IndexType d = 0; d < 3; ++d) {
            b(i_node, d) = r_b_i[d];
        }
    }
    const double rho0 = GetProperties().GetValue(DENSITY);

    // Set the auxiliary references matching the automatic differentiation symbols
    const auto& N = kinematic_variables.N;
    const auto& DN = kinematic_variables.DN_DX;
    const auto& u = kinematic_variables.Displacements;
    const auto& th = kinematic_variables.JacobianDeterminant;
    const auto& S = constitutive_variables.StressVector;
    const auto& C = constitutive_variables.ConstitutiveMatrix;

    // Aux RHS and LHS
    //TODO: To be removed
    BoundedVector<double,LocalSize> rhs;
    BoundedMatrix<double,LocalSize,LocalSize> lhs;

    const SizeType n_gauss = r_geometry.IntegrationPointsNumber(GetIntegrationMethod());
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());
    for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
        // Calculate kinematics
        CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());
        const double w_gauss = kinematic_variables.detJ0 * r_integration_points[i_gauss].Weight();

        // Calculate the constitutive response
        CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_geometry.IntegrationPoints(this->GetIntegrationMethod()), ConstitutiveLaw::StressMeasure_Cauchy);

        // Calculate and add the RHS Gauss point contribution
        //substitute_rhs_3D_4N
        //TODO: Amend this once the assembly is done in the input arrays
        rRightHandSideVector += w_gauss * rhs;
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::InitializeMaterial()
{
    KRATOS_TRY

    const auto& r_properties = GetProperties();
    if (r_properties[CONSTITUTIVE_LAW] != nullptr) {
        const auto& r_geometry = GetGeometry();
        const auto& r_N_values = r_geometry.ShapeFunctionsValues(mThisIntegrationMethod);
        IndexType aux = 0;
        for (auto &it_gauss_pt : mConstitutiveLawVector) {
            it_gauss_pt = (r_properties[CONSTITUTIVE_LAW])->Clone();
            (it_gauss_pt)->InitializeMaterial(r_properties, r_geometry, row(r_N_values, aux));
            aux++;
        }
    } else {
        KRATOS_ERROR << "A constitutive law needs to be specified for the element with ID " << this->Id() << std::endl;
    }

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
bool TotalLagrangianMixedDetJElement<TDim>::UseElementProvidedStrain() const
{
    return true;
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::SetConstitutiveVariables(
    KinematicVariables& rThisKinematicVariables,
    ConstitutiveVariables& rThisConstitutiveVariables,
    ConstitutiveLaw::Parameters& rValues,
    const IndexType PointNumber,
    const GeometryType::IntegrationPointsArrayType& IntegrationPoints) const
{
    // Here we essentially set the input parameters
    rValues.SetShapeFunctionsValues(rThisKinematicVariables.N); // shape functions
    rValues.SetStrainVector(rThisKinematicVariables.EquivalentStrain); // equivalent total strain
    //TODO: Check if these are really required. I think they shouldn't as we're computing the strain in the element
    // rValues.SetDeterminantF(rThisKinematicVariables.detF); // assuming that det(F) is computed somewhere else
    // rValues.SetDeformationGradientF(rThisKinematicVariables.F); // assuming that F is computed somewhere else

    // Here we set the space on which the results shall be written
    rValues.SetStressVector(rThisConstitutiveVariables.StressVector); //F computed somewhere else
    rValues.SetConstitutiveMatrix(rThisConstitutiveVariables.ConstitutiveMatrix); //assuming the determinant is computed somewhere else
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::CalculateConstitutiveVariables(
    KinematicVariables& rThisKinematicVariables,
    ConstitutiveVariables& rThisConstitutiveVariables,
    ConstitutiveLaw::Parameters& rValues,
    const IndexType PointNumber,
    const GeometryType::IntegrationPointsArrayType& IntegrationPoints,
    const ConstitutiveLaw::StressMeasure ThisStressMeasure) const
{
    // Set the constitutive variables
    SetConstitutiveVariables(rThisKinematicVariables, rThisConstitutiveVariables, rValues, PointNumber, IntegrationPoints);

    // Actually do the computations in the ConstitutiveLaw
    mConstitutiveLawVector[PointNumber]->CalculateMaterialResponse(rValues, ThisStressMeasure); //here the calculations are actually done
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::CalculateKinematicVariables(
    KinematicVariables& rThisKinematicVariables,
    const IndexType PointNumber,
    const GeometryType::IntegrationMethod& rIntegrationMethod) const
{
    const auto& r_geometry = GetGeometry();
    const auto& r_integration_points = r_geometry.IntegrationPoints(rIntegrationMethod);

    // Shape functions
    rThisKinematicVariables.N = r_geometry.ShapeFunctionsValues(rThisKinematicVariables.N, r_integration_points[PointNumber].Coordinates());

    // Calculate the inverse Jacobian
    GeometryUtils::JacobianOnInitialConfiguration(
        r_geometry,
        r_integration_points[PointNumber],
        rThisKinematicVariables.J0);
    MathUtils<double>::InvertMatrix(
        rThisKinematicVariables.J0,
        rThisKinematicVariables.InvJ0,
        rThisKinematicVariables.detJ0);
    KRATOS_ERROR_IF(rThisKinematicVariables.detJ0 < 0.0)
        << "Element ID: " << this->Id() << " is inverted. det(J0) = " << rThisKinematicVariables.detJ0 << std::endl;

    // Calculate the shape functions gradients
    GeometryUtils::ShapeFunctionsGradients(
        r_geometry.ShapeFunctionsLocalGradients(rIntegrationMethod)[PointNumber],
        rThisKinematicVariables.InvJ0,
        rThisKinematicVariables.DN_DX);

    // Calculate the equivalent total strain
    CalculateEquivalentStrain(rThisKinematicVariables);

    // Compute equivalent F
    //TODO: Check if these are really required. I think they shouldn't as we're computing the strain in the element
    // ComputeEquivalentF(rThisKinematicVariables.F, rThisKinematicVariables.EquivalentStrain);
    // rThisKinematicVariables.detF = MathUtils<double>::Det(rThisKinematicVariables.F);
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<2>::CalculateEquivalentStrain(KinematicVariables& rThisKinematicVariables) const
{
    // Define references to the auxiliary symbols
    const auto& N = rThisKinematicVariables.N;
    const auto& DN = rThisKinematicVariables.DN_DX;
    const auto& u = rThisKinematicVariables.Displacements;
    const auto& th = rThisKinematicVariables.JacobianDeterminant;
    auto& eq_green_strain = rThisKinematicVariables.EquivalentStrain;

    // Fill the equivalent Green strain values
    const double ceq_green_strain0 = DN(0,0)*u(0,1);
const double ceq_green_strain1 = DN(1,0)*u(1,1);
const double ceq_green_strain2 = DN(2,0)*u(2,1);
const double ceq_green_strain3 = ceq_green_strain0 + ceq_green_strain1 + ceq_green_strain2;
const double ceq_green_strain4 = DN(0,0)*u(0,0);
const double ceq_green_strain5 = DN(1,1)*u(1,1);
const double ceq_green_strain6 = DN(2,1)*u(2,1);
const double ceq_green_strain7 = DN(0,1)*u(0,1);
const double ceq_green_strain8 = DN(1,0)*u(1,0);
const double ceq_green_strain9 = DN(2,0)*u(2,0);
const double ceq_green_strain10 = DN(1,1)*u(1,0);
const double ceq_green_strain11 = DN(2,1)*u(2,0);
const double ceq_green_strain12 = DN(0,1)*u(0,0);
const double ceq_green_strain13 = ceq_green_strain4 + ceq_green_strain8 + ceq_green_strain9 + 1;
const double ceq_green_strain14 = ceq_green_strain5 + ceq_green_strain6 + ceq_green_strain7;
const double ceq_green_strain15 = pow(N[0]*th[0] + N[1]*th[1] + N[2]*th[2], 2)*1.0/(-ceq_green_strain0*ceq_green_strain10 - ceq_green_strain0*ceq_green_strain11 - ceq_green_strain1*ceq_green_strain11 - ceq_green_strain1*ceq_green_strain12 - ceq_green_strain10*ceq_green_strain2 - ceq_green_strain12*ceq_green_strain2 + ceq_green_strain13 + ceq_green_strain14 + ceq_green_strain4*ceq_green_strain5 + ceq_green_strain4*ceq_green_strain6 + ceq_green_strain5*ceq_green_strain9 + ceq_green_strain6*ceq_green_strain8 + ceq_green_strain7*ceq_green_strain8 + ceq_green_strain7*ceq_green_strain9);
const double ceq_green_strain16 = ceq_green_strain10 + ceq_green_strain11 + ceq_green_strain12;
const double ceq_green_strain17 = ceq_green_strain14 + 1;
eq_green_strain[0]=0.5*pow(ceq_green_strain13, 2)*ceq_green_strain15 + 0.5*ceq_green_strain15*pow(ceq_green_strain3, 2) - 0.5;
eq_green_strain[1]=0.5*ceq_green_strain15*pow(ceq_green_strain16, 2) + 0.5*ceq_green_strain15*pow(ceq_green_strain17, 2) - 0.5;
eq_green_strain[2]=1.0*ceq_green_strain15*(ceq_green_strain13*ceq_green_strain16 + ceq_green_strain17*ceq_green_strain3);

}

/***********************************************************************************/
/***********************************************************************************/

template<>
void TotalLagrangianMixedDetJElement<3>::CalculateEquivalentStrain(KinematicVariables& rThisKinematicVariables) const
{
    // Define references to the auxiliary symbols
    const auto& N = rThisKinematicVariables.N;
    const auto& DN = rThisKinematicVariables.DN_DX;
    const auto& u = rThisKinematicVariables.Displacements;
    const auto& th = rThisKinematicVariables.JacobianDeterminant;
    auto& eq_green_strain = rThisKinematicVariables.EquivalentStrain;

    // Fill the equivalent Green strain values
    //substitute_green_strain_3D_4N
}

// /***********************************************************************************/
// /***********************************************************************************/

// template<std::size_t TDim>
// void TotalLagrangianMixedDetJElement<TDim>::ComputeEquivalentF(
//     Matrix& rF,
//     const Vector& rStrainTensor) const
// {
//     StructuralMechanicsElementUtilities::ComputeEquivalentF(*this, rStrainTensor, rF);
// }

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
int  TotalLagrangianMixedDetJElement<TDim>::Check(const ProcessInfo& rCurrentProcessInfo) const
{
    KRATOS_TRY

    int check = TotalLagrangianMixedDetJElement::BaseType::Check(rCurrentProcessInfo);

    // Base check
    check = StructuralMechanicsElementUtilities::SolidElementCheck(*this, rCurrentProcessInfo, mConstitutiveLawVector);

    // Checking density
    KRATOS_ERROR_IF_NOT(GetProperties().Has(DENSITY)) << "DENSITY has to be provided for the calculation of body force." << std::endl;

    // Check that the element's nodes contain all required SolutionStepData and Degrees of freedom
    const auto& r_geometry = this->GetGeometry();
    for ( IndexType i = 0; i < r_geometry.size(); i++ ) {
        const NodeType& r_node = r_geometry[i];
        KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(DETERMINANT_F,r_node)
        KRATOS_CHECK_DOF_IN_NODE(DETERMINANT_F, r_node)
    }

    return check;

    KRATOS_CATCH( "" );
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::CalculateOnIntegrationPoints(
    const Variable<double>& rVariable,
    std::vector<double>& rOutput,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    const auto& r_geometry = GetGeometry();
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());

    const SizeType n_gauss = r_integration_points.size();
    if (rOutput.size() != n_gauss) {
        rOutput.resize(n_gauss);
    }

    if (mConstitutiveLawVector[0]->Has( rVariable)) {
        GetValueOnConstitutiveLaw(rVariable, rOutput);
    } else {
        CalculateOnConstitutiveLaw(rVariable, rOutput, rCurrentProcessInfo);
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::CalculateOnIntegrationPoints(
    const Variable<Vector>& rVariable,
    std::vector<Vector>& rOutput,
    const ProcessInfo& rCurrentProcessInfo
    )
{
    const auto& r_geometry = GetGeometry();
    const auto& r_integration_points = r_geometry.IntegrationPoints(GetIntegrationMethod());

    const SizeType n_gauss = r_integration_points.size();
    if (rOutput.size() != n_gauss) {
        rOutput.resize(n_gauss);
    }

    if (mConstitutiveLawVector[0]->Has( rVariable)) {
        GetValueOnConstitutiveLaw(rVariable, rOutput);
    } else if (rVariable == CAUCHY_STRESS_VECTOR || rVariable == PK2_STRESS_VECTOR) {
        // Create and initialize element variables:
        const SizeType n_nodes = r_geometry.PointsNumber();
        const SizeType dim = r_geometry.WorkingSpaceDimension();
        const SizeType strain_size = mConstitutiveLawVector[0]->GetStrainSize();

        // Create the kinematics container and fill the nodal data
        KinematicVariables kinematic_variables;
        for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
            const auto& r_disp = r_geometry[i_node].FastGetSolutionStepValue(DISPLACEMENT);
            for (IndexType d = 0; d < dim; ++d) {
                kinematic_variables.Displacements(i_node, d) = r_disp[d];
            }
            kinematic_variables.JacobianDeterminant[i_node] = r_geometry[i_node].FastGetSolutionStepValue(DETERMINANT_F);
        }

        // Create the constitutive variables and values containers
        ConstitutiveVariables constitutive_variables;
        ConstitutiveLaw::Parameters cons_law_values(r_geometry, GetProperties(), rCurrentProcessInfo);
        auto& r_cons_law_options = cons_law_values.GetOptions();
        r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_STRESS, true);
        r_cons_law_options.Set(ConstitutiveLaw::USE_ELEMENT_PROVIDED_STRAIN, true);
        r_cons_law_options.Set(ConstitutiveLaw::COMPUTE_CONSTITUTIVE_TENSOR, false);


        for (IndexType i_gauss = 0; i_gauss < n_gauss; ++i_gauss) {
            // Calculate kinematics
            CalculateKinematicVariables(kinematic_variables, i_gauss, GetIntegrationMethod());

            // Call the constitutive law to update material variables
            if( rVariable == CAUCHY_STRESS_VECTOR) {
                // Compute material reponse
                CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_integration_points, ConstitutiveLaw::StressMeasure_Cauchy);
            } else {
                // Compute material reponse
                CalculateConstitutiveVariables(kinematic_variables, constitutive_variables, cons_law_values, i_gauss, r_integration_points, ConstitutiveLaw::StressMeasure_PK2);
            }

            // Check sizes and save the output stress
            if (rOutput[i_gauss].size() != strain_size) {
                rOutput[i_gauss].resize(strain_size, false);
            }
            rOutput[i_gauss] = constitutive_variables.StressVector;
        }
    } else {
        CalculateOnConstitutiveLaw(rVariable, rOutput, rCurrentProcessInfo);
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
const Parameters TotalLagrangianMixedDetJElement<TDim>::GetSpecifications() const
{
    const Parameters specifications = Parameters(R"({
        "time_integration"           : ["static"],
        "framework"                  : "lagrangian",
        "symmetric_lhs"              : true,
        "positive_definite_lhs"      : true,
        "output"                     : {
            "gauss_point"            : ["CAUCHY_STRESS_VECTOR"],
            "nodal_historical"       : ["DISPLACEMENT","DETERMINANT_F"],
            "nodal_non_historical"   : [],
            "entity"                 : []
        },
        "required_variables"         : ["DISPLACEMENT","DETERMINANT_F"],
        "required_dofs"              : [],
        "flags_used"                 : [],
        "compatible_geometries"      : ["Triangle2D3", "Tetrahedra3D4"],
        "element_integrates_in_time" : true,
        "compatible_constitutive_laws": {
            "type"        : ["PlaneStrain","PlaneStress","ThreeDimensional"],
            "dimension"   : ["2D","3D"],
            "strain_size" : [3,6]
        },
        "required_polynomial_degree_of_geometry" : 1,
        "documentation"   :
            "This element implements a mixed displacement - volumetric strain formulation with Variational MultiScales (VMS) stabilization. This formulation is capable to deal with materials in the incompressible limit as well as with anisotropy."
    })");

    const SizeType dimension = GetGeometry().WorkingSpaceDimension();
    if (TDim == 2) {
        std::vector<std::string> dofs_2d({"DISPLACEMENT_X","DISPLACEMENT_Y","DETERMINANT_F"});
        specifications["required_dofs"].SetStringArray(dofs_2d);
    } else {
        std::vector<std::string> dofs_3d({"DISPLACEMENT_X","DISPLACEMENT_Y","DISPLACEMENT_Z","DETERMINANT_F"});
        specifications["required_dofs"].SetStringArray(dofs_3d);
    }

    return specifications;
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::save(Serializer& rSerializer) const
{
    KRATOS_SERIALIZE_SAVE_BASE_CLASS(rSerializer, TotalLagrangianMixedDetJElement::BaseType);
    int IntMethod = int(this->GetIntegrationMethod());
    rSerializer.save("IntegrationMethod",IntMethod);
    rSerializer.save("ConstitutiveLawVector", mConstitutiveLawVector);
}

/***********************************************************************************/
/***********************************************************************************/

template<std::size_t TDim>
void TotalLagrangianMixedDetJElement<TDim>::load(Serializer& rSerializer)
{
    KRATOS_SERIALIZE_LOAD_BASE_CLASS(rSerializer, TotalLagrangianMixedDetJElement::BaseType);
    int IntMethod;
    rSerializer.load("IntegrationMethod",IntMethod);
    mThisIntegrationMethod = IntegrationMethod(IntMethod);
    rSerializer.load("ConstitutiveLawVector", mConstitutiveLawVector);
}

/***********************************************************************************/
/***********************************************************************************/

// Explicit template instantiations
template class TotalLagrangianMixedDetJElement<2>;
template class TotalLagrangianMixedDetJElement<3>;

} // Namespace Kratos
