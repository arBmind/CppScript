#include "Boxed.h"

namespace CppScript {

const AnyType &Boxed::type() const
{
    static AnyType s_voidType;
    if (m_value) return m_value->type();
    return s_voidType;
}

void *Boxed::data() const
{
    if (m_value) return m_value->data();
    return nullptr;
}

const void *Boxed::constData() const
{
    if (m_value) return m_value->data();
    return nullptr;
}

} // namespace CppScript
