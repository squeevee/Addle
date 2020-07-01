#ifndef SIZESELECTOR_HPP
#define SIZESELECTOR_HPP

#include <QWidget>
#include <QHash>

#include <QToolButton>
#include <QWeakPointer>

#include "widgetsgui/utilities/presenterassignment.hpp"

#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

class QListWidget;
class QListWidgetItem;
class SizeSelector : public QWidget
{
    Q_OBJECT 
public:
    SizeSelector(QWidget* parent = nullptr);
    virtual ~SizeSelector() = default;

    void setPresenter(const PresenterAssignment<ISizeSelectionPresenter>& presenter);

signals:
    void changed();

private slots:
    void onSelectionChanged();
    void updatePresets();

private:
    PresenterAssignment<ISizeSelectionPresenter> _presenter;

    QListWidget* _list;

    QMetaObject::Connection _connection_iconsUpdated;

    QHash<double, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, double> _itemValues;
};


#endif // SIZESELECTOR_HPP