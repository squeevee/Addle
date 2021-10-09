#pragma once

#include "interfaces/rendering/irenderable.hpp"

namespace Addle {

class MockRenderable : public QObject, public IRenderable
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    MockRenderable(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    
    virtual ~MockRenderable() = default;
    
    RenderRoutine _renderRoutine;
    RenderRoutine renderRoutine() const override { return _renderRoutine; }

signals:
    void renderRoutineChanged(RenderRoutineChangedEvent) override;
    void renderChanged(QRegion affected, DataTreeNodeAddress entity = {}) override;
};

}
