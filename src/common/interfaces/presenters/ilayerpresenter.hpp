#ifndef ILAYERPRESENTER_HPP
#define ILAYERPRESENTER_HPP

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

#include "idocumentpresenter.hpp"

#include "interfaces/models/ilayer.hpp"

#include "interfaces/editing/irasteroperation.hpp"

#include <QPainter>
#include <QRectF>

class ILayerView;
class ILayerPresenter
{
public:
    virtual ~ILayerPresenter() = default;
    
    virtual void initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> layer) = 0;
    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    virtual ILayerView* getView() = 0;

    virtual QWeakPointer<ILayer> getModel() = 0;

    virtual QRect getCanvasBounds() = 0;
    virtual void render(QPainter& painter, QRect area) = 0;

    virtual void setRasterOperation(IRasterOperation* operation) = 0;
    virtual void unsetRasterOperation() = 0;

signals: 
    virtual void renderChanged(QRect area) = 0;

};

DECL_MAKEABLE(ILayerPresenter)
DECL_EXPECTS_INITIALIZE(ILayerPresenter)
DECL_INIT_DEPENDENCY(ILayerPresenter, IDocumentPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(ILayerPresenter)

#endif // ILAYERPRESENTER_HPP