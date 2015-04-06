#include "BoxedConversion.h"

#include <assert.h>
#include <tuple>
#include <vector>
#include <algorithm>

namespace CppScript {

struct BoxedConverters::Converters {
    using type = const std::type_info*;
    using converter = BoxedConvertFunc;
    using from_to_converter = std::tuple< type, type, converter >; // from, to, func
    using from_to_converters = std::vector< from_to_converter >;

    converter search(const from_to_converter& item) const {
        auto it = std::lower_bound(vector.begin(), vector.end(), item, order);
        if (vector.empty() || order(item, *it)) return nullptr;
        return std::get<2>(*it);
    }

    void add(from_to_converter&& item) {
        auto it = std::lower_bound(vector.begin(), vector.end(), item, order);
        if (vector.empty() || order(item, *it))
            vector.insert(it, std::move(item));
        else
            *it = item;
    }

private:
    static bool order(const from_to_converter& a, const from_to_converter& b) {
        const auto &a_from = *std::get<0>(a);
        const auto &b_from = *std::get<0>(b);
        if (a_from != b_from) return a_from.before(b_from);

        const auto &a_to = *std::get<1>(a);
        const auto &b_to = *std::get<1>(b);
        if (a_to != b_to) return a_to.before(b_to);

        return false;
    }

    from_to_converters vector;
};

BoxedConverters::BoxedConverters()
    : m_converters(std::make_shared< Converters >())
{}

void
BoxedConverters::add(const AnyType &from, const AnyType &to, BoxedConvertFunc func)
{
    assert( func != nullptr );
    std::lock_guard<std::mutex> write_guard(m_mutex);
    (void)write_guard; // unused
    std::shared_ptr<Converters> copy = std::make_shared< Converters >(*m_converters.get());
    copy->add( std::make_tuple( from.type_info, to.type_info, func ) );
    std::swap(m_converters, copy);
}

bool
BoxedConverters::canConvert(const AnyType &from, const AnyType &to) const
{
    std::shared_ptr<Converters> handle = m_converters;
    const auto func = handle->search(std::make_tuple(from.type_info, to.type_info, nullptr));
    return func != nullptr;
}

Boxed
BoxedConverters::convert(const Boxed &from, const AnyType &to) const
{
    std::shared_ptr<Converters> handle = m_converters;
    const auto func = handle->search( std::make_tuple( from.type().type_info, to.type_info, nullptr ) );
    return func( from );
}

} // namespace CppScript

