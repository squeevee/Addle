#ifndef ILAYERVIEW_HPP
#define ILAYERVIEW_HPP

#include <QRectF>

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ILayerView
{
public:
    virtual ~ILayerView() = default;

    virtual void initialize(ILayerPresenter* presenter) = 0;

    virtual ILayerPresenter* getPresenter() = 0;

public slots: 
    virtual void onRenderChanged(QRect area) = 0;
};

DECL_MAKEABLE(ILayerView)
DECL_IMPLEMENTED_AS_QOBJECT(ILayerView)
DECL_EXPECTS_INITIALIZE(ILayerView)

#endif // ILAYERVIEW_HPP