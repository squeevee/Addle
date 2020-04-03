#include "QtDebug"
#include <QApplication>

#include "maineditorpresenter.hpp"

#include "servicelocator.hpp"

#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iformatservice.hpp"

#include "interfaces/tasks/itaskcontroller.hpp"
#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"

#include "exceptions/fileexceptions.hpp"

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/toolpresenters/inavigatetoolpresenter.hpp"

#include "interfaces/views/imaineditorview.hpp"

#include <QFile>
#include <QImage>

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
    _mode = mode;

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

IMainEditorView* MainEditorPresenter::getView()
{
    if (!_view)
    {
        _view = ServiceLocator::make<IMainEditorView>(this);
    }
    return _view;
}

ICanvasPresenter* MainEditorPresenter::getCanvasPresenter()
{
    if (!_canvasPresenter)
    {
        _canvasPresenter = ServiceLocator::make<ICanvasPresenter>(this);
    }
    return _canvasPresenter;
}

IViewPortPresenter* MainEditorPresenter::getViewPortPresenter()
{
    if (!_viewPortPresenter)
    {
        _viewPortPresenter = ServiceLocator::make<IViewPortPresenter>(this);
    }
    return _viewPortPresenter;
}

void MainEditorPresenter::setDocumentPresenter(IDocumentPresenter* documentPresenter)
{
    _documentPresenter = documentPresenter;

    _selectedLayer = _documentPresenter->getLayers().first();
    _isEmptyCache.recalculate();
    emit documentPresenterChanged(_documentPresenter);
    emit selectedLayerChanged(_selectedLayer);
}

void MainEditorPresenter::setMode(Mode mode)
{
    _mode = mode;
    //emit
}

void MainEditorPresenter::newDocument()
{
    if (_mode == Editor && !isEmpty())
    {
        IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
        newPresenter->newDocument();
        newPresenter->getView()->start();
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
    if (_mode == Editor && !isEmpty())
    {
        IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
        newPresenter->loadDocument(url);
        newPresenter->getView()->start();
    }
    else
    {
        //leak
        if (url.isLocalFile())
        {
            auto task = ServiceLocator::make<ILoadDocumentFileTask>(QFileInfo(url.toLocalFile()));
            auto controller = task->getController().data();

            connect_interface(
                controller,
                SIGNAL(done(ITask*)),
                this,
                SLOT(onTaskDone_LoadDocument(ITask*)),
                Qt::ConnectionType::BlockingQueuedConnection
            );

            ServiceLocator::get<ITaskService>().queueTask(task);
        }
        else
        {
            qWarning() << qUtf8Printable(tr("Loading a document from a remote URL is not yet supported."));
        }
    }
}

void MainEditorPresenter::onTaskDone_LoadDocument(ITask* task)
{
    auto loadDocumentFileTask = dynamic_cast<ILoadDocumentFileTask*>(task);
    auto taskController = task->getController();

    if (!taskController->getError())
    {
        IDocumentPresenter* presenter = ServiceLocator::make<IDocumentPresenter>(
            loadDocumentFileTask->getDocument()
        );
        setDocumentPresenter(presenter);
    }
    else
    {
        auto error = taskController->getError();

        QSharedPointer<IErrorPresenter> errorPresenter(ServiceLocator::make<IErrorPresenter>());
        errorPresenter->setSeverity(IErrorPresenter::warning);

        if (error->type_info() == typeid(FileDoesNotExistException))
        {
            auto& e = static_cast<FileDoesNotExistException&>(*error);
            QFileInfo fi = e.getFileInfo();
            errorPresenter->setMessage(
                tr("Addle could not open the requested file \"%1\" because it did not exist.")
                .arg(fi.filePath())
            );
        }
        else
        {
            errorPresenter->setMessage(
                tr("An unknown error occurred attempting to open the file.")
            );
        }

        raiseError(errorPresenter);
    }
}

void MainEditorPresenter::selectTool(ToolId tool)
{
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
    emit _currentToolPresenter->selectionChanged(true);
    if (previousTool)
        emit previousTool->selectionChanged(false);
}

void MainEditorPresenter::selectLayer(QWeakPointer<ILayerPresenter> layer)
{

}

void MainEditorPresenter::selectLayerAt(int index)
{
    
}