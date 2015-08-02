#pragma once

#include "Boxed.h"

// impl
namespace CppScript {

#ifdef __GCC
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wreturn-type"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4715)
#endif

template< typename ToType >
bool
BoxedCast< ToType >::allowed(const Boxed &value) {
    return *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast< ToType >::Result
BoxedCast< ToType >::cast(const Boxed &value) {
    switch(value.type().ptr_type) {
    case AnyPtr::none: return *static_cast< ToType* >(value.data());
    case AnyPtr::raw: return **static_cast< ToType** >(value.data());
    case AnyPtr::ref: return static_cast< std::reference_wrapper<ToType>* >(value.data())->get();
    case AnyPtr::shared: return *static_cast< std::shared_ptr<ToType>* >(value.data())->get();
    }
}

template< typename ToType >
bool
BoxedCast<ToType &>::allowed(const Boxed &value) {
    return ! value.type().is_const && *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast<ToType &>::Result
CppScript::BoxedCast<ToType &>::cast(const Boxed &value) {
    switch(value.type().ptr_type) {
    case AnyPtr::none: return *static_cast< ToType* >(value.data());
    case AnyPtr::raw: return **static_cast< ToType** >(value.data());
    case AnyPtr::ref: return static_cast< std::reference_wrapper<ToType>* >(value.data())->get();
    case AnyPtr::shared: return *static_cast< std::shared_ptr<ToType>* >(value.data())->get();
    }
}

template< typename ToType >
bool
BoxedCast<const ToType *>::allowed(const Boxed &value) {
    return *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast<const ToType *>::Result
CppScript::BoxedCast<const ToType *>::cast(const Boxed &value) {
    switch(value.type().ptr_type) {
    case AnyPtr::none: return static_cast< ToType* >(value.data());
    case AnyPtr::raw: return *static_cast< ToType** >(value.data());
    case AnyPtr::ref: return &static_cast< std::reference_wrapper<ToType>* >(value.data())->get();
    case AnyPtr::shared: return static_cast< std::shared_ptr<ToType>* >(value.data())->get();
    }
}

template< typename ToType >
bool
BoxedCast<ToType *>::allowed(const Boxed &value) {
    return ! value.type().is_const && *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast<ToType *>::Result
BoxedCast<ToType *>::cast(const Boxed &value) {
    switch(value.type().ptr_type) {
    case AnyPtr::none: return static_cast< ToType* >(value.data());
    case AnyPtr::raw: return *static_cast< ToType** >(value.data());
    case AnyPtr::ref: return &static_cast< std::reference_wrapper<ToType>* >(value.data())->get();
    case AnyPtr::shared: return static_cast< std::shared_ptr<ToType>* >(value.data())->get();
    }
}

#ifdef __GCC
#  pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

template< typename ToType >
bool
BoxedCast<std::shared_ptr<const ToType> >::allowed(const Boxed &value) {
    return value.type().ptr_type == AnyPtr::shared && *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast<std::shared_ptr<const ToType> >::Result
CppScript::BoxedCast<std::shared_ptr<const ToType> >::cast(const Boxed &value) {
    return *static_cast< std::shared_ptr<ToType>* >(value.data());
}

template< typename ToType >
bool
BoxedCast<std::shared_ptr<ToType> >::allowed(const Boxed &value) {
    return ! value.type().is_const && value.type().ptr_type == AnyPtr::shared && *value.type().type_info == typeid(ToType);
}

template< typename ToType >
typename BoxedCast<std::shared_ptr<ToType> >::Result CppScript::BoxedCast<std::shared_ptr<ToType> >::cast(const Boxed &value) {
    return *static_cast< std::shared_ptr<ToType>* >(value.data());
}

} // namespace CppScript
