#ifndef INAVIGATETOOLPRESENTER_HPP
#define INAVIGATETOOLPRESENTER_HPP

#include <QObject>
#include <QString>
#include <QIcon>

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

#include "interfaces/traits/metaobjectinfo.hpp"
#include "interfaces/traits/compat.hpp"

class ADDLE_COMMON_EXPORT INavigateToolPresenter : public virtual IToolPresenter
{
	Q_GADGET
public:
    INTERFACE_META(INavigateToolPresenter)
		
    enum NavigateOperationOptions {
        gripPan,
        gripPivot,
        rectangleZoomTo
    };
	Q_ENUM(NavigateOperationOptions);

	static const ToolId ID;

    virtual ~INavigateToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner) = 0;
    
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