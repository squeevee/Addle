/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TOOLOPTIONBAR_HPP
#define TOOLOPTIONBAR_HPP

#include "compat.hpp"
#include <QMainWindow>
#include <QToolBar>
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/qobject.hpp"

namespace Addle {

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

} // namespace Addle

#endif // TOOLOPTIONBAR_HPP