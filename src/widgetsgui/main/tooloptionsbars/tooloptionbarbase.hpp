#ifndef TOOLOPTIONBAR_HPP
#define TOOLOPTIONBAR_HPP

#include "compat.hpp"
#include <QMainWindow>
#include <QToolBar>
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/qtextensions/qobject.hpp"

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