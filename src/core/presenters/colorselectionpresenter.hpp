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

public: 
    virtual ~ColorSelectionPresenter() = default;

    void initialize(QList<QSharedPointer<IPalettePresenter>> palettes);

    QList<QSharedPointer<IPalettePresenter>> palettes() const { _initHelper.check(); return _palettes; }
    void setPalettes(QList<QSharedPointer<IPalettePresenter>> palettes);

    QSharedPointer<IPalettePresenter> palette() const { _initHelper.check(); return _palette; }
    void setPalette(QSharedPointer<IPalettePresenter> palette);

    ColorInfo color(int which) const;
    void setColor(int which, ColorInfo color);

    ColorInfo color1() const { _initHelper.check(); return _color1; }
    void setColor1(ColorInfo color);

    ColorInfo color2() const { _initHelper.check(); return _color2; }
    void setColor2(ColorInfo color);

    ColorInfo activeColor() const { return color(_active); }
    void setActiveColor(ColorInfo color) { setColor(_active, color); }

    int active() const { _initHelper.check(); return _active; }
    void setActive(int active);

    QList<ColorInfo> history() const { _initHelper.check(); return QList<ColorInfo>(); } // TODO
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

    InitializeHelper<ColorSelectionPresenter> _initHelper;
};

} // namespace Addle
#endif // COLORSELECTIONPRESENTER_HPP
