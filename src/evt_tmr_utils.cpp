#include <stdio.h>
#include <stdlib.h>

#include <sys/timerfd.h>

#include <errno.h>
#include <evt_tmr_utils.h>

struct epoll_event 
EventTimerOnce()
{
    struct epoll_event ev;
    int timer_fd = 0;
    struct itimerspec new_value;
    struct timespec now;

    // open with non-blocking mode
    timer_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timer_fd == -1)
    {
        perror("timerfd_create");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_value.tv_sec = now.tv_sec;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(timer_fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
    {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = timer_fd;

    return ev;
}

struct epoll_event  
EventTimerOnceAndLater(int delay)
{
    struct epoll_event ev;
    int timer_fd = 0;
    struct itimerspec new_value;
    struct timespec now;

    // open with non-blocking mode
    timer_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timer_fd == -1)
    {
        perror("timerfd_create");
        exit(EXIT_FAILURE);
    }

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    new_value.it_value.tv_nsec = now.tv_nsec;
    new_value.it_value.tv_sec = now.tv_sec + delay;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    if (timerfd_settime(timer_fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
    {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = timer_fd;

    return ev;
}