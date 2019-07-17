#ifndef IFORMATDRIVER_H
#define IFORMATDRIVER_H

#include <QString>
#include <QList>
#include <QByteArray>

#include "interfaces/models/idocument.h"

class IFormatDriver
{
    virtual ~IFormatDriver() = default;

    virtual QString getName() = 0;
    virtual QString getPrimaryExtension() = 0;
    virtual QList<QString> getExtensions() = 0;
    virtual QByteArray getMagicSignature() = 0;

    virtual bool supportsTrueColor() = 0;
    virtual bool supportsAlpha() = 0;
    virtual bool supportsMultiLayers() = 0;

    // True if the driver supports Addle-unique metadata. False otherwise.
    //
    // If the format supports arbitrary metadata then it may be practical to
    // include certain data unique to Addle, like for example the background
    // color, whether infinite canvas is enabled and skirt color(s), and the
    // color palette.
    virtual bool supportsAddleMetadata() = 0;
};

#endif //IFORMATDRIVER_H