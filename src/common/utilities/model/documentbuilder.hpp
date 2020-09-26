/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef DOCUMENTBUILDER_HPP
#define DOCUMENTBUILDER_HPP

#include "compat.hpp"
#include <QString>
#include <QSharedData>
#include <QSharedDataPointer>
#include "layerbuilder.hpp"

namespace Addle {

class DocumentBuilder
{
public:
    inline QUrl url() const { return _url; }
    inline DocumentBuilder& setUrl(QUrl url) { _url = url; return *this; }

    inline QColor backgroundColor() const { return _backgroundColor; }
    inline DocumentBuilder& setBackgroundColor(QColor backgroundColor) { _backgroundColor = backgroundColor; return *this; }

    inline QList<LayerBuilder> layers() const { return _layers; }
    inline DocumentBuilder& addLayer(const LayerBuilder& layer) { _layers.append(layer); return *this; }

private:
    QUrl _url;
    QList<LayerBuilder> _layers;
    QColor _backgroundColor = Qt::transparent;
};
} // namespace Addle

#endif // DOCUMENTBUILDER_HPP