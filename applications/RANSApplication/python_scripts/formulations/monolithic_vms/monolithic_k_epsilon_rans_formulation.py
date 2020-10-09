# import kratos
import KratosMultiphysics as Kratos

# import formulation interface
from KratosMultiphysics.RANSApplication.formulations.rans_formulation import RansFormulation

# import formulations
from ..incompressible_potential_flow import IncompressiblePotentialFlowRansFormulation
from .monolithic_velocity_pressure_rans_formulation import MonolithicVelocityPressureRansFormulation
from ..turbulence_models.k_epsilon import KEpsilonRansFormulation

class MonolithicKEpsilonRansFormulation(RansFormulation):
    def __init__(self, model_part, settings):
        super().__init__(model_part, settings)

        default_settings = Kratos.Parameters(r'''
        {
            "formulation_name": "monolithic_k_epsilon",
            "incompressible_potential_flow_initialization_settings": {},
            "monolithic_flow_solver_settings": {},
            "k_epsilon_solver_settings": {},
            "max_iterations": 1
        }''')
        settings = self.GetParameters()
        settings.ValidateAndAssignDefaults(default_settings)

        if (not settings["incompressible_potential_flow_initialization_settings"].IsEquivalentTo(
                Kratos.Parameters("{}"))):
            self.incompressible_potential_flow_formulation = IncompressiblePotentialFlowRansFormulation(model_part, settings["incompressible_potential_flow_initialization_settings"])
            self.AddFormulation(self.incompressible_potential_flow_formulation)

        self.monolithic_formulation = MonolithicVelocityPressureRansFormulation(model_part, settings["monolithic_flow_solver_settings"])
        self.AddRansFormulation(self.monolithic_formulation)

        self.k_epsilon_formulation = KEpsilonRansFormulation(model_part, settings["k_epsilon_solver_settings"])
        self.AddRansFormulation(self.k_epsilon_formulation)

        self.SetMaxCouplingIterations(settings["max_iterations"].GetInt())

    def SetConstants(self, settings):
        self.k_epsilon_formulation.SetConstants(settings)
