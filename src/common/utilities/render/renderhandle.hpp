/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RENDERDATA_HPP
#define RENDERDATA_HPP

#include <QSharedData>

#include <QRect>
#include <QPainter>
#include <QPainterPath>
namespace Addle {

class RenderHandle
{
public:
    RenderHandle() = default;

    RenderHandle(QRect area, QPainter* painter)
        : _area(area), _painter(painter)
    {
    }

    RenderHandle(const RenderHandle& other) = default;
    RenderHandle& operator=(const RenderHandle& other) = default;

    QList<int> groupAddress() const { return _groupAddress; }
    void setGroupAddress(QList<int> groupAddress) { _groupAddress = groupAddress; }
    
    double minPriority() const { return _minPriority; }
    void setMinPriority(double minPriority) { _minPriority = minPriority; }
    
    double maxPriority() const { return _maxPriority; }
    void setMaxPriority(double maxPriority) { _maxPriority = maxPriority; }
    
    QPainter* painter() const { return _painter; }

    void setArea(QRect area) { _area = area; }
    QRect area() const { return _area; }

    bool isAborted() const { return _aborted; }
    void abort() { _aborted = true; }

private:
    QList<int> _groupAddress;
    double _minPriority = -qInf();
    double _maxPriority = qInf();
    
    QRect _area;
    QPainter* _painter = nullptr;
    bool _aborted = false;
};

} // namespace Addle

#endif // RENDERDATA_HPP
