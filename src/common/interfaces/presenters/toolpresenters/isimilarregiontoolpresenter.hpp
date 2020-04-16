#ifndef ISIMILARREGIONTOOLPRESENTER_HPP
#define ISIMILARREGIONTOOLPRESENTER_HPP

#include "interfaces/traits/compat.hpp"
#include "itoolpresenter.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/qobject_trait.hpp"

class ADDLE_COMMON_EXPORT ISimilarRegionToolPresenter : public virtual IToolPresenter
{
public:
    enum GapBridgeOptions {
        noBridge,
        smallGaps,
        mediumGaps,
        largeGaps
    };

    virtual ~ISimilarRegionToolPresenter() = default;

    virtual GapBridgeOptions getGapBridge() = 0;
    virtual void setGapBridge(GapBridgeOptions gapBridge) = 0;
    virtual double getGapBridgeLength() = 0;

    virtual double getThreshold() = 0;
    virtual void setThreshold(double threshold) = 0;

signals:
    virtual void gapBridgeChanged(GapBridgeOptions gapBridge) = 0;
    virtual void thresholdChanged(double threshold) = 0;
};

Q_DECLARE_METATYPE(ISimilarRegionToolPresenter::GapBridgeOptions)

DECL_IMPLEMENTED_AS_QOBJECT(ISimilarRegionToolPresenter)

#endif // ISIMILARREGIONTOOLPRESENTER_HPP