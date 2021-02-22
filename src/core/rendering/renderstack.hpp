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
#include "utilities/render/renderstackbuilder.hpp"

#include "utilities/datatree.hpp"

namespace Addle {
class ADDLE_CORE_EXPORT RenderStack : public QObject, public IRenderStack
{
    Q_OBJECT 
    IAMQOBJECT_IMPL
public: 
    virtual ~RenderStack() = default;

    void render(RenderHandle data) const override;
    
    void removeSteps(QList<QWeakPointer<IRenderStep>> steps) override;
    
    void clear() override;
    
public slots:
    void rebuild() override
    {
        QMetaObject::invokeMethod(this, "rebuild_p", Qt::AutoConnection);
    }
    
signals: 
    void changed(QRect area) override;
    void needsRebuild(QWeakPointer<RenderStackBuilder>) override;

private slots:
    void onRenderStepChange(QRect area);
    void onStepDestroyed(QObject* step);
    
private:
    Q_INVOKABLE void rebuild_p();
        
    struct StepLocation
    {
        QList<int> groupAddress;
        double priority;
        
        bool operator<(const StepLocation& x) const
        {
            int commonDepth = qMin(groupAddress.size(), x.groupAddress.size()) - 1;
            for (int i = 0; i < commonDepth; ++i)
            {
                auto v1 = groupAddress[i];
                auto v2 = x.groupAddress[i];
                
                if (v1 != v2)
                    return v1 < v2;
            }
            // addresses are the same.
            
            if (groupAddress.size() > x.groupAddress.size())
                return groupAddress[commonDepth + 1] < x.priority;
            else if (x.groupAddress.size() > groupAddress.size())
                return x.priority < x.groupAddress[commonDepth + 1];
            else
                return priority < x.priority;
        }
    };
    
//     using structure_t = DataTree<QMap<double, QWeakPointer<IRenderStep>>>;
//     structure_t _structure;
    
    QMap<StepLocation, QWeakPointer<IRenderStep>> _steps;
    QMap<StepLocation, QPainterPath*> _pMasks;
    QMap<StepLocation, QRegion*> _rMasks;
    
    QHash<IRenderStep*, StepLocation> _index_byStep;
    QHash<QObject*, StepLocation> _index_byQObject;
    
    QList<QMetaObject::Connection> _changeConnections;
    QList<QMetaObject::Connection> _destroyedConnections;
};
} // namespace Addle

#endif // RENDERSTACK_HPP
