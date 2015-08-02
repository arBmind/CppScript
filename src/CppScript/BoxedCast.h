#pragma once

#include <memory>

namespace CppScript {

class Boxed;

// type - use reference_wrapper
template< typename ToType >
struct BoxedCast {
    using Result = std::reference_wrapper<const ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// const type - treat as type
template< typename ToType >
struct BoxedCast< const ToType > : BoxedCast<ToType> {};

// const reference type - treat as type
template< typename ToType >
struct BoxedCast< const ToType& > : BoxedCast<ToType> {};

// reference type - use reference
template< typename ToType >
struct BoxedCast< ToType& > {
    using Result = ToType&;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// reference wrapped type - treat like reference
template< typename ToType >
struct BoxedCast< std::reference_wrapper<ToType> > : BoxedCast<ToType&> {};

// const pointer to type - use const pointer
template< typename ToType >
struct BoxedCast< const ToType* > {
    using Result = const ToType*;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// pointer to type - use pointer
template< typename ToType >
struct BoxedCast< ToType* > {
    using Result = ToType*;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// shared pointer to const type - use shared pointer
template< typename ToType >
struct BoxedCast< typename std::shared_ptr<const ToType> > {
    using Result = typename std::shared_ptr<const ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// shared pointer to type - use shared pointer
template< typename ToType >
struct BoxedCast< typename std::shared_ptr<ToType> > {
    using Result = typename std::shared_ptr<ToType>;

    static bool allowed(const Boxed& value);
    static Result cast(const Boxed& value);
};

// boxed type - use const boxed reference
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

// reference to boxed type - use boxed reference
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
