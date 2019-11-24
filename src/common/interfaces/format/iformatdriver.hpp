#ifndef IFORMATDRIVER_HPP
#define IFORMATDRIVER_HPP

#include <QString>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <type_traits>

#include "common/utilities/data/importexportinfo.hpp"
#include "common/interfaces/tasks/itaskstatuscontroller.hpp"
#include "common/interfaces/tasks/itaskcontroller.hpp"

#include "iformatmodel.hpp"

class IFormatDriver
{
public:
    virtual ~IFormatDriver() = default;

    //virtual QString getName() = 0;
    virtual QString getPrimaryFileExtension() = 0;
    virtual QList<QString> getFileExtensions() = 0;
    virtual QByteArray getSignature() = 0;

    virtual bool supportsImport() const = 0;
    virtual bool supportsExport() const = 0;

    virtual QString getMimeType() const = 0;
    virtual FormatId getFormatId() const = 0;

    virtual IFormatModel* importModel(QIODevice& device, ImportExportInfo info, ITaskStatusController* status = nullptr) = 0;
    virtual void exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info, ITaskStatusController* status = nullptr) = 0;
};

#endif // IFORMATDRIVER_HPP