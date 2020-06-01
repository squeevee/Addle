#ifndef ISIZESELECTIONPRESENTER_HPP
#define ISIZESELECTIONPRESENTER_HPP

#include <QList>
#include <QIcon>

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"

class ISizeSelectionPresenter
{
public:
    virtual ~ISizeSelectionPresenter() = default;

    virtual QList<double> presets() const = 0;

    virtual double get() const = 0;

    virtual int selectedPreset() const = 0;

    virtual void setIcon(QIcon icon) = 0;
    virtual QIcon icon() const = 0;

    virtual void setPresetIcons(QList<QIcon> icons) = 0;
    virtual QList<QIcon> presetIcons() const = 0;

public slots:
    virtual void setPresets(QList<double> presets) = 0;

    virtual void set(double size) = 0;
    virtual void selectPreset(int index) = 0;

signals:
    virtual void changed(double size) = 0;
    virtual void selectedPresetChanged(int index) = 0;
    virtual void presetsChanged(QList<double> presets) = 0;

    virtual void iconChanged(QIcon icon) = 0;
    virtual void presetIconsChanged(QList<QIcon> icons) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(ISizeSelectionPresenter);
DECL_MAKEABLE(ISizeSelectionPresenter);

#endif // ISIZESELECTIONPRESENTER_HPP