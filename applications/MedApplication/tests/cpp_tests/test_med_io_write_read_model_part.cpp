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

} // Kratos::Testing
