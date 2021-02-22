/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IRENDERSTACK_HPP
#define IRENDERSTACK_HPP

#include <QWeakPointer>
#include <QList>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"


#include "irenderstep.hpp"
namespace Addle {

class RenderStackBuilder;
class IRenderStack : public virtual IAmQObject
{
public:    
    virtual ~IRenderStack() = default;

    //TODO
    //virtual QList<QWeakPointer<IRenderStep>> steps() const = 0;
    
    virtual void removeSteps(QList<QWeakPointer<IRenderStep>> steps) = 0;
    inline void removeStep(QWeakPointer<IRenderStep> step)
    {
        removeSteps( QList<QWeakPointer<IRenderStep>>({ step }) );
    }

    virtual void render(RenderHandle data) const = 0;
    
    
    virtual void clear() = 0;
    
public slots:
    virtual void rebuild() = 0;
    
signals: 
    virtual void changed(QRect area) = 0;
    
    // Emitted after `rebuild` has been called. Subscribers are allowed to 
    // contribute to the structure of the render stack using the provided build 
    // handle.
    // Subscribers must handle this event synchronously on the main thread. 
    // (Otherwise the given pointer will be null or throw an error)
    virtual void needsRebuild(QWeakPointer<RenderStackBuilder>) = 0;
};

ADDLE_DECL_MAKEABLE(IRenderStack);

} // namespace Addle

#endif // IRENDERSTACK_HPP
