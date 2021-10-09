#pragma once

#include <QBrush>

#include "renderroutine.hpp"

namespace Addle::aux_render {
    
struct BrushEntity : Entity
{
    inline BrushEntity(QByteArray name_, QBrush brush_)
        : Entity(
            aux_render::EntityBindingBuilder(this)
                .setName(name_)
                .bindDraw<&BrushEntity::draw_p>()
        ),
        brush(brush_)
    {
    }
    
    QBrush brush;
    bool hidden = false;

private:
    void draw_p(QPainter* painter, const QRegion& drawRegion, bool*) const noexcept
    {
        assert(painter);
        if (Q_LIKELY(!hidden))
            painter->fillRect(drawRegion.boundingRect(), brush);
    }
};

}
