/** \file crps_ctrs_db.c
 * 
 *  crps module gather all functionality that related to the counter/statisitc processor driver for DNX.
 *  crps_ctrs_db.c responsible of the counters memory space database. In allocate/free sw counters memory set values in the memory and read from it.
 *  It is internal crps file . (do not have interfaces with other modules)
 *  THis file implemantion can be modified by customers who want to replace the sw counters memory handling/management
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_crps.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/crps_access.h>
#include <include/shared/utilex/utilex_integer_arithmetic.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/stat/crps/crps_mgmt.h>

/*
 * }
 */

/*
 * DEFINES
 * {
 */

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

/** see .h file */
shr_error_e
dnx_crps_ctrs_db_init(
    int unit)
{
    uint8 is_alloc;
    int nof_cores, core_id, nof_engines, engine_id;
    const dnx_data_crps_engine_engines_info_t *engine_info;

    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    nof_engines = dnx_data_crps.engine.nof_engines_get(unit);

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        /** for each engine allocate array of SW counters, according to the engine_size*SUB_COUNT. */
        /** driver holds 2 uint64 counters for each HW counter (one for packet and one for bytes */
        for (engine_id = 0; engine_id < nof_engines; engine_id++)
        {
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.is_allocated(unit, core_id, engine_id, &is_alloc));
            if (is_alloc == FALSE)
            {
                engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.alloc(unit, core_id, engine_id,
                                                               (engine_info->nof_counters * DNX_CRPS_MGMT_SUB_COUNT)));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INIT,
                             "dnx_crps_db.proc.counter.is_allocated=TRUE for unit=%d, core=%d, engine=%d\n",
                             unit, core_id, engine_id);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ctrs_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state memory free is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ctrs_db_counter_set(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int counter_id,
    int sub_counter,
    bcm_stat_counter_format_type_t format,
    uint64 value)
{
    int ofs;
    uint64 old_val;
    uint64 counter;
    SHR_FUNC_INIT_VARS(unit);

    ofs = counter_id * DNX_CRPS_MGMT_SUB_COUNT;
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.get(unit, core_id, engine_id, (ofs + sub_counter), &old_val));
    /** update the counter with new value */
    if (_SHR_IS_FLAG_SET(flags, DNX_CRPS_CTRS_OVERWRITE_COUNTER_VALUE))
    {
        /** update counter into sw state */
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.set(unit, core_id, engine_id, (ofs + sub_counter), value));
    }
    /** increase counter */
    else
    {
        if (format == bcmStatCounterFormatMaxSize)
        {
            if (COMPILER_64_GT(value, old_val))
            {
                /** update counter into sw state */
                SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.set(unit, core_id, engine_id, (ofs + sub_counter), value));
            }
        }
        else
        {
            COMPILER_64_ZERO(counter);
            COMPILER_64_ADD_64(counter, old_val);
            COMPILER_64_ADD_64(counter, value);
            if (COMPILER_64_LT(counter, old_val))
            {
                /** dont allow wrap; saturate instead */
                COMPILER_64_ALLONES(counter);
            }
            /** update counter into sw state */
            SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.set(unit, core_id, engine_id, (ofs + sub_counter), counter));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_crps_ctrs_db_counter_get(
    int unit,
    int flags,
    int core_id,
    int engine_id,
    int counter_id,
    int sub_counter,
    uint64 *value)
{
    int ofs;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);

    ofs = counter_id * DNX_CRPS_MGMT_SUB_COUNT;
    SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.get(unit, core_id, engine_id, (ofs + sub_counter), value));
    /*
     * if flag clear on read is set - after the value is get,
     *  clear the counter
     */
    COMPILER_64_ZERO(work_reg_64);
    if (_SHR_IS_FLAG_SET(flags, BCM_STAT_COUNTER_CLEAR_ON_READ))
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.set(unit, core_id, engine_id, (ofs + sub_counter), work_reg_64));
    }

exit:
    SHR_FUNC_EXIT;

}

/** see .h file */
shr_error_e
dnx_crps_ctrs_db_counter_clear(
    int unit,
    int core_id,
    int engine_id)
{
    int counter_index;
    uint8 is_alloc;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    uint64 work_reg_64;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.is_allocated(unit, core_id, engine_id, &is_alloc));
    /** check that memory is allocated */
    if (is_alloc == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_crps_db.proc.counter.is_allocated=FALSE for unit=%d, core=%d, engine=%d\n",
                     unit, core_id, engine_id);
    }
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
    /** run over all counters for this engine and set each one to zero */
    COMPILER_64_ZERO(work_reg_64);
    for (counter_index = 0; counter_index < (engine_info->nof_counters * DNX_CRPS_MGMT_SUB_COUNT); counter_index++)
    {
        SHR_IF_ERR_EXIT(dnx_crps_db.proc.counter.set(unit, core_id, engine_id, counter_index, work_reg_64));
    }

exit:
    SHR_FUNC_EXIT;
}
