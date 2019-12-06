#ifndef IBRUSHLIKETOOLPRESENTER_HPP
#define IBRUSHLIKETOOLPRESENTER_HPP

#include <QObject>
#include <QString>
#include <QIcon>
#include <QSet>

#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_traits.hpp"
#include "interfaces/traits/qobject_trait.hpp"
#include "data/brushtipid.hpp"

class IBrushLikeToolPresenter : public virtual IToolPresenter
{
    Q_GADGET
public:

    struct CommonBrushTips
    {
        static const BrushTipId Pixel;
        static const BrushTipId TwoSquare;
        static const BrushTipId Square;
        static const BrushTipId HardCircle;
        static const BrushTipId SoftCircle;
    };

    enum SizeOption
    {
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
        _33percent,
        _50percent,
        custom_px,
        custom_percent
    };
    Q_ENUM(SizeOption)

    virtual ~IBrushLikeToolPresenter() = default;

    virtual BrushTipId getBrushTip() = 0;
    virtual QSet<BrushTipId> getBrushTips() = 0;

    virtual SizeOption getSize() = 0;
    virtual double getCustomPixelSize() = 0;
    virtual double getCustomPercentSize() = 0;


public slots:
    virtual void setBrushTip(BrushTipId id) = 0;
    virtual void setSize(SizeOption size) = 0;
    virtual void setCustomPixelSize(double customPixels) = 0;
    virtual void setCustomPercentSize(double customPercentage) = 0;

signals:
    virtual void brushTipChanged(BrushTipId id) = 0;
    virtual void sizeChanged(SizeOption size) = 0;

};

DECL_IMPLEMENTED_AS_QOBJECT(IBrushLikeToolPresenter)

#endif // IBRUSHLIKETOOLPRESENTER_HPP