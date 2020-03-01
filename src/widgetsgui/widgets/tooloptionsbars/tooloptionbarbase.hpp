#ifndef TOOLOPTIONBAR_HPP
#define TOOLOPTIONBAR_HPP

#include <QMainWindow>
#include <QToolBar>
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"
//#include "widgetsgui/utilities/actiongroupenumhelper.hpp"

#include "utilities/qt_extensions/qobject.hpp"

class ToolOptionBarBase : public QToolBar 
{
    Q_OBJECT 
public:
    ~ToolOptionBarBase() = default;
protected:
    ToolOptionBarBase(IToolPresenter& presenter, QMainWindow* parent);

    // template<typename EnumType>
    // void setupEnumAction(ActionGroupEnumHelper<EnumType>& enumHelper, QAction** actionptr, const char* property, EnumType value)
    // {
    //     QAction* action = enumHelper.createAction(value);
    //     *actionptr = action;

    //     action->setText(QString(property));
    //     action->setToolTip(_presenter.getOptionToolTip(property, value));
    // }

private slots: 
    void onSelectedChanged(bool selected);

private:
    IToolPresenter& _presenter;
    QObject* _q_presenter;
    QMainWindow& _owner;
};

#endif // TOOLOPTIONBAR_HPP