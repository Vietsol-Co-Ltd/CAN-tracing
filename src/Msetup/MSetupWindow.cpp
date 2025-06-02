#include "MSetupWindow.h"
#include "ClickableLabel.h"
#include "onlineMode.h"
#include "offlineMode.h"

#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QPainter>
#include <QFile>
#include <QDir>
#include <QDebug>

MSetupWindow::MSetupWindow(QWidget *parent, QTabWidget* mainTabWidget)
    : QWidget(parent), mainTabWidget(mainTabWidget)
{
    setupUI();

    // Khởi tạo QSettings với file ../Data/MsetUp.cfg (dạng ini)
    QString configPath = QDir::currentPath() + "/../data/MsetUp.cfg";

    // Nếu file không tồn tại, tạo file rỗng
    if (!QFile::exists(configPath)) {
        QFile file(configPath);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
            qDebug() << "Created new config file:" << configPath;
        } else {
            qWarning() << "Failed to create config file:" << configPath;
        }
    }

    settings = new QSettings(configPath, QSettings::IniFormat, this);

    // Load trạng thái nút
    bool onlineChecked = settings->value("Buttons/OnlineChecked", false).toBool();
    bool offlineChecked = settings->value("Buttons/OfflineChecked", false).toBool();

    onlineButton->setChecked(onlineChecked);
    offlineButton->setChecked(offlineChecked);

    // Đồng bộ enable/disable nút theo trạng thái checked (giống handleOnlineOfflineClicked)
    onlineButton->setEnabled(!onlineChecked);
    offlineButton->setEnabled(!offlineChecked);

    // Nếu nút đã checked, gọi hàm tương ứng để bật chế độ online/offline
    if (onlineChecked) {
        startOnlineMode();
    } else if (offlineChecked) {
        startOfflineMode();
    }
}

void MSetupWindow::saveButtonStates()
{
    settings->setValue("Buttons/OnlineChecked", onlineButton->isChecked());
    settings->setValue("Buttons/OfflineChecked", offlineButton->isChecked());
    settings->sync(); // Ghi ngay ra file
    qDebug() << "Saved button states: Online =" << onlineButton->isChecked()
             << ", Offline =" << offlineButton->isChecked();
}

void MSetupWindow::setupUI()
{
    // Header trên cùng: icon folder + "Real >>" + nút Online Offline
    folderIconLabel = new ClickableLabel("📁", this);
    folderIconLabel->setFont(QFont("Segoe UI Emoji", 20));
    folderIconLabel->setGeometry(10, 10, 40, 40);

    realLabel = new ClickableLabel("Real >>", this);
    realLabel->setGeometry(10, 200, 80, 30);

    onlineButton = new QPushButton("Online", this);
    onlineButton->setGeometry(150, 100, 80, 40);

    offlineButton = new QPushButton("Offline", this);
    offlineButton->setGeometry(240, 100, 80, 40);

    // Các block phía dưới
    canStatisticsBlock = new ClickableLabel("📊 CAN Statistics", this);
    canStatisticsBlock->setGeometry(400, 70, 200, 40);
    canStatisticsBlock->setStyleSheet(
        "background-color: #d0e9ff; font-weight: bold; font-size: 16px; padding-left: 10px;");

    traceBlock = new ClickableLabel("📝 Trace", this);
    traceBlock->setGeometry(400, 115, 200, 40);
    traceBlock->setStyleSheet(
        "background-color: #d0ffd6; font-weight: bold; font-size: 16px; padding-left: 10px;");

    dataBlock = new ClickableLabel("📁 Data", this);
    dataBlock->setGeometry(400, 160, 200, 40);
    dataBlock->setStyleSheet(
        "background-color: #ffe4d6; font-weight: bold; font-size: 16px; padding-left: 10px;");

    graphicsLoggingBlock = new ClickableLabel("📉 Graphics Logging", this);
    graphicsLoggingBlock->setGeometry(400, 205, 200, 40);
    graphicsLoggingBlock->setStyleSheet(
        "background-color: #ffd6d6; font-weight: bold; font-size: 16px; padding-left: 10px;");

    onlineButton->setCheckable(true);
    offlineButton->setCheckable(true);
    connect(onlineButton, &QPushButton::clicked, this, &MSetupWindow::handleOnlineOfflineClicked);
    connect(offlineButton, &QPushButton::clicked, this, &MSetupWindow::handleOnlineOfflineClicked);
    connect(folderIconLabel, &ClickableLabel::clicked, this, &MSetupWindow::onFolderIconClicked);
    connect(realLabel, &ClickableLabel::clicked, this, &MSetupWindow::onRealLabelClicked);
    connect(canStatisticsBlock, &ClickableLabel::clicked, this, &MSetupWindow::onCanStatisticsClicked);
    connect(traceBlock, &ClickableLabel::clicked, this, &MSetupWindow::onTraceClicked);
    connect(dataBlock, &ClickableLabel::clicked, this, &MSetupWindow::onDataClicked);
    connect(graphicsLoggingBlock, &ClickableLabel::clicked, this, &MSetupWindow::onGraphicsLoggingClicked);
    }

// Hàm xử lý chung
void MSetupWindow::handleOnlineOfflineClicked(bool checked)
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn)
        return;

    if (senderBtn == onlineButton) {
        if (checked) {
            onlineButton->setEnabled(false);
            offlineButton->setEnabled(true);
            offlineButton->setChecked(false);

            startOnlineMode();  // gọi hàm trong onlineMode.cpp
        } else {
            onlineButton->setEnabled(true);
        }
    } else if (senderBtn == offlineButton) {
        if (checked) {
            offlineButton->setEnabled(false);
            onlineButton->setEnabled(true);
            onlineButton->setChecked(false);

            startOfflineMode();  // gọi hàm trong offlineMode.cpp
        } else {
            offlineButton->setEnabled(true);
        }
    }

    // Lưu trạng thái nút ngay sau khi thay đổi
    saveButtonStates();
}

QMap<QString, QPointF> MSetupWindow::getBlockPositions()
{
    QMap<QString, QPointF> positions;
    positions["canStatisticsBlock"] = canStatisticsBlock->pos();
    positions["traceBlock"] = traceBlock->pos();
    positions["dataBlock"] = dataBlock->pos();
    positions["graphicsLoggingBlock"] = graphicsLoggingBlock->pos();
    return positions;
}

void MSetupWindow::applyBlockPositions(const QMap<QString, QPointF>& positions)
{
    if (positions.contains("canStatisticsBlock"))
        canStatisticsBlock->move(positions["canStatisticsBlock"].toPoint());

    if (positions.contains("traceBlock"))
        traceBlock->move(positions["traceBlock"].toPoint());

    if (positions.contains("dataBlock"))
        dataBlock->move(positions["dataBlock"].toPoint());

    if (positions.contains("graphicsLoggingBlock"))
        graphicsLoggingBlock->move(positions["graphicsLoggingBlock"].toPoint());
    emit blockPositionChanged();
}

// Vẽ các đường nối theo sơ đồ
void MSetupWindow::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    // 1. Đường ngang từ icon folder đến đường dọc chính
    QPoint folderRight = folderIconLabel->geometry().center() + QPoint(folderIconLabel->width() / 2, 0);
    int verticalLineX = folderRight.x() + 300; // Tọa độ đường dọc chính

    painter.drawLine(folderRight, QPoint(folderRight.x() + 185, folderRight.y()));

    // 4. Khung chữ nhật bao quanh nút Online Offline
    QRect onlineOfflineRect(onlineButton->geometry().topLeft() - QPoint(5, 5),
                            offlineButton->geometry().bottomRight() + QPoint(5, 5));
    painter.drawRect(onlineOfflineRect);

    // 2. Đường dọc chính từ dưới khung Online Offline xuống dưới các block
    int verticalLineTopY = onlineOfflineRect.bottom() -52 ;
    int verticalLineBottomY = graphicsLoggingBlock->geometry().bottom() -20;

    // 3. Đường ngang từ đường dọc chính sang "Real >>"
    painter.drawLine(QPoint(verticalLineX, verticalLineTopY), QPoint(verticalLineX, verticalLineBottomY));

    QPoint realRightCenter = QPoint(realLabel->geometry().right(), realLabel->geometry().center().y());
    QPoint bottomCenter = QPoint(onlineOfflineRect.center().x(), onlineOfflineRect.bottom());
    QPoint topCenter = QPoint(onlineOfflineRect.center().x(), onlineOfflineRect.top());
    painter.drawLine(QPoint(folderRight.x() + 185, folderRight.y()), topCenter);
    painter.drawLine(QPoint(realRightCenter.x() + 145, realRightCenter.y()), bottomCenter);
    painter.drawLine(realRightCenter,QPoint(realRightCenter.x() + 145, realRightCenter.y()));

    // 5. Đường ngang nối từ đường dọc chính sang khung Online Offline
    int onlineOfflineMidY = onlineOfflineRect.center().y();
    painter.drawLine(QPoint(verticalLineX, onlineOfflineMidY), QPoint(onlineOfflineRect.right(), onlineOfflineMidY));

    // 6. Đường ngang nhỏ nối từ đường dọc chính đến từng block bên dưới
    int blockLeftX = canStatisticsBlock->geometry().left();  // bỏ -10

    QList<ClickableLabel*> blocks;
    blocks.append(static_cast<ClickableLabel*>(canStatisticsBlock));
    blocks.append(static_cast<ClickableLabel*>(traceBlock));
    blocks.append(static_cast<ClickableLabel*>(dataBlock));
    blocks.append(static_cast<ClickableLabel*>(graphicsLoggingBlock));

    for (ClickableLabel* block : blocks) {
        int y = block->geometry().center().y();
        int blockX = block->geometry().left();  // hoặc block->x()
        painter.drawLine(QPoint(verticalLineX, y), QPoint(blockX, y));
    }
}

void MSetupWindow::onFolderIconClicked() {
    qDebug() << "Folder icon clicked!";
    if (!mainTabWidget) {
        qDebug() << "mainTabWidget is null!";
        return;
    }

    for (int i = 0; i < mainTabWidget->count(); ++i) {
        qDebug() << "Tab" << i << ":" << mainTabWidget->tabText(i);
        if (mainTabWidget->tabText(i) == "Offline Mode") {
            mainTabWidget->setCurrentIndex(i);
            qDebug() << "Switched to tab Offline Mode!";
            return;
        }
    }

    qDebug() << "Tab 'Offline Mode' not found.";
}

void MSetupWindow::onRealLabelClicked() {
    qDebug() << "Real label clicked!";
}

void MSetupWindow::onCanStatisticsClicked() {
    qDebug() << "CAN Statistics block clicked!";
}

void MSetupWindow::onTraceClicked() {
    qDebug() << "Trace block clicked!";
}

void MSetupWindow::onDataClicked() {
    qDebug() << "Data block clicked!";
}

void MSetupWindow::onGraphicsLoggingClicked() {
    qDebug() << "Graphics Logging block clicked!";
}

