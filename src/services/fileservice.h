#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "servicebase.h"
#include "interfaces/services/ifileservice.h"

class FileService : public ServiceBase, public virtual IFileService
{
public:
    virtual ~FileService() = default;

    IDocument* loadImage(QString filename);
};

#endif //FILESERVICE_H