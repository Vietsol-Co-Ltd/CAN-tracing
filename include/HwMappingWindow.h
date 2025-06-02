#pragma once

#include <QWidget>
#include <windows.h>

class ChannelMappingDialog;  // forward declaration

class HwMappingWindow : public QWidget
{
    Q_OBJECT
public:
    explicit HwMappingWindow(QWidget *parent = nullptr);

private slots:
    void showChannelMappingDialog();
    void onChannelConfigChanged();

private:
    ChannelMappingDialog* mappingDialog = nullptr;
};
