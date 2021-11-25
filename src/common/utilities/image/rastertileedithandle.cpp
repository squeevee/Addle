#include "rastertileedithandle.hpp"

using namespace Addle;

RasterTilePaintHandle::iterator::State::State(RasterTile* tile)
{
    assert(tile);
    if (!(*tile).isNull())
    {
        image = (*tile).asImage();
        painter.begin(&image);
        painter.translate(-(*tile).offset());
    }
}

RasterTilePaintHandle::iterator::State::~State()
{
    if (!image.isNull())
    {
        painter.end();
    }
}
