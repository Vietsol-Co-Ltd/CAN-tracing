#ifndef GLOBALDATA_HPP
#define GLOBALDATA_HPP

#include <QVector>
#include "CanMessage.h"  // Dùng struct CanMessage từ đây

extern QVector<CanMessage> sharedCanLog;
extern QVector<CanMessage>* globalCanLog;

#endif // GLOBALDATA_HPP
