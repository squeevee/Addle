#include "editorpresenter.hpp"

#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"
#include "servicelocator.hpp"

EditorPresenter::EditorPresenter()
{
    _tools = {
        DefaultTools::SELECT,
        DefaultTools::BRUSH,
        DefaultTools::ERASER,
        DefaultTools::TEXT,
        DefaultTools::SHAPES,
        DefaultTools::STICKERS,
        DefaultTools::EYEDROP,
        DefaultTools::NAVIGATE,
        DefaultTools::MEASURE
    };

    _toolPresenters = {
        {
            DefaultTools::BRUSH,
            _brushTool = ServiceLocator::make<IBrushToolPresenter>(this)
        },
        {
            DefaultTools::NAVIGATE,
            _navigateTool = ServiceLocator::make<INavigateToolPresenter>(this)
        }
    };
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

void EditorPresenter::setDocument(QSharedPointer<IDocument> document)
{
    BaseDocumentPresenter::setDocument(document);

    setCurrentTool(DefaultTools::NAVIGATE);
    setSelectedLayer(0);
}