#ifndef INAVIGATETOOLPRESENTER_HPP
#define INAVIGATETOOLPRESENTER_HPP

#include <QObject>
#include <QString>
#include <QIcon>

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#include "interfaces/traits/metaobjectinfo.hpp"

class INavigateToolPresenter : public virtual IToolPresenter
{
    Q_GADGET
public:
    INTERFACE_META(INavigateToolPresenter)

    static const ToolId NAVIGATE_TOOL_ID;

    enum NavigateOperationOptions {
        gripPan,
        gripPivot,
        rectangleZoomTo
    };
    Q_ENUM(NavigateOperationOptions)

    static const NavigateOperationOptions DEFAULT_NAVIGATE_OPERATION_OPTION = gripPan;

    virtual ~INavigateToolPresenter() = default;

    virtual void initialize(IDocumentPresenter* owner) = 0;
    
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
DECL_INIT_DEPENDENCY(INavigateToolPresenter, IDocumentPresenter)
DECL_INIT_DEPENDENCY(INavigateToolPresenter, IViewPortPresenter)
DECL_IMPLEMENTED_AS_QOBJECT(INavigateToolPresenter)

#endif // INAVIGATETOOLPRESENTER_HPP