#include "renderstack.hpp"

#include <QVector>

void RenderStack::initialize(QWeakPointer<IRenderStep> step)
{
    if (step)
    {
        initialize(QList<QWeakPointer<IRenderStep>>({ step }));
    }
}

void RenderStack::initialize(QList<QWeakPointer<IRenderStep>> steps)
{
    _steps = steps;
}

void RenderStack::push(QWeakPointer<IRenderStep> step)
{
    _steps.append(step);
}

void RenderStack::render(RenderData data, int maxDepth)
{
    if (maxDepth == -1) maxDepth = _steps.size();
    if (maxDepth == 0) return;

    QVector<RenderData> stackData(qMin(maxDepth, _steps.size()));

    int depth = 1;
    RenderData lastData = data;
    while (depth <= qMin(maxDepth, _steps.size()))
    {
        auto s_step = _steps[depth - 1].toStrongRef();

        RenderData stepData = lastData;
        stepData.getPainter()->save();
        s_step->before(stepData);

        stackData[depth - 1] = stepData;
        lastData = stepData;

        depth++;
    }

    while (depth > 1)
    {
        depth--;

        auto s_step = _steps[depth - 1].toStrongRef();

        RenderData stepData = stackData[depth - 1];
        s_step->after(stepData);
        stepData.getPainter()->restore();
    }
}