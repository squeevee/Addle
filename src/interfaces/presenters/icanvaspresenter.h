#ifndef ICANVASPRESENTER_H
#define ICANVASPRESENTER_H

#include <QObject>
#include <QRect>

#include <QPixmap>

#include "interfaces/drawing/idrawingoperation.h"
#include "interfaces/models/idocument.h"
#include "ieditorpresenter.h"

class ICanvasPresenter
{
public:
    
    virtual ~ICanvasPresenter() {}

    virtual void initialize(IEditorPresenter* getEditorPresenter) = 0;

    virtual const QPixmap getRender() = 0;

    virtual bool isEmpty() = 0;
    virtual QSize getSize() = 0;
    virtual QColor getBackgroundColor() = 0;

    virtual bool hasDocument() = 0;
    virtual IDocument* getDocument() = 0;
    virtual void setDocument(IDocument* document) = 0;

signals:
    virtual void boundaryChanged(QRect newBoundary) = 0;

    virtual void applyingDrawingOperation(const IDrawingOperation& operation) = 0;
    virtual void appliedDrawingOperation(const IDrawingOperation& operation) = 0;
};

#endif //ICANVASPRESENTER_H