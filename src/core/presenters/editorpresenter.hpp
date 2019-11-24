#ifndef EDITORPRESENTER_HPP
#define EDITORPRESENTER_HPP

#include "basedocumentpresenter.hpp"
#include "common/interfaces/views/ieditorview.hpp"
#include "common/interfaces/presenters/ieditorpresenter.hpp"

#include "common/interfaces/presenters/tools/inavigatetoolpresenter.hpp"

class EditorPresenter : public BaseDocumentPresenter, public virtual IEditorPresenter
{
    Q_OBJECT
public:
    EditorPresenter();
    virtual ~EditorPresenter() = default;

    IDocumentView* getDocumentView() { return getEditorView(); }
    IEditorView* getEditorView();

public slots: 
    void onActionLoadDocument(QString filename);

protected:
    QList<IToolPresenter*> initializeTools();

private:
    INavigateToolPresenter* _navigateTool = nullptr;

    IEditorView* _view = nullptr;
};

#endif // EDITORPRESENTER_HPP