#ifndef IUNDOABLE_H
#define IUNDOABLE_H

#include "interfaces/undo/iundocontext.h"

class IUndoable
{
public:
    virtual ~IUndoable() {}

    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

#endif //IUNDOABLE_H