/** \file imb_ilu_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_ILU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_ILU_INTERNAL_H_INCLUDED

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
 * DEFINES:
 * {
 */

#define IMB_ILU_PHY_INVALID (-1)

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
 * \brief - get number of enabled ports in ilkn core (input excluded).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] nof_active_port - number of enabled ports in ilkn core (input excluded).
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_port_nof_active_ilkn_ports_in_core_get(
    int unit,
    bcm_port_t port,
    int *nof_active_port);

/**
 * \brief - returns weather to use extended memory for HRF for this ilkn port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [out] is_extended_memory_used - whether to use extended memory for port 0
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_is_extended_memory_used(
    int unit,
    bcm_port_t port,
    uint8 *is_extended_memory_used);

/**
 * \brief - Set ILKN Burst Editor configuration.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] enable_padding - enable small packets padding
 * \param [in] burst_max - maximum burst size in bytes
 * \param [in] burst_min - minimum burst size in bytes
 * \param [in] is_extended_mem_used - if to use extended memory
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_ibe_config(
    int unit,
    bcm_port_t port,
    uint32 enable_padding,
    uint32 burst_max,
    uint32 burst_min,
    uint8 is_extended_mem_used);

 /***
 * \brief - Reset ILKN Burst Editor
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core ID (ILU)
 * \param [in] ilkn_port_id - ILKN port ID (within the ILU)
 * \param [in] enable - TRUE or FALSE
 *
 * \return
 *   int
 */
int imb_ilu_ibe_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
 * \brief - parser wrap reset
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_parser_wrap_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
* \brief - HRF RX PRD reset
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
 * \brief - Set RX HRF configuration
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] is_extended_mem_used - whether to use extended memory for port 0
 * \param [in] enable_interleaving - indicates whether to enable interleaving mode
 * \param [in] drop_small_packets - indicates whether to drop small packets

 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_hrf_rx_config_set(
    int unit,
    bcm_port_t port,
    uint8 is_extended_mem_used,
    uint8 enable_interleaving,
    uint8 drop_small_packets);

/**
 * \brief - reset segmentation for RX HRF
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] is_complete - if to perform complete reset
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_hrf_rx_segmentation_reset(
    int unit,
    bcm_port_t port,
    uint8 is_complete,
    uint8 in_reset);

/**
 * \brief - Set TX HRF configuration
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] burst_max - maximum burst size in bytes
 * \param [in] is_extended_mem_used - whether to use extended memory for port 0
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_hrf_tx_config_set(
    int unit,
    bcm_port_t port,
    uint32 burst_max,
    uint8 is_extended_mem_used);

/*
 * \brief - Set TX HRF extended memory configuration
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] is_extended_mem_used - whether to use extended memory for port 0
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_hrf_tx_extended_mem_set(
    int unit,
    bcm_port_t port,
    uint8 is_extended_mem_used);

/**
 * \brief - ReSet RX HRF controller
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_hrf_controller_rx_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
 * \brief - ReSet Tx scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] in_reset - in/out of reset:
 * 0 => out-of-reset
 * 1 => in-reset
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_tx_nmg_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
 * \brief - Activate both RX/TX configurations if one port with 4 segments is used
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] use_half_nof_segments - whether to use 2 segments for port 0.
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_active_segment_set(
    int unit,
    bcm_port_t port,
    uint8 use_half_nof_segments);

/**
 * \brief - set number of segments to be used by the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] nof_segments - number of segments to set.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_port_nof_segments_set(
    int unit,
    bcm_port_t port,
    int nof_segments);

/**
 * \brief - Set flow control parameters per ILKN port.
 *
 * \param [in] unit - Chip Unit ID
 * \param [in] ilkn_core_id - ILKN core ID (ILU)
 * \param [in] ilkn_port_id - ILKN port ID (within the ILU)
 * \param [in] tx_cal_len - ILKN RX calendar length
 * \param [in] rx_cal_len - ILKN RX calendar length
 * \param [in] llfc_mode - ILKN LLFC mode
 * \param [in] calendar_mode - Inband ILKN calendar mode
 * \see
 *   * None
 */
int imb_ilu_internal_inband_ilkn_fc_init(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int tx_cal_len,
    int rx_cal_len,
    int llfc_mode,
    int calendar_mode);

/**
 * \brief - Enable / disable PM to stop TX according to a LLFC signal from CFC.
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 */
int imb_ilu_internal_port_stop_pm_from_cfc_llfc_enable_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 enable);

/**
 * \brief - Set the Flow Control Interface in/out of reset for a specified CDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core ID (ILU)
 * \param [in] in_reset - TRUE/FALSE
 *
 * \return
 *   int

 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_internal_fc_reset_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 in_reset);

/**
 * \brief
 *   Init all ilkn ports in the ilkn core scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_port_scheduler_config_hw_init(
    int unit,
    uint32 ilkn_core_id);

/**
 * \brief - Configure the ILU port in the specified Scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] ilkn_port_id - ilkn port id.
 * \param [in] is_max_sch_weight_tdm - set TDM scheduler to maximum weight.
 * \param [in] is_max_sch_weight_high - set High scheduler to maximum weight.
 * \param [in] dbal_sch_weight_low - dbal representation of the weight to be
 *                                   applied to Low scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_port_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int is_max_sch_weight_tdm,
    int is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low);

/**
 * \brief - Get scheduler weights of ILU port in NMG Scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] ilkn_port_id - ilkn port id.
 * \param [out] sch_weight_tdm -  TDM scheduler weight.
 * \param [out] sch_weight_high - High scheduler weight.
 * \param [out] dbal_sch_weight_low - dbal representation of the Low scheduler weight.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_port_scheduler_config_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *sch_weight_tdm,
    uint32 *sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e * dbal_sch_weight_low);

/**
 * \brief - Configure the ILU core in the specified Scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] is_max_sch_weight_tdm - set TDM scheduler to maximum weight.
 * \param [in] is_max_sch_weight_high - set High scheduler to maximum weight.
 * \param [in] dbal_sch_weight_low - dbal representation of the weight to be
 *                                   applied to Low scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_core_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 is_max_sch_weight_tdm,
    uint32 is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low);

/**
 * \brief - Retrieve the ILU in the specified Scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [out] is_max_sch_weight_tdm - set TDM scheduler to maximum weight.
 * \param [out] is_max_sch_weight_high - set High scheduler to maximum weight.
 * \param [out] dbal_sch_weight_low - dbal representation of the weight to be
 *                                   applied to Low scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_core_scheduler_config_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 *is_max_sch_weight_tdm,
    uint32 *is_max_sch_weight_high,
    dbal_enum_value_field_nif_sch_weight_e * dbal_sch_weight_low);

/**
 * \brief - Set the counter decrement threshold value for flush mechanism
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] threshold - threshold to set the scheduler counter decrement
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_sch_cnt_dec_thres_hw_set(
    int unit,
    uint32 ilkn_core_id,
    int threshold);

/**
 * \brief
 *   Configure low priority weight on the ILU Data HRF scheduler
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] ilkn_port_id - ilkn port id.
 * \param [in] dbal_sch_weight_low - dbal representation of the weight to be
 *                                   applied to Low scheduler.
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_internal_port_low_prio_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_nif_sch_weight_e dbal_sch_weight_low);

/**
 * \brief
 *   Configure low/high priority for Data HRF
 *
 * \param [in] unit - chip unit id.
 * \param [in] ilkn_core_id - ilkn core id.
 * \param [in] ilkn_port_id - ilkn port id.
 * \param [in] is_high - 1: high priority HRF
 *                       0: low priority HRF
 *
 *
 * \return
 *   int - see _SHR_E_*
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_internal_port_hrf_scheduler_config_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int is_high);

/**
 * \brief - Set the Flow Control Interface in/out of reset for a specified CDU.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core ID (ILU)
 * \param [in] ilkn_port_id - ILKN port ID (within the ILU)
 * \param [in] channel_id - Interface channel ID
 * \param [in] is_tdm - boolean
 */

int imb_ilu_internal_port_channel_tdm_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 channel_id,
    int is_tdm);

/**
 * \brief - Reset Rx HRF controller
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 */
int imb_ilu_hrf_controller_rx_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint8 in_reset);

/**
 * \brief - Reset Tx HRF controller
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 */
int imb_ilu_hrf_controller_tx_reset(
    int unit,
    bcm_port_t port,
    uint8 in_reset);

/**
 * \brief - Set Rx drop data for ILU flush sequence.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [in] is_drop_data - drop data:
 * 1 - enable drop data
 * 0 - disable drop data
 */
int imb_ilu_internal_port_rx_drop_data_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    int is_drop_data);

/**
 * \brief - Set the scheduler to flush mode for priority in ILU.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] sch_priority - SCH priority
 * \param [in] flush_context_enable - enable/disable flush for priority
 */
int imb_ilu_internal_port_rx_nmg_flush_context_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 sch_priority,
    int flush_context_enable);

/**
 * \brief - Enable flush mode in scheduler.
 *
 * \param [in] unit - Unit ID
 * \param [in] flush_enable - enable/disable flush in scheduler
 */
int imb_ilu_internal_port_rx_nmg_flush_mode_enable_hw_set(
    int unit,
    int flush_enable);

/**
 * \brief - Set HRF into flush mode.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [in] flush - enable/disable flush in HRF
 */
int imb_ilu_internal_port_rx_hrf_flush_mode_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    int flush);

/**
 * \brief - Enable flush in ILU.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] flush_enable - enable/disable flush in ILKN core
 */
int imb_ilu_internal_port_rx_core_flush_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    int flush_enable);

/**
 * \brief - Poll ILKN HRF counter.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [out] counter - HRF counter value
 */
int imb_ilu_port_rx_hrf_counter_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint32 *counter);

/**
 * \brief - Poll ILKN HRF counter.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 */
int imb_ilu_internal_port_rx_hrf_counter_polling(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type);

/**
 * \brief - Reset ILU Rx HRF in NMG.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 */
int imb_ilu_internal_nmg_rx_hrf_reset(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint8 in_reset);

/**
 * \brief - Reset ILU Rx core in NMG.
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] dbal_sch_prio - dbal scheduler priority
 * \param [in] in_reset - in reset indication:
 * 0 => out-of-reset
 * 1 => in-reset
 */
int imb_ilu_internal_nmg_rx_core_reset(
    int unit,
    uint32 ilkn_core_id,
    dbal_enum_value_field_nif_sch_priority_e dbal_sch_prio,
    uint8 in_reset);

    /*
     * }
     */

/**
 * \brief - set TX start threshold. this threshold represent the
 *        number of memory entries to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ILKN core ID (ILU)
 * \param [in] ilkn_port_id - ILKN port ID (within the ILU)
 * \param [in] start_thr - start tx threshold value
 */
int imb_ilu_tx_start_thr_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 start_thr);

/**
 * \brief - Enable or disable the ILKN padding feature
 */
int imb_ilu_internal_padding_enable_hw_set(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    int enable);

/**
 * \brief - Get the ILKN padding feature enable status
 */
int imb_ilu_internal_padding_enable_hw_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *enable);

/**
 * \brief - Get HRF RX fifo status
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [in] ilkn_hrf_type - type of ILKN HRF (Data/TDM)
 * \param [out] max_occupancy - HRF MAX fifo fullness
 * \param [out] fifo_level - HRF fifo fullness
 */
int imb_ilu_internal_port_rx_fifo_status_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    dbal_enum_value_field_ilkn_hrf_type_e ilkn_hrf_type,
    uint32 *max_occupancy,
    uint32 *fifo_level);

/**
 * \brief - Get HRF TX fifo status
 *
 * \param [in] unit - Unit ID
 * \param [in] ilkn_core_id - ILKN core id
 * \param [in] ilkn_port_id - ILKN port id
 * \param [out] max_occupancy - HRF MAX fifo fullness
 * \param [out] fifo_level - HRF fifo fullness
 */
int imb_ilu_internal_port_tx_fifo_status_get(
    int unit,
    uint32 ilkn_core_id,
    uint32 ilkn_port_id,
    uint32 *max_occupancy,
    uint32 *fifo_level);

int imb_ilu_prd_hard_stage_enable_hw_set(
    int unit,
    int ilkn_core_id,
    uint32 hrf,
    uint32 enable);

int imb_ilu_prd_hard_stage_enable_hw_get(
    int unit,
    int ilkn_core_id,
    uint32 hrf,
    uint32 *enable);

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 *
 * see .h file
 */
int imb_ilu_prd_ftmh_offsets_hw_set(
    int unit,
    int ilkn_core_id,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * see .h file
 */
int imb_ilu_prd_itmh_offsets_hw_set(
    int unit,
    int ilkn_core_id,
    uint32 tc_offset,
    uint32 dp_offset);

/**
 * \brief - set PRD Parser wether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [in] ignore_ip_dscp - ignore IP DSCP indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_ip_dscp_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_ip_dscp);

/**
 * \brief - get from PRD parser indication wether it ignores IP
 *        DSCP
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [out] ignore_ip_dscp - ignore IP DSCP indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_ip_dscp_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_ip_dscp);

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is
 *        identified as MPLS
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [in] ignore_mpls_exp - ignore MPLS EXP indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_mpls_exp_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_mpls_exp);

/**
 * \brief - get from PRD parser indication wether it ignores
 *        MPLS EXP
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [out] ignore_mpls_exp - ignore MPLS EXP indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_mpls_exp_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_mpls_exp);

/**
 * \brief - set PRD Parser wether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [in] ignore_inner_tag - ignore inner VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_inner_tag_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_inner_tag);

/**
 * \brief - get from PRD parser indication wether it ignores
 *        inner VLAN tag
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [out] ignore_inner_tag - ignore inner VLAN tag
 *        indication
 *
 * \return
 *   int -see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_inner_tag_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_inner_tag);

/**
 * \brief - set PRD Parser wether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [in] ignore_outer_tag - ignore outer VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_outer_tag_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 ignore_outer_tag);

/**
 * \brief - get from PRD parser indication wether it ignores
 *        outer VLAN tag
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [out] ignore_outer_tag - ignore outer VLAN tag
 *        indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_ignore_outer_tag_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *ignore_outer_tag);

/**
 * \brief - set default priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overriden by the priority from the soft
 *        stage.
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [in] default_priority - default prioroity
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_default_priority_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 default_priority);

/**
 * \brief - get default priority given in the PRD hard stage
 *        parser.
 *
 * \param [in] unit - chip unit id
 * \param [in] ilkn_core_id - ilu_id
 * \param [in] ilkn_port_id - prd port profile.
 * \param [out] default_priority - default priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_ilu_prd_default_priority_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *default_priority);

/**
 * \brief - fill the TM priority map (PRD hard stage). add the
 *        the map priorty value to be returned per TC + DP
 *        values
 *
 * see .h file
 */
int imb_ilu_prd_map_tm_tc_dp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority);
/**
 * \brief - get information from TM priority map (PRD hard
 *        stage). get the priority given for a conbination of
 *        TC+DP.
 *
 * see .h file
 */
int imb_ilu_prd_map_tm_tc_dp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority);

/**
 * \brief - fill the IP priority table (PRD hard stage). set
 *        priority value for a specific DSCP.
 *
 * see .h file
 */
int imb_ilu_prd_map_ip_dscp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority);
/**
 * \brief - get information from IP priority table (PRD hard
 *        stage). get the priorty returned for a specific DSCP
 *
 * See .h file
 */
int imb_ilu_prd_map_ip_dscp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority);
/**
 * \brief - fill the ETH (VLAN) Prioroity table (PRD hard
 *        stage). set a priority value to match a spcific
 *        PCP+DEI combination
 *
 * see .h file
 */
int imb_ilu_prd_map_eth_pcp_dei_hw_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority);
/**
 * \brief - get information from the ETH (VLAN) priority table
 *        (PRD hard stage). get the priorty returned for a
 *        specific combination of PCP+DEI
 *
 * see .h file
 */
int imb_ilu_prd_map_eth_pcp_dei_hw_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority);

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set
 *        priority value for a specific EXP value.
 *
 * see .h file
 */
int imb_ilu_prd_map_mpls_exp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority);
/**
 * \brief - get information from the MPLS priority table. get
 *        the priority returned for a specific EXP value
 *
 * see .h file
 */
int imb_ilu_prd_map_mpls_exp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority);

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct HRF. in each HRF there are thresholds per
 *        priority which are mapped to it.
 */
int imb_ilu_prd_threshold_hw_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold);
/**
 * \brief - get the current threshold for a specific priority in
 *        a specific HRF.
 */
int imb_ilu_prd_threshold_hw_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold);
/**
 * \brief - set one of the TPID values per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 */
int imb_ilu_prd_tpid_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 tpid_index,
    uint32 tpid_value);

/**
 * \brief - get the TPID value per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 */
int imb_ilu_prd_tpid_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 tpid_index,
    uint32 *tpid_value);

/**
 * \brief - get PRD drop counter value, per HRF
 */
int imb_ilu_prd_drop_count_hw_get(
    int unit,
    bcm_port_t port,
    uint32 hrf,
    uint64 *count);

/**
 * \brief - set the offset base for the TCAM key. it means the
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet
 * 1=>end of eth header
 * 2=>end of header after eth header.
 */
int imb_ilu_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base);

/**
 * \brief - get the current offset base for the given ether
 *        code.
 */
int imb_ilu_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base);

/**
 * \brief - set ether type size (in bytes) for the given ether
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 */
int imb_ilu_prd_ether_type_size_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size);

/**
 * \brief - get the ether type currently set for a specific
 *        ether type code
 */
int imb_ilu_prd_ether_type_size_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size);

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key
 *        per ether type (total of 16 keys). the key is build of
 *        4 offsets given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCAM entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        -----------------------------------------------------
 *        35              31       23       15       7        0
 */
int imb_ilu_prd_tcam_entry_key_offset_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value);

/**
 * \brief - get information about the TCAM key for a specific
 *        ether type.
 */
int imb_ilu_prd_tcam_entry_key_offset_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value);

int imb_ilu_prd_tcam_entry_hw_set(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - get information from the PRD soft stage (TCAM)
 *        table. get specific entry information
 */
int imb_ilu_prd_tcam_entry_hw_get(
    int unit,
    bcm_port_t port,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info);

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the higest priroity (3). each
 *        packet is compared against all control frame
 *        properties of the ilu
 */
int imb_ilu_prd_control_frame_hw_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - get Control Frame properties recognized by the
 *        parser.
 */
int imb_ilu_prd_control_frame_hw_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config);

/**
 * \brief - set MPLS special label properties. if one of the
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 */
int imb_ilu_prd_mpls_special_label_hw_set(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

/**
 * \brief - get current configuration of MPLS special label
 */
int imb_ilu_prd_mpls_special_label_hw_get(
    int unit,
    bcm_port_t port,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config);

int imb_ilu_prd_outer_tag_size_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 outer_tag_size);

int imb_ilu_prd_soft_stage_enable_hw_get(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 *enable_eth,
    uint32 *enable_tm);

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 *
 * see .h file
 */
int imb_ilu_prd_soft_stage_enable_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 enable_eth,
    uint32 enable_tm);

/**
 * \brief - set PRD port type, according to the header type of
 *        the ilkn port
 *
 * see .h file
 */
int imb_ilu_prd_port_type_hw_set(
    int unit,
    int ilkn_core_id,
    int ilkn_port_id,
    uint32 prd_port_type);

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * See .h file
 */
int imb_ilu_prd_custom_ether_type_hw_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val);

/**
 * \brief - get the ether type value mapped to a specific ether
 *        type code, out of the configurable ether types (codes
 *        1-6).
 *
 * see .h file
 */
int imb_ilu_prd_custom_ether_type_hw_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val);

/**
 * \brief - Configure power down of ILE
 */
int imb_ilu_power_down_set(
    int unit,
    int ilkn_core_id,
    int power_down);

#endif/*_IMB_ILU_INTERNAL_H_INCLUDED_*/
