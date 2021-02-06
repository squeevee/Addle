#include "notificationdialog.hpp"

using namespace Addle;

NotificationDialog::NotificationDialog(INotificationPresenter& presenter)
    : _presenter(presenter),
    _tlvHelper(this)
{
    //ADDLE_ASSERT(_presenter);
    
    _tlvHelper.onOpened.bind(&NotificationDialog::tlv_opened, this);
    _tlvHelper.onClosed.bind(&NotificationDialog::tlv_closed, this);
    
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);
    
    switch(_presenter.tone())
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
    
    setText(_presenter.text());
#ifdef ADDLE_DEBUG
    if (!_presenter.debugText().isEmpty())
    {
        setStyleSheet(
        "QTextEdit {"
            "font-family: monospace;"
            "min-width: 720px;"
        "}");
        
        setDetailedText(_presenter.debugText());
    }
#endif // ADDLE_DEBUG
}
