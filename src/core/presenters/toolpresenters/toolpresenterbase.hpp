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

#include "core/compat.hpp"
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ADDLE_CORE_EXPORT ToolPresenterBase : public QObject, public virtual IToolPresenter
{
    Q_OBJECT
public:
    ToolPresenterBase()
        : _mouseHelper(*this), _propertyDecorationHelper(this)
    {
        connect(this, &ToolPresenterBase::selectionChanged, this, &ToolPresenterBase::onSelectionChanged); // woof.
    }
    virtual ~ToolPresenterBase() = default;

    IMainEditorPresenter* getOwner() { _initHelper.check(); return _mainEditorPresenter; }

    QCursor getCursorHint();
    QIcon getIcon() { _initHelper.check(); return _icon; }
    QString getName() { _initHelper.check(); return _name; }
    QString getToolTip() { _initHelper.check(); return _toolTip; }

    bool isSelected()
    {
        return _mainEditorPresenter->getCurrentToolPresenter() == this;
    }

    PropertyDecoration getPropertyDecoration(const char* propertyName) const
    { 
        return _propertyDecorationHelper.getPropertyDecoration(propertyName);
    }

    virtual bool event(QEvent* e);

public slots: 
    void select();

signals: 
    void selectionChanging(bool willBeSelected);
    void selectionChanged(bool selected);

    void cursorHintChanging();
    void cursorHintChanged();

protected:
    void initialize_p(IMainEditorPresenter* mainEditorPresenter);

    virtual void onEngage() { }
    virtual void onMove() { }
    virtual void onDisengage() { }

    virtual void onSelected() { }
    virtual void onDeselected() { }

protected:

    IMainEditorPresenter* _mainEditorPresenter;
    IViewPortPresenter* _viewPortPresenter;

    QIcon _icon;
    QCursor _defaultCursor;
    QCursor _engagedCursor;

    QString _toolTip;
    QString _name;

    MouseHelper _mouseHelper;

    PropertyDecorationHelper _propertyDecorationHelper;

private slots:

    void onSelectionChanged(bool selected)
    {
        if (selected)
            onSelected();
        else 
            onDeselected();
    }

private:
    InitializeHelper<ToolPresenterBase> _initHelper;
};

#endif // TOOLTOOLPRESENTER_HPP