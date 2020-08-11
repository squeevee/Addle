#ifndef RENDERSUBSTACK_HPP
#define RENDERSUBSTACK_HPP

#include <QObject>
#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/rendering/irenderstack.hpp"
namespace Addle {

class RenderSubStack : public QObject, public IRenderStep
{
    Q_OBJECT
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
} // namespace Addle

#endif // RENDERSUBSTACK_HPP