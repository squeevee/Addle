#ifndef RENDERUTILS_HPP
#define RENDERUTILS_HPP

#include "interfaces/rendering/irenderstep.hpp"

void isolatedRender(IRenderStep& step, RenderData data)
{
    data.getPainter()->save();

    step.onPush(data);
    step.onPop(data);

    data.getPainter()->restore();
}

void isolatedRender(IRenderStep& step, QRect area, QPainter* painter)
{
    isolatedRender(step, RenderData(area, painter));
}

#endif // RENDERUTILS_HPP