/** \file appl_ref_init_utils.h
 *
 * init and deinit utitlity functions to be used by the INIT_DNX command.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef FILE_APPL_REF_DNXC_INIT_UTILS_H_INCLUDED
/* { */
#define FILE_APPL_REF_DNXC_INIT_UTILS_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

#include <shared/utilex/utilex_seq.h>
#include <bcm/init.h>
/*
* MACROs:
* {
*/

/*
 * }
 */

/*
 * Memory Leak detection Activated.
 */
#define APPL_INIT_ADVANCED_F_MEM_LEAK_DETECT   BCM_INIT_ADVANCED_F_MEM_LEAK_DETECT
/*
 * Memory Logging
 */
#define APPL_INIT_ADVANCED_F_MEM_LOG           BCM_INIT_ADVANCED_F_MEM_LOG

/*
 * Time Stamp saving Activated.
 */
#define APPL_INIT_ADVANCED_F_TIME_STAMP        BCM_INIT_ADVANCED_F_TIME_STAMP
/*
 * Time Stamp saving Activated.
 */
#define APPL_INIT_ADVANCED_F_TIME_LOG          BCM_INIT_ADVANCED_F_TIME_LOG
/*
 * SW state resources Logging
 */
#define APPL_INIT_ADVANCED_F_SWSTATE_LOG       BCM_INIT_ADVANCED_F_SWSTATE_LOG
/*
 * Access only
 */
#define APPL_INIT_ADVANCED_F_ACCESS_ONLY       BCM_INIT_ADVANCED_F_ACCESS_ONLY
/*
 * Multithreading activated.
 */
#define APPL_INIT_ADVANCED_F_MULTITHREAD       BCM_INIT_ADVANCED_F_MULTITHREAD
/*
 * KBP Multithreading activated.
 */
#define APPL_INIT_ADVANCED_F_KBP_MULTITHREAD   BCM_INIT_ADVANCED_F_KBP_MULTITHREAD
/*
 * Heat-up
 */
#define APPL_INIT_ADVANCED_F_HEAT_UP           BCM_INIT_ADVANCED_F_HEAT_UP

/**
 * \brief
 * Time threshold flag EXPLICIT - used for testing how much time each step in the Init takes.
 * If EXPLICIT flag is specified, the value will be taken from a user-defined array(appl_init_time_thresh_expl).
 * If no flag is specified, default value (APPL_INIT_STEP_TIME_THRESH_DEFAULT, defined at appl_ref_init_utils.h) will be assumed.
 * If flag is set to PER_DEVICE the value will be taken from device specific data.
 */
#define APPL_INIT_ADVANCED_F_TIME_THRESH_EXPLICIT      UTILEX_SEQ_STEP_F_TIME_THRESH_EXPLICIT
#define APPL_INIT_ADVANCED_F_TIME_THRESH_PER_DEVICE    UTILEX_SEQ_STEP_F_TIME_THRESH_PER_DEVICE

/*
 * Default time threshold (in microseconds) for the time each
 * step is taking during init. This value was chosen because the
 * majority of the steps are below it. For steps that are
 * expected to be above the default, a specific threshold can be
 * set.
 */
#define APPL_INIT_STEP_TIME_THRESH_DEFAULT       100000

/*
 * Structs and Enums:
 * {
 */

/**
 * \brief Application init step/sub-step structure.
 *
 * Each step in the application Init/Deinit process should be well defined.
 * Each step should have an Init and Deinit function.
 * General step flags are required so applications can be masked using soc properties and invoked within
 * the diag shell.
 */
typedef struct appl_dnxc_init_step_s appl_dnxc_init_step_t;
struct appl_dnxc_init_step_s
{
    /**
     * step id
     */
    int step_id;
    /**
     * Step name
     */
    char *name;
    /**
     * enable/disable soc property suffix
     */
    char *suffix;
    /**
     * Callback to add function
     */
    utilex_seq_cb_f init_func;
    /**
     * Callback to remove function
     */
    utilex_seq_cb_f deinit_func;
    /** 
     * Flag function CB that will run prior to the init function of
     * this step to determine which flags are needed to the step
     * according to the CB logic - could be looking for certain SOC
     * properties for example.
     */
    utilex_seq_flag_cb_f flag_function;
    /**
     * Step Flags, internal flags used by the system's logic
     */
    int step_flags;

    /**
     * List of substeps (optional - might be null if not required)
     */
    const appl_dnxc_init_step_t *sub_list;

};

/**
 * \brief Structure holding information for steps and their relative time thresholds.
 *
 * This structure is used for steps with APPL_INIT_ADVANCED_F_TIME_THRESH_EXPLICIT
 * flag in order to link the Step ID to the specific user defined time threshold.
 */
typedef struct
{
  /**
   * Step ID, used to uniquely identify a step.
   */
    int step_id;

  /**
   * Time threshold - the maximum time the step should take in
   * microseconds.
   */
    sal_time_t time_thresh;
} appl_init_time_thresh_info_t;

/*
 * }
 */


/*
* Function Declarations:
* {
*/

/**
 * \brief - Deep conversion of appl_dnxc_init_step_t to utilex_seq_step_t 
 *  The conversion allocate memory that should be freed using 'dnx_step_list_destroy()' 
 *    
 * \param [in] unit - Unit ID
 * \param [in] dnx_step_list -  Application list of steps
 * \param [in] step_list - utilex list of steps
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
appl_dnxc_init_step_list_convert(
    int unit,
    const appl_dnxc_init_step_t * dnx_step_list,
    utilex_seq_step_t ** step_list);

/**
 * \brief - Converting dnxc application sequence to utilex_seq
 *
 * \param [in] unit - Unit ID
 * \param [in] step_list - list of steps 
 * \param [in] forward - whether to run forward or backward functions 
 * \param [in] seq - returns utilex sequence
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
appl_dnxc_init_seq_convert(
    int unit,
    const appl_dnxc_init_step_t * step_list,
    int forward,
    utilex_seq_t * seq);

/**
 * \brief - Running dnxc application list backward and forward
 * 
 * \param [in] unit - Unit ID
 * \param [in] step_list - a list of steps
 * \param [in] forward - Whether to run the list forward (TRUE - forward, FALSE - backward)
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
appl_dnxc_steps_convert_and_run(
    int unit,
    const appl_dnxc_init_step_t *step_list,
    int forward);


/**
 * \brief - Deallocate an allocated list of utilex steps
 * 
 * \param [in] unit - Unit ID
 * \param [in] step_list - List of steps
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
appl_dnxc_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list);
/*
 * }
 */

/* } */
#endif
