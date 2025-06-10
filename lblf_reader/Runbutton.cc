#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QDir>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <vector>
#include <sstream>
#include <QCoreApplication>
#include <QString>
#include <QDialog>
#include <QMainWindow>
#include <QTreeWidget>
#include <QIcon>
#include <QSettings>
#include <QThread>

#include "blf_reader.hh"
#include "blf_structs.hh"
#include "print.hh"
#include "trace.hh"
#include "CanDataReader.hh"

QPushButton *btnToggle;
QPushButton *btnTrace ;
QPushButton *btnReload;

QTimer *timer;
QThread *readerThread = nullptr;
CanDataReader *reader = nullptr;
bool isRunning = false;
QLabel *lblInfo;
QLabel *lblMaxTime;
QLabel *lblReadSpeed;

class TraceWindow : public QMainWindow {
    Q_OBJECT
public:
    TraceWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Trace Window");
        resize(1000, 900);
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);
        QTreeWidget *treeWidget = lblf::trace::initializeApp(*centralWidget);
        lblf::trace::drawSomething(treeWidget);
        centralWidget->setLayout(layout);
        setCentralWidget(centralWidget);

        // loadWindowPosition();
    }

    ~TraceWindow() {
        saveWindowPosition();
    }

private:
    void loadWindowPosition() { //Save Pos
       
        QSettings settings("MyApp", "TraceWindow");
        QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
        this->move(pos);
    }

    void saveWindowPosition() {
        
        QSettings settings("MyApp", "TraceWindow");
        settings.setValue("pos", this->pos());
    }
};

TraceWindow *traceWindow = nullptr;

void traceRun() {
    std::cout << "Trace button clicked!" << std::endl;

    if (traceWindow) {
        traceWindow->close();
        delete traceWindow;
    }
    traceWindow = new TraceWindow();
    traceWindow->show();
}

void ReloadConf() {
    std::cout << "Reload Configs button clicked!" << std::endl;
    // Thực hiện các bước reload config
}


void loadLogFromFile() {
    QString filePath = QDir(QDir::currentPath() + "/../data").absoluteFilePath("selected_files.json");

    QFile file(filePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isNull()) {
            QJsonObject jsonObj = doc.object();
            QJsonArray selectedFilesArray = jsonObj["selected_files"].toArray();
            if (!selectedFilesArray.isEmpty()) {
                QString rawFilePath = selectedFilesArray[0].toObject()["file_path"].toString();
                QString normalizedFilePath = QDir::fromNativeSeparators(rawFilePath);
                std::cout << "File path loaded from JSON: " << normalizedFilePath.toStdString() << "\n";

                // Create and start thread
                readerThread = new QThread();
                reader = new CanDataReader();
                reader->moveToThread(readerThread);

                QObject::connect(readerThread, &QThread::started, [=]() {
                    reader->process(normalizedFilePath);
                });

                QObject::connect(reader, &CanDataReader::finished, readerThread, &QThread::quit);
                QObject::connect(reader, &CanDataReader::finished, reader, &QObject::deleteLater);
                QObject::connect(readerThread, &QThread::finished, readerThread, &QObject::deleteLater);
                QObject::connect(reader, &CanDataReader::error, [](const QString &msg) {
                    std::cerr << "Error: " << msg.toStdString() << "\n";
                });

                readerThread->start();
            } else {
                std::cout << "Không tìm thấy file trong JSON.\n";
            }
        } else {
            std::cout << "JSON không hợp lệ.\n";
        }
        file.close();
    } else {
        std::cout << "Không thể mở file selected_files.json: " << file.fileName().toStdString() << "\n";
    }
}

void toggleRun() {
    if (!isRunning) {
        btnToggle->setText("🛑");
        lblf::print::allCanMessages.clear(); // clear RAM
        loadLogFromFile();
        traceWindow = new TraceWindow();
        isRunning = true;
    } else {
        btnToggle->setText("⚡");
        isRunning = false;
    }
}

// Hàm xử lý cho nút "Trace"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("CAN Log Viewer");
    window.resize(300, 200);
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Nút "Run"
    btnToggle = new QPushButton("⚡");
    btnToggle->setFixedHeight(40);
    btnToggle->setFixedSize(40, 40);

    // Nút "Reload Configs" với biểu tượng
    btnReload = new QPushButton();
    btnReload->setIcon(QIcon("../../src/images/buno_database_database_server_storage_reload_update-256.webp"));
    btnReload->setIconSize(QSize(40, 40));
    btnReload->setFixedSize(40, 40);

    // Tạo một QHBoxLayout cho các nút
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(btnToggle);  // Thêm nút "Run"
    horizontalLayout->addWidget(btnReload);  // Thêm nút "Reload Configs"
    horizontalLayout->addStretch();

    // Thêm QHBoxLayout vào layout chính (QVBoxLayout)
    layout->addLayout(horizontalLayout);

    // Nút "Trace"
    btnTrace = new QPushButton("Trace");
    btnTrace->setFixedHeight(40);
    layout->addWidget(btnTrace);

    lblMaxTime = new QLabel("| Max Time: 0.000000 s");
    lblReadSpeed = new QLabel("| Read Speed: 0 msg/s");

    layout->addWidget(lblMaxTime);
    layout->addWidget(lblReadSpeed);

    timer = new QTimer;
    
    // Kết nối nút "Run" và "Trace"
    QObject::connect(btnReload, &QPushButton::clicked, &ReloadConf);
    QObject::connect(btnToggle, &QPushButton::clicked , &toggleRun);
    QObject::connect(btnTrace , &QPushButton::clicked  , &traceRun);
    

    window.show();
    return app.exec();
}

#include "Runbutton.moc"