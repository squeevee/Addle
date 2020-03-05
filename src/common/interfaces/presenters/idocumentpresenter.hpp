#ifndef IDOCUMENTPRESENTER_HPP
#define IDOCUMENTPRESENTER_HPP

#include <QUrl>
#include <QFileInfo>

#include "iraiseerrorpresenter.hpp"
#include "ihavetoolspresenter.hpp"
#include "ipropertydecoratedpresenter.hpp"

#include "interfaces/models/idocument.hpp"

#include "interfaces/views/icanvasview.hpp"

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

class ILayerPresenter;
class IViewPortPresenter;
class IDocumentView;
class IDocumentPresenter 
    : public virtual IHaveToolsPresenter,
    public virtual IRaiseErrorPresenter,
    public virtual IPropertyDecoratedPresenter
{
public:
    INTERFACE_META(IDocumentPresenter)

    virtual ~IDocumentPresenter() = default;


    virtual IDocumentView* getDocumentView() = 0;

    virtual IViewPortPresenter* getViewPortPresenter() = 0;

    virtual ICanvasView* getCanvasView() = 0;

    virtual bool isEmpty() = 0;
    virtual QSize getCanvasSize() = 0;
    virtual QColor getBackgroundColor() = 0;

    virtual QList<ILayerPresenter*> getLayerPresenters() = 0;

public slots: 
    virtual void onActionLoadDocument(QString filename) = 0;

    virtual void loadDocument(QFileInfo fileInfo) = 0;
    virtual void loadDocument(QUrl url) = 0;

signals:
    virtual void documentChanged(QSharedPointer<IDocument> document) = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IDocumentPresenter,
    DECL_INTERFACE_PROPERTY(currentTool)
    DECL_INTERFACE_PROPERTY(empty)
)

DECL_IMPLEMENTED_AS_QOBJECT(IDocumentPresenter)

#endif // IDOCUMENTPRESENTER_HPP