#ifndef NOTIFICATIONDIALOG_HPP
#define NOTIFICATIONDIALOG_HPP

#include <QMessageBox>
#include "interfaces/views/imessageview.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"

#include "utilities/initializehelper.hpp"

namespace Addle {
    
class NotificationDialog : public QMessageBox, public IMessageView
{
    Q_OBJECT
    Q_INTERFACES(Addle::IMessageView Addle::ITopLevelView)
    IAMQOBJECT_IMPL
public:
    virtual ~NotificationDialog() = default;
    
    void initialize(QSharedPointer<IMessagePresenter> presenter);
    
    QSharedPointer<IMessagePresenter> presenter() const override
    {
        ASSERT_INIT();
        return _presenter;
    }
    
public slots:
    void tlv_show() override;
    void tlv_close() override;

signals:
    void tlv_shown() override;
    void tlv_closed() override;
    
private:
    QSharedPointer<INotificationPresenter> _presenter;
        
    InitializeHelper _initHelper;
};
    
} // namespace Addle

#endif // NOTIFICATIONDIALOG_HPP
