#ifndef FILEISSUEPRESENTER_HPP
#define FILEISSUEPRESENTER_HPP

#include <limits>

#include <QObject>
#include <QFileInfo>

#include "interfaces/presenters/messages/ifileissuepresenter.hpp"

#include "utilities/lazyvalue.hpp"
#include "utilities/initializehelper.hpp"

namespace Addle {
    
class FileIssuePresenter : public QObject, public IFileIssuePresenter
{
    Q_OBJECT
    Q_INTERFACES(Addle::IFileIssuePresenter Addle::IMessagePresenter)
    IAMQOBJECT_IMPL
public:
    FileIssuePresenter()
    {
        _cache_text.calculateBy(&FileIssuePresenter::text_p, this);
        _cache_fileStats.calculateBy(&FileIssuePresenter::fileStats_p, this);
    }
    virtual ~FileIssuePresenter() = default;
    
    void initialize(const FileIssuePresenterBuilder& builder) override;
        
    IMessageContext* context() const override { ASSERT_INIT(); return _context; }
    void setContext(IMessageContext* context) override { ASSERT_INIT(); _context = context; }
    
    bool isUrgent() const override { return _isUrgent; }
    Tone tone() const override { return _tone; }
    QString text() const override { return _cache_text.value(); }
    
    Issue issue() const override { ASSERT_INIT(); return _issue; }
    
    bool canReload() const override { ASSERT_INIT(); return _canReload; }
    bool canOverwrite() const override { ASSERT_INIT(); return _canOverwrite; }
    bool canRetryWithPrivilege() const override { ASSERT_INIT(); return _canRetryWithPrivilege; }
    bool canPreview() const override { ASSERT_INIT(); return _canPreview; }
    
    KeyValueList<QString, QString> fileStats() const override { return _cache_fileStats.value(); }
    
    QDateTime modified() const override { ASSERT_INIT(); return _modified; }
    quint64 fileSize() const override { ASSERT_INIT(); return _fileSize; }
    
    QPixmap preview() const override { ASSERT_INIT(); return _preview; }
    QUrl url() const override { ASSERT_INIT(); return _url; }
    
    QString relativeDir() const override { ASSERT_INIT(); return _relativeDir.path(); }
    
    QSharedPointer<AddleException> exception() const override { ASSERT_INIT(); return _exception; }
    
signals:
    void reload() override;
    void chooseNewName() override;
    void overwrite() override;
    void cancel() override;
    
    void previewUpdated(QPixmap preview) override;

private:
    QString text_p() const;
    QString displayFilename_p() const;
    KeyValueList<QString, QString> fileStats_p() const;
    
    Issue _issue;
    QSharedPointer<AddleException> _exception;
    
    Tone _tone = Tone::Issue;
    bool _isUrgent = true;
    
    bool _canReload = false;
    bool _canRename = false;
    bool _canOverwrite = false;
    bool _canRetryWithPrivilege = false;
    bool _canPreview = false;
    
    LazyProperty<QString> _cache_text;
    LazyProperty<KeyValueList<QString, QString>> _cache_fileStats;
    
    bool _fileExists = false;
    QString _filename;
    QDateTime _modified;
    QFileDevice::Permissions _permissions;
    quint64 _fileSize = 0;
    
    QPixmap _preview;
    QUrl _url;
    
    QDir _relativeDir;
    
    IMessageContext* _context;
    
    InitializeHelper _initHelper;
};
    
} // namespace Addle

#endif // FILEISSUEPRESENTER_HPP
