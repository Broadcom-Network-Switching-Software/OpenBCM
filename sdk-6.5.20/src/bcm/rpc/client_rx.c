/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        client_rx.c
 * Purpose:     Client implementation of bcm_rx* functions.
 * Requires:
 */

#include <bcm/rx.h>
#include <bcm/error.h>
#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      bcm_client_rx_init
 * Purpose:
 *      Software initialization for RX API on client
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
bcm_client_rx_init(int unit)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_init(unit);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_cfg_init
 * Purpose:
 *      Re-initialize the user level configuration on client
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
bcm_client_rx_cfg_init(int unit)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cfg_init(unit);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_start
 * Purpose:
 *      Initialize and configure the RX subsystem for a given client unit
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
bcm_client_rx_start(int unit, bcm_rx_cfg_t *cfg)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_start(unit, cfg);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_stop
 * Purpose:
 *      Stop RX for the given client unit; saves current configuration
 * Parameters:
 *      unit - The unit to stop
 *      cfg - OUT Configuration copied to this parameter
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This signals the thread to exit.
 */

int
bcm_client_rx_stop(int unit, bcm_rx_cfg_t *cfg)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_stop(unit, cfg);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_clear
 * Purpose:
 *      Clear all RX info on client
 * Returns:
 *      BCM_E_NONE
 */

int
bcm_client_rx_clear(int unit)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_clear(unit);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_cfg_get
 * Purpose:
 *      Check if init done; get the current RX configuration on client
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
bcm_client_rx_cfg_get(int unit, bcm_rx_cfg_t *cfg)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cfg_get(unit, cfg);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_register
 * Purpose:
 *      Register an upper layer driver on client
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
 */

int
bcm_client_rx_register(int unit, const char *name, bcm_rx_cb_f callback,
                uint8 priority, void *cookie, uint32 flags)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_register(unit, name, callback,
                                  priority, cookie, flags);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_unregister
 * Purpose:
 *      De-register a callback function on client
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
bcm_client_rx_unregister(int unit, bcm_rx_cb_f callback, uint8 priority)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_unregister(unit, callback, priority);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_active
 * Purpose:
 *      Return boolean as to whether client unit is running
 * Parameters:
 *      unit - StrataXGS to check
 * Returns:
 *      Boolean:   TRUE if unit is running.
 * Notes:
 */
int
bcm_client_rx_active(int unit)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_active(unit);
#else
    return BCM_E_UNAVAIL;
#endif
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
bcm_client_rx_queue_register(int unit, const char *name, bcm_cos_queue_t cosq,
                             bcm_rx_cb_f callback, uint8 priority, void *cookie,
                             uint32 flags)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_queue_register(unit, name, cosq, callback,
                                     priority, cookie, flags);
#else
    return BCM_E_UNAVAIL;
#endif
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
bcm_client_rx_queue_unregister(int unit, bcm_cos_queue_t cosq,
                               bcm_rx_cb_f callback, uint8 priority)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_queue_unregister(unit, cosq, callback, priority);
#else
    return BCM_E_UNAVAIL;
#endif
}

# if 0
/* WAIT */
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
bcm_client_rx_cosq_mapping_size_get(int unit, int *size)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cosq_mapping_size_get(unit, size);
#else
    return BCM_E_UNAVAIL;
#endif
}
 
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
bcm_client_rx_cosq_mapping_set(int unit, int index,
                               bcm_rx_reasons_t reasons, bcm_rx_reasons_t reasons_mask,
                               uint8 int_prio, uint8 int_prio_mask,
                               uint32 packet_type, uint32 packet_type_mask,
                               bcm_cos_queue_t cosq)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cosq_mapping_set(unit, index, reasons, reasons_mask,
                                       int_prio, int_prio_mask, packet_type,
                                       packet_type_mask, cosq);
#else
    return BCM_E_UNAVAIL;
#endif
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
bcm_client_rx_cosq_mapping_get(int unit, int index,
                               bcm_rx_reasons_t *reasons, bcm_rx_reasons_t *reasons_mask,
                               uint8 *int_prio, uint8 *int_prio_mask,
                               uint32 *packet_type, uint32 *packet_type_mask,
                               bcm_cos_queue_t *cosq)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cosq_mapping_get(unit, index, reasons, reasons_mask,
                                       int_prio, int_prio_mask, packet_type,
                                       packet_type_mask, cosq);
#else
    return BCM_E_UNAVAIL;
#endif
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
bcm_client_rx_cosq_mapping_delete(int unit, int index)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cosq_mapping_delete(unit, index);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_rx_cosq_mapping_reasons_get
 * Purpose:
 *      Get the supported reasons for CPU rx cosq mapping
 * Parameters:
 *      unit - Unit reference
 *      reasons - rx packet "reasons" used to map COS queue 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_cosq_mapping_reasons_get(int unit, bcm_rx_reasons_t *reasons)
{
#ifdef  BCM_ESW_SUPPORT
    return bcm_esw_rx_cosq_mapping_reasons_get(unit, reasons);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_rx_reasons_get
 * Purpose:
 *      Get the supported reasons for CPU Rx packets
 * Parameters:
 *      unit - Unit reference
 *      reasons - rx packet "reasons" 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_reasons_get(int unit, bcm_rx_reasons_t *reasons)
{
#ifdef  BCM_ESW_SUPPORT
    return bcm_esw_rx_reasons_get(unit, reasons);
#else
    return BCM_E_UNAVAIL;
#endif
}

#endif
/* WAIT */

/*
 * Function:
 *      bcm_client_rx_running_channels
 * Purpose:
 *      Returns a bitmap indicating which channels are active on client
 * Parameters:
 *      unit       - Which unit to operate on
 * Returns:
 *      Bitmap of active channels
 * Notes:
 */

int
bcm_client_rx_channels_running(int unit, uint32 *channels)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_channels_running(unit, channels);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_alloc
 * Purpose:
 *      Gateway to configured RX allocation function on client
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
bcm_client_rx_alloc(int unit, int pkt_size, uint32 flags, void **buf)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_alloc(unit, pkt_size, flags, buf);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_free
 * Purpose:
 *      Gateway to configured RX free function on client.
 *      Generally, packet buffer was allocated with bcm_rx_alloc.
 * Parameters:
 *      unit - Unit reference
 *      pkt - Packet to free
 * Returns:
 * Notes:
 *      In particular, packets stolen from RX with BCM_RX_HANDLED_OWNED
 *      should use this to free packets.
 */

int
bcm_client_rx_free(int unit, void *pkt_data)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_free(unit, pkt_data);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_free_enqueue
 * Purpose:
 *      Queue a packet to be freed by the RX thread on client.
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
bcm_client_rx_free_enqueue(int unit, void *pkt_data)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_free_enqueue(unit, pkt_data);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Functions:
 *      bcm_client_rx_burst_set, get; bcm_client_rx_rate_set, get
 *      bcm_client_rx_cos_burst_set, get; bcm_client_rx_cos_rate_set, get;
 *      bcm_client_rx_cos_max_len_set, get
 * Purpose:
 *      Get/Set the global and per COS limits on client:
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
bcm_client_rx_rate_set(int unit, int pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_rate_set(unit, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_rate_get(int unit, int *pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_rate_get(unit, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cpu_rate_set(int unit, int pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cpu_rate_set(unit, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cpu_rate_get(int unit, int *pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cpu_rate_get(unit, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}


int
bcm_client_rx_burst_set(int unit, int burst)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_burst_set(unit, burst);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_burst_get(int unit, int *burst)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_burst_get(unit, burst);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_rate_set(int unit, int cos, int pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_rate_set(unit, cos, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_rate_get(int unit, int cos, int *pps)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_rate_get(unit, cos, pps);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_burst_set(int unit, int cos, int burst)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_burst_set(unit, cos, burst);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_burst_get(int unit, int cos, int *burst)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_burst_get(unit, cos, burst);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_max_len_set(int unit, int cos, int max_q_len)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_max_len_set(unit, cos, max_q_len);
#else
    return BCM_E_UNAVAIL;
#endif
}

int
bcm_client_rx_cos_max_len_get(int unit, int cos, int *max_q_len)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_cos_max_len_get(unit, cos, max_q_len);
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      bcm_client_rx_remote_pkt_enqueue
 * Purpose:
 *      Enqueue a remote packet for normal RX processing on client
 * Parameters:
 *      unit          - The BCM unit in which queue the pkt is placed
 *      pkt           - The packet to enqueue
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_client_rx_remote_pkt_enqueue(int unit, bcm_pkt_t *pkt)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_remote_pkt_enqueue(unit, pkt);
#else
    return BCM_E_UNAVAIL;
#endif
}


/*
 * Function:
 *      bcm_client_rx_sched_register
 * Purpose:
 *      Rx scheduler registration function. 
 * Parameters:
 *      unit       - (IN) Unused. 
 *      sched_cb   - (IN) Rx scheduler routine.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_sched_register(int unit, bcm_rx_sched_cb sched_cb)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_sched_register(unit, sched_cb);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/*
 * Function:
 *      bcm_client_rx_sched_unregister
 * Purpose:
 *      Rx scheduler de-registration function. 
 * Parameters:
 *      unit  - (IN) Unused. 
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_sched_unregister(int unit)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_sched_unregister(unit);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/*
 * Function:
 *      bcm_client_rx_unit_next_get
 * Purpose:
 *      Rx started units iteration routine.
 * Parameters:
 *      unit       - (IN)  BCM device number. 
 *      unit_next  - (OUT) Next attached unit with started rx.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_unit_next_get(int unit, int *unit_next)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_unit_next_get(unit, unit_next);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/*
 * Function:
 *      bcm_client_rx_queue_packet_count_get
 * Purpose:
 *      Get number of packets awaiting processing in the specific device/queue.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      cosq         - (IN) Queue priority.
 *      packet_count - (OUT) Next attached unit with initialized rx.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_client_rx_queue_packet_count_get(int unit, bcm_cos_queue_t cosq, int *packet_count)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_queue_packet_count_get(unit, cosq, packet_count);
#else
    return (BCM_E_UNAVAIL);
#endif
}

/*
 * Function:
 *      bcm_client_rx_queue_rate_limit_status
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
bcm_client_rx_queue_rate_limit_status_get(int unit, bcm_cos_queue_t cosq, 
                                          int *packet_tokens)
{
#ifdef	BCM_ESW_SUPPORT
    return bcm_esw_rx_queue_rate_limit_status_get(unit, cosq, packet_tokens);
#else
    return (BCM_E_UNAVAIL);
#endif
}
