#ifndef IDOCUMENT_H
#define IDOCUMENT_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QString>
#include <QPaintDevice>

#include "ilayer.h"
#include "interfaces/drawing/idrawingoperation.h"

struct DocumentBuilder;
class IDocument
{
public:

    enum InitEmptyOptions
    {
        nothing,
        useDefaults
    };

    virtual ~IDocument() {}

    virtual void initialize(InitEmptyOptions emptyOption = InitEmptyOptions::useDefaults) = 0;
    virtual void initialize(DocumentBuilder& builder) = 0;

    virtual void render(QRect area, QPaintDevice* device) = 0;

    virtual bool isEmpty() = 0;

    virtual QSize getSize() = 0;

    virtual void applyOperation(IDrawingOperation& operation) = 0;

    virtual QColor getBackgroundColor() = 0;

    virtual QString getFilename() = 0;
    virtual void setFilename(QString filename) = 0;

signals:
    void renderChanged(QRect area);
    void boundaryChanged(QRect newBoundary);

    void applyingDrawingOperation(const IDrawingOperation& operation);
    void appliedDrawingOperation(const IDrawingOperation& operation);
};

struct LayerBuilder;
struct DocumentBuilder
{
    QString filename;
    QList<LayerBuilder> layers;
};

#endif //IDOCUMENT_H