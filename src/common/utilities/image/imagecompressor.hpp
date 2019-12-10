#ifndef IMAGECOMPRESSOR_HPP
#define IMAGECOMPRESSOR_HPP

#include <QObject>
#include <QImage>
#include <QMutex>

#include "interfaces/tasks/itask.hpp"

/**
 * A holder for images that can manage asynchronously compressing them. Multiple
 * images can be compressed/decompressed together, for example an RGBA image and
 * a corresponding 1bbp alpha mask.
 */
class ImageCompressor: public QObject
{
    Q_OBJECT
public:
    ImageCompressor(QImage image = QImage())
        : _image(image),
        _compressing(QMutex::RecursionMode::Recursive),
        _uncompressing(QMutex::RecursionMode::Recursive)
    {
    }

    virtual ~ImageCompressor() = default;

    void setImage(QImage image) { /* assert not compressed */ _image = image; }

    bool isCompressing() { return _isCompressing; }
    bool isCompressed() { return _isCompressed; }

    void uncompress();
    QImage blockingGetUncompressed();

    void compress();

signals:
    void uncompressed(QImage images);
    void compressed();

private slots: 
    void onCompressTaskDone(ITask* task);
    void onUncompressTaskDone(ITask* task);

private:

    QImage _image;
    QByteArray _imageData;

    QMutex _compressing;
    QMutex _uncompressing;

    bool _isCompressing = false;
    bool _isCompressed = false;
};

#endif // IMAGECOMPRESSOR_HPP