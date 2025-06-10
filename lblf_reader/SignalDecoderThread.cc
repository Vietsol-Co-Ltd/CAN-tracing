#include "SignalDecoderThread.hh"
#include "print.hh" // Đảm bảo rằng bạn đã bao gồm file header cần thiết

namespace lblf::trace {

SignalDecoderThread::SignalDecoderThread(QTreeWidgetItem *parentItem, const CanMessage &msg, QTreeWidget *treeWidget)
    : m_parentItem(parentItem), m_msg(msg), m_treeWidget(treeWidget) {}

void SignalDecoderThread::run() {
    // Xử lý tín hiệu trong thread riêng
    if (!m_msg.signalName.empty()) {
        QStringList signalList = QString::fromStdString(m_msg.signalName).split("\n");

        bool ok = false;
        int intId = QString::fromStdString(m_msg.id).remove("0x").toInt(&ok, 16);

        if (ok && lblf::print::signalDetailMap.contains(intId)) {
            const auto& signalDetails = lblf::print::signalDetailMap[intId];

            for (const auto& detail : signalDetails) {
                // Tạo child item cho signal
                QTreeWidgetItem* child = new QTreeWidgetItem();

                // Chuyển chuỗi hex (dataBytes) thành vector<uint8_t>
                std::vector<uint8_t> byteArray;
                std::istringstream iss(m_msg.dataBytes);
                std::string byteStr;
                while (iss >> byteStr) {
                    uint8_t byte = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
                    byteArray.push_back(byte);
                }

                // Decode rawValue từ byteArray
                uint64_t rawValue = 0;
                for (int i = 0; i < detail.length; ++i) {
                    int bitIndex = detail.startBit + i;
                    int byteIndex = bitIndex / 8;
                    int bitOffset = bitIndex % 8;
                    if (byteIndex < byteArray.size()) {
                        uint8_t bit = (byteArray[byteIndex] >> bitOffset) & 0x01;
                        rawValue |= (uint64_t(bit) << i);
                    }
                }

                double decodedValue = rawValue * detail.factor + detail.offset;
                QString DecodeValue = QString::number(decodedValue);

                // Chuyển sang hex (nguyên phần số)
                int intValue = static_cast<int>(decodedValue);
                QString hex = QString("0x%1").arg(intValue, 0, 16).toUpper();

                // Canh lề từng phần tử thủ công bằng leftJustified
                QString nameStr    = QString::fromStdString(detail.name).leftJustified(20, ' ');
                QString valueStr   = DecodeValue.leftJustified(12, ' ');
                QString commentStr = QString::fromStdString(detail.comment).leftJustified(25, ' ');
                QString hexStr     = hex.leftJustified(10, ' ');

                // Ghép chuỗi hiển thị dạng bảng trong một dòng
                QString childText = QString("%1 %2 %3 %4")
                    .arg(nameStr)
                    .arg(valueStr)
                    .arg(hexStr)
                    .arg(commentStr);
                    

                child->setText(0, childText);
                child->setFirstColumnSpanned(true);
                for (int col = 1; col < m_treeWidget->columnCount(); ++col)
                    child->setText(col, "");  // Ẩn text cột khác

                // Gửi tín hiệu cho main thread để thêm child vào treeWidget
                emit addChildItem(m_parentItem, child);
            }
        }
    }
}

} // namespace lblf::trace
