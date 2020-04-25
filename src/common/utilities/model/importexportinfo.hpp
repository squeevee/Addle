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
    Direction getDirection() const { return _data->direction; }

    void setFormatId(const FormatId& format) { _data->format = format; }
    FormatId getFormatId() const { return _data->format; }

    template<class ModelType> void setModelType() { _data->modelType = &typeid(ModelType); }
    void setModelType(const std::type_info& modelType) { _data->modelType = &modelType; }
    const std::type_info& getModelType() const { return *_data->modelType; }

    template<class DriverType> void setDriverHint() { _data->driverHint = &typeid(DriverType); }
    void setDriverHint(const std::type_info& driverHint ) { _data->driverHint = &driverHint; }
    const std::type_info& getDriverHint() const { return *_data->driverHint; }

    void setFileInfo(const QFileInfo& fileInfo) { _data->fileInfo = fileInfo; }
    QFileInfo getFileInfo() const { return _data->fileInfo; }

    void setFilename(const QString& filename) { _data->fileInfo = QFileInfo(filename); }
    QString getFilename() const { return _data->fileInfo.filePath(); }

    void setUrl(const QUrl& url) { _data->url = url; }
    QUrl getUrl() const { return _data->url; }

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

#endif // FORMATPORTAGEINFO_HPP 