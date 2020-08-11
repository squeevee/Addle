#ifndef FORMATID_HPP
#define FORMATID_HPP

#include "persistentid.hpp"

#include "utilities/typeinforef.hpp"
namespace Addle {

class ADDLE_COMMON_EXPORT FormatId : public PersistentId
{
    PERSISTENT_ID_BOILERPLATE(FormatId)
public:
    FormatId(const char* key,
        QString mimeType,
        const std::type_info& modelType,
        QString fileExtension,
        QByteArray fileSignature)
    : FormatId(key, {
        { QStringLiteral("mimeType"), mimeType },
        { QStringLiteral("modelType"), TypeInfoRef(modelType) },
        { QStringLiteral("fileExtensions"), QStringList({ fileExtension }) },
        { QStringLiteral("fileSignature"), fileSignature }
    })
    {
    }

    FormatId(const char* key,
        QString mimeType,
        const std::type_info& modelType,
        QStringList fileExtensions,
        QByteArray fileSignature
        )
    : FormatId(key, {
        { QStringLiteral("mimeType"), mimeType },
        { QStringLiteral("modelType"), TypeInfoRef(modelType) },
        { QStringLiteral("fileExtensions"), fileExtensions },
        { QStringLiteral("fileSignature"), fileSignature }
    })
    {
    }

    inline QString mimeType() const
    {
        return isNull() ? QString() : metadata().value(QStringLiteral("mimeType")).toString();
    }
    
    inline const std::type_info& modelType() const
    {
        //!! dangerous if null
        return metadata().value(QStringLiteral("modelType")).value<TypeInfoRef>();
    }

    inline QString fileExtension() const
    {
        return isNull() ? QString() : metadata().value(QStringLiteral("fileExtensions")).toStringList().first();
    }

    inline QStringList fileExtensions() const
    {
        return isNull() ? QStringList() : metadata().value(QStringLiteral("fileExtensions")).toStringList();
    }

    inline QByteArray fileSignature() const
    {
        return isNull() ? QByteArray() : metadata().value(QStringLiteral("fileSignature")).toByteArray();
    }

};

} // namespace Addle

Q_DECLARE_METATYPE(Addle::FormatId)
Q_DECLARE_TYPEINFO(Addle::FormatId, Q_MOVABLE_TYPE);

#endif // FORMATID_HPP