/*! \file bcmfp.c
 *
 * Init/Cleanup APIs for FP module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmfp/bcmfp_sysm.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_em_internal.h>
#include <bcmfp/bcmfp_em_ft_internal.h>
#include <bcmfp/bcmfp_ingress_internal.h>
#include <bcmfp/bcmfp_vlan_internal.h>
#include <bcmfp/bcmfp_egress_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmlrd/bcmlrd_map.h>
#include <shr/shr_util.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV


int
bcmfp_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_common_init(unit));

    /* Create the memory for all FP stages. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stages_create(unit));

    /* Initialize logical tables defines for each stage. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_variant_attach(unit));

    /*
     * If any of the stage LTs are not defined, free the
     * memory created for the stage.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_stages_destroy(unit));

    /* Initialize device specific function pointers. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_device_attach(unit));

    /* Misc initialization of qualifier and actions. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_misc_init(unit));

    /*
     * Adjust the qualifier ID and action IDs of diffrent
     * LT fields.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_fid_maps_adjust(unit));

    /* Initialize each stage if it is created. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_vlan_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_ft_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ingress_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_egress_init(unit));

    /* HA space allocation and initialization. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_vlan_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_ft_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ingress_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_egress_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_common_cleanup(unit));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_imm_reg(int unit)
{

   SHR_FUNC_ENTER(unit);

   SHR_IF_ERR_VERBOSE_EXIT(bcmfp_vlan_imm_register(unit));

   SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_imm_register(unit));

   SHR_IF_ERR_VERBOSE_EXIT(bcmfp_em_ft_imm_register(unit));

   SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ingress_imm_register(unit));

   SHR_IF_ERR_VERBOSE_EXIT(bcmfp_egress_imm_register(unit));

exit:
   SHR_FUNC_EXIT();
}

