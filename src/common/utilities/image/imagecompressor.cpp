#include "interfaces/tasks/imagecompressiontasks.hpp"
#include "interfaces/services/itaskservice.hpp"

#include "utilities/qt_extensions/qobject.hpp"

#include "servicelocator.hpp"
#include "imagecompressor.hpp"

#include <QMutexLocker>

void ImageCompressor::uncompress()
{
    // if (!_isCompressed)
    //     return;

    // _uncompressing.lock();

    // IUncompressImagesTask* task = ServiceLocator::make<IUncompressImagesTask>(_imagesData);
    // connect_interface(
    //     task->getController().data(),
    //     SIGNAL(done(ITask*)),
    //     this,
    //     SLOT(onUncompressTaskDone(ITask*)),
    //     Qt::ConnectionType::DirectConnection
    // );

    // ServiceLocator::get<ITaskService>().queueTask(task);
}

QImage ImageCompressor::blockingGetUncompressed()
{
    // QList<QImage> result;

    // QMetaObject::Connection temp_blocking_connection = connect(
    //     this,
    //     &ImageCompressor::uncompressed,
    //     [&] (QList<QImage> result_images) {
    //         result = result_images;
    // });

    // uncompress();

    // const QMutexLocker locker(&_uncompressing);

    // disconnect(temp_blocking_connection);

    // return result;

    return _image;
}

void ImageCompressor::onUncompressTaskDone(ITask* task)
{
    // IUncompressImagesTask* uncompressTask = static_cast<IUncompressImagesTask*>(task);

    // QList<QImage> images = uncompressTask->getImages();

    // emit uncompressed(images);

    // _uncompressing.unlock();
}

void ImageCompressor::compress()
{
    // // assert has images
    // if (_isCompressed || _isCompressing)
    //     return;

    // _isCompressing = true;
    // _compressing.lock();

    // ICompressImagesTask* task = ServiceLocator::make<ICompressImagesTask>(_images);
    // connect_interface(
    //     task->getController().data(),
    //     SIGNAL(done(ITask*)),
    //     this,
    //     SLOT(onCompressTaskDone(ITask*)),
    //     Qt::ConnectionType::DirectConnection
    // );

    // ServiceLocator::get<ITaskService>().queueTask(task);
}

void ImageCompressor::onCompressTaskDone(ITask* task)
{
    // ICompressImagesTask* compressTask = static_cast<ICompressImagesTask*>(task);

    // _imagesData = compressTask->getCompressedImages();
    // _images.clear();

    // emit compressed();
    
    // _compressing.unlock();
    // _isCompressing = false;
}
