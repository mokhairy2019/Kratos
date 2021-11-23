# Importing the Kratos Library
import KratosMultiphysics as KM

# Importing the base class
from KratosMultiphysics.CoSimulationApplication.base_classes.co_simulation_coupling_operation import CoSimulationCouplingOperation

# CoSimulation imports
import KratosMultiphysics.CoSimulationApplication.co_simulation_tools as cs_tools

def Create(*args):
    return ResetPfemKinematics(*args)

class ResetPfemKinematics(CoSimulationCouplingOperation):
    """This operation is used to reset the PFEM kinematic values on a given model part.
    It does not touch the SOLID/RIGID nodes
    (See the PfemFluidDynamicsApp - move_mesh_utility.cpp implementation)
    TODO:
    - add tests
    - more cleanup
    """
    def __init__(self, settings, solver_wrappers, process_info):
        super().__init__(settings, process_info)
        model = solver_wrappers[self.settings["solver"].GetString()].model
        model_part_name = self.settings["model_part_name"].GetString()
        self.model_part = model[model_part_name]

        self.interval = KM.IntervalUtility(settings)

    def InitializeCouplingIteration(self):
        current_time = self.model_part.ProcessInfo[KM.TIME]
        if(self.interval.IsInInterval(current_time)):
            self._ResetPfemKinematicValues()

            if(self.model_part.GetCommunicator().MyPID() == 0):
                if(self.echo_level > 0):
                    # print to screen the results at echo level 1 or higher
                    result_msg = " PFEM KINEMATICS RESET "
                    cs_tools.cs_print_info(self._ClassName(), result_msg)

    def _ResetPfemKinematicValues(self):
        KM.PfemFluidDynamicsApplication.MoveMeshUtility.ResetPfemKinematicValues(self.model_part)

    @classmethod
    def _GetDefaultParameters(cls):
        this_defaults = KM.Parameters("""{
            "solver"                : "UNSPECIFIED",
            "model_part_name"       : "",
            "interval"              : [0.0, 1e30]
        }""")
        this_defaults.AddMissingParameters(super()._GetDefaultParameters())
        return this_defaults
