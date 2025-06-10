#include <QDebug>
#include "GlobalData.h"
#include "onlineMode.h"

void startOnlineMode()
{
    qDebug() << "Online mode started!";
}

void startOnlineModeData(const QString& text)
{
    qDebug() << text;

    if (globalCanLog) {
        CanMessage infoMsg;
        infoMsg.timestamp = 0;
        infoMsg.channel = 1;
        infoMsg.can_id = 0xFFFFFFFF;
        infoMsg.dlc = 0;
        infoMsg.data = {2};
        globalCanLog->append(infoMsg);
    }
}
