#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include "compat.hpp"
#include "toolpresenterbase.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/operations/ibrushoperationpresenter.hpp"

#include "../helpers/brushiconhelper.hpp"
#include "../helpers/toolwithassetshelper.hpp"

#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/ibrushengine.hpp"
#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

#include "utilities/presenter/propertycache.hpp"

#include "utilities/asynctask.hpp"

#include "servicelocator.hpp"
#include <QQueue>

#include <memory>

class ADDLE_CORE_EXPORT BrushToolPresenter : public ToolPresenterBase, public virtual IBrushToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        BrushId selectedBrush 
        READ selectedBrush
        WRITE selectBrush
        NOTIFY brushChanged
    )
public:
    BrushToolPresenter()
        : //_brushAssetsHelper(*this, IBrushToolPresenterAux::DEFAULT_BRUSH),
        _initHelper(this)
    {
        _icon = QIcon(":/icons/brush.png");
        //_brushAssetsHelper.onChange(std::bind(&BrushToolPresenter::updateSizeSelection, this));
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner);

    // ISizeSelectionPresenter& sizeSelection();

    ToolId getId() { return IBrushToolPresenterAux::ID; }

    // QList<QSharedPointer<IAssetPresenter>> getAllAssets() { _initHelper.check(); return _brushAssetsHelper.getAllAssets(); }
    // QSharedPointer<IAssetPresenter> getAssetPresenter(PersistentId id) { _initHelper.check(); return _brushAssetsHelper.getAssetPresenter(id); }

    // PersistentId getSelectedAsset() { _initHelper.check(); return _brushAssetsHelper.getSelectedAsset(); }
    // void selectAsset(PersistentId id) {_initHelper.check(); _brushAssetsHelper.selectAsset(id); }
    // QSharedPointer<IAssetPresenter> getSelectedAssetPresenter() { _initHelper.check(); return _brushAssetsHelper.getSelectedAssetPresenter(); }

    // BrushId getSelectedBrush() { _initHelper.check(); return _brushAssetsHelper.getSelectedAsset(); }
    // QSharedPointer<IBrushPresenter> getSelectedBrushPresenter() { _initHelper.check(); return _brushAssetsHelper.getSelectedAssetPresenter(); }
    
    IAssetSelectionPresenter& brushSelection() { _initHelper.check(); return *_brushSelection; }

    void selectBrush(BrushId id)
    {
        brushSelection().select(id);
    }

    BrushId selectedBrush()
    {
        return static_cast<BrushId>(brushSelection().selectedId());
    } 

    QSharedPointer<IBrushPresenter> selectedBrushPresenter()
    {
        return brushSelection().selectedPresenter().staticCast<IBrushPresenter>();
    }

    virtual bool event(QEvent* e);

signals:
    void brushChanged(BrushId brush);

protected:
    void onEngage();
    void onMove();
    void onDisengage();

    void onSelected();
    void onDeselected();

private slots:
    void onBrushSelectionChanged();
    void onSelectedLayerChanged(QWeakPointer<ILayerPresenter> layer);
    void onSizeChanged(double size);
    void onCanvasHasMouseChanged(bool hasMouse);
    void onViewPortZoomChanged(double zoom);

private:
    // void updateSizeSelection();

    // ISizeSelectionPresenter* _sizeSelection = nullptr;

    IMainEditorPresenter* _mainEditorPresenter;
    
    QSharedPointer<BrushStroke> _brushStroke;

    class HoverPreview;
    HoverPreview* _hoverPreview;

    //ToolWithAssetsHelper<IBrushPresenter, BrushId> _brushAssetsHelper;
    InitializeHelper<BrushToolPresenter> _initHelper;

    BrushIconHelper _iconHelper;
    bool _grace = false;

    std::unique_ptr<IAssetSelectionPresenter> _brushSelection;

    QMetaObject::Connection _connection_onSizeChanged;

    friend class HoverPreview;
};

class BrushToolPresenter::HoverPreview
{
public:
    HoverPreview(BrushToolPresenter& owner);
    ~HoverPreview();

    void updateBrush();
    void setPosition(QPointF);
    void setSize(double size);

    PropertyCache<bool> isVisible_cache;

private:
    bool calc_visible();
    void onVisibleChanged(bool);
    void paint();

    QPointF _position;
    double _size = 0;

    QSharedPointer<IRasterSurface> _surface;
    QSharedPointer<BrushStroke> _brushStroke;

    BrushToolPresenter& _owner;
};

#endif // BRUSHTOOLPRESENTER_HPP