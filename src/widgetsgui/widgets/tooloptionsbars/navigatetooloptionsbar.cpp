#include <QtDebug>

#include "navigatetooloptionsbar.hpp"

#include "utilities/qt_extensions/qobject.hpp"

NavigateToolOptionsBar::NavigateToolOptionsBar(INavigateToolPresenter& presenter, QMainWindow* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    _actionGroup_navigateOperation = new OptionActionGroup(this, _presenter, "navigateOperation");

    _action_navigateOperation_gripPan = _actionGroup_navigateOperation->createAction(NavigateOperationOptions::gripPan);
    _action_navigateOperation_gripPivot = _actionGroup_navigateOperation->createAction(NavigateOperationOptions::gripPivot);
    _action_navigateOperation_rectangleZoomTo = _actionGroup_navigateOperation->createAction(NavigateOperationOptions::rectangleZoomTo);

    QToolBar::addAction(_action_navigateOperation_gripPan);
    QToolBar::addAction(_action_navigateOperation_gripPivot);
    QToolBar::addAction(_action_navigateOperation_rectangleZoomTo);

    //_enumHelper_operation.setValue(_presenter.getNavigateOperation());
}
