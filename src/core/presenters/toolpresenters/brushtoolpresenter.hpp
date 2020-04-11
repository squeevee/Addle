#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"

#include "interfaces/editing/operations/irasteroperation.hpp"

#include "../helpers/toolwithassetshelper.hpp"

#include "interfaces/rendering/irenderstep.hpp"

#include "utilities/presenter/propertycache.hpp"

#include "utilities/asynctask.hpp"
#include <QQueue>

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
        : _brushAssetsHelper(*this, DEFAULT_BRUSH),
        //_previewVisibleCache(*this, ),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    ToolId getId() { return BRUSH_TOOL_ID; }

    QList<QSharedPointer<IAssetPresenter>> getAllAssets() { _initHelper.check(); return _brushAssetsHelper.getAllAssets(); }
    QSharedPointer<IAssetPresenter> getAssetPresenter(PersistentId id) { _initHelper.check(); return _brushAssetsHelper.getAssetPresenter(id); }

    PersistentId getSelectedAsset() { _initHelper.check(); return _brushAssetsHelper.getSelectedAsset(); }
    void selectAsset(PersistentId id) {_initHelper.check(); _brushAssetsHelper.selectAsset(id); }
    QSharedPointer<IAssetPresenter> getSelectedAssetPresenter() { _initHelper.check(); return _brushAssetsHelper.getSelectedAssetPresenter(); }

    BrushId getSelectedBrush() { _initHelper.check(); return _brushAssetsHelper.getSelectedAsset(); }
    QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() { _initHelper.check(); return _brushAssetsHelper.getSelectedAssetPresenter(); }
    
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
    class HoverPreview;

    IMainEditorPresenter* _mainEditorPresenter;
    
    QSharedPointer<IBrushPainter> _brushPainter;

    HoverPreview* _hoverPreview;

    ToolWithAssetsHelper<IBrushPresenter, BrushId> _brushAssetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;

    friend class HoverPreview;
};

class BrushToolPresenter::HoverPreview
{
public:
    HoverPreview(BrushToolPresenter& owner);
    ~HoverPreview();

    void updateBrush();
    void setPosition(QPointF);

    PropertyCache<bool> isVisible_cache;

private:
    bool calc_visible();
    void onVisibleChanged(bool);
    void paint();

    QPointF _position;

    QSharedPointer<IRasterSurface> _surface;
    QSharedPointer<IBrushPainter> _brushPainter;

    BrushToolPresenter& _owner;
};

#endif // BRUSHTOOLPRESENTER_HPP