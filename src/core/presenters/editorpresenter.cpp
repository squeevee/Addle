#include "editorpresenter.hpp"

#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"
#include "servicelocator.hpp"

#include "utilities/model/documentbuilder.hpp"

EditorPresenter::EditorPresenter()
    : _undoStackHelper(*this)
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

    _propertyDecorationHelper.setIconPool({
        { DefaultTools::BRUSH, QIcon(":/icons/brush.png") },
        { DefaultTools::NAVIGATE, QIcon(":/icons/navigate.png") }
    });

    _propertyDecorationHelper.initializeIdProperty<ToolId>(
        "currentTool",
        {
            DefaultTools::BRUSH,
            DefaultTools::NAVIGATE
        }
    );
}

IEditorView* EditorPresenter::getEditorView()
{
    if (!_view)
    {
        _view = ServiceLocator::make<IEditorView>(this);
    }
    return _view;
}

void EditorPresenter::newDocument()
{
    if (_document)
    {
        IEditorPresenter* newPresenter = ServiceLocator::make<IEditorPresenter>();
        newPresenter->newDocument();
        newPresenter->getDocumentView()->start();
    }
    else
    {
        //TODO: not this

        LayerBuilder blankLayer;
        blankLayer.setBoundary(QRect(0,0, 800, 600));

        QImage blank(QSize(800, 600), QImage::Format_ARGB32_Premultiplied);

        blankLayer.setImage(blank);

        LayerBuilder bglayer;
        bglayer.setBoundary(QRect(0,0, 800, 600));

        QImage whiteBg(QSize(800,600), QImage::Format_ARGB32_Premultiplied);
        whiteBg.fill(Qt::white);

        bglayer.setImage(whiteBg);

        DocumentBuilder document;
        document.addLayer(blankLayer);
        document.addLayer(bglayer);
        setDocument(ServiceLocator::makeShared<IDocument>(document));
    }
}

void EditorPresenter::loadDocument(QUrl file)
{
    if (_document)
    {
        IEditorPresenter* newPresenter = ServiceLocator::make<IEditorPresenter>();
        newPresenter->loadDocument(file);
        newPresenter->getDocumentView()->start();
    }
    else
    {
        BaseDocumentPresenter::loadDocument(file);
    }
}

void EditorPresenter::setDocument(QSharedPointer<IDocument> document)
{
    if (!_document)
        setCurrentTool(DefaultTools::BRUSH);

    BaseDocumentPresenter::setDocument(document);

    setSelectedLayer(0);
}