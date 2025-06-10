// CanDataReader.hh
#pragma once

#include <QObject>
#include <QString>
#include <string>

class CanDataReader : public QObject {
    Q_OBJECT
public:
    explicit CanDataReader(QObject *parent = nullptr);

public slots:
    void process(const QString &filePath);

signals:
    void finished();
    void error(const QString &msg);
    void statsAvailable(double maxTime, double readSpeed);
};
