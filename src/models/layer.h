#ifndef LAYER_H
#define LAYER_H

#include <QObject>
#include <QColor>
#include <QImage>
#include <QPoint>

#include <QTransform>

#include "interfaces/models/ilayer.h"
#include "interfaces/models/idocument.h"

#include "utilities/initializehelper.h"

class Layer : public QObject, public ILayer
{
    Q_OBJECT
public:
    Layer() : _initHelper(this) {}
    virtual ~Layer() = default;
    
    void initialize();
    void initialize(LayerBuilder& builder);

    bool isEmpty() { _initHelper.assertInitialized(); return _empty; }

    //Area is relative to the Document
    void render(QRect area, QPaintDevice* device);

    QRect getBoundary() { _initHelper.assertInitialized(); return _boundary; }
    QPoint getTopLeft() { _initHelper.assertInitialized(); return _boundary.topLeft(); }
    void setTopLeft(QPoint) {}

    QColor getSkirtColor() { _initHelper.assertInitialized(); return Qt::GlobalColor::transparent; }

private:
    const int DEFAULT_BUFFER_MARGIN = 1024;

    //Contains the raster data of the layer as well as extra space to optimize
    //boundary resizing.
    QImage _buffer;

    //Relative to the Document
    QRect _boundary;

    //Transforms between the Document coordinates and the buffer coordinates.
    QTransform _ontoBuffer;
    QTransform _fromBuffer;

    bool _empty;

    IDocument* _document;

protected:
    InitializeHelper _initHelper;

};

#endif //LAYER_H