#ifndef IRENDERSTACK_HPP
#define IRENDERSTACK_HPP

#include <QWeakPointer>

#include "interfaces/traits.hpp"


#include "irenderstep.hpp"

class IRenderStack
{
public:
    virtual ~IRenderStack() = default;

    virtual void initialize(QWeakPointer<IRenderStep> step = QWeakPointer<IRenderStep>()) = 0;
    virtual void initialize(QList<QWeakPointer<IRenderStep>> steps) = 0;

    virtual QList<QWeakPointer<IRenderStep>> getSteps() const = 0;

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
DECL_IMPLEMENTED_AS_QOBJECT(IRenderStack);

#endif // IRENDERSTACK_HPP