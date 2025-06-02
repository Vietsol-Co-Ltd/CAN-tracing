#include "SimulationWindow.h"
#include "ClickableLabel.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QWidgetAction>
#include <QFrame>
#include <QMouseEvent>
#include <QDir>
#include <QApplication>
#include <QSplitter>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSet>
#include <QCheckBox>
#include <QInputDialog>
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>
#include <QFont>
#include <QBrush>
#include <QColor>

SimulationWindow::SimulationWindow(QWidget *parent) : QWidget(parent), networkActiveState(false) {
    // Thiết lập đường dẫn file config
    QString dataPath = QApplication::applicationDirPath() + "/../data";
    QDir().mkpath(dataPath); // Tạo thư mục data nếu chưa tồn tại
    configPath = dataPath + "/SsetUp.cfg";

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // Loại bỏ margin để splitter kéo sát cạnh

    // Tạo splitter
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(5); // Độ rộng của thanh kéo
    splitter->setChildrenCollapsible(false); // Không cho phép collapse panels

    // Panel trái - System View
    QGroupBox* systemViewPanel = new QGroupBox("🧩 System View", this);
    systemViewPanel->setStyleSheet("QGroupBox { background-color: white; }");
    QVBoxLayout* systemViewLayout = new QVBoxLayout(systemViewPanel);

    systemTree = new QTreeWidget(systemViewPanel);
    systemTree->setHeaderHidden(true);
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(systemTree);
    rootItem->setText(0, "🖧 Network");
    systemTree->addTopLevelItem(rootItem);
    systemTree->expandAll();

    // Double click để expand/collapse
    connect(systemTree, &QTreeWidget::itemDoubleClicked, this, &SimulationWindow::handleTreeItemDoubleClick);

    // Xử lý phím Delete
    systemTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(systemTree, &QTreeWidget::customContextMenuRequested, this, &SimulationWindow::onSystemTreeContextMenu);
    systemTree->installEventFilter(this);

    systemViewLayout->addWidget(systemTree);
    systemViewPanel->setLayout(systemViewLayout);

    // Panel phải - Simulation Details
    rightPanel = new QGroupBox("🧩 Simulation Details", this);
    rightPanel->setStyleSheet("QGroupBox { background-color: white; }");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    QLabel* rightLabel = new QLabel("This is the Simulation Detail panel", rightPanel);
    rightLayout->addWidget(rightLabel);
    rightPanel->setLayout(rightLayout);

    rightPanel->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(rightPanel, &QWidget::customContextMenuRequested, this, &SimulationWindow::showRightPanelContextMenu);

    // Thêm panels vào splitter
    splitter->addWidget(systemViewPanel);
    splitter->addWidget(rightPanel);

    // Set kích thước ban đầu (tỷ lệ 1:5)
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 5);

    // Thêm splitter vào layout chính
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);

    // Load cấu hình khi khởi tạo
    loadConfig();
    
    // Bắt đầu timer kiểm tra database
    setupDatabaseCheckTimer();
}

SimulationWindow::~SimulationWindow() {
    // Lưu cấu hình khi đóng window
    saveConfig();
}

void SimulationWindow::saveConfig() {
    QSettings settings(configPath, QSettings::IniFormat);
    settings.clear(); // Xóa cấu hình cũ
    settings.beginGroup("SimulationSetup");
    
    // Lưu trạng thái network
    settings.setValue("networkActive", networkActiveState);
    
    // Lưu cấu trúc cây
    QTreeWidgetItem* rootItem = systemTree->topLevelItem(0);
    if (rootItem) {
        saveTreeState(rootItem, settings, "tree");
    }
    
    settings.endGroup();
    settings.sync();

    // Kiểm tra xem file có được tạo và ghi thành công không
    QFile file(configPath);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            file.close();
        }
    } else {
        qDebug() << "Failed to save config file!";
    }
}

void SimulationWindow::loadConfig() {
    QSettings settings(configPath, QSettings::IniFormat);
    settings.beginGroup("SimulationSetup");
    
    // Load trạng thái network
    networkActiveState = settings.value("networkActive", false).toBool();
    
    // Load cấu trúc cây
    QTreeWidgetItem* rootItem = systemTree->topLevelItem(0);
    if (rootItem) {
        loadTreeState(rootItem, settings, "tree");
    }
    
    settings.endGroup();
}

void SimulationWindow::saveTreeState(QTreeWidgetItem* item, QSettings& settings, const QString& prefix) {
    if (!item) return;
    
    settings.setValue(prefix + "/text", item->text(0));
    settings.setValue(prefix + "/expanded", item->isExpanded());
    
    // Lưu đường dẫn database nếu có
    QString dbPath = item->data(0, Qt::UserRole).toString();
    if (!dbPath.isEmpty()) {
        settings.setValue(prefix + "/dbPath", dbPath);
    }
    
    int childCount = item->childCount();
    settings.setValue(prefix + "/childCount", childCount);
    
    for (int i = 0; i < childCount; i++) {
        saveTreeState(item->child(i), settings, prefix + "/child" + QString::number(i));
    }
}

void SimulationWindow::loadTreeState(QTreeWidgetItem* item, QSettings& settings, const QString& prefix) {
    if (!item) return;
    
    QString text = settings.value(prefix + "/text", item->text(0)).toString();
    bool expanded = settings.value(prefix + "/expanded", false).toBool();
    QString dbPath = settings.value(prefix + "/dbPath", "").toString();
    
    item->setText(0, text);
    item->setExpanded(expanded);
    
    if (!dbPath.isEmpty()) {
        item->setData(0, Qt::UserRole, dbPath);
        updateDatabaseItemStatus(item);
    }
    
    int childCount = settings.value(prefix + "/childCount", 0).toInt();
    
    for (int i = 0; i < childCount; i++) {
        QString childPrefix = prefix + "/child" + QString::number(i);
        if (settings.contains(childPrefix + "/text")) {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(item);
            loadTreeState(childItem, settings, childPrefix);
        }
    }
}

// Thêm event filter để bắt phím Delete
bool SimulationWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == systemTree && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete) {
            QTreeWidgetItem* currentItem = systemTree->currentItem();
            if (currentItem && currentItem->parent() && 
                currentItem->parent()->text(0) == "🖧 Databases") {
                removeDatabase(currentItem);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

void SimulationWindow::handleTreeItemDoubleClick(QTreeWidgetItem* item, int column) {
    if (!item) return;

    // Toggle expanded state khi double click
    item->setExpanded(!item->isExpanded());
    saveConfig();
}

// Thêm hàm xử lý xóa database
void SimulationWindow::removeDatabase(QTreeWidgetItem* item) {
    if (!item) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Remove Database",
                                "Are you sure you want to remove this database?",
                                QMessageBox::Yes|QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        delete item;
        saveConfig();
    }
}

// Menu nhỏ cho node con hoặc các node khác (ví dụ thêm child nhanh)
void SimulationWindow::showAddChildContextMenu(QTreeWidgetItem* item, const QPoint &pos) {
    QTreeWidgetItem* rootItem = systemTree->topLevelItem(0);
    if (!rootItem || !item) return;

    // Chỉ cho phép menu trên các node network (child0)
    bool isNetworkNode = false;
    if (item->parent() == rootItem) {
        QString text = item->text(0);
        if (text.contains("Network")) {
            isNetworkNode = true;
        }
    }

    if (!isNetworkNode) {
        return;
    }

    QMenu menu;
    
    // Add Child...
    QAction* addAction = menu.addAction("Add...");
    
    // Separator 1
    menu.addSeparator();
    
    // Activate/Deactivate All
    QAction* activateAllAction = menu.addAction("Activate All");
    QAction* deactivateAllAction = menu.addAction("Deactivate All");
    
    // Separator 2
    menu.addSeparator();
    
    // Remove All...
    QAction* removeAllAction = menu.addAction("Remove All...");
    
    QAction* selectedAction = menu.exec(systemTree->viewport()->mapToGlobal(pos));
    
    if (selectedAction == addAction) {
        bool ok;
        QString newName = QInputDialog::getText(this, "Add Child", "Enter name:", QLineEdit::Normal, "", &ok);
        if (ok && !newName.trimmed().isEmpty()) {
            QSet<QString> existingNames;
            for (int i = 0; i < item->childCount(); ++i) {
                existingNames.insert(item->child(i)->text(0));
            }
            QString finalName = newName.trimmed();
            int suffix = 1;
            while (existingNames.contains(finalName)) {
                finalName = "🖧" + newName + " (" + QString::number(suffix++) + ")";
            }
            QTreeWidgetItem* newItem = new QTreeWidgetItem(item);
            newItem->setText(0, finalName);
            item->addChild(newItem);
            
            showFixedChildren(newItem);
            item->setExpanded(true);
            saveConfig();
        }
    }
    else if (selectedAction == activateAllAction) {
        // Activate all children
        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* child = item->child(i);
            QString currentText = child->text(0);
            if (currentText.contains("(Inactive)")) {
                currentText = currentText.replace(" (Inactive)", "");
                child->setText(0, currentText);
            }
        }
        saveConfig();
    }
    else if (selectedAction == deactivateAllAction) {
        // Deactivate all children
        for (int i = 0; i < item->childCount(); ++i) {
            QTreeWidgetItem* child = item->child(i);
            QString currentText = child->text(0);
            if (!currentText.contains("(Inactive)")) {
                currentText += " (Inactive)";
                child->setText(0, currentText);
            }
        }
        saveConfig();
    }
    else if (selectedAction == removeAllAction) {
        // Confirm before removing
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Remove All", 
                                    "Are you sure you want to remove all children?",
                                    QMessageBox::Yes|QMessageBox::No);
                                    
        if (reply == QMessageBox::Yes) {
            while (item->childCount() > 0) {
                delete item->takeChild(0);
            }
            saveConfig();
        }
    }
}

// Dialog thêm network mới
void SimulationWindow::onAddNetworkDialog() {
    QTreeWidgetItem* rootItem = systemTree->topLevelItem(0);
    if (!rootItem || rootItem->text(0) != "🖧 Network") return;

    QDialog dialog(this);
    dialog.setWindowTitle("Add Network");
    dialog.setFixedSize(300, 150);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("Enter network name (e.g., FrontCAN)");

    QComboBox* busCombo = new QComboBox(&dialog);
    busCombo->addItems({"CAN", "CAN FD", "LIN", "FLEXRAY", "ETHERNET"});

    QPushButton* okButton = new QPushButton("OK", &dialog);
    QPushButton* cancelButton = new QPushButton("Cancel", &dialog);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    layout->addWidget(new QLabel("Network Name:", &dialog));
    layout->addWidget(nameEdit);
    layout->addWidget(new QLabel("Bus Type:", &dialog));
    layout->addWidget(busCombo);
    layout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, &dialog, [&]() {
        QString busType = busCombo->currentText();
        QString groupName = "🖧" + busType + " Network";
        QString newName = nameEdit->text().trimmed();

        if (newName.isEmpty()) {
            QMessageBox::warning(&dialog, "Invalid", "Name cannot be empty.");
            return;
        }

        QTreeWidgetItem* busGroupItem = nullptr;
        for (int i = 0; i < rootItem->childCount(); ++i) {
            QTreeWidgetItem* child = rootItem->child(i);
            if (child->text(0) == groupName) {
                busGroupItem = child;
                break;
            }
        }

        if (!busGroupItem) {
            busGroupItem = new QTreeWidgetItem(rootItem);
            busGroupItem->setText(0, groupName);
            rootItem->addChild(busGroupItem);
        }

        // Kiểm tra tên trùng lặp trong group
        QSet<QString> existingNames;
        for (int i = 0; i < busGroupItem->childCount(); ++i) {
            existingNames.insert(busGroupItem->child(i)->text(0));
        }

        QString finalName = newName;
        int suffix = 1;
        while (existingNames.contains(finalName)) {
            finalName = newName + " (" + QString::number(suffix++) + ")";
        }

        QTreeWidgetItem* newItem = new QTreeWidgetItem(busGroupItem);
        newItem->setText(0, finalName);
        busGroupItem->addChild(newItem);

        showFixedChildren(newItem);

        rootItem->setExpanded(true);
        busGroupItem->setExpanded(true);
        
        saveConfig(); // Lưu ngay sau khi thêm network

        dialog.accept();
    });

    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

void SimulationWindow::showRightPanelContextMenu(const QPoint &pos) {
    QMenu contextMenu;

    QWidget* customPanel = new QWidget;
    QHBoxLayout* panelLayout = new QHBoxLayout(customPanel);
    panelLayout->setContentsMargins(5, 5, 5, 5);
    panelLayout->setSpacing(10);

    // Panel trái
    QVBoxLayout* leftPanelLayout = new QVBoxLayout();
    QCheckBox* networkActiveCheckbox = new QCheckBox("Network Active");
    networkActiveCheckbox->setChecked(networkActiveState);
    connect(networkActiveCheckbox, &QCheckBox::toggled, this, [=](bool checked) {
        networkActiveState = checked;
        // TODO: xử lý state active nếu cần
    });
    leftPanelLayout->addWidget(networkActiveCheckbox);
    leftPanelLayout->addStretch();

    QWidget* leftPanelWidget = new QWidget;
    leftPanelWidget->setLayout(leftPanelLayout);
    leftPanelWidget->setFixedWidth(120);

    // Panel phải
    QVBoxLayout* rightPanelLayout = new QVBoxLayout();
    QStringList actions = {
        "Network Hardware...",
        "Network Active",
        "Open Network Panel...",
        "Add...",
        "Remote Network...",
        "Assign Channel..."
    };

    for (const QString& text : actions) {
        QPushButton* button = new QPushButton(text);
        button->setFlat(true);
        button->setStyleSheet("text-align: left; border: none;");
        button->setCursor(Qt::PointingHandCursor);

        connect(button, &QPushButton::clicked, this, [=, &contextMenu]() {
            if (text == "Add..") {
                QTreeWidgetItem* currentItem = systemTree->currentItem();
                if (currentItem) {
                    bool ok;
                    QString newName = QInputDialog::getText(this, "Add Network", "Enter network name:", QLineEdit::Normal, "", &ok);

                    if (ok && !newName.trimmed().isEmpty()) {
                        QSet<QString> existingNames;
                        for (int i = 0; i < currentItem->childCount(); ++i) {
                            existingNames.insert(currentItem->child(i)->text(0));
                        }

                        QString finalName = newName.trimmed();
                        int suffix = 1;
                        while (existingNames.contains(finalName)) {
                            finalName = newName + " (" + QString::number(suffix++) + ")";
                        }

                        QTreeWidgetItem* newItem = new QTreeWidgetItem(currentItem);
                        newItem->setText(0, finalName);
                        currentItem->addChild(newItem);
                        currentItem->setExpanded(true);
                    }
                }
            }
            contextMenu.close();
        });

        rightPanelLayout->addWidget(button);
        rightPanelLayout->addSpacing(5);
    }

    QWidget* rightPanelWidget = new QWidget;
    rightPanelWidget->setLayout(rightPanelLayout);

    panelLayout->addWidget(leftPanelWidget);
    panelLayout->addWidget(rightPanelWidget);

    contextMenu.setMinimumWidth(300);
    QWidgetAction* widgetAction = new QWidgetAction(&contextMenu);
    widgetAction->setDefaultWidget(customPanel);
    contextMenu.addAction(widgetAction);

    contextMenu.exec(rightPanel->mapToGlobal(pos));
}

// Thêm hàm mới để xử lý menu cho network hardware
void SimulationWindow::showNetworkHardwareMenu(QTreeWidgetItem* item, const QPoint &pos) {
    QMenu menu;

    // Network Hardware và Network Active
    QAction* hardwareAction = menu.addAction("    Network Hardware...");
    QAction* activateAction = menu.addAction("    Network Active");
    
    // Chỉ dùng text để hiển thị trạng thái, không dùng setCheckable
    bool isActive = !item->text(0).contains("(Inactive)");
    if (isActive) {
        activateAction->setText(QString::fromUtf8("    \u2713 Network Active"));
    }
    
    // Separator 1
    menu.addSeparator();
    
    // Module actions
    QAction* assignModuleAction = menu.addAction("    Assign Module to all Network Nodes...");
    QAction* removeModuleAction = menu.addAction("    Remove Module from all Network Nodes...");
    
    // Separator 2
    menu.addSeparator();
    
    // Panel actions
    QAction* openNetworkPanelAction = menu.addAction("    Open Network Panel...");
    QAction* openILConfigAction = menu.addAction("    Open IL Configuration...");
    
    // Separator 3
    menu.addSeparator();
    
    // Rename action
    QAction* renameAction = menu.addAction("    Rename...");
    
    // Separator 4
    menu.addSeparator();
    
    // Remove action
    QAction* removeAction = menu.addAction("    Remove Network...");
    
    // Separator 5
    menu.addSeparator();
    
    // Channel action
    QAction* assignChannelAction = menu.addAction("    Assign Channel...");

    QAction* selectedAction = menu.exec(systemTree->viewport()->mapToGlobal(pos));
    
    if (selectedAction == hardwareAction) {
        qDebug() << "Network Hardware clicked for:" << item->text(0);
        // TODO: Implement Network Hardware dialog
    }
    else if (selectedAction == activateAction) {
        QString currentText = item->text(0);
        if (currentText.contains("(Inactive)")) {
            currentText = currentText.replace(" (Inactive)", "");
            selectedAction->setText(QString::fromUtf8("    \u2713 Network Active"));
        } else {
            currentText += " (Inactive)";
            selectedAction->setText("    Network Active");
        }
        item->setText(0, currentText);
        saveConfig();
    }
    else if (selectedAction == assignModuleAction) {
        qDebug() << "    Assign Module clicked for:" << item->text(0);
        // TODO: Implement module assignment
    }
    else if (selectedAction == removeModuleAction) {
        qDebug() << "    Remove Module clicked for:" << item->text(0);
        // TODO: Implement module removal
    }
    else if (selectedAction == openNetworkPanelAction) {
        qDebug() << "    Open Network Panel clicked for:" << item->text(0);
        // TODO: Implement network panel opening
    }
    else if (selectedAction == openILConfigAction) {
        qDebug() << "    Open IL Configuration clicked for:" << item->text(0);
        // TODO: Implement IL configuration
    }
    else if (selectedAction == renameAction) {
        bool ok;
        QString newName = QInputDialog::getText(this, "Rename Network",
                                              "Enter new name:", QLineEdit::Normal,
                                              item->text(0), &ok);
        if (ok && !newName.isEmpty()) {
            item->setText(0, newName);
            saveConfig();
        }
    }
    else if (selectedAction == removeAction) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Remove Network",
                                    "Are you sure you want to remove this network?",
                                    QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            delete item;
            saveConfig();
        }
    }
    else if (selectedAction == assignChannelAction) {
        qDebug() << "Assign Channel clicked for:" << item->text(0);
        // TODO: Implement channel assignment
    }
}

void SimulationWindow::showDatabasesMenu(QTreeWidgetItem* item, const QPoint &pos) {
    QMenu menu;

    // Add menu items
    QAction* addAction = menu.addAction("    Add...");
    QAction* importAction = menu.addAction("    Import Wizard...");
    QAction* pasteAction = menu.addAction("    Paste");

    QAction* selectedAction = menu.exec(systemTree->viewport()->mapToGlobal(pos));

    if (selectedAction == addAction) {
        QFileDialog fileDialog(this);
        fileDialog.setWindowTitle("Select Database File");
        fileDialog.setFileMode(QFileDialog::ExistingFiles);
        
        QStringList filters;
        filters << "CAN Databases Files (*.xml *.axml *.dbc)"
                << "All Files (*)";
        fileDialog.setNameFilters(filters);

        if (fileDialog.exec()) {
            QStringList selectedFiles = fileDialog.selectedFiles();
            for (const QString& filePath : selectedFiles) {
                QFileInfo fileInfo(filePath);
                QString fileName = fileInfo.fileName();
                QString absolutePath = fileInfo.absoluteFilePath();

                // Kiểm tra xem file đã tồn tại trong tree chưa
                bool exists = false;
                for (int i = 0; i < item->childCount(); ++i) {
                    if (item->child(i)->data(0, Qt::UserRole).toString() == absolutePath) {
                        exists = true;
                        break;
                    }
                }

                if (!exists) {
                    QTreeWidgetItem* newItem = new QTreeWidgetItem(item);
                    newItem->setText(0, fileName);
                    newItem->setData(0, Qt::UserRole, absolutePath); // Lưu đường dẫn đầy đủ
                    updateDatabaseItemStatus(newItem); // Cập nhật trạng thái hiển thị
                    item->addChild(newItem);
                    qDebug() << "Added database file:" << fileName << "Path:" << absolutePath;
                } else {
                    QMessageBox::warning(this, "Warning", 
                        QString("Database '%1' already exists.").arg(fileName));
                }
            }
            item->setExpanded(true);
            saveConfig();
        }
    }
    else if (selectedAction == importAction) {
        qDebug() << "Import Wizard clicked";
        // TODO: Implement Import Wizard
    }
    else if (selectedAction == pasteAction) {
        qDebug() << "Paste clicked";
        // TODO: Implement Paste functionality
    }
}

// Thêm hàm mới để cập nhật trạng thái hiển thị của database item
void SimulationWindow::updateDatabaseItemStatus(QTreeWidgetItem* item) {
    if (!item) return;
    
    QString filePath = item->data(0, Qt::UserRole).toString();
    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);
    
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        // File không tồn tại hoặc đã bị di chuyển
        QFont font = item->font(0);
        font.setStrikeOut(true); // Gạch ngang
        item->setFont(0, font);
        item->setForeground(0, QColor(Qt::gray)); // Làm mờ text
        
        // Thêm tooltip để hiển thị lỗi
        item->setToolTip(0, QString("File not found: %1").arg(filePath));
        
        qDebug() << "Database file not found:" << filePath;
    } else {
        // File tồn tại - reset style về mặc định
        QFont font = item->font(0);
        font.setStrikeOut(false);
        item->setFont(0, font);
        item->setForeground(0, QBrush());
        item->setToolTip(0, filePath);
    }
}

// Thêm timer để định kỳ kiểm tra trạng thái các file
void SimulationWindow::setupDatabaseCheckTimer() {
    QTimer* checkTimer = new QTimer(this);
    connect(checkTimer, &QTimer::timeout, this, &SimulationWindow::checkAllDatabases);
    checkTimer->start(5000); // Kiểm tra mỗi 5 giây
    
    // Kiểm tra ngay lập tức lần đầu
    checkAllDatabases();
}

void SimulationWindow::checkAllDatabases() {
    QTreeWidgetItem* rootItem = systemTree->topLevelItem(0);
    if (!rootItem) return;
    
    // Tìm tất cả các node Databases trong cây
    QList<QTreeWidgetItem*> stack;
    stack.append(rootItem);
    
    while (!stack.isEmpty()) {
        QTreeWidgetItem* currentItem = stack.takeFirst();
        
        // Nếu là node Databases, kiểm tra các file con
        if (currentItem->text(0) == "🖧 Databases") {
            for (int i = 0; i < currentItem->childCount(); ++i) {
                updateDatabaseItemStatus(currentItem->child(i));
            }
        }
        
        // Thêm các node con vào stack để duyệt tiếp
        for (int i = 0; i < currentItem->childCount(); ++i) {
            stack.append(currentItem->child(i));
        }
    }
}

// Thêm menu cho database files
void SimulationWindow::onSystemTreeContextMenu(const QPoint &pos) {
    QTreeWidgetItem* clickedItem = systemTree->itemAt(pos);
    if (!clickedItem) return;

    // Nếu click vào node gốc "Network"
    if (clickedItem->parent() == nullptr && clickedItem->text(0) == "🖧 Network") {
        SystemViewPanelContextMenu(pos);
    } 
    // Nếu là node network (child0)
    else if (clickedItem->parent() && clickedItem->parent()->text(0) == "🖧 Network") {
        showAddChildContextMenu(clickedItem, pos);
    }
    // Nếu là Databases node
    else if (clickedItem->text(0) == "🖧 Databases") {
        showDatabasesMenu(clickedItem, pos);
    }
    // Nếu là database file
    else if (clickedItem->parent() && clickedItem->parent()->text(0) == "🖧 Databases") {
        showDatabaseFileMenu(clickedItem, pos);
    }
    // Nếu là node con của network (child0/child0)
    else if (clickedItem->parent() && clickedItem->parent()->parent() && 
             clickedItem->parent()->parent()->text(0) == "🖧 Network") {
        showNetworkHardwareMenu(clickedItem, pos);
    }
}

// Thêm menu cho database files
void SimulationWindow::showDatabaseFileMenu(QTreeWidgetItem* item, const QPoint &pos) {
    QMenu menu;
    
    // Add Remove action
    QAction* removeAction = menu.addAction("    Remove");
    
    QAction* selectedAction = menu.exec(systemTree->viewport()->mapToGlobal(pos));
    
    if (selectedAction == removeAction) {
        removeDatabase(item);
    }
}

// Thêm lại hàm showFixedChildren
void SimulationWindow::showFixedChildren(QTreeWidgetItem* item) {
    // Danh sách các child cố định
    QStringList fixedChildren = {
        "🖧 Nodes",
        "🖧 Interactive Generators",
        "🖧 Replay blocks",
        "🖧 Databases",
        "🖧 Channels"
    };

    // Tạo các child nếu chưa tồn tại
    QSet<QString> existingNames;
    for (int i = 0; i < item->childCount(); ++i) {
        existingNames.insert(item->child(i)->text(0));
    }

    for (const QString& childName : fixedChildren) {
        if (!existingNames.contains(childName)) {
            QTreeWidgetItem* newItem = new QTreeWidgetItem(item);
            newItem->setText(0, childName);
            item->addChild(newItem);
        }
    }

    item->setExpanded(true);
}

// Thêm lại hàm SystemViewPanelContextMenu
void SimulationWindow::SystemViewPanelContextMenu(const QPoint &pos) {
    QMenu contextMenu;

    // Các action và các dòng phân cách
    QStringList actions = {
        "    Network Hardware...",
        "    Network Active",
        "    ______________",
        "    Open Network Panel...",
        "    ______________",
        "    Add...",
        "    ______________",
        "    Remote Network...",
        "    ______________",
        "    Assign Channel..."
    };

    QVBoxLayout* rightPanelLayout = new QVBoxLayout();

    for (const QString &action : actions) {
        QString trimmed = action.trimmed();

        if (trimmed == "______________") {
            QFrame* line = new QFrame(this);
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            line->setStyleSheet("color: gray;");
            rightPanelLayout->addWidget(line);
        } else if (trimmed == "Network Active") {
            ClickableLabel* label = new ClickableLabel(trimmed, this);
            label->setCursor(Qt::PointingHandCursor);

            if (networkActiveState) {
                label->setText(QString::fromUtf8("\u2714 ") + trimmed);
                label->setStyleSheet("margin-left: 10px; font-weight: bold;");
            } else {
                label->setStyleSheet("margin-left: 10px; color: black; font-weight: normal;");
            }

            connect(label, &ClickableLabel::clicked, this, [this, label, trimmed]() {
                networkActiveState = !networkActiveState;
                if (networkActiveState) {
                    label->setText(QString::fromUtf8("\u2714 ") + trimmed);
                    label->setStyleSheet("margin-left: 10px; font-weight: bold;");
                } else {
                    label->setText(trimmed);
                    label->setStyleSheet("margin-left: 10px; color: black; font-weight: normal;");
                }
                saveConfig();
            });

            rightPanelLayout->addWidget(label);
        } else if (trimmed == "Add...") {
            // Dùng QPushButton cho Add... để dễ xử lý click
            QPushButton* btnAdd = new QPushButton(trimmed, this);
            btnAdd->setFlat(true);
            btnAdd->setStyleSheet("text-align: left; margin-left: 10px;");
            connect(btnAdd, &QPushButton::clicked, this, [this, &contextMenu]() {
                onAddNetworkDialog();
                contextMenu.close();
            });
            rightPanelLayout->addWidget(btnAdd);
        } else {
            QLabel* label = new QLabel(trimmed, this);
            label->setStyleSheet("margin-left: 10px;");
            rightPanelLayout->addWidget(label);
        }
    }

    QWidget* rightPanelWidget = new QWidget;
    rightPanelWidget->setLayout(rightPanelLayout);

    QWidgetAction* widgetAction = new QWidgetAction(&contextMenu);
    widgetAction->setDefaultWidget(rightPanelWidget);
    contextMenu.addAction(widgetAction);

    contextMenu.exec(systemTree->viewport()->mapToGlobal(pos));
}