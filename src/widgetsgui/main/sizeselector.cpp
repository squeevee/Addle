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
    customPxLabel->setText("Custom (px)");
    layout->addWidget(customPxLabel, 1, 0);

    QDoubleSpinBox* customPxSpinBox = new QDoubleSpinBox(this);
    layout->addWidget(customPxSpinBox, 1, 1);

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
        item->setText(QString::number(preset));

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
        _items[selectedPreset]->setSelected(true);
    }
}

void SizeSelector::setPresenter(const PresenterAssignment<ISizeSelectionPresenter>& presenter)
{
    if (presenter == _presenter) return;
    _presenter = presenter;
    
    _presenter.connect(
        SIGNAL(presetIconsChanged(QList<QIcon>)),
        this, SLOT(updatePresets()));
    
    updatePresets();
}
