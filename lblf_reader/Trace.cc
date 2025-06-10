#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <sstream>
#include <map>
#include <tuple>
#include <vector>
#include <QHeaderView>
#include <string>
#include <QTimer>
#include <QThread>

#include "trace.hh"
#include "print.hh"
#include "SharedMemory.hh"
#include "FilterDialog.hh"
#include "SignalDecoderThread.hh"



namespace lblf::trace {

// Định nghĩa MyTreeWidgetItem để kiểm tra và sắp xếp dữ liệu theo thời gian và Δt
class MyTreeWidgetItem : public QTreeWidgetItem {
public:
    using QTreeWidgetItem::QTreeWidgetItem;

    bool operator<(const QTreeWidgetItem &other) const override {
        int column = treeWidget()->sortColumn();
        if (column == 0 || column == 1) { // Time, Δt
            bool ok1, ok2;
            double val1 = data(column, Qt::UserRole).toDouble(&ok1);
            double val2 = other.data(column, Qt::UserRole).toDouble(&ok2);
            if (ok1 && ok2)
                return val1 < val2;
        }
        return QTreeWidgetItem::operator<(other);
    }
};

// Khởi tạo giao diện chính của ứng dụng
QTreeWidget* initializeApp(QWidget &window) {
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(window.layout());

    // Top panel
    QWidget *topPanel = new QWidget;
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topPanel->setStyleSheet("background-color: lightblue;");
    topPanel->setFixedHeight(50);

    QPushButton *btnDeltaTime   = new QPushButton("Δ Time");
    QPushButton *btnCsvGen      = new QPushButton("CSV Generate");
    QPushButton *btnClearFilter = new QPushButton("Clear All Filters");

    QString buttonStyle = "QPushButton { font-size: 10pt; }";
    btnDeltaTime->setStyleSheet(buttonStyle);
    btnCsvGen->setStyleSheet(buttonStyle);
    btnClearFilter->setStyleSheet(buttonStyle);

    topLayout->addWidget(btnDeltaTime);
    topLayout->addWidget(btnCsvGen);
    topLayout->addWidget(btnClearFilter);
    topLayout->addStretch();
    mainLayout->addWidget(topPanel);

    // Bottom panel
    QWidget *bottomPanel = new QWidget;
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomPanel);
    bottomPanel->setStyleSheet("background-color: white;");
    bottomLayout->setContentsMargins(10, 10, 10, 10);

    // TreeWidget
    QTreeWidget *treeWidget = new QTreeWidget;
    treeWidget->setColumnCount(11);
    treeWidget->setHeaderLabels(QStringList() << "Time" << "Δt" << "Message" << "Channel"
                                              << "ID" << "Direction" << "DLC" << "Data"
                                              << "Bus" << "Counter" << " ");
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setRootIsDecorated(true);
    treeWidget->setUniformRowHeights(true);
    treeWidget->setStyleSheet("font-size: 10pt;");
    treeWidget->setSortingEnabled(true);

    bottomLayout->addWidget(treeWidget);

    QLabel *logLabel = new QLabel("File being read: ");
    logLabel->setWordWrap(true);
    bottomLayout->addWidget(logLabel);

    mainLayout->addWidget(bottomPanel, 1);

    // Kết nối sự kiện
    QObject::connect(btnDeltaTime, &QPushButton::clicked, [logLabel]() {
        qDebug() << "Delta Time clicked";
    });

    QObject::connect(btnCsvGen, &QPushButton::clicked, [logLabel]() {
        qDebug() << "CSV Generate clicked";
    });

    QObject::connect(btnClearFilter, &QPushButton::clicked, [logLabel]() {
        qDebug() << "Clear All Filters clicked";
    });

    return treeWidget;
}

// Hàm cập nhật dữ liệu vào QTreeWidget
void drawSomething(QTreeWidget *treeWidget) {
    std::map<std::pair<std::string, std::string>, int> idChannelCount;
    std::map<std::pair<std::string, std::string>, double> lastTimestamps;

    treeWidget->clear();

    // Lặp qua tất cả các tin nhắn CAN
    for (const auto& msg : lblf::print::allCanMessages) {
        auto key = std::make_pair(msg.channel, msg.id);
        int counter = ++idChannelCount[key];
        double timestamp = msg.timeStamp.empty() ? 0.0 : std::stod(msg.timeStamp);
        double timeInSec = timestamp / 1e9;
        double deltaTime = 0.0;

        if (counter > 1 && lastTimestamps.count(key))
            deltaTime = (timestamp - lastTimestamps[key]) / 1e9;

        lastTimestamps[key] = timestamp;

        MyTreeWidgetItem *item = new MyTreeWidgetItem(treeWidget);

        item->setText(0, QString::number(timeInSec, 'f', 6));
        item->setData(0, Qt::UserRole, timeInSec);

        item->setText(1, QString::number(deltaTime, 'f', 6));
        item->setData(1, Qt::UserRole, deltaTime);

        item->setText(2, msg.messageName.empty() ? "Unknown" : QString::fromStdString(msg.messageName));
        item->setText(3, msg.channel.empty()     ? "Unknown" : QString::fromStdString(msg.channel));
        item->setText(4, msg.id.empty()          ? "Unknown" : QString::fromStdString(msg.id));
        item->setText(5, msg.direction.empty()   ? "Unknown" : QString::fromStdString(msg.direction));
        item->setText(6, msg.dlc.empty()         ? "Unknown" : QString::fromStdString(msg.dlc));
        item->setText(7, msg.dataBytes.empty()   ? "Unknown" : QString::fromStdString(msg.dataBytes));
        item->setText(8, msg.busType.empty()     ? "Unknown" : QString::fromStdString(msg.busType));
        item->setText(9, QString::number(counter));
        item->setText(10, " ");

        // Nếu có signal, thêm thông tin chi tiết vào các child item
        if (!msg.signalName.empty()) {
            SignalDecoderThread *decoderThread = new SignalDecoderThread(item, msg, treeWidget);

            // Kết nối tín hiệu để thêm child item vào treeWidget
            QObject::connect(decoderThread, &SignalDecoderThread::addChildItem, treeWidget, [=](QTreeWidgetItem *parent, QTreeWidgetItem *child) {
                parent->addChild(child);
                child->setFirstColumnSpanned(true);
            });

            // Chạy thread để xử lý signals
            decoderThread->start();
        }
    }
    treeWidget->setSortingEnabled(true);
    treeWidget->sortItems(0, Qt::AscendingOrder);
}

//************************** The End ******************************************************************/

// Hàm cập nhật dữ liệu liên tục mỗi giây
void startUpdating(QTreeWidget *treeWidget) {
    static QTimer *timer = new QTimer(treeWidget);
    static bool isRunning = false;

    if (!isRunning) {
        QObject::connect(timer, &QTimer::timeout, [treeWidget]() {
            drawSomething(treeWidget); // Cập nhật dữ liệu vào QTreeWidget
        });
        timer->start(1000); // Cập nhật mỗi 1 giây
        isRunning = true;
    }
}

} // namespace lblf::trace

