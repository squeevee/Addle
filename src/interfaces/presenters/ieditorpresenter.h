#ifndef IEDITORPRESENTER_H
#define IEDITORPRESENTER_H

#include <QObject>

class IEditorViewPortPresenter;
class ICanvasPresenter;
//class IToolOptionsPresenter;

class IEditorPresenter
{
public:
    
    virtual ~IEditorPresenter() {}

    virtual IEditorViewPortPresenter* getEditorViewPortPresenter() = 0;

    virtual ICanvasPresenter* getCanvasPresenter() = 0;

    //virtual IToolOptionsPresenter::ToolRole getSelectedTool() = 0;
    //virtual IToolOptionsPresenter* getSelectedToolOptionsPresenter() = 0;
    //virtual void selectTool(IToolOptionsPresenter::ToolRole tool) = 0;

    // virtual QColor getColor1() = 0;
    // virtual void setColor1(QColor color) = 0;

    // virtual QColor getColor2() = 0;
    // virtual void setColor2(QColor color) = 0;
    
//signals:

    //virtual void selectedToolChanged(IToolOptionsPresenter::ToolRole tool) = 0;
};


#endif //IEDITORPRESENTER_H