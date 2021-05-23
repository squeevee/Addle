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
#include "utilities/render/renderhandle.hpp"

namespace Addle {

class IRenderable;
class RenderAugmentHandle;

class IRenderer : public virtual IAmQObject
{
public:    
    virtual ~IRenderer() = default;

    //TODO
    //virtual QList<QWeakPointer<IRenderable>> steps() const = 0;
    
    //virtual 
    
    virtual void removeSteps(QList<QWeakPointer<IRenderable>> steps) = 0;
    inline void removeStep(QWeakPointer<IRenderable> step)
    {
        removeSteps( QList<QWeakPointer<IRenderable>>({ step }) );
    }

    virtual void render(RenderHandle data) const = 0;
    
    virtual void clear() = 0;
    
// public slots:
//     virtual void rebuild() = 0;
    
signals: 
    virtual void changed(QRect area) = 0;
    
};

ADDLE_DECL_MAKEABLE(IRenderer);

} // namespace Addle

#endif // IRENDERSTACK_HPP
