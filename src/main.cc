#include <iostream>
#include "../include/createtestdata.h"
#include "../include/remote_estimator_proxy.h"
#include <pthread.h>
using namespace std;
void *PrintHello(void *threadid)
{  
   // 对传入的参数进行强制类型转换，由无类型指针变为整形数指针，然后再读取
   int tid = *((int*)threadid);
   cout << "Hello Runoob! 线程 ID, " << tid << endl;
   pthread_exit(NULL);
}
int main(){
	int temp_seq = 100000;
	TheCreaterofData temp_test;
	PacketData temp_struct_packet;
	cls::RemoteEstimatorProxy* temp_remote = new cls::RemoteEstimatorProxy;
	temp_remote->m_timer->StartTimer();
	cout << "go test !" << endl;
	




	while(1){
		
		//产生随机的测试数据
		temp_struct_packet = temp_test.CreateData();
		
		temp_seq++;
		
		//调用remot中，comingpacket函数
		temp_remote->IncomingPacket(temp_struct_packet.s_arrival_time_ms,
									temp_struct_packet.s_payload_size,
									temp_struct_packet.s_media_ssrc,
									temp_seq);

		

	}
	delete temp_remote;
	return 0;
}