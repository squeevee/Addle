#pragma once

#include <QString>
#include <QPainter>

namespace Addle {

template<class Derived>
class LayerNodeHelperBase
{
protected:
    LayerNodeHelperBase() = default;
    
    // call after _model is set
    inline void initialize()
    {
        assert(static_cast<Derived*>(this)->_model);
        isVisible = static_cast<Derived*>(this)
            ->_model->opacity() >= 0.001;
    }
    
    inline QString name_impl() const 
    { 
        return static_cast<const Derived*>(this)->_model->name(); 
    }
    
    inline void setName_impl(QString name)
    {
        if (static_cast<Derived*>(this)->_model->name() != name)
        {
            static_cast<Derived*>(this)->_model->setName(name);
            emit static_cast<Derived*>(this)->nameChanged(name);
        }
    }
    
    inline QPainter::CompositionMode compositionMode_impl() const
    { 
        return static_cast<const Derived*>(this)->_model->compositionMode(); 
    }
    
    inline void setCompositionMode_impl(QPainter::CompositionMode mode)
    {
        if (static_cast<Derived*>(this)->_model->compositionMode() != mode)
        {
            static_cast<Derived*>(this)->_model->setCompositionMode(mode);
            emit static_cast<Derived*>(this)->compositionModeChanged(mode);
        }
    }
    
    inline double opacity_impl() const
    { 
        return static_cast<const Derived*>(this)->_model->opacity(); 
    }
    
    inline void setOpacity_impl(double opacity)
    {
        opacity = qBound(0.000, opacity, 1.000);
        if (static_cast<Derived*>(this)->_model->opacity() != opacity)
        {
            static_cast<Derived*>(this)->_model->setOpacity(opacity);
            heldOpacity = qQNaN();
            
            bool wasVisibile = isVisible;
            isVisible = opacity >= 0.001;
            
            emit static_cast<Derived*>(this)->opacityChanged(opacity);
            if (wasVisibile != isVisible)
                emit static_cast<Derived*>(this)->visibilityChanged(isVisible);
        }
    }
    
    bool isVisible;
    inline void setVisibility_impl(bool isVisible_)
    {
        if (isVisible != isVisible_)
        {
            isVisible = isVisible_;
            
            if (isVisible)
            {
                if (!qIsNaN(heldOpacity) && heldOpacity >= 0.001)
                {
                    static_cast<Derived*>(this)->_model->setOpacity(heldOpacity);
                }
                else
                {
                    static_cast<Derived*>(this)->_model->setOpacity(1.000);
                    heldOpacity = 1.000;
                }
            }
            else
            {
                heldOpacity = static_cast<Derived*>(this)->_model->opacity();
                static_cast<Derived*>(this)->_model->setOpacity(0.000);
            }
            
            emit static_cast<Derived*>(this)->visibilityChanged(isVisible);
        }
    }
    
    double heldOpacity = qQNaN();
};

}
