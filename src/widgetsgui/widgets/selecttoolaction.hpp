#ifndef SELECTTOOLACTION_HPP
#define SELECTTOOLACTION_HPP

#include <QAction>

#include "utilities/qt_extensions/qobject.hpp"

#include "interfaces/presenters/tools/itoolpresenter.hpp"

class SelectToolAction : public QAction
{
    Q_OBJECT
public:
    SelectToolAction(IToolPresenter& presenter, QWidget* parent);
    ~SelectToolAction() = default;
private slots: 
    void onSelectedChanged(bool selected) { setChecked(selected); }

private:
    IToolPresenter& _presenter;
};

#endif // SELECTTOOLACTION_HPP