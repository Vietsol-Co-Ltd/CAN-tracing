#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFileDialog>
#include <QShortcut>
#include <QTabWidget>
#include <QTimer>
#include <QProgressDialog>
#include <QProcess>
#include <QApplication>
#include <QScreen>

#include "MainWindow.h"
#include "MSetupWindow.h"
#include "Configuration.h"
#include "SimulationWindow.h"
#include "OfflineModeWindow.h"
#include "TraceWindow.h"
#include "GraphicWindow.h"
#include "ConvertWindow.h"
#include "ClickableLabel.h"
#include "HwMappingWindow.h"
#include "LoadingOverlay.h"
#include "RunButton.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("CAN Tracing GUI");
    resize(800, 400);

    // --- Kiểm tra xem có đang load config không ---
    bool isLoadingConfig = QApplication::arguments().contains("--load-config");
    
    // --- Xử lý load config ---
    bool wantLoadConfig = false;
    QString defaultCfgPath = QDir::currentPath() + "/../data/MainW.cfg";
    QFile cfgFile(defaultCfgPath);
    
    if (isLoadingConfig) {
        // Nếu đang load config thì auto load luôn
        wantLoadConfig = true;
    } else if (cfgFile.exists() && cfgFile.size() > 0) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Do you want to reload lastused?");
        msgBox.setText("Bạn có muốn giữ lại cấu hình này không?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            wantLoadConfig = true;
        } else {
            // Xóa nội dung của tất cả các file .cfg trong thư mục Data
            QDir dataDir(QDir::currentPath() + "/../data");
            QStringList filters;
            filters << "*.cfg";
            QStringList cfgFiles = dataDir.entryList(filters, QDir::Files);
            
            for (const QString& file : cfgFiles) {
                QString filePath = dataDir.filePath(file);
                QFile cfgFile(filePath);
                if (cfgFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                    cfgFile.close();
                }
            }
        }
    }

    // Create RunButton handler
    runButtonHandler = new RunButton(this);

    // Nút Run/Stop toggle
    button = new QPushButton("⚡", this);

    // Nút Save và Load Config
    QPushButton* saveBtn = new QPushButton("Save Config");
    QPushButton* loadBtn = new QPushButton("Load Config");

    // --- topPanel1: Chứa các nút trên cùng ---
    QWidget* topPanel1 = new QWidget(this);
    topPanel1->setFixedHeight(60);

    QHBoxLayout* topLayout1 = new QHBoxLayout(topPanel1);
    topLayout1->setContentsMargins(5,5,5,5);
    topLayout1->setSpacing(10);

    topLayout1->addWidget(button);
    topLayout1->addWidget(saveBtn);
    topLayout1->addWidget(loadBtn);
    topLayout1->addStretch();

    // --- Tab widget ---
    tabWidget = new QTabWidget(this);

    // Measurement Setup tab dùng custom widget
    MSetupWindow* setupTab = new MSetupWindow(this, tabWidget);
    SimulationWindow* simulationTab = new SimulationWindow();
    OfflineModeWindow* offlineModeTab = new OfflineModeWindow();
    traceWindow = new TraceWindow();  // Store as member for access in other methods
    GraphicWindow* graphicTab = new GraphicWindow();
    ConvertWindow* convertTab = new ConvertWindow();
    HwMappingWindow* HwMapping = new HwMappingWindow();

    tabWidget->addTab(setupTab, "Measurement Setup");
    tabWidget->addTab(simulationTab, "Simulation");
    tabWidget->addTab(offlineModeTab, "Offline Mode");
    tabWidget->addTab(traceWindow, "Trace");
    tabWidget->addTab(graphicTab, "Graphic");
    tabWidget->addTab(convertTab, "Convert");
    tabWidget->addTab(HwMapping, "Hardware");

    // --- Progress bar ---
    progressBar = new QProgressBar(this);
    progressBar->setFixedHeight(15);
    progressBar->setFixedWidth(200);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);

    QWidget* bottomPanel = new QWidget(this);
    bottomPanel->setFixedHeight(20);

    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomPanel);
    bottomLayout->setContentsMargins(10, 0, 0, 0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(progressBar);
    bottomLayout->addStretch();

    // --- Layout chính ---
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(topPanel1);
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(bottomPanel);

    if (wantLoadConfig) {
        QMap<QString, QPointF> positions;
        if (Configuration::loadConfig(defaultCfgPath, positions)) {
            setupTab->applyBlockPositions(positions);
        }

        QFile file(defaultCfgPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            bool inMainGroup = false;

            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();

                // Nếu là dòng bắt đầu group
                if (line.startsWith('[') && line.endsWith(']')) {
                    QString groupName = line.mid(1, line.length() - 2);
                    inMainGroup = (groupName == "MainWindow");
                    continue;
                }

                // Nếu đang trong group MainWindow thì đọc active_tab
                if (inMainGroup && line.startsWith("active_tab=")) {
                    QString tabName = line.section('=', 1).trimmed();
                    switchToTabByName(tabName);
                    break; // xong thì thoát
                }
            }

            file.close();
        }
    }

    // --- Connect RunButton signals ---
    connect(runButtonHandler, &RunButton::realTimeLoggingStarted, this, [this]() {
        progressBar->setRange(0, 0); // Indeterminate progress
        button->setText("🛑");
        traceWindow->clear(); // Clear previous messages
        tabWidget->setCurrentWidget(traceWindow); // Switch to trace tab
    });

    connect(runButtonHandler, &RunButton::blfFileLoaded, this, [this](int messageCount) {
        progressBar->setRange(0, 100);
        progressBar->setValue(100);
        button->setText("🛑");
        
        // Display messages in trace window
        traceWindow->setMessages(g_canMessages);
        
        // Switch to trace tab
        tabWidget->setCurrentWidget(traceWindow);
        
        QMessageBox::information(this, "Success", 
            QString("Loaded %1 CAN messages from BLF file").arg(messageCount));
    });

    connect(runButtonHandler, &RunButton::errorOccurred, this, [this](const QString& error) {
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        button->setText("⚡");
        QMessageBox::critical(this, "Error", error);
    });

    // --- Nút toggle ---
    connect(button, &QPushButton::clicked, this, [this]() {
        if (button->text() == "⚡") {
            progressBar->setRange(0, 0); // Show indeterminate progress
            runButtonHandler->handleRunButtonClick();
        } else {
            button->setText("⚡");
            progressBar->setRange(0, 100);
            progressBar->setValue(0);
            // TODO: Implement stop logic
        }
    });

    QShortcut *saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut *loadShortcut = new QShortcut(QKeySequence("Ctrl+O"), this);

    connect(saveShortcut, &QShortcut::activated, saveBtn, &QPushButton::click);
    connect(loadShortcut, &QShortcut::activated, loadBtn, &QPushButton::click);

    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        QString fileName = QFileDialog::getSaveFileName(this,
            "Save Configuration",
            QDir::currentPath() + "/../data",
            "Configuration Files (*.cfg)");
            
        if (fileName.isEmpty()) {
            return;
        }

        // Make sure it has .cfg extension
        if (!fileName.endsWith(".cfg", Qt::CaseInsensitive)) {
            fileName += ".cfg";
        }

        QFile outputFile(fileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Error", "Could not open file for writing!");
            return;
        }

        QTextStream out(&outputFile);

        // Save MainWindow config
        out << "[MainWindow]\n";
        QMap<QString, QPointF> positions = dynamic_cast<MSetupWindow*>(tabWidget->widget(0))->getBlockPositions();
        for (auto it = positions.begin(); it != positions.end(); ++it) {
            out << it.key() << "=" << it.value().x() << "," << it.value().y() << "\n";
        }
        out << "active_tab=" << tabWidget->tabText(tabWidget->currentIndex()) << "\n\n";

        // Get all .cfg files from Data directory
        QDir dataDir(QDir::currentPath() + "/../data");
        QStringList filters;
        filters << "*.cfg";
        QStringList configFiles = dataDir.entryList(filters, QDir::Files);

        // Add full path to each file
        for (int i = 0; i < configFiles.size(); ++i) {
            configFiles[i] = dataDir.filePath(configFiles[i]);
        }

        // Process each config file
        for (const QString& cfgPath : configFiles) {
            QFile cfgFile(cfgPath);
            if (!cfgFile.exists()) continue;

            if (cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString baseName = QFileInfo(cfgPath).baseName();
                bool inGroup = false;
                QString currentGroup;
                
                QTextStream in(&cfgFile);
                while (!in.atEnd()) {
                    QString line = in.readLine().trimmed();
                    
                    if (line.isEmpty()) continue;
                    
                    // Handle group headers
                    if (line.startsWith('[') && line.endsWith(']')) {
                        if (inGroup) {
                            out << "\n"; // Add spacing between groups
                        }
                        currentGroup = line.mid(1, line.length() - 2);
                        // Prefix the group name with the config file name
                        out << "[" << baseName << "_" << currentGroup << "]\n";
                        inGroup = true;
                        continue;
                    }
                    
                    // Write content lines
                    if (inGroup) {
                        out << line << "\n";
                    }
                }
                if (inGroup) {
                    out << "\n"; // Add final spacing
                }
                cfgFile.close();
            }
        }

        outputFile.close();
        QMessageBox::information(this, "Success", "Configuration saved successfully!");
    });

    connect(loadBtn, &QPushButton::clicked, this, [=]() {
        QString fileName = QFileDialog::getOpenFileName(this,
            "Load Configuration",
            QDir::currentPath() + "/../data",
            "Configuration Files (*.cfg)");
            
        if (fileName.isEmpty()) {
            return;
        }

        // Create and show loading overlay
        LoadingOverlay* overlay = new LoadingOverlay(this);
        overlay->setGeometryToMatch(this->geometry());
        overlay->show();

        // Save current window geometry
        QSettings geometrySettings(QDir::currentPath() + "/../data/WindowGeometry.cfg", QSettings::IniFormat);
        geometrySettings.setValue("geometry", this->geometry());
        geometrySettings.sync();

        // Process the configuration file
        QProgressDialog progress("Loading configuration...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        processConfigFile(fileName, progress);

        // Start new instance with special flag after a short delay
        QTimer::singleShot(500, [=]() {
            QStringList arguments;
            arguments << "--load-config" << fileName;
            
            if (QProcess::startDetached(QApplication::applicationFilePath(), arguments)) {
                // Close current instance after a short delay to ensure new instance has started
                QTimer::singleShot(1000, [=]() {
                    QApplication::quit();
                });
            } else {
                overlay->hide();
                delete overlay;
                QMessageBox::critical(this, "Error", "Failed to start new instance!");
            }
        });
    });

    QTimer* autoSaveTimer = new QTimer(this);
    connect(autoSaveTimer, &QTimer::timeout, this, [=]() {
        // Lấy vị trí các block từ MSetupWindow instance
        QMap<QString, QPointF> positions = setupTab->getBlockPositions();

        // Đường dẫn file config mặc định
        QString path = QDir::currentPath() + "/../data/MainW.cfg";

        // Lưu cấu hình (vị trí block)
        Configuration::saveConfig(path, positions);

        // Đọc nội dung cũ
        QFile file(path);
        QString oldContent;
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            oldContent = file.readAll();
            file.close();
        }

        // Mở lại file để ghi mới, ghi [MainWindow] + nội dung cũ + active_tab mới
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QTextStream out(&file);
            out << "[MainWindow]\n";
            out << oldContent;

            // Thêm dòng active_tab mới ở cuối group
            out << "active_tab=" << tabWidget->tabText(tabWidget->currentIndex()) << "\n";

            file.close();
        }
    });
    autoSaveTimer->start(1000);
}

void MainWindow::switchToTabByName(const QString& tabName)
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == tabName) {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }
    qWarning() << "Tab name not found:" << tabName;
}

void MainWindow::processConfigFile(const QString& fileName, QProgressDialog& progress) {
    QFile inputFile(fileName);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for reading!");
        return;
    }

    // Temporary storage for each config file's content
    QMap<QString, QString> configContents;
    QString currentPrefix;
    QString currentContent;
    
    QTextStream in(&inputFile);
    int totalLines = 0;
    while (!in.atEnd()) {
        in.readLine();
        totalLines++;
    }
    in.seek(0);
    
    int currentLine = 0;
    bool inGroup = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        currentLine++;
        progress.setValue((currentLine * 100) / totalLines);
        
        if (progress.wasCanceled()) {
            inputFile.close();
            return;
        }

        // Skip empty lines
        if (line.isEmpty()) continue;

        // Handle group headers
        if (line.startsWith('[') && line.endsWith(']')) {
            // If we were in a group, save its content
            if (inGroup && !currentPrefix.isEmpty()) {
                if (configContents.contains(currentPrefix)) {
                    configContents[currentPrefix] += currentContent;
                } else {
                    configContents[currentPrefix] = currentContent;
                }
            }
            
            // Reset content for new group
            currentContent.clear();
            inGroup = true;

            // Extract group name and prefix
            QString fullGroup = line.mid(1, line.length() - 2);
            int underscorePos = fullGroup.indexOf('_');
            
            if (underscorePos > 0) {
                currentPrefix = fullGroup.left(underscorePos);
                QString groupName = fullGroup.mid(underscorePos + 1);
                currentContent = QString("[%1]\n").arg(groupName);
            } else {
                currentPrefix = fullGroup;
                currentContent = line + "\n";
            }
        } else if (inGroup) {
            // Add content line
            currentContent += line + "\n";
        }
    }
    
    // Save the last group's content
    if (inGroup && !currentPrefix.isEmpty()) {
        if (configContents.contains(currentPrefix)) {
            configContents[currentPrefix] += currentContent;
        } else {
            configContents[currentPrefix] = currentContent;
        }
    }
    
    inputFile.close();
    
    progress.setLabelText("Applying configurations...");
    progress.setValue(0);
    int processedConfigs = 0;

    // Scan data directory for all .cfg files and create if not exists
    QDir dataDir(QDir::currentPath() + "/../data");
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    // Write each config to its respective file
    for (auto it = configContents.begin(); it != configContents.end(); ++it) {
        QString configName = it.key();
        QString content = it.value();
        QString filePath = dataDir.filePath(configName + ".cfg");
        
        QFile outFile(filePath);
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outFile);
            out << content;
            outFile.close();
            qDebug() << "Wrote config to:" << filePath;
        } else {
            qWarning() << "Failed to write config to:" << filePath;
        }
        
        processedConfigs++;
        progress.setValue((processedConfigs * 100) / configContents.size());
        
        if (progress.wasCanceled()) {
            break;
        }
    }
}

MainWindow::~MainWindow() = default;
