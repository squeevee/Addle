#include "taskprogresshandle.hpp"

#include "utils.hpp"

using namespace Addle;

double TaskProgressHandle::Node::calc() const
{
    if (_children.isEmpty())
    {
        return _progress / (_max - _min) - _min;
    }
    else 
    {   
        double value = 0;
        for (Node* child : noDetach(_children))
        {
            value += child->calc() * child->_weight / _children.count();
        }
        
        return value;
    }
}

TaskProgressHandle::Node& TaskProgressHandle::spawn(double weight)
{
    checkThread();
    ADDLE_ASSERT(weight > 0.0 && qIsFinite(weight) && !qIsNaN(weight));
    
    auto child = new Node(this, _cursor, weight);
    _cursor->_children.append(child);
    
    if (_callback_onChange)
        _callback_onChange();
    
    return *child;
}

void TaskProgressHandle::pop()
{
    checkThread();
    if (_cursor->_parent)
        _cursor = _cursor->_parent;
    
    if (_callback_onChange)
        _callback_onChange();
}
