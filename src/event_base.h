#ifndef _EVENT_BASE_H_
#define _EVENT_BASE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>

#include <iostream>
#include <vector>

#include "coro_io.h"
#include "thread_pool.h"


#define MAX_EPOLL_EVENT 100

typedef void (*ev_callback)(struct epoll_event *event, void *args);

struct Event{
    struct epoll_event epEvent;
    ev_callback cb;
    void *cbArg;
};

struct EventHandler{
    int epollfd;
    std::vector<struct Event> events;

    // thread-pool object
    threadpool pThreadPool;
};

struct EventHandler *
EventNew();

void 
EventAddEvent(struct EventHandler *pEvent, int fd, struct epoll_event epEvent, ev_callback cb, void *arg);

void 
EventRemoveEvent(struct EventHandler *pEvent, int fd, struct epoll_event ev);

struct Event *
EventSearchEvent(struct EventHandler *pEvent, int fd);

void
EventLoopForever(struct EventHandler *pEvtHandle);

////////////////////////////////////////
// Schedule a callback function

// schedule now
void
EventCallOne(struct EventHandler *pEvent, void (*cb)(void *arg), void *arg);


////////////////////////////////////////
// Coroutine related function

// execute a cb function in a thread-pool
struct AwaitObject
EventRunInExecutor(struct EventHandler *pEvent, void (*cb)(void *arg), void *arg );

// await for second
struct AwaitObject
EventSleep(struct EventHandler *pEvent, int seconds );

#endif