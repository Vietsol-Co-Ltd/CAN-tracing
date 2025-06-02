#pragma once

#include <QDialog>
#include <QTreeWidgetItem>
#include <windows.h>
#include <QTimer>
#include <QDateTime>
#include <QCloseEvent>
#include <QMap>
#include <QVariant>
#include <QFileSystemWatcher>

#ifdef __cplusplus
extern "C" {
#endif

#include "vxlapi.h"

#ifdef __cplusplus
}
#endif

class QTreeWidget;
class QComboBox;
class QPushButton;
class QLabel;

struct ChannelInfo {
    QString network;     // Network name
    QString appChannel;  // CAN1, CAN2,...
    bool active;        // Active status
    QString hardware;   // Selected hardware
    QString status;     // Current status (Active/Inactive)
    QDateTime lastUpdated;  // Last update timestamp
    QMap<QString, QVariant> customSettings;  // Custom settings storage
};

class ChannelMappingDialog : public QDialog {
    Q_OBJECT

public:
    explicit ChannelMappingDialog(QWidget* parent = nullptr);
    bool loadAndShowDialog();
    const QMap<QString, ChannelInfo>& getChannelInfo() const { return channelInfoMap; }

signals:
    void configurationChanged();  // Signal when data changes

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void refreshChannelList();
    void applyModeFromConfig();
    void onItemChanged(QTreeWidgetItem* item);
    void onFileChanged(const QString& path);

private:
    void setupTree();
    QStringList readConfiguredChannels();
    QString resolveBusType(unsigned int busType);
    bool readNetworksFromSsetUp(const QString& dataPath);
    void updateTreeData();
    bool loadChannelConfig(const QString& dataPath);
    void saveChannelConfig();
    int getNextCANNumber();
    void setupFileWatcher();

    QLabel* loadingLabel;
    QTreeWidget* treeWidget;
    QTimer* refreshTimer;
    QStringList networkList;
    QMap<QString, QString> canNumberMap;
    QMap<QString, ChannelInfo> channelInfoMap;
    QStringList lastChannelList;
    
    // Track SsetUp.cfg changes
    QString lastSsetUpContent;
    QDateTime lastSsetUpModified;
    QFileSystemWatcher* fileWatcher;
}; 