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

    inline QString getMimeType() const
    {
        return isNull() ? QString() : getMetadata().value(QStringLiteral("mimeType")).toString();
    }
    
    inline const std::type_info& getModelType() const
    {
        //!! dangerous if null
        return getMetadata().value(QStringLiteral("modelType")).value<TypeInfoRef>();
    }

    inline QString getFileExtension() const
    {
        return isNull() ? QString() : getMetadata().value(QStringLiteral("fileExtensions")).toStringList().first();
    }

    inline QStringList getFileExtensions() const
    {
        return isNull() ? QStringList() : getMetadata().value(QStringLiteral("fileExtensions")).toStringList();
    }

    inline QByteArray getFileSignature() const
    {
        return isNull() ? QByteArray() : getMetadata().value(QStringLiteral("fileSignature")).toByteArray();
    }

};

Q_DECLARE_METATYPE(FormatId)
Q_DECLARE_TYPEINFO(FormatId, Q_MOVABLE_TYPE);

#endif // FORMATID_HPP