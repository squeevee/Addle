#ifndef LAYER_HPP
#define LAYER_HPP

#include <QObject>
#include <QColor>
#include <QImage>
#include <QPoint>

#include <QTransform>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/models/idocument.hpp"

#include "utilities/image/expandingbuffer.hpp"

#include "utilities/initializehelper.hpp"

class Layer : public QObject, public ILayer
{
    Q_OBJECT
public:
    Layer() : _initHelper(this), _buffer(DEFAULT_BUFFER_MARGIN) { }
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

    BufferPainter getBufferPainter(QRect area);

signals: 
    void renderChanged(QRect area);

protected:
    void beforeBufferPainted(QRect region) { }
    void afterBufferPainted(QRect region) { emit renderChanged(region); }

private:
    const int DEFAULT_BUFFER_MARGIN = 1024;

    QRect _boundary;

    ExpandingBuffer _buffer;

    bool _empty;

    IDocument* _document;

    InitializeHelper<Layer> _initHelper;

};

#endif // LAYER_HPP