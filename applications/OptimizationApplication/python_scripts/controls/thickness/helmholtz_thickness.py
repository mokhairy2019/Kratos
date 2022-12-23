# ==============================================================================
#  KratosOptimizationApplication
#
#  License:         BSD License
#                   license: OptimizationApplication/license.txt
#
#  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
#
# ==============================================================================

import KratosMultiphysics as KM
import KratosMultiphysics.ShapeOptimizationApplication as KSO
import KratosMultiphysics.OptimizationApplication as KOA
from KratosMultiphysics.OptimizationApplication.controls.thickness.thickness_control import ThicknessControl

class HelmholtzThickness(ThicknessControl):

    def __init__(self, name, model, settings):
        super().__init__(name,model,settings)
        self.technique_settings = self.settings["technique_settings"]

        self.default_technique_settings = KM.Parameters("""{
                    "automatic_filter_size" : true,
                    "filter_radius" : 0.000000000001,
                    "beta_settings": {
                        "initial_value" : 25,
                        "max_value" : 25,
                        "adaptive" : false,
                        "increase_fac" : 1.5,
                        "update_period" : 20
                    },
                    "initial_thickness":0.000001,
                    "physical_thicknesses": [],
                    "fixed_model_parts": [],
                    "fixed_model_parts_thicknesses": [],    
                    "linear_solver_settings" : {
                        "solver_type" : "amgcl",
                        "smoother_type":"ilu0",
                        "krylov_type": "gmres",
                        "coarsening_type": "aggregation",
                        "max_iteration": 200,
                        "provide_coordinates": false,
                        "gmres_krylov_space_dimension": 100,
                        "verbosity" : 0,
                        "tolerance": 1e-7,
                        "scaling": false,
                        "block_size": 1,
                        "use_block_matrices_if_possible" : true,
                        "coarse_enough" : 5000
                    }
                }""")

        self.technique_settings.RecursivelyValidateAndAssignDefaults(self.default_technique_settings)


        # add vars
        for model_part_name in self.controlling_objects:
            root_model = model_part_name.split(".")[0]
            self.model.GetModelPart(root_model).AddNodalSolutionStepVariable(KOA.HELMHOLTZ_VAR_THICKNESS)
            self.model.GetModelPart(root_model).AddNodalSolutionStepVariable(KOA.HELMHOLTZ_SOURCE_THICKNESS)

        ## Construct the linear solver
        import KratosMultiphysics.python_linear_solver_factory as python_linear_solver_factory
        self.linear_solvers = []
        root_model_parts = []
        for model_part_name in self.controlling_objects:
            extracted_root_model_part_name = model_part_name.split(".")[0]
            if not extracted_root_model_part_name in root_model_parts:
                root_model_parts.append(extracted_root_model_part_name)
                self.linear_solvers.append(python_linear_solver_factory.ConstructSolver(self.technique_settings["linear_solver_settings"]))

        self.helmholtz_thickness_control = KOA.HelmholtzThickness(self.name,self.model,self.linear_solvers,self.settings)

    def Initialize(self):
        super().Initialize()
        self.helmholtz_thickness_control.Initialize()
    
    def MapFirstDerivative(self,derivative_variable_name,mapped_derivative_variable_name):
        self.helmholtz_thickness_control.MapFirstDerivative(derivative_variable_name,mapped_derivative_variable_name)

    def Compute(self):
        pass
        # self.helmholtz_thickness_control.MapControlUpdate(KOA.D_CX,KOA.D_X) 

    def Update(self):
        self.helmholtz_thickness_control.Update() 

    def GetControllingObjects(self):
        return self.controlling_objects
            
            


