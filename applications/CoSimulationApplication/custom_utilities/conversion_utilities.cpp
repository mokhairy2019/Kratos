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
//                   Ashish Darekar
//

// System includes

// External includes

// Project includes
#include "utilities/parallel_utilities.h"
#include "utilities/variable_utils.h"
#include "utilities/atomic_utilities.h"
#include "conversion_utilities.h"

namespace Kratos {

void ConversionUtilities::ConvertElementalDataToNodalData(ModelPart& rModelPart)
{
    // initialize Forces
    VariableUtils().SetHistoricalVariableToZero(FORCE, rModelPart.Nodes());

    block_for_each(rModelPart.Elements(), [&](Element& rElement){
        const array_1d<double, 3>& elem_force =  rElement.GetValue(FORCE);

        const std::size_t num_nodes = rElement.GetGeometry().PointsNumber();

        for (auto& r_node : rElement.GetGeometry().Points()){
            AtomicAddVector( r_node.FastGetSolutionStepValue(FORCE), (elem_force / static_cast<double>(num_nodes)) );
        }
    });

    rModelPart.GetCommunicator().AssembleCurrentData(FORCE);
}

}  // namespace Kratos.


