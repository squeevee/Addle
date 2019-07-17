#ifndef IUNDOSERVICE_H
#define IUNDOSERVICE_H

#include "interfaces/services/iservice.h"

#include "interfaces/undo/iundoable.h"
#include "interfaces/undo/iundocontext.h"

class IUndoService : public IService
{
public:
    virtual ~IUndoService() {}

    virtual void Push(IUndoContext* context, IUndoable* action) = 0;

    virtual void Undo(IUndoContext* context) = 0;
    virtual void Redo(IUndoContext* context) = 0;

    virtual void ObliterateUndoStack(IUndoContext* context) = 0;
    virtual void ObliterateRedoStack(IUndoContext* context) = 0;
};

#endif //IUNDOSERVICE_H