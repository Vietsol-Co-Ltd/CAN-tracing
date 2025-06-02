//main.cpp
#include "MainWindow.h"
#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[]) {
    qRegisterMetaType<QMap<QString, QPointF>>("QMap<QString,QPointF>");
    QApplication app(argc, argv);
    MainWindow window;

    // Restore window geometry if exists
    QSettings geometrySettings(QDir::currentPath() + "/../Data/WindowGeometry.cfg", QSettings::IniFormat);
    if (geometrySettings.contains("geometry")) {
        window.setGeometry(geometrySettings.value("geometry").toRect());
    }

    // Check if we're loading a config
    QStringList args = QApplication::arguments();
    int loadConfigIndex = args.indexOf("--load-config");
    if (loadConfigIndex != -1 && loadConfigIndex + 1 < args.size()) {
        // Skip the initial config loading dialog
        window.show();
    } else {
        window.show();
    }

    return app.exec();
}