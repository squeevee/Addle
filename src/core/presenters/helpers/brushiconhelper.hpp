#ifndef BRUSHICONHELPER_HPP
#define BRUSHICONHELPER_HPP

#include <QIconEngine>

#include <QObject>
#include <QPointer>
#include <QSharedPointer>

#include "idtypes/brushid.hpp"
#include "utilities/editing/brushstroke.hpp"
#include "interfaces/editing/irastersurface.hpp"

class BrushIconHelper : public QObject
{
    Q_OBJECT
public:
    BrushIconHelper(QObject* parent = nullptr);
    virtual ~BrushIconHelper() = default;

    QIcon icon(BrushId brush, QColor color, double size) const;

    void setBackground(QColor background) { _background = background; }
    
public:
    class BrushIconEngine : public QIconEngine
    {
    public:
        BrushIconEngine(QPointer<const BrushIconHelper> helper, BrushId brush, QColor color, double size);

        virtual ~BrushIconEngine() = default;

        void addFile(const QString& fileName, const QSize& size, QIcon::Mode mode, QIcon::State state) override {}
        void addPixmap(const QPixmap& pixmap, QIcon::Mode mode, QIcon::State state) override {}

        QIconEngine* clone() const override { return new BrushIconEngine(_helper, _brushStroke.id(), _brushStroke.color(), _brushStroke.getSize()); }
        void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state);

    private:
        QPointer<const BrushIconHelper> _helper;
        BrushStroke _brushStroke;
    };

    mutable QSharedPointer<IRasterSurface> _surface;
    
    QColor _background;

    friend class BrushIconEngine;
};


#endif // BRUSHICONHELPER_HPP