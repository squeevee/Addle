#include <QGuiApplication>
#include <QPalette>
#include "appearanceservice.hpp"

using namespace Addle;
AppearanceService::AppearanceService()
{
    //kinda kludgy
    _isDarkTheme = qGuiApp->palette().window().color().lightnessF() <= 0.5;
    
    if (_isDarkTheme)
        _selector.setExtraSelectors({"dark"});
    else 
        _selector.setExtraSelectors({"light"});
}

QIcon AppearanceService::icon(QString identifier) const
{
    return QIcon(_selector.select(QStringLiteral(":/icons/%1.png").arg(identifier)));
}
