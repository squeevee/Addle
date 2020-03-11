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

    void initialize(IDocumentPresenter* documentPresenter);

    QCursor getCursor() { return QCursor(); }
    QString getStatusTip() { return QString(); }

signals:
    void cursorChanged(QCursor cursor);
    void statusTipChanged(QString statusTip);

private:

    IDocumentPresenter* _documentPresenter;
    InitializeHelper<CanvasPresenter> _initHelper;
};

#endif // CANVASPRESENTER_HPP