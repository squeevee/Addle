#include "loaddocumentfiletask.hpp"

#include "servicelocator.hpp"
#include "interfaces/services/iformatservice.hpp"

LoadDocumentFileTask::LoadDocumentFileTask()
    : _initHelper(this)
{
    _controller = QSharedPointer<ITaskController>(ServiceLocator::make<ITaskController>(this));
}

void LoadDocumentFileTask::initialize(QFileInfo fileInfo)
{
    _initHelper.initializeBegin();
    _fileInfo = fileInfo;
    _initHelper.initializeEnd();
}

void LoadDocumentFileTask::run()
{
    _initHelper.check();
    IFormatService& formatService = ServiceLocator::get<IFormatService>();
    
    QFile file(_fileInfo.filePath());
    ImportExportInfo info;
    info.setFileInfo(_fileInfo);

    _document = QSharedPointer<IDocument>(formatService.importModel<IDocument>(file, info));
}