#include "rasteroperation.hpp"
#include <cstring>

#include "utilities/render/renderutils.hpp"

void RasterOperation::initialize(QWeakPointer<IRasterSurface> surface, QImage data, QPoint offset, Mode mode)
{
    _surface = surface;
    _offset = offset;
    _mode = mode;

    auto s_surface = _surface.toStrongRef();

    QByteArray dataBytes(reinterpret_cast<const char*>(data.constBits()), data.sizeInBytes());
    
    _dataRect = QRect(_offset, data.size());

    QImage surfaceCopy(data.size(), QImage::Format_ARGB32_Premultiplied);
    surfaceCopy.fill(Qt::transparent);
    {
        QPainter painter(&surfaceCopy);
        painter.translate(-_offset);
        isolatedRender(*s_surface->getRenderStep(), _dataRect, &painter);
    }
    QByteArray surfaceCopyBytes(reinterpret_cast<const char*>(surfaceCopy.constBits()), surfaceCopy.sizeInBytes());

    QImage merge = surfaceCopy;
    {
        QPainter painter(&merge);
        painter.drawImage(QPoint(), data);
    }
    QByteArray mergeBytes(reinterpret_cast<const char*>(merge.constBits()), merge.sizeInBytes());

    _xor = QByteArray(surfaceCopyBytes.size(), 0x00);
    for (int i = 0; i < surfaceCopyBytes.size(); i++)
    {
        _xor[i] = surfaceCopyBytes[i] ^ mergeBytes[i];
    }
}

void RasterOperation::do_()
{
    if (!_surface) return;
    auto s_surface = _surface.toStrongRef();

    QImage surfaceCopy(_dataRect.size(), QImage::Format_ARGB32_Premultiplied);
    surfaceCopy.fill(Qt::transparent);
    {
        QPainter painter(&surfaceCopy);
        painter.translate(-_offset);
        isolatedRender(*s_surface->getRenderStep(), _dataRect, &painter);
    }
    QByteArray surfaceCopyBytes(reinterpret_cast<const char*>(surfaceCopy.constBits()), surfaceCopy.sizeInBytes());

    QImage unmerge = surfaceCopy;
    {
        uchar* unmergeData = unmerge.bits();

        for (int i = 0; i < _xor.size(); i++)
        {
            unmergeData[i] ^= _xor[i];
        }
    }
    
    auto handle = s_surface->getPaintHandle(QRect(_offset, _dataRect.size()));
    handle.getPainter().setCompositionMode(QPainter::CompositionMode_Source);
    handle.getPainter().translate(_offset);
    handle.getPainter().drawImage(QPoint(), unmerge);
}


void RasterOperation::undo()
{
    if (!_surface) return;
    auto s_surface = _surface.toStrongRef();

    QImage surfaceCopy(_dataRect.size(), QImage::Format_ARGB32_Premultiplied);
    surfaceCopy.fill(Qt::transparent);
    {
        QPainter painter(&surfaceCopy);
        painter.translate(-_offset);
        isolatedRender(*s_surface->getRenderStep(), _dataRect, &painter);
    }
    QByteArray surfaceCopyBytes(reinterpret_cast<const char*>(surfaceCopy.constBits()), surfaceCopy.sizeInBytes());

    QImage unmerge = surfaceCopy;
    {
        uchar* unmergeData = unmerge.bits();

        for (int i = 0; i < _xor.size(); i++)
        {
            unmergeData[i] ^= _xor[i];
        }
    }
    
    auto handle = s_surface->getPaintHandle(QRect(_offset, _dataRect.size()));
    handle.getPainter().setCompositionMode(QPainter::CompositionMode_Source);
    handle.getPainter().translate(_offset);
    handle.getPainter().drawImage(QPoint(), unmerge);
}