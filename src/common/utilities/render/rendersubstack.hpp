#ifndef RENDERSUBSTACK_HPP
#define RENDERSUBSTACK_HPP

#include <QObject>
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/rendering/irenderstack.hpp"

class RenderSubStack : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(IRenderStep)
public:
    RenderSubStack(IRenderStack* stack, QObject* parent = nullptr);
    virtual ~RenderSubStack() = default;

    void before(RenderData& data) { }
    void after(RenderData& data);

signals: 
    void changed(QRect area);

private slots: 
    void onStackDestroyed();
    void onStackChanged(QRect area);

private: 
    IRenderStack* _stack;
};

#endif // RENDERSUBSTACK_HPP