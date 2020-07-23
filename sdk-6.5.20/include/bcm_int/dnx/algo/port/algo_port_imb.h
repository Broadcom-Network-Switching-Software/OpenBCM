/** \file algo_port_imb.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef ALGO_PORT_IMB_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_IMB_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */

#include <shared/shrextend/shrextend_error.h>
#include <bcm/port.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */
 /*
  * }
  */
/*
 * Functions
 * {
 */
/**
 * \brief - 
 * Init algo port imb module: 
 * * Allocate resources (if required) 
 * * In cold boot init algo port imb data bases in sw state
 * * In warm boot no need to do anything
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port imb data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * internal algo port imb data base
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_init(
    int unit);

 /**
 * \brief - calculate the thershold after overflow for the fifo 
 *        according to the FIFO size.
 * After FIFO reches overflow, writing to the fifo will resume only after fifo level 
 * goes below this value.  
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fifo_size - fifo size in entries
 * \param [out] thr_after_ovf - threshold after overflow 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_threshold_after_ovf_get(
    int unit,
    bcm_port_t port,
    uint32 fifo_size,
    uint32 *thr_after_ovf);

/**
 * \brief - calculate the weight to be set for the ETHU in the 
 * Low priority scheduler. the weight is set per ETHU - the 
 * bigger the weight, the ETHU will get more BW in the RR. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication wether to include /
 *        exclude specified port for the weight calculation
 * \param [out] weight - weight for the ETHU 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_ethu_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *weight);

/**
 * \brief
 *   calculate the weight to be set for the ILU in the Low priority scheduler.
 *   The weight is set per bandwidth - the bigger the weight, the port/core
 *   will get more BW in the RR. 
 * 
 * \param [in] unit - chip unit id
 * \param [in] bandwidth - bandwidth of the port/core
 * \param [out] nof_weight_bits - weight (in bits) for the ILU port/core
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * port/core bandwidth is dependent on whether the ilkn scheduler
 *     context is per port or per core.
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_ilu_scheduler_weight_get(
    int unit,
    int bandwidth,
    int *nof_weight_bits);

/**
 * \brief - calculate the weight to be set for the RMC in the 
 * Low priority scheduler. the weight is set per RMC in the CDU 
 * - the more weight the RMC has, it will get more 
 *   BW in the RR.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] weight - weight for the RMC
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_rmc_scheduler_weight_get(
    int unit,
    bcm_port_t port,
    int *weight);

/**
 * \brief
 *   Allocate RMC id using resource manager.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] rmc_id - output RMC id.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_rmc_id_alloc(
    int unit,
    bcm_port_t port,
    int *rmc_id);

/**
 * \brief
 *   Free RMC id using resource manager.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id - RMC id to free.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_rmc_id_free(
    int unit,
    bcm_port_t port,
    int rmc_id);

/**
 * \brief
 *   Get number of free RMC IDs
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] nof_free_rmc_ids - number of free RMC IDs.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_rmc_id_nof_free_elements_get(
    int unit,
    bcm_port_t port,
    int *nof_free_rmc_ids);

/**
 * \brief - algorithm to calculate the first and last entry of a
 *        specific logical fifo in the CDU memory.
 * 
 * \param [in] unit - chip unit ID
 * \param [in] port - logical port
 * \param [in] nof_entries - size of the logical fifo to be set. 
 *        can be set ot -1 for equal devision of the memory
 *        between all logical fifos related to the port (can be
 *        up to 3 logical fifos per port)
 * \param [in] prev_last_entry - in case where there is more 
 *        than one logical fifo, prev_last_entry is the last
 *        entry of the previous logical fifo. this is the offset
 *        from which to start allocating for the current logical
 *        fifo. if set to -1, the allocation will start from
 *        offset 0. (expected to be set to -1 for the first
 *        logical fifo)
 * \param [in] nof_prio_groups - how many logical fifos 
 *        the port has.
 * \param [out] first_entry - output first entry
 * \param [out] last_entry - output last entry
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_algo_imb_rmc_entry_range_get(
    int unit,
    bcm_port_t port,
    int nof_entries,
    int prev_last_entry,
    int nof_prio_groups,
    uint32 *first_entry,
    uint32 *last_entry);

/**
 * \brief - algorithm to return start TX threshold for the port. 
 * this threshold represent the number of 64B words to be 
 * accumulated in the MLF before transmitting towards the PM. 
 * This is to prevent TX MAC starvation and is important for 
 * systems with oversubscription. 
 * 
 * \param [in] unit - chip unit id
 * \param [out] tx_start_thr - tx start threshold value
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_tx_start_thr_get(
    int unit,
    int *tx_start_thr);

/**
 * \brief - algorithm to calculate the the number of segments for specific ilkn port according to various parameters such as:
 *          device core clock, number of lanes, burst short size and serdes rate
 *
 * \param [in] unit - chip unit id
 * \param [in] core_clk_khz - device core clock in khz
 * \param [in] num_lanes - ilkn num of lanes
 * \param [in] ilkn_burst_short - burst short size
 * \param [in] serdes_speed - serdes rate
 * \param [out] nof_segments - nof segments calculated
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_ilkn_nof_segments_calc(
    int unit,
    int core_clk_khz,
    int num_lanes,
    int ilkn_burst_short,
    int serdes_speed,
    int *nof_segments);
/**
 * \brief - segment is a resource of ILKN, ehich has to be 
 *        shared between the two ports of the same ILKN core.
 *        the division can be as follows:
 * 1. ILKN0 has 4 segments (ILKN1 is not active) 
 * 2. ILKN0 and ILKN1 get two segments each. 
 * the number of segment a port should get is determined by the 
 * number of lanes, lane rate, burst short value and core clock 
 * of the device. if there is not enough segments to support the 
 * port, an error should be returned. 
 *  
 * \param [in] unit - chip unit id. 
 * \param [in] port - logical port # 
 * \param [out] nof_segments - returned nof segments.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_imb_nof_segments_default_get(
    int unit,
    bcm_port_t port,
    int *nof_segments);

/**
 * \brief - algorithm to get ETHU PortMacro lane boundaries,
 *         including the lower boundary and the upper boundary.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port #
 * \param [out] lower_bound - the first Phy ID for this ETHU PortMacro.
 * \param [out] upper_bound - the last Phy ID for this ETHU PortMacro.
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_algo_port_imb_ethu_port_pm_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound);

/**
 * \brief - calculate the credit value from TMC to EGQ. 
 * 
 * \param [in] unit - chip unit id.
 * \param [in] speed - port speed
 * \param [out] credit_val - the credit value
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int dnx_algo_imb_feu_tx_credit_val_get(
    int unit,
    int speed,
    int *credit_val);

 /*
  * }
  */
#endif/*_ALGO_PORT_IMB_H_INCLUDED__*/
