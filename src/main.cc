#include <iostream>
#include "../include/createtestdata.h"
#include "../include/remote_estimator_proxy.h"
#include <pthread.h>
#include "../include/timer.h"
#include "../include/mymutex.h"
using namespace std;


int main(){
	int temp_seq = 100000;
	int temp_time = 19931202;
	TheCreaterofData temp_test;
	PacketData temp_struct_packet;
	mymutex *themutex = new mymutex;
	int a = themutex->mutex_init();
	if(a == 1)
		cout << "can build the mutex"<<endl;
	timer *temp_timer  = new timer(themutex);
	temp_timer->StartTimer();
	
	cout << "go test !" << endl;
	
	
	cls::RemoteEstimatorProxy* temp_remote = new cls::RemoteEstimatorProxy(themutex,&temp_timer);
	
	while(1){
		
		//产生随机的测试数据
		temp_struct_packet = temp_test.CreateData();
		temp_time++;
		temp_seq++;
		
		//调用remot中，comingpacket函数
		temp_remote->IncomingPacket(temp_time,
									temp_struct_packet.s_payload_size,
									temp_struct_packet.s_media_ssrc,
									temp_seq);

		

	}
	delete temp_remote;
	delete temp_timer;
	delete themutex;
	return 0;
}