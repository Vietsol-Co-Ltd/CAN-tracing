#ifndef GRAPHICWINDOW_H
#define GRAPHICWINDOW_H

#include <QWidget>

class GraphicWindow : public QWidget {
    Q_OBJECT
public:
    explicit GraphicWindow(QWidget *parent = nullptr);
};

#endif // GRAPHICWINDOW_H
