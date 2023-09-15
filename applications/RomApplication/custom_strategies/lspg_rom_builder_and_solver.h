//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:        BSD License
//                  Kratos default license: kratos/license.txt
//
//  Main authors:   Sebastian Ares de Parga Regalado
//                  Eduard Gomez Escandell
//

#pragma once

/* System includes */

/* External includes */
#include <Eigen/Core>
#include <Eigen/Dense>

/* Project includes */
#include "includes/define.h"
#include "includes/model_part.h"
#include "solving_strategies/schemes/scheme.h"
#include "custom_strategies/global_rom_builder_and_solver.h"
#include "utilities/builtin_timer.h"
#include "utilities/dense_householder_qr_decomposition.h"

/* Application includes */
#include "rom_application_variables.h"
#include "custom_utilities/rom_auxiliary_utilities.h"
#include "custom_utilities/rom_residuals_utility.h"

namespace Kratos
{

///@name Kratos Globals
///@{


///@}
///@name Type Definitions
///@{


///@}
///@name  Enum's
///@{


///@}
///@name  Functions
///@{


///@}
///@name Kratos Classes
///@{

/**
 * @class LeastSquaresPetrovGalerkinROMBuilderAndSolver
 * @ingroup RomApplication
 * @brief This class provides an implementation for the LeastSquaresPetrovGalerkinROM builder and solver operations.
 * This B&S now inherits from the GlobalROMBuilderAndSolver, which in turn inherits 
 * from the ResidualBasedBlockBuilderAndSolver. The RHS is composed of unbalanced loads 
 * (residual) and is constructed using the ResidualBasedBlockBuilderAndSolver. Similarly, the 
 * LHS is constructed using the ResidualBasedBlockBuilderAndSolver and is then multiplied 
 * by the ROM RIGHT BASIS. This results in a rectangular system with dimensions of FOM size 
 * by ROM size. This system can be solved using either the normal equations or the QR 
 * decomposition. The degrees of freedom are rearranged so that the restrained ones are 
 * placed at the end of the system, ordered inversely to the DofSet, mirroring the arrangement 
 * in the FOM.
 * @tparam TSparseSpace Defines the sparse system under consideration
 * @tparam TDenseSpace Defines the dense system under consideration
 * @tparam TLinearSolver Specifies the linear solver being utilized
 * @author Sebastian Ares de Parga Regalado
 */

template <class TSparseSpace, class TDenseSpace, class TLinearSolver>
class LeastSquaresPetrovGalerkinROMBuilderAndSolver : public GlobalROMBuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>
{
public:

    ///@name Type Definitions
    ///@{

    // Class pointer definition
    KRATOS_CLASS_POINTER_DEFINITION(LeastSquaresPetrovGalerkinROMBuilderAndSolver);

    // The size_t types
    using SizeType = std::size_t;
    using IndexType = std::size_t;

    /// The definition of the current class
    using ClassType = LeastSquaresPetrovGalerkinROMBuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>;

    /// Definition of the classes from the base class
    using BaseBuilderAndSolverType = BuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>;
    using BaseType = GlobalROMBuilderAndSolver<TSparseSpace, TDenseSpace, TLinearSolver>;
    using TSchemeType = typename BaseType::TSchemeType;
    using TSystemMatrixType = typename BaseType::TSystemMatrixType;
    using TSystemVectorType = typename BaseType::TSystemVectorType;
    using ElementsArrayType = typename BaseType::ElementsArrayType;
    using ConditionsArrayType = typename BaseType::ConditionsArrayType;
    using LocalSystemVectorType = typename BaseBuilderAndSolverType::LocalSystemVectorType;
    using LocalSystemMatrixType = typename BaseBuilderAndSolverType::LocalSystemMatrixType;

    // Eigen definitions
    using EigenDynamicMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    using EigenDynamicVector = Eigen::Matrix<double, Eigen::Dynamic, 1>;
    using EigenSparseMatrix = Eigen::SparseMatrix<double, Eigen::RowMajor, int>;

    /// DoF types definition
    using NodeType = Node;


    ///@}
    ///@name Life cycle
    ///@{

    explicit LeastSquaresPetrovGalerkinROMBuilderAndSolver(
        typename TLinearSolver::Pointer pNewLinearSystemSolver,
        Parameters ThisParameters) : BaseType(pNewLinearSystemSolver) 
    {
        // Validate and assign defaults
        Parameters this_parameters_copy = ThisParameters.Clone();
        this_parameters_copy = BaseType::ValidateAndAssignParameters(this_parameters_copy, GetDefaultParameters());
        AssignSettings(this_parameters_copy);
    } 

    ~LeastSquaresPetrovGalerkinROMBuilderAndSolver() = default;

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    void SetUpDofSet(
        typename TSchemeType::Pointer pScheme,
        ModelPart &rModelPart) override
    {
        KRATOS_TRY;

        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 1)) << "Setting up the dofs" << std::endl;
        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 2)) << "Number of threads" << ParallelUtilities::GetNumThreads() << "\n" << std::endl;
        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 2)) << "Initializing element loop" << std::endl;

        // Get model part data
        if (this->mHromWeightsInitialized == false) {
            this->InitializeHROMWeights(rModelPart);
        }

        // Compute the complementary mesh for HROM
        if (this->mHromSimulation){
            FindNeighbouringElementsAndConditions(rModelPart);
        }

        auto dof_queue = this->ExtractDofSet(pScheme, rModelPart);
        
        // Fill a sorted auxiliary array of with the DOFs set
        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 2)) << "Initializing ordered array filling\n" << std::endl;
        auto dof_array = this->SortAndRemoveDuplicateDofs(dof_queue);

        // Update base builder and solver DOFs array and set corresponding flag
        BaseType::GetDofSet().swap(dof_array);
        BaseType::SetDofSetIsInitializedFlag(true);

        // Throw an exception if there are no DOFs involved in the analysis
        KRATOS_ERROR_IF(BaseType::GetDofSet().size() == 0) << "No degrees of freedom!" << std::endl;
        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 2)) << "Number of degrees of freedom:" << BaseType::GetDofSet().size() << std::endl;
        KRATOS_INFO_IF("GlobalLeastSquaresPetrovGalerkinROMBuilderAndSolver", (this->GetEchoLevel() > 2)) << "Finished setting up the dofs" << std::endl;

#ifdef KRATOS_DEBUG
        // If reactions are to be calculated, we check if all the dofs have reactions defined
        if (BaseType::GetCalculateReactionsFlag())
        {
            for (const auto& r_dof: BaseType::GetDofSet())
            {
                KRATOS_ERROR_IF_NOT(r_dof.HasReaction())
                    << "Reaction variable not set for the following :\n"
                    << "Node : " << r_dof.Id() << '\n'
                    << "Dof  : " << r_dof      << '\n'
                    << "Not possible to calculate reactions." << std::endl;
            }
        }
#endif
        KRATOS_CATCH("");
    } 

    /**
     * Builds and projects the reduced system of equations 
     */
    virtual void BuildAndProjectROM(
        typename TSchemeType::Pointer pScheme,
        ModelPart &rModelPart,
        TSystemMatrixType &rA,
        TSystemVectorType &rb,
        TSystemVectorType &rDx) override
    {
        KRATOS_TRY

        KRATOS_ERROR_IF(!pScheme) << "No scheme provided!" << std::endl;

        const auto assembling_timer = BuiltinTimer();

        BuildAndApplyDirichletConditions(pScheme, rModelPart, rA, rb, rDx);

        if (BaseType::GetMonotonicityPreservingFlag()) {
            BaseType::MonotonicityPreserving(rA, rb);
        }

        ProjectROM(rModelPart, rA, rb);

        double time = assembling_timer.ElapsedSeconds();
        KRATOS_INFO_IF("LeastSquaresPetrovGalerkinROMBuilderAndSolver", (BaseType::GetEchoLevel() > 0)) << "Build and project time: " << time << std::endl;

        KRATOS_CATCH("")
    }

    void BuildAndApplyDirichletConditions(
        typename TSchemeType::Pointer pScheme,
        ModelPart &rModelPart,
        TSystemMatrixType &rA,
        TSystemVectorType &rb,
        TSystemVectorType &rDx
    ){
        if (rA.size1() != BaseBuilderAndSolverType::mEquationSystemSize || rA.size2() != BaseBuilderAndSolverType::mEquationSystemSize) {
            rA.resize(BaseBuilderAndSolverType::mEquationSystemSize, BaseBuilderAndSolverType::mEquationSystemSize, false);
            BaseType::ConstructMatrixStructure(pScheme, rA, rModelPart);
        }

        BaseType::Build(pScheme, rModelPart, rA, rb);

        BaseType::ApplyDirichletConditions(pScheme, rModelPart, rA, rDx, rb);
    }

    void GetRightROMBasis(
        const ModelPart& rModelPart,
        Matrix& rPhiGlobal
    )
    {
        BaseType::BuildRightROMBasis(rModelPart, rPhiGlobal);
    }

    /**
     * Projects the reduced system of equations 
     */
    virtual void ProjectROM(
        ModelPart &rModelPart,
        TSystemMatrixType &rA,
        TSystemVectorType &rb) override
    {
        KRATOS_TRY

        if (mRightRomBasisInitialized==false){
            mPhiGlobal = ZeroMatrix(BaseBuilderAndSolverType::mEquationSystemSize, BaseType::GetNumberOfROMModes());
            mRightRomBasisInitialized = true;
        }

        BaseType::BuildRightROMBasis(rModelPart, mPhiGlobal);
        auto a_wrapper = UblasWrapper<double>(rA);
        const auto& eigen_rA = a_wrapper.matrix();
        Eigen::Map<EigenDynamicVector> eigen_rb(rb.data().begin(), rb.size());
        Eigen::Map<EigenDynamicMatrix> eigen_mPhiGlobal(mPhiGlobal.data().begin(), mPhiGlobal.size1(), mPhiGlobal.size2());

        EigenDynamicMatrix eigen_rA_times_mPhiGlobal = eigen_rA * eigen_mPhiGlobal; //TODO: Make it in parallel.

        if (mSolvingTechnique == "normal_equations"){
            // Compute the matrix multiplication
            mEigenRomA = eigen_rA_times_mPhiGlobal.transpose() * eigen_rA_times_mPhiGlobal; //TODO: Make it in parallel.
            mEigenRomB = eigen_rA_times_mPhiGlobal.transpose() * eigen_rb; //TODO: Make it in parallel.
        }
        else if (mSolvingTechnique == "qr_decomposition") {
            mEigenRomA = eigen_rA_times_mPhiGlobal; 
            mEigenRomB = eigen_rb;
        }

        KRATOS_CATCH("")
    }

    void BuildAndSolve(
        typename TSchemeType::Pointer pScheme,
        ModelPart &rModelPart,
        TSystemMatrixType &A,
        TSystemVectorType &Dx,
        TSystemVectorType &b) override
    {
        KRATOS_TRY

        BuildAndProjectROM(pScheme, rModelPart, A, b, Dx);

        // Obtain the assembled residuals vector (To build a basis for Petrov-Galerkin)
        if (mTrainPetrovGalerkinFlag & mBasisStrategy=="residuals"){
            std::stringstream matrix_market_vector_name;
            matrix_market_vector_name << "R_" << rModelPart.GetProcessInfo()[TIME] << "_" << rModelPart.GetProcessInfo()[NL_ITERATION_NUMBER] <<  ".res.mm";
            SparseSpaceType::WriteMatrixMarketVector((matrix_market_vector_name.str()).c_str(), b);
        }
        
        if (mSolvingTechnique == "normal_equations"){
            BaseType::SolveROM(rModelPart, mEigenRomA, mEigenRomB, Dx);
        }
        else if (mSolvingTechnique == "qr_decomposition"){
            BaseType::SolveROM(rModelPart, mEigenRomA, mEigenRomB, Dx);
        }

        KRATOS_CATCH("")
    }

    Parameters GetDefaultParameters() const override
    {
        Parameters default_parameters = Parameters(R"(
        {
            "name" : "lspg_rom_builder_and_solver",
            "nodal_unknowns" : [],
            "number_of_rom_dofs" : 10,
            "rom_bns_settings": {
                "train_petrov_galerkin" : false,
                "solving_technique" : "normal_equations",
                "basis_strategy" : "residuals",
                "monotonicity_preserving" : false
            }
        })");
        default_parameters.AddMissingParameters(BaseType::GetDefaultParameters());

        return default_parameters;
    }

    static std::string Name() 
    {
        return "lspg_rom_builder_and_solver";
    }

    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const override
    {
        return "LeastSquaresPetrovGalerkinROMBuilderAndSolver";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream &rOStream) const override
    {
        rOStream << Info();
    }

    /// Print object's 
    virtual void PrintData(std::ostream &rOStream) const override
    {
        rOStream << Info();
    }

    ///@}
    ///@name Friends
    ///@{

    ///@}
protected:

    ///@}
    ///@name Protected static member variables
    ///@{


    ///@}
    ///@name Protected member variables
    ///@{
    ElementsArrayType mNeighbouringAndSelectedElements;
    ConditionsArrayType mNeighbouringAndSelectedConditions;

    ///@}
    ///@name Protected operators
    ///@{


    ///@}
    ///@name Protected operations
    ///@{

    void AssignSettings(const Parameters ThisParameters) override
    {
        BaseType::AssignSettings(ThisParameters);

        // // Set member variables
        mTrainPetrovGalerkinFlag = ThisParameters["rom_bns_settings"]["train_petrov_galerkin"].GetBool();
        mBasisStrategy = ThisParameters["rom_bns_settings"]["basis_strategy"].GetString();
        mSolvingTechnique = ThisParameters["rom_bns_settings"]["solving_technique"].GetString();
    }

    /**
     * @brief Function to perform the build of the LHS and RHS on the selected elements and the corresponding complementary mesh. 
     * @param pScheme The integration scheme considered
     * @param rModelPart The model part of the problem to solve
     * @param A The LHS matrix
     * @param b The RHS vector
     */
    void BuildWithComplementaryMesh(
        typename TSchemeType::Pointer pScheme,
        ModelPart& rModelPart,
        TSystemMatrixType& A,
        TSystemVectorType& b)
    {
        KRATOS_TRY

        KRATOS_ERROR_IF(!pScheme) << "No scheme provided!" << std::endl;

        // // Getting the neighbouring (complementary mesh) and selected elements from the model
        auto& r_neighboring_and_selected_elems = mNeighbouringAndSelectedElements;

        const int nelements = r_neighboring_and_selected_elems.size();

        // // Getting the neighbouring (complementary mesh) and selected conditions from the model
        auto& r_neighboring_and_selected_conditions = mNeighbouringAndSelectedConditions;

        const int nconditions = r_neighboring_and_selected_conditions.size();

        const ProcessInfo& CurrentProcessInfo = rModelPart.GetProcessInfo();
        ModelPart::ElementsContainerType::iterator el_begin = mNeighbouringAndSelectedElements.begin();
        ModelPart::ConditionsContainerType::iterator cond_begin = mNeighbouringAndSelectedConditions.begin();

        //contributions to the system
        LocalSystemMatrixType LHS_Contribution = LocalSystemMatrixType(0, 0);
        LocalSystemVectorType RHS_Contribution = LocalSystemVectorType(0);

        //vector containing the localization in the system of the different terms
        Element::EquationIdVectorType EquationId;

        // Assemble all elements
        const auto timer = BuiltinTimer();

        #pragma omp parallel firstprivate(nelements,nconditions, LHS_Contribution, RHS_Contribution, EquationId )
        {
            # pragma omp for  schedule(guided, 512) nowait
            for (int k = 0; k < nelements; k++) {
                auto it_elem = el_begin + k;

                if (it_elem->IsActive()) {
                    // Calculate elemental contribution
                    pScheme->CalculateSystemContributions(*it_elem, LHS_Contribution, RHS_Contribution, EquationId, CurrentProcessInfo);

                    // Assemble the elemental contribution
                    BaseType::Assemble(A, b, LHS_Contribution, RHS_Contribution, EquationId);
                }

            }

            #pragma omp for  schedule(guided, 512)
            for (int k = 0; k < nconditions; k++) {
                auto it_cond = cond_begin + k;

                if (it_cond->IsActive()) {
                    // Calculate elemental contribution
                    pScheme->CalculateSystemContributions(*it_cond, LHS_Contribution, RHS_Contribution, EquationId, CurrentProcessInfo);

                    // Assemble the elemental contribution
                    BaseType::Assemble(A, b, LHS_Contribution, RHS_Contribution, EquationId);
                }
            }
        }

        KRATOS_INFO_IF("LeastSquaresPetrovGalerkinROMResidualBasedBlockBuilderAndSolver", this->GetEchoLevel() >= 1) << "Build time: " << timer.ElapsedSeconds() << std::endl;

        KRATOS_INFO_IF("LeastSquaresPetrovGalerkinROMResidualBasedBlockBuilderAndSolver", (this->GetEchoLevel() > 2 && rModelPart.GetCommunicator().MyPID() == 0)) << "Finished parallel building" << std::endl;

        KRATOS_CATCH("")
    }

    void FindNeighbouringElementsAndConditions(ModelPart& rModelPart) 
    {
        mNeighbouringAndSelectedElements.clear();
        mNeighbouringAndSelectedConditions.clear();

        // Create sets for storing Ids
        std::set<int> selected_element_ids;
        std::set<int> selected_condition_ids;

        FindGlobalNodalEntityNeighboursProcess<ModelPart::ElementsContainerType> find_nodal_elements_neighbours_process(rModelPart);
        find_nodal_elements_neighbours_process.Execute();
        FindGlobalNodalEntityNeighboursProcess<ModelPart::ConditionsContainerType> find_nodal_conditions_neighbours_process(rModelPart);
        find_nodal_conditions_neighbours_process.Execute();

        for (auto it_elem = this->mSelectedElements.ptr_begin(); it_elem != this->mSelectedElements.ptr_end(); ++it_elem) {
            mNeighbouringAndSelectedElements.push_back(*it_elem);
            selected_element_ids.insert((*it_elem)->Id());

            const auto& r_geom = (*it_elem)->GetGeometry();
            const SizeType n_nodes = r_geom.PointsNumber();
            for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
                NodeType::Pointer p_node = r_geom(i_node);
                auto& neighbour_elements = p_node->GetValue(NEIGHBOUR_ELEMENTS);
                for (auto& neighbor_elem : neighbour_elements.GetContainer()) {
                    if(selected_element_ids.find(neighbor_elem->Id()) == selected_element_ids.end()) {
                        mNeighbouringAndSelectedElements.push_back(&*neighbor_elem);
                        selected_element_ids.insert(neighbor_elem->Id());
                    }
                }

                // Check and add neighbouring conditions
                auto& neighbour_conditions = p_node->GetValue(NEIGHBOUR_CONDITIONS);
                for (auto& neighbor_cond : neighbour_conditions.GetContainer()) {
                    if(selected_condition_ids.find(neighbor_cond->Id()) == selected_condition_ids.end()) {
                        mNeighbouringAndSelectedConditions.push_back(&*neighbor_cond);
                        selected_condition_ids.insert(neighbor_cond->Id());
                    }
                }
            }
        }

        for (auto it_cond = this->mSelectedConditions.ptr_begin(); it_cond != this->mSelectedConditions.ptr_end(); ++it_cond) {
            mNeighbouringAndSelectedConditions.push_back(*it_cond);
            selected_condition_ids.insert((*it_cond)->Id());

            const auto& r_geom = (*it_cond)->GetGeometry();
            const SizeType n_nodes = r_geom.PointsNumber();
            for (IndexType i_node = 0; i_node < n_nodes; ++i_node) {
                NodeType::Pointer p_node = r_geom(i_node);
                auto& neighbour_conditions = p_node->GetValue(NEIGHBOUR_CONDITIONS);
                for (auto& neighbor_cond : neighbour_conditions.GetContainer()) {
                    if(selected_condition_ids.find(neighbor_cond->Id()) == selected_condition_ids.end()) {
                        mNeighbouringAndSelectedConditions.push_back(&*neighbor_cond);
                        selected_condition_ids.insert(neighbor_cond->Id());
                    }
                }

                // Check and add neighbouring elements
                auto& neighbour_elements = p_node->GetValue(NEIGHBOUR_ELEMENTS);
                for (auto& neighbor_elem : neighbour_elements.GetContainer()) {
                    if(selected_element_ids.find(neighbor_elem->Id()) == selected_element_ids.end()) {
                        mNeighbouringAndSelectedElements.push_back(&*neighbor_elem);
                        selected_element_ids.insert(neighbor_elem->Id());
                    }
                }
            }
        }

        std::unordered_map<int, Element::Pointer> unique_elements_map;
        for (const auto& elem : mNeighbouringAndSelectedElements) {
            unique_elements_map[elem.Id()] = intrusive_ptr<Element>(const_cast<Element*>(&elem));
        }
        mNeighbouringAndSelectedElements.clear();
        for (const auto& pair : unique_elements_map) {
            mNeighbouringAndSelectedElements.push_back(pair.second);
        }

        std::unordered_map<int, Condition::Pointer> unique_conditions_map;
        for (const auto& cond : mNeighbouringAndSelectedConditions) {
            unique_conditions_map[cond.Id()] = intrusive_ptr<Condition>(const_cast<Condition*>(&cond));
        }
        mNeighbouringAndSelectedConditions.clear();
        for (const auto& pair : unique_conditions_map) {
            mNeighbouringAndSelectedConditions.push_back(pair.second);
        }
    }

    ///@}
    ///@name Protected access
    ///@{


    ///@}
    ///@name Protected inquiry
    ///@{


    ///@}
    ///@name Protected life cycle
    ///@{
    
private:
    bool mTrainPetrovGalerkinFlag = false;
    std::string mBasisStrategy;
    std::string mSolvingTechnique;
    EigenDynamicMatrix mEigenRomA;
    EigenDynamicVector mEigenRomB;
    Matrix mPhiGlobal;
    bool mRightRomBasisInitialized = false;

    ///@}
    ///@name Private operations 
    ///@{

    ///@}
}; /* Class LeastSquaresPetrovGalerkinROMBuilderAndSolver */

///@}
///@name Type Definitions
///@{


///@}

} /* namespace Kratos.*/

