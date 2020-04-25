#ifndef IBRUSHPRESENTERAUX_HPP
#define IBRUSHPRESENTERAUX_HPP

#include "compat.hpp"
#include <QObject>

namespace IBrushPresenterAux
{
    Q_NAMESPACE_EXPORT(ADDLE_COMMON_EXPORT)
    
    enum SizeOption
    {
        _1px,
        _2px,
        _4px,
        _7px,
        _13px,
        _25px,
        _50px,
        _100px,
        _250px,
        _500px,
        _10percent,
        _25percent,
        onethird,
        _50percent,
        custom_px,
        custom_percent
    };
    Q_ENUM_NS(SizeOption)
}

#endif // IBRUSHPRESENTERAUX_HPP