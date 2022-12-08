//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Aron Noordam
//

// System includes

// External includes

// Project includes
#include "set_moving_load_process.h"

#include <utilities/function_parser_utility.h>
#include <utilities/mortar_utilities.h>

#include "utilities/interval_utility.h"
#include "structural_mechanics_application_variables.h"


namespace Kratos
{

SetMovingLoadProcess::SetMovingLoadProcess(ModelPart& rModelPart,
                                                            Parameters Settings)
                                                            : mrModelPart(rModelPart),
                                                            mParameters(Settings)
{
    Parameters default_parameters(R"(
        {
            "help"            : "This process applies a moving load condition belonging to a modelpart. The load moves over line elements.",
            "model_part_name" : "please_specify_model_part_name",
            "variable_name"   : "POINT_LOAD",
			"is_rotation"     : true,
            "load"            : [0.0, 1.0, 0.0],
            "direction"       : [1,1,1],
            "velocity"        : 1,
			"origin"          : [0.0, 0.0, 0.0]
        }  )"
    );
    Parameters mParameters;
    //IntervalUtility interval_utility(mParameters);

    mParameters.RecursivelyValidateAndAssignDefaults(default_parameters);


	// check if load parameter has size 3
    KRATOS_ERROR_IF(mParameters["load"].size() != 3) <<
        "'load' has to have size 3!" << std::endl;

    // check if all elements in load parameter are either string or a number
    bool is_all_string = true;
    bool is_all_number = true;
    for (unsigned int i = 0; i < mParameters["load"].size(); i++)
    {
        if (!mParameters["load"][i].IsString())
        {
            is_all_string = false;
        }
        if(!mParameters["load"][i].IsNumber())
        {
            is_all_number = false;
        }
    }

    KRATOS_ERROR_IF(!is_all_string && !is_all_number) << "'load' has to be a vector of numbers, or an array with strings" << std::endl;

}


std::vector<int> SetMovingLoadProcess::FindNonRepeatingIndices(std::vector<int> IndicesVector)
{
    // Insert all array elements in hash
    // table
    std::unordered_map<int, int> mp;
    for (int i = 0; i < IndicesVector.size(); i++)
        mp[IndicesVector[i]]++;

    // Traverse through map only and
    std::vector<int> non_repeating_indices;
    for (auto x : mp)
        if (x.second == 1)
            non_repeating_indices.push_back(x.first);

    return non_repeating_indices;
          
}


 bool SetMovingLoadProcess::IsSwapPoints(double first_coord, double second_coord, int direction)
{
    // swap points if points are sorted in opposite order compared to direction
    if ((first_coord < second_coord) && (direction < 0))
    {
        return true;
    }
    if ((first_coord > second_coord) && (direction > 0))
    {
        return true;
    }
    return false;
}


Condition& SetMovingLoadProcess::GetFirstConditionFromCoord(double first_coord, double second_coord, int direction, std::vector<Condition>& end_conditions)
{
    // if center1 coord  < center2 coord and direction is positive
    if ((first_coord < second_coord) && (direction > 0))
    {
        return end_conditions[0];
    }
    // if center1 coord  > center2 coord and direction is negative
    if ((first_coord > second_coord) && (direction < 0))
    {
        return end_conditions[0];
    }
    	return end_conditions[1];

}

Condition& SetMovingLoadProcess::GetFirstCondition(Point first_point, Point second_point, vector<int> direction, std::vector<Condition>& end_conditions)
{
    // sort on x-coord, if x coords are equal, sort on y coord, if y coord is equal sort on z-coord
    if (abs(first_point[0] - second_point[0]) > DBL_EPSILON)
    {
        return GetFirstConditionFromCoord(first_point[0], second_point[0], direction[0], end_conditions);
    }
    if (abs(first_point[1] - second_point[1]) > DBL_EPSILON)
    {
        return GetFirstConditionFromCoord(first_point[1], second_point[1], direction[1], end_conditions);
    }
        return GetFirstConditionFromCoord(first_point[2], second_point[2], direction[2], end_conditions);
}



bool SetMovingLoadProcess::IsConditionReversed(Condition& rCondition, vector<int> direction)
{
    auto& rPoints = rCondition.GetGeometry().Points();
    if (abs(rPoints[0].X0() - rPoints[1].X0()) > DBL_EPSILON)
    {
        return IsSwapPoints(rPoints[0].X0(), rPoints[1].X0(), direction[0]);
    }
    if (abs(rPoints[0].Y0() - rPoints[1].Y0()) > DBL_EPSILON)
    {
        return IsSwapPoints(rPoints[0].Y0(), rPoints[1].Y0(), direction[1]);
    }
    return IsSwapPoints(rPoints[0].Z0(), rPoints[1].Z0(), direction[2]);
}



std::vector<Condition> SetMovingLoadProcess::SortConditions(ModelPart::ConditionsContainerType& unsorted_conditions, Condition& first_condition)
{

    std::vector<Condition> unsorted_conditions_v(unsorted_conditions.begin(), unsorted_conditions.end());

    std::vector<Condition> sorted_conditions;
    std::vector<int> visited_indices;
    GeometricalObject::GeometryType& r_geom_first = first_condition.GetGeometry();
    std::set<IndexType> node_id_vector{ r_geom_first[0].Id(),r_geom_first[1].Id() };

    bool is_cond_reversed = mIsCondReversedVector[0];
    while (visited_indices.size() != unsorted_conditions_v.size())
    {
        for (int i =0; i< unsorted_conditions_v.size(); i++)
        {
            Condition& r_cond = unsorted_conditions_v[i];
            GeometricalObject::GeometryType& r_geom = r_cond.GetGeometry();


            // check if current index is already added to sorted condition vector
            if (!std::count(visited_indices.begin(), visited_indices.end(), i))
            {
                // check if geom has a shared node with previous geom
                if (node_id_vector.find(r_geom.Points()[0].Id()) != node_id_vector.end() || node_id_vector.find(r_geom.Points()[1].Id()) != node_id_vector.end())
                {
                    if (sorted_conditions.size() == 0)
                    {
                        // check if both nodes of geom are equal to nodes in start element, only do this to add the first element in the sorted conditions vector
                        if (node_id_vector.find(r_geom[0].Id()) != node_id_vector.end() && node_id_vector.find(r_geom.Points()[1].Id()) != node_id_vector.end())
                        {
                            node_id_vector = { r_geom[0].Id(),r_geom[1].Id() };
                            sorted_conditions.push_back(r_cond);
                            visited_indices.push_back(i);
                        }
                    }
                    else
                    {
                        // sort nodes in condition, such that new node is connected to previous condition
                        std::set<IndexType>::iterator prev_id;
                        if (is_cond_reversed)
                        {
                            prev_id = node_id_vector.begin();
                        }
                        else
                        {
                            prev_id = node_id_vector.end();
                            --prev_id;
                        }

                        if (*prev_id != r_geom.Points()[0].Id())
                        {
                            is_cond_reversed = true;
                            mIsCondReversedVector.push_back(is_cond_reversed);
                        }
                        else
                        {
                            is_cond_reversed = false;
                            mIsCondReversedVector.push_back(is_cond_reversed);
                        }

                        // add condition to sorted conditions vector
                        node_id_vector = { r_geom[0].Id(),r_geom[1].Id() };
                        sorted_conditions.push_back(r_cond);
                        visited_indices.push_back(i);
                    }
                }
            }
            
        }
    }

    return sorted_conditions;
}

/**
 * \brief Finds condition elements which are at the spatial ends of the conditions vector. This function checks which condition's points are not repeated
 * in this conditions model part. If nodes are not repeated, it means that the condition is at one of the spatial ends. 
 * \return Vector of the two end conditions.
 */
std::vector<Condition> SetMovingLoadProcess::FindEndConditions()
{
    std::vector<int> node_id_vector;
    const array_1d<double, 3> origin_point = mParameters["origin"].GetVector();

    // get all end node ids ( not the middle nodes, in case of line3 conditions)
    // simultaneously check if origin point is on line
    bool condition_is_on_line = false;
    for (auto& r_cond : mrModelPart.Conditions()) {

        auto& r_geom = r_cond.GetGeometry();
        Point local_point;
        if (r_geom.IsInside(origin_point, local_point))
        {
            condition_is_on_line = true;
        }

        node_id_vector.push_back(r_geom[0].Id());
        node_id_vector.push_back(r_geom[1].Id());
    }

    // error if origin point is not on line
    KRATOS_ERROR_IF_NOT(condition_is_on_line) << "Origin point of moving load is not on line" << std::endl;

    // find non repeating node ids
    const std::vector<int> non_repeating_node_ids = FindNonRepeatingIndices(node_id_vector);

    // error if model part does not have 1 end and 1 beginning
    KRATOS_ERROR_IF_NOT(non_repeating_node_ids.size() == 2) << "Moving load condition model part needs to be connected with a beginning and end" << std::endl;

    // find conditions at both ends of model part
    std::vector<Condition> end_conditions;
    for (Condition& r_cond : mrModelPart.Conditions()) {

        auto& r_geom = r_cond.GetGeometry();
        for (int i = 0; i < r_geom.size(); i++)
        {
            for (int j = 0; j < non_repeating_node_ids.size(); j++)
            {
                if (r_geom[i].Id() == non_repeating_node_ids[j])
                {
                    end_conditions.push_back(r_cond);
                }
            }
        }
    }
    return end_conditions;
}


void SetMovingLoadProcess::InitializeDistanceLoadInSortedVector()
{
    double global_distance = 0;
    // loop over sorted conditions
    for (unsigned int i = 0; i < mSortedConditions.size(); ++i)
    {
        auto& r_cond = mSortedConditions[i];
        auto& r_geom = r_cond.GetGeometry();
        const double element_length = r_geom.Length();

        Point local_point;

        // read origin point
        const array_1d<double, 3> origin_point = mParameters["origin"].GetVector();

        // if origin point is within the current condition, set the global distance of the load, else continue the loop
        if (r_geom.IsInside(origin_point, local_point))
        {
            const double local_to_global_distance = (local_point[0] + 1) / 2 * element_length;

            if (mIsCondReversedVector[i])
            {
                mCurrentDistance = global_distance + element_length - local_to_global_distance;
            }
            else
            {
                mCurrentDistance = global_distance + local_to_global_distance;
            }
        }

        // add element length of current condition to the global distance
        global_distance += element_length;
    }
}


void SetMovingLoadProcess::ExecuteInitialize()
{
    KRATOS_TRY

    // clear load functions vector
    mLoadFunctions.clear();

	// check if load input is a function or numeric and add load to member variable
    if (mParameters["load"][0].IsString())
    {
        mUseLoadFunction = true;
        for (unsigned int i = 0; i < mParameters["load"].size(); ++i)
        {
            BasicGenericFunctionUtility load_function = BasicGenericFunctionUtility(mParameters["load"][i].GetString());
            mLoadFunctions.push_back(load_function);
        }
    }
    else
    {
        mUseLoadFunction = false;
    }

    // check if velocity input is a function or numeric and add velocity to member variable
    if (mParameters["velocity"].IsString())
    {
        mUseVelocityFunction = true;
    }
    else
    {
        mUseVelocityFunction = false;
    }
    
    const vector<int> direction = mParameters["direction"].GetVector();

    // get the two line condition elements at both sides of the model part
    std::vector<Condition> end_conditions = FindEndConditions();

    // find start condition 
    const Point center_1 = end_conditions[0].GetGeometry().Center();
    const Point center_2 = end_conditions[1].GetGeometry().Center();
    Condition& r_first_cond = GetFirstCondition(center_1, center_2, direction, end_conditions);

	// Initialise vector which indicates if nodes in condition are in direction of movement
    mIsCondReversedVector.clear();
    mIsCondReversedVector.push_back(IsConditionReversed(r_first_cond, direction));
    mSortedConditions = SortConditions(mrModelPart.Conditions(), r_first_cond);

    InitializeDistanceLoadInSortedVector();
   
    KRATOS_CATCH("")
}


void SetMovingLoadProcess::ExecuteInitializeSolutionStep()
{

    array_1d<double, 3> load_vector;

	// retrieve load from load function if given
    if (mUseLoadFunction)
    {
        // get current time
        const double current_time = this->mrModelPart.GetProcessInfo().GetValue(TIME);

        for (unsigned int i =0; i< mLoadFunctions.size();++i)
        {
            load_vector[i] = mLoadFunctions[i].CallFunction(0, 0, 0, current_time, 0, 0, 0);
        }
    }
    else
    {
	    load_vector = mParameters["load"].GetVector();
    }
    
    double distance_cond = 0;

    // bool to check if load is already added, such that a load is not added twice if the load is exactly at a shared node.
    bool is_moving_load_added = false;

    // loop over sorted conditions vector
    for (unsigned int i = 0; i < mSortedConditions.size(); ++i) 
    {
        auto& r_cond = mSortedConditions[i];
        auto& r_geom = r_cond.GetGeometry();
        const double element_length = r_geom.Length();

        // if moving load is located at current condition element, apply moving load, else apply a zero load
        if ((distance_cond + element_length >= mCurrentDistance) && (distance_cond <= mCurrentDistance) && !is_moving_load_added)
        {
            double local_distance;
            if (mIsCondReversedVector[i])
            {
                local_distance = distance_cond + element_length - mCurrentDistance;
            }
            else
            {
                local_distance = mCurrentDistance - distance_cond;
            }
            
            r_cond.SetValue(POINT_LOAD, load_vector);

            // distance is correct assuming nodes in condition are correctly sorted, the sorting is done while initializing this process
            r_cond.SetValue(MOVING_LOAD_LOCAL_DISTANCE, local_distance);
            is_moving_load_added = true;
        }
        else
        {
            r_cond.SetValue(POINT_LOAD, ZeroVector(3));
            r_cond.SetValue(MOVING_LOAD_LOCAL_DISTANCE, 0);
        }
        distance_cond += element_length;
    }
}



void SetMovingLoadProcess::ExecuteFinalizeSolutionStep()
{
    double load_velocity;
    // retrieve load velocity from velocity function if given
    if (mUseVelocityFunction)
    {
        // get current time
        const double current_time = this->mrModelPart.GetProcessInfo().GetValue(TIME);

        BasicGenericFunctionUtility velocity_function = BasicGenericFunctionUtility(mParameters["velocity"].GetString());

		// update velocity value
        load_velocity = velocity_function.CallFunction(0, 0, 0, current_time, 0, 0, 0);
    }
    else
    {
        load_velocity = mParameters["velocity"].GetDouble();
    }

    // move the load
    mCurrentDistance = mCurrentDistance + mrModelPart.GetProcessInfo().GetValue(DELTA_TIME) * load_velocity;
}

}  // namespace Kratos.
