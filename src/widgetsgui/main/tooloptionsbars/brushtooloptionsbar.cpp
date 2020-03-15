#include "brushtooloptionsbar.hpp"

#include "utilities/presenter/propertybinding.hpp"
#include "widgetsgui/utilities/decorationhelper.hpp"
#include "widgetsgui/utilities/widgetproperties.hpp"

BrushToolOptionsBar::BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    DecorationHelper brush_decorHelper(
        IBrushLikeToolPresenter::Meta::Properties::brush,
        _presenter
    );
    _optionGroup_brush = new OptionGroup(this);

    _action_brush_basic = new OptionAction(IBrushToolPresenter::DefaultBrushes::Basic, this);
    brush_decorHelper.decorateOption(_action_brush_basic);
    _optionGroup_brush->addOption(_action_brush_basic);

    //_action_brush_aliasedCircle = _actionGroup_brush->createAction(IBrushToolPresenter::DefaultBrushes::AliasedCircle);
    //_action_brush_square = _actionGroup_brush->createAction(IBrushToolPresenter::DefaultBrushes::Square);

    new PropertyBinding(
        _optionGroup_brush,
        WidgetProperties::value,
        qobject_interface_cast(&_presenter),
        IBrushLikeToolPresenter::Meta::Properties::brush
    );

    QToolBar::addAction(_action_brush_basic);
    //QToolBar::addAction(_action_brush_aliasedCircle);
    //QToolBar::addAction(_action_brush_square);

    QToolBar::addSeparator();
}