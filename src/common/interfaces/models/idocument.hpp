/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IDOCUMENT_HPP
#define IDOCUMENT_HPP

#include <QSize>
#include <QColor>
#include <QString>
#include <QSharedPointer>
#include <QUrl>

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "utilities/model/documentbuilder.hpp"

namespace Addle {

class ILayer;
class IDocument : public virtual IAmQObject
{
public:
    static const QColor DEFAULT_BACKGROUND_COLOR; // ?

    virtual ~IDocument() {}

    virtual void initialize() = 0;
    virtual void initialize(const DocumentBuilder& builder) = 0;

    virtual void render(QRect area, QPaintDevice* device) const = 0;

    virtual bool isEmpty() const = 0;
    virtual QSize size() const = 0;
    virtual QColor backgroundColor() const = 0;

    virtual QUrl url() const = 0;
    virtual void setUrl(QUrl url) = 0;
    
    virtual QList<QSharedPointer<ILayer>> layers() const = 0;

signals:
    void boundaryChanged(QRect newBoundary);

};

DECL_MAKEABLE(IDocument)


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IDocument, "org.addle.IDocument")

#endif // IDOCUMENT_HPP