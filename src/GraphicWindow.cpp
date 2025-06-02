#include "GraphicWindow.h"
#include <QVBoxLayout>
#include <QLabel>

GraphicWindow::GraphicWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Hello World from Graphic", this);
    layout->addWidget(label);
    setLayout(layout);
}
