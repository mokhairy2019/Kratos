// KRATOS___
//     //   ) )
//    //         ___      ___
//   //  ____  //___) ) //   ) )
//  //    / / //       //   / /
// ((____/ / ((____   ((___/ /  MECHANICS
//
//  License:         geo_mechanics_application/license.txt
//
//  Main authors:    Jonathan Nuttall
//

#pragma once
#include <string>
#include <iostream>

#include <fstream>
#include <iterator>
#include <algorithm>

// Project includes
#include "containers/model.h"
#include "testing/testing.h"
#include "cpp_geomechanics_application.h"
#include "empty_progress.h"

using namespace std;
using namespace empty_progress;

bool compareFiles(const std::string& p1, const std::string& p2) {
    std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
        return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
        std::istreambuf_iterator<char>(),
        std::istreambuf_iterator<char>(f2.rdbuf()));
}

namespace Kratos
{
    namespace Testing
    {

    	KRATOS_TEST_CASE_IN_SUITE(CalculateExtrapolatedHeadFlow, KratosGeoMechanicsFastSuite)
        {

    		auto workingDirectory = "./applications/GeoMechanicsApplication/tests/test_head_extrapolation_custom_workflow_flow";
            auto projectFile = "ProjectParameters.json";

            auto execute = Kratos::KratosExecute();
            int status = execute.execute_flow_analysis(workingDirectory, projectFile, 
                0, 0, 0,
                "", &emptyLog, &emptyProgress,
                &emptyLog, &emptyCancel);

    		KRATOS_CHECK_EQUAL(status, 0);

			//output_files
            string original = (string)workingDirectory + "/test_head_extrapolate.orig.res";
            string result = (string)workingDirectory + "/test_head_extrapolate.post.res";

            KRATOS_CHECK(compareFiles(original, result));
        }
    }
}