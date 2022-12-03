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
#include <utility> // std::as_const

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

    rPopulateFunction(test_model_part_write);

    { // encapsulating to ensure memory (aka file handle) is freed
        MedModelPartIO io_write(rFileName);
        io_write.WriteModelPart(std::as_const(test_model_part_write));
    }
    { // encapsulating to ensure memory (aka file handle) is freed
        MedModelPartIO io_read(rFileName);
        io_read.ReadModelPart(test_model_part_read);
    }

    // remove before checking ModelParts, as would be left over if comparison fails
    if (std::filesystem::exists(rFileName)) {
        std::filesystem::remove(rFileName);
    }

    MedTestingUtilities::CheckModelPartsAreEqual(test_model_part_write, test_model_part_read);
}

} // helpers namespace

KRATOS_TEST_CASE(WriteReadMedNodes)
{
    MedWriteReadModelPart("med_nodes_only.hdf", [](ModelPart& rModelPart){
        for (int i=0; i<200; ++i) {
            rModelPart.CreateNewNode(i+1, i*1.15, i-i*1.45, i+153);
        }
    });
}

} // Kratos::Testing
