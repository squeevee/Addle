#ifndef IPROPERTYDECORATEDPRESENTER_HPP
#define IPROPERTYDECORATEDPRESENTER_HPP

#include "interfaces/traits/qobject_trait.hpp"
#include "utilities/presenter/propertydecoration.hpp"

class IPropertyDecoratedPresenter
{
public:
    virtual ~IPropertyDecoratedPresenter() = default;

    virtual PropertyDecoration getPropertyDecoration(const char* propertyName) const = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IPropertyDecoratedPresenter)

#endif // IPROPERTYDECORATEDPRESENTER_HPP