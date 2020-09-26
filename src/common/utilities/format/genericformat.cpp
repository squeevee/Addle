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
#include "interfaces/models/ibrush.hpp"
#include "interfaces/models/ipalette.hpp"
#include "interfaces/format/iformatdriver.hpp"

using namespace Addle;

const mpl_runtime_array<
    const std::type_info*,
    GenericFormatModelInfo::types,
    GenericFormatModelInfo::_typeInfoInitializer
> GenericFormatModelInfo::typeInfo;

struct _initializeGenericImportExportMakers
{
    template<typename T>
    static std::function<GenericImportExportInfo()> make()
    {
        return [] () -> GenericImportExportInfo {
            return GenericImportExportInfo(T());
        };
    }
};

const mpl_runtime_array<
    std::function<GenericImportExportInfo()>,
    GenericImportExportInfo::variant_t::types,
    _initializeGenericImportExportMakers
> _genericImportExportInfoMakers;

GenericImportExportInfo GenericImportExportInfo::make(int index)
{
    return _genericImportExportInfoMakers[index]();
}

struct visitor_importModel
{
    typedef GenericFormatModel result_type;

    visitor_importModel(QIODevice& device_, const GenericImportExportInfo& genericInfo_)
        : device(device_), genericInfo(genericInfo_)
    {
    }

    template<typename ModelType>
    GenericFormatModel operator()(IFormatDriver<ModelType>* driver) const
    {
        return driver->importModel(device, genericInfo.get<ModelType>());
    }

    QIODevice& device;
    const GenericImportExportInfo& genericInfo;
};

GenericFormatModel GenericFormatDriver::importModel(QIODevice& device, GenericImportExportInfo info)
{
    return boost::apply_visitor(visitor_importModel(device, info), _value);
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

struct visitor_destroy
{
    typedef void result_type;
    
    template<typename ModelType>
    void operator()(ModelType*& model) const
    {
        delete model; // in cpp so we're not deleting an incomplete type
        model = nullptr;
    }
};

void GenericFormatModel::destroy()
{
    boost::apply_visitor(visitor_destroy(), _value);
}
