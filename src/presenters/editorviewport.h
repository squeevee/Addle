#ifndef EDITORVIEWPORT_H
#define EDITORVIEWPORT_H

#include "presenters/viewport.h"

class EditorViewport : public Viewport
{
    Q_OBJECT

public:
    EditorViewport();
    
    virtual ~EditorViewport() {}

};

#endif //EDITORVIEWPORT_H