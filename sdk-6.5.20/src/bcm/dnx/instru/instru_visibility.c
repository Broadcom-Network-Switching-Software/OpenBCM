/** \file instru_visibility.c
 * 
 *
 * Visibility procedures for DNX in instrumentation module.
 *
 * This file contains functions for visibility configuration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU
/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/sand/sand_signals.h>

#include <bcm/instru.h>
#include <bcm_int/dnx/instru/instru_visibility.h>

/*
 * }
 */

/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */
 /*
  * }
  */

 /*
  * Internal functions.
  * {
  */

 /*
  * }
  */

/**
 * \brief - Resumes ability to record signals from new packet
 *
 * \param [in] unit - Relevant unit
 * \param [in] core_id - Core number inside unit
 * \param [in] flags - Flags controlling visibility status, see BCM_INSTRU_CONTROL_FLAG_XXX
 *
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_instru_vis_signal_get
 */
shr_error_e
bcm_dnx_instru_vis_resume(
    int unit,
    bcm_core_t core_id,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_visibility_resume(unit, core_id, flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets signal value and size according to the key
 *
 * \param [in] unit - Relevant unit
 * \param [in] core_id - Core number inside unit
 * \param [in] flags - Misc flags for signal fetching BCM_INSTRU_VIS_SIGNAL_FLAG_XXX
 * \param [in] nof_signals - Size of signal_key/signal_result arrays
 * \param [in] signal_key - Pointer to array of signal keys
 * \param [out] signal_result - Pointer to array of signal results, memory allocated by caller, filled by driver
 *
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   After each packet bcm_dnx_instru_vis_resume should be run to allow the chip new packet record
 *
 * \see
 *   bcm_dnx_instru_vis_resume
 */
shr_error_e
bcm_dnx_instru_vis_signal_get(
    int unit,
    bcm_core_t core_id,
    uint32 flags,
    int nof_signals,
    bcm_instru_vis_signal_key_t * signal_key,
    bcm_instru_vis_signal_result_t * signal_result)
{
    int i_sig;
    bcm_instru_vis_signal_key_t *cur_key;
    bcm_instru_vis_signal_result_t *cur_result;
    signal_output_t *signal_output = NULL;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    cur_key = signal_key;
    cur_result = signal_result;
    for (i_sig = 0; i_sig < nof_signals; i_sig++)
    {
        cur_result->status = sand_signal_output_find(unit, core_id, 0, cur_key->block, cur_key->from,
                                                     cur_key->to, cur_key->signal, &signal_output);

        if (cur_result->status != BCM_E_NONE)
        {
            if (flags & BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR)
            {
                SHR_ERR_EXIT(cur_result->status, "");
            }
        }
        else
        {
            int i_val;
            cur_result->size = signal_output->size;
            for (i_val = 0; i_val < UTILEX_DIV_ROUND_UP(cur_result->size, 32); i_val++)
            {
                cur_result->value[i_val] = signal_output->value[i_val];
            }

            /** update print_value as well */
            sal_strncpy(cur_result->print_value, signal_output->print_value,
                        BCM_INSTRU_VIS_SIGNAL_PRINT_VALUE_MAX_SIZE_STR - 1);
        }

        cur_key++;
        cur_result++;
    }

exit:
    sand_signal_output_free(signal_output);
    SHR_FUNC_EXIT;
}
