#ifndef INAVIGATETOOLPRESENTER_HPP
#define INAVIGATETOOLPRESENTER_HPP

#include "compat.hpp"

#include <QObject>
#include <QString>
#include <QIcon>

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#include "interfaces/traits/metaobjectinfo.hpp"

#include "inavigatetoolpresenteraux.hpp"

class INavigateToolPresenter : public virtual IToolPresenter
{
public:
    typedef INavigateToolPresenterAux::NavigateOperationOptions NavigateOperationOptions;
    INTERFACE_META(INavigateToolPresenter)

    virtual ~INavigateToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner,
        ICanvasPresenter* canvasPresenter,
        IViewPortPresenter* viewPortPresenter
    ) = 0;
    
    virtual NavigateOperationOptions getNavigateOperation() = 0;

public slots:
    virtual void setNavigateOperation(NavigateOperationOptions operation) = 0;

signals:
    virtual void navigateOperationChanged(NavigateOperationOptions operation) = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    INavigateToolPresenter,
    DECL_INTERFACE_PROPERTY(navigateOperation)
)

DECL_MAKEABLE(INavigateToolPresenter)
DECL_EXPECTS_INITIALIZE(INavigateToolPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(INavigateToolPresenter)

#endif // INAVIGATETOOLPRESENTER_HPP