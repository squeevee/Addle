#ifndef IINPUTDRIVER_H
#define IINPUTDRIVER_H

#include "iformatdriver.h"

class IInputDriver : public virtual IFormatDriver
{
    virtual ~IInputDriver() = default;

    // True if there is potentially non-trivial information present in the file
    // that will not be represented in Addle when the driver imports.
    // False otherwise.
    virtual bool isInputLossy() = 0;
};

#endif //IINPUTDRIVER_H