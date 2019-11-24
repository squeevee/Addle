#include "loaddocumentfiletask.hpp"

#include "common/servicelocator.hpp"
#include "common/interfaces/services/iformatservice.hpp"

LoadDocumentFileTask::LoadDocumentFileTask()
    : _initHelper(this)
{
}

void LoadDocumentFileTask::initialize(QFileInfo fileInfo)
{
    _initHelper.initializeBegin();
    _fileInfo = fileInfo;
    _initHelper.initializeEnd();
}

void LoadDocumentFileTask::run()
{
    _initHelper.assertInitialized();
    IFormatService& formatService = ServiceLocator::get<IFormatService>();
    
    QFile file(_fileInfo.filePath());
    ImportExportInfo info;
    info.setFileInfo(_fileInfo);

    _document = QSharedPointer<IDocument>(formatService.importModel<IDocument>(file, info));
}