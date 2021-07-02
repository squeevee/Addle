/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef LAYERSMANAGER_HPP
#define LAYERSMANAGER_HPP

#include <QObject>
#include <QDockWidget>

#include <QAction>

#include <QSlider>
#include <QTreeView>

#include "interfaces/presenters/idocumentpresenter.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"
#include "utilities/presenter/presenterassignment.hpp"

namespace Addle {

class DocumentLayersItemModel;

class LayerItem;
class BackgroundItem;
class LayerPreview;
class LayersManager : public QDockWidget
{
    Q_OBJECT
public:
    LayersManager(QWidget* parent = nullptr);
    virtual ~LayersManager() = default;

    void setPresenter(PresenterAssignment<IDocumentPresenter> presenter);

private slots:
    void presenterSelectionChanged();

private:
    QAction* _action_selectAllLayers;
    QAction* _action_unSelectAllLayers;
    QAction* _action_showAllLayers;
    QAction* _action_hideAllLayers;

    QAction* _action_addLayer;
    QAction* _action_addLayerGroup;
    QAction* _action_removeLayers;
    QAction* _action_mergeLayers;
    QAction* _action_duplicateLayers;

    QToolButton* _button_massSelection;
    QToolButton* _button_massVisibility;

    QToolButton* _button_addLayer;
    QToolButton* _button_addLayerGroup;
    QToolButton* _button_removeLayers;
    QToolButton* _button_mergeLayers;
    QToolButton* _button_duplicateLayers;

    QTreeView* _view;
    DocumentLayersItemModel* _itemModel;

    PresenterAssignment<IDocumentPresenter> _presenter;
};

} // namespace Addle

#endif // LAYERSMANAGER_HPP
