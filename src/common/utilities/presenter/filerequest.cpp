/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include <QStringBuilder>
#include <QRegularExpression>
#include <QDir>

#include "utils.hpp"
#include "filerequest.hpp"

using namespace Addle;

QUrl FileRequest::setUntitled(
    QString directory,
    QString baseName,
    QString discriminator,
    QString extension)
{
    const QWriteLocker lock(&_lock); 
    ADDLE_ASSERT(_action == Action::Save);
            
    if (directory.isEmpty() || !QDir(directory).exists())
        directory = QDir::currentPath();

    if (!QDir(directory).exists())
        //% "A suitable default directory could not be found."
        ADDLE_LOGIC_ERROR_M(qtTrId("debug-messages.file-request.default-directory-not-found"));

    if (baseName.isEmpty())
        //% "untitled"
        baseName = qtTrId("general.untitled-filename");

    if (extension.isEmpty())
    {
        if (_favoriteFormat)
            extension = "." % _favoriteFormat.fileExtension();
        else if (!_availableFormats.isEmpty())
            extension = "." % _availableFormats.constFirst().fileExtension();
    }

    if (discriminator.isEmpty())
    {
        QRegularExpression conflictFilter(
            "^" % QRegularExpression::escape(baseName) 
            % "[^\\p{L}\\d]*(\\d*)[^\\p{L}]*" 
            % QRegularExpression::escape(extension) % "$"
        ); 
        // Matches filenames beginning with baseName, an optional sequence of
        // non-letter characters, and ending with extension. The first sequence
        // of digits (if present) is captured in group 1.

        int highest = -1;

        for (QString peerName : QDir(directory).entryList(QDir::AllEntries))
        {
            auto match = conflictFilter.match(peerName);
            if (match.hasMatch())
            {
                QString group = match.captured(1);
                if (!group.isEmpty())
                {
#ifdef ADDLE_DEBUG
                    bool ok;
                    int num = group.toInt(&ok);
                    ADDLE_ASSERT(ok);
#else
                    int num = group.toInt();
#endif
                    if (num > highest)
                        highest = num;
                }
                else
                {
                    if (highest < 0)
                        highest = 0;
                }
            }
        }

        if (highest > -1)
            discriminator = QStringLiteral("_%1").arg(highest + 1);
    }

    QString path = directory % QDir::separator() % baseName % discriminator % extension;
    ADDLE_ASSERT(!QFileInfo(path).exists());

    setUrl(QUrl::fromLocalFile(path));
    return _url;
}
