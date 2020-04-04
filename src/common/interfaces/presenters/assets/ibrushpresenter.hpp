#ifndef IBRUSHPRESENTER_HPP
#define IBRUSHPRESENTER_HPP

#include <QObject>
#include <QSharedPointer>

#include "idtypes/brushid.hpp"
#include "iassetpresenter.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

// Brushes 

class IBrushPresenter : public IAssetPresenter
{
    Q_GADGET
public:
    virtual ~IBrushPresenter() = default;
    
    enum SizeOption
    {
        _1px,
        _2px,
        _4px,
        _7px,
        _13px,
        _25px,
        _50px,
        _100px,
        _250px,
        _500px,
        _10percent,
        _25percent,
        onethird,
        _50percent,
        custom_px,
        custom_percent
    };
    Q_ENUM(SizeOption)

    virtual void initialize(BrushId id) = 0;

    virtual BrushId getBrushId() = 0;
    
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