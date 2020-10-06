#include <QDir>
#include <QFileInfo>
#include <QStringBuilder>

#include "utilities/presenter/messagebuilders.hpp"
#include "exceptions/fileexception.hpp"
#include "exceptions/formatexception.hpp"

#include "fileissuepresenter.hpp"

using namespace Addle;

void FileIssuePresenter::initialize(const FileIssuePresenterBuilder& builder)
{
    const Initializer init(_initHelper);
    
    _issue = builder.issue();
    _exception = builder.exception();
    
    if (_issue == UnknownIssue)
    {
        auto& baseException = *_exception;
        if (typeid(baseException) == typeid(FileException))
        {
            auto& ex = static_cast<FileException&>(baseException);
            auto why = ex.why();
            
            if (why & FileException::AlreadyExists)
            {
                _issue = FileAlreadyExists;
                why &= ~FileException::AlreadyExists;
            }
            else if (why & FileException::NoReadPermission)
            {
                _issue = NoFileReadPermission;
                why &= ~FileException::NoReadPermission;
            }
            else if (why & FileException::NoWritePermission)
            {
                //TODO write-privilege/read-only heuristics
                
                _issue = FileIsWritePrivileged;
                why &= ~FileException::NoWritePermission;
            }
#ifdef ADDLE_DEBUG
            if (why)
            {
                //% "Why Flags were set on the FileException that did not get "
                //% "accounted for in FileIssuePresenter:\n"
                //% "   exception flags: %1\n"
                //% " unaccounted flags: %2"
                qWarning() << qUtf8Printable(qtTrId("debug-messages.file-issue.spare-file-exception-flags")
                    .arg(QMetaEnum::fromType<FileException::WhyFlag>().valueToKeys(ex.why()).constData())
                    .arg(QMetaEnum::fromType<FileException::WhyFlag>().valueToKeys(why).constData())
                );
            }
#endif
        }
        else if (typeid(baseException) == typeid(FormatException))
        {
            //TODO format heursitics
            _issue = IFileIssuePresenter::UnrecognizedFormat;
        }
        else 
        {
#ifdef ADDLE_DEBUG
            //% "FileIssuePresenter could not infer an issue from the given "
            //% "exception because it was not of a useful type."
            qWarning() << qUtf8Printable(qtTrId("debug-messages.file-issue.useless-exception-type"));
#endif
        }
    }
    
    switch(_issue)
    {
    case FileAlreadyExists:
        _canOverwrite = true;
        _canRename = true;
        break;
        
    case FileModifiedExternally:
        _canOverwrite = true;
        _canRename = true;
        _canReload = true;
        _isUrgent = boost::get_optional_value_or(builder.isUrgentHint(), false);
        break;
        
    case FileIsInUse:
        _canRename = true;
        _canReload = true;
        break;
        
    case FileIsReadOnly:
    case DirectoryIsReadOnly:
        _canRename = true;
        break;
        
    case FileIsWritePrivileged:
    case DirectoryIsWritePrivileged:
        _canRename = true;
        _canRetryWithPrivilege = true;
        break;
        
    case NoFileReadPermission:
    case NoDirectoryReadPermission:
        _canRetryWithPrivilege = true;
        break;
        
    case DamagedFormat:
        _isUrgent = boost::get_optional_value_or(builder.isUrgentHint(), false);
        
    default:
        break;
    }
    
    _url = builder.url();
    _filename = _url.fileName();
    
    if (builder.relativeDir().exists())
    {
        _relativeDir = builder.relativeDir();
    }
    
    if (_url.isLocalFile() && QFileInfo::exists(_url.toLocalFile()))
    {
        _fileExists = true;
        
        QFileInfo info(_url.toLocalFile());
        _modified = info.lastModified();
        _fileSize = info.size();
        _permissions = info.permissions();
    }
}

QString FileIssuePresenter::text_p() const
{
    ASSERT_INIT();
        
    switch(_issue)
    {
        case FileAlreadyExists:
            //: %1 = file name
            return qtTrId("messages.file-issue.file-already-exists")
                .arg(_filename);
            
        case FileModifiedExternally:
            //: %1 = file name
            return qtTrId("messages.file-issue.file-modified-externally")
                .arg(_filename);
        
        case FileIsInUse:
            //: %1 = file name
            return qtTrId("messages.file-issue.file-is-in-use")
                .arg(_filename);
        
        case FileIsReadOnly:
            //: %1 = file name
            return qtTrId("messages.file-issue.file-is-read-only")
                .arg(_filename);
        
        case FileIsWritePrivileged:
            //: %1 = file name
            return qtTrId("messages.file-issue.file-is-write-privileged")
                .arg(_filename);
        
        case DirectoryIsReadOnly:
            //: %1 = file name
            return qtTrId("messages.file-issue.directory-is-read-only")
                .arg(_filename);
        
        case DirectoryIsWritePrivileged:
            //: %1 = file name
            return qtTrId("messages.file-issue.directory-is-write-privileged")
                .arg(_filename);
        
        case NoFileReadPermission:
            //: %1 = file name
            return qtTrId("messages.file-issue.no-file-read-permission")
                .arg(_filename);
        
        case NoDirectoryReadPermission:
            //: %1 = file name
            return qtTrId("messages.file-issue.no-directory-read-permission")
                .arg(_filename);
        
        case UnrecognizedFormat:
            //: %1 = file name
            return qtTrId("messages.file-issue.unrecognized-format")
                .arg(_filename);
        
        case UnsupportedFormat:
            //: %1 = file name
            return qtTrId("messages.file-issue.unsupported-format")
                .arg(_filename);
        
        case UnreadableFormat:
            //: %1 = file name
            return qtTrId("messages.file-issue.unreadable-format")
                .arg(_filename);
        
        case DamagedFormat:
            //: %1 = file name
            return qtTrId("messages.file-issue.damaged-format")
                .arg(_filename);
        
        break;
    default:
        return QString();
    }
}

KeyValueList<QString, QString> FileIssuePresenter::fileStats_p() const
{
    ASSERT_INIT();
    KeyValueList<QString, QString> result;
    
    if (!_fileExists)
        return result;
    
    if (_url.isLocalFile())
    {
        result.append(
            qtTrId("ui.file-issue-dialog.file-stats-labels.path"),
            _url.toLocalFile()
        );
    }
    
    result.append(
        qtTrId("ui.file-issue-dialog.file-stats-labels.size"),
        QLocale().formattedDataSize(_fileSize)
    );
      
    result.append(
        qtTrId("ui.file-issue-dialog.file-stats-labels.last-modified"),
        _modified.toString()
    );
    
#ifdef Q_OS_UNIX
    if (_issue == FileIsReadOnly
        || _issue == FileIsWritePrivileged
        || _issue == DirectoryIsReadOnly
        || _issue == DirectoryIsWritePrivileged
        || _issue == NoFileReadPermission
        || _issue == NoDirectoryReadPermission)
    {
        result.append(
            qtTrId("ui.file-issue-dialog.file-stats-labels.unix-permissions"),
            QStringLiteral("<pre>")
                % ((_permissions & QFileDevice::ReadOwner)  ? "r" : "-")
                % ((_permissions & QFileDevice::WriteOwner) ? "w" : "-")
                % ((_permissions & QFileDevice::ExeOwner)   ? "x" : "-")
                % ((_permissions & QFileDevice::ReadGroup)  ? "r" : "-")
                % ((_permissions & QFileDevice::WriteGroup) ? "w" : "-")
                % ((_permissions & QFileDevice::ExeGroup)   ? "x" : "-")
                % ((_permissions & QFileDevice::ReadOther)  ? "r" : "-")
                % ((_permissions & QFileDevice::WriteOther) ? "w" : "-")
                % ((_permissions & QFileDevice::ExeOther)   ? "x" : "-")
                % QStringLiteral("</pre>")
        );
    }
#endif
    
    return result;
}
