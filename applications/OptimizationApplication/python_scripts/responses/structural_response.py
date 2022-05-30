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
    response_function: Cpp utilities object doing the actual computation of response value and gradient.
    """

    def __init__(self,response_name, response_settings,response_analysis,model):

        self.type = "strain_energy"
        super().__init__(response_name, response_settings, model, response_analysis)

        if not self.response_settings.Has("gradient_settings"):
            self.gradient_settings = KM.Parameters()
            self.gradient_settings.AddString("gradient_mode","semi_analytic")
            self.gradient_settings.AddDouble("step_size",1e-6)
        else:
            self.gradient_settings = self.response_settings["gradient_settings"]     

        if not self.analysis_model_part.HasNodalSolutionStepVariable(KM.KratosGlobals.GetVariable("D_STRAIN_ENERGY_1_D_X")):
            self.variable = "STRAIN_ENERGY_1"
        elif not self.analysis_model_part.HasNodalSolutionStepVariable(KM.KratosGlobals.GetVariable("D_STRAIN_ENERGY_2_D_X")):
            self.variable = "STRAIN_ENERGY_2"
        elif not self.analysis_model_part.HasNodalSolutionStepVariable(KM.KratosGlobals.GetVariable("D_STRAIN_ENERGY_3_D_X")):
            self.variable = "STRAIN_ENERGY_3"

        self.supported_control_types = ["shape","thickness","material"]
        self.gradients_variables = {"shape":"D_"+self.variable+"_D_X","thickness":"D_"+self.variable+"_D_PT","material":"D_"+self.variable+"_D_PE"}         

        if len(self.evaluated_model_parts) != 1:
            raise RuntimeError("StrainEnergyResponseFunction: 'evaluated_objects' of response '{}' must have only one entry !".format(self.name)) 

        for control_type in self.control_types:
            if not control_type in self.supported_control_types:
                raise RuntimeError("StrainEnergyResponseFunction: type {} in 'control_types' of response '{}' is not supported, supported types are {}  !".format(control_type,self.name,self.supported_control_types)) 
        
        # add vars
        for control_type in self.control_types:
            if control_type == "shape":
                self.response_settings["gradient_settings"].AddString("shape_gradient_field_name",self.gradients_variables[control_type])
                self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))
            if control_type == "thickness":
                self.response_settings["gradient_settings"].AddString("thickness_gradient_field_name",self.gradients_variables[control_type])
                self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))
            if control_type == "material":
                self.response_settings["gradient_settings"].AddString("material_gradient_field_name",self.gradients_variables[control_type])
                self.analysis_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))

        # create response
        self.response_function = KOA.LinearStrainEnergyOptResponse(response_name,model,self.response_settings)

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
        self.response_function.Initialize()

    def CalculateValue(self):
        Logger.PrintInfo("StrainEnergyResponse:CalculateValue: Starting value calculation for response ", self.name)
        startTime = timer.time()
        self.value = self.response_function.CalculateValue()
        Logger.PrintInfo("StrainEnergyResponse:CalculateValue: Time needed for calculating value ",round(timer.time() - startTime,2),"s")        
        return self.value

    def GetValue(self):
        self.value = self.response_function.CalculateValue()
        return self.value        

    def CalculateGradients(self):
        Logger.PrintInfo("StrainEnergyResponse", "Starting gradient calculation for response ", self.name)
        startTime = timer.time()
        self.response_function.CalculateGradient()
        Logger.PrintInfo("StrainEnergyResponse", "Time needed for calculating gradients ",round(timer.time() - startTime,2),"s")

    def CalculateGradientsForTypesAndObjects(self,control_types,controlled_objects,raise_error=True):

        if raise_error:
            for itr in range(len(controlled_objects)):
                controlled_object = controlled_objects[itr]
                control_type = control_types[itr]
                found = False
                for itr_2 in range(len(self.controlled_model_parts)):
                    controlled_model_part = self.controlled_model_parts[itr_2]
                    controlled_type = self.control_types[itr_2]
                    if controlled_type==control_type and controlled_model_part==controlled_object:
                        found = True
                        break
                if not found:
                    raise RuntimeError("StrainEnergyResponseFunction:CalculateGradientsForTypesAndObjects: control type {} of control object {} is not in the control_types of response {}".format(control_types[itr],controlled_object,self.name))

        Logger.PrintInfo("StrainEnergyResponse", "Starting ", control_types," gradients calculation of response ", self.name," for ",controlled_objects)        
        startTime = timer.time()
        self.response_function.CalculateGradient()                                
        Logger.PrintInfo("StrainEnergyResponse", "Time needed for calculating gradients ",round(timer.time() - startTime,2),"s")  


class MassResponseFunction(BaseResponseFunction):

    def __init__(self,response_name, response_settings,model):

        self.type = "mass"
        self.variable = "MASS"
        super().__init__(response_name, response_settings, model)

        if not self.response_settings.Has("gradient_settings"):
            self.gradient_settings = KM.Parameters()
            self.gradient_settings.AddString("gradient_mode","semi_analytic")
            self.gradient_settings.AddDouble("step_size",1e-6)
        else:
            self.gradient_settings = self.response_settings["gradient_settings"]     

        self.supported_control_types = ["shape","thickness","material"]
        self.gradients_variables = {"shape":"D_MASS_D_X","thickness":"D_MASS_D_PT","material":"D_MASS_D_PD"}

        if len(self.evaluated_model_parts) != 1:
            raise RuntimeError("MassResponseFunction: 'evaluated_objects' of response '{}' must have only one entry !".format(self.name)) 

        for control_type in self.control_types:
            if not control_type in self.supported_control_types:
                raise RuntimeError("MassResponseFunction: type {} in 'control_types' of response '{}' is not supported, supported types are {}  !".format(control_type,self.name,self.supported_control_types)) 

        
        root_model_part_name = self.evaluated_model_parts[0].split(".")[0]
        for evaluated_model_part in self.evaluated_model_parts:
            if evaluated_model_part.split(".")[0] != root_model_part_name:
                raise RuntimeError("MassResponseFunction: evaluated_model_parts of mass response must have the same root model part !")

        self.root_model_part = self.model.GetModelPart(root_model_part_name)

        # add vars and response
        self.response_function = KOA.MassOptResponse(response_name,model,self.response_settings)
        for control_type in self.control_types:
            self.root_model_part.AddNodalSolutionStepVariable(KM.KratosGlobals.GetVariable(self.gradients_variables[control_type]))

        
    def GetVariableName(self):
        return  self.variable

    def GetGradientsVariablesName(self):
        return self.gradients_variables

    def GetGradientVariableNameForType(self,control_type, raise_error=True):
        if raise_error:
            if not control_type in self.supported_control_types:
                raise RuntimeError("MassResponseFunction: type {} in 'control_types' of response '{}' is not supported, supported types are {}  !".format(control_type,self.name,self.supported_control_types)) 

        return self.gradients_variables[control_type]

    def Initialize(self):
        super().Initialize()
        self.response_function.Initialize()

    def CalculateValue(self):
        Logger.PrintInfo("MassResponseFunction:CalculateValue: Starting value calculation for response ", self.name)
        startTime = timer.time()
        self.value = self.response_function.CalculateValue()
        Logger.PrintInfo("MassResponseFunction:CalculateValue: Time needed for calculating value ",round(timer.time() - startTime,2),"s")        
        return self.value

    def CalculateGradientsForTypesAndObjects(self,control_types,controlled_objects,raise_error=True):

        if raise_error:
            for itr in range(len(controlled_objects)):
                controlled_object = controlled_objects[itr]
                control_type = control_types[itr]
                found = False
                for itr_2 in range(len(self.controlled_model_parts)):
                    controlled_model_part = self.controlled_model_parts[itr_2]
                    controlled_type = self.control_types[itr_2]
                    if controlled_type==control_type and controlled_model_part==controlled_object:
                        found = True
                        break
                if not found:
                    raise RuntimeError("MassResponseFunction:CalculateGradientsForTypesAndObjects: control type {} of control object {} is not in the control_types of response {}".format(control_types[itr],controlled_object,self.name))

        Logger.PrintInfo("MassResponseFunction", "Starting ", control_types," gradients calculation of response ", self.name," for ",controlled_objects)
        startTime = timer.time()
        self.response_function.CalculateGradient()               
        Logger.PrintInfo("MassResponseFunction", "Time needed for calculating gradients ",round(timer.time() - startTime,2),"s")  
       