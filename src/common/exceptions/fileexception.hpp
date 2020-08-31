/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FILEEXCEPTIONS_HPP
#define FILEEXCEPTIONS_HPP

#include "addleexception.hpp"

#include <QDir>
#include <QFileInfo>

#include <QFlags>
#include <QFileInfo>
#include <QMetaEnum>

namespace Addle {

/**
 * @class FileException
 * An exception describing a problem that occurred while attempting to access
 * a file or the file system. Thrown by IOCheck.
 */
class ADDLE_COMMON_EXPORT FileException : public AddleException
{
    Q_GADGET
    ADDLE_EXCEPTION_BOILERPLATE(FileException)
public:
    enum Operation
    {
        OpenReadOnly,
        OpenWriteOnly,
        OpenReadWrite,
        Read,
        Write,
        Create,
        Move,
        Delete
    };
    Q_ENUM(Operation)

    enum WhyFlag
    {
        // The file was does not exist.
        DoesNotExist = 0x01,

        // One or more directories in the file's path do not exist.
        IncompleteDirPath = 0x02,

        // The path indicated an object that wasn't a file or a symbolic link
        // to a file.
        NotAFile = 0x04,

        // The file already exists (and the operation expected it not to exist).
        AlreadyExists = 0x08,

        // The current user account does not have permission to read the file or
        // it is otherwise restricted.
        NoReadPermission = 0x10,

        // The current user account does not have permission to write to the
        // file, it is intrinsically read-only, or it is otherwise restricted.
        NoWritePermission = 0x20,

        // The current user account does not have permission to create a file in
        // the directory, the directory is intrinsically read-only, or is
        // otherwise restricted.
        NoCreatePermission = 0x40,

        // The current user account does not have permission to delete the file,
        // the directory is intrinsically read-only, or is otherwise restricted.
        NoDeletePermission = 0x80,
        
        // The file is currently in use by another process. (Probably a Windows-
        // only condition, POSIX file locks are uncommon and generally not
        // mandatory)
        InUse = 0x100,

        // Qt indicated a QFileDevice::AbortError.
        OperationAborted = 0x2000,

        // Qt indicated a QFileDevice::TimeOutError.
        OperationTimedOut = 0x4000,

        // Qt indicated a QFileDevice::ResourceError.
        ResourceProblem = 0x8000,

        // The nature of the problem is not known.
        UnknownProblem = 0x10000,
    };
    Q_DECLARE_FLAGS(Why, WhyFlag);
    Q_FLAG(WhyFlag);

    FileException(Operation operation, Why why, QFileInfo fileInfo = QFileInfo())
        : AddleException(
#ifdef ADDLE_DEBUG
            //% "A file exception occurred\n"
            //% " operation: %1\n"
            //% "       why: %2\n"
            //% " file path: %3"
            qtTrId("debug-messages.file-exception")
                .arg(QMetaEnum::fromType<Operation>().valueToKey(operation))
                .arg(QMetaEnum::fromType<WhyFlag>().valueToKeys(why).constData())
                .arg(fileInfo.filePath())
#endif
        ), _operation(operation), _why(why), _fileInfo(fileInfo)
    {
    }

    Operation operation() const { return _operation; }
    Why why() const { return _why; }
    QFileInfo fileInfo() const { return _fileInfo; }

    // If WhyFlag::IncompleteDirPath is set, this indicates the nearest ancestor
    // of the file that does exist.
    QDir nearestExtantAncestor() const { return _nearestExtantAncestor; }
    void setNearestExtantAncestor(QDir ancestor) { _nearestExtantAncestor = ancestor; }

private:
    Operation _operation;
    Why _why;
    QFileInfo _fileInfo;

    QDir _nearestExtantAncestor;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FileException::Why)

} // namespace Addle

#endif // FILEEXCEPTIONS_HPP