/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef LAYERBUILDER_HPP
#define LAYERBUILDER_HPP

#include "compat.hpp"
#include <QRect>
#include <QImage>
#include <QSharedData>
#include <QSharedDataPointer>
namespace Addle {

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
    QRect boundary() { return _data->boundary; }

    void setImage(QImage image) { _data->image = image; }
    QImage image() { return _data->image; }

private:
    QSharedDataPointer<LayerBuilderData> _data;
};

} // namespace Addle
#endif // LAYERBUILDER_HPP