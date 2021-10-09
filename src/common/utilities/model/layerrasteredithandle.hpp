#pragma once

#include <QEvent>
#include "utilities/qobject.hpp"
#include "globals.hpp"

namespace Addle {
    
class LayerRasterEditCommittedEvent : public QEvent
{
public:
    LayerRasterEditCommittedEvent()
    {
    }
    
};
    
}
