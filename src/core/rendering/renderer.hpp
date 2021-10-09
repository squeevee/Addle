#pragma once

#include <QObject>
#include "interfaces/rendering/irenderer.hpp"

namespace Addle {

class ADDLE_CORE_EXPORT Renderer : public QObject, public IRenderer
{
    Q_OBJECT
    Q_INTERFACES(Addle::IRenderer)
    IAMQOBJECT_IMPL
public:
    Renderer(Mode mode, const IRenderable* renderable, RenderRoutine routine);
    virtual ~Renderer() = default;
    
    Mode mode() const override { return _mode; }
    
    const IRenderable* renderable() const override { return _renderable; }
    
    RenderRoutine routine() const override { return _routine; }
    
    double scaleHint() const override { return _scaleHint; }
    void setScaleHint(double scale) override;

    int cacheCapacity() const override { return _cacheCapacity; }
    void setCacheCapacity(int capacity) override;
    
    void clearCache() override;
    
    int timeout() const override { return _timeout; }
    void setTimeout(int timeout) override;
    
    void render(QPainter& painter, QRegion region) const override
    {
        render_p_IO(painter, std::move(region));
//         switch(_mode)
//         {
//             case Mode_Live:
//                 render_p_Live(painter, std::move(region));
//                 return;
//                 
//             case Mode_IO:
//                 render_p_IO(painter, std::move(region));
//                 return;
//             
//             default:
//                 Q_UNREACHABLE();
//         }
    }
    
signals:
    void renderChanged(QRegion region) override;

private:
    void render_p_Live(QPainter& painter, QRegion region) const;
    void render_p_IO(QPainter& painter, QRegion region) const;
    
    const Mode _mode;
    
    const IRenderable* _renderable;
    RenderRoutine _routine;
    
    double _scaleHint = 1.0;
    int _cacheCapacity;
    int _timeout;
};
  
}
