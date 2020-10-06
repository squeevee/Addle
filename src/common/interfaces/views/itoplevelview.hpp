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

    inline void open() { tlv_open(); }
    inline void close() { tlv_close(); }
    
public slots:
    virtual void tlv_open() = 0;
    virtual void tlv_close() = 0;

signals:
    virtual void tlv_opened() = 0;
    virtual void tlv_closed() = 0;
};

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ITopLevelView, "org.addle.ITopLevelView")

#endif // ITOPLEVELVIEW_HPP
