#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"

#include "interfaces/editing/irasteroperation.hpp"

#include "helpers/brushliketoolhelper.hpp"

class BrushToolPresenter : public ToolPresenterBase, public virtual IBrushToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        BrushTipId brushTip
        READ getBrushTip
        WRITE setBrushTip
        NOTIFY brushTipChanged
    )
    Q_PROPERTY(
        SizeOption size
        READ getSize
        WRITE setSize
        NOTIFY sizeChanged
    )
public:
    BrushToolPresenter()
        : ToolPresenterBase(ToolPathHelper::TrackingOptions::endpoints),
        _brushHelper(*this, DEFAULT_BRUSH_TIP, DEFAULT_BRUSH_SIZE),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IEditorPresenter* owner);

    ToolId getId() { return BRUSH_TOOL_ID; }
  
    BrushTipId getBrushTip() { _initHelper.assertInitialized(); return _brushHelper.getBrushTip(); }
    QSet<BrushTipId> getBrushTips() { _initHelper.assertInitialized(); return _brushHelper.getBrushTips(); }

    SizeOption getSize() { _initHelper.assertInitialized(); return _brushHelper.getSize(); }
    double getCustomPixelSize() { _initHelper.assertInitialized(); return _brushHelper.getCustomPixelSize(); }
    double getCustomPercentSize() { _initHelper.assertInitialized(); return _brushHelper.getCustomPercentSize(); }

public slots:
    void setBrushTip(BrushTipId tip) { _initHelper.assertInitialized(); _brushHelper.setBrushTip(tip); }
    void setSize(SizeOption size)  { _initHelper.assertInitialized(); _brushHelper.setSize(size); }
    void setCustomPixelSize(double customPixels) { _initHelper.assertInitialized(); _brushHelper.setCustomPixelSize(customPixels); }
    void setCustomPercentSize(double customPercentage) { _initHelper.assertInitialized(); _brushHelper.setCustomPercentSize(customPercentage); }

signals:
    void brushTipChanged(BrushTipId tip);
    void sizeChanged(SizeOption size);

protected:

    void onPointerEngage();
    void onPointerMove();
    void onPointerDisengage();

private:
    IEditorPresenter* _editorPresenter;

    IRasterOperation* _operation = nullptr;

    BrushLikeToolHelper _brushHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;
};

#endif // BRUSHTOOLPRESENTER_HPP