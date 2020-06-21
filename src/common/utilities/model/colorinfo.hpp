#ifndef COLORINFO_HPP
#define COLORINFO_HPP

#include <QSharedData>
#include <QColor>
#include <QString>

class ColorInfo
{
    struct ColorInfoInner : QSharedData 
    {
        ColorInfoInner() = default;
        ColorInfoInner(QColor color_, QString name_)
            : color(color_), name(name_)
        {
        }

        QColor color;
        QString name;
    };

public: 
    ColorInfo()
        : _data(new ColorInfoInner)
    {
    }

    ColorInfo(QColor color, QString name)
        : _data( new ColorInfoInner(color, name))
    {
    }

    ColorInfo(const ColorInfo&) = default;

    QColor color() const { return _data->color; }
    void setColor(QColor color) { _data->color = color; }

    QString name() const { return _data->name; }
    void setName(QString name) { _data->name = name; }

private: 
    QSharedDataPointer<ColorInfoInner> _data;
};

#endif // COLORINFO_HPP