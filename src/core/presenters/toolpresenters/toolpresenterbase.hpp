#ifndef TOOLPRESENTER_HPP
#define TOOLPRESENTER_HPP

#include <QObject>
#include <QVariant>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QStringBuilder>

#include "../helpers/toolpathhelper.hpp"
#include "../helpers/propertydecorationhelper.hpp"

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"

class ToolPresenterBase : public QObject, public virtual IToolPresenter
{
    Q_OBJECT
public:
    ToolPresenterBase(ToolPathHelper::TrackingOptions pathTracking)
        : _toolPathHelper(pathTracking), _propertyDecorationHelper(this)
    {
    }
    virtual ~ToolPresenterBase() = default;

    IDocumentPresenter* getOwner() { _initHelper.assertInitialized(); return _documentPresenter; }

    QCursor getCursorHint();
    QIcon getIcon() { _initHelper.assertInitialized(); return _icon; }
    QString getName() { _initHelper.assertInitialized(); return _name; }
    QString getToolTip() { _initHelper.assertInitialized(); return _toolTip; }

    void pointerEngage(QPointF canvasPos);
    void pointerMove(QPointF canvasPos);
    void pointerDisengage(QPointF canvasPos);

    bool isSelected()
    {
        return _documentPresenter->getCurrentToolPresenter() == this;
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
    void initialize_p(IDocumentPresenter* documentPresenter);
    IDocumentPresenter* _documentPresenter;
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
    ToolPathHelper _toolPathHelper;

private:
    InitializeHelper<ToolPresenterBase> _initHelper;
};

#endif // TOOLTOOLPRESENTER_HPP