/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "iocheck.hpp"

#include <QDir>
#include "utils.hpp"

using namespace Addle;

#ifdef Q_OS_WIN
#include <windows.h>
#endif

void IOCheck::openFile(QFile& file, QIODevice::OpenMode mode, bool createPath) const
{
    if (file.isOpen())
    {
        ADDLE_ASSERT_M(
            !(mode & ~file.openMode()),
            //% "The file was already open in an incompatible file mode."
            qtTrId("debug-messages.io-check.incomaptible-file-mode")
        );
        return;
    }
    FileValidator validator(*this, &file, mode);
    validator.createPath = createPath;

    validator.validateExistance();
    validator.validatePermissions();

    if (!file.open(mode))
        validator.onFileDeviceError();
}

QByteArray IOCheck::peek(QIODevice& device, int maxSize, bool* eof) const
{
    if (eof)
        *eof = false;

    ADDLE_ASSERT(device.isOpen() && device.openMode() & QIODevice::ReadOnly);

    FileValidator fileValidator(*this);

    fileValidator.file = qobject_cast<QFile*>(&device);
    if (fileValidator.file)
    {
        fileValidator.info = QFileInfo(*fileValidator.file);
        fileValidator.reading = true;
    }

    QByteArray result;
    result.resize(maxSize);

    const qint64 peeked = device.peek(result.data(), maxSize);

    if (peeked == -1)
    {
        if (fileValidator.file)
        {
            fileValidator.onFileDeviceError();
            return QByteArray();
        }
        else
        {
            //% "An error occurred but the device type is unknown / unsupported so it could not be diagnosed."
            ADDLE_LOGIC_ERROR_M(qtTrId("debug-messages.io-check.unknown-device-error"));
        }
    }
    else if (peeked == 0 && eof)
    {
        *eof = true;
        return QByteArray();
    }
    else
    {
        ADDLE_ASSERT(peeked <= INT_MAX);
        result.resize((int)peeked);
        return result;
    }
}

IOCheck::FileValidator::FileValidator(const IOCheck& owner, QFile* file_, QIODevice::OpenMode mode)
    : _owner(owner), file(file_), info(*file_)
{
    if (
        !(mode & QIODevice::ReadOnly || mode & QIODevice::WriteOnly)
        || (mode & QIODevice::NewOnly && mode & QIODevice::ExistingOnly)
        || (mode & QIODevice::NewOnly && mode & QIODevice::ReadOnly && !(mode & QIODevice::WriteOnly))
        || (mode & QIODevice::Append && mode & QIODevice::ReadOnly && !(mode & QIODevice::WriteOnly))
        || (mode & QIODevice::Truncate && mode & QIODevice::ReadOnly && !(mode & QIODevice::WriteOnly))
        || (mode & QIODevice::ExistingOnly && !(mode & QIODevice::ReadOnly))
        || (mode & QIODevice::Append && mode & QIODevice::Truncate)
    )
    {
        //% "The file mode was not understood."
        ADDLE_LOGIC_ERROR_M(qtTrId("debug-messages.io-check.invalid-file-mode"));
    }
    
    writing = mode & QIODevice::WriteOnly
        || mode & QIODevice::Append
        || mode & QIODevice::Truncate;

    reading = mode & QIODevice::ReadOnly;

    if (reading && writing)
        operation = FileException::OpenReadWrite;
    else if (reading)
        operation = FileException::OpenReadOnly;
    else if (writing)
        operation = FileException::OpenWriteOnly;
    
    mustExist = mode & QIODevice::ReadOnly
        || mode & QIODevice::ExistingOnly;
        
    mustNotExist = mode & QIODevice::NewOnly;
}

void IOCheck::FileValidator::validateExistance() const
{
    ADDLE_ASSERT(!(mustExist && mustNotExist));
    
    if (mustExist && !info.exists())
    {
        Problems existenceProblems = Problem::DoesNotExist;
        QDir ancestor;

        if (!info.dir().exists())
        {
            existenceProblems |= Problem::IncompleteDirPath;
            //TODO: find nearest real ancestor; create path if createPath is set
        }
        
        FileException ex(operation, existenceProblems, info);

        _owner.throw_(std::move(ex), file);
        return;
    }
    else if (mustNotExist && info.exists())
    {
        _owner.throw_(FileException(operation, Problem::AlreadyExists));
        return;
    }

    if (!info.isFile() && mustBeFile)
    {
        _owner.throw_(FileException(operation, Problem::NotAFile, info), file);
        return;
    }
}

void IOCheck::FileValidator::validatePermissions() const
{
#ifdef Q_OS_WIN 
    const NtfsPermissionLoan permissionLoan;
#endif
    Problems permissionProblems;

    if (reading && !info.isReadable())
        permissionProblems |= Problem::NoReadPermission;
    if (writing && !info.isWritable())
        permissionProblems |= Problem::NoWritePermission;

    if (permissionProblems)
    {
        _owner.throw_(FileException(operation, permissionProblems, info), file);
        return;
    }
}

void IOCheck::FileValidator::onFileDeviceError() const
{
#ifdef Q_OS_WIN
    const DWORD winErr = GetLastError();
#endif 
    ADDLE_ASSERT(file);
#ifdef ADDLE_DEBUG
    qWarning() << file->errorString();
#endif
#ifdef Q_OS_WIN
    if (winErr == ERROR_SHARING_VIOLATION || winErr == ERROR_LOCK_VIOLATION)
    {
        _owner.throw_(FileException(operation, Problem::InUse, info), file);
    }
#endif

    switch(file->error())
    {
    case QFileDevice::ResourceError:
        _owner.throw_(FileException(operation, Problem::ResourceProblem | Problem::UnknownProblem, info), file);
        return;
    case QFileDevice::AbortError:
        _owner.throw_(FileException(operation, Problem::OperationAborted | Problem::UnknownProblem, info), file);
        return;
    case QFileDevice::TimeOutError:
        _owner.throw_(FileException(operation, Problem::OperationTimedOut | Problem::UnknownProblem, info), file);
        return;
    default:
        _owner.throw_(FileException(operation, Problem::UnknownProblem, info), file);
        return;
    }
}

void IOCheck::throw_(FileException&& ex, QIODevice* device) const
{
    if (device && device->isOpen() && _devicePolicy == DevicePolicy::CloseOnError)
        device->close();

    if (ex.why() & ~_filter)
    {
        ADDLE_THROW(ex);
    }
}
