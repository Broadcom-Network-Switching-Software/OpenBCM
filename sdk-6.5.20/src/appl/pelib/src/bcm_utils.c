/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Driver utility routines
 */
#include "bcm_utils.h"


/*
 * soc_timeout
 *
 *   These routines implement a polling timer that, in the normal case,
 *   has low overhead, but provides reasonably accurate timeouts for
 *   intervals longer than a millisecond.
 *
 *   min_polls should be chosen so the operation is expected to complete
 *   within min_polls, if possible.  If the operation completes within
 *   min_polls, there is very little overhead.  Otherwise, the routine
 *   starts making O/S calls to check the real time clock and uses an
 *   exponential timeout to avoid hogging the CPU.
 *
 *   Example usage:
 *
 *	soc_timeout_t	to;
 *	sal_usecs_t		timeout_usec = 100000;
 *	int				min_polls = 100;
 *
 *	soc_timeout_init(&to, timeout_usec, min_polls);
 *
 *	while (check_status(thing) != DONE)
 *		if (soc_timeout_check(&to)) {
 *              if (check_status(thing) == DONE) {
 *                  break;
 *              }
 *			BCM_LOG_DEBUG("Operation timed out\n");
 *			return ERROR;
 *		}
 *
 *   Note that even after timeout the status should be checked
 *   one more time.  Otherwise there is a race condition where an
 *   ill-placed O/S task reschedule could cause a false timeout.
 */

#ifdef PEMODE_BCM_BUILD
extern sal_usecs_t sal_time_usecs(void);
extern void sal_usleep(unsigned int usec);
extern void sal_udelay(unsigned int usec);
extern int soc_miimc45_write(int unit, uint16 phy_id, uint8 phy_devad,
           uint16 phy_reg_addr, uint16 phy_wr_data);
extern int soc_miimc45_read(int unit, uint16 phy_id, uint8 phy_devad,
          uint16 phy_reg_addr, uint16 *phy_rd_data);
extern void *sal_alloc(unsigned int, char *);
extern void sal_free(void *);
extern sal_sem_t sal_sem_create(char *desc, int binary, int initial_count);
extern void sal_sem_destroy(sal_sem_t b);
extern int sal_sem_take(sal_sem_t b, int usec);
extern int sal_sem_give(sal_sem_t b);
#endif /* PEMODE_BCM_BUILD */

phy_ctrl_t phyctrl[NUM_MODULES];

/* _bcm_axi_{read|write}() need to be optimized when the data only transactions are
   implemented in the CMIC MDIO code.
 */

/*
 * bcm_timeout_init():
 *	Initiaize timer to timeout in x usec.
 */
void
bcm_timeout_init(soc_timeout_t *to, sal_usecs_t usec, int min_polls)
{
#ifdef PEMODE_BCM_BUILD
    to->min_polls = min_polls;
    to->usec = usec;
    to->polls = 1;
    to->exp_delay = 1;   /* In case caller sets min_polls < 0 */
#else
    #warning "bcm_timeout_init() not found. Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

/*
 * bcm_timeout_check():
 *	Return 1 when timeout has elapsed.
 */
int
bcm_timeout_check(soc_timeout_t *to)
{
#ifdef PEMODE_BCM_BUILD
    if (++to->polls >= to->min_polls) {
	    if (to->min_polls >= 0) {
	        /*
    	     * Just exceeded min_polls; calculate expiration time by
	         * consulting O/S real time clock.
	         */

    	    to->min_polls = -1;
	        to->expire = SAL_USECS_ADD(sal_time_usecs(), to->usec);
	        to->exp_delay = 1;
    	} else {
	        /*
	         * Exceeded min_polls in a previous call.
    	     * Consult O/S real time clock to check for expiration.
	         */

    	    if (SAL_USECS_SUB(sal_time_usecs(), to->expire) >= 0) {
	    	    return 1;
	        }

    	    sal_usleep(to->exp_delay);

	        /* Exponential backoff with 10% maximum latency */

	        if ((to->exp_delay *= 2) > to->usec / 10) {
    		to->exp_delay = to->usec / 10;
	        }
    	}
    }

    return 0;
#else
    #warning "bcm_timeout_check() not found. Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

/*
 * bcm_udelay(int usec):
 *	Delay x useconds.
 */
void
bcm_udelay(int usec)
{
#ifdef PEMODE_BCM_BUILD
    sal_udelay(usec);
#else
    #warning "bcm_udelay() not found. Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

/*
 * bcm_mdio_read():
 * The mdio read must generate clause 45 transactions with DEVAD=1.
 */
int
bcm_mdio_read(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 *val)
{
    int rc;
#ifdef PEMODE_BCM_BUILD
    rc = soc_miimc45_read(unit, pc->phy_id, pc->phy_devad, phy_reg_addr, val);
  #if 0
    BCM_LOG_DEBUG("%s() unit %d phy_id 0x%x phy_devad 0x%x reg_addr 0x%x val 0x%x\n",
            __FUNCTION__, unit, pc->phy_id, pc->phy_devad, phy_reg_addr, *val);
  #endif
    return rc;
#else
    #warning "bcm_mdio_read() not found.  Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

/*
 * bcm_mdio_write():
 * The mdio write must generate clause 45 transactions with DEVAD=1.
 */
int
bcm_mdio_write(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 val)
{
#ifdef PEMODE_BCM_BUILD
  #if 0
    BCM_LOG_DEBUG("%s() unit %d phy_id 0x%x phy_devad 0x%x reg_addr 0x%x val 0x%x\n",
            __FUNCTION__, unit, pc->phy_id, pc->phy_devad, phy_reg_addr, val);
  #endif
    return soc_miimc45_write(unit, pc->phy_id, pc->phy_devad, phy_reg_addr, val);
#else
    #warning "bcm_mdio_write() not found.  Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

int
bcm_mdio_modify(int unit, phy_ctrl_t *pc, uint16 phy_reg_addr, uint16 val, uint16 mask)
{
    uint16  tmp, otmp;
    int rc;

#if 0
    BCM_LOG_DEBUG("%s() unit %d phy_id 0x%x phy_devad 0x%x reg_addr 0x%x val 0x%x mask 0x%x\n",
            __FUNCTION__, unit, pc->phy_id, pc->phy_devad, phy_reg_addr, val, mask);
#endif

    val = val & mask;

    rc = bcm_mdio_read(unit, pc, phy_reg_addr, &tmp);
    if (rc) {
        BCM_LOG_ERR("%s() error (%d) reading reg_addr 0x%x\n", __FUNCTION__, rc, phy_reg_addr);
    }
#if 0
    BCM_LOG_DEBUG("%s() read unit %d phy_id 0x%x phy_devad 0x%x reg_addr 0x%x val 0x%x\n",
            __FUNCTION__, unit, pc->phy_id, pc->phy_devad, phy_reg_addr, tmp);
#endif
    
    otmp = tmp;
    tmp &= ~(mask);
    tmp |= val;

    if (otmp != tmp) {
        rc = bcm_mdio_write(unit, pc, phy_reg_addr, tmp);
        if (rc) {
            BCM_LOG_ERR("%s() error (%d) writing (0x%x) to reg_addr 0x%x\n", __FUNCTION__, rc, tmp, phy_reg_addr);
        }
#if 0
        BCM_LOG_DEBUG("%s() write unit %d phy_id 0x%x phy_devad 0x%x reg_addr 0x%x val 0x%x\n",
                __FUNCTION__, unit, pc->phy_id, pc->phy_devad, phy_reg_addr, tmp);
#endif
    }

    return rc;
}


void *
bcm_memcpy(void *dst_void, const void *src_void, uint32 len)
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;

    while (len--) {
        *dst++ = *src++;
    }

    return dst_void;
}

void *
bcm_memset(void *dst_void, int val, size_t len)
{
    unsigned char *dst = dst_void;

    while (len--) {
        *dst++ = (unsigned char) val;
    }

    return dst_void;
}

sal_mutex_t
bcm_mutex_create(char *desc)
{
#ifdef PEMODE_BCM_BUILD
    bcm_mutex_t   *rm;

    if ((rm = sal_alloc(sizeof (bcm_mutex_t), desc)) == NULL) {
        BCM_LOG_ERR("%s() Error: failed to alloc memory\n", __FUNCTION__);
        return NULL;
    }

    rm->sem = sal_sem_create(desc, 1, 1);
    rm->desc = desc;
    return (sal_mutex_t) rm;
#else
    #warning "bcm_mutex_create() not found.  Customer platform specific."
    printf("Customer logic here\n");
    return NULL;
#endif /* PEMODE_BCM_BUILD */
}

void
bcm_mutex_destroy(sal_mutex_t m)
{
#ifdef PEMODE_BCM_BUILD
    bcm_mutex_t   *rm = (bcm_mutex_t *) m;

    sal_sem_destroy(rm->sem);

    sal_free(rm);
#else
    #warning "bcm_mutex_destroy() not found.  Customer platform specific."
    printf("Customer logic here\n");
#endif /* PEMODE_BCM_BUILD */
}

int
bcm_mutex_take(sal_mutex_t m, int usec)
{
#ifdef PEMODE_BCM_BUILD
    bcm_mutex_t   *rm = (bcm_mutex_t *) m;
    int           err;

    err = sal_sem_take(rm->sem, usec);

    return err ? -1 : 0;

#else
    #warning "bcm_mutex_take() not found.  Customer platform specific."
    printf("Customer logic here\n");
    return 0;
#endif /* PEMODE_BCM_BUILD */
}

int
bcm_mutex_give(sal_mutex_t m)
{
#ifdef PEMODE_BCM_BUILD
    bcm_mutex_t   *rm = (bcm_mutex_t *) m;
    int           err;

    err = sal_sem_give(rm->sem);

    return err ? -1 : 0;
#else
    #warning "bcm_mutex_give() not found.  Customer platform specific."
    printf("Customer logic here\n");
    return 0;
#endif /* PEMODE_BCM_BUILD */
}

