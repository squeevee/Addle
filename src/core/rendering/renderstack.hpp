/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef RENDERSTACK_HPP
#define RENDERSTACK_HPP

#include "compat.hpp"
#include "interfaces/rendering/irenderstack.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT RenderStack : public QObject, public IRenderStack
{
    Q_OBJECT 
    IAMQOBJECT_IMPL
public: 
    virtual ~RenderStack() = default;

    void initialize(QWeakPointer<IRenderStep> step);
    void initialize(QList<QWeakPointer<IRenderStep>> steps);

    QList<QWeakPointer<IRenderStep>> steps() const { return _steps; }

    void push(QWeakPointer<IRenderStep> step);
    void remove(QWeakPointer<IRenderStep> step);

    void render(RenderData data, int maxDepth);

signals: 
    void changed(QRect area);

private slots: 
    //void onStepDestroyed(QObject*)
    void onRenderStepChange(QRect);

private: 
    QList<QWeakPointer<IRenderStep>> _steps;
};
} // namespace Addle
#endif // RENDERSTACK_HPP