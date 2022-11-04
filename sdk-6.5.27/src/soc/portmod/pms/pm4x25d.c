/*
 *
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *     
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
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_system.h>
#include <soc/ua.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25D_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>

/*******************************************************************************
 * Local defines.
 ******************************************************************************/
#define MAX_PORTS_PER_PM4X25D 4

/* register per logical or physical port number accessing */
#define PM4X25D_REG_READ(unit, reg_name, phy_acc, reg_val) \
        ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
         UA_REG_READ_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
         UA_REG_READ_PORT(unit, reg_name, phy_acc, 0, reg_val))


#define PM4X25D_REG_WRITE(unit, reg_name, phy_acc, reg_val) \
    ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
     UA_REG_WRITE_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
     UA_REG_WRITE_PORT(unit, reg_name, phy_acc, 0, reg_val))

/*******************************************************************************
 * PM driver private functions.
 ******************************************************************************/


/*******************************************************************************
 * PM driver function vector
 ******************************************************************************/

int pm4x25d_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if (pm_info->pm_data.pm4x25_db != NULL) {
        sal_free(pm_info->pm_data.pm4x25_db);
        pm_info->pm_data.pm4x25_db = NULL;
    }

    SOC_FUNC_RETURN; 
}

int pm4x25d_port_tsc_reset_set(int unit, int phy_acc,
                               pm_info_t pm_info, int in_reset)
{
    uint32 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    /* Bring Internal Phy OOR */
    _SOC_IF_ERR_EXIT(PM4X25D_REG_READ(unit, CLPORT_XGXS0_CTRL_REG, phy_acc, &reg_val));
    fld_val = in_reset ? 0 : 1;
    UA_REG_FIELD_SET(unit, CLPORT_XGXS0_CTRL_REG, &reg_val,
                     RSTB_HW, &fld_val);
    _SOC_IF_ERR_EXIT(PM4X25D_REG_WRITE(unit, CLPORT_XGXS0_CTRL_REG, phy_acc, &reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25d_port_drv_name_get (int unit, int port, pm_info_t pm_info, 
                              char* buf, int len)
{
    strncpy(buf, "PM4X25D Driver", len);
    return (SOC_E_NONE);
}

#endif /* PORTMOD_PM4X25D_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
