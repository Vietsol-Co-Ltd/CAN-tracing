// CanDataReader.cc
#include "CanDataReader.hh"
#include "blf_reader.hh"
#include "blf_structs.hh"
#include "print.hh"
#include <QDebug>
#include <QElapsedTimer>

CanDataReader::CanDataReader(QObject *parent) : QObject(parent) {}

void CanDataReader::process(const QString &filePath) {
    std::string pathStd = filePath.toStdString();
    lblf::blf_reader reader(pathStd);

    size_t counter = 0;
    QElapsedTimer timer;
    timer.start();

    double maxDelta = 0.0;
    uint64_t lastTimestamp = 0;

    while (reader.next()) {
        const auto data = reader.data();
        struct lblf::CanMessage_common cmm;
        uint64_t ts = 0;

        switch (data.base_header.objectType) {
        case lblf::ObjectType_e::CAN_MESSAGE: {
            struct lblf::blf_struct::CanMessage_obh can;
            lblf::read_blf_struct(data, can);
            cmm.got_data = true;
            cmm.obh = can.obh;  // Lưu trữ object header
            cmm.id = can.id;
            cmm.dlc = can.dlc;
            cmm.channel = can.channel;
            cmm.flags = can.flags;
            cmm.data = can.data;

            ts = can.obh.objectTimeStamp;

            break;
        }
        case lblf::ObjectType_e::CAN_MESSAGE2: {
            struct lblf::blf_struct::CanMessage2_obh can2;
            lblf::read_blf_struct(data, can2);
            cmm.got_data = true;
            cmm.obh = can2.obh;  // Lưu trữ object header
            cmm.id = can2.id;
            cmm.dlc = can2.dlc;
            cmm.flags = can2.flags;
            cmm.channel = can2.channel;
            cmm.data = can2.data;

            ts = can2.obh.objectTimeStamp;

            break;
        }
        default:
            break;
        }

        if (cmm.got_data) {
            lblf::print::print(std::cout, cmm);
            counter++;

            if (lastTimestamp > 0) {
                double delta = static_cast<double>(ts - lastTimestamp) / 1e6; // µs -> s
                if (delta > maxDelta) maxDelta = delta;
            }
            lastTimestamp = ts;
        }
    }

    double elapsedSeconds = timer.elapsed() / 1000.0;
    double readSpeed = (elapsedSeconds > 0.0) ? (counter / elapsedSeconds) : 0.0;

    qDebug() << "Total messages:" << counter
             << "| Max time delta:" << maxDelta
             << "| Read speed:" << readSpeed << "msg/s";

    emit statsAvailable(maxDelta, readSpeed);
    emit finished();
}