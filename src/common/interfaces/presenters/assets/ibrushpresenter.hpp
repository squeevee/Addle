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

#include "ibrushpresenteraux.hpp"

// Brushes 
class IBrushModel;
class IBrushPresenter : public IAssetPresenter
{
public:
    typedef IBrushPresenterAux::SizeOption SizeOption;
    virtual ~IBrushPresenter() = default;
    
    virtual void initialize(IBrushModel& model) = 0;
    virtual void initialize(BrushId id) = 0;

    virtual BrushId getBrushId() = 0;
    virtual IBrushModel& model() const = 0;
    
    virtual SizeOption getSize() = 0;
    virtual double getCustomPixelSize() = 0;
    virtual double getCustomPercentSize() = 0;

public slots:
    virtual void setSize(SizeOption size) = 0;
    virtual void setCustomPixelSize(double customPixels) = 0;
    virtual void setCustomPercentSize(double customPercentage) = 0;

signals:
    virtual void sizeChanged(SizeOption size) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IBrushPresenter)
DECL_EXPECTS_INITIALIZE(IBrushPresenter)
DECL_MAKEABLE(IBrushPresenter)

#endif // IBRUSHPRESENTER_HPP