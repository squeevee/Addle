/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef MOUSEHELPER_HPP
#define MOUSEHELPER_HPP

#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/presenter/propertycache.hpp"
#include "utilities/canvas/canvasmouseevent.hpp"

#include <QEvent>
#include <QPointF>
#include <list>

#include "utilities/helpercallback.hpp"
namespace Addle {

/**
 * Tracks mouse state for a tool presenter
 */
class MouseHelper
{
public:

    inline void event(QEvent* e)
    {
        e->ignore();
        if (e->type() == CanvasMouseEvent::type())
        {
            CanvasMouseEvent* canvasMouseEvent = static_cast<CanvasMouseEvent*>(e);

            if (canvasMouseEvent->button() == Qt::RightButton)
            {
                // cancel tool operation
                disengage();
                e->accept();
                return;
            }

            switch (canvasMouseEvent->action())
            {
            case CanvasMouseEvent::down:
                if (engage(canvasMouseEvent->pos()))
                {
                    onEngage();
                }
                break;

            case CanvasMouseEvent::move:
                if (move(canvasMouseEvent->pos()))
                {
                    onMove();
                }
                break;

            case CanvasMouseEvent::up:
                if (move(canvasMouseEvent->pos()))
                {
                    onMove();
                }
                if (disengage())
                {
                    onDisengage();
                }
                break;
            }

            e->accept();
        }
    }

    inline bool engage(QPointF pos)
    {
        if (_engaged) return false;

        _engaged = true;

        _firstPosition = pos;
        _previousPosition = pos;
        _latestPosition = pos;

        _positions.clear();
        _positions.push_back(pos);

        return true;
    }

    inline bool move(QPointF pos)
    {
        if (!_engaged) return false;
        if (pos == _latestPosition) return false;

        _previousPosition = _latestPosition;
        _latestPosition = pos;

        _positions.push_back(pos);

        return true;
    }

    inline bool disengage()
    {
        if (!_engaged) return false;

        _engaged = false;

        return true;
    }

    inline bool isEngaged() { return _engaged; }

    inline QPointF firstPosition() { return _firstPosition; }
    inline QPointF previousPosition() { return _previousPosition; }
    inline QPointF latestPosition() { return _latestPosition; }

    inline std::list<QPointF> positions() { return _positions; }

    HelperCallback onEngage;
    HelperCallback onMove;
    HelperCallback onDisengage;

private:

    bool _engaged = false;

    std::list<QPointF> _positions;

    QPointF _firstPosition;
    QPointF _previousPosition;
    QPointF _latestPosition;

    QCursor _cursor;
    QCursor _engagedCursor;
};

} // namespace Addle
#endif // MOUSEHELPER_HPP
