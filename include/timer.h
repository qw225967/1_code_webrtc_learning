#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include"mymutex.h"
using namespace std;

static bool sendfeedback = false;
static mymutex theglobalmtx;

static void * runtimer(void * prama)
        {
            clock_t start, finish;
            start = clock();//计时函数
            double totaltime = 0;//定义时间变量
            int mes = 2000;
            while (1)
            {
                finish = clock();
                usleep(1000);
                totaltime = (double)(finish - start);

                if(mes<totaltime){
                    if(1==theglobalmtx.mylock()){
                        sendfeedback = true;
                    }
                    totaltime = 0;
                    start = finish;
                    cout<< "put out" << endl;
                }
            }

            return NULL;
        }
class timer{
    public:
    timer(){}
    ~timer(){}

    //开始定时器

    public:
        void StartTimer(){
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
};
