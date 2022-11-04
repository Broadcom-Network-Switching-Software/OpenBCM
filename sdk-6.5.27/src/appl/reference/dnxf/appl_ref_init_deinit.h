/** \file appl_ref_init_deinit.h
 *
 * init and deinit functions to be used by the INIT_DNX command.
 *
 * need to make minimal amount of steps before we are able to use call bcm_init
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef FILE_APPL_REF_DNXF_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXF_INIT_DEINIT_H_INCLUDED

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
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
#define APPL_DNXF_INIT_STEP_TIME_THRESH_DEFAULT   APPL_INIT_STEP_TIME_THRESH_DEFAULT
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
 * Function Declarations:
 * {
 */

/**
 * \brief - Get the list of DNXF application steps
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
shr_error_e appl_dnxf_step_list_get(
    int unit,
    const appl_dnxc_init_step_t ** appl_steps);

/**
 * \brief - Get the time threshold of DNXF application step in microseconds
 *
 * \param [in] unit - Unit ID
 * \param [in] unit - Step ID
 * \param [Out] step_id_dnx_data - Structure per step_id.
 *    It will hold the data from the DNX DATA
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnxf_per_dev_step_time_threshold_get(
    int unit,
    int step_id,
    utilex_step_time_dnx_data * step_id_dnx_data);

/**
 * \brief - Get the time threshold of DNXF full Application init in microseconds
 *
 * \param [in] unit - Unit ID
 * \param [Out] step_id_dnx_data - Structure per step_id.
 *    It will hold the data from the DNX DATA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e appl_dnxf_per_dev_full_time_threshold_get(
    int unit,
    utilex_step_time_dnx_data * step_id_dnx_data);

/**
 * \brief - BCM Shell command to init/deinit DNX
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] args - arguments recieved to parse over.
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   cmd_result_t 
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
cmd_result_t cmd_dnxf_init_dnx(
    int unit,
    args_t * args);

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
    APPL_DNXF_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * SDK init (attach and bcm_init()) 
     */
    APPL_DNXF_STEP_SDK,
    /**
     * Reference applications 
     */
    APPL_DNXF_STEP_APPS,
    /**
     * Parse device params
     */
    APPL_DNXF_STEP_DEVICE_PARAMS,
    /**
     * Parse device params
     */
    APPL_DNXF_STEP_SHELL_THREADS,
    /**
     * Links Isolate init
     */
    APPL_DNXF_LINKS_ISOLATE,
    /**
     * STK init
     */
    APPL_DNXF_STEP_STK_INIT,
    /*
     * Interrupt init
     */
    APPL_DNXF_STEP_INTR_INIT,
    /*
     * Packet DMA RX init
     */
    APPL_DNXF_STEP_RX,
    /*
     * Init done
     */
    APPL_DNXF_STEP_INIT_DONE,
    /**
     * Number of init steps, must be before last.
     */
    APPL_DNXF_INIT_STEP_COUNT,
    /**
     * Must be last
     */
    APPL_DNXF_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} appl_dnxf_step_id_e;

/*
 * }
 */

/* } */
#endif
