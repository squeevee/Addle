#ifndef IRASTERPREVIEW_HPP
#define IRASTERPREVIEW_HPP

#include "interfaces/traits/qobject_trait.hpp"

class IRasterPreview
{
public: 
    virtual ~IRasterPreview() = default;

signals: 
    virtual void changed() = 0;
};

#endif // IRASTERPREVIEW_HPP