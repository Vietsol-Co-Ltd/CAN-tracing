#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QMap>
#include <QPointF>
#include <QTimer>
#include <functional>

class Configuration
{
public:
    static bool loadConfig(const QString& path, QMap<QString, QPointF>& positions);
    // Nếu muốn dùng thêm 3 tham số thì tạo hàm overload khác

    static bool saveConfig(const QString& path, const QMap<QString, QPointF>& positions);
    static QString loadActiveTab(const QString& path);
    static bool saveActiveTab(const QString& path, const QString& activeTab);
    static QTimer* startAutoSaveTimer(const QString& path,
                                      std::function<QMap<QString, QPointF>()> getPositionsFunc,
                                      std::function<QString()> getActiveTabNameFunc);
};

#endif // CONFIGURATION_H
