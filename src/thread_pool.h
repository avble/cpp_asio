#ifndef _THR_POOL_H_
#define _THR_POOL_H_

#include "thpool.h"

typedef void (*workItemCB)(void *arg);
typedef void (*workItemStatusCB)(void *arg);

struct WorkItem{
    workItemCB wiCB;
    void *arg;
    workItemStatusCB stCB;
    void *statusArg;
};

void TPAddWorkItem(threadpool pool, struct WorkItem wi);

#endif