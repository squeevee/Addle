#include "QtDebug"
#include <QApplication>
#include <QtConcurrent>

#include "maineditorpresenter.hpp"

#include "servicelocator.hpp"

//#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iformatservice.hpp"

//#include "interfaces/tasks/itaskcontroller.hpp"
//#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "exceptions/fileexceptions.hpp"

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"

#include "interfaces/views/imaineditorview.hpp"

#include <QFile>
#include <QImage>

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "interfaces/models/ipalette.hpp"
#include "globals.hpp"
using namespace Addle;

using namespace IMainEditorPresenterAux;

MainEditorPresenter::~MainEditorPresenter()
{
    for (IToolPresenter* tool : _toolPresenters)
    {
        delete tool;
    }

    if (_viewPortPresenter)
        delete _viewPortPresenter;
}

void MainEditorPresenter::initialize(Mode mode)
{
    _initHelper.initializeBegin();
    
    _mode = mode;

    _viewPortPresenter = ServiceLocator::make<IViewPortPresenter>(this);
    _initHelper.setCheckpoint(Check_ViewPortPresenter);

    _canvasPresenter = ServiceLocator::make<ICanvasPresenter>(this);
    _initHelper.setCheckpoint(Check_CanvasPresenter);
    
    _palettes = { 
        ServiceLocator::makeShared<IPalettePresenter>(
            std::ref(ServiceLocator::get<IPalette>(CorePalettes::BasicPalette))
        ) // wow that's crunchy :)
    };

    _colorSelection = ServiceLocator::make<IColorSelectionPresenter>(_palettes);
    _colorSelection->setPalette(_palettes.first());
    _initHelper.setCheckpoint(Check_ColorSelection);

    _tools = {{
        Mode::Editor,
        {
            DefaultTools::SELECT,
            DefaultTools::BRUSH,
            DefaultTools::ERASER,
            DefaultTools::TEXT,
            DefaultTools::SHAPES,
            DefaultTools::STICKERS,
            DefaultTools::EYEDROP,
            DefaultTools::NAVIGATE,
            DefaultTools::MEASURE
        }
    }};

    _toolPresenters = {
        {
            DefaultTools::BRUSH,
            _brushTool = ServiceLocator::make<IBrushToolPresenter>(
                this,
                IBrushToolPresenter::Mode::Brush
            )
        },
        {
            DefaultTools::ERASER,
            _brushTool = ServiceLocator::make<IBrushToolPresenter>(
                this,
                IBrushToolPresenter::Mode::Eraser
            )
        },
        {
            DefaultTools::NAVIGATE,
            _navigateTool = ServiceLocator::make<INavigateToolPresenter>(
                this
            )
        }
    };

    _view = ServiceLocator::make<IMainEditorView>(this);
    _initHelper.setCheckpoint(Check_View);

    _loadDocumentTask = new LoadDocumentTask(this);
    connect(_loadDocumentTask, &AsyncTask::completed, this, &MainEditorPresenter::onLoadDocumentCompleted);

    _initHelper.initializeEnd();
}

void MainEditorPresenter::setDocumentPresenter(IDocumentPresenter* documentPresenter)
{
    _initHelper.check(); 

    auto oldTopSelectedLayer = topSelectedLayer();

    _documentPresenter = documentPresenter;

    if (_connection_topSelectedLayer)
        disconnect(_connection_topSelectedLayer);

    if (_documentPresenter)
        _connection_topSelectedLayer = connect_interface(
            _documentPresenter,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>)),
            this,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>))
        );
    
    _isEmptyCache.recalculate();
    emit documentPresenterChanged(_documentPresenter);

    auto newTopSelectedLayer = topSelectedLayer();
    if (newTopSelectedLayer != oldTopSelectedLayer)
        emit topSelectedLayerChanged(newTopSelectedLayer);
}

QSharedPointer<ILayerPresenter> MainEditorPresenter::topSelectedLayer() const
{
    _initHelper.check();
    if (_documentPresenter)
        return _documentPresenter->topSelectedLayer();
    else
        return nullptr;
}

void MainEditorPresenter::setMode(Mode mode)
{
    _initHelper.check(); 
    _mode = mode;
    //emit
}

void MainEditorPresenter::newDocument()
{
    _initHelper.check(); 
    if (_mode == Editor && !isEmpty())
    {
        IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
        newPresenter->newDocument();
        newPresenter->view()->start();
    }
    else
    {
        //leak
        setDocumentPresenter(
            ServiceLocator::make<IDocumentPresenter>(IDocumentPresenter::initBlankDefaults)
        );
    }
}

void MainEditorPresenter::loadDocument(QUrl url)
{
    _initHelper.check(); 
    if (_mode == Editor && !isEmpty())
    {
        IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
        newPresenter->loadDocument(url);
        newPresenter->view()->start();
    }
    else
    {            
        if (!_loadDocumentTask->isRunning())
        {
            _loadDocumentTask->setUrl(url);
            _loadDocumentTask->start();
        }
    }
}

void MainEditorPresenter::onLoadDocumentCompleted()
{
    _initHelper.check(); 
    setDocumentPresenter(_loadDocumentTask->documentPresenter());
}

void MainEditorPresenter::selectTool(ToolId tool)
{
    _initHelper.check(); 
    if (tool == _currentTool)
        return;

    if (!_toolPresenters.contains(tool))
    {
        //throw or something i dunno
        return;
    }

    _currentTool = tool;
    IToolPresenter* previousTool = _currentToolPresenter;
    _currentToolPresenter = _toolPresenters.value(tool);
    emit currentToolChanged(_currentTool);
    emit _currentToolPresenter->setSelected(true);
    if (previousTool)
        emit previousTool->setSelected(false);
}

void LoadDocumentTask::doTask()
{
    QUrl loadedUrl = url();

    if (loadedUrl.isLocalFile())
    {
        QFile file(loadedUrl.toLocalFile());

        ImportExportInfo info;
        info.setFilename(loadedUrl.toLocalFile());

        auto doc = QSharedPointer<IDocument>(ServiceLocator::get<IFormatService>().importModel<IDocument>(file, info));
        setDocumentPresenter(ServiceLocator::make<IDocumentPresenter>(doc));
    }
    else
    {
        qWarning() << qUtf8Printable(tr("Loading a document from a remote URL is not yet supported."));
    }
}