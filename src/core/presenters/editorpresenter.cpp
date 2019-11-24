#include "editorpresenter.hpp"

#include "common/interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "common/servicelocator.hpp"

EditorPresenter::EditorPresenter()
{
    setTools({
        _navigateTool = ServiceLocator::make<INavigateToolPresenter>(this)
        });
}

IEditorView* EditorPresenter::getEditorView()
{
    if (!_view)
    {
        _view = ServiceLocator::make<IEditorView>(this);
    }
    return _view;
}

void EditorPresenter::onActionLoadDocument(QString filename)
{
    if (_document)
    {
        IEditorPresenter* newPresenter = ServiceLocator::make<IEditorPresenter>();
        newPresenter->loadDocument(QFileInfo(filename));
        newPresenter->getDocumentView()->start();
    }
    else
    {
        loadDocument(QFileInfo(filename));
    }
}