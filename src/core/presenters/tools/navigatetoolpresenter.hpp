/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef NAVIGATETOOLPRESENTER_HPP
#define NAVIGATETOOLPRESENTER_HPP

#include <QObject>
#include "utilities/initializehelper.hpp"
#include "compat.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "interfaces/presenters/iviewportpresenter.hpp"
#include "toolhelpers/mousehelper.hpp"
#include "toolhelpers/toolselecthelper.hpp"

#include "utilities/lazyvalue.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT NavigateToolPresenter : public QObject, public virtual INavigateToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        NavigateOperationOptions navigateOperation
        READ navigateOperation
        WRITE setNavigateOperation
        NOTIFY navigateOperationChanged
    )
    Q_INTERFACES(Addle::IToolPresenter Addle::INavigateToolPresenter)
    IAMQOBJECT_IMPL
public:
    NavigateToolPresenter()
        : _selectHelper(*this)
    {
        _mouseHelper.onMove.bind(&NavigateToolPresenter::onMove, this);
        _mouseHelper.onEngage.bind(&LazyProperty<QCursor>::recalculate, &_cache_cursor);
        _mouseHelper.onDisengage.bind(&LazyProperty<QCursor>::recalculate, &_cache_cursor);

        _cache_cursor.calculateBy(&NavigateToolPresenter::cursor_p, this);
        _cache_cursor.onChange.bind(&NavigateToolPresenter::cursorChanged, this);
    }
    virtual ~NavigateToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    IMainEditorPresenter* owner() { return _owner; }
//     ToolId id() const { return CoreTools::Navigate; }

    NavigateOperationOptions navigateOperation() { ASSERT_INIT(); return _operation; }

    bool event(QEvent* e) override;

    bool isSelected() const { return _selectHelper.isSelected(); }
    void setSelected(bool isSelected) { _selectHelper.setSelected(isSelected); }

    QCursor cursor() { return _cache_cursor.value(); }

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

    LazyProperty<QCursor> _cache_cursor;

    MouseHelper _mouseHelper;
    ToolSelectHelper _selectHelper;
    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // NAVIGATETOOLPRESENTER_HPP
