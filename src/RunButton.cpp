#include "RunButton.h"
#include "lblf_reader/BlfReader.hpp"
#include "common_includes.h"
// Global variable to store CAN messages
std::vector<CanMessage> g_canMessages;
QProcess* debugTerminal = nullptr;

RunButton::RunButton(QObject* parent) : QObject(parent) {
    // Create debug terminal process
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
    // Start debug terminal if not running
    if (!debugTerminal || debugTerminal->state() != QProcess::Running) {
        debugTerminal->start();
    }

    writeToTerminal("=== Starting RunButton Operation ===");

    // Get data path
    QString dataPath = QDir::currentPath();
    if (dataPath.endsWith("bin")) {
        dataPath = QDir(dataPath).filePath("../data");
    } else {
        dataPath = QDir(dataPath).filePath("data");
    }

    writeToTerminal("Data path: " + dataPath);

    // Read MsetUp.cfg
    QString msetupPath = QDir(dataPath).filePath("MsetUp.cfg");
    QSettings msetup(msetupPath, QSettings::IniFormat);

    writeToTerminal("Reading MsetUp.cfg: " + msetupPath);
    writeToTerminal("File exists: " + QString(QFile::exists(msetupPath) ? "Yes" : "No"));

    // Debug: List all keys in config
    writeToTerminal("\nAll keys in MsetUp.cfg:");
    foreach(const QString &key, msetup.allKeys()) {
        writeToTerminal(QString("%1 = %2").arg(key).arg(msetup.value(key).toString()));
    }

    bool onlineMode = msetup.value("OnlineChecked", false).toBool();
    bool offlineMode = msetup.value("OfflineChecked", false).toBool();

    writeToTerminal(QString("\nMode values from config:"));
    writeToTerminal(QString("OnlineChecked raw value: %1").arg(msetup.value("OnlineChecked").toString()));
    writeToTerminal(QString("OfflineChecked raw value: %1").arg(msetup.value("OfflineChecked").toString()));
    writeToTerminal(QString("Mode: Online=%1, Offline=%2").arg(onlineMode).arg(offlineMode));

    if (!onlineMode && !offlineMode) {
        emit errorOccurred("Please select Online or Offline mode in Measurement Setup tab first!");
        return;
    }

    if (onlineMode) {
        // Start real-time logging
        startRealTimeLogging();
    } 
    else if (offlineMode) {
        // Read OfflineMode.cfg
        QString offlinePath = QDir(dataPath).filePath("OfflineMode.cfg");
        QSettings offline(offlinePath, QSettings::IniFormat);

        int size = offline.beginReadArray("Files");
        bool hasActivatedFile = false;
        
        writeToTerminal(QString("Found %1 files in OfflineMode.cfg").arg(size));
        
        for (int i = 0; i < size; i++) {
            offline.setArrayIndex(i);
            bool activated = offline.value("activated", false).toBool();
            if (activated) {
                hasActivatedFile = true;
                QString filePath = offline.value("path").toString();
                QString fileType = offline.value("type").toString();
                
                writeToTerminal(QString("\nProcessing file %1:").arg(i + 1));
                writeToTerminal("Path: " + filePath);
                writeToTerminal("Type: " + fileType);
                
                if (fileType == "BLF") {
                    writeToTerminal("Reading BLF file...");
                    
                    try {
                        // Read BLF file using lblf_reader
                        g_canMessages = loadCanMessagesFromBlf(filePath.toStdString());
                        
                        writeToTerminal(QString("Successfully loaded %1 CAN messages").arg(g_canMessages.size()));
                        
                        // Print first few messages for debugging
                        int debugCount = std::min(5, (int)g_canMessages.size());
                        writeToTerminal("\nFirst 5 messages:");
                        writeToTerminal("==================");
                        
                        for (int j = 0; j < debugCount; j++) {
                            const auto& msg = g_canMessages[j];
                            writeToTerminal(QString("\nMessage %1:").arg(j + 1));
                            writeToTerminal(QString("  Timestamp: %1").arg(msg.timestamp));
                            writeToTerminal(QString("  Channel: %1").arg(msg.channel));
                            writeToTerminal(QString("  CAN ID: 0x%1").arg(msg.can_id, 8, 16, QChar('0')));
                            writeToTerminal(QString("  DLC: %1").arg(msg.dlc));
                            
                            QString dataStr;
                            for (uint8_t byte : msg.data) {
                                dataStr += QString("%1 ").arg(byte, 2, 16, QChar('0'));
                            }
                            writeToTerminal("  Data: " + dataStr.trimmed());
                        }
                        
                        // Print global variable status
                        writeToTerminal(QString("\ng_canMessages status:"));
                        writeToTerminal(QString("Size: %1 messages").arg(g_canMessages.size()));
                        writeToTerminal(QString("Capacity: %1 messages").arg(g_canMessages.capacity()));
                        writeToTerminal(QString("Memory address: %1").arg((quintptr)g_canMessages.data(), 0, 16));
                        
                        emit blfFileLoaded(g_canMessages.size());
                    }
                    catch (const std::exception& e) {
                        QString error = QString("Error reading BLF file: %1").arg(e.what());
                        writeToTerminal("\nERROR: " + error);
                        emit errorOccurred(error);
                    }
                }
            }
        }
        offline.endArray();

        if (!hasActivatedFile) {
            emit errorOccurred("No file is activated in Offline Mode tab. Please activate at least one file!");
            return;
        }
    }
}

void RunButton::startRealTimeLogging() {
    writeToTerminal("Starting real-time logging...");
    // TODO: Implement real-time logging logic
    emit realTimeLoggingStarted();
}
