//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Vicente Mataix Ferrandiz
//

#pragma once

// System includes

// External includes

// Project includes
#include "geometries/point.h"
#include "includes/kratos_parameters.h"
#include "spatial_containers/spatial_search.h"
#include "utilities/parallel_utilities.h"

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

/**
 * @brief This enum defines the different spatial containers available
 * @details The different spatial containers available are:
 * - KDTree: This is a tree based on the k-d tree algorithm
 * - Octree: This is a tree based on the octree algorithm
 * - BinsStatic: This is a bin based on the static bins algorithm
 * - BinsDynamic: This is a bin based on the dynamic bins algorithm
 * - BinsStaticObjects: This is a bin based on the static bins algorithm, but with objects
 * - BinsDynamicObjects: This is a bin based on the dynamic bins algorithm, but with objects
 */
enum class SpatialContainer
{
    KDTree,
    Octree,
    BinsStatic,
    BinsDynamic//,
    // BinsStaticObjects, // NOTE: These can be considered in the future for the Inclusive methods
    // BinsDynamicObjects
};

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/**
 * @class PointObject
 * @ingroup KratosCore
 * @brief Custom Point container to be used by the search
 * @details It stores the pointer of a certain object
 * @author Vicente Mataix Ferrandiz
 */
template<class TObject>
class PointObject
    : public Point
{
public:

    ///@name Type Definitions
    ///@{

    /// Base class definition
    typedef Point BaseType;

    /// Counted pointer of PointObject
    KRATOS_CLASS_POINTER_DEFINITION( PointObject );

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructors
    PointObject():
        BaseType()
    {
    }

    /**
     * @brief Constructor with object
     * @param pObject The pointer to the object
     */
    PointObject(typename TObject::Pointer pObject):
        mpObject(pObject)
    {
        UpdatePoint();
    }

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief This function updates the database, using as base for the coordinates the condition center
     */
    void UpdatePoint();

    ///@}
    ///@name Acess
    ///@{

    /**
     * @brief Returns the geometry associated to the point
     * @return mrGeometry The reference to the geometry associated to the point
     */
    typename TObject::Pointer pGetObject()
    {
        return mpObject;
    }

    /**
     * @brief Sets the object associated to the point
     * @param pObject The pointer to the object
     */
    void pSetObject(typename TObject::Pointer pObject)
    {
        mpObject = pObject;
        UpdatePoint();
    }

    ///@}
private:
    ///@}
    ///@name Member Variables
    ///@{

    typename TObject::Pointer mpObject = nullptr;

    ///@}

}; // Class PointObject

/**
* @class SpecializedSpatialSearch
* @ingroup KratosCore
* @brief This class is used to search for elements, conditions and nodes in a given model part
* @details In order to perform the search it uses as backend some of the the spatial containers defined `spatial_containers` folder
* @tparam TSearchBackend The spatial container to be used as backend
* @author Vicente Mataix Ferrandiz
*/
template<SpatialContainer TSearchBackend>
class KRATOS_API(KRATOS_CORE) SpecializedSpatialSearch
    : public SpatialSearch
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of SpecializedSpatialSearch
    KRATOS_CLASS_POINTER_DEFINITION(SpecializedSpatialSearch);

    /// The base type
    using BaseType = SpatialSearch;

    /// Common Defines
    using BaseType::PointType;

    using BaseType::ElementsContainerType;
    using BaseType::ElementType;
    using BaseType::ElementPointerType;
    using BaseType::ResultElementsContainerType;
    using BaseType::VectorResultElementsContainerType;

    using BaseType::NodesContainerType;
    using BaseType::NodeType;
    using BaseType::NodePointerType;
    using BaseType::ResultNodesContainerType;
    using BaseType::VectorResultNodesContainerType;

    using BaseType::ConditionsContainerType;
    using BaseType::ConditionType;
    using BaseType::ConditionPointerType;
    using BaseType::ResultConditionsContainerType;
    using BaseType::VectorResultConditionsContainerType;

    using BaseType::RadiusArrayType;
    using BaseType::DistanceType;
    using BaseType::VectorDistanceType;

    using BaseType::ResultIteratorType;

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * @brief Default constructor.
     */
    SpecializedSpatialSearch()
    {
        // We asssign default parameters
        mParameters = GetDefaultParameters();
    }

    /**
     * @brief Constructor with parameters
     * @param ThisParameters The parameters to be considered
     */
    SpecializedSpatialSearch(Parameters ThisParameters)
        : mParameters(ThisParameters)
    {
        // We asssign default parameters
        const Parameters default_parameters = GetDefaultParameters();

        // We update the parameters
        mParameters.RecursivelyValidateAndAssignDefaults(default_parameters);
    }

    /// Destructor.
    ~SpecializedSpatialSearch() override = default;

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    /**
    * @brief Search neighbours for every element in "rInputElements" excluding itself
    * @param rStructureElements   List of elements against which the neighbours are searched
    * @param rInputElements       List of elements to be searched
    * @param rRadius              List of search radius for every element
    * @param rResults             Array of results for each element
    * @param rResultDistance      Array of distances for each result of each element
    */
    void SearchElementsInRadiusExclusive(
        const ElementsContainerType& rStructureElements,
        const ElementsContainerType& rInputElements,
        const RadiusArrayType& rRadius,
        VectorResultElementsContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override;

    /**
    * @brief Search neighbours for every element in "rInputElements" excluding itself
    * @param rStructureElements   List of elements against which the neighbours are searched
    * @param rInputElements       List of elements to be searched
    * @param rRadius              List of search radius for every element
    * @param rResults             Array of results for each element
    */
    void SearchElementsInRadiusExclusive(
        const ElementsContainerType& rStructureElements,
        const ElementsContainerType& rInputElements,
        const RadiusArrayType& rRadius,
        VectorResultElementsContainerType& rResults
        ) override;

    /**
    * @brief Search neighbours for every node in "rInputNodes" excluding itself
    * @param rModelPart          List of nodes against which the neighbours are searched
    * @param rInputNodes          List of nodes to be searched
    * @param rRadius             List of search radius for every node
    * @param rResults            Array of results for each node
    * @param rResultDistance     Array of distances for each result of each node
    */
    void SearchNodesInRadiusExclusive(
        const NodesContainerType& rStructureNodes,
        const NodesContainerType& rInputNodes,
        const RadiusArrayType& rRadius,
        VectorResultNodesContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override;

    /**
    * @brief Search neighbours for every node in "rInputNodes" excluding itself
    * @param rModelPart          List of nodes against which the neighbours are searched
    * @param rInputNodes          List of nodes to be searched
    * @param rRadius              List of search radius for every node
    * @param rResults            Array of results for each node
    */
    void SearchNodesInRadiusExclusive(
        const NodesContainerType& rStructureNodes,
        const NodesContainerType& rInputNodes,
        const RadiusArrayType& rRadius,
        VectorResultNodesContainerType& rResults
        ) override;

    /**
    * @brief Search neighbours for every Condition in "rInputConditions" excluding itself
    * @param rStructureConditions   List of Conditions against which the neighbours are searched
    * @param rInputConditions       List of Conditions to be searched
    * @param rRadius                List of search radius for every Condition
    * @param rResults               Array of results for each Condition
    * @param rResultDistance        Array of distances for each result of each Condition
    */
    void SearchConditionsInRadiusExclusive(
        const ConditionsContainerType& rStructureConditions,
        const ConditionsContainerType& rInputConditions,
        const RadiusArrayType& rRadius,
        VectorResultConditionsContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override;

    /**
    * @brief Search neighbours for every Condition in "rInputConditions" excluding itself
    * @param rStructureConditions   List of Conditions against which the neighbours are searched
    * @param rInputConditions       List of Conditions to be searched
    * @param rRadius                List of search radius for every Condition
    * @param rResults               Array of results for each Condition
    */
    void SearchConditionsInRadiusExclusive(
        const ConditionsContainerType& rStructureConditions,
        const ConditionsContainerType& rInputConditions,
        const RadiusArrayType& rRadius,
        VectorResultConditionsContainerType& rResults
        ) override;

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        std::stringstream buffer;
        buffer << "SpecializedSpatialSearch" ;

        return buffer.str();
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "SpecializedSpatialSearch";
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {

    }

    ///@}
    ///@name Friends
    ///@{

    ///@}
protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    Parameters mParameters; /// The configuration parameters

    ///@}
    ///@name Protected Operations
    ///@{

    /**
     * @brief This method returns the default parameters
     * @return The default parameters
     */
    Parameters GetDefaultParameters() const;

    ///@}
private:
    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Member Variables
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    /**
     * @brief This method prepares the search
     * @param rStructure The structure to be searched
     * @param rInput The input to be searched
     * @param rResults The results
     * @param rResultsDistance The results distance
     * @tparam TContainer The container type
     * @tparam TResultType The result type
     */
    template<class TContainer, class TResultType>
    std::vector<typename PointObject<typename TContainer::value_type>::Pointer> PrepareSearch(
        const TContainer& rStructure,
        const TContainer& rInput,
        TResultType& rResults,
        VectorDistanceType& rResultsDistance
        )
    {
        // Some definitions
        using ObjectType = typename TContainer::value_type;
        using PointType = PointObject<ObjectType>;
        using PointTypePointer = typename PointType::Pointer;
        using PointVector = std::vector<PointTypePointer>;

        // Creating the points
        PointVector points;
        const std::size_t structure_size = rStructure.size();
        points.reserve(structure_size);
        const auto it_begin = rStructure.begin();
        for (std::size_t i = 0; i < structure_size; ++i) {
            auto it = it_begin + i;
            points.push_back(PointTypePointer(new PointType(*(it.base()))));
        }

        // Resizing the results
        const std::size_t input_size = rInput.size();
        if (rResults.size() != input_size) {
            rResults.resize(input_size);
        }
        if (rResultsDistance.size() != input_size) {
            rResultsDistance.resize(input_size);
        }

        return points;
    }

    /**
     * @brief This method performs the search in parallel
     * @param rInput The input container
     * @param rRadius The radius array
     * @param rSearch The spatial search
     * @param rResults The results
     * @param rResultsDistance The results distance
     * @tparam TContainer The container type
     * @tparam TSpatialContainer The spatial container type
     * @tparam TResultType The result type
     */
    template<class TContainer, class TSpatialContainer, class TResultType>
    void ParallelSearch(
        const TContainer& rInput,
        const RadiusArrayType& rRadius,
        TSpatialContainer& rSearch,
        TResultType& rResults,
        VectorDistanceType& rResultsDistance
        )
    {
        // Some definitions
        using ObjectType = typename TContainer::value_type;
        using PointType = PointObject<ObjectType>;
        using PointTypePointer = typename PointType::Pointer;
        using PointVector = std::vector<PointTypePointer>;
        using DistanceVector = std::vector<double>;
        const std::size_t input_size = rInput.size();

        // Retrieving parameters
        const int allocation_size = mParameters["allocation_size"].GetInt();

        // Performing search
        IndexPartition<std::size_t>(input_size).for_each([&](std::size_t i) {
            auto it = rInput.begin() + i;
            PointType aux_point(*(it.base()));
            PointVector results(allocation_size);
            DistanceVector results_distances(allocation_size);
            const std::size_t number_of_results = rSearch.SearchInRadius(aux_point, rRadius[i], results.begin(), results_distances.begin(), allocation_size);
            if (number_of_results > 0) {
                auto& r_results = rResults[i];
                auto& r_results_distance = rResultsDistance[i];
                r_results.reserve(number_of_results);
                r_results_distance.reserve(number_of_results);
                for (std::size_t j = 0; j < number_of_results; ++j) {
                    auto p_point = results[j];
                    r_results.push_back(p_point->pGetObject());
                    r_results_distance.push_back(results_distances[j]);
                }
            }
        });
    }
    
    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    SpecializedSpatialSearch& operator=(SpecializedSpatialSearch const& rOther)
    {
        return *this;
    }

    /// Copy constructor.
    SpecializedSpatialSearch(SpecializedSpatialSearch const& rOther)
    {
        *this = rOther;
    }

    ///@}
}; // Class SpecializedSpatialSearch

/**
* @class SpecializedSpatialSearchFactory
* @ingroup KratosCore
* @brief Factory for the specialized spatial search
* @author Vicente Mataix Ferrandiz
*/
class SpecializedSpatialSearchFactory 
    : public SpatialSearch
{
public:
    ///@name Type Definitions
    ///@{

    KRATOS_CLASS_POINTER_DEFINITION(SpecializedSpatialSearchFactory);

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    SpecializedSpatialSearchFactory()
    {
        Parameters default_parameters = GetDefaultParameters();
        Parameters search_parameters(default_parameters["search_parameters"].WriteJsonString());
        mpSpatialSearch = SpatialSearch::Pointer(new SpecializedSpatialSearch<SpatialContainer::KDTree>(search_parameters));
    }

    /// Constructor with parameters
    SpecializedSpatialSearchFactory(Parameters ThisParameters)
    {
        Parameters default_parameters = GetDefaultParameters();
        ThisParameters.RecursivelyValidateAndAssignDefaults(default_parameters);
        const std::string& r_container_type = ThisParameters["container_type"].GetString();
        Parameters search_parameters(ThisParameters["search_parameters"].WriteJsonString());
        if (r_container_type == "KDTree" || r_container_type == "kd_tree") {
            mpSpatialSearch = SpatialSearch::Pointer(new SpecializedSpatialSearch<SpatialContainer::KDTree>(search_parameters));
        } else if (r_container_type == "Octree" || r_container_type == "octree") {
            mpSpatialSearch = SpatialSearch::Pointer(new SpecializedSpatialSearch<SpatialContainer::Octree>(search_parameters));
        } else if (r_container_type == "BinsStatic" || r_container_type == "bins_static") {
            mpSpatialSearch = SpatialSearch::Pointer(new SpecializedSpatialSearch<SpatialContainer::BinsStatic>(search_parameters));
        } else if (r_container_type == "BinsDynamic" || r_container_type == "bins_dynamic") {
            mpSpatialSearch = SpatialSearch::Pointer(new SpecializedSpatialSearch<SpatialContainer::BinsDynamic>(search_parameters));
        } else {
            KRATOS_ERROR << "Unknown container type: " << r_container_type << std::endl;
        }
    }

    /// Destructor.
    ~SpecializedSpatialSearchFactory() override = default;

    ///@}
    ///@name Operations
    ///@{

    /**
    * @brief Search neighbours for every element in "rInputElements" excluding itself
    * @param rStructureElements   List of elements against which the neighbours are searched
    * @param rInputElements       List of elements to be searched
    * @param rRadius              List of search radius for every element
    * @param rResults             Array of results for each element
    * @param rResultDistance      Array of distances for each result of each element
    */
    void SearchElementsInRadiusExclusive(
        const ElementsContainerType& rStructureElements,
        const ElementsContainerType& rInputElements,
        const RadiusArrayType& rRadius,
        VectorResultElementsContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override
    {
        mpSpatialSearch->SearchElementsInRadiusExclusive(rStructureElements, rInputElements, rRadius, rResults, rResultsDistance);
    }

    /**
    * @brief Search neighbours for every element in "rInputElements" excluding itself
    * @param rStructureElements   List of elements against which the neighbours are searched
    * @param rInputElements       List of elements to be searched
    * @param rRadius              List of search radius for every element
    * @param rResults             Array of results for each element
    */
    void SearchElementsInRadiusExclusive(
        const ElementsContainerType& rStructureElements,
        const ElementsContainerType& rInputElements,
        const RadiusArrayType& rRadius,
        VectorResultElementsContainerType& rResults
        ) override
    {
        mpSpatialSearch->SearchElementsInRadiusExclusive(rStructureElements, rInputElements, rRadius, rResults);
    }

    /**
    * @brief Search neighbours for every node in "rInputNodes" excluding itself
    * @param rModelPart          List of nodes against which the neighbours are searched
    * @param rInputNodes          List of nodes to be searched
    * @param rRadius             List of search radius for every node
    * @param rResults            Array of results for each node
    * @param rResultDistance     Array of distances for each result of each node
    */
    void SearchNodesInRadiusExclusive(
        const NodesContainerType& rStructureNodes,
        const NodesContainerType& rInputNodes,
        const RadiusArrayType& rRadius,
        VectorResultNodesContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override
    {
        mpSpatialSearch->SearchNodesInRadiusExclusive(rStructureNodes, rInputNodes, rRadius, rResults, rResultsDistance);
    }

    /**
    * @brief Search neighbours for every node in "rInputNodes" excluding itself
    * @param rModelPart          List of nodes against which the neighbours are searched
    * @param rInputNodes          List of nodes to be searched
    * @param rRadius              List of search radius for every node
    * @param rResults            Array of results for each node
    */
    void SearchNodesInRadiusExclusive(
        const NodesContainerType& rStructureNodes,
        const NodesContainerType& rInputNodes,
        const RadiusArrayType& rRadius,
        VectorResultNodesContainerType& rResults
        ) override
    {
        mpSpatialSearch->SearchNodesInRadiusExclusive(rStructureNodes, rInputNodes, rRadius, rResults);
    }

    /**
    * @brief Search neighbours for every Condition in "rInputConditions" excluding itself
    * @param rStructureConditions   List of Conditions against which the neighbours are searched
    * @param rInputConditions       List of Conditions to be searched
    * @param rRadius                List of search radius for every Condition
    * @param rResults               Array of results for each Condition
    * @param rResultDistance        Array of distances for each result of each Condition
    */
    void SearchConditionsInRadiusExclusive(
        const ConditionsContainerType& rStructureConditions,
        const ConditionsContainerType& rInputConditions,
        const RadiusArrayType& rRadius,
        VectorResultConditionsContainerType& rResults,
        VectorDistanceType& rResultsDistance
        ) override
    {
        mpSpatialSearch->SearchConditionsInRadiusExclusive(rStructureConditions, rInputConditions, rRadius, rResults, rResultsDistance);
    }

    /**
    * @brief Search neighbours for every Condition in "rInputConditions" excluding itself
    * @param rStructureConditions   List of Conditions against which the neighbours are searched
    * @param rInputConditions       List of Conditions to be searched
    * @param rRadius                List of search radius for every Condition
    * @param rResults               Array of results for each Condition
    */
    void SearchConditionsInRadiusExclusive(
        const ConditionsContainerType& rStructureConditions,
        const ConditionsContainerType& rInputConditions,
        const RadiusArrayType& rRadius,
        VectorResultConditionsContainerType& rResults
        ) override
    {
        mpSpatialSearch->SearchConditionsInRadiusExclusive(rStructureConditions, rInputConditions, rRadius, rResults);
    }

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        std::stringstream buffer;
        buffer << "SpecializedSpatialSearchFactory" ;

        return buffer.str();
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "SpecializedSpatialSearchFactory";
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {

    }

    ///@}
private:
    ///@name Member Variables
    ///@{

    SpatialSearch::Pointer mpSpatialSearch = nullptr; /// The spatial search object

    ///@}
    ///@name Private Operations
    ///@{

    /**
     * @brief This method returns the default parameters
     * @return The default parameters
     */
    Parameters GetDefaultParameters() const
    {
        Parameters default_parameters = Parameters(R"(
        {   "container_type"  : "KDTree",
            "search_parameters" : {
                "bucket_size"     : 4,
                "allocation_size" : 1000
            }
        })" );

        return default_parameters;
    }

    ///@}
}; // Class SpecializedSpatialSearchFactory

///@}
///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

/// output stream function
template<SpatialContainer TSearchBackend>
inline std::ostream& operator << (std::ostream& rOStream,
                const SpecializedSpatialSearch<TSearchBackend>& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}

/// output stream function
inline std::ostream& operator << (std::ostream& rOStream,
                const SpecializedSpatialSearchFactory& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}

///@}

///@}addtogroup block

}  // namespace Kratos.