from abc import ABC, abstractmethod

import KratosMultiphysics as Kratos
import KratosMultiphysics.OptimizationApplication as KratosOA
import KratosMultiphysics.KratosUnittest as kratos_unittest
from KratosMultiphysics.OptimizationApplication.responses.mass_response_function import MassResponseFunction
from KratosMultiphysics.OptimizationApplication.utilities.opt_line_search import *
from KratosMultiphysics.OptimizationApplication.utilities.optimization_problem import OptimizationProblem
from KratosMultiphysics.OptimizationApplication.utilities.component_data_view import ComponentDataView



class TestAlgorithmSteepestDescent(kratos_unittest.TestCase, ABC):
    @classmethod
    def setUpClass(cls):
        cls.model = Kratos.Model()
        cls.model_part = cls.model.CreateModelPart("test")
        cls.CreateElements()

        ### create response

        default_settings = Kratos.Parameters("""{
            "evaluated_model_part_names"     : [
                "test"
            ]
        }""")
        cls.response_function = MassResponseFunction("mass", cls.model, default_settings)

        cls.optimization_problem = OptimizationProblem()
        ComponentDataView("algorithm", cls.optimization_problem).SetDataBuffer(1)

    @classmethod
    def CreateElements(cls):
        cls.model_part.CreateNewNode(1, 0.0, 0.0, 0.0)
        cls.model_part.CreateNewNode(2, 1.0, 0.0, 0.0)
        cls.model_part.CreateNewNode(3, 1.0, 1.0, 0.0)
        cls.model_part.CreateNewNode(4, 0.0, 1.0, 0.0)

        properties = cls.model_part.CreateNewProperties(1)
        properties[Kratos.DENSITY] = 2.0
        properties[Kratos.THICKNESS] = 3.0
        cls.model_part.CreateNewElement("Element2D3N", 1, [1, 2, 3], properties)

        properties = cls.model_part.CreateNewProperties(2)
        properties[Kratos.DENSITY] = 4.0
        properties[Kratos.THICKNESS] = 6.0
        cls.model_part.CreateNewElement("Element2D3N", 2, [4, 1, 3], properties)

    def test_ConstantLineSearch(self):
        line_search_settings = Kratos.Parameters("""{
            "type"              : "const_step",
            "init_step"          : 3.0
        }""")  
        line_search = CreateLineSearch(line_search_settings, self.optimization_problem)
        self.response_function.Initialize()
        self.response_function.CalculateValue()
        sensitivity = KratosOA.ContainerExpression.CollectiveExpressions([KratosOA.ContainerExpression.ElementPropertiesExpression(self.model_part)])
        self.response_function.CalculateGradient({Kratos.DENSITY: sensitivity})
        ComponentDataView("algorithm", self.optimization_problem).GetBufferedData()["search_direction"] = sensitivity
        alpha = line_search.ComputeStep()
        self.assertEqual(alpha, 1.0)

if __name__ == "__main__":
    Kratos.Tester.SetVerbosity(Kratos.Tester.Verbosity.PROGRESS)  # TESTS_OUTPUTS
    kratos_unittest.main()