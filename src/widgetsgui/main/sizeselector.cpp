#include "sizeselector.hpp"

#include "utilities/qtextensions/qobject.hpp"

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

SizeSelector::SizeSelector(ISizeSelectionPresenter& presenter, QWidget* parent)
    : QWidget(parent), _presenter(presenter)
{
    QGridLayout* layout = new QGridLayout(this);
    QWidget::setLayout(layout);

    _list = new QListWidget(this);
    _list->setViewMode(QListView::IconMode);
    _list->setIconSize(QSize(64,64));
    layout->addWidget(_list, 0, 0, 1, 2);

    for (double preset : _presenter.presets())
    {
        QListWidgetItem* item = new QListWidgetItem(_list);
        item->setText(QString::number(preset));

        _items[preset] = item;
        _itemValues[item] = preset;
    }

    connect(_list, &QListWidget::itemSelectionChanged, this, &SizeSelector::onSelectionChanged);
    connect_interface(&_presenter, SIGNAL(presetIconsChanged(QList<QIcon>)), this, SLOT(updatePresetIcons()));

    QLabel* customPxLabel = new QLabel(this);
    customPxLabel->setText("Custom (px)");
    layout->addWidget(customPxLabel, 1, 0);

    QDoubleSpinBox* customPxSpinBox = new QDoubleSpinBox(this);
    layout->addWidget(customPxSpinBox, 1, 1);

    updatePresetIcons();
}

void SizeSelector::onSelectionChanged()
{
    if (!_list->selectedItems().isEmpty())
    {
        QListWidgetItem* item = _list->selectedItems().first();
        
        double size = _itemValues[item];
        _presenter.set(size);

        emit changed();
    }
}

void SizeSelector::updatePresetIcons()
{
    const QList<QIcon> icons = _presenter.presetIcons();

    if (icons.isEmpty()) return;

    int index = 0;
    for (double preset : _presenter.presets())
    {
        QListWidgetItem* item = _items[preset];
        QIcon icon = icons[index];

        item->setIcon(icon);

        index++;
    }
}

SizeSelectButton::SizeSelectButton(ISizeSelectionPresenter& presenter, QWidget* parent)
    : QToolButton(parent), _presenter(presenter)
{
    _sizeSelector = new SizeSelector(_presenter);

    setPopupMode(QToolButton::InstantPopup);

    QMenu* menu = new QMenu(this);
    QWidgetAction* selectorAction = new QWidgetAction(this);
    selectorAction->setDefaultWidget(_sizeSelector);
    menu->addAction(selectorAction);

    connect(_sizeSelector, &SizeSelector::changed, menu, &QWidget::close);
    connect_interface(&_presenter, SIGNAL(iconChanged(QIcon)), this, SLOT(setIcon_slot(QIcon)));
    
    setMenu(menu);
}