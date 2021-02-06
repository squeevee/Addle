#ifndef NOTIFICATIONDIALOG_HPP
#define NOTIFICATIONDIALOG_HPP

#include <QMessageBox>
#include "interfaces/views/imessageview.hpp"
#include "interfaces/views/itoplevelview.hpp"
#include "interfaces/presenters/messages/inotificationpresenter.hpp"

#include "utilities/initializehelper.hpp"

#include "../helpers/toplevelviewhelper.hpp"

#include "utils.hpp"

namespace Addle {
    
class NotificationDialog : public QMessageBox, public IMessageView, public ITopLevelView
{
    Q_OBJECT
    Q_INTERFACES(Addle::IMessageView Addle::ITopLevelView)
    IAMQOBJECT_IMPL
public:
    NotificationDialog(INotificationPresenter& presenter);
    virtual ~NotificationDialog() = default;
    
    //void initialize(QSharedPointer<IMessagePresenter> presenter) override;
    
    IMessagePresenter& presenter() const override
    {
        ASSERT_INIT();
        return _presenter;
    }
    
public slots:
    void tlv_open() override { try { ASSERT_INIT(); _tlvHelper.open(); } ADDLE_SLOT_CATCH }
    void tlv_close() override { try { ASSERT_INIT(); _tlvHelper.close(); } ADDLE_SLOT_CATCH }

signals:
    void tlv_opened() override;
    void tlv_closed() override;
    
private:
    void setupUi();
    
    INotificationPresenter& _presenter;
        
    TopLevelViewHelper _tlvHelper;
    InitializeHelper _initHelper;
};
    
} // namespace Addle

#endif // NOTIFICATIONDIALOG_HPP
