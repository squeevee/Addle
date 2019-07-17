#ifndef IFILESERVICE_H
#define IFILESERVICE_H

#include <QObject>

#include "iservice.h"
#include "interfaces/models/idocument.h"

class IFileService : public virtual IService
{
public:
    virtual IDocument* loadImage(QString filename) = 0;
};

#endif //IFILESERVICE_H