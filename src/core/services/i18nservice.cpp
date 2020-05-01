#include "i18nservice.hpp"

#include <QFile>
#include <QFileSelector>

#include <QJsonObject>
#include <QJsonValue>

I18nService::I18nService()
{
    QFile textFile(QFileSelector().select(":/i18n/text.json"));

    textFile.open(QIODevice::ReadOnly);

    _textDoc = QJsonDocument::fromJson(textFile.readAll());
}

QString I18nService::text(QString identifier) const
{
    QStringList path = identifier.split('.');

    QJsonObject obj = _textDoc.object();

    for (QString step : path)
    {
        if (!obj.contains(step)) break;

        QJsonValue val = obj[step];

        if (val.isString()) return val.toString();
        else if (val.isObject()) obj = val.toObject();
        else break;
    }

    return QString();
};