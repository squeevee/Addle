#pragma once

#include <QObject>

#include "interfaces/models/ilayergroup.hpp"
#include "interfaces/models/ilayer.hpp"

#include "interfaces/services/ifactory.hpp"

namespace Addle {

class LayerGroup : public QObject, public ILayerGroup
{
    Q_OBJECT
    Q_INTERFACES(Addle::ILayerGroup)
    IAMQOBJECT_IMPL
public:
    LayerGroup(
            IDocument& document,
            LayerNodeRef layerNode, 
            const LayerNodeBuilder& builder,
            const IFactory<ILayer>& layerFactory,
            const IFactory<ILayerGroup>& layerGroupFactory
        );
    virtual ~LayerGroup() = default;
    
    IDocument& document() override { return _document; }
    const IDocument& document() const override { return _document; }
    
    LayerNodeRef layerNode() override { return _layerNode; }
    ConstLayerNodeRef layerNode() const override { return ConstLayerNodeRef(_layerNode); }
    
    QString name() const override { return _name; }
    void setName(QString name) override
    {
        if (_name != name)
        {
            _name = name;
            emit nameChanged(_name);
        }
    }
    
    QPainter::CompositionMode compositionMode() const override { return _compositionMode; }
    void setCompositionMode(QPainter::CompositionMode mode) override
    {
        if (_compositionMode != mode && !_passThroughMode)
        {
            _compositionMode = mode;
            emit compositionModeChanged(_compositionMode);
        }
    }
    
    bool passThroughMode() const override { return _passThroughMode; }
    void setPassThroughMode(bool mode) override
    {
        if (_passThroughMode != mode)
        {
            _passThroughMode = mode;
            QPainter::CompositionMode oldCompositionMode = _compositionMode;
            
            if (_passThroughMode)
                _compositionMode = (QPainter::CompositionMode)(-1);
            
            emit passThroughModeChanged(_passThroughMode);
            if (_compositionMode == oldCompositionMode)
                emit compositionModeChanged(_compositionMode);
        }
    }
    
    double opacity() const override { return _opacity; }
    void setOpacity(double opacity) override
    {
        opacity = qBound(0.0, opacity, 1.0);
        if (_opacity != opacity)
        {
            _opacity = opacity;
            emit opacityChanged(_opacity);
        }
    }
    
    QRect boundary() const override
    {
    }

    QColor skirtColor() const override
    {
    }
    
    void setSkirtColor(QColor skirtColor)
    {
    }
    
signals:
    void nameChanged(QString name) override;
    void opacityChanged(double opacity) override;
    void passThroughModeChanged(bool mode) override;
    void compositionModeChanged(QPainter::CompositionMode mode) override;
    
    void boundaryChanged(QRect boundary) override;
    void skirtColorChanged(QColor skirtColor) override;
private:
    IDocument& _document;
    LayerNodeRef _layerNode;
    
    QString _name;
    bool _passThroughMode = false;
    QPainter::CompositionMode _compositionMode = (QPainter::CompositionMode)(-1);
    double _opacity = 0.0;
    
    const IFactory<ILayer>& _layerFactory;
    const IFactory<ILayerGroup>& _layerGroupFactory;
};
    
}
