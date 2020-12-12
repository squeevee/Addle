#ifndef BINDINGFILTER_HPP
#define BINDINGFILTER_HPP

#include <functional>

#include "interfaces/iamqobject.hpp"

#include "idtypes/addleid.hpp"

#include "bindingselector.hpp"
#include "utilities/collections.hpp"

namespace Addle { namespace Config { namespace detail {
   
class BindingFilter 
{
public:
    BindingFilter() = default;
    BindingFilter(const BindingFilter&) = default;
    
    inline BindingFilter& byId(AddleId id) { _ids.append(id); return *this; }
    
    template<typename IdType, typename id_predicate_t>
    inline BindingFilter& byId(id_predicate_t p, bool allowNull = false)
    {
        _idPredicate = [=](AddleId id) -> bool {
            auto id_ = static_cast<IdType>(id);
            return (id_ || allowNull) && static_cast<bool>(p(id_));
        };
        return *this;
    }
    
    template<typename T>
    inline typename std::enable_if<
        !Traits::implemented_as_QObject<T>::value,
        BindingFilter&
    >::type byParam(const T& v)
    {
        Param param;
        param.metaType = qMetaTypeId<T>();
        param.value = QVariant::fromValue(v);
        _params.append(param);
        return *this;
    }
        
    inline bool isSimple() const
    {
//         return !_idPredicate
//             && !(
//                     cpplinq::from(_params)
//                     >>  cpplinq::all([] (const Param& p) -> bool { 
//                         return !p.valuePredicate && !p.interface_iid; 
//                     })
//                 ); 
    }
    
    bool test(const BindingSelector& selector) const;
private:    
    struct Param
    {
        int metaType = QMetaType::UnknownType;
        const char* interface_iid = nullptr;
        QVariant value;
        
        std::function<bool(QVariant)> valuePredicate;
    };
    
    QList<AddleId> _ids;
    
    std::function<bool(AddleId)> _idPredicate;
    
    QList<Param> _params;
};
    
}}} // namespace Addle::Config::detail

#endif // BINDINGFILTER_HPP
