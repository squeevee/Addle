#pragma once

#include "utilities/render/renderroutine.hpp"

namespace Addle {

class MockRenderEntity : public aux_render::Entity
{
    auto build(QByteArray name, bool useRegion, bool useDraw)
    {
        auto b = aux_render::EntityBindingBuilder(this).setName(name);
            
        if (useRegion) b.bindRegion<&MockRenderEntity::_region>();
        if (useDraw) b.bindDraw<&MockRenderEntity::draw>();
            
        return b;
    }
    
public:
    MockRenderEntity(QByteArray name = QByteArray(), bool useRegion = true, bool useDraw = true)
        : aux_render::Entity(build(name, useRegion, useDraw))
    {
    }
                     
    virtual ~MockRenderEntity() noexcept = default;
    
    QRegion _region;

    std::function<void(QPainter*, const QRegion&, bool*)> _draw;
    void draw(QPainter* painter, const QRegion& region, bool* error) const noexcept 
    { 
        _draw(painter, region, error);
    }
};

}
