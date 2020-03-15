#include <QtDebug>

#include "navigatetooloptionsbar.hpp"

#include "widgetsgui/utilities/widgetproperties.hpp"
#include "widgetsgui/utilities/decorationhelper.hpp"
#include "utilities/qtextensions/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"

NavigateToolOptionsBar::NavigateToolOptionsBar(INavigateToolPresenter& presenter, QWidget* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    DecorationHelper decorHelper_navigateOperation(
        INavigateToolPresenter::Meta::Properties::navigateOperation,
        _presenter
    );

    _optionGroup_navigateOperation = new OptionGroup(this);

    _action_navigateOperation_gripPan = new OptionAction(NavigateOperationOptions::gripPan, this);
    decorHelper_navigateOperation.decorateOption(_action_navigateOperation_gripPan);
    _optionGroup_navigateOperation->addOption(_action_navigateOperation_gripPan);

    _action_navigateOperation_gripPivot = new OptionAction(NavigateOperationOptions::gripPivot, this);
    decorHelper_navigateOperation.decorateOption(_action_navigateOperation_gripPivot);
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
