#include "brushtooloptionsbar.hpp"

#include <utility>
#include <QMenu>
#include <QGridLayout>
#include <QToolButton>

#include <QWidgetAction>

#include "utilities/presenter/propertybinding.hpp"
#include "utilities/widgetproperties.hpp"
#include "globals.hpp"

#include "../favoriteassetspicker.hpp"

#include "utilities/popupbutton.hpp"

#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

using namespace Addle;

BrushToolOptionsBar::BrushToolOptionsBar(IBrushToolPresenter& presenter, QWidget* parent)
    : ToolOptionBarBase(presenter, parent),
    _presenter(presenter)
{
    _favoriteBrushes = new FavoriteAssetsPicker(_presenter.brushSelection(), this);
    QToolBar::addWidget(_favoriteBrushes);

    _brushSelector = new AssetSelector(_presenter.brushSelection(), this);
    _button_brushSelector = new PopupButton(this);
    _button_brushSelector->setPopup(_brushSelector);
    connect(_brushSelector, &AssetSelector::changed, _button_brushSelector, &PopupButton::closePopup);

    _button_brushSelector->setText(qtTrId("brush-tool.brush-selection.text"));

    QToolBar::addWidget(_button_brushSelector);

    QToolBar::addSeparator();

    _button_sizeSelector = new SizeSelectorButton(this);
    _button_sizeSelector->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    _button_sizeSelector->setText(qtTrId("tools.brush-tool.size-selection.text"));
    _button_sizeSelector->setToolTip(qtTrId("tools.brush-tool.size-selection.toolTip"));
    
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
        }
    }

    connect_interface(&_presenter,
        SIGNAL(refreshPreviews()),
        this, 
        SLOT(onRefreshPreviews())
    );
}

void BrushToolOptionsBar::onBrushChanged()
{
    PresenterAssignment<ISizeSelectionPresenter> sizePresenter(
        _presenter.selectedBrushPresenter()->sizeSelection(),
        _presenter.selectedBrushPresenter().toWeakRef()
    );

    _button_sizeSelector->setPresenter(sizePresenter);
}
void BrushToolOptionsBar::onRefreshPreviews()
{
    _button_sizeSelector->update();
    _brushSelector->update();
    _favoriteBrushes->update();
}