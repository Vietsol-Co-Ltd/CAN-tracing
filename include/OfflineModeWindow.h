#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QTimer>
#include <QDateTime>
#include <QCloseEvent>

class QDragEnterEvent;
class QDropEvent;

struct OfflineFile {
    QString path;
    QString name;
    QString type;
    bool exists;
    bool activated;
    QDateTime lastModified;
};

class OfflineModeWindow : public QWidget {
    Q_OBJECT

public:
    explicit OfflineModeWindow(QWidget *parent = nullptr);
    ~OfflineModeWindow() override;
    void reloadConfiguration();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshFileList();
    void removeSelectedFiles();
    void clearAllFiles();
    void onActivationChanged(int row, bool activated);

private:
    void setupUI();
    void loadConfig();
    void saveConfig();
    void addFile(const QString& filePath);
    void updateTableRow(int row, const OfflineFile& file);
    bool isValidFileType(const QString& filePath);
    
    QTableWidget* fileTable;
    QTimer* refreshTimer;
    QList<OfflineFile> files;
    
    const QStringList supportedExtensions = {".asc", ".blf", ".mdf4"};
    const int CHECK_INTERVAL = 3000; // 3 seconds
    
    enum Columns {
        COL_ACTIVATED = 0,
        COL_NAME = 1,
        COL_TYPE = 2,
        COL_PATH = 3,
        COL_LAST_MODIFIED = 4
    };
};
