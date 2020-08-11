#ifndef PRESENTERASSIGNMENT_HPP
#define PRESENTERASSIGNMENT_HPP

#include "utilities/qtextensions/qobject.hpp"

#include <functional>
#include <QSharedPointer>

#include <QList>
#include <QObject>
namespace Addle {

template<class PresenterType>
class PresenterAssignment
{
public:
    PresenterAssignment() = default;
    PresenterAssignment(PresenterType& presenter)
        : _presenter(&presenter)
    {
    }

    template<class ContextType>
    PresenterAssignment(PresenterType& presenter, QWeakPointer<ContextType> context)
        : _presenter(&presenter)
    {
        addContext(context);
    }

    PresenterAssignment(QWeakPointer<PresenterType> presenter)
        : _presenter(presenter.toStrongRef().data()),
        _safe([=]() -> bool { return !presenter.isNull(); })
    {
    }

    PresenterAssignment(QSharedPointer<PresenterType> presenter)
        : PresenterAssignment(presenter.toWeakRef())
    {
    }

    // TransientPresenterAssignment(QPointer<PresenterType> presenter)
    //     : _presenter(presenter.data()),
    //     _safe([=]() -> bool { return !presenter.isNull(); })
    // {
    // }

    PresenterAssignment(const PresenterAssignment& other) = default;

    PresenterAssignment(PresenterAssignment&& other)
        : _presenter(other._presenter),
        _safe(other._safe),
        _connections(other._connections)
    {
        other._presenter = nullptr;
        other._safe = nullptr;
        other._connections.clear();
    }

    ~PresenterAssignment()
    {
        disconnect();
    }
 
    PresenterAssignment<PresenterType>& operator=(const PresenterAssignment<PresenterType>& other)
    {
        disconnect();

        _presenter = other._presenter;
        _safe = other._safe;
        _connections = other._connections;

        return *this;
    }

    PresenterAssignment<PresenterType>& operator=(PresenterAssignment<PresenterType>&& other)
    {
        disconnect();

        _presenter = other._presenter;
        _safe = other._safe;
        _connections = other._connections;

        other._presenter = nullptr;
        other._safe = nullptr;
        other._connections.clear();

        return *this;
    }
    
    inline bool isNull() const { return !_presenter || (_safe && !_safe()); }

    inline explicit operator bool() const { return !isNull(); }
    inline bool operator !() const { return isNull(); }

    inline PresenterType& operator*() const { return *_presenter; }
    inline PresenterType* operator->() const { return _presenter; }

    inline PresenterType* data() const { return _presenter; }

    inline bool operator==(const PresenterAssignment<PresenterType>& other) const
    {
        return _presenter == other._presenter;
    }

    inline bool operator!=(const PresenterAssignment<PresenterType>& other) const
    {
        return !(other == *this);
    }

    inline bool operator==(const PresenterType& presenter) const
    {
        return _presenter == presenter;
    }

    inline bool operator!=(const PresenterType& presenter) const
    {
        return !(presenter == *this);
    }

    template<class ContextType>
    void addContext(QWeakPointer<ContextType> context)
    {
        if (!_safe)
            _safe = [=]() -> bool { return !context.isNull(); };
        else
            _safe = [=]() -> bool { return !context.isNull() && _safe(); };
        }

    QMetaObject::Connection connect(
        const char* signal,
        const QObject* receiver,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        QMetaObject::Connection conn = 
            connect_interface(
                _presenter, signal, receiver, slot, type
            );
        _connections.append(conn);
        return conn;
    }

    QMetaObject::Connection connect(
        const QObject* sender,
        const char* signal,
        const char* slot,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        QMetaObject::Connection conn = 
            connect_interface(
                sender, signal, _presenter, slot, type
            );
        _connections.append(conn);
        return conn;
    }

    // Associate a connection to this assignment so that it is disconnected when
    // disconnect() is called or when the assignment is changed
    // (for example, use this with connections to objects owned by the presenter)
    void addConnection(const QMetaObject::Connection& connection)
    {
        _connections.append(connection);
    }

    void disconnect()
    {
        for (auto connection : qAsConst(_connections))
        {
            if (connection) QObject::disconnect(connection);
        }
        _connections.clear();
    }

private:
    std::function<bool()> _safe;
    PresenterType* _presenter = nullptr;
    QList<QMetaObject::Connection> _connections;
};

template<class PresenterType>
inline bool operator==(const PresenterType& presenter, const PresenterAssignment<PresenterType>& other)
{
    return other == presenter;
}

template<class PresenterType>
inline bool operator!=(const PresenterType& presenter, const PresenterAssignment<PresenterType>& other)
{
    return !(other == presenter);
}


} // namespace Addle
#endif // TRANSIENTPRESENTERASSIGNMENT_HPP