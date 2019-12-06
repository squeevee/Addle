#ifndef ITOOLPRESENTER_HPP
#define ITOOLPRESENTER_HPP

#include <QObject>
#include <QPoint>
#include <QIcon>
#include <QCursor>
#include "../idocumentpresenter.hpp"

#include "data/toolid.hpp"

#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class IToolPresenter
{
public:
    virtual ~IToolPresenter() = default;
    
    //virtual void initialize(IDocumentPresenter* owner) = 0;
    virtual IDocumentPresenter* getOwner() = 0;

    virtual ToolId getId() = 0;

    virtual void pointerEngage(QPointF canvasPos) = 0;
    virtual void pointerMove(QPointF canvasPos) = 0;
    virtual void pointerDisengage(QPointF canvasPos) = 0;

    virtual QCursor getCursorHint() = 0;
    virtual QIcon getIcon() = 0;
    virtual QString getName() = 0;
    virtual QString getToolTip() = 0;

    virtual QIcon getOptionIcon(const char* name) = 0;
    virtual QIcon getOptionIcon(const char* name, QVariant value) = 0;
    virtual QString getOptionText(const char* name) = 0;
    virtual QString getOptionText(const char* name, QVariant value) = 0;
    virtual QString getOptionToolTip(const char* name) = 0;
    virtual QString getOptionToolTip(const char* name, QVariant value) = 0;

    virtual bool isSelected() = 0;

public slots:
    virtual void select() = 0;

signals:
    virtual void selectionChanging(bool willBeSelected) = 0;
    virtual void selectionChanged(bool selected) = 0;

    virtual void cursorHintChanging() = 0;
    virtual void cursorHintChanged() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IToolPresenter)

#endif // ITOOLPRESENTER_HPP