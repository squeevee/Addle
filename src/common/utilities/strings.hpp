#ifndef STRINGS_HPP
#define STRINGS_HPP

#include <QtGlobal>
#include <QSet>
#include <QString>
#include <QByteArray>
#include <QByteArrayList>

#include "qtextensions/qmeta.hpp"

#include "globals.hpp"

#ifdef ADDLE_DEBUG
#include <QtDebug>
#endif // ADDLE_DEBUG

namespace Addle
{

#ifdef ADDLE_DEBUG

const extern QSet<QByteArray> _DYNAMIC_TRIDS_REGISTRY;

#endif // ADDLE_DEBUG

// Composes a dynamic TrId from an array of segments and returns the translation
// associated with that TrId.
template<std::size_t N>
inline QString dynamic_qtTrId(const char * const (&segments)[N], int n = -1)
{
    QByteArray trId;
    trId.reserve(255);
        // sufficient to avoid resizes in basically all reasonable cases, small
        // enough to not be wasteful / enjoy malloc optimizations

    for (int i = 0; i < N; ++i)
    {
        trId.append(segments[i]);
        if (i < N - 1)
            trId.append('.');
    }

#ifdef ADDLE_DEBUG
    if (!_DYNAMIC_TRIDS_REGISTRY.contains(trId))
    {
        qWarning() << qUtf8Printable(
            //% "Unknown dynamic TrId requested: \"%1\""
            qtTrId("debug-messages.unknown-dynamic-trid")
                .arg(QString::fromUtf8(trId))
        );
    }
#endif // ADDLE_DEBUG

    return qtTrId(trId.constData(), n);
}

inline QString affixUnits(LayoutUnits unit, double a,
    int fieldwidth = 0, char format = 'g', int precision = -1,
    QChar fillChar = QLatin1Char(' '))
{
    const char* unitName = enumKeyFromValue(unit);

                    //% "%L1 %2"
    return qtTrId("unit-affix-formatter")
        .arg(a, fieldwidth, format, precision, fillChar)
        .arg(dynamic_qtTrId({ "units", unitName }));
}

inline QString affixUnits(LayoutUnits unit, int a, 
    int fieldwidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
{
    const char* unitName = enumKeyFromValue(unit);

    return qtTrId("unit-affix-formatter")
        .arg(a, fieldwidth, base, fillChar)
        .arg(dynamic_qtTrId({ "units", unitName }, a));
}

} // namespace Addle

#endif // STRINGS_HPP