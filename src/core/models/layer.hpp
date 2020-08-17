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
    Q_INTERFACES(Addle::ILayer)
    IAMQOBJECT_IMPL
public:
    virtual ~Layer() = default;
    
    void initialize();
    void initialize(LayerBuilder& builder);

    bool isEmpty() const { _initHelper.check(); return _empty; }

    QRect boundary() const { _initHelper.check(); return _boundary; }
    QPoint topLeft() const { _initHelper.check(); return _boundary.topLeft(); }

    QColor skirtColor() { _initHelper.check(); return Qt::GlobalColor::transparent; }

    QSharedPointer<IRasterSurface> rasterSurface() { return _rasterSurface; }

public slots:

    void setTopLeft(QPoint) { }

private:
    QRect _boundary;

    bool _empty;

    IDocument* _document;

    QSharedPointer<IRasterSurface> _rasterSurface;

    InitializeHelper _initHelper;
};

} // namespace Addle
#endif // LAYER_HPP