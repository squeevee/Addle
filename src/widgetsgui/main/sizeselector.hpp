#ifndef SIZESELECTOR_HPP
#define SIZESELECTOR_HPP

#include <QWidget>
#include <QHash>

#include <QToolButton>
#include <QWeakPointer>

#include "utilities/presenter/presenterassignment.hpp"
#include "widgetsgui/utilities/popupbutton.hpp"

#include "interfaces/presenters/tools/isizeselectionpresenter.hpp"

class QDoubleSpinBox;
class QListWidget;
class QListWidgetItem;
class PropertyBinding;
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
    QDoubleSpinBox* _customPxSpinBox;

    PropertyBinding* _customPxBinding = nullptr;

    QHash<double, QListWidgetItem*> _items;
    QHash<QListWidgetItem*, double> _itemValues;
};

class SizeSelectorButton : public PopupButton
{
    Q_OBJECT
public:
    SizeSelectorButton(QWidget* parent = nullptr);

    void setPresenter(const PresenterAssignment<ISizeSelectionPresenter>& presenter);

private slots:
    void onChange();
    
private:

    SizeSelector* _sizeSelector;
    PresenterAssignment<ISizeSelectionPresenter> _presenter;
};

#endif // SIZESELECTOR_HPP