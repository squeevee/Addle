#ifndef IDOCUMENT_HPP
#define IDOCUMENT_HPP

#include <QObject>
#include <QImage>
#include <QColor>
#include <QString>
#include <QPaintDevice>
#include <QSharedPointer>

#include "ilayer.hpp"
#include "common/interfaces/format/iformatmodel.hpp"
#include "common/interfaces/servicelocator/imakeable.hpp"

#include "common/utilities/data/documentbuilder.hpp"
#include "common/interfaces/traits/initialize_traits.hpp"
#include "common/interfaces/traits/qobject_trait.hpp"

class IDocument : public virtual IFormatModel, public virtual IMakeable
{
public:

    enum InitEmptyOptions
    {
        nothing,
        useDefaults
    };

    virtual ~IDocument() {}

    virtual void initialize(InitEmptyOptions emptyOption = InitEmptyOptions::useDefaults) = 0;
    virtual void initialize(DocumentBuilder& builder) = 0;

    virtual void render(QRect area, QPaintDevice* device) = 0;

    virtual bool isEmpty() = 0;

    virtual QSize getSize() = 0;

    virtual QColor getBackgroundColor() = 0;

    virtual QString getFilename() = 0;
    virtual void setFilename(QString filename) = 0;

signals:
    void renderChanged(QRect area);
    void boundaryChanged(QRect newBoundary);

    //void applyingDrawingOperation(const IDrawingOperation& operation);
    //void appliedDrawingOperation(const IDrawingOperation& operation);
};


DECL_EXPECTS_INITIALIZE(IDocument)
DECL_IMPLEMENTED_AS_QOBJECT(IDocument)
//DECL_INIT_DEPENDENCY(IDocument, ILayer)

Q_DECLARE_METATYPE(QSharedPointer<IDocument>)

#endif // IDOCUMENT_HPP