#ifndef IDOCUMENTVIEW_HPP
#define IDOCUMENTVIEW_HPP

#include "interfaces/presenters/idocumentpresenter.hpp"

class IDocumentView
{
public:
    virtual ~IDocumentView() = default;
    
    virtual IDocumentPresenter* getDocumentPresenter() = 0;

public slots:
    virtual void start() = 0;
};

#endif // IDOCUMENTVIEW_HPP