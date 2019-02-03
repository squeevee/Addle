#ifndef PREFERENCESSERVICE_H
#define PREFERENCESSERVICE_H

#include <QObject>
#include "interfaces/services/ipreferencesservice.h"

class PreferencesService : public IPreferencesService, public QObject
{
    Q_OBJECT

public:
    PreferencesService();
    virtual ~PreferencesService();
};

#endif //PREFERENCESSERVICE_H