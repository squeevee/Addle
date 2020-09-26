/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef GENERICEVENTFILTER_HPP
#define GENERICEVENTFILTER_HPP

#include <functional>

#include <QObject>
#include <QHash>
#include <QEvent>

#include "compat.hpp"

namespace Addle {

class ADDLE_COMMON_EXPORT GenericEventFilter : public QObject
{
    Q_OBJECT
public:
    typedef std::function<bool(QObject*, QEvent*)> FilterFunction;

    inline GenericEventFilter(
            QHash<QEvent::Type,
            FilterFunction> filters = QHash<QEvent::Type, FilterFunction>(),
            QObject* watched = nullptr
        )
        : QObject(watched), _filters(filters)
    {
        if (watched)
            watched->installEventFilter(this);
    }

    inline GenericEventFilter(
            QEvent::Type type,
            FilterFunction filter,
            QObject* watched = nullptr
        )
        : QObject(watched), _filters({{type, filter}})
    {
        if (watched)
            watched->installEventFilter(this);
    }

    virtual ~GenericEventFilter() = default;


    inline void addFilter(QEvent::Type type, FilterFunction filter)
    {
        _filters.insert(type, filter);
    }

    inline void removeFilter(QEvent::Type type)
    {
        _filters.remove(type);
    }

protected:
    bool eventFilter(QObject* obj, QEvent* e) override
    {
        if (_filters.contains(e->type()))
        {
            return _filters[e->type()](obj, e);
        }
        return false;
    }

private:
    QHash<QEvent::Type, FilterFunction> _filters;
};

} // namespace Addle

#endif // GENERICEVENTFILTER_HPP