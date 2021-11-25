#pragma once

#include <boost/mp11.hpp>

#include "interfaces/iamqobject.hpp"

#include <QByteArray>
#include <QString>
#include <QList>

namespace Addle
{
    
class IDocument;
    
namespace aux_format
{

enum FormatId
{
    Format_Id_NULL          = 0x00,
    
#define ADDLE_DOCUMENT_FORMAT_IDS_BEGIN 1
    FormatId_Document_PNG   = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x00,
    FormatId_Document_JPEG  = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x01,
    FormatId_Document_ORA   = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x02,
    FormatId_Document_WebP  = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x03,
    FormatId_Document_WebM  = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x04,
    FormatId_Document_MP4   = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x05,
    FormatId_Document_GIF   = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x06,
    FormatId_Document_PSD   = ADDLE_DOCUMENT_FORMAT_IDS_BEGIN + 0x07,
#undef ADDLE_DOCUMENT_FORMAT_IDS_BEGIN
    
    FormatId_Extensions_Begin = 0x1000
    
};
    
constexpr FormatId DocumentFormats[] = {
        FormatId_Document_PNG, FormatId_Document_JPEG, FormatId_Document_ORA,
        FormatId_Document_WebP, FormatId_Document_WebM, FormatId_Document_MP4,
        FormatId_Document_GIF, FormatId_Document_PSD
    };

using format_model_types = boost::mp11::mp_list<
        IDocument
    >;
    
}
    
class IFormatDriver : public virtual IAmQObject
{
public:
    virtual ~IFormatDriver() = default;
    
    virtual aux_format::FormatId id() const = 0;
    
    virtual QStringList filenameExtensions() const = 0;
    virtual QByteArrayList magicNumbers() const = 0;
    
    virtual QByteArrayList modelInterfaces() const = 0;
    virtual QByteArrayList exportModelInterfaces() const = 0;
    virtual QByteArrayList importModelInterfaces() const = 0;
};
    
template<class Model>
class IExportModelFormatDriver : public virtual IFormatDriver
{
public:
    virtual ~IExportModelFormatDriver() = default;
    
    // virtual QFuture<void> export_(...) = 0;
};

using IDocumentExportFormatDriver = IExportModelFormatDriver<IDocument>;
    
template<class Model>
class IImportModelFormatDriver : public virtual IFormatDriver
{
public:
    virtual ~IImportModelFormatDriver() = default;
    
    // virtual QFuture<QSharedPointer<Model>> import_(...) = 0;
};

using IDocumentImportFormatDriver = IImportModelFormatDriver<IDocument>;

}

Q_DECLARE_INTERFACE(Addle::IFormatDriver, "org.addle.IFormatDriver");
Q_DECLARE_INTERFACE(Addle::IDocumentExportFormatDriver, "org.addle.IDocumentExportFormatDriver");
Q_DECLARE_INTERFACE(Addle::IDocumentImportFormatDriver, "org.addle.IDocumentImportFormatDriver");
