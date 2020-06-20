#include "../include/timer.h"


mymutex * timer::m_globalmutex = NULL;
volatile bool timer::sendfeedback = false;
    //开始定时器
void timer::StartTimer(){
    pthread_t p_timerid;
    pthread_attr_t att;
    pthread_attr_init(&att);
    int result = pthread_create(&p_timerid,&att,runtimer,NULL);
    if(result){
        cout<< "线程建立失败!!" << endl;
        return;
    }
    return;
}