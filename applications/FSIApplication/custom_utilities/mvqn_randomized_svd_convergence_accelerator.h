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

#if !defined(KRATOS_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR)
#define  KRATOS_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR

// System includes
#include <random>

// External includes

// Project includes
#include "solving_strategies/convergence_accelerators/convergence_accelerator.h"
#include "utilities/dense_svd_decomposition.h"
#include "utilities/parallel_utilities.h"
#include "utilities/qr_utility.h"
#include "utilities/svd_utils.h"

// Application includes
#include "mvqn_convergence_accelerator.hpp"
#include "ibqn_mvqn_randomized_svd_convergence_accelerator.h"

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

/** @brief MVQN acceleration scheme
 * MultiVectorQuasiNewton convergence accelerator from Bogaers et al. 2016
 * @tparam TSparseSpace Linear algebra sparse space
 * @tparam TDenseSpace Linear algebra dense space
 */
template<class TSparseSpace, class TDenseSpace>
class MVQNRandomizedSVDConvergenceAccelerator: public MVQNFullJacobianConvergenceAccelerator<TSparseSpace, TDenseSpace>
{
public:

    ///@name Type Definitions
    ///@{
    KRATOS_CLASS_POINTER_DEFINITION( MVQNRandomizedSVDConvergenceAccelerator );

    typedef std::size_t SizeType;

    typedef MVQNFullJacobianConvergenceAccelerator<TSparseSpace, TDenseSpace> BaseType;
    typedef typename BaseType::Pointer BaseTypePointer;

    typedef typename BaseType::DenseVectorType VectorType;
    typedef typename BaseType::DenseVectorPointerType VectorPointerType;

    typedef typename BaseType::DenseMatrixType MatrixType;
    typedef typename BaseType::DenseMatrixPointerType MatrixPointerType;

    typedef typename DenseSingularValueDecomposition<TDenseSpace>::Pointer DenseSVDPointerType;

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * @brief Construct a new MVQNRandomizedSVDConvergenceAccelerator object
     * MultiVector Quasi-Newton convergence accelerator with randomized SVD Jacobian json settings constructor
     * @param pDenseSVD Pointer to the dense SVD utility instance
     * @param ConvAcceleratorParameters Json string encapsulating the settings
     */
    explicit MVQNRandomizedSVDConvergenceAccelerator(
        DenseSVDPointerType pDenseSVD,
        Parameters ConvAcceleratorParameters)
    : BaseType()
    , mpDenseSVD(pDenseSVD)
    {
        ConvAcceleratorParameters.ValidateAndAssignDefaults(GetDefaultParameters());
        mUserNumberOfModes = ConvAcceleratorParameters["jacobian_modes"].GetInt();
        mLimitModesToIterations = ConvAcceleratorParameters["limit_modes_to_iterations"].GetBool();
        BaseType::SetInitialRelaxationOmega(ConvAcceleratorParameters["w_0"].GetDouble());
        BaseType::SetCutOffTolerance(ConvAcceleratorParameters["cut_off_tol"].GetDouble());
    }

    /**
     * @brief Construct a new MVQNRandomizedSVDConvergenceAccelerator object
     * Multivector Quasi-Newton convergence accelerator with randomized SVD jacobian constructor
     * This constructor is intended to be used to set the subdomain MVQN pointers with the IBQN equations
     * @param pDenseSVD Pointer to the dense SVD utility instance
     * @param JacobianModes Number of modes to be kept in the randomization of the Jacobian
     * @param CutOffTolerance Tolerance for the new observation data cut off (relative to the maximum residual observation matrix eigenvalue)
     */
    explicit MVQNRandomizedSVDConvergenceAccelerator(
        DenseSVDPointerType pDenseSVD,
        const unsigned int JacobianModes,
        const double CutOffTolerance,
        const bool LimitModesToIterations)
    : BaseType(CutOffTolerance)
    , mUserNumberOfModes(JacobianModes)
    , mLimitModesToIterations(LimitModesToIterations)
    , mpDenseSVD(pDenseSVD)
    {
    }

    /**
     * Copy Constructor.
     */

    // Required to build the IBQN-MVQN
    MVQNRandomizedSVDConvergenceAccelerator(const MVQNRandomizedSVDConvergenceAccelerator& rOther);

    /**
     * Destructor.
     */
    virtual ~MVQNRandomizedSVDConvergenceAccelerator(){}

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{

    void FinalizeSolutionStep() override
    {
        KRATOS_TRY;

        // If required, calculate the SVD of the inverse Jacobian approximation
        if (!BaseType::IsUsedInBlockNewtonEquations()) {
            CalculateInverseJacobianSVD();
        }

        // Call the base class FinalizeSolutionStep
        // Note that this clears the observation matrices so it needs to be called after the inverse Jacobian SVD
        BaseType::FinalizeSolutionStep();

        // Save the current (last) inverse Jacobian decomposition for the next step
        mpOldJacQU = mpJacQU;
        mpOldJacSigmaV = mpJacSigmaV;

        // Clear the current step Jacobian decomposition pointers
        mpJacQU = nullptr;
        mpJacSigmaV = nullptr;

        KRATOS_CATCH( "" );
    }

    Parameters GetDefaultParameters() const override
    {
        Parameters mvqn_randomized_svd_default_parameters(R"({
            "solver_type"               : "MVQN_randomized_SVD",
            "jacobian_modes"            : 10,
            "w_0"                       : 0.825,
            "cut_off_tol"               : 1e-8,
            "interface_block_newton"    : false,
            "limit_modes_to_iterations" : false
        })");

        return mvqn_randomized_svd_default_parameters;
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

    friend class IBQNMVQNRandomizedSVDConvergenceAccelerator<TSparseSpace, TDenseSpace>;

    ///@}
protected:
    ///@name Protected  Operations
    ///@{

    void UpdateInverseJacobianApproximation(
        const VectorType& rResidualVector,
        const VectorType& rIterationGuess) override
    {
        // Append the current iteration information to the observation matrices
        // This method also checks the linear dependency of the new data to dismiss it
        BaseType::AppendCurrentIterationInformation(rResidualVector, rIterationGuess);

        // If the complete Jacobian is required for the IBQN equations calculate it
        // Note that this is only done after the first iteration since the observation matrices are empty at the first one
        if (BaseType::IsUsedInBlockNewtonEquations() && BaseType::GetConvergenceAcceleratorIteration() != 0) {
            CalculateInverseJacobianSVD();
        }
    }

    void CalculateCorrectionWithJacobian(VectorType& rCorrection) override
    {
        const SizeType n_dofs = BaseType::GetProblemSize();
        const auto p_V = BaseType::pGetResidualObservationMatrix();
        const auto p_W = BaseType::pGetSolutionObservationMatrix();
        auto& r_res_vect = *(BaseType::pGetCurrentIterationResidualVector());

        if (p_V != nullptr && p_W != nullptr) {
            // Get the required arrays
            const auto& r_V = *p_V;
            const auto& r_W = *p_W;

            // Calculate the correction as the inverse Jacobian approximation times the current iteration residual
            MatrixType aux_M;
            CalculateAuxiliaryMatrixM(aux_M);
            VectorType M_res = prod(aux_M, r_res_vect);
            VectorType V_M_res = prod(r_V, M_res);
            VectorType W_M_res = prod(r_W, M_res);

            noalias(rCorrection) = ZeroVector(n_dofs);
            TDenseSpace::UnaliasedAdd(rCorrection, 1.0, V_M_res);
            TDenseSpace::UnaliasedAdd(rCorrection, 1.0, W_M_res);
            TDenseSpace::UnaliasedAdd(rCorrection, -1.0, r_res_vect);

            if (mpOldJacQU != nullptr && mpOldJacSigmaV != nullptr) {
                const auto& r_A = *mpOldJacQU;
                const auto& r_B = *mpOldJacSigmaV;
                VectorType B_res = prod(r_B, r_res_vect);
                VectorType A_B_res = prod(r_A, B_res);
                VectorType B_V_M_res = prod(r_B, V_M_res);
                VectorType A_B_V_M_res = prod(r_A, B_V_M_res);
                TDenseSpace::UnaliasedAdd(rCorrection, 1.0, A_B_res);
                TDenseSpace::UnaliasedAdd(rCorrection, -1.0, A_B_V_M_res);
            }
        } else {
            if (mpOldJacQU != nullptr && mpOldJacSigmaV != nullptr) {
                const auto& r_A = *mpOldJacQU;
                const auto& r_B = *mpOldJacSigmaV;
                VectorType B_res(n_dofs);
                TDenseSpace::Mult(r_B, r_res_vect, B_res);
                TDenseSpace::Mult(r_A, B_res, rCorrection);
                TDenseSpace::UnaliasedAdd(rCorrection, -1.0, r_res_vect);
            } else {
                KRATOS_ERROR << "There is neither observation nor old Jacobian decomposition. Correction cannot be computed." << std::endl;
            }
        }
    }

    // void UpdateCurrentJacobianMatrix() override
    // {
    //     // Compute the current inverse Jacobian approximation
    //     MatrixType aux_M;
    //     CalculateAuxiliaryMatrixM(aux_M);
    //     const SizeType n_dofs = BaseType::GetProblemSize();
    //     MatrixPointerType p_aux_jac_k1 = MatrixPointerType(new MatrixType(n_dofs, n_dofs));
    //     auto& r_aux_jac = *p_aux_jac_k1;
    //     const auto& r_W = *(BaseType::pGetSolutionObservationMatrix());
    //     r_aux_jac = prod(r_W, aux_M);

    //     if (mpOldJacQU != nullptr && mpOldJacSigmaV != nullptr) {
    //         const auto& r_A = *mpOldJacQU;
    //         const auto& r_B = *mpOldJacSigmaV;
    //         const auto& r_V = *(BaseType::pGetResidualObservationMatrix());
    //         MatrixType A_B = prod(r_A, r_B);
    //         MatrixType V_M = prod(r_V, aux_M);
    //         MatrixType A_B_V_M = prod(A_B, V_M);

    //         if (!BaseType::IsUsedInBlockNewtonEquations()) {
    //             for (SizeType i = 0; i < n_dofs; ++i) {
    //                 for (SizeType j = 0; j < n_dofs; ++j) {
    //                     r_aux_jac(i,j) += A_B(i,j) - A_B_V_M(i,j) + V_M(i,j);
    //                 }
    //             }
    //         } else {
    //             for (SizeType i = 0; i < n_dofs; ++i) {
    //                 for (SizeType j = 0; j < n_dofs; ++j) {
    //                     r_aux_jac(i,j) += A_B(i,j) - A_B_V_M(i,j);
    //                 }
    //             }
    //         }
    //     }

    //     BaseType::SetInverseJacobianApproximation(p_aux_jac_k1);
    // }

    void CalculateInverseJacobianSVD()
    {
        const auto p_W = BaseType::pGetSolutionObservationMatrix();
        const auto p_V = BaseType::pGetResidualObservationMatrix();

        // If there exists new observations, use these to calculate the inverse Jacobian SVD
        // If not, the old Jacobian decomposition will be used
        if (p_V != nullptr && p_W != nullptr) {
            // Compute ((trans(V)*V)^-1)*trans(V)
            MatrixType aux_M;
            CalculateAuxiliaryMatrixM(aux_M);

            // If not initialized yet, create the random values matrix for the truncated SVD
            if (!mRandomValuesAreInitialized || mpOmega == nullptr) {
                InitializeRandomValuesMatrix();
            }

            // Do the randomized SVD decomposition of the current Jacobian approximation
            // Note that the identity part of the Jacobian will not be considered in the randomized SVD as this is full rank
            MatrixType y;
            MultiplyRight(aux_M, *mpOmega, y);

            QR<double, row_major> qr_util;
            const SizeType n_dofs = BaseType::GetProblemSize();
            qr_util.compute(n_dofs, mCurrentNumberOfModes, &(y(0,0)));
            qr_util.compute_q();
            MatrixType Q(n_dofs, mCurrentNumberOfModes);
            for (SizeType i = 0; i < n_dofs; ++i) { //TODO: This can be parallel
                for (SizeType j = 0; j < mCurrentNumberOfModes; ++j) {
                    Q(i,j) = qr_util.Q(i,j);
                }
            }

            MatrixType phi;
            MultiplyTransposeLeft(aux_M, Q, phi);

            VectorType s_svd; // Eigenvalues vector
            MatrixType u_svd; // Left orthogonal matrix
            MatrixType v_svd; // Right orthogonal matrix
            Parameters svd_settings(R"({
                "compute_thin_u" : true,
                "compute_thin_v" : true
            })");
            mpDenseSVD->Compute(phi, s_svd, u_svd, v_svd, svd_settings);

            auto p_aux_Q_U = Kratos::make_shared<MatrixType>(prod(Q, u_svd));
            auto p_aux_sigma_V = Kratos::make_shared<MatrixType>(v_svd.size2(), v_svd.size1());
            auto& r_aux_sigma_V = *p_aux_sigma_V;
            for (SizeType i = 0; i < r_aux_sigma_V.size1(); ++i) {
                const double aux_s = s_svd(i);
                for (SizeType j = 0; j < r_aux_sigma_V.size2(); ++j) {
                    r_aux_sigma_V(i,j) = aux_s * v_svd(j,i);
                }
            }
            std::swap(mpJacQU, p_aux_Q_U);
            std::swap(mpJacSigmaV, p_aux_sigma_V);

        } else {
            // Setting previous step Jacobian decomposition as current one
            // This is supposed to be used in the first iteration correction
            mpJacQU = mpOldJacQU;
            mpJacSigmaV = mpOldJacSigmaV;
        }
    }

    ///@}
    ///@name Protected  Access
    ///@{

    MatrixPointerType pGetJacobianDecompositionMatixQU() override
    {
        return mpJacQU;
    }

    MatrixPointerType pGetJacobianDecompositionMatixSigmaV() override
    {
        return mpJacSigmaV;
    }

    MatrixPointerType pGetOldJacobianDecompositionMatixQU() override
    {
        return mpOldJacQU;
    }

    MatrixPointerType pGetOldJacobianDecompositionMatixSigmaV() override
    {
        return mpOldJacSigmaV;
    }

    ///@}
private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{

    SizeType mUserNumberOfModes; // User-defined number of modes to be kept in the Jacobian randomized SVD
    SizeType mCurrentNumberOfModes; // Current number of modes to be kept in the Jacobian randomized SVD
    bool mLimitModesToIterations = true; // Limits the number of modes to the current iterations
    bool mRandomValuesAreInitialized = false; // Indicates if the random values for the truncated SVD have been already set

    DenseSVDPointerType mpDenseSVD; // Pointer to the dense SVD utility

    MatrixPointerType mpOmega = nullptr; // Matrix with random values for truncated SVD
    MatrixPointerType mpJacQU = nullptr; // Left DOFs x modes matrix from the previous Jacobian decomposition
    MatrixPointerType mpJacSigmaV = nullptr; // Right modes x DOFs matrix from the previous Jacobian decomposition
    MatrixPointerType mpOldJacQU = nullptr; // Left DOFs x modes matrix from the previous Jacobian decomposition
    MatrixPointerType mpOldJacSigmaV = nullptr; // Right modes x DOFs matrix from the previous Jacobian decomposition

    ///@}
    ///@name Private Operators
    ///@{

    void InitializeRandomValuesMatrix()
    {
        // Initialize auxiliary variables
        bool is_number_of_modes_limited = false;
        mCurrentNumberOfModes = mUserNumberOfModes;

        // Check if the user-defined number of modes exceeds the iteration number
        //TODO: CHECK WITH THE COLUMNS IN CASE WE DID CUTOFF!!!!
        const SizeType n_it = BaseType::GetConvergenceAcceleratorIteration();
        if (mLimitModesToIterations && n_it < mCurrentNumberOfModes) {
            mCurrentNumberOfModes = n_it;
            is_number_of_modes_limited = true;
        }

        // Set the random values matrix pointer
        const SizeType n_dofs = BaseType::GetProblemSize();
        auto p_aux_omega = Kratos::make_shared<MatrixType>(n_dofs, mCurrentNumberOfModes);
        std::swap(p_aux_omega, mpOmega);

        // Create the random values generator
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 generator(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> distribution(0.0, 1.0);

        // Fill the random values matrix
        auto& r_omega_matrix = *mpOmega;
        for (SizeType i = 0; i < n_dofs; ++i) {
            for (SizeType j = 0; j < mCurrentNumberOfModes; ++j) {
                // Use distribution to transform the random unsigned int generated by generator into a
                // double in [0.0, 1.0). Each call to distribution(generator) generates a new random double
                r_omega_matrix(i,j) = distribution(generator);
            }
        }

        // Set the flag to avoid performing this operation again (the random values are stored)
        mRandomValuesAreInitialized = !is_number_of_modes_limited;
    }

    void MultiplyRight(
        const Matrix& rAuxM,
        const Matrix& rRightMatrix,
        Matrix& rSolution)
    {
        const SizeType n_dofs = BaseType::GetProblemSize();
        KRATOS_ERROR_IF(rRightMatrix.size1() != n_dofs) << "Obtained right multiplication matrix size " << rRightMatrix.size1() << " does not match the problem size " << n_dofs << " expected one." << std::endl;
        if (rSolution.size1() != n_dofs|| rSolution.size2() != mCurrentNumberOfModes) {
            rSolution.resize(n_dofs, mCurrentNumberOfModes);
        }

        // Get observation matrices from base MVQN convergence accelerator
        const auto& r_W = *(BaseType::pGetSolutionObservationMatrix());
        const auto& r_V = *(BaseType::pGetResidualObservationMatrix());

        // Add to the solution matrix
        MatrixType M_omega = prod(rAuxM, rRightMatrix);
        noalias(rSolution) = prod(r_W, M_omega);

        if (mpOldJacQU == nullptr && mpOldJacSigmaV == nullptr) {
            if (!BaseType::IsUsedInBlockNewtonEquations()) {
                // Old Jacobian initialized to minus the identity matrix
                MatrixType V_M_omega = prod(r_V, M_omega);
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += V_M_omega(i,j);
                    }
                }
            }
        } else {
            MatrixType V_M_omega = prod(r_V, M_omega);
            MatrixType B_omega = prod(*mpOldJacSigmaV, rRightMatrix);
            MatrixType A_B_omega = prod(*mpOldJacQU, B_omega);
            MatrixType B_V_M_omega = prod(*mpOldJacSigmaV, V_M_omega);
            MatrixType A_B_V_M_omega = prod(*mpOldJacQU, B_V_M_omega);
            if (!BaseType::IsUsedInBlockNewtonEquations()) {
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += A_B_omega(i,j) + V_M_omega(i,j) - A_B_V_M_omega(i,j);
                    }
                }
            } else {
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += A_B_omega(i,j) - A_B_V_M_omega(i,j);
                    }
                }
            }
        }
    }

    void MultiplyTransposeLeft(
        const Matrix& rAuxM,
        const Matrix& rLeftMatrix,
        Matrix& rSolution)
    {
        const SizeType n_dofs = BaseType::GetProblemSize();
        KRATOS_ERROR_IF(rLeftMatrix.size1() != n_dofs) << "Obtained left multiplication matrix size " << rLeftMatrix.size1() << " does not match the problem size " << n_dofs << " expected one." << std::endl;
        if (rSolution.size1() != mCurrentNumberOfModes|| rSolution.size2() != n_dofs) {
            rSolution.resize(mCurrentNumberOfModes, n_dofs);
        }

        // Get observation matrices from base MVQN convergence accelerator
        const auto& r_W = *(BaseType::pGetSolutionObservationMatrix());
        const auto& r_V = *(BaseType::pGetResidualObservationMatrix());

        // Add to the solution matrix
        const auto Qtrans = trans(rLeftMatrix);
        MatrixType Qtrans_W = prod(Qtrans, r_W);
        noalias(rSolution) = prod(Qtrans_W, rAuxM);

        if (mpOldJacQU == nullptr && mpOldJacSigmaV == nullptr) {
            if (!BaseType::IsUsedInBlockNewtonEquations()) {
                // Old Jacobian initialized to minus the identity matrix
                MatrixType Qtrans_V = prod(Qtrans, r_V);
                MatrixType Qtrans_V_M = prod(Qtrans_V, rAuxM);
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += Qtrans_V_M(i,j);
                    }
                }
            }
        } else {
            MatrixType Qtrans_V = prod(Qtrans, r_V);
            MatrixType Qtrans_V_M = prod(Qtrans_V, rAuxM);
            MatrixType Qtrans_A = prod(Qtrans, *mpOldJacQU);
            MatrixType Qtrans_A_B = prod(Qtrans_A, *mpOldJacSigmaV);
            MatrixType Qtrans_A_B_V = prod(Qtrans_A_B, r_V);
            MatrixType Qtrans_A_B_V_M = prod(Qtrans_A_B_V, rAuxM);
            if (!BaseType::IsUsedInBlockNewtonEquations()) {
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += Qtrans_A_B(i,j) + Qtrans_V_M(i,j) - Qtrans_A_B_V_M(i,j);
                    }
                }
            } else {
                for (SizeType i = 0; i < rSolution.size1(); ++i) { //TODO: DO THIS PARALLEL
                    for (SizeType j = 0; j < rSolution.size2(); ++j) {
                        rSolution(i,j) += Qtrans_A_B(i,j) - Qtrans_A_B_V_M(i,j);
                    }
                }
            }
        }
    }

    void CalculateAuxiliaryMatrixM(MatrixType& rAuxM)
    {
        // Compute (trans(V)*V)^-1
        const auto& r_V = *(BaseType::pGetResidualObservationMatrix());
        const auto Vtrans = trans(r_V);
        const SizeType n_data_cols = r_V.size2();
        MatrixType transV_V(n_data_cols, n_data_cols);
        noalias(transV_V) = prod(Vtrans, r_V);

        // Perform the observation matrix V Singular Value Decomposition (SVD) such that
        // matrix V (m x n) is equal to the SVD matrices product V = u_svd * w_svd * v_svd
        MatrixType u_svd; // Orthogonal matrix (m x m)
        MatrixType w_svd; // Rectangular diagonal matrix (m x n)
        MatrixType v_svd; // Orthogonal matrix (n x n)
        std::string svd_type = "Jacobi"; // SVD decomposition type
        double svd_rel_tol = 1.0e-6; // Relative tolerance of the SVD decomposition (it will be multiplied by the input matrix norm)
        SVDUtils<double>::SingularValueDecomposition(transV_V, u_svd, w_svd, v_svd, svd_type, svd_rel_tol);

        // Compute the matrix pseudo-inverse
        // Note that we take advantage of the fact that the matrix is always squared
        MatrixType transV_V_inv = ZeroMatrix(n_data_cols, n_data_cols);
        for (SizeType i = 0; i < n_data_cols; ++i) {
            for (SizeType j = 0; j < n_data_cols; ++j) {
                const double aux = v_svd(j,i) / w_svd(j,j);
                for (SizeType k = 0; k < n_data_cols; ++k) {
                    transV_V_inv(i,k) += aux * u_svd(k,j);
                }
            }
        }

        // Compute ((trans(V)*V)^-1)*trans(V)
        const SizeType n_dofs = BaseType::GetProblemSize();
        if (rAuxM.size1() != n_data_cols || rAuxM.size2() != n_dofs) {
            rAuxM.resize(n_data_cols, n_dofs);
        }
        noalias(rAuxM) = prod(transV_V_inv, Vtrans);
    }

    ///@}
    ///@name Private Operations
    ///@{


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
}; /* Class MVQNRandomizedSVDConvergenceAccelerator */

///@}
///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


///@}

}  /* namespace Kratos.*/

#endif /* KRATOS_MVQN_RANDOMIZED_SVD_CONVERGENCE_ACCELERATOR defined */
