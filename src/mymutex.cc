
#include "../include/mymutex.h"
#include <iostream>
using namespace std;

mymutex::mymutex(){}
mymutex::~mymutex(){
    pthread_mutex_destroy(&m_mtx);
    }
int mymutex::mutex_init(){
    int a = pthread_mutex_init(&m_mtx,NULL);
    if(a == 1){
        cout << "defeate for init mutex" << endl;
        return 1;
    }
    return 0;
}

int mymutex::mylock(){
    int a = 0;
    a = pthread_mutex_trylock(&m_mtx);
    return a;
}
void mymutex::myunlock(){
    pthread_mutex_unlock(&m_mtx);
    return ;
}



