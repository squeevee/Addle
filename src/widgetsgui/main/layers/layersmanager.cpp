/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QApplication>

#include <QSignalBlocker>

#include "layersmanager.hpp"

#include "utilities/view/documentlayersitemmodel.hpp"

using namespace Addle;

LayersManager::LayersManager(QWidget* parent)
    : QDockWidget(parent)
{
    setWindowTitle(qtTrId("ui.layers.title"));

    QWidget* widget = new QWidget();
    setWidget(widget);

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    widget->setLayout(mainLayout);

    QBoxLayout* aboveRow = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->addLayout(aboveRow);

    _action_selectAllLayers = new QAction(qtTrId("ui.select-all-layers.name"), this);
    _action_unSelectAllLayers = new QAction(qtTrId("ui.unselect-all-layers.name"), this);

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

    connect(_view->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &LayersManager::viewSelectionChanged);

    QBoxLayout* belowRow = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->addLayout(belowRow);

    _action_addLayer = new QAction(qtTrId("ui.add-layer.name"), this);

    _button_addLayer = new QToolButton();
    _button_addLayer->setDefaultAction(_action_addLayer);
    belowRow->addWidget(_button_addLayer);

    _action_addLayerGroup = new QAction(qtTrId("ui.add-layer-group.name"), this);

    _button_addLayerGroup = new QToolButton();
    _button_addLayerGroup->setDefaultAction(_action_addLayerGroup);
    belowRow->addWidget(_button_addLayerGroup);

    _action_removeLayers = new QAction(qtTrId("ui.remove-layer.name"), this);

    _button_removeLayers = new QToolButton();
    _button_removeLayers->setDefaultAction(_action_removeLayers);
    belowRow->addWidget(_button_removeLayers);

    _button_duplicateLayers = new QToolButton();
    belowRow->addWidget(_button_duplicateLayers);

    belowRow->addItem(new QSpacerItem(
        0, 0, QSizePolicy::Expanding
    ));

    widget->setEnabled(false);
}

void LayersManager::setPresenter(PresenterAssignment<IDocumentPresenter> presenter)
{
    _presenter = presenter;
    _itemModel->setPresenter(_presenter);

    if (_presenter)
    {
        _presenter.connect(
            _action_addLayer, SIGNAL(triggered()), 
                                SLOT(addLayer())
        );
        _presenter.connect(
            _action_addLayerGroup, SIGNAL(triggered()), 
                                     SLOT(addLayerGroup())
        );
        _presenter.connect(
            _action_removeLayers, SIGNAL(triggered()),
                                    SLOT(removeSelectedLayers())
        );
//         _presenter.connect(
//                 SIGNAL(layerSelectionChanged(QSet<IDocumentPresenter::LayerNode*>)),
//             this, SLOT(presenterSelectionChanged())
//         );
    }

    presenterSelectionChanged();

    if (widget())
        widget()->setEnabled((bool)_presenter);
}

void LayersManager::viewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
//     if(!_presenter) return;
// 
//     QSet<IDocumentPresenter::LayerNode*> newSelection;
// 
//     for (QModelIndex index : selected.indexes())
//     {
//         newSelection.insert(DocumentLayersItemModel::nodeAt(index));
//     }
// 
//     _presenter->setLayerSelection(newSelection);
}

void LayersManager::presenterSelectionChanged()
{
//     QItemSelectionModel* selectionModel = _view->selectionModel();
//     if (!_presenter)
//     {
//         selectionModel->clear();
//         return;
//     }
// 
//     QItemSelection selection;
//     
//     // inefficient
//     for (IDocumentPresenter::LayerNode* node : _presenter->layerSelection())
//     {
//         QModelIndex index = _itemModel->indexOf(node);
//         selection.select(index, index);
//     }
// 
//     {
//         const QSignalBlocker block(selectionModel);
//         selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
//     }
}
