// KRATOS  __  __          _    _                _ _           _   _
//        |  \/  | ___  __| |  / \   _ __  _ __ | (_) ___ __ _| |_(_) ___  _ ___
//        | |\/| |/ _ \/ _` | / _ \ | '_ \| '_ \| | |/ __/ _` | __| |/ _ \| '_  |
//        | |  | |  __/ (_| |/ ___ \| |_) | |_) | | | (_| (_| | |_| | (_) | | | |
//        |_|  |_|\___|\__,_/_/   \_\ .__/| .__/|_|_|\___\__,_|\__|_|\___/|_| |_|
//                                  |_|   |_|
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

// System includes

// External includes

// Project includes
#include "containers/model.h"
#include "geometries/quadrilateral_2d_4.h"
#include "processes/structured_mesh_generator_process.h"
#include "testing/testing.h"
#include "custom_io/med_model_part_io.h"
#include "custom_utilities/med_testing_utilities.h"

namespace Kratos::Testing {

namespace { // helpers namespace

void MedWriteReadModelPart(
    const std::filesystem::path& rFileName,
    const std::function<void(ModelPart&)>& rPopulateFunction)
{
    Model model;
    auto& test_model_part_write = model.CreateModelPart("test_write");
    auto& test_model_part_read = model.CreateModelPart("test_read");

    auto full_name = rFileName;
    full_name.replace_extension(".med");

    rPopulateFunction(test_model_part_write);

    { // encapsulating to ensure memory (aka file handle) is freed
        MedModelPartIO io_write(full_name, IO::WRITE);
        io_write.WriteModelPart(test_model_part_write);
    }
    { // encapsulating to ensure memory (aka file handle) is freed
        MedModelPartIO io_read(full_name);
        io_read.ReadModelPart(test_model_part_read);
    }

    // remove before checking ModelParts, as would be left over if comparison fails
    if (std::filesystem::exists(full_name)) {
        std::filesystem::remove(full_name);
    }

    MedTestingUtilities::CheckModelPartsAreEqual(test_model_part_write, test_model_part_read);
}

} // helpers namespace

KRATOS_TEST_CASE_IN_SUITE(WriteReadMedEmpty, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        // deliberately do not create any entities
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteReadMedNodes, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        int node_id = 0;
        for (int x=0; x<20; ++x) {
            for (int y=0; y<10; ++y) {
                for (int z=0; z<15; ++z) {
                    rModelPart.CreateNewNode(++node_id, x, y, z);
                }
            }
        }
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteReadMedNodesNonConsecutiveNodeIds, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        int node_id = 1;
        for (int x=0; x<20; ++x) {
            for (int y=0; y<10; ++y) {
                for (int z=0; z<15; ++z) {
                    rModelPart.CreateNewNode(node_id, x, y, z);
                    node_id += 5;
                }
            }
        }
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteRead2DLineMesh, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        constexpr std::size_t num_nodes = 10;
        for (std::size_t i=0; i<num_nodes; ++i) {
            rModelPart.CreateNewNode(i+1, i,0,0);
        }
        for (std::size_t i=0; i<num_nodes-1; ++i) {
            rModelPart.CreateNewGeometry("Line3D2", std::vector<ModelPart::IndexType>{i+1, i+2});
        }
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteRead2DLineMeshNonConsecutiveNodeIds, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        constexpr std::size_t num_nodes = 10;
        for (std::size_t i=0; i<num_nodes; ++i) {
            rModelPart.CreateNewNode(i*10+1, i,0,0)->Id();
        }
        for (std::size_t i=0; i<num_nodes-1; ++i) {
            rModelPart.CreateNewGeometry("Line3D2", std::vector<ModelPart::IndexType>{i*10+1, (i+1)*10+1});
        }
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteRead2DTriangularMesh, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        auto p_point_1 = Kratos::make_intrusive<Node<3>>(1, 0.0, 0.0, 0.0);
        auto p_point_2 = Kratos::make_intrusive<Node<3>>(2, 0.0, 1.0, 0.0);
        auto p_point_3 = Kratos::make_intrusive<Node<3>>(3, 1.0, 1.0, 0.0);
        auto p_point_4 = Kratos::make_intrusive<Node<3>>(4, 1.0, 0.0, 0.0);

        Quadrilateral2D4<Node<3>> geometry(p_point_1, p_point_2, p_point_3, p_point_4);

        Parameters mesher_parameters(R"(
        {
            "number_of_divisions": 7,
            "element_name": "Element2D3N",
            "create_skin_sub_model_part" : false,
            "create_body_sub_model_part" : false
        })");

        StructuredMeshGeneratorProcess(geometry, rModelPart, mesher_parameters).Execute();
        // create geometries!
        MedTestingUtilities::AddGeometriesFromElements(rModelPart);
    });
}

KRATOS_TEST_CASE_IN_SUITE(WriteRead2DQuadrilateralMesh, KratosMedFastSuite)
{
    MedWriteReadModelPart(this->Name(), [](ModelPart& rModelPart){
        constexpr std::size_t num_quads = 10;
        int node_id = 0;
        for (std::size_t i=0; i<num_quads+1; ++i) {
            rModelPart.CreateNewNode(++node_id, i*1,0,0);
            rModelPart.CreateNewNode(++node_id, i*1,0.5,0);
        }

        for (std::size_t i=0; i<num_quads; ++i) {
            rModelPart.CreateNewGeometry("Quadrilateral2D4", std::vector<ModelPart::IndexType>{
                (i*2)+1,
                (i*2)+3,
                (i*2)+4,
                (i*2)+2
            });
        }
    });
}

} // Kratos::Testing
