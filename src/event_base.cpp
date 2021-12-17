#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

// C++
#include <vector>
#include <iostream>
#include <memory>

#include "event_base.h"
#include "evt_tmr_utils.h"
#include "coro_io.h"
#include "thread_pool.h"

struct EventHandler *
EventNew()
{
    struct EventHandler *pEventHandler = NULL;

    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        perror("epoll_create");
        return NULL;
    }

    pEventHandler = (struct EventHandler*)malloc(sizeof(struct EventHandler));
    pEventHandler->epollfd = epollfd;
    pEventHandler->pThreadPool = NULL;

    return pEventHandler;
}

void 
EventAddEvent(struct EventHandler *pEvent, int fd, struct epoll_event epEvent, ev_callback cb, void *arg)
{
    if (-1 == epoll_ctl(pEvent->epollfd, EPOLL_CTL_ADD, fd, &epEvent))
    {
        perror("epoll_ctl");
        return;
    }
    Event ev;
    epEvent.data.fd = fd;
    ev.epEvent = epEvent;
    ev.cb = cb;
    ev.cbArg = arg;
    pEvent->events.push_back(ev);
}

void 
EventRemoveEvent(struct EventHandler *pEvent, int fd)
{
    for (auto it = pEvent->events.begin(); it != pEvent->events.end(); it++)
    {
        if (it->epEvent.data.fd == fd)
        {
            if (-1 == epoll_ctl(pEvent->epollfd, EPOLL_CTL_DEL, fd, NULL))
            {
                perror("epoll_ctl");
            }            
            pEvent->events.erase(it);
            break;
        }
    }
}

struct Event *
EventSearchEvent(struct EventHandler *pEvent, int fd)
{
    auto it = pEvent->events.begin();
    for (; it != pEvent->events.end(); it++)
    {
        if (it->epEvent.data.fd == fd)
        {
            return &(*it);
        }
    }

    return NULL;
}

void 
EventLoopForever(struct EventHandler *pEvtHandle)
{
    struct epoll_event events[MAX_EPOLL_EVENT];

    for (;;){
        int nfds = epoll_wait(pEvtHandle->epollfd, &events[0], MAX_EPOLL_EVENT, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; i++)
        {
            struct Event *pEvent = EventSearchEvent(pEvtHandle, events[i].data.fd);

            if (pEvent != NULL)
            {
                pEvent->cb(&events[i], pEvent->cbArg);
                // EventRemoveEvent(pEvtHandle, events[i].data.fd);

            }
        }
    }
}

void EventQuit(struct EventHandler *pEvent)
{
    free(pEvent);
    pEvent = NULL;
}

struct EventTask{
    void *arg;
    void (*cb)(void *arg);

    struct EventHandler *pEvent;
};

static void EventTaskCB(struct epoll_event *p_epoll_ev, void *arg)
{
    uint64_t t;
    int rc = read(p_epoll_ev->data.fd, &t, sizeof(uint64_t));

    struct EventTask *pTask = (struct EventTask *)arg;
    pTask->cb(pTask->arg);

    EventRemoveEvent(pTask->pEvent, p_epoll_ev->data.fd);
    close(p_epoll_ev->data.fd);

    free(pTask);
}

// Event
void
EventCallOne(struct EventHandler *pEvent, void (*cb)(void *arg), void *arg)
{
    struct EventTask *pTask = (struct EventTask *)malloc(sizeof (struct EventTask));
    pTask->cb = cb;
    pTask->arg = arg;
    pTask->pEvent = pEvent;

    epoll_event ev =  EventTimerOnce();
    EventAddEvent(pEvent, ev.data.fd, ev, EventTaskCB, pTask);
}


static void TimerCBForSleep(struct epoll_event *ev, void* arg)
{
    uint64_t t;
    int rc = read(ev->data.fd, &t, sizeof(uint64_t));
    struct AwaitData *pTimerArg = (struct AwaitData *)arg;

    std::cout << "TimerCBForSleep: " <<  std::hex << pTimerArg <<  std::endl;
    pTimerArg->h.resume();
    
    // free the AsyncSleepObjecct
    free(pTimerArg);

    // TODO: Remove the event
    // Resume IO operation
    close(ev->data.fd);
}

// Awaitable object
AwaitObject
EventSleep(struct EventHandler *pEvent, int seconds )
{
    struct AwaitData *pTimerArg = NULL;
    pTimerArg = (struct AwaitData *)malloc(sizeof(struct AwaitData));

    epoll_event ev =  EventTimerOnceAndLater(seconds);

    EventAddEvent(pEvent, ev.data.fd, ev, TimerCBForSleep, pTimerArg);

    return AwaitObject(pTimerArg);
}

struct EventExecutor{
    void (*cb)(void *arg);
    void *arg;
    struct AwaitData *pAwaitData;
    struct EventHandler *pEvent;
};

static void 
EventRunInExecutorResumeTask(struct epoll_event *ev, void* arg)
{
    uint64_t t;
    int rc = read(ev->data.fd, &t, sizeof(uint64_t));
    struct EventExecutor *pExecutor = (struct EventExecutor *)arg;

    EventRemoveEvent(pExecutor->pEvent, ev->data.fd);
    close(ev->data.fd);

    std::cout << "EventRunInExecutorResumeTask" << " " << __LINE__  << std::endl;

    pExecutor->pAwaitData->h.resume();

    std::cout << "EventRunInExecutorResumeTask" << "  " << __LINE__  << std::endl;

    free(pExecutor);
}


static void EventRunInExecutorCompletedCB(void *arg)
{
    struct EventExecutor *pExecutor = (struct EventExecutor *)arg;

    std::cout << "EventRunInExecutorCompletedCB" << "  " << __LINE__ << std::endl;

    epoll_event ev =  EventTimerOnce();
    EventAddEvent(pExecutor->pEvent, ev.data.fd, ev, EventRunInExecutorResumeTask, pExecutor);

    std::cout << "EventRunInExecutorCompletedCB" << "  " << __LINE__ << std::endl;

}


static void EventRunInExecutorCommitTask(struct epoll_event *p_epoll_ev, void *arg)
{
    uint64_t t;
    struct EventExecutor *pExecutor = (struct EventExecutor *)arg;
    int rc = read(p_epoll_ev->data.fd, &t, sizeof(uint64_t));
    EventRemoveEvent(pExecutor->pEvent, p_epoll_ev->data.fd);
    close(p_epoll_ev->data.fd);

    // submit the a job
    // callback function 
    // callback argument
    struct WorkItem wi;
    wi.wiCB = pExecutor->cb;
    wi.arg = pExecutor->arg;
    wi.stCB = EventRunInExecutorCompletedCB;
    wi.statusArg = pExecutor;

    std::cout << "EventRunInExecutorCommitTask" << "  " << std::endl;

    TPAddWorkItem(pExecutor->pEvent->pThreadPool, wi);
}

// Awaitable object
struct AwaitObject
EventRunInExecutor(struct EventHandler *pEvent, void (*cb)(void *arg), void *arg )
{
    struct AwaitData *pAwaitData = NULL;
    pAwaitData = (struct AwaitData *)malloc(sizeof(struct AwaitData));

    // Schedule to run in next io loop
    struct EventExecutor *pExecutor = (struct EventExecutor *)malloc(sizeof (struct EventExecutor));
    pExecutor->cb = cb;
    pExecutor->arg = arg;
    pExecutor->pAwaitData = pAwaitData;
    pExecutor->pEvent = pEvent;

    epoll_event ev =  EventTimerOnce();
    EventAddEvent(pEvent, ev.data.fd, ev, EventRunInExecutorCommitTask, pExecutor);

    return AwaitObject(pAwaitData);
}
