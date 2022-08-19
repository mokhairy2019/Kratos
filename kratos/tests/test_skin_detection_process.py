import os

import KratosMultiphysics
import KratosMultiphysics.KratosUnittest as KratosUnittest
import KratosMultiphysics.kratos_utilities as kratos_utils
from KratosMultiphysics.gid_output_process import GiDOutputProcess
from KratosMultiphysics.testing.utilities import ReadModelPart

def GetFilePath(fileName):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), fileName)

def RemoveFiles(mdpa_name):
    kratos_utils.DeleteFileIfExisting(mdpa_name + ".time")

class TestSkinDetectionProcess(KratosUnittest.TestCase):
    @classmethod
    def setUpClass(cls):
        KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
        cls.current_model = KratosMultiphysics.Model()
        cls.model_part = cls.current_model.CreateModelPart("Main")
        cls.model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE] = 3
        cls.mdpa_name = GetFilePath("auxiliar_files_for_python_unittest/mdpa_files/coarse_sphere")
        ReadModelPart(cls.mdpa_name, cls.model_part)

    @classmethod
    def tearDownClass(cls):
        RemoveFiles(cls.mdpa_name)

    def test_SkinDetectionProcess(self):
        # We set a flag in the already known node in the skin
        for node in self.model_part.GetSubModelPart("Skin_Part").Nodes:
            node.Set(KratosMultiphysics.ACTIVE, True)

        detect_skin = KratosMultiphysics.SkinDetectionProcess3D(self.model_part)
        detect_skin.Execute()

        for node in self.model_part.Nodes:
            self.assertEqual(node.Is(KratosMultiphysics.INTERFACE), node.Is(KratosMultiphysics.ACTIVE))

    def test_SkinDetectionProcessWithAssign(self):
        skin_detection_parameters = KratosMultiphysics.Parameters("""
        {
            "list_model_parts_to_assign_conditions" : ["Skin_Part"]
        }
        """)

        detect_skin = KratosMultiphysics.SkinDetectionProcess3D(self.model_part, skin_detection_parameters)
        detect_skin.Execute()

        self.assertEqual(self.model_part.GetSubModelPart("Skin_Part").NumberOfConditions(), self.model_part.NumberOfConditions())

    def test_SubModelPartSkinDetectionProcess(self):
        # We set a flag in the already known node in the skin
        for node in self.model_part.GetSubModelPart("Partial_Skin_Part").Nodes:
            node.Set(KratosMultiphysics.ACTIVE, True)

        detect_skin = KratosMultiphysics.SubModelPartSkinDetectionProcess3D(self.model_part, KratosMultiphysics.Parameters(r"""
        {
            "name_auxiliar_model_part": "SkinModelPart",
            "selection_criteria": "nodes_on_sub_model_part",
            "selection_settings": {
                "sub_model_part_name" : "Partial_Skin_Part"
            }
        }"""))
        detect_skin.Execute()

        for node in self.model_part.Nodes:
            self.assertEqual(node.Is(KratosMultiphysics.INTERFACE), node.Is(KratosMultiphysics.ACTIVE))

    def test_NotOnSubModelPartSkinDetectionProcess(self):
        detect_skin = KratosMultiphysics.SubModelPartSkinDetectionProcess3D(self.model_part, KratosMultiphysics.Parameters(r"""
        {
            "name_auxiliar_model_part": "SkinModelPart",
            "selection_criteria": "node_not_on_sub_model_part",
            "selection_settings": {
                "sub_model_part_names" : ["Partial_Skin_Part"]
            }
        }"""))
        detect_skin.Execute()

        # Check the number of conditions created
        self.assertEqual(self.model_part.GetCommunicator().GlobalNumberOfConditions(), 112)

if __name__ == '__main__':
    KratosUnittest.main()
