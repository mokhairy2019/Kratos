//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Alejandro Cornejo
//
//

// System includes

// External includes

// Project includes
#include "includes/define_python.h"
#include "python/add_accessor_to_python.h"
#include "includes/accessor.h"

namespace Kratos::Python
{

void AddAccessorToPython(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<Accessor, Accessor::UniquePointer>(m, "Accessor");
}

}  // namespace Kratos::Python.

