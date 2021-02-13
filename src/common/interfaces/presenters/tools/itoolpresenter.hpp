/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ITOOLPRESENTER_HPP
#define ITOOLPRESENTER_HPP

#include <QObject>
#include <QPoint>
#include <QIcon>
#include <QCursor>
#include "../idocumentpresenter.hpp"

#include "idtypes/toolid.hpp"
#include "interfaces/iamqobject.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class IMainEditorPresenter;
class IToolPresenter : public virtual IAmQObject
{
public:
    virtual ~IToolPresenter() = default;
    
    virtual IMainEditorPresenter* owner() = 0;

    virtual ToolId id() const = 0;

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool isSelected) = 0;

    virtual QCursor cursor() = 0;// const;

signals:
    virtual void isSelectedChanged(bool isSelected) = 0;
    virtual void cursorChanged(QCursor cursor) = 0;
};

namespace aux_IToolPresenter {
    ADDLE_FACTORY_PARAMETER_ALIAS(config_detail::generic_id_parameter, id)
    ADDLE_FACTORY_PARAMETER_NAME(editor)
}

//ADDLE_DECL_UNIQUE_REPO_MEMBER(IToolPresenter, ToolId)
ADDLE_DECL_UNIQUE_REPO_MEMBER_BASIC(IToolPresenter, ToolId)
ADDLE_DECL_FACTORY_PARAMETERS(
    IToolPresenter,
    (required
        (id,        (ToolId))
        (editor,    (IMainEditorPresenter&))
    )
)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IToolPresenter, "org.addle.IToolPresenter");

#endif // ITOOLPRESENTER_HPP
