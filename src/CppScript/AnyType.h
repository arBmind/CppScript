#pragma once

#include <typeinfo>
#include <memory>
#include <cstdint>
#include <type_traits>

namespace CppScript {

/// types of pointers
enum class AnyPtr : uint8_t {
    none, // no pointer
    ref,  // std::reference_wrapper
    raw,  // raw pointer
    shared, // std::shared_ptr
};

/// value type capturing type information
struct AnyType {
    const std::type_info* type_info;
    bool is_void : 1;
    bool is_const : 1;
    AnyPtr ptr_type : 8;

    AnyType()
        : type_info(&typeid(void))
        , is_void(true)
        , is_const(false)
        , ptr_type(AnyPtr::none)
    {}

    AnyType(const std::type_info& type_info,
            bool is_void, bool is_const, AnyPtr ptr_type)
        : type_info(&type_info)
        , is_void(is_void)
        , is_const(is_const)
        , ptr_type(ptr_type)
    {}

    operator==(const AnyType& o) const {
        return *this->type_info == *o.type_info &&
                this->is_void == o.is_void &&
                this->is_const == o.is_const &&
                this->ptr_type == o.ptr_type;
    }

    bool operator!=(const AnyType& o) {
        return !(*this == o);
    }

};

/// Wrapper to mark a type explicitly as const
template <typename T>
struct ConstWrapper {
    T value;
};

/// meta function that removes ConstWrapper
template< typename Type >
struct UnwrapConst {
    using value_type = Type;
    static const bool is_const = false;
};

template< typename Type >
struct UnwrapConst< ConstWrapper<Type> > {
    using value_type = Type;
    static const bool is_const = true;
};

/// meta function that distinguishes references and pointers
template< typename Type, bool is_ptr = std::is_pointer<Type>::value >
struct ReducePtr {
    using value_type = Type;
    static const AnyPtr ptr_type = AnyPtr::none;
};

template< typename Type>
struct ReducePtr< Type, true > {
    using value_type = typename std::remove_pointer<Type>::type;
    static const AnyPtr ptr_type = AnyPtr::raw;
};

template< typename Type>
struct ReducePtr< std::reference_wrapper<Type>, false > {
    using value_type = Type;
    static const AnyPtr ptr_type = AnyPtr::ref;
};

template< typename Type >
struct ReducePtr< std::shared_ptr<Type>, false >
{
    using value_type = Type;
    static const AnyPtr ptr_type = AnyPtr::shared;
};

/// meta function that extracts const types
template< typename Type >
struct ReduceConst {
    using value_type = Type;
    static const bool is_const = false;
};

template< typename Type >
struct ReduceConst< Type const > {
    using value_type = Type;
    static const bool is_const = true;
};

/// Meta function to extract the AnyType from a C++ type
template< typename Type >
struct AnyTypeOf {
    using Wrapped = UnwrapConst< Type >;
    using Storage = typename Wrapped::value_type;
    using Ptr = ReducePtr< Storage >;
    using Const = ReduceConst< typename Ptr::value_type >;

    static AnyType anyType() {
        return { typeid(typename Const::value_type), std::is_void<Type>::value, Wrapped::is_const || Const::is_const, Ptr::ptr_type };
    }
};

template< typename Type >
struct AnyTypeOf< Type& >;

template< typename Type >
struct AnyTypeOf< const Type >;

template< typename Type >
struct AnyTypeOf< const Type& >;

} // namespace CppScript
