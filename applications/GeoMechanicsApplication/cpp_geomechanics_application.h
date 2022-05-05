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

// System includes

/* External includes */

/* Utility includes */
#include "includes/model_part.h"
#include "spaces/ublas_space.h"

// Linear solvers
#include "linear_solvers/linear_solver.h"

// The most basic scheme (static)
#include "custom_strategies/schemes/backward_euler_quasistatic_Pw_scheme.hpp"

// The most builder and solver (the block builder and solver)
#include "solving_strategies/builder_and_solvers/residualbased_block_builder_and_solver.h"

// The strategies to test
#include <custom_processes/apply_component_table_process.hpp>
#include <custom_processes/apply_constant_hydrostatic_pressure_process.hpp>
#include <ghc/filesystem.hpp>
#include <includes/gid_io.h>
#include <linear_solvers/skyline_lu_factorization_solver.h>

#include <solving_strategies/convergencecriterias/mixed_generic_criteria.h>
#include <solving_strategies/strategies/implicit_solving_strategy.h>
#include <solving_strategies/strategies/residualbased_newton_raphson_strategy.h>

#include "custom_strategies/strategies/geo_mechanics_newton_raphson_erosion_process_strategy.hpp"


using namespace std;

#pragma region NodeVariables

class NodeOperation
{
public:
	virtual ~NodeOperation() = default;
	virtual void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeDISPLACEMENT : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeTOTAL_DISPLACEMENT : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeWATER_PRESSURE : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeNORMAL_FLUID_FLUX : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeVOLUME_ACCELERATION : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class NodeHYDRAULIC_DISCHARGE : public NodeOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
#pragma endregion NodeVariables

#pragma region GaussVariables
class GaussOperation
{
public:
    virtual void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class GaussFLUID_FLUX_VECTOR : public GaussOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class GaussHYDRAULIC_HEAD : public GaussOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class GaussLOCAL_FLUID_FLUX_VECTOR : public GaussOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
class GaussLOCAL_PERMEABILITY_MATRIX : public GaussOperation {
public:
    void write(Kratos::GidIO<>& gid_io, Kratos::ModelPart& model_part);
};
#pragma endregion GaussVariables

namespace Kratos
{
    class KratosExecute
    {
    public:

        KratosExecute() {};

        int cpp_geomechanics(string meshpath, string projectpath, string materialpath);

        typedef Node<3> NodeType;
        typedef Geometry<NodeType> GeometryType;
        typedef UblasSpace<double, CompressedMatrix, Vector> SparseSpaceType;
        typedef UblasSpace<double, Matrix, Vector> LocalSpaceType;

        // The direct solver
        typedef LinearSolver<SparseSpaceType, LocalSpaceType> LinearSolverType;
        typedef SkylineLUFactorizationSolver<SparseSpaceType, LocalSpaceType > SkylineLUFactorizationSolverType;

        // The convergence criteria type
        typedef ConvergenceCriteria< SparseSpaceType, LocalSpaceType > ConvergenceCriteriaType;
        typedef MixedGenericCriteria<SparseSpaceType, LocalSpaceType> MixedGenericCriteriaType;
        typedef typename MixedGenericCriteriaType::ConvergenceVariableListType ConvergenceVariableListType;

        // The builder ans solver type
        typedef BuilderAndSolver< SparseSpaceType, LocalSpaceType, LinearSolverType > BuilderAndSolverType;
        typedef ResidualBasedBlockBuilderAndSolver< SparseSpaceType, LocalSpaceType, LinearSolverType > ResidualBasedBlockBuilderAndSolverType;

        // The time scheme
        typedef Scheme< SparseSpaceType, LocalSpaceType >  SchemeType;
        typedef BackwardEulerQuasistaticPwScheme< SparseSpaceType, LocalSpaceType > BackwardEulerQuasistaticPwSchemeType;

        // The strategies
        typedef ImplicitSolvingStrategy< SparseSpaceType, LocalSpaceType, LinearSolverType >
            ImplicitSolvingStrategyType;

        typedef ResidualBasedNewtonRaphsonStrategy< SparseSpaceType, LocalSpaceType, LinearSolverType >
            ResidualBasedNewtonRaphsonStrategyType;

        typedef GeoMechanicsNewtonRaphsonErosionProcessStrategy< SparseSpaceType, LocalSpaceType, LinearSolverType >
            GeoMechanicsNewtonRaphsonErosionProcessStrategyType;

        // Dof arrays
        typedef PointerVectorSet<Dof<double>, SetIdentityFunction<Dof<double>>, std::less<SetIdentityFunction<Dof<double>>::result_type>,
            std::equal_to<SetIdentityFunction<Dof<double>>::result_type>, Dof<double>* > DofsArrayType;

        ConvergenceCriteriaType::Pointer setup_criteria_dgeoflow();
        LinearSolverType::Pointer setup_solver_dgeoflow();
        GeoMechanicsNewtonRaphsonErosionProcessStrategyType::Pointer setup_strategy_dgeoflow(ModelPart& model_part);
        void parseMesh(ModelPart& model_part, string filepath);
        void parseMaterial(Model& model, string filepath);

        Parameters openProjectParamsFile(string filepath);
        std::vector<std::shared_ptr<Process>> parseProcess(ModelPart& model_part, Parameters projFile);
        void outputGiD(Model& model, ModelPart& model_part, Parameters parameters);
        

    };
}