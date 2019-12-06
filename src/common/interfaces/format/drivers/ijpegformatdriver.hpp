#ifndef IJPEGFORMATDRIVER_HPP
#define IJPEGFORMATDRIVER_HPP

#include "interfaces/servicelocator/imakeable.hpp"

#include "../iformatdriver.hpp"

#include "data/formatid.hpp"

class IJpegFormatDriver : public virtual IFormatDriver, public virtual IMakeable
{
public:
    virtual ~IJpegFormatDriver() = default;

#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_EXTENSION "jpg"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_1 "jpeg"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_2 "jpe"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_3 "jfif"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_4 "jif"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_MIME_TYPE "image/jpeg"
#define ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_SIGNATURE "\xff\xd8\xff"

    static const QString JPEG_FILE_EXTENSION;
    static const QStringList JPEG_FILE_EXTENSIONS;
    static const QString JPEG_MIME_TYPE;
    static const QByteArray JPEG_FILE_SIGNATURE;

    static const FormatId JPEG_FORMAT_ID;

};

#endif // IJPEGFORMATDRIVER_HPP