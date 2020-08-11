#ifndef OPTIONACTION_HPP
#define OPTIONACTION_HPP

#include <QAction>

namespace Addle {

class OptionAction : public QAction
{
    Q_OBJECT
    Q_PROPERTY(
        QVariant value
        READ value
    )
public:
    OptionAction(QVariant value, QObject* parent = nullptr)
        : QAction(parent), _value(value)
    {
    }

    QVariant value() const { return _value; }

private: 
    const QVariant _value;
};

} // namespace Addle

#endif // OPTIONACTION_HPP