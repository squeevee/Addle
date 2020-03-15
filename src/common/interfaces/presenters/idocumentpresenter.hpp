#ifndef IDOCUMENTPRESENTER_HPP
#define IDOCUMENTPRESENTER_HPP

#include <QUrl>
#include <QFileInfo>
#include <QWeakPointer>

#include "iraiseerrorpresenter.hpp"
#include "ihavetoolspresenter.hpp"
#include "ipropertydecoratedpresenter.hpp"

#include "interfaces/models/idocument.hpp"

#include "interfaces/traits/qobject_trait.hpp"
#include "interfaces/traits/initialize_trait.hpp"
#include "interfaces/traits/makeable_trait.hpp"
#include "interfaces/traits/metaobjectinfo.hpp"

class ICanvasPresenter;
class ILayerPresenter;
class IDocumentPresenter
{
public:
    enum EmptyInitOptions
    {
        initNoModel,
        initEmptyModel,
        initBlankDefaults
    };

    virtual ~IDocumentPresenter() = default;

    virtual void initialize(EmptyInitOptions option = initNoModel) = 0;
    virtual void initialize(QSize size, QColor backgroundColor) = 0;
    virtual void initialize(QWeakPointer<IDocument> model) = 0;

    virtual QWeakPointer<IDocument> getModel() = 0;

    virtual bool isEmpty() = 0;
    
    virtual QSize getSize() = 0;
    virtual QColor getBackgroundColor() = 0;

    virtual QList<ILayerPresenter*> getLayers() = 0;
};
DECL_MAKEABLE(IDocumentPresenter);
DECL_EXPECTS_INITIALIZE(IDocumentPresenter);
DECL_IMPLEMENTED_AS_QOBJECT(IDocumentPresenter);

#endif // IDOCUMENTPRESENTER_HPP