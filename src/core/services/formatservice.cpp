/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "formatservice.hpp"

#include "interfaces/format/iformatdriver.hpp"
#include "interfaces/models/idocument.hpp"

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
    // QList<IFormatDriver*> drivers = {
    //     ServiceLocator::make<IFormatDriver>(CoreFormats::JPEG),
    //     ServiceLocator::make<IFormatDriver>(CoreFormats::PNG)
    // };

    setupFormat<IDocument>();
}

GenericFormatModel FormatService::importModel_p(QIODevice& device, const GenericImportExportInfo& info)
{ 
    std::type_index modelTypeIndex(info.modelType());
    if (!_formats_byModelType.contains(modelTypeIndex))
    {
#ifdef ADDLE_DEBUG
        FormatModelNotSupportedException ex(info, info.modelType().name());
#else
        FormatModelNotSupportedException ex(info);
#endif
        ADDLE_THROW(ex);
    }

    if (!info.filename().isEmpty())
        assertCanReadFile(info.fileInfo());

    GenericFormatId impliedBySuffix = _formats_byExtension.value(info.fileInfo().completeSuffix());

    GenericFormatId format;
    if (
        (format = info.format()) ||
        (format = inferFormatFromSignature(device)) ||
        (format = impliedBySuffix)
    )
    {
        GenericFormatDriver driver;
        if (!_drivers_byFormat.contains(format))
        {
            ADDLE_THROW(FormatNotSupportedException(info, format));
        }

        driver = _drivers_byFormat.value(format);

        // if (!driver->supportsImport())
        // {
        //     ADDLE_THROW(ImportNotSupportedException(info, format, driver));
        // }

        GenericFormatModel result = driver.importModel(device, info);

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
        //ADDLE_THROW(FormatInferrenceFailedException(info));
    }
}

GenericFormatId FormatService::inferFormatFromSignature(QIODevice& device)
{
    if (device.isSequential())
        return GenericFormatId(); //Inferrence by signature not supported for sequential devices.

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

    return GenericFormatId();
}

template<class ModelType>
void FormatService::setupFormat()
{
    QSet<FormatId<ModelType>> formats;
    for (AddleId id : noDetach(ServiceLocator::getIds<IFormatDriver<ModelType>>()))
    {
        // gotta get linq or something
        formats.insert(static_cast<FormatId<ModelType>>(id));
    }

    for (FormatId<ModelType> format : formats)
    {
        auto& driver = ServiceLocator::get<IFormatDriver<ModelType>>(format);
        _drivers_byFormat.insert(format, GenericFormatDriver(driver));
        _formats_byMimeType.insert(format.mimeType(), GenericFormatId(format));

        int length = format.fileSignature().length();
        if (length > 0)
        {
            _formats_bySignature.insert(format.fileSignature(), format);
            if (length > _maxSignatureLength)
                _maxSignatureLength = length;
        }
        
        _formats_byModelType[std::type_index(typeid(ModelType))].insert(format);

        for (QString& extension : format.fileExtensions())
        {
            _formats_byExtension.insert(extension, format);
        }
    }
}