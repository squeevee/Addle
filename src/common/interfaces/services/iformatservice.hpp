#ifndef IFORMATSERVICE_HPP
#define IFORMATSERVICE_HPP

#include <QObject>
#include <QIODevice>
#include <QString>
#include <QFileInfo>
#include <typeinfo>
#include <typeindex>
#include <type_traits>

#include "iservice.hpp"
#include "interfaces/models/idocument.hpp"

#include "interfaces/format/iformatmodel.hpp"

#include "interfaces/tasks/itaskstatuscontroller.hpp"

#include "interfaces/traits/initialize_trait.hpp"
#include "utilities/model/importexportinfo.hpp"
#include "interfaces/traits/makeable_trait.hpp"

/**
 * @class IFormatService
 * @brief fooooo
 * 
 * @ingroup interfaces
 */
class IFormatService : public virtual IService
{
public:
    //virtual IDocument* loadFile(QString filename) = 0;

    template<class FormatModel>
    FormatModel* importModel(QIODevice& device, const ImportExportInfo& info, ITaskStatusController* status = nullptr)
    {
        static_assert (
            std::is_base_of<IFormatModel, FormatModel>::value,
            "FormatModel must inherit IFormatModel"
        );

        return dynamic_cast<FormatModel*>(importModel_p(typeid(FormatModel), device, info, status));
    }

protected:
    virtual IFormatModel* importModel_p(const std::type_info& modelType, QIODevice& device, const ImportExportInfo& info, ITaskStatusController* status = nullptr) = 0;
};

DECL_MAKEABLE(IFormatService)

#endif // IFORMATSERVICE_HPP