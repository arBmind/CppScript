#pragma once

#include "CppScript/Boxed.h"

#include <mutex>

namespace CppScript {

using BoxedConvertFunc = std::function< Boxed (const Boxed&) >;

class BoxedConverters
{
public:
    BoxedConverters();

    /// \return true if a conversion is known
    bool canConvert(const AnyType& from, const AnyType& to) const;

    template<typename From, typename To>
    bool canConvert() const {
        return canConvert( AnyTypeOf<From>::anyType(), AnyTypeOf<To>::anyType() );
    }

    /// perform a conversion
    Boxed convert(const Boxed& from, const AnyType& to) const;

    template<typename To>
    Boxed convert(const Boxed& from) const {
        return convert(from, AnyTypeOf<To>::anyType());
    }

    /// add a new conversion
    void add(const AnyType& from, const AnyType& to, BoxedConvertFunc func);

    template<typename From, typename To>
    void add(BoxedConvertFunc func) {
        auto inner = [=](const Boxed& from) -> Boxed {
            return Boxed::make(func(from.get<const From&>()));
        };
        add(AnyTypeOf<From>::anyType(), AnyTypeOf<To>::anyType(), inner);
    }

    template<typename From, typename To>
    void add_convert() {
        auto func = [](const Boxed& from) -> Boxed {
            To to(from.get<From>());
            return Boxed::make(to);
        };
        add(AnyTypeOf<From>::anyType(), AnyTypeOf<To>::anyType(), func);
    }

    template<typename From, typename To>
    static Boxed dynamic_convert(const Boxed& from) {
        switch(from.type().ptr_type) {
        case AnyPtr::none: {
            const auto src = static_cast< From* >(from.data());
            To* const res = dynamic_cast< To* >(src);
            return Boxed::make(res);
        }
        case AnyPtr::raw: {
            const auto src = *static_cast< From** >(from.data());
            To* const res = dynamic_cast< To* >(src);
            return Boxed::make(res);
        }
        case AnyPtr::ref: {
            const auto src = &static_cast< std::reference_wrapper<From>* >(from.data())->get();
            std::reference_wrapper<To> res = *dynamic_cast<To*>(src);
            return Boxed::make(res);
        }
        case AnyPtr::shared: {
            const auto src = *static_cast< std::shared_ptr<From>* >(from.data());
            std::shared_ptr<To> res = std::dynamic_pointer_cast<To>(src);
            return Boxed::make(res);
        }
        }
    }

    template<typename Derived, typename Base>
    void add_inherit() {
        static_assert(std::is_base_of<Base,Derived>::value, "Classes are not related by inheritance");
        static_assert(std::is_polymorphic<Base>::value, "Base class must be polymorphic");
        static_assert(std::is_polymorphic<Derived>::value, "Derived class must be polymorphic");

        add(AnyTypeOf<Derived>::anyType(), AnyTypeOf<Base>::anyType(), dynamic_convert<Derived, Base>);
        add(AnyTypeOf<Base>::anyType(), AnyTypeOf<Derived>::anyType(), dynamic_convert<Base, Derived>);
    }

    /// remove a conversion
    //    bool remove(const AnyType& from, const AnyType& to);

    /// forget any conversion from and to the type
    //    void unload(const AnyType& type);

private:
    struct Converters;
    std::shared_ptr< Converters > m_converters;
    std::mutex m_mutex;
};

} // namespace CppScript
