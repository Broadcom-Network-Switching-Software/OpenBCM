/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	thread.c
 * Purpose:	Defines SAL routines for Unix threads
 *
 * Thread Abstraction
 *
 * POSIX does not keep thread names.  The keep the names, we have a
 * linked list of all threads we create.  If your OS has the ability to
 * retrieve the thread name, most of this code can be deleted.  If you
 * don't care about thread names, you may just have sal_thread_name
 * always return the empty string.
 *
 * The most important use for thread names is in sal/appl/xxx/console.c,
 * where the console output of background tasks can be prefixed by the
 * task name.
 */

#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>
#include <sal/core/spl.h>
#include <sal/limits.h>

#if defined (__STRICT_ANSI__)
#define NO_CONTROL_C
#endif

#ifndef SAL_THREAD_RT_PRIO_HIGHEST
#define SAL_THREAD_RT_PRIO_HIGHEST  90
#endif

static pthread_mutex_t _sal_thread_lock = PTHREAD_MUTEX_INITIALIZER;

#define THREAD_LOCK() pthread_mutex_lock(&_sal_thread_lock)
#define THREAD_UNLOCK() pthread_mutex_unlock(&_sal_thread_lock)

#if defined(BROADCOM_DEBUG) && defined(INCLUDE_BCM_SAL_PROFILE)
static unsigned int _sal_thread_count_curr;
static unsigned int _sal_thread_count_max;
static unsigned int _sal_thread_stack_size_curr;
static unsigned int _sal_thread_stack_size_max;
#define SAL_THREAD_RESOURCE_USAGE_INCR(a_cnt, a_cnt_max, a_sz,      \
                                       a_sz_max, n_ssize, ilock)    \
    a_cnt++;                                                        \
    a_sz += (n_ssize);                                              \
    a_cnt_max = ((a_cnt) > (a_cnt_max)) ? (a_cnt) : (a_cnt_max);    \
    a_sz_max = ((a_sz) > (a_sz_max)) ? (a_sz) : (a_sz_max)

#define SAL_THREAD_RESOURCE_USAGE_DECR(a_count, a_ssize, n_ssize, ilock)\
        a_count--;                                                      \
        a_ssize -= (n_ssize)

/*
 * Function:
 *      sal_thread_resource_usage_get
 * Purpose:
 *      Provides count of active threads and stack allocation
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max - Memory usage high water mark
 */

void
sal_thread_resource_usage_get(unsigned int *sal_thread_count_curr,
                              unsigned int *sal_stack_size_curr,
                              unsigned int *sal_thread_count_max,
                              unsigned int *sal_stack_size_max)
{
    if (sal_thread_count_curr != NULL) {
        *sal_thread_count_curr = _sal_thread_count_curr;
    }
    if (sal_stack_size_curr != NULL) {
        *sal_stack_size_curr = _sal_thread_stack_size_curr;
    }
    if (sal_thread_count_max != NULL) {
        *sal_thread_count_max = _sal_thread_count_max;
    }
    if (sal_stack_size_max != NULL) {
        *sal_stack_size_max = _sal_thread_stack_size_max;
    }
}

#else
/* Resource tracking disabled */
#define SAL_THREAD_RESOURCE_USAGE_INCR(a_cnt, a_cnt_max, a_sz,      \
                                       a_sz_max, n_ssize, ilock)
#define SAL_THREAD_RESOURCE_USAGE_DECR(a_count, a_ssize, n_ssize, ilock)
#endif

/* If user defined min stack size in Make.local
 * then undefine local stack min
 * and set user defined value as the new min
 */
#ifdef SAL_THREAD_STACK_MIN
#ifdef PTHREAD_STACK_MIN
#undef PTHREAD_STACK_MIN
#endif
#define PTHREAD_STACK_MIN SAL_THREAD_STACK_MIN
#else
/* If no user defined min stack size
 * and if no local stack min
 * define local stack min
 */
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN 16384
#endif
#endif

/*
 * Function:
 *	thread_boot
 * Purpose:
 *	Entry point for each new thread created
 * Parameters:
 *	ti - information about thread being created
 * Notes:
 *	Signals and other parameters are configured before jumping to
 *	the actual thread's main routine.
 */

typedef struct thread_info_s {
    void		(*f)(void *);
    char		*name;
    pthread_t		id;
    void		*arg;
    int                 tid;
    int                 ss;
    sal_sem_t           sem;
    struct thread_info_s *next;
} thread_info_t;

static thread_info_t	*thread_head = NULL;

static void *
thread_boot(void *ti_void)
{

    thread_info_t	*ti = ti_void;
    void		(*f)(void *);
    void		*arg;
#ifndef NO_CONTROL_C
    sigset_t		new_mask, orig_mask;

    /* Make sure no child thread catches Control-C */
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &orig_mask);
#endif

#if defined(BCM_DNX_SUPPORT) ||  defined(BCM_DNXF_SUPPORT)
	/* init seq threads must be joinable */
    if ((strcmp(ti->name, "utilex step") && strcmp(ti->name, "mem_init_threads")))
#endif
    {
        /* Ensure that we give up all resources upon exit */
        pthread_detach(pthread_self());
    }

#ifdef PR_SET_NAME
    prctl(PR_SET_NAME, ti->name, 0, 0, 0);
#endif

#ifndef netbsd
    /* not supported */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif /* netbsd */

    f = ti->f;
    arg = ti->arg;

    ti->id = pthread_self();
    ti->tid = syscall(SYS_gettid);

    /* Notify parent to continue */
    sal_sem_give(ti->sem);

    /* Call thread function */
    (*f)(arg);

    /* Thread function did not call sal_thread_exit() */
    sal_thread_exit(0);

    /* Will never get here */
    return NULL;
}


/*
 * Function:
 *  sal_thread_join
 * Purpose:
 *  The function waits for the thread specified by thread
 *  to terminate.  If that thread has already terminated, then
 *  the function returns immediately. The thread specified by thread
 *  must be joinable.
 * Parameters:
 *  thread - Thread to wait for
 *  retval - If retval is not NULL, then sal_thread_join() copies the exit status of
 *           the target thread (i.e., the value that the target thread supplied to
 *           the thread exit() into the location pointed to by retval.
 * Returns:
 *  On success, pthread_join() returns 0; on error, it returns an error number.
 */

int
sal_thread_join(sal_thread_t thread, void **retval)
{
    return pthread_join((pthread_t) thread, retval);
}

/*
 * Function:
 *	sal_thread_create
 * Purpose:
 *	Abstraction for task creation
 * Parameters:
 *	name - name of task
 *	ss - stack size requested
 *	prio - scheduling prio (0 = highest, 255 = lowest)
 *	func - address of function to call
 *	arg - argument passed to func.
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_create(char *name, int ss, int prio, void (f)(void *), void *arg)
{
    pthread_attr_t	attribs;
    struct sched_param param;
    thread_info_t	*ti;
    pthread_t		id;
    sal_sem_t           sem;

    if (pthread_attr_init(&attribs)) {
        return(SAL_THREAD_ERROR);
    }

    ss += PTHREAD_STACK_MIN;
    pthread_attr_setstacksize(&attribs, ss);

    if (prio == SAL_THREAD_PRIO_NO_PREEMPT) {
        pthread_attr_setinheritsched(&attribs, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attribs, SCHED_FIFO);
        param.sched_priority = SAL_THREAD_RT_PRIO_HIGHEST;
        pthread_attr_setschedparam(&attribs, &param);
    }

    if ((ti = malloc(sizeof (*ti))) == NULL) {
	return SAL_THREAD_ERROR;
    }

    if ((sem = sal_sem_create("threadBoot", 1, 0)) == NULL) {
	free(ti);
	return SAL_THREAD_ERROR;
    }
    ti->name = NULL;
    if ((ti->name = malloc(strlen(name)+1)) == NULL) {
        free(ti);
        sal_sem_destroy(sem);
        return SAL_THREAD_ERROR;
    }
    /* coverity[secure_coding] */
    strcpy(ti->name, name);

    ti->f = f;
    
    ti->arg = arg;
    ti->id = (pthread_t)0;
    ti->ss = ss;
    ti->sem = sem;

    THREAD_LOCK();
    ti->next = thread_head;
    thread_head = ti;
    THREAD_UNLOCK();

    if (pthread_create(&id, &attribs, thread_boot, (void *)ti)) {
        THREAD_LOCK();
	thread_head = thread_head->next;
        THREAD_UNLOCK();
        if (ti->name != NULL) {
            free(ti->name);
        }
	free(ti);
        sal_sem_destroy(sem);
	return(SAL_THREAD_ERROR);
    }

    SAL_THREAD_RESOURCE_USAGE_INCR(
        _sal_thread_count_curr,
        _sal_thread_count_max,
        _sal_thread_stack_size_curr,
        _sal_thread_stack_size_max,
        ss,
        ilock);

    /*
     * Note that at this point ti can no longer be safely
     * dereferenced, as the thread we just created may have
     * exited already. Instead we wait for the new thread
     * to update thread_info_t and tell us to continue.
     */
    sal_sem_take(sem, sal_sem_FOREVER);
    sal_sem_destroy(sem);

    return ((sal_thread_t)id);
}

/*
 * Function:
 *	sal_thread_destroy
 * Purpose:
 *	Abstraction for task deletion
 * Parameters:
 *	thread - thread ID
 * Returns:
 *	0 on success, -1 on failure
 * Notes:
 *	This routine is not generally used by Broadcom drivers because
 *	it's unsafe.  If a task is destroyed while holding a mutex or
 *	other resource, system operation becomes unpredictable.  Also,
 *	some RTOS's do not include kill routines.
 *
 *	Instead, Broadcom tasks are written so they can be notified via
 *	semaphore when it is time to exit, at which time they call
 *	sal_thread_exit().
 */

int
sal_thread_destroy(sal_thread_t thread)
{
#ifdef netbsd
    /* not supported */
    return -1;
#else
    thread_info_t	*ti, **tp;
    pthread_t		id = (pthread_t) thread;

    if (pthread_cancel(id)) {
	return -1;
    }

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
	if ((*tp)->id == id) {
	    ti = (*tp);
	    (*tp) = (*tp)->next;
	    break;
	}
    }
    THREAD_UNLOCK();

    if (ti) {
        SAL_THREAD_RESOURCE_USAGE_DECR(
            _sal_thread_count_curr,
            _sal_thread_stack_size_curr,
            ti->ss,
            ilock);
        if (ti->name != NULL) {
            free(ti->name);
        }
        free(ti);
    }

    return 0;
#endif
}

/*
 * Function:
 *	sal_thread_self
 * Purpose:
 *	Return thread ID of caller
 * Parameters:
 *	None
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_self(void)
{
    return (sal_thread_t) pthread_self();
}

int
sal_thread_id_get(void)
{
    return syscall(SYS_gettid);
}

/*
 * Function:
 *	sal_thread_name
 * Purpose:
 *	Return name given to thread when it was created
 * Parameters:
 *	thread - thread ID
 *	thread_name - buffer to return thread name;
 *		gets empty string if not available
 *	thread_name_size - maximum size of buffer
 * Returns:
 *	NULL, if name not available
 *	thread_name, if name available
 */
char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
    thread_info_t	*ti;
    char                *name;

    name = NULL;

    THREAD_LOCK();
    for (ti = thread_head; ti != NULL; ti = ti->next) {
	if (ti->id == (pthread_t)thread) {
	    strncpy(thread_name, ti->name, thread_name_size);
	    thread_name[thread_name_size - 1] = 0;
	    name = thread_name;
            break;
	}
    }
    THREAD_UNLOCK();

    if (name == NULL) {
        thread_name[0] = 0;
    }

    return name;
}

/*
 * Function:
 *	sal_thread_exit
 * Purpose:
 *	Exit the calling thread
 * Parameters:
 *	rc - return code from thread.
 * Notes:
 *	Never returns.
 */

void
sal_thread_exit(int rc)
{
    thread_info_t	*ti, **tp;
    pthread_t		id = pthread_self();

    ti = NULL;

    THREAD_LOCK();
    for (tp = &thread_head; (*tp) != NULL; tp = &(*tp)->next) {
	if ((*tp)->id == id) {
	    ti = (*tp);
	    (*tp) = (*tp)->next;
	    break;
	}
    }
    THREAD_UNLOCK();

    if (ti) {
        SAL_THREAD_RESOURCE_USAGE_DECR(
            _sal_thread_count_curr,
            _sal_thread_stack_size_curr,
            ti->ss,
            ilock);
        if (ti->name != NULL) {
            free(ti->name);
        }
        free(ti);
    }

    pthread_exit(INT_TO_PTR(rc));
}

/*
 * Function:
 *	sal_thread_yield
 * Purpose:
 *	Yield the processor to other tasks.
 * Parameters:
 *	None
 */

void
sal_thread_yield(void)
{
    sal_usleep(1);
}

/*
 * Function:
 *	sal_thread_main_set
 * Purpose:
 *	Set which thread is the main thread
 * Parameters:
 *	thread - thread ID
 * Notes:
 *	The main thread is the one that runs in the foreground on the
 *	console.  It prints normally, takes keyboard signals, etc.
 */

static sal_thread_t _sal_thread_main = 0;

void
sal_thread_main_set(sal_thread_t thread)
{
    _sal_thread_main = thread;
}

/*
 * Function:
 *	sal_thread_main_get
 * Purpose:
 *	Return which thread is the main thread
 * Returns:
 *	Thread ID
 * Notes:
 *	See sal_thread_main_set().
 */

sal_thread_t
sal_thread_main_get(void)
{
    return _sal_thread_main;
}

/*
 * Function:
 *      sal_thread_traverse
 * Purpose:
 *      Go through thread list and do callback
 * Returns:
 *      None
 * Notes:
 *      None
 */

void
sal_thread_traverse(thread_traverse_cb_f cb_func, void *cb_date)
{
    thread_info_t *ti;

    for (ti = thread_head; ti != NULL; ti = ti->next) {
        if (cb_func) {
            cb_func(cb_date, ti->tid, ti->name);
        }
    }
}

/*
 * Function:
 *	sal_sleep
 * Purpose:
 *	Suspend calling thread for a specified number of seconds.
 * Parameters:
 *	sec - number of seconds to suspend
 * Notes:
 *	Other tasks are free to run while the caller is suspended.
 */

void
sal_sleep(int sec)
{
    struct timeval tv;
    tv.tv_sec = (time_t) sec;
    tv.tv_usec = 0;
    select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
}

/*
 * Function:
 *	sal_usleep
 * Purpose:
 *	Suspend calling thread for a specified number of microseconds.
 * Parameters:
 *	usec - number of microseconds to suspend
 * Notes:
 *	The actual delay period depends on the resolution of the
 *	Unix select routine, whose precision is limited to the
 *	the period of the scheduler tick, generally 1/60 or 1/100 sec.
 *	Other tasks are free to run while the caller is suspended.
 *
 *	A short usleep less than 20000 usec is done by doing a 'yield'
 *	within a loop, rather than a true 'sleep'. If no other tasks are
 *	using the CPU, this will effectively be a busy-wait, as the yield
 *	will immediately return. This can make it appear that the calling
 *	thread is using a lot of CPU, though this is only because no other
 *	task is in a running state. To avoid this appearance of 100% CPU
 *	load, the calling thread can increase the delay to 20000.
 *	The appearance of 100% CPU load for this thread should not have any
 *	effects on other threads. The thread that is spinning doing a
 *	sal_usleep() is continually calling an OS yield, so any other scheduled
 *	threads will run. The only time you will see this thread getting 100%
 *	CPU is if no other tasks are trying to run.
 */

void
sal_usleep(uint32 usec)
{
    struct timeval tv;

    if (usec < (2 * SECOND_USEC)/HZ) {
        sal_usecs_t now;
        sal_usecs_t earlier;
        sal_usecs_t delta;
        sal_usecs_t max_time = SAL_UINT32_MAX;

        earlier = sal_time_usecs();
        do {
#if defined(_POSIX_PRIORITY_SCHEDULING) && (_POSIX_PRIORITY_SCHEDULING >= 200112L)
            sched_yield();
#else
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
#endif
            now = sal_time_usecs();
            if (now < earlier) {
                delta = max_time - earlier + now;
            } else {
                delta = now - earlier;
            }
        } while (delta < usec);
    }
    else {
        tv.tv_sec = (time_t) (usec / SECOND_USEC);
        tv.tv_usec = (long) (usec % SECOND_USEC);
        select(0, (fd_set *) 0, (fd_set *) 0, (fd_set *) 0, &tv);
    }
}

#define UDELAY_USE_CLOCK_GETTIME (-1)
/*
 * Function:
 *	sal_udelay
 * Purpose:
 *	Spin wait for an approximate number of microseconds
 * Parameters:
 *	usec - number of microseconds
 * Notes:
 *	MUST be called once before normal use so it can self-calibrate.
 *	Code for self-calibrating delay loop is courtesy of
 *	Geoffrey Espin, the comp.os.vxworks Usenet group, and
 *	JA Borkhuis (http://www.xs4all.nl/~borkhuis).
 *      The current implementation assumes that sal_time_usecs has
 *      system tick resolution (or better).
 */

void
sal_udelay(uint32 usec)
{
    static volatile int _sal_udelay_counter;
    static int loops = 0;
    uint32 iy;
    int ix; 
#ifdef CLOCK_MONOTONIC
    int error;
    struct timespec now, earlier;
    long delta_usec, diff_nsec;

    if (loops == 0) {
        error = clock_getres(CLOCK_MONOTONIC, &now);
        if (!error && !now.tv_sec && (now.tv_nsec <= 1000)) {
            loops = UDELAY_USE_CLOCK_GETTIME;
        }
    }

    if (loops == UDELAY_USE_CLOCK_GETTIME) {
        error = clock_gettime(CLOCK_MONOTONIC, &earlier);
        assert(!error);
        do {
            error = clock_gettime(CLOCK_MONOTONIC, &now);
            assert(!error);
            diff_nsec = now.tv_nsec - earlier.tv_nsec;
            if (diff_nsec < 0) {
                diff_nsec += 1000000000;
                now.tv_sec--;
            }
            delta_usec = (now.tv_sec - earlier.tv_sec) * 1000000 +
                         (diff_nsec / 1000);
        } while ((uint32)delta_usec < usec);
        return;
    }
#endif
    if (loops == 0 || usec == 0) {      /* Need calibration? */
        int max_loops;
        int start = 0, stop = 0;
        int mpt = (SECOND_USEC / HZ);   /* usec/tick */

        for (loops = 1; loops < 0x1000 && stop == start; loops <<= 1) {
            /* Wait for clock turn over */
            for (stop = start = sal_time_usecs() / mpt;
                 start == stop;
                 start = sal_time_usecs() / mpt) {
                /* Empty */
            }
            sal_udelay(mpt);    /* Single recursion */
            stop = sal_time_usecs() / mpt;
        }

        max_loops = loops / 2;  /* Loop above overshoots */

        start = stop = 0;

        if (loops < 4) {
            loops = 4;
        }

        for (loops /= 4; loops < max_loops && stop == start; loops++) {
            /* Wait for clock turn over */
            for (stop = start = sal_time_usecs() / mpt;
                 start == stop;
                 start = sal_time_usecs() / mpt) {
                /* Empty */
            }
            sal_udelay(mpt);    /* Single recursion */
            stop = sal_time_usecs() / mpt;
        }
    }
   
    for (iy = 0; iy < usec; iy++) {
        for (ix = 0; ix < loops; ix++) {
            _sal_udelay_counter++;      /* Prevent optimizations */
        }
    }
}

#define MAX_TLS_KEY_SUPPORTED 8

typedef struct {
    pthread_key_t key;
    int mapped;
} pthread_key_map_t;

static pthread_key_map_t key_maps[MAX_TLS_KEY_SUPPORTED];
static int sal_tls_inited = FALSE;

static void 
sal_tls_init(void)
{  
    int i;

    if (sal_tls_inited) {
        return;    
    }
    for (i = 0; i < MAX_TLS_KEY_SUPPORTED; i++) {
        key_maps[i].mapped = FALSE;
    }
    sal_tls_inited = TRUE;
    return;
}

sal_tls_key_t * 
sal_tls_key_create(void (*destructor)(void *))
{
    int i;
    int lvl;
    
    lvl = sal_splhi();
    sal_tls_init();    
    for (i = 0; i < MAX_TLS_KEY_SUPPORTED; i++) {
        if (!key_maps[i].mapped) { 
        	key_maps[i].mapped = TRUE;         
            break;
        }
    }
    sal_spl(lvl);
     
    if (i >= MAX_TLS_KEY_SUPPORTED) {
        return NULL;
    }
    if (0 == pthread_key_create(&key_maps[i].key, destructor)) {
        return (void *)&key_maps[i].key;
    } else {
    	key_maps[i].mapped = FALSE;
        return NULL;    
    }
}

int 
sal_tls_key_set(sal_tls_key_t *key, void *val)
{
    if (key == NULL) {
        return FALSE;   
    }
    
    if (!sal_tls_inited) {
        return FALSE;   
    }
    
    if (0 == pthread_setspecific(*(pthread_key_t *)key, val)) {
        return TRUE;
    } else {
        return FALSE;   
    }
        
}

void *
sal_tls_key_get(sal_tls_key_t *key)
{   
    if (key == 0) {
        return NULL;
    }
    if (!sal_tls_inited) {
        return NULL;    
    }
    return pthread_getspecific(*(pthread_key_t *)key); 
}

int 
sal_tls_key_delete(sal_tls_key_t *key)
{
    if (key == 0) {
        return FALSE;
    }
    if (!sal_tls_inited) {
        return FALSE;   
    }
    if (0 == pthread_key_delete(*(pthread_key_t *)key)) {
        return TRUE;
    } else {
        return FALSE;   
    }
}

