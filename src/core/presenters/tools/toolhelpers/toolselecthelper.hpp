#ifndef TOOLSELECTHELPER_HPP
#define TOOLSELECTHELPER_HPP

#include "interfaces/presenters/imaineditorpresenter.hpp"
#include "interfaces/presenters/tools/itoolpresenter.hpp"

#include "utilities/helpercallback.hpp"

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
        if (_isSelected == (_owner.getId() == _owner.getOwner()->getCurrentTool())) return;

        if (isSelected)
            _owner.getOwner()->selectTool(_owner.getId());
        else if (_owner.getId() == _owner.getOwner()->getCurrentTool())
            _owner.getOwner()->selectTool(ToolId());

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

#endif // TOOLSELECTHELPER_HPP