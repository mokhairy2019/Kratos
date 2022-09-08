//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Philipp Bucher
//
//


// Project includes
#include "testing/testing.h"
#include "utilities/stl_vector_io.h"
#include "includes/"

namespace Kratos {
namespace Testing {

KRATOS_TEST_CASE_IN_SUITE(StdVectorOutputStream, KratosCoreFastSuite)
{
    using Kratos::operator<<; // needed bcs it is inside the namespace "Testing"

    std::vector<int> int_vector {1,5,-63,581,6};
    std::vector<double> double_vector {4.335, 8.639, -888.47, 9874.0};
    std::vector<std::string> string_vector {"val_1", "custom", "again_test"};

    std::stringstream ss_int;
    ss_int << int_vector;
    KRATOS_CHECK_STRING_EQUAL(ss_int.str(), "[1, 5, -63, 581, 6]");

    std::stringstream ss_double;
    ss_double << double_vector;
    KRATOS_CHECK_STRING_EQUAL(ss_double.str(), "[4.335, 8.639, -888.47, 9874]");

    std::stringstream ss_string;
    ss_string << string_vector;
    KRATOS_CHECK_STRING_EQUAL(ss_string.str(), "[val_1, custom, again_test]");
}

KRATOS_TEST_CASE_IN_SUITE(StdWeakPtrOutputStream, KratosCoreFastSuite)
{
    using Kratos::operator<<; // needed bcs it is inside the namespace "Testing"

    std::weak_ptr<int> wp;

    {
      auto sp = std::make_shared<int>(42);
      wp=sp;
      std::stringstream ss_wp;
      ss_wp << wp;
      KRATOS_CHECK_STRING_EQUAL(ss_wp.str(), "42");
    }

    std::stringstream ss_wp;
    ss_wp << wp;
    KRATOS_CHECK_STRING_EQUAL(ss_wp.str(), " expired weak_ptr ");
}

}   // namespace Testing
}  // namespace Kratos.
