#ifndef IDOCUMENT_HPP
#define IDOCUMENT_HPP

#include <QObject>
#include <QImage>
#include <QColor>
#include <QString>
#include <QPaintDevice>
#include <QSharedPointer>

#include "ilayer.hpp"
#include "interfaces/format/iformatmodel.hpp"
#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "utilities/model/documentbuilder.hpp"

namespace Addle {


class IDocument : public virtual IFormatModel, public virtual IAmQObject
{
public:
    static const QColor DEFAULT_BACKGROUND_COLOR; // ?

    virtual ~IDocument() {}

    virtual void initialize() = 0;
    virtual void initialize(DocumentBuilder& builder) = 0;

    virtual void render(QRect area, QPaintDevice* device) const = 0;

    virtual bool isEmpty() const = 0;
    virtual QSize size() const = 0;
    virtual QColor backgroundColor() const = 0;

    virtual QString filename() const = 0;
    
    virtual QList<QSharedPointer<ILayer>> layers() const = 0;

public slots:
    virtual void setFilename(QString filename) = 0;

signals:
    void boundaryChanged(QRect newBoundary);

};

DECL_MAKEABLE(IDocument)
DECL_EXPECTS_INITIALIZE(IDocument)

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IDocument, "org.addle.IDocument")

#endif // IDOCUMENT_HPP