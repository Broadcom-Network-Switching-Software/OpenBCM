/*
 *
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_chain.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X10TD_SUPPORT

#include <soc/portmod/xlmac.h>
#include <soc/portmod/pm4x10.h>

int pm4x10td_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (READ_PGW_CNTMAXSIZEr(unit, port, (uint32_t *)val));
}

int pm4x10td_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    int rv = 0;
    uint32 reg_val;

    rv = READ_PGW_CNTMAXSIZEr(unit, port, &reg_val);
    if (rv) return (rv);

    soc_reg_field_set(unit, PGW_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    return(WRITE_PGW_CNTMAXSIZEr(unit, port, reg_val));
}

int pm4x10td_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_PGW_MAC_RSV_MASKr(unit, port, rsv_mask));
}

int pm4x10td_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    /* PGW_MIB_RESET needs to be controlled in portctrl layer
       Do nothing in Portmod */
    return 0;
}

int pm4x10td_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                           const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_set(unit, port, pfc_cfg));
exit:
    SOC_FUNC_RETURN;
}

int pm4x10td_port_pfc_config_get (int unit, int port,pm_info_t pm_info,
                                portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(xlmac_pfc_config_get(unit, port, pfc_cfg));

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM4X10TD_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
