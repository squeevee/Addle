#ifndef GRPAHICSMOUSEEVENTBLOCKER_HPP
#define GRPAHICSMOUSEEVENTBLOCKER_HPP

#include <QObject>

class GraphicsMouseEventBlocker : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // GRPAHICSMOUSEEVENTBLOCKER_HPP