#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "compat.hpp"
#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"

#include "../helpers/brushiconhelper.hpp"
#include "../helpers/toolwithassetshelper.hpp"

#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/ibrushengine.hpp"

#include "utilities/presenter/propertycache.hpp"

#include "utilities/asynctask.hpp"

#include "servicelocator.hpp"
#include <QQueue>

class ADDLE_CORE_EXPORT BrushToolPresenter : public ToolPresenterBase, public virtual IBrushToolPresenter
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
        : _brushAssetsHelper(*this, IBrushToolPresenterAux::DEFAULT_BRUSH),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
        _brushAssetsHelper.onChange(std::bind(&BrushToolPresenter::updateSizeSelection, this));
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    ISizeSelectionPresenter& sizeSelection();

    ToolId getId() { return IBrushToolPresenterAux::ID; }

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
    void onSizeChanged(double size);

private:
    void updateSizeSelection();

    ISizeSelectionPresenter* _sizeSelection = nullptr;

    IMainEditorPresenter* _mainEditorPresenter;
    
    QSharedPointer<BrushStroke> _brushStroke;

    class HoverPreview;
    HoverPreview* _hoverPreview;

    ToolWithAssetsHelper<IBrushPresenter, BrushId> _brushAssetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;

    BrushIconHelper _iconHelper;

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
    QSharedPointer<BrushStroke> _brushStroke;

    BrushToolPresenter& _owner;
};

#endif // BRUSHTOOLPRESENTER_HPP