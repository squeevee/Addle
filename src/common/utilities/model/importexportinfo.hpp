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

// Data about a resource (i.e., a file or stream) that is to be imported or
// exported as a format model.
class ImportExportInfo
{
public:
    enum Direction
    {
        unassigned,
        importing,
        exporting
    };

    ImportExportInfo() : _data(new ImportExportInfoData) {}
    ImportExportInfo(const ImportExportInfo& other) : _data(other._data) {}

    void setDirection(Direction direction) { _data->direction = direction; }
    Direction direction() const { return _data->direction; }

    void setFormatId(const FormatId& format) { _data->format = format; }
    FormatId formatId() const { return _data->format; }

    template<class ModelType> void setModelType() { _data->modelType = &typeid(ModelType); }
    void setModelType(const std::type_info& modelType) { _data->modelType = &modelType; }
    const std::type_info& modelType() const { return *_data->modelType; }

    template<class DriverType> void setDriverHint() { _data->driverHint = &typeid(DriverType); }
    void setDriverHint(const std::type_info& driverHint ) { _data->driverHint = &driverHint; }
    const std::type_info& driverHint() const { return *_data->driverHint; }

    void setFileInfo(const QFileInfo& fileInfo) { _data->fileInfo = fileInfo; }
    QFileInfo fileInfo() const { return _data->fileInfo; }

    void setFilename(const QString& filename) { _data->fileInfo = QFileInfo(filename); }
    QString filename() const { return _data->fileInfo.filePath(); }

    void setUrl(const QUrl& url) { _data->url = url; }
    QUrl url() const { return _data->url; }

private:
    struct ImportExportInfoData : QSharedData
    {
        Direction direction = Direction::unassigned;
        FormatId format;
        const std::type_info* modelType = nullptr;
        const std::type_info* driverHint = nullptr;
        QUrl url;
        QFileInfo fileInfo;
        
    };
    QSharedDataPointer<ImportExportInfoData> _data;
};

} // namespace Addle
#endif // FORMATPORTAGEINFO_HPP 