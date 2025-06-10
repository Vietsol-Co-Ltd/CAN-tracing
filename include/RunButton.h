#pragma once

#include <QObject>
#include <QProcess>

class RunButton : public QObject
{
    Q_OBJECT

public:
    explicit RunButton(QObject* parent = nullptr);
    void handleRunButtonClick();

signals:
    void realTimeLoggingStarted();
    void blfFileLoaded(int messageCount);
    void errorOccurred(const QString& error);

private:
    void writeToTerminal(const QString& message);
};
