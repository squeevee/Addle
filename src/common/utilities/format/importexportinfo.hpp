/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FORMATPORTAGEINFO_HPP
#define FORMATPORTAGEINFO_HPP 

#include "compat.hpp"
#include <typeinfo>
#include <functional>

#include "idtypes/formatid.hpp"

#include <QSharedData>
#include <QSharedDataPointer>

#include <QString>
#include <QUrl>
#include <QFileInfo>
namespace Addle {

enum ImportExportDirection
{
    unassigned,
    importing,
    exporting
};

// Data about a resource (i.e., a file or stream) that is to be imported or
// exported as a format model.
template<class ModelType>
class ImportExportInfo
{
public:
    typedef ImportExportDirection Direction;

    Direction direction() const { return _direction; }
    void setDirection(Direction direction) { _direction = direction; }

    FormatId<ModelType> format() const { return _format; }
    void setFormat(const FormatId<ModelType>& format) { _format = format; }

    QFileInfo fileInfo() const { return _fileInfo; }
    void setFileInfo(const QFileInfo& fileInfo) { _fileInfo = fileInfo; }

    QString filename() const { return _fileInfo.filePath(); }
    void setFilename(const QString& filename) { _fileInfo = QFileInfo(filename); }

    QUrl url() const { return _url; }
    void setUrl(const QUrl& url) { _url = url; }

    void notifyAccepted() const { if (_callback_accepted) _callback_accepted(); }
    void setCallbackAccepted(std::function<void()> callback) { _callback_accepted = callback; }

private:
    Direction _direction = Direction::unassigned;
    FormatId<ModelType> _format;
    QUrl _url;
    QFileInfo _fileInfo;

    std::function<void()> _callback_accepted;
};

typedef ImportExportInfo<IDocument> DocumentImportExportInfo;

} // namespace Addle
#endif // FORMATPORTAGEINFO_HPP 