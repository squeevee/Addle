#ifndef INAVIGATETOOLPRESENTER_HPP
#define INAVIGATETOOLPRESENTER_HPP

#include "itoolpresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"
#include "common/interfaces/presenters/iviewportpresenter.hpp"

class INavigateToolPresenter : public virtual IToolPresenter, public virtual IMakeable
{
public:
    enum NavigateOperationOptions {
        gripPan,
        gripZoom,
        gripRotate,
        rectangleZoomTo
    };

    static const NavigateOperationOptions DEFAULT_NAVIGATE_OPERATION_OPTION = gripPan;

    virtual ~INavigateToolPresenter() = default;

    virtual void initialize(IDocumentPresenter* owner) = 0;

    virtual NavigateOperationOptions getNavigateOperation() = 0;
    virtual void setNavigateOperation(NavigateOperationOptions operation) = 0;

signals:
    virtual void navigateOperationChanged(NavigateOperationOptions operation) = 0;
};

Q_DECLARE_METATYPE(INavigateToolPresenter::NavigateOperationOptions)

DECL_EXPECTS_INITIALIZE(INavigateToolPresenter)
DECL_INIT_DEPENDENCY(INavigateToolPresenter, IDocumentPresenter)
DECL_INIT_DEPENDENCY(INavigateToolPresenter, IViewPortPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(INavigateToolPresenter)

#endif // INAVIGATETOOLPRESENTER_HPP