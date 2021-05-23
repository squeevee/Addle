/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "renderer.hpp"

#include "utilities/qobject.hpp"

#include <QVector>
#include <utilities/collections.hpp>

using namespace Addle;

void Renderer::clear()
{
}

void Renderer::render(RenderHandle data) const
{
//     if (maxDepth == -1) maxDepth = _steps.size();
//     if (maxDepth == 0) return;
//     maxDepth = qMin(maxDepth, _steps.size());
// 
//     QVector<RenderHandle> stackData(qMin(maxDepth, _steps.size()));
// 
//     int depth = maxDepth;
//     RenderHandle lastData = data;
//     while (depth >= 1)
//     {
//         auto s_step = _steps[depth - 1].toStrongRef();
// 
//         RenderHandle stepData = lastData;
//         stepData.painter()->save();
//         
//         if (s_step) //temporary stop gap
//             s_step->onPush(stepData);
// 
//         stackData[depth - 1] = stepData;
//         lastData = stepData;
//     
//         --depth;
//     }
// 
//     while (depth < maxDepth)
//     {
//         auto s_step = _steps[depth].toStrongRef();
// 
//         RenderHandle stepData = stackData[depth];
//         if (s_step)
//             s_step->onPop(stepData);
//         stepData.painter()->restore();
// 
//         ++depth;
//     }
}

void Renderer::removeSteps(QList<QWeakPointer<IRenderable>> steps)
{
}

void Renderer::onRenderableChange(QRect area)
{
    emit changed(area);
}


void Renderer::onStepDestroyed(QObject* step_)
{
//     auto i = _index_byQObject[step_];
//     this->removeStep(_steps[i]);
}

// void Renderer::rebuild_p()
// {
//     assert(this->thread() == QThread::currentThread());
//     
//     _steps.clear();
//     _pMasks.clear();
//     _rMasks.clear();
//     //_groups.clear();
//     
//     _index_byQObject.clear();
//     _index_byStep.clear();
//     
//     structure_t oldStructure(std::move(_structure));
//     
//     QList<RendererBuilder::StepInfo> builderSteps;
//     QList<RendererBuilder::MaskInfo> builderMasks;
//     QList<RendererBuilder::CompositeModeInfo> builderCompositeModes;
//     
//     {
//         auto builder = QSharedPointer<RendererBuilder>::create(this->thread());
//         emit needsRebuild(builder);
//         std::move(*builder).result(builderSteps, builderMasks, builderCompositeModes);
//     }
//     
// //     for (const auto& stepInfo : noDetach(builderSteps))
// //     {
// //         TODO support lookup by address in DataTree
// //         
// //         auto node = &_structure.root();
// //         {
// //             for (int i : noDetach(stepInfo.groupAddress))
// //             {
// //                 node = &((*node)[i]);
// //             }
// //         }
// //     }
// }
