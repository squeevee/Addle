/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHTOOLPRESENTER_HPP
#define BRUSHTOOLPRESENTER_HPP

#include <QQueue>
#include <QPointer>

#include "compat.hpp"
#include "interfaces/presenters/tools/ibrushtoolpresenter.hpp"
#include "interfaces/presenters/operations/ibrushoperationpresenter.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"

#include "interfaces/rendering/irenderstep.hpp"
#include "interfaces/editing/ibrushengine.hpp"
#include "interfaces/presenters/tools/iassetselectionpresenter.hpp"

#include "utilities/lazyvalue.hpp"

#include "utilities/asynctask.hpp"
#include "utilities/model/colorinfo.hpp"

#include "servicelocator.hpp"

#include "toolhelpers/toolselecthelper.hpp"
#include "utilities/initializehelper.hpp"
#include "toolhelpers/mousehelper.hpp"

#include <memory>

namespace Addle {

class ADDLE_CORE_EXPORT BrushToolPresenter : public QObject, public virtual IBrushToolPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        BrushId selectedBrush 
        READ selectedBrush
        WRITE selectBrush
        NOTIFY brushChanged
    )
    Q_INTERFACES(Addle::IToolPresenter Addle::IBrushToolPresenter)
    IAMQOBJECT_IMPL
public:
    BrushToolPresenter()
        : _selectHelper(*this)
    {
        _mouseHelper.onEngage.bind(&BrushToolPresenter::onEngage, this);  
        _mouseHelper.onMove.bind(&BrushToolPresenter::onMove, this);
        _mouseHelper.onDisengage.bind(&BrushToolPresenter::onDisengage, this);
        _selectHelper.onIsSelectedChanged.bind(&BrushToolPresenter::onSelectedChanged, this);
    }
    virtual ~BrushToolPresenter() = default;

    void initialize(IMainEditorPresenter* owner, Mode mode);

    IMainEditorPresenter* owner() { ASSERT_INIT(); return _mainEditor; }
    ToolId id();

    IAssetSelectionPresenter& brushSelection() { ASSERT_INIT(); return *_brushSelection; }
    void selectBrush(BrushId id) { brushSelection().select(id); }
    BrushId selectedBrush()
    {
        return static_cast<BrushId>(brushSelection().selectedId());
    } 
    QSharedPointer<IBrushPresenter> selectedBrushPresenter()
    {
        return brushSelection().selectedPresenter().staticCast<IBrushPresenter>();
    }

    bool event(QEvent* e) override;

    bool isSelected() const { return _selectHelper.isSelected(); }
    void setSelected(bool isSelected) { _selectHelper.setSelected(isSelected); }

    QCursor cursor() { return QCursor(Qt::CrossCursor); }

signals:
    void brushChanged(BrushId brush);
    void isSelectedChanged(bool isSelected);
    void refreshPreviews();

    void cursorChanged(QCursor cursor); 
    
private slots:
    void onDocumentChanged(QSharedPointer<Addle::IDocumentPresenter> document);
    void onColorChanged(ColorInfo info);
    void onBrushSelectionChanged();
    void onSelectedLayerChanged();
    void onSizeChanged(double size);
    void onCanvasHasMouseChanged(bool hasMouse);
    void onViewPortZoomChanged(double zoom);

private:
    class HoverPreview;

    void onEngage();
    void onMove();
    void onDisengage();
    void onSelectedChanged(bool isSelected);

    bool _grace;

    Mode _mode = (Mode)NULL;

    QMetaObject::Connection _connection_onSizeChanged;
    QMetaObject::Connection _connection_onSelectedLayerChanged;

    IMainEditorPresenter* _mainEditor;
    ICanvasPresenter* _canvas;
    IViewPortPresenter* _viewPort;
    
    QSharedPointer<IDocumentPresenter> _document;

    QSharedPointer<BrushStroke> _brushStroke;
    std::unique_ptr<IAssetSelectionPresenter> _brushSelection;
    IColorSelectionPresenter* _colorSelection;

    ToolSelectHelper _selectHelper;
    MouseHelper _mouseHelper;

    QWeakPointer<ILayerPresenter> _operatingLayer;

    std::unique_ptr<HoverPreview> _hoverPreview;

    QSharedPointer<const IBrushPresenter::PreviewInfoProvider> _previewProvider;

    InitializeHelper _initHelper;
    friend class HoverPreview;
    friend class BrushPreviewProvider;
};

class BrushToolPresenter::HoverPreview
{
public:
    HoverPreview(BrushToolPresenter& owner);

    void update();
    void setPosition(QPointF);

    LazyProperty<bool> isVisible_cache;
private:
    bool calc_visible();

    void onVisibleChanged(bool);
    
    void paint();

    QPointF _position;
    double _size = qQNaN();

    QSharedPointer<ILayerPresenter> _layer;

    QSharedPointer<IRasterSurface> _surface;
    std::unique_ptr<BrushStroke> _brushStroke;

    BrushToolPresenter& _owner;
};

class BrushPreviewProvider : public QObject, public IBrushPresenter::PreviewInfoProvider
{
    Q_OBJECT
public:
    BrushPreviewProvider(QPointer<BrushToolPresenter> presenter)
        : _presenter(presenter)
    {
    }
    virtual ~BrushPreviewProvider() = default;

    double scale() const override;
    QColor color() const override;

signals:
    void refresh();

private:
    QPointer<BrushToolPresenter> _presenter;
};

} // namespace Addle

#endif // BRUSHTOOLPRESENTER_HPP
