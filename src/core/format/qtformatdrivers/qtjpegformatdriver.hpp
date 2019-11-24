#ifndef QTJPEGFORMATDRIVER_HPP
#define QTJPEGFORMATDRIVER_HPP

#include "common/interfaces/format/drivers/ijpegformatdriver.hpp"

#include "qtimageformatdriver.hpp"

class QtJpegFormatDriver : public QtImageFormatDriver, public virtual IJpegFormatDriver
{
public:
    QtJpegFormatDriver()
        : QtImageFormatDriver("JPEG")
    {
    }

    virtual ~QtJpegFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    QString getMimeType() const { return JPEG_MIME_TYPE; }
    FormatId getFormatId() const { return JPEG_FORMAT_ID; }

    QString getPrimaryFileExtension() { return JPEG_FILE_EXTENSION; }
    QList<QString> getFileExtensions() { return JPEG_FILE_EXTENSIONS; }
    QByteArray getSignature() { return JPEG_FILE_SIGNATURE; }
};

#endif // QTJPEGFORMATDRIVER_HPP