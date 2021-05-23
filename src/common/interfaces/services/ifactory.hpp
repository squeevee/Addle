#ifndef IFACTORY_HPP_2
#define IFACTORY_HPP_2

#include <memory>
#include <type_traits>
#include <boost/type_traits/detected_or.hpp>
#include <boost/type_traits/is_detected.hpp>
#include <boost/type_traits/is_complete.hpp>

#include <boost/mp11.hpp>

#include <QSharedPointer>

#include "utilities/config/factoryparams.hpp"
#include "interfaces/traits.hpp"

//#include "utilities/initparams/baseinitparams.hpp"

namespace Addle {

template<class> class IFactory;
    
namespace config_detail {

template<class Interface>
class ifactory_base_with_params
{
public:
    virtual ~ifactory_base_with_params() = default;
    
    inline Interface* make(const factory_params_t<Interface>& p) const
    {
        return make_p(p);
    }
    
    template<typename... ArgTypes>
    inline Interface* make(ArgTypes&&... args) const
    {
        auto dispatcher = make_factory_param_dispatcher<Interface>(
                std::bind(
                    &ifactory_base_with_params<Interface>::make_p,
                    this,
                    std::placeholders::_1
                )
            );
        
        return dispatcher(std::forward<ArgTypes>(args)...);
    }
    
protected:
    // called by factory_param_dispatcher
    virtual Interface* make_p(const factory_params_t<Interface>&) const = 0;
    
//     friend class factory_param_dispatcher<Interface>;
};


template<class Interface>
class ifactory_base_without_params
{
public:
    virtual ~ifactory_base_without_params() = default;
    
    inline Interface* make() const { return make_p(); }
    
protected:
    virtual Interface* make_p() const = 0;
};

} // namespace config_detail

template<class Interface>
class IFactory 
    : public boost::mp11::mp_if<
        config_detail::has_factory_params<Interface>,
        config_detail::ifactory_base_with_params<Interface>,
        config_detail::ifactory_base_without_params<Interface>
    >
{
    static_assert(boost::is_complete<Interface>::value);
    static_assert(Traits::is_makeable<Interface>::value);
public:
    virtual ~IFactory() = default;
    
    template<typename... ArgTypes>
    inline QSharedPointer<Interface> makeShared(ArgTypes&&... args) const
    {
        return QSharedPointer<Interface>(this->make(std::forward<ArgTypes>(args)...));
    }
    
    template<typename... ArgTypes>
    inline std::unique_ptr<Interface> makeUnique(ArgTypes&&... args) const
    {
        return std::unique_ptr<Interface>(this->make(std::forward<ArgTypes>(args)...));
    }
};

namespace Traits {
    
template<class Interface>
struct is_singleton<IFactory<Interface>> : std::true_type {};

template<class T>
struct is_factory : std::false_type {};

template<class Interface>
struct is_factory<IFactory<Interface>> : std::true_type {};

} // namespace Traits
} // namespace Addle

#endif // IFACTORY_HPP_2
