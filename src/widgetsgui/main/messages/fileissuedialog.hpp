#ifndef FILEISSUEDIALOG_HPP
#define FILEISSUEDIALOG_HPP

#include <QDialog>
#include "interfaces/views/imessageview.hpp"
#include "interfaces/views/itoplevelview.hpp"
#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "../helpers/toplevelviewhelper.hpp"

#include "utilities/initializehelper.hpp"

class QLabel;

namespace Addle {

class FileIssueDialog : public QDialog, public IMessageView, public ITopLevelView
{
    Q_OBJECT 
    Q_INTERFACES(Addle::IMessageView Addle::ITopLevelView)
    IAMQOBJECT_IMPL
public:
    FileIssueDialog(QSharedPointer<IFileIssuePresenter> presenter);
    virtual ~FileIssueDialog() = default;
    
    //void initialize(QSharedPointer<IMessagePresenter> presenter) override;
    
    QSharedPointer<IMessagePresenter> presenter() const override { ASSERT_INIT(); return _presenter; }
        
public slots:
    void tlv_open() override { try { ASSERT_INIT(); _tlvHelper.open(); } ADDLE_SLOT_CATCH }
    void tlv_close() override { try { ASSERT_INIT(); _tlvHelper.close(); } ADDLE_SLOT_CATCH }

signals:
    void tlv_opened() override;
    void tlv_closed() override;
    
private:
    void setupUi();
    
    QSharedPointer<IFileIssuePresenter> _presenter;
    
    QLabel* _preview = nullptr;
        
    TopLevelViewHelper _tlvHelper;
    InitializeHelper _initHelper;
};
    
} // namespace Addle

#endif // FILEISSUEDIALOG_HPP
