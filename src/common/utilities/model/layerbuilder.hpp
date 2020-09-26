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
public:
    QRect boundary() const { return _boundary; }
    LayerBuilder& setBoundary(QRect boundary) { _boundary = boundary; return *this; }

    QImage image() const { return _image; }
    LayerBuilder& setImage(QImage image) { _image = image; return *this; }

private:
    QRect _boundary;
    QImage _image;
};

} // namespace Addle
#endif // LAYERBUILDER_HPP