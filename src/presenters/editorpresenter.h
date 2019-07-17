#ifndef EDITORPRESENTER_H
#define EDITORPRESENTER_H

#include <QObject>

#include "interfaces/presenters/ieditorpresenter.h"
#include "interfaces/presenters/ieditorviewportpresenter.h"
#include "interfaces/presenters/icanvaspresenter.h"
//#include "interfaces/presenters/itooloptionspresenter.h"


class EditorPresenter : public QObject, public IEditorPresenter
{
    Q_OBJECT

public:
    EditorPresenter();
    virtual ~EditorPresenter() = default;

    IEditorViewPortPresenter* getEditorViewPortPresenter() { return _viewPortPresenter; }
    ICanvasPresenter* getCanvasPresenter() { return _canvasPresenter; }

    //virtual IToolOptionsPresenter* getToolOptionsPresenter() { _toolOptionsPresenter; }

private:

    //IToolOptionsPresenter* _toolOptionsPresenter;

    IEditorViewPortPresenter* _viewPortPresenter;
    ICanvasPresenter* _canvasPresenter;

};

#endif //EDITORPRESENTER_H