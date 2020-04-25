#ifndef FILEEXCEPTIONS_HPP
#define FILEEXCEPTIONS_HPP

#include "baseaddleexception.hpp"

#include <QFileInfo>
#include <QCoreApplication>

class ADDLE_COMMON_EXPORT FileException : public BaseAddleException
{
public:
    enum Action
    {
        read,
        write
    };

#ifdef ADDLE_DEBUG
    FileException(QFileInfo fileInfo, Action action, const QString& what)
        : BaseAddleException(what), _fileInfo(fileInfo), _action(action)
    {
    }
#else
    FileException(QFileInfo fileInfo, Action action)
        : _fileInfo(fileInfo), _action(action)
    {
    }
#endif

    Action getAction() const { return _action; }
    QFileInfo getFileInfo() const { return _fileInfo; }

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
            fallback_translate(
                "FileDoesNotExistException",
                "what",
                QStringLiteral("The requested file at \"%1\" does not exist."))
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

#endif // FILEEXCEPTIONS_HPP