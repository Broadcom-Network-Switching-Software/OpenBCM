/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        stat.h
 * Purpose:     STAT internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_INIT_H_
#define   _BCM_INT_DNXF_INIT_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>
#include <sal/core/time.h>

/**
 * \brief step required for access only initialization (allow light initialization with access only), or for heat-up
 */
#define DNXF_INIT_STEP_F_ACCESS                   (UTILEX_SEQ_STEP_F_REQUIRED_FOR_ACCESS_ONLY | UTILEX_SEQ_STEP_F_REQUIRED_FOR_HEAT_UP)

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
     * Init Info config
     */
    DNXF_INIT_STEP_INFO_CONFIG,
    /**
     * INIT DNX Data
     */
    DNXF_INIT_STEP_DNX_DATA,
    /**
     * INIT SOC
     */
    DNXF_INIT_STEP_SOC,
    /**
     * INIT Collect soc property info
     */
    DNXF_INIT_STEP_SOC_INFO_LEGACY,
    /**
     * INIT Do reset on the board and device blocks
     */
    DNXF_INIT_STEP_DEVICE_RESET,
    /**
     * INIT SW State and HA modules
     */
    DNXF_INIT_STEP_HA_MODULES,
    /**
     * Init Utilties modules
     */
    DNXF_INIT_STEP_UTILITIES,
    /**
     * Family
     */
    DNXF_INIT_STEP_FAMILY,
    /**
     * Init needed mutexes.
     */
    DNXF_INIT_STEP_MUTEXES,
    /**
     * Init needed counters.
     */
    DNXF_INIT_STEP_COUNTERS,
    /**
     * init access step list
     */
    DNXF_INIT_STEP_ACCESS,
    /**
     * init SBUS step
     */
    DNXF_INIT_STEP_SBUS,
    /**
     * Init Schan
     */
    DNXF_INIT_STEP_SCHAN,
    /**
     * Configuration of the endianness
     */
    DNXF_INIT_STEP_ENDIANNESS_CONFIG,
    /**
     * Rings s-bus and broadcast blocks
     */
    DNXF_INIT_STEP_RING_CONFIG,
    /**
     * init Soft Reset step
     */
    DNXF_INIT_STEP_SOFT_RESET,
    /**
     * init DMA step
     */
    DNXF_INIT_STEP_DMA,
    /**
     * INIT Pre soft init
     */
    DNXF_INIT_STEP_PRE_SOFT_INIT,
    /**
     * INIT soft init
     */
    DNXF_INIT_STEP_SOFT_INIT,
    /**
     * Marks blocks as disabled/enabled
     */
    DNXF_INIT_STEP_BLOCKS_ENABLE,
    /**
     * Set the Broadcast configuration
     */
    DNXF_INIT_STEP_BRDC_CONFIG,
    /**
     * Shadow memory init
     */
    DNXF_INIT_STEP_SHADOW_MEMORY,
    /**
     * INIT SW State and HA
     */
    DNXF_INIT_STEP_HA,
    /**
     * INIT SW State
     */
    DNXF_INIT_STEP_SW_STATE,
    /**
     * INIT Adapter
     */
    DNXF_INIT_STEP_ADAPTER_REG_ACC,
    /**
     * INIT Initial blocks configurations
     */
    DNXF_INIT_STEP_BLOCKS_RESET,
    /**
     * INIT place blocks into soft init
     */
    DNXF_INIT_STEP_SOFT_INIT_IN,
    /**
     * INIT take blocks out of soft init
     */
    DNXF_INIT_STEP_SOFT_INIT_OUT,
    /**
     * INIT Counter attach
     */
    DNXF_INIT_STEP_COUNTER_ATTACH,
    /**
     * Perform Hard Reset
     */
    DNXF_INIT_STEP_HARD_RESET,
    /**
     * Iproc/PAXB configuration not configured earlier
     */
    DNXF_INIT_STEP_IPROC,
    /**
     * INIT interrupts
     */
    DNXF_INIT_STEP_INTERRUPTS,
    /**
     * Set memory cacheable
     */
    DNXF_INIT_STEP_MEM_CACHEABLE,
    /**
     * INIT mark that SOC INIT is done
     */
    DNXF_INIT_STEP_SOC_INIT_DONE,
    /**
     * INIT Common modules
     */
    DNXF_INIT_STEP_COMMON_MODULES,
    /**
     * SER interrupt handler
     */
    DNXF_INIT_STEP_SER,
    /**
     * INIT Port module
     */
    DNXF_INIT_STEP_PORT,
    /**
     * INIT SyncE module
     */
    DNXF_INIT_STEP_SYNCE,
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
     * INIT TX module
     */
    DNXF_INIT_STEP_TX,
    /**
     * Configure mem monitor mask.
     */
    DNXF_INIT_STEP_MEM_MASK,
    /**
     * INIT Tune
     */
    DNXF_INIT_STEP_TUNE,
    /**
     * Link mapping step
     */
    DNXF_INIT_STEP_LINK_MAPPING,
    /**
     * Mark init done
     */
    DNXF_INIT_STEP_INIT_DONE,
    /**
     * Threads init.
     */
    DNXF_INIT_STEP_THREADS,
    /**
     * Interrupt event thread
     */
    DNXF_INIT_STEP_INTR_EVT_THREAD,
    /**
     * MDIO init.
     */
    DNXF_INIT_STEP_MDIO,
    /**
     * PLLs init.
     */
    DNXF_INIT_STEP_PLL,
    /**
     * Core clock verification step.
     */
    DNXF_INIT_STEP_CORE_CLOCK_VERIFY,
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
 * Pointer to a sub list of steps to be used if current step
 * represents a sub list of steps.
 */
    const dnxf_init_step_t *step_sub_array;

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
 * \param [out] step_id_dnx_data - Structure per step_id.
 *    It will hold the data from the DNX DATA
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
    utilex_step_time_dnx_data * step_id_dnx_data);

shr_error_e dnxf_init_step_list_destory(
    int unit,
    utilex_seq_step_t * step_list);

/**
 * \brief Convert dnxf_init_deinit_seq to utilex list
 */
shr_error_e dnxf_init_step_list_convert(
    int unit,
    const dnxf_init_step_t * dnxf_step_list,
    utilex_seq_step_t ** step_list);

/**
 * \brief - First step for the init sequence
 */
shr_error_e dnxf_soc_flags_init(
    int unit);

/**
 * \brief - Init DNX data
 */
shr_error_e _dnxf_data_init(
    int unit);

/**
 * \brief - Last step for the deinit sequence
 */
shr_error_e dnxf_soc_flags_deinit(
    int unit);

/**
 * \brief - First and last step for the init/deinit step
 */
shr_error_e soc_dnxf_mark_not_inited(
    int unit);

/**
 * \brief - Mark INIT done
 */
shr_error_e dnxf_init_done_init(
    int unit);

/**
 * \brief - clear INIT done
 */
shr_error_e dnxf_init_done_deinit(
    int unit);

#endif /*_BCM_INT_DNXF_INIT_H_*/
