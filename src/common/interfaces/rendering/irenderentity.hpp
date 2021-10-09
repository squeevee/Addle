#pragma once

#include <QUuid>
#include <QRegion>
#include <QPainterPath>

namespace Addle {
 
class IRenderEntity
{
public:
    virtual ~IRenderEntity() = default;
    
    virtual QUuid id() const = 0;
    virtual QRegion region() const = 0;
};

}
