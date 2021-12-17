#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <inttypes.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

#include <iostream>
#include <coroutine>
#include <thread>

#include "event_base.h"
#include "evt_tmr_utils.h"
#include "coro_io.h"

#include "thread_pool.h"

struct EventHandler *pEvent = NULL;
threadpool pThpool = NULL;

// Run in the thread-pool context
// ---------------------------------
struct Handler1_Data{
    int state;
};

// Input 
void Handler1_Input(void *arg)
{
    struct Handler1_Data *pData = (struct Handler1_Data *)arg;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Input: " << "[ENTER]" << __LINE__ << std::endl;
    sleep(5);
    pData->state = 1;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Input: " << "[LEAVE]" << __LINE__ << std::endl;
}

// Process
void Handler1_Process(void *arg)
{
    struct Handler1_Data *pData = (struct Handler1_Data *)arg;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Process: " << "[ENTER]" << __LINE__ << std::endl;
    sleep(5);
    pData->state = 2;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Process: " << "[LEAVE]" << __LINE__ << std::endl;
}

// Output
void Handler1_Output(void *arg)
{
    struct Handler1_Data *pData = (struct Handler1_Data *)arg;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Output: " << "[ENTER]" << __LINE__ << std::endl;
    sleep(5);
    pData->state = 3;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Handler1_Output: " << "[LEAVE]" << __LINE__ << std::endl;
}

// Task
Task
Task_Handler1(void *arg)
{
    std::cout << "[" << std::this_thread::get_id() << "]" << "Task_Handler1: " <<  "[ENTER]" << std::endl;
    struct Handler1_Data *pData = (struct Handler1_Data *)malloc(sizeof (struct Handler1_Data));
    pData->state = 0;

    // process the io-bouding in a thread-pool
    co_await EventRunInExecutor(pEvent, Handler1_Input, pData);
    std::cout << "[" << std::this_thread::get_id() << "]" << "[" << std::this_thread::get_id() << "]" << "Task_Handler1: " <<  "[BODY]" << "State: " << pData->state << std::endl;

    co_await EventRunInExecutor(pEvent, Handler1_Process, pData);
    std::cout << "[" << std::this_thread::get_id() << "]" << "Task_Handler1: " <<  "[BODY]" << "State: " << pData->state << std::endl;

    co_await EventRunInExecutor(pEvent, Handler1_Output, pData);
    std::cout << "[" << std::this_thread::get_id() << "]" << "Task_Handler1: " <<  "[BODY]" << "State: " << pData->state << std::endl;

    // process the io-bouding task 
    std::cout << "[" << std::this_thread::get_id() << "]" << std::endl;
    std::cout << "[" << std::this_thread::get_id() << "]" << "Task_Handler1: " << "[LEAVE]" << __LINE__ << std::endl;
}

// IO handler
void
IO_Handler1(void *arg)
{
    std::cout << "[" << std::this_thread::get_id() << "]" << "IO_Handler1: " << "[ENTER]" << std::endl;
    Task_Handler1(arg);
    std::cout << "[" << std::this_thread::get_id() << "]" << "IO_Handler1: " << "[LEAVE]" << std::endl;
}

int main(int argc, char *argv[])
{
    
    pEvent = EventNew();
    pThpool = thpool_init(10);
    pEvent->pThreadPool = pThpool;

    EventCallOne(pEvent, IO_Handler1, NULL);

    EventLoopForever(pEvent);

	thpool_destroy(pThpool);
    
}