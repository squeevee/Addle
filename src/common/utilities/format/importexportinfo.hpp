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

#include <QReadWriteLock>

namespace Addle {

enum ImportExportDirection
{
    unassigned,
    importing,
    exporting
};

class TaskProgressHandle;

// Data about a resource (i.e., a file or stream) that is to be imported or
// exported as a format model.
template<class ModelType>
class ImportExportInfo
{
public:
    typedef ImportExportDirection Direction;

    ImportExportInfo() = default;
    ImportExportInfo(const ImportExportInfo& other)
    {
        const QReadLocker lock(&other._lock);
        
        _direction = other._direction;
        _format = other._format;
        _url = other._url;
        _progress = other._progress;
    }
    
    ImportExportInfo(ImportExportInfo&& other)
    {
        const QWriteLocker lock(&other._lock);
        
        _direction = other._direction;
        _format = other._format;
        _url = std::move(other._url);
        _progress = std::move(other._progress);
    }
    
    Direction direction() const 
    { 
        const QReadLocker lock(&_lock); 
        return _direction; 
    }
    
    void setDirection(Direction direction)
    { 
        const QWriteLocker lock(&_lock); 
        _direction = direction; 
    }

    FormatId<ModelType> format() const 
    { 
        const QReadLocker lock(&_lock);
        return _format;
    }
    
    void setFormat(const FormatId<ModelType>& format) 
    { 
        const QWriteLocker lock(&_lock);
        _format = format; 
    }

    QFileInfo fileInfo() const
    { 
        const QReadLocker lock(&_lock);
        return _fileInfo; 
    }
    
    void setFileInfo(const QFileInfo& fileInfo)
    { 
        const QWriteLocker lock(&_lock);
        _fileInfo = fileInfo; 
    }

    QString filename() const
    { 
        const QReadLocker lock(&_lock);
        return _fileInfo.filePath(); 
    }
    
    void setFilename(const QString& filename)
    { 
        const QWriteLocker lock(&_lock);
        _fileInfo = QFileInfo(filename); 
    }

    QUrl url() const
    { 
        const QReadLocker lock(&_lock);
        return _url; 
    }
    
    void setUrl(const QUrl& url)
    { 
        const QWriteLocker lock(&_lock);
        _url = url;
    }

//     void notifyAccepted() const
//     { 
//         {
//             const QReadLocker lock(&_lock); 
//             if (!_callback_accepted) return
//         }
//         _callback_accepted();
//     }
//     
//     void setCallbackAccepted(std::function<void()> callback)
//     { 
//         const QWriteLocker lock(&_lock); 
//         _callback_accepted = callback;
//     }

    TaskProgressHandle* progressHandle() const
    { 
        const QReadLocker lock(&_lock);
        return _progress; 
    }
    
    void setProgressHandle(TaskProgressHandle* progress)
    { 
        const QWriteLocker lock(&_lock);
        _progress = progress; 
    }
    
private:
    mutable QReadWriteLock _lock;
    
    Direction _direction = Direction::unassigned;
    FormatId<ModelType> _format;
    QUrl _url;
    QFileInfo _fileInfo;

//     std::function<void()> _callback_accepted;
    TaskProgressHandle* _progress = nullptr;
};

typedef ImportExportInfo<IDocument> DocumentImportExportInfo;

} // namespace Addle
#endif // FORMATPORTAGEINFO_HPP 
