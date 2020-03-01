#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"

#include "interfaces/editing/irasteroperation.hpp"

#include "../helpers/toolwithassetshelper.hpp"

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
        : ToolPresenterBase(ToolPathHelper::TrackingOptions::path),
        _assetsHelper(*this, DEFAULT_BRUSH),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IEditorPresenter* owner);

    ToolId getId() { return BRUSH_TOOL_ID; }

    QList<QSharedPointer<IAssetPresenter>> getAllAssets() { _initHelper.assertInitialized(); return _assetsHelper.getAllAssets(); }
    QSharedPointer<IAssetPresenter> getAssetPresenter(PersistentId id) { _initHelper.assertInitialized(); return _assetsHelper.getAssetPresenter(id); }

    PersistentId getSelectedAsset() { _initHelper.assertInitialized(); return _assetsHelper.getSelectedAsset(); }
    void selectAsset(PersistentId id) {_initHelper.assertInitialized(); _assetsHelper.selectAsset(id); }
    QSharedPointer<IAssetPresenter> getSelectedAssetPresenter() { _initHelper.assertInitialized(); return _assetsHelper.getSelectedAssetPresenter(); }

    BrushId getSelectedBrush() { _initHelper.assertInitialized(); return _assetsHelper.getSelectedAsset(); }
    QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() { _initHelper.assertInitialized(); return _assetsHelper.getSelectedAssetPresenter(); }
    
signals:
    void brushChanged(BrushId tip);
    void selectedAssetChanged(PersistentId id);

protected:
    void onPointerEngage();
    void onPointerMove();
    void onPointerDisengage();

private:
    IEditorPresenter* _editorPresenter;

    QSharedPointer<IBrushOperation> _operation;

    ToolWithAssetsHelper<IBrushPresenter, BrushId> _assetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;
};

#endif // BRUSHTOOLPRESENTER_HPP