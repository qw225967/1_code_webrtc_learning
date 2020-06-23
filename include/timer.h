#pragma once
#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include"mymutex.h"
using namespace std;


class timer{
    public:
    timer(mymutex * globalmutex){ m_globalmutex = globalmutex;}
    ~timer(){}
    
    void StartTimer();

    public:
    static mymutex * m_globalmutex;
    volatile static bool sendfeedback;
    
    //开始定时器
    static void * runtimer(void * prama){
    clock_t start, finish;
    start = clock();//计时函数
    double totaltime = 0;//定义时间变量
    int mes = 100;
    while (1){
        finish = clock();
  
        usleep(1000);
        totaltime = (double)(finish - start);

        if(mes<totaltime){
            int a = (m_globalmutex)->mylock();
            if(a == 0){
                sendfeedback = true;
                (m_globalmutex)->myunlock(); 
            }
            totaltime = 0;
            start = finish;
        }
    }

    return NULL;
}
};
