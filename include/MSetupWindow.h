#pragma once

#include <QWidget>
#include <QMap>
#include <QTimer>
#include <QPointF>
#include <QSettings>
#include <QTabWidget>


#include "ClickableLabel.h"


class QLabel;
class QPushButton;

class MSetupWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MSetupWindow(QWidget *parent = nullptr, QTabWidget* mainTabWidget = nullptr);

    QMap<QString, QPointF> getBlockPositions();
    void applyBlockPositions(const QMap<QString, QPointF>& positions);
    
protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void blockPositionChanged();

private:
    void setupUI();
    ClickableLabel* folderIconLabel;
    ClickableLabel* realLabel;
    QPushButton* onlineButton;
    QPushButton* offlineButton;
    QTabWidget* mainTabWidget;
    QSettings* settings;
    QTimer* autoSaveTimer;

    ClickableLabel* canStatisticsBlock;
    ClickableLabel* traceBlock;
    ClickableLabel* dataBlock;
    ClickableLabel* graphicsLoggingBlock;

    void handleOnlineOfflineClicked (bool checked);
    void handleOnlineClicked(bool checked);
    void handleOfflineClicked(bool checked);
    void onFolderIconClicked();
    void onRealLabelClicked();
    void onCanStatisticsClicked();
    void onTraceClicked();
    void onDataClicked();
    void onGraphicsLoggingClicked();
    void saveButtonStates();
};
