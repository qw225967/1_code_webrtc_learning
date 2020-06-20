#pragma once
#include <mutex>
//=================锁===============
class mymutex{
public:
    mymutex();
    ~mymutex();
    int mutex_init();
    int mylock();
    void myunlock();
    public:

    pthread_mutex_t m_mtx;

};
//=================锁===============



