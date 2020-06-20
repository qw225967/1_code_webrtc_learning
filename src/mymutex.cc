
#include "../include/mymutex.h"

mymutex::mymutex(){
    pthread_mutex_init(&m_mtx,NULL);
    }
mymutex::~mymutex(){
    pthread_mutex_destroy(&m_mtx);
    }

int mymutex::mylock(){
    return pthread_mutex_trylock(&m_mtx);
}
void mymutex::myunlock(){
    pthread_mutex_unlock(&m_mtx);
    return ;
}



