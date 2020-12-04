/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BCM_FLAIR_SUPPORT
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/custom.h>

#include <soc/dpp/fabric.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <appl/dpp/fabric_fgl/fgl_configuration.h>

int
jer_fabric_fgl_custom_handler(int unit, bcm_port_t port, int setget, int type, int len,
               uint32 *args, int *actual, void *user_data)
{
    int     rv = 0;

    switch (type) {
    case BCM_JER_FABRIC_FGL_EGRESS_CONFIG_GET:
        rv = soc_jer_fabric_fgl_egress_config_get(unit,(soc_fabric_fgl_egress_config_t *)args);
        break;
    case BCM_JER_FABRIC_FGL_EGRESS_CONFIG_SET:
        rv = soc_jer_fabric_fgl_egress_config_set(unit,*((soc_fabric_fgl_egress_config_t *)args));
        break;
    case BCM_JER_FABRIC_FGL_INGRESS_CONFIG_GET:
        rv = soc_jer_fabric_fgl_ingress_config_get(unit,(soc_fabric_fgl_ingress_config_t *)args);
        break;
    case BCM_JER_FABRIC_FGL_INGRESS_CONFIG_SET:
        rv = soc_jer_fabric_fgl_ingress_config_set(unit,*((soc_fabric_fgl_ingress_config_t *)args));
        break;
    case BCM_JER_FABRIC_FGL_LOOPBACK_CONFIG_GET:
        rv = soc_jer_fabric_fgl_loopback_config_get(unit,(soc_fabric_fgl_loopback_config_t *)args);
        break;
    case BCM_JER_FABRIC_FGL_LOOPBACK_CONFIG_SET:
        rv = soc_jer_fabric_fgl_loopback_config_set(unit,*((soc_fabric_fgl_loopback_config_t *)args));
        break;
    case BCM_JER_FABRIC_FGL_INGRESS_TDM_CONTEXT_DROP_GET:
        rv = soc_jer_fabric_fgl_ingress_tdm_context_drop_get(unit,(soc_fabric_fgl_ingress_tdm_context_drop_t *)args);
        break;
    case BCM_JER_FABRIC_FGL_INGRESS_TDM_CONTEXT_DROP_SET:
        rv = soc_jer_fabric_fgl_ingress_tdm_context_drop_set(unit,*((soc_fabric_fgl_ingress_tdm_context_drop_t *)args));
        break;
    case BCM_JER_FABRIC_EGR_MC_MAPPING_MODE_GET:
        rv = soc_jer_fabric_egr_mc_mapping_mode_get(unit,(int *)args);
        break;
    case BCM_JER_FABRIC_EGR_MC_MAPPING_MODE_SET:
        rv = soc_jer_fabric_egr_mc_mapping_mode_set(unit,*((int *)args));
        break;
    case BCM_JER_FABRIC_ECI_TDM_CONFIG_TABLE_GET:
        rv = soc_jer_fabric_eci_tdm_config_table_get(unit, (uint32)len,(soc_fabric_eci_egr_tdm_config_t *)args);
        break;
    case BCM_JER_FABRIC_ECI_TDM_CONFIG_TABLE_SET:
        rv = soc_jer_fabric_eci_tdm_config_table_set(unit,(uint32)len, *((soc_fabric_eci_egr_tdm_config_t *)args));
        break;
    case BCM_JER_FABRIC_ECI_EGR_PORT_MAP_TABLE_GET:
        rv = soc_jer_fabric_eci_egr_port_map_table_get(unit, (uint32)len,(soc_fabric_eci_egr_port_map_t *)args);
        break;
    case BCM_JER_FABRIC_ECI_EGR_PORT_MAP_TABLE_SET:
        rv = soc_jer_fabric_eci_egr_port_map_table_set(unit, (uint32)len,*((soc_fabric_eci_egr_port_map_t *)args));
        break;
    case BCM_JER_FABRIC_ECI_EGR_MCT_TABLE_GET:
        rv = soc_jer_fabric_eci_egr_mct_table_get(unit, (uint32)len,(soc_fabric_eci_egr_mct_t *)args);
        break;
    case BCM_JER_FABRIC_ECI_EGR_MCT_TABLE_SET:
        rv = soc_jer_fabric_eci_egr_mct_table_set(unit,(uint32)len, *((soc_fabric_eci_egr_mct_t *)args));
        break;
    case BCM_JER_FABRIC_ECI_EGR_MCT_MAPPING_TABLE_GET:
        rv = soc_jer_fabric_eci_egr_mct_mapping_table_get(unit, (uint32)len,(soc_fabric_eci_egr_mct_mapping_t *)args);
        break;
    case BCM_JER_FABRIC_ECI_EGR_MCT_MAPPING_TABLE_SET:
        rv = soc_jer_fabric_eci_egr_mct_mapping_table_set(unit,(uint32)len, *((soc_fabric_eci_egr_mct_mapping_t *)args));
        break;
    default:
        break;
    }

    return rv;
}

int jer_fabric_fgl_custom_api_init(int unit)
{
    int     rv = 0;

    if(!SOC_IS_FLAIR(unit)) {
        LOG_ERROR(BSL_S_FABRIC, (BSL_META_U(unit,"Available for Flair only.")));
        return SOC_E_UNAVAIL;
    }

    rv = bcm_custom_register(unit, jer_fabric_fgl_custom_handler, NULL);

    return rv;
}

int jer_fabric_fgl_custom_api_deinit(int unit)
{
    int     rv = 0;

    if(!SOC_IS_FLAIR(unit)) {
        LOG_ERROR(BSL_S_FABRIC, (BSL_META_U(unit,"Available for Flair only.")));
        return SOC_E_UNAVAIL;
    }

    rv = bcm_custom_unregister(unit);

    return rv;
}

#endif /* BCM_FLAIR_SUPPORT */

