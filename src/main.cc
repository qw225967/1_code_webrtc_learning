#include <iostream>
#include "../include/createtestdata.h"
#include "../include/remote_estimator_proxy.h"
using namespace std;

int main(){
	TheCreaterofData temp_test;
	PacketData temp_struct_packet;
	while(1){
		
		//产生随机的测试数据
		temp_struct_packet = temp_test.CreateData();
		int temp_seq = 100000;
		temp_struct_packet.s_seq = temp_seq;
		temp_seq++;
		
		//调用remot函数
		cls::RemoteEstimatorProxy* temp_remote = new cls::RemoteEstimatorProxy;
		temp_remote->IncomingPacket(temp_struct_packet.s_arrival_time_ms,
									temp_struct_packet.s_payload_size,
									temp_struct_packet.s_media_ssrc,
									temp_struct_packet.s_seq);
		delete temp_remote;

	}

	return 0;
}
