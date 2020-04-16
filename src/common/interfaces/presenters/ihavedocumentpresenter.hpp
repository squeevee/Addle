#ifndef IHAVEDOCUMENTPRESENTER_HPP
#define IHAVEDOCUMENTPRESENTER_HPP

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/compat.hpp"

class IDocumentPresenter;
class IDocumentView;
class ADDLE_COMMON_EXPORT IHaveDocumentPresenter
{
public:
    virtual ~IHaveDocumentPresenter() = default;

    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    virtual bool isEmpty() = 0;

public slots:
    virtual void newDocument() = 0;
    virtual void loadDocument(QUrl url) = 0;

signals:
    virtual void documentPresenterChanged(IDocumentPresenter* documentPresenter) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveDocumentPresenter);

#endif // IHAVEDOCUMENTPRESENTER_HPP