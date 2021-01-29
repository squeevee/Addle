/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IFORMATSERVICE_HPP
#define IFORMATSERVICE_HPP

#include <QObject>
#include <QIODevice>
#include <QString>
#include <QFileInfo>
#include <QSharedPointer>

#include "utilities/format/importexportinfo.hpp"
#include "utilities/format/genericformat.hpp"

#include "interfaces/traits.hpp"

namespace Addle {

/**
 * @class IFormatService
 * 
 * @ingroup interfaces
 */
class IFormatService
{
public:
    //virtual IDocument* loadFile(QString filename) = 0;

    template<class FormatModel>
    inline QSharedPointer<FormatModel> importModel(QIODevice& device, const ImportExportInfo& info)
    {
        return QSharedPointer<FormatModel>(
            boost::get<FormatModel*>( importModel_p(device, info, GenericFormatModelTypeInfo::fromType<FormatModel>() ).variant() )
        );
    }
    
    inline GenericSharedFormatModel importModel(QIODevice& device, const ImportExportInfo& info)
    {
        return GenericSharedFormatModel(importModel_p(device, info));
    }

    template<class FormatModel>
    inline void exportModel(QIODevice& device, const FormatModel&, const ImportExportInfo& info)
    {
    }

protected:
    virtual GenericFormatModel importModel_p(QIODevice& device,
        const ImportExportInfo& info,
        GenericFormatModelTypeInfo type = GenericFormatModelTypeInfo()) = 0;
};

ADDLE_DECL_SERVICE(IFormatService)

} // namespace Addle
#endif // IFORMATSERVICE_HPP
