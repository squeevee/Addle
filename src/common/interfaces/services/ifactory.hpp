#ifndef IFACTORY_HPP_2
#define IFACTORY_HPP_2

#include <memory>
#include <type_traits>
#include <boost/type_traits/detected_or.hpp>
#include <boost/type_traits/is_detected.hpp>

#include <QSharedPointer>

#include "interfaces/traits.hpp"

//#include "utilities/initparams/baseinitparams.hpp"

namespace Addle {
    
namespace Config { namespace detail {
    
struct _nil_params {};

template<class Interface>
using init_params_t = typename Traits::init_params<Interface>::type;

template<class Interface>
using init_params_placeholder_t = typename boost::detected_or<
        _nil_params, init_params_t, Interface
    >::type;
    
template<class Interface>
using has_init_params = boost::is_detected<init_params_t, Interface>;
        
}} // namespace Config::detail

template<class Interface>
class IFactory
{
    static_assert(
        Traits::is_makeable<Interface>::value,
        "A factory may only be instantiated for a makeable type."
    );
    
public:
    virtual ~IFactory() = default;

    template<typename... ArgTypes>
    inline Interface* make(ArgTypes... args) const
    {
        static_assert(
            Config::detail::has_init_params<Interface>::value || sizeof...(args) == 0,
            "Interface does not accept initialization parameters. You must call "
            "make without any arguments."
        );
        return make_p(Config::detail::init_params_placeholder_t<Interface>(args...));
    }
    
    template<typename... ArgTypes>
    inline QSharedPointer<Interface> makeShared(ArgTypes... args) const
    {
        static_assert(
            Config::detail::has_init_params<Interface>::value || sizeof...(args) == 0,
            "Interface does not accept initialization parameters. You must call "
            "make without any arguments."
        );
        return QSharedPointer<Interface>(
            make_p(Config::detail::init_params_placeholder_t<Interface>(args...))
        );
    }
    
    template<typename... ArgTypes>
    inline std::unique_ptr<Interface> makeUnique(ArgTypes... args) const
    {
        static_assert(
            Config::detail::has_init_params<Interface>::value || sizeof...(args) == 0,
            "Interface does not accept initialization parameters. You must call "
            "make without any arguments."
        );
        return std::unique_ptr<Interface>(
            make_p(Config::detail::init_params_placeholder_t<Interface>(args...))
        );
    }
    
protected:
    virtual Interface* make_p(const Config::detail::init_params_placeholder_t<Interface>& params) const = 0;
};

namespace Traits {
    
template<class Interface>
struct is_singleton_gettable<IFactory<Interface>> : std::true_type {};

template<class T>
struct is_factory : std::false_type {};

template<class Interface>
struct is_factory<IFactory<Interface>> : std::true_type {};

} // namespace Traits
} // namespace Addle

#endif // IFACTORY_HPP_2
