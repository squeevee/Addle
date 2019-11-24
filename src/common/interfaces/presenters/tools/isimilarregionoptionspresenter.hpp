#ifndef ISIMILARREGIONOPTIONSPRESENTER_HPP
#define ISIMILARREGIONOPTIONSPRESENTER_HPP

#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

class ISimilarRegionOptionsPresenter : public virtual IToolOptionsPresenter
{
public:
    enum GapBridgeOptions {
        noBridge,
        smallGaps,
        mediumGaps,
        largeGaps
    };

    virtual GapBridgeOptions getGapBridge() = 0;
    virtual void setGapBridge(GapBridgeOptions gapBridge) = 0;
    virtual double getGapBridgeLength() = 0;

    virtual double getThreshold() = 0;
    virtual void setThreshold(double threshold) = 0;

signals:
    virtual void gapBridgeChanged(GapBridgeOptions gapBridge) = 0;
    virtual void thresholdChanged(double threshold) = 0;
};

Q_DECLARE_METATYPE(ISimilarRegionOptionsPresenter::GapBridgeOptions)

DECL_IMPLEMENTED_AS_QOBJECT(ISimilarRegionOptionsPresenter)

#endif // ISIMILARREGIONOPTIONSPRESENTER_HPP