#ifndef IDOCUMENTPRESENTER_HPP
#define IDOCUMENTPRESENTER_HPP

#include <QUrl>
#include <QFileInfo>

#include "iraiseerrorpresenter.hpp"
#include "ihavetoolspresenter.hpp"

#include "common/interfaces/models/idocument.hpp"

#include "common/interfaces/traits/qobject_trait.hpp"

class ICanvasPresenter;
class IViewPortPresenter;
class IDocumentView;
class IDocumentPresenter: public virtual IHaveToolsPresenter, public virtual IRaiseErrorPresenter
{
public:
    virtual ~IDocumentPresenter() = default;


    virtual IDocumentView* getDocumentView() = 0;

    virtual ICanvasPresenter* getCanvasPresenter() = 0;
    virtual IViewPortPresenter* getViewPortPresenter() = 0;

public slots: 
    virtual void onActionLoadDocument(QString filename) = 0;

    virtual void loadDocument(QFileInfo fileInfo) = 0;
    virtual void loadDocument(QUrl url) = 0;

signals:
    virtual void documentChanged(QSharedPointer<IDocument> document) = 0;

};

DECL_IMPLEMENTED_AS_QOBJECT(IDocumentPresenter)

#endif // IDOCUMENTPRESENTER_HPP