#ifndef APPIMP_H
#define APPIMP_H

#include "zlog.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "topkg.h"
#include "recssl.h"
#include "ckafkaproducer.h"
#include "configpara.h"


class appManager;
class kafkaProduce;


class appManager
{
public:
    appManager();
    void init();
    static appManager* instance();

    void zlog_InitStep();
    void kafka_InitStep();
    int  pushKafkaData(char * recmsg);
    bool verifyHeaders(char * recMsg,RecHead &headers);
    std::vector<std::string> split(const  std::string& s, const std::string& delim);
    ToPkg *pkg ;


private:
    static appManager* appMan;
    RecHead head;
    PackTraj Traj[20];
    PackAlarm alarm;
    CKafkaProducer kp;
    KafkaPara kafkaPar;
    LaneToFTPMap*  m_planetoftpmap;
};




#endif // APPIMP_H
