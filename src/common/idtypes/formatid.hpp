/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FORMATID_HPP
#define FORMATID_HPP

#include "addleid.hpp"
#include <typeinfo>

namespace Addle {

template<class ModelType_>
class ADDLE_COMMON_EXPORT FormatId final : public AddleId
{
    ID_TYPE_BOILERPLATE(FormatId)
public:
    inline QString mimeType() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).mimeType : QString(); }
    inline QString fileExtension() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).fileExtension : QString(); }
    inline QStringList fileExtensions() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).fileExtensions : QStringList(); }
    inline QByteArray fileSignature() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).fileSignature : QByteArray(); }

private:
    struct MetaData : BaseMetaData
    {
        inline MetaData(const char* const key_, int metaTypeId_, QUuid uuid_, QString mimeType_,
            QString fileExtension_, QByteArray fileSignature_ = QByteArray())
            : BaseMetaData(key_, metaTypeId_, uuid_),
            mimeType(mimeType_),
            fileExtension(fileExtension_),
            fileExtensions({ fileExtension_ }),
            fileSignature(fileSignature_)
        {
        }
        inline MetaData(const char* const key_, int metaTypeId_, QUuid uuid_, QString mimeType_,
            QStringList fileExtensions_, QByteArray fileSignature_ = QByteArray())
            : BaseMetaData(key_, metaTypeId_, uuid_),
            mimeType(mimeType_),
            fileExtension(fileExtensions_.constFirst()),
            fileExtensions(fileExtensions_),
            fileSignature(fileSignature_)
        {
        }

        virtual ~MetaData() = default;

        QString mimeType;
        QString fileExtension;
        QStringList fileExtensions;
        QByteArray fileSignature;
    };

    template<typename IdType, quintptr id>
    friend class StaticIdMetaData;
};

#define DECLARE_STATIC_FORMAT_ID_METADATA(id, key_) \
template<> class StaticIdMetaData<std::remove_const<decltype(id)>::type, static_cast<quintptr>(id)> { \
    typedef std::remove_const<decltype(id)>::type IdType; \
public: \
    static constexpr const char* key = key_; \
    static QString mimeType() { return static_cast<const IdType::MetaData&>(*_metaData).mimeType; } \
    static QString fileExtension() { return static_cast<const IdType::MetaData&>(*_metaData).fileExtension; } \
    static QStringList fileExtensions() { return static_cast<const IdType::MetaData&>(*_metaData).fileExtensions; } \
    static QByteArray fileSignature() { return static_cast<const IdType::MetaData&>(*_metaData).fileSignature; } \
private: \
    static const QSharedPointer<const AddleId::BaseMetaData> _metaData; \
    friend class AddleId; \
};

class IDocument;
typedef FormatId<IDocument> DocumentFormatId;

} // namespace Addle

Q_DECLARE_METATYPE(Addle::DocumentFormatId)
Q_DECLARE_TYPEINFO(Addle::DocumentFormatId, Q_PRIMITIVE_TYPE);

template<class ModelType> inline Addle::FormatId<ModelType>::FormatId(const AddleId& other)
    : AddleId(!other.isNull() && other.metaTypeId() == qMetaTypeId<FormatId<ModelType>>() ? (quintptr)other : 0)
{
}

#endif // FORMATID_HPP