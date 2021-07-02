/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef LAYER_HPP
#define LAYER_HPP

#include "compat.hpp"
#include <QObject>
#include <QColor>
#include <QImage>
#include <QPoint>

#include <QTransform>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/models/idocument.hpp"

#include "interfaces/editing/irastersurface.hpp"

#include "utilities/initializehelper.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT Layer : public QObject, public ILayer
{
    Q_OBJECT
    Q_INTERFACES(Addle::ILayer)
    IAMQOBJECT_IMPL
public:
    Layer(
            IDocument& document,  
            LayerNodeRef layerNode,
            const LayerNodeBuilder& builder
        );
    virtual ~Layer() = default;
    
    IDocument& document() override { return _document; }
    const IDocument& document() const override { return _document; }
    
    LayerNodeRef layerNode() override { return _layerNode; }
    ConstLayerNodeRef layerNode() const override { return ConstLayerNodeRef(_layerNode); }
    
    QRect boundary() const override { return _boundary; }

    QColor skirtColor() const override { return _skirtColor; }
    void setSkirtColor(QColor skirtColor) override
    { 
        if (_skirtColor != skirtColor)
        {
            _skirtColor = skirtColor; 
            emit skirtColorChanged(_skirtColor);
        }
    }

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
        if (_compositionMode != mode)
        {
            _compositionMode = mode;
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
    
    QSharedPointer<IRasterSurface> rasterSurface() override { return _rasterSurface; }
    QSharedPointer<const IRasterSurface> rasterSurface() const override { return _rasterSurface; }
    
    QSharedPointer<IRenderable> renderable() override { return _renderable; }
    QSharedPointer<const IRenderable> renderable() const override { return _renderable; }

signals:
    void boundaryChanged(QRect boundary) override;
    void skirtColorChanged(QColor skirtColor) override;
    void nameChanged(QString name) override;
    void compositionModeChanged(QPainter::CompositionMode compositionMode) override;
    void opacityChanged(double opacity) override;
    
private:
    IDocument& _document;
    LayerNodeRef _layerNode;
    
    QRect _boundary;
    
    QString _name;
    QColor _skirtColor;
    QPainter::CompositionMode _compositionMode;
    double _opacity;
    
    QSharedPointer<IRasterSurface> _rasterSurface;
    QSharedPointer<IRenderable> _renderable;
};

} // namespace Addle
#endif // LAYER_HPP
