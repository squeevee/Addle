#include <QtDebug>
#include <QThread>

#ifdef ADDLE_DEBUG
#include "./stacktrace_config.hpp"
#include <boost/stacktrace.hpp>
#endif

#include "./qdebug_extensions.hpp"
#include "./debugstring.hpp"

#include "utilities/ranges.hpp"
#include "threadcrumbs.hpp"

using namespace Addle;

const ThreadCrumb::Entry* ThreadCrumb::push_p(Entry&& entry)
{
    if (Q_UNLIKELY(!_data))
        _data.reset(new Data);
    
    auto find = noDetach(_data->_index).find(entry.data);
    if (Q_LIKELY(find == _data->_index.constEnd()))
    {
        _data->_index.insert(entry.data, _data->_entries.size());
        auto it = _data->_entries.insert(_data->_entries.end(), std::move(entry));
        return &(*it);
    }
    else
    {
        assert(_data->_entries.size() > *find);
        auto& entry = _data->_entries[*find];
        
#ifdef ADDLE_DEBUG
        if (Q_UNLIKELY(!entry.allowance))
        {
            qFatal("%s", qUtf8Printable(
                //% "Repeated breadcrumb failure detected. Terminating to "
                //% "prevent unbounded recursion."
                qtTrId("debug-messages.threadcrumbs.terminating")
            ));
        }
#endif
        
        if (!(--entry.allowance))
        {
            qWarning("%s", qUtf8Printable(
                //% "The breadcrumb (%1) was already pushed to the thread (%2)."
                qtTrId("debug-messages.threadcrumbs.crumb-repeated")
                    .arg(aux_debug::debugString(entry))
                    .arg(aux_debug::debugString(aux_debug::CurrentThreadInfo))
            ));
            return nullptr;
        }
        
        return &entry;
    }
    
}

void ThreadCrumb::pop_p(const Entry* entry)
{
    assert(_data && !_data->_entries.empty() && &_data->_entries.back() == entry);
    _data->_index.remove(entry->data);
    _data->_entries.pop_back();
    
    if (_data->_entries.empty())
        _data.reset(nullptr);
}

ThreadCrumb::Data::~Data()
{
    if (Q_UNLIKELY(!_entries.empty()))
    {
        qWarning("%s", qUtf8Printable(
            //% "ThreadCrumbs destroyed with entries remaining on thread (%1)."
            qtTrId("debug-messages.threadcrumbs.non-empty-destruction")
                .arg(aux_debug::debugString(aux_debug::CurrentThreadInfo))
        ));
        
        for (const auto& entry : noDetach(_entries))
            qWarning() << entry;
    }
}

thread_local std::unique_ptr<ThreadCrumb::Data> ThreadCrumb::_data = nullptr;
