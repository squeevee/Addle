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
#include "interfaces/rendering/irenderer.hpp"

#include "utilities/datatree/addledatatree.hpp"

namespace Addle {
class ADDLE_CORE_EXPORT Renderer : public QObject, public IRenderer
{
    Q_OBJECT 
    IAMQOBJECT_IMPL
public: 
    virtual ~Renderer() = default;

    void render(RenderHandle data) const override;
    
    void removeSteps(QList<QWeakPointer<IRenderable>> steps) override;
    
    void clear() override;
    
// public slots:
//     void rebuild() override {}
    
signals: 
    void changed(QRect area) override;
    //void needsRebuild(QWeakPointer<RendererBuilder>) override;

private slots:
    void onRenderableChange(QRect area);
    void onStepDestroyed(QObject* step);
    
private:
/*        
    QMap<StepLocation, QWeakPointer<IRenderable>> _steps;
    QMap<StepLocation, QPainterPath*> _pMasks;
    QMap<StepLocation, QRegion*> _rMasks;
    
    QHash<IRenderable*, StepLocation> _index_byStep;
    QHash<QObject*, StepLocation> _index_byQObject;
    
    QList<QMetaObject::Connection> _changeConnections;
    QList<QMetaObject::Connection> _destroyedConnections;*/
};
} // namespace Addle

#endif // RENDERSTACK_HPP
