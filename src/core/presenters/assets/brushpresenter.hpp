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
public:
    BrushPresenter() : _initHelper(this) {}
    void initialize(IBrushModel& model, QSharedPointer<const PreviewInfoProvider> info = nullptr);
    void initialize(BrushId id, QSharedPointer<const PreviewInfoProvider> info = nullptr);

    PersistentId getId() { return getBrushId(); }
    BrushId getBrushId() { _initHelper.check(); return _id; }
    virtual IBrushModel& model() const { _initHelper.check(); return *_model; }

    QString name();
    QIcon icon();
    
    // SizeOption getSize() { return SizeOption::_25px; } //todo
    // double getCustomPixelSize() { return 0; } //
    // double getCustomPercentSize() { return 0; } //

    // IToolWithAssetsPresenter* getOwnerTool() { return nullptr; }
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
    IBrushModel* _model;

    std::unique_ptr<ISizeSelectionPresenter> _sizeSelection;
    BrushIconHelper _iconHelper;

    InitializeHelper<BrushPresenter> _initHelper;
};

} // namespace Addle
#endif // BRUSHPRESENTER_HPP