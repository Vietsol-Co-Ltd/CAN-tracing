#ifndef SIGNALDECODERTHREAD_H
#define SIGNALDECODERTHREAD_H

#include <QThread>
#include <QTreeWidgetItem>
#include <QString>
#include "SharedMemory.hh" // Bao gồm lớp CanMessage mà bạn sử dụng trong dự án

namespace lblf::trace {

class SignalDecoderThread : public QThread {
    Q_OBJECT

public:
    SignalDecoderThread(QTreeWidgetItem *parentItem, const CanMessage &msg, QTreeWidget *treeWidget);
    void run() override;

signals:
    void addChildItem(QTreeWidgetItem *parent, QTreeWidgetItem *child);

private:
    QTreeWidgetItem *m_parentItem;
    const CanMessage &m_msg;
    QTreeWidget *m_treeWidget;
};

} // namespace lblf::trace

#endif // SIGNALDECODERTHREAD_H
