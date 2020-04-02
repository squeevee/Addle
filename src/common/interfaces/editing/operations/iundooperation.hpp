#ifndef IUNDOOPERATION_HPP
#define IUNDOOPERATION_HPP

class IUndoOperation
{
public:
    virtual ~IUndoOperation() = default;

    virtual void do_() = 0;
    virtual void undo() = 0;
};

#endif // IUNDOOPERATION_HPP