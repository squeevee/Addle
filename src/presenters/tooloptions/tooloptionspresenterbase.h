#ifndef TOOLOPTIONSPRESENTER_H
#define TOOLOPTIONSPRESENTER_H

#include "interfaces/presenters/tooloptions/itooloptionspresenter.h"

#include "utilities/initializehelper.h"

class ToolOptionsPresenterBase : public virtual IToolOptionsPresenter
{
public:
    IEditorPresenter* getEditorPresenter() { return _editorPresenter; }
    void setEditorPresenter(IEditorPresenter* editorPresenter);

protected:
    InitializeHelper _initHelper;

private:
    IEditorPresenter* _editorPresenter;
};

#endif //TOOLOPTIONSPRESENTER_H