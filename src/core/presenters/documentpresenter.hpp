/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include "compat.hpp"

#include <memory>
#include <QList>
#include <QObject>

#include "utils.hpp"

#include "interfaces/models/idocument.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"

#include "helpers/documentlayershelper.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::IDocumentPresenter)
    IAMQOBJECT_IMPL
    enum InitCheckpoints
    {
        Layers
    };
public:
    DocumentPresenter()
        : _layersHelper(this)
    {
        _layersHelper.onLayersChanged.bind(&DocumentPresenter::layersChanged, this);
        _layersHelper.onLayersAdded.bind(&DocumentPresenter::layersAdded, this);
        _layersHelper.onLayersRemoved.bind(&DocumentPresenter::layersRemoved, this);
        _layersHelper.onLayerSelectionChanged.bind(&DocumentPresenter::layerSelectionChanged, this);
        _layersHelper.onTopSelectedLayerChanged.bind(&DocumentPresenter::topSelectedLayerChanged, this);

    }
    virtual ~DocumentPresenter() = default;

    void initialize(EmptyInitOptions option);
    void initialize(QSize size, QColor backgroundColor);
    void initialize(QSharedPointer<IDocument> model);

    QSharedPointer<IDocument> model() { _initHelper.check(); return _model; }
    bool isEmpty() { _initHelper.check(); return !_model; }

    QSize size() { _initHelper.check(); return _model ? _model->size() : QSize(); }
    QRect rect() { return QRect(QPoint(), size()); }
    QColor backgroundColor() { _initHelper.check(); return _model ? _model->backgroundColor() : QColor(); }

    const LayerList& layers() const { _initHelper.check(); return _layersHelper.layers(); }

    QSet<LayerNode*> layerSelection() const { _initHelper.check(); return _layersHelper.layerSelection(); }
    void setLayerSelection(QSet<LayerNode*> selection) { _initHelper.check(); _layersHelper.setLayerSelection(selection); }
    void addLayerSelection(QSet<LayerNode*> added) { _initHelper.check(); _layersHelper.addLayerSelection(added); }
    void removeLayerSelection(QSet<LayerNode*> removed) { _initHelper.check(); _layersHelper.removeLayerSelection(removed); }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const { _initHelper.check(); return _layersHelper.topSelectedLayer(); }

public slots:
    void addLayer() { try { _initHelper.check(); _layersHelper.addLayer(); } ADDLE_SLOT_CATCH }
    void addLayerGroup() { try {_initHelper.check(); _layersHelper.addLayerGroup(); } ADDLE_SLOT_CATCH }
    
    void removeSelectedLayers() { try { _initHelper.check(); _layersHelper.removeSelectedLayers(); } ADDLE_SLOT_CATCH }
    void moveSelectedLayers(int destination) { }
    void mergeSelectedLayers() { }

signals:
    void layersAdded(QList<IDocumentPresenter::LayerNode*> added);
    void layersRemoved(QList<IDocumentPresenter::LayerNodeRemoved> removed);
    void layersMoved(QList<IDocumentPresenter::LayerNode*> moved);

    void layersChanged();
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter> layer);
    void layerSelectionChanged(QSet<IDocumentPresenter::LayerNode*> selection);

private:
    QSize _size;
    QColor _backgroundColor;

    QSharedPointer<IDocument> _model;

    DocumentLayersHelper _layersHelper;

    InitializeHelper _initHelper;
};
} // namespace Addle

#endif // DOCUMENTPRESENTER_HPP