#ifndef VIEWREPOSITORY_HPP
#define VIEWREPOSITORY_HPP

#include "interfaces/services/iviewrepository.hpp"

namespace Addle {
    
template<class View>
class ViewRepository : public IViewRepository<View>
{
    typedef typename IViewRepository<View>::presenter_arg_t presenter_arg_t;
    typedef typename IViewRepository<View>::data_t data_t;
public:
    void add(presenter_arg_t presenter) override {}
    void remove(presenter_arg_t presenter) override {}
    
protected:
    const data_t& data_p() const { return _data; }
    
private:
    data_t _data;
};
    
}

#endif //VIEWREPOSITORY_HPP
