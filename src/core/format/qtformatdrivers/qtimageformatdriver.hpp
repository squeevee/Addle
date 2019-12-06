#ifndef QTIMAGEFORMATDRIVER_HPP
#define QTIMAGEFORMATDRIVER_HPP

#include "interfaces/format/iformatdriver.hpp"

// An image format driver that uses Qt's image processing functionality as its
// backend
class QtImageFormatDriver : public virtual IFormatDriver
{
public:
    QtImageFormatDriver(const char* qtFormatName)
        : _qtFormatName(qtFormatName)
    {
    }
    virtual ~QtImageFormatDriver() = default;

    IFormatModel* importModel(QIODevice& device, ImportExportInfo info, ITaskStatusController* status = nullptr);
    void exportModel(IFormatModel* model, QIODevice& device, ImportExportInfo info, ITaskStatusController* status = nullptr);

private:
    const char* _qtFormatName;
};

#endif // QTIMAGEFORMATDRIVER_HPP
