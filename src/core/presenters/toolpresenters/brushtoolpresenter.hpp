#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "toolpresenterbase.hpp"
#include "interfaces/presenters/toolpresenters/ibrushtoolpresenter.hpp"

#include "interfaces/editing/operations/irasteroperation.hpp"

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
    
signals:
    void brushChanged(BrushId tip);
    void selectedAssetChanged(PersistentId id);

protected:
    void onEngage();
    void onMove();
    void onDisengage();

private:
    IMainEditorPresenter* _mainEditorPresenter;

    QSharedPointer<IBrushOperation> _operation;

    ToolWithAssetsHelper<IBrushPresenter, BrushId> _assetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;
};

#endif // BRUSHTOOLPRESENTER_HPP