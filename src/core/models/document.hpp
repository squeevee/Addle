#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include "compat.hpp"
#include <QObject>
#include <QList>
#include <QString>
#include <QPaintDevice>

#include "interfaces/models/idocument.hpp"
#include "interfaces/models/ilayer.hpp"

#include "utilities/initializehelper.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT Document : public QObject, public IDocument
{
    Q_OBJECT
    Q_INTERFACES(Addle::IDocument)
    IAMQOBJECT_IMPL
public:
    virtual ~Document() = default;

    void initialize();
    void initialize(DocumentBuilder& builder);

    void render(QRect area, QPaintDevice* device) const;

    bool isEmpty() const { _initHelper.check(); return _empty; }

    QSize size() const { _initHelper.check(); return _size; }

    QColor backgroundColor() const { _initHelper.check(); return _backgroundColor; }

    QString filename() const { _initHelper.check(); return _filename; }

    QList<QSharedPointer<ILayer>> layers() const { _initHelper.check(); return _layers; }

    QImage exportImage();

public slots:
    void setFilename(QString filename) {}

signals:
    void boundaryChanged(QRect change);

private:

    QList<QSharedPointer<ILayer>> _layers;
    QSize _size;

    bool _empty = false; //true;

    QColor _backgroundColor = Qt::GlobalColor::transparent;

    QString _filename;

    void layersChanged(QList<ILayer*> layers);

    QRect unitedBoundary();

protected:
    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // DOCUMENT_HPP