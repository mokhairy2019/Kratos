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
//                   Ruben Zorrilla
//                   Vicente Mataix Ferrandiz
//
//

/* System includes */
#include <functional>
#include <unordered_set>

/* External includes */

/* Project includes */
#include "utilities/openmp_utils.h"
#include "utilities/variable_utils.h"

namespace Kratos
{

ModelPart::NodesContainerType VariableUtils::SelectNodeList(
    const DoubleVarType& Variable,
    const double Value,
    const NodesContainerType& rOriginNodes
    )
{
    KRATOS_TRY

    NodesContainerType selected_nodes;
    for (auto it_node = rOriginNodes.begin(); it_node != rOriginNodes.end(); ++it_node) {
        if (std::abs(it_node->FastGetSolutionStepValue(Variable) - Value) <
            std::numeric_limits<double>::epsilon()) {
            selected_nodes.push_back(*(it_node.base()));
        }
    }

    return selected_nodes;

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

array_1d<double, 3> VariableUtils::SumNonHistoricalNodeVectorVariable(
    const ArrayVarType& rVar,
    const ModelPart& rModelPart
    )
{
    KRATOS_TRY

    array_1d<double, 3> sum_value = ZeroVector(3);
    auto& r_comm = rModelPart.GetCommunicator();

    sum_value = block_for_each<SumReduction<array_1d<double,3>>>(r_comm.LocalMesh().Nodes(),[&](NodeType& rNode){
        return rNode.GetValue(rVar);
    });

    return r_comm.GetDataCommunicator().SumAll(sum_value);

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

array_1d<double, 3> VariableUtils::SumConditionVectorVariable(
    const ArrayVarType& rVar,
    const ModelPart& rModelPart
    )
{
    KRATOS_TRY

    array_1d<double, 3> sum_value = ZeroVector(3);
    auto& r_comm = rModelPart.GetCommunicator();

    sum_value = block_for_each<SumReduction<array_1d<double,3>>>(r_comm.LocalMesh().Conditions(),[&](ConditionType& rCond){
        return rCond.GetValue(rVar);
    });

    return r_comm.GetDataCommunicator().SumAll(sum_value);

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

array_1d<double, 3> VariableUtils::SumElementVectorVariable(
    const ArrayVarType& rVar,
    const ModelPart& rModelPart
    )
{
    KRATOS_TRY

    array_1d<double, 3> sum_value = ZeroVector(3);
    auto& r_comm = rModelPart.GetCommunicator();

    sum_value = block_for_each<SumReduction<array_1d<double,3>>>(r_comm.LocalMesh().Elements(),[&](ElementType& rElem){
        return rElem.GetValue(rVar);
    });

    return r_comm.GetDataCommunicator().SumAll(sum_value);

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void VariableUtils::AddDofsList(
    const std::vector<std::string>& rDofsVarNamesList,
    ModelPart& rModelPart)
{
    // Create a set with the variables to be added as DOFs
    std::set<const Variable<double>*> dofs_vars_set;
    const IndexType n_dofs = rDofsVarNamesList.size();
    for (IndexType i = 0; i < n_dofs; ++i) {
        const std::string r_var_name = rDofsVarNamesList[i];
        KRATOS_ERROR_IF_NOT(KratosComponents<Variable<double>>::Has(r_var_name)) << "Provided variable \'" << r_var_name << "\' is not in KratosComponents." << std::endl;
        const auto& r_dof_var = KratosComponents<Variable<double>>::Get(r_var_name);
        const Variable<double>* p_dof_var = &r_dof_var;
        dofs_vars_set.insert(p_dof_var);
        rModelPart.GetNodalSolutionStepVariablesList().AddDof(&r_dof_var);
    }

    // Add the DOFs to the model part nodes
    block_for_each(rModelPart.Nodes(), [&dofs_vars_set](Node<3>& rNode){
        for (auto p_var : dofs_vars_set) {
            rNode.AddDof(*p_var);
        }
    });
}

/***********************************************************************************/
/***********************************************************************************/

void VariableUtils::AddDofsWithReactionsList(
    const std::vector<std::array<std::string,2>>& rDofsAndReactionsNamesList,
    ModelPart& rModelPart)
{
    // Create a set with the variables to be added as DOFs and reactions
    std::array<const Variable<double>*,2> aux_dof_vars;
    const IndexType n_dofs = rDofsAndReactionsNamesList.size();
    std::set<std::array<const Variable<double>*, 2>> dofs_and_react_vars_set;
    for (IndexType i = 0; i < n_dofs; ++i) {
        const auto& r_dof_data = rDofsAndReactionsNamesList[i];
        const std::string r_var_name = r_dof_data[0];
        const std::string r_react_name = r_dof_data[1];
        KRATOS_ERROR_IF_NOT(KratosComponents<Variable<double>>::Has(r_var_name)) << "Provided variable \'" << r_var_name << "\' is not in KratosComponents." << std::endl;
        KRATOS_ERROR_IF_NOT(KratosComponents<Variable<double>>::Has(r_react_name)) << "Provided reaction \'" << r_react_name << "\' is not in KratosComponents." << std::endl;
        const auto& r_dof_var = KratosComponents<Variable<double>>::Get(r_var_name);
        const auto& r_react_var = KratosComponents<Variable<double>>::Get(r_react_name);
        aux_dof_vars[0] = &r_dof_var;
        aux_dof_vars[1] = &r_react_var;
        dofs_and_react_vars_set.insert(aux_dof_vars);

        rModelPart.GetNodalSolutionStepVariablesList().AddDof(&r_dof_var, &r_react_var);
    }

    // Add the DOFs and reactions to the model part nodes
    block_for_each(rModelPart.Nodes(), [&dofs_and_react_vars_set](Node<3>& rNode){
        for (auto& r_dof_data : dofs_and_react_vars_set) {
            rNode.AddDof(*(r_dof_data[0]), *(r_dof_data[1]));
        }
    });
}

/***********************************************************************************/
/***********************************************************************************/

std::vector<std::string> VariableUtils::GetDofsListFromSpecifications(const ModelPart& rModelPart)
{
    // Get DOFs variable names list from elements and conditions
    const auto elem_dofs_list = GetDofsListFromElementsSpecifications(rModelPart);
    const auto cond_dofs_list = GetDofsListFromConditionsSpecifications(rModelPart);

    // Remove duplicates
    std::vector<std::string> all_dofs;
    std::merge(elem_dofs_list.begin(), elem_dofs_list.end(), cond_dofs_list.begin(), cond_dofs_list.end(), std::back_inserter(all_dofs));
    std::sort(all_dofs.begin(),all_dofs.end());
    all_dofs.erase(std::unique(all_dofs.begin(),all_dofs.end()), all_dofs.end());

    return all_dofs;
}

/***********************************************************************************/
/***********************************************************************************/

std::vector<std::string> VariableUtils::GetDofsListFromElementsSpecifications(const ModelPart& rModelPart)
{
    KRATOS_ERROR_IF(rModelPart.IsDistributed()) << "This method is not MPI-compatible yet." << std::endl;
    return GetDofsListFromGenericEntitiesSpecifications(rModelPart.Elements());
}

/***********************************************************************************/
/***********************************************************************************/

std::vector<std::string> VariableUtils::GetDofsListFromConditionsSpecifications(const ModelPart& rModelPart)
{
    KRATOS_ERROR_IF(rModelPart.IsDistributed()) << "This method is not MPI-compatible yet." << std::endl;
    return GetDofsListFromGenericEntitiesSpecifications(rModelPart.Conditions());
}

/***********************************************************************************/
/***********************************************************************************/

template< class TContainerType>
std::vector<std::string> VariableUtils::GetDofsListFromGenericEntitiesSpecifications(const TContainerType& rContainer)
{
    // Create a set with the DOFs variables in the container entities specifications
    std::unordered_set<std::string> dofs_var_names_set;
    const std::size_t n_entities = rContainer.size();
    for (std::size_t i = 0; i < n_entities; ++i) {
        const auto it_entity = rContainer.begin() + i;
        const auto specifications = it_entity->GetSpecifications();
        const auto required_dofs = specifications["required_dofs"].GetStringArray();
        for (std::size_t i_dof = 0; i_dof < required_dofs.size(); ++i_dof) {
            dofs_var_names_set.insert(required_dofs[i_dof]);
        }
    }
    KRATOS_WARNING_IF("GetDofsListFromGenericEntitiesSpecifications", n_entities > 0 && dofs_var_names_set.empty())
        << "DOFs variables set is empty. Check and complete your element/condition GetSpecifications() implementation." << std::endl;

    // Check that all the DOFs variables exist
    for (auto& r_var_name : dofs_var_names_set) {
        KRATOS_ERROR_IF_NOT(KratosComponents<Variable<double>>::Has(r_var_name))
            << "DOF \'" << r_var_name << "\' is not in KratosComponents. Check your element/condition GetSpecifications() implementation." << std::endl;
    }

    // Return a list with the DOFs variables
    std::vector<std::string> dofs_var_names_list;
    dofs_var_names_list.insert(dofs_var_names_list.end(), dofs_var_names_set.begin(), dofs_var_names_set.end());

    return dofs_var_names_list;
}

/***********************************************************************************/
/***********************************************************************************/

bool VariableUtils::CheckVariableKeys()
{
    KRATOS_TRY

    CheckVariableKeysHelper< Variable<double> >();
    CheckVariableKeysHelper< Variable<array_1d<double,3> > >();
    CheckVariableKeysHelper< Variable<array_1d<double,4> > >();
    CheckVariableKeysHelper< Variable<array_1d<double,6> > >();
    CheckVariableKeysHelper< Variable<array_1d<double,9> > >();
    CheckVariableKeysHelper< Variable<bool> >();
    CheckVariableKeysHelper< Variable<int> >();
    CheckVariableKeysHelper< Variable<unsigned int> >();
    CheckVariableKeysHelper< Variable<Vector> >();
    CheckVariableKeysHelper< Variable<Matrix> >();

    return true;

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void VariableUtils::UpdateCurrentToInitialConfiguration(const ModelPart::NodesContainerType& rNodes)
{
    KRATOS_TRY;

    block_for_each(rNodes, [&](Node<3>& rNode){
        noalias(rNode.Coordinates()) = rNode.GetInitialPosition();
    });

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void VariableUtils::UpdateInitialToCurrentConfiguration(const ModelPart::NodesContainerType& rNodes)
{
    KRATOS_TRY;

    block_for_each(rNodes, [&](Node<3>& rNode){
        noalias(rNode.GetInitialPosition().Coordinates()) = rNode.Coordinates();
    });

    KRATOS_CATCH("");
}

/***********************************************************************************/
/***********************************************************************************/

void VariableUtils::UpdateCurrentPosition(
    const ModelPart::NodesContainerType& rNodes,
    const ArrayVarType& rUpdateVariable,
    const IndexType BufferPosition
    )
{
    KRATOS_TRY;

    block_for_each(rNodes, [&](Node<3>& rNode){
        noalias(rNode.Coordinates()) = (rNode.GetInitialPosition()).Coordinates() + rNode.FastGetSolutionStepValue(rUpdateVariable, BufferPosition);
    });

    KRATOS_CATCH("");
}

template <>
KRATOS_API(KRATOS_CORE) ModelPart::NodesContainerType& VariableUtils::GetContainer<ModelPart::NodesContainerType>(ModelPart& rModelPart)
{
    return rModelPart.Nodes();
}


template <>
KRATOS_API(KRATOS_CORE) ModelPart::ElementsContainerType& VariableUtils::GetContainer<ModelPart::ElementsContainerType>(ModelPart& rModelPart)
{
    return rModelPart.Elements();
}

template <>
KRATOS_API(KRATOS_CORE) ModelPart::ConditionsContainerType& VariableUtils::GetContainer<ModelPart::ConditionsContainerType>(ModelPart& rModelPart)
{
    return rModelPart.Conditions();
}

template <>
KRATOS_API(KRATOS_CORE) const ModelPart::NodesContainerType& VariableUtils::GetContainer<ModelPart::NodesContainerType>(const ModelPart& rModelPart)
{
    return rModelPart.Nodes();
}

template <>
KRATOS_API(KRATOS_CORE) const ModelPart::ElementsContainerType& VariableUtils::GetContainer<ModelPart::ElementsContainerType>(const ModelPart& rModelPart)
{
    return rModelPart.Elements();
}

template <>
KRATOS_API(KRATOS_CORE) const ModelPart::ConditionsContainerType& VariableUtils::GetContainer<ModelPart::ConditionsContainerType>(const ModelPart& rModelPart)
{
    return rModelPart.Conditions();
}

template <class TDataType, class TContainerType, class TWeightDataType>
void VariableUtils::WeightedAccumulateVariableOnNodes(
    ModelPart& rModelPart,
    const Variable<TDataType>& rVariable,
    const Variable<TWeightDataType>& rWeightVariable,
    const bool IsInverseWeightProvided)
{
    KRATOS_TRY

    SetNonHistoricalVariableToZero(rVariable, rModelPart.Nodes());

    auto& r_entities = GetContainer<TContainerType>(rModelPart);
    const int n_entities = r_entities.size();

    const std::function<double(const Node<3>&)>& r_weight_method =
        (IsInverseWeightProvided) ?
        static_cast<std::function<double(const Node<3>&)>>([rWeightVariable](const Node<3>& rNode) -> double {return 1.0 / rNode.GetValue(rWeightVariable);}) :
        static_cast<std::function<double(const Node<3>&)>>([rWeightVariable](const Node<3>& rNode) -> double {return rNode.GetValue(rWeightVariable);});

#pragma omp parallel for
    for (int i_entity = 0; i_entity < n_entities; ++i_entity)
    {
        auto it_entity = r_entities.begin() + i_entity;
        auto& r_geometry = it_entity->GetGeometry();

        const auto& r_value = it_entity->GetValue(rVariable);
        for (int i_node = 0; i_node < static_cast<int>(r_geometry.PointsNumber()); ++i_node)
        {
            auto& r_node = r_geometry[i_node];

            KRATOS_DEBUG_ERROR_IF(!r_node.Has(rWeightVariable))
                << "Non-historical nodal " << rWeightVariable.Name() << " at "
                << r_node << " is not initialized in " << rModelPart.Name()
                << ". Please initialize it first.";

            const double weight = r_weight_method(r_node);

            r_node.SetLock();
            r_node.GetValue(rVariable) += r_value * weight;
            r_node.UnSetLock();
        }
    }

    rModelPart.GetCommunicator().AssembleNonHistoricalData(rVariable);

    KRATOS_CATCH("");
}

// template instantiations
template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<double, ModelPart::ConditionsContainerType, int>(
    ModelPart&, const Variable<double>&, const Variable<int>&, const bool);
template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<array_1d<double, 3>, ModelPart::ConditionsContainerType, int>(
    ModelPart&, const Variable<array_1d<double, 3>>&, const Variable<int>&, const bool);

template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<double, ModelPart::ElementsContainerType, int>(
    ModelPart&, const Variable<double>&, const Variable<int>&, const bool);
template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<array_1d<double, 3>, ModelPart::ElementsContainerType, int>(
    ModelPart&, const Variable<array_1d<double, 3>>&, const Variable<int>&, const bool);

template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<double, ModelPart::ConditionsContainerType, double>(
    ModelPart&, const Variable<double>&, const Variable<double>&, const bool);
template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<array_1d<double, 3>, ModelPart::ConditionsContainerType, double>(
    ModelPart&, const Variable<array_1d<double, 3>>&, const Variable<double>&, const bool);

template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<double, ModelPart::ElementsContainerType, double>(
    ModelPart&, const Variable<double>&, const Variable<double>&, const bool);
template KRATOS_API(KRATOS_CORE) void VariableUtils::WeightedAccumulateVariableOnNodes<array_1d<double, 3>, ModelPart::ElementsContainerType, double>(
    ModelPart&, const Variable<array_1d<double, 3>>&, const Variable<double>&, const bool);

template KRATOS_API(KRATOS_CORE) std::vector<std::string> VariableUtils::GetDofsListFromGenericEntitiesSpecifications<ModelPart::ElementsContainerType>(const ModelPart::ElementsContainerType&);

template KRATOS_API(KRATOS_CORE) std::vector<std::string> VariableUtils::GetDofsListFromGenericEntitiesSpecifications<ModelPart::ConditionsContainerType>(const ModelPart::ConditionsContainerType&);

} /* namespace Kratos.*/
