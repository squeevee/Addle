#ifndef COLORINFO_HPP
#define COLORINFO_HPP

#include <boost/optional.hpp>
#include <QSharedData>
#include <QColor>
#include <QString>
#include <QPoint>
#include <QMetaType>

namespace Addle {

class ColorInfo
{
    struct ColorInfoInner : QSharedData 
    {
        ColorInfoInner() = default;
        ColorInfoInner(QColor color_, QString name_, boost::optional<QPoint> pos_)
            : color(color_), name(name_), pos(pos_)
        {
        }

        QColor color;
        QString name;

        boost::optional<QPoint> pos;
    };

public: 
    ColorInfo()
        : _data(new ColorInfoInner)
    {
    }

    ColorInfo(QColor color, QString name = QString(), boost::optional<QPoint> pos = boost::optional<QPoint>())
        : _data( new ColorInfoInner(color, name, pos))
    {
    }

    ColorInfo(const ColorInfo&) = default;

    bool operator==(const ColorInfo& x) const
    { 
        return _data->color == x._data->color
            && _data->name == x._data->name
            && _data->pos == x._data->pos;
    }
    bool operator!=(const ColorInfo& x) const { return !(*this == x); }

    QColor color() const { return _data->color; }
    void setColor(QColor color) { _data->color = color; }

    QString name() const { return _data->name; }
    void setName(QString name) { _data->name = name; }

    boost::optional<QPoint> pos() const { return _data->pos; }
    void setPos(boost::optional<QPoint> pos) { _data->pos = pos; }

private: 
    QSharedDataPointer<ColorInfoInner> _data;
};

inline uint qHash(ColorInfo info, uint seed)
{ 
    return QT_PREPEND_NAMESPACE(qHash(((quint32)info.color().rgb(), seed))); // TODO fix this
    // woof
    //uint x = QT_PREPEND_NAMESPACE(qHash(info.color().rgb(),seed)) ^ QT_PREPEND_NAMESPACE(qHash(info.name(), seed));
    //if (info.pos())
        //return QT_PREPEND_NAMESPACE(qHash(info.pos()->x()) ^ QT_PREPEND_NAMESPACE(qHash(info.pos()->y(), seed)), seed) ^ x;
    //else
        //return x;
}

} // namespace Addle

Q_DECLARE_METATYPE(Addle::ColorInfo);

#endif // COLORINFO_HPP