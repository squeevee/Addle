#ifndef MOUSEHELPER_HPP
#define MOUSEHELPER_HPP

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/presenter/propertycache.hpp"

#include <QPointF>
#include <list>

/**
 * Tracks mouse state for a tool presenter
 */
class MouseHelper
{
public:

    MouseHelper(IToolPresenter& presenter)
        : _presenter(presenter)
    {
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

    inline QPointF getFirstPosition() { return _firstPosition; }
    inline QPointF getPreviousPosition() { return _previousPosition; }
    inline QPointF getLatestPosition() { return _latestPosition; }

    inline std::list<QPointF> getPositions() { return _positions; }

private:

    bool _engaged = false;

    std::list<QPointF> _positions;

    QPointF _firstPosition;
    QPointF _previousPosition;
    QPointF _latestPosition;

    IToolPresenter& _presenter;

    QCursor _cursor;
    QCursor _engagedCursor;
};

#endif // MOUSEHELPER_HPP