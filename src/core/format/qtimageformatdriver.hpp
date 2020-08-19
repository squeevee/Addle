/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef QTIMAGEFORMATDRIVER_HPP
#define QTIMAGEFORMATDRIVER_HPP

#include "compat.hpp"
#include "interfaces/format/iformatdriver.hpp"
namespace Addle {

// An image format driver that uses Qt's image processing functionality as its
// backend
class ADDLE_CORE_EXPORT QtImageFormatDriver : public virtual IFormatDriver
{
public:
    QtImageFormatDriver(FormatId id, const char* name)
        : _id(id), _name(name)
    {
    }
    virtual ~QtImageFormatDriver() = default;

    bool supportsImport() const { return true; }
    bool supportsExport() const { return false; }

    FormatId id() const { return _id; }

    IFormatModel* importModel(QIODevice& device, ImportExportInfo info);
    void exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info);

private:
    const FormatId _id;
    const char* _name;
};

} // namespace Addle
#endif // QTIMAGEFORMATDRIVER_HPP
