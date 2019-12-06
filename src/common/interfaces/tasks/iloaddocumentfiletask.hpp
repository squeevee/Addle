#ifndef ILOADDOCUMENTFILETASK_HPP
#define ILOADDOCUMENTFILETASK_HPP

#include <QString>
#include <QFileInfo>
#include <QSharedPointer>

#include "itask.hpp"

#include "interfaces/models/idocument.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/servicelocator/imakeable.hpp"

class ILoadDocumentFileTask : public virtual ITask, public virtual IMakeable
{
public:
    virtual ~ILoadDocumentFileTask() = default;

    virtual void initialize(QFileInfo filename) = 0;

    virtual QSharedPointer<IDocument> getDocument() = 0;
};

DECL_EXPECTS_INITIALIZE(ILoadDocumentFileTask)

#endif // ILOADDOCUMENTFILETASK_HPP