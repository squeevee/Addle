#ifndef IFORMATDRIVER_HPP
#define IFORMATDRIVER_HPP

#include <QString>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <type_traits>

#include "interfaces/traits/by_id_traits.hpp"

#include "utilities/model/importexportinfo.hpp"
//#include "interfaces/tasks/itaskstatuscontroller.hpp"
//#include "interfaces/tasks/itaskcontroller.hpp"

#include "idtypes/formatid.hpp"

#include "iformatmodel.hpp"

class IFormatDriver
{
public:
    virtual ~IFormatDriver() = default;

    //virtual QString getName() = 0;

    virtual bool supportsImport() const = 0;
    virtual bool supportsExport() const = 0;

    virtual FormatId getId() const = 0;

    virtual IFormatModel* importModel(QIODevice& device, ImportExportInfo info) = 0;
    virtual void exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info) = 0;
};

DECL_MAKEABLE_BY_ID(IFormatDriver, FormatId);

#endif // IFORMATDRIVER_HPP