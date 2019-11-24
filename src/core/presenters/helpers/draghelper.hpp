#ifndef POINTERHELPER_HPP
#define POINTERHELPER_HPP

#include <QPointF>

class DragHelper
{
public:

    inline bool isEngaged() { return _engaged; }
    inline QPoint getFirstPosition() { return _firstPosition; }
    inline QPoint getLastPosition() { return _lastPosition; }

    inline void engage(QPoint position)
    { 
        _firstPosition = position;
        _engaged = true;
    }

    inline void disengage(QPoint position)
    { 
        _engaged = false;
    }
    
    inline void move(QPoint position)
    {
        if (_engaged)
        {
            _lastPosition = position;
        }
    }

private:

    bool _engaged = false;

    QPoint _firstPosition;
    QPoint _lastPosition;
};

#endif // POINTERHELPER_HPP