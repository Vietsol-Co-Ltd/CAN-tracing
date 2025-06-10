#ifndef TRACE_HH
#define TRACE_HH

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QLabel>

namespace lblf::trace
{
// Hàm khởi tạo giao diện và kết nối các sự kiện
QTreeWidget* initializeApp(QWidget &window);

// Hàm để vẽ dữ liệu lên QTreeWidget
void drawSomething(QTreeWidget *treeWidget);

#endif // APP_INIT_HH


}
