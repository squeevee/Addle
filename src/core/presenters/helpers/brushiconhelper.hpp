/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef BRUSHICONHELPER_HPP
#define BRUSHICONHELPER_HPP

#include <QIconEngine>

#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QPixmap>

#include <memory>

#include "compat.hpp"

#include "idtypes/brushid.hpp"
#include "utilities/editing/brushstroke.hpp"
#include "interfaces/presenters/assets/ibrushpresenter.hpp"
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"
#include "interfaces/editing/irastersurface.hpp"
#include "interfaces/rendering/irenderer.hpp"

#include "utilities/render/checkerboard.hpp"
namespace Addle {

class ADDLE_CORE_EXPORT BrushIconHelper : public QObject
{
    Q_OBJECT
public:
    BrushIconHelper(QObject* parent = nullptr);
    virtual ~BrushIconHelper() = default;

    QIcon icon() const;
    QIcon varySize(double size) const;
    //QIcon varyColor(QColor color) const;
    QIcon varyBrush(BrushId brush) const;

    void setBrush(BrushId brush) { _brush = brush;}
    void setInfoProvider(QSharedPointer<const IBrushPresenter::PreviewInfoProvider> info) { _info = info; }

    QSharedPointer<ISizeSelectionPresenter::ISizeIconProvider> sizeIconProvider();

private:
    class BrushIconEngine : public QIconEngine
    {
    public:
        BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush, double size);
        BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush);

        virtual ~BrushIconEngine() = default;

        void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override {}
        void addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state) override {}

        QIconEngine* clone() const override;
        void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state);

    private:
        BrushIconEngine(const BrushIconEngine&) = default;
        void paint_p(QSize frameSize);

        bool _autoSize = false;

        QPointer<const BrushIconHelper> _helper;
        QSharedPointer<BrushStroke> _brushStroke; //concurrency?

        QPixmap _cache;

        double _cachedScale;
        QColor _cachedColor;
        QColor _cachedBackground;

    };

    class SizeIconProvider : public ISizeSelectionPresenter::ISizeIconProvider
    {
    public:
        SizeIconProvider(QPointer<const BrushIconHelper> helper)
            : _helper(helper)
        {
        }
        virtual ~SizeIconProvider() = default;

        QIcon icon(double size) const;

    private:
        QPointer<const BrushIconHelper> _helper;
    };

    static QImage _pattern8;
    static QImage _pattern64;

    QSharedPointer<IRasterSurface> _underSurface;
    QSharedPointer<IRasterSurface> _brushSurface; // make static
//     QSharedPointer<IRenderer> _renderer;
    static CheckerBoard _checkerBoard;

    void paint(QPainter* painter, const QRect& rect, BrushId brush);

    BrushId _brush;

    inline double scale() const { return _info ? _info->scale() : 1; }
    inline QColor color() const { return _info ? _info->color() : QColor(); }
    inline QColor background() const { return Qt::white; }

    QSharedPointer<const IBrushPresenter::PreviewInfoProvider> _info;
    QSharedPointer<ISizeSelectionPresenter::ISizeIconProvider> _sizeIconProvider;

    // concurrency could ostensibly be a problem in this class, but I'm inclined
    // to think none of these functions would naturally be invoked outside the
    // UI thread -- maybe I want to add some asserts to enforce this

    friend class BrushIconEngine;
};

} // namespace Addle

#endif // BRUSHICONHELPER_HPP
