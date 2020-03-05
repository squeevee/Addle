#include "QtDebug"

#include "basedocumentpresenter.hpp"

#include "servicelocator.hpp"

#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iformatservice.hpp"

#include "interfaces/tasks/itaskcontroller.hpp"
#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"

#include "utilities/qt_extensions/qobject.hpp"

#include "exceptions/fileexceptions.hpp"

#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include <QFile>
#include <QImage>

BaseDocumentPresenter::~BaseDocumentPresenter()
{
    for (IToolPresenter* tool : _toolPresenters)
    {
        delete tool;
    }

    if (_viewPortPresenter)
        delete _viewPortPresenter;
}


ICanvasView* BaseDocumentPresenter::getCanvasView()
{
    if (!_canvasView)
    {
        _canvasView = ServiceLocator::make<ICanvasView>(this);
    }
    return _canvasView;
}

IViewPortPresenter* BaseDocumentPresenter::getViewPortPresenter()
{
    if (!_viewPortPresenter)
    {
        _viewPortPresenter = ServiceLocator::make<IViewPortPresenter>(this);
    }
    return _viewPortPresenter;
}


void BaseDocumentPresenter::setDocument(QSharedPointer<IDocument> document)
{
    _document = document;

    _layerPresenters.clear(); //leak
    for (QSharedPointer<ILayer> layer : _document->getLayers())
    {
        _layerPresenters.append(ServiceLocator::make<ILayerPresenter>(this, layer));
    }

    _isEmptyCache.recalculate();

    emit documentChanged(_document);
    getViewPortPresenter()->resetView();
}

void BaseDocumentPresenter::loadDocument(QFileInfo file)
{
    auto task = ServiceLocator::make<ILoadDocumentFileTask>(file);
    auto controller = task->getController().data();

    connect_interface(controller, SIGNAL(done(ITask*)), this, SLOT(onLoadDocumentTaskDone(ITask*)), Qt::ConnectionType::BlockingQueuedConnection);

    ServiceLocator::get<ITaskService>().queueTask(task);
}

void BaseDocumentPresenter::loadDocument(QUrl url)
{
    if (url.isLocalFile())
    {
        loadDocument(QFileInfo(url.toLocalFile()));
    }
    else
    {
        qWarning() << qUtf8Printable(tr("Loading a document from a remote URL is not yet supported."));
    }
}

void BaseDocumentPresenter::onLoadDocumentTaskDone(ITask* task)
{
    auto loadDocumentFileTask = dynamic_cast<ILoadDocumentFileTask*>(task);
    auto taskController = task->getController();

    if (!taskController->getError())
    {
        setDocument(loadDocumentFileTask->getDocument());
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

void BaseDocumentPresenter::setCurrentTool(ToolId tool)
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

bool BaseDocumentPresenter::isEmpty_p()
{
    return !_document;
}