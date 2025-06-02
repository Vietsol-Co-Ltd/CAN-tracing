// ClickableLabel.h
#pragma once
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>

class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}

    ClickableLabel(const QString& text, QWidget* parent = nullptr) : QLabel(parent) {
        setText(text);
    }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};
