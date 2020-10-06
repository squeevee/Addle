#include "notificationdialog.hpp"

using namespace Addle;

void NotificationDialog::initialize(QSharedPointer<IMessagePresenter> presenter)
{
    const Initializer init(_initHelper);
    
    _presenter = qobject_interface_cast<INotificationPresenter>(presenter);
    ADDLE_ASSERT(_presenter);
    
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
    default:
        break;
    }
    
    setText(_presenter->text());
#ifdef ADDLE_DEBUG
    if (!_presenter->debugText().isEmpty())
    {
        setStyleSheet(
        "QTextEdit {"
            "font-family: monospace;"
            "min-width: 720px;"
        "}");
        
        setDetailedText(_presenter->debugText());
    }
#endif // ADDLE_DEBUG
}
