#ifndef RENDERDATA_HPP
#define RENDERDATA_HPP

#include <QSharedData>

#include <QRect>
#include <QPainter>
#include <QPainterPath>

class RenderData
{
    struct RenderDataInner : QSharedData
    {
        RenderDataInner() = default;

        RenderDataInner(QRect area_, QPainter* painter_)
            : area(area_), painter(painter_)
        {
        }

        QPainterPath mask;
        QRect area;
        QPainter* painter = nullptr;
        bool aborted = false;
    };
public:

    RenderData()
        : _data(new RenderDataInner)
    {
    }

    RenderData(QRect area, QPainter* painter)
        : _data(new RenderDataInner(area, painter))
    {
    }

    RenderData(const RenderData& other)
        : _data(other._data)
    {
    }

    RenderData(RenderData&& other)
    {
        _data.swap(other._data);
    }

    void operator=(const RenderData& other)
    {
        _data = other._data;
    }

    QPainter* getPainter() const { return _data->painter; }

    void setArea(QRect area) { _data->area = area; }
    QRect getArea() const { return _data->area; }

    bool isAborted() const { return _data->aborted; }
    void abort() { _data->aborted = true; }

private:
    QSharedDataPointer<RenderDataInner> _data;
};

#endif // RENDERDATA_HPP