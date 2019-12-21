#ifndef QT_EXTENSIONS__QLIST_HPP
#define QT_EXTENSIONS__QLIST_HPP

#include <QList>
#include <QSharedPointer>

template<typename OutType, typename InType>
QList<QSharedPointer<OutType>> qSharedPointerListCast(const QList<QSharedPointer<InType>>& inList)
{
    QList<QSharedPointer<OutType>> outList;
    outList.reserve(inList.size());
    for (QSharedPointer<InType> entry : inList)
    {
        outList.append(entry.template staticCast<OutType>());
    }
    return outList;
}

#endif // QT_EXTENSIONS__QLIST_HPP