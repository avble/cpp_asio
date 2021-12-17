#ifndef __EVENT_TIMER_UTILS_H__
#define __EVENT_TIMER_UTILS_H__
// C and system
#include <sys/epoll.h>
// C++
#include <memory>

#include "coro_io.h"

struct epoll_event  
EventTimerOnce();

struct epoll_event  
EventTimerOnceAndLater(int delay);

#endif