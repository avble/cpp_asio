// C and system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// C++
#include <memory>
#include <iostream>

#include "thread_pool.h"

static void TPWorkItemCB(void *arg)
{
    struct WorkItem *pWorkItem = (struct WorkItem *)arg;

    std::cout << "TPWorkItemCB" << " -- " << __LINE__ << std::endl;

    pWorkItem->wiCB(pWorkItem->arg);

    if (pWorkItem->stCB != NULL)
    {
        std::cout << "TPWorkItemCB" << " -- " << __LINE__ << std::endl;
        pWorkItem->stCB(pWorkItem->statusArg);
        std::cout << "TPWorkItemCB" << " -- " << __LINE__ << std::endl;
    }

    std::cout << "TPWorkItemCB" << " -- " << __LINE__ << std::endl;


    free(pWorkItem);
}

void TPAddWorkItem(threadpool pool, struct WorkItem wi)
{
    struct WorkItem *pWorkItem = (struct WorkItem *)malloc(sizeof (struct WorkItem));

    memcpy(pWorkItem, &wi, sizeof(struct WorkItem));
    // pWorkItem->stCB = NULL;

    thpool_add_work(pool, TPWorkItemCB, pWorkItem);
}

