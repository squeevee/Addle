#include "servicelocator.hpp"

#include "interfaces/services/iformatservice.hpp"

#include "utilities/iocheck.hpp"
#include "loadhelper.hpp"

using namespace Addle;

void LoadTask::doTask()
{
    if (!_request || _request->url().isEmpty())
        interrupt();

    if (_request->url().isLocalFile())
    {    
        QString filename = _request->url().toLocalFile();
        auto info = GenericImportExportInfo::make(_request->modelType());
        info.setFileInfo(QFileInfo(filename));
        
        QFile file(filename);
        IOCheck().openFile(file, QFileDevice::ReadOnly);
        
        _model = ServiceLocator::get<IFormatService>().importModel(file, info);
    }
    else
    {
        ADDLE_LOGIC_ERROR_M("not implemented"); // did I already do that?
    }
}
