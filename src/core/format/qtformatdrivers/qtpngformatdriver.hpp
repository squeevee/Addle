#ifndef QTPNGFORMATDRIVER_HPP
#define QTPNGFORMATDRIVER_HPP

#include "interfaces/format/drivers/ipngformatdriver.hpp"

#include "qtimageformatdriver.hpp"
#include "globalconstants.hpp"

class ADDLE_CORE_EXPORT QtPNGFormatDriver : public QtImageFormatDriver, public virtual IPNGFormatDriver
{
public:
    QtPNGFormatDriver()
        : QtImageFormatDriver("PNG")
    {
    }

    virtual ~QtPNGFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    QString getMimeType() const { return "image/png"; }
    FormatId getFormatId() const { return GlobalConstants::CoreFormats::PNG; }

    QString getPrimaryFileExtension() { return "png"; }
    QList<QString> getFileExtensions() { return { "png" }; }
    QByteArray getSignature() { return QByteArrayLiteral("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"); }

};

#endif // QTPNGFORMATDRIVER_HPP