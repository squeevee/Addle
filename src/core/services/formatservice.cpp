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
#include "exceptions/formatexception.hpp"
#include "utilities/iocheck.hpp"
#include "utilities/errors.hpp"

using namespace Addle;
FormatService::FormatService()
{
    setupFormat<IDocument>();
}

GenericFormatModel FormatService::importModel_p(QIODevice& device, const GenericImportExportInfo& info)
{ 
    std::type_index modelTypeIndex(info.modelType());

    ADDLE_ASSERT(_formats_byModelType.contains(modelTypeIndex));

    QFile* file = qobject_cast<QFile*>(&device);
    if (file)
    {
        ADDLE_ASSERT(info.fileInfo() != QFileInfo());
        IOCheck().openFile(*file, QIODevice::ReadOnly);
        if (file->size() == 0)
            ADDLE_THROW(
                FormatException(FormatException::EmptyResource,
                GenericFormatId(),
                info)
            );
    }
    else
    {
        //TODO
        ADDLE_LOGIC_ERROR_M("Only file devices are currently supported by FormatService.");
    }

    GenericFormatId impliedBySuffix = _formats_bySuffix.value(info.fileInfo().completeSuffix());

    GenericFormatId format;
    if (
        (format = info.format()) ||
        (format = inferFormatFromSignature(device)) ||
        (format = impliedBySuffix)
    )
    {
        if(!_drivers_byFormat.contains(format))
            ADDLE_THROW(FormatException(FormatException::WrongModelType, format, info));

        GenericFormatDriver driver = _drivers_byFormat.value(format);

        ADDLE_ASSERT(driver.supportsImport());

        GenericFormatModel result = driver.importModel(device, info);

        return result;
    }
    else
    {
        ADDLE_THROW(FormatException(FormatException::FormatNotRecognized, GenericFormatId(), info));
    }
}

GenericFormatId FormatService::inferFormatFromSignature(QIODevice& device)
{
    if (device.isSequential())
        return GenericFormatId();

    const QByteArray peek = IOCheck().peek(device, _maxSignatureLength);
    QByteArray sniff;
    sniff.reserve(peek.size());
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
    for (FormatId<ModelType> format : noDetach(ServiceLocator::getIds<IFormatDriver<ModelType>>()))
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

        for (QString& suffix : format.fileExtensions())
        {
            _formats_bySuffix.insert(suffix, format);
        }
    }
}