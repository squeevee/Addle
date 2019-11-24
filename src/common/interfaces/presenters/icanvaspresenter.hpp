#ifndef ICANVASPRESENTER_HPP
#define ICANVASPRESENTER_HPP

#include <QObject>
#include <QRect>
#include <QSharedPointer>

#include <QPointF>
#include <QPixmap>

#include "common/interfaces/models/idocument.hpp"
#include "idocumentpresenter.hpp"
#include "ieditorpresenter.hpp"

#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

class ICanvasView;
class ICanvasPresenter : public virtual IMakeable
{
public:
    
    virtual ~ICanvasPresenter() {}

    virtual void initialize(IDocumentPresenter* documentPresenter) = 0;
    virtual ICanvasView* getView() = 0;
    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    virtual const QPixmap getRender() = 0;

    virtual bool isEmpty() = 0;
    virtual QSize getSize() = 0;
    virtual QColor getBackgroundColor() = 0;

signals:
    virtual void boundaryChanged(QRect newBoundary) = 0;

    //virtual void applyingDrawingOperation(const IDrawingOperation& operation) = 0;
    //virtual void appliedDrawingOperation(const IDrawingOperation& operation) = 0;
};

DECL_INIT_DEPENDENCY(ICanvasPresenter, IDocumentPresenter)
DECL_EXPECTS_INITIALIZE(ICanvasPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(ICanvasPresenter)

#endif // ICANVASPRESENTER_HPP