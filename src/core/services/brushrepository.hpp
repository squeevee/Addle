#ifndef BRUSHREPOSITORY_HPP
#define BRUSHREPOSITORY_HPP

#include <QObject>
#include "interfaces/services/ibrushrepository.hpp"
#include "helpers/persistentobjectrepositoryhelper.hpp"

class BrushRepository : public QObject, public IBrushRepository
{
    Q_OBJECT
public:
    BrushRepository();
    virtual ~BrushRepository() = default;

    QHash<BrushId, QSharedPointer<IBrush>> getCollection() { return _repoHelper.getCollection(); }
    void setCollection(const QHash<BrushId, QSharedPointer<IBrush>>& collection) { _repoHelper.setCollection(collection); }

    QSharedPointer<IBrush> get(const BrushId& id) { return _repoHelper.get(id); }

public slots:
    void add(const BrushId& id, QSharedPointer<IBrush> object) { _repoHelper.add(id, object); }
    void add(const BrushId& id, IBrush* object) { _repoHelper.add(id, object); }
    void remove(const BrushId& id) { _repoHelper.remove(id); }

signals:
    void collectionChanged(QList<BrushId> ids);

private:
    PersistentObjectRepositoryHelper<BrushId, IBrush> _repoHelper;
};

#endif // BRUSHREPOSITORY_HPP