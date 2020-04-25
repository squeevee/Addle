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

class ADDLE_CORE_EXPORT Document : public QObject, public IDocument
{
    Q_OBJECT
public:

    Document() : _initHelper(this) {}
    virtual ~Document() = default;

    void initialize();
    void initialize(DocumentBuilder& builder);

    void render(QRect area, QPaintDevice* device);

    bool isEmpty() { _initHelper.check(); return _empty; }

    QSize getSize() { _initHelper.check(); return _size; }

    //void applyOperation(IDrawingOperation& operation);

    QColor getBackgroundColor() { _initHelper.check(); return _backgroundColor; }

    QString getFilename() { _initHelper.check(); return _filename; }
    void setFilename(QString filename) {}

    QList<QSharedPointer<ILayer>> getLayers();
    ILayer* getLayer(int index);
    void addNewLayer(LayerBuilder& builder, int insertBefore = -1);
    void addNewLayers(QList<LayerBuilder> builders, int insertBefore = -1);
    void addNewEmptyLayer(int insertBefore = -1);
    void addNewEmptyLayers(int count, int insertBefore = -1);
    void deleteLayer(int index);
    void deleteLayers(QList<int> indices);
    void reorderLayer(int from, int to);
    int indexOfLayer(ILayer* layer);
    int layerCount();

    QImage exportImage();

signals:
    //void renderChanged(QRect area);
    void boundaryChanging(QRect change);
    void boundaryChanged(QRect change);

    //void applyingDrawingOperation(const IDrawingOperation& operation);
    //void appliedDrawingOperation(const IDrawingOperation& operation);

    void layersAdding(int count);
    void layersAdded(int startIndex, int count);

    void layersDeleting(QList<int> indices);
    void layersDeleted(int count);

    void layerReordering(int from, int to);
    void layerReordered(int from, int to);


private:

    QList<QSharedPointer<ILayer>> _layers;
    QSize _size;

    bool _empty = false; //true;

    QColor _backgroundColor = Qt::GlobalColor::transparent;

    QString _filename;

    void init(DocumentBuilder& builder);
    void updateGeometry();
    void layersChanged(QList<ILayer*> layers);

    QRect unitedBoundary();

protected:
    InitializeHelper<Document> _initHelper;
};

#endif // DOCUMENT_HPP