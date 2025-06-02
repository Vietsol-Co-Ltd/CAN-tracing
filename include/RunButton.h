#pragma once

#include <QObject>
#include <vector>
#include "lblf_reader/CanMessage.hpp"

class QProcess;

// Forward declare global variable
extern std::vector<CanMessage> g_canMessages;

class RunButton : public QObject {
    Q_OBJECT

public:
    explicit RunButton(QObject* parent = nullptr);

public slots:
    void handleRunButtonClick();

signals:
    void realTimeLoggingStarted();
    void blfFileLoaded(int messageCount);
    void errorOccurred(const QString& error);

private:
    void startRealTimeLogging();
    void writeToTerminal(const QString& message);
    
    QProcess* debugTerminal {nullptr};
}; 