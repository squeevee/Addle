/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef ICANVASPRESENTER_HPP
#define ICANVASPRESENTER_HPP

#include "interfaces/traits.hpp"
#include "interfaces/iamqobject.hpp"

#include "interfaces/rendering/irenderable.hpp"

namespace Addle {

class IRenderer;
class IDocumentPresenter;
class ICanvasPresenter : public virtual IAmQObject
{
public:
    virtual ~ICanvasPresenter() = default; 

    virtual QSharedPointer<IDocumentPresenter> document() const = 0;
    virtual void setDocument(QSharedPointer<IDocumentPresenter> document) = 0;
signals:
    virtual void documentChanged(QSharedPointer<IDocumentPresenter>) = 0;
    
    //virtual IRenderer& renderer() const = 0;
//     virtual void initialize(IMainEditorPresenter& mainEditorPresenter) = 0;
// 
//     virtual IMainEditorPresenter& mainEditorPresenter() = 0;
// 
//     virtual QCursor cursor() const = 0;
//     virtual QString statusTip() const = 0;
// 
//     virtual bool hasMouse() const = 0;
//     virtual void setHasMouse(bool value) = 0;
// 
// signals:
//     virtual void cursorChanged(QCursor cursor) = 0;
//     virtual void statusTipChanged(QString statusTip) = 0;
// 
//     virtual void hasMouseChanged(bool value) = 0;
};

ADDLE_DECL_MAKEABLE(ICanvasPresenter);


} // namespace Addle

Q_DECLARE_INTERFACE(Addle::ICanvasPresenter, "org.addle.ICanvasPresenter")

#endif // ICANVASPRESENTER_HPP
