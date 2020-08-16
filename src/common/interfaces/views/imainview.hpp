#ifndef IMAINVIEW_HPP
#define IMAINVIEW_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
namespace Addle {

class IMainView : public virtual IAmQObject
{
public:
    virtual ~IMainView() = default;

public slots:
    virtual void start() = 0;
};

//DECL_IMPLEMENTED_AS_QOBJECT(IMainView))

} // namespace Addle
#endif // IMAINVIEW_HPP