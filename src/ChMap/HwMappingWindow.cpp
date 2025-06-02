#include "HwMappingWindow.h"
#include "ChannelMappingDialog.h"
#include "common_includes.h"

HwMappingWindow::HwMappingWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Top panel
    QHBoxLayout* topPanel = new QHBoxLayout();
    QPushButton* btnChannelUsage = new QPushButton("Channel Usage", this);
    QPushButton* btnChannelMapping = new QPushButton("Channel Mapping", this);
    QPushButton* btnNetworkHardware = new QPushButton("Network Hardware", this);

    topPanel->addWidget(btnChannelUsage);
    topPanel->addWidget(btnChannelMapping);
    topPanel->addWidget(btnNetworkHardware);
    topPanel->addStretch();

    // Connect button
    connect(btnChannelMapping, &QPushButton::clicked, this, &HwMappingWindow::showChannelMappingDialog);

    // Main label
    QLabel* label = new QLabel("Hello World from Graphic", this);

    mainLayout->addLayout(topPanel);
    mainLayout->addWidget(label);
    setLayout(mainLayout);

    // Preload ChannelMappingDialog
    QTimer::singleShot(0, this, [this]() {
        if (!mappingDialog) {
            mappingDialog = new ChannelMappingDialog(this);
            
            // Connect configuration changed signal
            connect(mappingDialog, &ChannelMappingDialog::configurationChanged,
                    this, &HwMappingWindow::onChannelConfigChanged);
            
            // Load initial data
            mappingDialog->loadAndShowDialog();
            // But keep it hidden
            mappingDialog->hide();
            
            qDebug() << "ChannelMappingDialog preloaded and signal connected";
        }
    });
}

void HwMappingWindow::showChannelMappingDialog() {
    if (!mappingDialog) {
        // If somehow dialog is not created yet, create and load it
        QLabel* loadingLabel = new QLabel("Loading Channel Mapping...", this);
        loadingLabel->setStyleSheet(
            "QLabel {"
            "   color: #666666;"
            "   font-size: 14px;"
            "   padding: 10px;"
            "   background: rgba(255, 255, 255, 0.9);"
            "   border: 1px solid #E0E0E0;"
            "   border-radius: 4px;"
            "}"
        );
        loadingLabel->setAlignment(Qt::AlignCenter);
        loadingLabel->setGeometry((width() - 200) / 2, (height() - 40) / 2, 200, 40);
        loadingLabel->raise();
        loadingLabel->show();

        QTimer::singleShot(0, this, [this, loadingLabel]() {
            mappingDialog = new ChannelMappingDialog(this);
            
            // Connect configuration changed signal
            connect(mappingDialog, &ChannelMappingDialog::configurationChanged,
                    this, &HwMappingWindow::onChannelConfigChanged);
            
            if (mappingDialog->loadAndShowDialog()) {
                mappingDialog->raise();
                mappingDialog->activateWindow();
            }
            loadingLabel->deleteLater();
        });
    } else {
        // Dialog exists, just reload data and show it
        mappingDialog->loadAndShowDialog();
        mappingDialog->raise();
        mappingDialog->activateWindow();
    }
}

void HwMappingWindow::onChannelConfigChanged() {
    qDebug() << "Channel configuration changed, updating window state...";
    
    // Get latest channel info
    const auto& channelInfo = mappingDialog->getChannelInfo();
    
    // TODO: Update your window state based on new channel info
    // For example:
    // - Update any displays showing channel status
    // - Update any related components
    // - Trigger any necessary recalculations
    
    // Force window update
    update();
}
