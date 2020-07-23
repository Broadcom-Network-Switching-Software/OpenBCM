/** \file appl_ref_init_deinit.h
 *
 * init and deinit functions to be used by the INIT_DNX command.
 *
 * need to make minimal amount of steps before we are able to use call bcm_init
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FILE_APPL_REF_DNX_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNX_INIT_DEINIT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <appl/diag/diag.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>
#include <shared/utilex/utilex_seq.h>

/*
* MACROs:
* {
*/

/*
 * }
 */

/*
* Defines:
* {
*/
/*
 * Default time threshold (in microseconds) for the time each
 * step is taking during init. This value was chosen because the
 * majority of the steps are below it. For steps that are
 * expected to be above the default, a specific threshold can be
 * set.
 */
#define APPL_DNX_INIT_STEP_TIME_THRESH_DEFAULT   APPL_INIT_STEP_TIME_THRESH_DEFAULT
/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/*
 * }
 */

/*
 * Globals:
 * {
 */
extern char appl_dnx_init_usage[];
/*
 * }
 */
/*
* Function Declarations:
* {
*/

/**
 * \brief - Get the list of DNX application steps
 * 
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - pointer to the list of application steps
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_step_list_get(
    int unit, 
    const appl_dnxc_init_step_t **appl_steps);

/**
 * \brief - Get the list of DNX application steps' time thresholds
 *
 * \param [in] unit - Unit ID
 * \param [Out] time_thresh_list - pointer to the list of steps' time thresholds
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_step_time_thresh_list_get(
    int unit,
    const appl_init_time_thresh_info_t ** time_thresh_list);

/**
 * \brief - Get the time threshold of DNX application step in microseconds
 *
 * \param [in] unit - Unit ID
 * \param [in] unit - Step ID
 * \param [Out] time_thresh - time threshold in microseconds
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_per_dev_step_time_threshold_get(
    int unit,
    int step_id,
    sal_usecs_t * time_thresh);

/**
 * \brief - Get the time threshold of DNX full Application init in microseconds
 *
 * \param [in] unit - Unit ID
 * \param [Out] time_thresh - Device specific time thresholds
 *  for the step
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
appl_dnx_per_dev_full_time_threshold_get(
    int unit,
    sal_usecs_t * time_thresh);

/**
 * \brief Describes the IDs for the different steps and 
 *        sub-steps of the DNX APPL.
 */
typedef enum
{
    /**
     * Invalid Value, used to indicate to APIs that this input is
     * not relevant
     */
    APPL_DNX_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * SDK init (attach and bcm_init())
     */
    APPL_DNX_STEP_SDK,
    /**
     * Reference applications
     */
    APPL_DNX_STEP_APPS,
    /**
     * first step of application
     */
    APPL_DNX_STEP_FIRST,
    /**
     * parse system params application
     */
    APPL_DNX_STEP_SYS_PARAMS,
    /**
     * General device confiugurations application
     */
    APPL_DNX_STEP_SYS_DEVICE,
    /**
     * System ports application
     */
    APPL_DNX_STEP_SYS_PORTS,
    /**
     * Sniff recycle ports application
     */
    APPL_DNX_STEP_SNIFF_PORTS,
    /**
     * End to end scheduling scheme application
     */
    APPL_DNX_STEP_E2E_SCHEME,
    /**
     * OAM application
     */
    APPL_DNX_STEP_OAM,
    /**
     * PP ETH Processing application
     */
    APPL_DNX_STEP_PP_ETH_PROCESSING,
    /*
     * Used to disable all possible threads created after init
     * using the shell commands
     */
    APPL_DNX_STEP_SHELL_THREADS,
    /**
     * EEDB Access application
     */
    APPL_DNX_STEP_EEDB_ACCESS,
    /**
     * MDB EM FALSE HIT application
     */
    APPL_DNX_STEP_MDB_EM_FALSE_HIT,
    /**
     * L2 application
     */
    APPL_DNX_STEP_L2,
    /**
     * VLAN application
     */
    APPL_DNX_STEP_VLAN,
    /**
     * VLAN application
     */
    APPL_DNX_STEP_COMPRESSED_LAYER_TYPE,
    /**
     * Traffic enable application
     */
    APPL_DNX_STEP_TRAFFIC_EN,
    /**
     * CPU RX application
     */
    APPL_DNX_STEP_RX,
    /**
     * Reference FIELD applications
     */
    APPL_DNX_STEP_FP_APPS,
    /**
     * Fec Dest enable application
     */
    APPL_DNX_STEP_FP_FEC_DEST,
    /**
     * Oam Index enable application
     */
    APPL_DNX_STEP_FP_OAM_INDEX,
    /**
     * Trap L4 enable application
     */
    APPL_DNX_STEP_FP_TRAP_L4,
    /**
     * Flow ID enable application
     */
    APPL_DNX_STEP_FP_FLOW_ID,
    /**
     * ROO enable application
     */
    APPL_DNX_STEP_FP_ROO,
    /**
     * IPMC INLIF enable application
     */
    APPL_DNX_STEP_FP_IPMC_INLIF,
    /**
     * J1 Same port drop enable application
     */
    APPL_DNX_STEP_FP_J1_SAME_PORT,
    /**
     * J1 learning enable application
     */
    APPL_DNX_STEP_FP_LEARN_AND_LIMIT,
    /**
     * J1 learning nullify key msbs enable application
     */
    APPL_DNX_STEP_FP_LEARN_NULLIFY_KEY_MSBS,
    /**
     * J1 MPLS PHP application
     */
    APPL_DNX_STEP_FP_J1_PHP,
    /**
     * Oam stat enable application
     */
    APPL_DNX_STEP_FP_OAM_STAT,
#if defined(INCLUDE_KBP)
    /**
     * KBP application
     */
    APPL_DNX_STEP_KBP,
#endif
    /**
     * Linkscan application
     */
    APPL_DNX_STEP_LINKSCAN,
    /**
     * ITMH application
     */
    APPL_DNX_STEP_ITMH,
    /**
     * ITMH-PPH application
     */
    APPL_DNX_STEP_ITMH_PPH,
    /**
     * J1 ITMH application
     */
    APPL_DNX_STEP_J1_ITMH,
    /**
     * J1 ITMH-PPH application
     */
    APPL_DNX_STEP_J1_ITMH_PPH,
    /**
     * RCH remove application
     */
    APPL_DNX_STEP_RCH_REMOVE,
    /**
     * SRV6 application
     */
    APPL_DNX_STEP_SRV6,
    /**
     * NAT application
     */
    APPL_DNX_STEP_NAT,
    /*
     * Interrupt application
     */
    APPL_DNX_STEP_INTERRUPT,
    /**
     * Last meaningful step of application
     */
    APPL_DNX_STEP_END,
    /**
     * QOS application
     */
    APPL_DNX_STEP_QOS,
    /**
     * DRAM application
     */
    APPL_DNX_STEP_DRAM,
    /**
     * HARD RESET application
     */
    APPL_DNX_STEP_HARD_RESET,
    /**
     * VISIBILITY application
     */
    APPL_DNX_STEP_VISIBILITY,
    /**
     * COMPENSATION application
     */
    APPL_DNX_STEP_COMPENSATION,
    /**
     * RX Trap application
     */
    APPL_DNX_STEP_RX_TRAP,
    /**
     * init done step, used to update modules that the init was done
     */
    APPL_DNX_STEP_INIT_DONE,
    /**
     * Number of init steps, must be before last.
     */
    APPL_DNX_INIT_STEP_COUNT,
    /**
     * Must be last
     */
    APPL_DNX_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} appl_dnx_step_id_e;


/*
 * }
 */

/* } */
#endif
