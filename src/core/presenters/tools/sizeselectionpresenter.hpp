#ifndef SIZESELECTIONPRESENTER_HPP
#define SIZESELECTIONPRESENTER_HPP

#include "compat.hpp"
#include <QObject>
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

#include "utilities/presethelper2.hpp"

class ADDLE_CORE_EXPORT SizeSelectionPresenter : public QObject, public ISizeSelectionPresenter
{
    Q_OBJECT
public:
    virtual ~SizeSelectionPresenter() = default;

    QList<double> presets() const;

    double get() const;

    int selectedPreset() const;

    void setIcon(QIcon icon);
    QIcon icon() const { return _icon; }

    void setPresetIcons(QList<QIcon> icons);
    QList<QIcon> presetIcons() const { return _presetIcons; }

    double scale() const { return _scale; }
    void setScale(double scale);

public slots:
    void setPresets(QList<double> presets);

    void set(double size);
    void selectPreset(int index);

signals:
    void changed(double size);
    void selectedPresetChanged(int index);
    void presetsChanged(QList<double> presets);

    void iconChanged(QIcon icon);
    void presetIconsChanged(QList<QIcon> icons);

    void scaleChanged(double scale);

private:
    double _scale = 1;
    double _size = qQNaN();
    int _selectedPreset = -1;
    
    QList<double> _presets;
    QMap<double, int> _presetsIndex;

    QIcon _icon;
    QList<QIcon> _presetIcons;

    PresetHelper2<> _presetHelper;
};

#endif // SIZESELECTIONPRESENTER_HPP