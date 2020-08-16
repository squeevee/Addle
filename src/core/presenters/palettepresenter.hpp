#ifndef PALETTEPRESENTER_HPP
#define PALETTEPRESENTER_HPP

#include "compat.hpp"

#include "interfaces/presenters/ipalettepresenter.hpp"
#include "interfaces/models/ipalette.hpp"
#include "utilities/initializehelper.hpp"

#include <QObject>
namespace Addle {

class ADDLE_CORE_EXPORT PalettePresenter : public QObject, public IPalettePresenter
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    virtual ~PalettePresenter() = default;

    void initialize(IPalette& model);

    IPalette& model() const { _initHelper.check(); return *_model; }

    QMultiArray<ColorInfo, 2> colors() const { _initHelper.check(); return _model->colors(); }

private:
    IPalette* _model;

    InitializeHelper<PalettePresenter> _initHelper;
};

} // namespace Addle
#endif // PALETTEPRESENTER_HPP