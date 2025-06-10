#pragma once

#include <QWidget>
#include "CanMessage.h"
#include <vector>

class QTableWidget;
class QScrollBar;

class TraceWindow : public QWidget {
    Q_OBJECT

public:
    explicit TraceWindow(QWidget *parent = nullptr);
    void startTraceLogging();
    void setMessages(const std::vector<CanMessage>& messages);
    void clear();

private slots:
    void onScrollValueChanged(int value);
    void loadMoreMessages();

private:
    void setupUI();
    void addMessage(const CanMessage& msg);
    
    QTableWidget* messageTable;
    uint64_t lastTimestamp {0};  // For calculating delta time
    
    std::vector<CanMessage> allMessages;  // Store all messages
    size_t currentMessageIndex {0};       // Index of next message to display
    static constexpr size_t BATCH_SIZE = 100;  // Number of messages to load at once
};
