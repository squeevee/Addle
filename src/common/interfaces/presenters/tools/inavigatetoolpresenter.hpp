/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef INAVIGATETOOLPRESENTER_HPP
#define INAVIGATETOOLPRESENTER_HPP

#include "compat.hpp"

#include <QObject>
#include <QString>
#include <QIcon>

#include "itoolpresenter.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


#include "interfaces/presenters/iviewportpresenter.hpp"

#include "interfaces/metaobjectinfo.hpp"

#include "inavigatetoolpresenteraux.hpp"
namespace Addle {

class INavigateToolPresenter : public virtual IToolPresenter, public virtual IAmQObject
{
public:
    typedef INavigateToolPresenterAux::NavigateOperationOptions NavigateOperationOptions;
    INTERFACE_META(INavigateToolPresenter)

    virtual ~INavigateToolPresenter() = default;

    virtual void initialize(IMainEditorPresenter* owner) = 0;
    
    virtual NavigateOperationOptions navigateOperation() = 0;

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

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::INavigateToolPresenter, "org.addle.INavigateToolPresenter")

#endif // INAVIGATETOOLPRESENTER_HPP