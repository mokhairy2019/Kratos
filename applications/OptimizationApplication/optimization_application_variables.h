// ==============================================================================
//  KratosOptimizationApplication
//
//  License:         BSD License
//                   license: OptimizationApplication/license.txt
//
//  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
//
// ==============================================================================

#if !defined(KRATOS_OPTIMIZATION_APPLICATION_VARIABLES_H_INCLUDED )
#define  KRATOS_OPTIMIZATION_APPLICATION_VARIABLES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "includes/define.h"
#include "containers/variable.h"
#include <unordered_map>
using namespace std;

namespace Kratos
{

    // Optimization variables
    // strain energy
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_STRAIN_ENERGY_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_STRAIN_ENERGY_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_STRAIN_ENERGY_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_STRAIN_ENERGY_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_STRAIN_ENERGY_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_STRAIN_ENERGY_D_CD);

    // mass
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MASS_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MASS_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MASS_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MASS_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MASS_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MASS_D_CD);

    // eigenfrequency
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_EIGEN_FREQ_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_EIGEN_FREQ_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_EIGEN_FREQ_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_EIGEN_FREQ_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_EIGEN_FREQ_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_EIGEN_FREQ_D_CD);  

    // local_stress
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_LOCAL_STRESS_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_LOCAL_STRESS_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_LOCAL_STRESS_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_LOCAL_STRESS_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_LOCAL_STRESS_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_LOCAL_STRESS_D_CD);      


    // max_stress
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MAX_STRESS_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MAX_STRESS_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_CD);   

    // max_stress
	KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MAX_STRES_D_X);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION, D_MAX_STRES_D_CX);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_PT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_CT);
	KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_PD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION, double, D_MAX_STRESS_D_CD);        

    // shape control
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION,CX);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION,D_CX);  
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS(OPTIMIZATION_APPLICATION,D_X);   

    // thickness control
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,PT);    
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,FT);    
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,CT);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,D_CT);  
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,D_PT);  

    // density control
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,PD);    
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,FD);    
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,CD);
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,D_CD);  
    KRATOS_DEFINE_APPLICATION_VARIABLE(OPTIMIZATION_APPLICATION,double,D_PD);      

    // For implicit vertex-morphing with Helmholtz PDE
	KRATOS_DEFINE_VARIABLE( Matrix, HELMHOLTZ_MASS_MATRIX );
	KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_SURF_RADIUS_SHAPE );
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_BULK_RADIUS_SHAPE );
	KRATOS_DEFINE_VARIABLE( bool, COMPUTE_CONTROL_POINTS_SHAPE );
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_SURF_POISSON_RATIO_SHAPE );
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_BULK_POISSON_RATIO_SHAPE );
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS( OPTIMIZATION_APPLICATION, HELMHOLTZ_VARS_SHAPE);
    KRATOS_DEFINE_3D_APPLICATION_VARIABLE_WITH_COMPONENTS( OPTIMIZATION_APPLICATION, HELMHOLTZ_SOURCE_SHAPE); 

    // For thickness optimization
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_VAR_THICKNESS);
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_SOURCE_THICKNESS); 
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_RADIUS_THICKNESS); 
    // For topology optimization
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_VAR_DENSITY);
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_SOURCE_DENSITY); 
    KRATOS_DEFINE_VARIABLE( double, HELMHOLTZ_RADIUS_DENSITY);      

}

#endif	/* KRATOS_OPTIMIZATION_APPLICATION_VARIABLES_H_INCLUDED */