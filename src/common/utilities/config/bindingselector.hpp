#ifndef BINDINGSELECTOR_HPP
#define BINDINGSELECTOR_HPP

#include <typeinfo>

#include <boost/optional.hpp>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

#include <QVariant>

#include "idtypes/addleid.hpp" 

#include "interfaces/services/ifactory.hpp"

#include "utilities/collections.hpp"
#include "utilities/indexvariant.hpp"
#include "utilities/qobject.hpp"

namespace Addle { namespace Config { namespace detail { 

class BindingSelector
{
public:
    BindingSelector() = default;
    BindingSelector(const BindingSelector&) = default;
    BindingSelector(const std::type_info& interface, AddleId id = AddleId())
        : _interface(&interface), _id(id)
    {
    }
    
    inline bool isNull() const { return !_interface; }
    
    inline const std::type_info& interface() const { return *_interface; }
    
    inline AddleId id() const { return _id; }
    inline BindingSelector& setId(AddleId id) { 
        _preHash.reset(); 
        _id = id; 
        return *this;
    }
    
    template<class Interface>
    inline BindingSelector& setParams(const init_params_placeholder_t<Interface>& params)
    {
        _preHash.reset();
        setParams_impl<Interface>(params);
        return *this;
    }
    
    inline BindingSelector paramsStripped() const
    {
        if (isNull())
            return BindingSelector();
        else
            return BindingSelector(*_interface, _id);
    }
    
    inline bool operator==(const BindingSelector& x) const
    {
//         return (isNull() && x.isNull())
//             || (
//                 *_interface == *x._interface
//                 && _id == x._id 
//                 &&  
//                     cpplinq::from(_params)
//                 >>  cpplinq::sequence_equal(cpplinq::from(x._params))
//             );
    }
    
    inline bool operator!=(const BindingSelector& x) const
    {
        return !(*this == x);
    }
    
    template<class Interface>
    static BindingSelector from(AddleId id = AddleId())
    {
        return BindingSelector(typeid(Interface), id);
    }
    
private:    
    template<class Interface>
    inline typename std::enable_if<
        !has_init_params<Interface>::value
    >::type setParams_impl(const _nil_params&)
    {
    }
    
    template<class Interface>
    inline typename std::enable_if<
        has_init_params<Interface>::value
    >::type setParams_impl(const init_params_t<Interface>& params);
    
    const std::type_info* _interface = nullptr;
    AddleId _id;
    QVector<QVariant> _params;
    mutable boost::optional<uint> _preHash;
    
    friend uint qHash(const BindingSelector& x, uint seed = 0)
    {
        uint hash;
        if (Q_LIKELY(x._preHash))
        {
            hash = *x._preHash;
        }
        else if (x.isNull())
        {
            hash = 0;
            x._preHash = 0;
        }
        else
        {
            hash = qHash(std::type_index(*x._interface)) ^ qHash(x._id);
            
            if (!x._params.isEmpty())
            {
//                 auto range = 
//                         cpplinq::from(x._params)
//                     >>  cpplinq::cast<IndexVariant>();
                
//                 hash ^= qHashRange(begin(range), end(range));
            }
            
            x._preHash = hash;
        }
        return hash ^ seed;
    }
    
    friend class BindingFilter;
};

    
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
template<typename T>
struct _is_qt_metatype : std::integral_constant<bool, QMetaTypeId2<T>::Defined> {};
#else
#warning "A working MetaType trait kludge is not known for the current version of Qt. Binding selection may not work properly."
template<typename T>
struct _is_qt_metatype : std::false_type {};
#endif

template<class tuple_t>
struct extract_params_functor
{
    inline extract_params_functor(const tuple_t& params_, QVector<QVariant>& paramVariants_)
        : params(params_), paramVariants(paramVariants_)
    {
    }
    
    template<typename I_>
    inline void operator()(const I_&)
    {
        const int I = I_::value;
        typedef typename std::tuple_element<I, tuple_t>::type paramType;
        
        params[I] = getVariant(std::get<I>(params));
    }
    
    template<typename T>
    inline typename std::enable_if<
        _is_qt_metatype<T>::value,
        QVariant 
    >::type getVariant(const T& t)
    {
        return QVariant::fromValue(t);
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && std::is_base_of<QObject, T>::value,
        QVariant 
    >::type getVariant(T& t)
    {
        return QVariant::fromValue(&t);
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && std::is_base_of<QObject, T>::value,
        QVariant 
    >::type getVariant(QSharedPointer<T> t)
    {
        return QVariant::fromValue(t.data());
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && std::is_base_of<QObject, T>::value,
        QVariant 
    >::type getVariant(const T& t)
    {
        return QVariant::fromValue(&const_cast<T&>(t));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && std::is_base_of<QObject, T>::value,
        QVariant 
    >::type getVariant(const T* t)
    {
        return QVariant::fromValue(const_cast<T*>(t));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && std::is_base_of<QObject, T>::value,
        QVariant 
    >::type getVariant(QSharedPointer<const T> t)
    {
        return QVariant::fromValue(const_cast<T*>(t.data()));
    }
    
            template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(T& t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(&t));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(T* t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(t));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(QSharedPointer<T> t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(t.data()));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(const T& t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(&const_cast<T&>(t)));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(const T* t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(const_cast<T*>(t)));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_const<T>::value
        && !std::is_base_of<QObject, T>::value
        && Traits::implemented_as_QObject<T>::value,
        QVariant 
    >::type getVariant(QSharedPointer<const T> t)
    {
        return QVariant::fromValue(qobject_interface_cast<QObject*>(const_cast<T*>(t.data())));
    }
    
    template<typename T>
    inline typename std::enable_if<
        !_is_qt_metatype<T>::value
        && !std::is_base_of<QObject, T>::value
        && !Traits::implemented_as_QObject<T>::value,
        QVariant
    >::type getVariant(const T&)
    {
        return QVariant();
    }
    
    const tuple_t& params;
    QVector<QVariant>& paramVariants;
};

template<typename tuple_t>
inline QVector<QVariant> extractParams(const tuple_t& params)
{
    QVector<QVariant> result(std::tuple_size<tuple_t>::value);

    extract_params_functor<tuple_t> f(params, result);
    
    boost::mpl::for_each<
        boost::mpl::range_c<
            std::size_t,
            0,
            std::tuple_size<tuple_t>::value
        >
    >(f);
    
    return result;
}

template<class Interface>
inline typename std::enable_if<
    has_init_params<Interface>::value
>::type BindingSelector::setParams_impl(const init_params_t<Interface>& params)
{
    _params = extractParams<init_params_t<Interface>>(params);
}

}}} // namespace Addle::Config::detail

#endif // BINDINGSELECTOR_HPP
