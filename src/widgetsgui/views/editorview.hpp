#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include "common/interfaces/views/ieditorview.hpp"
#include "basedocumentview.hpp"

#include "common/utilities/initializehelper.hpp"

class EditorView : public BaseDocumentView, public virtual IEditorView
{
    Q_OBJECT
public:
    EditorView() : _initHelper(this)
    {
    }
    virtual ~EditorView() = default;

    void initialize(IEditorPresenter* presenter);

    IEditorPresenter* getEditorPresenter() { _initHelper.assertInitialized(); return _presenter; }

private:
    IEditorPresenter* _presenter;

    InitializeHelper<EditorView> _initHelper;
};

#endif // EDITORVIEW_HPP