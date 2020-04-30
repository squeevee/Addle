#include <QGuiApplication>
#include <QPalette>
#include "appearanceservice.hpp"

AppearanceService::AppearanceService()
{
    //kinda kludgy
    isDarkTheme = qGuiApp->palette().window().color().lightnessF() <= 0.5;
    
    if (isDarkTheme)
        iconSelector.setExtraSelectors({"dark"});
    else 
        iconSelector.setExtraSelectors({"light"});
}

QIcon AppearanceService::icon(QString identifier) const
{
    return QIcon(iconSelector.select(QStringLiteral(":/icons/%1.png").arg(identifier)));
}