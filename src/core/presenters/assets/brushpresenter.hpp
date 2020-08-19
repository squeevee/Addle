/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHPRESENTER_HPP
#define BRUSHPRESENTER_HPP

#include <memory>

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

#include "../helpers/brushiconhelper.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT BrushPresenter : public QObject, public IBrushPresenter
{
    Q_OBJECT 
    IAMQOBJECT_IMPL
public:
    virtual ~BrushPresenter() = default;

    void initialize(IBrush& model, QSharedPointer<const PreviewInfoProvider> info = nullptr);
    void initialize(BrushId id, QSharedPointer<const PreviewInfoProvider> info = nullptr);

    PersistentId id() { return brushId(); }
    BrushId brushId() { _initHelper.check(); return _id; }
    virtual IBrush& model() const { _initHelper.check(); return *_model; }

    QString name();
    QIcon icon();
    
    // SizeOption size() { return SizeOption::_25px; } //todo
    // double customPixelSize() { return 0; } //
    // double customPercentSize() { return 0; } //

    // IToolWithAssetsPresenter* ownerTool() { return nullptr; }
    // void setOwnerTool(IToolWithAssetsPresenter* owner) {}

    void selectInTool() { }

    ISizeSelectionPresenter& sizeSelection() { _initHelper.check(); return *_sizeSelection; }

    double size() { return _sizeSelection->get(); }
    void setSize(double size) { _sizeSelection->set(size); }

    void setPreviewInfo(QSharedPointer<const PreviewInfoProvider> info);

// public slots:
//     void setSize(SizeOption size) {} //
//     void setCustomPixelSize(double customPixels) {} //
//     void setCustomPercentSize(double customPercentage) {} //

// signals:
//     void sizeChanged(SizeOption size);

private:
    // SizeOption _size;
    // double _customPixelSize;
    // double _customPercentSize;

    QIcon _assetIcon;

    BrushId _id;
    IBrush* _model;

    std::unique_ptr<ISizeSelectionPresenter> _sizeSelection;
    BrushIconHelper _iconHelper;

    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // BRUSHPRESENTER_HPP