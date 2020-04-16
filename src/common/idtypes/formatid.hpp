#ifndef FORMATID_HPP
#define FORMATID_HPP

#include "persistentid.hpp"

#include "utilities/typeinforef.hpp"

class ADDLE_COMMON_EXPORT FormatId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(FormatId)
public:
    FormatId(const char* key,
        QString mimeType,
        const std::type_info& modelType)
    : FormatId(key, {
        { QStringLiteral("mimeType"), mimeType },
        { QStringLiteral("modelType"), TypeInfoRef(modelType) }
    })
    {
    }

    inline QString getMimeType()
    {
        return isNull() ? QString() : getMetadata().value(QStringLiteral("mimeType")).toString();
    }
    
    inline const std::type_info& getModelType()
    {
        //!! dangerous if null
        return getMetadata().value(QStringLiteral("modelType")).value<TypeInfoRef>();
    }
};

Q_DECLARE_METATYPE(FormatId)
Q_DECLARE_TYPEINFO(FormatId, Q_MOVABLE_TYPE);

#endif // FORMATID_HPP