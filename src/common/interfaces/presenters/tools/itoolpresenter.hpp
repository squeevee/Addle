#ifndef ITOOLPRESENTER_HPP
#define ITOOLPRESENTER_HPP

#include <QObject>
#include <QPoint>
#include <QIcon>
#include <QCursor>
#include "../idocumentpresenter.hpp"

#include "idtypes/toolid.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

class IMainEditorPresenter;
class IToolPresenter
{
public:
    virtual ~IToolPresenter() = default;
    
    virtual IMainEditorPresenter* getOwner() = 0;

    virtual ToolId getId() = 0;

    virtual bool isSelected() const = 0;
    virtual void setSelected(bool isSelected) = 0;

    virtual QCursor cursor() = 0;// const;

signals:
    virtual void isSelectedChanged(bool isSelected) = 0;
    virtual void cursorChanged(QCursor cursor) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IToolPresenter)

} // namespace Addle
#endif // ITOOLPRESENTER_HPP