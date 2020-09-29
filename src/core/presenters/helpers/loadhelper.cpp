#include "servicelocator.hpp"

#include "exceptions/fileexception.hpp"
#include "exceptions/formatexception.hpp"

#include "interfaces/services/iformatservice.hpp"

#include "utilities/iocheck.hpp"
#include "loadhelper.hpp"

using namespace Addle;

void LoadTask::doTask()
{
    ADDLE_ASSERT(_request);
    
    if (_request->url().isEmpty())
        interrupt();

    if (_request->url().isLocalFile())
    {    
        QString filename = _request->url().toLocalFile();
        auto info = GenericImportExportInfo::make(_request->modelType());
        info.setFileInfo(QFileInfo(filename));
        info.setProgressHandle(&progressHandle());
        
        QFile file(filename);
        IOCheck().openFile(file, QFileDevice::ReadOnly);
        
        _model = ServiceLocator::get<IFormatService>().importModel(file, info);
    }
    else
    {
        ADDLE_LOGIC_ERROR_M("not implemented"); // did I already do that?
    }
}

void LoadTask::onError()
{
    try
    {
        auto messageContext = _request->messageContext();
        auto taskError = error();

        ADDLE_ASSERT(taskError);
        if (!messageContext)
            ADDLE_THROW(*taskError);
        
        if (typeid(*taskError) == typeid(FileException))
        {
            FileException& ex = static_cast<FileException&>(*taskError);
        }
        else if (typeid(*taskError) == typeid(FormatException))
        {
            FormatException& ex = static_cast<FormatException&>(*taskError);
        }
        else
        {
            ADDLE_THROW(*taskError);
        }
    }
    ADDLE_SLOT_CATCH
}
