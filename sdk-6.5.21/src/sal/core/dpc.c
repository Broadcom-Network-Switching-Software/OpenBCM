/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	dpc.c
 * Purpose: 	Deferred Procedure Call module
 *
 * sal_dpc_init() starts a thread to process deferred procedure calls.
 * sal_dpc_term() stops the thread.
 *
 * sal_dpc() enqueues a function that will be called by the thread ASAP.
 * sal_dpc_time() enqueues a function that will be called by the
 *		thread a specified amount of time in the future.
 *
 * DPC allows owner range in full uint32 range but keeps owner status for
 * owner between 0 and SAL_DPC_MAX_NUM_UNITS, which is ususally the unit ID
 * in a multi-chip environment. DPC will check the status of owners in this
 * range while executing DPC from them. The following functions are supported
 * for the owners in SAL_DPC_MAX_NUM_UNITS range.
 *
 * sal_dpc_enable() enable a specified owner to execute DPCs
 * sal_dpc_disable() submit a dpc to disable a specified owner to execute DPCs
 * sal_dpc_disable_and_wait() disable a specified owner to execute DPCs and
 *      wait until all reamining dpc's are executed for this owner
 *
 * When DPC user pass NULL for parameter owner, DPC will assign the owner
 * to be SAL_DPC_OWNER_NULL so we can avoid unexpected owner value
 */

#include <assert.h>

#include <sal/core/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>
#include <sal/core/dpc.h>

#define SAL_DPC_COUNT           256
#define SAL_DPC_THREAD_PRIO     50
#define SAL_DPC_MAX_NUM_UNITS   128 /* align with BCM_MAX_NUM_UNITS */

/* DPC owner status flag define */
#define SAL_DPC_STATUS_ACTIVE       (0x01)  /* Bit 0 */

/* DPC internal macros */
#define SAL_DPC_OWNER_IS_DEV_ID(x)  \
    (PTR_TO_INT(x) < SAL_DPC_MAX_NUM_UNITS)
#define SAL_DPC_OWNER_IS_ACTIVE(x)  \
    (sal_dpc_owner_stat[PTR_TO_INT(x)] & SAL_DPC_STATUS_ACTIVE)

typedef struct sal_dpc_s {
    struct sal_dpc_s	*sd_next;	/* Forward pointer */
    sal_usecs_t		sd_t;		/* Absolute time in usec */
    void		(*sd_f)(void *, void *, void *, void *, void *);
    void		*sd_owner;	/* First parameter passed to sd_f */
    void		*sd_p2;		/* Four more parameters passed */
    void		*sd_p3;
    void		*sd_p4;
    void		*sd_p5;
} sal_dpc_t;

static int		sal_dpc_count	= SAL_DPC_COUNT;
static int		sal_dpc_prio	= SAL_DPC_THREAD_PRIO;
static sal_sem_t	sal_dpc_sem	= NULL;	/* Semaphore to sleep on */
static sal_dpc_t	*sal_dpc_free	= NULL;	/* Free callout structs */
static sal_dpc_t	*sal_dpc_alloc	= NULL;	/* Original allocation */
static sal_dpc_t	*sal_dpc_q	= NULL;	/* Pending callouts */
static sal_spinlock_t	sal_dpc_lock = NULL;
static volatile sal_thread_t	sal_dpc_threadid = SAL_THREAD_ERROR;
static int sal_dpc_owner_stat[SAL_DPC_MAX_NUM_UNITS];

/*
 * Function:
 * 	_sal_dpc_cleanup
 * Purpose:
 *	Free allocated DPC resources.
 * Parameters:
 *	None
 * Returns:
 *	Nothing.
 */

STATIC void
_sal_dpc_cleanup(void)
{
    if (sal_dpc_sem != NULL) {
	sal_sem_destroy(sal_dpc_sem);
	sal_dpc_sem = NULL;
    }

    if (sal_dpc_alloc != NULL) {
	sal_free(sal_dpc_alloc);
	sal_dpc_alloc = NULL;
    }

    if (sal_dpc_lock != NULL) {
        sal_spinlock_destroy(sal_dpc_lock);
        sal_dpc_lock = NULL;
    }
}

/*
 * Function:
 * 	sal_dpc_thread
 * Purpose:
 *	Background deferred procedure call thread used as context for DPCs.
 * Parameters:
 *	arg (Ignored)
 * Returns:
 *	Does not return
 */

STATIC void
sal_dpc_thread(void *arg)
{
    sal_dpc_t	*d;

    COMPILER_REFERENCE(arg);

    while (1) {
	sal_usecs_t	cur_time;

	/*
	 * Wait until it is time to do work, based on the top of the
	 * work queue.
	 */

	if (sal_dpc_q == NULL) {
	    (void)sal_sem_take(sal_dpc_sem, sal_sem_FOREVER);
	} else {
	    int t = SAL_USECS_SUB(sal_dpc_q->sd_t, sal_time_usecs());
	    if (t > 0) {
		(void)sal_sem_take(sal_dpc_sem, t);
	    }
	}

	/* Process all DPCs past due */

	cur_time = sal_time_usecs();

	sal_spinlock_lock(sal_dpc_lock);

	while ((d = sal_dpc_q) != NULL &&
	       SAL_USECS_SUB(d->sd_t, cur_time) <= 0) {
	    sal_dpc_q = d->sd_next;
	    sal_spinlock_unlock(sal_dpc_lock);
        if(SAL_DPC_OWNER_IS_DEV_ID(d->sd_owner)) {
            if (SAL_DPC_OWNER_IS_ACTIVE(d->sd_owner)) {
                d->sd_f(d->sd_owner, d->sd_p2, d->sd_p3, d->sd_p4, d->sd_p5);
            }
        } else {
            d->sd_f(d->sd_owner, d->sd_p2, d->sd_p3, d->sd_p4, d->sd_p5);
        }
	    sal_spinlock_lock(sal_dpc_lock);
	    d->sd_next = sal_dpc_free;	/* Free queue entry */
	    sal_dpc_free = d;
	}

	sal_spinlock_unlock(sal_dpc_lock);
    }
}

STATIC void
_sal_dpc_thread_exit(void *owner, void *p2, void *p3, void *p4, void *p5)
{
    COMPILER_REFERENCE(owner);
    COMPILER_REFERENCE(p2);
    COMPILER_REFERENCE(p3);
    COMPILER_REFERENCE(p4);
    COMPILER_REFERENCE(p5);

    sal_dpc_threadid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

/*
 * Function:
 *  sal_dpc_term
 * Purpose:
 *  Terminate DPC support.
 * Parameters:
 *  None
 * Returns:
 *  Nothing
 */
void
sal_dpc_term(void)
{
    if (sal_dpc_threadid != SAL_THREAD_ERROR) {
    sal_dpc(_sal_dpc_thread_exit, INT_TO_PTR(-1), 0, 0, 0, 0);

	sal_usleep(100);	/* tiny sleep first */
	while (sal_dpc_threadid != SAL_THREAD_ERROR) {
	    sal_usleep(10000);
	}
    }

    _sal_dpc_cleanup();

    sal_dpc_q = NULL;
}

/*
 * Function:
 * 	sal_dpc_init
 * Purpose:
 *	Initialize DPC support.
 * Parameters:
 *	None
 * Returns:
 *	0 on success, non-zero on failure
 * Notes:
 *	This must be called before other DPC routines can be used.
 *	It must not be called from interrupt context.
 */

int
sal_dpc_init(void)
{
    int		i;

    if (sal_dpc_threadid != SAL_THREAD_ERROR) {
	sal_dpc_term();
    }

    sal_dpc_sem = sal_sem_create("sal_dpc_sem", TRUE, 0);
    sal_dpc_alloc = sal_alloc(sizeof(sal_dpc_t) * sal_dpc_count, "sal_dpc");
    sal_dpc_lock = sal_spinlock_create("sal_dpc_lock");

    if (sal_dpc_sem == NULL ||
	sal_dpc_alloc == NULL ||
	sal_dpc_lock == NULL) {
        _sal_dpc_cleanup();
	return -1;
    }

    /* Reset all owner status before thread starts */
    for (i = 0; i < SAL_DPC_MAX_NUM_UNITS; i++) {
        sal_dpc_owner_stat[i] = SAL_DPC_STATUS_ACTIVE;
    }

    sal_dpc_threadid =
	sal_thread_create("bcmDPC",
			  SAL_THREAD_STKSZ,
			  sal_dpc_prio,
			  sal_dpc_thread, 0);

    if (sal_dpc_threadid == SAL_THREAD_ERROR) {
        _sal_dpc_cleanup();
	return -1;
    }

    sal_dpc_free = sal_dpc_alloc;

    for (i = 0; i < sal_dpc_count - 1; i++) {
	sal_dpc_free[i].sd_next = &sal_dpc_free[i + 1];
    }

    sal_dpc_free[sal_dpc_count - 1].sd_next = NULL;

    return 0;
}

/*
 * Function:
 *	sal_dpc_config
 * Purpose:
 *	set configuration parameters and reinitialize dpc subsystem
 * Parameters:
 *	count	- number of entries (use default if <= 0)
 *	prio	- task priority (use defaults if <= 0)
 * Returns:
 *	0 on success, non-zero on failure
 */

int
sal_dpc_config(int count, int prio)
{
    if (count <= 0) {
	count = SAL_DPC_COUNT;
    }

    if (prio <= 0) {
	prio = SAL_DPC_THREAD_PRIO;
    }

    if (count == sal_dpc_count &&
	prio == sal_dpc_prio &&
	sal_dpc_threadid != SAL_THREAD_ERROR) {
	return 0;		/* already running with these parameters */
    }

    sal_dpc_count = count;
    sal_dpc_prio = prio;
    sal_dpc_term();

    return sal_dpc_init();
}

/*
 * Function:
 * 	sal_dpc_time
 * Purpose:
 *	Deferred procedure mechanism with timeout.
 * Parameters:
 *	usec - number of microseconds < 2^31 to wait before the function
 *		is called.  May be 0 to cause immediate callout.
 *	f  - function to call when timeout expires.
 *	owner, p2, p3, p4, p5 - parameters passed to callout function.
 * Returns:
 *	0 - Queued.
 *	-1 - failed.
 * Notes:
 *	May be called from interrupt context.
 */

int
sal_dpc_time(sal_usecs_t usec, sal_dpc_fn_t f,
	     void *owner, void *p2, void *p3, void *p4, void *p5)
{
    sal_dpc_t	*nt, *ct, *lt;		/* new/current/last timer */
    sal_usecs_t	now;
    int		need_to_give_sem;

    need_to_give_sem = 0;

    if (sal_dpc_threadid == SAL_THREAD_ERROR) {
        return -1;
    }

    now = sal_time_usecs();

    sal_spinlock_lock(sal_dpc_lock);

    if ((nt = sal_dpc_free) != NULL) {
	sal_dpc_free = nt->sd_next;

	nt->sd_t = SAL_USECS_ADD(now, usec);
	nt->sd_f  = f;
	nt->sd_owner = owner;
	nt->sd_p2 = p2;
	nt->sd_p3 = p3;
	nt->sd_p4 = p4;
	nt->sd_p5 = p5;
	nt->sd_next = NULL;

	/*
	 * Find location in time queue and insert, note that this search and
	 * insert results in multiple entries for the same time being called
	 * in the order in which they were inserted.
	 */
	for (lt = NULL, ct = sal_dpc_q; ct; lt = ct, ct = ct->sd_next) {
	    if (SAL_USECS_SUB(nt->sd_t, ct->sd_t) < 0) {
		break;
	    }
	}

	/*
	 * lt is NULL if insert first entry, or points to entry after which
	 * we should insert.
	 */

	if (lt) {
	    nt->sd_next = lt->sd_next;
	    lt->sd_next = nt;
	} else {
	    nt->sd_next = sal_dpc_q;
	    sal_dpc_q	= nt;
	    need_to_give_sem = 1;
	}
    }

    sal_spinlock_unlock(sal_dpc_lock);

    if(need_to_give_sem) {
	sal_sem_give(sal_dpc_sem);	/* Wake up, old sleep value stale */
    }

    return (nt != NULL ? 0 : -1);
}

/*
 * Function:
 * 	sal_dpc
 * Purpose:
 *	Deferred procedure mechanism with-out timeout.
 * Parameters:
 *	f - function to call when timeout expires.
 *	owner, p2, p3, p4, p5 - parameters passed to callout function.
 * Returns:
 *	0 - Queued.
 *	-1 - failed.
 * Notes:
 *	May be called from interrupt context.
 */

int
sal_dpc(sal_dpc_fn_t f, void *owner, void *p2, void *p3, void *p4, void *p5)
{
    return (sal_dpc_time((sal_usecs_t) 0, f, owner, p2, p3, p4, p5));
}

/*
 * Function:
 *	sal_dpc_cancel
 *
 * Purpose:
 *	Cancel all DPCs belonging to a specified owner.
 *
 * Parameters:
 *	owner - first parameter to DPC calls
 *
 * Notes:
 *	Each DPC currently waiting on the queue whose first
 *	argument (owner) matches the specified value is removed
 *	from the queue and never executed.
 */
void
sal_dpc_cancel(void *owner)
{
    sal_dpc_t	**dp, *d;

    sal_spinlock_lock(sal_dpc_lock);;

    dp = &sal_dpc_q;

    while ((d = *dp) != NULL) {
	if (d->sd_owner == owner) {
	    (*dp) = d->sd_next;
	    d->sd_next = sal_dpc_free;
	    sal_dpc_free = d;
	} else {
	    dp = &(*dp)->sd_next;
	}
    }

    sal_spinlock_unlock(sal_dpc_lock);
}

/*
 * Function:
 *  sal_dpc_enable
 *
 * Purpose:
 *  Enable a specified owner to run DPCs
 *
 * Parameters:
 *  owner - first parameter to DPC calls
 * Returns:
 *	0  - succeed
 *	-1 - failed
 */
int
sal_dpc_enable(void *owner)
{
    if(SAL_DPC_OWNER_IS_DEV_ID(owner)) {
        sal_dpc_owner_stat[PTR_TO_INT(owner)] |= SAL_DPC_STATUS_ACTIVE;
        return 0;
    }
    return -1;
}

void
_sal_dpc_disable(void *owner, void *p2, void *p3, void *p4, void *p5)
{
    if(SAL_DPC_OWNER_IS_DEV_ID(owner)) {
        sal_dpc_owner_stat[PTR_TO_INT(owner)] &= ~SAL_DPC_STATUS_ACTIVE;
    }
}

/*
 * Function:
 *  sal_dpc_cancel_and_disable
 *
 * Purpose:
 *  Cancel all DPCs belonging to a specified owner, and then
 * disable a specified owner to execute DPCs
 *
 * Parameters:
 *  owner - first parameter to DPC calls
 *
 * Notes:
 *  Each DPC currently waiting on the queue whose first
 *  argument (owner) matches the specified value is removed
 *  from the queue and never executed.
 */
void
sal_dpc_cancel_and_disable(void *owner)
{
    sal_dpc_t   **dp, *d;

    sal_spinlock_lock(sal_dpc_lock);

    dp = &sal_dpc_q;

    while ((d = *dp) != NULL) {
        if (d->sd_owner == owner) {
            (*dp) = d->sd_next;
            d->sd_next = sal_dpc_free;
            sal_dpc_free = d;
        } else {
            dp = &(*dp)->sd_next;
        }
    }
    _sal_dpc_disable(owner, 0, 0, 0, 0);

    sal_spinlock_unlock(sal_dpc_lock);
}

/*
 * Function:
 *  sal_dpc_disable
 *
 * Purpose:
 *  Disable a specified owner to execute DPCs
 *
 * Parameters:
 *  owner - first parameter to DPC calls
 * Returns:
 *	0  - succeed
 *	-1 - failed
 */
int
sal_dpc_disable(void *owner)
{
    if(SAL_DPC_OWNER_IS_DEV_ID(owner)) {
        return (sal_dpc(_sal_dpc_disable, owner, 0, 0, 0, 0));
    }
    return -1;
}

/*
 * Function:
 *  sal_dpc_disable_and_cancel
 *
 * Purpose:
 * disable a specified owner to execute DPCs,
 * and wait all DPCs belonging to owner to be done.
 *
 * Parameters:
 *  unit
 * Returns:
 *	0  - succeed
 *	-1 - failed
 */
int
sal_dpc_disable_and_wait(void *owner)
{
    if(sal_dpc_disable(owner) == 0) {
        while (SAL_DPC_OWNER_IS_ACTIVE(owner)) {
            sal_usleep(100);
        }
        return 0;
    }
    return -1;
}

