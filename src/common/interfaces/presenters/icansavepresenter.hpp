#ifndef ICANSAVEPRESENTER
#define ICANSAVEPRESENTER

#include <QSharedPointer>
#include "interfaces/iamqobject.hpp"

namespace Addle {

class FileRequest;
class ICanSavePresenter : public virtual IAmQObject
{
public:
    virtual ~ICanSavePresenter() = default;
    
public slots:
    virtual void save(QSharedPointer<FileRequest> request) = 0;
};

}

#endif // ICANSAVEPRESENTER
