#ifndef LAYERGROUPINFO_HPP
#define LAYERGROUPINFO_HPP

#include <QSharedData>
#include <QMetaType>

class LayerGroupInfo
{
    struct Data : QSharedData
    {
        Data(QString name_) : name(name_) { }

        QString name;
    };

public:
    LayerGroupInfo(QString name = QString()) : _data (new Data(name)) { }

    QString name() const { return _data->name; }
    void setName(QString name) { _data->name = name; }

private:
    QSharedDataPointer<Data> _data;
};

Q_DECLARE_TYPEINFO(LayerGroupInfo, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(LayerGroupInfo);

#endif // LAYERGROUPINFO_HPP