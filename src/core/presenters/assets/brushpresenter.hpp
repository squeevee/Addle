#ifndef BRUSHPRESENTER_HPP
#define BRUSHPRESENTER_HPP

#include <QObject>
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "utilities/initializehelper.hpp"

class BrushPresenter : public QObject, public IBrushPresenter
{
    Q_OBJECT 
public:
    BrushPresenter() : _initHelper(this) {}
    void initialize(IBrushModel& model);
    void initialize(BrushId id);

    PersistentId getId() { return getBrushId(); }
    BrushId getBrushId() { _initHelper.check(); return _id; }
    
    SizeOption getSize() { return SizeOption::_25px; } //todo
    double getCustomPixelSize() { return 0; } //
    double getCustomPercentSize() { return 0; } //

    IToolWithAssetsPresenter* getOwnerTool() { return nullptr; }
    void setOwnerTool(IToolWithAssetsPresenter* owner) {}

    void selectInTool() { }

public slots:
    void setSize(SizeOption size) {} //
    void setCustomPixelSize(double customPixels) {} //
    void setCustomPercentSize(double customPercentage) {} //

signals:
    void sizeChanged(SizeOption size);

private:
    SizeOption _size;
    double _customPixelSize;
    double _customPercentSize;

    BrushId _id;
    IBrushModel* _model;

    InitializeHelper<BrushPresenter> _initHelper;
};

#endif // BRUSHPRESENTER_HPP