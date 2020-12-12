#ifndef IVIEWFOR_HPP
#define IVIEWFOR_HPP

#include <QSharedPointer>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {
    
template<class PresenterType_>
class IViewFor : public virtual IAmQObject
{
public:
    typedef PresenterType_ PresenterType;
    virtual ~IViewFor() = default;
    
    virtual PresenterType& presenter() const = 0;
};

template<class PresenterType_>
class IViewForShared : public virtual IAmQObject
{
public:
    typedef PresenterType_ PresenterType;
    virtual ~IViewForShared() = default;
    
    virtual QSharedPointer<PresenterType> presenter() const = 0;
};

namespace Traits {

template<class PresenterType_>
struct init_params<IViewFor<PresenterType_>> { typedef std::tuple<PresenterType_&> type; };

template<class PresenterType_>
struct init_params<IViewForShared<PresenterType_>> { typedef std::tuple<QSharedPointer<PresenterType_>> type; };
    
}

} // namespace Addle

#endif // IVIEWFOR_HPP
