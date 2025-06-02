    #include "ChannelMappingDialog.h"

    #include <QVBoxLayout>
#include <QTreeWidget>
#include <QHeaderView>
    #include <QLabel>
    #include <QFile>
    #include <QTextStream>
    #include <QDebug>
#include <QSettings>
#include <QCheckBox>
#include <QComboBox>
#include <QStyleFactory>
#include <QPalette>
#include <QTimer>
#include <QColor>
#include <QHBoxLayout>
#include "vxlapi.h"
#include <QDir>
#include <QRegularExpression>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileSystemWatcher>

    ChannelMappingDialog::ChannelMappingDialog(QWidget* parent)
        : QDialog(parent)
    {
        // Initialize tracking variables
        lastSsetUpContent = "";
        lastSsetUpModified = QDateTime();

        // Initialize file watcher
        fileWatcher = new QFileSystemWatcher(this);
        connect(fileWatcher, &QFileSystemWatcher::fileChanged,
                this, &ChannelMappingDialog::onFileChanged);

        setWindowTitle("Application Channel Mapping");
        setMinimumSize(1000, 500);

        // Set modern style
        setStyle(QStyleFactory::create("Fusion"));
        
        // Set light color scheme
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
        lightPalette.setColor(QPalette::WindowText, Qt::black);
        lightPalette.setColor(QPalette::Base, Qt::white);
        lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
        lightPalette.setColor(QPalette::Text, Qt::black);
        lightPalette.setColor(QPalette::Button, Qt::white);
        lightPalette.setColor(QPalette::ButtonText, Qt::black);
        lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
        lightPalette.setColor(QPalette::HighlightedText, Qt::white);
        setPalette(lightPalette);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(10);

        // Add title label with modern style
        QLabel* titleLabel = new QLabel("Channel Mapping", this);
        titleLabel->setStyleSheet(
            "font-size: 24px; "
            "font-weight: 500; "
            "color: #1F1F1F; "
            "margin-bottom: 16px;"
        );
        layout->addWidget(titleLabel);

        // Create loading label
        loadingLabel = new QLabel("Loading configuration...", this);
        loadingLabel->setAlignment(Qt::AlignCenter);
        loadingLabel->setStyleSheet(
            "font-size: 18px; "
            "color: #666666;"
        );
        layout->addWidget(loadingLabel);

        // Tạo tree widget thay vì table
        treeWidget = new QTreeWidget(this);
        treeWidget->hide(); // Hide initially
        setupTree();
        layout->addWidget(treeWidget);

        setLayout(layout);

        // Setup refresh timer with longer interval
        refreshTimer = new QTimer(this);
        connect(refreshTimer, &QTimer::timeout, this, &ChannelMappingDialog::refreshChannelList);
        refreshTimer->start(10000); // Change to 10 seconds

        // Kết nối signal cho việc thay đổi checkbox
        connect(treeWidget, &QTreeWidget::itemChanged, 
                this, [this](QTreeWidgetItem* item, int column) {
                    if (column == 2) { // Active column
                        onItemChanged(item);
                    }
                });
    }

    bool ChannelMappingDialog::loadAndShowDialog() {
        // Hide dialog while loading
        this->setVisible(false);
        
        // Show loading state
        if (loadingLabel) {
            loadingLabel->show();
        }
        if (treeWidget) {
            treeWidget->hide();
        }

        // Get absolute path to data directory
        QString dataPath = QDir::currentPath();
        if (dataPath.endsWith("bin")) {
            dataPath = QDir(dataPath).filePath("../data");
        } else {
            dataPath = QDir(dataPath).filePath("data");
        }
        qDebug() << "Looking for config files in:" << dataPath;

        // Check if config files exist
        QDir dataDir(dataPath);
        if (!dataDir.exists()) {
            qWarning() << "Data directory does not exist at:" << dataPath;
            QMessageBox::critical(this, "Error", 
                QString("Cannot find data directory at:\n%1").arg(dataPath));
            return false;
        }

        // Check SsetUp.cfg
        QString ssetupPath = dataDir.filePath("SsetUp.cfg");
        if (!QFile::exists(ssetupPath)) {
            qWarning() << "SsetUp.cfg not found at:" << ssetupPath;
            QMessageBox::critical(this, "Error", 
                QString("Cannot find SsetUp.cfg at:\n%1").arg(ssetupPath));
            return false;
        }

        // Check ChMap.cfg
        QString chmapPath = dataDir.filePath("ChMap.cfg");
        if (!QFile::exists(chmapPath)) {
            qWarning() << "Creating new ChMap.cfg at:" << chmapPath;
        }

        // Load all data with proper error handling
        bool success = true;
        QString errorMsg;

        if (!loadChannelConfig(dataDir.path())) {
            success = false;
            errorMsg += "Failed to load channel configuration.\n";
        }

        if (!readNetworksFromSsetUp(dataDir.path())) {
            success = false;
            errorMsg += "Failed to load network configuration.\n";
        }

        if (!success) {
            QMessageBox::critical(this, "Error", 
                QString("Error loading configuration:\n%1").arg(errorMsg));
            return false;
        }

        // Update tree only once after all data is loaded
        updateTreeData();

        // Hide loading, show content
        if (loadingLabel) {
            loadingLabel->hide();
        }
        if (treeWidget) {
            treeWidget->show();
        }

        // Show dialog
        this->setVisible(true);

        // Setup file watcher after confirming files exist
        setupFileWatcher();

        return true;
    }

void ChannelMappingDialog::setupTree() {
    treeWidget->setColumnCount(6);
    treeWidget->setHeaderLabels({
        "Status",
        "Bus Channel",
        "Active",
        "Network",
        "Hardware",
        "Transceiver"
    });

    // Enable features for expand/collapse
    treeWidget->setExpandsOnDoubleClick(true);
    treeWidget->setAnimated(true);
    treeWidget->setItemsExpandable(true);

    // Style for the tree including expand/collapse indicators
    treeWidget->setStyleSheet(
        "QTreeWidget { "
        "    background-color: white; "
        "    border: 1px solid #E0E0E0; "
        "} "
        "QTreeWidget::item { "
        "    padding: 5px; "
        "    border-bottom: 1px solid #F0F0F0; "
        "} "
        "QTreeWidget::item:selected { "
        "    background-color: #1a73e8; "
        "    color: white; "
        "} "
        "QTreeWidget::item:hover { "
        "    background-color: #1a73e8; "
        "    color: black; "
        "} "
        "QHeaderView::section { "
        "    background-color: #F5F5F5; "
        "    padding: 5px; "
        "    border: none; "
        "    border-bottom: 1px solid #E0E0E0; "
        "    font-weight: bold; "
        "} "
        "QTreeWidget::branch:has-children:!has-siblings:closed,"
        "QTreeWidget::branch:closed:has-children:has-siblings {"
        "    border-image: none;"
        "    image: url(:/images/plus.png);"
        "}"
        "QTreeWidget::branch:open:has-children:!has-siblings,"
        "QTreeWidget::branch:open:has-children:has-siblings {"
        "    border-image: none;"
        "    image: url(:/images/minus.png);"
        "}"
    );

    // Set column widths
    treeWidget->header()->setSectionResizeMode(0, QHeaderView::Fixed); // Status
    treeWidget->header()->setSectionResizeMode(1, QHeaderView::Fixed); // Bus Channel
    treeWidget->header()->setSectionResizeMode(2, QHeaderView::Fixed); // Active
    treeWidget->header()->setSectionResizeMode(3, QHeaderView::Fixed); // Network
    treeWidget->header()->setSectionResizeMode(4, QHeaderView::Stretch); // Hardware
    treeWidget->header()->setSectionResizeMode(5, QHeaderView::Fixed); // Transceiver

    treeWidget->setColumnWidth(0, 120);  // Status - increased from 80 to 120
    treeWidget->setColumnWidth(1, 150); // Bus Channel
    treeWidget->setColumnWidth(2, 80);  // Active
    treeWidget->setColumnWidth(3, 150); // Network
    treeWidget->setColumnWidth(5, 100); // Transceiver

    // Center align the Status column header and content
    treeWidget->headerItem()->setTextAlignment(0, Qt::AlignCenter);
}

bool ChannelMappingDialog::readNetworksFromSsetUp(const QString& dataPath) {
    QString filePath = QDir(dataPath).filePath("SsetUp.cfg");
    QFile file(filePath);
    
    qDebug() << "Attempting to open file:" << filePath;
    
    if (!file.exists()) {
        qWarning() << "SsetUp.cfg does not exist at:" << filePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open SsetUp.cfg:" << file.errorString();
        return false;
    }

    networkList.clear();
    canNumberMap.clear();
    
    // Store network type information
    struct NetworkType {
        QString name;      // Full name from config
        QString prefix;    // Prefix for numbering
        int childCount;    // Number of child networks
        QStringList networks; // List of network names under this type
    };
    QMap<int, NetworkType> networkTypes;

    //=== Reading SsetUp.cfg ===
    QTextStream in(&file);
    
    // First pass: Read network types (level 1)
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Find network type nodes (child0, child1, etc. at root level)
        if (line.startsWith("tree\\child") && line.contains("\\text=")) {
            QStringList parts = line.split("\\");
            if (parts.size() < 3) continue;
            
            // Check if this is a level 1 node (should have exactly 3 parts before text)
            QString childPart = parts[1];
            if (!childPart.startsWith("child") || parts.count() > 4) continue;
            
            int childIndex = childPart.mid(5).toInt();
            QString text = line.split("=").last().trimmed();
            
            // Skip if not a network type node
            if (!text.contains("Network")) continue;
            
            NetworkType type;
            type.name = text;
            
            if (text.contains("CAN")) {
                type.prefix = "CAN";
            } else if (text.contains("LIN")) {
                type.prefix = "LIN";
            } else if (text.contains("FLEXRAY")) {
                type.prefix = "FLEXRAY";
            } else if (text.contains("ETHERNET")) {
                type.prefix = "ETHERNET";
            }
            
            networkTypes[childIndex] = type;
        }
        
        // Read childCount for network types
        if (line.startsWith("tree\\child") && line.contains("childCount=")) {
            QStringList parts = line.split("\\");
            if (parts.size() != 3) continue;
            
            QString childPart = parts[1];
            if (!childPart.startsWith("child")) continue;
            
            int childIndex = childPart.mid(5).toInt();
            if (!networkTypes.contains(childIndex)) continue;
            
            networkTypes[childIndex].childCount = line.split("=").last().toInt();
        }
    }

    // Second pass: Read actual networks (level 2)
    file.seek(0);
    QMap<QString, int> networkCounter;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        for (auto it = networkTypes.begin(); it != networkTypes.end(); ++it) {
            QString pattern = QString("tree\\child%1\\child").arg(it.key());
            
            if (line.startsWith(pattern) && line.contains("\\text=")) {
                QStringList parts = line.split("\\");
                // Check if this is a level 2 node
                if (parts.size() != 4) continue;
                
                QString networkName = line.split("=").last().trimmed();
                
                // Skip special folders and inactive networks
                if (networkName.startsWith("🖧") || 
                    networkName.startsWith("⚡") || 
                    networkName.contains("Nodes") ||
                    networkName.contains("Generators") ||
                    networkName.contains("blocks") ||
                    networkName.contains("Databases") ||
                    networkName.contains("Channels") ||
                    networkName.contains("(Inactive)")) {
                    continue;
                }
                
                // Initialize counter if needed
                if (!networkCounter.contains(it->prefix)) {
                    networkCounter[it->prefix] = 1;
                }
                
                // Create bus number and store network info
                QString busNumber = QString("%1%2").arg(it->prefix)
                                                 .arg(networkCounter[it->prefix]++);
                networkList.append(networkName);
                canNumberMap[networkName] = busNumber;
                it->networks.append(networkName);
            }
        }
    }

    file.close();
    return true;
}

void ChannelMappingDialog::updateTreeData() {
    treeWidget->clear();
    QMap<QString, QTreeWidgetItem*> busTypeNodes;

    // Sort networks by type
    QMap<QString, QStringList> networksByType;
    for (const QString& networkName : networkList) {
        QString busNumber = canNumberMap[networkName];
        QString busType = busNumber.left(busNumber.indexOf(QRegularExpression("\\d")));
        networksByType[busType].append(networkName);
    }

    // Get list of all available hardware channels
    QStringList availableChannels = readConfiguredChannels();
    availableChannels.prepend("None");

    // Get list of already mapped hardware
    QSet<QString> mappedHardware;
    for (const auto& info : channelInfoMap) {
        if (!info.hardware.isEmpty() && info.hardware != "None") {
            mappedHardware.insert(info.hardware);
        }
    }

    // Create tree structure
    for (auto it = networksByType.begin(); it != networksByType.end(); ++it) {
        const QString& busType = it.key();
        const QStringList& networks = it.value();

        // Create bus type node with children
        QTreeWidgetItem* busTypeNode = new QTreeWidgetItem(treeWidget);
        busTypeNode->setText(0, busType + " Network");
        busTypeNode->setFlags(busTypeNode->flags() | Qt::ItemIsAutoTristate);
        busTypeNode->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
        
        // Add all networks for this type
        for (const QString& networkName : networks) {
            QTreeWidgetItem* networkNode = new QTreeWidgetItem();
            busTypeNode->addChild(networkNode);
            
            // Status
            networkNode->setText(0, "-");

            // Bus Channel
            networkNode->setText(1, canNumberMap[networkName]);

            // Active checkbox
            QWidget* checkboxContainer = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(checkboxContainer);
            QCheckBox* checkbox = new QCheckBox();
            checkbox->setChecked(channelInfoMap[networkName].active);
            connect(checkbox, &QCheckBox::toggled, this, [this, networkName](bool checked) {
                channelInfoMap[networkName].active = checked;
                saveChannelConfig();
            });
            layout->addWidget(checkbox);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            checkboxContainer->setLayout(layout);
            treeWidget->setItemWidget(networkNode, 2, checkboxContainer);

            // Network name
            networkNode->setText(3, networkName);

            // Hardware ComboBox
            QComboBox* hwCombo = new QComboBox();
            hwCombo->addItems(availableChannels);

            // Set current hardware if configured
            QString currentHw = channelInfoMap[networkName].hardware;
            if (!currentHw.isEmpty()) {
                int index = hwCombo->findText(currentHw);
                if (index >= 0) {
                    hwCombo->setCurrentIndex(index);
                }
            }

            // Disable already mapped hardware except current selection
            QStandardItemModel* model = qobject_cast<QStandardItemModel*>(hwCombo->model());
            if (model) {
                for (int i = 0; i < hwCombo->count(); ++i) {
                    QString itemText = hwCombo->itemText(i);
                    if (itemText != "None" && itemText != currentHw && mappedHardware.contains(itemText)) {
                        QStandardItem* item = model->item(i);
                        item->setEnabled(false);
                    }
                }
            }

            connect(hwCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                    this, [this, networkName, hwCombo](int index) {
                channelInfoMap[networkName].hardware = hwCombo->currentText();
                saveChannelConfig();
                updateTreeData(); // Refresh to update disabled states
            });
            treeWidget->setItemWidget(networkNode, 4, hwCombo);

            // Transceiver (empty)
            networkNode->setText(5, "");
        }

        // Expand by default
        busTypeNode->setExpanded(true);
    }
}

bool ChannelMappingDialog::loadChannelConfig(const QString& dataPath) {
    QString filePath = QDir(dataPath).filePath("ChMap.cfg");
    QSettings settings(filePath, QSettings::IniFormat);
    
    if (settings.status() != QSettings::NoError) {
        qWarning() << "Error loading ChMap.cfg:" << settings.status();
        return false;
    }

    channelInfoMap.clear();

    // Load global settings
    settings.beginGroup("Global");
    bool wasVisible = settings.value("WindowVisible", true).toBool();
    QSize size = settings.value("WindowSize", QSize(1000, 500)).toSize();
    QPoint pos = settings.value("WindowPosition", QPoint(100, 100)).toPoint();
    settings.endGroup();

    // Restore window state
    if (wasVisible) {
        resize(size);
        move(pos);
    }

    // Load network configurations
    settings.beginGroup("Networks");
    QStringList networks = settings.childGroups();
    
    for (const QString& network : networks) {
        settings.beginGroup(network);
        ChannelInfo info;
        info.network = network;
        info.appChannel = settings.value("AppChannel").toString();
        info.active = settings.value("Active", false).toBool();
        info.hardware = settings.value("Hardware").toString();
        info.status = settings.value("Status").toString();
        info.lastUpdated = settings.value("LastUpdated", QDateTime::currentDateTime()).toDateTime();
        
        // Load any custom settings for this network
        QStringList customKeys = settings.childKeys();
        for (const QString& key : customKeys) {
            if (!key.startsWith("_")) continue; // Custom keys start with underscore
            info.customSettings[key] = settings.value(key);
        }
        
        channelInfoMap[network] = info;
        settings.endGroup();
    }
    settings.endGroup();

    qDebug() << "Configuration loaded from" << settings.fileName();
    return true;
}

void ChannelMappingDialog::saveChannelConfig() {
    // Get correct data path
    QString dataPath = QDir::currentPath();
    if (dataPath.endsWith("bin")) {
        dataPath = QDir(dataPath).filePath("../data");
    } else {
        dataPath = QDir(dataPath).filePath("data");
    }
    
    QString configPath = QDir(dataPath).filePath("ChMap.cfg");
    QSettings settings(configPath, QSettings::IniFormat);
    
    qDebug() << "Saving configuration to" << configPath;
    
    // Clear existing configuration
    settings.clear();
    
    // Save global settings
    settings.beginGroup("Global");
    settings.setValue("WindowVisible", isVisible());
    settings.setValue("WindowSize", size());
    settings.setValue("WindowPosition", pos());
    settings.setValue("LastSaved", QDateTime::currentDateTime().toString(Qt::ISODate));
    settings.endGroup();
    
    // Save network configurations
    settings.beginGroup("Networks");
    for (auto it = channelInfoMap.begin(); it != channelInfoMap.end(); ++it) {
        const QString& network = it.key();
        const ChannelInfo& info = it.value();
        
        settings.beginGroup(network);
        settings.setValue("AppChannel", info.appChannel);
        settings.setValue("Active", info.active);
        settings.setValue("Hardware", info.hardware);
        settings.setValue("Status", info.active ? "Active" : "Inactive");
        settings.setValue("LastUpdated", QDateTime::currentDateTime().toString(Qt::ISODate));
        
        // Save any custom settings
        for (auto customIt = info.customSettings.begin(); customIt != info.customSettings.end(); ++customIt) {
            settings.setValue(customIt.key(), customIt.value());
        }
        
        settings.endGroup();
    }
    settings.endGroup();

    // Ensure settings are written to disk
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qWarning() << "Error saving configuration to" << configPath;
    } else {
        qDebug() << "Configuration saved successfully to" << configPath;
        
        // Notify about changes
        emit configurationChanged();
        
        // Force refresh after save
        QTimer::singleShot(100, this, [this]() {
            refreshChannelList();
        });
    }
}

void ChannelMappingDialog::onItemChanged(QTreeWidgetItem* item) {
    if (item->treeWidget()->currentColumn() == 2) { // Active column
        saveChannelConfig();
    }
}

int ChannelMappingDialog::getNextCANNumber() {
    int maxNumber = 0;
    for (const auto& info : channelInfoMap) {
        if (info.appChannel.startsWith("CAN")) {
            int number = info.appChannel.mid(3).toInt();
            maxNumber = qMax(maxNumber, number);
        }
    }
    return maxNumber + 1;
    }

    void ChannelMappingDialog::applyModeFromConfig() {
        QFile file("../data/MsetUp.cfg");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Cannot open config file";
            return;
        }

        bool online = false, offline = false;
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("OnlineChecked="))
                online = (line.section('=', 1).toLower() == "true");
            else if (line.startsWith("OfflineChecked="))
                offline = (line.section('=', 1).toLower() == "true");
        }

        file.close();

        if (online) {
            this->setEnabled(true);
        } else if (offline) {
            this->setEnabled(false);
        } else {
            this->setEnabled(true);
        }
    }

QStringList ChannelMappingDialog::readConfiguredChannels() {
        QStringList channelList;

        XLstatus status = xlOpenDriver();
        if (status != XL_SUCCESS) {
            return QStringList() << "Cannot open Vector driver";
        }

        XLdriverConfig driverConfig;
        memset(&driverConfig, 0, sizeof(driverConfig));
        driverConfig.channelCount = XL_CONFIG_MAX_CHANNELS;

        status = xlGetDriverConfig(&driverConfig);
        if (status != XL_SUCCESS) {
            xlCloseDriver();
            return QStringList() << "Cannot get driver config";
        }

        for (unsigned int i = 0; i < driverConfig.channelCount; ++i) {
            const XLchannelConfig& ch = driverConfig.channel[i];

            QString name = QString::fromLocal8Bit(ch.name);
            QString type = resolveBusType(ch.connectedBusType);
            QString info = QString("%1 | Serial: %2 | Bus: %3 | Channel: %4")
                            .arg(name)
                            .arg(ch.serialNumber)
                            .arg(type)
                            .arg(ch.hwChannel);
            channelList.append(info);
        }

        xlCloseDriver();
        return channelList;
    }

QString ChannelMappingDialog::resolveBusType(unsigned int busType) {
        switch (busType) {
            case XL_BUS_TYPE_CAN:      return "CAN";
            case XL_BUS_TYPE_LIN:      return "LIN";
            case XL_BUS_TYPE_FLEXRAY:  return "FLEXRAY";
            case XL_BUS_TYPE_ETHERNET: return "ETHERNET";
            default:                  return "Unknown";
        }
    }

    void ChannelMappingDialog::refreshChannelList() {
        // Get data path
        QString dataPath = QDir::currentPath();
        if (dataPath.endsWith("bin")) {
            dataPath = QDir(dataPath).filePath("../data");
        } else {
            dataPath = QDir(dataPath).filePath("data");
        }

        bool needsUpdate = false;

        // Check SsetUp.cfg changes
        QString ssetupPath = QDir(dataPath).filePath("SsetUp.cfg");
        if (QFile::exists(ssetupPath)) {
            QFile file(ssetupPath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString content = file.readAll();
                file.close();

                // Force update if content changed
                if (content != lastSsetUpContent) {
                    qDebug() << "SsetUp.cfg content changed";
                    lastSsetUpContent = content;
                    needsUpdate = true;
                }
            }
        }

        // Check hardware changes
        QStringList currentChannels = readConfiguredChannels();
        if (currentChannels != lastChannelList) {
            qDebug() << "Hardware channels changed";
            lastChannelList = currentChannels;
            needsUpdate = true;
        }

        if (!needsUpdate) {
            return;
        }

        qDebug() << "Updating channel mapping...";

        // Reload all configurations
        bool configChanged = false;

        if (!readNetworksFromSsetUp(dataPath)) {
            qWarning() << "Failed to read networks from SsetUp.cfg";
        } else {
            configChanged = true;
        }

        if (!loadChannelConfig(dataPath)) {
            qWarning() << "Failed to load channel config";
        } else {
            configChanged = true;
        }

        // Force UI update
        updateTreeData();

        // Update status indicators
        QTreeWidgetItemIterator it(treeWidget);
        while (*it) {
            QTreeWidgetItem* item = *it;
            if (!item->parent()) { // Skip root items
                ++it;
                continue;
            }

            QString networkName = item->text(3); // Network column
            if (!networkName.isEmpty()) {
                // Get hardware info
                QWidget* hwWidget = treeWidget->itemWidget(item, 4);
                QComboBox* hwCombo = qobject_cast<QComboBox*>(hwWidget);
                QString selectedHw = hwCombo ? hwCombo->currentText() : "";

                // Update status indicator
                if (channelInfoMap[networkName].active && !selectedHw.isEmpty() && selectedHw != "None") {
                    item->setText(0, "✓"); // Active and configured
                } else if (channelInfoMap[networkName].active) {
                    item->setText(0, "!"); // Active but not configured
                } else {
                    item->setText(0, "-"); // Inactive
                }
            }
            ++it;
        }

        qDebug() << "Channel mapping update completed";

        // Notify about changes
        if (configChanged) {
            emit configurationChanged();
        }
    }

void ChannelMappingDialog::closeEvent(QCloseEvent* event) {
    saveChannelConfig();
    QDialog::closeEvent(event);
    }

void ChannelMappingDialog::setupFileWatcher() {
    // Get data path
    QString dataPath = QDir::currentPath();
    if (dataPath.endsWith("bin")) {
        dataPath = QDir(dataPath).filePath("../data");
    } else {
        dataPath = QDir(dataPath).filePath("data");
    }

    // Watch SsetUp.cfg
    QString ssetupPath = QDir(dataPath).filePath("SsetUp.cfg");
    if (QFile::exists(ssetupPath)) {
        fileWatcher->addPath(ssetupPath);
        qDebug() << "Started watching:" << ssetupPath;
    }
}

void ChannelMappingDialog::onFileChanged(const QString& path) {
    qDebug() << "File changed detected:" << path;
    
    // Small delay to ensure file is completely written
    QTimer::singleShot(100, this, [this, path]() {
        // Re-add the file to the watcher as it might have been recreated
        if (!fileWatcher->files().contains(path) && QFile::exists(path)) {
            fileWatcher->addPath(path);
            qDebug() << "Re-added file to watcher:" << path;
        }

        // Force refresh
        refreshChannelList();
    });
}
