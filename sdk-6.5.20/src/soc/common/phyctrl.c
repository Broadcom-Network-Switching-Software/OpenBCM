/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * StrataSwitch PHY control API
 * All access to PHY drivers should call the following soc functions.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phyreg.h>
#include <soc/phyctrl.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#include <soc/error.h>
#include <soc/counter.h>
#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif
#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
#include <soc/firelight.h>
#endif
#ifdef INCLUDE_MACSEC
#include <soc/macsecphy.h>
#endif
#ifdef INCLUDE_FCMAP 
#include <soc/fcmapphy.h>
#endif
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/port.h>
#endif

/* PHY address to port re-mapping */
int *phy_rmap[SOC_MAX_NUM_DEVICES];

/* Per port data structure to manage PHY drivers */
soc_phy_info_t *phy_port_info[SOC_MAX_NUM_DEVICES];

/* Per PHY data structure to manage PHY drivers */
phy_ctrl_t **int_phy_ctrl[SOC_MAX_NUM_DEVICES];
phy_ctrl_t **ext_phy_ctrl[SOC_MAX_NUM_DEVICES];

#define SOC_PHYCTRL_INIT_CHECK(_ext_pc, _int_pc) \
    if ((NULL == (_ext_pc)) && (NULL == (_int_pc))) { \
        return SOC_E_INIT; \
    } 

#define SOC_NULL_PARAM_CHECK(_param) \
    if (NULL == (_param)) { \
        return SOC_E_PARAM; \
    }

#define SOC_PORT_PARAM_CHECK(_port) \
    if ((_port) >= SOC_MAX_NUM_PORTS || (_port) <  0) { \
        return SOC_E_PARAM; \
    }

#ifdef PHYCTRL_DEBUG_PRINT
#define PHYCTRL_WARN(_stuff) LOG_WARN(BSL_LS_SOC_PHY, _stuff)
#define PHYCTRL_VERBOSE(_stuff) LOG_VERBOSE(BSL_LS_SOC_PHY, _stuff)
#else
#define PHYCTRL_WARN(_stuff) 
#define PHYCTRL_VERBOSE(_stuff) 
#endif 

#define MAC_WAN_MODE_THROTTLE_10G_TO_5G   256
#define MAC_WAN_MODE_THROTTLE_10G_TO_2P5G 257

STATIC void
soc_phyctrl_phymod_free(phy_ctrl_t *pc)
{
#ifdef PHYMOD_SUPPORT
    if (pc && pc->phymod_ctrl.cleanup) {
        pc->phymod_ctrl.cleanup(&pc->phymod_ctrl);
    }
#endif
}

STATIC void
soc_phyctrl_free(phy_ctrl_t *pc)
{
    /* Free PHYMOD resources */
    soc_phyctrl_phymod_free(pc);
#ifdef INCLUDE_FCMAP
    if (pc->fcmap_enable) {
        soc_fcmapphy_uninit(pc->unit, pc->port);
    }
#endif
    /* Free main structure */
    sal_free(pc);
}

int
soc_phyctrl_software_deinit(int unit)
{
    int port;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_software_deinit: unit %d\n"), unit));
    
    if (NULL != phy_port_info[unit]) {
        PBMP_PORT_ITER(unit, port) {
            if (NULL != phy_port_info[unit][port].chip_info) {
                sal_free(phy_port_info[unit][port].chip_info);
                phy_port_info[unit][port].chip_info = NULL;
            }
        }
        sal_free(phy_port_info[unit]);
        phy_port_info[unit] = NULL;
    }

    if (NULL != int_phy_ctrl[unit]) {
        PBMP_PORT_ITER(unit, port) {
            if (NULL != int_phy_ctrl[unit][port]) {
                soc_phyctrl_free(int_phy_ctrl[unit][port]);
                int_phy_ctrl[unit][port] = NULL;
            }
        }
        sal_free(int_phy_ctrl[unit]);
        int_phy_ctrl[unit] = NULL;
    }

    if (NULL != ext_phy_ctrl[unit]) {
        PBMP_PORT_ITER(unit, port) {
            if (NULL != ext_phy_ctrl[unit][port]) {
                soc_phyctrl_free(ext_phy_ctrl[unit][port]);
                ext_phy_ctrl[unit][port] = NULL;
            }
        }
        sal_free(ext_phy_ctrl[unit]);
        ext_phy_ctrl[unit] = NULL;
    }

    if (NULL != phy_rmap[unit]) {
        sal_free(phy_rmap[unit]);
        phy_rmap[unit] = NULL;
    }

    SOC_IF_ERROR_RETURN (soc_phy_deinit(unit));

    return (SOC_E_NONE);
}

/* Validate phy address, TRUE is valid, FALSE is bad */
STATIC uint8
soc_phy_addr_valid(int unit, int port, int phy_addr)
{
    uint8 valid = TRUE;
    int bus_num;

    if (SOC_IS_TRIDENT2(unit)) {
        bus_num = PHY_ID_BUS_NUM(phy_addr);

        /* Only 0 to 5 MDIO rings are supported for Ethernet on TD2 */
        if ((bus_num < 0) || (bus_num >= 6)) {
            valid = FALSE;
        }
    }

    return valid;
}

int
soc_phyctrl_software_init(int unit)
{
    int port;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_software_init: unit %d\n"), unit));
    
        if ((phy_port_info[unit] != NULL) ||
            (int_phy_ctrl[unit] != NULL) ||
            (ext_phy_ctrl[unit] != NULL) ||
            (phy_rmap[unit] != NULL)) {
            soc_phyctrl_software_deinit(unit);
        }

        phy_port_info[unit] = sal_alloc(sizeof(soc_phy_info_t) * 
                                        SOC_MAX_NUM_PORTS,
                                        "phy_port_info");
        if (phy_port_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(phy_port_info[unit], 0, 
                   sizeof(soc_phy_info_t) * SOC_MAX_NUM_PORTS);

        int_phy_ctrl[unit] = sal_alloc(sizeof(phy_ctrl_t *) *
                                       SOC_MAX_NUM_PORTS,
                                       "int_phy_ctrl");
        if (int_phy_ctrl[unit] == NULL) {
            soc_phyctrl_software_deinit(unit);
            return SOC_E_MEMORY;
        }
        sal_memset(int_phy_ctrl[unit], 0, 
                   sizeof(phy_ctrl_t *) * SOC_MAX_NUM_PORTS);


        ext_phy_ctrl[unit] = sal_alloc(sizeof(phy_ctrl_t *) *
                                       SOC_MAX_NUM_PORTS,
                                       "ext_phy_ctrl");
        if (ext_phy_ctrl[unit] == NULL) {
            soc_phyctrl_software_deinit(unit);
            return SOC_E_MEMORY;
        }
        sal_memset(ext_phy_ctrl[unit], 0, 
                   sizeof(phy_ctrl_t *) * SOC_MAX_NUM_PORTS);

        phy_rmap[unit] = sal_alloc(sizeof(int) * EXT_PHY_ADDR_MAX,
                                   "phy_rmap");
        if (phy_rmap[unit] == NULL) {
            soc_phyctrl_software_deinit(unit);
            return SOC_E_MEMORY;
        }
        sal_memset(phy_rmap[unit], -1, sizeof(int) * EXT_PHY_ADDR_MAX);

    /* Initialize PHY driver table and assign default PHY address */
    SOC_IF_ERROR_RETURN
        (soc_phy_init(unit));

    PBMP_PORT_ITER(unit, port) {
        if (PHY_ADDR(unit, port) >= EXT_PHY_ADDR_MAX ||
            PHY_ADDR_INT(unit, port) >= EXT_PHY_ADDR_MAX ||
            !soc_phy_addr_valid(unit, port, PHY_ADDR(unit, port))) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "soc_phyctrl_software_init: intPhyAddr 0x%x "
                                  "or extPhyAddr 0x%x exceeds max size u=%d p=%d FAILED "), 
                       PHY_ADDR_INT(unit, port),PHY_ADDR(unit, port),unit, port));
            return SOC_E_PARAM;
        }
        PHY_ADDR_TO_PORT(unit, PHY_ADDR(unit, port)) = port;
        PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
    }

    PHYCTRL_VERBOSE(("soc_phyctrl_software_init Unit %d\n",
                     unit));

    return SOC_E_NONE;
}

int
soc_phyctrl_software_port_init(int unit, soc_port_t port)
{

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_software_port_init: "
                            "unit %d, port %d\n"), unit, port));
    
    /* Assign default PHY address */
    SOC_IF_ERROR_RETURN
        (soc_phy_port_init(unit, port));

    if (PHY_ADDR(unit, port) >= EXT_PHY_ADDR_MAX ||
        PHY_ADDR_INT(unit, port) >= EXT_PHY_ADDR_MAX) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "soc_phyctrl_software_init: intPhyAddr 0x%x "
                              "or extPhyAddr 0x%x exceeds max size u=%d p=%d FAILED "), 
                   PHY_ADDR_INT(unit, port),PHY_ADDR(unit, port),unit, port));
        return SOC_E_PARAM;
    }
    PHY_ADDR_TO_PORT(unit, PHY_ADDR(unit, port)) = port;
    PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
    

    PHYCTRL_VERBOSE(("soc_phyctrl_software_port_init Unit %d Port %d\n",
                     unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_drv_name_get
 * Purpose:
 *      Get PHY driver name.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      name - Buffer for PHY driver name.
 *      len  - Length of buffer.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_drv_name_get(int unit, soc_port_t port, char *name, int len)
{
    static char       unknown_driver[] = "unknown driver";
    phy_driver_t      *pd;
    int               string_len;
   
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_drv_name_get: "
                            "unit %d, port %d, name %s, len %d\n"), unit, port, name, len));
    
    pd = NULL;
    if (NULL != EXT_PHY_SW_STATE(unit, port)) {
        pd = EXT_PHY_SW_STATE(unit, port)->pd;
    } else if (NULL != INT_PHY_SW_STATE(unit, port)) {
        pd = INT_PHY_SW_STATE(unit, port)->pd;
    }
    
    if (NULL == pd) {
        string_len = (int)sizeof(unknown_driver);
        if (string_len <= len) {
            sal_strncpy(name, unknown_driver, len);
        }
        return SOC_E_NOT_FOUND;
    }

    string_len = (int)sal_strlen(pd->drv_name);
    if (string_len > len) {
        return SOC_E_MEMORY;
    }

    sal_strncpy(name, pd->drv_name, len);

    return SOC_E_NONE;
}

STATIC int
soc_phy_sbus_read(int unit, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 *phy_rd_data)
{
    soc_sbus_mdio_read_f sbus_read;
    uint32 data32;

    sbus_read = SOC_FUNCTIONS(unit)->soc_sbus_mdio_read;
    if (sbus_read == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        sbus_read(unit, phy_id, phy_reg_addr, &data32));

    *phy_rd_data = data32;

    return SOC_E_NONE;
}

STATIC int
soc_phy_sbus_wrmask(int unit, uint32 phy_id,
                    uint32 phy_reg_addr, uint16 phy_wr_data, uint16 wr_mask)
{
    soc_sbus_mdio_write_f sbus_write;
    uint32 wr_data;

    sbus_write = SOC_FUNCTIONS(unit)->soc_sbus_mdio_write;
    if (sbus_write == NULL) {
        return SOC_E_INTERNAL;
    }

    wr_data = wr_mask;
    wr_data <<= 16;
    wr_data |= phy_wr_data;

    return sbus_write(unit, phy_id, phy_reg_addr, wr_data);
}

STATIC int
soc_phy_sbus_write(int unit, uint32 phy_id,
                   uint32 phy_reg_addr, uint16 phy_wr_data)
{
    return soc_phy_sbus_wrmask(unit, phy_id, phy_reg_addr, phy_wr_data, 0);
}

/*
 * Function:
 *      soc_phyctrl_probe
 * Purpose:
 *      Probe for internal and external PHYs attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_probe(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *ext_pd;
    phy_driver_t *int_pd;
    phy_ctrl_t   ext_pc;
    phy_ctrl_t   int_pc;
    phy_ctrl_t   *pc;
    rv = SOC_E_NONE;

    sal_memset(&ext_pc, 0, sizeof(phy_ctrl_t));
    sal_memset(&int_pc, 0, sizeof(phy_ctrl_t));

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_probe: "
                            "unit %d, port %d\n"), unit, port));
    
    /* Free previously allocated PHYMOD resources, otherwise the probe
     * function may fail because the phy and core IDs we want to
     * assign already exist. This situation can happen if the BCM
     * layer is initialized multiple times without a full system
     * initialization.
     */
    soc_phyctrl_phymod_free(INT_PHY_SW_STATE(unit, port));
    soc_phyctrl_phymod_free(EXT_PHY_SW_STATE(unit, port));
    
    /* The soc layer probe function always uses the default PHY addresses
     * instead of PHY address stored in phy_info from previous probe.
     * This make sure that the external PHY probe works correctly even
     * when the device is hot plugged.  
     */
    int_pc.unit       = unit;
    int_pc.port       = port;
    int_pc.speed_max  = SOC_INFO(unit).port_init_speed[port] ?
                        SOC_INFO(unit).port_init_speed[port] :
                        SOC_INFO(unit).port_speed_max[port];
    ext_pc.unit       = unit;
    ext_pc.port       = port;
    ext_pc.speed_max  = SOC_INFO(unit).port_init_speed[port] ?
                        SOC_INFO(unit).port_init_speed[port] :
                        SOC_INFO(unit).port_speed_max[port];

    if (soc_feature(unit, soc_feature_port_encap_speed_max_config)){
        /*
         * Using the soc_feature_port_encap_speed_max_config would imply
         * port_init_speed as the max supported speed of ethernet port and
         * port_speed_max as the max supported speed of Higig port.
         */
        if (IS_HG_PORT(unit, port)) {
            /* for Higig port, giving the port_speed_max */
            int_pc.speed_max = SOC_INFO(unit).port_speed_max[port];
            ext_pc.speed_max = SOC_INFO(unit).port_speed_max[port];
        }
    }
#ifdef BCM_DFE_SUPPORT
     if (SOC_IS_DFE(unit)) {
        int phy_clause = 22;

        /* Clause 45 instead of Clause 22 MDIO access */
        phy_clause = soc_property_port_get(unit, port, spn_PORT_PHY_CLAUSE, phy_clause);

        int_pc.read  = soc_dcmn_miim_cl22_read;
        int_pc.write = soc_dcmn_miim_cl22_write;

        /* ARAD version of register access */
        if (45 == phy_clause) {
            ext_pc.read  = soc_dcmn_miim_cl45_read;
            ext_pc.write = soc_dcmn_miim_cl45_write;
            ext_pc.wb_write = NULL;
        } else {      
            ext_pc.read  = soc_dcmn_miim_cl22_read;
            ext_pc.write = soc_dcmn_miim_cl22_write;
        }

        if (SOC_IS_FE1600_B0_AND_ABOVE(unit) && SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SUPPORT_DUAL_RATE);
        }
    } else 
#endif /* BCM_DFE_SUPPORT */



#ifdef BCM_PETRA_SUPPORT
     if (SOC_IS_ARAD(unit)) {

        int phy_clause = 22;

         /* Clause 45 instead of Clause 22 MDIO access */
        phy_clause = soc_property_port_get(unit, port, spn_PORT_PHY_CLAUSE, phy_clause);

        if (!SOC_IS_ARDON(unit)) {
            int_pc.read  = soc_dcmn_miim_cl22_read; 
            int_pc.write = soc_dcmn_miim_cl22_write;
        }

        /* ARAD version of register access */
        if (45 == phy_clause) {
            ext_pc.read  = soc_dcmn_miim_cl45_read;
            ext_pc.write = soc_dcmn_miim_cl45_write;
            ext_pc.wb_write = NULL;
            if (SOC_IS_ARDON(unit)) {
                int_pc.read  = soc_dcmn_miim_cl45_read;
                int_pc.write = soc_dcmn_miim_cl45_write;
            }
        } else {
            ext_pc.read  = soc_dcmn_miim_cl22_read;
            ext_pc.write = soc_dcmn_miim_cl22_write;
            ext_pc.wb_write = NULL;
            if (SOC_IS_ARDON(unit)) {
                int_pc.read  = soc_dcmn_miim_cl22_read;
                int_pc.write = soc_dcmn_miim_cl22_write;
            }
        }
    } else 
#endif /* BCM_PETRA_SUPPORT */
    {
#ifdef BCM_ESW_SUPPORT
        int          phy_clause = 22;
        int_pc.read  = soc_esw_miim_read;
        int_pc.write = soc_esw_miim_write; 
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port) || IS_CE_PORT(unit,port)) {
            phy_clause = 45;
        }
        /* Clause 45 instead of Clause 22 MDIO access */
        phy_clause = soc_property_port_get(unit, port, spn_PORT_PHY_CLAUSE, 
                               phy_clause);

        if (phy_clause == 45) {
            ext_pc.read  = soc_esw_miimc45_read;
            ext_pc.write = soc_esw_miimc45_write;
            ext_pc.data_write = soc_esw_miimc45_data_write;
            ext_pc.addr_write = soc_esw_miimc45_addr_write;
            ext_pc.wb_write = soc_esw_miimc45_wb_write;
        } else {
            ext_pc.read  = soc_esw_miim_read;
            ext_pc.write = soc_esw_miim_write; 
        }
#ifdef INCLUDE_I2C
        if (soc_property_port_get(unit, port,
                             spn_PHY_BUS_I2C, 0)) {
            ext_pc.read  = phy_i2c_miireg_read;
            ext_pc.write = phy_i2c_miireg_write;
        }
#endif
#endif /* BCM_ESW_SUPPORT */
    }

    if (SOC_FUNCTIONS(unit) && SOC_FUNCTIONS(unit)->soc_sbus_mdio_write &&
        PHY_ADDR_INT(unit, port)) {
        if (SOC_IS_SABER2(unit)) {
            if (IS_XL_PORT(unit, port)) {
                int_pc.read  = soc_phy_sbus_read;
                int_pc.write = soc_phy_sbus_write; 
                int_pc.wrmask = soc_phy_sbus_wrmask; 
            }
#if defined(BCM_GREYHOUND2_SUPPORT)
        } else if (SOC_IS_GREYHOUND2(unit)) {
            int pport = 0, blk = 0;

            pport = SOC_INFO(unit).port_l2p_mapping[port];
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                if (soc_firelight_sbus_tsc_block(unit, pport, &blk) !=
                        SOC_E_PORT) {
                    /* no SOC_E_PORT return means SBus-MDIO can be served */
                    int_pc.read  = soc_phy_sbus_read;
                    int_pc.write = soc_phy_sbus_write;
                    int_pc.wrmask = soc_phy_sbus_wrmask;
                }
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                if (soc_greyhound2_sbus_tsc_block(unit, pport, &blk) !=
                        SOC_E_PORT) {
                    /* no SOC_E_PORT return means SBus-MDIO can be served */
                    int_pc.read  = soc_phy_sbus_read;
                    int_pc.write = soc_phy_sbus_write;
                    int_pc.wrmask = soc_phy_sbus_wrmask;
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
        } else {
        int_pc.read  = soc_phy_sbus_read;
        int_pc.write = soc_phy_sbus_write; 
        int_pc.wrmask = soc_phy_sbus_wrmask; 
    }
    }

    /* Check for software simulators */
    soc_phy_check_sim(unit, port, &int_pc);

    SOC_IF_ERROR_RETURN
        (soc_phy_probe(unit, port, &ext_pc, &int_pc));

    ext_pd = ext_pc.pd;
    int_pd = int_pc.pd;
    if (ext_pd == int_pd) {
        /* If external PHY driver and internal PHY driver are the same,
         * config setting must have override the PHY driver selection.
         * In this case just attach internal PHY driver. 
         * Internal driver is needed for all devices with internal
         * SerDes because MAC driver performs notify calls to internal
         * PHY driver. 
         */
        ext_pd = NULL;
    }

    if (NULL != ext_pd) {
        /* If there is allocated memory for
         * external PHY driver, free it then re-alloc.
         */ 
        if (NULL != EXT_PHY_SW_STATE(unit, port)) {
            soc_phyctrl_free(EXT_PHY_SW_STATE(unit, port));
            EXT_PHY_SW_STATE(unit, port) = NULL;
        }
        if (NULL == EXT_PHY_SW_STATE(unit, port)) {
            EXT_PHY_SW_STATE(unit, port) =
                sal_alloc (sizeof (phy_ctrl_t) + ext_pc.size, ext_pd->drv_name);
            if (NULL == EXT_PHY_SW_STATE(unit, port)) {
                rv = SOC_E_MEMORY;
            }
        }
        if (SOC_SUCCESS(rv)) {
            pc = EXT_PHY_SW_STATE(unit, port);
            sal_memcpy(pc, &ext_pc, sizeof(phy_ctrl_t));
            sal_memset(pc+1, 0, pc->size);
            rv = soc_phy_reset_register(unit, port, pc->pd->pd_reset, 
                                        NULL, TRUE);
            PHY_ADDR_TO_PORT(unit, PHY_ADDR(unit, port)) = port;

            /*
             * Make sure resolved port MDIO address is updated in CMIC external
             * PHY address for hardware linkscan
             */
            soc_linkscan_pause(unit);

            rv = soc_linkscan_hw_port_update(unit, port);

            soc_linkscan_continue(unit);

            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "soc_phyctrl_probe external: u=%d p=%d %s\n"),
                      unit, port, ext_pd->drv_name));
        } 
    } else {
        /* No external PHY detected. If there is allocated memory for
         * external PHY driver, free it.
         */ 
        if (NULL != EXT_PHY_SW_STATE(unit, port)) {
            soc_phyctrl_free(EXT_PHY_SW_STATE(unit, port));
            EXT_PHY_SW_STATE(unit, port) = NULL;
        }
    }

    if (SOC_SUCCESS(rv) && NULL != int_pd) {
        if (NULL == INT_PHY_SW_STATE(unit, port)) {
            INT_PHY_SW_STATE(unit, port) =
                sal_alloc (sizeof (phy_ctrl_t) + int_pc.size, int_pd->drv_name);
            if (NULL == INT_PHY_SW_STATE(unit, port)) {
                rv = SOC_E_MEMORY;
            }
        } else {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data && ppc->size == 0) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
            if (int_pc.size != ppc->size) {
                soc_phyctrl_free(ppc);
                INT_PHY_SW_STATE(unit, port) = 
                    sal_alloc (sizeof (phy_ctrl_t) + int_pc.size, int_pd->drv_name);
                if (NULL == INT_PHY_SW_STATE(unit, port)) {
                    rv = SOC_E_MEMORY;
                } 
            }
        }
        if (SOC_SUCCESS(rv)) {
            pc = INT_PHY_SW_STATE(unit, port);
            sal_memcpy(pc, &int_pc, sizeof(phy_ctrl_t));
            sal_memset(pc+1, 0, pc->size);
            PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
            if (NULL == ext_pd) {
                /* If there is no external PHY, the internal PHY 
                 * must be in fiber mode. 
                 */ 
                if (soc_property_port_get(unit, port,
                                              spn_SERDES_FIBER_PREF, 1)) {
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
                } else {
                    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
                }
                rv = soc_phy_reset_register(unit, port, pc->pd->pd_reset, 
                                            NULL, TRUE);
            }

            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "soc_phyctrl_probe internal: u=%d p=%d %s\n"),
                      unit, port, int_pd->drv_name));
        }
    } else {
        /* No internal PHY detected. If there is allocated memory for
         * internal PHY driver, free it.
         */ 
        if (NULL != INT_PHY_SW_STATE(unit, port)) {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data && ppc->size == 0) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
            soc_phyctrl_free(ppc);
            INT_PHY_SW_STATE(unit, port) = NULL;
        }
    }



    if (SOC_SUCCESS(rv)) {
        /* Set SOC related restriction/configuration/flags first */ 
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        if (soc_property_port_get(unit, port, spn_PHY_SGMII_AUTONEG, FALSE) &&
            soc_feature(unit, soc_feature_sgmii_autoneg)) {

            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        }
    } else {
        if (NULL != EXT_PHY_SW_STATE(unit, port)) {
            soc_phyctrl_free(EXT_PHY_SW_STATE(unit, port));
            EXT_PHY_SW_STATE(unit, port) = NULL;
        }
 
        if (NULL != INT_PHY_SW_STATE(unit, port)) {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data && ppc->size == 0) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
            soc_phyctrl_free(ppc);
            INT_PHY_SW_STATE(unit, port) = NULL;
        }
    }

    return rv;
}

soc_error_t
soc_phyctrl_set_speed_max(int unit, soc_port_t port, int speed_max)
{
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;

    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != int_pc) {
        int_pc->speed_max = speed_max;
    }
    if (NULL != ext_pc) {
        ext_pc->speed_max = speed_max;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_init
 * Purpose:
 *      Initialize the PHY drivers attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_init(int unit, soc_port_t port)
{
    int         rv;
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;
#ifdef BCM_SWITCHMACSEC_SUPPORT
    uint32      switchmacsec_attached;
#endif /* BCM_SWITCHMACSEC_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    uint32      rval=0;
#endif

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_init: "
                            "unit %d, port %d\n"), unit, port));
    
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    {
#ifdef BCM_ESW_SUPPORT
        if (SOC_IS_BRADLEY(unit)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        }
        if (IS_LMD_ENABLED_PORT(unit, port)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SINGLE_LANE);
        }
#endif /* BCM_ESW_SUPPORT */
    }

    rv = SOC_E_NONE;
    /* arbiter lock */
    INT_MCU_LOCK(unit);
    if (NULL != int_pc) {
        rv = (PHY_INIT(int_pc->pd, unit, port));
    }

    if (NULL != ext_pc) {
        rv = (PHY_INIT(ext_pc->pd, unit, port));
        if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "soc_phyctrl_probe: Init failed for"
                                  " u=%d p=%d FAILED "), unit, port));
            return SOC_E_FAIL;
        }
    } 
    INT_MCU_UNLOCK(unit);

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_INIT_DONE);

    PHYCTRL_VERBOSE(("soc_phyctrl_init: u=%d p=%d %s rv=%d\n",
                     unit, port, (ext_pc) ? "EXT" : "INT", rv));

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && !(SOC_IS_SABER2(unit))) {
       /* To recover from clock glitch(not always), Serdes init sequence 
          requires write to reset frequency downsizer logic in MXQPORT. */
       SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
       soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, SW_RESET_DOWNSIZERf , 1);
       SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
       soc_reg_field_set(unit, XPORT_MODE_REGr, &rval, SW_RESET_DOWNSIZERf , 0);
       SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));

       if ((port == KT2_OLP_PORT) &&  (SOC_INFO(unit).olp_port[0])) {
            SOC_IF_ERROR_RETURN(soc_phyctrl_auto_negotiate_set(unit, port, 0));
            SOC_IF_ERROR_RETURN(soc_phyctrl_speed_set(unit, port, 2500));
       }
   }
#endif

#ifdef BCM_SWITCHMACSEC_SUPPORT
    /* Switch_MACSEC init section.
     *
     * For the MACSEC-in-SWITCH solution, the attached PHY is expected as 
     *  the PHY device without MACSEC feature. 
     *
     * In the MACSEC-in-PHY solution, the MACSEC device is designed to be 
     *  init as well through PHY driver init process.
     *
     * For MACSEC-in-Switch solution, the simular init procedure is provided 
     *  through SOC driver, soc_switchmacsec_init(), which is exectued after 
     *  nomal PHY driver for thsi port is attached and init.
     */
    SOC_IF_ERROR_RETURN(soc_macsecphy_dev_info_get(unit, port, 
            SOC_MACSECPHY_DEV_INFO_SWITCHMACSEC_ATTACHED, 
            &switchmacsec_attached));
            
    if (switchmacsec_attached) {
        rv = soc_switchmacsec_init(unit, port);
        if (!SOC_SUCCESS(rv)) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "Switch-MACSEC Init failed!u=%d p=%d rv=%d\n"), 
                       unit, port, rv));
        }
    }
#endif /* BCM_SWITCHMACSEC_SUPPORT */

    return rv;
}

STATIC int
soc_phyctrl_pd_get(int unit, soc_port_t port, phy_driver_t **pd)
{
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        *pd = ext_pc->pd;
    } else {
        *pd = int_pc->pd;
    }

    return SOC_E_NONE;
}

STATIC int
soc_phyctrl_passthru_pd_get(int unit, soc_port_t port, phy_driver_t **pd)
{
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (PHY_PASSTHRU_MODE(unit, port)) {
        if (NULL != int_pc) {
            *pd = int_pc->pd;
        } else {
            *pd = ext_pc->pd;
        }
    } else {
        if (NULL != ext_pc) {
            *pd = ext_pc->pd;
        } else {
            *pd = int_pc->pd;
        }
    }
 
    return SOC_E_NONE;
}

int
soc_phyctrl_passthru_pc_get(int unit, soc_port_t port, phy_ctrl_t **pc)
{
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (PHY_PASSTHRU_MODE(unit, port)) {
        if (NULL != int_pc) {
            *pc = int_pc;
        } else {
            *pc = ext_pc;
        }
    } else {
        if (NULL != ext_pc) {
            *pc = ext_pc;
        } else {
            *pc = int_pc;
        }
    }
 
    return SOC_E_NONE;
}

int
soc_phyctrl_phyn_pc_get(int unit, soc_port_t port, int phyn, phy_ctrl_t **pc,
                        phy_ctrl_t **pc_ptr)
{
    int phy_index = 0;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    /* Check if both int pc & ext pc are not zero */
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    /* Start with int pc if != zero, otherwise start with ext pc */
    *pc_ptr = *pc = (int_pc) ? int_pc : ext_pc;
  
   /* PHYN out of limit */ 
   if (phyn > SOC_PHYCTRL_PHYN_MAX_LIMIT) {
      return SOC_E_PARAM;
   } 
    
    /* Default */
    if (0 == phyn) {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_passthru_pc_get(unit, port, pc));
        *pc_ptr = *pc;
        return SOC_E_NONE;
    }

    for (phy_index = 1; phy_index <= phyn; ++phy_index) {

        if (phy_index == phyn) {
            /* Found */
            if ((*pc_ptr != int_pc) && (*pc_ptr != ext_pc) && (*pc_ptr != NULL)) {
                (*pc_ptr)->flags |= PHYCTRL_CHAINED_PHY_CTRL;
            }
            return SOC_E_NONE;
        }

        /* Go through internal phy chain */
        if (*pc == int_pc) {
            if (PHY_IS_CHAINED(unit, port)) {
                *pc_ptr = (*pc_ptr)->driver_data;                    
            
                if (!(*pc_ptr)) {
                    if (ext_pc) {
                        /* End of int_phy phy chain. Move to ext phy */
                        *pc_ptr = *pc = ext_pc;
                        continue;
                    } else {
                        /* End of phy chain. 
                           Return default value i.e int phy in this case */
                        *pc_ptr = *pc;
                        return SOC_E_NONE;
                    }
                }
            } else {
                /* Not a chained internal phy, move onto external phy */
                *pc_ptr = *pc = ext_pc? ext_pc : int_pc;
                continue;
            }
        } 

        /* Go through external phy chain */
        if ((*pc == ext_pc) && (PHY_IS_CHAINED(unit, port))) {
            *pc_ptr = (*pc_ptr)->driver_data;                    
            
            if (!(*pc_ptr)) {
                /* End of ext_phy phy chain. 
                   Return default value i.e ext phy in this case */
                *pc_ptr = *pc;
                return SOC_E_NONE;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_reset
 * Purpose:
 *      Reset the PHY drivers attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reset(int unit, soc_port_t port, void *user_arg)
{
    int           rv;
    phy_driver_t *pd=NULL;
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_reset: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_RESET(pd, unit, port));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_reset: u=%d p=%d rv=%d\n",
                      unit, port, rv));
    } 
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_drv_name
 * Purpose:
 *      Get pointer to driver name.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      name - driver name.
 * Returns:
 *      SOC_E_XXX
 */
char *
soc_phyctrl_drv_name(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_drv_name: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        return pd->drv_name;
    }
    return NULL;
}


/*
 * Function:
 *      soc_phyctrl_link_get
 * Purpose:
 *      Read link status of the PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      link - Link status
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_link_get(int unit, soc_port_t port, int *link)
{
    int           rv;
    phy_driver_t *pd=NULL;
    phy_ctrl_t *int_pc;
    
    int_pc = INT_PHY_SW_STATE(unit, port);

    SOC_NULL_PARAM_CHECK(link);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_link_get: "
                            "unit %d, port %d\n"), unit, port));
    
    *link = FALSE;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);

    /* Handles cases where internal phy's link_get
       needs to be called for linkscan(link_get) based
       tasks that need processing context.
       NOTE- Not being used for link status.  
     */
    if (SOC_SUCCESS(rv) 
        && (int_pc != NULL)) {
        if ((int_pc->pd != pd) 
            && PHY_FLAGS_TST(unit, port, PHY_FLAGS_SERVICE_INT_PHY_LINK_GET)
            && !PHY_FLAGS_TST(unit, port, PHY_FLAGS_REPEATER)
            && !PHY_FLAGS_TST(unit, port, PHY_FLAGS_PASSTHRU)) {
            /* Passthru/Repeater devices such as 84328 call 
               internal phy's link_get on their own.
            */
            rv = (PHY_LINK_GET(int_pc->pd, unit, port, link));
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINK_GET(pd, unit, port, link));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_link_get failed %d\n", rv));
    }

    return rv;
}


/*
 * Function:
 *      soc_phyctrl_linkfault_get
 * Purpose:
 *      Read fault status of the PHY driver.
 *      This is used for external PHYs where the fault status is not propagated to the switch
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      fault - fault status
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_linkfault_get(int unit, soc_port_t port, int *fault)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(fault);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_linkfault_get: "
                            "unit %d, port %d\n"), unit, port));
    
    *fault = FALSE;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINKFAULT_GET(pd, unit, port, fault));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_linkfault_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_enable_set
 * Purpose:
 *      Enable/Disable the PHY driver attached to the port. 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - enable/disable PHY driver
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_enable_set(int unit, soc_port_t port, int enable)
{
    int           rv1 = SOC_E_NONE, rv2 = SOC_E_NONE;
    phy_ctrl_t    *int_pc;
    phy_ctrl_t    *ext_pc;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_enable_set: "
                            "unit %d, port %d, enable %d\n"), unit, port, enable));
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    INT_MCU_LOCK(unit);
    if (ext_pc != NULL) {
        if (enable && (int_pc != NULL)) { /* for plugin ext. PHYs */
            pd = int_pc->pd;
            rv2 = (PHY_ENABLE_SET(pd, unit, port, enable));
        }
        pd = ext_pc->pd;
        rv1 = (PHY_ENABLE_SET(pd, unit, port, enable));
    } else {
        pd = int_pc->pd;
        rv2 = (PHY_ENABLE_SET(pd, unit, port, enable));
    }

    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv1) || SOC_FAILURE(rv2)) {
        PHYCTRL_WARN(("soc_phyctrl_enable_set: u=%d p=%d e=%d rv1=%d rv2=%d\n",
                      unit, port, enable, rv1, rv2));
        return rv1 ? rv1 : rv2;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_enable_get
 * Purpose:
 *      Get the enable/disable state of the PHY driver. 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - Current enable/disable state.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_driver_t *pd = NULL;
    int           rv;
    
    SOC_NULL_PARAM_CHECK(enable);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_enable_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ENABLE_GET(pd, unit, port, enable));
    }
    INT_MCU_UNLOCK(unit);
    
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_enable_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_duplex_set
 * Purpose:
 *      Set duplex of the PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      duplex -  (1) Full duplex
 *                (0) Half duplex
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_duplex_set(int unit, soc_port_t port, int duplex)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_duplex_set: "
                            "unit %d, port %d, duplex %d\n"), unit, port, duplex));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_DUPLEX_SET(pd, unit, port, duplex));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_duplex_set: u=%d p=%d d=%d rv=%d\n",
                      unit, port, duplex, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_duplex_get
 * Purpose:
 *      Get current duplex setting of the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      duplex -  (1) Full duplex
 *                (0) Half duplex
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_duplex_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_DUPLEX_GET(pd, unit, port, duplex));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_duplex_get:  u=%d p=%d rv=%d\n",
                      unit, port, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_speed_set
 * Purpose:
 *      Set PHY speed.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      speed - new speed of the PHY
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_speed_set(int unit, soc_port_t port, int speed)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_speed_set: "
                            "unit %d, port %d, speed %d\n"), unit, port, speed));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_SPEED_SET(pd, unit, port, speed));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_speed_set: u=%d p=%d s=%d rv=%d\n",
                      unit, port, speed, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_speed_get
 * Purpose:
 *      Read current PHY speed
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      speed - Current speed of PHY.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_speed_get(int unit, soc_port_t port, int *speed)
{
    int           rv;
    phy_driver_t *pd = NULL;
    
    SOC_NULL_PARAM_CHECK(speed);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_speed_get: "
                            "unit %d, port %d\n"), unit, port));
    
    *speed = 0;

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_SPEED_GET(pd, unit, port, speed));
    }
    INT_MCU_UNLOCK(unit);
    
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_speed_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_master_set
 * Purpose:
 *      Ser Master/Slave configuration of the PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      master - (1) master mode.
 *               (0) slave mode. 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_master_set(int unit, soc_port_t port, int master)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_master_set: "
                            "unit %d, port %d, master %d\n"), unit, port, master));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MASTER_SET(pd, unit, port, master));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_master_set: u=%d p=%d m=%d rv=%d\n",
                      unit, port, master, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_master_get
 * Purpose:
 *      Read current Master/Slave setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      master - (1) master mode.
 *               (0) slave mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_master_get(int unit, soc_port_t port, int *master)
{
    int           rv;
    phy_driver_t *pd = NULL;
    
    SOC_NULL_PARAM_CHECK(master);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_master_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MASTER_GET(pd, unit, port, master));
    }
    INT_MCU_UNLOCK(unit);
    
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_master_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_auto_negotiate_set
 * Purpose:
 *      Enable/Disable autonegotiation of the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      an   - new autoneg setting 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_auto_negotiate_set(int unit, soc_port_t port, int an)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_auto_negotiate_set: "
                            "unit %d, port %d, an %d\n"), unit, port, an));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_SET(pd, unit, port, an));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_auto_negotiate_set: u=%d p=%d an=%d rv=%d\n", 
                      unit, port, an, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_redirect_auto_negotiate_set
 * Purpose:
 *      Enable/Disable autonegotiation for PHY specific port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied 
 *             on system side(1)or line side(0).
 *      an   - new autoneg setting 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_auto_negotiate_set(int unit, soc_port_t port, int phyn,
                                        int phy_lane, int sys_side, int an)
{
    int           rv;
    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get (unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_auto_negotiate_set failed: "
                      "u=%d p=%d an=%d rv=%d\n",
                      unit, port, an, rv));
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL:0);    
 
    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_SET(pc_head->pd, unit, port, an));
    }
    INT_MCU_UNLOCK(unit);
  
    /* Clear flags set earlier for PHY_CONTROL_SET(). Clear them anyway..*/
    pc_ptr->flags &= ~PHYCTRL_CHAINED_PHY_CTRL;
    pc_ptr->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_auto_negotiate_set: "
                      "u=%d p=%d an=%d rv=%d\n", 
                      unit, port, an, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_auto_negotiate_get
 * Purpose:
 *      Get current auto neg setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      an   - current autoneg setting
 *      an_done - autoneg completed 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_auto_negotiate_get(int unit, soc_port_t port, int *an, int *an_done)
{
    int           rv;
    phy_driver_t *pd = NULL;

    SOC_NULL_PARAM_CHECK(an);
    SOC_NULL_PARAM_CHECK(an_done);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_auto_negotiate_get: "
                            "unit %d, port %d\n"), 
                 unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);
    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_GET(pd, unit, port, an, an_done));
    }
    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_auto_negotiate_get failed %d\n", 
                      rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_redirect_auto_negotiate_get
 * Purpose:
 *      Get current autonegotiation settings for PHY specific port
 * Parameters:
 *      unit     - SOC Unit #.
 *      port     - Port number.
 *      phyn     - Phy number
 *      phy_lane - Lane number
 *      sys      - Control set to be applied 
 *                 on system side(1)or line side(0).
 *      an       - current autoneg setting 
 *      an_done  - autoneg completed
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_auto_negotiate_get(int unit, soc_port_t port, int phyn,
                                        int phy_lane, int sys_side, int *an,
                                        int *an_done)
{
    int           rv;
    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get(unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_auto_negotiate_get failed %d\n", 
                      rv));
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL:0);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_GET(pc_head->pd, unit, port, an, an_done));
    }
    INT_MCU_UNLOCK(unit);
    
    /* Clear flags set earlier for PHY_CONTROL_SET(). Clear them anyway..*/
    pc_ptr->flags &= ~PHYCTRL_CHAINED_PHY_CTRL;
    pc_ptr->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_auto_negotiate_get failed %d\n", 
                      rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_local_set
 * Purpose:
 *      Configure local advertising setting.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Advertising mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    int                   rv;
    phy_driver_t         *pd = NULL;
    soc_port_ability_t    ability;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_adv_local_set: "
                            "unit %d, port %d, mode %d\n"), unit, port, mode));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_SET(pd, unit, port, mode));
        if (SOC_E_UNAVAIL == rv) {
            rv = PHY_ABILITY_ADVERT_GET(pd, unit, port, &ability); /* Read the currently configured value. 
                                                                      So attributes are not available from *_PA_* */
            if (SOC_SUCCESS(rv)) {
                rv = soc_port_mode_to_ability(mode, &ability);
                if (SOC_SUCCESS(rv)) {
                    rv = (PHY_ABILITY_ADVERT_SET(pd, unit, port, &ability));
                }
            }
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_adv_local_set: u=%d p=%d "
                      "m=%x rv=%d\n", unit, port, mode, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_local_get
 * Purpose:
 *      Get current local advertising setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Current advertised mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int                   rv;
    phy_driver_t         *pd = NULL;
    soc_port_ability_t    ability;

    SOC_NULL_PARAM_CHECK(mode);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_adv_local_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_GET(pd, unit, port, mode));
        if (rv == SOC_E_UNAVAIL) {
            rv = (PHY_ABILITY_ADVERT_GET(pd, unit, port, &ability));
            if (SOC_SUCCESS(rv)) {
                rv = soc_port_ability_to_mode(&ability, mode);
            }
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_adv_local_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_remote_get
 * Purpose:
 *      Get link partner advertised mode
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Link partner advertised mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int                   rv;
    phy_driver_t         *pd = NULL;
    soc_port_ability_t    ability;

    SOC_NULL_PARAM_CHECK(mode);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_adv_remote_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);
    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_REMOTE_GET(pd, unit, port, mode));

        if (rv == SOC_E_UNAVAIL) {
            rv = (PHY_ABILITY_REMOTE_GET(pd, unit, port, &ability));
            if (SOC_SUCCESS(rv)) {
                rv = soc_port_ability_to_mode(&ability, mode);
            }
        }
    }
    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_adv_remote_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_loopback_set
 * Purpose:
 *      Enable/disable loopback mode.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 *      serdes_linkup_wait - (1) Wait for Linkup for serdes
 *                           (0) Don't wait for Linkup
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_loopback_set(int unit, soc_port_t port, int enable, int serdes_linkup_wait)
{
    int           rv;
    int           port_enable;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_loopback_set: "
                            "unit %d, port %d, enable %d\n"), unit, port, enable));
    
    rv     = SOC_E_NONE;
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        pd = ext_pc->pd;
    } else {
        pd = int_pc->pd;
    }
    INT_MCU_LOCK(unit);
    rv = (PHY_LOOPBACK_SET(pd, unit, port, enable));
    if (SOC_SUCCESS(rv)) {
        rv = PHY_ENABLE_GET(pd, unit, port, &port_enable);
    }

    if (serdes_linkup_wait && !SOC_IS_SAND(unit) &&
        !SOC_IS_TRIUMPH3(unit)) { 
        if (SOC_SUCCESS(rv) && enable && (NULL != int_pc) && (port_enable)) { 
            if ((PHY_PASSTHRU_MODE(unit, port)) || (NULL == ext_pc)) {
                int           link;
                soc_timeout_t to;

                /* Wait up to 5000 msec for link up */
                soc_timeout_init(&to, 5000000, 0);
                link = 0;
                /*
                 * Needs more than one read to clear Latched Link down bits.
                 */
                rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
                do {
                    rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
                    if (link || SOC_FAILURE(rv)) {
                        break;
                    }
                } while (!soc_timeout_check(&to));
                if (!link) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "soc_phyctrl_loopback_set: u=%d p=%d TIMEOUT\n"),
                              unit, port));
                    rv = SOC_E_TIMEOUT;
                }
            }
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_loopback_set: u=%d p=%d l=%d rv=%d\n",
                      unit, port, enable, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_redirect_loopback_set
 * Purpose:
 *      Enable/disable loopback mode.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied 
 *             on system side(1)or line side(0).
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 *      serdes_linkup_wait - (1) Wait for Linkup for serdes
 *                           (0) Don't wait for Linkup
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_loopback_set(int unit, soc_port_t port, int phyn, 
                                  int phy_lane, int sys_side, int enable,
                                  int serdes_linkup_wait)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;

    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_redirect_loopback_set: "
                            "unit %d, port %d, enable %d\n"), unit, port, enable));

    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get(unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_loopback_set: u=%d p=%d l=%d rv=%d\n",
                      unit, port, enable, rv));
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL:0);    

 
    rv     = SOC_E_NONE;
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    INT_MCU_LOCK(unit);
    rv = (PHY_LOOPBACK_SET(pc_head->pd, unit, port, enable));

    /* Wait until link up if internal PHY is put into loopback */

    if(serdes_linkup_wait && !SOC_IS_SAND(unit) &&
       !SOC_IS_TRIUMPH3(unit)) { 
        if (SOC_SUCCESS(rv) && enable && (NULL != int_pc)) { 
            if ((PHY_PASSTHRU_MODE(unit, port)) || (NULL == ext_pc)) {
                int           link;
                soc_timeout_t to;

                /* Wait up to 5000 msec for link up */
                soc_timeout_init(&to, 5000000, 0);
                link = 0;
                /*
                 * Needs more than one read to clear Latched Link down bits.
                 */
                rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
                do {
                    rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
                    if (link || SOC_FAILURE(rv)) {
                        break;
                    }
                } while (!soc_timeout_check(&to));
                if (!link) {
                    LOG_WARN(BSL_LS_SOC_PHY,
                             (BSL_META_U(unit,
                                         "soc_phyctrl_redirect_loopback_set: u=%d p=%d TIMEOUT\n"),
                              unit, port));
                    rv = SOC_E_TIMEOUT;
                }
            }
        }
    }
    INT_MCU_UNLOCK(unit);

    /* Clear flags set earlier for PHY_CONTROL_SET(). Clear them anyway..*/
    pc_ptr->flags &= ~PHYCTRL_CHAINED_PHY_CTRL;
    pc_ptr->flags &= ~PHYCTRL_SYS_SIDE_CTRL;

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_loopback_set: u=%d p=%d l=%d rv=%d\n",
                      unit, port, enable, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_loopback_extended_set
 * Purpose:
 *      Enable/disable loopback mode.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 *      serdes_linkup_wait - (1) Wait for Linkup for serdes
 *                           (0) Don't wait for Linkup
 *      en_int_phy - (1) Enable internal PHY lpbk by default
 *                   (0) Same operation as soc_phyctrl_loopback_set
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_loopback_extended_set(int unit, soc_port_t port, int enable,
                                  int serdes_linkup_wait, uint32 en_int_phy)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd;

    if (en_int_phy == FALSE) {
        return soc_phyctrl_loopback_set(unit, port, enable, serdes_linkup_wait);
    }

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_loopback_extended_set: "
                            "unit %d, port %d, enable %d\n"), unit, port, enable));

    rv     = SOC_E_NONE;
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    INT_MCU_LOCK(unit);
    if (NULL != ext_pc) {
        pd = ext_pc->pd;
        rv = (PHY_LOOPBACK_SET(pd, unit, port, enable));
    }
    pd = int_pc->pd;
    rv = (PHY_LOOPBACK_SET(pd, unit, port, enable));

    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_loopback_extended_set: u=%d p=%d l=%d rv=%d\n",
                      unit, port, enable, rv));
    }

    return rv;
}


/*
 * Function:
 *      soc_phyctrl_loopback_get
 * Purpose:
 *      Get current loopback setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_loopback_get(int unit, soc_port_t port, int *enable)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd = NULL;

    SOC_NULL_PARAM_CHECK(enable);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_loopback_get: "
                            "unit %d, port %d\n"), unit, port));
    
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        pd = ext_pc->pd;
    } else {
        pd = int_pc->pd;
    }

    INT_MCU_LOCK(unit);

    rv = (PHY_LOOPBACK_GET(pd, unit, port, enable));

    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_loopback_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_redirect_loopback_get
 * Purpose:
 *      Get current loopback mode for phy specific port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied 
 *             on system side(1)or line side(0).
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_loopback_get(int unit, soc_port_t port, int phyn, 
                                  int phy_lane, int sys_side, int *enable)
{
    int           rv;
    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get(unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_loopback_get failed %d\n", rv));
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL:0);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LOOPBACK_GET(pc_head->pd, unit, port, enable));
    }
    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_loopback_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_interface_set
 * Purpose:
 *      Set the interface between MAC and PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pif  - Interface type 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_interface_set: "
                            "unit %d, port %d, pif %d\n"), unit, port, pif));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_INTERFACE_SET(pd, unit, port, pif));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_interface_set: u=%d p=%d i=%d rv=%d\n",
                      unit, port, pif, rv));
    }
    return rv; 
}

/*
 * Function:
 *      soc_phyctrl_interface_get
 * Purpose:
 *      Get current interface setting between MAC and PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pif  - current interface setting
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_interface_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_INTERFACE_GET(pd, unit, port, pif));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_interface_get failed %d\n", rv));
    }
    return rv;
}

STATIC int
_soc_phy_ability_get(int unit, soc_port_t port, 
                         phy_driver_t *pd, soc_port_mode_t *mode)
{
    int                 rv;
    soc_port_ability_t  ability;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phy_ability_get: "
                            "unit %d, port %d\n"), unit, port));
    
    INT_MCU_LOCK(unit);
    rv = (PHY_ABILITY_GET(pd, unit, port, mode));

    if (SOC_E_UNAVAIL == rv) {
        rv = (PHY_ABILITY_LOCAL_GET(pd, unit, port, &ability));
        if (SOC_SUCCESS(rv)) {
            rv = soc_port_ability_to_mode(&ability, mode);
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("_soc_phy_ability_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_get
 * Purpose:
 *      Get PHY ability
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int              rv;
    soc_port_mode_t  mode_speed_int;
    soc_port_mode_t  mode_speed_ext;
    phy_ctrl_t      *int_pc;
    phy_ctrl_t      *ext_pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ability_get: "
                            "unit %d, port %d\n"), unit, port));
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);
    rv = SOC_E_NONE;

    mode_speed_int = mode_speed_ext = SOC_PM_SPEED_ALL;
    if (NULL != int_pc) {

        if (int_pc->speed_max > 16000) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "soc_phyctrl_ability_get: Speed support above 16Gbps will"
                                  "not work. Use soc_phyctrl_ability_local_get\n")));
        }

        rv = _soc_phy_ability_get(unit, port, int_pc->pd, mode);
        mode_speed_int = *mode & SOC_PM_SPEED_ALL;
    }
    if (SOC_SUCCESS(rv) && NULL != ext_pc) {
        rv = _soc_phy_ability_get(unit, port, ext_pc->pd, mode);
        mode_speed_ext = *mode & SOC_PM_SPEED_ALL;
    }

    if (SOC_SUCCESS(rv)) {
        *mode &= ~(SOC_PM_SPEED_ALL);
        *mode |= (mode_speed_int & mode_speed_ext);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_ability_get failed %d\n", rv));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "soc_phyctrl_ability_get E=%08x I=%08x C=%08x\n"),
              mode_speed_ext, mode_speed_int,*mode));
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_linkup_evt
 * Purpose:
 *      Force link up event to PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_linkup_evt(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_linkup_evt: "
                            "unit %d, port %d\n"), unit, port));
    
    pd = NULL;
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINKUP_EVT(pd, unit, port));
    }
    INT_MCU_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_linkdn_evt
 * Purpose:
 *      Force link down event to PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_linkdn_evt(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_linkdn_evt: "
                            "unit %d, port %d\n"), unit, port));
    
    pd = NULL;
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINKDN_EVT(pd, unit, port));
    }
    INT_MCU_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_port_phy_multi_get
 * Description:
 *      General PHY register read
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      flags - TBD
 *      dev_addr - Device address on the PHY's bus (ex. I2C addr)
 *      offset - Offset within device
 *      max_size - Requested data size
 *      data - Data buffer
 *      actual_size - Received data size
 * Returns:
 *      BCM_E_XXX
 */
int
soc_phyctrl_port_phy_multi_get(int unit, soc_port_t port, uint32 flags,
                 uint32 dev_addr, uint32 offset, int max_size, uint8 *data, int *actual_size)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_port_phy_multi_get: "
                            "unit=%d, port=%d\n"), unit, port));

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MULTI_GET(pd, unit, port, flags, dev_addr, offset, max_size, data, actual_size));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_port_phy_multi_get: u=%d p=%d rv=%d\n",
                       unit, port, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_set
 * Purpose:
 *      Set new mdix setting 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mdix - new mdix mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mdix)
{
    int           rv;
    phy_driver_t *pd=NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_mdix_set: "
                            "unit %d, port %d, mdix %d\n"), unit, port, mdix));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_SET(pd, unit, port, mdix));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_mdix_set: u=%d p=%d m=%d rv=%d\n",
                       unit, port, mdix, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_get
 * Purpose:
 *      Get current mdix setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mdix - current mdix mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mdix)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(mdix);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_mdix_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_GET(pd, unit, port, mdix));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_mdix_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_status_get
 * Purpose:
 *      Current resolved mdix status.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      status - Current resolved mdix status.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_status_get(int unit, soc_port_t port,
                        soc_port_mdix_status_t *status)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(status);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_mdix_status_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_STATUS_GET(pd, unit, port, status));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_mdix_status_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_config_set
 * Purpose:
 *      Set configuration of selected medium
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - Selected medium
 *      cfg    - New configuration of the selected medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_config_set(int unit, soc_port_t port, 
                          soc_port_medium_t medium,
                          soc_phy_config_t *cfg)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(cfg);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_medium_config_set: "
                            "unit %d, port %d, medium %d\n"), 
                 unit, port, medium));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_CONFIG_SET(pd, unit, port, medium, cfg));
    }
    INT_MCU_UNLOCK(unit);
 
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_medium_config_set: u=%d p=%d m=%d rv=%d\n",
                       unit, port, medium, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_config_get
 * Purpose:
 *      Get current configuration of the selected medium
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - Selected medium
 *      cfg    - Current configuration of the selected medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_config_get(int unit, soc_port_t port, 
                          soc_port_medium_t medium,
                          soc_phy_config_t *cfg)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(cfg);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_medium_config_get: "
                            "unit %d, port %d, medium %d\n"), 
                 unit, port, medium));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_CONFIG_GET(pd, unit, port, medium, cfg));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_medium_config_get failed %d\n", 
                       rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_get
 * Purpose:
 *      Get active medium type
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - active medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(medium);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_medium_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_GET(pd, unit, port, medium));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_medium_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_cable_diag
 * Purpose:
 *      Run cable diag on the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      status - Cable status
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_cable_diag(int unit, soc_port_t port, soc_port_cable_diag_t *status)
{
    int         rv;
    phy_ctrl_t *ext_pc;

    SOC_NULL_PARAM_CHECK(status);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_cable_diag: "
                            "unit %d, port %d\n"), unit, port));
    
    rv     = SOC_E_UNAVAIL;
    ext_pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != ext_pc) {
        INT_MCU_LOCK(unit);
        rv = (PHY_CABLE_DIAG(ext_pc->pd, unit, port, status));
        INT_MCU_UNLOCK(unit);

        if (SOC_FAILURE(rv)) {
            PHYCTRL_WARN(("soc_phyctrl_cable_diag failed %d\n", rv));
        }
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_link_change
 * Purpose:
 *      Force link change on the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      link - Link status to change
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_link_change(int unit, soc_port_t port, int *link)
{
    int           rv;
    phy_driver_t *pd=NULL;

    SOC_NULL_PARAM_CHECK(link);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_link_change: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINK_CHANGE(pd, unit, port, link));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_link_change failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_control_set
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_control_set(int unit, soc_port_t port, 
                    soc_phy_control_t phy_ctrl, uint32 value)
{

    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd = NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_control_set: "
                            "unit %d, port %d, phy_ctrl %d, value %u\n"), 
                 unit, port, phy_ctrl, value));
    
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        pd = ext_pc->pd;
    } else {
        pd = int_pc->pd;
    }


    INT_MCU_LOCK(unit);

    rv = (PHY_CONTROL_SET(pd, unit, port, phy_ctrl, value));

    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_control_set: u=%d p=%d c=%d rv=%d\n",
                       unit, port, phy_ctrl, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_control_get
 * Purpose:
 *      Get current setting of the PHY control
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phy_ctrl - PHY control type to read 
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_control_get(int unit, soc_port_t port, 
                    soc_phy_control_t phy_ctrl, uint32 *value)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd = NULL;

    SOC_NULL_PARAM_CHECK(value);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_control_get: "
                            "unit %d, port %d, phy_ctrl %d\n"), 
                 unit, port, phy_ctrl));
    
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        pd = ext_pc->pd;
    } else {
        pd = int_pc->pd;
    }

    INT_MCU_LOCK(unit);

    rv = (PHY_CONTROL_GET(pd, unit, port, phy_ctrl, value));

    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_control_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_redirect_control_set
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control set to be applied 
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_control_set(int unit, soc_port_t port, int phyn, 
                                 int phy_lane, int sys_side, 
                                 soc_phy_control_t phy_ctrl,  uint32 value)
{
    int           rv;
    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get(unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_control_set failed: u=%d p=%d c=%d rv=%d\n",
                      unit, port, phy_ctrl, rv));
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL : PHYCTRL_LINE_SIDE_CTRL);

    /* Lane */
    INT_MCU_LOCK(unit);

    /* In current system design, for chained phys phy_control flows 
       through/to the phy that is directly attached. Use *pc_head
       which points to either int or ext phy (head of the chain)
      */ 
    if (phy_lane >= 0) {
        rv = (PHY_LANE_CONTROL_SET(pc_head->pd, unit, port, phy_lane, phy_ctrl, value));
    } else {    
        rv = (PHY_CONTROL_SET(pc_head->pd, unit, port, phy_ctrl, value));
    }
    INT_MCU_UNLOCK(unit);


    /* Clear flags set earlier for PHY_CONTROL_SET(). Clear them anyway..*/
    pc_ptr->flags &= ~PHYCTRL_CHAINED_PHY_CTRL;
    pc_ptr->flags &= ~(PHYCTRL_SYS_SIDE_CTRL | PHYCTRL_LINE_SIDE_CTRL);


    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_control_set failed: u=%d p=%d c=%d rv=%d\n",
                      unit, port, phy_ctrl, rv));
    }
    return rv;
}


/*
 * Function:
 *      soc_phyctrl_redirect_control_get
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phyn - Phy number
 *      phy_lane - Lane number
 *      sys  - Control get to be applied 
 *             on system side(1)or line side(0).
 *      phy_ctrl - PHY control type to read 
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_redirect_control_get(int unit, soc_port_t port, int phyn, 
                                 int phy_lane, int sys_side, 
                                 soc_phy_control_t phy_ctrl,  uint32 *value)
{
    int           rv;
    /*
       If chained phy, pc_ptr points to the pc of chained phy. Otherwise
       to int/ext phy. 
     */
    phy_ctrl_t *pc_ptr = NULL;

    /*
       Either chanied phy or directly attached, pc_head points to the 
       *pc of head of the phy chain i.e int/ext phy. 
     */
    phy_ctrl_t *pc_head = NULL;


    /*phyn - int/ext/chained*/
    rv = soc_phyctrl_phyn_pc_get(unit, port, phyn, &pc_head, &pc_ptr);
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_redirect_control_get failed: u=%d p=%d c=%d rv=%d\n",
                      unit, port, phy_ctrl, rv));
        return rv;
    }

    /* sys_side */
    pc_ptr->flags |= (sys_side ? PHYCTRL_SYS_SIDE_CTRL : PHYCTRL_LINE_SIDE_CTRL);

    /* Lane */
    INT_MCU_LOCK(unit);

    /* In current system design, for chained phys phy_control flows 
       through/to the phy that is directly attached. Use *pc_head
       which points to either int or ext phy (head of the chain)
      */ 
    if (phy_lane >= 0) {
        rv = (PHY_LANE_CONTROL_GET(pc_head->pd, unit, port, phy_lane, phy_ctrl, value));
    } else {    
        rv = (PHY_CONTROL_GET(pc_head->pd, unit, port, phy_ctrl, value));
    }
    INT_MCU_UNLOCK(unit);


    /* Clear flags set earlier for PHY_CONTROL_SET(). Clear them anyway..*/
    pc_ptr->flags &= ~PHYCTRL_CHAINED_PHY_CTRL;
    pc_ptr->flags &= ~(PHYCTRL_SYS_SIDE_CTRL | PHYCTRL_LINE_SIDE_CTRL);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_control_get failed : u=%d p=%d c=%d rv=%d\n",
                      unit, port, phy_ctrl, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_read
 * Purpose:
 *      Read PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data read
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_read(int unit, soc_port_t port, uint32 flags, 
                 uint32 addr, uint32 *data)
{
    int         rv;
    phy_ctrl_t *pc;

    SOC_NULL_PARAM_CHECK(data);
    SOC_PORT_PARAM_CHECK(port);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_reg_read: "
                            "unit %d, port %d, flags %u, addr %u\n"), 
                 unit, port, flags, addr));
    
    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_REG_READ(pc->pd, unit, port, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_reg_read failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_write
 * Purpose:
 *      Write to PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data to be written 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_write(int unit, soc_port_t port, uint32 flags, 
                      uint32 addr, uint32 data)
{
    int         rv;
    phy_ctrl_t *pc;

    SOC_PORT_PARAM_CHECK(port);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_reg_write: "
                            "unit %d, port %d, flags %u, addr %u, data %u\n"), 
                 unit, port, flags, addr, data));
    
    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_REG_WRITE(pc->pd, unit, port, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_reg_write failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_lane_control_set
 * Purpose:
 *      Set PHY specific per-lane configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      lane - PHY lane number.
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_lane_control_set(int unit, soc_port_t port, int lane,
                             soc_phy_control_t phy_ctrl, uint32 value)
{

    int           rv;
    phy_driver_t *pd = NULL;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_lane_conrol_set: "
                            "unit %d, port %d, lane %d, phy_ctrl %d, value %u\n"), 
                 unit, port, lane, phy_ctrl, value));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LANE_CONTROL_SET(pd, unit, port, lane, phy_ctrl, value));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_lane_control_set: u=%d p=%d l=%d c=%d rv=%d\n",
                      unit, port, lane, phy_ctrl, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_lane_control_get
 * Purpose:
 *      Get current setting of the PHY control per-lane
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      lane - PHY lane number.
 *      phy_ctrl - PHY control type to read 
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_lane_control_get(int unit, soc_port_t port, int lane,
                             soc_phy_control_t phy_ctrl, uint32 *value)
{
    int           rv;
    phy_driver_t *pd = NULL;

    SOC_NULL_PARAM_CHECK(value);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_lane_control_get: "
                            "unit %d, port %d, lane %d, phy_ctrl %d\n"), 
                 unit, port, lane, phy_ctrl));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LANE_CONTROL_GET(pd, unit, port, lane, phy_ctrl, value));
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_lane_control_get: u=%d p=%d l=%d c=%d rv=%d\n",
                      unit, port, lane, phy_ctrl, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_lane_reg_read
 * Purpose:
 *      Read per-lane PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      lane - PHY lane number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data read
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_lane_reg_read(int unit, soc_port_t port, int lane,
                          uint32 flags, uint32 addr, uint32 *data)
{
    int         rv;
    phy_ctrl_t *pc;

    SOC_NULL_PARAM_CHECK(data);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_lane_reg_read: "
                            "unit %d, port %d, lane %d, flags %u, addr %u\n"), 
                 unit, port, lane, flags, addr));
    
    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_LANE_REG_READ(pc->pd, unit, port, lane, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_lane_reg_read: "
                      "u=%d p=%d l=%d f=x0%08x addr=0x%08x rv=%d\n",
                      unit, port, lane, flags, addr, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_lane_reg_write
 * Purpose:
 *      Write to per-lane PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      lane - PHY lane number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data to be written 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_lane_reg_write(int unit, soc_port_t port, int lane,
                           uint32 flags, uint32 addr, uint32 data)
{
    int         rv;
    phy_ctrl_t *pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_lane_reg_write: "
                            "unit %d, port %d, lane %d, flags %u, addr %u, data %u\n"), 
                 unit, port, lane, flags, addr, data));
    
    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_LANE_REG_WRITE(pc->pd, unit, port, lane, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_lane_reg_write: "
                      "u=%d p=%d l=%d f=x0%08x addr=0x%08x rv=%d\n",
                      unit, port, lane, flags, addr, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_modify
 * Purpose:
 *      Modify PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data to be written
 *      mask  - bit mask of data to be modified
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_modify(int unit, soc_port_t port, uint32 flags, 
                   uint32 addr, uint32 data, uint32 mask)
{
    int         rv;
    phy_ctrl_t *pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_reg_modify: "
                            "unit %d, port %d, flags %u, addr %u, data %u, mask %u\n"), 
                 unit, port, flags, addr, data, mask));
    
    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_REG_MODIFY(pc->pd, unit, port, flags, addr, data, mask);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_reg_modify failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_advert_get
 * Purpose:
 *      Get local PHY advertised ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_advert_get(int unit, soc_port_t port, 
                            soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd = NULL;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ability_advert_get: "
                            "unit %d, port %d\n"), unit, port));

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_ADVERT_GET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ADV_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            sal_memset(ability, 0, sizeof(*ability));
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_ability_advert_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_advert_set
 * Purpose:
 *      Set local PHY advertised ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_advert_set(int unit, soc_port_t port, 
                            soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd = NULL;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ability_advert_set: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_ADVERT_SET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = soc_port_ability_to_mode(ability, &mode);
        if (SOC_SUCCESS(rv)) {
            rv = PHY_ADV_SET(pd, unit, port, mode); 
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_ability_advert_set failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_remote_get
 * Purpose:
 *      Get remote PHY advertsied ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_remote_get(int unit, soc_port_t port, 
                                   soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd = NULL;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ability_remote_get: "
                            "unit %d, port %d\n"), unit, port));
    
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    INT_MCU_LOCK(unit);
    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_REMOTE_GET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ADV_REMOTE_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            sal_memset(ability, 0, sizeof(*ability));
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }
    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_ability_remote_get failed %d\n", rv));
    }

    return rv;
}

STATIC int
_soc_phy_ability_local_get(int unit, soc_port_t port, 
                         phy_driver_t *pd, soc_port_ability_t *ability)
{
    int              rv;
    soc_port_mode_t  mode;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phy_ability_local_get: "
                            "unit %d, port %d\n"), unit, port));
    
    INT_MCU_LOCK(unit);
    rv = PHY_ABILITY_LOCAL_GET(pd, unit, port, ability);

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ABILITY_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            sal_memset(ability, 0, sizeof(*ability));
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }
    INT_MCU_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_local_get
 * Purpose:
 *      Get PHY ability
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_local_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    int                 rv;
    soc_port_ability_t  ability_int;
    soc_port_ability_t  ability_ext;
    phy_ctrl_t         *int_pc;
    phy_ctrl_t         *ext_pc;

    SOC_NULL_PARAM_CHECK(ability);

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ability_local_get: "
                            "unit %d, port %d\n"), unit, port));
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    ability_int.speed_half_duplex = ability_ext.speed_half_duplex = SOC_PA_SPEED_ALL;
    ability_int.speed_full_duplex = ability_ext.speed_full_duplex = SOC_PA_SPEED_ALL;

    rv = SOC_E_NONE;
    if (NULL != int_pc) {
#ifdef PORTMOD_SUPPORT    
        if (!SOC_USE_PORTCTRL(unit)) {
#endif
        rv = _soc_phy_ability_local_get(unit, port, int_pc->pd, ability);
        ability_int.speed_full_duplex = ability->speed_full_duplex;
        ability_int.speed_half_duplex = ability->speed_half_duplex;
#ifdef PORTMOD_SUPPORT
       } /* if portmod feature enabled */
#endif        
    }

    if (SOC_SUCCESS(rv) && NULL != ext_pc) {
        /* next make sure it's not phy_null_driver */
        if (ext_pc->write != NULL) {
            ability->speed_half_duplex = ability->speed_full_duplex = 0;
            rv = _soc_phy_ability_local_get(unit, port, ext_pc->pd, ability);
            ability_ext.speed_full_duplex = ability->speed_full_duplex;
            ability_ext.speed_half_duplex = ability->speed_half_duplex;
        }
    }

    if (SOC_SUCCESS(rv)) {
#ifdef PORTMOD_SUPPORT    
        if (!SOC_USE_PORTCTRL(unit)) {
#endif        
        ability->speed_half_duplex = ability_int.speed_half_duplex & ability_ext.speed_half_duplex;
        ability->speed_full_duplex = ability_int.speed_full_duplex & ability_ext.speed_full_duplex;
#ifdef PORTMOD_SUPPORT    
        }
#endif        
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_ability_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_firmware_set
 * Purpose:
 *      Update the phy device's firmware 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      offset - offset to the data stream
 *      addr  - PHY register address
 *      data  - data to be written
 *      mask  - bit mask of data to be modified
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_firmware_set(int unit, soc_port_t port, uint32 flags,
                   int offset, uint8 *array, int len)
{
    int         rv;
    phy_ctrl_t *pc;
                  
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_firmare_set: "
                            "unit %d, port %d, flags %u, offset %d, len %d\n"), 
                 unit, port, flags, offset, len));
                                                                  
    rv = SOC_E_UNAVAIL;
                                                                                
    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }
                                                                                
    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_FIRMWARE_SET(pc->pd, unit, port, offset, array, len);
        INT_MCU_UNLOCK(unit);
    }
                                                                                
    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_firmware_set failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_synce_clock_set
 * Purpose:
 *      configure SyncE clock
 * Parameters:
 *      unit    - SOC Unit #.
 *      port    - Port number.
 *      mode0   - Stage0 mode
 *      mode1   - Stage1 mode
 *      sdm_value  - SDM value of stage0
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_synce_clock_set(int unit, soc_port_t port, uint32 mode0,
                   uint32 mode1, uint32 sdm_value)
{
    int         rv;
    phy_ctrl_t *pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_synce_clock_set: "
                            "unit %d, port %d, mode0 %u, mode1 %u, sdm_value %u\n"),
                 unit, port, mode0, mode1, sdm_value));

    rv = SOC_E_UNAVAIL;

    pc = INT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_SYNCE_CLK_SET(pc->pd, unit, port, mode0, mode1, sdm_value);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_synce_clock_set failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_synce_clock_get
 * Purpose:
 *      configure SyncE clock
 * Parameters:
 *      unit    - SOC Unit #.
 *      port    - Port number.
 *      mode0   - Stage0 mode
 *      mode1   - Stage1 mode
 *      sdm_value  - SDM value of stage0
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_synce_clock_get(int unit, soc_port_t port, uint32 *mode0,
                   uint32 *mode1, uint32 *sdm_value)
{
    int         rv;
    phy_ctrl_t *pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_synce_clock_set: "
                            "unit %d, port %d\n"),unit, port));

    rv = SOC_E_UNAVAIL;

    pc = INT_PHY_SW_STATE(unit, port);

    if (NULL != pc) {
        INT_MCU_LOCK(unit);
        rv = PHY_SYNCE_CLK_GET(pc->pd, unit, port, mode0, mode1, sdm_value);
        INT_MCU_UNLOCK(unit);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_synce_clock_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_diag_ctrl
 * Purpose:
 * Call underlying driver's routine to perform a range of diagnostic 
 * functions on the serdes and phy devices. This function allows a control 
 * action to be addressed to a specific instance of the device on the port.
 * In a scenario where a port is connected with a serdes device and a PHY 
 * device, This function can be used to access both devices.
 * This function doesn't provide mutual exclusion access to the device.
 * Caller should obtain it before calling this function.
 * 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      instance - the specific device block the control action directs to 
 *      op_type  - operation types: read,write or command sequence 
 *      op_cmd   - command code  
 *      arg      - command argument based on op_type/op_cmd 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_diag_ctrl( 
   int unit, /* unit */ 
   soc_port_t port, /* port */ 
   uint32 inst, /* the specific device block the control action directs to */ 
   int op_type,  /* operation types: read,write or command sequence */ 
   int op_cmd,   /* command code */ 
   void *arg)     /* command argument based on op_type/op_cmd */ 
{ 
    int         rv;
    phy_ctrl_t *pc;
    int dev  = PHY_DIAG_INST_DEV(inst);
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_diag_ctrl: "
                            "unit %d, port %d, inst %u, op_type %d, op_cmd %d\n"), 
                 unit, port, inst, op_type, op_cmd));
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    rv = SOC_E_UNAVAIL;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d inst=0x%x op_type=0x%x, op_cmd=0x%x, arg=0x%x\n"),
              unit, port, inst, op_type, op_cmd, PTR_TO_INT(arg)));
    
    if (dev == PHY_DIAG_DEV_DFLT) {
        pc = ext_pc? ext_pc: int_pc;
    } else if (dev == PHY_DIAG_DEV_INT) {
        pc = int_pc;
    } else {
        pc = ext_pc;
    }

    if (NULL != pc) {
        rv = PHY_DIAG_CTRL(pc->pd, unit, port,PHY_DIAG_INST_INTF_LN(inst),
                              op_type,op_cmd,arg);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_diag_ctrl failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_primary_set
 * Purpose:
 *      Set primary port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      primary  - primary port of the phy chip
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_primary_set(int unit, soc_port_t port, soc_port_t primary)
{
    soc_phy_chip_info_t *chip_info;
                                                                                
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_primary_set: "
                            "unit %d, port %d, primary %d\n"), unit, port, primary));
    
    if (SOC_PHY_INFO(unit, port).chip_info == NULL) {
        SOC_PHY_INFO(unit, port).chip_info = sal_alloc(sizeof(soc_phy_chip_info_t), 
                                             "phy_chip_info");
        if (SOC_PHY_INFO(unit, port).chip_info == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_PHY_INFO(unit, port).chip_info, -1, sizeof(soc_phy_chip_info_t));
    }

    chip_info = SOC_PHY_INFO(unit, port).chip_info;

    chip_info->primary = primary;
                                                                                
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phyctrl_primary_get
 * Purpose:
 *      Get primary port
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      primary  - primary port of the phy chip
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_primary_get(int unit, soc_port_t port, soc_port_t *primary)
{
    soc_phy_chip_info_t *chip_info;
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_primary_get: "
                            "unit %d, port %d\n"), unit, port));
                                                                                   
    chip_info = SOC_PHY_INFO(unit, port).chip_info;

    if ((chip_info == NULL) || (chip_info->primary == -1)) {
        return SOC_E_UNAVAIL;
    }
                                                                                
    *primary = chip_info->primary;

    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phyctrl_offset_set
 * Purpose:
 *      Set port offset
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      offset  - offset of the port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_offset_set(int unit, soc_port_t port, int offset)
{
    soc_phy_chip_info_t *chip_info, *primary_chip_info;
                      
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_offset_set: "
                            "unit %d, port %d, offset %d\n"), unit, port, offset));
                                                              
    chip_info = SOC_PHY_INFO(unit, port).chip_info;

    if ((chip_info == NULL) || (chip_info->primary == -1) || (!PHY_OFFSET_VALID(offset))) {
        /* set primary first and make sure that the offset is valid */
        return SOC_E_UNAVAIL;
    }

    if (SOC_PHY_INFO(unit, chip_info->primary).chip_info == NULL) {
        SOC_PHY_INFO(unit, chip_info->primary).chip_info = sal_alloc(sizeof(soc_phy_chip_info_t), 
                                             "phy_chip_info");
        if (SOC_PHY_INFO(unit, chip_info->primary).chip_info == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(SOC_PHY_INFO(unit, chip_info->primary).chip_info, -1, sizeof(soc_phy_chip_info_t));
    }

    primary_chip_info = SOC_PHY_INFO(unit, chip_info->primary).chip_info;
    primary_chip_info->offset_to_port[offset] = port;
    chip_info->offset = offset;
                                                                                
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phyctrl_offset_get
 * Purpose:
 *      Get port offset
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      offset  - offset of the port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_offset_get(int unit, soc_port_t port, soc_port_t *offset)
{
    soc_phy_chip_info_t *chip_info, *primary_chip_info;
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_offset_get: "
                            "unit %d, port %d\n"), unit, port));
                                                             
    chip_info = SOC_PHY_INFO(unit, port).chip_info;

    if ((chip_info == NULL) || (!PHY_OFFSET_VALID(chip_info->offset))) {
        return SOC_E_UNAVAIL;
    }
    primary_chip_info = SOC_PHY_INFO(unit, chip_info->primary).chip_info;

    if (primary_chip_info == NULL) {
        return SOC_E_UNAVAIL;
    }

    /* integrity check */
    *offset = (primary_chip_info->offset_to_port[chip_info->offset] == port) ? chip_info->offset : -1 ;
                                                                                
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phy_primary_and_offset_get
 * Purpose:
 *      Get primary port offset
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      *primary_port  - primary port of the phy chip
 *      *offset  - offset of the curent phy port
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phy_primary_and_offset_get(int unit, soc_port_t port, soc_port_t *primary_port, int *offset) 
{
    uint32 primary_and_offset;
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phy_primary_and_offset_get: "
                            "unit %d, port %d\n"), unit, port));
    
    if ((primary_and_offset = soc_property_port_get(unit, port, spn_PHY_PORT_PRIMARY_AND_OFFSET, -1)) == -1) {
        *primary_port = -1;
        *offset = -1;
        return  SOC_E_FAIL;
    }

    *primary_port = (primary_and_offset >> 8) & 0xffff;

    *offset = primary_and_offset & 0xff;

    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phy_boot_master_get
 * Purpose:
 *      Get boot master and chip master settings
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      *bm - port is a bootmaster
 *      *cm - port is also a chipmaster
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phy_boot_master_get(int unit, soc_port_t port, int *bm, int *cm) 
{
    char *config_str, *sub_str, *sub_str_end;
    int val;

    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phy_boot_master_get: "
                            "unit %d, port %d\n"), unit, port));
    
     config_str = soc_property_port_get_str(unit, port, spn_PHY_BOOT_MASTER);

     if (config_str == NULL) {
         return  SOC_E_FAIL;
     }
 
     sub_str = config_str;
 
     val = sal_ctoi(sub_str, &sub_str_end);

     *bm = (val == 1) ? 1 : 0;

     if ((sub_str == sub_str_end) || (val != 1)) {
         *cm = 0;
         return  SOC_E_NONE;
     }
 
     /* Skip ':' */
     sub_str = sub_str_end;
     if (*sub_str != ':') {
         *cm = 0;
         return  SOC_E_NONE;
     }
     sub_str++;

     *cm = (*sub_str == 'c') ? 1 : 0;

     return SOC_E_NONE;

}

/*
 * Function:
 *      soc_phyctrl_toplvl_reg_read
 * Purpose:
 *      Read a top level register from a supporting chip
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      primary_port - Primary Port number.
 *      reg_offset  - Offset to the reg
 *      data  - Pointer to data returned
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_toplvl_reg_read(int unit, soc_port_t port, soc_port_t primary_port, 
                          uint8 reg_offset, uint16 *data)

{
    phy_ctrl_t    *pc, *pc_port3, *pc_port5;
    uint16 reg_data, status;
    soc_phy_chip_info_t *chip_info;
    int         rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_toplvl_reg_read: "
                            "unit %d, port %d, primary_port %d, reg_offset %u\n"), 
                 unit, port, primary_port, reg_offset));
    
    if ((primary_port == -1) || (SOC_PHY_INFO(unit, primary_port).chip_info == NULL)) {
        return SOC_E_FAIL;
    }

    chip_info = SOC_PHY_INFO(unit, primary_port).chip_info;

    if ((chip_info->offset_to_port[2] == -1) ||
        (chip_info->offset_to_port[4] == -1)) {

        return SOC_E_FAIL;
    }

    pc       = EXT_PHY_SW_STATE(unit, port);
    pc_port3 = EXT_PHY_SW_STATE(unit, chip_info->offset_to_port[2]);
    pc_port5 = EXT_PHY_SW_STATE(unit, chip_info->offset_to_port[4]);

    if ((!pc) || (!pc_port3) || (!pc_port5)) {
        return SOC_E_FAIL;
    }

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Status READ from Port 3's register 0x15 */

    /* Write Reg offset to Port 5's register 0x1C, shadow 0x0B */
    reg_data = (0xAC00 | reg_offset);
    INT_MCU_LOCK(unit);
    rv = pc->write(unit, pc_port5->phy_id, 0x1c, reg_data);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }

    /* Read data from Top level MII Status register(0x15h) */
    rv = pc->write(unit, pc_port3->phy_id, 0x17, 0x8F0B);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }
    rv = pc->read(unit, pc_port3->phy_id, 0x15, &status);
    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    *data = (status & 0xff);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_toplvl_reg_write
 * Purpose:
 *      Write to a top level register from a supporting chip
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      primary_port - Primary Port number.
 *      reg_offset  - Offset to the reg
 *      data  - Data to be written
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_toplvl_reg_write(int unit, soc_port_t port, soc_port_t primary_port,
                           uint8 reg_offset, uint16 data)

{
    phy_ctrl_t    *pc, *pc_port3, *pc_port5, *pc_port6;
    uint16 reg_data;
#ifdef BROADCOM_DEBUG
    uint16 status;
#endif
    soc_phy_chip_info_t *chip_info;
    int         rv = SOC_E_NONE;
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_toplvl_reg_write: "
                            "unit %d, port %d, primary_port %d, reg_offset %u, data %u\n"), 
                 unit, port, primary_port, reg_offset, data));

    if ((primary_port == -1) || (SOC_PHY_INFO(unit, primary_port).chip_info == NULL)) {
        return SOC_E_FAIL;
    }

    chip_info = SOC_PHY_INFO(unit, primary_port).chip_info;

    if ((chip_info->offset_to_port[2] == -1) ||
        (chip_info->offset_to_port[4] == -1) ||
        (chip_info->offset_to_port[5] == -1)) {

        return SOC_E_FAIL;
    }

    pc       = EXT_PHY_SW_STATE(unit, port);
    pc_port3 = EXT_PHY_SW_STATE(unit, chip_info->offset_to_port[2]);
    pc_port5 = EXT_PHY_SW_STATE(unit, chip_info->offset_to_port[4]);
    pc_port6 = EXT_PHY_SW_STATE(unit, chip_info->offset_to_port[5]);

    if ((!pc) || (!pc_port3) || (!pc_port5) || (!pc_port6)) {
        return SOC_E_FAIL;
    }

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Write data to Port 6's register 0x1C, shadow 0x0c */
    /* Status READ from Port 3's register 0x15 */

    /* Write Data to port6, register 0x1C, shadow 0x0c */
    INT_MCU_LOCK(unit);
    reg_data = (0xB000 | (data & 0xff));
    rv = pc->write(unit, pc_port6->phy_id, 0x1c, reg_data);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }

    /* Write Reg address to Port 5's register 0x1C, shadow 0x0B */
    /* Enable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 1 */
    reg_data = (0xAC80 | reg_offset);
    rv = pc->write(unit, pc_port5->phy_id, 0x1c, reg_data);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }

    /* Disable Write ( Port 5's register 0x1C, shadow 0x0B) Bit 7 = 0 */
    reg_data = (0xAC00 | reg_offset);
    rv = pc->write(unit, pc_port5->phy_id, 0x1c, reg_data);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }

#ifdef BROADCOM_DEBUG
    /* Read data from Top level MII Status register(0x15h) */
    rv = pc->write(unit, pc_port3->phy_id, 0x17, 0x8F0B);
    if (SOC_FAILURE(rv)) {
        INT_MCU_UNLOCK(unit);
        return rv;
    }
    rv = pc->read(unit, pc_port3->phy_id, 0x15, &status);

#endif

    INT_MCU_UNLOCK(unit);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_detach
 * Purpose:
 *      Remove PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_detach(int unit, soc_port_t port)
{
    phy_driver_t  *phyd;
    phy_ctrl_t    *ext_pc;
    phy_ctrl_t    *int_pc;
 
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_detach: "
                            "unit %d, port %d\n"), unit, port));
    
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);

    soc_phyctrl_phymod_free(int_pc);
    soc_phyctrl_phymod_free(ext_pc);

    if (NULL != int_pc) {
        /* indicate to re-initialize the port. Needed for flex port op on wc */
        int_pc->flags &= ~PHYCTRL_INIT_DONE;
        int_pc->block_init_flag = 0;
    }

    SOC_IF_ERROR_RETURN(soc_phy_nocxn(unit, &phyd));

    if (NULL == ext_pc) {
        /* This function is used for Hot Swapping external PHY driver.
         * Therefore, always attach no connection driver to external PHY. 
         */
        ext_pc = sal_alloc (sizeof (phy_ctrl_t), phyd->drv_name);
        if (NULL == ext_pc) {
            return SOC_E_MEMORY;
        }
        
        sal_memset(ext_pc, 0, sizeof(phy_ctrl_t));
        ext_pc->unit                 = unit;
        ext_pc->port                 = port;
        ext_pc->phy_id               = PHY_ADDR(unit, port);        
        EXT_PHY_SW_STATE(unit, port) = ext_pc;
    }

    ext_pc->pd   = phyd;

    /* Clear info displayed in 'phy info' */
    PHY_NAME(unit, port) = "Null";
    PHY_ID0_REG(unit, port) = 0;
    PHY_ID1_REG(unit, port) = 0;

    /* don't clear the init done flag in independent lane mode */
    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        PHY_FLAGS_CLR(unit,port,~PHY_FLAGS_INIT_DONE);
    } else {
        /* Clear the PHY configuration flags */
        PHY_FLAGS_CLR_ALL(unit, port);
    }
    return SOC_E_NONE;
}

#define PHYDEV_TYPE_MAX  20
#define MDIO_BUS_NUM_MAX  8

STATIC int
_soc_phyctrl_bcst_init(int unit, pbmp_t pbmp,char *dev_name,int bus_num,int ctrl,int ext_bus)
{
    soc_port_t port;
    phy_ctrl_t *pc;
	int rv;
    char pfmt[SOC_PBMP_FMT_LEN];
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_bcst_init: "
                            "unit %d, pbmp %s, dev_name %s, bus_num %d, ctrl %d, ext_bus %d\n"), 
                 unit, SOC_PBMP_FMT(pbmp, pfmt), dev_name, bus_num, ctrl, ext_bus));
    
    PBMP_ITER(pbmp, port) {
        pc = ext_bus? EXT_PHY_SW_STATE(unit, port):
                      INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }

        if (((pc->dev_name != NULL) && 
             (sal_strcmp(pc->dev_name, dev_name) == 0)) &&   /* match device name*/
            (PHY_ID_BUS_NUM(pc->phy_id) == bus_num) &&  /* match bus num*/
            (pc->flags & (PHYCTRL_MDIO_BCST | PHYCTRL_MDIO_BCST2))) {

             if ((ctrl == PHYCTRL_UCODE_BCST_LOAD2) && !(pc->flags & PHYCTRL_MDIO_BCST2))
                continue;
             if (((ctrl == PHYCTRL_UCODE_BCST_uC_SETUP || ctrl == PHYCTRL_UCODE_BCST_LOAD) &&
                  (pc->flags & PHYCTRL_MDIO_BCST)) ||
                 ((ctrl == PHYCTRL_UCODE_BCST_LOAD2) && (pc->flags & PHYCTRL_MDIO_BCST2))){
                 rv = soc_phyctrl_firmware_set(unit,port,ext_bus?0:SOC_PHY_INTERNAL,
                               ctrl,NULL,0);
                 break;
             }
             rv = soc_phyctrl_firmware_set(unit,port,ext_bus?0:SOC_PHY_INTERNAL,
                           ctrl,NULL,0);

             if (rv != SOC_E_NONE) {
                 LOG_WARN(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "u=%d p=%d: Firmware download failed.\n"), unit, port));
             }

             if (ctrl == PHYCTRL_UCODE_BCST_END) {
                 pc->flags |= PHYCTRL_UCODE_BCST_DONE;
             }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_phyctrl_ucode_bcst(int unit, pbmp_t pbmp, int ext_bus)
{
    soc_port_t port;
    int dev_types;
    int i;
    int j;
    int bcst_num;
    phy_ctrl_t *pc;
    int rv = SOC_E_NONE;
    char *dev_name[PHYDEV_TYPE_MAX];
    char pfmt[SOC_PBMP_FMT_LEN];

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_ucode_bcst: "
                            "unit %d, pbmp %s, ext_bus %d\n"), 
                 unit, SOC_PBMP_FMT(pbmp, pfmt), ext_bus));
    
    /* first check if bcst is requested */
    bcst_num = 0;
    PBMP_ITER(pbmp, port) {
        pc = ext_bus? EXT_PHY_SW_STATE(unit, port):
                      INT_PHY_SW_STATE(unit, port);
        if ((pc != NULL) && (pc->flags & (PHYCTRL_MDIO_BCST | PHYCTRL_MDIO_BCST2))) {
            bcst_num++;
        }
    }

    /* if number doesn't seem valid, exit */
    if (bcst_num < 1) {
        return SOC_E_NONE;
    }

    for (i = 0; i < PHYDEV_TYPE_MAX; i++) {
        dev_name[i] = NULL;
    }

    /* find out all PHY device types */
    dev_types = 0;
    PBMP_ITER(pbmp, port) {
        pc = ext_bus? EXT_PHY_SW_STATE(unit, port):
                      INT_PHY_SW_STATE(unit, port);
        if (pc == NULL) {
            continue;
        }
        for (i = 0; i < dev_types; i++) {
            if ((pc->dev_name == NULL) ||
                (dev_name[i] == NULL) ||
                (sal_strcmp(pc->dev_name, dev_name[i]) == 0)) {
                break;
            }
        }
        if (i >= dev_types) {
            if (pc->dev_name) {
                dev_name[dev_types++] = pc->dev_name;

                /* exceed device name array limit */
                if (dev_types >= PHYDEV_TYPE_MAX) {
                    break;
                }
            }
        }
    }

    /* walk through each device type */
    for (i = 0; i < dev_types; i++) {

        /* walk through each MDIO bus */
        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {

            /* pass 1: setup bcst mode for same type devices on the
             * same MDIO bus. go through each device
             */
            rv = _soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_SETUP,ext_bus);
        }  /* mdio bus loop */

        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {
            /* pass 2: first encounterd device's function is called.
             * bcst setup  
             */
            if (rv == SOC_E_NONE) {
                rv = _soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_uC_SETUP,ext_bus);
            }
        }  /* mdio bus loop */

        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {
            /* pass 3: all device's functions are called.
             * 84740 do a reset to signal uC start of download.
             * However this reset clears bcst configuration register
             */
            if (rv == SOC_E_NONE) {
                rv = _soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_ENABLE,ext_bus);
            }
        }  /* mdio bus loop */

        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {
            /* pass 4: first encounterd device's function is called.
             * bcst data to MDIO bus.
             */
            if (rv == SOC_E_NONE) {
                rv = _soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_LOAD,ext_bus);
            }
        }  /* mdio bus loop */

        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {
            /* pass 5: first encounterd FC device's function is called.
             * bcst data to MDIO bus.
             */
            if (rv == SOC_E_NONE) {
                rv = _soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_LOAD2,ext_bus);
            }
        }  /* mdio bus loop */

        for (j = 0; j < MDIO_BUS_NUM_MAX; j++) {
            /* pass 6: all device's functions are called.
             * read and check status. At the end, the bcst mode
             * must be turned off
             */
            SOC_IF_ERROR_RETURN
                (_soc_phyctrl_bcst_init(unit,pbmp,dev_name[i],j,
                                  PHYCTRL_UCODE_BCST_END,ext_bus));
        }  /* mdio bus loop */
    }  /* device type loop */

    return SOC_E_NONE;
}

STATIC int
soc_phyctrl_mdio_ucode_bcst(int unit, pbmp_t pbmp)
{
    int rv = SOC_E_NONE;
    char pfmt[SOC_PBMP_FMT_LEN];

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_mdio_ucode_bcst: "
                            "unit %d, pbmp %s\n"), unit, SOC_PBMP_FMT(pbmp, pfmt)));
    
    /* do external PHY device first */
    rv = _soc_phyctrl_ucode_bcst(unit,pbmp,TRUE);

    /* do internal serdes */
    /* fow now skip the internal serdes */
    return rv;
}

int
soc_phyctrl_pbm_probe_init(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp)
{
    int rv = SOC_E_NONE;
    soc_port_t port;
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;
    char pfmt[SOC_PBMP_FMT_LEN];
    
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_pbm_probe_init: "
                            "unit %d, pbmp %s,\n"), 
                 unit, SOC_PBMP_FMT(pbmp, pfmt)));

#ifdef INCLUDE_FCMAP
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        /* Block blmi accesses */
        SOC_IF_ERROR_RETURN(
            soc_fcmapphy_blmi_enable(0));
    }
#endif
#endif

    SOC_PBMP_CLEAR(*okay_pbmp);

    PBMP_ITER(pbmp, port) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Init port %d PHY...\n"), port));

        if ((rv = soc_phyctrl_probe(unit, port)) < 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Port %s: Failed to probe PHY: %s\n"),
                      SOC_PORT_NAME(unit, port), soc_errmsg(rv)));
            break;
        }

        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (ext_pc) {
            PHYCTRL_INIT_STATE_SET(ext_pc,PHYCTRL_INIT_STATE_PASS1);
        }
        if (int_pc) {
            PHYCTRL_INIT_STATE_SET(int_pc,PHYCTRL_INIT_STATE_PASS1);
        }

        /* do PHY init pass1 */
        if ((rv = soc_phyctrl_init(unit, port)) < 0) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "Port %s: Failed to initialize PHY: %s\n"),
                      SOC_PORT_NAME(unit, port), soc_errmsg(rv)));
            break;
        }

        SOC_PBMP_PORT_ADD(*okay_pbmp, port);
#if defined(BCM_ESW_SUPPORT)
        if(!SOC_IS_SAND(unit)) {
            soc_counter_port_pbmp_add(unit, port);
        }
#endif
    }

    /* check/perform PHY ucode broadcast if requested on valid pbmp */
    (void)soc_phyctrl_mdio_ucode_bcst(unit,*okay_pbmp);

    /* Releases the acquired firmware as it's no longer needed. This also
       means that the firmware will have to be re-acquired if bcm_init is run again. */
    soc_phy_fw_put_all();

    /* do PHY init pass2 if requested */
    PBMP_ITER(*okay_pbmp, port) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (int_pc) {
            if (PHYCTRL_INIT_STATE(int_pc) == PHYCTRL_INIT_STATE_PASS2) {
                rv = (PHY_INIT(int_pc->pd, unit, port));
            }
        }
        if (ext_pc) {
            if (PHYCTRL_INIT_STATE(ext_pc) == PHYCTRL_INIT_STATE_PASS2) {
                rv = (PHY_INIT(ext_pc->pd, unit, port));
            }
        }
    }

    /* do PHY init pass3 if requested */
    PBMP_ITER(*okay_pbmp, port) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (int_pc) {
            if (PHYCTRL_INIT_STATE(int_pc) == PHYCTRL_INIT_STATE_PASS3) {
                rv = (PHY_INIT(int_pc->pd, unit, port));
            }
        }
        if (ext_pc) {
            if (PHYCTRL_INIT_STATE(ext_pc) == PHYCTRL_INIT_STATE_PASS3) {
                rv = (PHY_INIT(ext_pc->pd, unit, port));
            }
        }
    }

    /* do PHY init pass4 if requested */
    PBMP_ITER(*okay_pbmp, port) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (int_pc) {
            if (PHYCTRL_INIT_STATE(int_pc) == PHYCTRL_INIT_STATE_PASS4) {
                rv = (PHY_INIT(int_pc->pd, unit, port));
            }
        }
        if (ext_pc) {
            if (PHYCTRL_INIT_STATE(ext_pc) == PHYCTRL_INIT_STATE_PASS4) {
                rv = (PHY_INIT(ext_pc->pd, unit, port));
            }
        }
    }

    /* do PHY init pass5 if requested */
    PBMP_ITER(*okay_pbmp, port) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (int_pc) {
            if (PHYCTRL_INIT_STATE(int_pc) == PHYCTRL_INIT_STATE_PASS5) {
                rv = (PHY_INIT(int_pc->pd, unit, port));
            }
        }
        if (ext_pc) {
            if (PHYCTRL_INIT_STATE(ext_pc) == PHYCTRL_INIT_STATE_PASS5) {
                rv = (PHY_INIT(ext_pc->pd, unit, port));
            }
        }
    }

    PBMP_ITER(*okay_pbmp, port) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);
        int_pc = INT_PHY_SW_STATE(unit, port);

        if (ext_pc) {
            PHYCTRL_INIT_STATE_SET(ext_pc,PHYCTRL_INIT_STATE_DEFAULT);
        }
        if (int_pc) {
            PHYCTRL_INIT_STATE_SET(int_pc,PHYCTRL_INIT_STATE_DEFAULT);
        }
    }

    return rv;
}

char * 
soc_phyctrl_event_string(soc_phy_event_t event)
{
     static char *phy_event[] = PHY_EVENT_STRING;

     LOG_VERBOSE(BSL_LS_SOC_PHY,
                 (BSL_META("entered soc_phyctrl_event_string: "
                           "event %d\n"), event));
     
     assert((sizeof(phy_event) / sizeof(phy_event[0])) == phyEventCount);

     if (event >= phyEventCount) {
         return "Unknown Event";
     }

     return phy_event[event];
}

/*
 * Function:
 *      soc_phyctrl_notify
 * Purpose:
 *      Notify events to internal PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_notify(int unit, soc_port_t port,
                   soc_phy_event_t event, uint32 data)
{
    int         rv;
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;
#ifdef BCM_ESW_SUPPORT
    _bcm_port_info_t            *pinfo;
    int sys_speed, spacing_value = 0;
#endif

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_notify: "
                            "unit %d, port %d, event %d, data %u\n"), 
                 unit, port, event, data));

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);



    /* check events target to the internal serdes in all conditions */
    switch(event) {
        case phyEventTxFifoReset:
        case phyEventMacLoopback:
        case phyEventLpiBypass:
        case phyEventTxSquelch:
            if (NULL == int_pc) {
                return SOC_E_INIT;
            }            
            rv = (PHY_NOTIFY(int_pc->pd, unit, port, event, data));
            break;
        case phyEventSpeedLine:
#ifdef BCM_ESW_SUPPORT
#ifdef PORTMOD_SUPPORT
        if (!SOC_PORT_USE_PORTCTRL(unit, port)) {
#endif
            SOC_IF_ERROR_RETURN(PHY_SPEED_GET(int_pc->pd, unit, port, &sys_speed));
            if ((data == 2500) && (sys_speed == 10000)) {
                spacing_value = MAC_WAN_MODE_THROTTLE_10G_TO_2P5G;
            } else if ((data == 5000) && (sys_speed == 10000)) {
                spacing_value = MAC_WAN_MODE_THROTTLE_10G_TO_5G;
            }
            SOC_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            return (MAC_CONTROL_SET(pinfo->p_mac, unit, port,
                              SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                              spacing_value));
#ifdef PORTMOD_SUPPORT
        }/* if portmod feature enabled */
#endif
#endif /* BCM_ESW_SUPPORT */
            break;
        default:
            break;

    }

    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    rv     = SOC_E_NONE;

    if ((NULL != ext_pc) && (NULL != int_pc)) {
        if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
            rv = (PHY_NOTIFY(int_pc->pd, unit, port, event, data));

            PHYCTRL_VERBOSE(("u=%d p=%d event=%s data=0x%08x\n",
                             unit, port, soc_phyctrl_event_string(event), data));
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_notify failed %d\n", rv));
    }
 
    return rv;
}

int
soc_cmic_rate_param_get(int unit, int *dividend, int *divisor)
{
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_cmic_rate_param_get: "
                            "unit %d\n"), unit));
    
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            return (soc_trident_cmic_rate_param_get(unit, dividend, divisor));
        } else
#endif /* BCM_TRIDENT_SUPPORT */
        {
            return SOC_E_UNAVAIL;
        }
}

int
soc_cmic_rate_param_set(int unit, int dividend, int divisor)
{
    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_cmic_rate_param_set: "
                            "unit %d, dividend %d, divisor %d\n"), unit, dividend, divisor));
    
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            return (soc_trident_cmic_rate_param_set(unit, dividend, divisor));
        } else
#endif /* BCM_TRIDENT_SUPPORT */
        {
            return SOC_E_UNAVAIL;
        }
}

STATIC void 
_soc_phyctrl_dump(phy_ctrl_t *pc)
{
    static char * if_string[] = {"NOCXN", "NULL",
                                 "MII", "GMII",
                                 "SGMII", "TBI",
                                 "XGMII", "RGMII",
                                 "RvMII", "1000X"};

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META("entered soc_phyctrl_dump\n")));
                                                                      
                                 
    LOG_CLI((BSL_META("%s\n"), pc->pd->drv_name));
    LOG_CLI((BSL_META("port         %d\n"), pc->port));
    LOG_CLI((BSL_META("phy_id0      0x%04x\n"), pc->phy_id0));
    LOG_CLI((BSL_META("phy_id1      0x%04x\n"), pc->phy_id1));
    LOG_CLI((BSL_META("phy_model    0x%04x\n"), pc->phy_model));
    LOG_CLI((BSL_META("phy_rev      0x%04x\n"), pc->phy_rev));
    LOG_CLI((BSL_META("phy_oui      0x%04x\n"), pc->phy_oui));
    LOG_CLI((BSL_META("phy_id       0x%02x\n"), pc->phy_id));
    LOG_CLI((BSL_META("ledmode      0x%02x, 0x%02x, 0x%02x, 0x%02x\n"), 
pc->ledmode[0], pc->ledmode[1],
             pc->ledmode[2], pc->ledmode[3]));
    LOG_CLI((BSL_META("ledctrl      0x%04x\n"), pc->ledctrl));
    LOG_CLI((BSL_META("ledselect    0x%04x\n"), pc->ledselect));
    LOG_CLI((BSL_META("automedium   %s\n"), pc->automedium ? "Y" : "N"));
    LOG_CLI((BSL_META("tbi_capable  %s\n"), pc->tbi_capable ? "Y" : "N"));
    LOG_CLI((BSL_META("medium       %x\n"), pc->medium));
    LOG_CLI((BSL_META("fiber_detect %d\n"), pc->fiber_detect));
    LOG_CLI((BSL_META("halfout      %d\n"), pc->halfout));
    LOG_CLI((BSL_META("interface    %s\n"), if_string[pc->interface]));
}

STATIC void
_soc_phyinfo_dump(int unit, soc_port_t port) 
{
    LOG_CLI((BSL_META_U(unit,
                        "phy_id0 0x%04x\n"), PHY_ID0_REG(unit, port)));
    LOG_CLI((BSL_META_U(unit,
                        "phy_id1 0x%04x\n"), PHY_ID1_REG(unit, port)));
    LOG_CLI((BSL_META_U(unit,
                        "phy_addr 0x%02x\n"), PHY_ADDR(unit, port)));
    LOG_CLI((BSL_META_U(unit,
                        "phy_addr_int 0x%02x\n"), PHY_ADDR_INT(unit, port)));
    LOG_CLI((BSL_META_U(unit,
                        "phy_name %s\n"), PHY_NAME(unit, port)));
    LOG_CLI((BSL_META_U(unit,
                        "phy_flags %s%s%s%s%s%s\n"),
             PHY_COPPER_MODE(unit, port) ? "COPPER\t" : "",
             PHY_FIBER_MODE(unit, port) ? "FIBER\t" : "",
             PHY_PASSTHRU_MODE(unit, port) ? "PASSTHRU\t" : "",
             PHY_TBI_MODE(unit, port) ? "TBI\t" : "",
             PHY_FIBER_100FX_MODE(unit, port) ? "100FX\t" : "",
             PHY_SGMII_AUTONEG_MODE(unit, port) ? "SGMII_AN\t" : ""));
    LOG_CLI((BSL_META_U(unit,
                        "phy_flags %s%s%s%s%s%s\n"), 
             PHY_WAN_MODE(unit, port) ? "WAN\t" : "",
             PHY_EXTERNAL_MODE(unit, port) ? "EXTERNAL\t" : "",
             PHY_MEDIUM_CHANGED(unit, port) ? "MEDIUM_CHANGED\t" : "",
             PHY_SERDES_FIBER_MODE(unit, port) ? "SERDES_FIBER\t" : "",
             PHY_FORCED_SGMII_MODE(unit, port) ? "FORCED_SGMII\t" : "",
             PHY_FORCED_COPPER_MODE(unit, port) ? "FORCED_COPPER\t" : ""));
    LOG_CLI((BSL_META_U(unit,
                        "phy_flags %s%s%s\n"), 
             PHY_CLAUSE45_MODE(unit, port) ? "C45\t" : "",
             PHY_DISABLED_MODE(unit, port) ? "DISABLED" : "",
             PHY_EEE_CAPABLE(unit, port) ? "EEE Capable" : "Not EEE Capable"));
    LOG_CLI((BSL_META_U(unit,
                        "an_timeout %d\n"), PHY_AN_TIMEOUT(unit, port)));
}

void
soc_phyctrl_port_dump(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    LOG_VERBOSE(BSL_LS_SOC_PHY,
                (BSL_META_U(unit,
                            "entered soc_phyctrl_port_dump: "
                            "unit %d, port %d\n"), unit, port));
    
    if (NULL == phy_port_info[unit]) {
        LOG_CLI((BSL_META_U(unit,
                            "----------------------\n")));
        LOG_CLI((BSL_META_U(unit,
                            "PHY SW not initialized\n")));
        LOG_CLI((BSL_META_U(unit,
                            "----------------------\n")));
    } else {
        _soc_phyinfo_dump(unit, port);
        pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != pc) {
            LOG_CLI((BSL_META_U(unit,
                                "--------------------\n")));
            LOG_CLI((BSL_META_U(unit,
                                "Internal PHY Control\n")));
            LOG_CLI((BSL_META_U(unit,
                                "--------------------\n")));
            _soc_phyctrl_dump(pc);
        }
    
        pc = EXT_PHY_SW_STATE(unit, port);
        if (NULL != pc) {
            LOG_CLI((BSL_META_U(unit,
                                "--------------------\n")));
            LOG_CLI((BSL_META_U(unit,
                                "External PHY Control\n")));
            LOG_CLI((BSL_META_U(unit,
                                "--------------------\n")));
            _soc_phyctrl_dump(pc);
        }
    }
} 


/*
 * Function:
 *     soc_phyctrl_fec_error_inject_set
 * Purpose:
 *     Set one time fec error injection
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port.
 *      error_control_map - (IN) Control lanemap/80-bit block map.
 *      bit_error_mask - (IN) Bit mask to generate errors
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */

int
soc_phyctrl_fec_error_inject_set(int unit, soc_port_t port,
                                 uint16 error_control_map,
                                 soc_phy_control_fec_error_mask_t bit_error_mask)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_driver_t *pd = NULL;

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (NULL == int_pc) {
        return SOC_E_INIT;
    } else {
        pd = int_pc->pd;
    }

    INT_MCU_LOCK(unit);

    rv = (PHY_FEC_ERROR_INJECT_SET(pd, unit, port, error_control_map, bit_error_mask));

    INT_MCU_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *     soc_phyctrl_fec_error_inject_get
 * Purpose:
 *     get configuration of one time fec error injection
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port.
 *      error_control_map - (OUT) Control lanemap/80-bit block map.
 *      bit_error_mask - (OUT) Bit mask to generate errors
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
int
soc_phyctrl_fec_error_inject_get(int unit, soc_port_t port,
                                 uint16* error_control_map,
                                 soc_phy_control_fec_error_mask_t* bit_error_mask)

{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_driver_t *pd = NULL;

    SOC_NULL_PARAM_CHECK(error_control_map);
    SOC_NULL_PARAM_CHECK(bit_error_mask);

    int_pc = INT_PHY_SW_STATE(unit, port);
    if (NULL == int_pc) {
        return SOC_E_INIT;
    } else {
        pd = int_pc->pd;
    }

    INT_MCU_LOCK(unit);

    rv = (PHY_FEC_ERROR_INJECT_GET(pd, unit, port, error_control_map, bit_error_mask));

    INT_MCU_UNLOCK(unit);

    if (SOC_FAILURE(rv)) {
        PHYCTRL_WARN(("soc_phyctrl_control_get failed %d\n", rv));
    }
    return rv;
}    
