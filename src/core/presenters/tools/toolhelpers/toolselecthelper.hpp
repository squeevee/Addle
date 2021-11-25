/**
 * Addle source code
 * @file
 * @copyright Copyright 2020 Eleanor Hawk
 * @copyright Modification and distribution permitted under the terms of the
 * MIT License. See "LICENSE" for full details.
 */

#ifndef TOOLSELECTHELPER_HPP
#define TOOLSELECTHELPER_HPP

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/helpercallback.hpp"
namespace Addle {

class ToolSelectHelper
{
public:
    ToolSelectHelper(IToolPresenter& owner)
        : _owner(owner)
    {
    }

    inline bool isSelected() const { return _isSelected; }

    inline void setSelected(bool isSelected)
    {
//         if (_isSelected == (_owner.id() == _owner.owner()->currentTool())) return;

//         if (isSelected)
//             _owner.owner()->setCurrentTool(_owner.id());
//         else if (_owner.id() == _owner.owner()->currentTool())
//             _owner.owner()->setCurrentTool(ToolId());

        _isSelected = isSelected;

        onIsSelectedChanged(_isSelected);
        if (_isSelected)
            onSelect();
        else
            onDeselect();
        emit _owner.isSelectedChanged(_isSelected);
    }

    HelperCallback onSelect;
    HelperCallback onDeselect;
    HelperArgCallback<bool> onIsSelectedChanged;

private:
    bool _isSelected = false;

    IToolPresenter& _owner;
};

} // namespace Addle
#endif // TOOLSELECTHELPER_HPP
