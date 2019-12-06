#ifndef LAYER_HPP
#define LAYER_HPP

#include <QObject>
#include <QColor>
#include <QImage>
#include <QPoint>

#include <QTransform>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/models/idocument.hpp"

#include "utilities/initializehelper.hpp"

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
    void render(QPainter& painter, QRect area);

    QRect getBoundary() { _initHelper.assertInitialized(); return _boundary; }
    QPoint getTopLeft() { _initHelper.assertInitialized(); return _boundary.topLeft(); }
    void setTopLeft(QPoint) { _initHelper.assertInitialized(); }

    QColor getSkirtColor() { _initHelper.assertInitialized(); return Qt::GlobalColor::transparent; }

    void applyRasterOperation(IRasterOperation* operation);

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
    InitializeHelper<Layer> _initHelper;

};

#endif // LAYER_HPP