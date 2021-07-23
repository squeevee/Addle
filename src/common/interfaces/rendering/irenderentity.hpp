#pragma once

#include <QUuid>

namespace Addle {
 
class IRenderEntity
{
public:
    virtual ~IRenderEntity() = default;
    
    virtual QUuid id() const = 0;
};
    
}
