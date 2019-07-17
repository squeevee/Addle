#ifndef ILAYER_H
#define ILAYER_H


#include <QRect>
#include <QPoint>
#include <QImage>
#include <QPaintDevice>

class IDocument;

struct LayerBuilder;
class ILayer
{
public:
    virtual ~ILayer() {}
    virtual void initialize() = 0;
    virtual void initialize(LayerBuilder&) = 0;

    virtual bool isEmpty() = 0;

    virtual void render(QRect area, QPaintDevice* device) = 0;

    virtual QRect getBoundary() = 0;
    virtual QPoint getTopLeft() = 0;
    virtual void setTopLeft(QPoint) = 0;
};

struct LayerBuilder
{
    QRect boundary;
    QImage image;
};
#endif //ILAYER_H