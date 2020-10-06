/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FILESERVICE_HPP
#define FILESERVICE_HPP

#include <QHash>
#include <QList>
#include <QSet>

#include "compat.hpp"
#include "utils.hpp"
#include "interfaces/format/iformatdriver.hpp"
#include "interfaces/services/iformatservice.hpp"

#include "idtypes/formatid.hpp"
#include "utilities/mplarray.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT FormatService : public virtual IFormatService
{
public:
    virtual ~FormatService() = default;

protected:
    GenericFormatModel importModel_p(QIODevice& device, 
        const ImportExportInfo& info,
        GenericFormatModelTypeInfo type = GenericFormatModelTypeInfo());

private:
    struct visitor_setupFormatType
    {
        typedef void result_type;
        
        visitor_setupFormatType(FormatService& service_)
            : service(service_)
        {
        }
        
        template<typename ModelType>
        inline void operator()(_formatModelTypeWrapper<ModelType>) const 
        {
            service.setupFormatType_impl<ModelType>();
        }
        
        inline void operator()(_formatModelTypeWrapper<GenericFormatModelTypeInfo::NullModelType>) const
        {
            Q_UNREACHABLE();
        }
        
        FormatService& service;
    };
    
    void updateFormatType(GenericFormatModelTypeInfo type);
    
    template<class ModelType>
    void setupFormatType_impl();

    QHash<QString, GenericFormatId> _formats_bySuffix;
    QHash<QString, GenericFormatId> _formats_byMimeType;
    QHash<QByteArray, GenericFormatId> _formats_bySignature;
    
    MPLMappedArray<GenericFormatModelTypeInfo::types, QSet<GenericFormatId>> _formats_byModelType;
    
    GenericFormatId inferFormatFromSignature(QIODevice& device);

    int _maxSignatureLength = 0;
    
    friend class visitor_setupFormat;
};

} // namespace Addle
#endif // FILESERVICE_HPP
