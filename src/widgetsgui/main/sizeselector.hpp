#ifndef SIZESELECTOR_HPP
#define SIZESELECTOR_HPP

#include <QWidget>
#include <QHash>

#include <QToolButton>

#include "interfaces/presenters/toolpresenters/isizeselectionpresenter.hpp"

class QListWidget;
class QListWidgetItem;
class SizeSelector : public QWidget
{
    Q_OBJECT 
public:
    SizeSelector(ISizeSelectionPresenter& presenter, QWidget* parent = nullptr);
    virtual ~SizeSelector() = default;

signals:
    void changed();

private slots:
    void onSelectionChanged();
    void updatePresetIcons();

private:
    ISizeSelectionPresenter& _presenter;

    QListWidget* _list;

    QHash<double, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, double> _itemValues;
};

class SizeSelectButton : public QToolButton
{
    Q_OBJECT
public: 
    SizeSelectButton(ISizeSelectionPresenter& presenter, QWidget* parent = nullptr);

private slots:
    void setIcon_slot(QIcon icon) { setIcon(icon); }
private: 
    ISizeSelectionPresenter& _presenter;
    SizeSelector* _sizeSelector;
};

#endif // SIZESELECTOR_HPP