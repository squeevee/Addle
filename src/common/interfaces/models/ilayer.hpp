/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ILAYER_HPP
#define ILAYER_HPP

#include <QPainter>

#include <QRect>
#include <QPoint>
#include <QImage>

#include "utilities/model/layerbuilder.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

namespace Addle {

class IRasterSurface;
class IDocument;
class ILayer : public virtual IAmQObject
{
public:
    virtual ~ILayer() {}

    virtual void initialize() = 0;
    virtual void initialize(const LayerBuilder& builder) = 0;

    virtual bool isEmpty() const = 0;
    
    virtual QRect boundary() const = 0;
    virtual QPoint topLeft() const = 0;

    virtual QSharedPointer<IRasterSurface> rasterSurface() = 0;

public slots:
    virtual void setTopLeft(QPoint) = 0;

};

ADDLE_DECL_MAKEABLE(ILayer)


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ILayer, "org.addle.ILayer")

#endif // ILAYER_HPP