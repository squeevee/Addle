/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once
#include <initializer_list>

#include <QRect>
#include <QImage>
#include <QList>
#include <QPainter>

namespace Addle {

class LayerNodeBuilder
{
public:
    inline bool isDefault() const { return _isDefault; }
    
    inline bool isLayer() const { return !_isGroup; }
    inline bool isGroup() const { return _isGroup; }
    inline LayerNodeBuilder setIsGroup(bool isGroup = true)
    {
        _isGroup = isGroup;
        _isDefault = false;
        return *this;
    }
    
    inline QRect boundary() const { return _boundary; }
    inline LayerNodeBuilder& setBoundary(QRect boundary) 
    {
        _boundary = boundary; 
        _isDefault = false; 
        return *this; 
    }

    inline QImage data() const { return _data; }
    inline LayerNodeBuilder& setData(QImage data) 
    { 
        _data = data; 
        _isDefault = false; 
        return *this; 
    }
    
    inline QString name() const { return _name; }
    inline LayerNodeBuilder& setName(QString name) 
    { 
        _name = name; 
        _isDefault = false;
        return *this; 
    }
    
    inline QColor skirtColor() const { return _skirtColor; }
    inline LayerNodeBuilder& setSkirtColor(QColor skirtColor) 
    { 
        _skirtColor = skirtColor; 
        _isDefault = false; 
        return *this; 
    }

    inline bool passThroughMode() const { return _passThroughMode; }
    inline LayerNodeBuilder& setPassThroughMode(bool mode)
    {
        _passThroughMode = mode;
        _isDefault = false;
        return *this;
    }
    
    inline QPainter::CompositionMode compositionMode() const { return _compositionMode; }
    inline LayerNodeBuilder& setCompositionMode(QPainter::CompositionMode mode)
    {
        _compositionMode = mode;
        _isDefault = false; 
        return *this;
    }
    
    inline double opacity() const { return _opacity; }
    inline LayerNodeBuilder& setOpacity(double opacity) 
    { 
        _opacity = opacity; 
        _isDefault = false; 
        return *this; 
    }
    
    inline bool isVisible() const { return _isVisible; }
    inline LayerNodeBuilder& setVisibility(bool isVisible) 
    { 
        _isVisible = isVisible; 
        _isDefault = false; 
        return *this; 
    }
    
    inline QList<LayerNodeBuilder> childLayerNodes() const { return _childLayerNodes; }
    inline LayerNodeBuilder& setChildLayerNodes(const std::initializer_list<LayerNodeBuilder>& childLayerNodes)
    {
        _childLayerNodes = childLayerNodes;
        _isGroup = true;
        _isDefault = false;
        return *this;
    }
    
private:
    bool _isDefault = true;
    bool _isGroup = false;
    
    QRect _boundary;
    QImage _data;
    
    QString _name;
    
    QColor _skirtColor = Qt::transparent;
    bool _passThroughMode = false;
    QPainter::CompositionMode _compositionMode = QPainter::CompositionMode_SourceOver;
    double _opacity = 1.0;
    bool _isVisible = true;
    
    QList<LayerNodeBuilder> _childLayerNodes;
};

} // namespace Addle
