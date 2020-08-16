#ifndef LAYER_HPP
#define LAYER_HPP

#include "compat.hpp"
#include <QObject>
#include <QColor>
#include <QImage>
#include <QPoint>

#include <QTransform>

#include "interfaces/models/ilayer.hpp"
#include "interfaces/models/idocument.hpp"

#include "interfaces/editing/irastersurface.hpp"

#include "utilities/initializehelper.hpp"
namespace Addle {
class ADDLE_CORE_EXPORT Layer : public QObject, public ILayer
{
    Q_OBJECT
    IAMQOBJECT_IMPL
public:
    Layer() : _initHelper(this) { }
    virtual ~Layer() = default;
    
    void initialize();
    void initialize(LayerBuilder& builder);

    bool isEmpty() { _initHelper.check(); return _empty; }

    QRect boundary() { _initHelper.check(); return _boundary; }
    QPoint topLeft() { _initHelper.check(); return _boundary.topLeft(); }
    void setTopLeft(QPoint) { _initHelper.check(); }

    QColor skirtColor() { _initHelper.check(); return Qt::GlobalColor::transparent; }

    QSharedPointer<IRasterSurface> rasterSurface() { return _rasterSurface; }

private:
    QRect _boundary;

    bool _empty;

    IDocument* _document;

    QSharedPointer<IRasterSurface> _rasterSurface;

    InitializeHelper<Layer> _initHelper;
};

} // namespace Addle
#endif // LAYER_HPP