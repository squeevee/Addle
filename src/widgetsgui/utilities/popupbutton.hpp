#ifndef POPUPBUTTON_HPP
#define POPUPBUTTON_HPP

#include <QToolButton>
#include <QWidgetAction>

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

private slots:
    void setIcon_slot(QIcon icon) { setIcon(icon); }

private:
    QWidgetAction* _action = nullptr;
    QWidget* _popup = nullptr;
    QMenu* _menu;
};

#endif // POPUPBUTTON_HPP