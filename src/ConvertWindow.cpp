#include "ConvertWindow.h"
#include "common_includes.h"

ConvertWindow::ConvertWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Hello World from Convert", this);
    layout->addWidget(label);
    setLayout(layout);
}
