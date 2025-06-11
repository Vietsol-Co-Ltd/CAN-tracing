#include "RunButton.h"
#include "common_includes.h"
#include "onlineMode.h"
#include "offlineMode.h"
#include "GlobalData.h"
#include "TraceWindow.h" 


QProcess* debugTerminal = nullptr;

RunButton::RunButton(QObject* parent) : QObject(parent) {
    debugTerminal = new QProcess(this);

    #ifdef Q_OS_WIN
    debugTerminal->setProgram("cmd.exe");
    debugTerminal->setArguments({"/K", "echo Debug Terminal && echo ================"});
    #else
    debugTerminal->setProgram("xterm");
    debugTerminal->setArguments({"-T", "Debug Terminal", "-e", "bash"});
    #endif
}

void RunButton::writeToTerminal(const QString& message) {
    if (debugTerminal && debugTerminal->state() == QProcess::Running) {
        debugTerminal->write((message + "\n").toLocal8Bit());
    }
}

void RunButton::handleRunButtonClick() {
    sharedCanLog.clear();

    if (!debugTerminal || debugTerminal->state() != QProcess::Running) {
        debugTerminal->start();
    }
    
    writeToTerminal("=== Starting RunButton Operation ===");

    QString dataPath = QDir::currentPath();
        dataPath = QDir(dataPath).filePath("../data");

    writeToTerminal("Data path: " + dataPath);

    QString msetupPath = QDir(dataPath).filePath("MsetUp.cfg");
    QSettings msetup(msetupPath, QSettings::IniFormat);

    writeToTerminal("Reading MsetUp.cfg: " + msetupPath);
    writeToTerminal("File exists: " + QString(QFile::exists(msetupPath) ? "Yes" : "No"));

    msetup.beginGroup("Buttons");
    bool onlineModeChecked = msetup.value("OnlineChecked", true).toBool();
    bool offlineModeChecked = msetup.value("OfflineChecked", true).toBool();
    msetup.endGroup();

    if (!onlineModeChecked && !offlineModeChecked) {
        emit errorOccurred("Please select Online or Offline mode in Measurement Setup tab first!");
        return;
    }

    if (onlineModeChecked) {
        writeToTerminal("Switching to Online Mode...");
        globalCanLog = &sharedCanLog;

        startOnlineMode();
        startOnlineModeData("Online data start...");

        // Kiểm tra và lấy phần tử cuối cùng để log
        if (globalCanLog && !globalCanLog->isEmpty()) {
            const CanMessage& lastMsg = globalCanLog->last();
            qDebug() << "infoMsg: timestamp=" << lastMsg.timestamp
                    << ", channel=" << lastMsg.channel
                    << ", can_id=" << QString::number(lastMsg.can_id, 16)
                    << ", dlc=" << lastMsg.dlc
                    << ", data=" << lastMsg.data;
        }
    } else if (offlineModeChecked) {
        writeToTerminal("Switching to Offline Mode...");
        globalCanLog = &sharedCanLog;

        startOfflineMode();
        startOfflineModeData("Offline log start...");

        // Kiểm tra và lấy phần tử cuối cùng để log
        if (globalCanLog && !globalCanLog->isEmpty()) {
            const CanMessage& lastMsg = globalCanLog->last();
            qDebug() << "infoMsg: timestamp=" << lastMsg.timestamp
                    << ", channel=" << lastMsg.channel
                    << ", can_id=" << QString::number(lastMsg.can_id, 16)
                    << ", dlc=" << lastMsg.dlc
                    << ", data=" << lastMsg.data;
        }
    }
}