/*! \file bcmcfg_lt_config_base_variant_set.c
 *
 * Set variant to BASE if the config variant is unavailable.
 * When DEVICE.VARIANT is not specified, default variant will be applied.
 * If default variant is unavailable (excluded by setting compilation flags),
 * then apply BASE variant.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Public functions
 */
int
bcmcfg_lt_config_base_variant_set(int unit, bool warm)
{
    uint32_t sid = DEVICE_CONFIGt;
    uint32_t fid = DEVICE_CONFIGt_VARIANTf;
    bcmcfg_config_data_t *user_config = NULL;
    uint32_t default_count;
    uint64_t default_variant = 0;
    uint64_t base_variant = 0;
    bcmlrd_variant_t variant;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* No config is specified. */
    SHR_IF_ERR_EXIT(
        bcmcfg_tbl_user_config_get(unit, sid, &user_config));
    if (!user_config ||
        !(user_config[fid].config_src == BCMCFG_CONFIG_SRC_TYPE_READER ||
          user_config[fid].config_src == BCMCFG_CONFIG_SRC_TYPE_API)) {
        /* Get default variant and check if default variant is available or not. */
        rv = bcmlrd_field_default_get(unit,
                                      DEVICE_CONFIGt,
                                      DEVICE_CONFIGt_VARIANTf,
                                      1,
                                      &default_variant,
                                      &default_count);
        if (SHR_SUCCESS(rv) &&
            default_count == 1 &&
            default_variant != 0) {

            variant = bcmlrd_variant_from_device_variant_t(default_variant);
            /* default variant is unavailable. */
            if (variant == BCMLRD_VARIANT_T_NONE) {
                variant = bcmlrd_base_get(unit);

                base_variant = bcmlrd_device_variant_t_from_variant(variant);
                SHR_IF_ERR_EXIT(
                    bcmcfg_table_set_internal(unit, sid, fid, 0, 1,
                                              &base_variant, NULL));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
