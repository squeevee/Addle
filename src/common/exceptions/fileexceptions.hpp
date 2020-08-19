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

#include <QFileInfo>
#include <QCoreApplication>
namespace Addle {

class ADDLE_COMMON_EXPORT FileException : public AddleException
{
public:
    enum Action
    {
        read,
        write
    };

#ifdef ADDLE_DEBUG
    FileException(QFileInfo fileInfo, Action action, const QString& what)
        : AddleException(what), _fileInfo(fileInfo), _action(action)
    {
    }
#else
    FileException(QFileInfo fileInfo, Action action)
        : _fileInfo(fileInfo), _action(action)
    {
    }
#endif

    Action action() const { return _action; }
    QFileInfo fileInfo() const { return _fileInfo; }

private:
    const Action _action;
    const QFileInfo _fileInfo;
};

class ADDLE_COMMON_EXPORT FileDoesNotExistException : public FileException
{
    ADDLE_EXCEPTION_BOILERPLATE(FileDoesNotExistException)
#ifdef ADDLE_DEBUG
public:
    FileDoesNotExistException(QFileInfo fileInfo, Action action = Action::read)
        : FileException(
            fileInfo,
            action,
            //% "The requested file at \"%1\" does not exist."
            qtTrId("debug-messages.file-error.does-not-exist")
                .arg(fileInfo.filePath())
        )
    {
    }
#else
public:
    FileDoesNotExistException(QFileInfo fileInfo, Action action = Action::read)
        : FileException(fileInfo, action)
    {
    }
#endif
public:
    virtual ~FileDoesNotExistException() = default;
};

class FileNotPermittedException;

class FileIsEmptyException;

} // namespace Addle

#endif // FILEEXCEPTIONS_HPP