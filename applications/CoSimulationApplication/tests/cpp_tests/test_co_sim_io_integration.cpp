// KRATOS  / ___|___/ ___|(_)_ __ ___  _   _| | __ _| |_(_) ___  _ ___
//        | |   / _ \___ \| | '_ ` _ \| | | | |/ _` | __| |/ _ \| '_  |
//        | |__| (_) |__) | | | | | | | |_| | | (_| | |_| | (_) | | | |
//         \____\___/____/|_|_| |_| |_|\__,_|_|\__,_|\__|_|\___/|_| |_|
//
//  License:		 BSD License
//                   license: CoSimulationApplication/license.txt
//
//  Main authors:    Philipp Bucher (https://github.com/philbucher)
//

// System includes

// External includes

// Project includes
#include "includes/model_part.h"
#include "includes/parallel_environment.h"
#include "containers/model.h"
#include "testing/testing.h"
#include "custom_utilities/co_sim_io_conversion_utilities.h"
#include "co_simulation_testing_utilities.h"

// Utilities

namespace Kratos {
namespace Testing {

KRATOS_TEST_CASE_IN_SUITE(CoSimIOModelPartToKratosModelPart_NodesOnly, KratosCosimulationFastSuite)
{
    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_nodes = 5;

    for (std::size_t i=0; i<num_nodes; ++i) {
        co_sim_io_model_part.CreateNewNode(i+1, i*1.5, i+3.5, i-8.6);
    }

    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfNodes(), num_nodes);
    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfLocalNodes(), num_nodes);
    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfGhostNodes(), 0);
    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfElements(), 0);

    const auto& r_serial_data_comm = ParallelEnvironment::GetDataCommunicator("Serial");

    CoSimIOConversionUtilities::CoSimIOModelPartToKratosModelPart(co_sim_io_model_part, kratos_model_part, r_serial_data_comm);

    CheckModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_TEST_CASE_IN_SUITE(KratosModelPartToCoSimIOModelPart_NodesOnly, KratosCosimulationFastSuite)
{
    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_nodes = 5;

    for (std::size_t i=0; i<num_nodes; ++i) {
        kratos_model_part.CreateNewNode(i+1, i*1.5, i+3.5, i-8.6);
    }

    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfNodes(), num_nodes);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfProperties(), 0);

    CoSimIOConversionUtilities::KratosModelPartToCoSimIOModelPart(kratos_model_part, co_sim_io_model_part);

    CheckModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_TEST_CASE_IN_SUITE(CoSimIOModelPartToKratosModelPart, KratosCosimulationFastSuite)
{
    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    const int node_ids[] = {2,61,159};
    const std::array<double, 3> node_coords = {1.0, -2.7, 9.44};
    co_sim_io_model_part.CreateNewNode(node_ids[0], node_coords[0], node_coords[1], node_coords[2]);
    co_sim_io_model_part.CreateNewNode(node_ids[1], node_coords[1], node_coords[2], node_coords[0]);
    co_sim_io_model_part.CreateNewNode(node_ids[2], node_coords[2], node_coords[0], node_coords[1]);

    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfNodes(), 3);

    const int elem_ids[] = {1,19,21};
    const CoSimIO::ElementType elem_types[] = {
        CoSimIO::ElementType::Point2D,
        CoSimIO::ElementType::Point2D,
        CoSimIO::ElementType::Line2D2
    };

    co_sim_io_model_part.CreateNewElement(elem_ids[0], elem_types[0], {node_ids[0]});
    co_sim_io_model_part.CreateNewElement(elem_ids[1], elem_types[1], {node_ids[1]});
    co_sim_io_model_part.CreateNewElement(elem_ids[2], elem_types[2], {node_ids[1], node_ids[2]});

    KRATOS_CHECK_EQUAL(co_sim_io_model_part.NumberOfElements(), 3);

    const auto& r_serial_data_comm = ParallelEnvironment::GetDataCommunicator("Serial");
    CoSimIOConversionUtilities::CoSimIOModelPartToKratosModelPart(co_sim_io_model_part, kratos_model_part, r_serial_data_comm);

    CheckModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_TEST_CASE_IN_SUITE(KratosModelPartToCoSimIOModelPart, KratosCosimulationFastSuite)
{
    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_nodes = 5;

    for (std::size_t i=0; i<num_nodes; ++i) {
        kratos_model_part.CreateNewNode(i+1, i*1.5, i+3.5, i-8.6);
    }

    auto p_props = kratos_model_part.CreateNewProperties(0);

    std::vector<IndexType> conn {1,2};
    kratos_model_part.CreateNewElement("Element2D2N", 1, conn, p_props);
    conn = {2,3};
    kratos_model_part.CreateNewElement("Element2D2N", 2, conn, p_props);
    conn = {3,4,5};
    kratos_model_part.CreateNewElement("Element2D3N", 3, conn, p_props);

    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfNodes(), num_nodes);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfElements(), 3);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfProperties(), 1);

    CoSimIOConversionUtilities::KratosModelPartToCoSimIOModelPart(kratos_model_part, co_sim_io_model_part);

    CheckModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

namespace DistributedTestHelpers {

int GetPartnerRank()
{
    const auto& r_world_data_comm = ParallelEnvironment::GetDataCommunicator("World");
    const int my_rank = r_world_data_comm.Rank();
    const int world_size = r_world_data_comm.Size();

    return (my_rank+1) % world_size;
}

int GetId(const int NumLocalNodesPerRank, const int LocalNodeIndex)
{
    const int my_rank = ParallelEnvironment::GetDataCommunicator("World").Rank();
    return my_rank*NumLocalNodesPerRank + LocalNodeIndex+1;
}

int GetGhostId(const int NumLocalNodesPerRank, const int LocalNodeIndex)
{
    return GetPartnerRank()*NumLocalNodesPerRank + LocalNodeIndex+1;
}

void CreateDistributedNodes(
    CoSimIO::ModelPart& rCoSimIOModelPart,
    const int NumLocalNodesPerRank,
    const int NumGhostNodesPerRank)
{
    const int my_rank = ParallelEnvironment::GetDataCommunicator("World").Rank();

    auto create_ghost_nodes = [&](){
        for (std::size_t i=0; i<NumGhostNodesPerRank; ++i) {
            rCoSimIOModelPart.CreateNewGhostNode(GetGhostId(NumLocalNodesPerRank, i), 0,0,0, GetPartnerRank());
        }
    };

    if (GetPartnerRank() < my_rank) {
        // Kratos requires a consecutively ordered input as it sorts the nodes internally and then the matching would be lost
        // in case the ghost nodes have smaller ids than the local nodes they have to be added first as currently
        // the CoSimIO::ModelPart does not allow sorting
        create_ghost_nodes();
    }

    // create local nodes
    for (std::size_t i=0; i<NumLocalNodesPerRank; ++i) {
        rCoSimIOModelPart.CreateNewNode(GetId(NumLocalNodesPerRank, i), 0,0,0);
    }

    if (GetPartnerRank() > my_rank) {
        // in this case the ghost nodes have larger Ids than the local nodes
        create_ghost_nodes();
    }

    KRATOS_CHECK_EQUAL(rCoSimIOModelPart.NumberOfNodes(), NumLocalNodesPerRank+NumGhostNodesPerRank);
    KRATOS_CHECK_EQUAL(rCoSimIOModelPart.NumberOfLocalNodes(), NumLocalNodesPerRank);
    KRATOS_CHECK_EQUAL(rCoSimIOModelPart.NumberOfGhostNodes(), NumGhostNodesPerRank);
    KRATOS_CHECK_EQUAL(rCoSimIOModelPart.NumberOfElements(), 0);
}

} // DistributedTestHelpers

KRATOS_DISTRIBUTED_TEST_CASE_IN_SUITE(DistributedCoSimIOModelPartToKratosModelPart_NodesOnly, KratosCosimulationMPIFastSuite)
{
    const auto& r_world_data_comm = ParallelEnvironment::GetDataCommunicator("World");

    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_local_nodes_per_rank = 5;
    constexpr std::size_t num_ghost_nodes_per_rank = 3;

    DistributedTestHelpers::CreateDistributedNodes(co_sim_io_model_part, num_local_nodes_per_rank, num_ghost_nodes_per_rank);

    CoSimIOConversionUtilities::CoSimIOModelPartToKratosModelPart(co_sim_io_model_part, kratos_model_part, r_world_data_comm);

    CheckDistributedModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_DISTRIBUTED_TEST_CASE_IN_SUITE(DistributedKratosModelPartToCoSimIOModelPart_NodesOnly, KratosCosimulationMPIFastSuite)
{
    const auto& r_world_data_comm = ParallelEnvironment::GetDataCommunicator("World");
    const int my_rank = r_world_data_comm.Rank();
    const int world_size = r_world_data_comm.Size();

    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");
    kratos_model_part.AddNodalSolutionStepVariable(PARTITION_INDEX);

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_local_nodes_per_rank = 5;
    constexpr std::size_t num_ghost_nodes_per_rank = 3;

    // create local nodes
    for (std::size_t i=0; i<num_local_nodes_per_rank; ++i) {
        auto node = kratos_model_part.CreateNewNode(DistributedTestHelpers::GetId(num_local_nodes_per_rank, i), i*10,i/3.58,i-99.14);
        node->FastGetSolutionStepValue(PARTITION_INDEX) = my_rank;
    }

    for (std::size_t i=0; i<num_ghost_nodes_per_rank; ++i) {
        auto node = kratos_model_part.CreateNewNode(DistributedTestHelpers::GetGhostId(num_local_nodes_per_rank, i), i-10,i*3,i+99.14);
        node->FastGetSolutionStepValue(PARTITION_INDEX) = DistributedTestHelpers::GetPartnerRank();
    }

    // this calls the ParallelFillCommunicator
    ParallelEnvironment::CreateFillCommunicatorFromGlobalParallelism(kratos_model_part, r_world_data_comm)->Execute();

    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfNodes(), num_local_nodes_per_rank+num_ghost_nodes_per_rank);
    KRATOS_CHECK_EQUAL(kratos_model_part.GetCommunicator().LocalMesh().NumberOfNodes(), num_local_nodes_per_rank);
    KRATOS_CHECK_EQUAL(kratos_model_part.GetCommunicator().GhostMesh().NumberOfNodes(), num_ghost_nodes_per_rank);
    KRATOS_CHECK_EQUAL(kratos_model_part.GetCommunicator().GlobalNumberOfNodes(), (num_local_nodes_per_rank)*world_size);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfProperties(), 0);

    CoSimIOConversionUtilities::KratosModelPartToCoSimIOModelPart(kratos_model_part, co_sim_io_model_part);

    CheckDistributedModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_DISTRIBUTED_TEST_CASE_IN_SUITE(DistributedCoSimIOModelPartToKratosModelPart, KratosCosimulationMPIFastSuite)
{
    const auto& r_world_data_comm = ParallelEnvironment::GetDataCommunicator("World");

    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_local_nodes_per_rank = 5;
    constexpr std::size_t num_ghost_nodes_per_rank = 3;

    DistributedTestHelpers::CreateDistributedNodes(co_sim_io_model_part, num_local_nodes_per_rank, num_ghost_nodes_per_rank);

    CoSimIOConversionUtilities::CoSimIOModelPartToKratosModelPart(co_sim_io_model_part, kratos_model_part, r_world_data_comm);

    CheckDistributedModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

KRATOS_DISTRIBUTED_TEST_CASE_IN_SUITE(KratosDistributedModelPartToCoSimIOModelPart, KratosCosimulationMPIFastSuite)
{
    const auto& r_world_data_comm = ParallelEnvironment::GetDataCommunicator("World");
    const int my_rank = r_world_data_comm.Rank();
    const int world_size = r_world_data_comm.Size();

    Model model;
    auto& kratos_model_part = model.CreateModelPart("kratos_mp");
    kratos_model_part.AddNodalSolutionStepVariable(PARTITION_INDEX);

    CoSimIO::ModelPart co_sim_io_model_part("co_sim_io_mp");

    constexpr std::size_t num_nodes = 5;

    for (std::size_t i=0; i<num_nodes; ++i) {
        kratos_model_part.CreateNewNode(i+1, i*1.5, i+3.5, i-8.6);
    }

    auto p_props = kratos_model_part.CreateNewProperties(0);

    std::vector<IndexType> conn {1,2};
    kratos_model_part.CreateNewElement("Element2D2N", 1, conn, p_props);
    conn = {2,3};
    kratos_model_part.CreateNewElement("Element2D2N", 2, conn, p_props);
    conn = {3,4,5};
    kratos_model_part.CreateNewElement("Element2D3N", 3, conn, p_props);

    // this calls the ParallelFillCommunicator
    ParallelEnvironment::CreateFillCommunicatorFromGlobalParallelism(kratos_model_part, r_world_data_comm)->Execute();

    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfNodes(), num_nodes);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfElements(), 3);
    KRATOS_CHECK_EQUAL(kratos_model_part.NumberOfProperties(), 1);

    CoSimIOConversionUtilities::KratosModelPartToCoSimIOModelPart(kratos_model_part, co_sim_io_model_part);

    CheckDistributedModelPartsAreEqual(kratos_model_part, co_sim_io_model_part);
}

} // namespace Testing
} // namespace Kratos
