/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

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
    void initialize(const LayerBuilder& builder);

    bool isEmpty() const { ASSERT_INIT(); return _empty; }

    QRect boundary() const { ASSERT_INIT(); return _boundary; }
    QPoint topLeft() const { ASSERT_INIT(); return _boundary.topLeft(); }

    QColor skirtColor() { ASSERT_INIT(); return Qt::GlobalColor::transparent; }

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