from KratosFluidDynamicsApplication import *

from KratosMultiphysics import _ImportApplication, python_registry_utilities

application = KratosFluidDynamicsApplication()
application_name = "KratosFluidDynamicsApplication"

_ImportApplication(application, application_name)

from . import python_registry_lists
python_registry_utilities.RegisterModelersList("KratosMultiphysics.FluidDynamicsApplication", python_registry_lists)
python_registry_utilities.RegisterOperationsList("KratosMultiphysics.FluidDynamicsApplication", python_registry_lists)
python_registry_utilities.RegisterProcessesList("KratosMultiphysics.FluidDynamicsApplication", python_registry_lists)
