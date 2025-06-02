#include "LoadingOverlay.h"
#include <QPainter>
#include <QVBoxLayout>

LoadingOverlay::LoadingOverlay(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    loadingLabel = new QLabel(this);
    loadingLabel->setText("Loading Configuration...");
    loadingLabel->setStyleSheet(
        "QLabel {"
        "   color: white;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   background-color: rgba(0, 0, 0, 0);"
        "}"
    );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(loadingLabel, 0, Qt::AlignCenter);
}

void LoadingOverlay::showEvent(QShowEvent* event) {
    setGeometry(parentWidget()->geometry());
    QWidget::showEvent(event);
}

void LoadingOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 180)); // Semi-transparent black
}

void LoadingOverlay::setGeometryToMatch(const QRect& rect) {
    setGeometry(rect);
} 