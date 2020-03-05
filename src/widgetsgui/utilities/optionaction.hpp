#ifndef OPTIONACTION_HPP
#define OPTIONACTION_HPP

#include <QAction>

class OptionAction : public QAction
{
    Q_OBJECT
    Q_PROPERTY(
        QVariant value
        READ getValue
    )
public:
    OptionAction(QVariant value, QObject* parent = nullptr)
        : QAction(parent), _value(value)
    {
    }

    QVariant getValue() const { return _value; }

private: 
    const QVariant _value;
};

#endif // OPTIONACTION_HPP