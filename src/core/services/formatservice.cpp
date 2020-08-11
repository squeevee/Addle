#include "formatservice.hpp"

#include "interfaces/format/iformatdriver.hpp"

#include <typeinfo>
#include <algorithm>
#include <QSharedPointer>

#include "globals.hpp"

#include "servicelocator.hpp"
#include "exceptions/formatexceptions.hpp"

#include "utilities/fileassertions.hpp"
//#include "utilities/taskmessages/formatmismatchmessage.hpp"

using namespace Addle;
FormatService::FormatService()
{
    QList<IFormatDriver*> drivers = {
        ServiceLocator::make<IFormatDriver>(CoreFormats::JPEG),
        ServiceLocator::make<IFormatDriver>(CoreFormats::PNG)
    };

    for (IFormatDriver* driver : drivers)
    {
        FormatId format(driver->getId());
        _drivers_byFormat.insert(format, driver);
        _formats_byMimeType.insert(driver->getId().getMimeType(), format);

        int length = driver->getId().getFileSignature().length();
        if (length > 0)
        {
            _formats_bySignature.insert(driver->getId().getFileSignature(), format);
            if (length > _maxSignatureLength)
                _maxSignatureLength = length;
        }
        
        _formats_byModelType[std::type_index(format.getModelType())].insert(format);

        for (QString& extension : driver->getId().getFileExtensions())
        {
            _formats_byExtension.insert(extension, format);
        }
    }
}

FormatService::~FormatService()
{
    for (auto driver : _drivers_byFormat)
    {
        delete driver;
    }
}

IFormatModel* FormatService::importModel_p(const std::type_info& modelType, QIODevice& device, const ImportExportInfo& info)
{
    std::type_index modelTypeIndex(modelType);
    if (!_formats_byModelType.contains(modelTypeIndex))
    {
#ifdef ADDLE_DEBUG
        FormatModelNotSupportedException ex(info, modelType.name());
#else
        FormatModelNotSupportedException ex(info);
#endif
        ADDLE_THROW(ex);
    }

    if (!info.getFilename().isEmpty())
        assertCanReadFile(info.getFileInfo());

    FormatId impliedBySuffix = _formats_byExtension.value(info.getFileInfo().completeSuffix());

    FormatId format;
    if (
        (format = info.getFormatId()) ||
        (format = inferFormatFromSignature(device)) ||
        (format = impliedBySuffix)
    )
    {
        IFormatDriver* driver;
        if (!_drivers_byFormat.contains(format))
        {
            ADDLE_THROW(FormatNotSupportedException(info, format));
        }

        driver = _drivers_byFormat.value(format);

        if (!driver->supportsImport())
        {
            ADDLE_THROW(ImportNotSupportedException(info, format, driver));
        }

        IFormatModel* result = driver->importModel(device, info);

        // if (status && format != impliedBySuffix)
        // {
        //     QSharedPointer<ITaskMessage> mismatch(new FormatMismatchMessage(
        //         impliedBySuffix,
        //         format
        //     ));
        //     status->postMessage(mismatch);
        // }

        return result;
    }
    else
    {
        ADDLE_THROW(FormatInferrenceFailedException(info));
    }
}

FormatId FormatService::inferFormatFromSignature(QIODevice& device)
{
    if (device.isSequential())
        return FormatId(); //Inferrence by signature not supported for sequential devices.

    if (!device.isOpen())
        device.open(QIODevice::ReadOnly);

    qint64 peeksize = std::min((qint64)_maxSignatureLength, device.size());
    const QByteArray peek = device.peek(peeksize);
    QByteArray sniff;
    sniff.reserve(peeksize);
    for (char c : peek)
    {
        sniff.append(c);
        if (_formats_bySignature.contains(sniff))
        {
            return _formats_bySignature.value(sniff);
        }
    }

    return FormatId();
}