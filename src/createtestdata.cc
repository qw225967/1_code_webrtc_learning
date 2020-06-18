#include "../include/createtestdata.h"
#include <ctime>

TheCreaterofData::TheCreaterofData()
{
}

TheCreaterofData::~TheCreaterofData()
{
}

PacketData TheCreaterofData::CreateData(){
    srand((int)time(0));
    PacketData temppacket;
    temppacket.s_arrival_time_ms = rand()%100;
    temppacket.s_media_ssrc = rand()%100;
    temppacket.s_payload_size = rand()%100;
    
}


