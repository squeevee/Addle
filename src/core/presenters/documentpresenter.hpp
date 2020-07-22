#ifndef DOCUMENTPRESENTER_HPP
#define DOCUMENTPRESENTER_HPP

#include "compat.hpp"
#include <QList>
#include <QObject>
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"
#include <memory>

#include "helpers/layershelper.hpp"

class ADDLE_CORE_EXPORT DocumentPresenter : public QObject, public IDocumentPresenter
{
    Q_OBJECT

    enum InitCheckpoints
    {
        Layers
    };
public:
    DocumentPresenter() : _initHelper(this), _layersHelper(this) {}
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

    QSet<LayerNode> layerSelection() const { _initHelper.check(); return _layersHelper.layerSelection(); }
    void setLayerSelection(QSet<LayerNode> layer) { }
    void addLayerSelection(QSet<LayerNode> layer) { }
    void removeLayerSelection(QSet<LayerNode> layer) { }

    QSharedPointer<ILayerPresenter> topSelectedLayer() const { _initHelper.check(); return _layersHelper.topSelectedLayer(); }

    QSharedPointer<ILayerPresenter> addLayer() { return nullptr; }
    LayerNode addLayerGroup() { return nullptr; }
    
    void removeSelectedLayers() { }
    void moveSelectedLayers(int destination) { }
    void mergeSelectedLayers() { }

signals:
    void layersChanged(IDocumentPresenter::LayersList layers);
    void layerSelectionChanged(QSet<IDocumentPresenter::LayerNode> selection);

private:
    QSize _size;
    QColor _backgroundColor;

    QSharedPointer<IDocument> _model;

    LayersHelper _layersHelper;

    InitializeHelper<DocumentPresenter> _initHelper;
};

#endif // DOCUMENTPRESENTER_HPP