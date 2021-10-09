/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef CANVASPRESENTER_HPP
#define CANVASPRESENTER_HPP

#include "compat.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "interfaces/presenters/idocumentpresenter.hpp"
#include "utilities/initializehelper.hpp"

#include "interfaces/rendering/irenderer.hpp"

#include <QCursor>
#include <QString>
#include <QObject>

namespace Addle {

class ADDLE_CORE_EXPORT CanvasPresenter : public QObject, public ICanvasPresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::ICanvasPresenter)
    IAMQOBJECT_IMPL
public: 
    CanvasPresenter();
    virtual ~CanvasPresenter() = default;

    QSharedPointer<IDocumentPresenter> document() const override { return _document; }
    
    void setDocument(QSharedPointer<IDocumentPresenter> document) override;
    
//     IRenderer& renderer() const override { return *_renderer; }
//     void initialize(IMainEditorPresenter& mainEditorPresenter);

//     IMainEditorPresenter& mainEditorPresenter() { ASSERT_INIT(); return *_mainEditorPresenter; }
// 
//     QCursor cursor() const;
//     QString statusTip() const { return QString(); }
// 
//     bool event(QEvent* e);
// 
//     bool hasMouse() const override { return _hasMouse; }
//     void setHasMouse(bool value);

// signals:
//     void cursorChanged(QCursor cursor);
//     void statusTipChanged(QString statusTip);
// 
//     void hasMouseChanged(bool);
// 
// private slots:
//     void onEditorToolChanged();
//     void onToolCursorChanged();

// private:
//     bool _hasMouse = false;
// 
//     QMetaObject::Connection _connection_toolCursor;
// 
//     IMainEditorPresenter* _mainEditorPresenter;
//     InitializeHelper _initHelper;

signals:
    void documentChanged(QSharedPointer<IDocumentPresenter>) override;
    
private:
    QSharedPointer<IDocumentPresenter> _document;
};

} // namespace Addle
#endif // CANVASPRESENTER_HPP
