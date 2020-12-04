/*! \file bcmcth_meter_drv.c
 *
 * BCMCTH meter driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_meter_fp_ing.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>
#include <bcmcth/bcmcth_meter_sc.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc.h>
#include <bcmcth/bcmcth_meter_fp_ing_action.h>
#include <bcmcth/bcmcth_meter_fp_egr_action.h>

#include <bcmcth/bcmcth_meter_drv.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_drv_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_drv_init(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_drv_init(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_sc_drv_init(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_l2_iif_sc_drv_init(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_action_drv_init(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_action_drv_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_drv_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_sc_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_l2_iif_sc_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_ing_action_drv_cleanup(unit));
    SHR_IF_ERR_EXIT(bcmcth_meter_fp_egr_action_drv_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}
