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

namespace Addle {

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

    QUrl url() const { return _url; }
    void setUrl(QUrl url)
    { 
        if (isDone()) return;

        if (_url != url)
        { 
            _url = url; 
            emit urlChanged(_url);
        } 
    }
    
    QString filePath() const { return _url.toLocalFile(); }
    void setFilePath(QString path)
    {
        setUrl(QUrl::fromLocalFile(path));
    }
    
    QString directory() const { return _directory; }
    void setDirectory(QString directory) { _directory = directory; }
    
    QUrl setUntitled(QString directory = QString(),
        QString baseName = QString(),
        QString discriminator = QString(),
        QString extension = QString());

    bool urlIsAccepted() const { return _urlIsAccepted; }

    bool isCanceled() const { return _isCanceled; }
    bool isCompleted() const { return _isCompleted; }
    bool isDone() const { return _isCanceled || _isCompleted; }
    
    int modelType() const { return _modelType; }
    void setModelType(int type) { _modelType = type; }

    QList<GenericFormatId> availableFormats() { return _availableFormats; }
    void setAvailableFormats(QList<GenericFormatId> formats) { _availableFormats = formats; }

    // A TrId for the label for a file filter that selects for all available
    // formats, e.g., "Image files". If left blank, a generic label will be used.
    const char* allFormatsLabelTrId() { return _allFormatsLabelTrId; }
    void setAllFormatsLabelTrId(const char* label) { _allFormatsLabelTrId = label; }

    GenericFormatId favoriteFormat() { return _favoriteFormat; }
    void setFavoriteFormat(GenericFormatId format) { _favoriteFormat = format; }

public slots:
    void acceptUrl()
    { 
        if (isDone()) return;

        _urlIsAccepted = true;
        emit urlAccepted();
    }

    void cancel()
    {
        if (isDone()) return;

        _isCanceled = true;
        emit canceled();
        emit done();
    }

    void complete()
    {
        if (isDone()) return;

        _isCompleted = true;
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
    const Action _action;

    QUrl _url;
    bool _urlIsAccepted = false;
    bool _isCanceled = false;
    bool _isCompleted = false;
    
    int _modelType = -1;
    
    QString _directory;

    QList<GenericFormatId> _availableFormats;
    const char* _allFormatsLabelTrId = nullptr;

    GenericFormatId _favoriteFormat;
};

} // namespace Addle

#endif // FILEREQUEST_HPP
