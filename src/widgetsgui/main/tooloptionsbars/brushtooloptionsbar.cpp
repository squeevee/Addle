#include "brushtooloptionsbar.hpp"

#include <utility>
#include <QMenu>
#include <QGridLayout>
#include <QToolButton>

#include <QWidgetAction>

#include "utilities/presenter/propertybinding.hpp"
#include "widgetsgui/utilities/widgetproperties.hpp"
#include "utilities/addle_text.hpp"
#include "globalconstants.hpp"

#include "widgetsgui/utilities/popupbutton.hpp"

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

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
    _action_brush_soft->setText(ADDLE_TEXT("brushes.soft-brush.text"));
    _action_brush_soft->setToolTip(ADDLE_TEXT("brushes.soft-brush.toolTip"));
    _optionGroup_brush->addOption(_action_brush_soft);

    QToolBar::addAction(_action_brush_soft);

    new PropertyBinding(
        _optionGroup_brush,
        WidgetProperties::value,
        qobject_interface_cast(&_presenter),
        IBrushToolPresenter::Meta::Properties::selectedBrush
    );

    _brushSelector = new AssetSelector(_presenter.brushSelection(), this);
    _button_brushSelector = new PopupButton(this);
    _button_brushSelector->setPopup(_brushSelector);
    connect(_brushSelector, &AssetSelector::changed, _button_brushSelector, &PopupButton::closePopup);

    _button_brushSelector->setText("Brushes...");

    QToolBar::addWidget(_button_brushSelector);

    // QToolBar::addWidget(_brushSelector);

    //QToolBar::addAction(_action_brush_aliasedCircle);
    //QToolBar::addAction(_action_brush_square);

    QToolBar::addSeparator();

    _button_sizeSelector = new SizeSelectorButton(this);
    _button_sizeSelector->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    _button_sizeSelector->setText(ADDLE_TEXT("tools.brush-tool.size-selection.text"));
    _button_sizeSelector->setToolTip(ADDLE_TEXT("tools.brush-tool.size-selection.toolTip"));
    
    QToolBar::addWidget(_button_sizeSelector);

    connect_interface(&_presenter,
        SIGNAL(brushChanged(BrushId)),
        this,
        SLOT(onBrushChanged())
    );

    {
        if (_presenter.selectedBrushPresenter())
        {
            PresenterAssignment<ISizeSelectionPresenter> sizePresenter(
                _presenter.selectedBrushPresenter()->sizeSelection(),
                _presenter.selectedBrushPresenter().toWeakRef()
            );

            _button_sizeSelector->setPresenter(sizePresenter);
            //_button_sizeSelect->setPresenter(sizeSelectionPresenter);
        }
    }
}

void BrushToolOptionsBar::onBrushChanged()
{
    PresenterAssignment<ISizeSelectionPresenter> sizePresenter(
        _presenter.selectedBrushPresenter()->sizeSelection(),
        _presenter.selectedBrushPresenter().toWeakRef()
    );

    _button_sizeSelector->setPresenter(sizePresenter);
    //_button_sizeSelect->setPresenter(sizeSelectionPresenter);
}