# importing the Kratos Library
from numpy import gradient
from . import base_response
import KratosMultiphysics as KM
from KratosMultiphysics import Parameters, Logger
import KratosMultiphysics.OptimizationApplication as KOA
from KratosMultiphysics.OptimizationApplication.responses.base_response import BaseResponseFunction
import KratosMultiphysics.StructuralMechanicsApplication as KSM

import time as timer
import numpy as np

# ==============================================================================
class StrainEnergyResponseFunction(BaseResponseFunction):
    """Linear strain energy response function. It triggers the primal analysis and
    uses the primal analysis results to calculate response value and gradient.

    Attributes
    ----------
    primal_model_part : Model part of the primal analysis object
    primal_analysis : Primal analysis object of the response function
    response_function_utility: Cpp utilities object doing the actual computation of response value and gradient.
    """

    def __init__(self,response_name, response_settings,response_analysis,model):

        self.type = "strain_energy"
        self.variable = "STRAIN_ENERGY"
        super().__init__(response_name, response_settings, model, response_analysis)

        if not self.response_settings.Has("gradient_settings"):
            self.gradient_settings = KM.Parameters()
            self.gradient_settings.AddString("gradient_mode","semi_analytic")
            self.gradient_settings.AddDouble("step_size",1e-6)
        else:
            self.gradient_settings = self.response_settings["gradient_settings"]     

        self.supported_control_types = ["shape","thickness","density"]
        self.gradients_variables = {"shape":"D_STRAIN_ENERGY_D_X","thickness":"D_STRAIN_ENERGY_D_T","density":"D_STRAIN_ENERGY_D_P"}

        if len(self.evaluated_model_parts) != 1:
            raise RuntimeError("StrainEnergyResponseFunction: 'evaluated_objects' of response '{}' must have only one entry !".format(self.name)) 

        for control_type in self.control_types:
            if not control_type in self.supported_control_types:
                raise RuntimeError("StrainEnergyResponseFunction: type {} in 'control_types' of response '{}' is not supported, supported types are {}  !".format(control_type,self.name,self.supported_control_types)) 
        
        # add vars
        for control_type in self.control_types:
            if control_type == "shape":
                self.analysis_model_part.AddNodalSolutionStepVariable(KM.SHAPE_SENSITIVITY)
                self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))
            # if control_type == "thickness":
                self.analysis_model_part.AddNodalSolutionStepVariable(KSM.THICKNESS_SENSITIVITY)
                # self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))
            # if control_type == "density":
                self.analysis_model_part.AddNodalSolutionStepVariable(KSM.YOUNG_MODULUS_SENSITIVITY)
                # self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))

        # create response
        self.response_function_utility = KSM.StrainEnergyResponseFunctionUtility(self.analysis_model_part, self.response_settings)

    def GetVariableName(self):
        return  self.variable

    def GetGradientsVariablesName(self):
        return self.gradients_variables

    def GetGradientVariableNameForType(self,control_type, raise_error=True):
        if raise_error:
            if not control_type in self.supported_control_types:
                raise RuntimeError("StrainEnergyResponseFunction: type {} in 'control_types' of response '{}' is not supported, supported types are {}  !".format(control_type,self.name,self.supported_control_types)) 

        return self.gradients_variables[control_type]

    def Initialize(self):
        super().Initialize()
        self.response_function_utility.Initialize()

    def CalculateValue(self):
        Logger.PrintInfo("StrainEnergyResponse:CalculateValue: Starting value calculation for response ", self.name)
        startTime = timer.time()
        self.value = self.response_function_utility.CalculateValue()
        Logger.PrintInfo("StrainEnergyResponse:CalculateValue: Time needed for calculating value ",round(timer.time() - startTime,2),"s")        
        return self.value

    def GetValue(self):
        self.value = self.response_function_utility.CalculateValue()
        return self.value        

    def CalculateGradients(self):
        Logger.PrintInfo("StrainEnergyResponse", "Starting gradient calculation for response ", self.name)
        startTime = timer.time()
        self.response_function_utility.CalculateGradient()
        Logger.PrintInfo("StrainEnergyResponse", "Time needed for calculating gradients ",round(timer.time() - startTime,2),"s")

    def CalculateGradientsForTypesAndObjects(self,control_types,controlled_objects,raise_error=True):

        if raise_error:
            for itr in range(len(controlled_objects)):
                controlled_object = controlled_objects[itr]
                controlled_object_index = self.controlled_model_parts.index(controlled_object)
                if not control_types[itr] == self.control_types[controlled_object_index]:
                    raise RuntimeError("StrainEnergyResponseFunction:CalculateGradientsForTypesAndObjects: control type {} of control object {} is not in the control_types of response {}".format(control_types[itr],controlled_object,self.name))

        Logger.PrintInfo("StrainEnergyResponse", "Starting shape gradient calculation of response ", self.name," for ",controlled_objects)
        startTime = timer.time()
        self.response_function_utility.CalculateGradient()
        # copy values from SHAPE_SENSITIVITY to D_STRAIN_ENERGY_D_X
        for controlle_object in controlled_objects:
            model_part = self.model.GetModelPart(controlle_object)           
            for node in model_part.Nodes:
                gradient = node.GetSolutionStepValue(KM.SHAPE_SENSITIVITY)
                node.SetSolutionStepValue(KOA.D_STRAIN_ENERGY_D_X, gradient)
        Logger.PrintInfo("StrainEnergyResponse", "Time needed for calculating gradients ",round(timer.time() - startTime,2),"s")  

    def GetGradients(self):
      
        tot_num_nodes = 0
        for design_model_part_name in self.design_model_parts:
            model_part = self.model.GetModelPart(design_model_part_name)
            tot_num_nodes += model_part.NumberOfNodes()        
        all_gradients = np.zeros(3*tot_num_nodes)

        index = 0
        for design_model_part_name in self.design_model_parts:
            model_part = self.model.GetModelPart(design_model_part_name)           
            for node in model_part.Nodes:
                all_gradients[index] = node.GetSolutionStepValue(KM.SHAPE_SENSITIVITY_X)
                all_gradients[index+1] = node.GetSolutionStepValue(KM.SHAPE_SENSITIVITY_Y)
                all_gradients[index+2] = node.GetSolutionStepValue(KM.SHAPE_SENSITIVITY_Z)
                index +=3

        return all_gradients