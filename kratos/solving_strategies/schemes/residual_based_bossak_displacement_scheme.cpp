//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Riccardo Rossi
//
//

/* System includes */

/* External includes */

/* Project includes */
#include "spaces/ublas_space.h"
#include "solving_strategies/schemes/residual_based_bossak_displacement_scheme.hpp"

namespace Kratos
{

///@name Type Definitions
///@{

typedef TUblasSparseSpace<double> SparseSpaceType;
typedef TUblasDenseSpace<double> LocalSpaceType;

typedef Scheme<SparseSpaceType, LocalSpaceType> SchemeType;
typedef ResidualBasedBossakDisplacementScheme<SparseSpaceType,  LocalSpaceType> ResidualBasedBossakDisplacementSchemeType;

//NOTE: here we must create persisting objects for the strategies
static ResidualBasedBossakDisplacementSchemeType msResidualBasedBossakDisplacementScheme;

template<>
std::vector<Internals::RegisteredPrototypeBase<SchemeType>> ResidualBasedBossakDisplacementSchemeType::msPrototypes{
    Internals::RegisteredPrototype<ResidualBasedBossakDisplacementSchemeType, SchemeType>(ResidualBasedBossakDisplacementSchemeType::Name(), msResidualBasedBossakDisplacementScheme)};

///@}

} /* namespace Kratos.*/
