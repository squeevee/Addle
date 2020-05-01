#ifndef I18NSERVICE_HPP
#define I18NSERVICE_HPP

#include "interfaces/services/ii18nservice.hpp"

#include <QJsonDocument>
#include <QLocale>

class I18nService : public II18nService
{
public:
    I18nService();
    virtual ~I18nService() = default;

    QString text(QString identifier) const override;

private:
    QLocale _locale;
    QJsonDocument _textDoc;
};

#endif // I18NSERVICE_HPP