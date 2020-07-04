#ifndef BRUSHICONHELPER_HPP
#define BRUSHICONHELPER_HPP

#include <QIconEngine>

#include <QObject>
#include <QPointer>
#include <QSharedPointer>

#include "compat.hpp"

#include "idtypes/brushid.hpp"
#include "utilities/editing/brushstroke.hpp"
#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"
#include "interfaces/editing/irastersurface.hpp"

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
    void setColor(QColor color) { _color = color; }

    void setScale(double scale) { _scale = scale; }
    void setBackground(QColor background) { _background = background; }

    QSharedPointer<ISizeSelectionPresenter::IconProvider> sizeIconProvider();

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
        bool _autoSize = false;

        QPointer<const BrushIconHelper> _helper;
        BrushStroke _brushStroke;
    };

    class SizeIconProvider : public ISizeSelectionPresenter::IconProvider
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

    mutable QSharedPointer<IRasterSurface> _surface;

    BrushId _brush;
    QColor _color;

    double _scale = 1.0;
    QColor _background;

    QSharedPointer<ISizeSelectionPresenter::IconProvider> _sizeIconProvider;

    friend class BrushIconEngine;
};


#endif // BRUSHICONHELPER_HPP