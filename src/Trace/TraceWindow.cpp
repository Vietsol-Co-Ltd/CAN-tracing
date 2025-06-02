#include "TraceWindow.h"
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QDateTime>
#include <QScrollBar>

TraceWindow::TraceWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void TraceWindow::setupUI() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    // Create table widget
    messageTable = new QTableWidget(this);
    messageTable->setColumnCount(11);
    messageTable->setHorizontalHeaderLabels({
        "",             // Empty column for additional controls
        "Time",         // Timestamp
        "Δt",          // Delta time
        "Message",      // Message type/info
        "Channel",      // CAN channel
        "ID",          // CAN ID
        "Direction",    // Tx/Rx
        "DLC",         // Data Length Code
        "Data",        // CAN data bytes
        "Bus",         // CAN bus
        "Counter"      // Message counter
    });
    
    // Style the table
    messageTable->setStyleSheet(
        "QTableWidget { "
        "    background-color: white; "
        "    border: 1px solid #E0E0E0; "
        "    gridline-color: #F0F0F0; "
        "} "
        "QTableWidget::item { "
        "    padding: 5px; "
        "} "
        "QTableWidget::item:selected { "
        "    background-color: #1a73e8; "
        "    color: white; "
        "} "
        "QHeaderView::section { "
        "    background-color: #F5F5F5; "
        "    padding: 5px; "
        "    border: none; "
        "    border-bottom: 1px solid #E0E0E0; "
        "    font-weight: bold; "
        "}"
    );

    // Configure table properties
    messageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    messageTable->setSelectionMode(QAbstractItemView::SingleSelection);
    messageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    messageTable->setAlternatingRowColors(true);
    messageTable->verticalHeader()->setVisible(false);
    
    // Set column sizes
    messageTable->setColumnWidth(0, 30);   // Empty column
    messageTable->setColumnWidth(1, 150);  // Time
    messageTable->setColumnWidth(2, 80);   // Delta time
    messageTable->setColumnWidth(3, 100);  // Message
    messageTable->setColumnWidth(4, 80);   // Channel
    messageTable->setColumnWidth(5, 80);   // ID
    messageTable->setColumnWidth(6, 80);   // Direction
    messageTable->setColumnWidth(7, 50);   // DLC
    messageTable->setColumnWidth(8, 200);  // Data
    messageTable->setColumnWidth(9, 80);   // Bus
    messageTable->setColumnWidth(10, 80);  // Counter

    // Connect scroll bar signals
    connect(messageTable->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &TraceWindow::onScrollValueChanged);

    layout->addWidget(messageTable);
    setLayout(layout);
}

void TraceWindow::setMessages(const std::vector<CanMessage>& messages) {
    clear();
    allMessages = messages;
    currentMessageIndex = 0;
    
    // Load initial batch
    loadMoreMessages();
}

void TraceWindow::onScrollValueChanged(int value) {
    QScrollBar* scrollBar = messageTable->verticalScrollBar();
    
    // If we're near the bottom (within 20% of the maximum)
    if (value >= scrollBar->maximum() * 0.8) {
        loadMoreMessages();
    }
}

void TraceWindow::loadMoreMessages() {
    if (currentMessageIndex >= allMessages.size()) {
        return;  // No more messages to load
    }
    
    size_t endIndex = std::min(currentMessageIndex + BATCH_SIZE, allMessages.size());
    for (; currentMessageIndex < endIndex; ++currentMessageIndex) {
        addMessage(allMessages[currentMessageIndex]);
    }
}

void TraceWindow::addMessage(const CanMessage& msg) {
    int row = messageTable->rowCount();
    messageTable->insertRow(row);
    
    // Calculate timestamp
    QDateTime timestamp = QDateTime::fromMSecsSinceEpoch(msg.timestamp / 1000); // Convert microseconds to milliseconds
    
    // Calculate delta time (if not first message)
    QString deltaTime = row > 0 ? QString::number((msg.timestamp - lastTimestamp) / 1000.0, 'f', 3) : "0.000";
    lastTimestamp = msg.timestamp;

    // Convert CAN ID to hex string
    QString idString = QString("0x%1").arg(msg.can_id, 3, 16, QChar('0')).toUpper();
    
    // Convert data bytes to hex string
    QString dataString;
    for (int i = 0; i < msg.dlc && i < 8; i++) {
        dataString += QString("%1 ").arg(msg.data[i], 2, 16, QChar('0')).toUpper();
    }
    
    // Set items for each column
    int col = 0;
    messageTable->setItem(row, col++, new QTableWidgetItem(""));  // Empty column
    messageTable->setItem(row, col++, new QTableWidgetItem(timestamp.toString("hh:mm:ss.zzz")));
    messageTable->setItem(row, col++, new QTableWidgetItem(deltaTime));
    messageTable->setItem(row, col++, new QTableWidgetItem("Data")); // Default message type
    messageTable->setItem(row, col++, new QTableWidgetItem(QString::number(msg.channel)));
    messageTable->setItem(row, col++, new QTableWidgetItem(idString));
    messageTable->setItem(row, col++, new QTableWidgetItem("Unknown")); // Direction unknown
    messageTable->setItem(row, col++, new QTableWidgetItem(QString::number(msg.dlc)));
    messageTable->setItem(row, col++, new QTableWidgetItem(dataString.trimmed()));
    messageTable->setItem(row, col++, new QTableWidgetItem("Unknown")); // Bus unknown
    messageTable->setItem(row, col++, new QTableWidgetItem(QString::number(row + 1))); // Message counter
}

void TraceWindow::clear() {
    messageTable->setRowCount(0);
    lastTimestamp = 0;
    allMessages.clear();
    currentMessageIndex = 0;
}
