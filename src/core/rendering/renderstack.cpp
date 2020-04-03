#include "renderstack.hpp"

#include "utilities/qtextensions/qobject.hpp"

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

    auto s_step = step.toStrongRef();
    connect_interface(s_step.data(), SIGNAL(changed(QRect)), this, SLOT(onRenderStepChange(QRect)));
}


void RenderStack::remove(QWeakPointer<IRenderStep> step)
{
    _steps.removeAll(step);

    if (step)
    {
        auto s_step = step.toStrongRef();
        disconnect(qobject_interface_cast(s_step.data()), SIGNAL(changed(QRect)), this, SLOT(onRenderStepChange(QRect)));
    }
}

void RenderStack::render(RenderData data, int maxDepth)
{
    if (maxDepth == -1) maxDepth = _steps.size();
    if (maxDepth == 0) return;
    maxDepth = qMin(maxDepth, _steps.size());

    QVector<RenderData> stackData(qMin(maxDepth, _steps.size()));

    int depth = maxDepth;
    RenderData lastData = data;
    while (depth >= 1)
    {
        auto s_step = _steps[depth - 1].toStrongRef();

        RenderData stepData = lastData;
        stepData.getPainter()->save();
        s_step->before(stepData);

        stackData[depth - 1] = stepData;
        lastData = stepData;

        depth--;
    }

    while (depth < maxDepth)
    {
        depth++;

        auto s_step = _steps[depth - 1].toStrongRef();

        RenderData stepData = stackData[depth - 1];
        s_step->after(stepData);
        stepData.getPainter()->restore();
    }
}

void RenderStack::onRenderStepChange(QRect area)
{
    emit changed(area);
}