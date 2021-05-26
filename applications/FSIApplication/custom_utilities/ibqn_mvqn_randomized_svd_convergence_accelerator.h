//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main author:     Ruben Zorrilla
//

#if !defined(KRATOS_IBQN_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR)
#define  KRATOS_IBQN_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR

// System includes

// External includes

// Project includes
#include "utilities/dense_svd_decomposition.h"

// Application includes
#include "ibqn_mvqn_convergence_accelerator.h"

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

/// Forward declaration of MVQN with ranzomized SVD Jacobian convergence accelerator
/// This is required since the include ward avoids the inclusion of the MVQNRandomizedSVDConvergenceAccelerator
template<class TSparseSpace, class TDenseSpace>
class MVQNRandomizedSVDConvergenceAccelerator;

/** @brief Interface Block Newton MVQN with randomized Jacobian convergence accelerator
 * Interface Block Newton equations convergence accelerator with MVQN randomized SVD Jacobian for the subdomains
 * @tparam TSparseSpace Linear algebra sparse space
 * @tparam TDenseSpace Linear algebra dense space
 */
template<class TSparseSpace, class TDenseSpace>
class IBQNMVQNRandomizedSVDConvergenceAccelerator: public IBQNMVQNConvergenceAccelerator<TSparseSpace, TDenseSpace>
{
public:

    ///@name Type Definitions
    ///@{
    KRATOS_CLASS_POINTER_DEFINITION( IBQNMVQNRandomizedSVDConvergenceAccelerator );

    typedef IBQNMVQNConvergenceAccelerator<TSparseSpace, TDenseSpace>      BaseType;
    typedef typename BaseType::Pointer                              BaseTypePointer;

    typedef typename BaseType::DenseVectorType                           VectorType;
    typedef typename BaseType::DenseVectorPointerType             VectorPointerType;

    typedef typename BaseType::DenseMatrixType                           MatrixType;
    typedef typename BaseType::DenseMatrixPointerType             MatrixPointerType;

    typedef MVQNRandomizedSVDConvergenceAccelerator<TSparseSpace, TDenseSpace>        MVQNType;
    typedef typename BaseType::MVQNPointerType                                 MVQNPointerType;

    typedef typename DenseSingularValueDecomposition<TDenseSpace>::Pointer DenseSVDPointerType;

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * @brief Construct a new IBQNMVQNRandomizedSVDConvergenceAccelerator object
     * IBQN-MVQN with randomized SVD Jacobian convergence accelerator Json settings constructor
     * @param pDenseSVD Pointer to the dense SVD utility instance
     * @param rConvAcceleratorParameters Json string encapsulating the settings
     */
    explicit IBQNMVQNRandomizedSVDConvergenceAccelerator(
        DenseSVDPointerType pDenseSVD,
        Parameters ConvAcceleratorParameters)
    : BaseType()
    , mpSmallMatrixDenseSVD(pDenseSVD)
    {
        ConvAcceleratorParameters.ValidateAndAssignDefaults(GetDefaultParameters());
        BaseType::SetInitialRelaxationOmega(ConvAcceleratorParameters["w_0"].GetDouble());

        // Set the subdomains MVQN randomized SVD convergence accelerator pointers
        // Note that we call the simplified constructor with default zero relaxation omega and IBQN switch
        const double cut_off_tol = ConvAcceleratorParameters["cut_off_tol"].GetDouble();
        const unsigned int jacobian_modes = ConvAcceleratorParameters["jacobian_modes"].GetInt();
        const bool limit_modes_to_iterations = ConvAcceleratorParameters["limit_modes_to_iterations"].GetBool();
        MVQNPointerType p_convergence_accelerator_left = Kratos::make_unique<MVQNType>(pDenseSVD, jacobian_modes, cut_off_tol, limit_modes_to_iterations);
        MVQNPointerType p_convergence_accelerator_right = Kratos::make_unique<MVQNType>(pDenseSVD, jacobian_modes, cut_off_tol, limit_modes_to_iterations);
        BaseType::SetLeftConvergenceAcceleratorPointer(p_convergence_accelerator_left);
        BaseType::SetRightConvergenceAcceleratorPointer(p_convergence_accelerator_right);
    }

    /**
     * Copy Constructor.
     */
    IBQNMVQNRandomizedSVDConvergenceAccelerator(const IBQNMVQNRandomizedSVDConvergenceAccelerator& rOther) = delete;

    /**
     * Destructor.
     */
    virtual ~IBQNMVQNRandomizedSVDConvergenceAccelerator(){}

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    Parameters GetDefaultParameters() const override
    {
        Parameters ibqn_mvqn_default_parameters(R"({
            "solver_type"               : "IBQN_MVQN_randomized_SVD",
            "jacobian_modes"            : 10,
            "w_0"                       : 0.825,
            "cut_off_tol"               : 1e-8,
            "limit_modes_to_iterations" : true
        })");

        return ibqn_mvqn_default_parameters;
    }

    void SolveInterfaceBlockQuasiNewtonLeftUpdate(
        const VectorType& rDisplacementInputVector,
        const VectorType& rForceOutputVector,
        const VectorType& rIterationGuess,
        VectorType& rLeftCorrection) override
    {
        // Get the subdomains convergence accelerators
        auto p_conv_acc_left = BaseType::pGetConvergenceAcceleratorLeft();
        auto p_conv_acc_right = BaseType::pGetConvergenceAcceleratorRight();

        // Get both left and inverse Jacobian
        auto p_inv_jac_QU_left = p_conv_acc_left->pGetJacobianDecompositionMatixQU();
        auto p_inv_jac_SigmaV_left = p_conv_acc_left->pGetJacobianDecompositionMatixSigmaV();
        auto p_inv_jac_QU_right = p_conv_acc_right->pGetJacobianDecompositionMatixQU();
        auto p_inv_jac_SigmaV_right = p_conv_acc_right->pGetJacobianDecompositionMatixSigmaV();
        auto p_uncorrected_displacement_vector = BaseType::pGetUncorrectedDisplacementVector();

        //FIXME:
        KRATOS_WATCH(p_inv_jac_QU_left)
        KRATOS_WATCH(p_inv_jac_SigmaV_left)
        KRATOS_WATCH(p_inv_jac_QU_right)
        KRATOS_WATCH(p_inv_jac_SigmaV_right)

        if (p_inv_jac_QU_left == nullptr && p_inv_jac_SigmaV_left == nullptr) {
            p_inv_jac_QU_right = p_conv_acc_right->pGetOldJacobianDecompositionMatixQU();
            p_inv_jac_SigmaV_right = p_conv_acc_right->pGetOldJacobianDecompositionMatixSigmaV();
            p_inv_jac_QU_left = p_conv_acc_left->pGetOldJacobianDecompositionMatixQU();
            p_inv_jac_SigmaV_left = p_conv_acc_left->pGetOldJacobianDecompositionMatixSigmaV();
        }
        //FIXME:

        // Set the residual of the update problem to be solved
        VectorType aux_RHS = rForceOutputVector - rIterationGuess;
        if (p_inv_jac_QU_left != nullptr && p_inv_jac_SigmaV_left != nullptr) {
            const std::size_t n_dofs = TDenseSpace::Size(*p_uncorrected_displacement_vector);
            const std::size_t n_modes = TDenseSpace::Size1(*p_inv_jac_SigmaV_left);
            VectorType aux_vect(n_modes);
            VectorType aux_left_onto_right(n_dofs);
            VectorType displacement_iteration_update = *p_uncorrected_displacement_vector - rDisplacementInputVector;
            TSparseSpace::Mult(*p_inv_jac_SigmaV_left, displacement_iteration_update, aux_vect);
            TSparseSpace::Mult(*p_inv_jac_QU_left, aux_vect, aux_left_onto_right);
            TSparseSpace::UnaliasedAdd(aux_RHS, 1.0, aux_left_onto_right);

            if (p_inv_jac_QU_right != nullptr && p_inv_jac_SigmaV_right != nullptr) {

                MatrixType aux_C;
                const bool is_extended = CalculateAuxiliaryMatrixC(*p_inv_jac_SigmaV_left, *p_inv_jac_QU_right, aux_C);
                KRATOS_ERROR_IF(is_extended) << "Auxiliary matrix C is not expected to be extended in \'SolveInterfaceBlockQuasiNewtonLeftUpdate\'." << std::endl;

                // Calculate the correction with the Woodbury matrix identity
                ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_left, *p_inv_jac_SigmaV_right, aux_C, aux_RHS, rLeftCorrection);

            } else {
                rLeftCorrection = aux_RHS;
            }
        } else {
            //TODO: I THINK THIS CAN BE REMOVED
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            KRATOS_WATCH("NO POINTERS LEFT")
            rLeftCorrection = aux_RHS;
        }

        // // Set the residual of the update problem to be solved
        // VectorType aux_RHS = rForceOutputVector - rIterationGuess;
        // if (p_inv_jac_QU_left != nullptr && p_inv_jac_SigmaV_left != nullptr) {
        //     const std::size_t n_dofs = TDenseSpace::Size(*p_uncorrected_displacement_vector);
        //     const std::size_t n_modes = TDenseSpace::Size1(*p_inv_jac_SigmaV_left);
        //     VectorType aux_vect(n_modes);
        //     VectorType aux_left_onto_right(n_dofs);
        //     VectorType displacement_iteration_update = *p_uncorrected_displacement_vector - rDisplacementInputVector;
        //     TSparseSpace::Mult(*p_inv_jac_SigmaV_left, displacement_iteration_update, aux_vect);
        //     TSparseSpace::Mult(*p_inv_jac_QU_left, aux_vect, aux_left_onto_right);
        //     TSparseSpace::UnaliasedAdd(aux_RHS, 1.0, aux_left_onto_right);

        //     if (p_inv_jac_QU_right != nullptr && p_inv_jac_SigmaV_right != nullptr) {

        //         MatrixType aux_C;
        //         const bool is_extended = CalculateAuxiliaryMatrixC(*p_inv_jac_SigmaV_left, *p_inv_jac_QU_right, aux_C);
        //         KRATOS_ERROR_IF(is_extended) << "Auxiliary matrix C is not expected to be extended in \'SolveInterfaceBlockQuasiNewtonLeftUpdate\'." << std::endl;

        //         // Calculate the correction with the Woodbury matrix identity
        //         ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_left, *p_inv_jac_SigmaV_right, aux_C, aux_RHS, rLeftCorrection);

        //     } else {
        //         rLeftCorrection = aux_RHS;
        //     }
        // } else {
        //     //TODO: I THINK THIS CAN BE REMOVED
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     KRATOS_WATCH("NO POINTERS LEFT")
        //     rLeftCorrection = aux_RHS;
        // }
    }

    void SolveInterfaceBlockQuasiNewtonRightUpdate(
        const VectorType& rForceInputVector,
        const VectorType& rDisplacementOutputVector,
        const VectorType& rIterationGuess,
        VectorType& rRightCorrection) override
    {
        // Get the subdomains convergence accelerators
        auto p_conv_acc_left = BaseType::pGetConvergenceAcceleratorLeft();
        auto p_conv_acc_right = BaseType::pGetConvergenceAcceleratorRight();

        // Get both left and inverse Jacobian
        auto p_inv_jac_QU_left = p_conv_acc_left->pGetJacobianDecompositionMatixQU();
        auto p_inv_jac_SigmaV_left = p_conv_acc_left->pGetJacobianDecompositionMatixSigmaV();
        auto p_inv_jac_QU_right = p_conv_acc_right->pGetJacobianDecompositionMatixQU();
        auto p_inv_jac_SigmaV_right = p_conv_acc_right->pGetJacobianDecompositionMatixSigmaV();
        auto p_uncorrected_force_vector = BaseType::pGetUncorrectedForceVector();

        // If the decomposition has not been done yet (1st nonlinear iteration) use the previous step Jacobian decompositions
        if (p_inv_jac_QU_right == nullptr && p_inv_jac_SigmaV_right == nullptr) {
            p_inv_jac_QU_right = p_conv_acc_right->pGetOldJacobianDecompositionMatixQU();
            p_inv_jac_SigmaV_right = p_conv_acc_right->pGetOldJacobianDecompositionMatixSigmaV();
            p_inv_jac_QU_left = p_conv_acc_left->pGetOldJacobianDecompositionMatixQU();
            p_inv_jac_SigmaV_left = p_conv_acc_left->pGetOldJacobianDecompositionMatixSigmaV();
        }

        // Set the residual of the update problem to be solved
        if (p_inv_jac_QU_right != nullptr && p_inv_jac_SigmaV_right != nullptr) {
            const std::size_t n_dofs = TDenseSpace::Size(*p_uncorrected_force_vector);
            const std::size_t n_modes_right = TDenseSpace::Size1(*p_inv_jac_SigmaV_right);
            VectorType aux_vect(n_modes_right);
            VectorType aux_right_onto_left(n_dofs);
            VectorType force_iteration_update = *p_uncorrected_force_vector - rForceInputVector;
            TSparseSpace::Mult(*p_inv_jac_SigmaV_right, force_iteration_update, aux_vect);
            TSparseSpace::Mult(*p_inv_jac_QU_right, aux_vect, aux_right_onto_left);
            VectorType aux_RHS = rDisplacementOutputVector - rIterationGuess;
            TSparseSpace::UnaliasedAdd(aux_RHS, 1.0, aux_right_onto_left);

            // Calculate auxiliary intermediate small matrices
            if (p_inv_jac_QU_left != nullptr && p_inv_jac_SigmaV_left != nullptr) {

                MatrixType aux_C;
                const bool is_extended = CalculateAuxiliaryMatrixC(*p_inv_jac_SigmaV_right, *p_inv_jac_QU_left, aux_C);

                if (is_extended) {
                    const std::size_t n_modes_left = TDenseSpace::Size1(*p_inv_jac_SigmaV_left);
                    if (n_modes_right == n_modes_left + 1) {
                        // Extend the decomposition matrix from the left side as this is smaller since no force obseration is done yet
                        MatrixType extended_inv_jac_SigmaV_left = ZeroMatrix(n_modes_left + 1, n_dofs);
                        IndexPartition<std::size_t>(n_dofs).for_each(
                            [&extended_inv_jac_SigmaV_left, &p_inv_jac_SigmaV_left, &n_modes_left](std::size_t Col){
                                const auto& r_inv_jac_SigmaV_left = *p_inv_jac_SigmaV_left;
                                for (std::size_t row = 0; row < n_modes_left; ++row)
                                extended_inv_jac_SigmaV_left(row, Col) = r_inv_jac_SigmaV_left(row, Col);
                            }
                        );

                        // Calculate the correction with the Woodbury matrix identity
                        ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_right, extended_inv_jac_SigmaV_left, aux_C, aux_RHS, rRightCorrection);

                    } else if (n_modes_right < n_modes_left && n_modes_right == 1) {
                        // Extend the decomposition matrix from the left side as this is smaller since no force obseration is done yet
                        MatrixType extended_inv_jac_QU_right = ZeroMatrix(n_dofs, n_modes_left);

                        IndexPartition<std::size_t>(n_dofs).for_each(
                            [&extended_inv_jac_QU_right, &p_inv_jac_QU_right, &n_modes_left](std::size_t Rol) {
                                const auto &r_inv_jac_QU_right = *p_inv_jac_QU_right;
                                for (std::size_t col = 0; col < n_modes_left; ++col)
                                    extended_inv_jac_QU_right(Row, col) = r_inv_jac_QU_right(Rol, col);
                            });

                        // Calculate the correction with the Woodbury matrix identity
                        ApplyWoodburyMatrixIdentity(extended_inv_jac_QU_right, *p_inv_jac_SigmaV_left, aux_C, aux_RHS, rRightCorrection);

                    }
                } else {
                    //FIXME: I THINK WE SHOULD THROW AN ERROR IN HERE -> WE ALWAYS NEED TO EXTEND IN RIGHT
                    // Calculate the correction with the Woodbury matrix identity
                    // Note that in here no extension of the left side matrix is done. This happens when the previous step SVD is used (first iteration)
                    ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_right, *p_inv_jac_SigmaV_left, aux_C, aux_RHS, rRightCorrection);
                }
            } else {
                rRightCorrection = aux_RHS;
            }
        } else {
            KRATOS_ERROR << "Calling \'SolveInterfaceBlockQuasiNewtonRightUpdate\' with no Jacobian decomposition available." << std::endl;
        }
        // // Set the residual of the update problem to be solved
        // VectorType aux_RHS = rDisplacementOutputVector - rIterationGuess;
        // if (p_inv_jac_QU_right != nullptr && p_inv_jac_SigmaV_right != nullptr) {
        //     const std::size_t n_dofs = TDenseSpace::Size(*p_uncorrected_force_vector);
        //     const std::size_t n_modes_right = TDenseSpace::Size1(*p_inv_jac_SigmaV_right);
        //     VectorType aux_vect(n_modes_right);
        //     VectorType aux_right_onto_left(n_dofs);
        //     VectorType force_iteration_update = *p_uncorrected_force_vector - rForceInputVector;
        //     TSparseSpace::Mult(*p_inv_jac_SigmaV_right, force_iteration_update, aux_vect);
        //     TSparseSpace::Mult(*p_inv_jac_QU_right, aux_vect, aux_right_onto_left);
        //     TSparseSpace::UnaliasedAdd(aux_RHS, 1.0, aux_right_onto_left);

        //     // Calculate auxiliary intermediate small matrices
        //     if (p_inv_jac_QU_left != nullptr && p_inv_jac_SigmaV_left != nullptr) {

        //         MatrixType aux_C;
        //         const bool is_extended = CalculateAuxiliaryMatrixC(*p_inv_jac_SigmaV_right, *p_inv_jac_QU_left, aux_C);

        //         if (is_extended) {
        //             // Extend the decomposition matrix from the left side as this is smaller since no force obseration is done yet
        //             const std::size_t n_modes_left = TDenseSpace::Size1(*p_inv_jac_SigmaV_left);
        //             MatrixType extended_inv_jac_SigmaV_left = ZeroMatrix(n_modes_left + 1, n_dofs);
        //             IndexPartition<std::size_t>(n_dofs).for_each(
        //                 [&extended_inv_jac_SigmaV_left, &p_inv_jac_SigmaV_left, &n_modes_left](std::size_t Col){
        //                     const auto& r_inv_jac_SigmaV_left = *p_inv_jac_SigmaV_left;
        //                     for (std::size_t row = 0; row < n_modes_left; ++row)
        //                     extended_inv_jac_SigmaV_left(row, Col) = r_inv_jac_SigmaV_left(row, Col);
        //                 }
        //             );

        //             // Calculate the correction with the Woodbury matrix identity
        //             ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_right, extended_inv_jac_SigmaV_left, aux_C, aux_RHS, rRightCorrection);
        //         } else {
        //             // Calculate the correction with the Woodbury matrix identity
        //             // Note that in here no extension of the left side matrix is done. This happens when the previous step SVD is used (first iteration)
        //             ApplyWoodburyMatrixIdentity(*p_inv_jac_QU_right, *p_inv_jac_SigmaV_left, aux_C, aux_RHS, rRightCorrection);
        //         }

        //     } else {
        //         rRightCorrection = aux_RHS;
        //     }
        // } else {
        //     //TODO: I THINK THIS CAN BE REMOVED
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     KRATOS_WATCH("NO POINTERS RIGHT")
        //     rRightCorrection = aux_RHS;
        // }
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


    ///@}
    ///@name Friends
    ///@{


    ///@}
private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{

    DenseSVDPointerType mpSmallMatrixDenseSVD = nullptr;

    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{

    /**
     * @brief Calculates the auxiliary matrix C
     * This method calculates the auxiliary matrix C required for the application of the Woodbury matrix identity
     * Note that if the resultant matrix is not squared, it adds a zero columns with ones in the diagonal
     * @param rA Horizontal-shaped input matrix
     * @param rB Vertical-shaped input matrix
     * @param rC Squared output matrix
     * @return true if the matrix has been extended
     * @return false if the matrix has not been extended
     */
    bool CalculateAuxiliaryMatrixC(
        MatrixType& rA,
        MatrixType& rB,
        MatrixType& rC)
    {
        const std::size_t n_dofs = TDenseSpace::Size2(rA);
        const std::size_t n_row_A = TDenseSpace::Size1(rA);
        const std::size_t n_col_B = TDenseSpace::Size2(rB);

        auto matrix_A_B_product = [&rC, &rA, &rB, &n_row_A](std::size_t Col, VectorType& rAuxColumnTLS){
            TDenseSpace::GetColumn(Col, rB, rAuxColumnTLS);
            for (std::size_t row = 0; row < n_row_A; ++row) {
                rC(row,Col) = TDenseSpace::RowDot(row, rA, rAuxColumnTLS);
            }
        };

        bool is_extended = false;
        if (n_row_A != n_col_B) {
            if (n_row_A == n_col_B + 1) {
                // This case always happens when updating displacements as no force observation has been done yet
                is_extended = true;
                rC = ZeroMatrix(n_row_A, n_col_B +1);
                IndexPartition<std::size_t>(n_col_B).for_each(VectorType(n_dofs), matrix_A_B_product);
                rC(n_row_A - 1, n_col_B) = 1.0;
            } else if (n_row_A < n_col_B && n_row_A == 1) {
                // This case happens in the second displacement update since we have displacement observation but the previous force Jacobian is used
                is_extended = true;
                rC = ZeroMatrix(n_col_B, n_col_B);
                IndexPartition<std::size_t>(n_col_B).for_each(VectorType(n_dofs), matrix_A_B_product);
                for (std::size_t i_row = 1; i_row < n_col_B; ++i_row) {
                    rC(i_row, i_row) = 1.0;
                }
            } else {
                KRATOS_ERROR << "Auxiliary matrix C for Woodbury matrix identity cannot be computed." << std::endl;
            }
        } else {
            rC = ZeroMatrix(n_row_A, n_col_B);
            IndexPartition<std::size_t>(n_col_B).for_each(VectorType(n_dofs), matrix_A_B_product);
        }

        return is_extended;
    }

    /**
     * @brief Apply the Woodbury matrix identity to solve the update
     * This method applies the Woodbury matrix identity to solve the problem (I - ACB) * x = y
     */
    void ApplyWoodburyMatrixIdentity(
        MatrixType& rA,
        MatrixType& rB,
        const MatrixType& rC,
        VectorType& rY,
        VectorType& rX)
    {
        std::size_t n_row_C = TDenseSpace::Size1(rC);
        std::size_t n_col_C = TDenseSpace::Size2(rC);
        KRATOS_ERROR_IF(n_row_C != n_col_C) << "C matrix is not square. Woodbury matrix identity cannot be applied." << std::endl;

        TDenseSpace::Assign(rX, 1.0, rY);

        MatrixType aux_C_inv;
        CalculateMoorePenroseInverse(rC, aux_C_inv);

        const std::size_t n_dofs = TDenseSpace::Size(rY);
        const std::size_t n_col_A = TDenseSpace::Size2(rA);
        const std::size_t n_row_B = TDenseSpace::Size1(rB);

        MatrixType aux_D = ZeroMatrix(n_row_B, n_col_A);
        IndexPartition<std::size_t>(n_col_A).for_each(
            VectorType(n_dofs),
            [&aux_D, &rA, &rB, &aux_C_inv, &n_row_B](std::size_t Col, VectorType& rAuxColumnTLS)
        {
            TDenseSpace::GetColumn(Col, rA, rAuxColumnTLS);
            for (std::size_t row = 0; row < n_row_B; ++row) {
                aux_D(row,Col) = aux_C_inv(row,Col) - TDenseSpace::RowDot(row, rB, rAuxColumnTLS);
            }
        });

        MatrixType aux_D_inv;
        CalculateMoorePenroseInverse(aux_D, aux_D_inv);

        VectorType aux_vect(n_row_B);
        VectorType aux_vect_2(n_col_A);
        VectorType aux_vect_3(n_dofs);
        TDenseSpace::Mult(rB, rY, aux_vect);
        TDenseSpace::Mult(aux_D_inv, aux_vect, aux_vect_2);
        TDenseSpace::Mult(rA, aux_vect_2, aux_vect_3);

        TDenseSpace::UnaliasedAdd(rX, 1.0, aux_vect_3);
    }

    void CalculateMoorePenroseInverse(
        const MatrixType& rInputMatrix,
        MatrixType& rMoorePenroseInverse)
    {
        IndexType aux_size_1 = TDenseSpace::Size1(rInputMatrix);
        IndexType aux_size_2 = TDenseSpace::Size2(rInputMatrix);

        // MatrixType u_svd; // Orthogonal matrix (m x m)
        // MatrixType w_svd; // Rectangular diagonal matrix (m x n)
        // MatrixType v_svd; // Orthogonal matrix (n x n)
        // std::string svd_type = "Jacobi"; // SVD decomposition type
        // const double svd_rel_tol = 1.0e-6; // Relative tolerance of the SVD decomposition (it will be multiplied by the input matrix norm)
        // SVDUtils<double>::SingularValueDecomposition(rInputMatrix, u_svd, w_svd, v_svd, svd_type, svd_rel_tol);

        VectorType s_svd; // Eigenvalues vector
        MatrixType u_svd; // Left orthogonal matrix
        MatrixType v_svd; // Right orthogonal matrix
        Parameters svd_settings(R"({
            "compute_thin_u" : true,
            "compute_thin_v" : true
        })");
        // MatrixType input_test;
        // mpSmallMatrixDenseSVD->Compute(input_test, s_svd, u_svd, v_svd, svd_settings);
        mpSmallMatrixDenseSVD->Compute(const_cast<MatrixType&>(rInputMatrix), s_svd, u_svd, v_svd, svd_settings);
        const std::size_t n_eigs = s_svd.size();
        MatrixType s_inv = ZeroMatrix(n_eigs, n_eigs);
        for (std::size_t i = 0; i < n_eigs; ++i) {
            s_inv(i,i) = 1.0 / s_svd(i);
        }

        // Calculate and save the input matrix pseudo-inverse
        rMoorePenroseInverse = ZeroMatrix(aux_size_2, aux_size_1);

        // // Note that we take advantage of the fact that the input matrix is always square
        // for (std::size_t i = 0; i < aux_size; ++i) {
        //     for (std::size_t j = 0; j < aux_size; ++j) {
        //         const double aux = v_svd(j,i) / w_svd(j,j);
        //         for (std::size_t k = 0; k < aux_size; ++k) {
        //             rMoorePenroseInverse(i,k) += aux * u_svd(k,j);
        //         }
        //     }
        // }
        for (std::size_t i = 0; i < aux_size_2; ++i) {
            for (std::size_t j = 0; j < aux_size_1; ++j) {
                double& r_value = rMoorePenroseInverse(i,j);
                for (std::size_t k = 0; k < n_eigs; ++k) {
                    const double v_ik = v_svd(i,k);
                    for (std::size_t m = 0; m < n_eigs; ++m) {
                        const double ut_mj = u_svd(j,m);
                        const double s_inv_km = s_inv(k,m);
                        r_value += v_ik * s_inv_km * ut_mj;
                    }
                }
            }
        }
    }

    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Serialization
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{


    ///@}
}; /* Class IBQNMVQNRandomizedSVDConvergenceAccelerator */

///@}
///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


///@}
}  /* namespace Kratos.*/

#endif /* KRATOS_IBQN_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR defined */
