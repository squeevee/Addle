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
class FormatId final : public AddleId
{
    ID_TYPE_BOILERPLATE(FormatId)
public:
    inline QString mimeType() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).mimeType : QString(); }
    inline QStringList fileExtensions() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).fileExtensions : QStringList(); }
    inline QByteArrayList fileSignatures() const { return isValid() ? static_cast<const MetaData&>(baseMetaData()).fileSignatures : QByteArrayList(); }
    
    inline QString name() const { return isValid() ? dynamic_qtTrId({ "formats", key(), "name" }) : QString(); }

private:
    struct MetaData : BaseMetaData
    {
        inline MetaData(const char* const key_, int metaTypeId_, QUuid uuid_, QString mimeType_,
            QString fileExtension_, QByteArray fileSignature_ = QByteArray())
            : BaseMetaData(key_, metaTypeId_, uuid_),
            mimeType(mimeType_),
            fileExtensions(fileExtension_.isEmpty() ? QStringList()     : QStringList({ fileExtension_ })),
            fileSignatures(fileSignature_.isEmpty() ? QByteArrayList()  : QByteArrayList({fileSignature_}))
        {
        }
        inline MetaData(const char* const key_, int metaTypeId_, QUuid uuid_, QString mimeType_,
            QStringList fileExtensions_, QByteArrayList fileSignatures_ = QByteArrayList())
            : BaseMetaData(key_, metaTypeId_, uuid_),
            mimeType(mimeType_),
            fileExtensions(fileExtensions_),
            fileSignatures(fileSignatures_)
        {
        }

        virtual ~MetaData() = default;

        QString mimeType;
        QStringList fileExtensions;
        QByteArrayList fileSignatures;
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
    static QStringList fileExtensions() { return static_cast<const IdType::MetaData&>(*_metaData).fileExtensions; } \
    static QByteArrayList fileSignatures() { return static_cast<const IdType::MetaData&>(*_metaData).fileSignatures; } \
private: \
    static const QSharedPointer<const AddleId::BaseMetaData> _metaData; \
    friend class AddleId; \
};

class IDocument;
class IBrush;
class IPalette;
typedef FormatId<IDocument> DocumentFormatId;
typedef FormatId<IBrush>    BrushFormatId;
typedef FormatId<IPalette>  PaletteFormatId;

} // namespace Addle

Q_DECLARE_METATYPE(Addle::DocumentFormatId)
Q_DECLARE_TYPEINFO(Addle::DocumentFormatId, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(Addle::BrushFormatId)
Q_DECLARE_TYPEINFO(Addle::BrushFormatId, Q_PRIMITIVE_TYPE);

Q_DECLARE_METATYPE(Addle::PaletteFormatId)
Q_DECLARE_TYPEINFO(Addle::PaletteFormatId, Q_PRIMITIVE_TYPE);

template<class ModelType> inline Addle::FormatId<ModelType>::FormatId(const AddleId& other)
    : AddleId(!other.isNull() && other.metaTypeId() == qMetaTypeId<FormatId<ModelType>>() ? (quintptr)other : 0)
{
}

#endif // FORMATID_HPP
