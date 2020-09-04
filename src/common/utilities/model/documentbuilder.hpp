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
    struct DocumentBuilderData : QSharedData
    {
        QString filename;
        QList<LayerBuilder> layers;
        QColor backgroundColor = Qt::transparent;
    };
public:
    DocumentBuilder() { _data = new DocumentBuilderData; }
    DocumentBuilder(const DocumentBuilder& other) : _data(other._data) {}

    QString filename() const { return _data->filename; }
    void setFilename(QString filename) { _data->filename = filename; }

    QColor backgroundColor() const { return _data->backgroundColor; }
    void setBackgroundColor(QColor backgroundColor) { _data->backgroundColor = backgroundColor; }

    QList<LayerBuilder> layers() const { return _data->layers; }
    void addLayer(LayerBuilder& layer) { _data->layers.append(layer); }

private:
    QSharedDataPointer<DocumentBuilderData> _data;
};
} // namespace Addle

#endif // DOCUMENTBUILDER_HPP