#ifndef BRUSHPRESENTER_HPP
#define BRUSHPRESENTER_HPP

#include <memory>

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/assetpresenters/ibrushpresenter.hpp"
#include "utilities/initializehelper.hpp"

#include "interfaces/presenters/toolpresenters/isizeselectionpresenter.hpp"

#include "../helpers/brushiconhelper.hpp"

class ADDLE_CORE_EXPORT BrushPresenter : public QObject, public IBrushPresenter
{
    Q_OBJECT 
public:
    BrushPresenter() : _initHelper(this) {}
    void initialize(IBrushModel& model);
    void initialize(BrushId id);

    PersistentId getId() { return getBrushId(); }
    BrushId getBrushId() { _initHelper.check(); return _id; }
    virtual IBrushModel& model() const { _initHelper.check(); return *_model; }
    
    // SizeOption getSize() { return SizeOption::_25px; } //todo
    // double getCustomPixelSize() { return 0; } //
    // double getCustomPercentSize() { return 0; } //

    // IToolWithAssetsPresenter* getOwnerTool() { return nullptr; }
    // void setOwnerTool(IToolWithAssetsPresenter* owner) {}

    void selectInTool() { }

    ISizeSelectionPresenter& sizeSelection() { _initHelper.check(); return *_sizeSelection; }

    double size() { return _sizeSelection->get(); }
    void setSize(double size) { _sizeSelection->set(size); }

// public slots:
//     void setSize(SizeOption size) {} //
//     void setCustomPixelSize(double customPixels) {} //
//     void setCustomPercentSize(double customPercentage) {} //

// signals:
//     void sizeChanged(SizeOption size);

private slots:
    void onSizeSelectionScaleChanged();

private:
    // SizeOption _size;
    // double _customPixelSize;
    // double _customPercentSize;

    void updateSizeSelectionIcons();

    BrushId _id;
    IBrushModel* _model;

    std::unique_ptr<ISizeSelectionPresenter> _sizeSelection;

    BrushIconHelper _iconHelper;

    InitializeHelper<BrushPresenter> _initHelper;
};

#endif // BRUSHPRESENTER_HPP