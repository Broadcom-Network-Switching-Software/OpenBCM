/**
 * \file bcm_int/dnx/l2/l2_learn.h
 * Internal DNX L2 APIs
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef L2_LEARN_H_INCLUDED
/*
 * {
 */
#define L2_LEARN_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/l2.h>
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/shr_thread_manager.h>
/*
 * }
 */
/*
 * STRUCTS
 * {
 */

typedef enum dnx_learn_mode_e
{
    LEARN_MODE_DISTRIBUTED,
    LEARN_MODE_CENTRALIZED,
    LEARN_MODE_DMA
} dnx_learn_mode_t;

typedef enum dnx_learn_limit_mode_e
{
    LEARN_LIMIT_MODE_VLAN,
    LEARN_LIMIT_MODE_VLAN_PORT,
    LEARN_LIMIT_MODE_TUNNEL
} dnx_learn_limit_mode_t;

/*
 * }
 */
/*
 * GLOBALS
 * {
 */
extern shr_thread_manager_handler_t dnx_l2_cpu_learn_thread_info[];
/*
 * }
 */
/*
 * DEFINES
 * {
 */

#define BCM_DNX_L2_LEARN_SUPPORTED_FLAGS (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | BCM_L2_LEARN_LIMIT_PORT)

/** TOOD Replace it by string from DNX_DATA */
#define LEARN_LIMIT_MODE_NAME(unit)          \
            ((dnx_data_l2.general.l2_learn_limit_mode_get(unit) == LEARN_LIMIT_MODE_VLAN) ? "VLAN" : "VLAN_PORT")

/** Size of index to the table that maps VSI(or other field in MACT entry to profile) 17 means 128k entries */
#define MACT_VSI_TO_PROFILE_IND_SIZE   17

/** Size of index to the table that maps VSI(or other field in MACT entry to profile) for SVL, 16 means 64k entries */
#define MACT_VSI_TO_PROFILE_IND_SIZE_SVL   16

/*
 * MACT Entry and Key Sizes
 */
#define MACT_ENTRY_SIZE         150
#define MACT_KEY_SIZE           80

/* MACT Key is LSB aligned in the entry, assume it is right after MAC address,
 */
#define MACT_ENTRY_VSI_OFFSET           48
#define WA_MACT_ENTRY_VSI_OFFSET       (MACT_ENTRY_VSI_OFFSET + MACT_ENTRY_SIZE - MACT_KEY_SIZE)

/* MACT Payload is MSB aligned in the entry, and is adjusted to 6b VMV
 * | 6b VMV | Payload type(5b) | 21b Dst |  2b Learn Strength | 11b Entry Grouping | 1b Age_Profile | 22b OutLif
 */
#define MACT_ENTRY_OUTLIF_OFFSET       (MACT_ENTRY_SIZE - 6 - 5 - 21 - 2 - 11 - 1 - 22)

/** Default age profile */
#define MACT_DEFAULT_AGE_PROFILE 0

/** Number of LEM entries for one learn MAC entry */
#define MACT_EM_NOF_ENTRIES     4

/** CPU learning thread priority*/
#define FIFO_DMA_THREAD_PRIO     50
/*
 * }
 */

/**
 * \brief - Get the OLP logical port.
 *
 * \param [in] unit - unit ID
 * \param [in] cores_group - cores group ID
 * \param [in] olp_blk_offset - OLP block offset
 * \param [out] olp_port - return OLP logical port. Return -1 if more than 1 OLP port were configured or 0 ports configured.

 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_learn_dnx_olp_port_get(
    int unit,
    int cores_group,
    int olp_blk_offset,
    bcm_gport_t * olp_port);

/**
 * \brief - configures learn mode with its default values. The function is called from bcm_dnx_switch_control_set.
 * The function resets the OLP events' distribution to the defaults of the learn mode.
 * VSI learning profiles are reset to their defaults. Aging profiles are reset to their learn mode's default.
 * Specific VSI configurations that are different for the default like aging info, OLP events' distribution etc. should be configured againg.
 * The pipe is configured for ingress/egress learning according to the learn mode.
 *
 * \param [in] unit - unit id
 * \param [in] bcm_learn_mode - learn mode type.
 * learn mode type:
 * BCM_L2_INGRESS_CENT - Ingress and centralized learning
 * BCM_L2_INGRESS_DIST - Ingress and distributed learning
 * BCM_L2_EGRESS_DIST - Egress and distributed learning
 * BCM_L2_EGRESS_CENT - Egress and centralized learning
 * BCM_L2_LEARN_CPU - Learning events handled by CPU callbacks. DMA copying the event to the CPU's memory
 * BCM_L2_LEARN_DISABLE - Disabled learning
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_learn_mode_set(
    int unit,
    int bcm_learn_mode);

/**
 * \brief - Gets the current learn mode. The function is called from bcm_dnx_switch_control_get.
 *
 * \param [in] unit - unit id
 * \param [out] bcm_learn_mode - learn mode type
 *
 * learn mode type:
 * BCM_L2_INGRESS_CENT - Ingress and centralized learning
 * BCM_L2_INGRESS_DIST - Ingress and distributed learning
 * BCM_L2_EGRESS_DIST - Egress and distributed learning
 * BCM_L2_EGRESS_CENT - Egress and centralized learning
 * BCM_L2_LEARN_CPU - Learning events handled by CPU callbacks. DMA copying the event to the CPU's memory
 * BCM_L2_LEARN_DISABLE - Disabled learning
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_learn_mode_get(
    int unit,
    int *bcm_learn_mode);

/**
 * \brief
 * Initialize the learn limits default configurations.
 */
int dnx_l2_init_learn_limits(
    int unit);

int dnx_l2_learn_init_olp(
    int unit);

/**
 * \brief
 * Initialize the CPU learn thread information holder.
 *
 * \param [in]  unit - pointer to the unit id
 */
void dnx_l2_learn_mact_fifodma_cpu_learn_thread_info_init(
    int unit);

/**
 * \brief -
 * This function creates the cpu learning dedicated thread.
 *
 * \param [in]  unit - relevant unit
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   In the case it is already initiated the function will not perform any action
 *
 * \see
 *   None
 */
shr_error_e dnx_l2_learn_mact_fifodma_cpu_learn_init(
    int unit);
/**
 * \brief -
 * This function kills the cpu dedicated thread.
 *
 * \param [in]  unit - pointer to the unit id
 *
 * \return
 *   void
 *
 * \remark
 *   In the case that the above thread is not initiated the function will not perform any action
 *
 * \see
 *   None
 */
shr_error_e dnx_l2_learn_mact_fifodma_cpu_learn_cleanup(
    int unit);

int dnx_l2_init_learn_payload_context(
    int unit);

int dnx_l2_init_learn_key_context(
    int unit);

shr_error_e dnx_vsi_learning_profile_id_get(
    int unit,
    int vsi,
    int *vsi_profile);

shr_error_e dnx_vsi_learning_profile_hw_update(
    int unit,
    int vsi,
    int old_vsi_learning_profile,
    int new_vsi_learning_profile,
    uint8 is_first,
    uint8 is_last,
    dnx_vsi_learning_profile_info_t * vsi_learning_profile_info);

/**
 * \brief - Init the DMA channel of the OLP
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_olp_dma_init(
    int unit);

/**
 * \brief - Stop and free the dma channel of the olp
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_l2_olp_dma_deinit(
    int unit);

void dnx_olp_dma_handler(
    int unit,
    void *entry,
    int entry_size,
    int entry_number_unused,
    int total_amount_of_entries_unused,
    void **user_data_unused);

/**
 * \brief - Update the OLP events distribution of a given VSI.
 *
 * \param [in] unit - unit ID
 * \param [in] vsi - vsi to update
 * \param [in] is_lif - event configuration per LIF
 * \param [in] event_forwarding_profile_info - info to set in the event_forwarding_profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_event_forwarding_profile_update(
    int unit,
    int vsi,
    int is_lif,
    dnx_event_forwarding_profile_info_t * event_forwarding_profile_info);

/**
 * \brief
 *  Verify l2 learn mode and event configuration.
 * \param [in] unit - The unit number.
 * \param [in] is_lif - event configuration per LIF.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \remark
 *   * None
 * \see
 *   * None
 *
 */
shr_error_e dnx_l2_learn_mode_verify(
    int unit,
    int is_lif);

/*
 * }
 */
#endif /* L2_LEARN_H_INCLUDED */
