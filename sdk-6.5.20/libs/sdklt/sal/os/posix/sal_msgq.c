/*! \file sal_msgq.c
 *
 * Message queue API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <sal/sal_time.h>
#include <sal/sal_msgq.h>

#define SAL_MSGQ_SIGNATURE   0x8532b9d0

typedef struct basic_element_s {
    struct basic_element_s *next;
    uint8_t data[];
} basic_element_t;

typedef struct {
    basic_element_t *head;
    basic_element_t *tail;
} pri_list_t;

typedef struct rwlock_ctrl_s {
    uint32_t signature;
    size_t element_size;
    size_t num_of_element;
    size_t max_elements;
    uint8_t *buf;
    pri_list_t pri_lists[SAL_MSGQ_PRIORITIES];
    basic_element_t *free_element_list;
    pthread_mutex_t q_lock_mutex;
    pthread_mutex_t recv_mutex;
    pthread_mutex_t post_mutex;
    pthread_cond_t recv_cond;
    pthread_cond_t post_cond;
    uint32_t    pending_to_post;
    uint32_t    pending_to_recv;
    const char *desc;
} rwlock_ctrl_t;


sal_msgq_t
sal_msgq_create(size_t element_size, size_t max_elements, const char *desc)
{
    rwlock_ctrl_t *mq;
    size_t actual_element_size = element_size + sizeof(void *);

    mq = (rwlock_ctrl_t *)malloc(sizeof(rwlock_ctrl_t));
    if (!mq) {
        return NULL;
    }
    memset(mq, 0, sizeof(*mq));
    do {
        int j;
        basic_element_t *p_element;
        mq->free_element_list = (basic_element_t *)
            malloc(actual_element_size * max_elements);
        if (!mq->free_element_list) {
            break;
        }
        mq->buf = (uint8_t *)mq->free_element_list;  /* Keep pointer to free the memory */
        for (j = 0, p_element = mq->free_element_list;
             j < max_elements-1;
             j++) {
            p_element->next = (basic_element_t *)((uint8_t *)p_element + actual_element_size);
            p_element = p_element->next;
        }
        p_element->next = NULL; /* Terminate the list */

        for (j = 0; j < SAL_MSGQ_PRIORITIES; j++) {
            mq->pri_lists[j].head = NULL;
            mq->pri_lists[j].tail = NULL;
        }
        if ((0 != pthread_mutex_init(&mq->q_lock_mutex, NULL)) ||
            (0 != pthread_mutex_init(&mq->recv_mutex, NULL)) ||
            (0 != pthread_mutex_init(&mq->post_mutex, NULL))) {
            break;
        }
        if (0 != pthread_cond_init(&mq->recv_cond, NULL)) {
            break;
        }
        if (0 != pthread_cond_init(&mq->post_cond, NULL)) {
            break;
        }
        mq->desc = desc;
        mq->element_size = element_size;
        mq->max_elements = max_elements;
        mq->num_of_element = 0;
        mq->pending_to_post = 0;
        mq->pending_to_recv = 0;
        mq->signature = SAL_MSGQ_SIGNATURE;
        return (sal_msgq_t)mq;
    } while (0);

    /* Free all allocated resources and return failure */
    if (mq->buf) {
        free (mq->buf);
    }
    if (*(uint32_t *)(&mq->q_lock_mutex)) {
        pthread_mutex_destroy(&mq->q_lock_mutex);
    }
    if (*(uint32_t *)(&mq->recv_mutex)) {
        pthread_mutex_destroy(&mq->recv_mutex);
    }
    if (*(uint32_t *)(&mq->post_mutex)) {
        pthread_mutex_destroy(&mq->post_mutex);
    }
    if (*(uint32_t *)(&mq->recv_cond)) {
        pthread_cond_destroy(&mq->recv_cond);
    }
    if (*(uint32_t *)(&mq->post_cond)) {
        pthread_cond_destroy(&mq->post_cond);
    }
    free(mq);
    return NULL;
}

void
sal_msgq_destroy(sal_msgq_t msgq_hdl)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return;
    }

    /* Release Q resources */
    pthread_cond_destroy(&mq->recv_cond);
    pthread_cond_destroy(&mq->post_cond);
    pthread_mutex_destroy(&mq->q_lock_mutex);
    pthread_mutex_destroy(&mq->recv_mutex);
    pthread_mutex_destroy(&mq->post_mutex);
    free (mq->buf);
    free (mq);
}
static int
_timed_calc(uint32_t usec, struct timespec *ts)
{
#if defined(CLOCK_REALTIME)
    if (clock_gettime(CLOCK_REALTIME, ts) != 0) {
        return -1;
    }
#else
    /* Use time if realtime clock is not available */
    ts->tv_sec = time(NULL)+1; /* round up fractions of sec */
#endif

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;
    ts->tv_nsec += (usec % SECOND_USEC) * 1000;
    /* Currect overflow - if occured */
    if (ts->tv_nsec > SECOND_NSEC) {
        /* Can only have overflow of 1 sec */
        ts->tv_sec++;
        ts->tv_nsec -= SECOND_NSEC;
    }
    return 0;
}

int
sal_msgq_post(sal_msgq_t msgq_hdl,
              void *element,
              sal_msgq_priority_t pri,
              uint32_t usec)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;
    int rv;
    basic_element_t *p_element;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE) ||
        (pri < SAL_MSGQ_MIN_PRIORITY) || (pri > SAL_MSGQ_MAX_PRIORITY)) {
        return SAL_MSGQ_E_INVALID_PARAM;
    }

    /* Lock the queue */
    if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
        rv = __LINE__;
        return -rv;
    }
    /* Is the queue full or other pending ? */
    if (mq->num_of_element == mq->max_elements) {
        int ret_status;

        /* Need to wait until there is room */
        if (usec == SAL_MSGQ_NOWAIT) { /* Return error if can't wait */
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return SAL_MSGQ_E_TIMEOUT;
        }
        mq->pending_to_post++;

post_retry:
        rv = pthread_mutex_lock(&mq->post_mutex);
        if (rv != 0) {
            mq->pending_to_post--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        pthread_mutex_unlock(&mq->q_lock_mutex);

        /*
         * This can be critical place. If we get stuck here (task switch) and
         * the recv threads depleting the queue then nobody will signal the
         * condition variable ==> deadlock. For this reason, we make sure that
         * the recv thread locks the post mutex before signaling. Since at this
         * point we have the post_mutex, the recv thread will block and we
         * can get into the condition function which will unlock the post_mutex
        */
        ret_status = -2;
        if (usec == SAL_MSGQ_FOREVER) {
            rv = pthread_cond_wait(&mq->post_cond, &mq->post_mutex);
        } else  {
            struct timespec ts;
            int loop_count = 0;
            do {
                _timed_calc(usec, &ts);

                rv = pthread_cond_timedwait(&mq->post_cond,
                                            &mq->post_mutex, &ts);
                assert ((rv == 0) || (rv == ETIMEDOUT));
                if (rv == ETIMEDOUT) {
                    ret_status = SAL_MSGQ_E_TIMEOUT;
                }
            } while (rv == EINVAL && loop_count++ < 1);
        }
        /* Unlock the mutex */
        pthread_mutex_unlock(&mq->post_mutex);

        /* Lock the queue again */
        if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
            rv = __LINE__;
            return -rv;
        }
        if (rv != 0) {  /* If failed condition wait */
            mq->pending_to_post--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return ret_status;
        }
        /* It is possible, on multiple cores, that multiple threads will
         * wake up from a single condition signal (according to the
         * documentation). So check that everything is okay
         */
        if (mq->num_of_element == mq->max_elements) {
            goto post_retry;
        }
        mq->pending_to_post--;
    }
    /* When we here, the queue is locked and there is room */
    assert (mq->num_of_element < mq->max_elements);
    assert (mq->free_element_list != NULL);

    /* Get an element from the free list */
    p_element = mq->free_element_list;
    mq->free_element_list = p_element->next;
    p_element->next = NULL;

    /* Copy the data into the element */
    memcpy(p_element->data, element, mq->element_size);

    /* Attach the element to the tail of the list (based on its priority) */
    if (mq->pri_lists[pri].head) {    /* If the list is not empty */
        mq->pri_lists[pri].tail->next = p_element;  /* Attach to the tail end*/
    }  else {
        mq->pri_lists[pri].head = p_element;
    }
    mq->pri_lists[pri].tail = p_element;  /* Set the tail to be the new element */

    mq->num_of_element++;

    if (mq->pending_to_recv > 0) {   /* If someone waiting for recv */
        pthread_mutex_lock(&mq->recv_mutex);
        rv = pthread_cond_signal(&mq->recv_cond); /* Wake up pending recv threads */
        assert(rv == 0);
        /*
         * Unlock the queue before freeing the receive mutex, so if the receiver
         * wakes up it will not be required to perform another task switch as
         * it will be blocked on the q_lock_mutex.
         */
        pthread_mutex_unlock(&mq->q_lock_mutex);
        pthread_mutex_unlock(&mq->recv_mutex);
    } else {
        /* Unlock the queue and return */
        pthread_mutex_unlock(&mq->q_lock_mutex);
    }

    return 0;
}

int
sal_msgq_recv(sal_msgq_t msgq_hdl, void *element, uint32_t usec)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;
    int rv;
    basic_element_t *p_element;
    int j;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return SAL_MSGQ_E_INVALID_PARAM;
    }

    /* Lock the queue */
    if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
        return -1;
    }
    /* Is the queue empty or others are waiting to receive ? */
    if (mq->num_of_element == 0) {
        int ret_status;

        /* Need to wait, does the caller willing too? */
        if (usec == SAL_MSGQ_NOWAIT) { /* Return error if can't wait */
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        mq->pending_to_recv++;   /* Increment the number of waiting */

recv_retry:
        rv = pthread_mutex_lock(&mq->recv_mutex);
        if (0 != rv) {
            mq->pending_to_recv--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            rv = __LINE__;
            return -rv;
        }
        pthread_mutex_unlock(&mq->q_lock_mutex);

        ret_status = -2;
        if (usec == SAL_MSGQ_FOREVER) {
            rv = pthread_cond_wait(&mq->recv_cond, &mq->recv_mutex);
        } else  {
            struct timespec ts;
            int loop_count = 0;
            do {
                _timed_calc(usec, &ts);

                rv = pthread_cond_timedwait(&mq->recv_cond,
                                            &mq->recv_mutex,
                                            &ts);
                if (rv == ETIMEDOUT) {
                    ret_status = SAL_MSGQ_E_TIMEOUT;
                }
            } while (rv == EINVAL && loop_count++ < 1);
        }
        /* Unlock the mutex */
        pthread_mutex_unlock(&mq->recv_mutex);

        /* Lock the queue again */
        if (pthread_mutex_lock(&mq->q_lock_mutex) != 0) {
            rv = __LINE__;
            return -rv;
        }
        if (rv != 0) {  /* If failed condition wait */
            mq->pending_to_recv--;
            pthread_mutex_unlock(&mq->q_lock_mutex);
            return ret_status;
        }

        /* It is possible, on multiple cores, that multiple threads will
         * wake up from a single condition signal (according to the
         * documentation). So verify that if the Q is empty that try again.
         */
        if (mq->num_of_element == 0) {
            goto recv_retry;
        }

        mq->pending_to_recv--;  /* Decrement the number of receive pending */
    } /* End of if Q is empty or pending receivers */

    /* If we are here have have the Q lock and there are elements in the Q */
    assert (mq->num_of_element > 0);
    for (j = SAL_MSGQ_MAX_PRIORITY; j >= SAL_MSGQ_MIN_PRIORITY; j--) {
        if (mq->pri_lists[j].head) {
            break;
        }
    }
    assert(j >= SAL_MSGQ_MIN_PRIORITY);
    p_element = mq->pri_lists[j].head;
    mq->pri_lists[j].head = p_element->next;
    if (mq->pri_lists[j].tail == p_element) {
        mq->pri_lists[j].tail = NULL;  /* was the last element */
    }

    /* Copy the data */
    memcpy(element, p_element->data, mq->element_size);

    /* Free the element to the free list */
    p_element->next = mq->free_element_list;
    mq->free_element_list = p_element;

    mq->num_of_element--;
    if (mq->pending_to_post > 0) {  /* If someone is waiting to post */
        pthread_mutex_lock(&mq->post_mutex);
        rv = pthread_cond_signal(&mq->post_cond); /* Wake up pending post threads */
        assert(rv == 0);
        pthread_mutex_unlock(&mq->post_mutex);
    }

    pthread_mutex_unlock(&mq->q_lock_mutex);
    return 0;
}

size_t
sal_msgq_count_get(sal_msgq_t msgq_hdl)
{
    rwlock_ctrl_t *mq = (rwlock_ctrl_t *)msgq_hdl;

    /* Input validation */
    if (!mq || (mq->signature != SAL_MSGQ_SIGNATURE)) {
        return 0;
    }
    /* No need to lock the queue as this value being updated atomically */
    return mq->num_of_element;
}
