#ifndef IRASTEROPERATION_HPP
#define IRASTEROPERATION_HPP

#include "iundoableoperation.hpp"

class IRasterOperation : public IUndoableOperation
{
public:
    virtual ~IRasterOperation() = default;

    virtual const IRasterSurface& getSurface() = 0;
};

#endif // IRASTEROPERATION_HPP