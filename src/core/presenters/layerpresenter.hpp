/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef LAYERPRESENTER_HPP
#define LAYERPRESENTER_HPP

#include "compat.hpp"
#include <QObject>

#include "helpers/layernodehelperbase.hpp"

#include "interfaces/models/ilayer.hpp"
#include "interfaces/presenters/ilayerpresenter.hpp"

#include "interfaces/rendering/irenderable.hpp"
#include "utilities/initializehelper.hpp"

namespace Addle {

class LayerPresenterRenderable;
class ADDLE_CORE_EXPORT LayerPresenter 
    : public QObject, 
    public ILayerPresenter,
    private LayerNodeHelperBase<LayerPresenter>
{
    Q_OBJECT
    Q_INTERFACES(Addle::ILayerPresenter Addle::ILayerNodePresenter)
    IAMQOBJECT_IMPL
    
    using HelperBase = LayerNodeHelperBase<LayerPresenter>;
public:
    LayerPresenter(
            IDocumentPresenter& document, 
            ILayerNodePresenter::LayerNode& layerNode,
            QSharedPointer<ILayer> model
        );
    virtual ~LayerPresenter() = default;

    ConstLayerNode layerNode() const override { return ConstLayerNode(&_layerNode); }
    const LayerNode& layerNode() override { return _layerNode; }
    
    IDocumentPresenter& document() override { return _document; }
    const IDocumentPresenter& document() const override { return _document; }

    QSharedPointer<ILayer> model() override { return _model; }
    QSharedPointer<const ILayer> model() const override { return _model; }

    QString name() const override { return HelperBase::name_impl(); }
    void setName(QString name) override { HelperBase::setName_impl(name); }
    
    QPainter::CompositionMode compositionMode() const override { return HelperBase::compositionMode_impl(); }
    void setCompositionMode(QPainter::CompositionMode mode) override { HelperBase::setCompositionMode_impl(mode); }
    
    double opacity() const override { return HelperBase::opacity_impl(); }
    void setOpacity(double opacity) override { HelperBase::setOpacity_impl(opacity); }
    
    bool isVisible() const override { return HelperBase::isVisible; }
    void setVisibility(bool isVisible) override { HelperBase::setVisibility_impl(isVisible); }
    
    QSharedPointer<IRenderable> renderable() const override { return nullptr; }
    
signals:
    void nameChanged(QString name) override;
    void compositionModeChanged(QPainter::CompositionMode mode) override;
    void opacityChanged(double opacity) override;
    void visibilityChanged(bool isVisible) override;

private:
    IDocumentPresenter& _document;
    ILayerNodePresenter::LayerNode& _layerNode;
    
    double _heldOpacity = 0;
    bool _isVisible;
    
    QSharedPointer<ILayer> _model;
    
    friend class LayerPresenterRenderable;
    friend class LayerNodeHelperBase<LayerPresenter>;
};

class LayerPresenterRenderable : public QObject, public IRenderable
{
    Q_OBJECT
    Q_INTERFACES(Addle::IRenderable)
    IAMQOBJECT_IMPL
public:
    LayerPresenterRenderable(LayerPresenter& owner) : _owner(owner) { }
    virtual ~LayerPresenterRenderable() = default;

    void render(RenderHandle& data) const override {}

    QRect areaHint() const override { return QRect(); }

signals: 
    void changed(QRect area) override;

private:
    LayerPresenter& _owner;
};
} // namespace Addle

#endif // LAYERPRESENTER_HPP
