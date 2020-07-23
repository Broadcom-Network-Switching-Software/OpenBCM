/**
 * \file algo_l2.h 
 * Internal DNX L2 Managment APIs 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */

#ifndef ALGO_L2_H_INCLUDED
/*
 * { 
 */
#define ALGO_L2_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * Resource name defines for algo l2. 
 * {
 */
#define DNX_ALGO_L2_EVENT_FORWARDING_PROFILE_TEMPLATE     "VSI forwarding profile template"
#define DNX_ALGO_L2_VSI_LEARNING_PROFILE_TEMPLATE         "VSI learning profile template"
#define DNX_ALGO_L2_VSI_AGING_PROFILE_TEMPLATE            "VSI aging profile template"

#define DNX_VSI_LEARNING_PROFILE_DEFAULT_PROFILE_ID 0
#define DNX_FORWARDING_EVENT_PROFILE_DEFAULT_PROFILE_ID 0
#define DNX_VSI_AGING_PROFILE_DEFAULT_PROFILE_ID 0

#define OLP_ILLEGAL_DISTRIBUTION 0xFFFFFFFF

#define BUILD_OLP_DISTRIBUTION_BUFFER(loopback, mc, cpu, cpu_dma)   ((loopback) | ((mc)<<1) | ((cpu)<<2) | ((cpu_dma)<<3))
/** 1 if the distribution is set in the buffer; 0 otherwise */
#define GET_LOOPBACK_DISTRIBUTION_FROM_BUFFER(buffer)   (((buffer) & 0x1))
#define GET_MC_DISTRIBUTION_FROM_BUFFER(buffer)         (((buffer) & 0x2)>>1)
#define GET_CPU_DISTRIBUTION_FROM_BUFFER(buffer)        (((buffer) & 0x4)>>2)
#define GET_CPU_DMA_DISTRIBUTION_FROM_BUFFER(buffer)    (((buffer) & 0x8)>>3)

#define AGING_PROFILE_INVALID_AGE (-1)
/**
 * }
 */

  /**
 * \brief - data of a vsi profile.
 */
typedef struct
{
    /** A limit for the number of entries in the MACT */
    uint32 limit;

    /** Enable interrupts when limit is crossed */
    uint8 interrupt_enable;

    /** Enable OLP messages when limit is crossed */
    uint8 message_enable;

    /** Profile describing the distribution of the OLP events */
    uint32 event_forwarding_profile;

    /** Profile describing which events are raised in each age state */
    uint32 aging_profile;

    /** Control the aging cycle period */
    uint32 cycle_period;

    /** Mask the cycle period */
    uint32 cycle_mask;

} dnx_vsi_learning_profile_info_t;

/**
 * \brief - data of a vsi forwarding profile.
 */
typedef struct
{
    /** Distribution queues of the learn event */
    uint32 learn_distribution;

    /** Distribution queues of the transplant event */
    uint32 transplant_distribution;

    /** Distribution queues of the delete event */
    uint32 delete_distribution;

    /** Distribution queues of the refresh event */
    uint32 refresh_distribution;

    /** Distribution queues of the ack event */
    uint32 ack_distribution;

} dnx_event_forwarding_profile_info_t;

typedef struct
{
    /** The age which is configured for age out event. RBD bit is concatenated as the MSB */
    uint32 ageout_age;

    /** The age which is configured for delete operation. RBD bit is concatenated as the MSB */
    uint32 delete_age;

    /** The age which is configured for refresh event. RBD bit is concatenated as the MSB */
    uint32 refresh_age;

} dnx_aging_profile_info_t;

/** Set this value if the event's distribution should stay as is and you are updating only the other events */
#define DISTRIBUTION_NO_CHANGE 0xFFFFFFFF

/**
 * \brief - Increase the reference of a forwarding profile using get and allocate on this profile.
 *
 * \param [in] unit - unit id
 * \param [in] forwarding_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l2_event_forwarding_profile_increase_ref(
    int unit,
    int forwarding_profile);

/**
 * \brief - Decrease the reference of a forwarding profile.
 *
 * \param [in] unit - unit id
 * \param [in] forwarding_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l2_event_forwarding_profile_decrease_ref(
    int unit,
    int forwarding_profile);

/**
 * \brief - Increase the reference of a aging profile using get and allocate on this profile.
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l2_aging_profile_increase_ref(
    int unit,
    int aging_profile);

/**
 * \brief - Decrease the reference of a aging profile.
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile - profile id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l2_aging_profile_decrease_ref(
    int unit,
    int aging_profile);

/**
 * \brief - Print an entry of the vsi profile template. See
 *       \ref dnx_algo_template_print_data_cb for more details.
 * 
 * \param [in] unit - unit id
 * \param [in] data - Pointer to the struct to be printed.
 *      \b As \b input - \n
 *       The pointer is expected to hold a struct of type uint32[2]. \n
 *       It's the user's responsibility to verify it beforehand.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_l2_vsi_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Print an entry of the vsi forwarding profile template. See
 *       \ref dnx_algo_template_print_data_cb for more details.
 * 
 * \param [in] unit - unit id
 * \param [in] data - Pointer to the struct to be printed.
 * \return
 * \remark 
 *  None
 * \see
 *    dnx_algo_template_print_data_cb
 */
void dnx_l2_event_forwarding_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Print an entry of the vsi aging profile template. See
 *       \ref dnx_algo_template_print_data_cb for more details.
 * 
 * \param [in] unit - unit id
 * \param [in] data - Pointer to the struct to be printed.
 * \return
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_l2_vsi_aging_print_cb(
    int unit,
    const void *data);

/**
 * \brief - Intialize l2 algorithms.
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
shr_error_e dnx_algo_l2_init(
    int unit);

/*
 * } 
 */
#endif/*_ALGO_L3_API_INCLUDED__*/
