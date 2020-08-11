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
public: 
    CanvasPresenter() : _initHelper(this) { }
    virtual ~CanvasPresenter() = default;

    void initialize(IMainEditorPresenter* mainEditorPresenter);

    IMainEditorPresenter* mainEditorPresenter() { _initHelper.check(); return _mainEditorPresenter; }

    QCursor cursor();
    QString statusTip() { return QString(); }

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
    InitializeHelper<CanvasPresenter> _initHelper;
};

} // namespace Addle
#endif // CANVASPRESENTER_HPP