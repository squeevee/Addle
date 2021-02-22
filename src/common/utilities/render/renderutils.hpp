/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RENDERUTILS_HPP
#define RENDERUTILS_HPP

#include <QList>
#include <QWeakPointer>
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/rendering/irenderstack.hpp"
#include "servicelocator.hpp"
namespace Addle {

inline void render(QList<QWeakPointer<IRenderStep>> steps, RenderHandle data)
{
    auto stack = ServiceLocator::makeUnique<IRenderStack>(steps);
    stack->render(data);
}

inline void render(QList<QWeakPointer<IRenderStep>> steps, QRect area, QPainter* painter)
{
    RenderHandle data(area, painter);
    render(steps, data);
}

inline void render(QWeakPointer<IRenderStep> step, QRect area, QPainter* painter)
{
    render(QList<QWeakPointer<IRenderStep>> { step }, area, painter);
}
// void isolatedRender(IRenderStep& step, RenderHandle data)
// {
//     data.painter()->save();

//     step.onPush(data);
//     step.onPop(data);

//     data.painter()->restore();
// }

// void isolatedRender(IRenderStep& step, QRect area, QPainter* painter)
// {
//     isolatedRender(step, RenderHandle(area, painter));
// }
} // namespace Addle

#endif // RENDERUTILS_HPP