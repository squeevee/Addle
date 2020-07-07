#ifndef NAVIGATETOOLPRESENTER_HPP
#define NAVIGATETOOLPRESENTER_HPP

#include <QObject>
#include "utilities/initializehelper.hpp"
#include "compat.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "toolhelpers/mousehelper.hpp"
#include "toolhelpers/toolselecthelper.hpp"

#include "utilities/presenter/propertycache.hpp"

class ADDLE_CORE_EXPORT NavigateToolPresenter : public QObject, public virtual INavigateToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        NavigateOperationOptions navigateOperation
        READ getNavigateOperation
        WRITE setNavigateOperation
        NOTIFY navigateOperationChanged
    )
public:
    NavigateToolPresenter()
        : _selectHelper(*this)
    {
        _mouseHelper.onMove += std::bind(&NavigateToolPresenter::onMove, this);
        _mouseHelper.onEngage += std::bind(&PropertyCache<QCursor>::recalculate, &_cache_cursor);
        _mouseHelper.onDisengage += std::bind(&PropertyCache<QCursor>::recalculate, &_cache_cursor);

        _cache_cursor.calculateBy(std::bind(&NavigateToolPresenter::cursor_p, this));
        _cache_cursor.onChange(&NavigateToolPresenter::cursorChanged, this);
    }
    virtual ~NavigateToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner,
        ICanvasPresenter* canvas,
        IViewPortPresenter* viewPort
    );

    IMainEditorPresenter* getOwner() { return _owner; }
    ToolId getId() { return INavigateToolPresenterAux::ID; }

    NavigateOperationOptions getNavigateOperation() { _initHelper.check(); return _operation; }

    bool event(QEvent* e) override { _mouseHelper.event(e); return e->isAccepted() || QObject::event(e); }

    bool isSelected() const { return _selectHelper.isSelected(); }
    void setSelected(bool isSelected) { _selectHelper.setSelected(isSelected); }

    QCursor cursor() { return _cache_cursor.getValue(); }

signals:
    void isSelectedChanged(bool isSelected);

public slots:
    void setNavigateOperation(NavigateOperationOptions operation);
signals:
    void navigateOperationChanged(NavigateOperationOptions operation);
    void cursorChanged(QCursor cursor);

private:
    void onMove();
    QCursor cursor_p();

    NavigateOperationOptions _operation = INavigateToolPresenterAux::DEFAULT_NAVIGATE_OPERATION_OPTION;

    IMainEditorPresenter* _owner;
    IViewPortPresenter* _viewPort;

    PropertyCache<QCursor> _cache_cursor;

    MouseHelper _mouseHelper;
    ToolSelectHelper _selectHelper;
    InitializeHelper<NavigateToolPresenter> _initHelper;
};

#endif // NAVIGATETOOLPRESENTER_HPP