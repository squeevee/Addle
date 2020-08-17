#ifndef ICANVASPRESENTER_HPP
#define ICANVASPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IMainEditorPresenter;
class ICanvasPresenter : public virtual IAmQObject
{
public:
    virtual ~ICanvasPresenter() = default; 

    virtual void initialize(IMainEditorPresenter& mainEditorPresenter) = 0;

    virtual IMainEditorPresenter& mainEditorPresenter() = 0;

    virtual QCursor cursor() const = 0;
    virtual QString statusTip() const = 0;

    virtual bool hasMouse() const = 0;
    virtual void setHasMouse(bool value) = 0;

signals:
    virtual void cursorChanged(QCursor cursor) = 0;
    virtual void statusTipChanged(QString statusTip) = 0;

    virtual void hasMouseChanged(bool value) = 0;
};

DECL_MAKEABLE(ICanvasPresenter);
DECL_EXPECTS_INITIALIZE(ICanvasPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ICanvasPresenter, "org.addle.ICanvasPresenter")

#endif // ICANVASPRESENTER_HPP