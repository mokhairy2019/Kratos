// KRATOS ___ ___  _  ___   __   ___ ___ ___ ___
//       / __/ _ \| \| \ \ / /__|   \_ _| __| __|
//      | (_| (_) | .` |\ V /___| |) | || _|| _|
//       \___\___/|_|\_| \_/    |___/___|_| |_|  APPLICATION
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Ruben Zorrilla
//

#pragma once

// System includes

// External includes

// Project includes
#include "containers/model.h"
#include "containers/pointer_vector.h"
#include "includes/define.h"
#include "includes/key_hash.h"
#include "modified_shape_functions/modified_shape_functions.h"

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

/// Short class definition.
class ShiftedBoundaryMeshlessInterfaceUtility
{
public:

    ///@name Type Definitions
    ///@{

    using IndexType = ModelPart::IndexType;

    using NodeType = ModelPart::NodeType;

    using GeometryType = ModelPart::GeometryType;

    using ShapeFunctionsGradientsType = GeometryType::ShapeFunctionsGradientsType;

    using ModifiedShapeFunctionsFactoryType = std::function<ModifiedShapeFunctions::UniquePointer(const GeometryType::Pointer, const Vector&)>;

    using RBFShapeFunctionsFunctionType = std::function<void(const Matrix&, const array_1d<double,3>&, Vector&)>;

    using MLSShapeFunctionsFunctionType = std::function<void(const Matrix&, const array_1d<double,3>&, const double, Vector&)>;

    using MLSShapeFunctionsAndGradientsFunctionType = std::function<void(const Matrix&, const array_1d<double,3>&, const double, Vector&, Matrix&)>;

    using ElementSizeFunctionType = std::function<double(const GeometryType&)>;

    using NodesCloudSetType = std::unordered_set<NodeType::Pointer, SharedPointerHasher<NodeType::Pointer>, SharedPointerComparator<NodeType::Pointer>>;

    using CloudDataVectorType = DenseVector<std::pair<NodeType::Pointer, double>>;

    using NodesCloudMapType = std::unordered_map<NodeType::Pointer, CloudDataVectorType, SharedPointerHasher<NodeType::Pointer>, SharedPointerComparator<NodeType::Pointer>>;

    ///@}
    ///@name Pointer Definitions

    /// Pointer definition of ShiftedBoundaryMeshlessInterfaceUtility
    KRATOS_CLASS_POINTER_DEFINITION(ShiftedBoundaryMeshlessInterfaceUtility);

    ///@}
    ///@name Life Cycle
    ///@{

    /// @brief Standard constructor
    /// @param rModel Model container
    /// @param ThisParameters Parameters object encapsulating the settings
    ShiftedBoundaryMeshlessInterfaceUtility(
        Model& rModel,
        Parameters ThisParameters);

    /// Copy constructor.
    ShiftedBoundaryMeshlessInterfaceUtility(ShiftedBoundaryMeshlessInterfaceUtility const& rOther) = delete;

    /// Destructor.
    virtual ~ShiftedBoundaryMeshlessInterfaceUtility() = default;

    ///@}
    ///@name Operators
    ///@{

    /// Assignment operator.
    ShiftedBoundaryMeshlessInterfaceUtility& operator=(ShiftedBoundaryMeshlessInterfaceUtility const& rOther) = delete;

    ///@}
    ///@name Operations
    ///@{

    void CalculateExtensionOperator();

    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    const Parameters GetDefaultParameters() const;

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const
    {
        return "ShiftedBoundaryMeshlessInterfaceUtility";
    }

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const
    {
        rOStream << "ShiftedBoundaryMeshlessInterfaceUtility";
    }

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const
    {
    }

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

    ModelPart* mpModelPart = nullptr;
    ModelPart* mpBoundarySubModelPart = nullptr;

    bool mMLSConformingBasis;

    bool mGradientBasedConformingBasis;

    std::size_t mMLSExtensionOperatorOrder;

    const Condition* mpConditionPrototype;

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    /**
     * @brief Calculate the extension basis coefficients
     * This method calculates the extension operator coefficients from a nodal reconstruction of the gradients
     * The gradient reconstruction is done at each node of the surrogate boundary from the values of the \tilde{\Omega} domain
     * Then the value at the first layer of "inner" nodes is calculated from the projection of such gradients along all the
     * intersected edges that arrive to such node.
     * Finally, a conforming basis is build by calculating the values at the location of each point condition
     */
    void CalculateGradientBasedConformingExtensionBasis();

    /**
     * @brief Calculate the extension basis coefficients
     * This method calculates the extension operator coefficients in a meshless fashion
     * First, a cloud of points is created for each one of the "inner" nodes. This cloud of points is then used in the
     * calculation of the MLS (or RBF) approximants at these points. Then, the basis is made conformant by using the meshless
     * approximants to calculate the interpolation at each one of the point conditions location.
     */
    void CalculateMeshlessBasedConformingExtensionBasis();

    /**
     * @brief Calculate the extension basis coefficients
     * This method calculates the extension operator coefficients in a meshless fashion
     * First, a cloud of points is created for each one of the point conditions. This cloud of points is then used in the
     * calculation of the MLS (or RBF) approximants.
     */
    void CalculateMeshlessBasedNonConformingExtensionBasis();

    /**
     * @brief Set the corresponding flags at the interface
     * This methods sets the corresponding flags at the interface nodes and elements. These are
     * node ACTIVE : nodes that belong to the elements to be assembled
     * node BOUNDARY : nodes that belong to the surrogate boundary
     * node INTERFACE : nodes that belong to the cloud of points
     * element ACTIVE : elements in the positive distance region (the ones to be assembled)
     * element BOUNDARY : intersected elements
     * element INTERFACE : positive distance elements owning the surrogate boundary nodes
     */
    void SetInterfaceFlags();

    /**
     * @brief Check if current geometry is split
     * This method checks if current geometry is split from the nodal historical DISTANCE values
     * @param rGeometry Geometry to be checked
     * @return true If split
     * @return false If not split
     */
    bool IsSplit(const GeometryType& rGeometry);

    /**
     * @brief Check if current geometry is in negative side
     * This method checks if current geometry has negative DISTANCE value in all the nodes
     * @param rGeometry Geometry to be checked
     * @return true If all nodes are negative
     * @return false If there is one or more positive nodes
     */
    bool IsNegative(const GeometryType& rGeometry);

    /**
     * @brief Set the nodal distances vector
     * This method saves the nodal historical values of DISTANCE in the provided vector
     * @param rGeometry Geometry from which the nodal values are retrieved
     * @param rNodalDistances Vector container to store the distance values
     */
    void SetNodalDistancesVector(
        const GeometryType& rGeometry,
        Vector& rNodalDistances);

    /**
     * @brief Get the standard modified shape functions factory object
     * This function returns a prototype for the split shape functions calculation from the provided geometry
     * @param rGeometry Input geometry
     * @return ModifiedShapeFunctionsFactoryType Factory to be used for the split shape functions calculation
     */
    ModifiedShapeFunctionsFactoryType GetStandardModifiedShapeFunctionsFactory(const GeometryType& rGeometry);

    /**
     * @brief Get the MLS shape functions and gradients factory object
     * This function returns a prototype for the MLS shape functions and gradients calculation
     * @return MLSShapeFunctionsAndGradientsFunctionType MLS shape functions and gradients call prototype
     */
    MLSShapeFunctionsAndGradientsFunctionType GetMLSShapeFunctionsAndGradientsFunction();

    /**
     * @brief Get the MLS shape functions factory object
     * This function returns a prototype for the MLS shape functions calculation
     * @return MLSShapeFunctionsFunctionType MLS shape functions call prototype
     */
    MLSShapeFunctionsFunctionType GetMLSShapeFunctionsFunction();

    /**
     * @brief Get the RBF shape functions factory object
     * This function returns a prototype for the RBF shape functions calculation
     * @return RBFShapeFunctionsFunctionType RBF shape functions call prototype
     */
    RBFShapeFunctionsFunctionType GetRBFShapeFunctionsFunction();

    /**
     * @brief Get the element size function object
     * This function returns a prototype for the element size calculation call
     * @param rGeometry Geometry to calculate the element size
     * @return ElementSizeFunctionType Element size calculation call
     */
    ElementSizeFunctionType GetElementSizeFunction(const GeometryType& rGeometry);

    /**
     * @brief Set the split element support cloud
     * For a split element, this function creates the cloud of nodes to calculate the extension operator
     * @param rSplitElement Reference to the split element
     * @param rCloudNodes Vector containing pointers to the nodes of the cloud
     * @param rCloudCoordinates Matrix containing the coordinates of the nodes of the cloud
     */
    void SetSplitElementSupportCloud(
        const Element& rSplitElement,
        PointerVector<NodeType>& rCloudNodes,
        Matrix& rCloudCoordinates);

    /**
     * @brief Set the negative node support cloud
     * For a negative node, this function creates the cloud of nodes to calculate the extension operator
     * This function is to be used in the gradient-based extension operator case
     * @param rNegativeNode Reference to the negative element
     * @param rCloudNodes Vector containing pointers to the nodes of the cloud
     * @param rCloudCoordinates Matrix containing the coordinates of the nodes of the cloud
     */
    void SetNegativeNodeSupportCloud(
        const NodeType& rNegativeNode,
        PointerVector<NodeType>& rCloudNodes,
        Matrix& rCloudCoordinates);

    /**
     * @brief Calculates the kernel function radius
     * For the given cloud of points coordinates, this function calculates the maximum distance between
     * the center of the kernel (origin) and the points. This is supposed to be used in the meshless
     * approximants calculation.
     * @param rCloudCoordinates Matrix containing the coordinates of the nodes of the cloud
     * @param rOrigin Coordinates of the point on which the kernel function is centered
     * @return double Kernel function radius
     */
    double CalculateKernelRadius(
        const Matrix& rCloudCoordinates,
        const array_1d<double,3>& rOrigin);

    /**
     * @brief Get the number of required points
     * For the MLS approximants, this function returns the minimum number of required points according to
     * dimension and order. If the cloud of points has less points than the value returned by this function
     * the case is ill-conditioned, meaning that the cloud needs to be enlarged.
     * @return std::size_t Number of required points
     */
    std::size_t GetRequiredNumberOfPoints();

    /**
     * @brief Set the surrogate boundary nodal gradient weights
     * For the gradient-based extension operator case, this function calculates the positive nodal gradients weights.
     * Positive in this case means that, for each node in the surrogate boundary, only the positive elements are considered
     * for the nodal gradients calculation. The results are stored in a map which entry is each one of the surrogate nodes ids.
     * and the value is another map containing the surrounding nodes and their weighted gradients.
     * @return std::map<std::size_t, std::map<std::size_t, Vector>> Map with the nodal gradients for each node in the surrogate boundary
     */
    std::map<std::size_t, std::map<std::size_t, Vector>> SetSurrogateBoundaryNodalGradientWeights();

    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}
}; // Class ShiftedBoundaryMeshlessInterfaceUtility

} // namespace Kratos.
