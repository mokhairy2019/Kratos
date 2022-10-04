//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//

#include "shallow_water_application_variables.h"

namespace Kratos
{
    // Primary variables
    KRATOS_CREATE_VARIABLE(double, HEIGHT)
    KRATOS_CREATE_VARIABLE(double, FREE_SURFACE_ELEVATION)
    KRATOS_CREATE_VARIABLE(double, VERTICAL_VELOCITY)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(FLOW_RATE)

    // Physical variables
    KRATOS_CREATE_VARIABLE(double, BATHYMETRY)
    KRATOS_CREATE_VARIABLE(double, TOPOGRAPHY)
    KRATOS_CREATE_VARIABLE(double, FROUDE)
    KRATOS_CREATE_VARIABLE(double, RAIN)
    KRATOS_CREATE_VARIABLE(double, MANNING)
    KRATOS_CREATE_VARIABLE(double, CHEZY)
    KRATOS_CREATE_VARIABLE(double, ATMOSPHERIC_PRESSURE)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(WIND)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DISPERSION_H)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DISPERSION_V)

    // Auxiliary variables
    KRATOS_CREATE_VARIABLE(bool,   INTEGRATE_BY_PARTS)
    KRATOS_CREATE_VARIABLE(double, SHOCK_STABILIZATION_FACTOR)
    KRATOS_CREATE_VARIABLE(double, DRY_HEIGHT)
    KRATOS_CREATE_VARIABLE(double, RELATIVE_DRY_HEIGHT)
    KRATOS_CREATE_VARIABLE(double, DRY_DISCHARGE_PENALTY)
    KRATOS_CREATE_VARIABLE(Vector, FIRST_DERIVATIVE_WEIGHTS)
    KRATOS_CREATE_VARIABLE(Vector, SECOND_DERIVATIVE_WEIGHTS)

    // Absorbing boundaries variables
    KRATOS_CREATE_VARIABLE(double, ABSORBING_DISTANCE)
    KRATOS_CREATE_VARIABLE(double, DISSIPATION)
    KRATOS_CREATE_VARIABLE(Node<3>::Pointer, BOUNDARY_NODE)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(BOUNDARY_VELOCITY)

    // Post-process variables
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(TOPOGRAPHY_GRADIENT)

    // Specific variables for PFEM2
    KRATOS_CREATE_VARIABLE(double, PROJECTED_SCALAR)
    KRATOS_CREATE_VARIABLE(double, DELTA_SCALAR)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(PROJECTED_VECTOR)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(DELTA_VECTOR)

    // Variables for Flux Corrected Transport algorithm
    KRATOS_CREATE_VARIABLE(Vector, POSITIVE_FLUX)
    KRATOS_CREATE_VARIABLE(Vector, NEGATIVE_FLUX)
    KRATOS_CREATE_VARIABLE(double, POSITIVE_RATIO)
    KRATOS_CREATE_VARIABLE(double, NEGATIVE_RATIO)
    KRATOS_CREATE_VARIABLE(Vector, CUMULATIVE_CORRECTIONS)

    // Benchark variables
    KRATOS_CREATE_VARIABLE(double, EXACT_HEIGHT)
    KRATOS_CREATE_VARIABLE(double, HEIGHT_ERROR)
    KRATOS_CREATE_VARIABLE(double, EXACT_FREE_SURFACE)
    KRATOS_CREATE_VARIABLE(double, FREE_SURFACE_ERROR)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(EXACT_VELOCITY)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(VELOCITY_ERROR)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(EXACT_MOMENTUM)
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(MOMENTUM_ERROR)
}
