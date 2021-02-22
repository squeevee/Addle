/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * Modification and distribution permitted under the terms of the MIT License. 
 * See "LICENSE" for full details.
 * 
 * Low-level classes and interfaces involved in formatting and serialization are
 * defined in terms of the particular model type being formatted. This file
 * provides high-level abstractions of those classes and interfaces, so they can 
 * be handled and used as needed by generic format management and serialization 
 * pipeline code.
 * 
 * Boost Variants and MPL are used to achieve this. Use those libraries to
 * interact with the underlying strong types if desired.
 */

#ifndef GENERICFORMAT_HPP
#define GENERICFORMAT_HPP

#include <typeinfo>

#include <boost/mpl/contains.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>

#include <boost/type_traits/add_pointer.hpp>

#include <boost/variant.hpp>

#include <QSharedPointer>
#include <QMetaType>
#include <QIODevice>

#include "idtypes/formatid.hpp"

#include "../multirepository.hpp"

namespace Addle {

template<class ModelType>
class IFormatDriver;

template<typename ModelType>
struct _formatModelTypeWrapper {};

class ImportExportInfo;

/**
 * @class
 * Identifies and provides information about one of the model types supported by 
 * the generic format types. The default-constructed instance is a null object 
 * (represented internally / in visitors  by identifying the placeholder class
 * NullModelType.)
 * 
 * The canonical list of model types supported by the generic format classes is
 * provided by `GenericFormatModelTypeInfo::types` as a Boost.MPL sequence.
 */
class GenericFormatModelTypeInfo
{
public:
    
    typedef boost::mpl::vector<
        IDocument,
        IBrush,
        IPalette
    >::type types;
    
    GenericFormatModelTypeInfo()
        : _value(_formatModelTypeWrapper<NullModelType>())
    {   
    }
    GenericFormatModelTypeInfo(const GenericFormatModelTypeInfo&) = default;
    GenericFormatModelTypeInfo& operator=(const GenericFormatModelTypeInfo&) = default;

    inline bool isNull() const { return _value.which() == 0; }
    inline int index() const { return _value.which() - 1; }
    
    inline bool operator==(const GenericFormatModelTypeInfo& other) const { return _value.which() == other._value.which(); }
    inline bool operator!=(const GenericFormatModelTypeInfo& other) const { return _value.which() != other._value.which(); }
    
    inline const std::type_info& typeInfo() const;
    
    inline const char* q_iid() const;
    
    inline int formatidMetaTypeId() const;
    inline const char* formatIdMetaTypeName() const;
    
    template<typename ModelType>
    inline static GenericFormatModelTypeInfo fromType()
    {
        static_assert(
            boost::mpl::contains<types, ModelType>::type::value,
            "Given Type is not a supported generic format model."
        );
        return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
    }
    
    template<typename Visitor>
    inline typename Visitor::result_type apply_visitor(Visitor v)
    {
        return boost::apply_visitor(v, _value);
    }

    struct NullModelType {};
    
private:
    typedef boost::make_variant_over<
        boost::mpl::transform<
            boost::mpl::push_front<
                types,
                NullModelType
            >::type,
            _formatModelTypeWrapper<boost::mpl::placeholders::_1>
        >::type
    >::type variant_t;
        
    explicit GenericFormatModelTypeInfo(const variant_t& value)
        : _value(value)
    {
    }
    
    struct visitor_getType
    {
        typedef GenericFormatModelTypeInfo result_type;
        
        template<typename ModelType>
        inline GenericFormatModelTypeInfo operator()(FormatId<ModelType>) const;
                
        template<typename ModelType>
        inline GenericFormatModelTypeInfo operator()(const ModelType*) const;
        
        template<typename ModelType>
        inline GenericFormatModelTypeInfo operator()(QSharedPointer<ModelType>) const;
        
        template<typename ModelType>
        inline GenericFormatModelTypeInfo operator()(QSharedPointer<const ModelType>) const;
        
        template<typename ModelType>
        inline GenericFormatModelTypeInfo operator()(const IFormatDriver<ModelType>*) const;
    };
        
    variant_t _value;
    
    friend class GenericFormatId;
    friend class GenericFormatModel;
    friend class GenericSharedFormatModel;
    friend class GenericFormatDriver;
    
    friend class visitor_getIdsFor;
};

/**
 * @class
 * @brief Generic abstraction of a FormatId.
 */
class ADDLE_COMMON_EXPORT GenericFormatId
{
public:
    typedef boost::make_variant_over<
        boost::mpl::transform<
            GenericFormatModelTypeInfo::types,
            FormatId<boost::mpl::placeholders::_1>
        >::type
    >::type variant_t;
    
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

    inline QString mimeType() const { return boost::apply_visitor(visitor_getMimeType(), _value); }
    inline QString fileExtension() const { return boost::apply_visitor(visitor_getFileExtensions(), _value).constFirst(); }
    inline QStringList fileExtensions() const { return boost::apply_visitor(visitor_getFileExtensions(), _value); }
    inline QByteArray fileSignature() const;

    inline QString name() const { return boost::apply_visitor(visitor_getName(), _value); }
    
    template<class ModelType>
    inline FormatId<ModelType> get() const
    {   
        boost::get<FormatId<ModelType>>(_value);
    }
    inline const variant_t& variant() const { return _value; }
    inline GenericFormatModelTypeInfo type() const { return boost::apply_visitor(GenericFormatModelTypeInfo::visitor_getType(), _value); }
    
    static QSet<GenericFormatId> getAll();
    static QSet<GenericFormatId> getFor(GenericFormatModelTypeInfo type);
    
private:
    struct visitor_getId
    {
        typedef AddleId result_type;
        template<typename ModelType>
        inline AddleId operator()(FormatId<ModelType> id) const { return static_cast<AddleId>(id); }
    };

    struct visitor_getMimeType
    {
        typedef QString result_type;
        template<typename ModelType>
        inline QString operator()(FormatId<ModelType> id) const { return id.mimeType(); }
    };

    struct visitor_getFileExtensions
    {
        typedef QStringList result_type;
        template <typename ModelType>
        inline QStringList operator()(FormatId<ModelType> id) const { return id.fileExtensions(); }
    };
    
    struct visitor_getName
    {
        typedef QString result_type;
        template<typename ModelType>
        inline QString operator()(FormatId<ModelType> id) const { return id.name(); }
    };
    
    variant_t _value;

    inline AddleId id_p() const { return boost::apply_visitor(visitor_getId(), _value); }

    inline friend uint qHash(const GenericFormatId& id, uint seed = 0)
    {
        return qHash(id.id_p(), seed);
    }
};

/**
 * @class
 * @brief Generic abstraction of a plain pointer to a format model.
 */
class ADDLE_COMMON_EXPORT GenericFormatModel
{
public:
    typedef boost::make_variant_over<
        boost::mpl::transform<
            GenericFormatModelTypeInfo::types,
            boost::add_pointer<boost::mpl::placeholders::_1>
        >::type
    >::type variant_t;

    GenericFormatModel() = default;
    GenericFormatModel(const GenericFormatModel&) = default;
    
    template<class ModelType>
    GenericFormatModel(ModelType* model)
        : _value(model)
    {
    }
    
    void destroy();

    template<class ModelType>
    inline ModelType* get() const
    {
        return boost::get<ModelType*>(_value);
    }
    
    inline const variant_t& variant() const { return _value; }
    inline GenericFormatModelTypeInfo type() const { return boost::apply_visitor(GenericFormatModelTypeInfo::visitor_getType(), _value); }

private:    
    variant_t _value;
};

/**
 * @class
 * @brief Generic abstraction of a QSharedPointer to a format model.
 */
class ADDLE_COMMON_EXPORT GenericSharedFormatModel
{
public:
    typedef boost::make_variant_over<
        boost::mpl::transform<
            GenericFormatModelTypeInfo::types,
            QSharedPointer<boost::mpl::placeholders::_1>
        >::type
    >::type variant_t;
    
    GenericSharedFormatModel() = default;
    GenericSharedFormatModel(const GenericSharedFormatModel&) = default;
    
    explicit GenericSharedFormatModel(const GenericFormatModel& model)
        : _value(boost::apply_visitor(visitor_wrapSharedModel(), model.variant()))
    {
    }
    
    template<class ModelType>
    GenericSharedFormatModel(QSharedPointer<ModelType> model)
        : _value(model)
    {
    }
    
    template<class ModelType>
    inline QSharedPointer<ModelType> get() const
    {
        return boost::get<QSharedPointer<ModelType>>(_value);
    }
    
    inline void clear() { boost::apply_visitor(visitor_clear(), _value); }
    
    inline const variant_t& variant() const { return _value; }
    inline GenericFormatModelTypeInfo type() const { 
        return boost::apply_visitor(GenericFormatModelTypeInfo::visitor_getType(), _value);
    }
    
private:
    struct visitor_wrapSharedModel
    {
        typedef variant_t result_type;
        
        template<typename ModelType>
        variant_t operator()(ModelType* model) const
        {
            return QSharedPointer<ModelType>(model);
        }
    };
    
    struct visitor_clear
    {
        typedef void result_type;
        
        template<typename ModelType>
        void operator()(QSharedPointer<ModelType>& model) const
        {
            model.clear();
        }
    };
    
    variant_t _value;
};


typedef make_multirepository_over<
    boost::mpl::transform<
        GenericFormatModelTypeInfo::types,
        IFormatDriver<boost::mpl::placeholders::_1>
    >::type
>::type GenericFormatDriverRepository;

/**
 * @class
 * @brief Generic abstraction of a reference to IFormatDriver.
 */
class ADDLE_COMMON_EXPORT GenericFormatDriver
{
public:
    typedef boost::make_variant_over<
        boost::mpl::transform<
            GenericFormatModelTypeInfo::types,
            boost::add_pointer<IFormatDriver<boost::mpl::placeholders::_1>>
        >::type
    >::type variant_t;

    GenericFormatDriver() = default;

    GenericFormatDriver(GenericFormatDriverRepository::value_t value)
    {
    }
    
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

    GenericFormatModel importModel(QIODevice& device, const ImportExportInfo& info);
    void exportModel(GenericFormatModel model, QIODevice& device, const ImportExportInfo& info);
    
    template<class ModelType>
    inline IFormatDriver<ModelType>* get() const
    {
        return boost::get<IFormatDriver<ModelType>*>(_value);
    }
    
    inline const variant_t& variant() const { return _value; }
    inline GenericFormatModelTypeInfo type() const { return boost::apply_visitor(GenericFormatModelTypeInfo::visitor_getType(), _value); }

    static GenericFormatDriver get(GenericFormatId id);
    
private:
    
    struct visitor_isNull
    {
        typedef bool result_type;
        template<typename T> bool operator()(const T* ptr) const { return (bool)ptr; }
    };

    variant_t _value;
};

template<typename ModelType>
inline GenericFormatModelTypeInfo GenericFormatModelTypeInfo::visitor_getType::operator()(FormatId<ModelType>) const
{
    return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
}

template<typename ModelType>
inline GenericFormatModelTypeInfo GenericFormatModelTypeInfo::visitor_getType::operator()(const ModelType*) const
{
    return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
}

template<typename ModelType>
inline GenericFormatModelTypeInfo GenericFormatModelTypeInfo::visitor_getType::operator()(QSharedPointer<ModelType>) const
{
    return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
}

template<typename ModelType>
inline GenericFormatModelTypeInfo GenericFormatModelTypeInfo::visitor_getType::operator()(QSharedPointer<const ModelType>) const
{
    return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
}

template<typename ModelType>
inline GenericFormatModelTypeInfo GenericFormatModelTypeInfo::visitor_getType::operator()(const IFormatDriver<ModelType>*) const
{
    return GenericFormatModelTypeInfo(_formatModelTypeWrapper<ModelType>());
}

} // namespace Addle

Q_DECLARE_METATYPE(Addle::GenericFormatId);
Q_DECLARE_METATYPE(Addle::GenericFormatModel);
Q_DECLARE_METATYPE(Addle::GenericSharedFormatModel);
Q_DECLARE_METATYPE(Addle::GenericFormatDriver);

#endif // GENERICFORMAT_HPP
