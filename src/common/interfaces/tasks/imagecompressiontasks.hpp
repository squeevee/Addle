#ifndef ICOMPRESSIMAGETASK_HPP
#define ICOMPRESSIMAGETASK_HPP

#include <QImage>
#include <QByteArray>

#include "itask.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/initialize_traits.hpp"

class ICompressImagesTask : public ITask
{
public:
    virtual ~ICompressImagesTask() = default;

    virtual void initialize(QList<QImage> images) = 0;

    virtual QList<QByteArray> getCompressedImages() = 0;
};

DECL_EXPECTS_INITIALIZE(ICompressImagesTask)
DECL_MAKEABLE(ICompressImagesTask)

class IUncompressImagesTask : public ITask
{
public:
    virtual ~IUncompressImagesTask() = default;

    virtual void initialize(QList<QByteArray> imagesData) = 0;

    virtual QList<QImage> getImages() = 0;
};

DECL_EXPECTS_INITIALIZE(IUncompressImagesTask)
DECL_MAKEABLE(IUncompressImagesTask)

#endif // ICOMPRESSIMAGETASK_HPP