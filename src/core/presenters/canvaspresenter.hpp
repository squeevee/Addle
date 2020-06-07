#ifndef CANVASPRESENTER_HPP
#define CANVASPRESENTER_HPP

#include "compat.hpp"
#include "interfaces/presenters/icanvaspresenter.hpp"
#include "utilities/initializehelper.hpp"

#include <QCursor>
#include <QString>
#include <QObject>

class ADDLE_CORE_EXPORT CanvasPresenter : public QObject, public ICanvasPresenter
{
    Q_OBJECT
public: 
    CanvasPresenter() : _initHelper(this) { }
    virtual ~CanvasPresenter() = default;

    void initialize(IMainEditorPresenter* mainEditorPresenter);

    IMainEditorPresenter* getMainEditorPresenter() { _initHelper.check(); return _mainEditorPresenter; }

    QCursor getCursor() { return QCursor(); }
    QString getStatusTip() { return QString(); }

    bool event(QEvent* e);

    bool hasMouse() const override { return _hasMouse; }
    void setHasMouse(bool value);

signals:
    void cursorChanged(QCursor cursor);
    void statusTipChanged(QString statusTip);

    void hasMouseChanged(bool);

private:
    bool _hasMouse = false;

    IMainEditorPresenter* _mainEditorPresenter;
    InitializeHelper<CanvasPresenter> _initHelper;
};

#endif // CANVASPRESENTER_HPP