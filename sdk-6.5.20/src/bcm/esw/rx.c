/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rx.c
 * Purpose:     Receive packet mechanism
 * Requires:
 *
 * See sdk/doc/txrx.txt and pkt.txt for
 * information on the RX API and implementation.
 *
 * Quick overview:
 *
 *     Packet buffer allocation/deallocation is user configurable.
 *     This expects to be given monolithic (single block) buffers.
 *     When "HANDLED_OWNED" is returned by a handler, that means
 *     that the data buffer is stolen, not the packet structure.
 *
 *     Callback functions may be registered in interrupt or non-
 *     interrupt mode.  Non-interrupt is preferred.
 *
 *     Interrupt load is limited by setting overall rate limit
 *     (bcm_rx_rate_burst_set/get).
 *
 *     If a packet is not serviced in interrupt mode, it is queued
 *     based on its COS.
 *
 *     Each queue has a rate limit (bcm_rx_cos_rate_set/get) which
 *     controls the number of callbacks that will be made for the queue.
 *     The non-interrupt thread services these queues from highest to
 *     lowest and will discard packets in the queue when they exceed
 *     the queue's rate limit.
 *
 *     Packets handled at interrupt level are still accounted for in
 *     the COS rate limiting.
 *
 *     A channel is:
 *          Physically:  A separate hardware DMA process
 *          Logically:  A collection of COS bundled together.
 *     Rate limiting per channel is no longer supported (replaced
 *     by COS queue rate limiting).
 *
 *     Channels may be enabled and disabled separately from starting RX
 *     running.  However, stopping RX disables all channels.
 *
 *     Packets are started in groups called "chains", each of which
 *     is controlled by a "DV" (DMA-descriptor vector).
 *
 *     Updates to the handler linked list need to be synchronized
 *     both with thread packet processing (mutex) and interrupt
 *     packet processing (spl).
 *
 *     If no real callouts are registered (other than internal discard)
 *     don't bother starting DVs, nor queuing input pkts into cos queues.
 */

/* We need to call top-level APIs on other units */
#ifdef BCM_HIDE_DISPATCHABLE
#undef BCM_HIDE_DISPATCHABLE
#endif
#include <shared/bsl.h>

#include <shared/alloc.h>
#include <shared/bitop.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/higig.h>
#include <bcm/rx.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/rx.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/firebolt6.h>
#include <bcm_int/esw/hurricane4.h>
#include <bcm_int/esw/apache.h>
#include <bcm_int/esw/monterey.h>
#ifdef BCM_HURRICANE2_SUPPORT
#include <bcm_int/esw/hurricane2.h>
#endif
#if defined (BCM_EP_REDIRECT_VERSION_2)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_EP_REDIRECT_VERSION_2 */


#include <bcm_int/api_xlate_port.h>

#include <bcm_int/rpc/rlink.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_RX_WB_VERSION_1_0                     SOC_SCACHE_VERSION(1,0)
#define BCM_RX_WB_VERSION_1_1                     SOC_SCACHE_VERSION(1,1)
#define BCM_RX_WB_VERSION_1_2                     SOC_SCACHE_VERSION(1,2)
#define BCM_RX_WB_DEFAULT_VERSION                 BCM_RX_WB_VERSION_1_2

STATIC int _bcm_esw_rx_recover(int unit);
STATIC int _bcm_esw_rx_wb_scache_alloc(int unit);
#endif
#ifdef BCM_TRX_SUPPORT
STATIC int _bcm_esw_rx_cosq_mapping_init(int unit);
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/format.h>
#include <soc/mcm/formatacc.h>
#endif


/*
 * Function:
 *      bcm_esw_rx_sched_register
 * Purpose:
 *      Rx scheduler registration function. 
 * Parameters:
 *      unit       - (IN) Unused. 
 *      sched_cb   - (IN) Rx scheduler routine.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_sched_register(int unit, bcm_rx_sched_cb sched_cb)
{
    return (_bcm_common_rx_sched_register(unit, sched_cb));
}

/*
 * Function:
 *      bcm_esw_rx_sched_unregister
 * Purpose:
 *      Rx scheduler de-registration function. 
 * Parameters:
 *      unit  - (IN) Unused. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_sched_unregister(int unit)
{
    return (_bcm_common_rx_sched_unregister(unit));
}

/*
 * Function:
 *      bcm_esw_rx_unit_next_get
 * Purpose:
 *      Rx started units iteration routine.
 * Parameters:
 *      unit       - (IN)  BCM device number. 
 *      unit_next  - (OUT) Next attached unit with started rx.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_unit_next_get(int unit, int *unit_next)
{
    return (_bcm_common_rx_unit_next_get(unit, unit_next));
}

/*
 * Function:
 *      bcm_esw_rx_queue_max_get
 * Purpose:
 *      Get maximum cos queue number for the device.
 * Parameters:
 *      unit    - (IN) BCM device number. 
 *      cosq    - (OUT) Maximum queue priority.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_queue_max_get(int unit, bcm_cos_queue_t  *cosq)
{  
    return (_bcm_common_rx_queue_max_get(unit, cosq));
}

/*
 * Function:
 *      bcm_esw_rx_queue_packet_count_get
 * Purpose:
 *      Get number of packets awaiting processing in the specific device/queue.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      cosq         - (IN) Queue priority.
 *      packet_count - (OUT) Number of packets awaiting processing. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_queue_packet_count_get(int unit, bcm_cos_queue_t cosq, int *packet_count)
{
    return (_bcm_common_rx_queue_packet_count_get(unit, cosq, packet_count));
}

/*
 * Function:
 *      bcm_esw_rx_queue_rate_limit_status_get
 * Purpose:
 *      Get number of packet that can be rx scheduled 
 *      until system hits queue rx rate limit. 
 * Parameters:
 *      unit           - (IN) BCM device number. 
 *      cosq           - (IN) Queue priority.
 *      packet_tokens  - (OUT)Maximum number of packets that can be  scheduled.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_queue_rate_limit_status_get(int unit, bcm_cos_queue_t cosq, 
                                       int *packet_tokens)
{
    return (_bcm_common_rx_queue_rate_limit_status_get(unit, cosq, 
						       packet_tokens));
}


/*
 * Function:
 *      bcm_rx_init
 * Purpose:
 *      Software initialization for RX API
 * Parameters:
 *      unit - Unit to init
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Allocates rx control structure
 *      Copies default config into active config
 *      Adds discard handler
 */

int
bcm_esw_rx_init(int unit)
{
    int rv = BCM_E_NONE;

    /* RX init is done already during early attach */
    if (BCM_CONTROL(unit)->attach_state != _bcmControlStateTxRxInited) {
        rv =_bcm_common_rx_init(unit);
    }
    if (!RX_IS_LOCAL(unit) || !SOC_UNIT_VALID(unit)) {
        return rv;
    }

#if defined (BCM_EP_REDIRECT_VERSION_2)

    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

        int size = 0;

        /* Create a mutex for copy-to-cpu protection */
        _bcmi_rx_copy_to_cpu_config_mutex[unit] =
            sal_mutex_create("RX_COPY_TO_CPU_MUTEX");
        if (_bcmi_rx_copy_to_cpu_config_mutex[unit] == NULL) {
            return (BCM_E_MEMORY);
        }

        /* Allocate space to store the copy-to-cpu config data */
        size = soc_mem_index_count(unit, EP_CTC_RES_TABLEm) *
                   sizeof(_bcmi_rx_CopyToCpu_config_t);
        _bcmi_rx_egr_drop_copy_to_cpu_config_data[unit] =
                        sal_alloc(size, "copy-to-cpu table data config alloc");
        if (_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit], 0, size);

        /* Allocate space to store the inuse bitmap for copy-to-cpu config */
        size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, EP_CTC_RES_TABLEm));
        _bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit] =
                        sal_alloc(size, "copy-to-cpu table bitmap alloc");
        if (_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit] == NULL) {
            sal_free(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit]);
            return BCM_E_MEMORY;
        }
        sal_memset(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], 0, size);
    }
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) && (rv == BCM_E_NONE)) {
        /* Early attach: Postpone RX WB recovery until bcm_attach() */
        if (BCM_CONTROL(unit)->attach_state != _bcmControlStateTxRxInit) {
            rv = _bcm_esw_rx_recover(unit);
        }
    } else if (rv == BCM_E_NONE) {

       /* Allocate Warm boot level 2 cache size */
       rv = _bcm_esw_rx_wb_scache_alloc(unit);

       if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
           return rv;
       }

        rv = BCM_E_NONE;
    }

    if (!SOC_WARM_BOOT(unit))
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
#ifdef BCM_TRX_SUPPORT
        /* TODO: many need to add !TRIDENT3 */
        if (SOC_IS_TRX(unit)) {
        if(rv == BCM_E_NONE){
                /*Init cosq mapping*/
            rv = _bcm_esw_rx_cosq_mapping_init(unit);
        }
    }
#endif
    }

    return rv;
}


/*
 * Function:
 *      bcm_rx_cfg_init
 * Purpose:
 *      Re-initialize the user level configuration
 * Parameters:
 *      unit - StrataXGS unit number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Can't use if currently running.  Should be called before
 *      doing a simple modification of the RX configuration in case
 *      the previous user has left it in a strange state.
 */

int
bcm_esw_rx_cfg_init(int unit)
{
    return (_bcm_common_rx_cfg_init(unit));
}

/*
 * Function:
 *      bcm_rx_start
 * Purpose:
 *      Initialize and configure the RX subsystem for a given unit
 * Parameters:
 *      unit - Unit to configure
 *      cfg - Configuration to use.  See include/bcm/rx.h
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Starts the packet receive thread if not already running.
 *      cfg may be null:  Use default config.
 *      alloc/free in cfg may be null:  Use default alloc/free functions
 */

int
bcm_esw_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
    return (_bcm_common_rx_start(unit, cfg));
}

/*
 * Function:
 *      bcm_rx_stop
 * Purpose:
 *      Stop RX for the given unit; saves current configuration
 * Parameters:
 *      unit - The unit to stop
 *      cfg - OUT Configuration copied to this parameter
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This signals the thread to exit.
 */

int
bcm_esw_rx_stop(int unit, bcm_rx_cfg_t *cfg)
{
    return (_bcm_common_rx_stop(unit, cfg));
}


/*
 * Function:
 *      bcm_esw_rx_clear
 * Purpose:
 *      Clear all RX info
 * Returns:
 *      BCM_E_NONE
 */

int
bcm_esw_rx_clear(int unit)
{
    return (_bcm_common_rx_clear(unit));
}


int bcm_esw_rx_deinit(int unit)
{

#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        /* De-allocate space to store the copy-to-cpu
           config data */
        sal_free(_bcmi_rx_egr_drop_copy_to_cpu_config_data[unit]);
        /* De-allocate space to store the inuse bitmap
           for copy-to-cpu config */
        sal_free(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit]);
        /* Deinit the mutex */
        sal_mutex_destroy(_bcmi_rx_copy_to_cpu_config_mutex[unit]);
    }
#endif
    return _bcm_rx_shutdown(unit);
}

/*
 * Function:
 *      bcm_rx_cfg_get
 * Purpose:
 *      Check if init done; get the current RX configuration
 * Parameters:
 *      unit - Strata device ID
 *      cfg - OUT Configuration copied to this parameter.  May be NULL
 * Returns:
 *      BCM_E_INIT if not running on unit
 *      BCM_E_NONE if running on unit
 *      < 0 BCM_E_XXX error code
 * Notes:
 */

int
bcm_esw_rx_cfg_get(int unit, bcm_rx_cfg_t *cfg)
{
    return (_bcm_common_rx_cfg_get(unit, cfg));
}

/*
 * Function:
 *      bcm_rx_queue_register
 * Purpose:
 *      Register an application callback for the specified CPU queue
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      name - constant character string for debug purposes.
 *      cosq - CPU cos queue
 *      callback - callback function pointer.
 *      priority - priority of handler in list (0 is lowest priority).
 *      cookie - cookie passed to driver when packet arrives.
 *      flags - Register for interrupt or non-interrupt callback
 * Returns:
 *      BCM_E_NONE - callout registered.
 *      BCM_E_MEMORY - memory allocation failed.
 */

int
bcm_esw_rx_queue_register(int unit, const char *name, bcm_cos_queue_t cosq, 
                          bcm_rx_cb_f callback, uint8 priority, void *cookie, 
                          uint32 flags)
{
    return (_bcm_common_rx_queue_register(unit, name, cosq, callback,
                                       priority, cookie, flags));
}

/*
 * Function:
 *      bcm_rx_register
 * Purpose:
 *      Register an upper layer driver
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      chan - DMA channel number
 *      name - constant character string for debug purposes.
 *      priority - priority of handler in list (0 is lowest priority).
 *      f - function to call for that driver.
 *      cookie - cookie passed to driver when packet arrives.
 *      flags - Register for interrupt or non-interrupt callback
 * Returns:
 *      BCM_E_NONE - callout registered.
 *      BCM_E_MEMORY - memory allocation failed.
 * Notes:
 *      Refer bcm_rx_queue_register() if cosq is bigger than 16.
 */

int
bcm_esw_rx_register(int unit, const char *name, bcm_rx_cb_f callback,
                uint8 priority, void *cookie, uint32 flags)
{
    return (_bcm_common_rx_register(unit, name, callback,
                                    priority, cookie, flags));
}


/*
 * Function:
 *      bcm_rx_unregister
 * Purpose:
 *      De-register a callback function
 * Parameters:
 *      unit - Unit reference
 *      priority - Priority of registered callback
 *      callback - The function being registered
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Run through linked list looking for match of function and priority
 */

int
bcm_esw_rx_unregister(int unit, bcm_rx_cb_f callback, uint8 priority)
{
    return (_bcm_common_rx_unregister(unit, callback, priority));
}

/*
 * Function:
 *      bcm_rx_queue_unregister
 * Purpose:
 *      Unregister a callback function
 * Parameters:
 *      unit - Unit reference
 *      cosq - CPU cos queue
 *      priority - Priority of registered callback
 *      callback - The function being registered
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_rx_queue_unregister(int unit, bcm_cos_queue_t cosq,
                            bcm_rx_cb_f callback, uint8 priority)
{
    return (_bcm_common_rx_queue_unregister(unit, cosq, callback, priority));
}

/*
 * Function:
 *      bcm_rx_cosq_mapping_size_get
 * Purpose:
 *      Get number of COSQ mapping entries
 * Parameters:
 *      unit - Unit reference
 *      size - (OUT) number of entries
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_size_get(int unit, int *size)
{
    if (size == NULL) {
        return BCM_E_PARAM;
    }

    if (SOC_UNIT_VALID(unit) && SOC_MEM_IS_VALID(unit, CPU_COS_MAPm)) {
        *size = soc_mem_index_count(unit, CPU_COS_MAPm);
        return BCM_E_NONE;
    }

    *size = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_rx_olp_match_rule_set
 * Purpose:
 *      Set Olp Match Rule value in rx_ctl
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      olp_match_rule - Olp Match Rule Value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_rx_olp_match_rule_set (int unit, int olp_match_rule)
{
#if defined BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        return _bcm_common_rx_olp_match_rule_set (unit, olp_match_rule);
    }
#endif
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_esw_rx_olp_match_rule_get
 * Purpose:
 *      Get Olp Match Rule value in rx_ctl
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      olp_match_rule - Olp Match Rule Value.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_rx_olp_match_rule_get (int unit, int *olp_match_rule)
{
#if defined BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        return _bcm_common_rx_olp_match_rule_get (unit, olp_match_rule);
    }
#endif
    return BCM_E_UNAVAIL;
}



#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      _bcm_esw_rx_wb_scache_alloc
 * Purpose:
 *      Allocates scache space for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_rx_wb_scache_alloc(int unit)
{
    int                 rv = BCM_E_NONE;
    uint8               *rx_scache_ptr;
    uint32              rx_scache_size = 0;
    int                 stable_size = 0;
    soc_scache_handle_t scache_handle;

    BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* To store global_pps, max_burst and system_pps configuration */
    rx_scache_size += 3 * sizeof(int32);

    /* Store per queue PPS and Burst settings in L2 cache
       if harware rate limit is not supported */
    if (!soc_feature(unit, soc_feature_packet_rate_limit)) {
        /* Per cosq PPS, Burst and Max permitted packets configuration */
        rx_scache_size += (RX_QUEUE_MAX(unit) + 1) * (3 * sizeof(int32));
    } else {
        /* To store max permitted packets configuration */
        rx_scache_size += (RX_QUEUE_MAX(unit) + 1) * sizeof(int32);
    }
    /* OLP match rule for fp based oam */
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) &&
            soc_feature(unit, soc_feature_fp_based_oam)) {
        rx_scache_size += sizeof(RX_OLP_MATCH_RULE(unit));
    }
#endif

#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

        int size = 0;

        /* Allocate space to store the copy-to-cpu config data */
        size = soc_mem_index_count(unit, EP_CTC_RES_TABLEm) *
                          sizeof(_bcmi_rx_CopyToCpu_config_t);
        rx_scache_size += size;
        /* Allocate space to store the inuse bitmap for copy-to-cpu config */
        size = SHR_BITALLOCSIZE(
            soc_mem_index_count(unit, EP_CTC_RES_TABLEm));
        rx_scache_size += size;
    }
#endif
#if defined (BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit)) {
        /* Allocate space to store rx_ctl[unit]->flags */
        rx_scache_size += sizeof(rx_ctl[unit]->flags);
    }
#endif

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RX, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 rx_scache_size,
                                 &rx_scache_ptr,
                                 BCM_RX_WB_DEFAULT_VERSION, NULL);

    return rv;
}

/*
 * Function:
 *      bcm_esw_rx_recover
 * Purpose:
 *      Recover RX configuration from Hardware and L2 scache.
 * Parameters:
 *      unit - Unit reference
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_rx_recover(int unit)
{
   soc_scache_handle_t scache_handle;
   uint8               *rx_scache_ptr;
   rx_queue_t          *queue;
   bcm_cos_queue_t     cosq;
   int		           system_pps;
   int                 rv = BCM_E_NONE;
   uint16              recovered_ver;
   uint32              additional_scache_size = 0;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
   bcm_port_t          cpu_port;
   cpu_port = CMIC_PORT(unit);
#endif

   if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
       return BCM_E_NONE;
   }
   SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RX, 0);
   rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                0, &rx_scache_ptr,
                                BCM_RX_WB_DEFAULT_VERSION, &recovered_ver);
   if (rv == SOC_E_NOT_FOUND) {
       /* Use non-Level 2 case for recovery */
       /* and Allocate scache space for further syncs */
       BCM_IF_ERROR_RETURN(_bcm_esw_rx_wb_scache_alloc(unit));
   }  else if (SOC_FAILURE(rv)) {
       return rv;
   } else { /* Use Level 2 cache */

       sal_memcpy(&RX_PPS(unit), rx_scache_ptr, sizeof(RX_PPS(unit)));
       rx_scache_ptr += sizeof(RX_PPS(unit));

       sal_memcpy(&RX_BURST(unit), rx_scache_ptr, sizeof(RX_BURST(unit)));
       rx_scache_ptr += sizeof(RX_BURST(unit));

       sal_memcpy(&system_pps, rx_scache_ptr, sizeof(system_pps));
       BCM_IF_ERROR_RETURN(bcm_esw_rx_cpu_rate_set(unit, system_pps));
       rx_scache_ptr += sizeof(system_pps);

       for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
            queue = RX_QUEUE(unit, cosq);
            /* Retrieve PPS and Burst configuration of cpu cosq's from L2 
               cache if harware rate limit is not supported */
            if (!soc_feature(unit, soc_feature_packet_rate_limit)) {
                sal_memcpy(&queue->pps, rx_scache_ptr, sizeof(queue->pps));
                rx_scache_ptr += sizeof(queue->pps);
                sal_memcpy(&queue->burst, rx_scache_ptr, sizeof(queue->burst));
                rx_scache_ptr += sizeof(queue->burst);
            }
            sal_memcpy(&queue->max_len, rx_scache_ptr, sizeof(queue->max_len));
            if (cosq < RX_QUEUE_MAX(unit)) {
                rx_scache_ptr += sizeof(queue->max_len);
            }
       } 
       if (recovered_ver >= BCM_RX_WB_VERSION_1_1) {
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
           if (SOC_IS_TRIDENT2PLUS(unit) &&
                   soc_feature(unit, soc_feature_fp_based_oam)) {
               sal_memcpy(&RX_OLP_MATCH_RULE(unit), rx_scache_ptr, sizeof(RX_OLP_MATCH_RULE(unit)));
               rx_scache_ptr += sizeof(RX_OLP_MATCH_RULE(unit));
           }
#endif
       } else {
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
           if (SOC_IS_TRIDENT2PLUS(unit) &&
                   soc_feature(unit, soc_feature_fp_based_oam)) {

               additional_scache_size += sizeof(RX_OLP_MATCH_RULE(unit));
           }
#endif 
       }
       if (additional_scache_size > 0) {
           SOC_IF_ERROR_RETURN(soc_scache_realloc(unit, scache_handle,
                                                       additional_scache_size));
       }
   }

   /* Recover PPS and Burst settings from Hardware */
#if defined(BCM_TRX_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
   if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_TRIDENT_SUPPORT
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
      if (SOC_IS_TOMAHAWK3(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_th3_cosq_port_pps_get(unit, cpu_port,
                                           cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_th3_cosq_port_burst_get(unit, cpu_port,
                                          cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
      if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_th_cosq_port_pps_get(unit, cpu_port,
                                           cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_th_cosq_port_burst_get(unit, cpu_port,
                                          cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
      if (SOC_IS_HURRICANE4(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
              BCM_IF_ERROR_RETURN
                  (bcm_hr4_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq)));
              BCM_IF_ERROR_RETURN
                  (bcm_hr4_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq)));
          }
      } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
      if (SOC_IS_HELIX5(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
              BCM_IF_ERROR_RETURN
                  (bcm_hx5_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq)));
              BCM_IF_ERROR_RETURN
                  (bcm_hx5_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq)));
          }
      } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
      if (SOC_IS_MONTEREY(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               rv = bcm_mn_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
               rv = bcm_mn_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
          }
      } else
#endif
#ifdef BCM_TRIDENT3_SUPPORT
      if (SOC_IS_TRIDENT3X(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
              BCM_IF_ERROR_RETURN
                  (bcm_td3_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq)));
              BCM_IF_ERROR_RETURN
                  (bcm_td3_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq)));
          }
      } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
      if (SOC_IS_APACHE(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               rv = bcm_ap_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
               rv = bcm_ap_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
          }
      } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
      if (SOC_IS_FIREBOLT6(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
              BCM_IF_ERROR_RETURN
                  (bcm_fb6_cosq_port_pps_get(unit, cpu_port, cosq,
                                             &RX_COS_PPS(unit, cosq)));
              BCM_IF_ERROR_RETURN
                  (bcm_fb6_cosq_port_burst_get(unit, cpu_port, cosq,
                                               &RX_COS_BURST(unit, cosq)));
          }
      } else
#endif /* BCM_FIREBOLT6_SUPPORT */
      if (SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TRIDENT3X(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               rv = bcm_td2_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
               rv = bcm_td2_cosq_port_burst_get(unit, cpu_port,
                                           cosq, &RX_COS_BURST(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
          }
      }
#endif 
      if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_td_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_td_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq)));
          }
      } 
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
      if (SOC_IS_TRIUMPH3(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               rv = bcm_tr3_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq));
               /* cosq config may not be complete before warmboot. such
                * cases will return BCM_E_NOT_FOUND and needs to be ignored.
                */
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
               rv = bcm_tr3_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq));
               if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                   return rv;
               }
          }
      } 
#endif
#ifdef BCM_KATANA2_SUPPORT
      if (SOC_IS_KATANA2(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_kt2_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_kt2_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif
#ifdef BCM_KATANA_SUPPORT 
      if (SOC_IS_KATANA(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_kt_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_kt_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif
#ifdef BCM_TRIUMPH2_SUPPORT
      if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
          SOC_IS_VALKYRIE2(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_tr2_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_tr2_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif
#ifdef BCM_FIREBOLT2_SUPPORT
      if (SOC_IS_FIREBOLT2(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_fb2_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_fb2_cosq_port_burst_get(unit, cpu_port,
                                            cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif
#ifdef BCM_TRX_SUPPORT
      if (SOC_IS_TRIUMPH(unit) || SOC_IS_VALKYRIE(unit) || 
          SOC_IS_ENDURO(unit) || SOC_IS_SCORPION(unit)) {
          for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
               BCM_IF_ERROR_RETURN(bcm_tr_cosq_port_pps_get(unit, cpu_port,
                                            cosq, &RX_COS_PPS(unit, cosq)));
               BCM_IF_ERROR_RETURN(bcm_tr_cosq_port_burst_get(unit, cpu_port,
                                           cosq, &RX_COS_BURST(unit, cosq)));
          }
      }
#endif
   }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined (BCM_EP_REDIRECT_VERSION_2)
   /* Sync the inuse bitmap for copy-to-cpu config */
   if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

        int size = 0, table_count, i = 0;

        /* Get the table depth */
        table_count = soc_mem_index_count(unit, EP_CTC_RES_TABLEm);

        /* Copy over the indidivual values into s/w structs */
        for(i = 0; i < table_count; i++) {

            sal_memcpy(&BCMI_GET_COPY_TOCPU_FLAGS_GET(unit, i),
                        rx_scache_ptr,
                        BCMI_GET_COPY_TOCPU_FLAGS_SIZE_GET(unit));
            rx_scache_ptr += BCMI_GET_COPY_TOCPU_FLAGS_SIZE_GET(unit );

            sal_memcpy(&BCMI_GET_COPY_TOCPU_DROP_REASONS_GET(unit, i),
                        rx_scache_ptr,
                        BCMI_GET_COPY_TOCPU_DROP_REASONS_SIZE_GET(unit));
            rx_scache_ptr += BCMI_GET_COPY_TOCPU_DROP_REASONS_SIZE_GET(unit);

            sal_memcpy(&BCMI_GET_COPY_TOCPU_STRENGTH_GET(unit, i),
                        rx_scache_ptr,
                        BCMI_GET_COPY_TOCPU_STRENGTH_SIZE_GET(unit));
            rx_scache_ptr += BCMI_GET_COPY_TOCPU_STRENGTH_SIZE_GET(unit);

            sal_memcpy(&BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_GET(unit, i),
                        rx_scache_ptr,
                        BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SIZE_GET(unit));
            rx_scache_ptr += BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SIZE_GET(unit);

            sal_memcpy(&BCMI_GET_COPY_TOCPU_COS_Q_GET(unit, i),
                        rx_scache_ptr,
                        BCMI_GET_COPY_TOCPU_COS_Q_SIZE_GET(unit));
            rx_scache_ptr += BCMI_GET_COPY_TOCPU_COS_Q_SIZE_GET(unit);
        }

        /* Recover the copy-to-cpu inuse bitmap */
        size = SHR_BITALLOCSIZE(table_count);
        sal_memcpy(_bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], rx_scache_ptr, size);
        rx_scache_ptr += size;
   }
#endif
#if defined (BCM_KATANA_SUPPORT)
   if (SOC_IS_KATANAX(unit)) {
       if (recovered_ver >= BCM_RX_WB_VERSION_1_2) {
           sal_memcpy(&rx_ctl[unit]->flags, rx_scache_ptr, sizeof(rx_ctl[unit]->flags));
       }
   }
#endif

   return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_rx_sync
 * Purpose:
 *      Record rx module persisitent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */

int 
_bcm_esw_rx_sync(int unit) 
{
   soc_scache_handle_t scache_handle;
   uint8 *rx_scache_ptr;
   rx_queue_t *cpu_queue;
   int system_pps;
   int cosq;
	
   if (!RX_IS_LOCAL(unit)|| !SOC_UNIT_VALID(unit)) {
       return BCM_E_NONE;
   }
   if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
       return BCM_E_NONE;
   }
   SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_RX, 0);
   BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &rx_scache_ptr,
                                 BCM_RX_WB_DEFAULT_VERSION, NULL));

   /* global_pps configuration */
   sal_memcpy(rx_scache_ptr, &RX_PPS(unit), sizeof(RX_PPS(unit)));   
   rx_scache_ptr += sizeof(RX_PPS(unit));

   /* max_burst configuration */
   sal_memcpy(rx_scache_ptr, &RX_BURST(unit), sizeof(RX_BURST(unit)));
   rx_scache_ptr += sizeof(RX_BURST(unit));

   /* system_pps configuration */
   BCM_IF_ERROR_RETURN(bcm_esw_rx_cpu_rate_get(unit, &system_pps));
   sal_memcpy(rx_scache_ptr, &system_pps, sizeof(system_pps)); 
   rx_scache_ptr += sizeof(system_pps);

   for (cosq = 0; cosq <= RX_QUEUE_MAX(unit); cosq++) {
       cpu_queue = RX_QUEUE(unit, cosq);

       /* Store per queue PPS and Burst settings in L2 cache
           if harware rate limit is not supported */
       if (!soc_feature(unit, soc_feature_packet_rate_limit)) {
           sal_memcpy(rx_scache_ptr, &cpu_queue->pps, sizeof(cpu_queue->pps));
           rx_scache_ptr += sizeof(cpu_queue->pps);

           sal_memcpy(rx_scache_ptr, &cpu_queue->burst, sizeof(cpu_queue->burst));
           rx_scache_ptr += sizeof(cpu_queue->burst);
       }

       /* To store max permitted packets configuration */
       sal_memcpy(rx_scache_ptr, &cpu_queue->max_len, sizeof(cpu_queue->max_len));
       if (cosq < RX_QUEUE_MAX(unit)) {
           rx_scache_ptr += sizeof(cpu_queue->max_len);
       }
   }
#if defined (BCM_TRIDENT2PLUS_SUPPORT)
   /* save OLP match_rule value */
   if (SOC_IS_TRIDENT2PLUS(unit) &&
           soc_feature(unit, soc_feature_fp_based_oam)) {
       sal_memcpy(rx_scache_ptr, &RX_OLP_MATCH_RULE(unit), sizeof(RX_OLP_MATCH_RULE(unit)));
       rx_scache_ptr += sizeof(RX_OLP_MATCH_RULE(unit));
   }
#endif

#if defined (BCM_EP_REDIRECT_VERSION_2)
   /* Sync the inuse bitmap for copy-to-cpu config */
   if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

       int i = 0, table_count = 0, size = 0;

       /* Sync the copy-to-cpu config data */
       /* Get the table depth */
       table_count = soc_mem_index_count(unit, EP_CTC_RES_TABLEm);

       /* Sync over the indidivual values from s/w structs */
       for(i = 0; i < table_count; i++) {
           sal_memcpy(rx_scache_ptr,
                      &BCMI_GET_COPY_TOCPU_FLAGS_GET(unit, i),
                           BCMI_GET_COPY_TOCPU_FLAGS_SIZE_GET(unit));
           rx_scache_ptr += BCMI_GET_COPY_TOCPU_FLAGS_SIZE_GET(unit);

           sal_memcpy(rx_scache_ptr,
                      &BCMI_GET_COPY_TOCPU_DROP_REASONS_GET(unit, i),
                      BCMI_GET_COPY_TOCPU_DROP_REASONS_SIZE_GET(unit));
           rx_scache_ptr += BCMI_GET_COPY_TOCPU_DROP_REASONS_SIZE_GET(unit);

           sal_memcpy(rx_scache_ptr,
                      &BCMI_GET_COPY_TOCPU_STRENGTH_GET(unit, i),
                      BCMI_GET_COPY_TOCPU_STRENGTH_SIZE_GET(unit));
           rx_scache_ptr += BCMI_GET_COPY_TOCPU_STRENGTH_SIZE_GET(unit);

           sal_memcpy(rx_scache_ptr,
                      &BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_GET(unit, i),
                      BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SIZE_GET(unit));
           rx_scache_ptr += BCMI_GET_COPY_TOCPU_BUFFER_PRIORITY_SIZE_GET(unit);

           sal_memcpy(rx_scache_ptr,
                      &BCMI_GET_COPY_TOCPU_COS_Q_GET(unit, i),
                      BCMI_GET_COPY_TOCPU_COS_Q_SIZE_GET(unit));
           rx_scache_ptr += BCMI_GET_COPY_TOCPU_COS_Q_SIZE_GET(unit);

       }

       /* Sync the copy-to-cpu inuse bitmap */
       size = SHR_BITALLOCSIZE(table_count);
       sal_memcpy(rx_scache_ptr, _bcmi_rx_egr_drop_copy_to_cpu_loc_bitmap[unit], size);
       rx_scache_ptr += size;
   }
#endif
#if defined (BCM_KATANA_SUPPORT)
   if (SOC_IS_KATANAX(unit)) {
       /* Clear BCM_RX_F_STARTED before sync. Need to restart CPU rx after warmboot */
       rx_ctl[unit]->flags &= ~BCM_RX_F_STARTED;
       sal_memcpy(rx_scache_ptr, &rx_ctl[unit]->flags, sizeof(rx_ctl[unit]->flags));
   }
#endif
  return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_TRX_SUPPORT

/* CPU_COS_MAP_KEY format for TRX */
#define _bcm_scorpion_rx_cpu_cosmap_key_max             39
#define _bcm_triumph_rx_cpu_cosmap_key_max              44
#define _bcm_triumph2_rx_cpu_cosmap_key_max             59
#define _bcm_trident_rx_cpu_cosmap_key_max              60
#define _bcm_katana_rx_cpu_cosmap_key_max               62
#define _bcm_enduro_rx_cpu_cosmap_key_max               50
#define _bcm_triumph_rx_cpu_max_cosq                    48
#define _bcm_trident2_rx_cpu_max_cosq                   48
#define _bcm_tomahawk3_rx_cpu_max_cosq                  48
#define _bcm_katana2_rx_cpu_max_cosq                    48
#define _bcm_triumph3_rx_cpu_max_cosq                   45
#define _bcm_triumph3_rx_cpu_max_cosq_a0_a1             48
#define _bcm_scorpion_rx_cpu_max_cosq                   32
#define _bcm_hurricane_rx_cpu_max_cosq                   8
#define _bcm_tr3_rx_ip_cpu_cosmap_key_max               90
#define _bcm_tr3_rx_ep_cpu_cosmap_key_max               15
#define _bcm_tr3_rx_nlf_cpu_cosmap_key_max               7
#define _bcm_tr3_rx_cpu_cosmap_key_overlays              3
#define _bcm_tr3_rx_cpu_cosmap_key_words                 3
#define _bcm_tr3_rx_cpu_cosmap_reason_type_mask        0x3
#define _bcm_rx_cpu_cosmap_key_words_max                 3 /* For now */



static bcm_rx_reason_t 
_bcm_trx_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */

    /* Below are not available for Scorpion */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */

    /* Below are not available Triumph */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonWlanSlowpathKeepalive,   /* CAPWAP_KEEPALIVE */
    bcmRxReasonWlanClientError,         /* WLAN_CLIENT_DATABASE_ERROR */
    bcmRxReasonWlanDot1xDrop,           /* WLAN_DOT1X_DROP */
    bcmRxReasonWlanSlowpath,            /* WLAN_CAPWAP_SLOWPATH */
    bcmRxReasonEncapHigigError,         /* EHG_NONHG */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonIpfixRateViolation       /* IPFIX_FLOW */
};

#ifdef BCM_TOMAHAWK3_SUPPORT
static bcm_rx_reason_t
_bcm_th3_cpu_cos_map_key [] =
{
    bcmRxReasonTunnelControl,           /* 0: AMT_CONTROL_PKT */
    bcmRxReasonBfd,                     /* 1: BFD_ERROR */
    bcmRxReasonBfdSlowpath,             /* 2: BFD_SLOWPATH */
    bcmRxReasonClassBasedMove,          /* 3:CBSM_PREVENTED */
    bcmRxReasonDLBMonitor,              /* 4:DLB_MONITOR_TO_CPU */
    bcmRxReasonL2DestMiss,              /* 5:CPU_DLF */
    bcmRxReasonDosAttack,               /* 6:CPU_DOS_ATTACK */
    bcmRxReasonETrapMonitor,            /* 7:ETRAP_MONITOR_TO_CPU */
    bcmRxReasonFilterMatch,             /* 8:CPU_FFP */
    bcmRxReasonInvalid,                 /* 9:RESERVED Remote CPU*/
    bcmRxReasonInvalid,                 /* 10:RESERVED */
    bcmRxReasonInvalid,                 /* 11:RESERVED */
    bcmRxReasonInvalid,                 /* 12:RESERVED */
    bcmRxReasonInvalid,                 /* 13:RESERVED */
    bcmRxReasonInvalid,                 /* 14:RESERVED */
    bcmRxReasonInvalid,                 /* 15:RESERVED */
    bcmRxReasonInvalid,                 /* 16:RESERVED */
    bcmRxReasonInvalid,                 /* 17:RESERVED */
    bcmRxReasonHigigHdrError,           /* 18:HGHDR_ERROR */
    bcmRxReasonClassTagPackets,         /* 19:HG_HDR_TYPE1 */
    bcmRxReasonIcmpRedirect,            /* 20:ICMP_REDIRECT */
    bcmRxReasonTimesyncUnknownVersion,  /* 21:IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonReserved0,               /* 22:INSTRUMENT_OFFSET */
    bcmRxReasonIntHopLimit,             /* 23:IXME_KP:INT_HOP_LIMIT_TO_CPU */
    bcmRxReasonIntTurnAround,           /* 24:INT_TURNAROUND_TO_CPU */
    bcmRxReasonIpmcInterfaceMismatch,   /* 25:CPU_IPMC_INTERFACE_MISMATCH */
    bcmRxReasonIpMcastMiss,             /* 26:CPU_IPMC_MISS */
    bcmRxReasonL3Slowpath,              /* 27:IP_OPTIONS_PKT. */
    bcmRxReasonL2Cpu,                   /* 28:CPU_L2CPU */
    bcmRxReasonL2GreSipMiss,            /* 29:L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* 30:L2GRE_VPNID_MISS */
    bcmRxReasonL2Move,                  /* 31:CPU_L2MOVE */
    bcmRxReasonL3DestMiss,              /* 32:CPU_L3DST_MISS */
    bcmRxReasonL3HeaderError,           /* 33:CPU_L3HDR_ERR */
    bcmRxReasonL3SourceMiss,            /* 34:CPU_L3SRC_MISS. */
    bcmRxReasonL3SourceMove,            /* 35:CPU_L3SRC_MOVE */
    bcmRxReasonL3MtuFail,               /* 36:L3_MTU_CHECK_FAIL */
    bcmRxReasonL3AddrBindFail,          /* 37:MAC_BIND_FAIL */
    bcmRxReasonMartianAddr,             /* 38:CPU_MARTIAN_ADDR */
    bcmRxReasonMcastIdxError,           /* 39:MCIDX_ERROR */
    bcmRxReasonMcastMiss,               /* 40:CPU_MC_MISS */
    bcmRxReasonHigigControl,            /* 41:MH_CONTROL */
    bcmRxReasonMplsRouterAlertLabel,    /* 42:MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* 43:MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonMplsInvalidAction,       /* 44:MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* 45:MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsLabelMiss,           /* 46:MPLS_LABEL_MISS */
    bcmRxReasonMplsSequenceNumber,      /* 47:MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsTtl,                 /* 48:MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsUnknownAch,          /* 49:MPLS_UNKNOWN_ACH_ERROR */
    bcmRxReasonStation,                 /* 50:MY_STATION */
    bcmRxReasonNat,                     /* 51:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNat,                     /* 52:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNat,                     /* 53:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNhop,                    /* 54:NHOP_OFFSET */
    bcmRxReasonOAMError,                /* 55:OAM_ERROR */
    bcmRxReasonOAMSlowpath,             /* 56:OAM_SLOWPATH */
#if 1
    bcmRxReasonInvalid,                 /* 57:NOT DEFINED */
#endif
    bcmRxReasonParityError,             /* 58:PARITY_ERROR */
    bcmRxReasonL2NonUnicastMiss,        /* 59:PBT_NONUC_PKT */
    bcmRxReasonArp,                     /* 60:PROTOCOL_ARP */
    bcmRxReasonBpdu,                    /* 61:PROTOCOL_BPDU */
    bcmRxReasonDhcp,                    /* 62:PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* 63:PROTOCOL_IGMP */
    bcmRxReasonIpmcReserved,            /* 64:PROTOCOL_IPMC_RSVD */
    bcmRxReasonL2Marked,                /* 65:PROTOCOL_L2_PKT */
    bcmRxReasonMmrp,                    /* 66:PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* 67:PROTOCOL_SRP */
    bcmRxReasonSharedVlanMismatch,      /* 68:PVLAN_MISMATCH */
    bcmRxReasonSampleDest,              /* 69:CPU_SFLOW_DST */
    bcmRxReasonSampleSourceFlex,        /* 70:CPU_SFLOW_FLEX */
    bcmRxReasonSampleSource,            /* 71:CPU_SFLOW_SRC */
    bcmRxReasonL2SourceMiss,            /* 72:CPU_SLF */
    bcmRxReasonTimeSync,                /* 73:TIME_SYNC_PKT */
    bcmRxReasonTtl1,                    /* 74:TTL_1 */
    bcmRxReasonTunnelDecapEcnError,     /* 75:TUNNEL_DECAP_ECN_ERR */
    bcmRxReasonTunnelError,             /* 76:CPU_TUNNEL_ERR */
    bcmRxReasonUrpfFail,                /* 77:URPF_FAILED */
    bcmRxReasonUnknownVlan,             /* 78:CPU_UVLAN. */
    bcmRxReasonVlanFilterMatch,         /* 79:VFP */
    bcmRxReasonVlanTranslate,           /* 80:VXLT_MISS */
};
#endif

#ifdef BCM_ENDURO_SUPPORT
static bcm_rx_reason_t 
_bcm_enduro_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonL2Marked                 /* PROTOCOL_L2_PKT */
};
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_HURRICANE2_SUPPORT
static bcm_rx_reason_t
_bcm_hurricane2_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonL3AddrBindFail          /* MAC_BIND_FAIL */
};
#endif /* BCM_HURRICANE2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
static bcm_rx_reason_t 
_bcm_trident_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                     /* 3 bits */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonTrill,                   /* TRILL_DROP_REASON_ENCODING */
    bcmRxReasonTrill,                   /*   -> */
    bcmRxReasonTrill                    /* 3 bits */
};

/* From FORMAT TRILL_CPU_OPCODE_ENCODING */
static bcm_rx_reason_t
_bcm_trill_cpu_opcode_encoding[] =
{
    bcmRxReasonTrill,            /* 0:NO_ERRORS
                                  * Base field, must match the entries above */
    bcmRxReasonTrillInvalid,     /* 1:TRILL_HDR_ERROR */
    bcmRxReasonTrillMiss,        /* 2:TRILL_LOOKUP_MISS */
    bcmRxReasonTrillRpfFail,     /* 3:TRILL_RPF_CHECK_FAIL */
    bcmRxReasonTrillSlowpath,    /* 4:TRILL_SLOWPATH */
    bcmRxReasonTrillCoreIsIs,    /* 5:TRILL_CORE_IS_IS_PKT */
    bcmRxReasonTrillTtl          /* 6:TRILL_HOP_COUNT_CHECK_FAIL */
};

#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
/* From FORMAT NIV_CPU_OPCODE_ENCODING */
static bcm_rx_reason_t
_bcm_niv_cpu_opcode_encoding[] =
{
    bcmRxReasonNiv,              /* 0: NO_ERRORS
                                  * Base field, must match the entries above */
    bcmRxReasonNivPrioDrop,      /* 1:DOT1P_ADMITTANCE_DISCARD */
    bcmRxReasonNivInterfaceMiss, /* 2:VIF_LOOKUP_MISS */
    bcmRxReasonNivRpfFail,       /* 3:RPF_LOOKUP_MISS */
    bcmRxReasonNivTagInvalid,    /* 4:VNTAG_FORMAT_ERROR */
    bcmRxReasonNivTagDrop,       /* 5:VNTAG_PRESENT_DROP */
    bcmRxReasonNivUntagDrop      /* 6:VNTAG_NOT_PRESENT_DROP */
};

#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
/* From FORMAT CPU_COS_MAP_KEY (exclude common bits (bit 0 to 6) */
static bcm_rx_reason_t
_bcm_trident2_cpu_cos_map_key[] =
{
    bcmRxReasonBpdu,                    /* 07:PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* 08:PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* 09:PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* 10:PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* 11:PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* 12:CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* 13:PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* 14:CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* 15:PARITY_ERROR */
    bcmRxReasonHigigControl,            /* 16:MH_CONTROL */
    bcmRxReasonTtl1,                    /* 17:TTL_1 */
    bcmRxReasonL3Slowpath,              /* 18:IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* 19:CPU_SLF */
    bcmRxReasonL2DestMiss,              /* 20:CPU_DLF */
    bcmRxReasonL2Move,                  /* 21:CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* 22:CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* 23:PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* 24:CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* 25:CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* 26:CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* 27:CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* 28:CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* 29:CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* 30:CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* 31:CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* 32:HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* 33:MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* 34:VFP */
    bcmRxReasonClassBasedMove,          /* 35:CBSM_PREVENTED */
    bcmRxReasonCongestionCnm,           /* 36:ICNM */
    bcmRxReasonE2eHolIbp,               /* 37:E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* 38:HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* 39:NHOP */
    bcmRxReasonUrpfFail,                /* 40:URPF_FAILED */
    bcmRxReasonFilterMatch,             /* 41:CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* 42:ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* 43:CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* 44:CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* 45:L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* 46:MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* 47:MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* 48:MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* 49:MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* 50:MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsUnknownAch,          /* 51:MPLS_UNKNOWN_ACH_ERROR */
    bcmRxReasonMmrp,                    /* 52:PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* 53:PROTOCOL_SRP */
    bcmRxReasonTimesyncUnknownVersion,  /* 54:IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonMplsRouterAlertLabel,    /* 55:MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* 56:MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonVlanTranslate,           /* 57:VXLT_MISS */
    bcmRxReasonTunnelControl,           /* 58:AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* 59:TIME_SYNC */
    bcmRxReasonOAMSlowpath,             /* 60:OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* 61:OAM_ERROR */
    bcmRxReasonL2Marked,                /* 62:PROTOCOL_L2_PKT */
    bcmRxReasonL3AddrBindFail,          /* 63:MAC_BIND_FAIL */
    bcmRxReasonStation,                 /* 64:MY_STATION */
    bcmRxReasonNiv,                     /* 65:NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /* 66:  -> */
    bcmRxReasonNiv,                     /* 67:3 bits */
    bcmRxReasonTrill,                   /* 68:TRILL_DROP_REASON_ENCODING */
    bcmRxReasonTrill,                   /* 69:  -> */
    bcmRxReasonTrill,                   /* 70:3 bits */
    bcmRxReasonL2GreSipMiss,            /* 71:L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* 72:L2GRE_VPNID_MISS */
    bcmRxReasonBfdSlowpath,             /* 73:BFD_SLOWPATH */
    bcmRxReasonBfd,                     /* 74:BFD_ERROR */
    bcmRxReasonOAMLMDM,                 /* 75:OAM_LMDM */
    bcmRxReasonCongestionCnmProxy,      /* 76:QCN_CNM_PRP */
    bcmRxReasonCongestionCnmProxyError, /* 77:QCN_CNM_PRP_DLF */
    bcmRxReasonVxlanSipMiss,            /* 78:VXLAN_SIP_MISS */
    bcmRxReasonVxlanVpnIdMiss,          /* 79:VXLAN_VN_ID_MISS */
    bcmRxReasonFcoeZoneCheckFail,       /* 80:FCOE_ZONE_CHECK_FAIL */
    bcmRxReasonNat,                     /* 81:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNat,                     /* 82:  -> */
    bcmRxReasonNat,                     /* 83:3 bits */
    bcmRxReasonIpmcInterfaceMismatch    /* 84:CPU_IPMC_INTERFACE_MISMATCH */
};

/* From FORMAT NAT_CPU_OPCODE_ENCODING */
static bcm_rx_reason_t
_bcm_nat_cpu_opcode_encoding[] =
{
    bcmRxReasonNat,              /* 0:NOP
                                  * Base field, must match the entries above */
    bcmRxReasonTcpUdpNatMiss,    /* 1:NORMAL */
    bcmRxReasonIcmpNatMiss,      /* 2:ICMP */
    bcmRxReasonNatFragment,      /* 3:FRAGMEMT */
    bcmRxReasonNatMiss           /* 4:OTHER */
};

static bcm_rx_reason_t
/* From FORMAT TRILL_CPU_OPCODE_ENCODING */
_bcm_trident2_trill_cpu_opcode_encoding[] =
{
    bcmRxReasonTrill,            /* 0:NO_ERRORS
                                  * Base field, must match the entries above */
    bcmRxReasonTrillInvalid,     /* 1:TRILL_HDR_ERROR */
    bcmRxReasonTrillMiss,        /* 2:TRILL_LOOKUP_MISS */
    bcmRxReasonTrillRpfFail,     /* 3:TRILL_RPF_CHECK_FAIL */
    bcmRxReasonTrillSlowpath,    /* 4:TRILL_SLOWPATH */
    bcmRxReasonTrillCoreIsIs,    /* 5:TRILL_CORE_IS_IS_PKT */
    bcmRxReasonTrillTtl,         /* 6:TRILL_HOP_COUNT_CHECK_FAIL */
    bcmRxReasonTrillName         /* 7:NICKNAME_TABLE_COPYTOCPU */
};
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
/* From FORMAT CPU_COS_MAP_KEY (exclude common bits (bit 0 to 6) */
static bcm_rx_reason_t
_bcm_tomahawk_cpu_cos_map_key[] =
{
    bcmRxReasonBpdu,                    /* 07:PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* 08:PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* 09:PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* 10:PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* 11:PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* 12:CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* 13:PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* 14:CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* 15:PARITY_ERROR */
    bcmRxReasonHigigControl,            /* 16:MH_CONTROL */
    bcmRxReasonTtl1,                    /* 17:TTL_1 */
    bcmRxReasonL3Slowpath,              /* 18:IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* 19:CPU_SLF */
    bcmRxReasonL2DestMiss,              /* 20:CPU_DLF */
    bcmRxReasonL2Move,                  /* 21:CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* 22:CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* 23:PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* 24:CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* 25:CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* 26:CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* 27:CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* 28:CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* 29:CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* 30:CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* 31:CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* 32:HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* 33:MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* 34:VFP */
    bcmRxReasonClassBasedMove,          /* 35:CBSM_PREVENTED */
    bcmRxReasonInvalid,                 /* 36:Reserved */
    bcmRxReasonTunnelDecapEcnError,     /* 37:TUNNEL_DECAP_ECN_ERR */
    bcmRxReasonClassTagPackets,         /* 38:HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* 39:NHOP */
    bcmRxReasonUrpfFail,                /* 40:URPF_FAILED */
    bcmRxReasonFilterMatch,             /* 41:CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* 42:ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* 43:CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* 44:CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* 45:L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* 46:MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* 47:MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* 48:MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* 49:MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* 50:MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsUnknownAch,          /* 51:MPLS_UNKNOWN_ACH_ERROR */
    bcmRxReasonMmrp,                    /* 52:PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* 53:PROTOCOL_SRP */
    bcmRxReasonTimesyncUnknownVersion,  /* 54:IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonMplsRouterAlertLabel,    /* 55:MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* 56:MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonVlanTranslate,           /* 57:VXLT_MISS */
    bcmRxReasonTunnelControl,           /* 58:AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* 59:TIME_SYNC */
    bcmRxReasonOAMSlowpath,             /* 60:OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* 61:OAM_ERROR */
    bcmRxReasonL2Marked,                /* 62:PROTOCOL_L2_PKT */
    bcmRxReasonL3AddrBindFail,          /* 63:MAC_BIND_FAIL */
    bcmRxReasonStation,                 /* 64:MY_STATION */
    bcmRxReasonNiv,                     /* 65:NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /* 66:  -> */
    bcmRxReasonNiv,                     /* 67:3 bits */
    bcmRxReasonTrill,                   /* 68:TRILL_DROP_REASON_ENCODING */
    bcmRxReasonTrill,                   /* 69:  -> */
    bcmRxReasonTrill,                   /* 70:3 bits */
    bcmRxReasonL2GreSipMiss,            /* 71:L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* 72:L2GRE_VPNID_MISS */
    bcmRxReasonBfdSlowpath,             /* 73:BFD_SLOWPATH */
    bcmRxReasonBfd,                     /* 74:BFD_ERROR */
    bcmRxReasonOAMLMDM,                 /* 75:OAM_LMDM */
    bcmRxReasonInvalid,                 /* 76:Reserved */
    bcmRxReasonInvalid,                 /* 77:Reserved */
    bcmRxReasonVxlanSipMiss,            /* 78:VXLAN_SIP_MISS */
    bcmRxReasonVxlanVpnIdMiss,          /* 79:VXLAN_VN_ID_MISS */
    bcmRxReasonFcoeZoneCheckFail,       /* 80:FCOE_ZONE_CHECK_FAIL */
    bcmRxReasonNat,                     /* 81:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNat,                     /* 82:  -> */
    bcmRxReasonNat,                     /* 83:3 bits */
    bcmRxReasonIpmcInterfaceMismatch,   /* 84:CPU_IPMC_INTERFACE_MISMATCH */
    bcmRxReasonSampleSourceFlex         /* 85:CPU_SFLOW_FLEX */
};
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
/* From FORMAT CPU_COS_MAP_KEY (exclude common bits (bit 0 to 6) */
static bcm_rx_reason_t
_bcm_trident2plus_cpu_cos_map_key[] =
{
    bcmRxReasonBpdu,                    /* 07:PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* 08:PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* 09:PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* 10:PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* 11:PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* 12:CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* 13:PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* 14:CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* 15:PARITY_ERROR */
    bcmRxReasonHigigControl,            /* 16:MH_CONTROL */
    bcmRxReasonTtl1,                    /* 17:TTL_1 */
    bcmRxReasonL3Slowpath,              /* 18:IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* 19:CPU_SLF */
    bcmRxReasonL2DestMiss,              /* 20:CPU_DLF */
    bcmRxReasonL2Move,                  /* 21:CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* 22:CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* 23:PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* 24:CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* 25:CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* 26:CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* 27:CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* 28:CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* 29:CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* 30:CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* 31:CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* 32:HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* 33:MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* 34:VFP */
    bcmRxReasonClassBasedMove,          /* 35:CBSM_PREVENTED */
    bcmRxReasonCongestionCnm,           /* 36:ICNM */
    bcmRxReasonE2eHolIbp,               /* 37:E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* 38:HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* 39:NHOP */
    bcmRxReasonUrpfFail,                /* 40:URPF_FAILED */
    bcmRxReasonFilterMatch,             /* 41:CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* 42:ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* 43:CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* 44:CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* 45:L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* 46:MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* 47:MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* 48:MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* 49:MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* 50:MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsUnknownAch,          /* 51:MPLS_UNKNOWN_ACH_ERROR */
    bcmRxReasonMmrp,                    /* 52:PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* 53:PROTOCOL_SRP */
    bcmRxReasonTimesyncUnknownVersion,  /* 54:IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonMplsRouterAlertLabel,    /* 55:MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* 56:MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonVlanTranslate,           /* 57:VXLT_MISS */
    bcmRxReasonTunnelControl,           /* 58:AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* 59:TIME_SYNC */
    bcmRxReasonOAMSlowpath,             /* 60:OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* 61:OAM_ERROR */
    bcmRxReasonL2Marked,                /* 62:PROTOCOL_L2_PKT */
    bcmRxReasonL3AddrBindFail,          /* 63:MAC_BIND_FAIL */
    bcmRxReasonStation,                 /* 64:MY_STATION */
    bcmRxReasonNiv,                     /* 65:NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /* 66:  -> */
    bcmRxReasonNiv,                     /* 67:3 bits */
    bcmRxReasonTrill,                   /* 68:TRILL_DROP_REASON_ENCODING */
    bcmRxReasonTrill,                   /* 69:  -> */
    bcmRxReasonTrill,                   /* 70:3 bits */
    bcmRxReasonL2GreSipMiss,            /* 71:L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* 72:L2GRE_VPNID_MISS */
    bcmRxReasonBfdSlowpath,             /* 73:BFD_SLOWPATH */
    bcmRxReasonBfd,                     /* 74:BFD_ERROR */
    bcmRxReasonOAMLMDM,                 /* 75:OAM_LMDM */
    bcmRxReasonCongestionCnmProxy,      /* 76:QCN_CNM_PRP */
    bcmRxReasonCongestionCnmProxyError, /* 77:QCN_CNM_PRP_DLF */
    bcmRxReasonVxlanSipMiss,            /* 78:VXLAN_SIP_MISS */
    bcmRxReasonVxlanVpnIdMiss,          /* 79:VXLAN_VN_ID_MISS */
    bcmRxReasonFcoeZoneCheckFail,       /* 80:FCOE_ZONE_CHECK_FAIL */
    bcmRxReasonNat,                     /* 81:NAT_DROP_REASON_ENCODING */
    bcmRxReasonNat,                     /* 82:  -> */
    bcmRxReasonNat,                     /* 83:3 bits */
    bcmRxReasonIpmcInterfaceMismatch,   /* 84:CPU_IPMC_INTERFACE_MISMATCH */
    bcmRxReasonFailoverDrop,            /* 85:PROTECTION_DATA_DROP_OFFSET */
    bcmRxReasonEncapHigigError,         /* 86:NON_EMBEDDED_HG_OFFSET */
#if 1
    bcmRxReasonInvalid                  /* 87:NOT DEFINED */
#endif
};

static bcm_rx_reason_t
/* From FORMAT NIV_CPU_OPCODE_ENCODING */
_bcm_trident2plus_niv_cpu_opcode_encoding[] =
{
    bcmRxReasonNiv,              /* 0: NO_ERRORS
                                  * Base field, must match the entries above */
    bcmRxReasonNivPrioDrop,      /* 1:DOT1P_ADMITTANCE_DISCARD */
    bcmRxReasonNivInterfaceMiss, /* 2:VIF_LOOKUP_MISS */
    bcmRxReasonNivRpfFail,       /* 3:RPF_LOOKUP_MISS */
    bcmRxReasonNivTagInvalid,    /* 4:VNTAG_FORMAT_ERROR */
    bcmRxReasonNivTagDrop,       /* 5:VNTAG_PRESENT_DROP */
    bcmRxReasonNivUntagDrop,     /* 6:VNTAG_NOT_PRESENT_DROP */
    bcmRxReasonUnknownSubtendingPort /* 7: UNKNOWN_SUBTENDING_PORT */
};

static bcm_rx_reason_t
/* From FORMAT NAT_CPU_OPCODE_ENCODING */
_bcm_trident2plus_nat_cpu_opcode_encoding[] =
{
    bcmRxReasonNat,              /* 0:NOP
                                  * Base field, must match the entries above */
    bcmRxReasonTcpUdpNatMiss,    /* 1:NORMAL */
    bcmRxReasonIcmpNatMiss,      /* 2:ICMP */
    bcmRxReasonNatFragment,      /* 3:FRAGMEMT */
    bcmRxReasonNatMiss,          /* 4:OTHER */
    bcmRxReasonInvalid,          /* 5:RESERVED */
    bcmRxReasonInvalid,          /* 6:RESERVED */
#if 1
    bcmRxReasonInvalid           /* 7:NOT DEFINED */
#endif
};
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
/* From FORMAT CPU_COS_MAP_KEY (exclude common bits (bit 0 to 6) */
static bcm_rx_reason_t
_bcm_trident3_cpu_cos_map_key[] =
{
    bcmRxReasonBpdu,                    /* 07:PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* 08:PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* 09:PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* 10:PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* 11:PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* 12:CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* 13:PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* 14:CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* 15:PARITY_ERROR */
    bcmRxReasonHigigControl,            /* 16:MH_CONTROL */
    bcmRxReasonTtl1,                    /* 17:TTL_1 */
    bcmRxReasonL3Slowpath,              /* 18:IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* 19:CPU_SLF */
    bcmRxReasonL2DestMiss,              /* 20:CPU_DLF */
    bcmRxReasonL2Move,                  /* 21:CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* 22:CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* 23:PBT_NONUC_PKT */
    bcmRxReasonInvalid,                 /* 24:RESERVED */
    bcmRxReasonInvalid,                 /* 25:RESERVED */
    bcmRxReasonL3SourceMove,            /* 26:CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* 27:CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* 28:CPU_IPMC_MISS */
    bcmRxReasonL3HeaderMismatch,        /* 29:CPU_L3HDR_MISMATCH */
    bcmRxReasonMartianAddr,             /* 30:CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* 31:CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* 32:HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* 33:MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* 34:VFP */
    bcmRxReasonClassBasedMove,          /* 35:CBSM_PREVENTED */
    bcmRxReasonInvalid,                 /* 36:RESERVED */
    bcmRxReasonTunnelDecapEcnError,     /* 37:TUNNEL_DECAP_ECN_ERROR */
    bcmRxReasonClassTagPackets,         /* 38:HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* 39:NHOP */
    bcmRxReasonUrpfFail,                /* 40:URPF_FAILED */
    bcmRxReasonFilterMatch,             /* 41:CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* 42:ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* 43:CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* 44:CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* 45:L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* 46:MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* 47:MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* 48:MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsControlPacket,       /* 49:MPLS_CONTROL_PKT */
    bcmRxReasonMplsSequenceNumber,      /* 50:MPLS_SEQ_NUM_FAIL */
    bcmRxReasonInvalidTpid,             /* 51:INVALID_TPID */
    bcmRxReasonInvalid,                 /* 52:RESERVED */
    bcmRxReasonInvalid,                 /* 53:RESERVED */
    bcmRxReasonTimesyncUnknownVersion,  /* 54:IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonMplsRouterAlertLabel,    /* 55:MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* 56:MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonPacketFlowSelectMiss,    /* 57:FLOW_SELECT_MISS */
    bcmRxReasonTimeSync,                /* 58:TIME_SYNC */
    bcmRxReasonL2Marked,                /* 59:PROTOCOL_L2_PKT */
    bcmRxReasonL3AddrBindFail,          /* 60:MAC_BIND_FAIL */
    bcmRxReasonStation,                 /* 61:MY_STATION */
    bcmRxReasonNivPrioDrop,             /* 62:DOT1P_ADMITTANCE_DISCARD */
    bcmRxReasonNivRpfFail,              /* 63:RPF_LOOKUP_MISS */
    bcmRxReasonUnknownSubtendingPort,   /* 64:UNKOWN SUBTENDING PORT */
    bcmRxReasonL3Cpu,                   /* 65:L3CPU */
    bcmRxReasonReserved0,               /* 66:INSTRUMENT */
    bcmRxReasonTunnelObjectValidationFail, /*67:TUNNEL_OBJECT_VALIDATION_FAIL */
    bcmRxReasonTunnelTtlError,          /* 68:TUNNEL_TTL_CHECK_FAIL */
    bcmRxReasonL3HeaderError,           /* 69:L3_HDR_ERROR */
    bcmRxReasonL2HeaderError,           /* 70:L2_HDR_ERROR */
    bcmRxReasonTtl,                     /* 71:TTL */
    bcmRxReasonFcoeZoneCheckFail,       /* 72:FCOE_ZONE_CHECK_FAIL */
    bcmRxReasonNat,                     /* 73:NAT_ERROR */
    bcmRxReasonInvalid,                 /* 74:RESERVED */
    bcmRxReasonInvalid,                 /* 75:RESERVED */
    bcmRxReasonIpmcInterfaceMismatch,   /* 76:CPU_IPMC_INTERFACE_MISMATCH */
    bcmRxReasonSampleSourceFlex,        /* 77:CPU_SFLOW_FLEX */
    bcmRxReasonInvalid,                 /* 78:RESERVED */
    bcmRxReasonInvalid,                 /* 79:RESERVED */
    bcmRxReasonInvalid,                 /* 80:RESERVED */
    bcmRxReasonInvalid,                 /* 81:RESERVED */
    bcmRxReasonInvalid,                 /* 82:RESERVED */
    bcmRxReasonInvalid,                 /* 83:RESERVED */
    bcmRxReasonInvalid,                 /* 84:RESERVED */
    bcmRxReasonInvalid,                 /* 85:RESERVED */
    bcmRxReasonInvalid,                 /* 86:RESERVED */
#if 1
    bcmRxReasonInvalid,                 /* 87:NOT DEFINED */
#endif
    bcmRxReasonL2SrcLookupMiss,         /* 88:FORWARDING_L2_SOURCE_LOOKUP_MISS_OFFSET */
    bcmRxReasonInvalid,                 /* 89:RESERVED */
    bcmRxReasonL2DstLookupMiss,         /* 90:FORWARDING_L2_DEST_LOOKUP_MISS_OFFSET */
    bcmRxReasonInvalid,                 /* 91:RESERVED */
    bcmRxReasonInvalid,                 /* 92:RESERVED */
    bcmRxReasonL3SrcHostLookupMiss,     /* 93:FORWARDING_L3_SOURCE_HOST_LOOKUP_MISS_OFFSET */
    bcmRxReasonInvalid,                 /* 94:RESERVED */
    bcmRxReasonInvalid,                 /* 95:RESERVED */
    bcmRxReasonInvalid,                 /* 96:RESERVED */
    bcmRxReasonL3DstHostLookupMiss,     /* 97:FORWARDING_L3_DEST_HOST_LOOKUP_MISS_OFFSET */
    bcmRxReasonInvalid,                 /* 98:RESERVED */
    bcmRxReasonInvalid,                 /* 99:RESERVED */
    bcmRxReasonVlanTranslate1Lookup1Miss, /*100:TUNNEL_ADAPT_1_LOOKUP_1_OFFSET */
    bcmRxReasonVlanTranslate1Lookup2Miss, /*101:TUNNEL_ADAPT_1_LOOKUP_2_OFFSET */
    bcmRxReasonMplsLookup1Miss,           /*102:TUNNEL_ADAPT_2_LOOKUP_1_OFFSET */
    bcmRxReasonMplsLookup2Miss,           /*103:TUNNEL_ADAPT_2_LOOKUP_2_OFFSET */
    bcmRxReasonL3TunnelLookupMiss,        /*104:TUNNEL_ADAPT_3_LOOKUP_1_OFFSET */
    bcmRxReasonVlanTranslate2Lookup1Miss, /*105:TUNNEL_ADAPT_4_LOOKUP_1_OFFSET */
    bcmRxReasonVlanTranslate2Lookup2Miss, /*106:TUNNEL_ADAPT_4_LOOKUP_2_OFFSET */
    bcmRxReasonPacketFlowSelect,        /* 107:PKT_FLOW_SELECT */
    bcmRxReasonFailoverDrop             /* 108:PROTECTION_DATA_DROP_OFFSET */
};

#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
static bcm_rx_reason_t 
_bcm_katana_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                     /* 3 bits */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonInvalid,                 /* RESERVED_0 */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonBfd,                     /* BFD_ERROR_ENCODING */
    bcmRxReasonBfdUnknownVersion
};

static bcm_rx_reason_t
_bcm_bfd_cpu_opcode_encoding[] =
{
    bcmRxReasonBfd,        /* Base field, must match the entries above */
    bcmRxReasonBfdUnknownVersion,
    bcmRxReasonBfdInvalidVersion,
    /* bcmRxReasonBfdLookupFailure, use special case to handle */
    bcmRxReasonBfdInvalidPacket
};
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
static bcm_rx_reason_t 
_bcm_katana2_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                     /* 3 bits */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonInvalid,                 /* RESERVED_0 */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonBfd,                     /* BFD_ERROR_ENCODING */
    bcmRxReasonBfdUnknownVersion,
    bcmRxReasonInvalid,                 /* UNKNOWN_SUBTENDING_PORT */
    bcmRxReasonOAMCCMSlowpath,          /* OAM_CCM */
    bcmRxReasonInvalid,                 /* LLTAG_PRESENT */
    bcmRxReasonInvalid,                 /* LLTAG_NOT_PRESENT */
    bcmRxReasonInvalid,                 /* ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    bcmRxReasonInvalid,                 /* NIV_RPF_LOOKUP_MISS */
    bcmRxReasonInvalid,                 /* NIV_VNTAG_PRESENT */
    bcmRxReasonInvalid,                 /* NIV_VNTAG_NOT_PRESENT */
    bcmRxReasonInvalid,                 /* MPLS_OUT_OF_LOOKUPS */
    bcmRxReasonBHHOAM,                  /* BHH_OAM_PACKET */
    bcmRxReasonMplsRouterAlertLabel,    /* MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* MPLS_ILLEGAL_RESERVED_LABEL */
};
#endif

#ifdef BCM_SABER2_SUPPORT
static bcm_rx_reason_t 
_bcm_saber2_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                     /* 3 bits */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonInvalid,                 /* RESERVED_0 */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonBfd,                     /* BFD_ERROR_ENCODING */
    bcmRxReasonBfdUnknownVersion,
    bcmRxReasonInvalid,                 /* UNKNOWN_SUBTENDING_PORT */
    bcmRxReasonOAMCCMSlowpath,          /* OAM_CCM */
    bcmRxReasonInvalid,                 /* LLTAG_PRESENT */
    bcmRxReasonInvalid,                 /* LLTAG_NOT_PRESENT */
    bcmRxReasonInvalid,                 /* ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    bcmRxReasonInvalid,                 /* NIV_RPF_LOOKUP_MISS */
    bcmRxReasonInvalid,                 /* NIV_VNTAG_PRESENT */
    bcmRxReasonInvalid,                 /* NIV_VNTAG_NOT_PRESENT */
    bcmRxReasonInvalid,                 /* MPLS_OUT_OF_LOOKUPS */
    bcmRxReasonBHHOAM,                  /* BHH_OAM_PACKET */
    bcmRxReasonMplsRouterAlertLabel,    /* MPLS_ALERT_LABEL */
    bcmRxReasonMplsIllegalReservedLabel,/* MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonInvalid,                 /* SAT_DN_SAMP_RX */
    bcmRxReasonOAMMplsLmDm,             /* MPLS_LMDM_OAM_PKT */
};
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT


static bcm_rx_reason_t 
_bcm_triumph3_ip_cpu_cos_map_key [] =
{
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonInvalid,                 /* L3_SLOWPATH - duplicate */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonIpfixRateViolation,      /* IPFIX_FLOW */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonEncapHigigError,         /* EHG_NONHG */
    bcmRxReasonRegexMatch,              /* FLOW_TRACKER */
    bcmRxReasonBfd,                     /* BFD_ERROR */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonFailoverDrop,            /* PROTECTION_DROP_DATA */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonNivUntagDrop,            /* NIV_UNTAG_DROP */
    bcmRxReasonNivTagDrop,              /* NIV_TAG_DROP */
    bcmRxReasonNivTagInvalid,           /* NIV_TAG_INVALID */
    bcmRxReasonNivRpfFail,              /* NIV_RPF_FAIL */
    bcmRxReasonNivInterfaceMiss,        /* NIV_INTERFACE_MISS */
    bcmRxReasonNivPrioDrop,             /* NIV_PRIO_DROP */
    bcmRxReasonTrillName,               /* NICKNAME_TABLE_COPYTOCPU */
    bcmRxReasonTrillTtl,                /* TRILL_HOP_COUNT_CHECK_FAIL */
    bcmRxReasonTrillCoreIsIs,           /* TRILL_CORE_IS_IS_PKT */
    bcmRxReasonTrillSlowpath,           /* TRILL_SLOWPATH */
    bcmRxReasonTrillRpfFail,            /* TRILL_RPF_CHECK_FAIL */
    bcmRxReasonTrillMiss,               /* TRILL_LOOKUP_MISS */
    bcmRxReasonTrillInvalid,            /* TRILL_HDR_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonWlanSlowpathKeepalive,   /* CAPWAP_KEEPALIVE */
    bcmRxReasonWlanTunnelError,         /* WLAN_SHIM_HEADER_ERROR_TO_CPU */
    bcmRxReasonWlanSlowpath,            /* WLAN_CAPWAP_SLOWPATH */
    bcmRxReasonWlanDot1xDrop,           /* WLAN_DOT1X_DROP */
    bcmRxReasonMplsReservedEntropyLabel, /* ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    bcmRxReasonCongestionCnmProxy,      /* QCN_CNM_PRP */
    bcmRxReasonCongestionCnmProxyError, /* QCN_CNM_PRP_DLF */
    bcmRxReasonMplsUnknownAch,          /* MPLS_UNKNOWN_ACH_TYPE */
    bcmRxReasonMplsLookupsExceeded,     /* MPLS_OUT_OF_LOOKUPS */
    bcmRxReasonMplsIllegalReservedLabel, /* MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonMplsRouterAlertLabel,    /* MPLS_ALERT_LABEL */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonCongestionCnm,           /* ICNM */
    bcmRxReasonL2GreSipMiss,            /* L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* L2GRE_VPNID_MISS */
};

static bcm_rx_reason_t 
_bcm_triumph3_ep_cpu_cos_map_key [] =
{
    bcmRxReasonUnknownVlan,             /* CPUE_VLAN */
    bcmRxReasonStp,                     /* CPUE_STG */
    bcmRxReasonVlanTranslate,           /* CPUE_VXLT */
    bcmRxReasonTunnelError,             /* CPUE_TUNNEL */
    bcmRxReasonIpmc,                    /* CPUE_L3ERR */
    bcmRxReasonL3HeaderError,           /* CPUE_L3PKT_ERR */
    bcmRxReasonTtl,                     /* CPUE_TTL_DROP */
    bcmRxReasonL2MtuFail,               /* CPUE_MTU */
    bcmRxReasonHigigHdrError,           /* CPUE_HIGIG */
    bcmRxReasonSplitHorizon,            /* CPUE_PRUNE */
    bcmRxReasonNivPrune,                /* CPUE_NIV_DISCARD */
    bcmRxReasonVirtualPortPrune,        /* CPUE_SPLIT_HORIZON */
    bcmRxReasonInvalid,                 /* CPUE_EFP */
    bcmRxReasonNonUnicastDrop,          /* CPUE_MULTI_DEST */
    bcmRxReasonTrillPacketPortMismatch, /* CPUE_TRILL */
    bcmRxReasonInvalid,                 /* Reserved */
    bcmRxReasonInvalid,                 /* Reserved */
    bcmRxReasonInvalid,                 /* Reserved */
    bcmRxReasonInvalid,                 /* Reserved */
    bcmRxReasonInvalid,                 /* Reserved */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonInvalid,                 /* L3_SLOWPATH - duplicate */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonIpfixRateViolation,      /* IPFIX_FLOW */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonEncapHigigError,         /* EHG_NONHG */
    bcmRxReasonRegexMatch,              /* FLOW_TRACKER */
    bcmRxReasonBfd,                     /* BFD_ERROR */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonFailoverDrop,            /* PROTECTION_DROP_DATA */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonNivUntagDrop,            /* NIV_UNTAG_DROP */
    bcmRxReasonNivTagDrop,              /* NIV_TAG_DROP */
    bcmRxReasonNivTagInvalid,           /* NIV_TAG_INVALID */
    bcmRxReasonNivRpfFail,              /* NIV_RPF_FAIL */
    bcmRxReasonNivInterfaceMiss,        /* NIV_INTERFACE_MISS */
    bcmRxReasonNivPrioDrop,             /* NIV_PRIO_DROP */
    bcmRxReasonTrillName,               /* NICKNAME_TABLE_COPYTOCPU */
    bcmRxReasonTrillTtl,                /* TRILL_HOP_COUNT_CHECK_FAIL */
    bcmRxReasonTrillCoreIsIs,           /* TRILL_CORE_IS_IS_PKT */
    bcmRxReasonTrillSlowpath,           /* TRILL_SLOWPATH */
    bcmRxReasonTrillRpfFail,            /* TRILL_RPF_CHECK_FAIL */
    bcmRxReasonTrillMiss,               /* TRILL_LOOKUP_MISS */
    bcmRxReasonTrillInvalid,            /* TRILL_HDR_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonWlanSlowpathKeepalive,   /* CAPWAP_KEEPALIVE */
    bcmRxReasonWlanTunnelError,         /* WLAN_SHIM_HEADER_ERROR_TO_CPU */
    bcmRxReasonWlanSlowpath,            /* WLAN_CAPWAP_SLOWPATH */
    bcmRxReasonWlanDot1xDrop,           /* WLAN_DOT1X_DROP */
    bcmRxReasonMplsReservedEntropyLabel, /* ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    bcmRxReasonCongestionCnmProxy,      /* QCN_CNM_PRP */
    bcmRxReasonCongestionCnmProxyError, /* QCN_CNM_PRP_DLF */
    bcmRxReasonMplsUnknownAch,          /* MPLS_UNKNOWN_ACH_TYPE */
    bcmRxReasonMplsLookupsExceeded,     /* MPLS_OUT_OF_LOOKUPS */
    bcmRxReasonMplsIllegalReservedLabel, /* MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonMplsRouterAlertLabel,    /* MPLS_ALERT_LABEL */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonCongestionCnm,           /* ICNM */
    bcmRxReasonL2GreSipMiss,            /* L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* L2GRE_VPNID_MISS */
};

static bcm_rx_reason_t 
_bcm_triumph3_nlf_cpu_cos_map_key [] =
{
    bcmRxReasonRegexAction,              /* SM_TOCPU */
    bcmRxReasonWlanClientMove,          /* WLAN_MOVE_TOCPU */
    bcmRxReasonWlanSourcePortMiss,      /* WLAN_SVP_MISS_TOCPU */
    bcmRxReasonWlanClientError,         /* WLAN_DATABASE_ERROR */
    bcmRxReasonWlanClientSourceMiss,    /* WLAN_CLIENT_DATABASE_SA_MISS */
    bcmRxReasonWlanClientDestMiss,      /* WLAN_CLIENT_DATABASE_DA_MISS */
    bcmRxReasonWlanMtu,                 /* WLAN_MTU_FAIL */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonMmrp,                    /* PROTOCOL_MMRP */
    bcmRxReasonSrp,                     /* PROTOCOL_SRP */
    bcmRxReasonTunnelControl,           /* AMT_CONTROL_PKT */
    bcmRxReasonL2Marked,                /* PROTOCOL_L2_PKT */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonInvalid,                 /* L3_SLOWPATH - duplicate */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonStation,                 /* MY_STATION */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonIpfixRateViolation,      /* IPFIX_FLOW */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonEncapHigigError,         /* EHG_NONHG */
    bcmRxReasonRegexMatch,              /* FLOW_TRACKER */
    bcmRxReasonBfd,                     /* BFD_ERROR */
    bcmRxReasonBfdSlowpath,             /* BFD_SLOWPATH */
    bcmRxReasonFailoverDrop,            /* PROTECTION_DROP_DATA */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonNivUntagDrop,            /* NIV_UNTAG_DROP */
    bcmRxReasonNivTagDrop,              /* NIV_TAG_DROP */
    bcmRxReasonNivTagInvalid,           /* NIV_TAG_INVALID */
    bcmRxReasonNivRpfFail,              /* NIV_RPF_FAIL */
    bcmRxReasonNivInterfaceMiss,        /* NIV_INTERFACE_MISS */
    bcmRxReasonNivPrioDrop,             /* NIV_PRIO_DROP */
    bcmRxReasonTrillName,               /* NICKNAME_TABLE_COPYTOCPU */
    bcmRxReasonTrillTtl,                /* TRILL_HOP_COUNT_CHECK_FAIL */
    bcmRxReasonTrillCoreIsIs,           /* TRILL_CORE_IS_IS_PKT */
    bcmRxReasonTrillSlowpath,           /* TRILL_SLOWPATH */
    bcmRxReasonTrillRpfFail,            /* TRILL_RPF_CHECK_FAIL */
    bcmRxReasonTrillMiss,               /* TRILL_LOOKUP_MISS */
    bcmRxReasonTrillInvalid,            /* TRILL_HDR_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonWlanSlowpathKeepalive,   /* CAPWAP_KEEPALIVE */
    bcmRxReasonWlanTunnelError,         /* WLAN_SHIM_HEADER_ERROR_TO_CPU */
    bcmRxReasonWlanSlowpath,            /* WLAN_CAPWAP_SLOWPATH */
    bcmRxReasonWlanDot1xDrop,           /* WLAN_DOT1X_DROP */
    bcmRxReasonMplsReservedEntropyLabel, /* ENTROPY_LABEL_IN_UNALLOWED_RANGE */
    bcmRxReasonCongestionCnmProxy,      /* QCN_CNM_PRP */
    bcmRxReasonCongestionCnmProxyError, /* QCN_CNM_PRP_DLF */
    bcmRxReasonMplsUnknownAch,          /* MPLS_UNKNOWN_ACH_TYPE */
    bcmRxReasonMplsLookupsExceeded,     /* MPLS_OUT_OF_LOOKUPS */
    bcmRxReasonMplsIllegalReservedLabel, /* MPLS_ILLEGAL_RESERVED_LABEL */
    bcmRxReasonMplsRouterAlertLabel,    /* MPLS_ALERT_LABEL */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonCongestionCnm,           /* ICNM */
    bcmRxReasonL2GreSipMiss,            /* L2GRE_SIP_MISS */
    bcmRxReasonL2GreVpnIdMiss,          /* L2GRE_VPNID_MISS */
};

static bcm_rx_reason_t *_bcm_tr3_cpu_cos_map_overlays[_bcm_tr3_rx_cpu_cosmap_key_overlays] = {
    _bcm_triumph3_ip_cpu_cos_map_key,
    _bcm_triumph3_ep_cpu_cos_map_key,
    _bcm_triumph3_nlf_cpu_cos_map_key,
};

static uint32 _bcm_tr3_cpu_cos_map_maxs[_bcm_tr3_rx_cpu_cosmap_key_overlays] = {
    _bcm_tr3_rx_ip_cpu_cosmap_key_max,
    _bcm_tr3_rx_ep_cpu_cosmap_key_max,
    _bcm_tr3_rx_nlf_cpu_cosmap_key_max,
};

#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_GREYHOUND_SUPPORT
static bcm_rx_reason_t 
_bcm_greyhound_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonL2Marked,                 /* PROTOCOL_L2_PKT */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                      /* 3 bits */
};
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
static bcm_rx_reason_t
_bcm_greyhound2_cpu_cos_map_key [] =
{
    bcmRxReasonBpdu,                    /* PROTOCOL_BPDU */
    bcmRxReasonIpmcReserved,            /* PROTOCOL_IPMC_RSVD */
    bcmRxReasonDhcp,                    /* PROTOCOL_DHCP */
    bcmRxReasonIgmp,                    /* PROTOCOL_IGMP */
    bcmRxReasonArp,                     /* PROTOCOL_ARP */
    bcmRxReasonUnknownVlan,             /* CPU_UVLAN. */
    bcmRxReasonSharedVlanMismatch,      /* PVLAN_MISMATCH */
    bcmRxReasonDosAttack,               /* CPU_DOS_ATTACK */
    bcmRxReasonParityError,             /* PARITY_ERROR */
    bcmRxReasonHigigControl,            /* MH_CONTROL */
    bcmRxReasonTtl1,                    /* TTL_1 */
    bcmRxReasonL3Slowpath,              /* IP_OPTIONS_PKT. */
    bcmRxReasonL2SourceMiss,            /* CPU_SLF */
    bcmRxReasonL2DestMiss,              /* CPU_DLF */
    bcmRxReasonL2Move,                  /* CPU_L2MOVE */
    bcmRxReasonL2Cpu,                   /* CPU_L2CPU */
    bcmRxReasonL2NonUnicastMiss,        /* PBT_NONUC_PKT */
    bcmRxReasonL3SourceMiss,            /* CPU_L3SRC_MISS. */
    bcmRxReasonL3DestMiss,              /* CPU_L3DST_MISS */
    bcmRxReasonL3SourceMove,            /* CPU_L3SRC_MOVE */
    bcmRxReasonMcastMiss,               /* CPU_MC_MISS */
    bcmRxReasonIpMcastMiss,             /* CPU_IPMC_MISS */
    bcmRxReasonL3HeaderError,           /* CPU_L3HDR_ERR */
    bcmRxReasonMartianAddr,             /* CPU_MARTIAN_ADDR */
    bcmRxReasonTunnelError,             /* CPU_TUNNEL_ERR */
    bcmRxReasonHigigHdrError,           /* HGHDR_ERROR */
    bcmRxReasonMcastIdxError,           /* MCIDX_ERROR */
    bcmRxReasonVlanFilterMatch,         /* VFP */
    bcmRxReasonClassBasedMove,          /* CBSM_PREVENTED */
    bcmRxReasonL2LearnLimit,            /* MAC_LIMIT */
    bcmRxReasonE2eHolIbp,               /* E2E_HOL_IBP */
    bcmRxReasonClassTagPackets,         /* HG_HDR_TYPE1 */
    bcmRxReasonNhop,                    /* NHOP */
    bcmRxReasonUrpfFail,                /* URPF_FAILED */
    bcmRxReasonFilterMatch,             /* CPU_FFP */
    bcmRxReasonIcmpRedirect,            /* ICMP_REDIRECT */
    bcmRxReasonSampleSource,            /* CPU_SFLOW_SRC */
    bcmRxReasonSampleDest,              /* CPU_SFLOW_DST */
    bcmRxReasonL3MtuFail,               /* L3_MTU_CHECK_FAIL */
    bcmRxReasonMplsLabelMiss,           /* MPLS_LABEL_MISS */
    bcmRxReasonMplsInvalidAction,       /* MPLS_INVALID_ACTION */
    bcmRxReasonMplsInvalidPayload,      /* MPLS_INVALID_PAYLOAD */
    bcmRxReasonMplsTtl,                 /* MPLS_TTL_CHECK_FAIL */
    bcmRxReasonMplsSequenceNumber,      /* MPLS_SEQ_NUM_FAIL */
    bcmRxReasonMplsCtrlWordError,       /* MPLS_CW_TYPE_NOT_ZERO */
    bcmRxReasonTimeSync,                /* TIME_SYNC_PKT */
    bcmRxReasonOAMSlowpath,             /* OAM_SLOWPATH */
    bcmRxReasonOAMError,                /* OAM_ERROR */
    bcmRxReasonOAMLMDM,                 /* OAM_LMDM */
    bcmRxReasonL2Marked,                 /* PROTOCOL_L2_PKT */
    bcmRxReasonTimesyncUnknownVersion,  /* IEEE1588_UNKNOWN_VERSION */
    bcmRxReasonL3AddrBindFail,          /* MAC_BIND_FAIL */
    bcmRxReasonVlanTranslate,           /* VXLT_MISS */
    bcmRxReasonNiv,                     /* NIV_DROP_REASON_ENCODING */
    bcmRxReasonNiv,                     /*   -> */
    bcmRxReasonNiv,                     /* 3 bits */
    bcmRxReasonStation,                 /* MY_STATION_PKT_OFFSET */
    bcmRxReasonL2StuFail,               /* STU_CHECK_FAIL_OFFSET */
    bcmRxReasonSrCounterExceeded,       /* SR_COUNTER_THRESHOLD_EXCEEDED_OFFSET */
    bcmRxReasonSrCopyToCpuBit0,         /* USER_DEFINED_CPU_OFFSET */
    bcmRxReasonSrCopyToCpuBit1,         /*   -> */
    bcmRxReasonSrCopyToCpuBit2,         /*   -> */
    bcmRxReasonSrCopyToCpuBit3,         /*   -> */
    bcmRxReasonSrCopyToCpuBit4,         /*   -> */
    bcmRxReasonSrCopyToCpuBit5,         /* 6 bits */
};
#endif /* BCM_GREYHOUND2_SUPPORT */


#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
/*
 * Function:
 *      _bcm_esw_rcmr_overlay_get
 * Purpose:
 *      Get all supported reasons for overlayed CPU cosq mapping 
 * Parameters:
 *      unit - Unit reference
 *      reasons - cpu cosq "reasons" mapping bitmap
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_rcmr_overlay_get(int unit, bcm_rx_reasons_t * reasons)
{
    uint32          sz, *sz_overlays;
    uint32          ix, ovx;
    bcm_rx_reason_t *cpu_cos_map_key, **cpu_cos_map_overlays;

    if (SOC_IS_TRIUMPH3(unit)) {
        cpu_cos_map_overlays = _bcm_tr3_cpu_cos_map_overlays;
        sz_overlays = _bcm_tr3_cpu_cos_map_maxs;
    } else {
        /* If the feature check passes, we should have an implementation */
        return BCM_E_INTERNAL;
    }

    for (ovx = 0; ovx < _bcm_tr3_rx_cpu_cosmap_key_overlays; ovx++) {
        cpu_cos_map_key = cpu_cos_map_overlays[ovx];
        sz = sz_overlays[ovx];
        for (ix = 0; ix < sz; ix++) {
            /* coverity [mixed_enums] */
             if (cpu_cos_map_key[ix] != (bcm_rx_reason_t) bcmRxReasonInvalid) {
                 BCM_RX_REASON_SET(*reasons, cpu_cos_map_key[ix]);
             }
        }
    }
    return BCM_E_NONE;
}

static soc_field_t _bcm_tr3_reason_fields[_bcm_tr3_rx_cpu_cosmap_key_words] = {
    REASONS_KEY_LOWf,
    REASONS_KEY_MIDf,
    REASONS_KEY_HIGHf,
};

static soc_field_t _bcm_tr3_mask_fields[_bcm_tr3_rx_cpu_cosmap_key_words] = {
    REASONS_MASK_LOWf,
    REASONS_MASK_MIDf,
    REASONS_MASK_HIGHf,
};

STATIC int
_bcm_tr3_rx_cosq_mapping_get(int unit, int index,
                            bcm_rx_reasons_t *reasons, 
                            bcm_rx_reasons_t *reasons_mask,
                            uint8 *prio, uint8 *prio_mask,
                            uint32 *packet_type, uint32 * packet_type_mask,
                            bcm_cos_queue_t *cosq)
{
    cpu_cos_map_entry_t entry; 
    uint32 key[_bcm_rx_cpu_cosmap_key_words_max];
    uint32 mask[_bcm_rx_cpu_cosmap_key_words_max];
    uint32 maskwordlen[_bcm_rx_cpu_cosmap_key_words_max];
    uint32 keywordlen[_bcm_rx_cpu_cosmap_key_words_max];
    uint32 ix, ovx, word, words, key_len, mask_len, cur_len;
    uint32 sw_pkt_type_mask, sw_pkt_type_key;
    uint32 reason_type_mask, reason_type_key;
    uint32 maskbitset, keybitset;
    soc_field_t *reason_fields, *mask_fields;
    bcm_rx_reason_t *cpu_cos_map_key, **cpu_cos_map_overlays;

    if (SOC_IS_TRIUMPH3(unit)) {
        cpu_cos_map_overlays = _bcm_tr3_cpu_cos_map_overlays;
        reason_fields = _bcm_tr3_reason_fields;
        mask_fields = _bcm_tr3_mask_fields;
        words = _bcm_tr3_rx_cpu_cosmap_key_words;
    } else {
        
        return BCM_E_INTERNAL;
    }

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* NULL pointer check */
    if (reasons == NULL  || reasons_mask == NULL  || 
        prio == NULL || prio_mask == NULL || 
        packet_type == NULL || packet_type_mask == NULL || 
        cosq == NULL) {
        return BCM_E_PARAM;
    }
 
    /* Read the entry */
    SOC_IF_ERROR_RETURN
        (READ_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, index, &entry));

    /* Return BCM_E_NOT_FOUND if invalid */
    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, VALIDf) == 0) {
        return (BCM_E_NOT_FOUND);
    }        

    sal_memset(reasons, 0, sizeof(bcm_rx_reasons_t));
    sal_memset(reasons_mask, 0, sizeof(bcm_rx_reasons_t));

    *cosq = (bcm_cos_queue_t)
              soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, COSf);
    
    sw_pkt_type_mask = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, SW_PKT_TYPE_MASKf);
    sw_pkt_type_key  = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, SW_PKT_TYPE_KEYf);

    if (sw_pkt_type_mask == 0 && sw_pkt_type_key == 0) {
        /* all packets matched */
        *packet_type_mask = 0;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 0) {
        /* Only non-switched packets */
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED | 
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 3) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                        BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
    }
 
    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, MIRR_PKT_MASKf)) {
        *packet_type_mask |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }

    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, MIRR_PKT_KEYf)) {
        *packet_type |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }

    reason_type_mask = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, REASON_CODE_TYPE_MASKf);
    reason_type_key  = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, REASON_CODE_TYPE_KEYf);

    if ((0 != reason_type_mask) && 
        (_bcm_tr3_rx_cpu_cosmap_reason_type_mask != reason_type_mask)) {
        /* This should be set to exact match or all */
        return BCM_E_INTERNAL;
    }

    switch (reason_type_key) {
    case 0:
        ovx = 0; /* IP overlay */
        break;
    case 2:
        ovx = 1; /* EP overlay */
        break;
    case 3:
        ovx = 2; /* NLF overlay */
        break;
    default:
        /* Not a valid choice on current devices */
        return BCM_E_INTERNAL;
    }
    cpu_cos_map_key = cpu_cos_map_overlays[ovx];

    *prio_mask = 
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, INT_PRI_MASKf);
    *prio = 
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, INT_PRI_KEYf);

    mask_len = key_len = 0;
    for (word = 0; word < words; word++) {
        mask[word] = 
            soc_mem_field32_get(unit, CPU_COS_MAPm, &entry,
                                mask_fields[word]);
        maskwordlen[word] =
            soc_mem_field_length(unit, CPU_COS_MAPm, mask_fields[word]);
        mask_len += maskwordlen[word];

        key[word] = 
            soc_mem_field32_get(unit, CPU_COS_MAPm, &entry,
                                reason_fields[word]);
        keywordlen[word] =
            soc_mem_field_length(unit, CPU_COS_MAPm, reason_fields[word]);
        key_len += keywordlen[word];

        if (keywordlen[word] != maskwordlen[word]) {
            /* FIXME: The regsfile has a very serious issue! */
            return BCM_E_INTERNAL;
        }
    }

    if (key_len != mask_len) {
        /* FIXME: The regsfile has a very serious issue! */
        return BCM_E_INTERNAL;
    }

    cur_len = word = 0;
    for (ix = 0; ix < key_len; ix++) {
        if (ix == (cur_len + keywordlen[word])) {
            /* Start of new word, advance values */
            cur_len += keywordlen[word];
            word++;
        }

        keybitset = key[word] & (1 << (ix - cur_len));
        maskbitset = mask[word] & (1 << (ix - cur_len));

        if (maskbitset) {
            BCM_RX_REASON_SET(*reasons_mask, cpu_cos_map_key[ix]);
        }
        if (keybitset) {
            BCM_RX_REASON_SET(*reasons, cpu_cos_map_key[ix]);
        }
    }

    return BCM_E_NONE;
}

int
_bcm_tr3_rx_cosq_mapping_set(int unit, int index,
                            bcm_rx_reasons_t reasons, bcm_rx_reasons_t
                            reasons_mask,
                            uint8 int_prio, uint8 int_prio_mask,
                            uint32 packet_type, uint32 packet_type_mask,
                            bcm_cos_queue_t cosq)
{
    bcm_rx_reason_t  ridx;
    bcm_rx_reasons_t reasons_remain;
    uint32 key[_bcm_tr3_rx_cpu_cosmap_key_words];
    uint32 mask[_bcm_tr3_rx_cpu_cosmap_key_words];
    uint32 maskwordlen[_bcm_tr3_rx_cpu_cosmap_key_words];
    uint32 keywordlen[_bcm_tr3_rx_cpu_cosmap_key_words];
    uint32 word, words, key_len, mask_len, cur_len;
    int32 ovx, ovx_max;
    int reason_set;
    cpu_cos_map_entry_t entry; 
    uint8 sw_pkt_type_key = 0;
    uint8 sw_pkt_type_mask = 0;
    uint32 reason_type_mask = 0;
    uint32 reason_type_key = 0;
    uint32 bit;
    soc_field_t *reason_fields, *mask_fields;
    bcm_rx_reason_t *cpu_cos_map_key, **cpu_cos_map_overlays;
    int match_count[_bcm_tr3_rx_cpu_cosmap_key_overlays], max_match, match_ovx;
    int max_overlay_index = 20;

    if (SOC_IS_TRIUMPH3(unit)) {
        cpu_cos_map_overlays = _bcm_tr3_cpu_cos_map_overlays;
        reason_fields = _bcm_tr3_reason_fields;
        mask_fields = _bcm_tr3_mask_fields;
        words = _bcm_tr3_rx_cpu_cosmap_key_words;
        ovx_max = _bcm_tr3_rx_cpu_cosmap_key_overlays - 1;
    } else {
        
        return BCM_E_INTERNAL;
    }

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* Verify the cosq */
    if (SOC_IS_TRIUMPH3(unit) &&
        soc_feature(unit, soc_feature_cmic_reserved_queues) && 
        cosq >= _bcm_triumph3_rx_cpu_max_cosq) {
        return BCM_E_PARAM;
    }

    if (SOC_IS_TRIUMPH3(unit) &&
        !soc_feature(unit, soc_feature_cmic_reserved_queues) && 
        cosq >= _bcm_triumph3_rx_cpu_max_cosq_a0_a1) {
        return BCM_E_PARAM;
    }

    if (SOC_IS_HELIX4(unit) && 
        cosq >= _bcm_trident2_rx_cpu_max_cosq) {
        return BCM_E_PARAM;
    }

    /* Verify the packet type */

    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_key |= 1;
    }
    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_key |= 2;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_mask |= 1;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_mask |= 2;
    }
    sw_pkt_type_key &= sw_pkt_type_mask;

    if ((sw_pkt_type_mask == 0x1) || 
        ((sw_pkt_type_mask != 0) && (sw_pkt_type_key == 0x1))) { 
        /* Hw doesn't support these cases */
        return BCM_E_PARAM;
    }

    BCM_RX_REASON_CLEAR(reasons_mask, bcmRxReasonInvalid);
    reasons_remain = reasons_mask;

    mask_len = key_len = 0;
    for (word = 0; word < words; word++) {
        mask[word] = 0;
        maskwordlen[word] =
            soc_mem_field_length(unit, CPU_COS_MAPm, mask_fields[word]);
        mask_len += maskwordlen[word];

        key[word] = 0;
        keywordlen[word] =
            soc_mem_field_length(unit, CPU_COS_MAPm, reason_fields[word]);
        key_len += keywordlen[word];

        if (keywordlen[word] != maskwordlen[word]) {
            /* FIXME: The regsfile has a very serious issue! */
            return BCM_E_INTERNAL;
        }
    }

    if (key_len != mask_len) {
        /* FIXME: The regsfile has a very serious issue! */
        return BCM_E_INTERNAL;
    }

    /* Since all overlays are same from max_overlay_index onwards,
       traverse till max_overlay_index to find Max Matching overlay */
    max_match = 0;
    match_ovx = 0;
    for (ovx = ovx_max; ovx >= 0; ovx--) {
        cpu_cos_map_key = cpu_cos_map_overlays[ovx];
        match_count[ovx] = 0;
        for (bit =0; bit < max_overlay_index; bit++) {
            ridx  = cpu_cos_map_key[bit];
            if (BCM_RX_REASON_GET(reasons_mask, ridx)) {
                match_count[ovx]++;
            }            
        }
        if (max_match <= match_count[ovx]) {
            max_match = match_count[ovx];
            match_ovx = ovx;
        }
    }

    ovx = match_ovx;
    cpu_cos_map_key = cpu_cos_map_overlays[ovx];
    reason_set = FALSE;
            bit = 0;
    /* Run through entire selcted overlay */
    for (; bit < _bcm_tr3_rx_ip_cpu_cosmap_key_max; bit++) {
            /* Find reason being set */
            ridx  = cpu_cos_map_key[bit];

        if (ridx == bcmRxReasonInvalid) {
            continue;
        }
            if (!BCM_RX_REASON_GET(reasons_mask, ridx)) {
                continue;
            }

            cur_len = 0;
            for (word = 0; word < words; word++) {
                if (bit < (cur_len + keywordlen[word])) {
                    break;
                } else {
                    cur_len += keywordlen[word];
                }
            }
            if (word == words) {
                /* FIXME: The regsfile has a very serious issue! */
                return BCM_E_INTERNAL;
            }

            mask[word] |=  1 << (bit - cur_len);
            if (BCM_RX_REASON_GET(reasons, ridx)) {
                key[word] |=  1 << (bit - cur_len);
            }

            /* clean the bit of reasons_remain */
            BCM_RX_REASON_CLEAR(reasons_remain, ridx);

            if (!reason_set) {
                reason_set = TRUE;
                switch (ovx) {
                case 0:
                    if (bit < _bcm_tr3_cpu_cos_map_maxs[1]) {
                        /* First bit in the overlayed range, must be IP */
                        reason_type_mask = 0x3;
                    } else {
                        /* First bit in the common range, no restriction */
                        reason_type_mask = 0;
                    }
                    reason_type_key = 0; /* IP overlay */
                    break;
                case 1:
                    reason_type_mask = 0x3;
                    reason_type_key = 2; /* EP overlay */
                    break;
                case 2:
                    reason_type_mask = 0x3;
                    reason_type_key = 3; /* NLF overlay */
                    break;
                default:
                    return BCM_E_INTERNAL;
                }
            }
        }

    /* check whether there are reasons unsupported or unmappped */
    /* coverity [mixed_enums] */
    for (ridx = bcmRxReasonInvalid; ridx < bcmRxReasonCount; ridx++) {
        if (BCM_RX_REASON_GET(reasons_remain, ridx)) {
            return BCM_E_PARAM;
        }
    }

    /* Now zero entry */
    sal_memset (&entry, 0, sizeof(cpu_cos_map_entry_t));

    /* Program the key and mask */
    for (word = 0; word < words; word++) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            reason_fields[word], key[word]);   

        /* program the mask field */
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            mask_fields[word], mask[word]);   
    }

    /* Program the reasons type */ 
    if (0 != reason_type_mask) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            REASON_CODE_TYPE_MASKf, reason_type_mask);   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            REASON_CODE_TYPE_KEYf, reason_type_key);   
    }

    /* Program the packet type */ 
    if (packet_type_mask & (BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST |
                         BCM_RX_COSQ_PACKET_TYPE_SWITCHED)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            SW_PKT_TYPE_MASKf, sw_pkt_type_mask);   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            SW_PKT_TYPE_KEYf, sw_pkt_type_key);   
    }

    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            MIRR_PKT_MASKf, 1);   
        if (packet_type & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
            soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                                MIRR_PKT_KEYf, 1);   
        }
    }

    /* Handle priority when int_prio_mask != 0 */
    if (int_prio_mask) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            INT_PRI_KEYf, (int_prio & 0xf));   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            INT_PRI_MASKf, (int_prio_mask & 0xf));   
    }

    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, COSf, cosq);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, VALIDf, 1);

    /* write to memory */
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, 
                                           index, &entry));

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRX_SUPPORT
/* Each device has 1 cos map key table and zero or more encoding tables */
STATIC int
_bcm_rx_reason_table_get(int unit,
                         bcm_rx_reason_t **cpu_cos_map_key,
                         int *key_table_len,
                         bcm_rx_reason_t **cpu_opcode_encoding,
                         int *encoding_table_len,
                         int *encoding_table_count)
{
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_HELIX5_SUPPORT
        if (soc_feature(unit, soc_feature_hx5_oam_support)) {
        /* OAM_SLOWPATH_OFFSET for Helix5 */
            _bcm_trident3_cpu_cos_map_key[59] = bcmRxReasonOAMSlowpath;
        }
#endif
        *cpu_cos_map_key = _bcm_trident3_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_trident3_cpu_cos_map_key) /
            sizeof(_bcm_trident3_cpu_cos_map_key[0]);

        *encoding_table_len = 0;
        *encoding_table_count = 0;

        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        *cpu_cos_map_key = _bcm_th3_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_th3_cpu_cos_map_key) /
            sizeof(_bcm_th3_cpu_cos_map_key[0]);

        *encoding_table_len = 0;
        *encoding_table_count = 0;

        return BCM_E_NONE;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
        *cpu_cos_map_key = _bcm_trident2plus_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_trident2plus_cpu_cos_map_key) /
            sizeof(_bcm_trident2plus_cpu_cos_map_key[0]);

        *encoding_table_count = 3;

        cpu_opcode_encoding[0] = _bcm_trident2plus_niv_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_trident2plus_niv_cpu_opcode_encoding) /
            sizeof(_bcm_trident2plus_niv_cpu_opcode_encoding[0]);

        cpu_opcode_encoding[1] = _bcm_trident2_trill_cpu_opcode_encoding;
        encoding_table_len[1] = sizeof(_bcm_trident2_trill_cpu_opcode_encoding) /
            sizeof(_bcm_trident2_trill_cpu_opcode_encoding[0]);

        cpu_opcode_encoding[2] = _bcm_trident2plus_nat_cpu_opcode_encoding;
        encoding_table_len[2] = sizeof(_bcm_trident2plus_nat_cpu_opcode_encoding) /
            sizeof(_bcm_trident2plus_nat_cpu_opcode_encoding[0]);

        return BCM_E_NONE;
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
    if (SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            *cpu_cos_map_key = _bcm_tomahawk_cpu_cos_map_key;
            *key_table_len = sizeof(_bcm_tomahawk_cpu_cos_map_key) /
                sizeof(_bcm_tomahawk_cpu_cos_map_key[0]);
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            *cpu_cos_map_key = _bcm_trident2_cpu_cos_map_key;
            *key_table_len = sizeof(_bcm_trident2_cpu_cos_map_key) /
                sizeof(_bcm_trident2_cpu_cos_map_key[0]);
        }

        *encoding_table_count = 3;

        cpu_opcode_encoding[0] = _bcm_niv_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_niv_cpu_opcode_encoding) /
            sizeof(_bcm_niv_cpu_opcode_encoding[0]);

        cpu_opcode_encoding[1] = _bcm_trident2_trill_cpu_opcode_encoding;
        encoding_table_len[1] = sizeof(_bcm_trident2_trill_cpu_opcode_encoding) /
            sizeof(_bcm_trident2_trill_cpu_opcode_encoding[0]);

        cpu_opcode_encoding[2] = _bcm_nat_cpu_opcode_encoding;
        encoding_table_len[2] = sizeof(_bcm_nat_cpu_opcode_encoding) /
            sizeof(_bcm_nat_cpu_opcode_encoding[0]);

        return BCM_E_NONE;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    if (SOC_IS_TD_TT(unit)) {
        *cpu_cos_map_key = _bcm_trident_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_trident_cpu_cos_map_key) /
            sizeof(_bcm_trident_cpu_cos_map_key[0]);

        *encoding_table_count = 2;

        cpu_opcode_encoding[0] = _bcm_niv_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_niv_cpu_opcode_encoding) /
            sizeof(_bcm_niv_cpu_opcode_encoding[0]);

        cpu_opcode_encoding[1] = _bcm_trill_cpu_opcode_encoding;
        encoding_table_len[1] = sizeof(_bcm_trill_cpu_opcode_encoding) /
            sizeof(_bcm_trill_cpu_opcode_encoding[0]);

        return BCM_E_NONE;
    }

#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        cpu_opcode_encoding[0] = _bcm_bfd_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_bfd_cpu_opcode_encoding) /
            sizeof(_bcm_bfd_cpu_opcode_encoding[0]);
        *encoding_table_count = 1;
        *cpu_cos_map_key = _bcm_katana_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_katana_cpu_cos_map_key) /
            sizeof(_bcm_katana_cpu_cos_map_key[0]);
        return BCM_E_NONE;
    }
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        cpu_opcode_encoding[0] = _bcm_bfd_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_bfd_cpu_opcode_encoding) /
            sizeof(_bcm_bfd_cpu_opcode_encoding[0]);
        *encoding_table_count = 1;
        *cpu_cos_map_key = _bcm_saber2_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_saber2_cpu_cos_map_key) /
            sizeof(_bcm_saber2_cpu_cos_map_key[0]);
        return BCM_E_NONE;
    }
#endif /* BCM_SABER2_SUPPORT */


#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        cpu_opcode_encoding[0] = _bcm_bfd_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_bfd_cpu_opcode_encoding) /
            sizeof(_bcm_bfd_cpu_opcode_encoding[0]);
        *encoding_table_count = 1;
        *cpu_cos_map_key = _bcm_katana2_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_katana2_cpu_cos_map_key) /
            sizeof(_bcm_katana2_cpu_cos_map_key[0]);
        return BCM_E_NONE;
    }
#endif /* BCM_KATANA2_SUPPORT */



#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
        *encoding_table_count = 0;
        *cpu_cos_map_key = _bcm_enduro_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_enduro_cpu_cos_map_key) /
            sizeof(_bcm_enduro_cpu_cos_map_key[0]);
        return BCM_E_NONE;
    }
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        *encoding_table_count = 0;
        *cpu_cos_map_key = _bcm_hurricane2_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_hurricane2_cpu_cos_map_key) /
            sizeof(_bcm_hurricane2_cpu_cos_map_key[0]);
        return BCM_E_NONE;
    }
#endif /* BCM_HURRICANE2_SUPPORT */

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
        cpu_opcode_encoding[0] = _bcm_niv_cpu_opcode_encoding;
        encoding_table_len[0] = sizeof(_bcm_niv_cpu_opcode_encoding) /
            sizeof(_bcm_niv_cpu_opcode_encoding[0]);
        *encoding_table_count = 1;

#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            *cpu_cos_map_key = _bcm_greyhound2_cpu_cos_map_key;
            *key_table_len = sizeof(_bcm_greyhound2_cpu_cos_map_key) /
                sizeof(_bcm_greyhound2_cpu_cos_map_key[0]);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
        *cpu_cos_map_key = _bcm_greyhound_cpu_cos_map_key;
        *key_table_len = sizeof(_bcm_greyhound_cpu_cos_map_key) /
            sizeof(_bcm_greyhound_cpu_cos_map_key[0]);
        }
        return BCM_E_NONE;
    }
#endif /* BCM_GREYHOUND_SUPPORT */

    *encoding_table_count = 0;
    *cpu_cos_map_key = _bcm_trx_cpu_cos_map_key;
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        *key_table_len = _bcm_triumph2_rx_cpu_cosmap_key_max;
    } else if (SOC_IS_TR_VL(unit)) {
        *key_table_len = _bcm_triumph_rx_cpu_cosmap_key_max;
    } else if (SOC_IS_SC_CQ(unit)) {
        *key_table_len = _bcm_scorpion_rx_cpu_cosmap_key_max;
    } else {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_rx_cosq_mapping_reasons_get
 * Purpose:
 *      Get all supported reasons for CPU cosq mapping  
 * Parameters:
 *      unit - Unit reference
 *      reasons - cpu cosq "reasons" mapping bitmap
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_rx_cosq_mapping_reasons_get(int unit, bcm_rx_reasons_t * reasons)
{
#ifdef BCM_TRX_SUPPORT
    bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[3];
    int key_table_len, enc_table_len[3], enc_count;
    int key_idx, table_idx, enc_idx;

    if (reasons == NULL) {
        return BCM_E_PARAM;
    }

    BCM_RX_REASON_CLEAR_ALL(*reasons);

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_rx_reason_overlay)) {
        return _bcm_esw_rcmr_overlay_get(unit, reasons);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    if (SOC_IS_TRX(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                      cpu_opcode_enc, enc_table_len,
                                      &enc_count));

        for (key_idx = 0; key_idx < key_table_len; key_idx++) {
            /* coverity [mixed_enums] */
            if (cpu_cos_map_key[key_idx] != (bcm_rx_reason_t)bcmRxReasonInvalid) {
                BCM_RX_REASON_SET(*reasons, cpu_cos_map_key[key_idx]);
            }
        }
        for (table_idx = 0; table_idx < enc_count; table_idx++) {
            for (enc_idx = 0; enc_idx < enc_table_len[table_idx]; enc_idx++) {
                /* coverity [mixed_enums] */
                if (cpu_opcode_enc[table_idx][enc_idx] != (bcm_rx_reason_t)bcmRxReasonInvalid) {
                    BCM_RX_REASON_SET(*reasons,
                                      cpu_opcode_enc[table_idx][enc_idx]);
                }
            }
        }

        return BCM_E_NONE;
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rx_reasons_get
 * Purpose:
 *      Get all supported reasons for rx packets
 * Parameters:
 *      unit - Unit reference
 *      reasons - rx packet "reasons" bitmap
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_rx_reasons_get (int unit, bcm_rx_reasons_t *reasons)
{
    int rv;

    rv = _bcm_common_rx_reasons_get(unit, reasons);

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[3];
        int key_table_len, enc_table_len[3], enc_count;
        int table_idx, enc_idx;

        BCM_IF_ERROR_RETURN
            (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                      cpu_opcode_enc, enc_table_len,
                                      &enc_count));
        for (table_idx = 0; table_idx < enc_count; table_idx++) {
            if (BCM_RX_REASON_GET(*reasons, cpu_opcode_enc[table_idx][0])) {
                for (enc_idx = 1; enc_idx < enc_table_len[table_idx];
                     enc_idx++) {
                    /* coverity [mixed_enums] */
                    if (cpu_opcode_enc[table_idx][enc_idx] != 
                        (bcm_rx_reason_t)bcmRxReasonInvalid) {
                        BCM_RX_REASON_SET(*reasons,
                                          cpu_opcode_enc[table_idx][enc_idx]);
                    }
                }
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */

    return rv;
}

static soc_reg_t _rx_redirect_reg[3][2] = {
    { CMIC_PKT_REASONr, CMIC_PKT_REASON_HIr },
    { CMIC_PKT_REASON_DIRECTr, CMIC_PKT_REASON_DIRECT_HIr },
    { CMIC_PKT_REASON_MINIr, CMIC_PKT_REASON_MINI_HIr }
};

#ifdef BCM_CMICM_SUPPORT
static soc_reg_t _cmicm_rx_redirect_reg[3][2] = {
    {CMIC_PKT_REASON_0_TYPEr, CMIC_PKT_REASON_1_TYPEr},
    {CMIC_PKT_REASON_DIRECT_0_TYPEr, CMIC_PKT_REASON_DIRECT_1_TYPEr},
    {CMIC_PKT_REASON_MINI_0_TYPEr, CMIC_PKT_REASON_MINI_0_TYPEr}
};
#endif

#ifdef BCM_CMICX_SUPPORT
static soc_reg_t _cmicx_rx_redirect_reg[3][2] = {
    {CMIC_RPE_PKT_REASON_0_TYPEr, CMIC_RPE_PKT_REASON_1_TYPEr},
    {CMIC_RPE_PKT_REASON_DIRECT_0_TYPEr, CMIC_RPE_PKT_REASON_DIRECT_1_TYPEr},
    {CMIC_RPE_PKT_REASON_MINI_0_TYPEr, CMIC_RPE_PKT_REASON_MINI_0_TYPEr}
};
#endif

int
bcm_esw_rx_redirect_reasons_set(int unit, bcm_rx_redirect_t mode, bcm_rx_reasons_t reasons)
{
    uint32 ix, i, max_index = 32;
    uint32 addr, rval, rval_hi = 0;
    soc_rx_reason_t *map;
    uint8 set = 0;
    soc_reg_t cmic_reg;

    if (!SOC_UNIT_VALID(unit)) { 
        return BCM_E_UNIT;
    }
    /* coverity [mixed_enums] */
    if ((mode < 0) || (mode > (bcm_rx_redirect_t)_SHR_RX_REDIRECT_MAX)) {
        return SOC_E_PARAM;
    }
    if ((map = (SOC_DCB_RX_REASON_MAPS(unit))[0]) == NULL) {
        return SOC_E_INTERNAL;
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        addr = soc_reg_addr(unit, _cmicm_rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        addr = soc_reg_addr(unit, _cmicx_rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    } else
#endif
    {
        addr = soc_reg_addr(unit, _rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    }
    rval = soc_pci_read(unit, addr);
    if (soc_feature(unit, soc_feature_dcb_reason_hi)) {
        max_index = 64;
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            addr = soc_reg_addr(unit, _cmicm_rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        } else
#endif
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            addr = soc_reg_addr(unit, _cmicx_rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        } else
#endif
        {
            addr = soc_reg_addr(unit, _rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        }
        rval_hi = soc_pci_read(unit, addr);
    }
    
    for (ix = 0; ix < max_index ; ix++) {
        /* coverity [mixed_enums] */
        if (map[ix] != (soc_rx_reason_t)socRxReasonInvalid && 
               map[ix] != (soc_rx_reason_t)socRxReasonCount) {
            if (BCM_RX_REASON_GET(reasons, (bcm_rx_reason_t)map[ix])) {
                set++;
                if (ix < 32) {
                    rval |= (1 << ix);
                } else {
                    rval_hi |= (1 << (ix-32));
                }
            } else {
                set++;
                if (ix < 32) {
                    rval &= ~(1 << ix);
                } else {
                    rval_hi &= ~(1 << (ix-32));
                }
            }
        }
    }
    
    if (set) {
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            cmic_reg = _cmicm_rx_redirect_reg[mode][0];
        } else
#endif
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            cmic_reg = _cmicx_rx_redirect_reg[mode][0];
        } else
#endif
        {
            cmic_reg = _rx_redirect_reg[mode][0];
        }
        for (i = 0; i < SOC_REG_NUMELS(unit, cmic_reg); i++) {
             addr = soc_reg_addr(unit, cmic_reg, REG_PORT_ANY, i);
             soc_pci_write(unit, addr, rval);
        }
        if (soc_feature(unit, soc_feature_dcb_reason_hi)) {
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                cmic_reg = _cmicm_rx_redirect_reg[mode][1];
            } else
#endif
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                cmic_reg = _cmicx_rx_redirect_reg[mode][1];
            } else
#endif
            {
                cmic_reg = _rx_redirect_reg[mode][1];
            }
            for (i = 0; i < SOC_REG_NUMELS(unit, cmic_reg); i++) {
                addr = soc_reg_addr(unit, cmic_reg, REG_PORT_ANY, i);
                soc_pci_write(unit, addr, rval_hi);
            }
        }
    }
    return SOC_E_NONE;
}

int
bcm_esw_rx_redirect_reasons_get(int unit, 
                                bcm_rx_redirect_t mode, 
                                bcm_rx_reasons_t *reasons)
{
    uint32 ix, max_index = 32;
    uint32 addr, rval;
    uint32 rval_hi = 0;
    soc_rx_reason_t *map;

    if (!SOC_UNIT_VALID(unit)) { 
        return BCM_E_UNIT;
    }
    /* coverity [mixed_enums] */
    if ((mode < 0) || (mode > (bcm_rx_redirect_t)_SHR_RX_REDIRECT_MAX)) {
        return SOC_E_PARAM;
    }
    if ((map = (SOC_DCB_RX_REASON_MAPS(unit))[0]) == NULL) {
        return SOC_E_INTERNAL;
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        addr = soc_reg_addr(unit, _cmicm_rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        addr = soc_reg_addr(unit, _cmicx_rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    } else
#endif
    {
        addr = soc_reg_addr(unit, _rx_redirect_reg[mode][0], REG_PORT_ANY, 0);
    }
    rval = soc_pci_read(unit, addr);
    if (soc_feature(unit, soc_feature_dcb_reason_hi)) {
        max_index = 64;
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            addr = soc_reg_addr(unit, _cmicm_rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        } else
#endif
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            addr = soc_reg_addr(unit, _cmicx_rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        } else
#endif
        {
            addr = soc_reg_addr(unit, _rx_redirect_reg[mode][1], REG_PORT_ANY, 0);
        }
        rval_hi = soc_pci_read(unit, addr);
    }
    BCM_RX_REASON_CLEAR_ALL(*reasons);
    for (ix = 0; ix < max_index ; ix++) {
        /* coverity [mixed_enums] */
        if (map[ix] != (soc_rx_reason_t)socRxReasonInvalid && 
               map[ix] != (soc_rx_reason_t)socRxReasonCount) {
            if (ix < 32) {
                if (rval & (1 << ix)) {
                    BCM_RX_REASON_SET(*reasons, (bcm_rx_reason_t)map[ix]);
                }
            } else {
                if (rval_hi & (1 << (ix - 32))) {
                    BCM_RX_REASON_SET(*reasons, (bcm_rx_reason_t)map[ix]);
                }
            }
        }
    }
    return SOC_E_NONE;
}

#ifdef BCM_TRX_SUPPORT
extern int _bcm_esw_cpu_cosq_mapping_default_set(int unit, int numq);
extern int _bcm_esw_cosq_config_property_get(int unit);
/* Delete an entry of CPU_COS_MAPm */
int
_bcm_trx_rx_cosq_mapping_delete(int unit, int index)
{
    cpu_cos_map_entry_t entry; 

    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit) || SOC_IS_KATANAX(unit)) {
        int rv;
        bcm_rx_reasons_t reasons;
        bcm_rx_reasons_t reasons_mask;
        uint8 prio;
        uint8 prio_mask;
        uint32 packet_type;
        uint32 packet_type_mask;
        bcm_cos_queue_t cosq;
        rv = bcm_esw_rx_cosq_mapping_get(
            unit, index, &reasons, &reasons_mask, &prio,
            &prio_mask,&packet_type, &packet_type_mask, &cosq);

        if (rv == BCM_E_NONE) {
            if (!SOC_IS_KATANAX(unit)) {
                if (BCM_RX_REASON_GET(reasons, bcmRxReasonSampleDest)) {
                    BCM_IF_ERROR_RETURN(WRITE_EGR_SFLOW_CPU_COS_CONFIGr(unit, 0));
                }

                if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
                    BCM_IF_ERROR_RETURN(WRITE_MIRROR_CPU_COS_CONFIGr(unit, 0));
                }
            } else {
                if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfd)) {
                    RX_UNIT_REASONS_BFD_CLEAR(unit);
                }
                if (BCM_RX_REASON_GET(reasons, bcmRxReasonMplsCtrlWordError) ||
                    BCM_RX_REASON_GET(reasons, bcmRxReasonMplsUnknownAch)) {
                    RX_UNIT_REASONS_MPLS_BFD_CLEAR(unit);
                }
            }
        }
    }
#endif

    /* Now zero entry */
    sal_memset (&entry, 0, sizeof(cpu_cos_map_entry_t));

    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, VALIDf, 0);

    /* write to memory */
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, 
                            index, &entry));

    return BCM_E_NONE;
}


/* Delete all entry of CPU_COS_MAPm */
int
_bcm_trx_rx_cosq_mapping_init(int unit)
{
    cpu_cos_map_entry_t entry; 
    int index;
    int numq = 0;

    /*one by one,space instead of time if we use soc_mem_write_range*/
    for (index = soc_mem_index_min(unit, CPU_COS_MAPm);
         index <= soc_mem_index_max(unit, CPU_COS_MAPm);index++){
        /* Now zero entry */
        sal_memset (&entry, 0, sizeof(cpu_cos_map_entry_t));

        /* write to memory */
        SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, 
                                index, &entry));
    }

    numq = _bcm_esw_cosq_config_property_get(unit);
    
    /* Map internal priority levels to CPU CoS queues */
    
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        RX_UNIT_REASONS_BFD_CLEAR(unit);
        RX_UNIT_REASONS_MPLS_BFD_CLEAR(unit);
    }
#endif /* BCM_KATANA_SUPPORT */

    return BCM_E_NONE;
}

/* Get the rx cosq mapping */
int
_bcm_trx_rx_cosq_mapping_get(int unit, int index,
                            bcm_rx_reasons_t *reasons, 
                            bcm_rx_reasons_t *reasons_mask,
                            uint8 *prio, uint8 *prio_mask,
                            uint32 *packet_type, uint32 * packet_type_mask,
                            bcm_cos_queue_t *cosq, uint32 *flags)
{
    bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[6] = {0};
    int key_table_len, enc_table_len[6] = {0}, enc_count;
    int word_idx, bit_idx, key_idx, table_idx, word_count, bit_count, idx;
    uint32 mask[6] = {0}, key[6] = {0}, enc_mask[6] = {0};
    uint32 enc_key[6] = {0}, field_size[6] = {0}, bit_key[6] = {0};
    cpu_cos_map_entry_t entry; 
    uint32 sw_pkt_type_mask, sw_pkt_type_key;
    soc_field_t field = -1;
    uint32 bit_skip = 0;
#ifdef BCM_KATANA_SUPPORT
    int rx_reasons_bfd_get = 0, rx_reasons_mpls_bfd_get = 0;
#endif
    BCM_IF_ERROR_RETURN
        (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                  cpu_opcode_enc, enc_table_len, &enc_count));

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* NULL pointer check */
    if (reasons == NULL  || reasons_mask == NULL  || 
        prio == NULL || prio_mask == NULL || 
        packet_type == NULL || packet_type_mask == NULL || 
        cosq == NULL) {
        return BCM_E_PARAM;
    }
 
    /* Read the entry */
    SOC_IF_ERROR_RETURN(READ_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, index, &entry));

    /* Return BCM_E_NOT_FOUND if invalid */
    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, VALIDf) == 0) {
        return (BCM_E_NOT_FOUND);
    }        

    sal_memset(reasons, 0, sizeof(bcm_rx_reasons_t));
    sal_memset(reasons_mask, 0, sizeof(bcm_rx_reasons_t));

    *cosq = (bcm_cos_queue_t)
              soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, COSf);
    
    sw_pkt_type_mask = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, SW_PKT_TYPE_MASKf);
    sw_pkt_type_key  = soc_mem_field32_get(unit, CPU_COS_MAPm, 
                                           &entry, SW_PKT_TYPE_KEYf);

    if (sw_pkt_type_mask == 0 && sw_pkt_type_key == 0) {
        /* all packets matched */
        *packet_type_mask = 0;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 0) {
        /* Only non-switched packets */
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED | 
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 3) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                        BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
    }
 
    if (soc_mem_field_valid(unit, CPU_COS_MAPm, RESERVED_MIRR_PKT_MASKf)) {
        field = RESERVED_MIRR_PKT_MASKf;
    } else {
        field = MIRR_PKT_MASKf;
    }

    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, field)) {
        *packet_type_mask |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }

    if (soc_mem_field_valid(unit, CPU_COS_MAPm, RESERVED_MIRR_PKT_KEYf)) {
        field = RESERVED_MIRR_PKT_KEYf;
    } else {
        field = MIRR_PKT_KEYf;
    }

    if (soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, field)) {
        *packet_type |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }
#ifdef BCM_TRIDENT2_SUPPORT    
    if (soc_mem_field_valid(unit, CPU_COS_MAPm, TRUNCATE_CPU_COPYf) && 
        flags != NULL) {
        *flags = soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, TRUNCATE_CPU_COPYf) ? 
                 BCM_RX_COPYTOCPU_TRUNCATE : 0;
    }
#endif

    *prio_mask = 
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, INT_PRI_MASKf);
    *prio = 
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, INT_PRI_KEYf);

#if defined BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        int length = 0;
        word_count = 0;

        length = soc_mem_field_length(unit, CPU_COS_MAPm, KEYf);
        while (length > 32) {
            field_size[word_count] = 32;
            word_count++;
            length -= 32;
        }
        field_size[word_count] = length;
        word_count++;

        soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, MASKf,&mask[0]);
        soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, KEYf, &key[0]);
        /* 4bits INT_PRI + 1bit RESERVED_MIRR_PKT + 2bits SW_PKT_TYPE */
        bit_skip = 0x7;
    } else
#endif
    {
    mask[0] =
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, REASONS_MASK_LOWf);
    key[0] =
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, REASONS_KEY_LOWf);
    field_size[0] =
        soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_LOWf);
    word_count = 1;
    if (soc_mem_field_valid(unit, CPU_COS_MAPm, REASONS_MASK_MIDf)) {
        mask[word_count] =
            soc_mem_field32_get(unit, CPU_COS_MAPm,
                                &entry, REASONS_MASK_MIDf);
        key[word_count] =
            soc_mem_field32_get(unit, CPU_COS_MAPm,
                                &entry, REASONS_KEY_MIDf);
        field_size[word_count] =
            soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_MIDf);
        word_count++;
    }
    mask[word_count] =
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, REASONS_MASK_HIGHf);
    key[word_count] =
        soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, REASONS_KEY_HIGHf);
    field_size[word_count] =
        soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_HIGHf);
    word_count++;
    }

    sal_memset(enc_mask, 0, sizeof(enc_mask));
    sal_memset(enc_key, 0, sizeof(enc_key));
    sal_memset(bit_key, 0, sizeof(bit_key));
    for (word_idx = 0; word_idx < word_count; word_idx++) {
        bit_count = field_size[word_idx];
#if defined BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            bit_idx = (word_idx == 0 ? bit_skip:0);
        } else
#endif
        {
            bit_idx = 0;
        }
        for (; bit_idx < bit_count; bit_idx++) {
            uint32 maskbitset;
            uint32 keybitset;

            maskbitset = mask[word_idx] & (1 << bit_idx);
            keybitset = key[word_idx] & (1 << bit_idx);
            key_idx = word_idx * 32 + bit_idx;
            key_idx -= bit_skip;
            if (key_idx >= key_table_len) {
                break;
            }

           for (table_idx = 0; table_idx < enc_count; table_idx++) {
               if (cpu_cos_map_key[key_idx] == cpu_opcode_enc[table_idx][0]) {
                       if (maskbitset) { /* should always be set */
                       enc_mask[table_idx] |= (1 << bit_key[table_idx]);
                       }
                       if (keybitset) {
                       enc_key[table_idx] |= (1 << bit_key[table_idx]);
                   }
                   bit_key[table_idx]++;
               }
           }
           if (maskbitset) {
               BCM_RX_REASON_SET(*reasons_mask, cpu_cos_map_key[key_idx]);
           }
           if (keybitset) {
               BCM_RX_REASON_SET(*reasons, cpu_cos_map_key[key_idx]);
           }
       }
    }

    for (table_idx = 0; table_idx < enc_count; table_idx++) {
        idx = enc_key[table_idx];
        if (idx > 0) {
                BCM_RX_REASON_SET(*reasons, cpu_opcode_enc[table_idx][idx]);
            }
        }
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        uint32 bfdval = 0;
        bfdval = (key[1] & 0x3fffffff) >> 28;
        BCM_RX_REASON_CLEAR(*reasons, bcmRxReasonBfd);
        BCM_RX_REASON_CLEAR(*reasons, bcmRxReasonBfdUnknownVersion);
        BCM_RX_REASON_CLEAR(*reasons_mask, bcmRxReasonBfd);
        BCM_RX_REASON_CLEAR(*reasons_mask, bcmRxReasonBfdUnknownVersion);
        RX_UNIT_REASONS_MPLS_BFD_GET(unit, rx_reasons_mpls_bfd_get);
        RX_UNIT_REASONS_BFD_GET(unit, rx_reasons_bfd_get);

        if (bfdval == 0x1) {
            if (rx_reasons_bfd_get) {
                BCM_RX_REASON_SET(*reasons, bcmRxReasonBfd);
                BCM_RX_REASON_SET(*reasons, bcmRxReasonBfdUnknownVersion);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfd);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfdUnknownVersion);
            }
            if (rx_reasons_mpls_bfd_get) {
                BCM_RX_REASON_SET(*reasons, bcmRxReasonMplsUnknownAch);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonMplsUnknownAch);
            }
        } else if(bfdval == 0x2) {
           BCM_RX_REASON_SET(*reasons, bcmRxReasonBfd);
           BCM_RX_REASON_SET(*reasons, bcmRxReasonBfdInvalidVersion);
           BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfd);
           BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfdInvalidVersion);
        } else if (bfdval == 0x3) {
            if (rx_reasons_bfd_get) {
                BCM_RX_REASON_SET(*reasons, bcmRxReasonBfd);
                BCM_RX_REASON_SET(*reasons, bcmRxReasonBfdInvalidPacket);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfd);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonBfdInvalidPacket);
            }
            if (rx_reasons_mpls_bfd_get) {
                BCM_RX_REASON_SET(*reasons, bcmRxReasonMplsCtrlWordError);
                BCM_RX_REASON_SET(*reasons_mask, bcmRxReasonMplsCtrlWordError);
            }
        }
    }
#endif

    return BCM_E_NONE;
}

/* Set the rx cosq mapping */
int
_bcm_trx_rx_cosq_mapping_set(int unit, int index,
                            bcm_rx_reasons_t reasons, bcm_rx_reasons_t
                            reasons_mask,
                            uint8 int_prio, uint8 int_prio_mask,
                            uint32 packet_type, uint32 packet_type_mask,
                            bcm_cos_queue_t cosq, uint32 flags)
{
    bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[6] = {0};
    int key_table_len, enc_table_len[3], enc_count;
    int word_idx, bit_idx, key_idx, table_idx, enc_idx, word_count;
    uint32 mask[6] = {0}, key[6] = {0}, enc_key, field_size[6] = {0};
    bcm_rx_reason_t  reason_enum, enc_enum;
    bcm_rx_reasons_t reasons_remain;
    cpu_cos_map_entry_t entry; 
    uint8 sw_pkt_type_key = 0;
    uint8 sw_pkt_type_mask = 0;
    soc_field_t field = -1;
    uint32 bit_skip = 0;
#ifdef BCM_KATANA_SUPPORT
    int rx_reasons_bfd_set = 0, rx_reasons_mpls_bfd_set = 0;
#endif

    BCM_IF_ERROR_RETURN
        (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                  cpu_opcode_enc, enc_table_len, &enc_count));

    /* Verify COSQ */
    if (SOC_IS_TD_TT(unit)) {
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) { 
            /* TD2 */
            if (cosq >= _bcm_trident2_rx_cpu_max_cosq) {
                return BCM_E_PARAM;
            }
        } else { 
            /* TD */
            if (cosq >= _bcm_triumph_rx_cpu_max_cosq) {
                return BCM_E_PARAM;
            }
        }
#endif /* BCM_TRIDENT_SUPPORT */
    } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
               SOC_IS_VALKYRIE2(unit)) {
        if (cosq >= _bcm_triumph_rx_cpu_max_cosq) {
            return BCM_E_PARAM;
        }
#ifdef BCM_KATANA_SUPPORT
    } else if (SOC_IS_KATANAX(unit)) {
        if (SOC_IS_KATANA2(unit)) {
            /* KATANA2 */
            if (cosq >= _bcm_katana2_rx_cpu_max_cosq) {
                return BCM_E_PARAM;
            }
        } else {
            /* KATANA */
            if (cosq >= _bcm_triumph_rx_cpu_max_cosq) {
                return BCM_E_PARAM;
            }
        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
    } else if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANEX(unit) || \
                SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
#if defined(BCM_HURRICANE_SUPPORT)
        if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_GREYHOUND2(unit)) {
            if (cosq >= _bcm_hurricane_rx_cpu_max_cosq) {
                return BCM_E_PARAM;
            }            
        } else
#endif /* BCM_HURRICANE_SUPPORT */
        if (cosq >= _bcm_triumph_rx_cpu_max_cosq) {
            return BCM_E_PARAM;
        }
#endif /* BCM_ENDURO_SUPPORT */
    } else if (SOC_IS_TR_VL(unit)) {
        if (cosq >= _bcm_triumph_rx_cpu_max_cosq) {
            return BCM_E_PARAM;
        }
    } else if (SOC_IS_SC_CQ(unit)) {
        if (cosq >= _bcm_scorpion_rx_cpu_max_cosq) {
            return BCM_E_PARAM;
        }
    } else {
        return BCM_E_UNAVAIL;
    }

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* Verify the packet type */

    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_key |= 1;
    }
    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_key |= 2;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_mask |= 1;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_mask |= 2;
    }
    sw_pkt_type_key &= sw_pkt_type_mask;

    if ((sw_pkt_type_mask == 0x1) || 
        ((sw_pkt_type_mask != 0) && (sw_pkt_type_key == 0x1))) { 
        /* Hw doesn't support these cases */
        return BCM_E_PARAM;
    }

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfd)) {
            rx_reasons_bfd_set = 1;
        }
        if (BCM_RX_REASON_GET(reasons, bcmRxReasonMplsCtrlWordError)){
            BCM_RX_REASON_CLEAR(reasons, bcmRxReasonMplsCtrlWordError);
            BCM_RX_REASON_CLEAR(reasons_mask, bcmRxReasonMplsCtrlWordError);
            BCM_RX_REASON_SET(reasons, bcmRxReasonBfd);
            BCM_RX_REASON_SET(reasons, bcmRxReasonBfdInvalidPacket);
            BCM_RX_REASON_SET(reasons_mask, bcmRxReasonBfd);
            BCM_RX_REASON_SET(reasons_mask, bcmRxReasonBfdInvalidPacket);
            rx_reasons_mpls_bfd_set = 1;
        }
        if (BCM_RX_REASON_GET(reasons, bcmRxReasonMplsUnknownAch)){
            BCM_RX_REASON_CLEAR(reasons, bcmRxReasonMplsUnknownAch);
            BCM_RX_REASON_CLEAR(reasons_mask, bcmRxReasonMplsUnknownAch);
            BCM_RX_REASON_SET(reasons, bcmRxReasonBfd);
            BCM_RX_REASON_SET(reasons, bcmRxReasonBfdUnknownVersion);
            BCM_RX_REASON_SET(reasons_mask, bcmRxReasonBfd);
            BCM_RX_REASON_SET(reasons_mask, bcmRxReasonBfdUnknownVersion);
            rx_reasons_mpls_bfd_set = 1;
        }
       if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfdLookupFailure)) {
           BCM_RX_REASON_CLEAR(reasons, bcmRxReasonBfdLookupFailure);
           BCM_RX_REASON_SET(reasons, bcmRxReasonBfdInvalidVersion);
       }
       if (BCM_RX_REASON_GET(reasons_mask, bcmRxReasonBfdLookupFailure)) {
           BCM_RX_REASON_CLEAR(reasons_mask, bcmRxReasonBfdLookupFailure);
           BCM_RX_REASON_SET(reasons_mask, bcmRxReasonBfdInvalidVersion);
       }

       if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfd) &&
               !(BCM_RX_REASON_GET(reasons, bcmRxReasonBfdUnknownVersion) ||
                   BCM_RX_REASON_GET(reasons, bcmRxReasonBfdInvalidVersion) ||
                   BCM_RX_REASON_GET(reasons, bcmRxReasonBfdInvalidPacket))) {
           return BCM_E_PARAM;
       }
    }

#endif /* BCM_KATANA_SUPPORT */
#if defined BCM_TRIDENT3_SUPPORT
if (SOC_IS_TRIDENT3X(unit)) {
        int length = 0;
        word_count = 0;
        length = soc_mem_field_length(unit, CPU_COS_MAPm, KEYf);
        while (length > 32) {
            field_size[word_count] = 32;
            word_count++;
            length -= 32;
        }
        field_size[word_count] = length;
        word_count++;

        /* 4bits INT_PRI + 1bit RESERVED_MIRR_PKT + 2bits SW_PKT_TYPE */
        bit_skip = 0x7;
    } else
#endif
    {
    field_size[0] =
        soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_LOWf);
    word_count = 1;
    if (soc_mem_field_valid(unit, CPU_COS_MAPm, REASONS_MASK_MIDf)) {
        field_size[word_count] =
            soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_MIDf);
        word_count++;
    }
    field_size[word_count] =
        soc_mem_field_length(unit, CPU_COS_MAPm, REASONS_KEY_HIGHf);
    word_count++;
    }
    reasons_remain = reasons_mask;
    enc_enum = -1;
    enc_key = 0;
    word_idx = 0;
    bit_idx = bit_skip;
    sal_memset(mask, 0, sizeof(mask));
    sal_memset(key, 0, sizeof(key));
    for (key_idx = 0; key_idx < key_table_len; key_idx++, bit_idx++) {
        if (bit_idx == field_size[word_idx]) {
            bit_idx = 0;
            word_idx++;
            if (word_idx == word_count) {
                /* Trap implementation error */
                return BCM_E_INTERNAL;
            }
        }
 
        reason_enum = cpu_cos_map_key[key_idx];
        if (reason_enum == enc_enum) { /* following bit of encoding */
            mask[word_idx] |= 1 << bit_idx;
            key[word_idx] |= (enc_key & 1) << bit_idx;
            enc_key >>= 1;
            continue;
        }

        /* Find reason being set */
        if (!BCM_RX_REASON_GET(reasons_mask, reason_enum)) {
            continue;
        }
        BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);

        for (table_idx = 0; table_idx < enc_count; table_idx++) {
            if (reason_enum != cpu_opcode_enc[table_idx][0]) {
                continue;
            }

            enc_enum = reason_enum;
            for (enc_idx = 1; enc_idx < enc_table_len[table_idx]; enc_idx++) {
                reason_enum = cpu_opcode_enc[table_idx][enc_idx];
                if (!BCM_RX_REASON_GET(reasons, reason_enum)) {
                    BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);
                    continue;
                } else if (enc_key != 0) { /* multiple reasons supplied */
                    return BCM_E_PARAM;
                }
                BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);
                enc_key = enc_idx;
            }
            mask[word_idx] |= 1 << bit_idx;
            key[word_idx] |= (enc_key & 1) << bit_idx;
            enc_key >>= 1;
            break;
        }
        if (table_idx == enc_count) {
            mask[word_idx] |= 1 << bit_idx;
            if (BCM_RX_REASON_GET(reasons, reason_enum)) {
                key[word_idx] |= 1 << bit_idx;
            }
        }
    }

    /* check whether there are reasons unsupported */
    if (!BCM_RX_REASON_IS_NULL(reasons_remain)) {
        return BCM_E_PARAM;
    }

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        uint32 bfdval = 0, bfdmask = 0, bfdkey = 0;
        if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfd)) {
            if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfdUnknownVersion)) {
                bfdval |= 0x1;
            } else if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfdInvalidVersion)) {
                bfdval |= 0x2;
            } else if (BCM_RX_REASON_GET(reasons, bcmRxReasonBfdInvalidPacket)) {
                bfdval |= 0x3;
            }
            if (bfdval) {
                bfdkey = key[1];
                bfdmask = mask[1];
                SHR_BITCLR(&bfdkey, 28);
                SHR_BITCLR(&bfdkey, 29);
                SHR_BITCLR(&bfdmask, 28);
                SHR_BITCLR(&bfdmask, 29);
                mask[1] =  bfdmask | (bfdval << 28);
                key[1] = bfdkey | (bfdval << 28);
            }
        }
    }
#endif
    /* Now zero entry */
    sal_memset (&entry, 0, sizeof(cpu_cos_map_entry_t));

    /* Program the key and mask when reasons_mask is not NULL */
    if (!BCM_RX_REASON_IS_NULL(reasons_mask)) {
#if defined BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry, MASKf,
                &mask[0]);
            soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry, KEYf,
                &key[0]);
        } else
#endif
        {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, REASONS_MASK_LOWf,
                            mask[0]);   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, REASONS_KEY_LOWf,
                            key[0]);
        word_count = 1;
        if (soc_mem_field_valid(unit, CPU_COS_MAPm, REASONS_MASK_MIDf)) {
                soc_mem_field32_set(unit, CPU_COS_MAPm, &entry,
                    REASONS_MASK_MIDf, mask[word_count]);
                soc_mem_field32_set(unit, CPU_COS_MAPm, &entry,
                    REASONS_KEY_MIDf, key[word_count]);
            word_count++;
        }
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, REASONS_MASK_HIGHf,
                            mask[word_count]);   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, REASONS_KEY_HIGHf,
                            key[word_count]);
    }
    }

    /* Program the packet type */ 

    if (packet_type_mask & (BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST |
                         BCM_RX_COSQ_PACKET_TYPE_SWITCHED)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            SW_PKT_TYPE_MASKf, sw_pkt_type_mask);   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            SW_PKT_TYPE_KEYf, sw_pkt_type_key);   
    }

    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
        if (soc_mem_field_valid(unit, CPU_COS_MAPm, RESERVED_MIRR_PKT_MASKf)) {
            field = RESERVED_MIRR_PKT_MASKf;
        } else {
            field = MIRR_PKT_MASKf;
        }

        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry,
                            field, 1);

        if (packet_type & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
            if (soc_mem_field_valid(unit, CPU_COS_MAPm, RESERVED_MIRR_PKT_KEYf)) {
                field = RESERVED_MIRR_PKT_KEYf;
            } else {
                field = MIRR_PKT_KEYf;
            }

            soc_mem_field32_set(unit, CPU_COS_MAPm, &entry,
                                field, 1);
        }
    }

    /* Handle priority when int_prio_mask != 0 */
    if (int_prio_mask) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            INT_PRI_KEYf, (int_prio & 0xf));   
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, 
                            INT_PRI_MASKf, (int_prio_mask & 0xf));   
    }

    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, COSf, cosq);
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, VALIDf, 3);
    } else
#endif
    {
    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, VALIDf, 1);
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if(SOC_MEM_FIELD_VALID(unit, CPU_COS_MAPm, STRENGTHf)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, STRENGTHf,
                    (soc_mem_index_max(unit, CPU_COS_MAPm) - index));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
    if(SOC_MEM_FIELD_VALID(unit, CPU_COS_MAPm, TRUNCATE_CPU_COPYf)) {
        soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, TRUNCATE_CPU_COPYf,
                            (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
    }    
#endif

    


    /* write to memory */
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, 
                                           index, &entry));

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        uint32 cpu_cos_config;
        if (BCM_RX_REASON_GET(reasons, bcmRxReasonSampleDest)) {
            BCM_IF_ERROR_RETURN
                (READ_EGR_SFLOW_CPU_COS_CONFIGr(unit, &cpu_cos_config));
            soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                              COSf, cosq);
            soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                              STRENGTHf,
                              (soc_mem_index_max(unit, CPU_COS_MAPm) - index));
            soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                              TRUNCATE_CPU_COPYf,
                              (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
            BCM_IF_ERROR_RETURN
                (WRITE_EGR_SFLOW_CPU_COS_CONFIGr(unit, cpu_cos_config));
        }
        if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
            BCM_IF_ERROR_RETURN
                (READ_MIRROR_CPU_COS_CONFIGr(unit, &cpu_cos_config));
            soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                              COSf, cosq);
            soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                              STRENGTHf,
                              (soc_mem_index_max(unit, CPU_COS_MAPm) - index));
            soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                              TRUNCATE_CPU_COPYf,
                              (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
            BCM_IF_ERROR_RETURN
                (WRITE_MIRROR_CPU_COS_CONFIGr(unit, cpu_cos_config));
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        if (rx_reasons_bfd_set) {
            RX_UNIT_REASONS_BFD_SET(unit);
        }
        if (rx_reasons_mpls_bfd_set) {
            RX_UNIT_REASONS_MPLS_BFD_SET(unit);
        }
    }
#endif /* BCM_KATANA_SUPPORT */

    return BCM_E_NONE;
}
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
int
_bcm_th3_rx_cosq_mapping_get(int unit, int index,
                            bcm_rx_reasons_t *reasons,
                            bcm_rx_reasons_t *reasons_mask,
                            uint8 *prio, uint8 *prio_mask,
                            uint32 *packet_type, uint32 * packet_type_mask,
                            bcm_cos_queue_t *cosq, uint32 *flags)
{
#define KEY_ENTRY_WORDS_MAX 6
    bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[KEY_ENTRY_WORDS_MAX] = {0};
    int key_table_len, enc_table_len[KEY_ENTRY_WORDS_MAX] = {0}, enc_count;
    int word_idx, bit_idx, key_idx, table_idx, word_count, bit_count, idx;
    uint32 mask[KEY_ENTRY_WORDS_MAX] = {0}, key[KEY_ENTRY_WORDS_MAX] = {0}, enc_mask[KEY_ENTRY_WORDS_MAX] = {0};
    uint32 enc_key[KEY_ENTRY_WORDS_MAX] = {0}, field_size[KEY_ENTRY_WORDS_MAX] = {0}, bit_key[KEY_ENTRY_WORDS_MAX] = {0};
    uint32 sw_pkt_type_mask, sw_pkt_type_key;
    uint32 bit_skip = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 cpu_cos_map_mask_entry[SOC_MAX_MEM_WORDS];
    uint32 cpu_cos_map_key_entry[SOC_MAX_MEM_WORDS];
    int length = 0;

    word_count = 0;

    sal_memset (&cpu_cos_map_mask_entry, 0, sizeof(cpu_cos_map_mask_entry));
    sal_memset (&cpu_cos_map_key_entry, 0, sizeof(cpu_cos_map_key_entry));
    sal_memset (&entry, 0, sizeof(entry));


    BCM_IF_ERROR_RETURN
        (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                  cpu_opcode_enc, enc_table_len, &enc_count));

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* NULL pointer check */
    if (reasons == NULL  || reasons_mask == NULL  ||
        prio == NULL || prio_mask == NULL ||
        packet_type == NULL || packet_type_mask == NULL ||
        cosq == NULL) {
        return BCM_E_PARAM;
    }

    /* Read the entry */
    SOC_IF_ERROR_RETURN(READ_CPU_COS_MAPm(unit, MEM_BLOCK_ANY, index, entry));

    /* Return BCM_E_NOT_FOUND if invalid */
    if (soc_mem_field32_get(unit, CPU_COS_MAPm, entry, VALIDf) == 0) {
        return (BCM_E_NOT_FOUND);
    }

    sal_memset(reasons, 0, sizeof(bcm_rx_reasons_t));
    sal_memset(reasons_mask, 0, sizeof(bcm_rx_reasons_t));

    *cosq = (bcm_cos_queue_t)
              soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, COSf);

    soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, MASKf, &cpu_cos_map_mask_entry[0]);
    soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, KEYf, &cpu_cos_map_key_entry[0]);

    sw_pkt_type_mask = soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt,
                                           &cpu_cos_map_mask_entry, SW_PKT_TYPEf);
    sw_pkt_type_key  = soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt,
                                           &cpu_cos_map_key_entry, SW_PKT_TYPEf);

    if (sw_pkt_type_mask == 0 && sw_pkt_type_key == 0) {
        /* all packets matched */
        *packet_type_mask = 0;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 0) {
        /* Only non-switched packets */
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = 0;
    } else if (sw_pkt_type_mask == 2 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 2) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED;
    } else if (sw_pkt_type_mask == 3 && sw_pkt_type_key == 3) {
        *packet_type_mask = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                             BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
        *packet_type = BCM_RX_COSQ_PACKET_TYPE_SWITCHED |
                        BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST;
    }

    if (soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt, &cpu_cos_map_mask_entry, RESERVED_MIRR_PKTf)) {
        *packet_type_mask |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }

    if (soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt, &cpu_cos_map_key_entry, RESERVED_MIRR_PKTf)) {
        *packet_type |= BCM_RX_COSQ_PACKET_TYPE_MIRROR;
    }

    if (flags != NULL) {
        *flags = soc_mem_field32_get(unit, CPU_COS_MAPm, &entry, TRUNCATE_CPU_COPYf) ?
                 BCM_RX_COPYTOCPU_TRUNCATE : 0;
    }

    *prio_mask =
        soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt, &cpu_cos_map_mask_entry, INT_PRIf);
    *prio =
        soc_format_field32_get(unit, CPU_COS_MAP_KEYfmt, &cpu_cos_map_key_entry, INT_PRIf);


    length = soc_mem_field_length(unit, CPU_COS_MAPm, KEYf);
    while (length > 32) {
        field_size[word_count] = 32;
        word_count++;
        length -= 32;
    }
    field_size[word_count] = length;
    word_count++;

    soc_mem_field_get(unit, CPU_COS_MAPm, entry, MASKf, mask);
    soc_mem_field_get(unit, CPU_COS_MAPm, entry, KEYf, key);

    /* 4bits INT_PRI + 1bit RESERVED_MIRR_PKT + 2bits SW_PKT_TYPE */
    bit_skip = 0x7;

    sal_memset(enc_mask, 0, sizeof(enc_mask));
    sal_memset(enc_key, 0, sizeof(enc_key));
    sal_memset(bit_key, 0, sizeof(bit_key));
    for (word_idx = 0; word_idx < word_count; word_idx++) {
        bit_count = field_size[word_idx];
        bit_idx = (word_idx == 0 ? bit_skip:0);
        for (; bit_idx < bit_count; bit_idx++) {
            uint32 maskbitset;
            uint32 keybitset;

            maskbitset = mask[word_idx] & (1 << bit_idx);
            keybitset = key[word_idx] & (1 << bit_idx);
            key_idx = word_idx * 32 + bit_idx;
            key_idx -= bit_skip;
            if (key_idx >= key_table_len) {
                break;
            }

           for (table_idx = 0; table_idx < enc_count; table_idx++) {
               if (cpu_cos_map_key[key_idx] == cpu_opcode_enc[table_idx][0]) {
                       if (maskbitset) { /* should always be set */
                       enc_mask[table_idx] |= (1 << bit_key[table_idx]);
                       }
                       if (keybitset) {
                       enc_key[table_idx] |= (1 << bit_key[table_idx]);
                   }
                   bit_key[table_idx]++;
               }
           }
           if (maskbitset) {
               BCM_RX_REASON_SET(*reasons_mask, cpu_cos_map_key[key_idx]);
           }
           if (keybitset) {
               BCM_RX_REASON_SET(*reasons, cpu_cos_map_key[key_idx]);
           }
       }
    }

    for (table_idx = 0; table_idx < enc_count; table_idx++) {
        idx = enc_key[table_idx];
        if (idx > 0) {
                BCM_RX_REASON_SET(*reasons, cpu_opcode_enc[table_idx][idx]);
            }
        }

#undef KEY_ENTRY_WORDS_MAX
    return BCM_E_NONE;
}

/* Set the rx cosq mapping for Tomahawk3 */
int
_bcm_th3_rx_cosq_mapping_set(int unit, int index,
                            bcm_rx_reasons_t reasons, bcm_rx_reasons_t
                            reasons_mask,
                            uint8 int_prio, uint8 int_prio_mask,
                            uint32 packet_type, uint32 packet_type_mask,
                            bcm_cos_queue_t cosq, uint32 flags)
{
#define KEY_ENTRY_WORDS_MAX 6
    bcm_rx_reason_t *cpu_cos_map_key, *cpu_opcode_enc[KEY_ENTRY_WORDS_MAX] = {0};
    int key_table_len, enc_table_len[3], enc_count;
    int word_idx, bit_idx, key_idx, table_idx, enc_idx, word_count;
    uint32 mask[KEY_ENTRY_WORDS_MAX] = {0}, key[KEY_ENTRY_WORDS_MAX] = {0}, enc_key, field_size[KEY_ENTRY_WORDS_MAX] = {0};
    bcm_rx_reason_t  reason_enum, enc_enum;
    bcm_rx_reasons_t reasons_remain;
    cpu_cos_map_entry_t entry;
    uint8 sw_pkt_type_key = 0;
    uint8 sw_pkt_type_mask = 0;
    uint32 bit_skip = 0;
    uint32 cpu_cos_map_mask_entry[SOC_MAX_MEM_WORDS];
    uint32 cpu_cos_map_key_entry[SOC_MAX_MEM_WORDS];
    uint32 final_int_prio, final_int_prio_mask;
    int length = 0;
    uint32 cpu_cos_config;

    word_count = 0;
    bit_idx = 0;

    sal_memset (&cpu_cos_map_mask_entry, 0, sizeof(cpu_cos_map_mask_entry));
    sal_memset (&cpu_cos_map_key_entry, 0, sizeof(cpu_cos_map_key_entry));

    BCM_IF_ERROR_RETURN
        (_bcm_rx_reason_table_get(unit, &cpu_cos_map_key, &key_table_len,
                                  cpu_opcode_enc, enc_table_len, &enc_count));

    /* Verify COSQ */
    if (cosq >= _bcm_tomahawk3_rx_cpu_max_cosq) {
        return BCM_E_PARAM;
    }

    /* Verify the index */
    if (index < soc_mem_index_min(unit, CPU_COS_MAPm) ||
        index > soc_mem_index_max(unit, CPU_COS_MAPm)) {
        return BCM_E_PARAM;
    }

    /* Verify the packet type */
    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_key |= 1;
    }
    if (packet_type & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_key |= 2;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST) {
        sw_pkt_type_mask |= 1;
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_SWITCHED) {
        sw_pkt_type_mask |= 2;
    }
    sw_pkt_type_key &= sw_pkt_type_mask;

    if ((sw_pkt_type_mask == 0x1) ||
        ((sw_pkt_type_mask != 0) && (sw_pkt_type_key == 0x1))) {
        /* Hw doesn't support these cases */
        return BCM_E_PARAM;
    }

    length = soc_mem_field_length(unit, CPU_COS_MAPm, KEYf);
    while (length > 32) {
        field_size[word_count] = 32;
        word_count++;
        length -= 32;
    }
    field_size[word_count] = length;
    word_count++;

    /* 4bits INT_PRI + 1bit RESERVED_MIRR_PKT + 2bits SW_PKT_TYPE */
    bit_skip = 0x7;

    reasons_remain = reasons_mask;
    enc_enum = -1;
    enc_key = 0;
    word_idx = 0;
    bit_idx = bit_skip;
    sal_memset(mask, 0, sizeof(mask));
    sal_memset(key, 0, sizeof(key));
    for (key_idx = 0; key_idx < key_table_len; key_idx++, bit_idx++) {
        if (bit_idx == field_size[word_idx]) {
            bit_idx = 0;
            word_idx++;
            if (word_idx == word_count) {
                /* Trap implementation error */
                return BCM_E_INTERNAL;
            }
        }

        reason_enum = cpu_cos_map_key[key_idx];
        if (reason_enum == enc_enum) { /* following bit of encoding */
            mask[word_idx] |= 1 << bit_idx;
            key[word_idx] |= (enc_key & 1) << bit_idx;
            enc_key >>= 1;
            continue;
        }

        /* Find reason being set */
        if (!BCM_RX_REASON_GET(reasons_mask, reason_enum)) {
            continue;
        }
        BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);

        for (table_idx = 0; table_idx < enc_count; table_idx++) {
            if (reason_enum != cpu_opcode_enc[table_idx][0]) {
                continue;
            }

            enc_enum = reason_enum;
            for (enc_idx = 1; enc_idx < enc_table_len[table_idx]; enc_idx++) {
                reason_enum = cpu_opcode_enc[table_idx][enc_idx];
                if (!BCM_RX_REASON_GET(reasons, reason_enum)) {
                    BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);
                    continue;
                } else if (enc_key != 0) { /* multiple reasons supplied */
                    return BCM_E_PARAM;
                }
                BCM_RX_REASON_CLEAR(reasons_remain, reason_enum);
                enc_key = enc_idx;
            }
            mask[word_idx] |= 1 << bit_idx;
            key[word_idx] |= (enc_key & 1) << bit_idx;
            enc_key >>= 1;
            break;
        }
        if (table_idx == enc_count) {
            mask[word_idx] |= 1 << bit_idx;
            if (BCM_RX_REASON_GET(reasons, reason_enum)) {
                key[word_idx] |= 1 << bit_idx;
            }
        }
    }

    /* check whether there are reasons unsupported */
    if (!BCM_RX_REASON_IS_NULL(reasons_remain)) {
        return BCM_E_PARAM;
    }

    /* Now zero entry */
    sal_memset (&entry, 0, sizeof(cpu_cos_map_entry_t));

    /* Program the key and mask when reasons_mask is not NULL */
    if (!BCM_RX_REASON_IS_NULL(reasons_mask)) {
        soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry, MASKf, mask);
        soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry, KEYf, key);
    }
    soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, MASKf, cpu_cos_map_mask_entry);
    soc_mem_field_get(unit, CPU_COS_MAPm, (uint32 *)&entry, KEYf, cpu_cos_map_key_entry);

    /* Program the packet type */

    if (packet_type_mask & (BCM_RX_COSQ_PACKET_TYPE_NON_UNICAST |
                         BCM_RX_COSQ_PACKET_TYPE_SWITCHED)) {
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_mask_entry,
                            SW_PKT_TYPEf, sw_pkt_type_mask);
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_key_entry,
                            SW_PKT_TYPEf, sw_pkt_type_key);
    }

    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_key_entry,
                            RESERVED_MIRR_PKTf, 1);
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_mask_entry,
                            RESERVED_MIRR_PKTf, 1);
    }

    /* Handle priority when int_prio_mask != 0 */
    if (int_prio_mask) {
        final_int_prio = int_prio & 0xf;
        final_int_prio_mask = int_prio_mask & 0xf;
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_key_entry,
                            INT_PRIf, final_int_prio);
        soc_format_field32_set(unit, CPU_COS_MAP_KEYfmt, cpu_cos_map_mask_entry,
                            INT_PRIf, final_int_prio_mask);
    }

    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, COSf, cosq);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, VALIDf, 1);
    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, STRENGTHf,
                          (soc_mem_index_max(unit, CPU_COS_MAPm) - index));

    soc_mem_field32_set(unit, CPU_COS_MAPm, &entry, TRUNCATE_CPU_COPYf,
                        (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
    soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry,
                            KEYf, &cpu_cos_map_key_entry[0]);
    soc_mem_field_set(unit, CPU_COS_MAPm, (uint32 *)&entry,
                            MASKf, &cpu_cos_map_mask_entry[0]);

    /* write to memory */
    SOC_IF_ERROR_RETURN(WRITE_CPU_COS_MAPm(unit, MEM_BLOCK_ALL, index,
                                               &entry));

    if (BCM_RX_REASON_GET(reasons, bcmRxReasonSampleDest)) {
        BCM_IF_ERROR_RETURN
            (READ_EGR_SFLOW_CPU_COS_CONFIGr(unit, &cpu_cos_config));
        soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                          COSf, cosq);
        soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                          STRENGTHf,
                          (soc_mem_index_max(unit, CPU_COS_MAPm) - index));
        soc_reg_field_set(unit, EGR_SFLOW_CPU_COS_CONFIGr, &cpu_cos_config,
                          TRUNCATE_CPU_COPYf,
                          (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
        BCM_IF_ERROR_RETURN
            (WRITE_EGR_SFLOW_CPU_COS_CONFIGr(unit, cpu_cos_config));
    }
    if (packet_type_mask & BCM_RX_COSQ_PACKET_TYPE_MIRROR) {
        BCM_IF_ERROR_RETURN
            (READ_MIRROR_CPU_COS_CONFIGr(unit, &cpu_cos_config));
        soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                          COSf, cosq);
        soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                          STRENGTHf,
                          (soc_mem_index_max(unit, CPU_COS_MAPm) - index));
        soc_reg_field_set(unit, MIRROR_CPU_COS_CONFIGr, &cpu_cos_config,
                          TRUNCATE_CPU_COPYf,
                          (flags & BCM_RX_COPYTOCPU_TRUNCATE) ? 1 : 0);
        BCM_IF_ERROR_RETURN
            (WRITE_MIRROR_CPU_COS_CONFIGr(unit, cpu_cos_config));
    }
#undef KEY_ENTRY_WORDS_MAX

    return BCM_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

/*
 * Function:
 *      bcm_rx_cosq_mapping_set
 * Purpose:
 *      Set the COSQ mapping to map qualified packets to the a CPU cos queue.
 * Parameters: 
 *      unit - Unit reference
 *      index - Index into COSQ mapping table (0 is lowest match priority)
 *      reasons - packet "reasons" bitmap
 *      reasons_mask - mask for packet "reasons" bitmap
 *      int_prio - internal priority value
 *      int_prio_mask - mask for internal priority value
 *      packet_type - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *      packet_type_mask - mask for packet type bitmap
 *      cosq - CPU cos queue
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_set(int unit, int index, 
                            bcm_rx_reasons_t reasons,
                            bcm_rx_reasons_t reasons_mask, 
                            uint8 int_prio, uint8 int_prio_mask,
                            uint32 packet_type, uint32 packet_type_mask,
                            bcm_cos_queue_t cosq)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
   if (SOC_IS_TOMAHAWK3(unit)) {
       return _bcm_th3_rx_cosq_mapping_set(unit, index,
                                           reasons, reasons_mask,
                                           int_prio, int_prio_mask,
                                           packet_type, packet_type_mask,
                                           cosq, 0);
   }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_rx_reason_overlay)) {
       return _bcm_tr3_rx_cosq_mapping_set(unit, index,
                                           reasons, reasons_mask,
                                           int_prio, int_prio_mask, 
                                           packet_type, packet_type_mask,
                                           cosq);
   }
#endif
#ifdef BCM_TRX_SUPPORT
   if (SOC_IS_TRX(unit)) {
       return _bcm_trx_rx_cosq_mapping_set(unit, index,
                                           reasons, reasons_mask,
                                           int_prio, int_prio_mask, 
                                           packet_type, packet_type_mask,
                                           cosq, 0);
   }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rx_cosq_mapping_get
 * Purpose:
 *      Get the COSQ mapping at the specified index
 * Parameters:
 *      unit - Unit reference
 *      index - Index into COSQ mapping table (0 is lowest match priority)
 *      reasons - packet "reasons" bitmap
 *      reasons_mask - mask for packet "reasons" bitmap
 *      int_prio - internal priority value
 *      int_prio_mask - mask for internal priority value
 *      packet_type - packet type bitmap (BCM_RX_COSQ_PACKET_TYPE_*)
 *      packet_type_mask - mask for packet type bitmap 
 *      cosq - CPU cos queue 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_get(int unit, int index, 
                            bcm_rx_reasons_t *reasons, bcm_rx_reasons_t *reasons_mask, 
                            uint8 *int_prio, uint8 *int_prio_mask,
                            uint32 *packet_type, uint32 *packet_type_mask,
                            bcm_cos_queue_t *cosq)
{
#ifdef BCM_TOMAHAWK3_SUPPORT
   if (SOC_IS_TOMAHAWK3(unit)) {
       return _bcm_th3_rx_cosq_mapping_get(unit, index, reasons, reasons_mask,
                                           int_prio, int_prio_mask,
                                           packet_type, packet_type_mask,
                                           cosq, NULL);
   }
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_rx_reason_overlay)) {
       return _bcm_tr3_rx_cosq_mapping_get(unit, index, reasons, reasons_mask,
                                           int_prio, int_prio_mask, 
                                           packet_type, packet_type_mask,
                                           cosq);
   }
#endif
#ifdef BCM_TRX_SUPPORT
   if (SOC_IS_TRX(unit)) {
       return _bcm_trx_rx_cosq_mapping_get(unit, index, reasons, reasons_mask,
                                           int_prio, int_prio_mask, 
                                           packet_type, packet_type_mask,
                                           cosq, NULL);
   }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rx_cosq_mapping_delete
 * Purpose:
 *      Delete the COSQ mapping at the specified index
 * Parameters: 
 *      unit - Unit reference
 *      index - Index into COSQ mapping table 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_delete(int unit, int index)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rx_cosq_mapping_delete(unit, index);
    }
#endif
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_rx_cosq_mapping_extended_set
 * Purpose:
 *      Set the COSQ mapping to map qualified packets to the a CPU cos queue.
 * Parameters: 
 *      unit - Unit reference
 *      rx_cosq_mapping - rx cosq mapping ptr
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_extended_set(int unit, uint32 options, 
                                                      bcm_rx_cosq_mapping_t *rx_cosq_mapping)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_truncate_cpu_copy)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
       if (SOC_IS_TOMAHAWK3(unit)) {
            return _bcm_th3_rx_cosq_mapping_set(unit, rx_cosq_mapping->index,
                                                rx_cosq_mapping->reasons,
                                                rx_cosq_mapping->reasons_mask,
                                                rx_cosq_mapping->int_prio,
                                                rx_cosq_mapping->int_prio_mask,
                                                rx_cosq_mapping->packet_type,
                                                rx_cosq_mapping->packet_type_mask,
                                                rx_cosq_mapping->cosq,
                                                rx_cosq_mapping->flags);
       }
       else
#endif
       {
        return _bcm_trx_rx_cosq_mapping_set(unit, rx_cosq_mapping->index,
                                            rx_cosq_mapping->reasons, 
                                            rx_cosq_mapping->reasons_mask,
                                            rx_cosq_mapping->int_prio, 
                                            rx_cosq_mapping->int_prio_mask, 
                                            rx_cosq_mapping->packet_type, 
                                            rx_cosq_mapping->packet_type_mask,
                                            rx_cosq_mapping->cosq, 
                                            rx_cosq_mapping->flags);
       }

   }
#endif 
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_rx_cosq_mapping_extended_get
 * Purpose:
 *      Get the COSQ mapping at the specified index
 * Parameters:
 *      unit - Unit reference
 *      rx_cosq_mapping - rx cosq mapping ptr
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_cosq_mapping_extended_get(int unit, bcm_rx_cosq_mapping_t *rx_cosq_mapping)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_truncate_cpu_copy)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            return _bcm_th3_rx_cosq_mapping_get(unit, rx_cosq_mapping->index,
                                                &rx_cosq_mapping->reasons,
                                                &rx_cosq_mapping->reasons_mask,
                                                &rx_cosq_mapping->int_prio,
                                                &rx_cosq_mapping->int_prio_mask,
                                                &rx_cosq_mapping->packet_type,
                                                &rx_cosq_mapping->packet_type_mask,
                                                &rx_cosq_mapping->cosq,
                                                &rx_cosq_mapping->flags);
        }
        else
#endif
        {
        return _bcm_trx_rx_cosq_mapping_get(unit, rx_cosq_mapping->index, 
                                            &rx_cosq_mapping->reasons, 
                                            &rx_cosq_mapping->reasons_mask,
                                            &rx_cosq_mapping->int_prio, 
                                            &rx_cosq_mapping->int_prio_mask, 
                                            &rx_cosq_mapping->packet_type, 
                                            &rx_cosq_mapping->packet_type_mask,
                                            &rx_cosq_mapping->cosq, 
                                            &rx_cosq_mapping->flags);
        }

   }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_rx_cosq_mapping_init
 * Purpose:
 *      Delete all the COSQ mapping
 * Parameters: 
 *      unit - Unit reference
 * Returns:
 *      BCM_E_XXX
 */
#ifdef BCM_TRX_SUPPORT
STATIC int _bcm_esw_rx_cosq_mapping_init(int unit)
{
    if (SOC_IS_TRX(unit)) {
        return _bcm_trx_rx_cosq_mapping_init(unit);
    }
    return BCM_E_UNAVAIL;
}
#endif


/*  
 * Function:
 *      bcm_rx_queue_channel_set
 * Purpose:
 *      Assign a RX channel to a cosq 
 * Parameters:
 *      unit - Unit reference
 *      queue_id - CPU cos queue index (0 - (max cosq - 1)) 
 *                                      (Negative for all)
 *      chan_id - channel index (0 - (SOC_CMCS_NUM(unit) * BCM_RX_CHANNELS-1))
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_queue_channel_set (int unit, bcm_cos_queue_t queue_id, 
                              bcm_rx_chan_t chan_id)
{
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    uint32 max_rx_channels = 0;

    soc_dma_max_rx_channels_get(unit, &max_rx_channels);

    if (SOC_CMCS_NUM(unit) != 0 &&
        chan_id >= (SOC_CMCS_NUM(unit) * max_rx_channels)) {
        /* API access is constrained to only the PCI host channels. */
        return BCM_E_PARAM;
    }
#endif

    return (_bcm_common_rx_queue_channel_set(unit, queue_id, 
	  				     chan_id));
}

/*  
 * Function:
 *      bcm_rx_queue_channel_get
 * Purpose:
 *      Get the associated rx channel with a given cosq
 * Parameters:
 *      unit - Unit reference
 *      queue_id - CPU cos queue index (0 - (max cosq - 1)) 
 *      chan_id - channel index (0 - (BCM_RX_CHANNELS-1))
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_rx_queue_channel_get(int unit, bcm_cos_queue_t queue_id, 
                             bcm_rx_chan_t *chan_id)
{
    return (_bcm_common_rx_queue_channel_get(unit, queue_id, 
	  				     chan_id));
}

/*
 * Function:
 *      bcm_rx_active
 * Purpose:
 *      Return boolean as to whether unit is running
 * Parameters:
 *      unit - StrataXGS to check
 * Returns:
 *      Boolean:   TRUE if unit is running.
 * Notes:
 */

int
bcm_esw_rx_active(int unit)
{
    return (_bcm_common_rx_active(unit));
}


/*
 * Function:
 *      bcm_rx_running_channels_get
 * Purpose:
 *      Returns a bitmap indicating which channels are active
 * Parameters:
 *      unit       - Which unit to operate on
 * Returns:
 *      Bitmap of active channels
 * Notes:
 */

int
bcm_esw_rx_channels_running(int unit, uint32 *channels)
{
    return (_bcm_common_rx_channels_running(unit, channels));
}

/*
 * Function:
 *      bcm_rx_alloc
 * Purpose:
 *      Gateway to configured RX allocation function
 * Parameters:
 *      unit - Unit reference
 *      pkt_size - Packet size, see notes.
 *      flags - Used to set up packet flags
 * Returns:
 *      Pointer to new packet buffer or NULL if cannot alloc memory
 * Notes:
 *      Although the packet size is normally configured per unit,
 *      the option of using a different size is given here.  If
 *      pkt_size <= 0, then the default packet size for the unit
 *      is used.
 */

int
bcm_esw_rx_alloc(int unit, int pkt_size, uint32 flags, void **buf)
{
    return (_bcm_common_rx_alloc(unit, pkt_size, flags, buf));
}


/*
 * Function:
 *      bcm_rx_free
 * Purpose:
 *      Gateway to configured RX free function.  Generally, packet
 *      buffer was allocated with bcm_rx_alloc.
 * Parameters:
 *      unit - Unit reference
 *      pkt - Packet to free
 * Returns:
 * Notes:
 *      In particular, packets stolen from RX with BCM_RX_HANDLED_OWNED
 *      should use this to free packets.
 */

int
bcm_esw_rx_free(int unit, void *pkt_data)
{
    return (_bcm_common_rx_free(unit, pkt_data));
}


/*
 * Function:
 *      bcm_rx_free_enqueue
 * Purpose:
 *      Queue a packet to be freed by the RX thread.
 * Parameters:
 *      unit - Unit reference
 *      pkt - Packet to free
 * Returns:
 * Notes:
 *      This may be called in interrupt context to queue
 *      a packet to be freed.
 *
 *      Assumes pkt_data is 32-bit aligned.
 *      Uses the first word of the freed data as a "next" pointer
 *      for the free list.
 */
int
bcm_esw_rx_free_enqueue(int unit, void *pkt_data)
{
    return (_bcm_common_rx_free_enqueue(unit, pkt_data));
}


/****************************************************************
 *
 * Global (all COS) and per COS rate limiting configuration
 */


/*
 * Functions:
 *      bcm_rx_burst_set, get; bcm_rx_rate_set, get
 *      bcm_rx_cos_burst_set, get; bcm_rx_cos_rate_set, get;
 *      bcm_rx_cos_max_len_set, get
 * Purpose:
 *      Get/Set the global and per COS limits:
 *           rate:      Packets/second
 *           burst:     Packets (max tokens in bucket)
 *           max_len:   Packets (max permitted in queue).
 * Parameters:
 *      unit - Unit reference
 *      cos - For per COS functions, which COS queue affected
 *      pps - Rate in packets per second (OUT for get functions)
 *      burst - Burst rate for the system in packets (OUT for get functions)
 *      max_q_len - Burst rate for the system in packets (OUT for get functions)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      PPS must be >= 0 and
 *      Max queue length must be >= 0;
 *          otherwise param error.
 *
 *      PPS == 0 -> rate limiting disabled.
 *      max_q_len == 0 -> no limit on queue length (not recommended)
 */

int
bcm_esw_rx_rate_set(int unit, int pps)
{
    RX_INIT_CHECK(unit);

    if (pps < 0) {
        return BCM_E_PARAM;
    }
    RX_PPS(unit) = pps;

#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
        return bcm_rx_cos_rate_set(unit, BCM_RX_COS_ALL, pps);
    }
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (RX_IS_LOCAL(unit) && SOC_UNIT_VALID(unit)) {
        if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_TRIUMPH2_SUPPORT
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit)) {
                return bcm_rx_cos_rate_set(unit, BCM_RX_COS_ALL, pps);
            } else if (SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit) || 
                SOC_IS_TRIUMPH3(unit)) {
                int burst;
                BCM_IF_ERROR_RETURN(bcm_esw_rx_burst_get(unit, &burst));

                if (SOC_IS_TITAN(unit)) {
                    return bcm_tr_port_pps_rate_egress_set(unit, CMIC_PORT(unit),
                                                       (uint32)pps, (uint32)burst);
                } else {
                    return bcm_esw_port_rate_egress_pps_set(unit, CMIC_PORT(unit),
                                                        (uint32)pps, (uint32)burst);
                }
            }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit) && \
                !(SOC_IS_HURRICANEX(unit)||SOC_IS_GREYHOUND(unit)||
                  SOC_IS_GREYHOUND2(unit))) {
                return bcm_tr_cosq_port_pps_set(unit, CMIC_PORT(unit),
                                               -1, pps);
            }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                return bcm_fb2_cosq_port_pps_set(unit, CMIC_PORT(unit),
                                                 -1, pps);
            }
#endif /* BCM_FIREBOLT2_SUPPORT */
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

    return BCM_E_NONE;
}

int
bcm_esw_rx_rate_get(int unit, int *pps)
{
    return (_bcm_common_rx_rate_get(unit, pps));
}

int 
bcm_esw_rx_cpu_rate_set(int unit, int pps)
{
    return (_bcm_common_rx_cpu_rate_set(unit, pps));
}

int 
bcm_esw_rx_cpu_rate_get(int unit, int *pps)
{
    return (_bcm_common_rx_cpu_rate_get(unit, pps));
}


int
bcm_esw_rx_burst_set(int unit, int burst)
{
    RX_INIT_CHECK(unit);

    RX_BURST(unit) = burst;
    RX_TOKENS(unit) = burst;

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (RX_IS_LOCAL(unit) && SOC_UNIT_VALID(unit)) {
        if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_TRIUMPH2_SUPPORT
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit) || SOC_IS_KATANAX(unit)) {
                return bcm_rx_cos_burst_set(unit, BCM_RX_COS_ALL, burst);
            } else 
#if defined(BCM_TRIDENT2_SUPPORT)
            if (SOC_IS_TD2_TT2(unit)) {
                int pps;
                BCM_IF_ERROR_RETURN(bcm_esw_rx_rate_get(unit, &pps));
                return mbcm_driver[unit]->mbcm_port_rate_egress_set
                    (unit, CMIC_PORT(unit), pps, burst,
                     _BCM_PORT_RATE_PPS_MODE);
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            if (SOC_IS_TD_TT(unit)) {
                int pps;
                BCM_IF_ERROR_RETURN(bcm_esw_rx_rate_get(unit, &pps));
                return bcm_tr_port_pps_rate_egress_set(unit, CMIC_PORT(unit),
                                                       (uint32)pps, (uint32)burst);
            }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit) && \
                !(SOC_IS_HURRICANEX(unit)||SOC_IS_GREYHOUND(unit) ||
                SOC_IS_GREYHOUND2(unit))) {
#ifdef BCM_TRIUMPH3_SUPPORT
                if (SOC_IS_TRIUMPH3(unit)) {
                    int pps;
                    BCM_IF_ERROR_RETURN(bcm_esw_rx_rate_get(unit, &pps));
                    return mbcm_driver[unit]->mbcm_port_rate_egress_set
                        (unit, CMIC_PORT(unit), pps, burst,
                         _BCM_PORT_RATE_PPS_MODE);
                } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
                {
                    return bcm_tr_cosq_port_burst_set(unit, CMIC_PORT(unit), -1,
                                                  burst);
                }
            }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                return bcm_fb2_cosq_port_burst_set(unit, CMIC_PORT(unit), -1,
                                                   burst);
            }
#endif /* BCM_FIREBOLT2_SUPPORT */
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

    return BCM_E_NONE;
}

int
bcm_esw_rx_burst_get(int unit, int *burst)
{
    return (_bcm_common_rx_burst_get(unit, burst));
}


int
bcm_esw_rx_cos_rate_set(int unit, int cos, int pps)
{
    int i;

    RX_INIT_CHECK(unit);
    if (!LEGAL_COS(cos) || pps < 0) {
        return BCM_E_PARAM;
    }

    if (cos > RX_QUEUE_MAX(unit)) {
        return BCM_E_PARAM;
    }

    if (cos == BCM_RX_COS_ALL) {
        for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
            RX_COS_PPS(unit, i) = pps;
        }
    } else {
        RX_COS_PPS(unit, cos) = pps;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (RX_IS_LOCAL(unit) && SOC_UNIT_VALID(unit)) {
        if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_mn_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_mn_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_APACHE_SUPPORT
            if (SOC_IS_APACHE(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_ap_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_ap_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_th3_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_th3_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_th_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_th_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_hr4_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                       pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_hr4_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                   pps));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_hx5_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                       pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_hx5_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                   pps));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td3_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                       pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_td3_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                   pps));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
            if (SOC_IS_FIREBOLT6(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_fb6_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                       pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_fb6_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                   pps));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_TRIDENT2_SUPPORT
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td2_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_td2_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
#else
                return BCM_E_UNAVAIL;
#endif
            } else 
            if (SOC_IS_TD_TT(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_td_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr3_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr2_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr2_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_HURRICANE1_SUPPORT)
            if (SOC_IS_HURRICANE(unit)) {
                if (cos >= NUM_CPU_COSQ(unit)) {
                    return BCM_E_PARAM;
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_KATANA_SUPPORT
            if (SOC_IS_KATANA(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_kt_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_kt2_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_HURRICANE2_SUPPORT
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_hr2_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_hr2_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr_cosq_port_pps_set(unit, CMIC_PORT(unit), i,
                                                      pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                                  pps));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_fb2_cosq_port_pps_set(unit, CMIC_PORT(unit),
                                                       i, pps));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_fb2_cosq_port_pps_set(unit, CMIC_PORT(unit),
                                                   cos, pps));
                }
                return BCM_E_NONE;
            }
 #endif /* BCM_FIREBOLT2_SUPPORT */
       }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    return BCM_E_NONE;
}

int
bcm_esw_rx_cos_rate_get(int unit, int cos, int *pps)
{
    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    if ( (cos < 0) || (cos > RX_QUEUE_MAX(unit))) {
        return BCM_E_PARAM;
    }

    return (_bcm_common_rx_cos_rate_get(unit, cos, pps));
}

int
bcm_esw_rx_cos_burst_set(int unit, int cos, int burst)
{
    rx_queue_t *queue;
    int i;

    RX_INIT_CHECK(unit);
    if (!LEGAL_COS(cos)) {
        return BCM_E_PARAM;
    }

    if (cos > RX_QUEUE_MAX(unit)) {
        return BCM_E_PARAM;
    }

    if (cos == BCM_RX_COS_ALL) {
        for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
            queue = RX_QUEUE(unit, i);
            queue->burst = burst;
            queue->tokens = burst;
        }
    } else {
        queue = RX_QUEUE(unit, cos);
        queue->burst = burst;
        queue->tokens = burst;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (RX_IS_LOCAL(unit) && SOC_UNIT_VALID(unit)) {
        if (soc_feature(unit, soc_feature_packet_rate_limit)) {
#ifdef BCM_MONTEREY_SUPPORT
             if (SOC_IS_MONTEREY(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_mn_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_mn_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_APACHE_SUPPORT
             if (SOC_IS_APACHE(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_ap_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_ap_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_th3_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_th3_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_th_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_th_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
#ifdef BCM_HURRICANE4_SUPPORT
                    if (SOC_IS_HURRICANE4(unit)) {
                        if (cos == BCM_RX_COS_ALL) {
                            for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                                BCM_IF_ERROR_RETURN
                                    (bcm_hr4_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                                 i, burst));
                            }
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_hr4_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                             burst));
                        }
                        return BCM_E_NONE;
                    } else
#endif
#ifdef BCM_HELIX5_SUPPORT
                    if (SOC_IS_HELIX5(unit)) {
                        if (cos == BCM_RX_COS_ALL) {
                            for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                                BCM_IF_ERROR_RETURN
                                    (bcm_hx5_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                                 i, burst));
                            }
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_hx5_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                             burst));
                        }
                        return BCM_E_NONE;
                    } else
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
                    if (SOC_IS_FIREBOLT6(unit)) {
                        if (cos == BCM_RX_COS_ALL) {
                            for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                                BCM_IF_ERROR_RETURN
                                    (bcm_fb6_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                                 i, burst));
                            }
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_fb6_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                             burst));
                        }
                        return BCM_E_NONE;
                    } else
#endif /* BCM_FIREBOLT6_SUPPORT */

                    {
                        if (cos == BCM_RX_COS_ALL) {
                            for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                                BCM_IF_ERROR_RETURN
                                    (bcm_td3_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                                 i, burst));
                            }
                        } else {
                            BCM_IF_ERROR_RETURN
                                (bcm_td3_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                             burst));
                        }
                        return BCM_E_NONE;
                    }
                } else
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_TRIDENT2_SUPPORT
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td2_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_td2_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
#else
                return BCM_E_UNAVAIL;
#endif
            } else 
            if (SOC_IS_TD_TT(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_td_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_td_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr3_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr2_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr2_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
             if (SOC_IS_KATANA(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_kt_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_KATANA2_SUPPORT
             if (SOC_IS_KATANA2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_kt2_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_kt2_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_HURRICANE2_SUPPORT
            if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
                SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_hr2_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                        i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_hr2_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                                    burst));
                }
                return BCM_E_NONE;
            }
#endif
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) { 
#ifdef BCM_HURRICANE1_SUPPORT
                if (SOC_IS_HURRICANE(unit)) {
                    if (cos >= NUM_CPU_COSQ(unit)) {
                        return BCM_E_PARAM;
                    }
                } else
#endif
                {
                    if (cos == BCM_RX_COS_ALL) {
                        for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                            BCM_IF_ERROR_RETURN
                                (bcm_tr_cosq_port_burst_set(unit, 
                                         CMIC_PORT(unit), i, burst));
                        }
                    } else {
                        BCM_IF_ERROR_RETURN
                            (bcm_tr_cosq_port_burst_set(unit, 
                                         CMIC_PORT(unit), cos, burst));
                    }
                }
                return BCM_E_NONE;
            }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                if (cos == BCM_RX_COS_ALL) {
                    for (i = 0; i <= RX_QUEUE_MAX(unit); i++) {
                        BCM_IF_ERROR_RETURN
                            (bcm_fb2_cosq_port_burst_set(unit,
                                                         CMIC_PORT(unit),
                                                         i, burst));
                    }
                } else {
                    BCM_IF_ERROR_RETURN
                        (bcm_fb2_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                     cos, burst));
                }
                return BCM_E_NONE;
            }
#endif /* BCM_FIREBOLT2_SUPPORT */
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

    return BCM_E_NONE;
}

int
bcm_esw_rx_cos_burst_get(int unit, int cos, int *burst)
{
    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    if ( (cos < 0) || (cos > RX_QUEUE_MAX(unit))) {
        return BCM_E_PARAM;
    }

    return (_bcm_common_rx_cos_burst_get(unit, cos, burst));
}

int
bcm_esw_rx_cos_max_len_set(int unit, int cos, int max_q_len)
{
    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    if (!LEGAL_COS(cos)) {
        return BCM_E_PARAM;
    }

    if (cos > RX_QUEUE_MAX(unit)) {
        return BCM_E_PARAM;
    }

    return (_bcm_common_rx_cos_max_len_set(unit, cos, max_q_len));
}

int
bcm_esw_rx_cos_max_len_get(int unit, int cos, int *max_q_len)
{
    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    if ( (cos < 0) || (cos > RX_QUEUE_MAX(unit))) {
        return BCM_E_PARAM;
    }

    return (_bcm_common_rx_cos_max_len_get(unit, cos, max_q_len));
}

/****************************************************************
 *
 * RX Control
 */

STATIC int
_bcm_esw_rx_chan_flag_set(int unit, uint32 flag, int value)
{
    int chan;

    FOREACH_SETUP_CHANNEL(unit, chan) {
        if (value) {
            RX_CHAN_FLAGS(unit, chan) |= flag;
        } else {
            RX_CHAN_FLAGS(unit, chan) &= ~flag;
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_esw_rx_chan_flag_get(int unit, uint32 flag, int *value)
{
    int chan;

    FOREACH_SETUP_CHANNEL(unit, chan) {
        *value = RX_CHAN_FLAGS(unit, chan) & flag;
        break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_esw_rx_user_flag_set(int unit, uint32 flag, int value)
{
    if (value) {
        RX_USER_FLAGS(unit) |= flag;
    } else {
        RX_USER_FLAGS(unit) &= ~flag;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_esw_rx_user_flag_get(int unit, uint32 flag, int *value)
{
    *value = RX_USER_FLAGS(unit) & flag;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_rx_control_get(int unit, bcm_rx_control_t type, int *value)
 * Description:
 *      Get the status of specified RX feature.
 * Parameters:
 *      unit - Device number
 *      type - RX control parameter
 *      value - (OUT) Current value of control parameter
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */

int
bcm_esw_rx_control_get(int unit, bcm_rx_control_t type, int *value)
{
    int rv = BCM_E_UNAVAIL;

    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmRxControlCRCStrip:
        rv = _bcm_esw_rx_chan_flag_get(unit, BCM_RX_F_CRC_STRIP, value);
        break;
    case bcmRxControlVTagStrip:
        rv = _bcm_esw_rx_chan_flag_get(unit, BCM_RX_F_VTAG_STRIP, value);
        break;
    case bcmRxControlRateStall:
        rv = _bcm_esw_rx_chan_flag_get(unit, BCM_RX_F_RATE_STALL, value);
        break;
    case bcmRxControlMultiDCB:
        rv = _bcm_esw_rx_chan_flag_get(unit, BCM_RX_F_MULTI_DCB, value);
        break;
    case bcmRxControlOversizedOK:
        rv = _bcm_esw_rx_chan_flag_get(unit, BCM_RX_F_OVERSIZED_OK, value);
        break;
    case bcmRxControlIgnoreHGHeader:
        rv = _bcm_esw_rx_user_flag_get(unit, BCM_RX_F_IGNORE_HGHDR, value);
        break;
    case bcmRxControlIgnoreSLHeader:
        rv = _bcm_esw_rx_user_flag_get(unit, BCM_RX_F_IGNORE_SLTAG, value);
        break;
    default:
        /* unsupported flag */
        break;
    }
    return rv;
}

/*
 * Function:
 *      bcm_rx_control_set(int unit, bcm_rx_control_t type, int value)
 * Description:
 *      Enable/Disable specified RX feature.
 * Parameters:
 *      unit - Device number
 *      type - RX control parameter
 *      value - new value of control parameter
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */

int
bcm_esw_rx_control_set(int unit, bcm_rx_control_t type, int value)
{
    int rv = BCM_E_UNAVAIL;

    if (!RX_INIT_DONE(unit)) {
        return BCM_E_INIT;
    }

    switch (type) {
    case bcmRxControlCRCStrip:
        rv = _bcm_esw_rx_chan_flag_set(unit, BCM_RX_F_CRC_STRIP, value);
        break;
    case bcmRxControlVTagStrip:
        rv = _bcm_esw_rx_chan_flag_set(unit, BCM_RX_F_VTAG_STRIP, value);
        break;
    case bcmRxControlRateStall:
        rv = _bcm_esw_rx_chan_flag_set(unit, BCM_RX_F_RATE_STALL, value);
        break;
    case bcmRxControlMultiDCB:
        rv = _bcm_esw_rx_chan_flag_set(unit, BCM_RX_F_MULTI_DCB, value);
        break;
    case bcmRxControlOversizedOK:
        rv = _bcm_esw_rx_chan_flag_set(unit, BCM_RX_F_OVERSIZED_OK, value);
        break;
    case bcmRxControlIgnoreHGHeader:
        rv = _bcm_esw_rx_user_flag_set(unit, BCM_RX_F_IGNORE_HGHDR, value);
        break;
    case bcmRxControlIgnoreSLHeader:
        rv = _bcm_esw_rx_user_flag_set(unit, BCM_RX_F_IGNORE_SLTAG, value);
        break;
    default:
        /* unsupported flag */
        break;
    }
    return rv;
}

/*
 * Function:
 *      bcm_rx_remote_pkt_enqueue
 * Purpose:
 *      Enqueue a remote packet for normal RX processing
 * Parameters:
 *      unit          - The BCM unit in which queue the pkt is placed
 *      pkt           - The packet to enqueue
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_rx_remote_pkt_enqueue(int unit, bcm_pkt_t *pkt)
{
    return (_bcm_common_rx_remote_pkt_enqueue(unit, pkt));
}


/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_t_init
 * Purpose:
 *      Initialize a copy-to-cpu config entry
 * Parameters:
 *      copyToCpu_config  - (IN) Pointer to the copy-to-cpu config structure
 * Returns:
 *      BCM_E_*
 */

void bcm_esw_rx_CopyToCpu_config_t_init(
            bcm_rx_CopyToCpu_config_t *copyToCpu_config)
{
    return;
}

/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_add
 * Purpose:
 *      Add a copy-to-cpu entry
 * Parameters:
 *      unit              - (IN) device id.
 *      options           - (IN) options.
 *      copyToCpu_config  - (IN) Pointer to the copy-to-cpu config structure
 * Returns:
 *      BCM_E_*
 */

int bcm_esw_rx_CopyToCpu_config_add(int unit, uint32 options,
                                bcm_rx_CopyToCpu_config_t *copyToCpu_config)
{
    int rv = BCM_E_UNAVAIL;

#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        if (!RX_INIT_DONE(unit)) {
            return BCM_E_INIT;
        }

        RX_COPY_TO_CPU_LOCK(unit);
        rv = bcmi_xgs5_rx_CopyToCpu_config_add(unit, options,
                                copyToCpu_config);
        RX_COPY_TO_CPU_UNLOCK(unit);
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_get
 * Purpose:
 *      Get a copy-to-cpu entry
 * Parameters:
 *      unit              - (IN) device id.
 *      index             - (IN) Index of the entry.
 *      copyToCpu_config  - (OUT) Pointer to the copy-to-cpu config structure
 * Returns:
 *      BCM_E_*
 */

int bcm_esw_rx_CopyToCpu_config_get(int unit, int index,
                           bcm_rx_CopyToCpu_config_t *copyToCpu_config)
{
#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        return bcmi_xgs5_rx_CopyToCpu_config_get(unit, index,
                                                 copyToCpu_config);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_delete
 * Purpose:
 *      Delete a copy-to-cpu entry
 * Parameters:
 *      unit   - (IN) device id.
 *      index  - (IN) Index of the entry to delete
 * Returns:
 *      BCM_E_*
 */

int bcm_esw_rx_CopyToCpu_config_delete(int unit, int index)
{
    int rv = BCM_E_UNAVAIL;

#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        if (!RX_INIT_DONE(unit)) {
            return BCM_E_INIT;
        }

        RX_COPY_TO_CPU_LOCK(unit);
        rv = bcmi_xgs5_rx_CopyToCpu_config_delete(unit, index);
        RX_COPY_TO_CPU_UNLOCK(unit);
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_get_all
 * Purpose:
 *      Retrieve muultiple copy-to-cpu entries
 * Parameters:
 *      unit              - (IN) device id.
 *      entries_max       - (IN) Max entries to retrieve
 *      copyToCpu_config  - (OUT) Pointer to the copy-to-cpu config structure
 *      entries_count     - (OUT) Number of entries actually retrieved
 * Returns:
 *      BCM_E_*
 */

int bcm_esw_rx_CopyToCpu_config_get_all(int unit, int entries_max,
                                 bcm_rx_CopyToCpu_config_t *copyToCpu_config,
                                 int *entries_count)
{
#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        return bcmi_xgs5_rx_CopyToCpu_config_get_all(unit, entries_max,
                                 copyToCpu_config, entries_count);
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_rx_CopyToCpu_config_delete_all
 * Purpose:
 *      Delete all copy-to-cpu entries configured
 * Parameters:
 *      unit              - (IN) device id.
 * Returns:
 *      BCM_E_*
 */

int bcm_esw_rx_CopyToCpu_config_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined (BCM_EP_REDIRECT_VERSION_2)
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        if (!RX_INIT_DONE(unit)) {
            return BCM_E_INIT;
        }

        RX_COPY_TO_CPU_LOCK(unit);
        rv = bcmi_xgs5_rx_CopyToCpu_config_delete_all(unit);
        RX_COPY_TO_CPU_UNLOCK(unit);
    }
#endif

    return rv;
}


#if defined(BROADCOM_DEBUG)

/*
 * Function:
 *      bcm_rx_show
 * Purpose:
 *      Show RX information for the specified device.
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      Nothing.
 */

int
bcm_esw_rx_show(int unit)
{
    return _bcm_common_rx_show(unit);
}

#endif  /* BROADCOM_DEBUG */
