#ifndef IBRUSHLIKEOPTIONSPRESENTER_HPP
#define IBRUSHLIKEOPTIONSPRESENTER_HPP

#include <QMetaType>

#include "itooloptionspresenter.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

class IBrushLikeOptionsPresenter : public virtual IToolOptionsPresenter
{
public:
    enum ShapeOption
    {
        square,
        circle
    };

    enum SizeOption
    {
        _1px,
        _2px,
        _7px,
        _13px,
        _25px,
        _50px,
        _100px,
        _250px,
        _500px,
        _10percent,
        _25percent,
        _33percent,
        _50percent,
        custom_px,
        custom_percent
    };

    enum EdgeOption
    {
        hard,
        medium,
        soft
    };

    virtual ShapeOption getShape() = 0;

    virtual SizeOption getSize() = 0;
    virtual double getCustomPixelSize() = 0;
    virtual double getCustomPercentSize() = 0;

    virtual EdgeOption getEdge() = 0;


public slots:
    virtual void setShape(ShapeOption shape) = 0;
    virtual void setSize(SizeOption size) = 0;
    virtual void setCustomPixelSize(double customPixels) = 0;
    virtual void setCustomPercentSize(double customPercentage) = 0;
    virtual void setEdge(EdgeOption edge) = 0;

signals:
    virtual void ShapeChanged(ShapeOption shape) = 0;
    virtual void SizeChanged(SizeOption size) = 0;
    virtual void EdgeChanged(EdgeOption edge) = 0;

};

Q_DECLARE_METATYPE(IBrushLikeOptionsPresenter::ShapeOption)
Q_DECLARE_METATYPE(IBrushLikeOptionsPresenter::SizeOption)
Q_DECLARE_METATYPE(IBrushLikeOptionsPresenter::EdgeOption)

DECL_IMPLEMENTED_AS_QOBJECT(IBrushLikeOptionsPresenter)

#endif // IBRUSHLIKEOPTIONSPRESENTER_HPP