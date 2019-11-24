#ifndef ITOOLPRESENTER_HPP
#define ITOOLPRESENTER_HPP

#include <QObject>
#include <QPoint>
#include "../ihavetoolspresenter.hpp"

#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

class IToolPresenter
{
public:
    virtual ~IToolPresenter() = default;
    
    //virtual void initialize(IHaveToolsPresenter* owner) = 0;
    virtual IHaveToolsPresenter* getOwner() = 0;

    virtual void pointerEngage(QPoint pos) = 0;
    virtual void pointerMove(QPoint pos) = 0;
    virtual void pointerDisengage(QPoint pos) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IToolPresenter)

#endif // ITOOLPRESENTER_HPP