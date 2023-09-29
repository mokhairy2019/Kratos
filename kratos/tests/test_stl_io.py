# Import necessary modules from KratosMultiphysics
import KratosMultiphysics
import KratosMultiphysics.KratosUnittest as KratosUnittest
import KratosMultiphysics.kratos_utilities as kratos_utils
from KratosMultiphysics.testing.utilities import ReadModelPart

# Import the pathlib module
from pathlib import Path

# Import os module
import os

# Define a function to get the file path given a file name
def GetFilePath(fileName):
    """
    Get the absolute file path for a given file name.

    Args:
        fileName (str): The name of the file.

    Returns:
        str: The absolute file path.
    """
    return os.path.join(os.path.dirname(os.path.realpath(__file__)), fileName)

# Define a function to remove files with the given name
def RemoveFiles(file_name):
    """
    Remove files with a given name.

    Args:
        file_name (str): The name of the file to be removed.
    """
    kratos_utils.DeleteFileIfExisting(file_name)

# Define a function to write a Kratos model part to STL format
def WriteModelPartToSTL(model_part, stl_file):
    """
    Write a Kratos model part to an STL file.

    Args:
        model_part (KratosMultiphysics.ModelPart): The Kratos model part to be written.
        stl_file (str): The name of the STL file to write to.
    """
    write_settings = KratosMultiphysics.Parameters("""{"open_mode" : "write"}""")
    stl_io = KratosMultiphysics.StlIO(stl_file, write_settings)
    stl_io.WriteModelPart(model_part)

# Define a function to read a Kratos model part from an STL file
def ReadModelPartFromSTL(model_part, data_comm, stl_file):
    """
    Read a Kratos model part from an STL file.

    Args:
        model_part (KratosMultiphysics.ModelPart): The Kratos model part to populate with data from the STL file.
        data_comm (KratosMultiphysics.DataCommunicator): The data communicator.
        stl_file (str): The name of the STL file to read from.
    """
    if data_comm.Rank() == 0:
        read_settings = KratosMultiphysics.Parameters("""{"open_mode" : "read", "new_entity_type" : "element"}""")
        stl_io = KratosMultiphysics.StlIO(stl_file, read_settings)
        stl_io.ReadModelPart(model_part)

class TestStlIO(KratosUnittest.TestCase):
    """
    Define a test class for testing STL input/output operations
    """
    @classmethod
    def setUpClass(cls):
        """
        Set up the test class before running tests.
        """
        KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
        cls.current_model = KratosMultiphysics.Model()
        cls.model_part = cls.current_model.CreateModelPart("Main")
        cls.model_part.ProcessInfo[KratosMultiphysics.DOMAIN_SIZE] = 3
        cls.stl_file = GetFilePath("aux.stl")
        # Check if the path is writable
        path = Path(cls.stl_file)
        try:
            # Try to open the file in write mode.
            # If this succeeds, the file is writable.
            with path.open('w') as file:
                pass
        except Exception as e:
            # An exception was raised, so the file is likely not writable.
            raise PermissionError(f"The path '{path}' is not writable.") from e

    @classmethod
    def tearDownClass(cls):
        """
        Clean up after running tests.
        """
        RemoveFiles(cls.stl_file)

    # TODO: Add Read test, this test is added mainly to test Write STL files in MPI

    def test_WriteStlIO(self):
        """
        Test the WriteModelPart function form StlIO
        """
        mdpa_name = GetFilePath("auxiliar_files_for_python_unittest/mdpa_files/coarse_sphere_skin")
        ReadModelPart(mdpa_name, self.model_part)
        data_comm = self.model_part.GetCommunicator().GetDataCommunicator()

        # Compute area of the given mesh
        area_1 = 0.0
        for cond in self.model_part.Conditions:
            geom = cond.GetGeometry()
            area_1 += geom.Area()
        area_1 = data_comm.SumAll(area_1)

        # Write current mesh into STL
        WriteModelPartToSTL(self.model_part, self.stl_file)

        # Read it back
        stl_model_part = self.current_model.CreateModelPart("STL")
        ReadModelPartFromSTL(stl_model_part, data_comm, self.stl_file)

        # Compute resulting area
        area_2 = 0.0
        for elem in stl_model_part.Elements:
            geom = elem.GetGeometry()
            area_2 += geom.Area()
        area_2 = data_comm.SumAll(area_2)

        # Assert that the areas match approximately
        self.assertAlmostEqual(area_1, area_2)

if __name__ == '__main__':
    # Set the logger severity level and run the KratosUnittest
    KratosMultiphysics.Logger.GetDefaultOutput().SetSeverity(KratosMultiphysics.Logger.Severity.WARNING)
    KratosUnittest.main()