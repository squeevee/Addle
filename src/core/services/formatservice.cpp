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

#include "utilities/taskprogresshandle.hpp"

#include "servicelocator.hpp"
#include "exceptions/formatexception.hpp"
#include "utilities/iocheck.hpp"
#include "utilities/errors.hpp"
#include "utilities/idinfo.hpp"

using namespace Addle;

GenericFormatModel FormatService::importModel_p(QIODevice& device, const GenericImportExportInfo& info)
{ 
    updateFormatType(info.type());
    
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
        GenericFormatDriver driver = GenericFormatDriver::get(format);
        
        ADDLE_ASSERT(driver.supportsImport());

        GenericFormatModel result = driver.importModel(device, info);
        
        if (info.progressHandle())
        {
            auto& cursor = info.progressHandle()->cursor();
            double min = cursor.min();
            double max = cursor.max();
            for (int i = 0; i < 1000; ++i)
            {
                cursor.setProgress(i / (1000 * (max - min)) + min);
            }
        }

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

void FormatService::updateFormatType(GenericFormatModelTypeInfo type)
{
    if (_formats_byModelType[type.index()] == GenericFormatId::getFor(type))
        return;
    
    _formats_byModelType[type.index()].clear();
    boost::apply_visitor(visitor_setupFormatType(*this), type);
}

template<class ModelType>
void FormatService::setupFormatType_impl()
{
    for (FormatId<ModelType> format : noDetach(IdInfo::getIds<FormatId<ModelType>>()))
    {
        _formats_byMimeType.insert(format.mimeType(), GenericFormatId(format));

        int length = format.fileSignature().length();
        if (length > 0)
        {
            _formats_bySignature.insert(format.fileSignature(), format);
            if (length > _maxSignatureLength)
                _maxSignatureLength = length;
        }
        
        _formats_byModelType.get<ModelType>().insert(format);

        for (QString& suffix : format.fileExtensions())
        {
            _formats_bySuffix.insert(suffix, format);
        }
    }
}
