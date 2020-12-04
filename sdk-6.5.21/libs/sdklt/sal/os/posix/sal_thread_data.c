/*! \file sal_thread_data.c
 *
 * Thread data API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <sal/sal_thread.h>

#ifndef SAL_THREAD_DATA_MAX
#define SAL_THREAD_DATA_MAX             8
#endif

static pthread_mutex_t _sal_thread_lock = PTHREAD_MUTEX_INITIALIZER;

#define THREAD_LOCK() pthread_mutex_lock(&_sal_thread_lock)
#define THREAD_UNLOCK() pthread_mutex_unlock(&_sal_thread_lock)

typedef struct {
    pthread_key_t key;
    int mapped;
} thread_data_ctrl_t;

static thread_data_ctrl_t thread_data[SAL_THREAD_DATA_MAX];

sal_thread_data_t *
sal_thread_data_create(sal_thread_data_destroy_f destructor)
{
    int i;

    THREAD_LOCK();
    for (i = 0; i < SAL_THREAD_DATA_MAX; i++) {
        if (!thread_data[i].mapped) {
            thread_data[i].mapped = 1;
            break;
        }
    }
    THREAD_UNLOCK();

    if (i >= SAL_THREAD_DATA_MAX) {
        return NULL;
    }
    if (pthread_key_create(&thread_data[i].key, destructor) != 0) {
        thread_data[i].mapped = 0;
        return NULL;
    }
    return (void *)&thread_data[i].key;
}

int
sal_thread_data_set(sal_thread_data_t *tdata, void *val)
{
    if (tdata == NULL) {
        return 0;
    }
    return pthread_setspecific(*(pthread_key_t *)tdata, val);
}

void *
sal_thread_data_get(sal_thread_data_t *tdata)
{
    if (tdata == 0) {
        return NULL;
    }
    return pthread_getspecific(*(pthread_key_t *)tdata);
}

int
sal_thread_data_delete(sal_thread_data_t *tdata)
{
    if (tdata == NULL) {
        return 0;
    }
    return pthread_key_delete(*(pthread_key_t *)tdata);
}
