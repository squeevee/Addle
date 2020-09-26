#include "notificationdialog.hpp"

using namespace Addle;

void NotificationDialog::initialize(QSharedPointer<IMessagePresenter> presenter)
{
    const Initializer init(_initHelper);
    
    ADDLE_ASSERT(qobject_cast<INotificationPresenter*>(qobject_interface_cast(presenter)));
    _presenter = presenter.staticCast<INotificationPresenter>();
    
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    
    switch(_presenter->tone())
    {
    case IMessagePresenter::Issue:
        setIcon(QMessageBox::Warning);
        break;
    case IMessagePresenter::Problem:
        setIcon(QMessageBox::Critical);
        break;
    }
    
    setText(_presenter->text());
#ifdef ADDLE_DEBUG
    if (_presenter->exception())
    {
        setStyleSheet(
        "QTextEdit {"
            "font-family: monospace;"
            "min-width: 720px;"
        "}");
        
        setDetailedText(_presenter->exception()->what());
    }
#endif // ADDLE_DEBUG
}

void NotificationDialog::tlv_show()
{
    ASSERT_INIT();
    QWidget::show();
}

void NotificationDialog::tlv_close()
{
    ASSERT_INIT();
    QWidget::close();
}

