#ifndef GENERICFORMAT_HPP
#define GENERICFORMAT_HPP

#include <boost/variant.hpp>
#include <typeinfo>

#include "idtypes/formatid.hpp"
#include "importexportinfo.hpp"

namespace Addle {

class Alternate; // placeholder

class ADDLE_COMMON_EXPORT GenericFormatId
{
public:
    typedef boost::variant<
        DocumentFormatId,
        FormatId<Alternate>
    > variant_t;

    GenericFormatId() = default;

    template<class ModelType>
    inline GenericFormatId(const FormatId<ModelType>& id)
        : _value(id)
    {
    }

    GenericFormatId(const GenericFormatId&) = default;

    inline const char* key() const { return id_p().key(); }
    inline QUuid uuid() const { return id_p().uuid(); }
    inline int metaTypeId() const { return id_p().metaTypeId(); }

    inline bool isNull() const { return id_p().isNull(); }
    explicit inline operator bool() const { return static_cast<bool>(id_p()); }
    inline bool operator!() const { return !id_p(); }

    inline bool operator==(const GenericFormatId& other) const
    {
        return _value == other._value;
    }
    inline bool operator==(const AddleId& other) const
    {
        return id_p() == other;
    }

    inline bool operator!=(const GenericFormatId& other) const
    {
        return _value != other._value;
    }
    inline bool operator!=(const AddleId& other) const
    {
        return id_p() != other;
    }

    inline bool isValid() const { return id_p().isValid(); }

    inline operator AddleId() const { return id_p(); }

    QString mimeType() const { return boost::apply_visitor(visitor_getMimeType(), _value); }
    QString fileExtension() const;
    QStringList fileExtensions() const;
    QByteArray fileSignature() const;

    template<class ModelType>
    inline FormatId<ModelType> get() const
    {   
        boost::get<FormatId<ModelType>>(_value);
    }

    inline const variant_t variant() const { return _value; }

    const std::type_info& modelType() const { return *_modelTypeInfo; }

private:
    struct visitor_getId
    {
        typedef AddleId result_type;
        template<typename ModelType>
        inline AddleId operator()(const FormatId<ModelType>& id) const { return static_cast<AddleId>(id); }
    };

    struct visitor_getMimeType
    {
        typedef QString result_type;
        template<typename ModelType>
        inline QString operator()(const FormatId<ModelType>& id) const { return id.mimeType(); }
    };

    variant_t _value;
    const std::type_info* _modelTypeInfo = nullptr;

    inline AddleId id_p() const { return boost::apply_visitor(visitor_getId(), _value); }

    inline friend uint qHash(const GenericFormatId& id, uint seed = 0)
    {
        return qHash(id.id_p(), seed);
    }
};

class GenericImportExportInfo
{
public:
    typedef boost::variant<
        DocumentImportExportInfo,
        ImportExportInfo<Alternate>
    > variant_t;

    inline GenericImportExportInfo() = default;

    template<class ModelType>
    inline GenericImportExportInfo(const ImportExportInfo<ModelType>& info)
        : _value(info), _modelTypeInfo(&typeid(ModelType))
    {
    }
    
    GenericImportExportInfo(const GenericImportExportInfo&) = default;

    ImportExportDirection direction() const;
    void setDirection(ImportExportDirection direction);

    GenericFormatId format() const { return boost::apply_visitor(visitor_getFormat(), _value); }
    template<class ModelType>
    inline void setFormat(FormatId<ModelType> id)
    {
        setFormat(id);
    }
    void setFormat(GenericFormatId format);

    QFileInfo fileInfo() const { return boost::apply_visitor(visitor_getFileInfo(), _value); }
    void setFileInfo(QFileInfo fileInfo);

    QString filename() const { return boost::apply_visitor(visitor_getFilename(), _value); }
    void setFilename(const QString& filename);

    QUrl url() const;
    void setUrl(const QUrl& url);

    template<class ModelType>
    inline const ImportExportInfo<ModelType>& get() const
    {
        return boost::get<const ImportExportInfo<ModelType>&>(_value);
    }
    inline const variant_t& variant() { return _value; }
    const std::type_info& modelType() const { return *_modelTypeInfo; }

private:
    struct visitor_getFormat
    {
        typedef GenericFormatId result_type;
        template<class ModelType>
        inline GenericFormatId operator()(const ImportExportInfo<ModelType>& info) const { return info.format(); }
    };

    struct visitor_getFilename
    {
        typedef QString result_type;
        template<class ModelType>
        inline QString operator()(const ImportExportInfo<ModelType>& info) const { return info.filename(); }
    };

    struct visitor_getFileInfo
    {
        typedef QFileInfo result_type;
        template<class ModelType>
        inline QFileInfo operator()(const ImportExportInfo<ModelType>& info) const { return info.fileInfo(); }
    };

    variant_t _value;
    const std::type_info* _modelTypeInfo = nullptr;
};

class GenericFormatModel
{
public:
    typedef boost::variant<
        IDocument*,
        Alternate*
    > variant_t;

    GenericFormatModel() = default;

    GenericFormatModel(const GenericFormatModel&) = default;

    template<class ModelType>
    GenericFormatModel(ModelType* model)
        : _value(model)
    {
    }

    const variant_t& variant() const { return _value; }

private:
    variant_t _value;
};

template<class ModelType>
class IFormatDriver;

class GenericFormatDriver
{
public:
    typedef boost::variant<
        IFormatDriver<IDocument>*,
        IFormatDriver<Alternate>*
    > variant_t;

    GenericFormatDriver() = default;

    template<class ModelType>
    GenericFormatDriver(IFormatDriver<ModelType>& driver)
        : _value(&driver)
    {
    }

    GenericFormatDriver(const GenericFormatDriver&) = default;

    inline bool isNull() const
    {
        visitor_isNull visitor;
        return boost::apply_visitor(visitor, _value);
    }

    inline bool operator!() const { return isNull(); }
    inline explicit operator bool() const { return !isNull(); }

    bool supportsImport() const;
    bool supportsExport() const;

    GenericFormatModel importModel(QIODevice& device, GenericImportExportInfo info);
    void exportModel(GenericFormatModel model, QIODevice& device, GenericImportExportInfo info);
    
    inline const variant_t& variant() const { return _value; }
    const std::type_info& modelType() const { return *_modelTypeInfo; }

private:
    struct visitor_isNull
    {
        typedef bool result_type;
        template<typename T> bool operator()(const T* ptr) const { return (bool)ptr; }
    };

    variant_t _value;
    const std::type_info* _modelTypeInfo = nullptr;
};

}

#endif // GENERICFORMAT_HPP