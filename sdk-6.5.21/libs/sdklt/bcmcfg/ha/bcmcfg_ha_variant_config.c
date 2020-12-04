/*! \file bcmcfg_ha_variant_config.c
 *
 * BCMCFG HA(High Availability) related functions for variant config data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_ha_internal.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/generated/bcmcfg_ha_config.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_HA

/*******************************************************************************
 * Private variables
 */

static bcmcfg_ha_alloc_info_t bcmcfg_ha_variant_config[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Get variant string.
 *
 * \param [in]    unit       Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static const char *
bcmcfg_ha_variant_config_get(int unit)
{
    int rv = SHR_E_NONE;
    const char *str = NULL;
    bcmcfg_config_data_t *user_config = NULL;
    uint64_t value;
    bcmlrd_variant_t lrd_variant;
    uint32_t field_idx;

    do {
        rv = bcmcfg_tbl_user_config_get(unit,
                                        DEVICE_CONFIGt,
                                        &user_config);
        if (SHR_FAILURE(rv) ||
            user_config == NULL) {
            break;
        }

        rv = bcmcfg_tbl_user_config_field_index_get(unit,
                                                    DEVICE_CONFIGt,
                                                    DEVICE_CONFIGt_VARIANTf,
                                                    0, &field_idx);
        if (SHR_FAILURE(rv)) {
            break;
        }

        value = user_config[field_idx].value;

        lrd_variant = bcmlrd_variant_from_device_variant_t(value);

        if (lrd_variant != BCMLRD_VARIANT_T_NONE) {
            rv = bcmlrd_variant_set(unit, lrd_variant);
            if (SHR_FAILURE(rv)) {
               str = NULL;
               break;
            }
        }

        str = bcmlrd_variant_name(unit);
    } while (0);

    return str;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcfg_ha_variant_config_init(int unit, bool warm)
{
    uint32_t req_size;
    uint8_t *alloc_ptr = NULL;
    bcmcfg_ha_header_t *hdr;
    const char *variant_str = NULL;

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        variant_str = bcmcfg_ha_variant_config_get(unit);

        /* Add +2 for length and termination. */
        req_size = (variant_str) ? sal_strlen(variant_str) + 2 : 0;
    } else {
        req_size = 0;
    }
    SHR_IF_ERR_EXIT(
        bcmcfg_ha_unit_alloc(unit, warm, req_size,
                             "BCMCFG_VARIANT_CONFIG",
                             BCMCFG_HA_SUBID_VARIANT_CONFIG,
                             "bcmcfgVariantCfg",
                             BCMCFG_HA_SIGN_VARIANT_CONFIG,
                             &alloc_ptr));
    hdr = (bcmcfg_ha_header_t *) alloc_ptr;
    bcmcfg_ha_variant_config[unit].alloc_ptr = alloc_ptr;
    bcmcfg_ha_variant_config[unit].size = hdr->size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_variant_config_save(int unit, bool warm)
{
    uint8_t *ha_ptr = bcmcfg_ha_variant_config[unit].alloc_ptr;
    uint32_t size = bcmcfg_ha_variant_config[unit].size;
    uint8_t len;
    const char *variant_str = NULL;

    SHR_FUNC_ENTER(unit);

    if (size == 0) {
        SHR_EXIT();
    }

    variant_str = bcmcfg_ha_variant_config_get(unit);
    len = sal_strlen(variant_str);

    ha_ptr += sizeof(bcmcfg_ha_header_t);
    /* Length */
    *ha_ptr = len + 1;

    ha_ptr += 1;
    /* Variant string */
    sal_memcpy(ha_ptr, variant_str, len);

    ha_ptr += len;
    /* Termination */
    *ha_ptr = '\0';

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_variant_config_restore(int unit, bool warm)
{
    uint8_t *ha_ptr = bcmcfg_ha_variant_config[unit].alloc_ptr;
    uint32_t size = bcmcfg_ha_variant_config[unit].size;
    uint8_t len;
    char *variant_str = NULL;
    bcmlrd_variant_t lrd_variant;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (size == 0) {
        SHR_EXIT();
    }

    ha_ptr += sizeof(bcmcfg_ha_header_t);
    len = ha_ptr[0];

    SHR_ALLOC(variant_str, len, "bcmcfgVariantString");
    SHR_NULL_CHECK(variant_str, SHR_E_MEMORY);
    sal_memcpy(variant_str, &ha_ptr[1], len);

    rv = bcmlrd_variant_from_variant_string(variant_str, &lrd_variant);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Recovered variant %s not found\n"),
                              variant_str));
        SHR_ERR_EXIT(rv);
    } else if (lrd_variant != BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(bcmlrd_variant_set(unit, lrd_variant));
    }

exit:
    SHR_FREE(variant_str);
    SHR_FUNC_EXIT();
}

