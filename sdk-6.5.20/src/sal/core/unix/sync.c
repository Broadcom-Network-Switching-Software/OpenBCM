/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	sync.c
 * Purpose:	Defines sal routines for mutexes and semaphores
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
#include <sal/core/libc.h>

#include <soc/dnxc/multithread_analyzer.h>

#ifndef BCM_MONOTONIC_MUTEXES

#if defined(LINUX) && defined(__USE_UNIX98)

#define USE_POSIX_RECURSIVE_MUTEX

#endif


#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS >= 200112L) && \
        defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS >= 200112L)
/* Sometimes customers want to set new real time, so they must use monotic time.
  * But if we define USE_POSIX_SEM_TIMEDWAIT or USE_POSIX_MUTEX_TIMEDLOCK,
  * we will use real time, so we add this new Micro */
#if !defined(BCM_MONOTONIC_TIME)
#define USE_POSIX_SEM_TIMEDWAIT
#define USE_POSIX_MUTEX_TIMEDLOCK
#endif /*BCM_MONOTONIC_TIMER*/
#endif

#ifndef USE_POSIX_SPINLOCK
#if defined(LINUX) && defined(__USE_XOPEN2K) && defined(__GLIBC__)
#define USE_POSIX_SPINLOCK 1
#else
#define USE_POSIX_SPINLOCK 0
#endif
#endif

#if defined (__STRICT_ANSI__)
#define NO_CONTROL_C
#endif

#ifndef SECOND_NSEC
#define SECOND_NSEC     (SECOND_USEC * 1000)
#endif

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
static unsigned int _sal_sem_count_curr;
static unsigned int _sal_sem_count_max;
static unsigned int _sal_mutex_count_curr;
static unsigned int _sal_mutex_count_max;
#define SAL_SEM_RESOURCE_USAGE_INCR(a_curr, a_max, ilock)               \
        a_curr++;                                                       \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)
    
#define SAL_SEM_RESOURCE_USAGE_DECR(a_curr, ilock)                      \
        a_curr--

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
#endif
#endif

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
static sal_thread_t main_thread = SAL_THREAD_ERROR;
#endif

static void
ctrl_c_block(void)
{
#ifndef NO_CONTROL_C
    if (main_thread == SAL_THREAD_ERROR) {
        main_thread = sal_thread_main_get();
    }
    if (sal_thread_self() == main_thread) {
	if (ctrl_c_depth++ == 0) {
	    sigprocmask(SIG_BLOCK, &ctrl_c_sigset, NULL);
        ctrl_c_blocked = 1;
	}
    }
#endif    
}

static void
ctrl_c_unblock(void)
{
#ifndef NO_CONTROL_C
    if (main_thread == SAL_THREAD_ERROR) {
        main_thread = sal_thread_main_get();
    }
    if (sal_thread_self() == main_thread) {
	assert(ctrl_c_depth > 0);
	if (--ctrl_c_depth == 0) {
	    sigprocmask(SIG_UNBLOCK, &ctrl_c_sigset, NULL);
        ctrl_c_blocked = 0;
	}
    }
#endif
}

#if defined(USE_POSIX_SEM_TIMEDWAIT) || defined(USE_POSIX_MUTEX_TIMEDLOCK)
static
int
_sal_compute_timeout(struct timespec *ts, int usec)
{
    int sec;
    uint32 nsecs;

#ifdef CLOCK_REALTIME
    if (clock_gettime(CLOCK_REALTIME, ts) == 0) {
        ;
    }
    else
#endif
    {
        struct timeval	ltv;

        /* Fall back to RTC if realtime clock unavailable */
        gettimeofday(&ltv, 0);
        ts->tv_sec = ltv.tv_sec;
        ts->tv_nsec = ltv.tv_usec * 1000;
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
#endif

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
    pthread_mutex_t	mutex;
    char		*desc;
#ifndef USE_POSIX_RECURSIVE_MUTEX
    sal_thread_t	owner;
    int			recurse_count;
#endif

#ifdef BROADCOM_DEBUG_MUTEX
#define FILE_LOC_NAME_MAX 128
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

#include "string.h"

#define MUTEX_DBG_ARR_MAX 5000
static recursive_mutex_t *mutex_dbg_arr_ptr[MUTEX_DBG_ARR_MAX] = {0};

/*If you want to set GDB on a breakpoint for certain events */
void sal_mutext_dbg_break(void)
{
    printf("\nGot Debugger Break Function Indication. Examine Call Stack Now\n");
}

int sal_mutex_take_intern(sal_mutex_t m, int usec);
int sal_mutex_give_intern(sal_mutex_t m);

int sal_mutex_take_bcm_debug(sal_mutex_t m, int usec, const char *take_loc, int line)
{
    char lineNum[8] = {'\0'};
    int retVal;
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    assert(rm);

    retVal = sal_mutex_take_intern(m, usec);

    rm->take_count++;

    sprintf(lineNum, ":%d", line);
	strncpy(rm->prev_file_tk_location, rm->last_file_tk_location, FILE_LOC_NAME_MAX - 6);
	strncpy(rm->last_file_tk_location, take_loc, FILE_LOC_NAME_MAX - 6);
    strcat(rm->last_file_tk_location, lineNum); 

   /* Detect recursion usage and flag*/
    if (rm->take_count > rm->give_count + 1 && !rm->tk_exc_gv_ind) {
        rm->tk_exc_gv_ind = 1; /* Only print first occurence */
        printf ("\nMTX TK:%d EXCEEDS GV:%d\n", rm->take_count, rm->give_count);
        printf ("AT: Last Loc:%s  Prev Loc: %s",
                rm->last_file_tk_location, rm->prev_file_tk_location);
        sal_mutext_dbg_break();
    }

    return retVal;
}


int sal_mutex_give_bcm_debug(sal_mutex_t m, const char *give_loc, int line)
{
    char lineNum[8] = {'\0'};
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    assert(rm);
    rm->give_count++;

    sprintf(lineNum, ":%d", line);
	strncpy(rm->prev_file_gv_location, rm->last_file_gv_location, FILE_LOC_NAME_MAX - 6);
	strncpy(rm->last_file_gv_location, give_loc, FILE_LOC_NAME_MAX - 6);
    strcat(rm->last_file_gv_location, lineNum);

    if (rm->give_count > rm->take_count) {
        printf ("\nERROR: MTX GV:%d EXCEEDS TK:%d\n", rm->give_count, rm->take_count);
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
                printf ("\n\nMUTEX STATS[%d]  For:%s  Owner:ox%x CTRL_C_Depth:%d\n", 
                        i, rm->desc, (unsigned int)rm->owner, rm->ctrl_c_blk);

                printf("\nT_CNT:%d G_CNT:%d \nPREV_T_LOC:%s  LST_T_LOC:%s\nPREV_G_LOC:%s  LST_G_LOC:%s", 
                       rm->take_count, rm->give_count,
                       rm->prev_file_tk_location, rm->last_file_tk_location,
                       rm->prev_file_gv_location, rm->last_file_gv_location);
            }

            if (rm->ctrl_c_blk > 0) {
                printf ("\nWARNING: CTRL_C Left DISABLED??");
                temp_ch = getchar(); /*Pause  */
            }
 
            if (rm->take_count != rm->give_count) {
                printf ("\nWARNING: Take != Give");
                temp_ch = getchar(); /*Pause  */
            }
        }
    }
}



#endif


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
    recursive_mutex_t	*rm;
    pthread_mutexattr_t attr;

#ifdef BROADCOM_DEBUG_MUTEX
    int i = 0;
#endif

#ifndef NO_CONTROL_C
    if (!ctrl_c_initialized) {
        sigemptyset(&ctrl_c_sigset);
        sigaddset(&ctrl_c_sigset, SIGINT);
        ctrl_c_initialized = 1;
    }
#endif
    
    if ((rm = malloc(sizeof (recursive_mutex_t))) == NULL) {
	return NULL;
    }

    rm->desc = desc;
    pthread_mutexattr_init(&attr);
#ifdef USE_POSIX_RECURSIVE_MUTEX
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#else
    rm->owner = 0;
    rm->recurse_count = 0;

#ifdef BROADCOM_DEBUG_MUTEX
    rm->ctrl_c_blk = 0;
    rm->take_count = 0;
    rm->give_count = 0;
    rm->tk_exc_gv_ind = 0;

    for (i = 0; i < FILE_LOC_NAME_MAX; i++) {
        rm->prev_file_tk_location[i] = '\0';
        rm->last_file_tk_location[i] = '\0';

        rm->prev_file_gv_location[i] = '\0';
        rm->last_file_gv_location[i] = '\0';
    }

    for (i = 0; i < MUTEX_DBG_ARR_MAX; i++) {
        /* Find an empty slot */
        if (mutex_dbg_arr_ptr[i] == 0) {
            mutex_dbg_arr_ptr[i] = rm;
            break;
        }
    }

#endif

#endif

    pthread_mutex_init(&rm->mutex, &attr);

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_INCR(
            _sal_mutex_count_curr,
            _sal_mutex_count_max,
            ilock);
#endif
#endif
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
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;

#ifdef BROADCOM_DEBUG_MUTEX
    int i;
#endif

    assert(rm);

#ifndef USE_POSIX_RECURSIVE_MUTEX
    /* Check for pending mutex unlocks */
    if ((rm->recurse_count > 0) || (rm->owner != 0)) {
        char thread_name[SAL_THREAD_NAME_MAX_LEN];
        sal_thread_name(rm->owner, thread_name, sizeof (thread_name));
        printf("WARNING: Mutex \"%s\" has not been unlocked before being destroyed.\n",
                        rm->desc);
        printf("\t Current owner is \"%s\"(%p) \n", thread_name, (void*)rm->owner);
    }
#ifdef SAL_MUTEX_DEBUG
    assert(rm->recurse_count == 0);
    assert(rm->owner == 0);
#endif /* SAL_MUTEX_DEBUG */

#ifdef BROADCOM_DEBUG_MUTEX
    assert(rm->take_count == rm->give_count);

    for (i = 0; i < FILE_LOC_NAME_MAX; i++) {
        rm->prev_file_tk_location[i] = '\0';
        rm->last_file_tk_location[i] = '\0';

        rm->prev_file_gv_location[i] = '\0';
        rm->last_file_gv_location[i] = '\0';
    }

    for (i = 0; i < MUTEX_DBG_ARR_MAX; i++) {
        /* Find it's slot and mark empty*/
        if (mutex_dbg_arr_ptr[i] == rm) {
            mutex_dbg_arr_ptr[i] = 0;
            break;
        }
    }

#endif

#endif /* !USE_POSIX_RECURSIVE_MUTEX */

    pthread_mutex_destroy(&rm->mutex);

    free(rm);

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_DECR(
            _sal_mutex_count_curr,
            ilock);
#endif
#endif
}

int
#ifdef BROADCOM_DEBUG_MUTEX
sal_mutex_take_intern(sal_mutex_t m, int usec)
#else
sal_mutex_take(sal_mutex_t m, int usec)
#endif
{
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    int			err = 0;

#ifndef USE_POSIX_RECURSIVE_MUTEX
    sal_thread_t	myself = sal_thread_self();
#endif

#ifdef USE_POSIX_MUTEX_TIMEDLOCK
    struct timespec	ts;
#endif

    assert(rm);

    
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(0, rm->desc, MTA_FLAG_MUTEX, 1));

#ifndef USE_POSIX_RECURSIVE_MUTEX
    if (rm->owner == myself) {

        rm->recurse_count++;

        return 0;
    }

#endif

    ctrl_c_block();

#ifdef BROADCOM_DEBUG_MUTEX
    rm->ctrl_c_blk++;
#endif

    if (usec == sal_mutex_FOREVER) {
        do {
            err = pthread_mutex_lock(&rm->mutex);
        } while (err != 0 && errno == EINTR);
    }

#ifdef USE_POSIX_MUTEX_TIMEDLOCK
    else if (_sal_compute_timeout(&ts, usec)) {
        /* Treat EAGAIN as a fatal error on Linux */
        err = pthread_mutex_timedlock(&rm->mutex, &ts);
    }
#else
    else {
        int		time_wait = 1;

        /* Retry algorithm with exponential backoff */

        for (;;) {
            err = pthread_mutex_trylock(&rm->mutex);

            if (err != EBUSY) {
                break;		/* Done (0), or error other than EBUSY */
            }

            if (time_wait > usec) {
                time_wait = usec;
            }

            sal_usleep(time_wait);

            usec -= time_wait;

            if (usec == 0) {
                err = ETIMEDOUT;
                break;
            }

            if ((time_wait *= 2) > 100000) {
                time_wait = 100000;
            }
        }
    }
#endif

    if (err) {
        ctrl_c_unblock();

#ifdef BROADCOM_DEBUG_MUTEX
        printf("\n\nERROR in TAKING MUTEX \n\n");
        rm->ctrl_c_blk--;
#endif
        if (err != ETIMEDOUT) {
            fprintf(stderr, "sal_mutex_take: %s\n", strerror(err));
        }
        assert(usec != sal_mutex_FOREVER);

        return -1;
    }

#ifndef USE_POSIX_RECURSIVE_MUTEX
    assert(rm->owner == 0);
    rm->owner = myself;
#endif
 
    return 0;
}



int
#ifdef BROADCOM_DEBUG_MUTEX
sal_mutex_give_intern(sal_mutex_t m)
#else
sal_mutex_give(sal_mutex_t m)
#endif
{
    recursive_mutex_t	*rm = (recursive_mutex_t *) m;
    int			err;

    assert(rm);

    
    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(0, rm->desc, MTA_FLAG_MUTEX, 0));

#ifndef USE_POSIX_RECURSIVE_MUTEX
    if ((rm->owner != sal_thread_self())) {
        assert(rm->owner == sal_thread_self());
    }

    if (rm->recurse_count > 0) {
        rm->recurse_count--;
        return 0;
    }

    rm->owner = 0;
#endif


    err = pthread_mutex_unlock(&rm->mutex);
    ctrl_c_unblock();

#ifdef BROADCOM_DEBUG_MUTEX
    rm->ctrl_c_blk--;
#endif

    if (err) {
        fprintf(stderr, "sal_mutex_give: %s\n", strerror(err));
    }
    assert(err == 0);

    return err ? -1 : 0;
}


/*
 * Wrapper class to hold additional info
 * along with the semaphore.
 */
typedef struct {
    sem_t       s;
    char        *desc;
    int         binary;
} wrapped_sem_t;

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
    wrapped_sem_t *s = NULL;

    if ((s = malloc(sizeof (wrapped_sem_t))) == NULL) {
        return NULL;
    }

    /* 
     * This is needed by some libraries with a bug requiring to zero sem_t before calling sem_init(),
     * even though this it is not required by the function description.
     * Threads using sem_timedwait() to maintain polling interval use 100% CPU if we not set the memory to zero SDK-77724 
     */ 
    sal_memset(s, 0, sizeof(wrapped_sem_t));

    sem_init(&s->s, 0, initial_count);
    s->desc = desc;
    s->binary = binary;

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_INCR(
            _sal_sem_count_curr,
            _sal_sem_count_max,
            ilock);
#endif
#endif

    return (sal_sem_t) s;
}

void
sal_sem_destroy(sal_sem_t b)
{
    wrapped_sem_t *s = (wrapped_sem_t *) b;

    assert(s);

    sem_destroy(&s->s);

    free(s);

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_DECR(
            _sal_sem_count_curr,
            ilock);
#endif
#endif
}

int
sal_sem_take(sal_sem_t b, int usec)
{
    wrapped_sem_t		*s = (wrapped_sem_t *) b;
    int			err = 0;
#ifdef USE_POSIX_SEM_TIMEDWAIT
    struct timespec	ts;
#endif

    if (s == NULL) {
        return 0;
    }

    if ((usec < 0) && (usec != sal_sem_FOREVER)) {
        /* Return error if negative timeout is specified */
        return -1;
    }

    if (usec == sal_sem_FOREVER) {
        do {
            err = sem_wait(&s->s);
        } while (err != 0 && errno == EINTR);
    }
#ifdef USE_POSIX_SEM_TIMEDWAIT
    else if (_sal_compute_timeout(&ts, usec)) {
        while (1) {
            if (!sem_timedwait(&s->s, &ts)) {
                err = 0;
                break;
            }
            if (errno != EAGAIN && errno != EINTR) {
                err = errno;
                break;
            }
        }
    }
#else
    else {
	int		time_wait = 1;

	/* Retry algorithm with exponential backoff */

	for (;;) {
	    if (sem_trywait(&s->s) == 0) {
		err = 0;
		break;
	    }

	    if (errno != EAGAIN && errno != EINTR) {
		err = errno;
		break;
	    }

	    if (time_wait > usec) {
		time_wait = usec;
	    }

	    sal_usleep(time_wait);

	    usec -= time_wait;

	    if (usec == 0) {
		err = ETIMEDOUT;
		break;
	    }

#ifdef BCM_MONOTONIC_TIME
        /* To reduce CPU share for some threads */
        if ((time_wait *= 4) > 100000) {
            time_wait = 100000;
        }
#else
        if ((time_wait *= 2) > 100000) {
            time_wait = 100000;
        }
#endif
	}
    }
#endif

    return err ? -1 : 0;
}

int
sal_sem_give(sal_sem_t b)
{
    wrapped_sem_t       *s = (wrapped_sem_t *) b;
    int                 err = 0;
    int                 sem_val = 0;

    if (s == NULL) {
        return 0;
    }

    /* Binary sem only post if sem_val == 0 */
    if (s->binary) {
        /* Post sem on getvalue failure */
        sem_getvalue(&s->s, &sem_val);
        if (sem_val == 0) {
            err = sem_post(&s->s);
        }
    } else {
        err = sem_post(&s->s);
    }

    return err ? -1 : 0;
}

#endif /* BCM_MONOTONIC_MUTEXES */

/*
 * spinlock_ctrl_t
 *
 *   This is an abstract type built on the POSIX spinlock.
 */

#if USE_POSIX_SPINLOCK
typedef struct spinlock_ctrl_s {
    pthread_spinlock_t spinlock;
    char *desc;
} *spinlock_ctrl_t;
#endif

/*
 * Function:
 *  sal_spinlock_create
 * Purpose:
 *  Create a spinlock
 * Parameters:
 *  desc - spinlock description
 * Returns:
 *  The spinlock or NULL if creation failed
 */

sal_spinlock_t
sal_spinlock_create(char *desc)
{
#if USE_POSIX_SPINLOCK
    spinlock_ctrl_t sl = malloc(sizeof(*sl));
    int result;

    if (sl != NULL) {
        result = pthread_spin_init(&(sl->spinlock), PTHREAD_PROCESS_SHARED);
        if (result != 0) {
            free(sl);
            return (sal_spinlock_t)NULL;
        }
        sl->desc = desc;
    }
    return (sal_spinlock_t)sl;
#else
    return (sal_spinlock_t)sal_mutex_create(desc);
#endif
}

/*
 * Function:
 *  sal_spinlock_destroy
 * Purpose:
 *  Destroy a spinlock
 * Parameters:
 *  lock - spinlock to destroy
 * Returns:
 *  0 on success, error code on failure
 */

int
sal_spinlock_destroy(sal_spinlock_t lock)
{
#if USE_POSIX_SPINLOCK
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;
    int result;

    assert(sl);
    result = pthread_spin_destroy(&sl->spinlock);
    free(sl);
    return result;
#else
    sal_mutex_destroy((sal_mutex_t)lock);
    return 0;
#endif
}

/*
 * Function:
 *  sal_spinlock_lock
 * Purpose:
 *  Obtains a spinlock
 * Parameters:
 *  lock - spninlock to obtain
 * Returns:
 *  0 on success, error code on failure
 */

int
sal_spinlock_lock(sal_spinlock_t lock)
{
#if USE_POSIX_SPINLOCK
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;
    struct timeval tv;

    assert(sl);
    /* On some systems pthread_spin_lock() is unsafe and won't block preemption,
       which could lead to deadlock. So here pthread_spin_trylock() is used instead
       to avoid this kind of issue. */
    while (pthread_spin_trylock(&sl->spinlock)) {
        tv.tv_sec = 0;
        tv.tv_usec = SECOND_USEC / sysconf(_SC_CLK_TCK);
        select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &tv);
    }
    return 0;
#else
    return sal_mutex_take((sal_mutex_t)lock, sal_mutex_FOREVER);
#endif
}

/*
 * Function:
 *  sal_spinlock_unlock
 * Purpose:
 *  Releases a spinlock
 * Parameters:
 *  lock - spinlock to release
 * Returns:
 *  0 on success, error code on failure
 */

int
sal_spinlock_unlock(sal_spinlock_t lock)
{
#if USE_POSIX_SPINLOCK
    spinlock_ctrl_t sl = (spinlock_ctrl_t)lock;

    assert(sl);
    return pthread_spin_unlock(&sl->spinlock);
#else
    return sal_mutex_give((sal_mutex_t)lock);
#endif
}

