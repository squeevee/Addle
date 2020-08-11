#ifndef ICANVASPRESENTER_HPP
#define ICANVASPRESENTER_HPP

#include "interfaces/traits.hpp"



class IMainEditorPresenter;
class ICanvasPresenter
{
public:
    virtual ~ICanvasPresenter() = default; 

    virtual void initialize(IMainEditorPresenter* mainEditorPresenter) = 0;

    virtual IMainEditorPresenter* getMainEditorPresenter() = 0;

    virtual QCursor getCursor() = 0;
    virtual QString getStatusTip() = 0;

    virtual bool hasMouse() const = 0;
    virtual void setHasMouse(bool value) = 0;

signals:
    virtual void cursorChanged(QCursor cursor) = 0;
    virtual void statusTipChanged(QString statusTip) = 0;

    virtual void hasMouseChanged(bool value) = 0;
};

DECL_MAKEABLE(ICanvasPresenter);
DECL_EXPECTS_INITIALIZE(ICanvasPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(ICanvasPresenter);

#endif // ICANVASPRESENTER_HPP