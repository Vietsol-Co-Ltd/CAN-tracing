#pragma once

#include <QWidget>
#include <QLabel>

class LoadingOverlay : public QWidget {
    Q_OBJECT

public:
    explicit LoadingOverlay(QWidget* parent = nullptr);
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void setGeometryToMatch(const QRect& rect);

private:
    QLabel* loadingLabel;
}; 