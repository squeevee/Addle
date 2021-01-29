#ifndef VIEWREPOSITORY_HPP
#define VIEWREPOSITORY_HPP

#include "interfaces/services/ifactory.hpp"
#include "interfaces/services/iviewrepository.hpp"

namespace Addle {
    
template<class View>
class ViewRepository : public IViewRepository<View>
{
//     typedef typename IViewRepository<View>::presenter_arg_t presenter_arg_t;
//     typedef typename IViewRepository<View>::data_t data_t;
// public:
//     ViewRepository(const IFactory<View>& factory)
//         : _factory(factory)
//     {
//     }
//     
//     void add(presenter_arg_t presenter) override
//     {
//         _data.insert(
//             detail::view::presenter_arg_to_key<View>(presenter),
//             _factory.make(std::ref(presenter)) // TODO: Gotta forward references, this is silly.
//         );
//     }
//     
//     void remove(presenter_arg_t presenter) override
//     {
//         
//     }
//     
// protected:
//     const data_t& data_p() const { return _data; }
//     
// private:
//     
//     const IFactory<View>& _factory;
//     data_t _data;
};
    
}

#endif //VIEWREPOSITORY_HPP
