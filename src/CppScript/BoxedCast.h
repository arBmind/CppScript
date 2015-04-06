#pragma once

#include <memory>

namespace CppScript {

class Boxed;

template< typename ToType >
struct BoxedCast {
    using Result = std::reference_wrapper<const ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template< typename ToType >
struct BoxedCast< const ToType > : BoxedCast<ToType> {};

template< typename ToType >
struct BoxedCast< const ToType& > : BoxedCast<ToType> {};

template< typename ToType >
struct BoxedCast< ToType& > {
    using Result = ToType&;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template< typename ToType >
struct BoxedCast< std::reference_wrapper<ToType> > : BoxedCast<ToType&> {};

template< typename ToType >
struct BoxedCast< const ToType* > {
    using Result = const ToType*;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template< typename ToType >
struct BoxedCast< ToType* > {
    using Result = ToType*;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template< typename ToType >
struct BoxedCast< typename std::shared_ptr<const ToType> > {
    using Result = typename std::shared_ptr<const ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template< typename ToType >
struct BoxedCast< typename std::shared_ptr<ToType> > {
    using Result = typename std::shared_ptr<ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

template<>
struct BoxedCast< Boxed > {
    using Result = const Boxed&;

    static bool allowed(const Boxed&) {
        return true;
    }
    static Result cast(const Boxed& value) {
        return value;
    }
};

template<>
struct BoxedCast< Boxed& > {
    using Result = Boxed&;

    static bool allowed(const Boxed&) {
        return true;
    }
    static Result cast(const Boxed& value) {
        return const_cast< Result >(value);
    }
};

} // namespace CppScript
