#ifndef ENUMACTIONGROUP_HPP
#define ENUMACTIONGROUP_HPP

#include <QToolBar>
#include <QActionGroup>
#include <QAction>
#include "interfaces/presenters/tools/itoolpresenter.hpp"
#include "utilities/qt_extensions/propertyobserver.hpp"

class OptionActionGroup : public QActionGroup
{
    Q_OBJECT
public:
    OptionActionGroup(QObject* parent, IToolPresenter& presenter, const char* propertyName);

    QAction* createAction(QVariant value);

private slots:
    void onActionTriggered(QAction* action);
    void onPropertyChanged(QVariant value);

private:

    QHash<int, QAction*> _actions;
    QHash<QAction*, QVariant> _values;

    QAction* _currentChecked = nullptr;

    IToolPresenter& _presenter;
    const char* _propertyName;
    PropertyObserver* _propertyObserver;
};

#endif // ENUMACTIONGROUP_HPP