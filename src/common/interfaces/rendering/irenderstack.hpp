/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IRENDERSTACK_HPP
#define IRENDERSTACK_HPP

#include <QWeakPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


#include "irenderstep.hpp"
namespace Addle {

class IRenderStack : public virtual IAmQObject
{
public:
    virtual ~IRenderStack() = default;

    virtual void initialize(QWeakPointer<IRenderStep> step = QWeakPointer<IRenderStep>()) = 0;
    virtual void initialize(QList<QWeakPointer<IRenderStep>> steps) = 0;

    virtual QList<QWeakPointer<IRenderStep>> steps() const = 0;

    virtual void push(QWeakPointer<IRenderStep> step) = 0;
    virtual void remove(QWeakPointer<IRenderStep> step) = 0;
    // virtual void remove(const char* className) = 0;

    virtual void render(RenderData data, int maxDepth = -1) = 0;
    // may support more selective partial rendering

signals: 
    virtual void changed(QRect area) = 0;
};

DECL_MAKEABLE(IRenderStack);
DECL_EXPECTS_INITIALIZE(IRenderStack);

} // namespace Addle

#endif // IRENDERSTACK_HPP