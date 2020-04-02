#ifndef IRENDERSTEP_HPP
#define IRENDERSTEP_HPP

#include <QPainterPath>

#include "interfaces/traits/qobject_trait.hpp"
#include "utilities/renderdata.hpp"

class IRenderStep
{
public:
    virtual ~IRenderStep() = default;

    virtual void before(RenderData& data) = 0;
    virtual void after(RenderData& data) = 0;

signals: 
    virtual void changed(QRect area) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IRenderStep);

#endif // IRENDERSTEP_HPP