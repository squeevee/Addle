#ifndef BRUSHOPERATIONPRESENTER_HPP
#define BRUSHOPERATIONPRESENTER_HPP

#include <QObject>

#include "interfaces/presenters/operationpresenters/ibrushoperationpresenter.hpp"

class BrushOperationPresenter : public QObject, public IBrushOperationPresenter
{
    Q_OBJECT
public: 
    virtual ~BrushOperationPresenter() = default;

    void addPainterData(BrushPainterData data);
    
    QString getText() { return tr("Brush Stroke"); }

    bool isFinalized() { return _isFinalized; }
    void finalize();

    QPainterPath getMask();
    void render(QPainter& painter, QRect rect);

public slots: 
    void do_();
    void undo();

signals: 
    void isFinalizedChanged(bool isFinalized);

private:
    bool _isFinalized = false;
};

#endif // BRUSHOPERATIONPRESENTER_HPP