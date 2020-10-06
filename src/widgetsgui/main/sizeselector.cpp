/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "sizeselector.hpp"

#include "utilities/qobject.hpp"

#include "utilities/presenter/propertybinding.hpp"
#include "utilities/strings.hpp"

#include <QtDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QDoubleSpinBox>

#include <QMenu>
#include <QWidgetAction>
#include <QGridLayout>
#include <QToolButton>
#include <QPaintEvent>
#include <QPainter>

using namespace Addle;

SizeSelector::SizeSelector(QWidget* parent)
    : QWidget(parent)
{
    QGridLayout* layout = new QGridLayout(this);
    QWidget::setLayout(layout);

    _list = new QListWidget(this);
    _list->setViewMode(QListView::IconMode);
    _list->setIconSize(QSize(64,64));
    layout->addWidget(_list, 0, 0, 1, 2);

    connect(_list, &QListWidget::itemSelectionChanged, this, &SizeSelector::onSelectionChanged);
    
    QLabel* customPxLabel = new QLabel(this);
    customPxLabel->setText(qtTrId("ui.size-selector.custom-label"));
    layout->addWidget(customPxLabel, 1, 0);

    _customPxSpinBox = new QDoubleSpinBox(this);
    _customPxSpinBox->setSuffix(qtTrId("units.pixels"));
    layout->addWidget(_customPxSpinBox, 1, 1);

    updatePresets();
}

void SizeSelector::onSelectionChanged()
{
    if (!_list->selectedItems().isEmpty())
    {
        if (!_presenter) return;

        QListWidgetItem* item = _list->selectedItems().first();
        
        double size = _itemValues[item];
        _presenter->set(size);

        emit changed();
    }
}

void SizeSelector::updatePresets()
{
    _list->clear();

    if (!_presenter) return;

    const QList<QIcon> icons = _presenter->presetIcons();

    int index = 0;
    for (double preset : _presenter->presets())
    {   
        QListWidgetItem* item = new QListWidgetItem(_list);
        item->setText(affixUnits(LayoutUnits::pixels, preset));

        _items[preset] = item;
        _itemValues[item] = preset;

        if (icons.size() > index)
        {
            QIcon icon = icons[index];
            item->setIcon(icon);
        }

        _list->addItem(item);

        index++;
    }

    if (_presenter->selectedPreset() != -1)
    {
        double selectedPreset = _presenter->get();
        _items.value(selectedPreset)->setSelected(true);
    }
}

void SizeSelector::setPresenter(const PresenterAssignment<ISizeSelectionPresenter>& presenter)
{
    if (presenter == _presenter) return;
    _presenter = presenter;
    
    _presenter.connect(
        SIGNAL(presetsChanged(QList<double>)),
        this, SLOT(updatePresets()));

    _presenter.connect(
        SIGNAL(refreshPreviews()),
        this, SLOT(update()));

    if (_customPxBinding) delete _customPxBinding;

    _customPxSpinBox->setMinimum(_presenter->min());
    _customPxSpinBox->setMaximum(qMin(_presenter->max(), 6000.0));
    if (_presenter->strictSizing())
    {
        _customPxSpinBox->setEnabled(false);
        _customPxBinding = new PropertyBinding(_customPxSpinBox, "value",
            _presenter.data(), "size",
            PropertyBinding::ReadOnly);
    }
    else
    {
        _customPxSpinBox->setEnabled(true);
        _customPxBinding = new PropertyBinding(_customPxSpinBox, "value",
            _presenter.data(), "size",
            PropertyBinding::ReadWrite);
    }

    updatePresets();
}

SizeSelectorButton::SizeSelectorButton(QWidget* parent)
    : PopupButton(parent)
{
    _sizeSelector = new SizeSelector();
    
    setPopup(_sizeSelector);
    connect(_sizeSelector, &SizeSelector::changed, this, &PopupButton::closePopup);
}

void SizeSelectorButton::setPresenter(const PresenterAssignment<ISizeSelectionPresenter>& presenter)
{
    if (presenter == _presenter) return;

    _sizeSelector->setPresenter(presenter);
    _presenter = presenter;

    _presenter.connect(
        SIGNAL(changed(double)),
        this, SLOT(onChange()));

    _presenter.connect(
        SIGNAL(refreshPreviews()),
        this, SLOT(update()));

    onChange();
}

void SizeSelectorButton::onChange()
{
    QIcon icon;
    if (_presenter)
        icon = _presenter->icon();
    setIcon(icon);

    double value = _presenter->get();
    if (!qIsNaN(value))
        setText(affixUnits(LayoutUnits::pixels, value));
}
