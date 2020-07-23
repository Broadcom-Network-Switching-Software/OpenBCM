/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * StrataSwitch FCMAP PHY control
 * FCMAP PHY initialization 
 */


#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/phy/phyctrl.h>
#include <soc/debug.h>

#ifdef INCLUDE_FCMAP
#include <bfcmap.h>
#if defined(INCLUDE_PHY_8806X)
#include <src/soc/phy/phy8806x.h>
#endif /* INCLUDE_PHY_8806X */

#include <soc/fcmapphy.h>


/*
 * Function:
 *      soc_fcmapphy_init
 * Purpose:
 *      Initializes the FCMAP phy port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pc  - Phy control structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_fcmapphy_init(int unit, soc_port_t port, phy_ctrl_t *pc, 
                   bfcmap_core_t core_type, bfcmap_dev_io_f iofn)
{
    int                     /* ii, */ p, rv;
    int                     fcmap_enable = 0; 

    p = SOC_FCMAP_PORTID(unit, port);
    /* Destroy the port if it were created before. */
    (void)bfcmap_port_destroy(p);

    fcmap_enable = soc_property_port_get(unit, port, spn_FCMAP_ENABLE, 0);
    pc->fcmap_enable = fcmap_enable;

    if (fcmap_enable) {
        /* Create FCMAP Port */
        rv = bfcmap_port_create(p, core_type, pc->fcmap_dev_addr, 
                                 pc->fcmap_uc_dev_addr, 
                                 pc->fcmap_dev_port, iofn, pc->fcmap_port_cfg);
        if (rv != BFCMAP_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_fcmapphy_init: "
                                 "FCMAP port create failed for u=%d p=%d rv = %d\n"),
                      unit, port, rv));
            return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_fcmapphy_uninit
 * Purpose:
 *      Uninitializes the FCMAP phy port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_fcmapphy_uninit(int unit, soc_port_t port)
{
    int p;

    p = SOC_FCMAP_PORTID(unit, port);
    /* Destroy the port if it were created before. */
    (void)bfcmap_port_destroy(p);

    return SOC_E_NONE;

}

int 
soc_fcmapphy_miim_read(soc_fcmap_dev_addr_t dev_addr, 
                        uint32 phy_reg_addr, uint16 *data)
{
    int unit, phy_id, clause45;
    int rv = SOC_E_NONE;

    /*
     * Decode dev_addr into phyid and unit.
     */
    unit = SOC_FCMAPPHY_ADDR2UNIT(dev_addr);
    phy_id = SOC_FCMAPPHY_ADDR2MDIO(dev_addr);
    clause45 = SOC_FCMAPPHY_ADDR_IS_CLAUSE45(dev_addr);

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_ESW(unit)) {
        if (clause45) {
            rv = soc_esw_miimc45_read(unit, phy_id, 
                                  phy_reg_addr, data);
        } else {
            rv = soc_miim_read(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_ESW_SUPPORT */

    return (rv == SOC_E_NONE) ? 0 : -1;
}

int 
soc_fcmapphy_miim_write(soc_fcmap_dev_addr_t dev_addr, 
                         uint32 phy_reg_addr, uint16 data)
{
    int unit, phy_id, clause45;
    int rv = SOC_E_NONE;

    /*
     * Decode dev_addr into phyid and unit.
     */
    unit = SOC_FCMAPPHY_ADDR2UNIT(dev_addr);
    phy_id = SOC_FCMAPPHY_ADDR2MDIO(dev_addr);

    clause45 = SOC_FCMAPPHY_ADDR_IS_CLAUSE45(dev_addr);

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    if ( SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit) ) {
        return SOC_E_NONE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_ESW(unit)) {
        if (clause45) {
            rv = soc_esw_miimc45_write(unit, phy_id, 
                                   phy_reg_addr, data);
        } else {
            rv = soc_miim_write(unit, phy_id, phy_reg_addr, data);
        }
    }
#endif /* BCM_ESW_SUPPORT */

    return (rv == SOC_E_NONE) ? 0 : -1;
}


#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
extern int bfcmap_cli_sync_recovery_needed ;

int
soc_fcmapphy_blmi_enable(int enable) 
{
    int rv;

    bfcmap_cli_sync_recovery_needed = enable ? FALSE : TRUE;

    rv =  blmi_io_mmi1_operational_flag_set(
            enable ? 0 : BLMI_OP_FLAG_ACCESS_INHIBIT,
            BLMI_OP_FLAG_ACCESS_INHIBIT);

    if (rv != BFCMAP_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META(
                              "soc_fcmapphy_blmi_enable(%d): "
                              "FAILED ! rv = %d\n"), enable, rv));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


#else

int 
soc_fcmapphy_init(int unit, soc_port_t port, void *pc)
{
    return SOC_E_NONE;
}
#endif /* INCLUDE_FCMAP */

