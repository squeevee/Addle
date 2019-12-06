#ifndef FILEASSERTIONS_HPP
#define FILEASSERTIONS_HPP

#include <QFileInfo>

#include "exceptions/fileexceptions.hpp"

//throws verbose exceptions if a file is not readable
//accesses the filesystem but not the file itself

QSharedPointer<FileException> checkCanReadFile(QFileInfo fileInfo, bool allowEmptyFile = false)
{
    if (!fileInfo.exists())
        return QSharedPointer<FileException>(new FileDoesNotExistException(fileInfo, FileException::Action::read));

    if (fileInfo.isDir())
        return nullptr;

    if (!fileInfo.isReadable())
        return nullptr;

    if (!allowEmptyFile && fileInfo.size() == 0)
        return nullptr;

    return nullptr;
}

void assertCanReadFile(QFileInfo fileInfo, bool allowEmptyFile = false)
{
    QSharedPointer<FileException> ex = checkCanReadFile(fileInfo, allowEmptyFile);

    if (ex)
        ADDLE_THROW(*ex);
}

//throws verbose exceptions if a file is not writeable
//accesses the filesystem but not the file itself
void assertCanWriteFile(QFileInfo file)
{

}

#endif // FILEASSERTIONS_HPP