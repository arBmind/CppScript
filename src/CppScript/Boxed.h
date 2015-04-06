#pragma once

#include "BoxedCast.h"
#include "AnyType.h"

#include <QString>
#include <QVector>

#include <exception>
#include <memory>

namespace CppScript {

/// value implementation of an any
class AnyBase {
public:
    AnyBase(AnyType type)
        : m_type(type)
    {}

    // type specific overloads
    virtual ~AnyBase() {}
    virtual void* data() const = 0;
    virtual std::shared_ptr<AnyBase> clone() const = 0;

    const AnyType& type() const { return m_type; }

private:
    AnyType m_type;
};

/// type specific any implementation
template< typename ValueType >
class AnyImpl : public AnyBase {
    using TypeOf = AnyTypeOf<ValueType>;
    using StoreType = ValueType;
public:
    explicit AnyImpl(ValueType value)
        : AnyBase(TypeOf::anyType())
        , m_value(*static_cast< StoreType* >(&value))
    {}
    ~AnyImpl() {}
private:
    void* data() const { return static_cast<void*>(const_cast<StoreType*>(&m_value)); }
    std::shared_ptr< AnyBase > clone() const { return std::make_shared< AnyImpl<ValueType> >( m_value ); }

private:
    StoreType m_value;
};

/// prepares real types for usage
template< typename Type >
struct AnyTake {
    using type = Type;
    static type convert(Type value) {
        return value;
    }
};

/// addresses are not treated (use std::ref)
template< typename Type >
struct AnyTake< Type& > {
    using type = Type;
    static type convert(Type& value) {
        return value;
    }
};

/// const is handled through wrapper
template< typename Type >
struct AnyTake< const Type > {
    using type = ConstWrapper<Type>;
    static type convert(const Type value) {
        return {value};
    }
};

/// combination of the above
template< typename Type >
struct AnyTake< const Type& > {
    using type = ConstWrapper<Type>;
    static type convert(const Type& value) {
        return {value};
    }
};

/// convert a given value to the const representation
struct ToConst {
    template< typename Type >
    static auto convert(Type value) -> ConstWrapper<Type> {
        return {value};
    }

    template< typename Type >
    static auto convert(ConstWrapper<Type> value) -> ConstWrapper<Type> {
        return value;
    }
};

/// stores boxed value and type information
class Boxed {
public:
    /// creates an undefined value
    static Boxed undefined() {
        return {};
    }

    /// create Boxed for any C++ value
    template< typename ValueType >
    static Boxed make(ValueType value) {
        return Boxed(AnyTake<ValueType>::convert(value));
    }

    /// create Boxed using given allocator for any C++ value
    template< typename Alloc, typename ValueType >
    static Boxed allocate(const Alloc &alloc, ValueType value) {
        return Boxed(alloc, AnyTake<ValueType>::convert(value));
    }

    /// create const valued Boxed for any C++ value
    template< typename ValueType >
    static Boxed make_const(ValueType value) {
        return Boxed(ToConst::convert(AnyTake<ValueType>::convert(value)));
    }

    /// create const valued Boxed using given allocator for any C++ value
    template< typename Alloc, typename ValueType >
    static Boxed allocate_const(const Alloc &alloc, ValueType value) {
        return Boxed(alloc, ToConst::convert(AnyTake<ValueType>::convert(value)));
    }

    Boxed() {}
private:
    template< typename ValueType >
    explicit Boxed(ValueType value)
        : m_value(std::make_shared< AnyImpl<ValueType> >(value))
    {}

    template< typename Alloc, typename ValueType >
    Boxed(const Alloc &alloc, ValueType value)
        : m_value(std::allocate_shared< AnyImpl<ValueType> >(alloc, value))
    {}

public:
    const AnyType& type() const;

    void* data() const;
    const void* constData() const;

    template< typename Type >
    bool is() const {
        return BoxedCast<Type>::allowed(*this);
    }

    template< typename Type >
    auto get() const -> typename BoxedCast<Type>::Result {
        return BoxedCast<Type>::cast(*this);
    }

private:
    std::shared_ptr< AnyBase > m_value;
};

} // namespace CppScript

#include "BoxedCastImpl.h"
