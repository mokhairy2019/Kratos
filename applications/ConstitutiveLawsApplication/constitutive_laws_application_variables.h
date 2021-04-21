// KRATOS   ___                _   _ _         _   _             __                       _
//        / __\___  _ __  ___| |_(_) |_ _   _| |_(_)_   _____  / /  __ ___      _____   /_\  _ __  _ __
//      / /  / _ \| '_ \/ __| __| | __| | | | __| \ \ / / _ \/ /  / _` \ \ /\ / / __| //_\\| '_ \| '_  |
//     / /__| (_) | | | \__ \ |_| | |_| |_| | |_| |\ V /  __/ /__| (_| |\ V  V /\__ \/  _  \ |_) | |_) |
//     \____/\___/|_| |_|___/\__|_|\__|\__,_|\__|_| \_/ \___\____/\__,_| \_/\_/ |___/\_/ \_/ .__/| .__/
//                                                                                         |_|   |_|
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Alejandro Cornejo Velazquez
//                   Riccardo Rossi
//

#if !defined(KRATOS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED)
#define  KRATOS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/mat_variables.h"

namespace Kratos
{
    enum class SofteningType {Linear = 0, Exponential = 1, HardeningDamage = 2};

    enum class TangentOperatorEstimation {Analytic = 0, FirstOrderPerturbation = 1, SecondOrderPerturbation = 2, Secant = 3, SecondOrderPerturbationV2 = 4};

///@name  Functions
///@{
    // Constitutive laws variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, HIGH_CYCLE_FATIGUE_COEFFICIENTS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, FATIGUE_REDUCTION_FACTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, LOCAL_NUMBER_OF_CYCLES)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, WOHLER_STRESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, REVERSION_FACTOR_RELATIVE_ERROR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, MAX_STRESS_RELATIVE_ERROR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, MAX_STRESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, THRESHOLD_STRESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, CYCLE_INDICATOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, CYCLES_TO_FAILURE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, TIME_INCREMENT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, DAMAGE_ACTIVATION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, PREVIOUS_CYCLE);
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, CYCLE_PERIOD)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, ADVANCE_STRATEGY_APPLIED);

    // Constitutive laws variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, INELASTIC_FLAG)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, YIELD_STRESS_TENSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, PLASTIC_STRAIN_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, BACK_STRESS_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, PLASTIC_DEFORMATION_GRADIENT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, YIELD_STRESS_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DILATANCY_ANGLE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, SOFTENING_TYPE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, SOFTENING_TYPE_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, HARDENING_CURVE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, MAX_NUMBER_NL_CL_ITERATIONS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, VISCOUS_PARAMETER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DELAY_TIME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, MAXIMUM_STRESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, MAXIMUM_STRESS_POSITION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, PLASTIC_DISSIPATION_LIMIT_LINEAR_SOFTENING)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, UNIAXIAL_STRESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, FRICTION_ANGLE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE_FIBER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE_MATRIX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, THRESHOLD)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, COHESION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, INTEGRATED_STRESS_TENSOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, PLASTIC_STRAIN_TENSOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE_TENSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, THRESHOLD_TENSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, THRESHOLD_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, UNIAXIAL_STRESS_TENSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, UNIAXIAL_STRESS_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, FRACTURE_ENERGY_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, CURVE_FITTING_PARAMETERS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, TANGENCY_REGION2)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, PLASTIC_STRAIN_INDICATORS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, EQUIVALENT_PLASTIC_STRAIN)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, KINEMATIC_PLASTICITY_PARAMETERS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, KINEMATIC_HARDENING_TYPE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, bool, CONSIDER_PERTURBATION_THRESHOLD)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, TANGENT_OPERATOR_ESTIMATION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, TENSION_STRESS_TENSOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, BACK_STRESS_TENSOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, COMPRESSION_STRESS_TENSOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, TENSION_STRESS_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, LAYER_EULER_ANGLES)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, COMPRESSION_STRESS_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, EFFECTIVE_TENSION_STRESS_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Vector, EFFECTIVE_COMPRESSION_STRESS_VECTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, CAUCHY_STRESS_TENSOR_FIBER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, CAUCHY_STRESS_TENSOR_MATRIX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, GREEN_LAGRANGE_STRAIN_TENSOR_MATRIX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, Matrix, GREEN_LAGRANGE_STRAIN_TENSOR_FIBER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, ACCUMULATED_PLASTIC_STRAIN)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS( CONSTITUTIVE_LAWS_APPLICATION, EULER_ANGLES)

    // D+D- Damage Constitutive laws variables, additional Masonry 2D & 3D
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, DAMAGE_ONSET_STRESS_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, BIAXIAL_COMPRESSION_MULTIPLIER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, FRACTURE_ENERGY_TENSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, FRACTURE_ENERGY_DAMAGE_PROCESS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, RESIDUAL_STRESS_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, BEZIER_CONTROLLER_C1)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, BEZIER_CONTROLLER_C2)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, BEZIER_CONTROLLER_C3)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, YIELD_STRAIN_COMPRESSION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, SHEAR_COMPRESSION_REDUCTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, TRIAXIAL_COMPRESSION_COEFFICIENT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, INTEGRATION_IMPLEX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, int, TENSION_YIELD_MODEL)

    // For anisotropy + orthotrophy
    // The ratios between the yield strength in the isotropic space and the anisotropic space
    // at each direction in local coordinates ratio_x = ft / ft,x
    KRATOS_DEFINE_SYMMETRIC_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(CONSTITUTIVE_LAWS_APPLICATION, ISOTROPIC_ANISOTROPIC_YIELD_RATIO );
    KRATOS_DEFINE_SYMMETRIC_3D_TENSOR_APPLICATION_VARIABLE_WITH_COMPONENTS(CONSTITUTIVE_LAWS_APPLICATION, ORTHOTROPIC_ELASTIC_CONSTANTS );

    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, SERIAL_PARALLEL_EQUILIBRIUM_TOLERANCE);
    KRATOS_DEFINE_APPLICATION_VARIABLE(CONSTITUTIVE_LAWS_APPLICATION, double, PLASTIC_DAMAGE_PROPORTION);

}

#endif // KRATOS_CONSTITUTIVE_LAWS_APPLICATION_VARIABLES_H_INCLUDED
