#include "renderer.hpp"

using namespace Addle;

Renderer::Renderer(Mode mode, const IRenderable* renderable, RenderRoutine routine)
    : _mode(mode), 
    _renderable(renderable),
    _routine(routine)
{
#ifdef ADDLE_DEBUG
    if (Q_UNLIKELY(_renderable && !routine.isNull()))
    {
        qWarning("%s",
            //% "Renderer was created with a non-null renderable and a non-null "
            //% "render routine. The routine given will be overwritten with the "
            //% "routine of the renderable."
            qUtf8Printable(qtTrId("debug-messages.renderer.nonnull-renderable-and-routine"))
        );
    }
    else if (Q_UNLIKELY(!_renderable && routine.isEmpty()))
    {
        qWarning("%s",
            //% "Renderer was created with a null renderable and an empty render "
            //% "routine. The renderer will be unable to do anything useful."
            qUtf8Printable(qtTrId("debug-messages.renderer.null-renderable-and-routine"))
        );
    }
#endif

    if (_renderable) _routine = _renderable->renderRoutine();
    
    switch(_mode)
    {
        case Mode_Live:
            _cacheCapacity = 100;
            _timeout = 0;
            break;
            
        case Mode_IO:
            _cacheCapacity = 0;
            _timeout = -1;
            break;
        
        default:
            Q_UNREACHABLE();
    }
}

void Renderer::setScaleHint(double scale) 
{
}

void Renderer::setCacheCapacity(int capacity) 
{
}

void Renderer::clearCache() 
{
}

void Renderer::setTimeout(int timeout) 
{
}

void Renderer::render_p_Live(QPainter& painter, QRegion region) const
{
}

void Renderer::render_p_IO(QPainter& painter, QRegion region) const
{
    
}
