/*! \file event_mgr.c
 *
 * This file contains the implementation of the event manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/event_mgr.h>

#include <shr/shr_timeout.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_thread.h>
#include <sal/sal_sleep.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition. */
#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Maximum length of assist thread name. */
#define ASSTHD_NAME_LEN_MAX 20

/* Assist thread priority. */
#define ASSTHD_PRI 50

/* Maximum callbacks registered to a event in event handle. */
#define EVENT_CB_MAX 5

/* Wait 1ms. */
#define WAIT_ONE_MSEC   1000

/* Wait 1s. */
#define WAIT_ONE_SEC    1000000

/* Retry count for thread exiting. */
#define RETRY_COUNT     10000

/*
 * Number of buckets of event hash table.
 *
 * The table count is 432 (BCMLTD_TABLE_COUNT) in SDKLT. Given that
 * application and BCM API Emulator are only interested in changes to
 * some of the tables, set the number of buckets with 64 for now.
 */
#define MAX_EVENT_HASH_COUNT 64

/* Assist thread types. */
typedef enum {
    /* Assist thread that invokes internal callback to process
     * table notification. */
    ASSTHD_INT,

    /* Assist thread that invokes application callback to process
     * table notification. */
    ASSTHD_EXT,

    ASSTHD_MAX
} assthd_type_t;

/* Assist thread name. Must align with assthd_type_t. */
static char *assthd_name[] = {
    "AssistThreadInt",
    "AssistThreadExt",
    "Invalid"
};

/* Callback list of an event. */
typedef struct event_cb_s {
    /* Pointer of next callback. */
    struct event_cb_s *next;

    /* Pointer of evnet callback. */
    bcmi_ltsw_event_cb cb;

    /* pointer to the opaque context which can be used during callback.*/
    void *user_data;
} event_cb_t;

/* Event handle structure. */
typedef struct event_hdl_s {
    /* Pointer of next event. */
    struct event_hdl_s *next;

    /* Table name. */
    const char *event;

    /* Callbacks to be invoked to process the event. */
    event_cb_t *cbs;

    /* Callback number. */
    int cb_cnt;

    /* Event handle is active, having snapshot and invoking callbacks. */
    bool active;

    /* Callback to be invoked to parse the table notification.  */
    bcmi_lt_parser parser;

    /* Length of event info. */
    uint32_t notif_info_len;

    /* Event attributes . */
    uint32_t attrib;

    /* Number of msg queued. */
    int pending_msg_num;

    /* Threshold of msg that can be queued. */
    int pending_msg_threshold;

} event_hdl_t;

/* Assist msgq message structure. */
typedef struct assq_msg_s {
    /* Next msg in the queue. */
    struct assq_msg_s *next;

    /* Event string. */
    const char *event;

    /* Event handle */
    event_hdl_t *eh;

    /* Message data. */
    void *data;
} assq_msg_t;

/* Assist thread message queue structure. */
typedef struct assist_msgq_s {
    /* Queue head. */
    assq_msg_t *head;

    /* Queue rear. */
    assq_msg_t *rear;

    /* Queued message count. */
    uint32_t count;

    /* Watermark for maximum message in queue. */
    int watermark;

    /* Queue lock. */
    sal_spinlock_t lock;
} assist_msgq_t;

/* Assist thread info structure. */
typedef struct assist_thread_s {
    /* Thread name. */
    char name[ASSTHD_NAME_LEN_MAX];

    /* Thread control object. */
    shr_thread_t tc;

    /* Message queue. */
    assist_msgq_t msgq;


} assist_thread_t;

/* Event manager control info. */
typedef struct event_mgr_ctrl_s {
    /* Unit number. */
    int unit;

    /* Mutex handle. */
    sal_mutex_t mutex;

    /* Assit threads. */
    assist_thread_t assthd[ASSTHD_MAX];

    /* Hash table that keep the event manage info for subscribed table. */
    event_hdl_t *event_hdl[MAX_EVENT_HASH_COUNT];

    /* Event management is initialized. */
    int inited;

    /* Number of running assist thread. */
    int running_threads;
} event_mgr_ctrl_t;

static event_mgr_ctrl_t evm_ctrl_info[BCM_MAX_NUM_UNITS] = {{0}};

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Initialized assist thread message queue.
 *
 * \param [in] msgq Message queue.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY Out of memory.
 */
static int
assist_msgq_init(assist_msgq_t *msgq)
{
    msgq->head = NULL;
    msgq->rear = NULL;
    msgq->count = 0;
    msgq->lock = sal_spinlock_create("bcmLtswEventmgrMsgqLock");
    if (!msgq->lock) {
        return SHR_E_MEMORY;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Clean up assist thread message queue.
 *
 * \param [in] msgq Message queue.
 *
 * \retval SHR_E_NONE No error.
 */
static int
assist_msgq_cleanup(assist_msgq_t *msgq)
{
    sal_spinlock_lock(msgq->lock);
    msgq->head = NULL;
    msgq->rear = NULL;
    msgq->count = 0;
    sal_spinlock_unlock(msgq->lock);

    sal_spinlock_destroy(msgq->lock);
    msgq->lock = NULL;

    return SHR_E_NONE;
}

/*!
 * \brief Check if the msgq is empty.
 *
 * \param [in] msgq Message queue.
 *
 * \retval SHR_E_NONE No error.
 */
static int
assist_msgq_is_empty(assist_msgq_t *msgq)
{
    return (msgq->head == NULL) ? true : false;
}

/*!
 * \brief Post msg into assist msgq.
 *
 * \param [in] msgq Message queue.
 * \param [in] msg Message.
 *
 * \retval SHR_E_NONE No error.
 */
static int
assist_msgq_enqueue(assist_msgq_t *msgq, assq_msg_t *msg)
{
    sal_spinlock_lock(msgq->lock);
    if (!msgq->head) {
        msgq->head = msg;
    } else {
        msgq->rear->next = msg;
    }
    msgq->rear = msg;
    msgq->count++;
    if (msgq->count > msgq->watermark) {
        msgq->watermark = msgq->count;
    }
    sal_spinlock_unlock(msgq->lock);

    return SHR_E_NONE;
}

/*!
 * \brief Pop msg from assist msgq.
 *
 * \param [in] msgq Message queue.
 * \param [out] msg Message.
 *
 * \retval SHR_E_NONE No error.
 */
static int
assist_msgq_dequeue(assist_msgq_t *msgq, assq_msg_t **msg)
{
    sal_spinlock_lock(msgq->lock);
    *msg = msgq->head;
    if (*msg) {
        msgq->head = (*msg)->next;
        (*msg)->next = NULL;
        msgq->count--;
    }
    sal_spinlock_unlock(msgq->lock);

    return SHR_E_NONE;
}

/*!
 * \brief Hashing string function.
 *
 * This function being called to generate the index in event hash table
 * (event_hdl[MAX_EVENT_HASH_COUNT]).
 *
 * Based on djb2 hash function.
 *
 * \param [in] str String.
 *
 * \retval Hash id.
 */
static int
hash(const char *str)
{
    uint32_t h = 5381;
    int i = 0;

    while(str[i] != 0 ) {
        h = ((h << 5) + h) + str[i]; /* h * 33 + str[i] */
        i++;
    }

    h = (((h << 5) + h) + i) % MAX_EVENT_HASH_COUNT;

    return h;
}

/*!
 * \brief Find handle of given event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event string.
 *
 * \return Pointer to event handle.
 */
static event_hdl_t *
event_hdl_find(int unit, const char *event)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    event_hdl_t *eh;
    int h = hash(event);

    eh = ci->event_hdl[h];
    while (eh != NULL) {
        if (sal_strcmp(eh->event, event) == 0) {
            break;
        }
        eh = eh->next;
    }

    return eh;
}

/*!
 * \brief Create handle for given event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event string.
 * \param [in] event_hdl Pointer to created event handle.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_EXISTS Event manage info of given table exists.
 * \retval SHR_E_MEMORY Out of memory.
 */
static int
event_hdl_create(int unit, const char *event, event_hdl_t **event_hdl)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    event_hdl_t *eh;
    int h = hash(event);

    SHR_FUNC_ENTER(unit);

    eh = ci->event_hdl[h];
    while (eh != NULL) {
        if (sal_strcmp(eh->event, event) == 0) {
            break;
        }
        eh = eh->next;
    }

    if (eh != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    eh = sal_alloc(sizeof(event_hdl_t), "eventmgrEventHdl");
    SHR_NULL_CHECK(eh, SHR_E_MEMORY);
    sal_memset(eh, 0, sizeof(event_hdl_t));
    eh->event = event;
    eh->next = ci->event_hdl[h];
    ci->event_hdl[h] = eh;
    *event_hdl = eh;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy handle of given event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event string.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_NOT_FOUND Event handle does not exist.
 */
static int
event_hdl_destroy(int unit, const char *event)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    event_hdl_t *eh, *pre = NULL;
    event_cb_t *item;
    int h = hash(event);

    SHR_FUNC_ENTER(unit);

    eh = ci->event_hdl[h];
    while (eh) {
        if (sal_strcmp(eh->event, event) == 0) {
            break;
        }
        pre = eh;
        eh = eh->next;
    }

    if (eh == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {

        if (eh->pending_msg_num) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Deleting event handle for %s: "
                                    "%d pending msg in the queue.\n"),
                         eh->event, eh->pending_msg_num));
        }

        while (eh->cbs != NULL) {
            item = eh->cbs;
            eh->cbs = item->next;
            sal_free(item);
        }

        if (ci->event_hdl[h] == eh) {
            ci->event_hdl[h] = eh->next;
        } else {
            pre->next = eh->next;
        }
        sal_free(eh);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up event manage info.
 *
 * \param [in] unit Unit number.
 *
 * \reture none.
 */
static void
event_hdl_cleanup(int unit)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    event_hdl_t *eh;
    event_cb_t *item;
    int i;

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);

    for (i = 0; i < MAX_EVENT_HASH_COUNT; i++) {
        while (ci->event_hdl[i] != NULL) {
            eh = ci->event_hdl[i];
            item = eh->cbs;
            while (eh->cbs != 0) {
                item = eh->cbs;
                eh->cbs = item->next;
                sal_free(item);
            }
            ci->event_hdl[i] = eh->next;
            sal_free(eh);
        }
    }

    sal_mutex_give(ci->mutex);
}

/*!
 * \brief Register callback function to event.
 *
 * \param [in] unit Unit number.
 * \param [in] event Event to be associated with.
 * \param [in] callback Callback function to call when the event being posted.
 * \param [in] user_data Opaque context for notifications.
 * \param [out] first_cb First callback registered to an event.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Invalid event or callback function.
 */
static int
event_cb_register(int unit,
                  const char *event,
                  bcmi_ltsw_event_cb callback,
                  void *user_data,
                  bool *first_cb)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    event_cb_t *item;
    bool locked = false, first = false, new_eh = false;

    SHR_FUNC_ENTER(unit);

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, event);
    if (eh == NULL) {
        SHR_IF_ERR_EXIT
            (event_hdl_create(unit, event, &eh));
        new_eh = true;
    } else {
        item = eh->cbs;
        while (item) {
            if (item->cb == callback) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            }
            item = item->next;
        }
    }

    if (eh->cbs == NULL) {
        first = true;
    } else if (eh->cb_cnt >= EVENT_CB_MAX) {
        SHR_ERR_MSG_EXIT(SHR_E_FULL,
                         (BSL_META_U(unit,
                                     "Number of callbacak registered on "
                                     "event %s reach the maximum (%d).\n"),
                          event, eh->cb_cnt));
    }

    item = sal_alloc(sizeof(event_cb_t), "eventmgrEventCb");
    SHR_NULL_CHECK(item, SHR_E_MEMORY);
    sal_memset(item, 0, sizeof(event_cb_t));
    item->cb = callback;
    item->user_data = user_data;
    item->next = eh->cbs;
    eh->cbs = item;
    eh->cb_cnt++;

    sal_mutex_give(ci->mutex);
    locked = false;

exit:
    if (SHR_FUNC_ERR() && (new_eh)) {
        event_hdl_destroy(unit, event);
        first = false;
    }
    if (first_cb) {
        *first_cb = first;
    }
    if (locked) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deregister a callback to an event.
 *
 * \param [in] unit The device number for the table of interest.
 * \param [in] event Event name.
 * \param [in] callback The callback function to be deregistered.
 * \param [out] last_cb Indicate that the last callback is deregistered.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
event_cb_unregister(int unit,
                    const char *event,
                    bcmi_ltsw_event_cb callback,
                    bool *last_cb)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    event_cb_t *item, *pre;
    bool locked = false, last = false;
    shr_timeout_t to;

    SHR_FUNC_ENTER(unit);

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, event);
    if (eh == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        item = eh->cbs;
        while (item) {
            if (item->cb == callback) {
                break;
            }
            pre = item;
            item = item->next;
        }
        if (item == NULL) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else if (eh->cbs == item) {
            /* The first element matches. */
            eh->cbs = item->next;
        } else {
            pre->next = item->next;
        }
        sal_free(item);
        eh->cb_cnt--;
        if (eh->cbs == NULL) {
            last = true;
        }
    }
    sal_mutex_give(ci->mutex);
    locked = false;

    if (last_cb) {
        *last_cb = last;
    }

    /* Wait for callback in snapshot to complete, to prevent the
     * module from being detached before the callback complete. */
    if (last && eh->active) {
        shr_timeout_init(&to, WAIT_ONE_SEC, 0);

        while (true) {
            if (!eh->active) {
                break;
            }
            if (shr_timeout_check(&to)) {
                SHR_ERR_EXIT(SHR_E_TIMEOUT);
            }
        }
    }

exit:
    if (locked == true) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal assist thread.
 *
 * This thread handles the notifications of changes to the tables that are
 * subscribed by modules internally.
 * This thread is running per unit.
 *
 * \param [in] arg Table event manage info context (\ref event_mgr_ctrl_t).
 *
 * \retval none.
 */
static void
internal_assist_thread(shr_thread_t tc, void *arg)
{
    event_mgr_ctrl_t *ci = (event_mgr_ctrl_t *)arg;
    assist_thread_t *assthd = &ci->assthd[ASSTHD_INT];
    assq_msg_t *msg = NULL;
    event_hdl_t *eh;
    event_cb_t *item;
    event_cb_t cb_snapshot[EVENT_CB_MAX];
    int unit = ci->unit;
    bool running = true;
    int cnt, i;

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    ci->running_threads++;
    sal_mutex_give(ci->mutex);

    while (running) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        while (!assist_msgq_is_empty(&assthd->msgq)) {
            assist_msgq_dequeue(&assthd->msgq, &msg);
            if (!msg) {
                break;
            }

            cnt = 0;
            i = 0;

            sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
            eh = msg->eh;
            if (eh) {
                eh->pending_msg_num--;
                item = eh->cbs;
                cnt = eh->cb_cnt;
                if (cnt < 0 || cnt > EVENT_CB_MAX) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Number of callback for event %s "
                                          "is invalid %d.\n"),
                              eh->event, cnt));
                    cnt = EVENT_CB_MAX;
                }
                if (cnt) {
                    while (item) {
                        cb_snapshot[i].cb = item->cb;
                        cb_snapshot[i].user_data = item->user_data;
                        item = item->next;
                        i++;
                        if (i == EVENT_CB_MAX) {
                            break;
                        }
                    }
                    eh->active = true;
                }
            }
            sal_mutex_give(ci->mutex);

            if (cnt) {
                for (i = 0; i < cnt; i++) {
                    cb_snapshot[i].cb(unit, msg->event, msg->data,
                                      cb_snapshot[i].user_data);
                }
            }

            SHR_FREE(msg);

            sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
            eh->active = false;
            sal_mutex_give(ci->mutex);
        }

        if (shr_thread_stopping(tc)) {
            break;
        }
    }

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    ci->running_threads--;
    sal_mutex_give(ci->mutex);
}

/*!
 * \brief External assist thread.
 *
 * This thread handles the notifications of changes to the tables that are
 * subscribed by applications externally.
 * This thread is running per unit.
 *
 * \param [in] arg Table event manage info context (\ref event_mgr_ctrl_t).
 *
 * \retval none.
 */
static void
external_assist_thread(shr_thread_t tc, void *arg)
{
    event_mgr_ctrl_t *ci = (event_mgr_ctrl_t *)arg;
    assist_thread_t *assthd = &ci->assthd[ASSTHD_EXT];
    assq_msg_t *msg = NULL;
    event_hdl_t *eh;
    event_cb_t *item;
    event_cb_t cb_snapshot[EVENT_CB_MAX];
    int unit = ci->unit;
    bool running = true;
    int cnt, i;

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    ci->running_threads++;
    sal_mutex_give(ci->mutex);

    while (running) {
        shr_thread_sleep(tc, SHR_THREAD_FOREVER);
        while (!assist_msgq_is_empty(&assthd->msgq)) {
            assist_msgq_dequeue(&assthd->msgq, &msg);
            if (!msg) {
                break;
            }

            cnt = 0;
            i = 0;

            sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
            eh = msg->eh;
            if (eh) {
                eh->pending_msg_num--;
                item = eh->cbs;
                cnt = eh->cb_cnt;
                if (cnt < 0 || cnt > EVENT_CB_MAX) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "Number of callback for event %s "
                                          "is invalid %d.\n"),
                              eh->event, cnt));
                    cnt = EVENT_CB_MAX;
                }
                if (cnt) {
                    while (item) {
                        cb_snapshot[i].cb = item->cb;
                        cb_snapshot[i].user_data = item->user_data;
                        item = item->next;
                        i++;
                        if (i == EVENT_CB_MAX) {
                            break;
                        }
                    }
                    eh->active = true;
                }
            }
            sal_mutex_give(ci->mutex);

            if (cnt) {
                for (i = 0; i < cnt; i++) {
                    cb_snapshot[i].cb(unit, msg->event, msg->data,
                                      cb_snapshot[i].user_data);
                }
            }

            SHR_FREE(msg);

            sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
            eh->active = false;
            sal_mutex_give(ci->mutex);
        }

        if (shr_thread_stopping(tc)) {
            break;
        }
    }

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    ci->running_threads--;
    sal_mutex_give(ci->mutex);
}

static void
(*event_assist_threads[ASSTHD_MAX])(shr_thread_t tc, void *arg) = {
    &internal_assist_thread,
    &external_assist_thread
};

/*!
 * \brief Callback function to handle table notification.
 *
 * \param [in] table_notify_info The table notify information.
 * \param [in] user_data The context provided at the time of
 * table subscription.
 *
 * \return none
 */
static void
lt_event_cb(bcmlt_table_notif_info_t *notify_info, void *user_data)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    bcmi_lt_parser parser;
    uint32_t notif_info_len, attrib;
    assq_msg_t *msg = NULL;
    int unit, thd, msglen;
    bcmi_ltsw_event_status_t status = bcmiLtswEventStatusPassNotif;

    unit = bcmi_ltsw_dev_unit(notify_info->unit);
    if (unit < 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Received LT event on invalid unit.\n")));
        return;
    }

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);

    eh = event_hdl_find(unit, notify_info->table_name);
    if (!eh || !eh->parser || !eh->cbs) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No callback to handle the event %s.\n"),
                     notify_info->table_name));
        sal_mutex_give(ci->mutex);
        return;
    }

    if (eh->pending_msg_threshold &&
        (eh->pending_msg_num >= eh->pending_msg_threshold)) {
        sal_mutex_give(ci->mutex);
        return;
    }

    parser = eh->parser;
    attrib = eh->attrib;
    notif_info_len = eh->notif_info_len;

    sal_mutex_give(ci->mutex);

    msglen = sizeof(assq_msg_t) + notif_info_len;
    msg = sal_alloc(msglen, "bcmLtswEventmgrMsg");
    if (msg == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to allocate memory for msg.\n")));
        return;
    }

    sal_memset(msg, 0, msglen);
    msg->data = (char *)msg + sizeof(assq_msg_t);
    parser(unit, notify_info->table_name, notify_info->entry_hdl, msg->data,
           &status);
    if (status == bcmiLtswEventStatusDismissNotif) {
        SHR_FREE(msg);
        return;
    }

    thd = attrib & BCMI_LTSW_EVENT_APPL_CALLBACK ? ASSTHD_EXT : ASSTHD_INT;

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    eh->pending_msg_num++;
    sal_mutex_give(ci->mutex);

    msg->event = notify_info->table_name;
    msg->eh = eh;
    assist_msgq_enqueue(&ci->assthd[thd].msgq, msg);
    shr_thread_wake(ci->assthd[thd].tc);

    return;
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_event_mgr_init(int unit)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    assist_thread_t *assthd;
    int i;

    SHR_FUNC_ENTER(unit);

    if (ci->inited) {
        SHR_IF_ERR_EXIT
            (bcmi_event_mgr_deinit(unit));
    } else {
        sal_memset(ci, 0, sizeof(event_mgr_ctrl_t));
    }

    if (ci->mutex == NULL) {
       ci->mutex = sal_mutex_create("event_mgr_ctrl_mutex");
       SHR_NULL_CHECK(ci->mutex, SHR_E_MEMORY);
    }

    for (i = 0; i < ASSTHD_MAX; i++) {
        assthd = &ci->assthd[i];

        SHR_IF_ERR_EXIT
            (assist_msgq_init(&assthd->msgq));

        sal_snprintf(assthd->name, ASSTHD_NAME_LEN_MAX, "%s.%d",
                     assthd_name[i], unit);
        assthd->tc = shr_thread_start(assthd->name,
                                      ASSTHD_PRI,
                                      event_assist_threads[i],
                                      (void *)ci);
        SHR_NULL_CHECK(assthd->tc, SHR_E_MEMORY);
    }

    ci->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        bcmi_event_mgr_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_event_mgr_deinit(int unit)
{
    event_mgr_ctrl_t *ci = &evm_ctrl_info[unit];
    assist_thread_t *assthd;
    assq_msg_t *msg = NULL;
    int i, rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!ci->inited) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    ci->inited = 0;
    sal_mutex_give(ci->mutex);

    for (i = 0; i < ASSTHD_MAX; i++) {
        assthd = &ci->assthd[i];
        if (SHR_FAILURE(shr_thread_stop(assthd->tc, 500000))) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Assist thread will not exit.\n")));
            rv = SHR_E_TIMEOUT;
        }
        assthd->tc = NULL;

        while(1) {
            assist_msgq_dequeue(&assthd->msgq, &msg);
            if (!msg) {
                break;
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Pending msg in assist thread.\n")));
                SHR_FREE(msg);
            }
        }
        assist_msgq_cleanup(&assthd->msgq);
    }

    event_hdl_cleanup(unit);

    if (ci->mutex) {
        sal_mutex_destroy(ci->mutex);
    }

    sal_memset(ci, 0, sizeof(*ci));

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_event_register(int unit,
                         const char *event,
                         bcmi_ltsw_event_cb callback,
                         void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (event_cb_register(unit, event, callback, user_data, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_event_unregister(int unit,
                           const char *event,
                           bcmi_ltsw_event_cb callback)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (event_cb_unregister(unit, event, callback, NULL));

exit:
    SHR_FUNC_EXIT();
}

void
bcmi_ltsw_event_notify(int unit,
                       const char *event,
                       void *notif_info,
                       int notif_info_len)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh = NULL;
    assq_msg_t *msg = NULL;
    int thd, msglen;

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);

    eh = event_hdl_find(unit, event);
    if (!eh || !eh->cbs) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "No callback to handle the event %s.\n"),
                     event));
        sal_mutex_give(ci->mutex);
        return;
    }

    if (eh->pending_msg_threshold &&
        (eh->pending_msg_num >= eh->pending_msg_threshold)) {
        sal_mutex_give(ci->mutex);
        return;
    }

    thd = eh->attrib & BCMI_LTSW_EVENT_APPL_CALLBACK ? ASSTHD_EXT : ASSTHD_INT;

    sal_mutex_give(ci->mutex);

    msglen = sizeof(assq_msg_t) + notif_info_len;
    msg = sal_alloc(msglen, "bcmLtswEventmgrMsg");
    if (msg == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to allocate memory for msg.\n")));
        return;
    }

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    eh->pending_msg_num++;
    sal_mutex_give(ci->mutex);

    sal_memset(msg, 0, msglen);
    msg->event = event;
    msg->data = (char *)msg + sizeof(assq_msg_t);
    msg->eh = eh;
    sal_memcpy(msg->data, notif_info, notif_info_len);

    assist_msgq_enqueue(&ci->assthd[thd].msgq, msg);
    shr_thread_wake(ci->assthd[thd].tc);

    return;
}

int
bcmi_ltsw_event_attrib_set(int unit, const char *event, uint32_t attrib)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, event);
    SHR_NULL_CHECK(eh, SHR_E_NOT_FOUND);
    eh->attrib = attrib;

    sal_mutex_give(ci->mutex);
    locked = false;

exit:
    if (locked == true) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_lt_table_subscribe(int unit,
                        const char *lt_name,
                        bcmi_ltsw_event_cb callback,
                        void *user_data)
{
    int dunit;
    bool first_cb = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (event_cb_register(unit, lt_name, callback, user_data, &first_cb));

    if (first_cb) {
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_EXIT
            (bcmlt_table_subscribe(dunit, lt_name, lt_event_cb, NULL));
    }

exit:
    if (SHR_FUNC_ERR()) {
        (void)bcmi_lt_table_unsubscribe(unit, lt_name, callback);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_lt_table_parser_set(int unit,
                         const char *lt_name,
                         bcmi_lt_parser parser,
                         uint32_t event_info_len)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, lt_name);
    if (eh == NULL) {
        SHR_IF_ERR_EXIT
            (event_hdl_create(unit, lt_name, &eh));
    }
    eh->parser = parser;
    eh->notif_info_len = event_info_len;

    sal_mutex_give(ci->mutex);
    locked = false;

exit:
    if (locked == true) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_lt_table_attrib_set(int unit,
                         const char *lt_name,
                         uint32_t attrib)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_event_attrib_set(unit, lt_name, attrib));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_lt_table_unsubscribe(int unit,
                          const char *lt_name,
                          bcmi_ltsw_event_cb callback)
{
    int dunit;
    bool last_cb = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT
        (event_cb_unregister(unit, lt_name, callback, &last_cb));

    if (last_cb) {
        /* No callback for this table. */
        dunit = bcmi_ltsw_dev_dunit(unit);
        SHR_IF_ERR_EXIT
            (bcmlt_table_unsubscribe(dunit, lt_name));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_event_msg_num_get(int unit, const char *event, uint32_t *num)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh = NULL;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(num, SHR_E_PARAM);

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, event);
    SHR_NULL_CHECK(eh, SHR_E_NOT_FOUND);

    *num = eh->pending_msg_num;

exit:
    if (locked) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_event_msg_threshold_set(int unit, const char *event,
                                  uint32_t threshold)
{
    event_mgr_ctrl_t *ci;
    event_hdl_t *eh;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    ci = &evm_ctrl_info[unit];

    sal_mutex_take(ci->mutex, SAL_MUTEX_FOREVER);
    locked = true;

    eh = event_hdl_find(unit, event);
    SHR_NULL_CHECK(eh, SHR_E_NOT_FOUND);
    eh->pending_msg_threshold = threshold;

    sal_mutex_give(ci->mutex);
    locked = false;

exit:
    if (locked == true) {
        sal_mutex_give(ci->mutex);
    }
    SHR_FUNC_EXIT();
}
