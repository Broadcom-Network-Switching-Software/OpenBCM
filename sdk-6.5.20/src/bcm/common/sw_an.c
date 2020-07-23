/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * SW AN thread is an optional feature to support 
 * consortium AN mode on devices using PM4x25 D0. THis module is responsible for 
 * driving the Page exchange An state machine as recomennded by IEEE802.3by/MSA spec
 * Once the page exchange is completed and speed is resolved HW will take over and complete
 * the Autoneg.  
 */

#include <shared/bsl.h>
#ifdef SW_AUTONEG_SUPPORT 

#include <sal/types.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/common/sw_an.h>

#include <soc/phy/phymod_ctrl.h>
#include <soc/phy/phyctrl.h>

#include <sal/appl/io.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/esw/portctrl.h>
#include <bcm_int/esw/portctrl.h>

#define PORT_MAX_PHY_ACCESS_STRUCTURES (6)
#endif

/*
 * SW AN Port context
 */

typedef struct _sw_an_port_ctxt_s {
    bcm_port_sw_an_event_t    an_event;
    phymod_sw_an_ctxt_t       phymod_sw_an_ctxt;
    int                       an_retry;
    uint32_t                  an_state;
    int                       an_cool_down;
#ifdef PORTMOD_SUPPORT
    phymod_phy_access_t       phy_access[PORT_MAX_PHY_ACCESS_STRUCTURES];
#endif
} _sw_an_port_ctxt_t;

/*
 * AW AN module context structure
 */
typedef struct _sw_autoneg_ctxt_s {
	int                     enable; /* enable SW AN Thread */ 
    sal_mutex_t	  		    lock; /* lock to access the port bitmap */
	sal_sem_t       		signal_event; /* semaphore to signal new event to AN thread */
	sal_thread_t	  		thread_id; /* Therad id */
	char                    thread_name[SAL_THREAD_NAME_MAX_LEN]; 
	pbmp_t          		add_ports; /* bitmap of ports that are part of the SW AN */
	pbmp_t                  process_ports; /* list of ports currently serviced by SW AN */
	_sw_an_port_ctxt_t      port_sw_an_ctxt[SOC_MAX_NUM_PORTS]; /* per port SW AN context */
	uint32_t                polling_interval; /* AN thread polling intervakl in msec */
} _sw_autoneg_ctxt_t;

/* static alocation of SW AN CTXT for all units */
_sw_autoneg_ctxt_t    *_sw_autoneg_ctxt[BCM_LOCAL_UNITS_MAX];

#define SW_AN_CTXT(unit)    (_sw_autoneg_ctxt[unit])

typedef enum _sw_an_page_exchange_sm_cmd_e {
	SW_AN_PAGE_EXCHANGE_SM_CMD_STOP = 0,
	SW_AN_PAGE_EXCHANGE_SM_CMD_START,
	SW_AN_PAGE_EXCHANGE_SM_CMD_MAX
} _sw_an_page_exchange_sm_cmd_t;

/*
 * Define:
 *     SW_AN_LOCK/SW_AN_UNLOCK
 * Purpose:
 *     Serialization Macros for access to _sw_autoneg_ctxt_t structure.
 */

#define SW_AN_LOCK(unit) \
        sal_mutex_take(_sw_autoneg_ctxt[unit]->lock, sal_mutex_FOREVER)

#define SW_AN_UNLOCK(unit) \
        sal_mutex_give(_sw_autoneg_ctxt[unit]->lock)

#define SW_AN_WAIT_FOR_EVENT(unit) \
		sal_sem_take(_sw_autoneg_ctxt[unit]->signal_event, sal_sem_FOREVER)

#define SW_AN_POST_EVENT(unit) \
		sal_sem_give(_sw_autoneg_ctxt[unit]->signal_event)		        

#define UNIT_VALID_CHECK(unit) \
    if (((unit) < 0) || ((unit) >= BCM_LOCAL_UNITS_MAX)) { return BCM_E_UNIT; }

#define UNIT_INIT_CHECK(unit) \
    do { \
        UNIT_VALID_CHECK(unit); \
        if (_sw_autoneg_ctxt[unit] == NULL) { return BCM_E_INIT; } \
    } while (0)


#define SW_AN_THREAD_PRIORITY 50 /* 0 the highest and 255 the lowest*/
#define SW_AN_NUM_PAGES_TO_TX 0x3 

#define SW_AN_POLLING_INTERVAL 50
#define SW_AN_FAILING_COOL_DOWN_TIME 10 /* Cool down for 10 Polling interval when AN enters DOWN state. */

static int _bcm_sw_an_page_exchange_sm(int unit, bcm_port_t port, bcm_sw_an_fsm_event_t event);
int _bcm_sw_an_event_get(int unit, bcm_port_t port, bcm_sw_an_fsm_event_t *event);
phymod_phy_access_t * _bcm_sw_an_phy_access_get(int unit, bcm_port_t port);
static int _bcm_sw_an_rx_sigdet_get(int unit, bcm_port_t port, uint32_t *sigdet);

int 
bcm_sw_an_module_deinit(int unit) 
{
    _sw_autoneg_ctxt_t    *sw_an_ctxt;

	UNIT_VALID_CHECK(unit);

	sw_an_ctxt = SW_AN_CTXT(unit);

	if (sw_an_ctxt == NULL) {
		return BCM_E_NONE;
	}

	if (sw_an_ctxt->signal_event != NULL) {
		sal_sem_destroy(sw_an_ctxt->signal_event);
		sw_an_ctxt->signal_event = NULL;
	}

	if (sw_an_ctxt->lock != NULL) {
		sal_mutex_destroy(sw_an_ctxt->lock);
	}

    /* sal_thread_destroy is supported only with netbsd
     * thread is should be NULL once we reach this 
     * function. Incase if we delete a thread while its holding lock
     * we may have unpredicatble behaviour in non linux machines  
     */
    if (sw_an_ctxt->thread_id != NULL) {
    	sal_thread_destroy(sw_an_ctxt->thread_id);
    }
	sal_free(sw_an_ctxt);

	SW_AN_CTXT(unit) = NULL;

	return BCM_E_NONE;
}

int 
bcm_sw_an_module_init(int unit) 
{
	_sw_autoneg_ctxt_t    *sw_an_ctxt;
	uint32                size;
	uint32                polling_interval;
	int                   rv = BCM_E_NONE;


	UNIT_VALID_CHECK(unit);

	if (SW_AN_CTXT(unit) != NULL) {
		/* free the allocated resource */
		rv = bcm_sw_an_module_deinit(unit);
		if (rv < BCM_E_NONE) {
			return rv;
		}
	}

	size = sizeof(_sw_autoneg_ctxt_t);
	if ((sw_an_ctxt = sal_alloc(size, "sw_an_module")) == NULL) {
		return BCM_E_MEMORY;
	}
	sal_memset(sw_an_ctxt, 0, size);

	/* initalize the mutex and semaphore */
	sw_an_ctxt->lock = sal_mutex_create("SW_AN_LOCK");
	if (sw_an_ctxt->lock == NULL) {
		rv = BCM_E_MEMORY;
	}

	if (BCM_SUCCESS(rv)) {
		sw_an_ctxt->signal_event = sal_sem_create("SW_AN_signal_event",
												  sal_sem_BINARY,
											      0);
		if (sw_an_ctxt->signal_event == NULL) {
			sal_mutex_destroy(sw_an_ctxt->lock);
			rv = BCM_E_MEMORY;
		}
	}

	if (BCM_FAILURE(rv)) {
		sal_free(sw_an_ctxt);
	}

	/* initialize the port bitmap */
	BCM_PBMP_CLEAR(sw_an_ctxt->add_ports);
	BCM_PBMP_CLEAR(sw_an_ctxt->process_ports);

	sw_an_ctxt->thread_id = NULL;

	polling_interval = SW_AN_POLLING_INTERVAL;
	polling_interval = soc_property_get(unit, spn_SW_AUTONEG_POLLING_INTERVAL, 
						polling_interval);

	sw_an_ctxt->polling_interval = polling_interval;

    _sw_autoneg_ctxt[unit] = sw_an_ctxt;

    return BCM_E_NONE;
}

/* Run the through each port in the bit map and if they are
 * scheduled to run service that port 
 */
STATIC void
_bcm_sw_an_thread(int unit) 
{
	_sw_autoneg_ctxt_t    *sw_an_ctxt;
	_sw_an_port_ctxt_t    *port_an_ctxt;
	bcm_sw_an_fsm_event_t event; 
    bcm_port_t     		  port;
    int                   rv = BCM_E_NONE;
    int                   is_process = 0;
    uint32_t              rx_sigdet = 0;

    sw_an_ctxt = SW_AN_CTXT(unit);
    if (sw_an_ctxt == NULL) {
  	    sal_thread_exit(0);
  	    return;
    }

    sw_an_ctxt->thread_id = sal_thread_self();

    while(sw_an_ctxt->enable) {
    	SW_AN_LOCK(unit);
    	SOC_PBMP_ASSIGN(sw_an_ctxt->process_ports,
    					sw_an_ctxt->add_ports);
    	SW_AN_UNLOCK(unit);
    	/* run through the port bitmap */
    	do 
    	{
    		is_process = 0;
	    	PBMP_ITER(sw_an_ctxt->process_ports, port) {
	    		/* check if the port needs to be serviced */
	    		port_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port]);
	    		if (port_an_ctxt->an_event != BCM_PORT_SW_AN_EVENT_AN_STOP) {
	    			is_process = 1;
		    		switch(port_an_ctxt->an_event) {
		    			case BCM_PORT_SW_AN_EVENT_AN_START:
		  				    PORT_LOCK(unit);
		    				rv = _bcm_sw_an_page_exchange_sm(unit, port, BCM_FSM_SW_AN_EVT_START);
		   				    PORT_UNLOCK(unit);
		    				if (rv < BCM_E_NONE) {
		    					SW_AN_LOCK(unit);
		    					port_an_ctxt->an_event = BCM_PORT_SW_AN_EVENT_AN_DOWN;
                                port_an_ctxt->an_cool_down = 0;
		    					SW_AN_UNLOCK(unit);
		    					/* log error */
		    				}
		    				break;
						case BCM_PORT_SW_AN_EVENT_AN_RESTART:
						case BCM_PORT_SW_AN_EVENT_AN_LINK_DOWN: 
							/* update the an retry count */
							port_an_ctxt->an_retry++;
							PORT_LOCK(unit);
							rv = _bcm_sw_an_page_exchange_sm(unit, port, BCM_FSM_SW_AN_EVT_SEND_RESTART);
		   				    PORT_UNLOCK(unit);
							if (rv < BCM_E_NONE) {
								SW_AN_LOCK(unit);
		    					port_an_ctxt->an_event = BCM_PORT_SW_AN_EVENT_AN_DOWN;
                                port_an_ctxt->an_cool_down = 0;
		    					SW_AN_UNLOCK(unit);
								/* log error */
							}
							break;
						case BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS:
							PORT_LOCK(unit);
							_bcm_sw_an_event_get(unit, port, &event);
							rv = _bcm_sw_an_page_exchange_sm(unit, port, event);
		   				    PORT_UNLOCK(unit);
							if (rv < BCM_E_NONE) {
								SW_AN_LOCK(unit);
		    					port_an_ctxt->an_event = BCM_PORT_SW_AN_EVENT_AN_DOWN;
                                port_an_ctxt->an_cool_down = 0;
		    					SW_AN_UNLOCK(unit);
								/* log error */
							}
							break;
						case BCM_PORT_SW_AN_EVENT_AN_STOP:
							break;
                        case BCM_PORT_SW_AN_EVENT_AN_DOWN:
                            port_an_ctxt->an_cool_down++;
                            if (port_an_ctxt->an_cool_down >= SW_AN_FAILING_COOL_DOWN_TIME) {
                                PORT_LOCK(unit);
                                rv = _bcm_sw_an_rx_sigdet_get(unit, port, &rx_sigdet);
                                PORT_UNLOCK(unit);
                                if ((rv < BCM_E_NONE) || !rx_sigdet) {
                                    SW_AN_LOCK(unit);
                                    port_an_ctxt->an_event = BCM_PORT_SW_AN_EVENT_AN_DOWN;
                                    port_an_ctxt->an_cool_down = 0;
                                    SW_AN_UNLOCK(unit);
                                } else {
                                    SW_AN_LOCK(unit);
                                    port_an_ctxt->an_event = BCM_PORT_SW_AN_EVENT_AN_RESTART;
                                    SW_AN_UNLOCK(unit);
                                }
                            }
    					default:
							break;		    				
		    		}
	    		}
    		}
    		sal_msleep(sw_an_ctxt->polling_interval); 
    		SW_AN_LOCK(unit);
    		SOC_PBMP_ASSIGN(sw_an_ctxt->process_ports,
    					sw_an_ctxt->add_ports);
    		SW_AN_UNLOCK(unit);
    	} while(is_process);

    	/* wait on a semaphore */
    	SW_AN_WAIT_FOR_EVENT(unit);
    }

    sw_an_ctxt->thread_id = NULL;
    sal_thread_exit(0);
    return;
}



int 
bcm_sw_an_enable_set(int unit, int enable) 
{
	_sw_autoneg_ctxt_t   *sw_an_ctxt = NULL;
	int 				 rv          = BCM_E_NONE;
	soc_timeout_t   	 to;
	sal_usecs_t          wait_usec;

	/* Time to wait for thread to start/end (longer for BCMSIM) */
    wait_usec = (SAL_BOOT_BCMSIM || SAL_BOOT_QUICKTURN) ? 30000000 : 10000000;

    UNIT_INIT_CHECK(unit);

    sw_an_ctxt = SW_AN_CTXT(unit);

    if (enable) {

    	if (sw_an_ctxt->thread_id != NULL) {
    		/* wakeup the SW AN thread */
    		SW_AN_POST_EVENT(unit);
    		return BCM_E_NONE;
    	}

    	sw_an_ctxt->enable = enable;

		sal_snprintf(sw_an_ctxt->thread_name,
	                 sizeof (sw_an_ctxt->thread_name),
	                 "SW_AN.%0x",
	                 unit);

		if (sal_thread_create(sw_an_ctxt->thread_name,
                              SAL_THREAD_STKSZ,
                              SW_AN_THREAD_PRIORITY,
                              (void (*)(void*))_bcm_sw_an_thread,
                              INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
			rv = BCM_E_MEMORY;
		} else {
			/* wait for the timeout period to expire to 
			 * check if the thread creation is successful
			 */
			soc_timeout_init(&to, wait_usec, 0);

			while(sw_an_ctxt->thread_id == NULL) {
				if ((soc_timeout_check(&to))) {
					LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "%s: Thread did not start\n"),
                               			  sw_an_ctxt->thread_name));
                    rv = BCM_E_INTERNAL;
                    break;
				}
			}

		}
    } else {
    	/* set the enable flag to 0 and wait for the 
    	 * the thread to exit
    	 */
    	sw_an_ctxt->enable = enable; 

    }   
    return rv;
}

int 
bcm_sw_an_enable_get(int unit, int *enable) 
{
	_sw_autoneg_ctxt_t   *sw_an_ctxt = NULL;

	UNIT_INIT_CHECK(unit);

    sw_an_ctxt = SW_AN_CTXT(unit);

    *enable = sw_an_ctxt->enable;

    return BCM_E_NONE;
}

int 
bcm_sw_an_post_event(int unit, bcm_port_t port, bcm_port_sw_an_event_t event) 
{
	_sw_autoneg_ctxt_t    *sw_an_ctxt;
	_sw_an_port_ctxt_t    *port_an_ctxt;

    UNIT_INIT_CHECK(unit);

    if (event <= BCM_PORT_SW_AN_EVENT_NONE ||
    	event >= BCM_PORT_SW_AN_EVENT_AN_MAX)  {
    	/* nothing to notify the thread */
    	return BCM_E_NONE;
    }

    sw_an_ctxt = SW_AN_CTXT(unit);

    if (!SOC_PBMP_MEMBER(sw_an_ctxt->add_ports, port)) {
    	/* given port is not part of SW AN process thread 
    	 * nothing to notify
    	 */
    	return BCM_E_NONE; 
    }

    port_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port]);

    SW_AN_LOCK(unit);
    port_an_ctxt->an_event = event;
    SW_AN_UNLOCK(unit);

    /* notify the SW AN thread */
    SW_AN_POST_EVENT(unit);

    return BCM_E_NONE;
}

int
bcm_sw_an_port_register(int unit, bcm_port_t port) 
{
	_sw_autoneg_ctxt_t    *sw_an_ctxt;
    int                   enable = 0;
    int                   rv = BCM_E_NONE;

	UNIT_INIT_CHECK(unit);

	sw_an_ctxt = SW_AN_CTXT(unit);

    if (!SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port) || 
    	!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
    	return BCM_E_PORT;
	}

	 /* if AN Thread is not enabled enable the AN thread */
    rv = bcm_sw_an_enable_get(unit, &enable);
    if (rv != BCM_E_NONE) {
    	return rv;
    }

    if (!enable) {
    	rv = bcm_sw_an_enable_set(unit, 1);	
    	if (rv != BCM_E_NONE) {
    		return rv;
    	}
    }

    if (!SOC_PBMP_MEMBER(sw_an_ctxt->add_ports, port)) {
		SW_AN_LOCK(unit);
		BCM_PBMP_PORT_ADD(sw_an_ctxt->add_ports, port);
		SW_AN_UNLOCK(unit);

	    rv = bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_START);
	    return rv;
	} else {
		/* SW AN is already enabled for this port  restart SW AN */
		rv = bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_RESTART);
	}

	return BCM_E_NONE;	
}

int
bcm_sw_an_port_unregister(int unit, bcm_port_t port) 
{
	_sw_autoneg_ctxt_t    *sw_an_ctxt;
	phymod_phy_access_t   *phy = NULL;

	UNIT_INIT_CHECK(unit);

	sw_an_ctxt = SW_AN_CTXT(unit);

    if (!SOC_PBMP_MEMBER(sw_an_ctxt->add_ports, port) || 
    	!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
    	return BCM_E_PORT;
	}
	
	SW_AN_LOCK(unit);
	BCM_PBMP_PORT_REMOVE(sw_an_ctxt->add_ports, port);
	SW_AN_UNLOCK(unit);

	phy = _bcm_sw_an_phy_access_get(unit, port);
	if (NULL == phy) {
		return BCM_E_INTERNAL;
	}

	PHYMOD_IF_ERR_RETURN
		(phymod_phy_sw_autoneg_enable(phy, 0));

	return BCM_E_NONE;	
}

#ifdef PORTMOD_SUPPORT
phymod_phy_access_t *
_bcm_sw_an_portmod_phy_access_get(int unit, bcm_port_t port) {
    _sw_autoneg_ctxt_t    *sw_an_ctxt;
    pm_info_t pm_info;
    int chain_length = 0;
    portmod_dispatch_type_t __type__;
    int rv = 0;


    sw_an_ctxt = SW_AN_CTXT(unit);

    rv = portmod_port_pm_type_get(unit, port, &port, &__type__);
    if (rv != BCM_E_NONE) {
        return NULL;
    }

    rv = portmod_pm_info_get(unit, port, &pm_info);
    if (rv != BCM_E_NONE) {
        return NULL;
    }

    rv = portmod_port_chain_phy_access_get(unit, port, pm_info, sw_an_ctxt->port_sw_an_ctxt[port].phy_access,
                                           PORT_MAX_PHY_ACCESS_STRUCTURES,
                                           &chain_length);

    if (rv != BCM_E_NONE) {
        return NULL;
    }

    return &(sw_an_ctxt->port_sw_an_ctxt[port].phy_access[0]);
}
#endif

phymod_phy_access_t *
_bcm_sw_an_phy_access_get(int unit, bcm_port_t port) 
{
	phy_ctrl_t                *pc;
    soc_phymod_ctrl_t         *pmc;
    soc_phymod_phy_t          *phy;


    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return _bcm_sw_an_portmod_phy_access_get(unit, port);
    }

    /* locate phy control */
    pc = INT_PHY_SW_STATE(unit, port);
    if (pc == NULL) {
        return NULL;
    }

    pmc = &pc->phymod_ctrl;

    /* only request autoneg on the first core */
    phy = pmc->phy[0];
    if (phy == NULL) {
        return NULL;
    }

    return (&phy->pm_phy);
}

int
_bcm_sw_an_event_get(int unit, bcm_port_t port, bcm_sw_an_fsm_event_t *event) 
{
	phymod_sw_an_ctrl_status_t		an_ctrl_status;
	phymod_phy_access_t             *phy = NULL;

	phymod_sw_an_ctrl_status_t_init(&an_ctrl_status);
	phy = _bcm_sw_an_phy_access_get(unit, port);
	if (NULL == phy) {
		return BCM_E_INTERNAL;
	}
	/* call the phymod API to get the event*/
	PHYMOD_IF_ERR_RETURN
		(phymod_phy_sw_an_control_status_get(phy, &an_ctrl_status));

	*event = BCM_FSM_SW_AN_EVT_WAIT;

	if (an_ctrl_status.seq_restart) {
		/* not handled */
	} else if (an_ctrl_status.page_req) {
		/* handled as part of lp_page_rdy*/
	} else if (an_ctrl_status.lp_page_rdy) {
		*event = BCM_FSM_SW_AN_EVT_LP_PAGE_RDY;
	} else if (an_ctrl_status.an_completed) {
		*event = BCM_FSM_SW_AN_EVT_PAGE_EX_COMPLETE;
	} else {
		/* do nothing */
	}
	return BCM_E_NONE;
}

int
_bcm_sw_an_page_exchange_sm(int unit, bcm_port_t port, bcm_sw_an_fsm_event_t event) 
{
	_sw_autoneg_ctxt_t    			 *sw_an_ctxt;
	_sw_an_port_ctxt_t               *port_an_ctxt;
	phymod_sw_an_ctxt_t              *phymod_an_ctxt;
	phymod_phy_access_t              *phy;
	int                              rv; 

	UNIT_INIT_CHECK(unit);

	sw_an_ctxt = SW_AN_CTXT(unit);
	rv    = BCM_E_NONE;

	phy = _bcm_sw_an_phy_access_get(unit, port);
    if (NULL == phy) {
    	return BCM_E_INTERNAL;
    }

    port_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port]);
    phymod_an_ctxt = &port_an_ctxt->phymod_sw_an_ctxt;

    port_an_ctxt->an_state |= 0x1 << event;

	switch(event) {
		case BCM_FSM_SW_AN_EVT_START:
		    phymod_an_ctxt->tx_pages_cnt = 0;
			phymod_an_ctxt->rx_pages_cnt = 0;
			phymod_an_ctxt->no_pages_to_be_txed = SW_AN_NUM_PAGES_TO_TX;
			PHYMOD_IF_ERR_RETURN
				(phymod_phy_sw_an_base_page_exchange_handler(phy, phymod_an_ctxt));
			bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS);	
			break;
		case BCM_FSM_SW_AN_EVT_SEND_RESTART:
			phymod_an_ctxt->tx_pages_cnt = 0;
			phymod_an_ctxt->rx_pages_cnt = 0;
			phymod_an_ctxt->no_pages_to_be_txed = SW_AN_NUM_PAGES_TO_TX;
			/* Diable the SW AN to toggle SW AN bit */
			PHYMOD_IF_ERR_RETURN
				(phymod_phy_sw_autoneg_enable(phy, 0));
			PHYMOD_IF_ERR_RETURN
				(phymod_phy_sw_an_base_page_exchange_handler(phy, phymod_an_ctxt));
			bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS);
			break;
		case BCM_FSM_SW_AN_EVT_LP_PAGE_RDY:
			PHYMOD_IF_ERR_RETURN
				(phymod_phy_sw_an_lp_page_rdy_handler(phy, phymod_an_ctxt));
			bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS);	
			break;
		case BCM_FSM_SW_AN_EVT_LD_PAGE:
			/* do nothing */
			break;
		case BCM_FSM_SW_AN_EVT_PAGE_EX_COMPLETE:
				bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_STOP);
			break;
		case BCM_FSM_SW_AN_EVT_WAIT:
			bcm_sw_an_post_event(unit, port, BCM_PORT_SW_AN_EVENT_AN_IN_PROCESS);
			break;	
		default:
			break;					
	}
    return rv;
}

int
_bcm_sw_an_rx_sigdet_get(int unit, bcm_port_t port, uint32_t *sigdet)
{
    phymod_phy_access_t *phy;

    phy = _bcm_sw_an_phy_access_get(unit, port);
    if (NULL == phy) {
        return BCM_E_INTERNAL;
    }

    PHYMOD_IF_ERR_RETURN
        (phymod_phy_rx_signal_detect_get(phy, sigdet));

    return BCM_E_NONE;
}

int
bcm_sw_an_ability_advert_set(int unit, soc_port_t port,
                                 soc_port_ability_t *ability)
{
    int                       port_num_lanes;
    bcm_port_if_t             line_interface;
    uint32_t                  an_tech_ability;
    uint32_t                  an_bam37_ability;
    uint32_t                  an_bam73_ability;
    _shr_port_mode_t          speed_full_duplex;
    phymod_autoneg_ability_t  phymod_autoneg_ability;
    int                       rv = BCM_E_NONE;
    uint8_t                   sw_an_mode = 0;

    port_num_lanes = SOC_INFO(unit).port_num_lanes[port];
    rv = bcmi_esw_portctrl_interface_get(unit, port, &line_interface);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    an_tech_ability  = 0;
    an_bam37_ability = 0;
    an_bam73_ability = 0;
    speed_full_duplex = ability->speed_full_duplex;

    PHYMOD_IF_ERR_RETURN
        (phymod_autoneg_ability_t_init(&phymod_autoneg_ability));

    sw_an_mode = soc_property_port_get(unit, port,
                      spn_PHY_AN_C73, sw_an_mode);

    if (port_num_lanes == 4) {
        if (speed_full_duplex & SOC_PA_SPEED_100GB) {
            if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
            } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
            } else {
                PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
            }
        }
        if (speed_full_duplex & SOC_PA_SPEED_40GB) {
            if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
            } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
            } else {
                PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
            }
        }
    } else if (port_num_lanes == 2) {
        /* Supports only 50G in SW AN */
        if(speed_full_duplex & SOC_PA_SPEED_50GB) {
            /* need to fix CR2 bit shift issue */
            if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
            } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_bam73_ability);
            } else {
                PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
            }
        }
    } else {
        if(speed_full_duplex & SOC_PA_SPEED_25GB) {
            if (ability->medium & SOC_PA_MEDIUM_BACKPLANE) {
                if (sw_an_mode != SW_AN_MODE_MSA_ONLY) {
                      if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                         PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                      } else {
                           PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                      }
                }
                PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
            } else if (ability->medium & SOC_PA_MEDIUM_COPPER) {
                if (sw_an_mode != SW_AN_MODE_MSA_ONLY) {
                        if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        } else {
                            PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                        }
                }
                PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_bam73_ability);
            } else {
                if (sw_an_mode != SW_AN_MODE_MSA_ONLY) {
                      if (ability->channel & SOC_PA_CHANNEL_SHORT) {
                         PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                      } else {
                           PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                      }
                }
                PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
            }
        }
        if (speed_full_duplex & SOC_PA_SPEED_10GB) {
            PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
        }
    }

    phymod_autoneg_ability.an_cap = an_tech_ability;
    phymod_autoneg_ability.cl73bam_cap = an_bam73_ability;
    phymod_autoneg_ability.cl37bam_cap = an_bam37_ability;
    phymod_autoneg_ability.sgmii_speed = 0;

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_RX:
        /* an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(&phymod_autoneg_ability);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(&phymod_autoneg_ability);
        break;
    }

    phymod_autoneg_ability.an_cl72 = 1;


    /*next need to check FEC ability */
    if (ability->fec == SOC_PA_FEC_NONE) {
        PHYMOD_AN_FEC_OFF_SET(phymod_autoneg_ability.an_fec);
    } else {
        if (ability->fec & SOC_PA_FEC_CL74)
            PHYMOD_AN_FEC_CL74_SET(phymod_autoneg_ability.an_fec);
        if (ability->fec & SOC_PA_FEC_CL91)
            PHYMOD_AN_FEC_CL91_SET(phymod_autoneg_ability.an_fec);
    }

    rv = bcm_sw_an_advert_set(unit, port, &phymod_autoneg_ability);

    return rv;

}

int 
bcm_sw_an_advert_set(int unit, bcm_port_t port, phymod_autoneg_ability_t *ability) 
{
	_sw_autoneg_ctxt_t    			 *sw_an_ctxt;
	phymod_sw_an_ctxt_t              *phymod_an_ctxt;
	phymod_phy_access_t              *phy;
	uint8_t                          sw_an_mode = 0;

	UNIT_INIT_CHECK(unit);

	phy = _bcm_sw_an_phy_access_get(unit, port);
    if (NULL == phy) {
    	return BCM_E_INTERNAL;
    }

	sw_an_ctxt = SW_AN_CTXT(unit);
	phymod_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port].phymod_sw_an_ctxt);

	sw_an_mode = soc_property_port_get(unit, port,
                       spn_PHY_AN_C73, sw_an_mode);

	switch(sw_an_mode) {
		case SW_AN_MODE_CL73_MSA:
			SW_AN_LOCK(unit);
			phymod_an_ctxt->an_mode = phymod_AN_MODE_CL73_MSA;
			SW_AN_UNLOCK(unit);
			break;
		case SW_AN_MODE_MSA_ONLY:
			SW_AN_LOCK(unit);
			phymod_an_ctxt->an_mode = phymod_AN_MODE_MSA;
			SW_AN_UNLOCK(unit);
			break;
		default:
			/* Ideally we should not hit this case*/
			SW_AN_LOCK(unit);
			phymod_an_ctxt->an_mode = phymod_AN_MODE_CL73_MSA;
			SW_AN_UNLOCK(unit);
			break;
	}

	PHYMOD_IF_ERR_RETURN
		(phymod_phy_sw_an_advert_set(phy, ability, phymod_an_ctxt));

	return BCM_E_NONE;	
}

int
_bcm_sw_an_ability_get(const phymod_an_pages_t *an_pages, soc_port_ability_t *portAbility)
{
    uint16_t            ability = 0, pause_cap = 0;
    _shr_port_mode_t    spd_fd = 0;
    uint32_t            msa_code_13_23 = 0, msa_code_2_12 = 0, msa_code_0_1 =0;
    uint16_t            msa_mode = 0;

    portAbility->channel = SOC_PA_CHANNEL_LONG;
    portAbility->medium = SOC_PA_MEDIUM_BACKPLANE;
    portAbility->fec = 0;
    portAbility->pause = 0;

    /* Get PAUSE ability */
    ability = an_pages->base_page.page_0;
    pause_cap = (ability >>  SW_AN_BASE0_PAGE_CAP_OFFSET) & SW_AN_BASE0_PAGE_CAP_MASK;
    if (pause_cap == 3) {
        portAbility->pause = SOC_PA_PAUSE_RX;
    } else if (pause_cap == 2) {
        portAbility->pause = SOC_PA_PAUSE_TX;
    } else if (pause_cap == 1) {
        portAbility->pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
    }

    /* Get CL73 speed ability */
    ability = an_pages->base_page.page_1;
    if ((ability >> SW_AN_BASE_TECH_ABILITY_25GKR_OFFSET) & SW_AN_BASE_TECH_ABILITY_25GKR_MASK) {
        spd_fd|= SOC_PA_SPEED_25GB;
    }
    if ((ability >> SW_AN_BASE_TECH_ABILITY_25GKRS_OFFSET) & SW_AN_BASE_TECH_ABILITY_25GKRS_MASK) {
        spd_fd|= SOC_PA_SPEED_25GB;
        portAbility->channel = SOC_PA_CHANNEL_SHORT;
    }
    if ((ability >> SW_AN_BASE_TECH_ABILITY_10GKR_OFFSET) & SW_AN_BASE_TECH_ABILITY_10GKR_MASK) {
        spd_fd|= SOC_PA_SPEED_10GB;
    }

    /* Get CL73 FEC ability */
    ability = an_pages->base_page.page_2;
    if ((ability >> SW_AN_BASE_CL91_ABILITY_REQ_OFFSET) & SW_AN_BASE_CL91_ABILITY_REQ_MASK) {
        portAbility->fec |= SOC_PA_FEC_CL91;
    }
    if ((ability >> SW_AN_BASE_CL74_25GKRS_REQ_OFFSET) & SW_AN_BASE_CL74_25GKRS_REQ_MASK) {
        portAbility->fec |= SOC_PA_FEC_CL74;
    }
    if ((ability >> SW_AN_BASE_CL74_ABILITY_REQ_OFFSET) & SW_AN_BASE_CL74_ABILITY_REQ_MASK) {
        portAbility->fec |= SOC_PA_FEC_CL74;
    }

    msa_code_13_23 = ((an_pages->msg_page.page_1 >> SW_AN_MSG_PAGE1_OUI_13to23_OFFSET) &
                      SW_AN_MSG_PAGE1_OUI_13to23_MASK);
    msa_code_2_12 = ((an_pages->msg_page.page_2 >> SW_AN_MSG_PAGE2_OUI_2to12_OFFSET) &
                      SW_AN_MSG_PAGE2_OUI_2to12_MASK);
    msa_code_0_1  = ((an_pages->ufmt_page.page_0 >> SW_AN_UF_PAGE0_OUI_OFFSET) &
                      SW_AN_UF_PAGE0_OUI_MASK);
    msa_mode = ((an_pages->base_page.page_0 >> SW_AN_BASE0_PAGE_NP_OFFSET) & SW_AN_BASE0_PAGE_NP_MASK);

    /* Check MSA ability if applicable */
    if ((msa_mode) && (msa_code_0_1 == SW_AN_MSA_OUI_0to1) &&
        (msa_code_13_23 == SW_AN_MSA_OUI_13to23) && (msa_code_2_12  == SW_AN_MSA_OUI_2to12)) {
        ability = an_pages->ufmt_page.page_1;
        if ((ability >> SW_AN_UF_PAGE1_50G_KR2_ABILITY_OFFSET) & SW_AN_UF_PAGE1_50G_KR2_ABILITY_MASK) {
            spd_fd|= SOC_PA_SPEED_50GB;
        }
        if ((ability >> SW_AN_UF_PAGE1_50G_CR2_ABILITY_OFFSET) & SW_AN_UF_PAGE1_50G_CR2_ABILITY_MASK) {
            spd_fd|= SOC_PA_SPEED_50GB;
            portAbility->medium = SOC_PA_MEDIUM_COPPER;
        }
        if ((ability >> SW_AN_UF_PAGE1_25G_KR1_ABILITY_OFFSET) & SW_AN_UF_PAGE1_25G_KR1_ABILITY_MASK) {
            spd_fd|= SOC_PA_SPEED_25GB;
        }
        if ((ability >> SW_AN_UF_PAGE1_25G_CR1_ABILITY_OFFSET) & SW_AN_UF_PAGE1_25G_CR1_ABILITY_MASK) {
            spd_fd|= SOC_PA_SPEED_25GB;
            portAbility->medium = SOC_PA_MEDIUM_COPPER;
        }
        ability = an_pages->ufmt_page.page_2;
        if ((ability >> SW_AN_UF_PAGE2_CL91_REQ_OFFSET) & SW_AN_UF_PAGE2_CL91_REQ_MASK) {
            portAbility->fec |= SOC_PA_FEC_CL91;
        }
        if ((ability >> SW_AN_UF_PAGE2_CL74_REQ_OFFSET) & SW_AN_UF_PAGE2_CL74_REQ_MASK) {
            portAbility->fec |= SOC_PA_FEC_CL74;
        }
    }
    portAbility->speed_full_duplex = spd_fd;

    /* If there's no FEC ability, set FEC_NONE */
    if (portAbility->fec == 0) {
        portAbility->fec = SOC_PA_FEC_NONE;
    }

    return BCM_E_NONE;
}


int
bcm_sw_an_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *portAbility)
{
    _sw_autoneg_ctxt_t        *sw_an_ctxt;
    phymod_sw_an_ctxt_t       *phymod_an_ctxt;
    phymod_an_pages_t         *an_pages;
    int                       rv = 0;

    UNIT_INIT_CHECK(unit);

    sw_an_ctxt = SW_AN_CTXT(unit);
    phymod_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port].phymod_sw_an_ctxt);
    an_pages = &phymod_an_ctxt->tx_pages;

    rv = _bcm_sw_an_ability_get(an_pages, portAbility);

    return rv;
}

int
bcm_sw_an_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *portAbility)
{
    _sw_autoneg_ctxt_t        *sw_an_ctxt;
    phymod_sw_an_ctxt_t       *phymod_an_ctxt;
    phymod_an_pages_t         *an_pages;
    int                       rv = 0;

    UNIT_INIT_CHECK(unit);

    sw_an_ctxt = SW_AN_CTXT(unit);
    phymod_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port].phymod_sw_an_ctxt);
    an_pages = &phymod_an_ctxt->rx_pages;

    rv = _bcm_sw_an_ability_get(an_pages, portAbility);

    return rv;
}

int
bcm_sw_an_port_diag(int unit, int port) 
{
	_sw_autoneg_ctxt_t    			 *sw_an_ctxt;
	phymod_sw_an_ctxt_t              *phymod_an_ctxt;
	_sw_an_port_ctxt_t               *port_an_ctxt;

	UNIT_INIT_CHECK(unit);

	sw_an_ctxt = SW_AN_CTXT(unit);
	phymod_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port].phymod_sw_an_ctxt);
	port_an_ctxt = &(sw_an_ctxt->port_sw_an_ctxt[port]);

    sal_printf("********** SW AN CONTEXT INFORMATION FOR THE UNIT:%0x PORT:%0x **********\n", unit, port);
    sal_printf("PORT SW AN STATE:        \t %0x\n", port_an_ctxt->an_state);
    sal_printf("PORT SW AN TX PAGES:     \t %0x\n", phymod_an_ctxt->tx_pages_cnt);
    sal_printf("PORT SW AN RX PAGES:     \t %0x\n", phymod_an_ctxt->rx_pages_cnt);
    
    sal_printf("PORT SW AN TX BASE PAGE: \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->tx_pages.base_page.page_2, phymod_an_ctxt->tx_pages.base_page.page_1, 
    	phymod_an_ctxt->tx_pages.base_page.page_0);
        sal_printf("PORT SW AN TX MSG PAGE:  \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->tx_pages.msg_page.page_2, phymod_an_ctxt->tx_pages.msg_page.page_1, 
    	phymod_an_ctxt->tx_pages.msg_page.page_0);
    sal_printf("PORT SW AN TX UP PAGE:   \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->tx_pages.ufmt_page.page_2, phymod_an_ctxt->tx_pages.ufmt_page.page_1, 
    	phymod_an_ctxt->tx_pages.ufmt_page.page_0);

    sal_printf("PORT SW AN RX BASE PAGE: \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->rx_pages.base_page.page_2, phymod_an_ctxt->rx_pages.base_page.page_1, 
    	phymod_an_ctxt->rx_pages.base_page.page_0);
    sal_printf("PORT SW AN RX MSG PAGE:  \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->rx_pages.msg_page.page_2, phymod_an_ctxt->rx_pages.msg_page.page_1, 
    	phymod_an_ctxt->rx_pages.msg_page.page_0);
    sal_printf("PORT SW AN RX UP PAGE:   \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->rx_pages.ufmt_page.page_2, phymod_an_ctxt->rx_pages.ufmt_page.page_1, 
    	phymod_an_ctxt->rx_pages.ufmt_page.page_0);

    sal_printf("PORT SW AN  EXT PAGE:    \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->tx_pages.null_page.page_2, phymod_an_ctxt->tx_pages.null_page.page_1, 
    	phymod_an_ctxt->tx_pages.null_page.page_0);
    sal_printf("PORT SW AN  EXT PAGE:    \t (48-32):%0x (31-16):%0x (15-0):%0x\n", 
    	phymod_an_ctxt->rx_pages.null_page.page_2, phymod_an_ctxt->rx_pages.null_page.page_1, 
    	phymod_an_ctxt->rx_pages.null_page.page_0);

    return BCM_E_NONE;
}
#endif /* SW_AUTONEG_SUPPORT */

