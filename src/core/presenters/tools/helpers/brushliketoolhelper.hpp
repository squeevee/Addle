#ifndef BRUSHLIKETOOLHELPER_HPP
#define BRUSHLIKETOOLHELPER_HPP

#include "interfaces/presenters/tools/ibrushliketoolpresenter.hpp"
#include "utilities/presethelpers/brushsizepresethelper.hpp"

#include "brushrenderers/commonbrushrenderers.hpp"

#include "utilities/mathutils.hpp"

#include "data/brushtipid.hpp"

class BrushLikeToolHelper
{
    typedef IBrushLikeToolPresenter::SizeOption SizeOption;
public:

    BrushLikeToolHelper(
        IBrushLikeToolPresenter& presenter,
        BrushTipId defaultBrushTip,
        SizeOption defaultSize,
        QHash<BrushTipId, BaseBrushRenderer*> brushTipRenderers = QHash<BrushTipId, BaseBrushRenderer*>())
        : _presenter(presenter),
        _defaultBrushId(defaultBrushTip), _brushTip(defaultBrushTip),
        _defaultSize(defaultSize), _size(defaultSize)
    {
        _brushTipRenderers = QHash<BrushTipId, BaseBrushRenderer*>({
            { IBrushLikeToolPresenter::CommonBrushTips::Pixel, new PixelBrushTipRenderer() },
            { IBrushLikeToolPresenter::CommonBrushTips::TwoSquare, new TwoSquareBrushTipRenderer() },
            { IBrushLikeToolPresenter::CommonBrushTips::Square, new SquareBrushTipRenderer() },
            { IBrushLikeToolPresenter::CommonBrushTips::HardCircle, new HardCircleBrushTipRenderer() },
            { IBrushLikeToolPresenter::CommonBrushTips::SoftCircle, new SoftCircleBrushTipRenderer() }
        }).unite(brushTipRenderers);

        _brushTips = _brushTipRenderers.keys().toSet();
    }

    const BrushTipId _defaultBrushId;
    inline BrushTipId getBrushTip() { return _brushTip; }
    inline void setBrushTip(BrushTipId brushTip)
    { 
        if (_brushTip != brushTip && _presenter.getBrushTips().contains(brushTip))
        {
            _brushTip = brushTip;
            emit _presenter.brushTipChanged(_brushTip);
        }
    }
    QSet<BrushTipId> getBrushTips() { return _brushTips; }
    BaseBrushRenderer& getBrushTipRenderer(BrushTipId id)
    {
        // todo: throw if invalid or something
        return *(_brushTipRenderers[id]);
    }
    BaseBrushRenderer& getCurrentBrushTipRenderer()
    {
        return getBrushTipRenderer(_brushTip);
    }

    const SizeOption _defaultSize;
    inline SizeOption getSize() { return _size; }
    inline void setSize(SizeOption size)
    {
        // assert can't set custom_xx through this method ?
        if (_size != size)
        {
            _size = size;
            _customPixelSize = 0;
            _customPercentSize = 0;
            emit _presenter.sizeChanged(_size);
        }
    }

    inline double getCustomPixelSize() { return _customPixelSize; }
    inline void setCustomPixelSize(double custom)
    {
        if (custom != _customPixelSize)
        {
            _customPixelSize = custom;
            _size = SizeOption::custom_px;
            emit _presenter.sizeChanged(_size);
        }
    }

    inline double getCustomPercentSize() { return _customPercentSize; }
    inline void setCustomPercentSize(double custom)
    {
        if (custom != _customPercentSize)
        {
            _customPercentSize = custom;
            _size = SizeOption::custom_percent;
            emit _presenter.sizeChanged(_size);
        }
    }

    inline double getAbsoluteBrushSize()
    {
        switch (_size)
        {
            case SizeOption::custom_px:
                return _customPixelSize;
            case SizeOption::custom_percent:
                return qQNaN(); // TODO
            default:
                return BrushSizePresetHelper::pixelValueOf(_size);
        }
    }

private:

    BrushTipId _brushTip;
    QSet<BrushTipId> _brushTips;
    QHash<BrushTipId, BaseBrushRenderer*> _brushTipRenderers;

    double _customPixelSize = 0;
    double _customPercentSize = 0;
    SizeOption _size;

    IBrushLikeToolPresenter& _presenter;
};


#endif // BRUSHLIKETOOLHELPER_HPP