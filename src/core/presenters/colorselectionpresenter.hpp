/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef COLORSELECTIONPRESENTER_HPP
#define COLORSELECTIONPRESENTER_HPP

#include "interfaces/presenters/icolorselectionpresenter.hpp"
#include "interfaces/presenters/ipalettepresenter.hpp"
#include <QObject>

#include "compat.hpp"

#include "utilities/initializehelper.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT ColorSelectionPresenter : public QObject, public IColorSelectionPresenter
{
    Q_OBJECT
    Q_PROPERTY(
        Addle::ColorInfo color1
        READ color1
        WRITE setColor1
        NOTIFY color1Changed
    )
    Q_PROPERTY(
        Addle::ColorInfo color2
        READ color2
        WRITE setColor2
        NOTIFY color2Changed
    )
    Q_INTERFACES(Addle::IColorSelectionPresenter)
    IAMQOBJECT_IMPL

public: 
    virtual ~ColorSelectionPresenter() = default;

    void initialize(QList<QSharedPointer<IPalettePresenter>> palettes);
    
    QList<QSharedPointer<IPalettePresenter>> palettes() const { ASSERT_INIT; return _palettes; }
    QSharedPointer<IPalettePresenter> palette() const { ASSERT_INIT; return _palette; }
    ColorInfo color(int which) const;
    ColorInfo color1() const { ASSERT_INIT; return _color1; }
    ColorInfo color2() const { ASSERT_INIT; return _color2; }
    ColorInfo activeColor() const { return color(_active); }
    int active() const { ASSERT_INIT; return _active; }
    
    QList<ColorInfo> history() const { ASSERT_INIT; return QList<ColorInfo>(); } // TODO

public slots:
    void setPalettes(QList<QSharedPointer<IPalettePresenter>> palettes);
    void setPalette(QSharedPointer<IPalettePresenter> palette);
    void setColor(int which, ColorInfo color);
    void setColor1(ColorInfo color);
    void setColor2(ColorInfo color);
    void setActiveColor(ColorInfo color) { setColor(_active, color); }
    void setActive(int active);

signals:
    void palettesChanged(QList<QSharedPointer<IPalettePresenter>>);
    void paletteChanged(QSharedPointer<IPalettePresenter>);

    void colorChanged(int which, ColorInfo color);
    void color1Changed(ColorInfo color);
    void color2Changed(ColorInfo color);

    void historyChanged(QList<ColorInfo>);

    void activeChanged(int active);
private:
    ColorInfo _color1;
    ColorInfo _color2;

    int _active = 1;

    QList<QSharedPointer<IPalettePresenter>> _palettes;
    QSharedPointer<IPalettePresenter> _palette;

    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // COLORSELECTIONPRESENTER_HPP
