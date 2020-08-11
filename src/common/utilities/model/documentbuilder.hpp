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

    QString getFilename() { return _data->filename; }
    void setFilename(QString filename) { _data->filename = filename; }

    QColor getBackgroundColor() { return _data->backgroundColor; }
    void setBackgroundColor(QColor backgroundColor) { _data->backgroundColor = backgroundColor; }

    void addLayer(LayerBuilder& layer) { _data->layers.append(layer); }
    QList<LayerBuilder> getLayers() { return _data->layers; }

private:
    QSharedDataPointer<DocumentBuilderData> _data;
};
} // namespace Addle

#endif // DOCUMENTBUILDER_HPP