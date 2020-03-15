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
    Layer() : _initHelper(this) { }
    virtual ~Layer() = default;
    
    void initialize();
    void initialize(LayerBuilder& builder);

    bool isEmpty() { _initHelper.check(); return _empty; }

    QRect getBoundary() { _initHelper.check(); return _boundary; }
    QPoint getTopLeft() { _initHelper.check(); return _boundary.topLeft(); }
    void setTopLeft(QPoint) { _initHelper.check(); }

    QColor getSkirtColor() { _initHelper.check(); return Qt::GlobalColor::transparent; }

    IRasterSurface* getRasterSurface() { return _rasterSurface; }

private:
    QRect _boundary;

    bool _empty;

    IDocument* _document;

    IRasterSurface* _rasterSurface;

    InitializeHelper<Layer> _initHelper;
};

#endif // LAYER_HPP