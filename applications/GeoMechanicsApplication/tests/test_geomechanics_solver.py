
import KratosMultiphysics as Kratos
from KratosMultiphysics.GeoMechanicsApplication import geomechanics_solver

import KratosMultiphysics.KratosUnittest as KratosUnittest


class KratosGeoMechanicsSolverTests(KratosUnittest.TestCase):
    """
    This class contains benchmark tests which are checked with the analytical solution
    """

    def setUp(self):
        # Code here will be placed BEFORE every test in this TestCase.
        pass

    def tearDown(self):
        # Code here will be placed AFTER every test in this TestCase.
        pass

    @staticmethod
    def create_solver(model, parameters):
        """
        Creates the geomechanics solver in a function, which is required for assertRaises
        """
        geomechanics_solver.CreateSolver(model, parameters)

    def test_validate_settings(self):
        """
        Tests if the settings are validated correctly and if the correct exceptions are thrown
        """

        # initialize parameters
        parameters = geomechanics_solver.GeoMechanicalSolver.GetDefaultParameters()

        # set scaling on true and rebuild level on 1, such that an exception is thrown
        parameters["linear_solver_settings"]["scaling"].SetBool(True)
        parameters["rebuild_level"].SetInt(1)

        with self.assertRaises(ValueError) as context:
            self.create_solver(Kratos.Model(), parameters)
        self.assertTrue("Scaling can only be used if rebuild level is 2" in str(context.exception))

        # reinitialize parameters
        parameters = geomechanics_solver.GeoMechanicalSolver.GetDefaultParameters()
        # set scaling on true and rebuild level on 1, such that an exception is thrown
        parameters["linear_solver_settings"]["scaling"].SetBool(True)
        parameters["prebuild_dynamics"].SetBool(True)

        with self.assertRaises(ValueError) as context:
            self.create_solver(Kratos.Model(), parameters)
        self.assertTrue("Scaling can not be used if prebuild dynamics is true" in str(context.exception))

