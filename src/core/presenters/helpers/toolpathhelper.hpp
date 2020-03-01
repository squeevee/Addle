#ifndef TOOLPATHHELPER_HPP
#define TOOLPATHHELPER_HPP

#include <QPointF>

class ToolPathHelper
{
public:

    enum TrackingOptions
    {
        endpoints,
        path
    };

    ToolPathHelper(TrackingOptions tracking)
    {
        _tracking = tracking;
    }

    inline bool isEngaged() { return _engaged; }
    inline QPoint getFirstViewPortPosition() { return _firstViewPortPosition; }
    inline QPoint getPreviousViewPortPosition() { return _previousViewPortPosition; }
    inline QPoint getLastViewPortPosition() { return _lastViewPortPosition; }

    inline QPointF getFirstCanvasPosition() { return _firstCanvasPosition; }
    inline QPointF getPreviousCanvasPosition() { return _previousCanvasPosition; }
    inline QPointF getLastCanvasPosition() { return _lastCanvasPosition; }

    inline void engage(QPoint viewPortPosition, QPointF canvasPosition)
    { 
        _firstViewPortPosition = viewPortPosition;
        _lastViewPortPosition = viewPortPosition;

        _firstCanvasPosition = canvasPosition;
        _lastCanvasPosition = canvasPosition;

        _engaged = true;
    }

    inline void disengage(QPoint viewPortPosition, QPointF canvasPosition)
    { 
        _engaged = false;
    }
    
    inline void move(QPoint viewPortPosition, QPointF canvasPosition)
    {
        _previousViewPortPosition = _lastViewPortPosition;
        _previousCanvasPosition = _lastCanvasPosition;

        if (!_engaged)
        {
            engage(viewPortPosition, canvasPosition);
        }

        _lastViewPortPosition = viewPortPosition;
        _lastCanvasPosition = canvasPosition;
    }

private:

    TrackingOptions _tracking;

    bool _engaged = false;

    QPoint _firstViewPortPosition;
    QPoint _previousViewPortPosition;
    QPoint _lastViewPortPosition;

    QPointF _firstCanvasPosition;
    QPointF _previousCanvasPosition;
    QPointF _lastCanvasPosition;
};

#endif // TOOLPATHHELPER_HPP