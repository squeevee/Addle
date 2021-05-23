/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IRENDERSTEP_HPP
#define IRENDERSTEP_HPP

#include <QPainterPath>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "utilities/render/renderhandle.hpp"

namespace Addle {

class IRenderable : public virtual IAmQObject
{
public:
    virtual ~IRenderable() = default;

    virtual void render(RenderHandle& handle) const = 0;

    virtual QRect areaHint() const = 0;

signals: 
    virtual void changed(QRect area) = 0;
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IRenderable, "org.addle.IRenderable")

#endif // IRENDERSTEP_HPP
