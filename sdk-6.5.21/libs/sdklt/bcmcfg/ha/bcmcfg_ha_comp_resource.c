/*! \file bcmcfg_ha_comp_resource.c
 *
 * BCMCFG HA(High Availability) related functions for component data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_ha_internal.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/comp/bcmcfg_comp_scanner_ha.h>
#include <bcmdrd_config.h>
#include <bcmcfg/generated/bcmcfg_ha_config.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_HA

/*******************************************************************************
 * Private variables
 */
static bcmcfg_ha_alloc_info_t bcmcfg_ha_comp_resource;

/*******************************************************************************
 * Private Functions
 */
static int
bcmcfg_ha_data_comp_resource_size_get(void)
{
    size_t i;
    uint32_t req_size = 0;
    const bcmcfg_comp_scanner_conf_t *conf = bcmcfg_component_conf;

    for (i = 0; i < conf->count; i++) {
        if ((conf->scanner[i]->data_size % sizeof(uint32_t)) != 0) {
            req_size = 0;
            break;
        }

        req_size += conf->scanner[i]->data_size;
   }

    return req_size;
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcfg_ha_comp_resource_init(bool warm)
{
    uint32_t req_size = 0;
    uint8_t *alloc_ptr;
    bcmcfg_ha_header_t *hdr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    req_size = bcmcfg_ha_data_comp_resource_size_get();

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_gen_alloc(warm, req_size,
                            "BCMCFG_COMP_RESOURCE",
                            BCMCFG_HA_SUBID_COMP_RESOURCE,
                            "bcmcfgSwComp",
                            BCMCFG_HA_SIGN_COMP_RESOURCE,
                            &alloc_ptr));
    hdr = (bcmcfg_ha_header_t *) alloc_ptr;
    bcmcfg_ha_comp_resource.alloc_ptr = alloc_ptr;
    bcmcfg_ha_comp_resource.size = hdr->size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_comp_resource_save(bool warm)
{
    uint8_t *ha_ptr = bcmcfg_ha_comp_resource.alloc_ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_data_comp_resource_issu_report());

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_data_comp_resource_save(ha_ptr));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_comp_resource_restore(bool warm)
{
    uint8_t *ha_ptr = bcmcfg_ha_comp_resource.alloc_ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_data_comp_resource_restore(ha_ptr));

exit:
    SHR_FUNC_EXIT();
}

