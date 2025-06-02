#include "Configuration.h"
#include "common_includes.h"

bool Configuration::loadConfig(const QString& path, QMap<QString, QPointF>& positions)
{
    positions.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("active_tab="))
            continue;

        // Định dạng mỗi dòng: key=x,y
        QStringList parts = line.split('=');
        if (parts.size() != 2)
            continue;

        QString key = parts[0].trimmed();
        QStringList coords = parts[1].split(',');
        if (coords.size() != 2)
            continue;

        bool okX = false, okY = false;
        double x = coords[0].toDouble(&okX);
        double y = coords[1].toDouble(&okY);
        if (okX && okY) {
            positions.insert(key, QPointF(x, y));
        }
    }
    file.close();
    return true;
}

bool Configuration::saveConfig(const QString& path, const QMap<QString, QPointF>& positions)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    for (auto it = positions.constBegin(); it != positions.constEnd(); ++it) {
        out << it.key() << "=" << it.value().x() << "," << it.value().y() << "\n";
    }
    file.close();
    return true;
}

QString Configuration::loadActiveTab(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("active_tab=")) {
            QString tabName = line.section('=', 1, 1).trimmed();
            file.close();
            return tabName;
        }
    }
    file.close();
    return QString();
}

bool Configuration::saveActiveTab(const QString& path, const QString& activeTab)
{
    // Đọc config hiện tại, lấy tất cả nội dung config ngoại trừ dòng active_tab
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QStringList lines;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.startsWith("active_tab="))
            lines.append(line);
    }
    file.close();

    // Ghi lại config kèm active_tab mới
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;

    QTextStream out(&file);
    for (const QString& l : lines)
        out << l << "\n";
    out << "active_tab=" << activeTab << "\n";

    file.close();
    return true;
}

QTimer* Configuration::startAutoSaveTimer(const QString& path,
                                          std::function<QMap<QString, QPointF>()> getPositionsFunc,
                                          std::function<QString()> getActiveTabFunc)
{
    QTimer* timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [=]() {
        QMap<QString, QPointF> positions = getPositionsFunc();
        if (!saveConfig(path, positions)) {
            qWarning() << "Failed to save config positions to" << path;
        }
        QString activeTab = getActiveTabFunc();
        if (!saveActiveTab(path, activeTab)) {
            qWarning() << "Failed to save active tab to" << path;
        }
    });
    timer->start(1000); // 1 giây
    return timer;
}
