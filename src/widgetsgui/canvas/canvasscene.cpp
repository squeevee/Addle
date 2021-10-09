/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "QtDebug"

#include <QGraphicsView>
#include "utilities/qobject.hpp"
#include "canvasscene.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include <QGraphicsSceneMouseEvent>

#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/imaineditorpresenter.hpp"

#include "utilities/canvas/canvasmouseevent.hpp"

#include "utilities/guiutils.hpp"

#include "docbackgrounditem.hpp"
#include "documentitem.hpp"
#include "layeritem.hpp"

#include "utils.hpp"

using namespace Addle;

const double MINIMUM_LAYER_Z = 1;

CanvasScene::CanvasScene(ICanvasPresenter& presenter, 
    const IFactory<IRenderer>& rendererFactory, 
    QObject* parent)
    : QGraphicsScene(parent), _presenter(presenter), _rendererFactory(rendererFactory)
{
    auto document = _presenter.document();
    if (document)
    {
        _documentItem = new DocumentItem(
            document, 
            _rendererFactory.makeUnique(
                IRenderer::Mode_Live,
                aux_IRenderer::renderable_ = document.get()
            ));
        addItem(_documentItem);
    }
    
//     _presenter = presenter;
// 
//     auto documentPresenter = _presenter.mainEditorPresenter().documentPresenter();
//     if (documentPresenter)
//     {
//         connect_interface(documentPresenter,
//             SIGNAL(layersChanged()),
//             this,
//             SLOT(layersUpdated())
//         );
//     }
// 
//     layersUpdated();
}

void CanvasScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
//     QGraphicsScene::mouseMoveEvent(mouseEvent);
//     if (mouseEvent->isAccepted())
//         return;
// 
//     CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
//     mouseEvent->setAccepted(
//         sendInterfaceEvent(&_presenter, &canvasMouseEvent) && canvasMouseEvent.isAccepted()
//     );
}

void CanvasScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
//     QGraphicsScene::mouseMoveEvent(mouseEvent);
//     if (mouseEvent->isAccepted())
//         return;
// 
//     CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
//     mouseEvent->setAccepted(
//         sendInterfaceEvent(&_presenter, &canvasMouseEvent) && canvasMouseEvent.isAccepted()
//     );
}

void CanvasScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
//     QGraphicsScene::mouseMoveEvent(mouseEvent);
//     if (mouseEvent->isAccepted())
//         return;
// 
//     CanvasMouseEvent canvasMouseEvent = graphicsMouseToCanvasMouseEvent(mouseEvent);
//     mouseEvent->setAccepted(
//         sendInterfaceEvent(&_presenter, &canvasMouseEvent) && canvasMouseEvent.isAccepted()
//     );
}

bool CanvasScene::event(QEvent* e)
{
//     switch (e->type())
//     {
//     case QEvent::Enter:
//         _presenter.setHasMouse(true);
//         break;
// 
//     case QEvent::Leave:
//         _presenter.setHasMouse(false);
//         break;
//     }
// 
//     return QGraphicsScene::event(e);
}

void CanvasScene::onDocumentChanged(QSharedPointer<IDocumentPresenter> document)
{
    if (_documentItem)
    {
        removeItem(_documentItem);
        delete _documentItem;
    }
    
    if (document)
    {
        _documentItem = new DocumentItem(
            document, 
            _rendererFactory.makeUnique(
                IRenderer::Mode_Live,
                aux_IRenderer::renderable_ = document.get()
            ));
        addItem(_documentItem);
    }
}
    
void CanvasScene::layersUpdated()
{
//     clear();
// 
//     QSharedPointer<IDocumentPresenter> document = _presenter.mainEditorPresenter().documentPresenter();
// 
//     if (!document) return;
// 
//     
//     DocBackgroundItem* background = new DocBackgroundItem(*_presenter.mainEditorPresenter().documentPresenter());
// 
//     addItem(background);
// 
//     double z = MINIMUM_LAYER_Z + document->layers().nodeCount() + 1;
// 
//     for (auto& node : noDetach(document->layers()))
//     {
//         if (!node.isValue()) continue;
//         
//         LayerItem* layerItem = new LayerItem(*node.asValue());
//         layerItem->setZValue(z);
//         addItem(layerItem);
// 
//         z -= 1.0;
//     }
}
