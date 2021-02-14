/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License.
 * See "LICENSE" for full details.
 */

#include "QtDebug"
#include <QApplication>
#include <QtConcurrent>

#include "maineditorpresenter.hpp"

#include "utils.hpp"

#include "servicelocator.hpp"

//#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iformatservice.hpp"

//#include "interfaces/tasks/itaskcontroller.hpp"
//#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "interfaces/presenters/ilayerpresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"

#include "interfaces/models/idocument.hpp"
#include "utilities/qobject.hpp"

#include "exceptions/fileexception.hpp"

#include "interfaces/services/iapplicationservice.hpp"

#include "interfaces/presenters/iviewportpresenter.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/icolorselectionpresenter.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/tools/inavigatetoolpresenter.hpp"

#include "interfaces/presenters/messages/inotificationpresenter.hpp"

#include "interfaces/views/imaineditorview.hpp"

#include <QFile>
#include <QImage>
#include <QStandardPaths>

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "interfaces/models/ipalette.hpp"
#include "globals.hpp"

//#include "utilities/presenter/genericerrorpresenter.hpp"
#include "exceptions/formatexception.hpp"

#include "utilities/iocheck.hpp"

using namespace Addle;

MainEditorPresenter::MainEditorPresenter(
        Mode mode,
        std::unique_ptr<IColorSelectionPresenter> colorSelection,
        std::unique_ptr<IViewPortPresenter> viewPortPresenter,
        std::unique_ptr<IMessageContext> messageContext,
        std::unique_ptr<IRepository<IPalettePresenter>> palettes,
        std::unique_ptr<IRepository<IToolPresenter>> tools
    )
    : _mode(mode),
    _colorSelection(std::move(colorSelection)),
    _viewPortPresenter(std::move(viewPortPresenter)),
    _messageContext(std::move(messageContext)),
    _palettes(std::move(palettes)),
    _tools(std::move(tools))
{
    _undoStackHelper.undoStateChanged.bind(&MainEditorPresenter::undoStateChanged, this);

    _isEmptyCache.calculateBy(&MainEditorPresenter::isEmpty_p, this);
    _isEmptyCache.onChange.bind(&MainEditorPresenter::isEmptyChanged, this);
    
    _loadDocumentHelper.onLoaded.bind(&MainEditorPresenter::onLoadDocumentCompleted, this);
    
    _palettes->addStaticIds<CorePalettes::all_palettes>();
    _colorSelection->setPalette(_palettes->getShared(CorePalettes::BasicPalette));
    
    _tools->bindFactoryParameters(aux_IToolPresenter::editor_ = *this);
    _tools->add({ CoreTools::Brush, CoreTools::Eraser, CoreTools::Navigate });
        
//     _loadDocumentTask = new LoadDocumentTask(this);
//     connect(_loadDocumentTask, &AsyncTask::completed, this, &MainEditorPresenter::onLoadDocumentCompleted);
//     connect(_loadDocumentTask, &AsyncTask::failed, this, &MainEditorPresenter::onLoadDocumentFailed);
//     
//     _saveDocumentTask = new SaveDocumentTask(this);
//     connect(_saveDocumentTask, &AsyncTask::completed, this, &MainEditorPresenter::onSaveDocumentCompleted);
//     connect(_saveDocumentTask, &AsyncTask::failed, this, &MainEditorPresenter::onSaveDocumentFailed);
}

void MainEditorPresenter::setDocument(QSharedPointer<IDocumentPresenter> document)
{
    auto oldTopSelectedLayer = topSelectedLayer();

    _document = document;

    if (_connection_topSelectedLayer)
        disconnect(_connection_topSelectedLayer);

    if (_document)
        _connection_topSelectedLayer = connect_interface(
            _document,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>)),
            this,
            SIGNAL(topSelectedLayerChanged(QSharedPointer<ILayerPresenter>))
        );
    
    _isEmptyCache.recalculate();
    emit documentPresenterChanged(_document);

    auto newTopSelectedLayer = topSelectedLayer();
    if (newTopSelectedLayer != oldTopSelectedLayer)
        emit topSelectedLayerChanged(newTopSelectedLayer);
}

QSharedPointer<ILayerPresenter> MainEditorPresenter::topSelectedLayer() const
{
    if (_document)
        return _document->topSelectedLayer();
    else
        return nullptr;
}

void MainEditorPresenter::setMode(Mode mode)
{
    _mode = mode;
    //emit
}

void MainEditorPresenter::newDocument()
{
    try
    {
        if (_mode == Editor && !isEmpty())
        {
//             IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
//             newPresenter->newDocument();
//             newPresenter->view().open();
        }
        else
        {
            //leak
//             setDocument(
//                 ServiceLocator::makeShared<IDocumentPresenter>(IDocumentPresenter::initBlankDefaults)
//             );
        }
    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::loadDocument(QSharedPointer<FileRequest> request)
{
    try
    {
        if (!_loadDocumentHelper.canLoad()) return;
        
        request->setModelType(GenericFormatModelTypeInfo::fromType<IDocument>());
        request->setFavoriteFormat(CoreFormats::PNG);
//         request->setDirectory(
//                 cpplinq::from(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation))
//             >>  cpplinq::first_or_default()
//         );
        request->setMessageContext(_messageContext.get());
//         request->setUrl(
//             QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).constFirst())
//         );
        _loadDocumentHelper.load(request);
        
/*        if (_mode == Editor && !isEmpty())
        {
            IMainEditorPresenter* newPresenter = ServiceLocator::make<IMainEditorPresenter>(_mode);
            newPresenter->loadDocument(url);
        }*/

    }
    ADDLE_SLOT_CATCH
}

void MainEditorPresenter::onLoadDocumentCompleted()
{
    setDocument(_loadDocumentHelper.presenter());
    //view().show();
}

void MainEditorPresenter::onLoadDocumentFailed()
{
//     try
//     {
//         ASSERT_INIT();
// 
//         const auto& mainEditorPresenters = ServiceLocator::get<IApplicationService>().mainEditorPresenters();
//         ADDLE_ASSERT(mainEditorPresenters.contains(this));
// 
//         if (isEmpty() && mainEditorPresenters.size() > 1)
//             deleteLater();
// 
//         QString filename = QFileInfo(_loadDocumentTask->url().toLocalFile()).fileName();
//         QString errorMessage;
// 
//         if (typeid(*_loadDocumentTask->error()) == typeid(FormatException))
//         {
//             auto ex = static_cast<FormatException&>(*_loadDocumentTask->error());
// 
//             switch(ex.why())
//             {
//             case FormatException::FormatNotRecognized:
//             case FormatException::EmptyResource:
//             case FormatException::WrongModelType:
//                     : Displayed after attempting to open a file whose format
//                     : is not supported, not recognized, damaged, or otherwise
//                     : incorrect.
//                     : %1 = the name of the file
//                 errorMessage = qtTrId("ui.open-document.invalid-format-error")
//                         .arg(filename);
//                 break;
//             }
//         }
//         else if (typeid(*_loadDocumentTask->error()) == typeid(FileException))
//         {
// 
//             auto ex = static_cast<FileException&>(*_loadDocumentTask->error());
// 
//             QStringList messages;
// 
//             if (ex.why() & FileException::InUse)
//                 : Displayed after attempting to open a file that is in use by 
//                 : another process/application
//                 : %1 = the name of the file
//                 messages.append(qtTrId("ui.open-document.file-in-use-error")
//                     .arg(filename));
//             if (ex.why() & FileException::NoReadPermission)
//                 : Displayed after attempting to open a file that the user's
//                 : account does not have permission to access, or is otherwise
//                 : restricted.
//                 : %1 = the name of the file
//                 messages.append(qtTrId("ui.open-document.permission-denied-error")
//                     .arg(filename));
//             if (ex.why() & FileException::DoesNotExist
//                 || ex.why() & FileException::IncompleteDirPath
//                 || ex.why() & FileException::NotAFile)
//                 : Displayed after attempting to open a file path that did not
//                 : lead to a file.
//                 : %1 = the file path
//                 messages.append(qtTrId("ui.open-document.file-not-found-error")
//                     .arg(_loadDocumentTask->url().toLocalFile()));
//             if (ex.why() & FileException::UnknownProblem || messages.isEmpty())
//                 goto unknownError;
//             
//             errorMessage = messages.join('\n');
//         }
//         else
//         {
//             ADDLE_THROW(*_loadDocumentTask->error());
//         }
// 
//         postMessage(ServiceLocator::makeShared<INotificationPresenter>(
//             errorMessage,               //text
//             IMessagePresenter::Issue,   //tone
//             true,                       //isUrgent
//             this,                       //context
//             _loadDocumentTask->error()  //exception
//         ));
//         return;
// 
//     unknownError:
//         : Displayed when the document failed to open, but the reason why could
//         : not be determined.
//         errorMessage = qtTrId("ui.open-document.unknown-error");
//         postMessage(ServiceLocator::makeShared<INotificationPresenter>(
//             errorMessage,               //text
//             IMessagePresenter::Issue,   //tone
//             true,                       //isUrgent
//             this,                       //context
//             _loadDocumentTask->error()  //exception
//         ));
//         return;
//     }
//     ADDLE_SLOT_CATCH
}
// 
// void MainEditorPresenter::saveDocument(QSharedPointer<FileRequest> request)
// {
//     try 
//     {
//         ASSERT_INIT();
// 
//         if (_pendingDocumentFileRequest)
//             return; // queue?
// 
//         _pendingDocumentFileRequest = request;
// 
//         _pendingDocumentFileRequest->setAvailableFormats(
//             { CoreFormats::PNG, CoreFormats::JPEG }
//         );
//         _pendingDocumentFileRequest->setFavoriteFormat(CoreFormats::PNG);
//         _pendingDocumentFileRequest->setUntitled(
//             QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
//         );
// 
//         ADDLE_ASSERT(_document);
//         ADDLE_ASSERT(!_saveDocumentTask->isRunning());
// 
//         _saveDocumentTask->setRequest(_pendingDocumentFileRequest);
//         _saveDocumentTask->setDocument(_document);
// 
//         _connection_onSaveDocumentUrlUpdated = connect(
//             _pendingDocumentFileRequest.data(), &FileRequest::urlChanged,
//             _saveDocumentTask, &SaveDocumentTask::start);
// 
//         _connection_onSaveDocumentComplete = connect(
//             _pendingDocumentFileRequest.data(), &FileRequest::completed,
//             this, &MainEditorPresenter::onSaveDocumentCompleted);
// 
//         if (_pendingDocumentFileRequest->url().isEmpty())
//         {
//             emit _pendingDocumentFileRequest->urlNeeded();
//         }
//         else
//         {
//             _saveDocumentTask->start();
//         }
//     }
//     ADDLE_SLOT_CATCH
// }
// 
// void MainEditorPresenter::onSaveDocumentCompleted()
// {
//     disconnect(_connection_onSaveDocumentUrlUpdated);
//     disconnect(_connection_onSaveDocumentComplete);
// 
//     _pendingDocumentFileRequest = nullptr;
//     _saveDocumentTask->setRequest(nullptr);
// }
// 
// void MainEditorPresenter::onSaveDocumentFailed()
// {
// 
// }

void MainEditorPresenter::setCurrentTool(ToolId tool)
{
//     ASSERT_INIT(); 
//     if (tool == _currentTool)
//         return;
// 
//     const auto& toolPresenters = _tools.value(_mode);
//     ADDLE_ASSERT(!tool || toolPresenters.contains(tool));
// 
//     _currentTool = tool;
//     auto previousTool = _currentToolPresenter;
//     _currentToolPresenter = toolPresenters.value(tool);
//     emit currentToolChanged(_currentTool);
//     emit _currentToolPresenter->setSelected(true);
//     if (previousTool)
//         emit previousTool->setSelected(false);
}
