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
#include <QUrl>

#include "compat.hpp"

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"
#include "interfaces/metaobjectinfo.hpp"

// #include "idtypes/toolid.hpp"

#include "tools/itoolpresenter.hpp"

#include "ihaveundostackpresenter.hpp"
#include "interfaces/services/ifactory.hpp"
// #include "interfaces/services/irepository.hpp"

namespace Addle {

class IErrorPresenter;
class IPalettePresenter;
class ILayerPresenter;
class IMainEditorView;
class ICanvasPresenter;
class IViewPortPresenter;
class IColorSelectionPresenter;
class IDocumentPresenter;
class IMessageContext;

class FileRequest;

class IMainEditorPresenter 
    : public IHaveUndoStackPresenter,
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

    virtual IViewPortPresenter& viewPortPresenter() const = 0;
    virtual IColorSelectionPresenter& colorSelection() const = 0;
    virtual IMessageContext& messageContext() const = 0;
    
    virtual QSharedPointer<ICanvasPresenter> canvas() const = 0;
    
    virtual void setMode(Mode mode) = 0;
    virtual Mode mode() const = 0;

    virtual QSharedPointer<IDocumentPresenter> documentPresenter() const = 0;

    virtual bool isEmpty() const = 0;

    virtual QSharedPointer<ILayerPresenter> topSelectedLayer() const = 0;

    virtual QHash<QByteArray, QSharedPointer<IToolPresenter>> tools() const = 0;

    virtual QSharedPointer<IToolPresenter> currentTool() const = 0;
    virtual void setCurrentTool(QSharedPointer<IToolPresenter> tool) = 0;

    virtual QSharedPointer<FileRequest> pendingDocumentFileRequest() const = 0;

public slots:
    virtual void newDocument() = 0;
    virtual void loadDocument(QSharedPointer<FileRequest> request) = 0;

signals:
//     virtual void currentToolChanged(ToolId tool) = 0;

    virtual void topSelectedLayerChanged(QSharedPointer<ILayerPresenter>) = 0;
    virtual void documentPresenterChanged(QSharedPointer<IDocumentPresenter> documentPresenter) = 0;
    virtual void isEmptyChanged(bool) = 0;
};

DECL_INTERFACE_META_PROPERTIES(
    IMainEditorPresenter,
    DECL_INTERFACE_PROPERTY(currentTool)
    DECL_INTERFACE_PROPERTY(empty)
)

ADDLE_DECL_MAKEABLE(IMainEditorPresenter)

namespace aux_IMainEditorPresenter {
    ADDLE_FACTORY_PARAMETER_NAME( mode )
}

ADDLE_DECL_FACTORY_PARAMETERS(
    IMainEditorPresenter,
    (optional 
        (mode,  (IMainEditorPresenter::Mode), IMainEditorPresenter::Mode::Editor)
    )
)

} // namespace Addle


Q_DECLARE_INTERFACE(Addle::IMainEditorPresenter, "org.addle.IMainEditorPresenter")
//Q_DECLARE_METATYPE(Addle::IMainEditorPresenter::Mode)

#endif // IMAINEDITORPRESENTER_HPP
