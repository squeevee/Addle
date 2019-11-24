#ifndef IPNGFORMATDRIVER_HPP
#define IPNGFORMATDRIVER_HPP

#include "common/interfaces/servicelocator/imakeable.hpp"

#include "../iformatdriver.hpp"

#include "common/utilities/data/formatid.hpp"

class IPNGFormatDriver : public virtual IFormatDriver, public virtual IMakeable
{
public:
    virtual ~IPNGFormatDriver() = default;

#define ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_EXTENSION "png"
#define ADDLE_STRING__IPNGFORMATDRIVER__PNG_MIME_TYPE "image/png"
#define ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_SIGNATURE "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"

    static const QString PNG_FILE_EXTENSION;
    static const QString PNG_MIME_TYPE;
    static const QByteArray PNG_FILE_SIGNATURE;

    static const FormatId PNG_FORMAT_ID;

};

#endif // IPNGFORMATDRIVER_HPP