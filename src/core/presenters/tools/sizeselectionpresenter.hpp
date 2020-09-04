/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef SIZESELECTIONPRESENTER_HPP
#define SIZESELECTIONPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

#include "utilities/initializehelper.hpp"
#include "utilities/presetmap.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT SizeSelectionPresenter : public QObject, public ISizeSelectionPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        double size
        READ get 
        WRITE set
        NOTIFY changed
    )
    Q_INTERFACES(Addle::ISizeSelectionPresenter)
    IAMQOBJECT_IMPL
public:
    virtual ~SizeSelectionPresenter() = default;

    void initialize(QSharedPointer<ISizeIconProvider> ISizeIconProvider = nullptr);
    QList<double> presets() const;

    double get() const;

    int selectedPreset() const;

    QIcon icon() const { ASSERT_INIT(); return _icon; }
    QList<QIcon> presetIcons() const { ASSERT_INIT(); return _presetIcons; }

    double min() const { return _min; }
    double max() const { return _max; }

    bool strictSizing() const { return _strictSizing; }

public slots:
    void set(double size);
    void selectPreset(int index);

    void setMin(double min) { _min = min; }
    void setMax(double max) { _max = max; }

    void setStrictSizing(bool strict) { _strictSizing = strict; }
    void setPresets(QList<double> presets);
    
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

    QSharedPointer<ISizeIconProvider> _iconProvider;
    QIcon _icon;
    QList<QIcon> _presetIcons;

    InitializeHelper _initHelper;
    PresetMap<> _PresetMap;
};

} // namespace Addle

#endif // SIZESELECTIONPRESENTER_HPP