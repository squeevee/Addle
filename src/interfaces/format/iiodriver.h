#ifndef IIODRIVER_H
#define IIODRIVER_H

#include "iformatdriver.h"
#include "iinputdriver.h"
#include "ioutputdriver.h"

class IIODriver : public virtual IInputDriver, public virtual IOutputDriver
{
    
};

#endif //IIODRIVER_H