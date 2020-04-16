#ifndef TOOLOPTIONBAR_HPP
#define TOOLOPTIONBAR_HPP

#include <QMainWindow>
#include <QToolBar>
#include "interfaces/presenters/toolpresenters/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"
#include "widgetsgui/utilities/decorationhelper.hpp"
#include "widgetsgui/compat.hpp"

class ADDLE_WIDGETSGUI_EXPORT ToolOptionBarBase : public QToolBar
{
    Q_OBJECT 
public:
    ~ToolOptionBarBase() = default;
protected:
    ToolOptionBarBase(IToolPresenter& presenter, QWidget* parent = nullptr);

signals:
    void needsShown();
    void needsHidden();

private slots: 
    void onSelectedChanged(bool selected);

private:
    IToolPresenter& _presenter;
    QWidget* _owner;
};

#endif // TOOLOPTIONBAR_HPP