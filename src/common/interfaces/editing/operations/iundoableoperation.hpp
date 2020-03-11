#ifndef IUNDOABLEOPERATION_HPP
#define IUNDOABLEOPERATION_HPP

class IUndoableOperation
{
public:
    virtual ~IUndoableOperation() = default;

    virtual void doOperation() = 0;
    virtual void undoOperation() = 0;
};

#endif // IUNDOABLEOPERATION_HPP