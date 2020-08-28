/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "genericformat.hpp"
#include "utilities/errors.hpp"

#include "interfaces/models/idocument.hpp"
#include "interfaces/format/iformatdriver.hpp"

class Alternate
{

};

using namespace Addle;

struct visitor_importModel
{
    typedef GenericFormatModel result_type;

    visitor_importModel(QIODevice& device_, const GenericImportExportInfo& genericInfo_)
        : device(device_), genericInfo(genericInfo_)
    {
    }

    template<typename ModelType>
    GenericFormatModel operator()(IFormatDriver<ModelType>* driver)
    {
        auto info = genericInfo.get<ModelType>();
        return driver->importModel(device, info);
    }

    QIODevice& device;
    const GenericImportExportInfo& genericInfo;
};

GenericFormatModel GenericFormatDriver::importModel(QIODevice& device, GenericImportExportInfo info)
{
    visitor_importModel visitor(device, info);
    return boost::apply_visitor(visitor, _value);
}

struct visitor_supportsImport
{
    typedef bool result_type;

    template<typename ModelType>
    bool operator()(IFormatDriver<ModelType>* driver) const
    {
        return driver->supportsImport();
    }
};

bool GenericFormatDriver::supportsImport() const
{
    return boost::apply_visitor(visitor_supportsImport(), _value);
}