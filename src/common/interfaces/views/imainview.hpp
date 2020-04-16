#ifndef IMAINVIEW_HPP
#define IMAINVIEW_HPP

#include "interfaces/traits/compat.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class ADDLE_COMMON_EXPORT IMainView
{
public:
    virtual ~IMainView() = default;

public slots:
    virtual void start() = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IMainView);

#endif // IMAINVIEW_HPP