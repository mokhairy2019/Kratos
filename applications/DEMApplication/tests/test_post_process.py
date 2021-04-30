import os
import shutil
import KratosMultiphysics
from KratosMultiphysics import Logger
Logger.GetDefaultOutput().SetSeverity(Logger.Severity.WARNING)
import KratosMultiphysics.DEMApplication as DEM
import KratosMultiphysics.KratosUnittest as KratosUnittest
import KratosMultiphysics.DEMApplication.DEM_analysis_stage

import auxiliary_functions_for_tests

this_working_dir_backup = os.getcwd()

def GetFilePath(fileName):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), fileName)

class TestPostProcessClass1(KratosMultiphysics.DEMApplication.DEM_analysis_stage.DEMAnalysisStage, KratosUnittest.TestCase):

    @classmethod
    def GetMainPath(self):
        return os.path.join(os.path.dirname(os.path.realpath(__file__)), "post_process_tests_files")

    def GetProblemNameWithPath(self):
        return os.path.join(self.main_path, self.DEM_parameters["problem_name"].GetString())

    def Finalize(self):
        self.procedures.RemoveFoldersWithResults(str(self.main_path), str(self.problem_name), '')
        super().Finalize()

class TestPostProcess(KratosUnittest.TestCase):

    def setUp(self):
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "post_process_tests_files")
        tests_path = os.path.dirname(os.path.realpath(__file__))
        clusters_path = os.path.join(tests_path, "..", "custom_elements", "custom_clusters")
        ring_cluster_path = os.path.join(clusters_path, "ringcluster3D.clu")
        shutil.copy(ring_cluster_path, path)

    @classmethod
    def test_gid_printing_many_results(self):
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "post_process_tests_files")
        parameters_file_name = os.path.join(path, "ProjectParametersDEM.json")
        model = KratosMultiphysics.Model()
        auxiliary_functions_for_tests.CreateAndRunStageInSelectedNumberOfOpenMPThreads(TestPostProcessClass1, model, parameters_file_name, 1)


    @staticmethod
    def tearDown():
        files_to_delete_list = []
        files_to_delete_list.append(os.path.join("TimesPartialRelease"))
        files_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "processes.post.lst"))
        files_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "flux_data_new.hdf5"))

        for to_erase_file in files_to_delete_list:
            if os.path.exists(to_erase_file):
                os.remove(to_erase_file)

        #............Getting rid of unuseful folders
        folders_to_delete_list      = []
        folders_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "processes_Graphs"))
        folders_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "processes_MPI_results"))
        folders_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "processes_Post_Files"))
        folders_to_delete_list.append(os.path.join("kinematic_constraints_tests_files", "processes_Results_and_Data"))


        for to_erase_folder in folders_to_delete_list:
            shutil.rmtree(to_erase_folder)

        os.chdir(this_working_dir_backup)


if __name__ == "__main__":
    Logger.GetDefaultOutput().SetSeverity(Logger.Severity.WARNING)
    KratosUnittest.main()
