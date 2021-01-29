#ifndef IVIEWREPOSITORY_HPP
#define IVIEWREPOSITORY_HPP

#include <list>
#include <tuple>

#include <boost/type_traits/is_detected_convertible.hpp>

#include <QMultiHash>

#include "interfaces/traits.hpp"

#include "utilities/collections.hpp"
#include "utilities/lazyvalue.hpp"
#include "utilities/qobject.hpp"

namespace Addle {

template<class View>
class IViewRepository 
{
    static_assert(
        Traits::implemented_as_QObject<View>::value,
        "IViewRepository is only supported for views implemented as QObject"
    );

    
};

namespace Traits
{
    template<typename View> 
    struct is_singleton<IViewRepository<View>> : std::true_type {};
}

}

#endif // IVIEWREPOSITORY_HPP
