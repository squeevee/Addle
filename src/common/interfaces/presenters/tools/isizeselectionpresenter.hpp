/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ISIZESELECTIONPRESENTER_HPP
#define ISIZESELECTIONPRESENTER_HPP

#include <QList>
#include <QIcon>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {


class ISizeSelectionPresenter : public virtual IAmQObject
{
public:
    class ISizeIconProvider
    {
    public:
        virtual ~ISizeIconProvider() = default;
        virtual QIcon icon(double size) const = 0;
    };

    virtual ~ISizeSelectionPresenter() = default;

    virtual void initialize(QSharedPointer<ISizeIconProvider> ISizeIconProvider = nullptr) = 0;
    virtual QList<double> presets() const = 0;

    virtual double get() const = 0;

    virtual int selectedPreset() const = 0;

    virtual QIcon icon() const = 0;
    virtual QList<QIcon> presetIcons() const = 0;

    virtual double min() const = 0;
    virtual double max() const = 0;

    virtual bool strictSizing() const = 0;

public slots:
    virtual void set(double size) = 0;
    virtual void selectPreset(int index) = 0;

    virtual void setMin(double min) = 0;
    virtual void setMax(double max) = 0;
    
    virtual void setStrictSizing(bool strict) = 0;
    virtual void setPresets(QList<double> presets) = 0;

signals:
    virtual void changed(double size) = 0;
    virtual void selectedPresetChanged(int index) = 0;
    virtual void presetsChanged(QList<double> presets) = 0;

    virtual void refreshPreviews() = 0;
};

ADDLE_DECL_MAKEABLE(ISizeSelectionPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ISizeSelectionPresenter, "org.addle.ISizeSelectionPresenter")

#endif // ISIZESELECTIONPRESENTER_HPP