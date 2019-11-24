#include "editorview.hpp"

#include "common/utilities/qt_extensions/qobject.hpp"

void EditorView::initialize(IEditorPresenter* presenter)
{
    _initHelper.initializeBegin();

    _presenter = presenter;
    BaseDocumentView::initialize(presenter);

    _initHelper.initializeEnd();
}