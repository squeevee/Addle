/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef STRINGS_HPP
#define STRINGS_HPP

#include "compat.hpp"

#include <QSet>
#include <QString>
#include <QByteArray>
#include <QByteArrayList>
#include <QMetaEnum>

#include "globals.hpp"
#include "dynamic_qttrid.hpp"

namespace Addle
{

inline QString affixUnits(LayoutUnits unit, double a,
    int fieldwidth = 0, char format = 'g', int precision = -1,
    QChar fillChar = QLatin1Char(' '))
{
                    //% "%L1 %2"
    return qtTrId("templates.affix-units")
        .arg(a, fieldwidth, format, precision, fillChar)
        .arg(dynamic_qtTrId({ "units", QMetaEnum::fromType<LayoutUnits>().valueToKey(unit) }));
}

inline QString affixUnits(LayoutUnits unit, int a, 
    int fieldwidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
{
    return qtTrId("templates.affix-units")
        .arg(a, fieldwidth, base, fillChar)
        .arg(dynamic_qtTrId({ "units", QMetaEnum::fromType<LayoutUnits>().valueToKey(unit) }, a));
}

} // namespace Addle

#endif // STRINGS_HPP
