#ifndef PROPERTYACTIONGROUP_HPP
#define PROPERTYACTIONGROUP_HPP

#include <QToolBar>
#include <QActionGroup>
#include <QAction>
#include "interfaces/presenters/ipropertydecoratedpresenter.hpp"
#include "utilities/indexvariant.hpp"
#include "utilities/propertyobserver.hpp"

class PropertyActionGroup : public QActionGroup
{
    Q_OBJECT
public:
    PropertyActionGroup(QObject* parent, IPropertyDecoratedPresenter& presenter, const char* propertyName);

    QAction* createAction(QVariant value);

private slots:
    void onActionTriggered(QAction* action);
    void onPropertyChanged(QVariant value);

private:

    QHash<IndexVariant, QAction*> _actions;
    QHash<QAction*, QVariant> _values;

    QAction* _currentChecked = nullptr;

    IPropertyDecoratedPresenter& _presenter;
    const char* _propertyName;

    const PropertyDecoration _decoration;

    PropertyObserver* _propertyObserver;
};

#endif // PROPERTYACTIONGROUP_HPP