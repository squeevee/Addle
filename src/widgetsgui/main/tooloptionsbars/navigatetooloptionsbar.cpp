#include <QtDebug>

#include "navigatetooloptionsbar.hpp"

#include "utilities/widgetproperties.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"

using namespace Addle;

NavigateToolOptionsBar::NavigateToolOptionsBar(INavigateToolPresenter& presenter, QWidget* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{

    _optionGroup_navigateOperation = new OptionGroup(this);

    _action_navigateOperation_gripPan = new OptionAction(NavigateOperationOptions::gripPan, this);
    _action_navigateOperation_gripPan->setText(qtTrId("tools.navigate-tool.gripPan.text"));
    _action_navigateOperation_gripPan->setToolTip(qtTrId("tools.navigate-tool.gripPan.toolTip"));
    _optionGroup_navigateOperation->addOption(_action_navigateOperation_gripPan);

    _action_navigateOperation_gripPivot = new OptionAction(NavigateOperationOptions::gripPivot, this);
    _action_navigateOperation_gripPivot->setText(qtTrId("tools.navigate-tool.gripPivot.text"));
    _action_navigateOperation_gripPivot->setToolTip(qtTrId("tools.navigate-tool.gripPivot.toolTip"));
    _optionGroup_navigateOperation->addOption(_action_navigateOperation_gripPivot);

    new PropertyBinding(
        _optionGroup_navigateOperation, 
        WidgetProperties::value,
        qobject_interface_cast(&_presenter),
        INavigateToolPresenter::Meta::Properties::navigateOperation
    );

    QToolBar::addAction(_action_navigateOperation_gripPan);
    QToolBar::addAction(_action_navigateOperation_gripPivot);
    //QToolBar::addAction(_action_navigateOperation_rectangleZoomTo);
}
