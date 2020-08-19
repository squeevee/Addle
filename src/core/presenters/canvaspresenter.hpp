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
#include "utilities/initializehelper.hpp"

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
    virtual ~CanvasPresenter() = default;

    void initialize(IMainEditorPresenter& mainEditorPresenter);

    IMainEditorPresenter& mainEditorPresenter() { _initHelper.check(); return *_mainEditorPresenter; }

    QCursor cursor() const;
    QString statusTip() const { return QString(); }

    bool event(QEvent* e);

    bool hasMouse() const override { return _hasMouse; }
    void setHasMouse(bool value);

signals:
    void cursorChanged(QCursor cursor);
    void statusTipChanged(QString statusTip);

    void hasMouseChanged(bool);

private slots:
    void onEditorToolChanged();
    void onToolCursorChanged();

private:
    bool _hasMouse = false;

    QMetaObject::Connection _connection_toolCursor;

    IMainEditorPresenter* _mainEditorPresenter;
    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // CANVASPRESENTER_HPP