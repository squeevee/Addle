#ifndef IFACTORY_HPP
#define IFACTORY_HPP
 
#include <typeinfo>

namespace Addle {

class IFactory
{
public:
    virtual ~IFactory() = default;
    virtual void* make() const = 0;
    virtual void delete_(void* obj) const = 0;
};
} // namespace Addle

#endif // IFACTORY_HPP
