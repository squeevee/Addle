#ifndef IRASTEROPERATION_HPP
#define IRASTEROPERATION_HPP

#include "iundoableoperation.hpp"

class IRasterOperation : public IUndoableOperation
{
public:
    virtual ~IRasterOperation() = default;

    virtual void render(QPainter& painter, QRect region) = 0;
    virtual QRect getBoundingRect() = 0;
};

#endif // IRASTEROPERATION_HPP