// ==============================================================================
//  KratosOptimizationApplication
//
//  License:         BSD License
//                   license: OptimizationApplication/license.txt
//
//  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
//
// ==============================================================================

#include "optimization_application_variables.h"

namespace Kratos
{

    // KRATOS_CREATE_VARIABLE(double,TEST_MAP);

    //Auxilary field
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(AUXILIARY_FIELD);

    //linear function
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LINEAR_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LINEAR_D_CX); 

    //symmetry plane
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_PLANE_SYMMETRY_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_PLANE_SYMMETRY_D_CX);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(NEAREST_NEIGHBOUR_POINT); 
    KRATOS_CREATE_VARIABLE(double, NEAREST_NEIGHBOUR_DIST);    
    KRATOS_CREATE_VARIABLE(int, NEAREST_NEIGHBOUR_COND_ID);

    //strain energy
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_1_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_1_D_CX);
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_2_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_2_D_CX);
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_3_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_STRAIN_ENERGY_3_D_CX);  

    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_1_D_PE);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_1_D_CD);

    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_2_D_PE);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_2_D_CD); 

    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_3_D_PE);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_3_D_CD);       


    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_1_D_PT);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_1_D_CT);

    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_2_D_PT);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_2_D_CT);

    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_3_D_PT);    
    KRATOS_CREATE_VARIABLE(double, D_STRAIN_ENERGY_3_D_CT);        
    

    //mass
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MASS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MASS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_MASS_D_PT);
    KRATOS_CREATE_VARIABLE(double, D_MASS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_MASS_D_PD);
    KRATOS_CREATE_VARIABLE(double, D_MASS_D_CD);  

    //eigenfrequency
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_EIGEN_FREQ_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_EIGEN_FREQ_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_PT);
    KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_PD);
    KRATOS_CREATE_VARIABLE(double, D_EIGEN_FREQ_D_CD);       

    //local_stress
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LOCAL_STRESS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_LOCAL_STRESS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_PT);
    KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_PD);
    KRATOS_CREATE_VARIABLE(double, D_LOCAL_STRESS_D_CD);     

    //max_stress
	KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MAX_STRESS_D_X);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_MAX_STRESS_D_CX);
	KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_PT);
    KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_CT);
	KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_PD);
    KRATOS_CREATE_VARIABLE(double, D_MAX_STRESS_D_CD);          

    // shape control
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(SX);    
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(CX);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_CX);  
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS(D_X);   

    // thickness control
    KRATOS_CREATE_VARIABLE(double,PT);
    KRATOS_CREATE_VARIABLE(double,FT);        
    KRATOS_CREATE_VARIABLE(double,CT);
    KRATOS_CREATE_VARIABLE(double,D_CT);  
    KRATOS_CREATE_VARIABLE(double,D_PT);   

    // density control
    KRATOS_CREATE_VARIABLE(double,PD);
    KRATOS_CREATE_VARIABLE(double,PE);
    KRATOS_CREATE_VARIABLE(double,FD);    
    KRATOS_CREATE_VARIABLE(double,CD);
    KRATOS_CREATE_VARIABLE(double,D_CD);  
    KRATOS_CREATE_VARIABLE(double,D_PD);            

    // For implicit vertex-morphing with Helmholtz PDE
	KRATOS_CREATE_VARIABLE( Matrix, HELMHOLTZ_MASS_MATRIX );
	KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_SURF_RADIUS_SHAPE );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_BULK_RADIUS_SHAPE );
	KRATOS_CREATE_VARIABLE( bool, COMPUTE_CONTROL_POINTS_SHAPE );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_SURF_POISSON_RATIO_SHAPE );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_BULK_POISSON_RATIO_SHAPE );
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( HELMHOLTZ_VARS_SHAPE);
    KRATOS_CREATE_3D_VARIABLE_WITH_COMPONENTS( HELMHOLTZ_SOURCE_SHAPE);  

    // for thickness optimization
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_VAR_THICKNESS );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_SOURCE_THICKNESS );  
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_RADIUS_THICKNESS );     
    // for topology optimization
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_VAR_DENSITY );
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_SOURCE_DENSITY );  
    KRATOS_CREATE_VARIABLE( double, HELMHOLTZ_RADIUS_DENSITY );         

}
