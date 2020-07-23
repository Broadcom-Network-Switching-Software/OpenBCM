/*! \file bcmcfg_ha.c
 *
 * BCMCFG HA(High Availability) related functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_ha.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_ha_internal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcfg/generated/bcmcfg_ha_config.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_HA

/*******************************************************************************
 * Public Functions
 */

int
bcmcfg_ha_unit_alloc(int unit, bool warm, uint32_t in_req_size,
                     const char *submod_name, uint8_t submod_id,
                     const char *submod_string,
                     uint32_t ref_sign, uint8_t **alloc_ptr)
{
    uint32_t req_size, alloc_size, obs_sign;
    uint32_t hdr_size = sizeof(bcmcfg_ha_header_t);
    SHR_FUNC_ENTER(unit);

    req_size = in_req_size + hdr_size;
    alloc_size = req_size;
    if (unit == BSL_UNIT_UNKNOWN) {
        *alloc_ptr = shr_ha_gen_mem_alloc(BCMMGMT_CFG_COMP_ID,
                                          submod_id,
                                          submod_string,
                                          &alloc_size);
    } else {
        *alloc_ptr = shr_ha_mem_alloc(unit,
                                      BCMMGMT_CFG_COMP_ID,
                                      submod_id,
                                      submod_string,
                                      &alloc_size);
    }

    if (alloc_size < req_size) {
        if (unit == BSL_UNIT_UNKNOWN) {
            *alloc_ptr = shr_ha_gen_mem_realloc(*alloc_ptr, req_size);
        } else {
            *alloc_ptr = shr_ha_mem_realloc(unit,
                                            *alloc_ptr,
                                            req_size);
        }
    }
    SHR_NULL_CHECK(*alloc_ptr, SHR_E_MEMORY);

    if (warm) {
        bcmcfg_ha_header_t *hdr = (bcmcfg_ha_header_t *) *alloc_ptr;
        obs_sign = hdr->signature;
        SHR_IF_ERR_MSG_EXIT(
            obs_sign != ref_sign ? SHR_E_FAIL : SHR_E_NONE,
            (BSL_META_U(unit, "WB: %s: signature mismatch, "
                        "exp=0x%8x, obs=0x%8x\n"),
             submod_name, ref_sign, obs_sign));
    } else { /* init ref_sign */
        bcmcfg_ha_header_t *hdr;

        sal_memset(*alloc_ptr, 0, alloc_size);
        hdr = (bcmcfg_ha_header_t *) (*alloc_ptr);
        hdr->signature = ref_sign;
        hdr->size = in_req_size;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "%s: req bytes = %0u, alloc bytes = %0u, \n"),
         submod_name, in_req_size, alloc_size));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_gen_alloc(bool warm, uint32_t in_req_size,
                    const char *submod_name, uint8_t submod_id,
                    const char *submod_string,
                    uint32_t ref_sign, uint8_t **alloc_ptr)
{
    return bcmcfg_ha_unit_alloc(BSL_UNIT_UNKNOWN,
                                warm,
                                in_req_size,
                                submod_name,
                                submod_id,
                                submod_string,
                                ref_sign,
                                alloc_ptr);
}

int
bcmcfg_ha_generic_init(bool warm)
{
    int rv = SHR_E_NONE;

    do {
        rv = bcmcfg_ha_comp_resource_init(warm);
        if (SHR_FAILURE(rv)) {
            break;
        }

        if (!warm) {
            rv = bcmcfg_ha_comp_resource_save(warm);
            if (SHR_FAILURE(rv)) {
                break;
            }
        } else {
            rv = bcmcfg_ha_comp_resource_restore(warm);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
    } while(0);

    return rv;
}

int
bcmcfg_ha_generic_deinit(void)
{
    int rv;

    rv = bcmcfg_read_component_free(bcmcfg_component_conf);

    return rv;
}

int
bcmcfg_ha_data_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_lt_config_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_data_save(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_lt_config_save(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_data_restore(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_lt_config_restore(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

