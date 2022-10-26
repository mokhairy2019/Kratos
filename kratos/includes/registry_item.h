//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//

#pragma once

// System includes
#include <string>
#include <iostream>
#include <unordered_map>

// External includes


// Project includes
#include "includes/define.h"

namespace Kratos
{
///@addtogroup KratosCore
///@{

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

/// The registry item to be stored by Registry class. It is the base class for some more specific ones.
/** RegistryItem has a tree node structure and stores its name, an optional
 *  value, and an unorder_set of its sub data.
 *  This structure let us to have registry of the elements and then different
 *  registries for each elements inside it.
 *  Please note that RegistryItem stores a pointer to the value.
 *  To have a copy of the value you may use the derived RegistryValueItem
 *  which crates a copy in construction and delete it in its destructor
 *  to make the memory management easier.
*/
class RegistryItem
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of RegistryItem
    KRATOS_CLASS_POINTER_DEFINITION(RegistryItem);

    /// Subregistry item type definition
    using SubRegistryItemType = std::unordered_map<std::string, Kratos::shared_ptr<RegistryItem>>;
    // using SubRegistryItemType = std::unordered_map<std::string, Kratos::unique_ptr<RegistryItem>>;


     /// Custom iterator with key as return type to be used in the Python export
    class key_return_iterator
    {
    public:
        ///@name Type Definitions
        ///@{

        using BaseIterator      = SubRegistryItemType::iterator;
        using iterator_category = BaseIterator::iterator_category;
        using difference_type   = BaseIterator::difference_type;
        using value_type        = BaseIterator::value_type;
        using pointer           = BaseIterator::pointer;
        using reference         = BaseIterator::reference;

        ///@}
        ///@name Life Cycle
        ///@{

        key_return_iterator()
        {}

        key_return_iterator(BaseIterator Iterator)
            : mIterator(Iterator)
        {}

        key_return_iterator(const key_return_iterator& rIterator)
            : mIterator(rIterator.mIterator)
        {}

        ///@}
        ///@name Operators
        ///@{

        key_return_iterator& operator=(const key_return_iterator& rIterator)
        {
            this->mIterator = rIterator.mIterator;
            return *this;
        }

        std::string operator*() const
        {
            return mIterator->first;
        }

        std::string operator->()
        {
            return mIterator->first;
        }

        key_return_iterator& operator++()
        {
            ++mIterator;
            return *this;
        }

        key_return_iterator operator++(int)
        {
            key_return_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator==(const key_return_iterator& rIterator) const
        {
            return this->mIterator == rIterator.mIterator;
        }

        bool operator!=(const key_return_iterator& rIterator) const
        {
            return this->mIterator != rIterator.mIterator;
        }

        ///@}
        ///@name Operations
        ///@{


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
    private:
        ///@name Member Variables
        ///@{

        BaseIterator mIterator;

        ///@}
        ///@name Private Operators
        ///@{


        ///@}
        ///@name Private Operations
        ///@{


        ///@}
        ///@name Private  Access
        ///@{


        ///@}
        ///@name Private Inquiry
        ///@{


        ///@}
    };

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor deleted.
    RegistryItem() = delete;

    /// Constructor with the name
    RegistryItem(std::string Name) : mName(Name), mpValue(nullptr){}

    /// Constructor with the name and value
    template<typename TValueType>
    RegistryItem(std::string Name, TValueType const& Value) : mName(Name), mpValue(&Value){}

    /// Destructor.
    virtual ~RegistryItem(){}

    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Items
    ///@{

    template< typename TItemType, class... TArgumentsList >
    RegistryItem& AddItem(
        std::string const& ItemName,
        TArgumentsList&&... Arguments)
    {
        KRATOS_ERROR_IF(this->HasItem(ItemName)) << "The RegistryItem '" << this->Name() << "' already has an item with name " << ItemName << "." << std::endl;
        KRATOS_ERROR_IF(this->HasValue()) <<
            "Trying to add '"<< ItemName << "' item to the RegistryItem '" << this->Name() << "' but this already has value. Items cannot have both value and subitem." << std::endl;
        auto insert_result = mSubRegistryItem.emplace(std::make_pair(ItemName, Kratos::make_unique<TItemType>(ItemName, std::forward<TArgumentsList>(Arguments)...)));
        KRATOS_ERROR_IF_NOT(insert_result.second) << "Error in inserting '" << ItemName << "' in registry item with name '" << this->Name() << "'." << std::endl;
        return *insert_result.first->second;
    }

    ///@}
    ///@name Access
    ///@{

    SubRegistryItemType::iterator begin();

    SubRegistryItemType::const_iterator cbegin() const;

    SubRegistryItemType::iterator end();

    SubRegistryItemType::const_iterator cend() const;

    key_return_iterator key_begin();

    key_return_iterator key_end();

    const std::string& Name() const
    {
        return mName;
    }

    RegistryItem const& GetItem(std::string const& rItemName) const;

    RegistryItem& GetItem(std::string const& rItemName);

    template<typename TDataType> TDataType const& GetValue() const
    {
        KRATOS_ERROR_IF(mpValue == nullptr) << "Item " << Name() << " does not have value to be returned." << std::endl;
        return *static_cast<const TDataType*>(mpValue);
    }

    void RemoveItem(std::string const& rItemName);

    ///@}
    ///@name Inquiry
    ///@{

    const std::size_t size()
    {
        KRATOS_ERROR_IF(HasValue()) << "Item " << Name() << " has value and size() cannot be retrieved." << std::endl;
        return mSubRegistryItem.size();
    }

    bool HasValue() const
    {
        return (mpValue != nullptr);
    }

    bool HasItems() const
    {
        return (!mSubRegistryItem.empty());
    }

    bool HasItem(std::string const& rItemName) const
    {
        return (mSubRegistryItem.find(rItemName) != mSubRegistryItem.end());
    }


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    virtual std::string Info() const;

    /// Print information about this object.
    virtual void PrintInfo(std::ostream& rOStream) const;

    /// Print object's data.
    virtual void PrintData(std::ostream& rOStream) const;

    virtual std::string ToJson(std::string const& rIndetation = "") const;

    ///@}
    ///@name Friends
    ///@{


    ///@}
protected:
    ///@name Protected static Member Variables
    ///@{

    std::string mName;
    const void* mpValue;
    SubRegistryItemType mSubRegistryItem;

    ///@}
    ///@name Protected member Variables
    ///@{


    ///@}
    ///@name Protected Operators
    ///@{


    ///@}
    ///@name Protected Operations
    ///@{


    ///@}
    ///@name Protected  Access
    ///@{


    ///@}
    ///@name Protected Inquiry
    ///@{


    ///@}
    ///@name Protected LifeCycle
    ///@{


    ///@}
private:
    ///@name Static Member Variables
    ///@{


    ///@}
    ///@name Member Variables
    ///@{


    ///@}
    ///@name Private Operators
    ///@{


    ///@}
    ///@name Private Operations
    ///@{


    ///@}
    ///@name Private  Access
    ///@{


    ///@}
    ///@name Private Inquiry
    ///@{


    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    RegistryItem& operator=(RegistryItem const& rOther);

    /// Copy constructor.
    RegistryItem(RegistryItem const& rOther);

    ///@}
}; // Class RegistryItem

///@}
///@name Type Definitions
///@{


///@}
///@name Input and output
///@{

/// input stream function
inline std::istream& operator >> (
    std::istream& rIStream,
    RegistryItem& rThis);

/// output stream function
inline std::ostream& operator << (
    std::ostream& rOStream,
    const RegistryItem& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}

///@} addtogroup block

}  // namespace Kratos.
