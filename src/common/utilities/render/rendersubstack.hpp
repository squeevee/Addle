#ifndef RENDERSUBSTACK_HPP
#define RENDERSUBSTACK_HPP

#include <QObject>
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/rendering/irenderstack.hpp"
#include "interfaces/traits/compat.hpp"

class ADDLE_COMMON_EXPORT RenderSubStack : public QObject, public IRenderStep
{
    Q_OBJECT
    Q_INTERFACES(IRenderStep)
public:
    RenderSubStack(IRenderStack* stack, QObject* parent = nullptr);
    virtual ~RenderSubStack() = default;

    void onPush(RenderData& data) { }
    void onPop(RenderData& data);

signals: 
    void changed(QRect area);

private slots: 
    void onStackDestroyed();
    void onStackChanged(QRect area);

private: 
    IRenderStack* _stack;
};

#endif // RENDERSUBSTACK_HPP