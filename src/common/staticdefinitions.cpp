// Definitions of Qt string and collection constants

// These constants are compilation static, and therefore should not contain any
// strings that need to be translated.

#include <QString>
#include <QByteArray>

#include "interfaces/services/iservice.hpp"
#include "interfaces/services/itaskservice.hpp"
#include "interfaces/services/iapplicationsservice.hpp"

#include "interfaces/models/idocument.hpp"

#include "interfaces/format/drivers/ipngformatdriver.hpp"
#include "interfaces/format/drivers/ijpegformatdriver.hpp"

const QString IApplicationService::CMD_EDITOR_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_OPTION);
const QString IApplicationService::CMD_EDITOR_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_EDITOR_SHORT_OPTION);
const QString IApplicationService::CMD_BROWSER_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_OPTION);
const QString IApplicationService::CMD_BROWSER_SHORT_OPTION = QStringLiteral(ADDLE_STRING__IAPPLICATIONSERVICE__CMD_BROWSER_SHORT_OPTION);

const QString IService::SERVICE_THREAD_NAME_TEMPLATE = QStringLiteral(ADDLE_STRING__ISERVICE__SERVICE_THREAD_NAME_TEMPLATE);

const QString IPNGFormatDriver::PNG_FILE_EXTENSION = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_EXTENSION);
const QString IPNGFormatDriver::PNG_MIME_TYPE = QStringLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_MIME_TYPE);
const QByteArray IPNGFormatDriver::PNG_FILE_SIGNATURE = QByteArrayLiteral(ADDLE_STRING__IPNGFORMATDRIVER__PNG_FILE_SIGNATURE);
const FormatId IPNGFormatDriver::PNG_FORMAT_ID = FormatId(
    IPNGFormatDriver::PNG_MIME_TYPE,
    typeid(IDocument)
);

const QString IJpegFormatDriver::JPEG_FILE_EXTENSION = QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_EXTENSION);
const QStringList IJpegFormatDriver::JPEG_FILE_EXTENSIONS = {
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_EXTENSION),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_1),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_2),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_3),
    QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_ALT_FILE_EXTENSION_4)
};
const QString IJpegFormatDriver::JPEG_MIME_TYPE = QStringLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_MIME_TYPE);
const QByteArray IJpegFormatDriver::JPEG_FILE_SIGNATURE = QByteArrayLiteral(ADDLE_STRING__IJPEGFORMATDRIVER__JPEG_FILE_SIGNATURE);
const FormatId IJpegFormatDriver::JPEG_FORMAT_ID = FormatId(
    IJpegFormatDriver::JPEG_MIME_TYPE,
    typeid(IDocument)
);