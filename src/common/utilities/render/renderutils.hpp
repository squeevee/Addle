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
#include "interfaces/rendering/irenderable.hpp"
#include "interfaces/rendering/irenderer.hpp"
#include "servicelocator.hpp"
namespace Addle {

// inline void render(QList<QWeakPointer<IRenderable>> steps, RenderHandle data)
// {
// //     auto stack = ServiceLocator::makeUnique<IRenderer>(steps);
// //     stack->render(data);
// }

// inline void render(QList<QWeakPointer<IRenderable>> steps, QRect area, QPainter* painter)
// {
//     RenderHandle data(area, painter);
//     render(steps, data);
// }

// inline void render(QWeakPointer<IRenderable> step, QRect area, QPainter* painter)
// {
//     render(QList<QWeakPointer<IRenderable>> { step }, area, painter);
// }
// void isolatedRender(IRenderable& step, RenderHandle data)
// {
//     data.painter()->save();

//     step.onPush(data);
//     step.onPop(data);

//     data.painter()->restore();
// }

// void isolatedRender(IRenderable& step, QRect area, QPainter* painter)
// {
//     isolatedRender(step, RenderHandle(area, painter));
// }
} // namespace Addle

#endif // RENDERUTILS_HPP
