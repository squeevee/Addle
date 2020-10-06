/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef FILEREQUEST_HPP
#define FILEREQUEST_HPP

#include "compat.hpp"

#include "utilities/format/genericformat.hpp"

#include <QList>
#include <QObject>
#include <QUrl>

#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

namespace Addle {

class IMessageContext;
    
/**
 * @class FileRequest
 * @brief Communicates (potentially asynchronously) between a presenter and a
 * view about an ongoing request to take an action on a file.
 */
class ADDLE_COMMON_EXPORT FileRequest : public QObject
{
    Q_OBJECT
public:
    enum Action
    {
        Save,
        Load
    };

    FileRequest(Action action)
        : _action(action)
    {
    }
    virtual ~FileRequest() = default;

    Action action() const { return _action; }

    QUrl url() const { const QReadLocker lock(&_lock); return _url; }
    void setUrl(QUrl url)
    { 
        if (isDone()) return;
        if (_url == url) return;
        
        {
            const QWriteLocker lock(&_lock); 
            _url = url;
        }
        
        emit urlChanged(_url);
    }
    
    QString filePath() const { const QReadLocker lock(&_lock); return _url.toLocalFile(); }
    void setFilePath(QString path)
    {
        const QWriteLocker lock(&_lock); 
        setUrl(QUrl::fromLocalFile(path));
    }
    
    QString directory() const { const QReadLocker lock(&_lock); return _directory; }
    void setDirectory(QString directory) { const QWriteLocker lock(&_lock); _directory = directory; }
    
    QUrl setUntitled(QString directory = QString(),
        QString baseName = QString(),
        QString discriminator = QString(),
        QString extension = QString());

    bool overwriteIsAllowed() const { const QReadLocker lock(&_lock); return _overwriteIsAllowed; }
    
    bool urlIsAccepted() const { const QReadLocker lock(&_lock); return _urlIsAccepted; }
    
    bool isCanceled() const { const QReadLocker lock(&_lock); return _isCanceled; }
    bool isCompleted() const { const QReadLocker lock(&_lock); return _isCompleted; }
    bool isDone() const { const QReadLocker lock(&_lock); return _isCanceled || _isCompleted; }
    
    GenericFormatModelTypeInfo modelType() const { const QReadLocker lock(&_lock); return _modelType; }
    void setModelType(GenericFormatModelTypeInfo type) { const QWriteLocker lock(&_lock); _modelType = type; }

    QList<GenericFormatId> availableFormats() const { const QReadLocker lock(&_lock); return _availableFormats; }
    void setAvailableFormats(QList<GenericFormatId> formats) { const QWriteLocker lock(&_lock); _availableFormats = formats; }

    // A TrId for the label for a file filter that selects for all available
    // formats, e.g., "Image files". If left blank, a generic label will be used.
    const char* allFormatsLabelTrId() const { const QReadLocker lock(&_lock); return _allFormatsLabelTrId; }
    void setAllFormatsLabelTrId(const char* label) { const QWriteLocker lock(&_lock); _allFormatsLabelTrId = label; }

    GenericFormatId favoriteFormat() const { const QReadLocker lock(&_lock); return _favoriteFormat; }
    void setFavoriteFormat(GenericFormatId format) { const QWriteLocker lock(&_lock); _favoriteFormat = format; }

    IMessageContext* messageContext() const { const QReadLocker lock(&_lock); return _messageContext; }
    void setMessageContext(IMessageContext* messageContext) { const QWriteLocker lock(&_lock); _messageContext = messageContext; }
    
public slots:
    void acceptUrl()
    { 
        if (isDone()) return;
        
        {
            const QWriteLocker lock(&_lock); 
            _urlIsAccepted = true;
        }
        
        emit urlAccepted();
    }

    void cancel()
    {
        if (isDone()) return;
        
        {
            const QWriteLocker lock(&_lock);
            _isCanceled = true;
        }
        
        emit canceled();
        emit done();
    }

    void complete()
    {
        if (isDone()) return;
        
        {
            const QWriteLocker lock(&_lock); 
            _isCompleted = true;
        }
        
        emit completed();
        emit done();
    }

signals:
    void urlNeeded();
    void urlAccepted();

    void urlChanged(QUrl url);

    void canceled();
    void completed();
    void done();

private:
    mutable QReadWriteLock _lock;
    
    const Action _action;

    QUrl _url;
    bool _overwriteIsAllowed = false;
    
    bool _urlIsAccepted = false;
    bool _isCanceled = false;
    bool _isCompleted = false;
    
    GenericFormatModelTypeInfo _modelType;
    
    QString _directory;
    
    IMessageContext* _messageContext = nullptr;

    QList<GenericFormatId> _availableFormats;
    const char* _allFormatsLabelTrId = nullptr;

    GenericFormatId _favoriteFormat;
};

} // namespace Addle

#endif // FILEREQUEST_HPP
