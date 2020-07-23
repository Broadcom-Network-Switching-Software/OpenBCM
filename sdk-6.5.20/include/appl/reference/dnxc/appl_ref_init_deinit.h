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

#ifndef FILE_APPL_REF_DNXC_INIT_DEINIT_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXC_INIT_DEINIT_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <appl/diag/diag.h>
#include <shared/utilex/utilex_seq.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>

#ifndef AGGRESSIVE_ALLOC_DEBUG_TESTING
  #define AGGRESSIVE_ALLOC_DEBUG_TESTING 0
#endif

/*
* MACROs:
* {
*/
/**
 * brief - Default flags for applications steps. 
 * By default each application should have the following behaviour: 
 * 1. Can be run as standalone (useful for running steps from diag command shell) 
 * 2. Is enabled during init. 
 */
#define DEF_PROP_EN (UTILEX_SEQ_STEP_F_STANDALONE_EN | UTILEX_SEQ_STEP_F_SOC_PROP_EN)

#define WB_SKIP     (UTILEX_SEQ_STEP_F_WB_SKIP)

#define STANDALONE  (UTILEX_SEQ_STEP_F_STANDALONE_EN)

#define ACCESS      (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY | UTILEX_SEQ_STEP_F_REQUIRED_FOR_HEAT_UP)
#define ACCESS_ONLY (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY)

#define PARALLEL    (UTILEX_SEQ_STEP_F_RUN_ON_THREAD)

#define JOIN        (UTILEX_SEQ_STEP_F_JOIN_ALL_THREADS)

#define SKIP_NO_PRINT (UTILEX_SEQ_STEP_F_MEM_TEST_SKIP | UTILEX_SEQ_STEP_F_VERBOSE)


/**
 * brief - Default flags for applications steps. 
 * By default each application should have the following behaviour: 
 * 1. Can be run as standalone (useful for running steps from diag command shell) 
 * 2. Is disabled during init. 
 */
#define DEF_PROP_DIS (UTILEX_SEQ_STEP_F_STANDALONE_EN | UTILEX_SEQ_STEP_F_SOC_PROP_DIS)

/**
 * \brief
 * Time threshold flag EXPLICIT - used for testing how much time each step in the Init takes.
 * If EXPLICIT flag is specified, the value will be taken from a user-defined array(appl_init_time_thresh_expl).
 * If no flag is specified, default value (APPL_INIT_STEP_TIME_THRESH_DEFAULT, defined at appl_ref_init_utils.h) will be assumed.
 * If flag is set to PER_DEVICE the value will be taken from device specific data.
 */
#define TIME_EXPL      UTILEX_SEQ_STEP_F_TIME_THRESH_EXPLICIT
#define TIME_PER_DEV   UTILEX_SEQ_STEP_F_TIME_THRESH_PER_DEVICE

/**
 * \brief step should be skipped in memory test
 */
#define MEM_TEST_SKIP     UTILEX_SEQ_STEP_F_MEM_TEST_SKIP

/*
 * }
 */

/*
 * Structs and Enums:
 * {
 */

/**
 * \brief dnxc init parameters
 */
typedef struct
{
    int no_init;
    int no_deinit;
    int no_dump;
    int no_clean_up;
    int warmboot;
    int is_ctrl_c_check;
#if AGGRESSIVE_ALLOC_DEBUG_TESTING
    int is_resources_check;
    uint32 aggressive_num_to_test;
    uint32 aggressive_alloc_debug_testing_keep_order;
    uint32 aggressive_long_free_search;
    int loop_id;
#endif
} appl_dnxc_init_param_t;


/*
 * }
 */



/*
* Function Declarations:
* {
*/


/**
 * \brief - This function parses init params and starts the init and deinit sequence according to init params. 
 * 
 * \param [in] unit - Unit ID  
 * \param [in] params - Explict init params
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
appl_dnxc_init_step_list_run(
    int unit,
    appl_dnxc_init_param_t *params);


/**
 * \brief - Run a certain step (step_id) within the list of steps
 * 
 * \param [in] unit - Unit ID
 * \param [in] appl_id - step id to run 
 * \param [in] appl_steps - list of application steps 
 * \param [in] is_forward - Run forward or backward  
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
appl_dnxc_step_list_id_run(
    int unit, 
    int appl_id, 
    const appl_dnxc_init_step_t *appl_steps,
    int is_forward);

/**
 * \brief - Run a certain step (step_name) within the list of steps
 * 
 * \param [in] unit - Unit ID
 * \param [in] step_name - step id to run 
 * \param [in] appl_steps - list of application steps 
 * \param [in] is_forward - Run forward or backward  
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
appl_dnxc_step_list_name_run(
    int unit, 
    char *appl_name, 
    const appl_dnxc_init_step_t *appl_steps,
    int is_forward);

/**
 * \brief - Main BCM SDK init function. perfroms the following operation: 
 *          1. Attach and create unit. 
 *          2. Init BCM module (bcm_init()) 
 * 
 * \param [in] unit - Unit ID 
 * \param [in] init_param - Init application parameters 
 *   
 * \return
 *   shr_error_e    
 *   
 * \remark
 *   * This function is shared both for DNX and DNXF as the same init procedures should be invoked by both.
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_sdk_init(
    int unit);

/**
 * \brief - Main BCM SDK deinit function. perfroms the following operation: 
 *          1. Deinit BCM module (bcm_detach())
 *          2. Destroy unit.
 * 
 * \param [in] unit - Unit ID  
 *   
 * \return
 *   shr_error_e    
 *   
 * \remark
 *   * This function is shared both for DNX and DNXF as the same init procedures should be invoked by both.
 * \see
 *   * None
 */
shr_error_e
appl_dnxc_sdk_deinit(
    int unit);

/**
 * \brief - Get the list of application steps
 * 
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - list of application steps
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
appl_dnxc_step_list_get(
    int unit, 
    const appl_dnxc_init_step_t **appl_steps);

/**
 * \brief - Get the list of application steps' time thresholds
 *
 * \param [in] unit - Unit ID
 * \param [Out] appl_steps - list of application steps' time thresholds
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
appl_dnxc_step_time_thresh_list_get(
    int unit,
    const appl_init_time_thresh_info_t ** time_thresh_list);

/**
 * \brief - Get the time threshold of DNXC application step in microseconds
 *
 * \param [in] unit - Unit ID
 * \param [in] step_id - Application step ID
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
appl_dnxc_per_dev_step_time_threshold_get(
    int unit,
    int step_id,
    sal_usecs_t * time_thresh);

/**
 * \brief - Get the time threshold of DNXC full Application init in microseconds
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
appl_dnxc_per_dev_full_time_threshold_get(
    int unit,
    sal_usecs_t * time_thresh);

/*
 * }
 */

/* } */
#endif
