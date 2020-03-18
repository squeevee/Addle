#ifndef CANVASPRESENTER_HPP
#define CANVASPRESENTER_HPP

#include "interfaces/presenters/icanvaspresenter.hpp"
#include "utilities/initializehelper.hpp"

#include <QCursor>
#include <QString>
#include <QObject>

class CanvasPresenter : public QObject, public ICanvasPresenter
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

signals:
    void cursorChanged(QCursor cursor);
    void statusTipChanged(QString statusTip);

private:

    IMainEditorPresenter* _mainEditorPresenter;
    InitializeHelper<CanvasPresenter> _initHelper;
};

#endif // CANVASPRESENTER_HPP