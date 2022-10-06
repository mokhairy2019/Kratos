//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: Kratos/license.txt
//
//  Main authors:    Miguel Maso Sotomayor
//


#ifndef KRATOS_SHALLOW_WATER_APPLICATION_VARIABLES_H_INCLUDED
#define KRATOS_SHALLOW_WATER_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "containers/variable.h"
#include "includes/define.h"
#include "includes/element.h"

namespace Kratos
{
    // Primary variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, HEIGHT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, FREE_SURFACE_ELEVATION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, VERTICAL_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, FLOW_RATE)

    // Physical variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, BATHYMETRY)                // Topographic definition of the marine domain
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, TOPOGRAPHY)                // Topographic definition of the domain
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, FROUDE)                    // Ratio of the flow inertia to the gravity forces
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, RAIN)                      // Source term
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, MANNING)                   // Friction coefficient
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, CHEZY)                     // Friction coefficient
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, ATMOSPHERIC_PRESSURE)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, WIND)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, DISPERSION_H)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, DISPERSION_V)

    // Auxiliary variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, bool,   INTEGRATE_BY_PARTS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, SHOCK_STABILIZATION_FACTOR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, DRY_HEIGHT)                // Definition of the dry domain
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, RELATIVE_DRY_HEIGHT)       // Definition of the dry domain
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, DRY_DISCHARGE_PENALTY)     // Dry domain penalty for the specific discharge
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Vector, FIRST_DERIVATIVE_WEIGHTS)  // For the derivatives recovery
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Vector, SECOND_DERIVATIVE_WEIGHTS) // For the derivatives recovery

    // Absorbing boundaries variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, ABSORBING_DISTANCE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, DISSIPATION)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Node<3>::Pointer, BOUNDARY_NODE)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, BOUNDARY_VELOCITY)

    // Post-process variables
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, TOPOGRAPHY_GRADIENT)

    // Specific variables for PFEM2
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, PROJECTED_SCALAR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, DELTA_SCALAR)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, PROJECTED_VECTOR)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, DELTA_VECTOR)

    // Variables for Flux Corrected Transport algorithm
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Vector, POSITIVE_FLUX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Vector, NEGATIVE_FLUX)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, POSITIVE_RATIO)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, NEGATIVE_RATIO)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, Vector, CUMULATIVE_CORRECTIONS)

    // Benchark variables
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, EXACT_HEIGHT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, HEIGHT_ERROR)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, EXACT_FREE_SURFACE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(SHALLOW_WATER_APPLICATION, double, FREE_SURFACE_ERROR)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, EXACT_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, VELOCITY_ERROR)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, EXACT_MOMENTUM)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(SHALLOW_WATER_APPLICATION, MOMENTUM_ERROR)
}

#endif	/* KRATOS_SHALLOW_WATER_APPLICATION_VARIABLES_H_INCLUDED */
