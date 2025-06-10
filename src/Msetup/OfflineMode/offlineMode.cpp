#include <QDebug>
#include "GlobalData.h"
#include "offlineMode.h"

void startOfflineMode()
{
    qDebug() << "Offline mode started!";
}

void startOfflineModeData(const QString& text)
{
    qDebug() << text;

    if (globalCanLog) {
        CanMessage infoMsg;
        infoMsg.timestamp = 0;
        infoMsg.channel = 1;
        infoMsg.can_id = 0xFFFFFFFF;
        infoMsg.dlc = 0;
        infoMsg.data = {1};
        globalCanLog->append(infoMsg);
    }
}
