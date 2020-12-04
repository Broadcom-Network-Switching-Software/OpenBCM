
/** \file oam_lm_dm.c
 * 
 *
 * OAM LM DM procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

/* { */
 /*
  * Include files.
  * {
  */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_bfd.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm/oam.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <include/bcm_int/dnx/algo/oam/algo_oam.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#ifdef BCM_DNX2_SUPPORT
/* { */
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1.h>
/* } */
#endif

/**
 * \brief - Add an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_add(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_loss_t), loss_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_loss_add(unit, loss_ptr));
/* } */
#endif
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_get(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_loss_t), loss_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_loss_get(unit, loss_ptr));
/* } */
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_delete(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_loss_t), loss_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_loss_delete(unit, loss_ptr));
/* } */
#endif
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an OAM Delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_add(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_delay_t), delay_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_delay_add(unit, delay_ptr));
/* } */
#endif
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_get(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_delay_t), delay_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_delay_get(unit, delay_ptr));
/* } */
#endif
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Delete an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_delete(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    DNX_ERR_RECOVERY_REGRESSION_RESTORE_IN_OUT_VAR_PTR(unit, sizeof(bcm_oam_delay_t), delay_ptr);

    {
#ifdef BCM_DNX2_SUPPORT
/* { */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_v1_delay_delete(unit, delay_ptr));
/* } */
#endif
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}
