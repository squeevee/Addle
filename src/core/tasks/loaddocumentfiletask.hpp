#ifndef LOADDOCUMENTFILETASK_HPP
#define LOADDOCUMENTFILETASK_HPP

#include <QString>

#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "utilities/initializehelper.hpp"

class LoadDocumentFileTask : public ILoadDocumentFileTask
{
public:
    LoadDocumentFileTask();
    virtual ~LoadDocumentFileTask() = default;

    void initialize(QFileInfo fileInfo);

    QSharedPointer<IDocument> getDocument() { return _document; }

    void run();

    virtual QSharedPointer<ITaskController> getController() { return _controller; }

private:
    QSharedPointer<ITaskController> _controller;
    
    QSharedPointer<IDocument> _document;
    QFileInfo _fileInfo;

    InitializeHelper<LoadDocumentFileTask> _initHelper;
};

#endif // LOADDOCUMENTFILETASK_HPP