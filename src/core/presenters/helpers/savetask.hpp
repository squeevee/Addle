#ifndef SAVETASK_HPP
#define SAVETASK_HPP

#include "utilities/asynctask.hpp"
#include "utilities/presenter/filerequest.hpp"
// #include "utilities/format/genericformat.hpp"

namespace Addle {
    
class IMessageContext;
class SaveTask : public AsyncTask 
{
    Q_OBJECT 
public:
    SaveTask(IMessageContext& messageContext);
    virtual ~SaveTask() = default;
    
//     GenericSharedFormatModel model() const;
    
protected:
    void doTask() override;
    
private:
    IMessageContext& _messageContext;
};
    
} // namespace Addle

#endif // SAVETASK_HPP
