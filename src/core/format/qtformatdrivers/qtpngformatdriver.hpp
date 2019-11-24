#ifndef QTPNGFORMATDRIVER_HPP
#define QTPNGFORMATDRIVER_HPP

#include "common/interfaces/format/drivers/ipngformatdriver.hpp"

#include "qtimageformatdriver.hpp"

class QtPNGFormatDriver : public QtImageFormatDriver, public virtual IPNGFormatDriver
{
public:
    QtPNGFormatDriver()
        : QtImageFormatDriver("PNG")
    {
    }

    virtual ~QtPNGFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    QString getMimeType() const { return PNG_MIME_TYPE; }
    FormatId getFormatId() const { return PNG_FORMAT_ID; }

    QString getPrimaryFileExtension() { return PNG_FILE_EXTENSION; }
    QList<QString> getFileExtensions() { return { PNG_FILE_EXTENSION }; }
    QByteArray getSignature() { return PNG_FILE_SIGNATURE; }

};

#endif // QTPNGFORMATDRIVER_HPP