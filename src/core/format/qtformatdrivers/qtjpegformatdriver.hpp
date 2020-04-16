#ifndef QTJPEGFORMATDRIVER_HPP
#define QTJPEGFORMATDRIVER_HPP

#include "interfaces/format/drivers/ijpegformatdriver.hpp"
#include "globalconstants.hpp"

#include "qtimageformatdriver.hpp"

class ADDLE_CORE_EXPORT QtJPEGFormatDriver : public QtImageFormatDriver, public virtual IJPEGFormatDriver
{
public:
    QtJPEGFormatDriver()
        : QtImageFormatDriver("JPEG")
    {
    }

    virtual ~QtJPEGFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    QString getMimeType() const { return "image/jpeg"; }
    FormatId getFormatId() const { return GlobalConstants::CoreFormats::JPEG; }

    QString getPrimaryFileExtension() { return "jpg"; }
	QList<QString> getFileExtensions() { return { "jpg", "jpeg", "jpe", "jfif", "jif" }; }
    QByteArray getSignature() { return QByteArrayLiteral("\xff\xd8\xff"); }
};

#endif // QTJPEGFORMATDRIVER_HPP