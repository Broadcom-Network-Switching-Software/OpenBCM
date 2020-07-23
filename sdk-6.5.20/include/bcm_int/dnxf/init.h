/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stat.h
 * Purpose:     STAT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_INIT_H_
#define   _BCM_INT_DNXF_INIT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>
#include <sal/core/time.h>

/**
 * Default time threshold (in microseconds) for the time each
 * step is taking during init. This value was chosen because the
 * majority of the steps are below it. For steps that are
 * expected to be above the default DNXF Data can be used.
 */
#define BCM_DNXF_INIT_STEP_TIME_THRESH_DEFAULT       1000000

/**
 * \brief
 * Time threshold flags - used for testing how much time each step in the Init takes.
 * If flag is set to PER_DEVICE the value will be taken from DNX data. THIS FLAG CAN NOT BE USED FOR STEPS BEFORE DNX DATA IS INITIALIZED
 * If no flag is specified, default value will be assumed.
 */
#define DNXF_INIT_STEP_F_TIME_THRESH_PER_DEVICE   UTILEX_SEQ_STEP_F_TIME_THRESH_PER_DEVICE

/**
 * Skip Step due to WB
 */
#define DNXF_INIT_STEP_F_WB_SKIP                     UTILEX_SEQ_STEP_F_WB_SKIP

typedef enum
{
    /**
     * Invalid Value, used to indicate to APIs that this input is
     * not relevant
     */
    DNXF_INIT_STEP_INVALID_STEP = UTILEX_SEQ_STEP_INVALID,
    /**
     * Value used get the first step of the enum
     */
    DNXF_INIT_STEP_FIRST_STEP = UTILEX_SEQ_STEP_INVALID + 1,
    /**
     * INIT Mark SOC FLAGS NOT INITED
     */
    DNXF_INIT_STEP_MARK_NOT_INITED,
    /**
     * INIT DNX Data
     */
    DNXF_INIT_STEP_DNX_DATA,
    /**
     * INIT SW State and HA modules
     */
    DNXF_INIT_STEP_SW_STATE,
    /**
     * INIT SOC
     */
    DNXF_INIT_STEP_SOC,
    /**
     * INIT Port module
     */
    DNXF_INIT_STEP_PORT,
    /**
     * INIT Linkscan module
     */
    DNXF_INIT_STEP_LINKSCAN,
    /**
     * INIT Stat module
     */
    DNXF_INIT_STEP_STAT,
    /**
     * INIT Stack module
     */
    DNXF_INIT_STEP_STACK,
    /**
     * INIT Multicast module
     */
    DNXF_INIT_STEP_MULTICAST,
    /**
     * INIT Fabric module
     */
    DNXF_INIT_STEP_FABRIC,
    /**
     * INIT RX module
     */
    DNXF_INIT_STEP_RX,
    /**
     * INIT Tune
     */
    DNXF_INIT_STEP_TUNE,
    /**
     * Mark init done
     */
    DNXF_INIT_STEP_INIT_DONE,
    /**
     * Number of init steps.
     */
    DNXF_INIT_STEP_COUNT,
    /**
     * Dummy step, used to indicate the last step for a step list.
     */
    DNXF_INIT_STEP_LAST_STEP = UTILEX_SEQ_STEP_LAST
} dnxf_init_step_id_e;

/**
 * \brief ptr to step cb function returning shr_error_e, to be
 * used in each init/deinit step. the step_list_info and the
 * flags are used for internal purposes, and are managed by the
 * mechanism.
 */
typedef shr_error_e(
    *dnxf_step_cb) (
    int unit);

/**
 * \brief ptr to step flag cb function returning
 * shr_error_e, to be used in each init step to decode flags
 * according to a given step logic ( for example Step X should
 * be skipped if SOC property Y is set to value Z etc). to do
 * so, one would need to return in dynamic flags the
 * DNX_INIT_STEP_F_SKIP_DYNAMIC flag.
 */
typedef shr_error_e(
    *dnxf_step_flag_cb) (
    int unit,
    int *dynamic_flags);

/**
 * \brief step/sub-step structure.
 *
 * each step in the Init/Deinit process should be well defined.
 * each step should have an Init and Deinit function.
 * the Deinit function should not access the HW, just free SW resources.
 * a flag CB can also be used, so each step can activate flags according
 * to dedicated SOC properties or other logic.
 */
typedef struct dnxf_init_step_s dnxf_init_step_t;

struct dnxf_init_step_s
{
  /**
   * Step ID, used to uniquely identify a step.
   */
    dnxf_init_step_id_e step_id;

  /**
   * Step name
   */
    char *step_name;

  /**
   * Init function CB that will run during the Init Sequence.
   */
    dnxf_step_cb init_function;

  /**
   * Deinit function CB that will run during the Deinit Sequence.
   */
    dnxf_step_cb deinit_function;

  /**
   * Step Flags, internal flags used by the system's logic
   */
    int step_flags;

  /**
   * Time threshold - the maximum time the step should take in
   * microseconds. If set to 0 the default value will be used.
   */
    sal_time_t time_thresh;
};

/**
 * \brief dnxf init arguments structure.
 *
 * This structure should contain user's information.
 * currently soc properties can be used to update it
 * until advanced init is implemented
 */
typedef struct
{
  /**
   * this is used to start the init sequence from a certain step.
   * this will be used after stopping the init sequence with last
   * step. the default for this should be the
   * DNXF_INIT_STEP_LAST_STEP step id.
   */
    dnxf_init_step_id_e first_step;

  /**
   * this is used to stop the init sequence at a certain step.
   * to continue use the init function with first step defined as
   * next step of this. the default for this should be the
   * DNXF_INIT_STEP_LAST_STEP step id.
   */
    dnxf_init_step_id_e last_step;
} dnxf_init_info_t;

/**
 * \brief - Get the time threshold of a given step in BCM init according to the
 *          step ID and the step fags. If called with flag UTILEX_SEQ_STEP_INVALID
 *          will return the overall threshold for the full sequence. In this case
 *          flags are not relevant.
 *
 * \param [in] unit - Unit #
 * \param [in] step_id - Step ID
 * \param [in] flags - Step flags
 * \param [out] time_thresh - Time threshold for the specified step
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnxf_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t *time_thresh);

shr_error_e
dnxf_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list);

/**
 * \brief Convert dnxf_init_deinit_seq to utilex list
 */
shr_error_e
dnxf_init_step_list_convert(
    int unit,
    const dnxf_init_step_t * dnxf_step_list,
    utilex_seq_step_t ** step_list);

#endif /*_BCM_INT_DNXF_INIT_H_*/
