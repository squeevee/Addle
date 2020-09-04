/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
    void initialize(const DocumentBuilder& builder);

    void render(QRect area, QPaintDevice* device) const;

    bool isEmpty() const { ASSERT_INIT(); return _empty; }

    QSize size() const { ASSERT_INIT(); return _size; }

    QColor backgroundColor() const { ASSERT_INIT(); return _backgroundColor; }

    QString filename() const { ASSERT_INIT(); return _filename; }

    QList<QSharedPointer<ILayer>> layers() const { ASSERT_INIT(); return _layers; }

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