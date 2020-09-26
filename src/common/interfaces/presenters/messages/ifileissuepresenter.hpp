#ifndef IFILEISSUEPRESENTER_HPP
#define IFILEISSUEPRESENTER_HPP

#include <QPixmap>
#include <QUrl>
#include <QDir>
#include <QDateTime>

#include "imessagepresenter.hpp"
#include "interfaces/traits.hpp"

#include "utilities/keyvaluelist.hpp"

namespace Addle {

/**
 * @class
 * Presenter for messages describing user-relevant issues involving files,
 * providing rich information and actions.
 */
class IFileIssuePresenter : public IMessagePresenter
{
public:
    enum Issue 
    {
        FileAlreadyExists,
        FileModifiedExternally,
        
        FileIsInUse,
        FileIsReadOnly,
        FileIsWritePrivileged,
        DirectoryIsReadOnly,
        DirectoryIsWritePrivileged
    };

    virtual ~IFileIssuePresenter() = default;
    
    virtual void initialize(Issue issue, QUrl url = QUrl(), QDir relativeDir = QDir()) = 0;

    virtual Issue issue() const = 0;
    
    virtual bool allowsReload() const = 0;
    virtual bool allowsOverwrite() const = 0;
    virtual bool allowsRetryWithPrivilege() const = 0;
    virtual bool allowsPreview() const = 0;
    
    virtual KeyValueList<QString, QString> fileStats() const = 0;
    
    virtual QDateTime modified() const = 0;
    virtual quint64 fileSize() const = 0;
    
    // TODO an idea
    //
    // A real value between 0 and 1 indicating the similarity of a perceptual
    // hash of the image in the file and a reference image (e.g., the working
    // document of the editor sending this message). For files that do not
    // contain a known image format, this is always 0.
    //
    // This stat could be useful to inform some messsaging, but is also likely
    // to be much too slow for synchronous calculation.
    //
    // virtual double perceptualSimilarity() const = 0;

    virtual QPixmap preview() const = 0;
    virtual QUrl url() const = 0;
    
    // A directory relative to which the message should resolve file names
    // (e.g., the directory of the working document in the editor sending this
    // message, or the current working directory of the application).
    virtual QDir relativeDir() const = 0;
    
signals:
    virtual void reload() = 0;
    virtual void chooseNewName() = 0;
    virtual void overwrite() = 0;
    virtual void cancel() = 0;
    
    virtual void previewUpdated(QPixmap preview) = 0;
};

DECL_MAKEABLE(IFileIssuePresenter);

} // namespace Addle

Q_DECLARE_INTERFACE(Addle::IFileIssuePresenter, "org.addle.IFileIssuePresenter");

#endif // IFILEISSUEPRESENTER_HPP
