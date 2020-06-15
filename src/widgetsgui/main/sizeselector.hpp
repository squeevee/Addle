#ifndef SIZESELECTOR_HPP
#define SIZESELECTOR_HPP

#include <QWidget>
#include <QHash>

#include <QToolButton>
#include <QWeakPointer>

#include "widgetsgui/utilities/presenterassignment.hpp"

#include "interfaces/presenters/toolpresenters/isizeselectionpresenter.hpp"

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

class SizeSelectButton : public QToolButton
{
    Q_OBJECT
public: 
    SizeSelectButton(
        SizeSelector* selector = nullptr,
        QWidget* parent = nullptr
    );
    virtual ~SizeSelectButton() = default;

    void setSelector(SizeSelector* selector);
    void setPresenter(QWeakPointer<ISizeSelectionPresenter> presenter);

private slots:
    void setIcon_slot(QIcon icon) { setIcon(icon); }

private:
    SizeSelector* _sizeSelector;
    QWeakPointer<ISizeSelectionPresenter> _presenter;
    QMenu* _menu;

    QMetaObject::Connection _connection_closeMenu;
    QMetaObject::Connection _connection_setIcon;
};

#endif // SIZESELECTOR_HPP