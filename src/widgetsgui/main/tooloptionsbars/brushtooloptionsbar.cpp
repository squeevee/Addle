#include "brushtooloptionsbar.hpp"

#include <QMenu>
#include <QGridLayout>
#include <QToolButton>

#include <QWidgetAction>

#include "utilities/presenter/propertybinding.hpp"
#include "widgetsgui/utilities/widgetproperties.hpp"
#include "utilities/addle_text.hpp"
#include "globalconstants.hpp"

BrushToolOptionsBar::BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    _optionGroup_brush = new OptionGroup(this);

    _action_brush_basic = new OptionAction(GlobalConstants::CoreBrushes::BasicBrush, this);
    _action_brush_basic->setText(ADDLE_TEXT("brushes.basic-brush.text"));
    _action_brush_basic->setToolTip(ADDLE_TEXT("brushes.basic-brush.toolTip"));
    _optionGroup_brush->addOption(_action_brush_basic);

    QToolBar::addAction(_action_brush_basic);

    _action_brush_soft = new OptionAction(GlobalConstants::CoreBrushes::SoftBrush, this);
    _action_brush_soft->setText(ADDLE_TEXT("brushes.brush-soft.text"));
    _action_brush_soft->setToolTip(ADDLE_TEXT("brushes.brush-soft.toolTip"));
    _optionGroup_brush->addOption(_action_brush_soft);

    QToolBar::addAction(_action_brush_soft);

    new PropertyBinding(
        _optionGroup_brush,
        WidgetProperties::value,
        qobject_interface_cast(&_presenter),
        IBrushLikeToolPresenter::Meta::Properties::brush
    );

    //QToolBar::addAction(_action_brush_aliasedCircle);
    //QToolBar::addAction(_action_brush_square);

    QToolBar::addSeparator();

    _button_sizeSelect = new SizeSelectButton(_presenter.sizeSelection(), this);

    QToolBar::addWidget(_button_sizeSelect);
    _button_sizeSelect->setText(ADDLE_TEXT("tools.brush-tool.size-selection.text"));
    _button_sizeSelect->setToolTip(ADDLE_TEXT("tools.brush-tool.size-selection.toolTip"));
}