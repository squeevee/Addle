#ifndef MOUSEHELPER_HPP
#define MOUSEHELPER_HPP

class MouseHelper
{
public:

//     enum TrackingOptions
//     {
//         endpoints,
//         path
//     };

//     MouseHelper(ICanvasMousePresenter& presenter, TrackingOptions tracking = TrackingOptions::endpoints)
//         : _presenter(presenter), _tracking(tracking)
//     {
//     }

//     void engage(QPointF canvasPos)
//     {
//         if (_engagedCursor != QCursor())
//         {
//             emit cursorHintChanging();
//         }

//         _toolPathHelper.engage(viewPortPos, canvasPos);
//         onPointerEngage();

//         if (_engagedCursor != QCursor())
//         {
//             emit cursorHintChanged();
//         }
//     }
//     void move(QPointF canvasPos)
//     {
//         if (_toolPathHelper.isEngaged())
//         {
//             QPoint viewPortPos = _viewPortPresenter->getFromCanvasTransform().map(canvasPos).toPoint();
//             _toolPathHelper.move(viewPortPos, canvasPos);
//             onPointerMove();
//         }
//     }
//     void disengage(QPointF canvasPos)
//     {
//         if (_toolPathHelper.isEngaged())
//         {
//             if (_engagedCursor != QCursor())
//             {
//             }

//             onPointerDisengage();

//             if (_engagedCursor != QCursor())
//             {
//                 emit cursorChanged();
//             }
//         }
//     }

//     inline bool isEngaged() { return _engaged; }
//     inline QPointF getFirstCanvasPosition() { return _firstCanvasPosition; }
//     inline QPointF getPreviousCanvasPosition() { return _previousCanvasPosition; }
//     inline QPointF getLastCanvasPosition() { return _lastCanvasPosition; }

//     inline void engage(QPoint viewPortPosition, QPointF canvasPosition)
//     {
//         _firstCanvasPosition = canvasPosition;
//         _lastCanvasPosition = canvasPosition;

//         _engaged = true;
//     }

//     inline void disengage(QPoint viewPortPosition, QPointF canvasPosition)
//     { 
//         _engaged = false;
//     }
    
//     inline void move(QPoint viewPortPosition, QPointF canvasPosition)
//     {
//         _previousCanvasPosition = _lastCanvasPosition;

//         if (!_engaged)
//         {
//             engage(viewPortPosition, canvasPosition);
//         }

//         _lastViewPortPosition = viewPortPosition;
//         _lastCanvasPosition = canvasPosition;
//     }

// private:

//     TrackingOptions _tracking;

//     bool _engaged = false;

//     QPointF _firstCanvasPosition;
//     QPointF _previousCanvasPosition;
//     QPointF _lastCanvasPosition;

//     ICanvasMousePresenter& _presenter;

//     QCursor _cursor;
//     QCursor _engagedCursor;

};

#endif // MOUSEHELPER_HPP