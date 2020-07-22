#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QApplication>

#include "layersmanager.hpp"

#include "utilities/view/documentlayersitemmodel.hpp"

LayersManager::LayersManager(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle("Layers");

    QWidget* widget = new QWidget();
    setWidget(widget);

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    widget->setLayout(mainLayout);

    QBoxLayout* aboveRow = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->addLayout(aboveRow);

    _action_selectAllLayers = new QAction("Select All", this);
    _action_unSelectAllLayers = new QAction("Deselect All", this);

    _button_massSelection = new QToolButton();
    _button_massSelection->setDefaultAction(_action_selectAllLayers);
    aboveRow->addWidget(_button_massSelection);

    aboveRow->addItem(new QSpacerItem(
        0, 0, QSizePolicy::Expanding
    ));

    // _action_showAllLayers = new QAction(this);
    // _action_hideAllLayers = new QAction(this);

    // _button_massVisibility = new QToolButton();
    // _button_massVisibility->setDefaultAction(_action_hideAllLayers);
    // aboveRow->addWidget(_button_massVisibility);
    
    // aboveRow->addItem(new QSpacerItem(
    //     style()->pixelMetric(QStyle::PM_ScrollBarExtent), 0,
    //     QSizePolicy::Fixed
    // ));

    _itemModel = new DocumentLayersItemModel(this);

    _view = new QTreeView();
    _view->setModel(_itemModel);
    mainLayout->addWidget(_view);


    QBoxLayout* belowRow = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->addLayout(belowRow);

    _action_addLayer = new QAction("Add", this);

    _button_addLayer = new QToolButton();
    _button_addLayer->setDefaultAction(_action_addLayer);
    belowRow->addWidget(_button_addLayer);

    _action_addLayerGroup = new QAction("Add Group", this);

    _button_addLayerGroup = new QToolButton();
    _button_addLayerGroup->setDefaultAction(_action_addLayerGroup);
    belowRow->addWidget(_button_addLayerGroup);

    _action_removeLayers = new QAction("Remove", this);

    _button_removeLayers = new QToolButton();
    _button_removeLayers->setDefaultAction(_action_removeLayers);
    belowRow->addWidget(_button_removeLayers);

    _button_duplicateLayers = new QToolButton();
    belowRow->addWidget(_button_duplicateLayers);

    belowRow->addItem(new QSpacerItem(
        0, 0, QSizePolicy::Expanding
    ));

}

void LayersManager::setPresenter(PresenterAssignment<IDocumentPresenter> presenter)
{
    _presenter = presenter;
    _itemModel->setPresenter(_presenter);
}