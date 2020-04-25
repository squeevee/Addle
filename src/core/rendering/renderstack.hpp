#ifndef RENDERSTACK_HPP
#define RENDERSTACK_HPP

#include "compat.hpp"
#include "interfaces/rendering/irenderstack.hpp"

class ADDLE_CORE_EXPORT RenderStack : public QObject, public IRenderStack
{
    Q_OBJECT 
public: 
    virtual ~RenderStack() = default;

    void initialize(QWeakPointer<IRenderStep> step);
    void initialize(QList<QWeakPointer<IRenderStep>> steps);

    QList<QWeakPointer<IRenderStep>> getSteps() const { return _steps; }

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

#endif // RENDERSTACK_HPP