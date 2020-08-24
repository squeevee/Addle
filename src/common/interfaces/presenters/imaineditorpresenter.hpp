/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IMAINEDITORPRESENTER_HPP
#define IMAINEDITORPRESENTER_HPP

#include <QObject>
#include <QHash>

#include "compat.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "interfaces/metaobjectinfo.hpp"

#include "idtypes/toolid.hpp"

#include "ihaveundostackpresenter.hpp"
#include "iraiseerrorpresenter.hpp"

namespace Addle {

class IPalettePresenter;
class ILayerPresenter;
class IMainEditorView;
class ICanvasPresenter;
class IViewPortPresenter;
class IColorSelectionPresenter;
class IToolPresenter;
class IDocumentPresenter;

class IMainEditorPresenter 
    : public IHaveUndoStackPresenter,
    public IRaiseErrorPresenter,
    public virtual IAmQObject
{
public:
    INTERFACE_META(IMainEditorPresenter);
    
    virtual ~IMainEditorPresenter() = default;
    
    enum Mode
    {
        Editor,
        Viewer
    };

    virtual void initialize(Mode mode) = 0;

    virtual IMainEditorView& view() const = 0;

    virtual ICanvasPresenter& canvasPresenter() const = 0;
    virtual IViewPortPresenter& viewPortPresenter() const = 0;
    virtual IColorSelectionPresenter& colorSelection() const = 0;

    virtual void setMode(Mode mode) = 0;
    virtual Mode mode() const = 0;

    virtual QSharedPointer<IDocumentPresenter> documentPresenter() const = 0;

    virtual bool isEmpty() const = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

    virtual ToolId currentTool() const = 0;
    virtual void setCurrentTool(ToolId tool) = 0;

    virtual QHash<ToolId, QSharedPointer<IToolPresenter>> tools() const = 0;

    virtual QSharedPointer<IToolPresenter> currentToolPresenter() const = 0;

    virtual void newDocument() = 0;
    virtual void loadDocument(QUrl url) = 0;

signals:

    virtual void currentToolChanged(ToolId tool) = 0;

    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void documentPresenterChanged(QSharedPointer<IDocumentPresenter> documentPresenter) = 0;
    virtual void isEmptyChanged(bool) = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IMainEditorPresenter,
    DECL_INTERFACE_PROPERTY(currentTool)
    DECL_INTERFACE_PROPERTY(empty)
)

DECL_MAKEABLE(IMainEditorPresenter)
DECL_EXPECTS_INITIALIZE(IMainEditorPresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IMainEditorPresenter, "org.addle.IMainEditorPresenter")

#endif // IMAINEDITORPRESENTER_HPP