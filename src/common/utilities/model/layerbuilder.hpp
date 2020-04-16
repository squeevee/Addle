#ifndef LAYERBUILDER_HPP
#define LAYERBUILDER_HPP

#include <QRect>
#include <QImage>
#include <QSharedData>
#include <QSharedDataPointer>
#include "interfaces/traits/compat.hpp"

class LayerBuilder
{
    struct LayerBuilderData : QSharedData
    {
        QRect boundary;
        QImage image;
    };
public:
    LayerBuilder() { _data = new LayerBuilderData; }
    LayerBuilder(const LayerBuilder& other) : _data(other._data) {}
    
    void setBoundary(QRect boundary) { _data->boundary = boundary; }
    QRect getBoundary() { return _data->boundary; }

    void setImage(QImage image) { _data->image = image; }
    QImage getImage() { return _data->image; }

private:
    QSharedDataPointer<LayerBuilderData> _data;
};

#endif // LAYERBUILDER_HPP