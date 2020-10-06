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

class FileIssuePresenterBuilder;
class AddleException;

/**
 * @class
 * Presents a message describing an issue involving one or more files.
 */
class IFileIssuePresenter : public IMessagePresenter
{
public:
    enum Issue 
    {
        FileAlreadyExists,
        FileModifiedExternally,
        
        FileIsInUse,
        
        /**
         * @note 
         * A distinction is made bewteen "read-only" and "write-privileged" for 
         * the purposes of messaging.
         * 
         * Read-only indicates that writing is either not possible (e.g.,
         * because of a physical limitation of the file medium) or would be a 
         * logical error (e.g., a temporary or virtual file that will be deleted
         * after the operation).
         * 
         * Write-privileged indicates that writing is not allowed by the 
         * operating system because of the user's access privileges. The
         * operation might succeed if retried with elevated privileges.
         * 
         * The availability and accuracy of this information may be dependent on
         * platform or filesystem. "Write-privileged" (having more options) is 
         * to be favored in cases of uncertainty.
         */
        
        FileIsReadOnly,
        FileIsWritePrivileged,
        DirectoryIsReadOnly,
        DirectoryIsWritePrivileged,
        
        NoFileReadPermission,
        NoDirectoryReadPermission,
        
        // The format is wholly unknown to Addle.
        UnrecognizedFormat,
        
        // The format is inferred to be a known format that Addle does not
        // support.
        UnsupportedFormat,
        
        // The format is inferred to be a known format supported by Addle, but
        // it is either damaged/malformed beyond readability or is actually an 
        // unsupported format and was misidentified.
        UnreadableFormat,
        
        // The format is known and supported by Addle and is partially readable,
        // but some damage/malformation was detected in reading it.
        DamagedFormat,
        
        // The format was confidently identified and the file has an incorrect
        // file extension.
        WrongFileExtension,
        
        UnknownIssue
    };

    virtual ~IFileIssuePresenter() = default;
    
    virtual void initialize(const FileIssuePresenterBuilder& builder) = 0;

    virtual Issue issue() const = 0;
    
    virtual bool canReload() const = 0;
    virtual bool canOverwrite() const = 0;
    virtual bool canRetryWithPrivilege() const = 0;
    virtual bool canPreview() const = 0;
    
    virtual KeyValueList<QString, QString> fileStats() const = 0;
    
    virtual QDateTime modified() const = 0;
    virtual quint64 fileSize() const = 0;
    
    // TODO an idea
    //
    // A real value within [0, 1] indicating the similarity of a perceptual hash 
    // of the image in the file and a reference image (e.g., the working
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
    virtual QString relativeDir() const = 0;
    
    virtual QSharedPointer<AddleException> exception() const = 0;
    
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
