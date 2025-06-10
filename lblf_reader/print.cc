#include "print.hh"
#include "blf_structs.hh"
#include <iomanip>
#include <sstream>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

using SignalDetail = lblf::blf_struct::SignalDetail;
#include "SharedMemory.hh"


namespace lblf {
namespace print {
    std::vector<CanMessage> allCanMessages; // <- đây là nơi cấp phát bộ nhớ thật
    std::map<int, std::vector<lblf::blf_struct::SignalDetail>> signalDetailMap;
    QJsonObject globalDbcJson;
}
}

namespace lblf::print
{


auto print(ObjectType_e ot) -> std::string
{
    switch (ot)
        {
        case ObjectType_e::UNKNOWN:
            return "UNKNOWN";
        case ObjectType_e::CAN_MESSAGE:
            return "CAN_MESSAGE";
        case ObjectType_e::CAN_ERROR:
            return "CAN_ERROR";
        case ObjectType_e::CAN_OVERLOAD:
            return "CAN_OVERLOAD";
        case ObjectType_e::CAN_STATISTIC:
            return "CAN_STATISTIC";
        case ObjectType_e::APP_TRIGGER:
            return "APP_TRIGGER";
        case ObjectType_e::ENV_INTEGER:
            return "ENV_INTEGER";
        case ObjectType_e::ENV_DOUBLE:
            return "ENV_DOUBLE";
        case ObjectType_e::ENV_STRING:
            return "ENV_STRING";
        case ObjectType_e::ENV_DATA:
            return "ENV_DATA";
        case ObjectType_e::LOG_CONTAINER:
            return "LOG_CONTAINER";
        case ObjectType_e::LIN_MESSAGE:
            return "LIN_MESSAGE";
        case ObjectType_e::LIN_CRC_ERROR:
            return "LIN_CRC_ERROR";
        case ObjectType_e::LIN_DLC_INFO:
            return "LIN_DLC_INFO";
        case ObjectType_e::LIN_RCV_ERROR:
            return "LIN_RCV_ERROR";
        case ObjectType_e::LIN_SND_ERROR:
            return "LIN_SND_ERROR";
        case ObjectType_e::LIN_SLV_TIMEOUT:
            return "LIN_SLV_TIMEOUT";
        case ObjectType_e::LIN_SCHED_MODCH:
            return "LIN_SCHED_MODCH";
        case ObjectType_e::LIN_SYN_ERROR:
            return "LIN_SYN_ERROR";
        case ObjectType_e::LIN_BAUDRATE:
            return "LIN_BAUDRATE";
        case ObjectType_e::LIN_SLEEP:
            return "LIN_SLEEP";
        case ObjectType_e::LIN_WAKEUP:
            return "LIN_WAKEUP";
        case ObjectType_e::MOST_SPY:
            return "MOST_SPY";
        case ObjectType_e::MOST_CTRL:
            return "MOST_CTRL";
        case ObjectType_e::MOST_LIGHTLOCK:
            return "MOST_LIGHTLOCK";
        case ObjectType_e::MOST_STATISTIC:
            return "MOST_STATISTIC";
        case ObjectType_e::reserved_1:
            return "reserved_1";
        case ObjectType_e::reserved_2:
            return "reserved_2";
        case ObjectType_e::reserved_3:
            return "reserved_3";
        case ObjectType_e::FLEXRAY_DATA:
            return "FLEXRAY_DATA";
        case ObjectType_e::FLEXRAY_SYNC:
            return "FLEXRAY_SYNC";
        case ObjectType_e::CAN_DRIVER_ERROR:
            return "CAN_DRIVER_ERROR";
        case ObjectType_e::MOST_PKT:
            return "MOST_PKT";
        case ObjectType_e::MOST_PKT2:
            return "MOST_PKT2";
        case ObjectType_e::MOST_HWMODE:
            return "MOST_HWMODE";
        case ObjectType_e::MOST_REG:
            return "MOST_REG";
        case ObjectType_e::MOST_GENREG:
            return "MOST_GENREG";
        case ObjectType_e::MOST_NETSTATE:
            return "MOST_NETSTATE";
        case ObjectType_e::MOST_DATALOST:
            return "MOST_DATALOST";
        case ObjectType_e::MOST_TRIGGER:
            return "MOST_TRIGGER";
        case ObjectType_e::FLEXRAY_CYCLE:
            return "FLEXRAY_CYCLE";
        case ObjectType_e::FLEXRAY_MESSAGE:
            return "FLEXRAY_MESSAGE";
        case ObjectType_e::LIN_CHECKSUM_INFO:
            return "LIN_CHECKSUM_INFO";
        case ObjectType_e::LIN_SPIKE_EVENT:
            return "LIN_SPIKE_EVENT";
        case ObjectType_e::CAN_DRIVER_SYNC:
            return "CAN_DRIVER_SYNC";
        case ObjectType_e::FLEXRAY_STATUS:
            return "FLEXRAY_STATUS";
        case ObjectType_e::GPS_EVENT:
            return "GPS_EVENT";
        case ObjectType_e::FR_ERROR:
            return "FR_ERROR";
        case ObjectType_e::FR_STATUS:
            return "FR_STATUS";
        case ObjectType_e::FR_STARTCYCLE:
            return "FR_STARTCYCLE";
        case ObjectType_e::FR_RCVMESSAGE:
            return "FR_RCVMESSAGE";
        case ObjectType_e::REALTIMECLOCK:
            return "REALTIMECLOCK";
        case ObjectType_e::AVAILABLE2:
            return "AVAILABLE2";
        case ObjectType_e::AVAILABLE3:
            return "AVAILABLE3";
        case ObjectType_e::LIN_STATISTIC:
            return "LIN_STATISTIC";
        case ObjectType_e::J1708_MESSAGE:
            return "J1708_MESSAGE";
        case ObjectType_e::J1708_VIRTUAL_MSG:
            return "J1708_VIRTUAL_MSG";
        case ObjectType_e::LIN_MESSAGE2:
            return "LIN_MESSAGE2";
        case ObjectType_e::LIN_SND_ERROR2:
            return "LIN_SND_ERROR2";
        case ObjectType_e::LIN_SYN_ERROR2:
            return "LIN_SYN_ERROR2";
        case ObjectType_e::LIN_CRC_ERROR2:
            return "LIN_CRC_ERROR2";
        case ObjectType_e::LIN_RCV_ERROR2:
            return "LIN_RCV_ERROR2";
        case ObjectType_e::LIN_WAKEUP2:
            return "LIN_WAKEUP2";
        case ObjectType_e::LIN_SPIKE_EVENT2:
            return "LIN_SPIKE_EVENT2";
        case ObjectType_e::LIN_LONG_DOM_SIG:
            return "LIN_LONG_DOM_SIG";
        case ObjectType_e::APP_TEXT:
            return "APP_TEXT";
        case ObjectType_e::FR_RCVMESSAGE_EX:
            return "FR_RCVMESSAGE_EX";
        case ObjectType_e::MOST_STATISTICEX:
            return "MOST_STATISTICEX";
        case ObjectType_e::MOST_TXLIGHT:
            return "MOST_TXLIGHT";
        case ObjectType_e::MOST_ALLOCTAB:
            return "MOST_ALLOCTAB";
        case ObjectType_e::MOST_STRESS:
            return "MOST_STRESS";
        case ObjectType_e::ETHERNET_FRAME:
            return "ETHERNET_FRAME";
        case ObjectType_e::SYS_VARIABLE:
            return "SYS_VARIABLE";
        case ObjectType_e::CAN_ERROR_EXT:
            return "CAN_ERROR_EXT";
        case ObjectType_e::CAN_DRIVER_ERROR_EXT:
            return "CAN_DRIVER_ERROR_EXT";
        case ObjectType_e::LIN_LONG_DOM_SIG2:
            return "LIN_LONG_DOM_SIG2";
        case ObjectType_e::MOST_150_MESSAGE:
            return "MOST_150_MESSAGE";
        case ObjectType_e::MOST_150_PKT:
            return "MOST_150_PKT";
        case ObjectType_e::MOST_ETHERNET_PKT:
            return "MOST_ETHERNET_PKT";
        case ObjectType_e::MOST_150_MESSAGE_FRAGMENT:
            return "MOST_150_MESSAGE_FRAGMENT";
        case ObjectType_e::MOST_150_PKT_FRAGMENT:
            return "MOST_150_PKT_FRAGMENT";
        case ObjectType_e::MOST_ETHERNET_PKT_FRAGMENT:
            return "MOST_ETHERNET_PKT_FRAGMENT";
        case ObjectType_e::MOST_SYSTEM_EVENT:
            return "MOST_SYSTEM_EVENT";
        case ObjectType_e::MOST_150_ALLOCTAB:
            return "MOST_150_ALLOCTAB";
        case ObjectType_e::MOST_50_MESSAGE:
            return "MOST_50_MESSAGE";
        case ObjectType_e::MOST_50_PKT:
            return "MOST_50_PKT";
        case ObjectType_e::CAN_MESSAGE2:
            return "CAN_MESSAGE2";
        case ObjectType_e::LIN_UNEXPECTED_WAKEUP:
            return "LIN_UNEXPECTED_WAKEUP";
        case ObjectType_e::LIN_SHORT_OR_SLOW_RESPONSE:
            return "LIN_SHORT_OR_SLOW_RESPONSE";
        case ObjectType_e::LIN_DISTURBANCE_EVENT:
            return "LIN_DISTURBANCE_EVENT";
        case ObjectType_e::SERIAL_EVENT:
            return "SERIAL_EVENT";
        case ObjectType_e::OVERRUN_ERROR:
            return "OVERRUN_ERROR";
        case ObjectType_e::EVENT_COMMENT:
            return "EVENT_COMMENT";
        case ObjectType_e::WLAN_FRAME:
            return "WLAN_FRAME";
        case ObjectType_e::WLAN_STATISTIC:
            return "WLAN_STATISTIC";
        case ObjectType_e::MOST_ECL:
            return "MOST_ECL";
        case ObjectType_e::GLOBAL_MARKER:
            return "GLOBAL_MARKER";
        case ObjectType_e::AFDX_FRAME:
            return "AFDX_FRAME";
        case ObjectType_e::AFDX_STATISTIC:
            return "AFDX_STATISTIC";
        case ObjectType_e::KLINE_STATUSEVENT:
            return "KLINE_STATUSEVENT";
        case ObjectType_e::CAN_FD_MESSAGE:
            return "CAN_FD_MESSAGE";
        case ObjectType_e::CAN_FD_MESSAGE_64:
            return "CAN_FD_MESSAGE_64";
        case ObjectType_e::ETHERNET_RX_ERROR:
            return "ETHERNET_RX_ERROR";
        case ObjectType_e::ETHERNET_STATUS:
            return "ETHERNET_STATUS";
        case ObjectType_e::CAN_FD_ERROR_64:
            return "CAN_FD_ERROR_64";
        case ObjectType_e::LIN_SHORT_OR_SLOW_RESPONSE2:
            return "LIN_SHORT_OR_SLOW_RESPONSE2";
        case ObjectType_e::AFDX_STATUS:
            return "AFDX_STATUS";
        case ObjectType_e::AFDX_BUS_STATISTIC:
            return "AFDX_BUS_STATISTIC";
        case ObjectType_e::reserved_4:
            return "reserved_4";
        case ObjectType_e::AFDX_ERROR_EVENT:
            return "AFDX_ERROR_EVENT";
        case ObjectType_e::A429_ERROR:
            return "A429_ERROR";
        case ObjectType_e::A429_STATUS:
            return "A429_STATUS";
        case ObjectType_e::A429_BUS_STATISTIC:
            return "A429_BUS_STATISTIC";
        case ObjectType_e::A429_MESSAGE:
            return "A429_MESSAGE";
        case ObjectType_e::ETHERNET_STATISTIC:
            return "ETHERNET_STATISTIC";
        case ObjectType_e::reserved_5:
            return "reserved_5";
        case ObjectType_e::reserved_6:
            return "reserved_6";
        case ObjectType_e::reserved_7:
            return "reserved_7";
        case ObjectType_e::TEST_STRUCTURE:
            return "TEST_STRUCTURE";
        case ObjectType_e::DIAG_REQUEST_INTERPRETATION:
            return "DIAG_REQUEST_INTERPRETATION";
        case ObjectType_e::ETHERNET_FRAME_EX:
            return "ETHERNET_FRAME_EX";
        case ObjectType_e::ETHERNET_FRAME_FORWARDED:
            return "ETHERNET_FRAME_FORWARDED";
        case ObjectType_e::ETHERNET_ERROR_EX:
            return "ETHERNET_ERROR_EX";
        case ObjectType_e::ETHERNET_ERROR_FORWARDED:
            return "ETHERNET_ERROR_FORWARDED";
        case ObjectType_e::FUNCTION_BUS:
            return "FUNCTION_BUS";
        case ObjectType_e::DATA_LOST_BEGIN:
            return "DATA_LOST_BEGIN";
        case ObjectType_e::DATA_LOST_END:
            return "DATA_LOST_END";
        case ObjectType_e::WATER_MARK_EVENT:
            return "WATER_MARK_EVENT";
        case ObjectType_e::TRIGGER_CONDITION:
            return "TRIGGER_CONDITION";
        }
    return "missing Object Type";
}


auto print(AppId_e ai) -> std::string
{
    switch (ai)
        {
        case AppId_e::UNKNOWN:
            return "Unknown";
        case AppId_e::CANALYZER:
            return "CANALYZER";
        case AppId_e::CANOE:
            return "CANOE";
        case AppId_e::CANSTRESS:
            return "CANSTRESS";
        case AppId_e::CANLOG:
            return "CANLOG";
        case AppId_e::CANAPE:
            return "CANAPE";
        case AppId_e::CANCASEXLLOG:
            return "CANCASEXLLOG";
        case AppId_e::VLCONFIG:
            return "VLCONFIG";
        case AppId_e::PORSCHELOGGER:
            return "PORSCHELOGGER";
        }
    return "Undeclared AppId: " + std::to_string(static_cast<int>(ai));
}


std::string print(compressionMethod_e cm)
{
    switch (cm)
        {
        case compressionMethod_e::uncompressed:
            return "Uncompressed";
        case compressionMethod_e::zlib:
            return "zlib";
        }
    return "Undeclared AppId: ";
}


auto print(blf_struct::ObjectFlags_e of) -> std::string
{
    switch (of)
        {
        case blf_struct::ObjectFlags_e::TimeTenMics:
            return "Time Ten Microseconds";
        case blf_struct::ObjectFlags_e::TimeNano:
            return "Time in Nanoseconds";
        }
    return "Unknown Timeflags";
}


auto print(blf_struct::timeStampStatus_e tss) -> std::string
{
    switch (tss)
        {
        case blf_struct::timeStampStatus_e::original:
            return "Original";
        case blf_struct::timeStampStatus_e::SwGen:
            return "Software Generated";
        case blf_struct::timeStampStatus_e::User:
            return "User defined";
        }
    return "Unknown time stamp status.";
}


void print(std::ostream &stream, const blf_struct::sysTime_t &ts)
{
    stream << std::dec;
    stream << "year: " << (int) ts.year;
    stream << " month: " << (int) ts.month;
    stream << " dayOfWeek : " << (int) ts.dayOfWeek;
    stream << " day :" << (int) ts.day;
    stream << " hour :" << (int) ts.hour;
    stream << " minute :" << (int) ts.minute;
    stream << " second :" << (int) ts.second;
    stream << " milliseconds :" << (int) ts.milliseconds;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::fileStatistics &os)
{
    stream << "os.FileSign           " << std::hex << static_cast<uint64_t>(os.FileSign) << '\n';
    stream << "os.StatSize           " << static_cast<uint64_t>(os.StatSize);
    stream << " dec: " << std::dec << static_cast<uint64_t>(os.StatSize) << '\n';
    stream << "os.AppId              " << print(os.AppId);
    stream << " " << static_cast<uint64_t>(os.AppId) << '\n';
    stream << "os.AppMaj             " << static_cast<uint64_t>(os.AppMaj) << '\n';
    stream << "os.AppMin             " << static_cast<uint64_t>(os.AppMin) << '\n';
    stream << "os.AppBuild           " << static_cast<uint64_t>(os.AppBuild) << '\n';
    stream << "os.ApiMaj             " << static_cast<uint64_t>(os.ApiMaj) << '\n';
    stream << "os.ApiMin             " << static_cast<uint64_t>(os.ApiMin) << '\n';
    stream << "os.ApiBuild           " << static_cast<uint64_t>(os.ApiBuild) << '\n';
    stream << "os.ApiPatch           " << static_cast<uint64_t>(os.ApiPatch) << '\n';
    stream << "os.fileSize           " << static_cast<uint64_t>(os.fileSize) << '\n';
    stream << "os.uncompressedSize   " << static_cast<uint64_t>(os.uncompressedSize) << '\n';
    stream << "os.objCount           " << static_cast<uint64_t>(os.objCount) << '\n';
    stream << "os.applicationBuild   " << static_cast<uint64_t>(os.applicationBuild) << '\n';
    stream << "os.meas_start_time:   ";
    print(stream, os.meas_start_time);
    stream << "os.last_obj_time:     ";
    print(stream, os.last_obj_time);
    stream << "os.fileSize_less115   " << static_cast<uint64_t>(os.fileSize_less115) << '\n';
}


void print(std::ostream &stream, const blf_struct::BaseHeader &ohb)
{
    stream << "BaseHeader ";
    // s << "ObjSign " << std::hex << (int)ohb.ObjSign;
    // s << std::dec;
    stream << std::hex;
    stream << " headerSize 0x" << std::hex << (int) ohb.headerSize;
    stream << std::dec << "(" << (int) ohb.headerSize << ")";
    stream << ", headerVer 0x" << std::hex << (int) ohb.headerVer;
    stream << std::dec << "(" << (int) ohb.headerVer << ")";
    stream << ", objSize 0x" << std::hex << (int) ohb.objSize;
    stream << std::dec << "(" << (int) ohb.objSize << ")";
    stream << ", objectType 0x" << std::hex << static_cast<int>(ohb.objectType);
    stream << std::dec << "(" << (int) ohb.objectType << ")";
    stream << ", " << print(ohb.objectType);
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::ObjectHeader2 &oh2)
{
    stream << "ObjectHeader2: ";
    stream << std::dec;
    stream << "objectFlags " << std::hex << (int) oh2.objectFlags;
    stream << ", timeStampStatus " << (int) oh2.timeStampStatus;
    stream << ", reservObjHeader " << (int) oh2.reservObjHeader;
    stream << ", ObjectTimeStamp " << (int) oh2.ObjectTimeStamp;
    stream << ", originalObjectTimeStamp " << (int) oh2.originalObjectTimeStamp;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::ObjectHeader &oh)
{
    stream << "ObjectHeader : ";
    stream << std::dec;
    stream << "objectFlag: " << (int) oh.clientIndex;
    stream << ", objectVersion: " << std::dec << oh.objectVersion;
    stream << ", objectTimeStamp: " << std::dec << oh.objectTimeStamp;
    stream << '\n';
}


void print(std::ostream &stream, const ObjectHeaderCarry &ohc)
{
    switch (ohc.oh_enum)
        {
        case ObjectHeaders_e::NONE:
            stream << "No Header Carry defined\n";
            break;
        case ObjectHeaders_e::ONLY_HEADER_BASE:
            print(stream, ohc.ohb);
            break;
        case ObjectHeaders_e::BASE_AND_HEADER:
            print(stream, ohc.ohb);
            print(stream, ohc.oh);
            break;
        case ObjectHeaders_e::BASE_AND_HEADER2:
            print(stream, ohc.ohb);
            print(stream, ohc.oh2);
            break;
        }
}


void print(std::ostream &stream, const blf_struct::LogContainer &lc)
{
    stream << "LogContainer : ";
    stream << std::dec;
    stream << "compressionMethod: " << print(lc.compressionMethod);
    stream << ", uncompressedFileSize: " << std::dec << (uint32_t) lc.unCompressedFileSize;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanMessage &cm)
{
    stream << "CanMessage : ";
    stream << std::dec;
    stream << "channel: " << (int) cm.channel;
    stream << ", flags: " << std::dec << (int) cm.flags;
    stream << ", dlc: " << std::dec << (int) cm.dlc;
    stream << ", id: 0x" << std::hex << (int) cm.id;
    stream << ", data: ";
    for (auto d: cm.data)
        {
            stream << " " << std::hex << std::setfill('0') << std::setw(2) << (int) d;
        }
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanOverload &co)
{
    stream << "CanOverload : ";
    stream << std::dec;
    stream << "channel: " << (int) co.channel;
    stream << ", reservedCanOverloadFrame1: " << std::hex << (int) co.reservedCanOverloadFrame1;
    stream << ", reservedCanOverloadFrame2: " << std::hex << (int) co.reservedCanOverloadFrame2;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanOverload_short &co)
{
    stream << "CanOverload : ";
    stream << std::dec;
    stream << "channel: " << (int) co.channel;
    stream << ", reservedCanOverloadFrame1: " << std::hex << (int) co.reservedCanOverloadFrame1;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanMessage2 &cm2)
{
    stream << "CanMessage2 : ";
    stream << "channel: " << std::dec << (int) cm2.channel;
    stream << ", flags: " << std::dec << (int) cm2.flags;
    stream << ", dlc: " << std::dec << (int) cm2.dlc;
    stream << ", id: 0x" << std::hex << (int) cm2.id;
    stream << ", data: ";
    for (auto d: cm2.data)
        {
            stream << " " << std::hex << std::setfill('0') << std::setw(2) << (int) d;
        }
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanMessage2_obh &cm2)
{
    stream << "CanMessage2 : ";
    stream << "channel: " << std::dec << (int) cm2.channel;
    stream << ", flags: " << std::dec << (int) cm2.flags;
    stream << ", dlc: " << std::dec << (int) cm2.dlc;
    stream << ", id: 0x" << std::hex << (int) cm2.id;
    stream << ", data: ";
    for (size_t i = 0; i < cm2.dlc; ++i)
        {
            stream << " " << std::hex << std::setfill('0') << std::setw(2) << (int) cm2.data.at(i);
        }
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::AppTrigger &at)
{
    stream << "Apptrigger: ";
    stream << std::dec;
    stream << "preTriggerTime: " << (uint64_t) at.preTriggerTime;
    stream << ", postTriggerTime : " << (uint64_t) at.postTriggerTime;
    stream << ", channel: " << (uint64_t) at.channel;
    stream << ", flags: " << (uint64_t) at.flags;
    stream << ", appSpecific2: " << (uint64_t) at.appSpecific2;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::AppText &at)
{
    stream << "AppText: ";
    stream << std::dec;
    stream << "mSource: " << (uint64_t) at.mSource;
    stream << ", mTextLength: " << (uint64_t) at.mTextLength;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::reserved_5 &r)
{
    stream << "Reserved_5: ";
    for (auto a: r.data)
        {
            stream << " 0x" << std::hex << a;
        }
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanError &cfe)
{
    stream << "CanError: ";
    stream << std::dec;
    stream << ", channel : " << (uint64_t) cfe.channel;
    stream << ", length : " << (uint64_t) cfe.length;
    stream << ", reservedCanErrorFrame : " << (uint64_t) cfe.reservedCanErrorFrame;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CanError_short &cfe)
{
    stream << "CanError_short: ";
    stream << std::dec;
    stream << ", channel : " << (uint64_t) cfe.channel;
    stream << ", length : " << (uint64_t) cfe.length;
    stream << '\n';
}


void print(std::ostream &stream, const blf_struct::CANErrorFrameExt &ce)
{
    stream << "mChannel: " << static_cast<int>(ce.Channel);
    stream << " mLength: " << (int) ce.Length;
    stream << " mFlags: " << (int) ce.Flags;
    stream << " mECC: " << (int) ce.ECC;
    stream << " mPosition: " << (int) ce.Position;
    stream << " mDLC: " << (int) ce.DLC << '\n';
    stream << " mFrameLengthInNS: " << (int) ce.FrameLengthInNS;
    stream << " mID: " << (int) ce.ID;
    stream << " mFlagsExt: " << (int) ce.FlagsExt;
    stream << " mReserved2: " << (int) ce.Reserved2;
    stream << " mData:";
    for (auto n: ce.Data)
        {
            stream << " " << std::hex << std::setfill('0') << std::setw(2) << (int) n;
        }
    stream << '\n';
}


void print(std::ostream &s, const blf_struct::CANDriverStatistic &can_stat)
{
    s << std::dec;
    s << "mChannel: " << can_stat.mChannel << '\n';
    s << "mBusLoad: " << can_stat.mBusLoad << '\n';
    s << "mStandardDataFrames: " << can_stat.mStandardDataFrames << '\n';
    s << "mExtendedDataFrames: " << can_stat.mExtendedDataFrames << '\n';
    s << "mStandardRemoteFrames: " << can_stat.mStandardRemoteFrames << '\n';
    s << "mExtendedRemoteFrames: " << can_stat.mExtendedRemoteFrames << '\n';
    s << "mErrorFrames: " << can_stat.mErrorFrames << '\n';
    s << "mOverloadFrames: " << can_stat.mOverloadFrames << '\n';
}


// *****************************phần chỉnh sửa*************************************88
void LoadDbcLoading() { 
    
    QString DbcLoading = "C:/TOOL/Graphic/dbc_loader.json";
    // QString DbcLoading = QDir(QDir::currentPath() + "/../../src").absoluteFilePath("dbc_loader.json");
    QFile file(DbcLoading);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        std::cout << "Không thể mở dbc_loader.json: " << DbcLoading.toStdString() << "\n";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        std::cout << "dbc_loader.json issue.\n\n";
        return;
    }

    globalDbcJson = doc.object();
    file.close();

}

bool FindMessageInfo(int channel, int id, QString& busOut, QString& messageNameOut, QJsonArray& signalOut) {
    QString channelStr = QString("CAN  %1").arg(channel);
    
    QString idStr = QString::number(id); // JSON lưu ID ở dạng decimal

    if (!globalDbcJson.contains("CAN")) return false;

    QJsonObject canObject = globalDbcJson["CAN"].toObject();
    for (const QString& dbcName : canObject.keys()) {
        QJsonObject dbc = canObject[dbcName].toObject();
        if (!dbc.contains("channel") || !dbc.contains("messages")) continue;

        QString chan = dbc["channel"].toString();
        if (chan != channelStr) continue;

        QString bus = dbc.value("bus").toString();
        QJsonObject messages = dbc["messages"].toObject();
        if (!messages.contains(idStr)) continue;

        QJsonObject msgObj = messages[idStr].toObject();
        QString messageName = msgObj.value("message_name").toString();
        QJsonArray signal = msgObj.value("signals").toArray();

        busOut = bus;
        messageNameOut = messageName;
        signalOut = signal;
        return true;
    }

    return false;
}

void print(std::ostream &stream, const lblf::CanMessage_common &cm) {
    LoadDbcLoading();
    // ===== Ghi vào RAM =====
    CanMessage msg;
    msg.timeStamp = std::to_string(cm.obh.objectTimeStamp);
    msg.channel = std::to_string(cm.channel);
    msg.flags = std::to_string(cm.flags);
    msg.dlc = std::to_string(cm.dlc);
    msg.id = "0x" + (static_cast<std::ostringstream&&>(std::ostringstream() << std::hex << cm.id)).str();

    std::stringstream dataStream;   
    for (int i = 0; i < cm.dlc; ++i) {
        dataStream << std::setw(2) << std::setfill('0') << std::hex << (int)cm.data[i] << " ";
    }
    msg.dataBytes = dataStream.str();

    msg.signalName = "";
    msg.deltaTime = "";
    msg.messageName = "";
    msg.direction = "";
    msg.busType = "";
    msg.cycleTime = "";

    QString bus, messageName;
    QJsonArray signal;

    if (FindMessageInfo(cm.channel, cm.id, bus, messageName, signal)) {
        msg.busType = bus.toStdString();
        msg.messageName = messageName.toStdString();

        for (const auto& sig : signal) {
            QJsonObject sigObj = sig.toObject();
            QString signalName = sigObj["name"].toString();

            if (!msg.signalName.empty()) {
                msg.signalName += "\n";
            }
            msg.signalName += signalName.toStdString();

            // ===================== Thêm đoạn này để lưu detail =====================
            SignalDetail detail;
            detail.name = sigObj["name"].toString().toStdString();
            detail.startBit = sigObj["start_bit"].toInt();
            detail.length = sigObj["length"].toInt();
            detail.factor = sigObj["factor"].toDouble();
            detail.offset = sigObj["offset"].toDouble();
            detail.min = sigObj["min"].toDouble();
            detail.max = sigObj["max"].toDouble();
            detail.unit = sigObj["unit"].isNull() ? "" : sigObj["unit"].toString().toStdString();
            detail.isSigned = sigObj["is_signed"].toBool();
            detail.multiplexing = sigObj["multiplexing"].isNull() ? "" : sigObj["multiplexing"].toString().toStdString();
            detail.comment = sigObj["Comment"].toString().toStdString();

            QJsonObject attrObj = sigObj["attributes"].toObject();
            for (const QString& key : attrObj.keys()) {
                detail.attributes[key.toStdString()] = attrObj[key].toString().toStdString();
            }

            QJsonObject valObj = sigObj["Val_Comment"].toObject();
            for (const QString& key : valObj.keys()) {
                detail.valComment[key.toStdString()] = valObj[key].toString().toStdString();
            }

            signalDetailMap[cm.id].push_back(detail);
            // =====================================================================
        }
    }

    allCanMessages.push_back(msg);

    // std::cout << "allCanMessages.size() == " << lblf::print::allCanMessages.size() << std::endl;
    // std::cout << "allCanMessages.empty() == " 
    //           << (lblf::print::allCanMessages.empty() ? "true" : "false") 
    //           << std::endl;
    // const auto& lastMsg = allCanMessages.back();
    // stream  << "[MSG] Time: " << lastMsg.timeStamp
    //         << ", Ch: " << lastMsg.channel
    //         << ", msgName: " << lastMsg.messageName
    //         << ", ID: " << lastMsg.id
    //         << ", Data: " << lastMsg.dataBytes
    //         << ", bustype: " << lastMsg.busType
    //         << ", Signal: " << lastMsg.signalName
    //         << "\n";


}

} // namespace lblf::print
