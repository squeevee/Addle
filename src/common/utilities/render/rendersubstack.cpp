/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "rendersubstack.hpp"

#include "utilities/qobject.hpp"

using namespace Addle;

RenderSubStack::RenderSubStack(IRenderStack* stack, QObject* parent)
    : QObject(parent), _stack(stack)
{
    connect_interface(_stack, SIGNAL(destroyed()), this, SLOT(onStackDestroyed()));
    connect_interface(_stack, SIGNAL(changed(QRect)), this, SLOT(onStackChanged(QRect)));
}

void RenderSubStack::onPop(RenderData& data)
{
    if (_stack)
        _stack->render(data);
}

void RenderSubStack::onStackDestroyed()
{
    _stack = nullptr;
    deleteLater();
}

void RenderSubStack::onStackChanged(QRect area)
{
    emit changed(area);
}