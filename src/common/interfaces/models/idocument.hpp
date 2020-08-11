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

#include "utilities/model/documentbuilder.hpp"

namespace Addle {


class IDocument : public virtual IFormatModel
{
public:
    static const QColor DEFAULT_BACKGROUND_COLOR;

    virtual ~IDocument() {}

    virtual void initialize() = 0;
    virtual void initialize(DocumentBuilder& builder) = 0;

    virtual void render(QRect area, QPaintDevice* device) = 0;

    virtual bool isEmpty() = 0;

    virtual QSize getSize() = 0;

    virtual QColor getBackgroundColor() = 0;

    virtual QString getFilename() = 0;
    virtual void setFilename(QString filename) = 0;

    virtual QList<QSharedPointer<ILayer>> getLayers() = 0;

signals:
    //void renderChanged(QRect area);
    void boundaryChanged(QRect newBoundary);

    //void applyingDrawingOperation(const IDrawingOperation& operation);
    //void appliedDrawingOperation(const IDrawingOperation& operation);
};

DECL_MAKEABLE(IDocument)
DECL_EXPECTS_INITIALIZE(IDocument)
DECL_IMPLEMENTED_AS_QOBJECT(IDocument)
// Q_DECLARE_METATYPE(QSharedPointer<IDocument>)
// Q_DECLARE_METATYPE(QWeakPointer<IDocument>)

} // namespace Addle
#endif // IDOCUMENT_HPP