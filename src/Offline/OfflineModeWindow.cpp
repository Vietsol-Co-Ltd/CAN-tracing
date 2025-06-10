#include "OfflineModeWindow.h"
#include "common_includes.h"

OfflineModeWindow::OfflineModeWindow(QWidget *parent) : QWidget(parent) {
    setAcceptDrops(true);
    setupUI();
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &OfflineModeWindow::refreshFileList);
    refreshTimer->start(CHECK_INTERVAL);
    
    loadConfig();
}

OfflineModeWindow::~OfflineModeWindow() = default;

void OfflineModeWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Create table
    fileTable = new QTableWidget(this);
    fileTable->setColumnCount(5);
    fileTable->setHorizontalHeaderLabels({
        "Active",
        "File Name", 
        "Type", 
        "Path", 
        "Last Modified"
    });
    fileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    fileTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileTable->setAlternatingRowColors(true);
    fileTable->horizontalHeader()->setSectionResizeMode(COL_ACTIVATED, QHeaderView::Fixed);
    fileTable->horizontalHeader()->setSectionResizeMode(COL_NAME, QHeaderView::Stretch);
    fileTable->horizontalHeader()->setSectionResizeMode(COL_TYPE, QHeaderView::Fixed);
    fileTable->horizontalHeader()->setSectionResizeMode(COL_PATH, QHeaderView::Stretch);
    fileTable->horizontalHeader()->setSectionResizeMode(COL_LAST_MODIFIED, QHeaderView::Fixed);
    
    fileTable->setColumnWidth(COL_ACTIVATED, 60);  // Active column
    fileTable->setColumnWidth(COL_TYPE, 80);      // Type column
    fileTable->setColumnWidth(COL_LAST_MODIFIED, 150); // Last Modified column

    // Style the table
    fileTable->setStyleSheet(
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
        "} "
    );

    // Create buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* removeButton = new QPushButton("Remove Selected", this);
    QPushButton* clearButton = new QPushButton("Clear All", this);
    
    connect(removeButton, &QPushButton::clicked, this, &OfflineModeWindow::removeSelectedFiles);
    connect(clearButton, &QPushButton::clicked, this, &OfflineModeWindow::clearAllFiles);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(removeButton);
    buttonLayout->addWidget(clearButton);

    mainLayout->addWidget(fileTable);
    mainLayout->addLayout(buttonLayout);
}

void OfflineModeWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void OfflineModeWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        bool hasValidFile = false;
        for (const QUrl& url : event->mimeData()->urls()) {
            if (isValidFileType(url.toLocalFile())) {
                hasValidFile = true;
                break;
            }
        }
        if (hasValidFile) {
            event->acceptProposedAction();
        } else {
            event->ignore();
        }
    } else {
        event->ignore();
    }
}

void OfflineModeWindow::dropEvent(QDropEvent *event) {
    bool added = false;
    for (const QUrl& url : event->mimeData()->urls()) {
        QString filePath = url.toLocalFile();
        if (isValidFileType(filePath)) {
            addFile(filePath);
            added = true;
        }
    }
    if (added) {
        saveConfig();
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void OfflineModeWindow::reloadConfiguration() {
    loadConfig();
} 

bool OfflineModeWindow::isValidFileType(const QString& filePath) {
    QString extension = QFileInfo(filePath).suffix().toLower();
    return supportedExtensions.contains("." + extension);
}

void OfflineModeWindow::addFile(const QString& filePath) {
    // Check if file already exists in the list
    for (const OfflineFile& file : files) {
        if (file.path == filePath) {
            QMessageBox::warning(this, "Warning", "File already exists in the list!");
            return;
        }
    }

    OfflineFile newFile;
    newFile.path = filePath;
    newFile.name = QFileInfo(filePath).fileName();
    newFile.type = QFileInfo(filePath).suffix().toUpper();
    newFile.exists = QFile::exists(filePath);
    newFile.activated = false; // Default to not activated
    newFile.lastModified = QFileInfo(filePath).lastModified();
    
    files.append(newFile);
    updateTableRow(files.size() - 1, newFile);
}

void OfflineModeWindow::updateTableRow(int row, const OfflineFile& file) {
    if (fileTable->rowCount() <= row) {
        fileTable->insertRow(row);
    }

    // Create checkbox for activation
    QWidget* checkboxContainer = new QWidget(fileTable);
    QHBoxLayout* layout = new QHBoxLayout(checkboxContainer);
    QCheckBox* checkbox = new QCheckBox(checkboxContainer);
    checkbox->setChecked(file.activated);
    layout->addWidget(checkbox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    checkboxContainer->setLayout(layout);

    // Connect checkbox state change
    connect(checkbox, &QCheckBox::toggled, this, [this, row](bool checked) {
        onActivationChanged(row, checked);
    });

    QTableWidgetItem* nameItem = new QTableWidgetItem(file.name);
    QTableWidgetItem* typeItem = new QTableWidgetItem(file.type);
    QTableWidgetItem* pathItem = new QTableWidgetItem(file.path);
    QTableWidgetItem* dateItem = new QTableWidgetItem(
        file.lastModified.toString("yyyy-MM-dd HH:mm:ss")
    );

    if (!file.exists) {
        QFont font = nameItem->font();
        font.setStrikeOut(true);
        nameItem->setFont(font);
        typeItem->setFont(font);
        pathItem->setFont(font);
        dateItem->setFont(font);
        
        nameItem->setForeground(Qt::gray);
        typeItem->setForeground(Qt::gray);
        pathItem->setForeground(Qt::gray);
        dateItem->setForeground(Qt::gray);
        
        // Disable checkbox if file doesn't exist
        checkbox->setEnabled(false);
    }

    fileTable->setCellWidget(row, COL_ACTIVATED, checkboxContainer);
    fileTable->setItem(row, COL_NAME, nameItem);
    fileTable->setItem(row, COL_TYPE, typeItem);
    fileTable->setItem(row, COL_PATH, pathItem);
    fileTable->setItem(row, COL_LAST_MODIFIED, dateItem);
}

void OfflineModeWindow::onActivationChanged(int row, bool activated) {
    if (row >= 0 && row < files.size()) {
        files[row].activated = activated;
        saveConfig();
    }
}

void OfflineModeWindow::refreshFileList() {
    bool changed = false;
    for (int i = 0; i < files.size(); ++i) {
        bool exists = QFile::exists(files[i].path);
        if (exists != files[i].exists) {
            files[i].exists = exists;
            changed = true;
        }
        if (exists) {
            QDateTime lastMod = QFileInfo(files[i].path).lastModified();
            if (lastMod != files[i].lastModified) {
                files[i].lastModified = lastMod;
                changed = true;
            }
        }
        updateTableRow(i, files[i]);
    }
    if (changed) {
        saveConfig();
    }
}

void OfflineModeWindow::removeSelectedFiles() {
    QList<int> selectedRows;
    for (QTableWidgetItem* item : fileTable->selectedItems()) {
        selectedRows.append(item->row());
    }
    
    // Remove duplicates using QSet and sort in descending order
    QSet<int> uniqueRows(selectedRows.begin(), selectedRows.end());
    selectedRows = QList<int>(uniqueRows.begin(), uniqueRows.end());
    std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());
    
    for (int row : selectedRows) {
        fileTable->removeRow(row);
        files.removeAt(row);
    }
    saveConfig();
}

void OfflineModeWindow::clearAllFiles() {
    fileTable->setRowCount(0);
    files.clear();
    saveConfig();
}

void OfflineModeWindow::loadConfig() {
    QSettings settings("../data/OfflineMode.cfg", QSettings::IniFormat);
    files.clear();
    
    int size = settings.beginReadArray("Files");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        OfflineFile file;
        file.path = settings.value("path").toString();
        file.name = settings.value("name").toString();
        file.type = settings.value("type").toString();
        file.exists = QFile::exists(file.path);
        file.activated = settings.value("activated", false).toBool();
        file.lastModified = settings.value("lastModified").toDateTime();
        files.append(file);
        updateTableRow(i, file);
    }
    settings.endArray();
}

void OfflineModeWindow::saveConfig() {
    QSettings settings("../data/OfflineMode.cfg", QSettings::IniFormat);
    settings.clear();
    
    settings.beginWriteArray("Files");
    for (int i = 0; i < files.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("path", files[i].path);
        settings.setValue("name", files[i].name);
        settings.setValue("type", files[i].type);
        settings.setValue("activated", files[i].activated);
        settings.setValue("lastModified", files[i].lastModified);
    }
    settings.endArray();
}
