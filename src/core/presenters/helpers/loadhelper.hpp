#ifndef LOADHELPER_HPP
#define LOADHELPER_HPP

#include <QFileInfo>

#include "utilities/asynctask.hpp"
#include "utilities/helpercallback.hpp"
#include "utilities/presenter/filerequest.hpp"

// #include "utilities/format/genericformat.hpp"
// #include "utilities/idinfo.hpp"
#include "utilities/ranges.hpp"
#include "servicelocator.hpp"

namespace Addle {

class LoadTask;
template<class ModelType, class PresenterType>
class LoadHelper 
{
public:
    LoadHelper();
    ~LoadHelper();
    
    HelperArgCallback<QSharedPointer<PresenterType>> onLoaded;
    
    inline bool canLoad() const;
    void load(QSharedPointer<FileRequest> request);
    
    QSharedPointer<PresenterType> presenter() const { return _presenter; }
    
private:
    void onTaskComplete_p();
//     void onTaskFailed_p();
    
    QSharedPointer<FileRequest> _request;
    QSharedPointer<PresenterType> _presenter;
    
    LoadTask* _task;
    friend class LoadTask;
};

class LoadTask : public AsyncTask
{
    Q_OBJECT
public:
    template<class ModelType, class PresenterType>
    LoadTask(LoadHelper<ModelType, PresenterType>& helper)
    {
        connect(
            this, &AsyncTask::completed,
            this, std::bind(&LoadHelper<ModelType, PresenterType>::onTaskComplete_p, &helper)
        );
//         connect(
//             this, &AsyncTask::failed,
//             this, std::bind(&LoadHelper<ModelType, PresenterType>::onTaskFailed_p, &helper)
//         );
    }
    
//     GenericSharedFormatModel model() const
//     {
//         const auto lock = lockRead();
//         return _model;
//     }
    
//     void setRequest(QSharedPointer<FileRequest> request)
//     {
//         const auto lock = lockWrite();
//         _request = request;
//         
//         connect(
//             request.data(), &FileRequest::urlChanged,
//             this, &LoadTask::onUrlChanged
//         ); //disconnect
//     }
    
private slots:
    void onUrlChanged()
    {
        if (!isRunning())
        {
            start();
        }
    }

protected:
    void doTask() override;
    void onError() override;
    
private:
    QSharedPointer<FileRequest> _request;
//     GenericSharedFormatModel _model;
};
    
template<class ModelType, class PresenterType>
LoadHelper<ModelType, PresenterType>::LoadHelper()
    : _task(new LoadTask(*this))
{
}

template<class ModelType, class PresenterType>
LoadHelper<ModelType, PresenterType>::~LoadHelper()
{
    delete _task;
}

template<class ModelType, class PresenterType>
bool LoadHelper<ModelType, PresenterType>::canLoad() const
{
    return !_request && !_task->isRunning();
}

template<class ModelType, class PresenterType>
void LoadHelper<ModelType, PresenterType>::load(QSharedPointer<FileRequest> request)
{
    if (!canLoad()) return;
    
    _request = request;
//     _task->setRequest(request);
    
//     _request->setAvailableFormats(
//         cpplinq::from(IdInfo::getIds<FormatId<ModelType>>())
//             >>  cpplinq::cast<GenericFormatId>()
//             >>  cpplinq::to_QList()
//     );
        
    if (_request->url().isEmpty() || !QFileInfo(_request->url().toLocalFile()).isFile())
    {
        emit _request->urlNeeded();
    }
    else
    {
        _task->start();
    }
}

template<class ModelType, class PresenterType>
void LoadHelper<ModelType, PresenterType>::onTaskComplete_p()
{
//     auto model = _task->model().get<ModelType>();
//     _request = nullptr; //set on _task
//     _presenter = ServiceLocator::makeShared<PresenterType>(model);
//     onLoaded(_presenter);
}

// template<class ModelType, class PresenterType>
// void LoadHelper<ModelType, PresenterType>::onTaskFailed_p()
// {
// 
// }

} // namespace Addle

#endif // LOADHELPER_HPP
