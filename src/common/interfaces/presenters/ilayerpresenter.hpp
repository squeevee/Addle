#ifndef ILAYERPRESENTER_HPP
#define ILAYERPRESENTER_HPP

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/compat.hpp"

#include "idocumentpresenter.hpp"

#include "interfaces/models/ilayer.hpp"

#include <QWeakPointer>
#include <QPainter>
#include <QRectF>

class IRenderStack;
class ADDLE_COMMON_EXPORT ILayerPresenter
{
public:
    virtual ~ILayerPresenter() = default;
    
    virtual void initialize(IDocumentPresenter* documentPresenter, QWeakPointer<ILayer> layer) = 0;
    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    virtual QWeakPointer<ILayer> getModel() = 0;

    virtual IRenderStack& getRenderStack() = 0;

signals: 
    virtual void updated(QRect area) = 0;
};

DECL_MAKEABLE(ILayerPresenter)
DECL_EXPECTS_INITIALIZE(ILayerPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(ILayerPresenter)

#endif // ILAYERPRESENTER_HPP