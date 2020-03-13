#ifndef TOOLOPTIONBAR_HPP
#define TOOLOPTIONBAR_HPP

#include <QMainWindow>
#include <QToolBar>
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "widgetsgui/utilities/decorationhelper.hpp"

class ToolOptionBarBase : public QToolBar 
{
    Q_OBJECT 
public:
    ~ToolOptionBarBase() = default;
protected:
    ToolOptionBarBase(IToolPresenter& presenter, QMainWindow* parent);

private slots: 
    void onSelectedChanged(bool selected);

private:
    IToolPresenter& _presenter;
    QObject* _q_presenter;
    QMainWindow& _owner;
};

#endif // TOOLOPTIONBAR_HPP