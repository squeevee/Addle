#ifndef LOADDOCUMENTFILETASK_HPP
#define LOADDOCUMENTFILETASK_HPP

#include <QString>

#include "interfaces/tasks/iloaddocumentfiletask.hpp"

#include "taskbase.hpp"
#include "utilities/initializehelper.hpp"

class LoadDocumentFileTask : public virtual TaskBase, public virtual ILoadDocumentFileTask
{
public:
    LoadDocumentFileTask();
    virtual ~LoadDocumentFileTask() = default;

    void initialize(QFileInfo fileInfo);

    QSharedPointer<IDocument> getDocument() { return _document; }

    void run();

private:
    QSharedPointer<IDocument> _document;
    QFileInfo _fileInfo;

    InitializeHelper<LoadDocumentFileTask> _initHelper;
};

#endif // LOADDOCUMENTFILETASK_HPP