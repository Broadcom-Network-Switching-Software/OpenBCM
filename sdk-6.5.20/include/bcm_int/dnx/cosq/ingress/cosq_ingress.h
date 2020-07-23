/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file cosq_ingress.h
 * Reserved.$ 
 */

#ifndef _DNX_COSQ_INGRESS_H_INCLUDED_
/*
 * { 
 */
#define _DNX_COSQ_INGRESS_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/types.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

/*
 * specifies the ingress counter type
 */
typedef enum dnx_ingress_counter_type_e
{
    DNX_INGRESS_COUTNER_TYPE_PACKET,
    DNX_INGRESS_COUTNER_TYPE_BYTE
} dnx_ingress_counter_type_t;

/**
 * \brief - Structure for collecting IRE counter info
 */
typedef struct
{
    /**
     * NOF NIF packets
     */
    int nif_pkt_ovf;
    uint64 nif_pkt_cnt;
    /**
     * NOF RCYH packets
     */
    int rcyh_pkt_ovf;
    uint64 rcyh_pkt_cnt;
    /**
     * NOF RCYL packets
     */
    int rcyl_pkt_ovf;
    uint64 rcyl_pkt_cnt;
    /**
     * NOF CPU packets
     */
    int cpu_pkt_ovf;
    uint64 cpu_pkt_cnt;
    /**
     * NOF OLP packets
     */
    int olp_pkt_ovf;
    uint64 olp_pkt_cnt;
    /**
     * NOF OAMP packets
     */
    int oamp_pkt_ovf;
    uint64 oamp_pkt_cnt;
    /**
     * NOF register interface packets
     */
    int reg_if_pkt_ovf;
    uint64 reg_if_pkt_cnt;
} dnx_ingress_ire_counter_info_t;

/**
 * \brief - Structure for collecting SQM counter info
 */
typedef struct
{
    /**
     * NOF en-queue packets
     */
    int enq_pkt_ovf;
    uint64 pkt_enq_cnt;
    /**
     * NOF de-queue packets
     */
    int deq_pkt_ovf;
    uint64 pkt_deq_cnt;
} dnx_ingress_sqm_counter_info_t;

/**
 * \brief - Structure for collecting IPT counter info
 */
typedef struct
{
    /**
     * NOF packets from CGM
     */
    int cgm_pkt_ovf;
    uint32 pkt_cgm_cnt;
    /**
     * NOF packet request from SPB
     */
    int spb_pkt_ovf;
    uint32 pkt_spb_cnt;
    /**
     * NOF packets with discard request from ITPP
     */
    int itpp_disc_pkt_ovf;
    uint32 pkt_itpp_disc_cnt;
} dnx_ingress_ipt_counter_info_t;

/**
 * \brief - Initialize dnx ingress_congestion module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_init(
    int unit);

/**
 * \brief - Deinitialize  dnx ingress_congestion module
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_deinit(
    int unit);

/**
 * \brief - set provided port to have fixed/ not fixed priority
 * according to provided parameter
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT_INPUT:
 *   \param [in] port -  port-ID 
 * \par DIRECT_INPUT:
 *   \param [in] is_fixed - the value of is_fixed property
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_port_fixed_priority_set(
    int unit,
    bcm_port_t port,
    uint8 is_fixed);

/**
 * \brief - get  is_fixed  property for provided port
 * according to provided parameter
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 * \par DIRECT_INPUT:
 *   \param [in] port -  port-ID 
 * \par DIRECT_INPUT:
 *   \param [in] is_fixed - pointer to return the value of is_fixed property
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_port_fixed_priority_get(
    int unit,
    bcm_port_t port,
    uint8 *is_fixed);

/**
 * \brief - Callback to perform COSQ ingress part
 *  for port to be added
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being added
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of ingress port
 * \remark
 *   * Invoked during bcm_dnx_port_add
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_port_add_handle(
    int unit);

/**
 * \brief - Callback to clean COSQ ingress data
 *  from port which is in process of removing
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID 
 *   
 * \par INDIRECT INPUT:
 *   * port which is in process of being removed
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   *  HW configuration of the port
 * \remark
 *   * Invoked during bcm_dnx_port_remove
 *   * Must be invoked before Ingress Port handles port removing
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_port_remove_handle(
    int unit);

/**
 * \brief - return (list of) input port for the specified local port as provided to CGM block.
 * Currently,
 *   CGM uses PP port as an input port, unless "overwrite pp_port by reassembly" is turned on.
 * Probably in future devices, CGM would use TM port as an input port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] local_port -  local_port
 * \param [out] nof_entries - nof of entries in table cgm_in_port
 * \param [out] cgm_in_port - output array
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_cgm_in_port_get(
    int unit,
    bcm_port_t local_port,
    int *nof_entries,
    uint32 cgm_in_port[]);

/**
 * \brief - restore usage of dram by allowing traffic movement from sram to dram,
 *          this is only part of the sequence of restoring traffic to dram
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_dram_bound_traffic_to_dram_restore(
    int unit);

/**
 * \brief - stop usage of dram by blocking traffic movement from sram to dram,
 *          this is only part of the sequence of stopping traffic to dram
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_cosq_ingress_dram_bound_traffic_to_dram_stop(
    int unit);

/**
 * \brief - Function to get the IRE counter information
 *
 * \param [in] unit - unit number 
 * \param [in] core - core id
 * \param [out] counter_info - store counter information
 * 
 * \return int 
 */
shr_error_e dnx_ingress_ire_counter_info_get(
    int unit,
    int core,
    dnx_ingress_ire_counter_info_t * counter_info);

/**
 * \brief - Function to get the SQM counter information
 *
 * \param [in] unit - unit number 
 * \param [in] core - core id
 * \param [out] counter_info - store counter information
 * 
 * \return int 
 */
shr_error_e dnx_ingress_sqm_counter_info_get(
    int unit,
    int core,
    dnx_ingress_sqm_counter_info_t * counter_info);

/**
 * \brief - Function to get the IPT counter information
 *
 * \param [in] unit - unit number 
 * \param [in] core - core id
 * \param [out] counter_info - store counter information
 * 
 * \return int 
 */
shr_error_e dnx_ingress_ipt_counter_info_get(
    int unit,
    int core,
    dnx_ingress_ipt_counter_info_t * counter_info);

/**
 * \brief Function to create a VSQ gport from vsq index and vsq 
 *        type.
 * 
 * \param unit 
 * \param core 
 * \param vsq_group 
 * \param vsq_group_idx 
 * \param vsq_gport 
 * 
 * \return int 
 */
int dnx_ingress_congestion_vsq_gport_get(
    int unit,
    int core,
    int vsq_group,
    int vsq_group_idx,
    bcm_gport_t * vsq_gport);

/**
 * 
 * \brief - Function to configure the device overall ingress 
 *        receive rate. The rate is given in Mega packets per
 *        second
 * 
 * \param unit 
 * \param rate_mpps - the rate in mega packets per second
 * 
 * \return int 
 */
int dnx_cosq_ingress_receive_packet_rate_limit_set(
    int unit,
    int rate_mpps);

/**
 * 
 * \brief - Function to get the device overall ingress 
 *        receive rate. The rate is given in Mega packets per
 *        second.
 * 
 * \param [in] unit 
 * \param [in] rate_mpps - the rate in mega packets per second
 * 
 * \return int 
 */

int dnx_cosq_ingress_receive_packet_rate_limit_get(
    int unit,
    int *rate_mpps);

/**
 * brief - callback function to print the template manager for VOQ compensation profile
 *
 * \param [in] unit - Relevant unit.
 * \param [in] data - data to print. here it is the VOQ Compensation per profile
 *
 * \retval shr_error_e -
 *   Error return value
 * \remark
 *   None
 * \see
 *   shr_error_e
 */
void dnx_cosq_ingress_compensation_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Function to set the guaranteed rate to one of the 
 *        ingress shapers
 * 
 * \param unit - unit
 * \param gport - gport of type
 *  BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED
 *  BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED
 * \param cosq - unused. Must be 0
 * \param kbits_sec_min - unused. Must be 0
 * \param kbits_sec_max - rate in Kbps
 * \param flags - unused. Must be 0
 * 
 * \return int 
 */
int dnx_cosq_ingress_receive_bandwith_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 kbits_sec_min,
    uint32 kbits_sec_max,
    uint32 flags);

/**
 * 
 * \brief - Function to retrieve the actual ingress shaper rate
 *  
 * \param unit - unit
 * \param gport - gport of type
 *  BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_RCY_GUARANTEED
 *  BCM_COSQ_GPORT_IS_INGRESS_RECEIVE_CPU_GUARANTEED
 * \param cosq - unused. Must be 0
 * \param kbits_sec_min - ingress shaper rate min. Always 0.
 * \param kbits_sec_max - ingress shaper rate
 * \param flags 
 * 
 * \return int 
 */
int dnx_cosq_ingress_receive_bandwith_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    uint32 *kbits_sec_min,
    uint32 *kbits_sec_max,
    uint32 *flags);

/**
 * \brief - Function to calculate the best timer cycles and 
 *        grant size for the ingress shaper in order to reach as
 *        close as possible the passed rate. 
 *        Rate is in Mega bytes per second
 *
 * \param [in] unit - unit id.
 * \param [in] rate - desired rate
 * \param [in] max_nof_timer_cycles - maximal value of timer cycles
 * \param [in] max_grant_size_val   - maximal value of grant size
 * \param [out] best_grant_size   - result grant size
 * \param [out] best_timer_cycles - result timer cycles
 */
shr_error_e dnx_cosq_ingress_receive_shaper_grant_size_and_timer_cycles_calc(
    int unit,
    uint32 rate,
    uint32 max_nof_timer_cycles,
    uint32 max_grant_size_val,
    uint32 *best_grant_size,
    uint32 *best_timer_cycles);

#endif /** _DNX_COSQ_INGRESS_H_INCLUDED_ */
