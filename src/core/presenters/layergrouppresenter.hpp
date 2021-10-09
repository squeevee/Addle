#pragma once

#include <QObject>
#include "interfaces/models/ilayergroup.hpp"
#include "interfaces/presenters/ilayergrouppresenter.hpp"

#include "helpers/layernodehelperbase.hpp"

namespace Addle {
 
class LayerGroupPresenter 
    : public QObject, 
    public ILayerGroupPresenter,
    private LayerNodeHelperBase<LayerGroupPresenter>
{
    Q_OBJECT
    Q_INTERFACES(Addle::ILayerGroupPresenter Addle::ILayerNodePresenter)
    IAMQOBJECT_IMPL
    
    using HelperBase = LayerNodeHelperBase<LayerGroupPresenter>;
public:
    LayerGroupPresenter(
            IDocumentPresenter& document, 
            LayerNodeRef node,
            QSharedPointer<ILayerGroup> model
        );
    ~LayerGroupPresenter() = default;
    
    ConstLayerNodeRef layerNode() const override { return ConstLayerNodeRef(_layerNode); }
    LayerNodeRef layerNode() override { return _layerNode; }
    
    IDocumentPresenter& document() override { return _document; }
    const IDocumentPresenter& document() const override { return _document; }

    QSharedPointer<ILayerGroup> model() override { return _model; }
    QSharedPointer<const ILayerGroup> model() const override { return _model; }

    QString name() const override { return HelperBase::name_impl(); }
    void setName(QString name) override { HelperBase::setName_impl(name); }
    
    QPainter::CompositionMode compositionMode() const override { return HelperBase::compositionMode_impl(); }
    void setCompositionMode(QPainter::CompositionMode mode) override { HelperBase::setCompositionMode_impl(mode); }
    
    double opacity() const override { return HelperBase::opacity_impl(); }
    void setOpacity(double opacity) override { HelperBase::setOpacity_impl(opacity); }
    
    bool isVisible() const override { return HelperBase::isVisible; }
    void setVisibility(bool isVisible) override { HelperBase::setVisibility_impl(isVisible); }
    
signals:
    void nameChanged(QString name) override;
    void compositionModeChanged(QPainter::CompositionMode mode) override;
    void opacityChanged(double opacity) override;
    void visibilityChanged(bool isVisible) override;

public:
    RenderRoutine renderRoutine() const override { return _renderRoutine; }

signals:
    void renderRoutineChanged(RenderRoutineChangedEvent) override;
    void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) override;
    
private:
    IDocumentPresenter& _document;
    LayerNodeRef _layerNode;
    
    RenderRoutine _renderRoutine;
    
    QSharedPointer<ILayerGroup> _model;
    
    friend class LayerNodeHelperBase<LayerGroupPresenter>;
};
    
}
