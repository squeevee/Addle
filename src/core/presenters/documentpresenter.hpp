#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include "compat.hpp"
#include <QList>
#include <QObject>
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"
#include <memory>

#include "helpers/documentlayershelper.hpp"

class ADDLE_CORE_EXPORT DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT

    enum InitCheckpoints
    {
        Layers
    };
public:
    DocumentPresenter()
        : _initHelper(this),
        _layersHelper(this)
    {

        _layersHelper.onLayersChanged += std::bind(&DocumentPresenter::layersChanged, this, std::placeholders::_1);
        _layersHelper.onLayersAdded += std::bind(&DocumentPresenter::layersAdded, this, std::placeholders::_1);
        _layersHelper.onLayerSelectionChanged += std::bind(&DocumentPresenter::layerSelectionChanged, this, std::placeholders::_1);
        _layersHelper.onTopSelectedLayerChanged += std::bind(&DocumentPresenter::topSelectedLayerChanged, this, std::placeholders::_1);
    }
    virtual ~DocumentPresenter() = default;

    void initialize(EmptyInitOptions option);
    void initialize(QSize size, QColor backgroundColor);
    void initialize(QSharedPointer<IDocument> model);

    QSharedPointer<IDocument> getModel() { _initHelper.check(); return _model; }
    bool isEmpty() { _initHelper.check(); return !_model; }

    QSize getSize() { _initHelper.check(); return _model ? _model->getSize() : QSize(); }
    QRect getRect() { return QRect(QPoint(), getSize()); }
    QColor getBackgroundColor() { _initHelper.check(); return _model ? _model->getBackgroundColor() : QColor(); }

    HeirarchyList<QSharedPointer<ILayerPresenter>> layers() const { _initHelper.check(); return _layersHelper.layers(); }

    QSet<LayerNode*> layerSelection() const { _initHelper.check(); return _layersHelper.layerSelection(); }
    void setLayerSelection(QSet<LayerNode*> selection) { _initHelper.check(); _layersHelper.setLayerSelection(selection); }
    void addLayerSelection(QSet<LayerNode*> added) { _initHelper.check(); _layersHelper.addLayerSelection(added); }
    void removeLayerSelection(QSet<LayerNode*> removed) { _initHelper.check(); _layersHelper.removeLayerSelection(removed); }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const { _initHelper.check(); return _layersHelper.topSelectedLayer(); }

public slots:
    LayerNode& addLayer() { _initHelper.check(); return _layersHelper.addLayer(); }
    LayerNode& addLayerGroup() { _initHelper.check(); return _layersHelper.addLayerGroup(); }
    
    void removeSelectedLayers() { }
    void moveSelectedLayers(int destination) { }
    void mergeSelectedLayers() { }

signals:
    void layersAdded(QList<IDocumentPresenter::LayerNode*> added);
    void layersRemoved(QList<IDocumentPresenter::LayerNode*> removed);
    void layersMoved(QList<IDocumentPresenter::LayerNode*> moved);

    void layersChanged(IDocumentPresenter::LayerList layers);
    void topSelectedLayerChanged(QSharedPointer<ILayerPresenter> layer);
    void layerSelectionChanged(QSet<IDocumentPresenter::LayerNode*> selection);

private:
    QSize _size;
    QColor _backgroundColor;

    QSharedPointer<IDocument> _model;

    DocumentLayersHelper _layersHelper;

    InitializeHelper<DocumentPresenter> _initHelper;
};

#endif // DOCUMENTPRESENTER_HPP