#ifndef IBRUSHPRESENTER_HPP
#define IBRUSHPRESENTER_HPP

#include <QObject>
#include <QSharedPointer>

#include "idtypes/brushid.hpp"
#include "iassetpresenter.hpp"

#include "interfaces/models/ibrushmodel.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

namespace IBrushPresenterAux
{
    static constexpr double DEFAULT_SIZES[] = { 
        4.0,
        7.0,
        12.0,
        21.0,
        36.0,
        60.0,
        100.0,
        180.0,
        320.0,
        600.0 
    };
}

// Brushes 
class ISizeSelectionPresenter;
class IBrushModel;
class IBrushPresenter : public IAssetPresenter
{
public:
    virtual ~IBrushPresenter() = default;
    
    virtual void initialize(IBrushModel& model) = 0;
    virtual void initialize(BrushId id) = 0;

    virtual BrushId getBrushId() = 0;
    virtual IBrushModel& model() const = 0;

    virtual ISizeSelectionPresenter& sizeSelection() = 0;

    virtual double size() = 0;
    virtual void setSize(double size) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IBrushPresenter)
DECL_EXPECTS_INITIALIZE(IBrushPresenter)
DECL_MAKEABLE(IBrushPresenter)

#endif // IBRUSHPRESENTER_HPP