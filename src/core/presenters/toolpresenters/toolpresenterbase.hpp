#ifndef TOOLPRESENTER_HPP
#define TOOLPRESENTER_HPP

#include <QObject>
#include <QVariant>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QStringBuilder>

#include "../helpers/mousehelper.hpp"
#include "../helpers/propertydecorationhelper.hpp"

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ToolPresenterBase : public QObject, public virtual IToolPresenter
{
    Q_OBJECT
public:
    ToolPresenterBase()
        : _propertyDecorationHelper(this)
    {
    }
    virtual ~ToolPresenterBase() = default;

    IMainEditorPresenter* getOwner() { _initHelper.check(); return _mainEditorPresenter; }

    QCursor getCursorHint();
    QIcon getIcon() { _initHelper.check(); return _icon; }
    QString getName() { _initHelper.check(); return _name; }
    QString getToolTip() { _initHelper.check(); return _toolTip; }

    void pointerEngage(QPointF canvasPos);
    void pointerMove(QPointF canvasPos);
    void pointerDisengage(QPointF canvasPos);

    bool isSelected()
    {
        return _mainEditorPresenter->getCurrentToolPresenter() == this;
    }

    PropertyDecoration getPropertyDecoration(const char* propertyName) const
    { 
        return _propertyDecorationHelper.getPropertyDecoration(propertyName);
    }

public slots: 
    void select();

signals: 
    void selectionChanging(bool willBeSelected);
    void selectionChanged(bool selected);

    void cursorHintChanging();
    void cursorHintChanged();

protected:
    void initialize_p(IMainEditorPresenter* mainEditorPresenter);
    IMainEditorPresenter* _mainEditorPresenter;
    IViewPortPresenter* _viewPortPresenter;

    QIcon _icon;
    QCursor _defaultCursor;
    QCursor _engagedCursor;

    QString _toolTip;
    QString _name;

    virtual void onPointerEngage() { }
    virtual void onPointerMove() { }
    virtual void onPointerDisengage() { }

    PropertyDecorationHelper _propertyDecorationHelper;

private:
    InitializeHelper<ToolPresenterBase> _initHelper;
};

#endif // TOOLTOOLPRESENTER_HPP