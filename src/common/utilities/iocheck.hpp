/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef IOCHECK_HPP
#define IOCHECK_HPP

#include "exceptions/fileexception.hpp"

#include <QIODevice>
#include <QFileInfo>

extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

namespace Addle {

/**
 * @class IOCheck
 * @brief High-level error checking for IO device and file system access.
 * 
 * If an error occurs, a detailed FileException (or logic error) is thrown.
 */
class ADDLE_COMMON_EXPORT IOCheck
{
public:
    typedef FileException::WhyFlag Problem;
    typedef FileException::Why Problems;

    struct NtfsPermissionLoan
    {
        inline NtfsPermissionLoan()  { ++qt_ntfs_permission_lookup; }
        inline ~NtfsPermissionLoan() { --qt_ntfs_permission_lookup; }
    };

    enum DevicePolicy
    {
        CloseOnError,
        DoNotCloseOnError
    };

    /**
     * @param filter Any FileException `ex` for which `ex.why() & ~filter == 0`
     * will not be thrown.
     * 
     * @param policy For functions that handle/create a device, this determines
     * whether the device should be closed if an error occurs.
     */
    inline IOCheck(Problems filter = Problems(), DevicePolicy policy = CloseOnError)
        : _filter(filter), _devicePolicy(policy)
    {
    }

    inline Problems filter() const { return _filter; }
    inline IOCheck& setFilter(Problems filter) { _filter = filter; return *this; }

    inline DevicePolicy devicePolicy() const { return _devicePolicy; }
    inline IOCheck& setDevicePolicy(DevicePolicy devicePolicy) { _devicePolicy = devicePolicy; return *this; }

    /**
     * Checked call to `file.open(mode)`.
     * 
     * Does nothing if file is already open (to a compatible open mode).
     * 
     * @param createPath (For open modes that create a file) indicates that any
     * missing directories on the file's path should also be created.
     */
    void openFile(QFile& file, QIODevice::OpenMode mode, bool createPath = false) const;

    /**
     * Checked calls to corresponding QIODevice functions, outputting data as 
     * QByteArray where applicable:
     * 
     * @param eof if not null, *eof is set to true when there is no more data to
     * read (e.g., the end of the file has been reached)
     */

    QByteArray read(QIODevice& device, int maxSize, bool* eof = nullptr) const;
    QByteArray readLine(QIODevice& device, bool* eof = nullptr) const;
    QByteArray readAll(QIODevice& device) const;
    QByteArray peek(QIODevice& device, int maxSize, bool* eof = nullptr) const;

    void write(QIODevice& device, const QByteArray& data) const;
    void write(QIODevice& device, const char* data, int maxSize) const;
    void flush(QIODevice& device) const;

    void skip(QIODevice& device, qint64 maxSize) const;
    void seek(QIODevice& device, qint64 pos) const;
    void reset(QIODevice& device) const;

    void createFile(QFileInfo info, bool createPath = false) const;
    void deleteFile(QFileInfo info) const;
    void moveFile(QFileInfo source, QFileInfo destination) const;
    void sendToTrash(QFileInfo source) const;

private:
    struct FileValidator
    {
        FileValidator(const IOCheck& owner)
            : _owner(owner)
        {
        }

        FileValidator(const IOCheck& owner, QFile* file, QIODevice::OpenMode mode);

        QFileInfo info;
        QFile* file = nullptr;

        FileException::Operation operation = (FileException::Operation)(NULL);

        bool reading = false;
        bool writing = false;

        bool mustBeFile = true;
        bool createPath = false;
        
        bool mustExist = false;
        bool mustNotExist = false;

        void validateExistance() const;
        void validatePermissions() const;
        void onFileDeviceError() const;

    private:
        const IOCheck& _owner;
    };

    void throw_(FileException&& ex, QIODevice* device = nullptr) const;

    Problems _filter;
    DevicePolicy _devicePolicy;
};

} // namespace Addle

#endif // IOCHECK_HPP
