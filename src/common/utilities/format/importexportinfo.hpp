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

    ImportExportInfo() : _data(new Data) {}
    ImportExportInfo(const ImportExportInfo& other) : _data(other._data) {}

    Direction direction() const { return _data->direction; }
    void setDirection(Direction direction) { _data->direction = direction; }

    FormatId<ModelType> format() const { return _data->format; }
    void setFormat(const FormatId<ModelType>& format) { _data->format = format; }

    QFileInfo fileInfo() const { return _data->fileInfo; }
    void setFileInfo(const QFileInfo& fileInfo) { _data->fileInfo = fileInfo; }

    QString filename() const { return _data->fileInfo.filePath(); }
    void setFilename(const QString& filename) { _data->fileInfo = QFileInfo(filename); }

    QUrl url() const { return _data->url; }
    void setUrl(const QUrl& url) { _data->url = url; }

private:
    struct Data : QSharedData
    {
        Direction direction = Direction::unassigned;
        FormatId<ModelType> format;
        QUrl url;
        QFileInfo fileInfo;
        
    };
    QSharedDataPointer<Data> _data;
};

typedef ImportExportInfo<IDocument> DocumentImportExportInfo;

} // namespace Addle
#endif // FORMATPORTAGEINFO_HPP 