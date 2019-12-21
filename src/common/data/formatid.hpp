#ifndef FORMATID_HPP
#define FORMATID_HPP

#include "persistentid.hpp"

#include "utilities/qt_extensions/typeinforef.hpp"

class FormatId : public PersistentId
{
public:
    FormatId() = default;
    FormatId(const char* key,
        QString mimeType,
        const std::type_info& modelType)
    : PersistentId(key, {
        { QStringLiteral("mimeType"), mimeType },
        { QStringLiteral("modelType"), TypeInfoRef(modelType) }
    })
    {
    }

    inline QString getMimeType() { return getMetadata().value(QStringLiteral("mimeType")).toString(); }
    
    inline const std::type_info& getModelType()
    {
        return getMetadata().value(QStringLiteral("modelType")).value<TypeInfoRef>();
    }
};

Q_DECLARE_METATYPE(FormatId)

#endif // FORMATID_HPP