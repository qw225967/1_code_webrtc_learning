#include "../include/createtestdata.h"
#include <ctime>
#include <time.h>

TheCreaterofData::TheCreaterofData()
{
}

TheCreaterofData::~TheCreaterofData()
{
}

PacketData TheCreaterofData::CreateData(){
    srand((int)time(0));
    PacketData temppacket;
    temppacket.s_media_ssrc = rand()%100;
    temppacket.s_payload_size = rand()%100;
   return temppacket; 
}


