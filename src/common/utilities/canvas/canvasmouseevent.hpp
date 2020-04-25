#ifndef CANVASMOUSEEVENT_HPP
#define CANVASMOUSEEVENT_HPP

#include "compat.hpp"
#include <QEvent>
#include <QPointF>

/**
 * 
 */
class ADDLE_COMMON_EXPORT CanvasMouseEvent : public QEvent
{
public: 
    enum Action 
    {
        up,
        down,
        move
    };

    CanvasMouseEvent(Action action,
        QPointF pos,
        Qt::MouseButton button,
        Qt::MouseButtons buttons,
        Qt::MouseEventFlags flags,
        Qt::MouseEventSource source,
        const QEvent* underlying = nullptr)
        : QEvent((QEvent::Type)_type),
        _action(action),
        _pos(pos),
        _button(button),
        _buttons(buttons),
        _flags(flags),
        _source(source)
    {
    }

    CanvasMouseEvent(const CanvasMouseEvent& other)
        : QEvent((QEvent::Type)_type),
        _action(other._action),
        _pos(other._pos),
        _button(other._button),
        _buttons(other._buttons),
        _flags(other._flags),
        _source(other._source)
    {
    }

    virtual ~CanvasMouseEvent() = default;

    Action action() const { return _action; }
    QPointF pos() const { return _pos; }
    Qt::MouseButton button() const { return _button; }
    Qt::MouseButtons buttons() const { return _buttons; }
    Qt::MouseEventFlags flags() const { return _flags; }
    Qt::MouseEventSource source() const { return _source; }
    const QEvent* underlying() const { return _underlying; }

    static inline int getType()
    {
        return _type;
    }

private: 
    Action _action;

    QPointF _pos;

    Qt::MouseButton _button;
    Qt::MouseButtons _buttons;
    Qt::MouseEventFlags _flags;
    Qt::MouseEventSource _source;

    const QEvent* _underlying;

    static int _type;
    friend void registerQMetaTypes();
};

#endif // CANVASMOUSEEVENT_HPP