//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics FemDem Application
//
//  License:         BSD License
//                     Kratos default license: kratos/license.txt
//
//  Main authors:    Alejandro Cornejo Velazquez
//

#pragma once

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "includes/variables.h"
#include "includes/kratos_application.h"
#include "custom_elements/spheric_particle.h"

namespace Kratos
{
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,BACKUP_LAST_STRUCTURAL_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,BACKUP_LAST_STRUCTURAL_DISPLACEMENT)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,SMOOTHED_STRUCTURAL_VELOCITY)

    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,OLD_RELAXED_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,RELAXED_VELOCITY)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,FSI_INTERFACE_RESIDUAL)

    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,ACCELERATION_BACKUP)
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,DISPLACEMENT_BACKUP)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, HARDENING_MODULUS)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, MAX_DAMAGE_ERASE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, SMOOTHING_OF_STRESSES)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, PRESSURE_VOLUME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, PRESSURE_INITIAL_VOLUME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,SphericParticle*, DEM_PARTICLE_POINTER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, VOLUME_COUNTED)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, FRAGILE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, COHESION_MC)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, DAMAGE_ELEMENT)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, ERASED_VOLUME)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, TIME_UNIT_CONVERTER)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, STRESS_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, DISPLACEMENT_INCREMENT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, YIELD_STRESS_C);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, YIELD_STRESS_T);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, INTERNAL_PRESSURE_ITERATION);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, PFEM_PRESSURE_ITERATION);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, FRAC_ENERGY_T)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, FRAC_ENERGY_C)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, NODAL_DAMAGE)
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, STRESS_VECTOR_INTEGRATED);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, SMOOTHED_STRESS_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,std::string, YIELD_SURFACE);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, STRAIN_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, TANGENT_CONSTITUTIVE_TENSOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, DEMFEM_CONTACT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, SMOOTHING);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, GENERATE_DEM);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, RECOMPUTE_NEIGHBOURS);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, IS_DAMAGED);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, RECONSTRUCT_PRESSURE_LOAD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, IS_DYNAMIC);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, STRESS_THRESHOLD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, INITIAL_THRESHOLD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, INTEGRATION_COEFFICIENT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,std::string, MAPPING_PROCEDURE);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, IS_DEM);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, IS_SKIN);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, DEM_RADIUS);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, DEM_GENERATED);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, PRESSURE_EXPANDED);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, INACTIVE_NODE);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, NUMBER_OF_ACTIVE_ELEMENTS);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, NODAL_FORCE_APPLIED);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, NODAL_FORCE_X);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, NODAL_FORCE_Y);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, NODAL_FORCE_Z);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, PLASTIC_UNIAXIAL_STRESS);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, MAX_PLASTIC_STRAIN);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, NODAL_STRESS_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double, EQUIVALENT_NODAL_STRESS);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,EQUIVALENT_NODAL_STRESS_GRADIENT);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,AUXILIAR_GRADIENT);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(FEM_TO_DEM_APPLICATION,FORCE_LOAD);


    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix, STRAIN_TENSOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix, STRESS_TENSOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix, STRESS_TENSOR_INTEGRATED);

    // composite
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix, MATRIX_STRESS_TENSOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix, FIBER_STRESS_TENSOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, MATRIX_STRESS_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Vector, FIBER_STRESS_VECTOR);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,YOUNG_MODULUS_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,DENSITY_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,POISSON_RATIO_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,FIBER_VOLUMETRIC_PART);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,Matrix,MATRIX_STRESS_TENSOR_INTEGRATED);

    //plasticity FIBER
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,YIELD_STRESS_C_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,YIELD_STRESS_T_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,FRACTURE_ENERGY_FIBER);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,ACUMULATED_PLASTIC_STRAIN);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,double,EQUIVALENT_STRESS_VM);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int,HARDENING_LAW);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,bool, IS_TAKEN);
    KRATOS_DEFINE_APPLICATION_VARIABLE(FEM_TO_DEM_APPLICATION,int, PRESSURE_ID);

}
