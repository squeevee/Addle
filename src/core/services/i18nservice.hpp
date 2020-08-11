#ifndef I18NSERVICE_HPP
#define I18NSERVICE_HPP

#include "compat.hpp"
#include "interfaces/services/ii18nservice.hpp"

#include <QJsonDocument>
#include <QLocale>

namespace Addle {

class ADDLE_CORE_EXPORT I18nService : public II18nService
{
public:
    I18nService();
    virtual ~I18nService() = default;

    QString text(QString identifier) const override;

private:
    QLocale _locale;
    QJsonDocument _textDoc;
};

} // namespace Addle

#endif // I18NSERVICE_HPP