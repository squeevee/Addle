#ifndef ITOOLOPTIONSPRESENTER_H
#define ITOOLOPTIONSPRESENTER_H

#include <QObject>
#include "../ieditorpresenter.h"

class IToolOptionsPresenter
{
public:
    enum ToolRole {
        select,
        brush,
        eraser,
        fill,
        shapes,
        text,
        stickers,
        eyedrop,
        navigate,
        measure
    };

    virtual ToolRole getRole() = 0;

    virtual IEditorPresenter* getEditorPresenter() = 0;
    virtual void setEditorPresenter(IEditorPresenter* editorPresenter) = 0;
};

Q_DECLARE_METATYPE(IToolOptionsPresenter::ToolRole)

#endif //ITOOLOPTIONSPRESENTER_H