#ifndef GRPAHICSMOUSEEVENTBLOCKER_HPP
#define GRPAHICSMOUSEEVENTBLOCKER_HPP

#include <QObject>

namespace Addle {

class GraphicsMouseEventBlocker : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
};

} // namespace Addle

#endif // GRPAHICSMOUSEEVENTBLOCKER_HPP