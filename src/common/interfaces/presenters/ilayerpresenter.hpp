/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ILAYERPRESENTER_HPP
#define ILAYERPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "idocumentpresenter.hpp"

#include "interfaces/models/ilayer.hpp"

#include <QWeakPointer>
#include <QPainter>
#include <QRectF>

namespace Addle {

class IRenderStack;
class ILayerPresenter: public virtual IAmQObject
{
public:
    virtual ~ILayerPresenter() = default;
    
    virtual void initialize(IDocumentPresenter* documentPresenter, QSharedPointer<ILayer> layer = nullptr) = 0;
    virtual IDocumentPresenter* documentPresenter() = 0;

    virtual QSharedPointer<ILayer> model() = 0;

    virtual QString name() const = 0;
    virtual void setName(QString name) = 0;

    virtual IRenderStack& renderStack() = 0;

signals: 
    virtual void nameChanged(QString name) = 0;

    virtual void updated(QRect area) = 0;
};

DECL_MAKEABLE(ILayerPresenter)


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ILayerPresenter, "org.addle.ILayerPresenter")

#endif // ILAYERPRESENTER_HPP