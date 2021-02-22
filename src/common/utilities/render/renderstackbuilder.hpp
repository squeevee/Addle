#pragma once

#include <QSharedPointer>
#include <QThread>
#include <QMap>

#include "interfaces/rendering/irenderstack.hpp"
#include "interfaces/rendering/irenderstep.hpp"

namespace Addle {

class RenderStackBuilder
{
public:
    struct StepInfo
    {
        QWeakPointer<IRenderStep> step;
        QList<int> groupAddress;
        double priority;
    };
    
    struct MaskInfo
    {
        const QPainterPath* pMask = nullptr;
        const QRegion* rMask = nullptr;
        QList<int> groupAddress;
        double priority;
    };
    
    struct CompositeModeInfo
    {
        QPainter::CompositionMode mode;
        QList<int> groupAddress;
    };
    
    RenderStackBuilder(QThread* thread)
        : _thread(thread)
    {
    }
    
    RenderStackBuilder(const RenderStackBuilder&) = default;
    
    void addStep(const StepInfo& step)
    {
        assert(_thread == QThread::currentThread());
        _steps.append(step);
    }
    
    void addStep(QWeakPointer<IRenderStep> step, QList<int> groupAddress = {}, double priority = 0)
    {
        assert(_thread == QThread::currentThread());
        _steps.append({ step, groupAddress, priority });
    }
    
    void addStep(QWeakPointer<IRenderStep> step, int group, double priority = 0)
    {
        assert(_thread == QThread::currentThread());
        _steps.append({ step, QList<int>({group}), priority });
    }
    
    // Applies a subtractive mask to the render stack (provided as a
    // QPainterPath), which applies to all steps in groupAddress of strictly
    // lower priority
    void addMask(const QPainterPath& mask, QList<int> groupAddress, double priority)
    {
        assert(_thread == QThread::currentThread());
        _masks.append( { &mask, nullptr, groupAddress, priority });
    }
    
    // Applies a subtractive mask to the render stack (provided as a
    // QRegion), which applies to all steps in groupAddress of strictly lower
    // priority
    void addMask(const QRegion& mask, QList<int> groupAddress, double priority)
    {
        assert(_thread == QThread::currentThread());
        _masks.append( { nullptr, &mask, groupAddress, priority });
    }
    
    // NOTE: there is no mechanism in place to notify the stack if a mask is
    // destroyed or changes. Ideally, masks should be owned by render steps, and
    // the render step 
    
    // Returns the contents and invalidates the builder.
    //
    // For use ONLY by RenderStack. Contributors must not make any assumptions 
    // about other contributors or the order in which their contributions are 
    // made and so can't be allowed to know the contents of the builder.
    void result(QList<StepInfo>& steps, 
                QList<MaskInfo>& masks, 
                QList<CompositeModeInfo>& compositionModes) &&
    {
        assert(_thread == QThread::currentThread());
        
        steps = std::move(_steps);
        masks = std::move(_masks);
        compositionModes = std::move(_compositionModes);
        
        _thread = nullptr;
    }
    
private:
    QThread* _thread;
    QList<StepInfo> _steps;
    QList<MaskInfo> _masks;
    QList<CompositeModeInfo> _compositionModes;
};
    
} // namespace Addle
