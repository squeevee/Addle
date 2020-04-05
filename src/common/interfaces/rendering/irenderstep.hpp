#ifndef IRENDERSTEP_HPP
#define IRENDERSTEP_HPP

#include <QPainterPath>

#include "interfaces/traits/qobject_trait.hpp"
#include "utilities/render/renderdata.hpp"

class IRenderStep
{
public:
    virtual ~IRenderStep() = default;

    virtual void onPush(RenderData& data) = 0;
    virtual void onPop(RenderData& data) = 0;

    // virtual bool isVisible() const = 0;
    // virtual void hide() = 0;
    // virtual void unhide() = 0;

signals: 
    virtual void changed(QRect area) = 0;
    // virtual void removeFromStack() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IRenderStep);

#endif // IRENDERSTEP_HPP