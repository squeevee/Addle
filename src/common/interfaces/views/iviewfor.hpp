#ifndef IVIEWFOR_HPP
#define IVIEWFOR_HPP

#include <QSharedPointer>

#include "interfaces/iamqobject.hpp"

namespace Addle {
    
template<class PresenterType_>
class IViewFor : public virtual IAmQObject
{
public:
    typedef PresenterType_ PresenterType;
    
    virtual ~IViewFor() = default;
    
    virtual void initialize(PresenterType& presenter) = 0;
    virtual PresenterType& presenter() const = 0;
};

template<class PresenterType_>
class IViewForShared : public virtual IAmQObject
{
public:
    typedef PresenterType_ PresenterType;
    
    virtual ~IViewForShared() = default;
    
    virtual void initialize(QSharedPointer<PresenterType> presenter) = 0;
    virtual QSharedPointer<PresenterType> presenter() const = 0;
};
    
} // namespace Addle

#endif // IVIEWFOR_HPP
