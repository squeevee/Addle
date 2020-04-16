#ifndef ICANVASPRESENTER_HPP
#define ICANVASPRESENTER_HPP

#include "interfaces/traits/compat.hpp"

#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IMainEditorPresenter;
class ADDLE_COMMON_EXPORT ICanvasPresenter
{
public:
    virtual ~ICanvasPresenter() = default; 

    virtual void initialize(IMainEditorPresenter* mainEditorPresenter) = 0;

    virtual IMainEditorPresenter* getMainEditorPresenter() = 0;

    virtual QCursor getCursor() = 0;
    virtual QString getStatusTip() = 0;

signals:
    virtual void cursorChanged(QCursor cursor) = 0;
    virtual void statusTipChanged(QString statusTip) = 0;
};

DECL_MAKEABLE(ICanvasPresenter);
DECL_EXPECTS_INITIALIZE(ICanvasPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(ICanvasPresenter);

#endif // ICANVASPRESENTER_HPP