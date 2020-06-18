#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>
using namespace std;


//一个数据包结构体存四个IncomingPacket的输入参数。
struct PacketData{
	int64_t s_arrival_time_ms;
	size_t s_payload_size;
	uint32_t s_media_ssrc;
	int64_t s_seq;
};
class TheCreaterofData
{
	public:
	TheCreaterofData();
	~TheCreaterofData();

	public:
	PacketData CreateData();
	public:
	vector<PacketData> m_vec_PacketData;
};
