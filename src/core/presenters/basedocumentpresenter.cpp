#include "QtDebug"

#include "basedocumentpresenter.hpp"

#include "common/servicelocator.hpp"

#include "common/interfaces/services/itaskservice.hpp"
#include "common/interfaces/services/iformatservice.hpp"
#include "common/interfaces/presenters/icanvaspresenter.hpp"

#include "common/interfaces/tasks/itaskcontroller.hpp"
#include "common/interfaces/tasks/iloaddocumentfiletask.hpp"

#include "common/utilities/qt_extensions/qobject.hpp"

#include "common/exceptions/fileexceptions.hpp"

#include "common/interfaces/presenters/tools/itoolpresenter.hpp"

#include <QFile>
#include <QImage>

BaseDocumentPresenter::~BaseDocumentPresenter()
{
    for (IToolPresenter* tool : _tools)
    {
        delete tool;
    }

    if (_viewPortPresenter)
        delete _viewPortPresenter;

    if (_canvasPresenter)
        delete _canvasPresenter;
}

IViewPortPresenter* BaseDocumentPresenter::getViewPortPresenter()
{
    if (!_viewPortPresenter)
    {
        _viewPortPresenter = ServiceLocator::make<IViewPortPresenter>(this, getCanvasPresenter());
    }
    return _viewPortPresenter;
}

ICanvasPresenter* BaseDocumentPresenter::getCanvasPresenter()
{
    if (!_canvasPresenter)
    {
        _canvasPresenter = ServiceLocator::make<ICanvasPresenter>(this);
    }
    return _canvasPresenter;
}

void BaseDocumentPresenter::setDocument(QSharedPointer<IDocument> document)
{
    _document = document;
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

void BaseDocumentPresenter::setTools(const QList<IToolPresenter*>& tools)
{
    _tools = tools;
    _toolsSet = _tools.toSet();
    _currentTool = _tools.first();
}

void BaseDocumentPresenter::setCurrentTool(IToolPresenter* tool)
{
    if (!_toolsSet.contains(tool))
    {
        //throw or something i dunno
        return;
    }

    _currentTool = tool;
    emit currentToolChanged(_currentTool);
}