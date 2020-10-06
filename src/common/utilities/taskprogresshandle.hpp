#ifndef TASKPROGRESSHANDLE_HPP
#define TASKPROGRESSHANDLE_HPP

#include <functional>

#include <QThread>
#include <QSharedPointer>
#include <QReadWriteLock>

#include "utilities/errors.hpp"

namespace Addle {
    
/**
 * @class
 * @brief Controller object used by a task to report its progress.
 * 
 * A task's progress is modeled here as a tree, where each node is meant to
 * correspond to some logical subdivision of the task. A terminal node's
 * progress value is a scalar, otherwise a node's progress value is the
 * normalized and weighted sum of its children's progress values.
 * 
 * TaskProgressHandle manages the tree, and exposes a "cursor", which is the
 * node relative to which new nodes are created.
 * 
 * In typical usage, one subroutine of a task may create nodes to represent its 
 * own logic, and nodes to represent other subroutines. Before invoking one of
 * the other subroutines, the cursor is set to that subroutine's corresponding
 * progress node.
 * 
 * This allows the model of a given task's progress to be arbitrarily complex
 * (and thus potentially more accurate or useful for complicated tasks), without
 * entangling any particular subroutine with the context in which it was called.
 * 
 * To avoid unwanted apparent regressions, all of a node's children should be
 * created before values are set on any of them.
 * 
 * The owner of TaskProgressHandle can monitor the overall progress of the whole
 * tree.
 * 
 * @todo The current design explicitly assumes all progress is being reported
 * (and thus presumably all work is being done) on the same thread. This design
 * probably won't have to be overhauled to support parallel operations, but it
 * certainly would need some work.
 */
class TaskProgressHandle 
{
public:
    class Node
    {
    public:
        Node(const Node&) = delete;
        Node(Node&&) = delete;
        
        ~Node()
        {
            for (auto child : noDetach(_children))
            {
                delete child;
            }
        }
        
        double progress() const { return _progress; }
        void setProgress(double progress, bool allowRegression = false)
        {
            _handle->checkThread();
            _progress = qBound(
                allowRegression ? _min : _progress, 
                progress, 
                _max
            );
            
            if (_handle->_callback_onChange)
                _handle->_callback_onChange();
        }
        
        void increment(double by = 1.0)  { setProgress(_progress + by); }
        void complete() { setProgress(_max); }
        
        double max() const { return _max; }
        void setMax(double max)
        {
            ADDLE_ASSERT(max > _min);
            _handle->checkThread(); 
            _max = max; 
            
            if (_handle->_callback_onChange)
                _handle->_callback_onChange();
        }
        
        double min() const { return _min; }
        void setMin(double min)
        { 
            ADDLE_ASSERT(min < _max);
            _handle->checkThread();
            _min = min; 
            
            if (_handle->_callback_onChange)
                _handle->_callback_onChange();
        }
        
        double weight() const { return _weight; }
        void setWeight(double weight)
        { 
            ADDLE_ASSERT(weight > 0);
            _handle->checkThread(); 
            _weight = weight; 
            
            if (_handle->_callback_onChange)
                _handle->_callback_onChange();
        }
        
    private:
        Node(TaskProgressHandle* handle, Node* parent, double weight = 1.0)
            : _handle(handle), _parent(parent), _weight(weight)
        {
            _handle->checkThread();
        }
        
        // Recursively calculated normalized value
        double calc() const;
        
        double _progress = 0.0;
        double _max = 1.0;
        double _min = 0.0;
        
        double _weight;
        
        QList<Node*> _children;
        
        Node* _parent;
        TaskProgressHandle* _handle;
        friend class TaskProgressHandle;
    };
    
    TaskProgressHandle(std::function<void()> callbackOnChange = nullptr)
        : _callback_onChange(callbackOnChange)
    {
        _thread = QThread::currentThread();
        _root = new Node(this, nullptr);
        _cursor = _root;
    }
    
    ~TaskProgressHandle()
    {
        delete _root;
    }
    
    double progress() const
    { 
        return _root->calc() * (_cursor->_max - _cursor->_min) + _cursor->_min;
    }
    
    double min() const { return _root->min(); }
    void setMin(double min) { _root->setMin(min); }
    
    double max() const { return _root->max(); }
    void setMax(double max) { _root->setMax(max); }
    
    void complete() const { _root->complete(); }
    
    Node& spawn(double weight = 1.0);
    Node& push(double weight = 1.0)
    {
        auto child = &spawn(weight);
        _cursor = child;
        return *child;
    }
    void pop();
    
    Node& cursor() { return *_cursor; }
    void setCursor(Node& Node) { _cursor = &Node; }
    
private:
    inline void checkThread()
    {
        ADDLE_ASSERT(QThread::currentThread() == _thread);
    }
    
    const std::function<void()> _callback_onChange;
    
    Node* _root;
    Node* _cursor;
    QThread* _thread;
};
    
} // namespace Addle

#endif // TASKPROGRESSHANDLE_HPP
