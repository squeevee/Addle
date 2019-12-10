#ifndef QTJPEGFORMATDRIVER_HPP
#define QTJPEGFORMATDRIVER_HPP

#include "interfaces/format/drivers/ijpegformatdriver.hpp"

#include "qtimageformatdriver.hpp"

class QtJPEGFormatDriver : public QtImageFormatDriver, public virtual IJPEGFormatDriver
{
public:
    QtJPEGFormatDriver()
        : QtImageFormatDriver("JPEG")
    {
    }

    virtual ~QtJPEGFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    QString getMimeType() const { return JPEG_MIME_TYPE; }
    FormatId getFormatId() const { return JPEG_FORMAT_ID; }

    QString getPrimaryFileExtension() { return JPEG_FILE_EXTENSION; }
    QList<QString> getFileExtensions() { return JPEG_FILE_EXTENSIONS; }
    QByteArray getSignature() { return JPEG_FILE_SIGNATURE; }
};

#endif // QTJPEGFORMATDRIVER_HPP