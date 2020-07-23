/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    sync_monotonic.c
 * Purpose: Defines sal routines for mutexes and semaphores using more modern
 *          condition variables and the monotonic clock.
 *
 * Mutex and Binary Semaphore abstraction
 *
 * Note: the SAL mutex abstraction is required to allow the same mutex
 * to be taken recursively by the same thread without deadlock.
 *
 * The POSIX mutex used here has been further abstracted to ensure this.
 */

#if defined(LINUX) || defined(linux) || defined(__linux__)
#define _XOPEN_SOURCE 600
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>

#include <assert.h>
#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <sal/core/time.h>

#ifdef BCM_MONOTONIC_MUTEXES

#if defined (__STRICT_ANSI__)
#define NO_CONTROL_C
#endif

#ifndef SECOND_NSEC
#define SECOND_NSEC     (SECOND_USEC * 1000)
#endif

#if defined(BROADCOM_DEBUG) && defined(INCLUDE_BCM_SAL_PROFILE)
#include "string.h"

static unsigned int _sal_sem_count_curr;
static unsigned int _sal_sem_count_max;
static unsigned int _sal_mutex_count_curr;
static unsigned int _sal_mutex_count_max;

#define FILE_LOC_NAME_MAX 128
#define MUTEX_DBG_ARR_MAX 5000
#define DEBUG_SAL_PROFILE 1

#define SAL_SEM_RESOURCE_USAGE_INCR(a_curr, a_max)             \
    do {                                                       \
        a_curr++;                                              \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max);     \
    } while (0)
    
#define SAL_SEM_RESOURCE_USAGE_DECR(a_curr) \
    do {                                    \
      a_curr--;                             \
    } while(0)

#else
#define SAL_SEM_RESOURCE_USAGE_INCR(a_curr, a_max)  ;
#define SAL_SEM_RESOURCE_USAGE_DECR(a_curr)         ;
#endif

#ifdef DEBUG_SAL_PROFILE
/*
 * Function:
 *      sal_sem_resource_usage_get
 * Purpose:
 *      Provides count of active sem and maximum sem allocation
 * Parameters:
 *      sem_curr - Current semaphore allocation.
 *      sem_max - Maximum semaphore allocation.
 */

void
sal_sem_resource_usage_get(unsigned int *sem_curr, unsigned int *sem_max)
{
    if (sem_curr != NULL) {
        *sem_curr = _sal_sem_count_curr;
    }
    if (sem_max != NULL) {
        *sem_max = _sal_sem_count_max;
    }
}

/*
 * Function:
 *      sal_mutex_resource_usage_get
 * Purpose:
 *      Provides count of active mutex and maximum mutex allocation
 * Parameters:
 *      mutex_curr - Current mutex allocation.
 *      mutex_max - Maximum mutex allocation.
 */

void
sal_mutex_resource_usage_get(unsigned int *mutex_curr, unsigned int *mutex_max)
{
    if (mutex_curr != NULL) {
        *mutex_curr = _sal_mutex_count_curr;
    }
    if (mutex_max != NULL) {
        *mutex_max = _sal_mutex_count_max;
    }
}
#endif /* DEBUG_SAL_PROFILE */

/*
 * Keyboard interrupt protection
 *
 *   When a thread is running on a console, the user could Control-C
 *   while a mutex is held by the thread.  Control-C results in a signal
 *   that longjmp's somewhere else.  We prevent this from happening by
 *   blocking Control-C signals while any mutex is held.
 */

#ifndef NO_CONTROL_C

static int ctrl_c_depth = 0;
int ctrl_c_blocked = 0;
static sigset_t ctrl_c_sigset;
static int ctrl_c_initialized;

static void 
ctrl_c_block(void)
{
    if (sal_thread_self() == sal_thread_main_get()) {
        if (ctrl_c_depth++ == 0) {
            sigprocmask(SIG_BLOCK, &ctrl_c_sigset, NULL);
            ctrl_c_blocked = 1;
        }
    }
}

static void
ctrl_c_unblock(void)
{
    if (sal_thread_self() == sal_thread_main_get()) {
        assert(ctrl_c_depth > 0);
        if (--ctrl_c_depth == 0) {
            sigprocmask(SIG_UNBLOCK, &ctrl_c_sigset, NULL);
            ctrl_c_blocked = 0;
        }
    }
}

static void _init_ctrl_c() {
    if (!ctrl_c_initialized) {
        sigemptyset(&ctrl_c_sigset);
        sigaddset(&ctrl_c_sigset, SIGINT);
        ctrl_c_initialized = 1;
    }
}

#else /* NO_CONTROL_C is defined */

static void ctrl_c_block(void)
{
}

static void ctrl_c_unblock(void)
{
}

static void _init_ctrl_c(void) 
{
}

#endif /* NO_CONTROL_C */

static int
_sal_compute_timeout(struct timespec *ts, int usec)
{
    int sec;
    uint32 nsecs;

    if (clock_gettime(CLOCK_MONOTONIC, ts)) {
        if (clock_gettime(CLOCK_REALTIME, ts)) {
            struct timeval  ltv;

            /* Fall back to RTC if realtime clock unavailable */
            gettimeofday(&ltv, 0);
            ts->tv_sec = ltv.tv_sec;
            ts->tv_nsec = ltv.tv_usec * 1000;
        }
    }

    /* Add in the delay */
    ts->tv_sec += usec / SECOND_USEC;

    /* compute new nsecs */
    nsecs = ts->tv_nsec + (usec % SECOND_USEC) * 1000;

    /* detect and handle rollover */
    if (nsecs < ts->tv_nsec) {
        ts->tv_sec += 1;
        nsecs -= SECOND_NSEC;
    }
    ts->tv_nsec = nsecs;

    /* Normalize if needed */
    sec = ts->tv_nsec / SECOND_NSEC;
    if (sec) {
        ts->tv_sec += sec;
        ts->tv_nsec = ts->tv_nsec % SECOND_NSEC;
    }

    /* indicate that we successfully got the time */
    return 1;
}

/*
 * recursive_mutex_t
 *
 *   This is an abstract type built on the POSIX mutex that allows a
 *   mutex to be taken recursively by the same thread without deadlock.
 *
 *   The Linux version of pthreads supports recursive mutexes
 *   (a non-portable extension to posix). In this case, we 
 *   use the Linux support instead of our own. 
 */

typedef struct recursive_mutex_s {
    int             val;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    char            *desc;
    sal_thread_t    owner;
    int             recurse_count;

#ifdef BROADCOM_DEBUG_MUTEX
    unsigned int ctrl_c_blk;
    unsigned int take_count;
    unsigned int give_count;
    unsigned int tk_exc_gv_ind;
    char         prev_file_tk_location[FILE_LOC_NAME_MAX];
    char         last_file_tk_location[FILE_LOC_NAME_MAX];

    char         prev_file_gv_location[FILE_LOC_NAME_MAX];
    char         last_file_gv_location[FILE_LOC_NAME_MAX];
#endif

} recursive_mutex_t;


#ifdef BROADCOM_DEBUG_MUTEX

static recursive_mutex_t *mutex_dbg_arr_ptr[MUTEX_DBG_ARR_MAX] = {0};

/*If you want to set GDB on a breakpoint for certain events */
void sal_mutext_dbg_break(void)
{
    sal_printf("\nGot Debugger Break Function Indication. Examine Call Stack Now\n");
}

int sal_mutex_take_intern(sal_mutex_t m, int usec);
int sal_mutex_give_intern(sal_mutex_t m);

int sal_mutex_take_bcm_debug(sal_mutex_t m, int usec, const char *take_loc, 
                             int line)
{
    char lineNum[8] = {'\0'};
    int retVal;
    recursive_mutex_t   *rm = (recursive_mutex_t *) m;
    assert(rm);

    retVal = sal_mutex_take_intern(m, usec);

    rm->take_count++;

    sal_sprintf(lineNum, ":%d", line);
    sal_strncpy(rm->prev_file_tk_location, rm->last_file_tk_location, 
                FILE_LOC_NAME_MAX - 6);
    sal_strncpy(rm->last_file_tk_location, take_loc, FILE_LOC_NAME_MAX - 6);
    sal_strcat(rm->last_file_tk_location, lineNum); 

   /* Detect recursion usage and flag*/
    if (rm->take_count > rm->give_count + 1 && !rm->tk_exc_gv_ind) {
        rm->tk_exc_gv_ind = 1; /* Only print first occurence */
        sal_printf ("\nMTX TK:%d EXCEEDS GV:%d\n", rm->take_count, 
                    rm->give_count);
        sal_printf ("AT: Last Loc:%s  Prev Loc: %s",
                rm->last_file_tk_location, rm->prev_file_tk_location);
        sal_mutext_dbg_break();
    }

    return retVal;
}

int sal_mutex_give_bcm_debug(sal_mutex_t m, const char *give_loc, int line)
{
    char lineNum[8] = {'\0'};
    recursive_mutex_t   *rm = (recursive_mutex_t *) m;
    assert(rm);
    rm->give_count++;

    sal_sprintf(lineNum, ":%d", line);
    sal_strncpy(rm->prev_file_gv_location, rm->last_file_gv_location, 
                FILE_LOC_NAME_MAX - 6);
    sal_strncpy(rm->last_file_gv_location, give_loc, FILE_LOC_NAME_MAX - 6);
    sal_strcat(rm->last_file_gv_location, lineNum);

    if (rm->give_count > rm->take_count) {
        sal_printf("\nERROR: MTX GV:%d EXCEEDS TK:%d\n", rm->give_count, 
                   rm->take_count);
        sal_mutext_dbg_break();
    }

    return sal_mutex_give_intern(m);
}

void sal_mutex_dbg_dump(void)
{
    int i = 0;
    char temp_ch;

    for (i = 0; i < MUTEX_DBG_ARR_MAX; i++) {
        /* Find nonempty slots */
        if (mutex_dbg_arr_ptr[i] != 0) {
            recursive_mutex_t *rm = mutex_dbg_arr_ptr[i];


            /* If MUTEX was created but never taken or given don't display it */
            if (rm->take_count || rm->give_count) {
                sal_printf ("\n\nMUTEX STATS[%d]  For:%s  Owner:ox%x CTRL_C_Depth:%d\n", 
                        i, rm->desc, (unsigned int)rm->owner, rm->ctrl_c_blk);

                sal_printf("\nT_CNT:%d G_CNT:%d \nPREV_T_LOC:%s  LST_T_LOC:%s\nPREV_G_LOC:%s  LST_G_LOC:%s", 
                       rm->take_count, rm->give_count,
                       rm->prev_file_tk_location, rm->last_file_tk_location,
                       rm->prev_file_gv_location, rm->last_file_gv_location);
            }

            if (rm->ctrl_c_blk > 0) {
                sal_printf ("\nWARNING: CTRL_C Left DISABLED??");
                temp_ch = getchar(); /*Pause  */
            }
 
            if (rm->take_count != rm->give_count) {
                sal_printf ("\nWARNING: Take != Give");
                temp_ch = getchar(); /*Pause  */
            }
        }
    }
}
#endif /* BROADCOM_DEBUG_MUTEX */

#ifdef netbsd
/*
 * The netbsd pthreads implementation we are using
 * does not seem to have his function
 */
static int
pthread_mutexattr_init(pthread_mutexattr_t* attr)
{
    attr->m_type = PTHREAD_MUTEXTYPE_DEBUG;
    attr->m_flags = 0;
    return 0;
}
#endif /* netbsd */

static sal_mutex_t
_sal_mutex_create(char *desc)
{
    recursive_mutex_t   *rm;
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t  cond_attr;
    struct timespec     ts;

    _init_ctrl_c();

    if ((rm = malloc(sizeof (recursive_mutex_t))) == NULL) {
        return NULL;
    }

    rm->desc          = desc;
    rm->owner         = 0;
    rm->recurse_count = 0;
    rm->val           = 1;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&rm->mutex, &mutex_attr);

    pthread_condattr_init(&cond_attr);
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
    }
    pthread_cond_init(&rm->cond, &cond_attr);

#ifdef BROADCOM_DEBUG_MUTEX
    {
        int i;

        rm->ctrl_c_blk = 0;
        rm->take_count = 0;
        rm->give_count = 0;
        rm->tk_exc_gv_ind = 0;

        sal_memset(rm->prev_file_tk_location, FILE_LOC_NAME_MAX, 0);
        sal_memset(rm->last_file_tk_location, FILE_LOC_NAME_MAX, 0);
        sal_memset(rm->prev_file_gv_location, FILE_LOC_NAME_MAX, 0);
        sal_memset(rm->last_file_gv_location, FILE_LOC_NAME_MAX, 0);

        for (i = 0; i < MUTEX_DBG_ARR_MAX; i++) {
            /* Find an empty slot */
            if (mutex_dbg_arr_ptr[i] == 0) {
                mutex_dbg_arr_ptr[i] = rm;
                break;
            }
        }
    }
#endif /* BROADCOM_DEBUG_MUTEX */

    SAL_SEM_RESOURCE_USAGE_INCR(_sal_mutex_count_curr, _sal_mutex_count_max);

    return (sal_mutex_t) rm;
}

/*
 * Mutex and semaphore abstraction
 */
sal_mutex_t
sal_mutex_create(char *desc)
{
#ifdef SAL_GLOBAL_MUTEX
    static sal_mutex_t _m = NULL;
    if (!_m) {
        _m = _sal_mutex_create("sal_global_mutex");
        assert(_m);
    }
    if (strcmp(desc, "spl mutex")) {
        return _m;
    }
#endif

    return _sal_mutex_create(desc);
}

void
sal_mutex_destroy(sal_mutex_t m)
{
    recursive_mutex_t   *rm = (recursive_mutex_t *) m;

    assert(rm);

    pthread_mutex_destroy(&rm->mutex);
    pthread_cond_destroy(&rm->cond);

    free(rm);

    SAL_SEM_RESOURCE_USAGE_DECR(_sal_mutex_count_curr);
}

/*
 * Both semaphores and mutexes are secretly mutexes, so the core of the take
 * logic can be shared between them
 */
static int 
sal_mutex_sem_take(pthread_mutex_t *mutex, pthread_cond_t *cond, 
                   int *val, int forever, int usec) 
{
    struct timespec ts;
    int             err = 0;

    if ((!forever) && (!_sal_compute_timeout(&ts, usec))) {
        err = -1; 
    }

    if (err == 0) {
        err = pthread_mutex_lock(mutex);

        while ((*val == 0) && (err == 0)) {
            if (forever) {
                err = pthread_cond_wait(cond, mutex);
            } else {
                err = pthread_cond_timedwait(cond, mutex, &ts);
            }
            
        }

        if (err == 0) {
            *val -= 1;
        }

        /* even if there's an error, try to unlock this... */
        pthread_mutex_unlock(mutex);
    }

    return err ? -1 : 0;
}

int
#ifdef BROADCOM_DEBUG_MUTEX
sal_mutex_take_intern(sal_mutex_t m, int usec)
#else
sal_mutex_take(sal_mutex_t m, int usec)
#endif
{
    recursive_mutex_t *rm = (recursive_mutex_t *)m;
    sal_thread_t      myself = sal_thread_self();
    int               err = 0;

    assert(rm);

    if (rm->owner == myself) {
        rm->recurse_count++;
        return 0;
    }

    ctrl_c_block();

#ifdef BROADCOM_DEBUG_MUTEX
    rm->ctrl_c_blk++;
#endif

    err = sal_mutex_sem_take(&rm->mutex, &rm->cond, &rm->val, 
                             usec == sal_mutex_FOREVER, usec);

    rm->owner = myself;

    if (err) {
        ctrl_c_unblock();

#ifdef BROADCOM_DEBUG_MUTEX
        printf("\n\nERROR in TAKING MUTEX \n\n");
        rm->ctrl_c_blk--;
#endif
        
        return -1;
    }
 
    return err ? -1 : 0;
}


int
#ifdef BROADCOM_DEBUG_MUTEX
sal_mutex_give_intern(sal_mutex_t m)
#else
sal_mutex_give(sal_mutex_t m)
#endif
{
    recursive_mutex_t  *rm = (recursive_mutex_t *) m;
    int                err = 0;
    sal_thread_t       myself = sal_thread_self();

    assert(rm);

    assert(rm->owner == myself);

    if (rm->recurse_count > 0) {
        rm->recurse_count--;
        return 0;
    }
    rm->owner = 0;
    pthread_mutex_lock(&rm->mutex);
    rm->val++;
    err = pthread_cond_broadcast(&rm->cond);
    /* even if there's an error, try to unlock this... */
    pthread_mutex_unlock(&rm->mutex);
    ctrl_c_unblock();

#ifdef BROADCOM_DEBUG_MUTEX
    rm->ctrl_c_blk--;
#endif

    return err ? -1 : 0;
}


/*
 * Wrapper class to hold additional info
 * along with the semaphore.
 */
typedef struct {
    int             binary;
    int             val;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    char            *desc;
} wrapped_sem_t;

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
    wrapped_sem_t       *s = NULL;
    struct timespec     ts;
    pthread_condattr_t  cond_attr;
    pthread_mutexattr_t mutex_attr;

    /* Ignore binary for now */

    if ((s = malloc(sizeof (wrapped_sem_t))) == NULL) {
        return NULL;
    }

    s->binary = binary;
    s->val = initial_count;
    s->desc = desc;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&s->mutex, &mutex_attr);

    pthread_condattr_init(&cond_attr);
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
    }
    pthread_cond_init(&s->cond, &cond_attr);

    SAL_SEM_RESOURCE_USAGE_INCR(_sal_sem_count_curr, _sal_sem_count_max);

    return (sal_sem_t) s;
}

void
sal_sem_destroy(sal_sem_t b)
{
    wrapped_sem_t *s = (wrapped_sem_t *) b;

    assert(s);

    pthread_mutex_destroy(&s->mutex);
    pthread_cond_destroy(&s->cond);

    free(s);

    SAL_SEM_RESOURCE_USAGE_DECR(_sal_sem_count_curr);
}

int
sal_sem_take(sal_sem_t b, int usec)
{
    wrapped_sem_t    *s = (wrapped_sem_t *) b;
    int              err = 0;

    if ((usec < 0) && (usec != sal_sem_FOREVER)) {
        /* Return error if negative timeout is specified */
        return -1;
    }

    err = sal_mutex_sem_take(&s->mutex, &s->cond, &s->val, 
                             usec == sal_sem_FOREVER, usec);

    return err ? -1 : 0;
}

int
sal_sem_give(sal_sem_t b)
{
    wrapped_sem_t *s = (wrapped_sem_t *) b;
    int           err;

    pthread_mutex_lock(&s->mutex);
    if (s->binary && s->val) {
        pthread_mutex_unlock(&s->mutex);
        return 0;
    }
    s->val++;
    err = pthread_cond_broadcast(&s->cond);
    pthread_mutex_unlock(&s->mutex);

    return err ? -1 : 0;
}

#endif /* BCM_MONOTONIC_MUTEXES */
