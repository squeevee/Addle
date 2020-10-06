#ifndef TOPLEVELVIEWHELPER_HPP
#define TOPLEVELVIEWHELPER_HPP

#include <functional>
#include <QWidget>

#include "utilities/helpercallback.hpp"

namespace Addle {
    
class TopLevelViewHelper
{
public:
    TopLevelViewHelper(QWidget* widget, std::function<void()> setupUi = nullptr)
        : _widget(widget), _setupUi(setupUi)
    {
    }
    
    void open()
    {
        if (_setupUi && !_uiIsSetup)
        {
            _setupUi();
            _uiIsSetup = true;
        }
        
        _widget->show();
        if (_widget->isWindow())
        {
            _widget->activateWindow();
            _widget->raise();
        }
        onOpened();
    }
    
    HelperCallback onOpened;
    
    void close()
    {
        if (_widget->close())
            onClosed();
    }
    
    HelperCallback onClosed;
    
private:
    QWidget* _widget;
    std::function<void()> _setupUi;
    
    bool _uiIsSetup = false;
};
    
} // namespace Addle

#endif // TOPLEVELVIEWHELPER_HPP
