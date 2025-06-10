#ifndef PRINT_HH
#define PRINT_HH

#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include <QFile>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

#include "blf_structs.hh"
#include "SharedMemory.hh"

namespace lblf::print
{
extern std::map<int, std::vector<lblf::blf_struct::SignalDetail>> signalDetailMap;
extern std::vector<CanMessage> allCanMessages;
extern QJsonObject globalDbcJson;
void LoadDbcLoading();
void print(std::ostream &stream, const std::vector<uint8_t> &data);
void print(std::ostream &stream, const std::deque<char> &data, size_t counts_to_print);
auto print(lblf::exit_codes ec) -> std::string;
auto print(lblf::ObjectType_e ot) -> std::string;
auto print(lblf::AppId_e ai) -> std::string;
auto print(lblf::blf_struct::ObjectFlags_e of) -> std::string;
auto print(lblf::blf_struct::timeStampStatus_e tss) -> std::string;
void print(std::ostream &stream, const lblf::blf_struct::sysTime_t &ts);
void print(std::ostream &stream, const lblf::blf_struct::fileStatistics &os);
void print(std::ostream &stream, const lblf::blf_struct::BaseHeader &ohb);
void print(std::ostream &stream, const lblf::blf_struct::ObjectHeader &oh);
void print(std::ostream &stream, const lblf::blf_struct::ObjectHeader2 &oh2);
void print(std::ostream &stream, const lblf::ObjectHeaderCarry &ohc);
void print(std::ostream &stream, const lblf::blf_struct::LogContainer &lc);
void print(std::ostream &stream, const lblf::blf_struct::CanMessage &cm);
void print(std::ostream &stream, const lblf::blf_struct::CanOverload &co);
void print(std::ostream &stream, const lblf::blf_struct::CanOverload_short &co);
void print(std::ostream &stream, const lblf::blf_struct::CanMessage2 &cm2);
void print(std::ostream &stream, const blf_struct::CanMessage2_obh &cm2);
void print(std::ostream &stream, const lblf::blf_struct::AppTrigger &at);
void print(std::ostream &stream, const lblf::blf_struct::AppText &at);
void print(std::ostream &stream, const lblf::blf_struct::reserved_5 &r);
void print(std::ostream &stream, const lblf::blf_struct::CanError &cfe);
void print(std::ostream &stream, const lblf::blf_struct::CanError_short &cfe);
void print(std::ostream &stream, const lblf::blf_struct::CANErrorFrameExt &ce);
void print(std::ostream &stream, const lblf::blf_struct::CANDriverStatistic &can_stat);
void print(std::ostream &stream, const lblf::CanMessage_common &cm);
} // namespace lblf::print

#endif