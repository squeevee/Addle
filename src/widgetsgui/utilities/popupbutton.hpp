#ifndef POPUPBUTTON_HPP
#define POPUPBUTTON_HPP

#include <QToolButton>
#include <QWidgetAction>

namespace Addle {

class PopupButton : public QToolButton
{
    Q_OBJECT
public: 
    PopupButton(
        QWidget* parent = nullptr
    );
    virtual ~PopupButton() = default;

    void setPopup(QWidget* popup);

public slots:
    void closePopup();

private:
    QWidgetAction* _action = nullptr;
    QWidget* _popup = nullptr;
    QMenu* _menu;
};

} // namespace Addle

#endif // POPUPBUTTON_HPP