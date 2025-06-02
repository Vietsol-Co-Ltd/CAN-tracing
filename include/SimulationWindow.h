#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QMenu>
#include <QTreeWidget>
#include <QSet>
#include <QSettings>
#include <QFileDialog>
#include <QKeyEvent>
#include "ClickableLabel.h"
#include <QTreeWidgetItem>

class SimulationWindow : public QWidget {
    Q_OBJECT
public:
    explicit SimulationWindow(QWidget *parent = nullptr);
    ~SimulationWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onSystemTreeContextMenu(const QPoint &pos);
    void showRightPanelContextMenu(const QPoint &pos);
    void handleTreeItemDoubleClick(QTreeWidgetItem* item, int column);
    void saveConfig();

private:
    void SystemViewPanelContextMenu(const QPoint &pos);
    void showAddChildContextMenu(QTreeWidgetItem* item, const QPoint &pos);
    void showFixedChildren(QTreeWidgetItem* item);
    void onAddNetworkDialog();
    void loadConfig();
    void saveTreeState(QTreeWidgetItem* item, QSettings& settings, const QString& prefix);
    void loadTreeState(QTreeWidgetItem* item, QSettings& settings, const QString& prefix);
    void showNetworkHardwareMenu(QTreeWidgetItem* item, const QPoint &pos);
    void showDatabasesMenu(QTreeWidgetItem* item, const QPoint &pos);
    void showDatabaseFileMenu(QTreeWidgetItem* item, const QPoint &pos);
    void removeDatabase(QTreeWidgetItem* item);
    void updateDatabaseItemStatus(QTreeWidgetItem* item);
    void setupDatabaseCheckTimer();
    void checkAllDatabases();

    QTreeWidget* systemTree;
    QGroupBox* rightPanel;
    bool networkActiveState;
    QString configPath;
};
