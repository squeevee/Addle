#ifndef ICOLORSELECTIONPRESENTER_HPP
#define ICOLORSELECTIONPRESENTER_HPP

#include <QColor>
#include <QSharedPointer>

#include "utilities/model/colorinfo.hpp"

#include "interfaces/traits.hpp"


#include "interfaces/metaobjectinfo.hpp"
namespace Addle {

class IPalettePresenter;
class IColorSelectionPresenter
{
public:
    INTERFACE_META(IColorSelectionPresenter);

    virtual ~IColorSelectionPresenter() = default;

    virtual void initialize(QList<QSharedPointer<IPalettePresenter>> palettes) = 0;

    virtual QList<QSharedPointer<IPalettePresenter>> palettes() const = 0;
    virtual void setPalettes(QList<QSharedPointer<IPalettePresenter>> palettes) = 0;

    virtual QSharedPointer<IPalettePresenter> palette() const = 0;
    virtual void setPalette(QSharedPointer<IPalettePresenter> palette) = 0;

    virtual ColorInfo color(int which) const = 0;
    virtual void setColor(int which, ColorInfo color) = 0;
    //virtual QHash<int, ColorInfo> colors() const = 0;

    virtual ColorInfo color1() const = 0;
    virtual void setColor1(ColorInfo color) = 0;

    virtual ColorInfo color2() const = 0;
    virtual void setColor2(ColorInfo color) = 0;

    virtual ColorInfo activeColor() const = 0;
    virtual void setActiveColor(ColorInfo color) = 0;

    virtual int active() const = 0; // may be 1 or 2
    virtual void setActive(int active) = 0;

    virtual QList<ColorInfo> history() const = 0;

signals:
    virtual void palettesChanged(QList<QSharedPointer<IPalettePresenter>>) = 0;
    virtual void paletteChanged(QSharedPointer<IPalettePresenter>) = 0;

    virtual void colorChanged(int which, ColorInfo color) = 0;
    virtual void color1Changed(ColorInfo color) = 0;
    virtual void color2Changed(ColorInfo color) = 0;

    virtual void historyChanged(QList<ColorInfo>) = 0;
    
    virtual void activeChanged(int active) = 0;
};

DECL_MAKEABLE(IColorSelectionPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IColorSelectionPresenter);
DECL_EXPECTS_INITIALIZE(IColorSelectionPresenter);

DECL_INTERFACE_META_PROPERTIES(IColorSelectionPresenter,
    DECL_INTERFACE_PROPERTY(color1)
    DECL_INTERFACE_PROPERTY(color2)
    DECL_INTERFACE_PROPERTY(active)
);

} // namespace Addle
#endif // ICOLORSELECTIONPRESENTER_HPP