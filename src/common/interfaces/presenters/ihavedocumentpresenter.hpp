#ifndef IHAVEDOCUMENTPRESENTER_HPP
#define IHAVEDOCUMENTPRESENTER_HPP

#include "interfaces/traits.hpp"
#include <QSharedPointer>

class ILayerPresenter;
class IDocumentPresenter;
class IDocumentView;
class IHaveDocumentPresenter
{
public:
    virtual ~IHaveDocumentPresenter() = default;

    virtual IDocumentPresenter* getDocumentPresenter() = 0;

    virtual bool isEmpty() = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

public slots:
    virtual void newDocument() = 0;
    virtual void loadDocument(QUrl url) = 0;

signals:
    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void documentPresenterChanged(IDocumentPresenter* documentPresenter) = 0;
};

DECL_IMPLEMENTED_AS_QOBJECT(IHaveDocumentPresenter);

#endif // IHAVEDOCUMENTPRESENTER_HPP