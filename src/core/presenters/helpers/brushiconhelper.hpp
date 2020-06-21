#ifndef BRUSHICONHELPER_HPP
#define BRUSHICONHELPER_HPP

#include <QIconEngine>

#include <QObject>
#include <QPointer>
#include <QSharedPointer>

#include "compat.hpp"

#include "idtypes/brushid.hpp"
#include "utilities/editing/brushstroke.hpp"
#include "interfaces/editing/irastersurface.hpp"

class ADDLE_CORE_EXPORT BrushIconHelper : public QObject
{
    Q_OBJECT
public:
    BrushIconHelper(QObject* parent = nullptr);
    virtual ~BrushIconHelper() = default;

    QIcon icon(BrushId brush, QColor color, double size, double scale) const;
    QIcon icon(BrushId brush, QColor color) const;

    void setBackground(QColor background) { _background = background; }
    
public:
    class BrushIconEngine : public QIconEngine
    {
    public:
        BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush, QColor color, double size, double scale);
        BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush, QColor color);

        virtual ~BrushIconEngine() = default;

        void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override {}
        void addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state) override {}

        QIconEngine* clone() const override;
        void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state);

    private:
        bool _autoSize = false;
        double _scale;

        QPointer<const BrushIconHelper> _helper;
        BrushStroke _brushStroke;
    };

    mutable QSharedPointer<IRasterSurface> _surface;
    
    QColor _background;

    friend class BrushIconEngine;
};


#endif // BRUSHICONHELPER_HPP