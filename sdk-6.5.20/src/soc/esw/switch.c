/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC switch Utilities
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/scache.h>
#include <shared/alloc.h>
#include <soc/phy/phyctrl.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_CONTROL_WB_PART_0_VER_1_0       SOC_SCACHE_VERSION(1,0)
#define SOC_CONTROL_WB_PART_0_DEFAULT_VER   SOC_CONTROL_WB_PART_0_VER_1_0

/*
 * List of software-only switch controls that need to be recovered in Level 2.
 * There is no way to recover these controls in level 1 warmboot.
 *
 * The following enum is used to identify the boolean controls.
 */
enum soc_wb_boolean_controls_0_e {
    socControlUseGport = 0,
    socControlL2PortBlocking = 1,
    socControlAbortOnError = 2,
    socControlCount = 3
};

#define SOC_CONTROL_WB_NUM_ELE_0 8
static int soc_wb_boolean_controls_0[SOC_CONTROL_WB_NUM_ELE_0] =
{
    socControlUseGport,
    socControlL2PortBlocking,
    socControlAbortOnError,
    -1,
    -1,
    -1,
    -1,
    -1
};

/*
 * Function:
 *      soc_switch_control_scache_size_get
 * Description:
 *      Get the size of scache size required for level 2 WarmBoot.
 * Parameters:
 *      unit -    (IN) SOC device number.
 *      part -    (IN) Warmboot partition number.
 *      version - (IN) WarmBoot Level 2 scache version for switch module.
 * Returns:
 *      Size of scache space required in bytes.
 */
STATIC int
soc_switch_control_scache_size_get(int unit, int part, int version)
{
    int alloc_size = 0;

    if (part == SOC_SCACHE_SWITCH_CONTROL_PART0) {
        if (version >= SOC_CONTROL_WB_PART_0_VER_1_0) {
            alloc_size += 1;
        }

        /*
         * if (version >= SOC_CONTROL_WB_PART_0_VER_1_1) {
         *     // alloc_size += n;
         * }
 */

    }

    return alloc_size;
}

/*
 * Function:
 *      soc_switch_control_scache_init
 * Description:
 *      WarmBoot recovery procedure for recovering global switch controls.
 * Parameters:
 *      unit -    (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
soc_switch_control_scache_init(int unit)
{
    int rv = SOC_E_NONE;
    int i, arg, part;
    int stable_size;
    uint32 alloc_sz;
    uint8 *switch_scache;
    uint16 default_ver, recovered_ver;
    soc_scache_handle_t  scache_handle;
    int create = SOC_WARM_BOOT(unit) ? FALSE : TRUE;

    /* Limited scache mode unsupported */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (!stable_size) {
        /* stable not allocated */
        return SOC_E_NONE;
    }

    part = SOC_SCACHE_SWITCH_CONTROL_PART0;
    default_ver = SOC_CONTROL_WB_PART_0_DEFAULT_VER;

    alloc_sz = soc_switch_control_scache_size_get(unit, part, default_ver);

    SOC_SCACHE_ALIGN_SIZE(alloc_sz);
    alloc_sz += SOC_WB_SCACHE_CONTROL_SIZE;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_SWITCH_CONTROL, part);

    /* Get the pointer for the Level 2 cache */
    rv = soc_extended_scache_ptr_get(unit, scache_handle, create,
                                     create ? alloc_sz : 0,
                                     &switch_scache);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    } else if (SOC_E_NOT_FOUND == rv && !create) {
        /* Not yet allocated in upgrade */
        rv = soc_extended_scache_ptr_get(unit, scache_handle, TRUE,
                                         alloc_sz,
                                         &switch_scache);
        if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
            return rv;
        }
    } else if (SOC_FAILURE(rv)) {
        return rv;
    }

    /* recover version info */
    sal_memcpy(&recovered_ver, switch_scache, sizeof(uint16));

    /* Advance over scache control info */
    switch_scache += SOC_WB_SCACHE_CONTROL_SIZE;

    /* If cold-booting or re-init in cold, return from here */
    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_NONE;
    }

    /* Scache Version 1.0 */
    if (recovered_ver >= SOC_CONTROL_WB_PART_0_VER_1_0) {
        for (i = 0; (i < SOC_CONTROL_WB_NUM_ELE_0); i++) {
            if (-1 != soc_wb_boolean_controls_0[i]) {
                arg = !!(*switch_scache & (1 << i));
                switch (i) {
                    case socControlUseGport:
                        SOC_USE_GPORT_SET(unit, arg);
                        break;
                    case socControlL2PortBlocking:
                        SOC_L2X_GROUP_ENABLE_SET(unit, arg);
                        break;
                    case socControlAbortOnError:
#ifdef BCM_CB_ABORT_ON_ERR
                        SOC_CB_ABORT_ON_ERR(unit) = arg;
#endif /* BCM_CB_ABORT_ON_ERR */
                        break;
                    default:
                        break;
                }
            }
        }
        switch_scache++;
    }

    /*
     * if (recovered_ver >= SOC_CONTROL_WB_PART_0_VER_1_1) {
     *    // Recover state corresponding to WB version 1.1
     * }
 */

    return rv;
}

/*
 * Function:
 *      soc_switch_control_scache_sync
 * Description:
 *      Save golbal switch state to scache.
 * Parameters:
 *      unit -    (IN) BCM device number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_switch_control_scache_sync(int unit)
{
    int                  i, arg;
    int                  rv = SOC_E_NONE;
    uint8                sc_map = 0;
    uint16               default_ver;
    uint32               alloc_get;
    uint8                *switch_scache;
    soc_scache_handle_t  scache_handle;

    /* Limited scache mode unsupported */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return SOC_E_NONE;
    }

    default_ver = SOC_CONTROL_WB_PART_0_DEFAULT_VER;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_SWITCH_CONTROL,
                          SOC_SCACHE_SWITCH_CONTROL_PART0);

    /* Get the pointer for the Level 2 cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &switch_scache, &alloc_get);

    SOC_IF_ERROR_RETURN(rv);

    /* save default version */
    sal_memcpy(switch_scache, &default_ver, sizeof(uint16));
    switch_scache += SOC_WB_SCACHE_CONTROL_SIZE;

    /* Enable bits if the corresponding switch control is set */
    for (i = 0; (i < SOC_CONTROL_WB_NUM_ELE_0); i++) {
        if (-1 != soc_wb_boolean_controls_0[i]) {
            arg = 0;
            switch (i) {
                case socControlUseGport:
                    arg = SOC_USE_GPORT(unit);
                    break;
                case socControlL2PortBlocking:
                    arg = SOC_L2X_GROUP_ENABLE_GET(unit);
                    break;
                case socControlAbortOnError:
#ifdef BCM_CB_ABORT_ON_ERR
                    arg = SOC_CB_ABORT_ON_ERR(unit);
#endif /* BCM_CB_ABORT_ON_ERR */
                    break;
                default:
                    break;
            }
            if (arg) {
                sc_map |= (1 << i);
            }
        }
    }
    *switch_scache = sc_map;

    switch_scache++;

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      soc_switch_sync_mux_from_pbm
 * Description:
 *      Convert a port index to SyncE L1 recovered clock mux setting
 * Parameters:
 *      unit -    (IN)  BCM device number.
 *      port -    (IN)  Port index
 *      mux  -    (OUT) Mux setting corresponding to port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_switch_sync_mux_from_port(int unit,
                              int port,
                              int *mux)
{
    if (!SOC_PORT_VALID(unit, port) ||
        !SOC_PBMP_MEMBER(PBMP_PORT_ALL(unit), port)) {
        return SOC_E_PARAM;
    }

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        if (SOC_PORT_VALID(unit, port)) {
            int addr = soc_phy_addr_of_port(unit, port);
            /* See _hurricane3_phy_addr_default for values used below */
            if (addr >= 0x81 && addr <= 0x88) {
                *mux = (16 + (addr - 0x81));
                return SOC_E_NONE;
            }
            if (addr >= 0x89 && addr <= 0x90) {
                *mux = (32 + (addr - 0x89));
                return SOC_E_NONE;
            }
            if (addr == 0xa1) {
                int lane = INT_PHY_SW_STATE(unit, port)->lane_num;
                *mux = 4 + lane;
                return SOC_E_NONE;
            }
            if (addr == 0xa5) {
                int lane = INT_PHY_SW_STATE(unit, port)->lane_num;
                *mux = 8 + lane;
                return SOC_E_NONE;
            }
            if (addr == 0xa9) {
                int lane = INT_PHY_SW_STATE(unit, port)->lane_num;
                *mux = 24 + lane;
                return SOC_E_NONE;
            }
            if (addr == 0xad) {
                int lane = INT_PHY_SW_STATE(unit, port)->lane_num;
                *mux = 40 + lane;
                return SOC_E_NONE;
            }
        }
        return SOC_E_PARAM;
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    /* Default behavior: */
    *mux = port - 1;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_switch_sync_mux_from_pbm
 * Description:
 *      Convert a PBM value to SyncE L1 recovered clock mux setting
 * Parameters:
 *      unit -    (IN)  BCM device number.
 *      pbm  -    (IN)  Port BitMap with desired port
 *      mux  -    (OUT) Mux setting corresponding to PBM
 * Returns:
 *      SOC_E_XXX
 */
int
soc_switch_sync_mux_from_pbm(int unit,
                             pbmp_t pbm,
                             int *mux)
{
    int count, port;

    SOC_PBMP_COUNT(pbm, count);

    if (count != 1) {
        return SOC_E_PARAM;
    }

    SOC_PBMP_ITER(pbm, port) {
        SOC_IF_ERROR_RETURN(soc_switch_sync_mux_from_port(unit, port, mux));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_switch_sync_mux_from_pll
 * Description:
 *      Convert a port index to SyncE L1 recovered clock mux setting
 * Parameters:
 *      unit -    (IN)  BCM device number.
 *      pll  -    (IN)  PLL reference
 *      mux  -    (OUT) Mux setting corresponding to PLL
 * Returns:
 *      SOC_E_XXX
 */
int
soc_switch_sync_mux_from_pll(int unit,
                             soc_recov_clk_src_t src,
                             int *mux)
{
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_SERDES_PLL:
            *mux = 34;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_MASTER_PLL:
            *mux = 35;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_SERDES_PLL:
            *mux = 40;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_MASTER_PLL:
            *mux = 41;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_0:
            *mux = 41;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_SERDES_PLL:
            *mux = 28;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_MASTER_PLL:
            *mux = 29;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_0:
            *mux = 30;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_HELIX4_SUPPORT
    if (SOC_IS_HELIX4(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_XGPLL_0:
            *mux = 64;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_1:
            *mux = 65;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_XGPLL_0:
            *mux = 28;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_1:
            *mux = 29;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_0:
            *mux = 26;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_1:
            *mux = 27;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_XGPLL_0:
            *mux = 0;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_1:
            *mux = 1;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_0:
            *mux = 2;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_1:
            *mux = 3;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_XGPLL_0:
            *mux = 28;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_1:
            *mux = 29;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_0:
            *mux = 26;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_EXT_PHY_1:
            *mux = 27;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif

#if (defined(BCM_TOMAHAWK_SUPPORT) ||  \
     defined(BCM_APACHE_SUPPORT) ||    \
     defined(BCM_TRIDENT2X_SUPPORT))

    /* Note:  These chips have a separate mux to pick between port and
       a specific LCPLL.  We return the value for that mux here. */

    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_APACHE(unit) || SOC_IS_TRIDENT2X(unit)) {
        switch (src) {
        case SOC_RCVR_CLK_XGPLL_0:
            *mux = 1;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_1:
            *mux = 2;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_2:
            *mux = 3;
            return SOC_E_NONE;
        case SOC_RCVR_CLK_XGPLL_3:
            *mux = 4;
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
        }
    }
#endif /* TOMAHAWK / APACHE / TRIDENT2X */

    return SOC_E_UNAVAIL;
}
