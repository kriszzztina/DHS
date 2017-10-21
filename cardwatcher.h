#ifndef CARDWATCHER_H
#define CARDWATCHER_H
#include <QThread>
#include "Common.h"
#include <QMessageBox>
#include <QObject>

class CardWatcher : public QThread
{
    Q_OBJECT

public:
    void run();

signals:
    void Detached();
    void Attached();

};

#endif // CARDWATCHER_H
