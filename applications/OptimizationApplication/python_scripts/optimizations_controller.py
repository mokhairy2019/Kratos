# ==============================================================================
#  KratosOptimizationApplication
#
#  License:         BSD License
#                   license: OptimizationApplication/license.txt
#
#  Main authors:    Reza Najian Asl, https://github.com/RezaNajian
#
# ==============================================================================

# Making KratosMultiphysics backward compatible with python 2.6 and 2.7
from __future__ import print_function, absolute_import, division

# Kratos Core and Apps
import KratosMultiphysics as KM
from KratosMultiphysics import Parameters, Logger

# Additional imports
import KratosMultiphysics.OptimizationApplication.algorithms.algorithm_steepest_descent as steepest_descent
import KratosMultiphysics.OptimizationApplication.algorithms.algorithm_gradient_projection as gradient_projection
import time as timer

# ==============================================================================
def CreateController(optimizations_settings,model,model_parts_controller,analyses_controller,responses_controller,controls_controller):
    return OptimizationsController(optimizations_settings,model,model_parts_controller,analyses_controller,responses_controller,controls_controller)

# ==============================================================================
class OptimizationsController:
    # --------------------------------------------------------------------------
    def __init__(self,optimizations_settings,model,model_parts_controller,analyses_controller,responses_controller,controls_controller):
        
        self.optimizations_settings = optimizations_settings
        self.controls_controller = controls_controller
        self.responses_controller = responses_controller
        self.model = model

        default_settings = KM.Parameters("""
        {
            "name"                : "OPT_NAME",
            "type"                : "OPT_TYPE",
            "settings"                : {
                "objectives": [],
                "objectives_weights": [],
                "constraints": [],
                "constraints_types": [],
                "constraints_ref_values": [],
                "controls": [],
                "controls_maximum_updates": [],
                "controls_lower_bounds": [],
                "controls_lower_bounds_values": [],
                "controls_upper_bounds": [],
                "controls_upper_bounds_values": [],                
                "algorithm" : "ALG_NAME",
                "algorithm_settings" : {}
            }
        }""")

        if not self.optimizations_settings.size()>0:
            raise RuntimeError("OptimizationsController: optimizations list in optimizer's parameters can not be empty !") 

        for itr in range(self.optimizations_settings.size()):
            for key in default_settings.keys():
                if not self.optimizations_settings[itr].Has(key):
                    raise RuntimeError("OptimizationsController: Required entry '{}' missing in 'optimization Nr.{}'!".format(key,itr+1))  
            self.optimizations_settings[itr].ValidateAndAssignDefaults(default_settings)
            opt_name = self.optimizations_settings[itr]["name"].GetString()
            for key in default_settings["settings"].keys():
                if not self.optimizations_settings[itr]["settings"].Has(key):
                    raise RuntimeError("OptimizationsController: Required entry '{}' missing in settings of optimization '{}' !".format(key,opt_name))
            self.optimizations_settings[itr]["settings"].ValidateAndAssignDefaults(default_settings["settings"])  


        self.optimizations = {}
        self.optimizations_types={}
        self.optimizations_objectives={}
        self.optimizations_objectives_weights={}
        self.optimizations_constraints={}
        self.optimizations_constraints_type={}
        self.optimizations_constraints_ref_values={}
        self.optimizations_controls={}
        self.optimizations_controls_maximum_updates={}
        self.optimizations_controls_lower_bounds={}
        self.optimizations_controls_lower_bounds_values={}
        self.optimizations_controls_upper_bounds={}
        self.optimizations_controls_upper_bounds_values={}
        self.optimizations_algorithm={}
        self.supported_opt_types = ["gradient_based"]
        self.supported_algorithms = ["steepest_descent","gradient_projection"]
        for itr in range(self.optimizations_settings.size()):
            opt_settings = self.optimizations_settings[itr]
            opt_name = opt_settings["name"].GetString()
            opt_type = opt_settings["type"].GetString()   
            # check for name
            if opt_name in self.optimizations.keys():  
                raise RuntimeError("OptimizationsController: Optimization name '{}' already exists.".format(opt_name))       
            # check for type
            if not opt_type in self.supported_opt_types:  
                raise RuntimeError("OptimizationsController: Optimization type '{}' is not supported, supprted types {}.".format(opt_type,self.supported_opt_types))                  
            self.optimizations_types[opt_name]=opt_type
            # checks for objectives
            objectives_names = opt_settings["settings"]["objectives"].GetStringArray()
            if not len(objectives_names)>0:  
                raise RuntimeError("OptimizationsController: Objectives list of optimization '{}' can not be empty.".format(opt_name))   
            if len(set(objectives_names)) != len(objectives_names):
                raise RuntimeError("OptimizationsController: Objectives list of optimization '{}' has duplicate response names .".format(opt_name))
            self.responses_controller.CheckIfResponsesExist(objectives_names)

            # for key,values in self.optimizations_objectives.items():
            #     compare_results = set(objectives_names) & set(values)
            #     if len(compare_results):
            #         raise RuntimeError("OptimizationsController: Responses {} can not be shared between optimizations '{}' and '{}' .".format(compare_results,key,opt_name))

            # for key,values in self.optimizations_constraints.items():
            #     compare_results = set(objectives_names) & set(values)
            #     if len(compare_results):
            #         raise RuntimeError("OptimizationsController: Responses {} can not be shared between optimizations '{}' and '{}' .".format(compare_results,key,opt_name))

            self.optimizations_objectives[opt_name]=objectives_names
            # check for objectives weights
            if not opt_settings["settings"]["objectives_weights"].IsVector():  
                raise RuntimeError("OptimizationsController:'objectives_weights' of optimization '{}' should be vector of doubles.".format(opt_name)) 
            objectives_weights = opt_settings["settings"]["objectives_weights"].GetVector()     
            if not len(objectives_weights) == len(objectives_names):
                raise RuntimeError("OptimizationsController:'objectives_weights' of optimization '{}' should be of the same size of objectives list.".format(opt_name))
            self.optimizations_objectives_weights[opt_name]=objectives_weights



            # checks for constraints
            if opt_settings["settings"]["constraints"].size()>0:
                constraints_names = opt_settings["settings"]["constraints"].GetStringArray()
                if len(set(constraints_names)) != len(constraints_names):
                    raise RuntimeError("OptimizationsController: Constraint list of optimization '{}' has duplicate response names .".format(opt_name))
                self.responses_controller.CheckIfResponsesExist(constraints_names)

                # for key,values in self.optimizations_objectives.items():
                #     compare_results = set(constraints_names) & set(values)
                #     if len(compare_results):
                #         raise RuntimeError("OptimizationsController: Responses {} can not be shared between optimizations '{}' and '{}' .".format(compare_results,key,opt_name))

                # for key,values in self.optimizations_constraints.items():
                #     compare_results = set(constraints_names) & set(values)
                #     if len(compare_results):
                #         raise RuntimeError("OptimizationsController: Responses {} can not be shared between optimizations '{}' and '{}' .".format(compare_results,key,opt_name))


                for constraints_name in constraints_names:
                    if constraints_name in objectives_names:
                        raise RuntimeError("OptimizationsController: Response {} in optimization {} is used as both objective and constraint.".format(constraints_name,opt_name))
                self.optimizations_constraints[opt_name]=constraints_names

                constraints_types = opt_settings["settings"]["constraints_types"].GetStringArray()  
                if not len(constraints_types) == len(constraints_names):
                    raise RuntimeError("OptimizationsController:'constraints_types' of optimization '{}' should be of the same size of constraint list.".format(opt_name))
                for index, type in enumerate(constraints_types):
                    if not type in ["equality","smaller_than","bigger_than","initial_value_equality","smaller_than_initial_value","bigger_than_initial_value"]: 
                        raise RuntimeError("OptimizationsController: constraint type {} of constraint {} of optimization '{}' should be either 'equality' or 'inequality'.".format(type,constraints_names[index],opt_name,))
                self.optimizations_constraints_type[opt_name]=constraints_types                

                if not opt_settings["settings"]["constraints_ref_values"].IsVector():  
                    raise RuntimeError("OptimizationsController:'constraints_ref_values' of optimization '{}' should be vector of doubles.".format(opt_name)) 
                constraints_ref_values = opt_settings["settings"]["constraints_ref_values"].GetVector() 
                if not len(constraints_ref_values) == len(constraints_names):
                    raise RuntimeError("OptimizationsController:'constraints_ref_values' of optimization '{}' should be of the same size of constraint list.".format(opt_name))
                self.optimizations_constraints_ref_values[opt_name]=constraints_ref_values
            
            # checks for controls
            controls_names = opt_settings["settings"]["controls"].GetStringArray()
            if not len(controls_names)>0:  
                raise RuntimeError("OptimizationsController: Controls list of optimization '{}' can not be empty.".format(opt_name))   
            if len(set(controls_names)) != len(controls_names):
                raise RuntimeError("OptimizationsController: Controls list of optimization '{}' has duplicate control names .".format(opt_name))
            self.controls_controller.CheckIfControlsExist(controls_names)


            for key,values in self.optimizations_controls.items():
                compare_results = set(controls_names) & set(values)
                if len(compare_results):
                    raise RuntimeError("OptimizationsController: Controls {} can not be shared between optimizations '{}' and '{}' .".format(compare_results,key,opt_name))
            
            self.optimizations_controls[opt_name]=controls_names

            if opt_settings["settings"]["controls_maximum_updates"].size() != len(controls_names):
                raise RuntimeError("OptimizationsController: 'controls_maximum_updates' of optimization '{}' should be of the same size of control list .".format(opt_name))            

            controls_maximum_updates=[]
            for i in range(opt_settings["settings"]["controls_maximum_updates"].size()):
                if not opt_settings["settings"]["controls_maximum_updates"][i].IsNumber():
                    raise RuntimeError("OptimizationsController: entry {} of 'controls_maximum_updates' of optimization '{}' should be a number .".format(i+1,opt_name))
                else:
                    controls_maximum_updates.append(opt_settings["settings"]["controls_maximum_updates"][i].GetDouble())

            self.optimizations_controls_maximum_updates[opt_name] = controls_maximum_updates

            if opt_settings["settings"]["controls_lower_bounds"].size() != len(controls_names):
                raise RuntimeError("OptimizationsController: 'controls_lower_bounds' of optimization '{}' should be of the same size of control list .".format(opt_name))

            controls_lower_bounds=[]
            for i in range(opt_settings["settings"]["controls_lower_bounds"].size()):
                if not opt_settings["settings"]["controls_lower_bounds"][i].IsBool():
                    raise RuntimeError("OptimizationsController: entry {} of 'controls_lower_bounds' of optimization '{}' should be a bool type .".format(i+1,opt_name))
                else:
                    controls_lower_bounds.append(opt_settings["settings"]["controls_lower_bounds"][i].GetBool())

            self.optimizations_controls_lower_bounds[opt_name] = controls_lower_bounds

            if opt_settings["settings"]["controls_lower_bounds_values"].size() != len(controls_names):
                raise RuntimeError("OptimizationsController: 'controls_lower_bounds_values' of optimization '{}' should be of the same size of control list .".format(opt_name))

            controls_lower_bounds_values=[]
            for i in range(opt_settings["settings"]["controls_lower_bounds_values"].size()):
                if not opt_settings["settings"]["controls_lower_bounds_values"][i].IsNumber():
                    raise RuntimeError("OptimizationsController: entry {} of 'controls_lower_bounds_values' of optimization '{}' should be a number .".format(i+1,opt_name))
                else:
                    controls_lower_bounds_values.append(opt_settings["settings"]["controls_lower_bounds_values"][i].GetDouble())

            self.optimizations_controls_lower_bounds_values[opt_name] = controls_lower_bounds_values

            if opt_settings["settings"]["controls_upper_bounds"].size() != len(controls_names):
                raise RuntimeError("OptimizationsController: 'controls_upper_bounds' of optimization '{}' should be of the same size of control list .".format(opt_name))

            controls_upper_bounds=[]
            for i in range(opt_settings["settings"]["controls_upper_bounds"].size()):
                if not opt_settings["settings"]["controls_upper_bounds"][i].IsBool():
                    raise RuntimeError("OptimizationsController: entry {} of 'controls_upper_bounds' of optimization '{}' should be a bool type .".format(i+1,opt_name))
                else:
                    controls_upper_bounds.append(opt_settings["settings"]["controls_upper_bounds"][i].GetBool())
            
            self.optimizations_controls_upper_bounds[opt_name] = controls_upper_bounds

            if opt_settings["settings"]["controls_upper_bounds_values"].size() != len(controls_names):
                raise RuntimeError("OptimizationsController: 'controls_upper_bounds_values' of optimization '{}' should be of the same size of control list .".format(opt_name))

            controls_upper_bounds_values=[]
            for i in range(opt_settings["settings"]["controls_upper_bounds_values"].size()):
                if not opt_settings["settings"]["controls_upper_bounds_values"][i].IsNumber():
                    raise RuntimeError("OptimizationsController: entry {} of 'controls_upper_bounds_values' of optimization '{}' should be a number .".format(i+1,opt_name))
                else:
                    controls_upper_bounds_values.append(opt_settings["settings"]["controls_upper_bounds_values"][i].GetDouble())

            self.optimizations_controls_upper_bounds_values[opt_name] = controls_upper_bounds_values

            algorithm = opt_settings["settings"]["algorithm"].GetString()
            if not algorithm in self.supported_algorithms:
                raise RuntimeError("OptimizationsController: Optimization algorithm '{}' is not supported, supprted types {}.".format(algorithm,self.supported_algorithms))                  

            if algorithm == "steepest_descent":
                self.optimizations[opt_name] = steepest_descent.AlgorithmSteepestDescent(opt_name,opt_settings["settings"],model,model_parts_controller,analyses_controller,responses_controller,controls_controller)
            elif algorithm == "gradient_projection":
                self.optimizations[opt_name] = gradient_projection.AlgorithmGradientProjection(opt_name,opt_settings["settings"],model,model_parts_controller,analyses_controller,responses_controller,controls_controller)
    # --------------------------------------------------------------------------
    def Initialize(self):
        for opt in self.optimizations.values():
            opt.InitializeOptimizationLoop()

    # --------------------------------------------------------------------------
    def Optimize(self,opt_name):
        if not opt_name in self.optimizations.keys():
            raise RuntimeError("OptimizationsController:Optimize: Optimization {} doesn not exist !.".format(opt_name))
        
        self.optimizations[opt_name].RunOptimizationLoop()
    # --------------------------------------------------------------------------
    def OptimizeAll(self):
        for opt in self.optimizations.values():
            opt.RunOptimizationLoop()



            

               

