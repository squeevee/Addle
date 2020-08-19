/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
