import os
import KratosMultiphysics as Kratos
from KratosMultiphysics import Logger
Logger.GetDefaultOutput().SetSeverity(Logger.Severity.WARNING)
import KratosMultiphysics.KratosUnittest as KratosUnittest
import KratosMultiphysics.DEMApplication.DEM_analysis_stage as DEM_analysis_stage

import auxiliary_functions_for_tests

this_working_dir_backup = os.getcwd()

def GetFilePath(fileName):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), fileName)

class GluedParticlesTestSolution(DEM_analysis_stage.DEMAnalysisStage, KratosUnittest.TestCase):

    @classmethod
    def GetMainPath(self):
        return os.path.join(os.path.dirname(os.path.realpath(__file__)), "glued_particles_tests_files")

    def GetProblemNameWithPath(self):
        return os.path.join(self.main_path, self.DEM_parameters["problem_name"].GetString())

    def FinalizeSolutionStep(self):
        super().FinalizeSolutionStep()
        tolerance = 1e-4
        for node in self.spheres_model_part.Nodes:
            angular_velocity = node.GetSolutionStepValue(Kratos.ANGULAR_VELOCITY)
            if node.Id == 1:
                if self.time > 0.01:
                    self.assertAlmostEqual(angular_velocity[0], 2.0, delta=tolerance)

                if self.time > 0.499999 and self.time < 0.5000001:
                    self.assertAlmostEqual(node.X, -1.0, delta=tolerance)
                    self.assertAlmostEqual(node.Y, 0.6634116060768411, delta=tolerance)
                    self.assertAlmostEqual(node.Z, 0.21612092234725555, delta=tolerance)

                if self.time > 0.999999 and self.time < 1.0000001:
                    self.assertAlmostEqual(node.X, -1.0, tolerance)
                    self.assertAlmostEqual(node.Y, 0.6362810292697275, delta=tolerance)
                    self.assertAlmostEqual(node.Z, -0.16645873461885752, delta=tolerance)

class TestGluedParticles(KratosUnittest.TestCase):

    def setUp(self):
        pass

    @classmethod
    def test_Glued_Particles_1(self):
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "glued_particles_tests_files")
        parameters_file_name = os.path.join(path, "ProjectParametersDEM.json")
        model = Kratos.Model()
        auxiliary_functions_for_tests.CreateAndRunStageInSelectedNumberOfOpenMPThreads(GluedParticlesTestSolution, model, parameters_file_name, 1)


    @staticmethod
    def tearDown():
        files_to_delete_list = []
        files_to_delete_list.append(os.path.join("TimesPartialRelease"))
        files_to_delete_list.append(os.path.join("glued_particles_tests_files", "sticky_wall_test.post.lst"))
        files_to_delete_list.append(os.path.join("glued_particles_tests_files", "flux_data_new.hdf5"))

        for to_erase_file in files_to_delete_list:
            if os.path.exists(to_erase_file):
                os.remove(to_erase_file)

        #............Getting rid of unuseful folders
        folders_to_delete_list      = []
        folders_to_delete_list.append(os.path.join("glued_particles_tests_files", "sticky_wall_test_Graphs"))
        folders_to_delete_list.append(os.path.join("glued_particles_tests_files", "sticky_wall_test_MPI_results"))
        folders_to_delete_list.append(os.path.join("glued_particles_tests_files", "sticky_wall_test_Post_Files"))
        folders_to_delete_list.append(os.path.join("glued_particles_tests_files", "sticky_wall_test_Results_and_Data"))


        for to_erase_folder in folders_to_delete_list:
            import shutil
            shutil.rmtree(to_erase_folder)

        os.chdir(this_working_dir_backup)





if __name__ == "__main__":
    Kratos.Logger.GetDefaultOutput().SetSeverity(Logger.Severity.WARNING)
    KratosUnittest.main()
