#include "servicelocator.hpp"

#include "exceptions/fileexception.hpp"
// #include "exceptions/formatexception.hpp"
#include "exceptions/notimplementedexception.hpp"

// #include "interfaces/services/iformatservice.hpp"

#include "interfaces/presenters/imessagecontext.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "utilities/presenter/messagebuilders.hpp"
#include "utilities/iocheck.hpp"
#include "loadhelper.hpp"

using namespace Addle;

void LoadTask::doTask()
{
//     ADDLE_ASSERT(_request);
//     ADDLE_ASSERT(_request->action() == FileRequest::Load);
//     
//     if (_request->url().isEmpty())
//         interrupt();
// 
//     if (_request->url().isLocalFile())
//     {    
//         QString filename = _request->url().toLocalFile();
//         
//         ImportExportInfo info;
//         info.setModelType(_request->modelType());
//         info.setFileInfo(QFileInfo(filename));
//         info.setProgressHandle(&progressHandle());
//         
//         QFile file(filename);
//         IOCheck().openFile(file, QIODevice::ReadOnly | QIODevice::ExistingOnly);
//         
// //         _model = ServiceLocator::get<IFormatService>().importModel(file, info);
//     }
//     else
//     {
//         ADDLE_THROW(NotImplementedException());
//     }
}

void LoadTask::onError()
{
    try
    {
        auto messageContext = _request->messageContext();
        auto taskError      = error();
        QUrl url            = _request->url();
        QDir relativeDir    = QDir(_request->directory());

        ADDLE_ASSERT(taskError);
        
        auto issuePresenter = ServiceLocator::makeShared<IFileIssuePresenter>(
            FileIssuePresenterBuilder()
                .setException(taskError)
                .setUrl(url)
                .setRelativeDir(relativeDir)
        );
        
        if (!messageContext || issuePresenter->issue() == IFileIssuePresenter::UnknownIssue)
        {
            ADDLE_THROW(*taskError);
        }
        
        messageContext->postMessage(issuePresenter);
    }
    ADDLE_SLOT_CATCH
}
