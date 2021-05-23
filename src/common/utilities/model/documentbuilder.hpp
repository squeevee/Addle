/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 */

#pragma once
#include <initializer_list>

#include <QString>
#include <QUrl>
#include <QColor>
#include <QList>

#include "layernodebuilder.hpp"

namespace Addle {

class DocumentBuilder
{
public:
    inline bool isDefault() const { return _isDefault; }
    
    inline QUrl url() const { return _url; }
    inline DocumentBuilder& setUrl(QUrl url) 
    { 
        _url = url;
        _isDefault = false;
        return *this; 
    }

    inline QColor backgroundColor() const { return _backgroundColor; }
    inline DocumentBuilder& setBackgroundColor(QColor backgroundColor)
    { 
        _backgroundColor = backgroundColor; 
        _isDefault = false;
        return *this; 
    }

    inline QList<LayerNodeBuilder> layerNodes() const { return _layerNodes; }
    inline DocumentBuilder& setLayerNodes(const std::initializer_list<LayerNodeBuilder>& layerNodes)
    { 
        _layerNodes = layerNodes; 
        _isDefault = false;
        return *this; 
    }

private:
    bool _isDefault = true;
    
    QUrl _url;
    QList<LayerNodeBuilder> _layerNodes;
    QColor _backgroundColor = Qt::transparent;
};

} // namespace Addle
