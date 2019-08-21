from __future__ import print_function, absolute_import, division #makes KratosMultiphysics backward compatible with python 2.6 and 2.7
# Importing the Kratos Library
import KratosMultiphysics as KratosMultiphysics
import KratosMultiphysics.MeshingApplication as MeshingApplication

try:
    import KratosMultiphysics.StructuralMechanicsApplication as StructuralMechanicsApplication
    structural_dependencies = True
except ImportError as e:
    structural_dependencies = False

from json_utilities import *
import json
import os

def Factory(settings, Model):
    if not isinstance(settings, KratosMultiphysics.Parameters):
        raise Exception("expected input shall be a Parameters object, encapsulating a json string")
    return MmgProcess(Model, settings["Parameters"])

class MmgProcess(KratosMultiphysics.Process):
    """This process remeshes using MMG library. This process uses different utilities and processes

    Only the member variables listed below should be accessed directly.

    Public member variables:
    Model -- the container of the different model parts.
    settings -- Kratos parameters containing solver settings.
    """

    def __init__(self, Model, settings ):
        """ The default constructor of the class

        Keyword arguments:
        self -- It signifies an instance of a class.
        Model -- the container of the different model parts.
        settings -- Kratos parameters containing solver settings.
        """

        KratosMultiphysics.Process.__init__(self)

        ## Settings string in json format
        default_parameters = KratosMultiphysics.Parameters("""
        {
            "help"                             : "This process remeshes using MMG library. This process uses different utilities and processes",
            "mesh_id"                          : 0,
            "filename"                         : "out",
            "model_part_name"                  : "PLEASE_SPECIFY_MODEL_PART_NAME",
            "blocking_threshold_size"          : false,
            "threshold_sizes" : {
                "minimal_size"                     : 0.1,
                "maximal_size"                     : 10.0
            },
            "strategy"                         : "LevelSet",
            "level_set_strategy_parameters"              :{
                "scalar_variable"                  : "DISTANCE",
                "gradient_variable"                : "DISTANCE_GRADIENT"
            },
            "error_strategy_parameters"              :{
                "compute_error_extra_parameters":
                {
                    "stress_vector_variable"              : "CAUCHY_STRESS_VECTOR"
                },
                "error_metric_parameters"                 :
                {
                    "error_threshold"                       : 0.05,
                    "interpolation_error"                   : 0.04
                },
                "set_target_number_of_elements"       : false,
                "target_number_of_elements"           : 1000,
                "perform_nodal_h_averaging"           : false,
                "max_iterations"                      : 3
            },
            "discretization_type"                  : "Standard",
            "isosurface_parameters"                :
            {
                "isosurface_variable"              : "DISTANCE",
                "nonhistorical_variable"           : false,
                "remove_internal_regions"          : false
            },
            "framework"                            : "Eulerian",
            "internal_variables_parameters"        :
            {
                "allocation_size"                      : 1000,
                "bucket_size"                          : 4,
                "search_factor"                        : 2,
                "interpolation_type"                   : "LST",
                "internal_variable_interpolation_list" :[]
            },
            "hessian_strategy_parameters"              :{
                "metric_variable"                  : ["DISTANCE"],
                "non_historical_metric_variable"   : [false],
                "estimate_interpolation_error"     : false,
                "interpolation_error"              : 0.04,
                "mesh_dependent_constant"          : 0.28125
            },
            "enforce_current"                  : true,
            "initial_step"                     : 1,
            "step_frequency"                   : 0,
            "automatic_remesh"                 : true,
            "automatic_remesh_parameters"      :{
                "automatic_remesh_type"            : "Ratio",
                "min_size_ratio"                   : 1.0,
                "max_size_ratio"                   : 3.0,
                "refer_type"                       : "Mean",
                "min_size_current_percentage"      : 50.0,
                "max_size_current_percentage"      : 98.0
            },
            "initial_remeshing"                : false,
            "fix_contour_model_parts"          : [],
            "fix_conditions_model_parts"       : [],
            "fix_elements_model_parts"         : [],
            "force_min"                        : false,
            "minimal_size"                     : 0.1,
            "force_max"                        : false,
            "maximal_size"                     : 10.0,
            "sizing_parameters":
            {
                "reference_variable_name"          : "DISTANCE",
                "boundary_layer_max_distance"      : 1.0,
                "interpolation"                    : "constant"
            },
            "advanced_parameters"                  :
            {
                "force_hausdorff_value"               : false,
                "hausdorff_value"                     : 0.0001,
                "no_move_mesh"                        : false,
                "no_surf_mesh"                        : false,
                "no_insert_mesh"                      : false,
                "no_swap_mesh"                        : false,
                "deactivate_detect_angle"             : false,
                "force_gradation_value"               : false,
                "gradation_value"                     : 1.3
            },
            "anisotropy_remeshing"                 : true,
            "enforce_anisotropy_relative_variable" : false,
            "anisotropy_parameters":{
                "reference_variable_name"          : "DISTANCE",
                "hmin_over_hmax_anisotropic_ratio" : 0.01,
                "boundary_layer_max_distance"      : 1.0,
                "boundary_layer_min_size_ratio"    : 2.0,
                "interpolation"                    : "Linear"
            },
            "collapse_prisms_elements"         : false,
            "save_external_files"              : false,
            "save_colors_files"                : false,
            "save_mdpa_file"                   : false,
            "max_number_of_searchs"            : 1000,
            "preserve_flags"                   : true,
            "interpolate_non_historical"       : true,
            "extrapolate_contour_values"       : true,
            "surface_elements"                 : false,
            "search_parameters"                : {
                "allocation_size"                  : 1000,
                "bucket_size"                      : 4,
                "search_factor"                    : 2.0
            },
            "debug_mode"                       : "",
            "debug_result_mesh"                : false,
            "initialize_entities"              : true,
            "echo_level"                       : 3
        }
        """)

        # Identify the dimension first
        if not settings.Has("model_part_name"):
            settings.AddValue("model_part_name", default_parameters["model_part_name"])

        # Getting model part and working dimension
        self.main_model_part = Model[settings["model_part_name"].GetString()]
        self.domain_size = self.main_model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE]
        self.is_surface = False
        if self.domain_size == 3:
            for elem in self.main_model_part.Elements:
                geom = elem.GetGeometry()
                if geom.WorkingSpaceDimension() != geom.LocalSpaceDimension():
                    self.is_surface = True
                break

        # The mesh dependent constant depends on dimension
        if self.domain_size == 2:
            default_parameters["hessian_strategy_parameters"]["mesh_dependent_constant"].SetDouble(2.0/9.0)
        else:
            default_parameters["hessian_strategy_parameters"]["mesh_dependent_constant"].SetDouble(9.0/32.0)

        # Overwrite the default settings with user-provided parameters
        self.settings = settings
        self.settings.RecursivelyValidateAndAssignDefaults(default_parameters)

        # Getting some settings
        self.enforce_current = self.settings["enforce_current"].GetBool()
        self.initial_remeshing = self.settings["initial_remeshing"].GetBool()
        self.initial_step = self.settings["initial_step"].GetInt()
        self.step_frequency = self.settings["step_frequency"].GetInt()
        self.settings["surface_elements"].SetBool(self.is_surface)

        # Setting initial_step_done here
        self.initial_step_done = False

    def ExecuteInitialize(self):
        """ This method is executed at the begining to initialize the process

        Keyword arguments:
        self -- It signifies an instance of a class.
        """

        # Calculate NODAL_H
        KratosMultiphysics.VariableUtils().SetNonHistoricalVariable(KratosMultiphysics.NODAL_H, 0.0, self.main_model_part.Nodes)
        KratosMultiphysics.VariableUtils().SetNonHistoricalVariable(KratosMultiphysics.NODAL_AREA, 0.0, self.main_model_part.Nodes)
        self.find_nodal_h = KratosMultiphysics.FindNodalHNonHistoricalProcess(self.main_model_part)
        self.find_nodal_h.Execute()

        # Calculate the parameters of automatic remeshing
        if self.settings["automatic_remesh"].GetBool():
            import statistics as stat
            nodal_h_values = []
            for node in self.main_model_part.Nodes:
                nodal_h_values.append(node.GetValue(KratosMultiphysics.NODAL_H))

            # Calculate the minimum size
            if self.settings["automatic_remesh_parameters"]["automatic_remesh_type"].GetString() == "Ratio":
                # NOTE: For mode: https://docs.python.org/3/library/statistics.html
                if self.settings["automatic_remesh_parameters"]["refer_type"].GetString() == "Mean":
                    ref = stat.mean(nodal_h_values)
                elif self.settings["automatic_remesh_parameters"]["refer_type"].GetString() == "Median":
                    ref = stat.median(nodal_h_values)

                self.settings["minimal_size"].SetDouble(ref * (self.settings["automatic_remesh_parameters"]["min_size_ratio"].GetDouble()))
                self.settings["maximal_size"].SetDouble(ref * (self.settings["automatic_remesh_parameters"]["max_size_ratio"].GetDouble()))
            elif self.settings["automatic_remesh_parameters"]["automatic_remesh_type"].GetString() == "Percentage":
                mean = stat.mean(nodal_h_values)
                stdev = stat.stdev(nodal_h_values)
                prob = (self.settings["automatic_remesh_parameters"]["min_size_current_percentage"].GetDouble())/100
                self.settings["minimal_size"].SetDouble(_normvalf(prob, mean, stdev)) # Using normal normal distribution to get the minimal size as a stadistical meaninful value

                prob = (self.settings["automatic_remesh_parameters"]["max_size_current_percentage"].GetDouble())/100
                self.settings["maximal_size"].SetDouble(_normvalf(prob, mean, stdev)) # Using normal normal distribution to get the maximal size as a stadistical meaninful value

        # Anisotropic remeshing parameters
        self.anisotropy_remeshing = self.settings["anisotropy_remeshing"].GetBool()
        if self.anisotropy_remeshing:
            if self.settings["automatic_remesh"].GetBool():
                self.settings["anisotropy_parameters"]["boundary_layer_max_distance"].SetDouble(self.settings["minimal_size"].GetDouble() * self.settings["anisotropy_parameters"]["boundary_layer_min_size_ratio"].GetDouble())

        # Select the remeshing strategy
        self.strategy = self.settings["strategy"].GetString()
        if self.strategy == "LevelSet":
            self.scalar_variable = KratosMultiphysics.KratosGlobals.GetVariable( self.settings["level_set_strategy_parameters"]["scalar_variable"].GetString() )
            self.gradient_variable = KratosMultiphysics.KratosGlobals.GetVariable( self.settings["level_set_strategy_parameters"]["gradient_variable"].GetString() )
        elif self.strategy == "Hessian":
            self.metric_variables = self.__generate_variable_list_from_input(self.settings["hessian_strategy_parameters"]["metric_variable"])
            self.non_historical_metric_variable = self.__generate_boolean_list_from_input(self.settings["hessian_strategy_parameters"]["non_historical_metric_variable"])
            len_metric_variables = len(self.metric_variables)
            len_non_historical_metric_variable = len(self.non_historical_metric_variable)
            if len_metric_variables > len_non_historical_metric_variable:
                for i in range(len_non_historical_metric_variable, len_metric_variables):
                    self.non_historical_metric_variable.append(False)
            mesh_dependent_constant = self.settings["hessian_strategy_parameters"]["mesh_dependent_constant"].GetDouble()
            if mesh_dependent_constant == 0.0:
                self.settings["hessian_strategy_parameters"]["mesh_dependent_constant"].SetDouble(0.5 * (self.domain_size/(self.domain_size + 1))**2.0)
        elif self.strategy == "superconvergent_patch_recovery":
            self.error_threshold = self.settings["error_strategy_parameters"]["error_metric_parameters"]["error_threshold"].GetDouble()
            self.estimated_error = 0
            self.remeshing_cycle = 0
            self.main_model_part.ProcessInfo[MeshingApplication.EXECUTE_REMESHING] = True

        self.internal_variable_interpolation_list = self.__generate_internal_variable_list_from_input(self.settings["internal_variables_parameters"]["internal_variable_interpolation_list"])

        # Model parts to fix the nodes
        fix_contour_model_parts = self.__generate_submodelparts_list_from_input(self.settings["fix_contour_model_parts"])

        # Setting flag BLOCKED to the non nodes
        for submodelpart in fix_contour_model_parts:
            KratosMultiphysics.VariableUtils().SetFlag(KratosMultiphysics.BLOCKED, True, submodelpart.Nodes)

        # Model parts to fix the conditions
        fix_conditions_model_parts = self.__generate_submodelparts_list_from_input(self.settings["fix_conditions_model_parts"])

        # Setting flag BLOCKED to the non conditions
        for submodelpart in fix_conditions_model_parts:
            KratosMultiphysics.VariableUtils().SetFlag(KratosMultiphysics.BLOCKED, True, submodelpart.Conditions)

        # Model parts to fix the nodes
        fix_elements_model_parts = self.__generate_submodelparts_list_from_input(self.settings["fix_elements_model_parts"])

        # Setting flag BLOCKED to the non elements
        for submodelpart in fix_elements_model_parts:
            KratosMultiphysics.VariableUtils().SetFlag(KratosMultiphysics.BLOCKED, True, submodelpart.Elements)

        if self.strategy == "LevelSet":
            self._CreateGradientProcess()

        if self.domain_size == 2:
            self.initialize_metric = MeshingApplication.MetricFastInit2D(self.main_model_part)
        else:
            self.initialize_metric = MeshingApplication.MetricFastInit3D(self.main_model_part)

        self.initialize_metric.Execute()

        self._CreateMetricsProcess()

        mmg_parameters = KratosMultiphysics.Parameters("""{"force_sizes":{}}""")
        mmg_parameters.AddValue("filename",self.settings["filename"])
        mmg_parameters.AddValue("framework",self.settings["framework"])
        mmg_parameters.AddValue("discretization_type",self.settings["discretization_type"])
        mmg_parameters.AddValue("isosurface_parameters",self.settings["isosurface_parameters"])
        mmg_parameters.AddValue("internal_variables_parameters",self.settings["internal_variables_parameters"])
        mmg_parameters.AddValue("collapse_prisms_elements",self.settings["collapse_prisms_elements"])
        mmg_parameters.AddValue("save_external_files",self.settings["save_external_files"])
        mmg_parameters.AddValue("save_colors_files",self.settings["save_colors_files"])
        mmg_parameters.AddValue("save_mdpa_file",self.settings["save_mdpa_file"])
        mmg_parameters.AddValue("max_number_of_searchs",self.settings["max_number_of_searchs"])
        mmg_parameters.AddValue("preserve_flags",self.settings["preserve_flags"])
        mmg_parameters.AddValue("interpolate_non_historical",self.settings["interpolate_non_historical"])
        mmg_parameters.AddValue("extrapolate_contour_values",self.settings["extrapolate_contour_values"])
        mmg_parameters.AddValue("search_parameters",self.settings["search_parameters"])
        mmg_parameters["force_sizes"].AddValue("force_min",self.settings["force_min"])
        mmg_parameters["force_sizes"].AddValue("minimal_size",self.settings["minimal_size"])
        mmg_parameters["force_sizes"].AddValue("force_max",self.settings["force_max"])
        mmg_parameters["force_sizes"].AddValue("maximal_size",self.settings["maximal_size"])
        mmg_parameters.AddValue("advanced_parameters",self.settings["advanced_parameters"])
        mmg_parameters.AddValue("debug_result_mesh",self.settings["debug_result_mesh"])
        mmg_parameters.AddValue("initialize_entities",self.settings["initialize_entities"])
        mmg_parameters.AddValue("echo_level",self.settings["echo_level"])
        if self.domain_size == 2:
            self.mmg_process = MeshingApplication.MmgProcess2D(self.main_model_part, mmg_parameters)
        else:
            # Differentiate between 3D volumes and 3D surfaces
            if self.is_surface:
                self.mmg_process = MeshingApplication.MmgProcess3DSurfaces(self.main_model_part, mmg_parameters)
            else:
                self.mmg_process = MeshingApplication.MmgProcess3D(self.main_model_part, mmg_parameters)

        # We reset the step
        self.step = 0

        # We compute initial remeshing is desired
        if self.initial_remeshing:
            if not self.main_model_part.Is(KratosMultiphysics.MODIFIED):
                self._ExecuteRefinement()
            else:
                self.main_model_part.Set(KratosMultiphysics.MODIFIED, False)

    def ExecuteInitializeSolutionStep(self):
        """ This method is executed in order to initialize the current step

        Keyword arguments:
        self -- It signifies an instance of a class.
        """

        if not self.initial_remeshing:
            execute_remesh = False
            # We need to check if the model part has been modified recently
            if self.main_model_part.Is(KratosMultiphysics.MODIFIED):
                self.main_model_part.Set(KratosMultiphysics.MODIFIED, False)
                self.step = 0  # Reset (just to be sure)
            else:
                self.step += 1
                if self.step_frequency > 0:
                    if self.main_model_part.ProcessInfo[KratosMultiphysics.STEP] >= self.initial_step:
                        if not self.initial_step_done:
                                execute_remesh = True
                        else:
                            if self.step >= self.step_frequency:
                                execute_remesh = True
                    # We remesh if needed
                    if execute_remesh:
                        if self.settings["blocking_threshold_size"].GetBool():
                            MeshingApplication.BlockThresholdSizeElements(self.main_model_part, self.settings["threshold_sizes"])
                        self._ExecuteRefinement()
                        self.initial_step_done = True
                        self.step = 0  # Reset

    def ExecuteFinalizeSolutionStep(self):
        """ This method is executed in order to finalize the current step

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        if self.strategy == "superconvergent_patch_recovery":
            self._ErrorCalculation()

    def ExecuteAfterOutputStep(self):
        """ This method is executed right after the ouput process computation

        Keyword arguments:
        self -- It signifies an instance of a class.
        """
        if self.strategy == "superconvergent_patch_recovery":
            if self.main_model_part.ProcessInfo[MeshingApplication.ERROR_ESTIMATE] > self.error_threshold:
                self.__execute_refinement()
            self.remeshing_cycle += 1
            if self.main_model_part.ProcessInfo[MeshingApplication.ERROR_ESTIMATE] <= self.error_threshold or self.remeshing_cycle > self.params["max_iterations"].GetInt():
                self.main_model_part.ProcessInfo[MeshingApplication.EXECUTE_REMESHING] = False

    def _CreateMetricsProcess(self):
        self.metric_processes = []
        if self.strategy == "LevelSet":
            level_set_parameters = KratosMultiphysics.Parameters("""{}""")
            level_set_parameters.AddValue("minimal_size",self.settings["minimal_size"])
            level_set_parameters.AddValue("maximal_size",self.settings["maximal_size"])
            level_set_parameters.AddValue("sizing_parameters",self.settings["sizing_parameters"])
            level_set_parameters.AddValue("enforce_current",self.settings["enforce_current"])
            level_set_parameters.AddValue("anisotropy_remeshing",self.settings["anisotropy_remeshing"])
            level_set_parameters.AddValue("anisotropy_parameters",self.settings["anisotropy_parameters"])
            level_set_parameters["anisotropy_parameters"].RemoveValue("boundary_layer_min_size_ratio")
            if self.domain_size == 2:
                self.metric_processes.append(MeshingApplication.ComputeLevelSetSolMetricProcess2D(self.main_model_part, self.gradient_variable, level_set_parameters))
            else:
                self.metric_processes.append(MeshingApplication.ComputeLevelSetSolMetricProcess3D(self.main_model_part, self.gradient_variable, level_set_parameters))

        elif self.strategy == "Hessian":
            hessian_parameters = KratosMultiphysics.Parameters("""{}""")
            hessian_parameters.AddValue("minimal_size",self.settings["minimal_size"])
            hessian_parameters.AddValue("maximal_size",self.settings["maximal_size"])
            hessian_parameters.AddValue("enforce_current",self.settings["enforce_current"])
            hessian_parameters.AddValue("hessian_strategy_parameters",self.settings["hessian_strategy_parameters"])
            hessian_parameters["hessian_strategy_parameters"].RemoveValue("metric_variable")
            hessian_parameters["hessian_strategy_parameters"].RemoveValue("non_historical_metric_variable")
            hessian_parameters["hessian_strategy_parameters"].AddEmptyValue("non_historical_metric_variable")
            hessian_parameters.AddValue("anisotropy_remeshing",self.settings["anisotropy_remeshing"])
            hessian_parameters.AddValue("enforce_anisotropy_relative_variable",self.settings["enforce_anisotropy_relative_variable"])
            hessian_parameters.AddValue("enforced_anisotropy_parameters",self.settings["anisotropy_parameters"])
            hessian_parameters["enforced_anisotropy_parameters"].RemoveValue("boundary_layer_min_size_ratio")
            for current_metric_variable, non_historical_metric_variable in zip(self.metric_variables, self.non_historical_metric_variable):
                hessian_parameters["hessian_strategy_parameters"]["non_historical_metric_variable"].SetBool(non_historical_metric_variable)
                self.metric_processes.append(MeshingApplication.ComputeHessianSolMetricProcess(self.main_model_part, current_metric_variable, hessian_parameters))
        elif self.strategy == "superconvergent_patch_recovery":
            if not structural_dependencies:
                raise Exception("You need to compile the StructuralMechanicsApplication in order to use this criteria")

            # We compute the error
            error_compute_parameters = KratosMultiphysics.Parameters("""{}""")
            error_compute_parameters.AddValue("stress_vector_variable", self.settings["compute_error_extra_parameters"]["stress_vector_variable"])
            error_compute_parameters.AddValue("echo_level", self.settings["echo_level"])
            if self.domain_size == 2:
                self.error_compute = StructuralMechanicsApplication.SPRErrorProcess2D(self.main_model_part, error_compute_parameters)
            else:
                self.error_compute = StructuralMechanicsApplication.SPRErrorProcess3D(self.main_model_part, error_compute_parameters)

            # Now we compute the metric
            error_metric_parameters = KratosMultiphysics.Parameters("""{}""")
            error_metric_parameters.AddValue("minimal_size",self.settings["minimal_size"])
            error_metric_parameters.AddValue("maximal_size",self.settings["maximal_size"])
            error_metric_parameters.AddValue("target_error",self.settings["error_strategy_parameters"]["error_metric_parameters"]["interpolation_error"])
            error_metric_parameters.AddValue("set_target_number_of_elements", self.settings["error_strategy_parameters"]["set_target_number_of_elements"])
            error_metric_parameters.AddValue("target_number_of_elements", self.settings["error_strategy_parameters"]["target_number_of_elements"])
            error_metric_parameters.AddValue("perform_nodal_h_averaging", self.settings["error_strategy_parameters"]["perform_nodal_h_averaging"])
            error_metric_parameters.AddValue("echo_level", self.settings["echo_level"])

            if self.domain_size == 2:
                self.metric_process = MeshingApplication.MetricErrorProcess2D(self.main_model_part, error_metric_parameters)
            else:
                self.metric_process = MeshingApplication.MetricErrorProcess3D(self.main_model_part, error_metric_parameters)

    def _CreateGradientProcess(self):
        # We compute the scalar value gradient
        if self.domain_size == 2:
            self.local_gradient = KratosMultiphysics.ComputeNodalGradientProcess2D(self.main_model_part, self.scalar_variable, self.gradient_variable, KratosMultiphysics.NODAL_AREA)
        else:
            self.local_gradient = KratosMultiphysics.ComputeNodalGradientProcess3D(self.main_model_part, self.scalar_variable, self.gradient_variable, KratosMultiphysics.NODAL_AREA)

    def _ExecuteRefinement(self):
        if self.strategy == "LevelSet":
            # Calculate the gradient
            self.local_gradient.Execute()

        # Recalculate NODAL_H
        self.find_nodal_h.Execute()

        # Initialize metric
        self.initialize_metric.Execute()

        KratosMultiphysics.Logger.PrintInfo("MMG Remeshing Process", "Calculating the metrics")
        # Execute metric computation
        for metric_process in self.metric_processes:
            metric_process.Execute()

        # Debug before remesh
        if self.settings["debug_mode"].GetString() == "GiD": # GiD
            self._debug_output_gid(self.step, "", "BEFORE_")
        elif self.settings["debug_mode"].GetString() == "VTK": # VTK
            self._debug_output_vtk(self.step, "", "BEFORE_")

        KratosMultiphysics.Logger.PrintInfo("MMG Remeshing Process", "Remeshing")
        self.mmg_process.Execute()

        # Debug after remesh
        if self.settings["debug_mode"].GetString() == "GiD": # GiD
            self._debug_output_gid(self.step, "", "AFTER_")
        elif self.settings["debug_mode"].GetString() == "VTK": # VTK
            self._debug_output_vtk(self.step, "", "AFTER_")

        if self.strategy == "LevelSet":
            self.local_gradient.Execute() # Recalculate gradient after remeshing

        # Recalculate NODAL_H
        self.find_nodal_h.Execute()

        # We need to set that the model part has been modified (later on we will act in consequence)
        self.main_model_part.Set(KratosMultiphysics.MODIFIED, True)

        KratosMultiphysics.Logger.PrintInfo("MMG Remeshing Process", "Remesh finished")

    def _ErrorCalculation(self):

        # Initialize metric
        self.initialize_metric.Execute()

        KratosMultiphysics.Logger.PrintInfo("MMG Remeshing Process", "Calculating the metrics")
        # Execute error computation
        self.error_compute.Execute()
        # Execute metric computation
        self.metric_process.Execute()
        self.estimated_error = self.main_model_part.ProcessInfo[MeshingApplication.ERROR_ESTIMATE]

    def __generate_boolean_list_from_input(self,param):
      '''Parse a list of booleans from input.'''
      # At least verify that the input is an array
      if not param.IsArray():
          raise Exception("{0} Error: Variable list is unreadable".format(self.__class__.__name__))

      # Retrieve the boolean from the arrays
      boolean_list = []

      for i in range( 0,param.size()):
          boolean_list.append(param[i].GetBool())

      return boolean_list

    def __generate_submodelparts_list_from_input(self,param):
        '''Parse a list of variables from input.'''
        # At least verify that the input is a string
        if not param.IsArray():
            raise Exception("{0} Error: Variable list is unreadable".format(self.__class__.__name__))

        # Retrieve submodelparts name from input (a string) and request the corresponding C++ object to the kernel
        return [self.main_model_part.GetSubModelPart(param[i].GetString()) for i in range(0, param.size())]

    def __generate_variable_list_from_input(self,param):
      '''Parse a list of variables from input.'''
      # At least verify that the input is a string
      if not param.IsArray():
          raise Exception("{0} Error: Variable list is unreadable".format(self.__class__.__name__))

      # Retrieve variable name from input (a string) and request the corresponding C++ object to the kernel
      variable_list = []
      if len(self.main_model_part.Nodes) > 0:
          node = (self.main_model_part.Nodes)[1]
          for i in range( 0,param.size()):
              aux_var = KratosMultiphysics.KratosGlobals.GetVariable( param[i].GetString() )
              val = node.GetSolutionStepValue(aux_var, 0)
              if isinstance(val,float):
                  variable_list.append(aux_var)
              else:
                  variable_list.append( KratosMultiphysics.KratosGlobals.GetVariable( param[i].GetString()+"_X" ))
                  variable_list.append( KratosMultiphysics.KratosGlobals.GetVariable( param[i].GetString()+"_Y" ))
                  if self.domain_size == 3:
                      variable_list.append( KratosMultiphysics.KratosGlobals.GetVariable( param[i].GetString()+"_Z" ))

      return variable_list

    def __generate_internal_variable_list_from_input(self,param):
      '''Parse a list of variables from input.'''
      # At least verify that the input is an array
      if not param.IsArray():
          raise Exception("{0} Error: Variable list is unreadable".format(self.__class__.__name__))

      # Retrieve variable name from input (a string) and request the corresponding C++ object to the kernel
      variable_list = []

      for i in range( 0,param.size()):
          aux_var = KratosMultiphysics.KratosGlobals.GetVariable( param[i].GetString() )
          variable_list.append(aux_var)

      return variable_list

    def _debug_output_gid(self, label, name, prefix):
        '''Debug postprocess with GiD.'''
        gid_mode = KratosMultiphysics.GiDPostMode.GiD_PostBinary
        singlefile = KratosMultiphysics.MultiFileFlag.SingleFile
        deformed_mesh_flag = KratosMultiphysics.WriteDeformedMeshFlag.WriteUndeformed
        write_conditions = KratosMultiphysics.WriteConditionsFlag.WriteConditions
        gid_io = KratosMultiphysics.GidIO(prefix + "REMESHING_" + name + "_STEP_" + str(label), gid_mode, singlefile, deformed_mesh_flag, write_conditions)

        gid_io.InitializeMesh(label)
        gid_io.WriteMesh(self.main_model_part.GetMesh())
        gid_io.FinalizeMesh()
        gid_io.InitializeResults(label, self.main_model_part.GetMesh())
        if self.settings["framework"].GetString() ==  "Lagrangian":
            gid_io.WriteNodalResults(KratosMultiphysics.DISPLACEMENT, self.main_model_part.Nodes, label, 0)
            for var in self.internal_variable_interpolation_list:
                gid_io.PrintOnGaussPoints(var, self.main_model_part, label)
        else:
            gid_io.WriteNodalResults(KratosMultiphysics.VELOCITY, self.main_model_part.Nodes, label, 0)
        gid_io.FinalizeResults()

        #raise NameError("DEBUG")

    def _debug_output_vtk(self, label, name, prefix):
        '''Debug postprocess with VTK.'''
        vtk_settings = KratosMultiphysics.Parameters("""{
            "model_part_name"                    : "PLEASE_SPECIFY_MODEL_PART_NAME",
            "file_format"                        : "ascii",
            "output_precision"                   : 7,
            "output_control_type"                : "step",
            "output_frequency"                   : 1.0,
            "output_sub_model_parts"             : false,
            "custom_name_prefix"                 : "",
            "save_output_files_in_folder"        : false,
            "nodal_solution_step_data_variables" : [],
            "nodal_data_value_variables"         : [],
            "element_data_value_variables"       : [],
            "condition_data_value_variables"     : [],
            "gauss_point_variables"              : []
        }""")

        vtk_settings["custom_name_prefix"].SetString(prefix + "REMESHING_" + name + "_STEP_" + str(label) + "_")
        if self.settings["framework"].GetString() ==  "Lagrangian":
            vtk_settings["nodal_solution_step_data_variables"].Append("DISPLACEMENT")
            for var in self.internal_variable_interpolation_list:
                vtk_settings["gauss_point_variables"].Append(var.Name())
        else:
            vtk_settings["nodal_solution_step_data_variables"].Append("VELOCITY")

        vtk_io = KratosMultiphysics.VtkOutput(self.main_model_part, vtk_settings)
        vtk_io.PrintOutput()

        #raise NameError("DEBUG")

def _linear_interpolation(x, x_list, y_list):
    tb = KratosMultiphysics.PiecewiseLinearTable()
    for i in range(len(x_list)):
        tb.AddRow(x_list[i], y_list[i])

    return tb.GetNearestValue(x)

def _normpdf(x, mean, sd):
    dir_path = os.path.dirname(os.path.realpath(__file__))
    data = read_external_json(dir_path+"/normal_distribution.json")
    z = (x-mean)/sd
    z_list = data["Z"]
    prob_list = data["Prob"]
    if (z > 0):
        prob = _linear_interpolation(z, z_list, prob_list)
    else:
        prob = 1.0 - _linear_interpolation(-z, z_list, prob_list)
    return prob


def _normvalf(prob, mean, sd):
    dir_path = os.path.dirname(os.path.realpath(__file__))
    data = read_external_json(dir_path+"/normal_distribution.json")
    z_list = data["Z"]
    prob_list = data["Prob"]
    if (prob >= 0.5):
        z = _linear_interpolation(prob, prob_list, z_list)
    else:
        z = - _linear_interpolation(1.0 - prob, prob_list, z_list)
    x = z * sd + mean
    return x
