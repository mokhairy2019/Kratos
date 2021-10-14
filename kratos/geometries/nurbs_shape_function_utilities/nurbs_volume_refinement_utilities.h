//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Manuel Messmer
//

#if !defined(KRATOS_NURBS_VOLUME_REFINEMENT_UTILITIES_H_INCLUDED )
#define  KRATOS_NURBS_VOLUME_REFINEMENT_UTILITIES_H_INCLUDED

// System includes

// External includes

// Project includes
#include "geometries/nurbs_volume_geometry.h"
#include "nurbs_utilities.h"
#include "includes/node.h"

namespace Kratos {

class NurbsVolumeRefinementUtilities
{
public:
    ///@name Type Definitions
    ///@{

    typedef std::size_t IndexType;
    typedef std::size_t SizeType;
    typedef Node<3> NodeType;

    typedef NurbsVolumeGeometry<PointerVector<NodeType>> NurbsVolumeGeometryType;
    typedef typename NurbsVolumeGeometryType::Pointer NurbsVolumeGeometryPointerType;

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief Refines the u-knotvector of a NurbsVolumeGeometry.
     * @details This function adopts the surface knot-refinement algorithm from Piegl1995 (p.164 Algorithm A5.5).
     *          The algorithm is modified to suit trivariant B-Spline volumes.
     * @param rGeometry Geometry to be refined.
     * @param vector<double> Knots to be inserted.
     * @return Pointer to refined geometry.
     * @note This function does not consider weights, thus only B-Spline-Volumes can be refined.
     **/
    static void KnotRefinementU(NurbsVolumeGeometryType& rGeometry, std::vector<double>& rInsertKnots,
                            PointerVector<NodeType>& rPointsRefined, Vector& rKnotsURefined ){

        // Sort the knots which are to be inserted!
        std::sort(rInsertKnots.begin(),rInsertKnots.end());
        // Get current order
        const SizeType polynomial_degree_u = rGeometry.PolynomialDegreeU();

        // Get current knot information
        const Kratos::Vector& old_knots_u = rGeometry.KnotsU();

        const SizeType old_num_of_knots_u = rGeometry.NumberOfKnotsU();
        // Get current cp's information
        const SizeType old_num_of_cp_u = rGeometry.NumberOfControlPointsU();
        const SizeType old_num_of_cp_v = rGeometry.NumberOfControlPointsV();
        const SizeType old_num_of_cp_w = rGeometry.NumberOfControlPointsW();

        // Get current span's
        SizeType a = NurbsUtilities::GetLowerSpan(polynomial_degree_u, old_knots_u, rInsertKnots.front());
        SizeType b = NurbsUtilities::GetLowerSpan(polynomial_degree_u, old_knots_u, rInsertKnots.back()) + 1;

        SizeType r = rInsertKnots.size();
        // Initialize new containers
        SizeType new_num_of_knots_u = old_num_of_knots_u + r;
        rKnotsURefined.resize(new_num_of_knots_u);

        SizeType new_num_of_cp_u = old_num_of_cp_u + r;
        rPointsRefined.resize(new_num_of_cp_u*old_num_of_cp_v*old_num_of_cp_w);

        r = r - 1;
        // Create new ordered knot vector.
        for( IndexType i = 0; i <= a; i++)
            rKnotsURefined[i] = old_knots_u[i];
        for( IndexType i = b+polynomial_degree_u; i < old_num_of_knots_u; ++i)
            rKnotsURefined[i+r+1] = old_knots_u[i];

        // Copy unaltered cp's.
        for( IndexType column=0; column < old_num_of_cp_v; ++column){
            for( IndexType depth=0; depth < old_num_of_cp_w; ++depth){
                // Copy unaltered control points.
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = 0; i <= a - polynomial_degree_u + 1; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, i, column, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, i, column, depth);
                    rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                }
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = b; i < old_num_of_cp_u; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, i+r+1, column, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, i, column, depth);
                    rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                }
            }
        }

        // Find new CP's
        const SizeType p = polynomial_degree_u;
        int i = b + p - 1;
        int k = b + p + r;
        for( int j = r; j >= 0; j--){
            while( rInsertKnots[j] <= old_knots_u[i] && i > static_cast<int>(a) ){
                for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                    for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                        // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                        // Also keep attention to the first argument. The left index is mapped to new_num_of_cp_u, but the right index is mapped
                        // to old_num_of_cp_u.
                        IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, k-p-1+1, column, depth);
                        IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, i-p-1+1, column, depth);
                        rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                    }
                }
                rKnotsURefined[k] = old_knots_u[i];
                k--;
                i--;
            }
            for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                   // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, k-p-1+1, column, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, k-p+1, column, depth);
                    rPointsRefined(cp_index_left) =  rPointsRefined(cp_index_right);
                }
            }
            for( IndexType l=1; l <= p; ++l){
                IndexType index = k - p + l;
                double alpha = rKnotsURefined[k+l] - rInsertKnots[j];
                if( std::abs(alpha) < 1e-10){
                    for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                        for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, index-1+1, column, depth);
                            IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, index+1, column, depth);
                            rPointsRefined(cp_index_left) = rPointsRefined(cp_index_right);
                        }
                    }
                }
                else {
                    alpha = alpha / ( rKnotsURefined[k+l] - old_knots_u[i-p+l]);
                    for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                        for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_minus = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, index-1+1, column, depth);
                            IndexType cp_index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                new_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, index+1, column, depth);

                            double x = alpha * rPointsRefined[cp_index_minus][0] + (1.0-alpha) * rPointsRefined[cp_index][0];
                            double y = alpha * rPointsRefined[cp_index_minus][1] + (1.0-alpha) * rPointsRefined[cp_index][1];
                            double z = alpha * rPointsRefined[cp_index_minus][2] + (1.0-alpha) * rPointsRefined[cp_index][2];

                            rPointsRefined(cp_index_minus) = Kratos::make_intrusive<NodeType>(0, x, y, z);
                        }
                    }
                }

            }
            rKnotsURefined[k] = rInsertKnots[j];
            k -= 1;
        }
    }

    /**
     * @brief Refines the v-knotvector of a NurbsVolumeGeometry.
     * @details This function adopts the surface knot-refinement algorithm from Piegl1995 (p.164 Algorithm A5.5).
     *          The algorithm is modified to suit trivariant B-Spline volumes.
     * @param rGeometry Geometry to be refined.
     * @param vector<double> Knots to be inserted.
     * @return Pointer to refined geometry.
     * @note This function does not consider weights, thus only B-Spline-Volumes can be refined.
     **/
    static void KnotRefinementV(NurbsVolumeGeometryType& rGeometry, std::vector<double>& rInsertKnots,
            PointerVector<NodeType>& rPointsRefined, Vector& rKnotsVRefined ){

        // Sort the knots which are to be inserted!
        std::sort(rInsertKnots.begin(),rInsertKnots.end());
        // Get current order
        const SizeType polynomial_degree_v = rGeometry.PolynomialDegreeV();
        // Get current knot information
        const Kratos::Vector& old_knots_v = rGeometry.KnotsV();

        const SizeType old_num_of_knots_v = rGeometry.NumberOfKnotsV();
        // Get current cp's information
        const SizeType old_num_of_cp_u = rGeometry.NumberOfControlPointsU();
        const SizeType old_num_of_cp_v = rGeometry.NumberOfControlPointsV();
        const SizeType old_num_of_cp_w = rGeometry.NumberOfControlPointsW();

        // Get current span's
        SizeType a = NurbsUtilities::GetLowerSpan(polynomial_degree_v, old_knots_v, rInsertKnots.front());
        SizeType b = NurbsUtilities::GetLowerSpan(polynomial_degree_v, old_knots_v, rInsertKnots.back()) + 1;

        SizeType r = rInsertKnots.size();
        // Initialize new containers
        SizeType new_num_of_knots_v = old_num_of_knots_v + r;
        rKnotsVRefined.resize(new_num_of_knots_v);

        SizeType new_num_of_cp_v = old_num_of_cp_v + r;
        rPointsRefined.resize(old_num_of_cp_u*new_num_of_cp_v*old_num_of_cp_w);

        r = r - 1;
        // Create new ordered knot vector.
        for( IndexType i = 0; i <= a; i++)
            rKnotsVRefined[i] = old_knots_v[i];
        for( IndexType i = b+polynomial_degree_v; i < old_num_of_knots_v; ++i)
            rKnotsVRefined[i+r+1] = old_knots_v[i];

        // Copy unaltered cp's.
        for( IndexType row=0; row < old_num_of_cp_u; ++row){
            for( IndexType depth=0; depth < old_num_of_cp_w; ++depth){
                // Copy unaltered control points.
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = 0; i <= a - polynomial_degree_v + 1; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, i, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, i, depth);
                    rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                }
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = b; i < old_num_of_cp_v; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, i+r+1, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, i, depth);
                    rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                }
            }
        }

        // Find new CP's
        const SizeType p = polynomial_degree_v;
        int i = b + p - 1;
        int k = b + p + r;
        for( int j = r; j >= 0; j--){
            while( rInsertKnots[j] <= old_knots_v[i] && i > static_cast<int>(a)){
                for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                    for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                        // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                        // Also keep attention to the second argument. The left index is mapped to new_num_of_cp_v, but the right index is mapped
                        // to old_num_of_cp_v.
                        IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, k-p-1+1, depth);
                        IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, i-p-1+1, depth);
                        rPointsRefined(cp_index_left) = Kratos::make_intrusive<NodeType>(0, rGeometry[cp_index_right]);
                    }
                }
                rKnotsVRefined[k] = old_knots_v[i];
                k--;
                i--;
            }
            for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                   // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, k-p-1+1, depth);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, k-p+1, depth);
                    rPointsRefined(cp_index_left) =  rPointsRefined(cp_index_right);
                }
            }
            for( IndexType l=1; l <= p; ++l){
                IndexType index = k - p + l;
                double alpha = rKnotsVRefined[k+l] - rInsertKnots[j];
                if( std::abs(alpha) < 1e-10){
                    for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                        for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, index-1+1, depth);
                            IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, index+1, depth);
                            rPointsRefined(cp_index_left) = rPointsRefined(cp_index_right);
                        }
                    }
                }
                else {
                    alpha = alpha / ( rKnotsVRefined[k+l] - old_knots_v[i-p+l]);
                    for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                        for( IndexType depth=0; depth < old_num_of_cp_w; ++depth) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_minus = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, index-1+1, depth);
                            IndexType cp_index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, new_num_of_cp_v, old_num_of_cp_w, row, index+1, depth);

                            double x = alpha * rPointsRefined[cp_index_minus][0] + (1.0-alpha) * rPointsRefined[cp_index][0];
                            double y = alpha * rPointsRefined[cp_index_minus][1] + (1.0-alpha) * rPointsRefined[cp_index][1];
                            double z = alpha * rPointsRefined[cp_index_minus][2] + (1.0-alpha) * rPointsRefined[cp_index][2];

                            rPointsRefined(cp_index_minus) = Kratos::make_intrusive<NodeType>(0, x, y, z);
                        }
                    }
                }

            }
            rKnotsVRefined[k] = rInsertKnots[j];
            k -= 1;
        }
    }

    /**
     * @brief Refines the w-knotvector of a NurbsVolumeGeometry.
     * @details This function adopts the surface knot-refinement algorithm from Piegl1995 (p.164 Algorithm A5.5).
     *          The algorithm is modified to suit trivariant B-Spline volumes.
     * @param rGeometry Geometry to be refined.
     * @param vector<double> Knots to be inserted.
     * @return Pointer to refined geometry.
     * @note This function does not consider weights, thus only B-Spline-Volumes can be refined.
     **/
    static NurbsVolumeGeometryPointerType KnotRefinementW(NurbsVolumeGeometryType& rGeometry, std::vector<double>& rInsertKnots ){

        // Sort the knots which are to be inserted!
        std::sort(rInsertKnots.begin(),rInsertKnots.end());
        // Get current order
        const SizeType polynomial_degree_u = rGeometry.PolynomialDegreeU();
        const SizeType polynomial_degree_v = rGeometry.PolynomialDegreeV();
        const SizeType polynomial_degree_w = rGeometry.PolynomialDegreeW();
        // Get current knot information
        const Kratos::Vector& old_knots_u = rGeometry.KnotsU();
        const Kratos::Vector& old_knots_v = rGeometry.KnotsV();
        const Kratos::Vector& old_knots_w = rGeometry.KnotsW();

        const SizeType old_num_of_knots_w = rGeometry.NumberOfKnotsW();
        // Get current cp's information
        const SizeType old_num_of_cp_u = rGeometry.NumberOfControlPointsU();
        const SizeType old_num_of_cp_v = rGeometry.NumberOfControlPointsV();
        const SizeType old_num_of_cp_w = rGeometry.NumberOfControlPointsW();

        // Get current span's
        SizeType a = NurbsUtilities::GetLowerSpan(polynomial_degree_w, old_knots_w, rInsertKnots.front());
        SizeType b = NurbsUtilities::GetLowerSpan(polynomial_degree_w, old_knots_w, rInsertKnots.back()) + 1;

        SizeType r = rInsertKnots.size();
        // Initialize new containers
        SizeType new_num_of_knots_w = old_num_of_knots_w + r;
        Kratos::Vector new_knots_w(new_num_of_knots_w);

        SizeType new_num_of_cp_w = old_num_of_cp_w + r;
        PointerVector<NodeType> new_points(old_num_of_cp_u*old_num_of_cp_v*new_num_of_cp_w);

        r = r - 1;
        // Create new ordered knot vector.
        for( IndexType i = 0; i <= a; i++)
            new_knots_w[i] = old_knots_w[i];
        for( IndexType i = b+polynomial_degree_w; i < old_num_of_knots_w; ++i)
            new_knots_w[i+r+1] = old_knots_w[i];

        // Copy unaltered cp's.
        for( IndexType row=0; row < old_num_of_cp_u; ++row){
            for( IndexType column=0; column < old_num_of_cp_v; ++column){
                // Copy unaltered control points.
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = 0; i <= a - polynomial_degree_w + 1; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, i);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, column, i);
                    new_points(cp_index_left) = Kratos::make_intrusive<NodeType>(cp_index_left+1, rGeometry[cp_index_right]);
                }
                // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                for( IndexType i = b; i < old_num_of_cp_w; ++i){
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, i+r+1);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, column, i);
                    new_points(cp_index_left) = Kratos::make_intrusive<NodeType>(cp_index_left+1, rGeometry[cp_index_right]);
                }
            }
        }

        // Find new CP's
        const SizeType p = polynomial_degree_w;
        int i = b + p - 1;
        int k = b + p + r;
        for( int j = r; j >= 0; j--){
            while( rInsertKnots[j] <= old_knots_w[i] && i > static_cast<int>(a)){
                for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                    for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                        // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                        // Also keep attention to the second argument. The left index is mapped to new_num_of_cp_w, but the right index is mapped
                        // to old_num_of_cp_w.
                        IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, k-p-1+1);
                        IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                            old_num_of_cp_u, old_num_of_cp_v, old_num_of_cp_w, row, column, i-p-1+1);
                        new_points(cp_index_left) = Kratos::make_intrusive<NodeType>(cp_index_left+1, rGeometry[cp_index_right]);
                    }
                }
                new_knots_w[k] = old_knots_w[i];
                k--;
                i--;
            }
            for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                   // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                    IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, k-p-1+1);
                    IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                        old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, k-p+1);
                    new_points(cp_index_left) =  new_points(cp_index_right);
                    new_points(cp_index_left)->SetId(cp_index_left+1);
                }
            }
            for( IndexType l=1; l <= p; ++l){
                IndexType index = k - p + l;
                double alpha = new_knots_w[k+l] - rInsertKnots[j];
                if( std::abs(alpha) < 1e-10){
                    for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                        for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_left = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, index-1+1);
                            IndexType cp_index_right = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, index+1);
                            new_points(cp_index_left) = new_points(cp_index_right);
                            new_points(cp_index_left)->SetId(cp_index_left+1);
                        }
                    }
                }
                else {
                    alpha = alpha / ( new_knots_w[k+l] - old_knots_w[i-p+l]);
                    for( IndexType row=0; row < old_num_of_cp_u; ++row) {
                        for( IndexType column=0; column < old_num_of_cp_v; ++column) {
                            // Note: The "+1" accounts for the fact that first and last knots only appear "p"-times (and not "p+1"-times).
                            IndexType cp_index_minus = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, index-1+1);
                            IndexType cp_index = NurbsUtilities::GetVectorIndexFromMatrixIndices(
                                old_num_of_cp_u, old_num_of_cp_v, new_num_of_cp_w, row, column, index+1);

                            double x = alpha * new_points[cp_index_minus][0] + (1.0-alpha) * new_points[cp_index][0];
                            double y = alpha * new_points[cp_index_minus][1] + (1.0-alpha) * new_points[cp_index][1];
                            double z = alpha * new_points[cp_index_minus][2] + (1.0-alpha) * new_points[cp_index][2];

                            new_points(cp_index_minus) = Kratos::make_intrusive<NodeType>(cp_index_minus+1, x, y, z);
                        }
                    }
                }

            }
            new_knots_w[k] = rInsertKnots[j];
            k -= 1;
        }

        return Kratos::make_shared<NurbsVolumeGeometry<PointerVector<NodeType>>>(
            new_points, polynomial_degree_u, polynomial_degree_v, polynomial_degree_w,
            old_knots_u, old_knots_v, new_knots_w);
    }
    ///@}
};

} // End namespace kratos

#endif // KRATOS_NURBS_VOLUME_REFINEMENT_UTILITIES_H_INCLUDED