#pragma once

#include <QWidget>
#include <QPushButton>
#include <QTabWidget>
#include <QProgressBar>
#include "RunButton.h"
#include <QMap>
#include <QPointF>

class QPushButton;
class MSetupWindow;
class QProgressDialog;
class TraceWindow;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // Cho phép các nơi khác gọi được
    void switchToTabByName(const QString& tabName);
    void autoSaveConfig(MSetupWindow* setupTab);
    void processConfigFile(const QString& fileName, QProgressDialog& progress);

private:
    QPushButton* button;
    MSetupWindow* setupTab;
    QTabWidget* tabWidget;
    QProgressBar* progressBar;
    RunButton* runButtonHandler;
    TraceWindow* traceWindow;
};
