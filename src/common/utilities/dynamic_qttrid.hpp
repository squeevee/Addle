#ifndef DYNAMIC_QTTRID_HPP
#define DYNAMIC_QTTRID_HPP

#include <QtGlobal>

#ifdef ADDLE_DEBUG
#include <QtDebug>
#endif // ADDLE_DEBUG

namespace Addle
{
    
#ifdef ADDLE_DEBUG

const ADDLE_COMMON_EXPORT extern QSet<QByteArray> _DYNAMIC_TRIDS_REGISTRY;

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
}

#endif // DYNAMIC_QTTRID_HPP
