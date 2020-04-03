#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"

#include "interfaces/editing/operations/irasteroperation.hpp"

#include "../helpers/toolwithassetshelper.hpp"

#include "interfaces/rendering/irenderstep.hpp"

#include "utilities/presenter/propertycache.hpp"

#include "utilities/canvas/brushpainterdata.hpp"

class HoveringBrushPreview;
class BrushToolPresenter : public ToolPresenterBase, public virtual IBrushToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        PersistentId selectedAsset 
        READ getSelectedAsset
        WRITE selectAsset 
        NOTIFY selectedAssetChanged
    )
    Q_PROPERTY(
        BrushId brush 
        READ getSelectedBrush
        WRITE selectAsset 
        NOTIFY brushChanged
    )
public:
    BrushToolPresenter()
        : _assetsHelper(*this, DEFAULT_BRUSH),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    ToolId getId() { return BRUSH_TOOL_ID; }

    QList<QSharedPointer<IAssetPresenter>> getAllAssets() { _initHelper.check(); return _assetsHelper.getAllAssets(); }
    QSharedPointer<IAssetPresenter> getAssetPresenter(PersistentId id) { _initHelper.check(); return _assetsHelper.getAssetPresenter(id); }

    PersistentId getSelectedAsset() { _initHelper.check(); return _assetsHelper.getSelectedAsset(); }
    void selectAsset(PersistentId id) {_initHelper.check(); _assetsHelper.selectAsset(id); }
    QSharedPointer<IAssetPresenter> getSelectedAssetPresenter() { _initHelper.check(); return _assetsHelper.getSelectedAssetPresenter(); }

    BrushId getSelectedBrush() { _initHelper.check(); return _assetsHelper.getSelectedAsset(); }
    QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() { _initHelper.check(); return _assetsHelper.getSelectedAssetPresenter(); }
    
    virtual bool event(QEvent* e);

signals:
    void brushChanged(BrushId tip);
    void selectedAssetChanged(PersistentId id);

protected:
    void onEngage();
    void onMove();
    void onDisengage();

    void onSelected();
    void onDeselected();

private slots:
    void onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer);

private:
    bool calc_hoverVisible();
    void hoverVisibleChanged(bool visible);

    QSharedPointer<HoveringBrushPreview> _hoveringBrushPreview;
    
    IMainEditorPresenter* _mainEditorPresenter;

    QSharedPointer<IBrushOperationPresenter> _operation;

    ToolWithAssetsHelper<IBrushPresenter, BrushId> _assetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;

    friend class HoveringBrushPreview;
};

class HoveringBrushPreview : public QObject, public IRenderStep
{
    Q_OBJECT 
public:
    HoveringBrushPreview(BrushToolPresenter& owner)
        : _owner(owner),
        _visibleCache(*this, &HoveringBrushPreview::calculateVisible)
    {
        _visibleCache.initialize(false);
        _visibleCache.onChange({
            [&]() { onVisibleChanged(); }
        });

        _previewData.setColor(Qt::blue);
        _previewData.setSize(10);
        _previewData.setPreview(true);
    }
    virtual ~HoveringBrushPreview() = default;

    void before(RenderData& data);
    void after(RenderData& data);

signals: 
    void changed(QRect area);

private: 
    bool calculateVisible();
    void onVisibleChanged();

    void updateBuffer();
    void move(QPointF to);

    PropertyCache<HoveringBrushPreview, bool> _visibleCache;

    static constexpr double BUFFER_RESIZE_FACTOR = 2.0;

    BrushPainterData _previewData;
    QRect _previewRect;
    QRect _previousPreviewRect;

    QImage _buffer;
    QPoint _bufferOffset;

    BrushToolPresenter& _owner;

    friend class BrushToolPresenter;
};
#endif // BRUSHTOOLPRESENTER_HPP