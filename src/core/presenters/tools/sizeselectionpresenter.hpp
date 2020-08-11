#ifndef SIZESELECTIONPRESENTER_HPP
#define SIZESELECTIONPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/presethelper.hpp"

class ADDLE_CORE_EXPORT SizeSelectionPresenter : public QObject, public ISizeSelectionPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        double size
        READ get 
        WRITE set
        NOTIFY changed
    )
public:
    virtual ~SizeSelectionPresenter() = default;

    void initialize(QSharedPointer<IconProvider> iconProvider = nullptr);
    QList<double> presets() const;

    double get() const;

    int selectedPreset() const;

    QIcon icon() const { _initHelper.check(); return _icon; }
    QList<QIcon> presetIcons() const { _initHelper.check(); return _presetIcons; }

    double min() const { return _min; }
    void setMin(double min) { _min = min; }

    double max() const { return _max; }
    void setMax(double max) { _max = max; }

    bool strictSizing() const { return _strictSizing; }
    void setStrictSizing(bool strict) { _strictSizing = strict; }

public slots:
    void setPresets(QList<double> presets);

    void set(double size);
    void selectPreset(int index);

signals:
    void changed(double size);
    void selectedPresetChanged(int index);
    void presetsChanged(QList<double> presets);

    void refreshPreviews();
    
private:
    void updateIcon();

    double _size = qQNaN();
    int _selectedPreset = -1;
    QColor _color;
    
    QList<double> _presets;
    QMap<double, int> _presetsIndex;

    double _min = 0;
    double _max = Q_INFINITY;
    bool _strictSizing = false;

    QSharedPointer<IconProvider> _iconProvider;
    QIcon _icon;
    QList<QIcon> _presetIcons;

    InitializeHelper<SizeSelectionPresenter> _initHelper;
    PresetHelper<> _presetHelper;
};

#endif // SIZESELECTIONPRESENTER_HPP