/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ITOPLEVELVIEW_HPP
#define ITOPLEVELVIEW_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class ITopLevelView : public virtual IAmQObject
{
public:
    virtual ~ITopLevelView() = default;

public slots:
    virtual void show() = 0;
    virtual void close() = 0;

signals:
    virtual void closed() = 0;

};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ITopLevelView, "org.addle.ITopLevelView")

#endif // ITOPLEVELVIEW_HPP