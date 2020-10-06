/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#include "genericformat.hpp"

#include <boost/mpl/for_each.hpp>

#include "interfaces/models/idocument.hpp"
#include "interfaces/models/ibrush.hpp"
#include "interfaces/models/ipalette.hpp"
#include "interfaces/format/iformatdriver.hpp"

#include "utilities/collections.hpp"
#include "utilities/errors.hpp"
#include "utilities/idinfo.hpp"

#include "servicelocator.hpp"

using namespace Addle;

struct visitor_getIdsFor
{
    typedef QSet<GenericFormatId> result_type;
    
    template<typename ModelType>
    QSet<GenericFormatId> operator()(_formatModelTypeWrapper<ModelType>) const
    {
        return cpplinq::from(IdInfo::getIds<FormatId<ModelType>>())
            >> cpplinq::cast<GenericFormatId>()
            >> cpplinq::to_QSet();
    }
    
    QSet<GenericFormatId> operator()(_formatModelTypeWrapper<GenericFormatModelTypeInfo::NullModelType>) const
    {
        Q_UNREACHABLE();
    }
};

struct functor_aggregateIds
{
    QSet<GenericFormatId> ids;
    
    template<typename ModelType>
    void operator()(_formatModelTypeWrapper<ModelType>)
    {
        ids |= visitor_getIdsFor().operator()<ModelType>(_formatModelTypeWrapper<ModelType>());
    }
};

QSet<GenericFormatId> GenericFormatId::getAll()
{
    functor_aggregateIds f;
    
    boost::mpl::for_each<
        GenericFormatModelTypeInfo::types,
        _formatModelTypeWrapper<boost::mpl::placeholders::_1>
    >(f);
    
    return f.ids;
}

QSet<GenericFormatId> GenericFormatId::getFor(GenericFormatModelTypeInfo type)
{
    ADDLE_ASSERT(!type.isNull());
    return boost::apply_visitor(visitor_getIdsFor(), type._value);
}

struct visitor_importModel
{
    typedef GenericFormatModel result_type;

    visitor_importModel(QIODevice& device_, const ImportExportInfo& info_)
        : device(device_), info(info_)
    {
    }

    template<typename ModelType>
    GenericFormatModel operator()(IFormatDriver<ModelType>* driver) const
    {
        return driver->importModel(device, info);
    }

    QIODevice& device;
    const ImportExportInfo& info;
};

GenericFormatModel GenericFormatDriver::importModel(QIODevice& device, const ImportExportInfo& info)
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

struct visitor_getDriver
{
    typedef GenericFormatDriver result_type;
    
    template<typename ModelType>
    GenericFormatDriver operator()(FormatId<ModelType> id) const
    {
        return GenericFormatDriver(ServiceLocator::get<IFormatDriver<ModelType>>(id));
    }
};

GenericFormatDriver GenericFormatDriver::get(GenericFormatId id)
{
    return boost::apply_visitor(visitor_getDriver(), id.variant());
}
