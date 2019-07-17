#ifndef EDITORVIEWPORT_H
#define EDITORVIEWPORT_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "interfaces/presenters/ieditorviewportpresenter.h"
#include "interfaces/presenters/ieditorpresenter.h"
#include "interfaces/presenters/icanvaspresenter.h"

class EditorViewPort : public QGraphicsView
{
    Q_OBJECT
public:
    EditorViewPort(QWidget* parent);
    virtual ~EditorViewPort();

    void initialize(IEditorViewPortPresenter* presenter);

protected:

    void resizeEvent(QResizeEvent* event);

private:

    IEditorViewPortPresenter* _presenter;
    IEditorPresenter* _editorPresenter;
    ICanvasPresenter* _canvasPresenter;

    QGraphicsScene* _scene;

};

#endif //EDITORVIEWPORT_H