#ifndef IOUTPUTDRIVER_H
#define IOUTPUTDRIVER_H

#include "iformatdriver.h"

class IOutputDriver : public virtual IFormatDriver
{
    virtual ~IOutputDriver() = default;

    // True if there is potentially non-trivial data in Addle that will be
    // destroyed 
    // False otherwise.
    virtual bool isOutputLossy() = 0;
};

#endif //IOUTPUTDRIVER_H