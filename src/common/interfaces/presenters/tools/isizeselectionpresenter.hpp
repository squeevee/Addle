#ifndef ISIZESELECTIONPRESENTER_HPP
#define ISIZESELECTIONPRESENTER_HPP

#include <QList>
#include <QIcon>

#include "interfaces/traits.hpp"



class ISizeSelectionPresenter
{
public:
    class IconProvider
    {
    public:
        virtual ~IconProvider() = default;
        virtual QIcon icon(double size) const = 0;
    };

    virtual ~ISizeSelectionPresenter() = default;

    virtual void initialize(QSharedPointer<IconProvider> iconProvider = nullptr) = 0;
    virtual QList<double> presets() const = 0;

    virtual double get() const = 0;

    virtual int selectedPreset() const = 0;

    virtual QIcon icon() const = 0;
    virtual QList<QIcon> presetIcons() const = 0;

    virtual double min() const = 0;
    virtual void setMin(double min) = 0;

    virtual double max() const = 0;
    virtual void setMax(double max) = 0;

    virtual bool strictSizing() const = 0;
    virtual void setStrictSizing(bool strict) = 0;

public slots:
    virtual void setPresets(QList<double> presets) = 0;

    virtual void set(double size) = 0;
    virtual void selectPreset(int index) = 0;

signals:
    virtual void changed(double size) = 0;
    virtual void selectedPresetChanged(int index) = 0;
    virtual void presetsChanged(QList<double> presets) = 0;

    virtual void refreshPreviews() = 0;
};

DECL_EXPECTS_INITIALIZE(ISizeSelectionPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(ISizeSelectionPresenter);
DECL_MAKEABLE(ISizeSelectionPresenter);

#endif // ISIZESELECTIONPRESENTER_HPP