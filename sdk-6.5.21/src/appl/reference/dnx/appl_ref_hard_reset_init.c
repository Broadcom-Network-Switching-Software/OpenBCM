/** \file appl_ref_hard_reset_init.c
 * 
 *
 * L@ application procedures for DNX. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DEVICE

 /*
  * Include files.
  * {
  */
#include <shared/bsl.h>
#include <shared/bslenum.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <soc/i2c.h>
#include <sal/appl/i2c.h>

/*
 * }
 */

/*
 * DEFINEs
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

 /*
  * Global and Static
  */
/*
 * }
 */

/**
* \brief
*   This function is informing the user that hard reset is
*   required.
* \param [in] unit - The unit number.
*
* \return
*   Error code (as per 'shr_error_e').
* \see
*   shr_error_e
*/
void
appl_dnx_inout_hard_reset_handle(
    int unit,
    uint32 flags,
    void *userdata)
{
    LOG_ERROR(BSL_LS_BCMDNX_DEVICE, (BSL_META("%d Fatal Error - Device Hard Reset is required! \n"), unit));
}

 /**
 * \brief
 *   This function does hard reset callback registration, part of
 *   application init
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_hard_reset_cb_register(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_switch_hard_reset_cb_register(unit, 0, appl_dnx_inout_hard_reset_handle, NULL));

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *   This function does unregistration of hard reset callback
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_hard_reset_cb_unregister(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_switch_hard_reset_cb_unregister(unit, appl_dnx_inout_hard_reset_handle, NULL));

exit:
    SHR_FUNC_EXIT;
}
