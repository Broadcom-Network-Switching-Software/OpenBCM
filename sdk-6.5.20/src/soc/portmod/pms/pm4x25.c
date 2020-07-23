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
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_system.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM4X25_SUPPORT

#include <soc/portmod/clmac.h>
#include <soc/portmod/pm4x25.h>
#include <soc/portmod/pm4x25_shared.h>

#ifdef PORTMOD_PM4X25TD_SUPPORT
#include <soc/portmod/pm4x25td_int.h>
#endif /* PORTMOD_PM4X25TD_SUPPORT */


#define PM_4x25_INFO(pm_info) ((pm_info)->pm_data.pm4x25_db)

/* WB defines */

/*
 *  MACROs are in pm4x25_shared.h file.
 */

#define PM4x25_WB_BUFFER_VERSION        (15)

#define PM4X25_FS_ABILITY_TABLE_SIZE 10

/*
 * Entries of the force speed ability table; each entry specifies a
 * unique FS port speed ability and its associated VCO rate
 */
typedef struct pm4x25_fs_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    portmod_vco_type_t vco; /* associated VCO rate of the ability */
} pm4x25_fs_ability_table_entry_t;


/* a comprehensive list of pm8x50 forced speed abilities and their VCO rates */
const pm4x25_fs_ability_table_entry_t pm4x25_fs_ability_table[PM4X25_FS_ABILITY_TABLE_SIZE] =
{
    /* port_speed, num_lanes, vco */
    {1000 ,  1, portmodVCO25P781G},
    {2500 ,  1, portmodVCO25P781G},
    {5000 ,  1, portmodVCO20P625G},
    {10000,  1, portmodVCO25P781G},
    {12000,  1, portmodVCO25G    },
    {40000,  2, portmodVCO20P625G},
    {40000,  4, portmodVCO20P625G},
    {25000,  1, portmodVCO25P781G},
    {50000,  2, portmodVCO25P781G},
    {100000, 4, portmodVCO25P781G},
};

#define PM4x25_QUAD_MODE_IF(interface) \
    (interface == SOC_PORT_IF_CAUI || interface == SOC_PORT_IF_XLAUI)

#define PM4x25_DUAL_MODE_IF(interface) \
    (interface == SOC_PORT_IF_XLAUI2)

#define PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc) \
    do { \
        uint32 is_bypass; \
        phy_acc = 0; \
        PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypass); \
        phy_acc = (is_bypass && PM_4x25_INFO(pm_info)->first_phy != -1) ? (PM_4x25_INFO(pm_info)->first_phy | SOC_REG_ADDR_PHY_ACC_MASK) : port ; \
    } while (0)

/* Gen1 and Gen2  */
#define PORTMOD_PM4x25_IS_GEN1(pm_version)             ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN1))
#define PORTMOD_PM4x25_IS_GEN1_GEN2(pm_version)        ((pm_version.type == PM_TYPE_PM4X25) && ((pm_version.gen == PM_GEN1) || (pm_version.gen == PM_GEN2)))
#define PORTMOD_PM4x25_IS_GEN2_16nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN2) && (pm_version.tech_process == PM_TECH_16NM))
#define PORTMOD_PM4x25_IS_GEN2_28nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN2) && (pm_version.tech_process == PM_TECH_28NM))

/* Gen3  */
#define PORTMOD_PM4x25_IS_GEN3(pm_version)             ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN3))
#define PORTMOD_PM4x25_IS_GEN3_16nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN3) && (pm_version.tech_process == PM_TECH_16NM))
#define PORTMOD_PM4x25_IS_GEN3_28nm(pm_version)        ((pm_version.type == PM_TYPE_PM4X25) && (pm_version.gen == PM_GEN3) && (pm_version.tech_process == PM_TECH_28NM))
#define PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)       ((pm_version.type == PM_TYPE_CPM4X25) && (pm_version.gen == PM_GEN3) && (pm_version.tech_process == PM_TECH_28NM))

#define PORTMOD_PM4x25_REVISION_IS_A0(pm_version)      ((pm_version.revision_letter == PM_REVISION_A) && (pm_version.revision_number == PM_REVISION_0))
/* Add other revision check here */

#define TXPI_DSM_VALUE_1PPM_HI (uint32) 0x29   /* 1ppm value is 1.80144E+11 (0x29F16B11C7) in register */
#define TXPI_DSM_VALUE_1PPM_LO (uint32) 0xF16B11C7
#define PFC_FRAME_DETECTED_RSV_MASK     0x40000

#define DEFAULT_OSR2P5_SET 1

#define PM4X25_EGR_1588_TIMESTAMP_MODE_32BIT (0)
#define PM4X25_EGR_1588_TIMESTAMP_MODE_48BIT (1)

STATIC
int pm4x25_port_soft_reset(int unit, int port, pm_info_t pm_info, int enable);

STATIC int
_pm4x25_pm_ext_tsc_clk_set(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info);

STATIC int
_pm4x25_init_plls_get(int unit, int port, pm_info_t pm_info, portmod_vcos_speed_config_t *speed_config, int size,  phymod_dual_plls_t *init_plls);

STATIC int
_pm4x25_port_tsc_is_master_pll(int unit, int port, pm_info_t pm_info);

STATIC int
_pm4x25_port_speed_config_to_vco_get(int unit, const portmod_speed_config_t* speed_config,
                                     int speed_for_pcs_bypass_port,
                                     portmod_vco_type_t* vco);

#ifdef PORTMOD_CPM4X25_SUPPORT
STATIC int _pm4x25_speed_to_vco(int unit, int speed, int num_lanes, int serdes_1000x_at_25g_vco, int serdes_10g_at_25g_vco, int *vco, uint32_t *pll_mul);
#endif


STATIC
int _pm4x25_pm_version_get(int unit, pm_info_t pm_info, pm_version_t *version)
{
    uint32 reg_val, pm_type;
    int phy_port, phy_acc;

    SOC_INIT_FUNC_DEFS;

    version->type            = PM_TYPE_INVALID;
    version->gen             = PM_GEN_INVALID;
    version->tech_process    = PM_TECH_INVALID;
    version->revision_number = PM_REVISION_NUM_INVALID;
    version->revision_letter = PM_REVISION_LETTER_INVALID;

    /*
     * PM version ID register is not defined in PM GEN1.
     * PM12x10 only has gen1 now. To be fixed if support PM12X10 gen2.
     */
    if (SOC_REG_IS_VALID(unit, CLPORT_PM_VERSION_IDr) &&
        (!PM_4x25_INFO(pm_info)->in_pm12x10)) {

        /* Get the first physical port of the pm core */
        PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_port) {
            break;
        }

        phy_acc = phy_port | SOC_REG_ADDR_PHY_ACC_MASK;
        _SOC_IF_ERR_EXIT(READ_CLPORT_PM_VERSION_IDr(unit, phy_acc, &reg_val));

        pm_type  = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val, PM_TYPEf);
        if (pm_type == 1) {
            version->type = PM_TYPE_PM4X25;
            version->gen  = PM_GEN2;
        }
        else if (pm_type == 6) {
            version->type = PM_TYPE_PM4X25;
            version->gen  = PM_GEN3;
        }
        else if (pm_type == 7) {
            version->type = PM_TYPE_CPM4X25;
            version->gen  = PM_GEN3;
        }
        /* The hardware value of techology/revision starts from 0 */
        version->tech_process    = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val, TECH_PROCESSf) + 1;
        version->revision_number = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val, REV_NUMBERf) + 1;
        version->revision_letter = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val, REV_LETTERf) + 1;
    } else {
        version->type = PM_TYPE_PM4X25;
        version->gen  = PM_GEN1;
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_pm_interface_type_is_supported(int unit, soc_port_if_t interface,
                                          int* is_supported)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface){
        case SOC_PORT_IF_CAUI:
        case SOC_PORT_IF_XLAUI2:
        case SOC_PORT_IF_XLAUI:
        case SOC_PORT_IF_XFI:
        case SOC_PORT_IF_SFI:
        case SOC_PORT_IF_RXAUI:
        case SOC_PORT_IF_DNX_XAUI:
        case SOC_PORT_IF_SR:
        case SOC_PORT_IF_SR2:
        case SOC_PORT_IF_SR4:
        case SOC_PORT_IF_KR:
        case SOC_PORT_IF_KR2:
        case SOC_PORT_IF_KR4:
        case SOC_PORT_IF_CR:
        case SOC_PORT_IF_CR2:
        case SOC_PORT_IF_CR4:
        case SOC_PORT_IF_SGMII:
        case SOC_PORT_IF_GMII:
        case SOC_PORT_IF_CAUI4:
            *is_supported = TRUE;
            break;
        default:
            *is_supported = FALSE;
            break;
    }
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_port_index_get(int unit, int port, pm_info_t pm_info,
                           int *first_index, uint32 *bitmap)
{
   int i, rv = 0, tmp_port = 0;

   SOC_INIT_FUNC_DEFS;

   *first_index = -1;
   *bitmap = 0;

   for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
       rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
       _SOC_IF_ERR_EXIT(rv);

       if(tmp_port == port){
           *first_index = (*first_index == -1 ? i : *first_index);
           SHR_BITSET(bitmap, i);
       }
   }

   if(*first_index == -1) {
       _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
              (_SOC_MSG("port was not found in internal DB %d"), port));
   }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      portmod_pm4x25_wb_upgrade_func
 * Purpose:
 *      This function will take care of the warmboot variable manipulation
 *      in case of upgrade case, when variable definition got change or
 *      unspoorted varaible from previous version.
 * Parameters:
 *      unit                -(IN) Device unit number .
 *      arg                 -(IN) Generic pointer for a specific module to be used.
 *      recovered_version   -(IN) Warmboot version of the existing data.
 *      new_version         -(IN) Warmboot version of new data.
 * Returns:
 *      Status
 */

int portmod_pm4x25_wb_upgrade_func(int unit, void *arg, int recovered_version, int new_version)
{
    pm_info_t pm_info;
    int i, adjust_index;
    int nof_phys, dynamic_state, pll_divider_req, serdes_1000x_at_25g_vco, serdes_10g_at_25g_vco;
    portmod_port_ts_adjust_t port_ts_adjust;
    soc_port_if_t interface;
    uint32 vsd_active_group[4]={0};
    portmod_link_recovery_t link_recovery;
#ifdef PORTMOD_CPM4X25_SUPPORT
    int pll_active_lane_bitmap;
    int pll_vco_rate;
#endif
    SOC_INIT_FUNC_DEFS;
    
    pm_info = arg;

    /*
     * skip the warmboot db restore, it is only needed for
     * upgrades for version below 7.
     */

    if (recovered_version <= 6) {
        PM_4x25_INFO(pm_info)->warmboot_skip_db_restore = FALSE;
    } else {
        PM_4x25_INFO(pm_info)->warmboot_skip_db_restore = TRUE;
    }

    if ((recovered_version <= 5) &&
        (new_version >= 6)) {

        /*
         * in version 4 or earlier, nof_phys was not populated properly.
         * It need to initialize to 1  to count for internal phy. 1 may not
         * be proper value as some port may have ext phy.
         */
        if(!PM_4x25_INFO(pm_info)->in_pm12x10){
        nof_phys = 1;
        } else {
            nof_phys = 0;
        }
        for(i = 0 ; i < PM4X25_LANES_PER_CORE ; i++){
            _SOC_IF_ERR_EXIT(PM4x25_PORT_NUM_PHYS_SET(unit, pm_info, nof_phys, i));
            PM_4x25_INFO(pm_info)->nof_phys[i] = nof_phys;
        }
    }

    if ((recovered_version <= 6) &&
        (new_version >= 7)) {
        /* interface_written_into_outmost_phy does not exist in version 6 or earlier */
        interface = SOC_PORT_IF_COUNT;
        for (i = 0 ; i < MAX_PORTS_PER_PM4X25 ; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, interface, i));
        }
    }

    if ((recovered_version <= 7) &&
        (new_version >= 8)) {
        dynamic_state = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info, dynamic_state, i));
        }
    }

    if ((recovered_version <= 8) &&
        (new_version >= 9)) {
        sal_memset(&port_ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
             for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM_OLD; adjust_index++) {
                  _SOC_IF_ERR_EXIT(
                      PM4x25_PORT_TIME_STAMP_ADJUST_OLD_SET(unit, pm_info, &port_ts_adjust,
                                                            adjust_index, i));
             }
        }
    }

    if ((recovered_version <= 9) &&
        (new_version >= 10)) {
        pll_divider_req = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_PORT_PLL_DIVIDER_REQ_SET(unit, pm_info, pll_divider_req, i));
        }
    }

    if ((recovered_version <= 10) &&
        (new_version >= 11)) {
        serdes_1000x_at_25g_vco = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_PORT_1000X_AT_25G_SET(unit, pm_info, serdes_1000x_at_25g_vco, i));
        }

        serdes_10g_at_25g_vco = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_PORT_10G_AT_25G_SET(unit, pm_info, serdes_10g_at_25g_vco, i));
        }
    }
    if ((recovered_version <= 11) &&
        (new_version >= 12)) {
        sal_memset(vsd_active_group, 0, sizeof(uint32_t)*NUM_VSD_ACTIVE_GROUP_WORD);
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_SET(unit, pm_info, vsd_active_group, i));
        }
    }

    if ((recovered_version <= 11) &&
        (new_version >= 12)) {
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            /* Copy existing timestamp adjust configs from old WB variable. */
            for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM_OLD; adjust_index++) {
                _SOC_IF_ERR_EXIT(
                    PM4x25_PORT_TIME_STAMP_ADJUST_OLD_GET(unit, pm_info, &port_ts_adjust,
                                                          adjust_index, i));
                _SOC_IF_ERR_EXIT(
                    PM4x25_PORT_TIME_STAMP_ADJUST_SET(unit, pm_info, &port_ts_adjust,
                                                      adjust_index, i));
            }
            /* Initialize new timestamp adjust configs as 0. */
            while (adjust_index < PORTMOD_TS_ADJUST_NUM) {
                sal_memset(&port_ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));
                _SOC_IF_ERR_EXIT(
                    PM4x25_PORT_TIME_STAMP_ADJUST_SET(unit, pm_info, &port_ts_adjust,
                                                      adjust_index, i));
                adjust_index++;
            }
        }
    }

    if ((recovered_version <= 12) &&
        (new_version >= 13)) {
        link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK;
        link_recovery.enabled = TRUE;
        link_recovery.tick_cnt = 0;
        for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
            _SOC_IF_ERR_EXIT(
                PM4x25_PORT_LINK_RECOVERY_SET(unit, pm_info, link_recovery, i));
        }
    }

#ifdef PORTMOD_CPM4X25_SUPPORT
    if (pm_info->type == portmodDispatchTypeCpm4x25) {
        if ((recovered_version <= 14) &&
            (new_version >= 15)) {
            pll_active_lane_bitmap = 0;
            _SOC_IF_ERR_EXIT(PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_active_lane_bitmap));
            _SOC_IF_ERR_EXIT(PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_active_lane_bitmap));

            pll_vco_rate = -1;
            _SOC_IF_ERR_EXIT(PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, pll_vco_rate));
            _SOC_IF_ERR_EXIT(PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, pll_vco_rate));
        }
    }
#endif

    if ((recovered_version <= 14) &&
        (new_version >= 15)) {
        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[userTimeAdjust], 0));

        _SOC_IF_ERR_EXIT(SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[ptpTimeAdjust], 0));
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_pm_wb_debug_log (int unit, pm_info_t pm_info)
{
    uint32 pm_is_active = 0, core_is_initialized = 0;
    uint32 pm_is_bypassed = 0;
    uint32 group_list[NUM_VSD_ACTIVE_GROUP_WORD];
    uint32 interface_modes;
    uint32 an_master_lane;
    uint32 nof_phys, port_dynamic_state;
    uint32 user_time_adjust, ptp_time_adjust;
    int three_ports_mode, phyn, lane;
    int rv = 0;
    int i, j, tmp_port;
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    int vco_rate, max_speed, interface, interface_type;
    int is_interface_restore, is_hg, fs_cl72, cx4_10G, an_cl72, an_fec ;
    int adjust_index;
    int pll_divider_req;
    int serdes_10g_at_25g_vco = 0;
    int serdes_1000x_at_25g_vco = 0;
    phymod_an_mode_type_t an_mode;
    portmod_port_ts_adjust_t port_ts_adjust;
    portmod_link_recovery_t link_recovery;
    portmod_xphy_lane_connection_t db_lane_conn;


    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{wb_buffer_index}\n")));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "{%d}\n"), pm_info->wb_buffer_id));
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{wb_buffer_index}\n")));


    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{is_active}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), pm_is_active));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{is_active}\n")));
    }

    rv = PM4x25_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{is_initialized}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), core_is_initialized));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{is_initialized}\n")));
    }

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{is_bypassed}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), pm_is_bypassed));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{is_bypassed}\n")));
    }

    rv = PM4x25_THREE_PORTS_MODE_GET(unit, pm_info, &three_ports_mode);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{three_ports_mode}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), three_ports_mode));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{three_ports_mode}\n")));
    }

    rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{pll0_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), pll0_active_lane_bitmap));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{pll0_active_lane_bitmap}\n")));
    }

    rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{pll1_active_lane_bitmap}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{0x%x}\n"), pll1_active_lane_bitmap));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{pll1_active_lane_bitmap}\n")));
    }


    rv = PM4x25_PLL0_VCO_RATE_GET(unit, pm_info, &vco_rate);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{pll0_vco_rate}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), vco_rate));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{pll0_vco_rate}\n")));
    }

    rv = PM4x25_PLL1_VCO_RATE_GET(unit, pm_info, &vco_rate);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{pll1_vco_rate}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), vco_rate));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{pll1_vco_rate}\n")));
    }

    rv = PM4x25_VCO_GET(unit, pm_info, &vco_rate);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$BEGIN:portmod_pm4x25:{pll_vco_rate}\n")));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "{%d}\n"), vco_rate));
        LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                        "S$END:portmod_pm4x25:{pll_vco_rate}\n")));
    }

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{lane,lane2portmap}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {

        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, tmp_port));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{lane,lane2portmap}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,max_speed}\n")));

    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {

        rv = PM4x25_MAX_SPEED_GET(unit, pm_info, &max_speed, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, max_speed));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,max_speed}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,interface_type}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_INTERFACE_TYPE_GET(unit, pm_info, &interface_type, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, interface_type));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,interface_type}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,interface_modes}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,0x%x}\n"), i, interface_modes));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,interface_modes}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,interface_written_into_outmost_phy}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_OUTMOST_PHY_INTERFACE_TYPE_GET(unit, pm_info, &interface, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, interface));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,interface_written_into_outmost_phy}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,is_interface_restore}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_IS_INTERFACE_RESTORE_GET(unit, pm_info, &is_interface_restore, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_interface_restore));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,is_interface_restore}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,is_hg}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_IS_HG_GET(unit, pm_info, &is_hg, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, is_hg));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,is_hg}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,an_mode}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_AN_MODE_GET(unit, pm_info, &an_mode, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_mode));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,an_mode}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,fs_cl72}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_FS_CL72_GET(unit, pm_info, &fs_cl72, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, fs_cl72));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,fs_cl72}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,cx4_10G}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_CX4_10G_GET(unit, pm_info, &cx4_10G, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, cx4_10G));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,cx4_10G}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,an_cl72}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_AN_CL72_GET(unit, pm_info, &an_cl72, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_cl72));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,an_cl72}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,an_fec}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_AN_FEC_GET(unit, pm_info, &an_fec, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_fec));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,an_fec}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,an_master_lane}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_AN_MASTER_LANE_GET(unit, pm_info, &an_master_lane, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, an_master_lane));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,an_master_lane}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,nof_phys}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_NUM_PHYS_GET(unit, pm_info, &nof_phys, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, nof_phys));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,nof_phys}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,port_dynamic_state}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, &port_dynamic_state, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, port_dynamic_state));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,port_dynamic_state}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,time_stamp_adjust_index,speed,osts_adjust,tsts_adjust}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM_OLD; adjust_index++) {
            rv = PM4x25_PORT_TIME_STAMP_ADJUST_OLD_GET(unit, pm_info, &port_ts_adjust, adjust_index, i);
            if (SOC_E_NONE == rv) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "{%d,%d,%d,%d,%d}\n"),
                                i, adjust_index, port_ts_adjust.speed,
                                port_ts_adjust.osts_adjust, port_ts_adjust.tsts_adjust));
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,time_stamp_adjust_index,speed,osts_adjust,tsts_adjust}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,new_time_stamp_adjust_index,speed,osts_adjust,tsts_adjust}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM; adjust_index++) {
            rv = PM4x25_PORT_TIME_STAMP_ADJUST_GET(unit, pm_info, &port_ts_adjust, adjust_index, i);
            if (SOC_E_NONE == rv) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "{%d,%d,%d,%d,%d}\n"),
                                i, adjust_index, port_ts_adjust.speed, port_ts_adjust.osts_adjust,
                                port_ts_adjust.tsts_adjust));
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,new_time_stamp_adjust_index,speed,osts_adjust,tsts_adjust}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,pll_divider_req}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_PLL_DIVIDER_REQ_GET(unit, pm_info, &pll_divider_req, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, pll_divider_req));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,pll_divider_req}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,serdes_1000x_at_25g_vco}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_1000X_AT_25G_GET(unit, pm_info, &serdes_1000x_at_25g_vco, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, serdes_1000x_at_25g_vco));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,serdes_1000x_at_25g_vco}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,serdes_10g_at_25g_vco}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_10G_AT_25G_GET(unit, pm_info, &serdes_10g_at_25g_vco, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, serdes_10g_at_25g_vco));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,serdes_10g_at_25g_vco}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,cpri_tx_vsd_active_grp[]}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_GET(unit, pm_info, group_list, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,{"), i));
            for (j = 0; j < NUM_VSD_ACTIVE_GROUP_WORD; j++) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "%d,"), group_list[j]));
            }
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "}\n")));

        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,cpri_tx_vsd_active_grp[]}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{port,link_recovery_state,enabled,tick_cnt}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PORT_LINK_RECOVERY_GET(unit, pm_info, &link_recovery, i);

        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d,%d,%d}\n"), i, link_recovery.state,
                            link_recovery.enabled, link_recovery.tick_cnt));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{port,link_recovery_state,enabled,tick_cnt}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{lane_idx,user_time_adjust}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_USER_TIME_ADJUST_GET(unit, pm_info, &user_time_adjust, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, user_time_adjust));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{lane_idx,user_time_adjust}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{lane_idx,ptp_time_adjust}\n")));
    for (i = 0 ; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_PTP_TIME_ADJUST_GET(unit, pm_info, &ptp_time_adjust, i);
        if (SOC_E_NONE == rv) {
            LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                            "{%d,%d}\n"), i, ptp_time_adjust));
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{lane_idx,ptp_time_adjust}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$BEGIN:portmod_pm4x25:{phyn,lane,lane_connection_db.xphy_id,"
                                             "ss_lane_mask, ls_lane_mask,"
                                             "rsvd1,rsvd2}\n")));
    for (phyn = 0 ; phyn < MAX_PHYN ; phyn++) {
        for (lane = 0 ; lane < PM4X25_LANES_PER_CORE ; lane++) {

            rv = PM4x25_PORT_LANE_CONNECTION_DB_GET(unit, pm_info, &db_lane_conn, phyn, lane);
            if (SOC_E_NONE == rv) {
                LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                                "{%d,%d,%d,0x%x,0x%x,%d,%d\n"), phyn, lane,
                                db_lane_conn.xphy_id, db_lane_conn.ss_lane_mask,
                                db_lane_conn.ls_lane_mask, db_lane_conn.rsvd1,
                                db_lane_conn.rsvd2));
            }
        }
    }
    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25:{phyn,lane,lane_connection_db.xphy_id,"
                                             "ss_lane_mask, ls_lane_mask,"
                                             "rsvd1,rsvd2}\n")));

    LOG_VERBOSE(BSL_LS_SHARED_SCACHE, (BSL_META_U(unit,
                    "S$END:portmod_pm4x25\n")));

    return SOC_E_NONE;
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in the variables like
 * isInitialized, isActive, isBypassed, ports.. etc.,
 * All of these variables need to be added to warmboot
 * any variables added to save the state of warmboot should be 
 * included here.  
 */
STATIC
int pm4x25_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pm4x25", portmod_pm4x25_wb_upgrade_func, pm_info, PM4x25_WB_BUFFER_VERSION, 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2portMap", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X25, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[lane2portMap] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_active", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isActive] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInitialized] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isBypassed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "three_ports_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[threePortsMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_PM4X25, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[phy_type] = wb_var_id;

    /* Deleting the below WB structures since most likely to be added in the future in a diffrent format */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_config", wb_buffer_index, sizeof(portmod_port_interface_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(3), VERSION(4), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceConfig] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "port_init_config", wb_buffer_index, sizeof(portmod_port_init_config_t), NULL, 1, 1, 0xffffffff, 0xffffffff, VERSION(3), VERSION(4), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portInitConfig] = wb_var_id;

    /* Keeping the below WB arr since most likely to be used in the future */
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "nof_phys", wb_buffer_index, sizeof(int), NULL, PM4X25_LANES_PER_CORE,  VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[nofPhys] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "max_speed", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM4X25, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[maxSpeed] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_type", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM4X25, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceType] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_modes", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM4X25, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceModes] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_interface_restore", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM4X25, VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isInterfaceRestore] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_hg", wb_buffer_index, sizeof(uint32), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isHg] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_mode", wb_buffer_index, sizeof(int), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMode] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "fs_cl72", wb_buffer_index, sizeof(uint32), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[fsCl72] = wb_var_id;

     _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "cx4_10G", wb_buffer_index, sizeof(uint32), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[cx4In10G] = wb_var_id;

     _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_cl72", wb_buffer_index, sizeof(uint32), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anCl72] = wb_var_id;

     _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_fec", wb_buffer_index, sizeof(uint32), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anFec] = wb_var_id;

     _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "an_master_lane", wb_buffer_index, sizeof(int), NULL, PM4X25_LANES_PER_CORE,  VERSION(5));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[anMasterLane] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane_connection_db",
                          wb_buffer_index, sizeof(portmod_xphy_lane_connection_t), NULL,
                          PM4X25_LANES_PER_CORE, MAX_PHYN, VERSION(6));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[laneConnectionDb] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "interface_written_into_outmost_phy",
                          wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_PM4X25, VERSION(7));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[interfaceWrittenIntoOutmostPhy] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "port_dynamic_state", wb_buffer_index,
                          sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(8));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[portDynamicState] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "time stamp adjust setting on MAC", wb_buffer_index,
                          sizeof(portmod_port_ts_adjust_t), NULL,
                          PORTMOD_TS_ADJUST_NUM_OLD, MAX_PORTS_PER_PM4X25,
                          VERSION(9));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tsAdjustOld] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "pll_divider_req", wb_buffer_index,
                          sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(10));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pllDividerReq] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "serdes_1000x_at_25g_vco", wb_buffer_index,
                          sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(11));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[serdes1000xAt25gVco] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "serdes_10g_at_25g_vco", wb_buffer_index,
                          sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(11));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[serdes10gAt25gVco] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "cpri_tx_vsd_active_grp", wb_buffer_index,
                          sizeof(uint32_t)*NUM_VSD_ACTIVE_GROUP_WORD, NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(12));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[cpriVsdActiveGrp] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_2D_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "new time stamp adjust setting on MAC", wb_buffer_index,
                          sizeof(portmod_port_ts_adjust_t), NULL,
                          PORTMOD_TS_ADJUST_NUM, MAX_PORTS_PER_PM4X25,
                          VERSION(12));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[tsAdjust] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id,
                          "link_recovery", wb_buffer_index,
                          sizeof(portmod_link_recovery_t), NULL, MAX_PORTS_PER_PM4X25,
                          VERSION(13));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[linkRecovery] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll_vco_rate", wb_buffer_index, sizeof(portmod_vco_type_t), NULL, VERSION(14));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[pll_vco_rate] = wb_var_id;

#ifdef PORTMOD_CPM4X25_SUPPORT    
    if (pm_info->type == portmodDispatchTypeCpm4x25) {
        _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
        SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0ActiveLaneBitmap", wb_buffer_index, sizeof(uint32_t), NULL, VERSION(15));
        _SOC_IF_ERR_EXIT(rv);
        pm_info->wb_vars_ids[pll0ActiveLaneBitmap] = wb_var_id;

        _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
        SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1ActiveLaneBitmap", wb_buffer_index, sizeof(uint32_t), NULL, VERSION(15));
        _SOC_IF_ERR_EXIT(rv);
        pm_info->wb_vars_ids[pll1ActiveLaneBitmap] = wb_var_id;

        _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
        SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll0vcoRate", wb_buffer_index, sizeof(int), NULL, VERSION(15));
        _SOC_IF_ERR_EXIT(rv);
        pm_info->wb_vars_ids[pll0vcoRate] = wb_var_id;

        _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
        SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "pll1vcoRate", wb_buffer_index, sizeof(int), NULL, VERSION(15));
        _SOC_IF_ERR_EXIT(rv);
        pm_info->wb_vars_ids[pll1vcoRate] = wb_var_id;
    }
#endif

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "userTimeAdjust", wb_buffer_index, sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25, VERSION(15));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[userTimeAdjust] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ptpTimeAdjust", wb_buffer_index, sizeof(uint32_t), NULL, MAX_PORTS_PER_PM4X25, VERSION(15));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ptpTimeAdjust] = wb_var_id;

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
    if(!SOC_WARM_BOOT(unit)){
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[lane2portMap], -1);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[maxSpeed], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceType], _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceModes], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isInterfaceRestore], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isHg], _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMode], phymod_AN_MODE_Count);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[fsCl72], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[cx4In10G], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anCl72], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anFec], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[anMasterLane], -1);
        _SOC_IF_ERR_EXIT(rv);
        
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[interfaceWrittenIntoOutmostPhy], _SHR_PORT_IF_COUNT);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                      pm_info->wb_vars_ids[portDynamicState], 0);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                                      pm_info->wb_vars_ids[pllDividerReq], 0);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[serdes1000xAt25gVco], 0);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[serdes10gAt25gVco], 0);

       rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[cpriVsdActiveGrp], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[linkRecovery], 0);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[userTimeAdjust], 0);
        _SOC_IF_ERR_EXIT(rv);

       rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
                              pm_info->wb_vars_ids[ptpTimeAdjust], 0);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC portmod_ucode_buf_t pm4x25_ucode_buf[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};
STATIC portmod_ucode_buf_t pm4x25_ucode_buf_2nd[SOC_MAX_NUM_DEVICES] = {{NULL, 0}};

int pm4x25_pm_destroy(int unit, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    if(pm_info->pm_data.pm4x25_db != NULL){
        sal_free(pm_info->pm_data.pm4x25_db);
        pm_info->pm_data.pm4x25_db = NULL;
    }

    /* free pm4x25_ucode_buf */
    if(pm4x25_ucode_buf[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm4x25_ucode_buf[unit].ucode_dma_buf);
        pm4x25_ucode_buf[unit].ucode_dma_buf = NULL;
    }
    pm4x25_ucode_buf[unit].ucode_alloc_size = 0;

    if(pm4x25_ucode_buf_2nd[unit].ucode_dma_buf != NULL){
        portmod_sys_dma_free(unit, pm4x25_ucode_buf_2nd[unit].ucode_dma_buf);
        pm4x25_ucode_buf_2nd[unit].ucode_dma_buf = NULL;
    }
    pm4x25_ucode_buf_2nd[unit].ucode_alloc_size = 0;

    SOC_FUNC_RETURN; 
}

int
pm4x25_default_fw_loader(const phymod_core_access_t* core, uint32_t length, const uint8_t* data)
{
    int rv, bcast_en = 0;
    portmod_default_user_access_t *user_data;
    int unit = ((portmod_default_user_access_t*)core->access.user_acc)->unit;
    portmod_ucode_buf_order_t load_order;
    SOC_INIT_FUNC_DEFS;

    user_data = (portmod_default_user_access_t*)core->access.user_acc;

    if (PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_GET(user_data)) {
        load_order = portmod_ucode_buf_order_reversed;
    } else {
        load_order = portmod_ucode_buf_order_straight;
    }

#ifdef FW_BCAST_DOWNLOAD
    if (PORTMOD_USER_ACCESS_FW_LOAD_BROADCAST_GET(user_data)) {
        bcast_en = 1;
    }
#endif

    rv = portmod_firmware_set(unit,
                              user_data->blk_id,
                              data,
                              length,
                              load_order,
                              bcast_en,
                              &(pm4x25_ucode_buf[unit]),
                              &(pm4x25_ucode_buf_2nd[unit]),
                              CLPORT_WC_UCMEM_DATAm,
                              CLPORT_WC_UCMEM_CTRLr);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int
pm4x25_default_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val)
{
    return portmod_common_phy_sbus_reg_write(CLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int
pm4x25_default_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t *val)
{
    return portmod_common_phy_sbus_reg_read(CLPORT_WC_UCMEM_DATAm, user_acc, core_addr, reg_addr, val);
}

STATIC
int _pm4x25_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx, int val)
{
    int phy_acc;
    uint32 reg_val;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));

    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT0f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT1f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT2f, val);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT3f, val);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN; 
}
int pm4x25_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{
    int phy_acc;
    uint32 reg_val, old_val;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    old_val = reg_val;

    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT0f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT1f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT2f, 1);
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, PORT3f, 1);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, old_val));

exit:
    SOC_FUNC_RETURN; 
}

phymod_bus_t pm4x25_default_bus = {
    "PM4x25 Bus",
    pm4x25_default_bus_read,
    pm4x25_default_bus_write,
    NULL,
    portmod_common_mutex_take,
    portmod_common_mutex_give,
    NULL,
    NULL,
    PHYMOD_BUS_CAP_WR_MODIFY | PHYMOD_BUS_CAP_LANE_CTRL
};

int pm4x25_pm_init(int unit,
                   const portmod_pm_create_info_internal_t* pm_add_info,
                   int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_pm4x25_create_info_t *info = &pm_add_info->pm_specific_info.pm4x25;
    pm4x25_t pm4x25_data = NULL;
    int i,j, pm_is_active, rv, three_ports_mode;
    int bypass_enable;
    int pm_is_initialized;
#ifdef PORTMOD_CPM4X25_SUPPORT
    int pll_active_lane_bitmap = 0;
    int pll_vco_rate = 0;
#endif
    portmod_xphy_lane_connection_t lane_conn;
    portmod_xphy_lane_connection_t *db_lane_conn;
    portmod_vco_type_t vco;
    int lane, phyn;
    SOC_INIT_FUNC_DEFS;

    pm_info->type = pm_add_info->type;
    pm_info->unit = unit;
    pm_info->wb_buffer_id = wb_buffer_index;

    /* PM4x25 specific info */
    pm4x25_data = sal_alloc(sizeof(struct pm4x25_s), "specific_db");
    SOC_NULL_CHECK(pm4x25_data);
    pm_info->pm_data.pm4x25_db = pm4x25_data;

    PORTMOD_PBMP_ASSIGN(pm4x25_data->phys, pm_add_info->phys);
    pm4x25_data->int_core_access.type = phymodDispatchTypeInvalid;
    pm4x25_data->portmod_mac_soft_reset = pm_add_info->pm_specific_info.pm4x25.portmod_mac_soft_reset;
    pm4x25_data->first_phy = -1;
    pm4x25_data->warmboot_skip_db_restore = TRUE;
    pm4x25_data->rescal = pm_add_info->pm_specific_info.pm4x25.rescal;

    for(i = 0 ; i < PM4X25_LANES_PER_CORE; i++){
        pm4x25_data->nof_phys[i] = 0;
    }

    /* init intertnal SerDes core access */
    phymod_core_access_t_init(&pm4x25_data->int_core_access);

    /* initialize lane connections */
    for(i = 0 ; i < MAX_PHYN ; i++){
        for(j = 0 ; j < PM4X25_LANES_PER_CORE ; j++){
            portmod_xphy_lane_connection_t_init(unit, &pm4x25_data->lane_conn[i][j]);
        }
    }

    if(!info->in_pm_12x10){
        sal_memcpy(&pm4x25_data->polarity, &info->polarity, sizeof(phymod_polarity_t));
        sal_memcpy(&(pm4x25_data->int_core_access.access), &info->access,
                    sizeof(phymod_access_t));

        sal_memcpy(&pm4x25_data->lane_map, &info->lane_map,
                    sizeof(pm4x25_data->lane_map));
        pm4x25_data->ref_clk = info->ref_clk;
        pm4x25_data->fw_load_method = info->fw_load_method;
        pm4x25_data->external_fw_loader = info->external_fw_loader;

        if(info->access.bus == NULL) {
            /* if null - use default */
            pm4x25_data->int_core_access.access.bus = &pm4x25_default_bus;
        }

        if(pm4x25_data->external_fw_loader == NULL) {
            /* if null - use default */
            pm4x25_data->external_fw_loader = pm4x25_default_fw_loader;
        }

        for(i = 0 ; i < PM4X25_LANES_PER_CORE; i++){
            pm4x25_data->nof_phys[i] = 1; /* Internal Phy */
        }
    }
    pm4x25_data->in_pm12x10 = info->in_pm_12x10;
    pm4x25_data->core_num   = info->core_num;
    pm4x25_data->afe_pll = info->afe_pll;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pm4x25_wb_buffer_init(unit, wb_buffer_index,  pm_info));

    if(SOC_WARM_BOOT(unit)){
         
        /* print scache information for debugging */
        pm4x25_pm_wb_debug_log(unit, pm_info);

        rv = SOC_WB_ENGINE_GET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                         pm_info->wb_vars_ids[phy_type], &pm4x25_data->int_core_access.type );
        _SOC_IF_ERR_EXIT(rv);

        /* restore nof_phys from wb */
        for(i = 0 ; i < PM4X25_LANES_PER_CORE ; i++){
            _SOC_IF_ERR_EXIT(PM4x25_PORT_NUM_PHYS_GET(unit, pm_info, &pm4x25_data->nof_phys[i], i));
        }

        for(phyn = 0 ; phyn < MAX_PHYN ; phyn++){
            for(lane = 0 ; lane < PM4X25_LANES_PER_CORE ; lane++){
                db_lane_conn = &(PM_4x25_INFO(pm_info)->lane_conn[phyn][lane]);
                _SOC_IF_ERR_EXIT(PM4x25_PORT_LANE_CONNECTION_DB_GET(unit, pm_info,
                                 db_lane_conn, phyn, lane));
            }
        }
    }

    if(!SOC_WARM_BOOT(unit)){
        pm_is_active = 0;
        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        pm_is_initialized = 0;
        rv = PM4x25_IS_INITIALIZED_SET(unit, pm_info, pm_is_initialized);
        _SOC_IF_ERR_EXIT(rv);

        bypass_enable = 0;
        rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        _SOC_IF_ERR_EXIT(rv);
#ifdef PORTMOD_CPM4X25_SUPPORT    
        if (pm_info->type == portmodDispatchTypeCpm4x25) {

            pll_active_lane_bitmap = 0;
            rv = PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_active_lane_bitmap);
            _SOC_IF_ERR_EXIT(rv);
            rv = PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll_active_lane_bitmap);
            _SOC_IF_ERR_EXIT(rv);

            pll_vco_rate = -1;
            rv = PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, pll_vco_rate);
            _SOC_IF_ERR_EXIT(rv);
            rv = PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, pll_vco_rate);
            _SOC_IF_ERR_EXIT(rv);
        }
#endif

        three_ports_mode = (info->port_mode_aux_info == portmodModeInfoThreePorts) ? 1 : 0;
        rv = PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode);
        _SOC_IF_ERR_EXIT(rv);

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
            pm_info->wb_vars_ids[phy_type],
            &PM_4x25_INFO(pm_info)->int_core_access.type, 0);
        _SOC_IF_ERR_EXIT(rv);

        for(i = 0 ; i < PM4X25_LANES_PER_CORE ; i++){
            rv = PM4x25_PORT_NUM_PHYS_SET(unit, pm_info, pm4x25_data->nof_phys[i], i);
            _SOC_IF_ERR_EXIT(rv);
        }

        vco = portmodVCOInvalid;
        rv = PM4x25_VCO_SET(unit, pm_info, vco);
        _SOC_IF_ERR_EXIT(rv);

        portmod_xphy_lane_connection_t_init(unit, &lane_conn);
        for(phyn = 0 ; phyn < MAX_PHYN ; phyn++){
            for(lane = 0 ; lane < PM4X25_LANES_PER_CORE ; lane++){
                _SOC_IF_ERR_EXIT(PM4x25_PORT_LANE_CONNECTION_DB_SET(unit, pm_info,
                                 &lane_conn, phyn, lane));
            }
        }
    }

exit:
    if(SOC_FUNC_ERROR){
        pm4x25_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

int pm4x25_port_tsc_reset_set(int unit, int phy_acc, int in_reset)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Bring Internal Phy OOR */
    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, in_reset ? 0 : 1);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, in_reset ? 1 : 0);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, in_reset ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

    /* Based on the feedback from SJ pmd support team, ~10-15usecs would be sufficient
     * for the PLL/AFE to settle down out of IDDQ reset.
     */
    sal_usleep(10);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_port_tsc_refclock_set(int unit, int phy_acc, int ref_in, int ref_out)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* choose refclk into this TSC PLL : 0 - PAD or 1- LCREF */
    /* enable ref_out: LCREFout which connects to other PM blocks */
    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, REFIN_ENf, ref_in ? 1 : 0);
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, REFOUT_ENf, ref_out ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_port_tsc_refclock_get(int unit, int phy_acc, int *ref_in, int *ref_out)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* choose refin : PAD or LCREF */
    /* enable ref_out: LCREFout which connects to other PM blocks */
    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    *ref_in  = soc_reg_field_get(unit, CLPORT_XGXS0_CTRL_REGr, reg_val, REFIN_ENf);
    *ref_out = soc_reg_field_get(unit, CLPORT_XGXS0_CTRL_REGr, reg_val, REFOUT_ENf);

exit:
    SOC_FUNC_RETURN;
}

/*
*  Set PM TSC refer clock source and enable refer clock output to other TSC.
*  ref_in: Selects between pad and LCREF as source of refclk into this TSC PLL. 0 - PAD or 1- LCREF
*  ref_out: Enables LCREFOUT to be driven by this TSC. 0 - disable, 1 - enable
*  Note: If enable LCREF_out to other PMs TSC, this PM TSC work as master PM.
*        This TSC should be reset at in level before this port attaching.
*/
int pm4x25_pm_tsc_refclock_set(int unit, int pm_id, pm_info_t pm_info, int ref_in, int ref_out)
{
    int phy_port, phy_acc;
    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_port) {
        break;
    }

    phy_acc = phy_port | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(_pm4x25_port_tsc_refclock_set(unit, phy_acc, ref_in, ref_out));

    /* Reset out master PMs */
    if (ref_out == 1)
    {
        _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));
        _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 0));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
*  Get PM TSC refer clock source and enable status of refer clock output to other TSC.
*  ref_in: 0 - PAD or 1- LCREF
*  ref_out: 1 - enable LCREF_out which connects to other PM blocks
*/
int pm4x25_pm_tsc_refclock_get(int unit, int pm_id, pm_info_t pm_info, int* ref_in, int* ref_out)
{
    int phy_port, phy_acc;
    SOC_INIT_FUNC_DEFS;

    /* Get the first physical port of the pm core */
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_port) {
        break;
    }

    phy_acc = phy_port | SOC_REG_ADDR_PHY_ACC_MASK;

    _SOC_IF_ERR_EXIT(_pm4x25_port_tsc_refclock_get(unit, phy_acc, ref_in, ref_out));

exit:
    SOC_FUNC_RETURN;
}

/*
 *  Check the PM is a master PLL or not
 *  For GEN1/GEN2, only need to check one PLL
 *  For GEN3, need to check PLL0 and PLL1
 */
STATIC
int _pm4x25_port_tsc_is_master_pll(int unit, int port, pm_info_t pm_info)
{
   int phy_acc;
   uint32 reg_val;
   uint32 refclk_out_en_0 = 0, refclk_out_en_1 = 0;
   pm_version_t pm_version;
   SOC_INIT_FUNC_DEFS;

   PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

   _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

   if (PORTMOD_PM4x25_IS_GEN1_GEN2(pm_version) || PORTMOD_PM4x25_IS_GEN3_16nm(pm_version)) {
        _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
        refclk_out_en_0 = soc_reg_field_get(unit, CLPORT_XGXS0_CTRL_REGr, reg_val, REFOUT_ENf);
    } else if (PORTMOD_PM4x25_IS_GEN3_28nm(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
        /* Valid only for G3 28nm PMD_28_FC, don't care for 16nm */
        _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
        refclk_out_en_0 = soc_reg_field_get(unit, CLPORT_XGXS0_CTRL_REGr, reg_val, PLL0_REFOUT_ENf);
        refclk_out_en_1 = soc_reg_field_get(unit, CLPORT_XGXS0_CTRL_REGr, reg_val, PLL1_REFOUT_ENf);
    } else {
        refclk_out_en_0 = 0;
        refclk_out_en_1 = 0;
    }

   if (refclk_out_en_0 || refclk_out_en_1) {
       return TRUE;
   }

   return FALSE;

exit:
    SOC_FUNC_RETURN;
}

static int _clport_mode_get(int unit, int phy_acc, int bindex, 
                      portmod_core_port_mode_t *core_mode, int *cur_lanes)
{
    uint32_t rval;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &rval));

    *core_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, rval, XPORT0_CORE_PORT_MODEf);

    switch (*core_mode) {
        case portmodPortModeQuad:
            *cur_lanes = 1;
            break;

        case portmodPortModeTri012: 
            *cur_lanes = bindex == 2 ? 2 : 1;
            break;

        case portmodPortModeTri023:
            *cur_lanes = bindex == 0 ? 2 : 1;
            break;

        case portmodPortModeDual: 
            *cur_lanes = 2;
            break;

        case portmodPortModeSingle:
            *cur_lanes = 4;
            break;

        default:
            return SOC_E_FAIL;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cntmaxsize_get(int unit, int port, pm_info_t pm_info, int *val)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, (uint32_t *)val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cntmaxsize_set(int unit, int port, pm_info_t pm_info, int val)
{
    uint32 reg_val;
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, &reg_val));

    soc_reg_field_set(unit, CLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, val);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CNTMAXSIZEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_modid_set (int unit, int port, pm_info_t pm_info, int value)
{
    uint32_t rval, modid;
    int      flen, phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    flen = soc_reg_field_length(unit, CLPORT_CONFIGr, MY_MODIDf)? value : 0;
    modid =  (value < (1 <<  flen))? value : 0;

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, MY_MODIDf, modid);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig2_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG2_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig2_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, HIGIG2_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig_mode_set (int unit, int port, pm_info_t pm_info, int mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG_MODEf, mode);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));
   
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_higig_mode_get (int unit, int port, pm_info_t pm_info, int *mode)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(mode) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, HIGIG_MODEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_encap_set (int unit, int port, pm_info_t pm_info, 
                                int flags, portmod_encap_t encap)
{
    return (clmac_encap_set(unit, port, flags, encap));

}

int pm4x25_port_encap_get (int unit, int port, pm_info_t pm_info, 
                            int *flags, portmod_encap_t *encap)
{
    return (clmac_encap_get(unit, port, flags, encap));
}

int pm4x25_port_config_port_type_set (int unit, int port, pm_info_t pm_info, int type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, PORT_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_config_port_type_get (int unit, int port, pm_info_t pm_info, int *type)
{
    int phy_acc;
    uint32_t rval;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_CONFIGr(unit, phy_acc, &rval));
    *(type) = soc_reg_field_get(unit, CLPORT_CONFIGr, rval, PORT_TYPEf);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_led_chain_config (int unit, int port, pm_info_t pm, int value)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm, phy_acc);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LED_CHAIN_CONFIGr (unit, phy_acc, value));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info,
                                    const portmod_access_get_params_t* params,
                                    int max_phys,
                                    phymod_phy_access_t* phy_access,
                                    int* nof_phys,
                                    int* is_most_ext)
{
    int phyn = 0, rv;
    uint32  iphy_lane_mask;
    int port_index=0;
    int serdes_lane=0;
    int xphy_lane_mask=0;
    uint32 xphy_id = 0;
    phymod_core_access_t core_access;
    uint32 lane_mask[MAX_NUM_CORES];
    uint32 pm_is_bypassed = 0;
    phymod_port_loc_t port_loc[MAX_NUM_CORES];
    int i, is_legacy_phy;
    uint32 xphy_idx;
    int num_of_phys;
    int done;

    SOC_INIT_FUNC_DEFS;

    if(max_phys > MAX_NUM_CORES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("max_phys parameter exceeded the MAX value. max_phys=%d, max allowed %d."),
            		   max_phys, MAX_NUM_CORES));
    }
    
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("phy access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    /* get port index and lane mask */
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index,
                                            &iphy_lane_mask));

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if(params->phyn >= PM_4x25_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"),
               PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1, params->phyn));
    }

    for( i = 0 ; i < max_phys; i++) {
        lane_mask[i] = 0;
        _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access[i]));
    }

    phyn = params->phyn;

    /* if phyn is -1, it is looking for outer most phy.
       assumption is that all lane has same phy depth. */
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1;
    }


    if( phyn == 0 ) {
        *nof_phys = 1;
        /* internal core */
        sal_memcpy (&phy_access[0], &(PM_4x25_INFO(pm_info)->int_core_access),
                    sizeof(phymod_phy_access_t));
        phy_access[0].access.lane_mask = iphy_lane_mask;
        if (params->lane != -1) {
            serdes_lane = port_index + params->lane;
            phy_access[0].access.lane_mask &= (0x1 << serdes_lane );
            if (phy_access[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        phy_access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */

        /* PCS bypass flag is set when the underlying device is DNX */
        if ((pm_is_bypassed) && (!PM_4x25_INFO(pm_info)->in_pm12x10)) {
            PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access[0].device_op_mode);
        }

        /* if it is warm boot, get probed information from wb db instead of re-probing. */
        if(SOC_WARM_BOOT(unit)) {
            rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &(phy_access[0].type));
            _SOC_IF_ERR_EXIT(rv);
        }
        
    } else {

        /* external phy */
        num_of_phys = 0;

        /* xphy idx is 1 less than phyn since internal core does not count */    
        xphy_idx = phyn-1;
    
        if (params->lane != -1){ /* specific lane */
            serdes_lane = port_index + params->lane;
            xphy_id = PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
            sal_memcpy (&phy_access[num_of_phys], &core_access,
                        sizeof(phymod_phy_access_t));
            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)? PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                          PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
            if( xphy_lane_mask != -1 ){
                phy_access[0].access.lane_mask |= xphy_lane_mask;
            }
            phy_access[0].port_loc = ( params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                       phymodPortLocSys : phymodPortLocLine;
            *nof_phys = 1;
        } else { /* all lanes */
            /* go thru all the lanes related to this port. */
            serdes_lane = 0;

            for (serdes_lane = 0; serdes_lane < MAX_PORTS_PER_PM4X25; serdes_lane++) 
                if (iphy_lane_mask & (1U<<serdes_lane)) {
                xphy_id = PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].xphy_id;
                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                    _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                    if(num_of_phys == 0 ) {/* first one */
                        sal_memcpy (&phy_access[num_of_phys], &core_access,
                                    sizeof(phymod_phy_access_t));
                        xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                    PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                    PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                        if( xphy_lane_mask != -1) {
                            lane_mask[num_of_phys] |= xphy_lane_mask;
                        }
                        port_loc[num_of_phys] = (params->sys_side == PORTMOD_SIDE_SYSTEM) ?
                                                  phymodPortLocSys : phymodPortLocLine;
                        
                        num_of_phys++;
                    } else {
                        /* if not the first one, see if this one sharing core with other 
                           lanes.  */
                        done = 0;
                        for( i = 0 ; (i < num_of_phys) &&(!done) ; i++) {
                             if((!sal_memcmp (&phy_access[i], &core_access,
                                               sizeof(phymod_phy_access_t)))) { /* found a match */
                                /* update lane */
                                xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                            PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                            PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                                if( xphy_lane_mask != -1) {
                                    lane_mask[i] |= xphy_lane_mask;
                                }
                                done = 1;
                            }
                        }
                        if((!done) && (num_of_phys < max_phys)){
                            sal_memcpy (&phy_access[num_of_phys], &core_access,
                            sizeof(phymod_phy_access_t));
                            xphy_lane_mask = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                        PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ss_lane_mask:
                                        PM_4x25_INFO(pm_info)->lane_conn[xphy_idx][serdes_lane].ls_lane_mask;
                            if( xphy_lane_mask != -1) {
                                lane_mask[num_of_phys] |= xphy_lane_mask;
                            }
                            port_loc[num_of_phys] = ( params->sys_side == PORTMOD_SIDE_SYSTEM)?
                                                                phymodPortLocSys : phymodPortLocLine ;
                            num_of_phys++;
                        } 
                    }
                }
            }
            *nof_phys = num_of_phys;
            for( i = 0 ; (i < *nof_phys) ; i++) {
                phy_access[i].access.lane_mask = lane_mask[i];
                phy_access[i].port_loc = port_loc[i];
            }
        }
    } /* ext phys */ 
       
    /* assumption is that all logical port have same phy depths, that 
       will not true when simplex introduced , until then we can keep 
       this code. */
    if (is_most_ext) {
        if (phyn == PM_4x25_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }
exit:
    SOC_FUNC_RETURN;

}

/*
 * Get lane phymod access structure via pm id.
 * This function can be used for getting phy access
 * when port is not attached.
 *
 * If a single lane access is required, please specify the params.lane.
 * Otherwise, set params.lane = -1, meaning get lane access for all the
 * lanes in the given PM.
 */
int pm4x25_pm_phy_lane_access_get(
    int unit, int pm_id, pm_info_t pm_info,
    const portmod_access_get_params_t* params,
    int max_phys, phymod_phy_access_t* phy_access,
    int* nof_phys, int* is_most_ext)
{

    SOC_INIT_FUNC_DEFS;

    *nof_phys = 1;
    /* internal core */
    sal_memcpy (&phy_access[0], &(PM_4x25_INFO(pm_info)->int_core_access),
                sizeof(phymod_phy_access_t));
    
    if (params->lane != -1) {
        phy_access[0].access.lane_mask = (0x1 << (params->lane));
    }
    else {
        phy_access[0].access.lane_mask = (0x1 << MAX_PORTS_PER_PM4X25) - 1;
        
    }
    phy_access[0].port_loc = phymodPortLocLine; /* only line is availabe for internal. */

    if (is_most_ext) {
        *is_most_ext = 1;
    } 

    SOC_FUNC_RETURN;
}

typedef enum clmac_port_mode_e{
    CLMAC_4_LANES_SEPARATE  = 0,
    CLMAC_3_TRI_0_1_2_2     = 1,
    CLMAC_3_TRI_0_0_2_3     = 2,
    CLMAC_2_LANES_DUAL      = 3,
    CLMAC_4_LANES_TOGETHER  = 4
} clmac_port_mode_t;

static phymod_dispatch_type_t pm4x25_serdes_list[] =
{
#ifdef PHYMOD_TSCF_SUPPORT
    phymodDispatchTypeTscf,
#endif
#ifdef PHYMOD_TSCF16_SUPPORT
    phymodDispatchTypeTscf16,
#endif
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
    phymodDispatchTypeTscf_gen3,
#endif
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
    phymodDispatchTypeTscf16_gen3,
#endif
    phymodDispatchTypeInvalid
};

STATIC
int _pm4x25_pm_core_probe(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int     rv = SOC_E_NONE;
    int     temp, phy;
    portmod_pbmp_t port_phys_in_pm;
    int probe =0;
    uint32 xphy_id;
    uint32 nof_ext_phys =0;
    int i, is_legacy_phy;
    portmod_xphy_lane_connection_t lane_conn;
    phymod_core_access_t core_access;
    int core_probed;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&lane_conn,0,sizeof(portmod_xphy_lane_connection_t));

    /* probe phys (both internal and external cores. */

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);

    /* if internal core is not probe, probe it. Currently checking to type to see
       phymodDispatchTypeInvalid, later need to move to WB.  */
    _SOC_IF_ERR_EXIT(portmod_common_serdes_probe(pm4x25_serdes_list, &PM_4x25_INFO(pm_info)->int_core_access, &probe));

    /* save probed phy type to wb. */
    if(probe) {
          rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD,
                     pm_info->wb_vars_ids[phy_type],
                     &PM_4x25_INFO(pm_info)->int_core_access.type);
          _SOC_IF_ERR_EXIT(rv);
    }

    /* probe ext core related to this logical port. */
    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if( PM_4x25_INFO(pm_info)->nof_phys[temp] >= 2 ){
                nof_ext_phys = PM_4x25_INFO(pm_info)->nof_phys[temp]-1;
                for(i=0 ; i<nof_ext_phys ; i++) {
                    xphy_id = PM_4x25_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        _SOC_IF_ERR_EXIT(portmod_common_ext_phy_probe(unit, port, &core_access, &probe));
                        if ((probe == 0) &&
                            (core_access.type == phymodDispatchTypeInvalid)){
                            pm4x25_xphy_lane_detach_from_pm(unit, pm_info, phy, i+1, &lane_conn);
                            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "Warning: EXT PHY p=%d xphy_id=%d can't be probed\n"), port, xphy_id));
                        } else {
                            core_probed = 1;
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_set(unit, xphy_id, core_probed));
                            /* Check if after the probe the ext phy is legacy, so we dont overrun the current value */
                            _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
                            _SOC_IF_ERR_EXIT(portmod_xphy_core_access_set(unit, xphy_id, &core_access, is_legacy_phy));
                        }
                    }
                }
            }
        }
        temp++;
    }

    /*update warmboot engine*/
    rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD,
        pm_info->wb_vars_ids[phy_type],
        &PM_4x25_INFO(pm_info)->int_core_access.type, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_serdes_core_init(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int rv;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    phymod_phy_access_t serdes_access;
    const portmod_port_interface_config_t* config;
    uint32 core_is_initialized;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    uint32 init_flags = 0, is_bypass;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    core_conf.firmware_load_method = PM_4x25_INFO(pm_info)->fw_load_method;
    core_conf.firmware_loader = PM_4x25_INFO(pm_info)->external_fw_loader;
    core_conf.lane_map = PM_4x25_INFO(pm_info)->lane_map;

    core_conf.afe_pll.afe_pll_change_default = PM_4x25_INFO(pm_info)->afe_pll.afe_pll_change_default;
    core_conf.afe_pll.ams_pll_iqp = PM_4x25_INFO(pm_info)->afe_pll.ams_pll_iqp;
    core_conf.afe_pll.ams_pll_en_hrz = PM_4x25_INFO(pm_info)->afe_pll.ams_pll_en_hrz;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;
    core_conf.interface.pll_divider_req = config->pll_divider_req;

    _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
    core_status.pmd_active = 0;

    init_flags = PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) | PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info);

    if (PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }
    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_conf);
    }
#ifdef FW_BCAST_DOWNLOAD
    if (PORTMOD_PORT_ADD_F_FIRMWARE_DOWNLOAD_BCAST_GET(add_info)) {
        PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_SET(&core_conf);
    }
#endif

    if (PORTMOD_PORT_ADD_F_IS_ENHANCED_PM4X25_GEN2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_CLK4PCS_3BIT_40T_SET(&core_conf);
    }
#ifdef DEFAULT_OSR2P5_SET
    if (PORTMOD_PORT_ADD_F_DEFAULT_OSR2P5_GET(add_info)) {
        PHYMOD_CORE_INIT_F_DEFAULT_OSR2P5_SET(&core_conf);
    }
#endif /* DEFAULT_OSR2P5_SET */

    core_conf.op_datapath = add_info->phy_op_datapath;

    rv = PM4x25_IS_INITIALIZED_GET(unit, pm_info, &core_is_initialized);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    if ((PORTMOD_PM4x25_IS_GEN3_28nm(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version))
         && PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)
         && (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized))) {
        portmod_vcos_speed_config_t speed_config;
        phymod_dual_plls_t init_plls;

        portmod_vcos_speed_config_t_init(unit, &speed_config);
        speed_config.higig_mode = add_info->init_config.is_hg;
        speed_config.speed = add_info->interface_config.speed;
        speed_config.num_lanes = add_info->interface_config.port_num_lanes;
        if(add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
            speed_config.port_type = portmodCpmPrtEthOfMixedMode;
        }
        rv = _pm4x25_init_plls_get(unit, 0 , pm_info, &speed_config, 1, &init_plls);
        _SOC_IF_ERR_EXIT(rv);
        core_conf.pll0_div_init_value = init_plls.pll0_div;
        core_conf.pll1_div_init_value = init_plls.pll1_div;
    }

    if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(core_is_initialized)) {
         /* firmware load will happen after pass 1 */
         if (!PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_GET(core_is_initialized) ||
             PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
             if ((is_bypass) && (!PM_4x25_INFO(pm_info)->in_pm12x10)) {
                 PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(PM_4x25_INFO(pm_info)->int_core_access.device_op_mode);
             }

            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                if (PM_4x25_INFO(pm_info)->rescal != -1) {
                    sal_memcpy(&serdes_access, &(PM_4x25_INFO(pm_info)->int_core_access), sizeof(phymod_phy_access_t));
                    phymod_phy_rescal_set(&serdes_access, 1, PM_4x25_INFO(pm_info)->rescal);
                }
            }
            if (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeTscf_gen3;
#else
                PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeCount;
#endif
            }
             _SOC_IF_ERR_EXIT(phymod_core_init(&PM_4x25_INFO(pm_info)->int_core_access,
                                               &core_conf,
                                               &core_status));

            if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_flags == 0) {
                PORTMOD_CORE_INIT_FLAG_FIRMWARE_LOADED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }
            if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info) || init_flags == 0) {
                PORTMOD_CORE_INIT_FLAG_INITIALZIED_SET(core_is_initialized);
                rv = SOC_WB_ENGINE_SET_VAR (unit, SOC_WB_ENGINE_PORTMOD,
                                            pm_info->wb_vars_ids[isInitialized],
                                            &core_is_initialized);
                _SOC_IF_ERR_EXIT(rv);
            }

        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_ext_phy_core_init(int unit, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int temp, phy;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    const portmod_port_interface_config_t* config;
    portmod_pbmp_t port_phys_in_pm;
    phymod_firmware_load_method_t fw_load_method;
    phymod_lane_map_t lane_map;
    phymod_core_access_t core_access;
    uint32 primary_core_num;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    uint32 xphy_id;
    int is_legacy_phy = 0;
    uint32 nof_ext_phys = 0;
    int force_fw_load, is_initialized = 0;
    int init_all = 0, lane, i;

    SOC_INIT_FUNC_DEFS;

    config = &(add_info->interface_config);

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;

    /* Get the first phy related to this port */
    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);

    _SOC_IF_ERR_EXIT(phymod_core_init_config_t_init(&core_conf));

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS1_SET(&core_conf);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_EXECUTE_PASS2_SET(&core_conf);
    }

    core_conf.afe_pll.afe_pll_change_default = PM_4x25_INFO(pm_info)->afe_pll.afe_pll_change_default;
    core_conf.afe_pll.ams_pll_iqp = PM_4x25_INFO(pm_info)->afe_pll.ams_pll_iqp;
    core_conf.afe_pll.ams_pll_en_hrz = PM_4x25_INFO(pm_info)->afe_pll.ams_pll_en_hrz;

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&core_conf.interface));
    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));
    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                interface, &core_conf.interface.interface_type));
    core_conf.interface.data_rate = config->speed;
    core_conf.interface.interface_modes = config->interface_modes;
    core_conf.interface.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

    core_conf.op_datapath = add_info->phy_op_datapath;

    core_conf.firmware_load_method = (PM_4x25_INFO(pm_info)->fw_load_method ==
                                            phymodFirmwareLoadMethodNone) ?
                                            phymodFirmwareLoadMethodNone :
                                            phymodFirmwareLoadMethodInternal;
    core_conf.firmware_loader = NULL;

    /* config "no swap" for external phys.set later using APIs*/
    for (lane=0 ; lane < PM4X25_LANES_PER_CORE; lane++) {
        core_conf.lane_map.lane_map_rx[lane] = lane;
        core_conf.lane_map.lane_map_tx[lane] = lane;
    }

    temp = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
        if (PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)) {
            if (PM_4x25_INFO(pm_info)->nof_phys[temp] >= 2 ) {
                nof_ext_phys = PM_4x25_INFO(pm_info)->nof_phys[temp]-1;
                for (i=0 ; i<nof_ext_phys; i++) {
                    xphy_id = PM_4x25_INFO(pm_info)->lane_conn[i][temp].xphy_id;
                    if (xphy_id != PORTMOD_XPHY_ID_INVALID) {
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        _SOC_IF_ERR_EXIT(portmod_xphy_fw_load_method_get(unit, xphy_id, &fw_load_method));
                        _SOC_IF_ERR_EXIT(portmod_xphy_force_fw_load_get(unit, xphy_id, &force_fw_load));
                        _SOC_IF_ERR_EXIT(portmod_xphy_lane_map_get(unit, xphy_id, &lane_map));
                        _SOC_IF_ERR_EXIT(portmod_xphy_primary_core_num_get(unit, xphy_id, &primary_core_num));

                        _SOC_IF_ERR_EXIT(phymod_core_status_t_init(&core_status));
                        core_status.pmd_active = 0;

                        if (force_fw_load == phymodFirmwareLoadForce) {
                            PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&core_conf);
                        } else if (force_fw_load == phymodFirmwareLoadAuto) {
                            PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&core_conf);
                        }

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) || init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS1))
                            continue;

                        if ((PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)|| init_all) &&
                            (is_initialized & PHYMOD_CORE_INIT_F_EXECUTE_PASS2))
                            continue;
                        
                        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
                        if (is_legacy_phy) {
                            /* Dont do a core init for legacy Phys.*/
                        } else {
                            _SOC_IF_ERR_EXIT(phymod_core_init(&core_access,
                                    &core_conf,
                                    &core_status));
                        }

                        if (init_all) {
                            is_initialized = (PHYMOD_CORE_INIT_F_EXECUTE_PASS2 | PHYMOD_CORE_INIT_F_EXECUTE_PASS1);
                        } else if (PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
                            is_initialized |= PHYMOD_CORE_INIT_F_EXECUTE_PASS2;
                        } else {
                            is_initialized = PHYMOD_CORE_INIT_F_EXECUTE_PASS1;
                        }

                        _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, xphy_id, is_initialized));
                    }
                }
            }
        }
        temp++;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_core_init(int unit, int port, pm_info_t pm_info,
                             const portmod_port_add_info_t* add_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    if (enable) {
        _SOC_IF_ERR_EXIT(_pm4x25_pm_serdes_core_init(unit, port, pm_info, add_info));
        _SOC_IF_ERR_EXIT(_pm4x25_pm_ext_phy_core_init(unit, pm_info, add_info));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_enable(int unit, int port, pm_info_t pm_info, int port_index, const portmod_port_add_info_t* add_info, int enable)
{
    uint32 reg_val, port_mode;
    int caui_mode, three_ports_mode;
    int  rv, phy_acc;
    int pm_ver_is_d0 = 0;
    int is_master_pll = FALSE;
    pm_version_t pm_version;
    uint32 port_ctrl_reg_val;
    SOC_INIT_FUNC_DEFS;

    /* The below code is to distinguish PM4x25, before D0 versus D0 and beyond. This is since some sequences were modified in PM4x25 D0 */
    rv = READ_CLPORT_GENERAL_SPARE1_REGr(unit, port, &reg_val);
    if(rv == SOC_E_NONE) {
        pm_ver_is_d0 = 1;
    }else if (rv != SOC_E_PARAM) { /* rv == SOC_E_PARAM is OK, since it means CLPORT_GENERAL_SPARE1_REG register doesn't exist -> pm_ver_is_d0=FALSE */
        _SOC_EXIT_WITH_ERR(rv, (_SOC_MSG("Port %d: We were trying to read CLPORT_GENERAL_SPARE1_REG but recieved an error that is not SOC_E_PARAM!\n"),
                                port));
    }

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = _pm4x25td_pm_enable(unit, port, pm_info, port_index, add_info, enable);
        _SOC_IF_ERR_EXIT(rv);
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        is_master_pll = _pm4x25_port_tsc_is_master_pll(unit, port, pm_info);

        _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

        if(enable) {
            /* Power Save */
            _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));

            /* Port configuration */
            rv = PM4x25_THREE_PORTS_MODE_GET(unit, pm_info, &three_ports_mode);
            _SOC_IF_ERR_EXIT(rv);

            caui_mode = (add_info->interface_config.interface == SOC_PORT_IF_CAUI ? 1 : 0);
            /* if(PM4x25_QUAD_MODE_IF(add_info->interface_config.interface)) */
            if(add_info->interface_config.port_num_lanes >= 4) {
                port_mode = CLMAC_4_LANES_TOGETHER;
            /*} else if(PM4x25_DUAL_MODE_IF(add_info->interface_config.interface)){ */
            } else if(add_info->interface_config.port_num_lanes == 2){
                if(!three_ports_mode) {
                    port_mode = CLMAC_2_LANES_DUAL;
                } else {
                    if(port_index < 2) {
                        port_mode = CLMAC_3_TRI_0_0_2_3;
                    } else {
                        port_mode = CLMAC_3_TRI_0_1_2_2;
                    }
                }
            } else { /*single mode*/
                if(!three_ports_mode) {
                    port_mode = CLMAC_4_LANES_SEPARATE;
                } else {
                    if(port_index < 2) {
                        port_mode = CLMAC_3_TRI_0_1_2_2;
                    } else {
                        port_mode = CLMAC_3_TRI_0_0_2_3;
                    }
                }
            }

            _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &reg_val));
            if (SOC_REG_FIELD_VALID(unit, CLPORT_MODE_REGr, FIELD_10_10f)) {
                soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, FIELD_10_10f, caui_mode);
            } else if (SOC_REG_FIELD_VALID(unit, CLPORT_MODE_REGr, SINGLE_PORT_MODE_SPEED_100Gf)) {
                soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, SINGLE_PORT_MODE_SPEED_100Gf, caui_mode);
            }
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_CORE_PORT_MODEf, port_mode);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, XPORT0_PHY_PORT_MODEf,  port_mode);
            soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val, EGR_1588_TIMESTAMPING_MODEf,
                add_info->flags & PORTMOD_PORT_ADD_F_EGR_1588_TIMESTAMP_MODE_48BIT ? 1 : 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MODE_REGr(unit, phy_acc, reg_val));

            if(PORTMOD_PM4x25_IS_GEN3_28nm(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)){
                /* Set txpi sdm scheme */
                _SOC_IF_ERR_EXIT(_pm4x25_txpi_sdm_scheme_set(unit, port, pm_info, add_info->init_config.txpi_sdm_scheme));
             }

            if (PM_4x25_INFO(pm_info)->nof_phys[0] > 0) { 
                /* Get Serdes OOR */
                if(is_master_pll != TRUE) {  /* This PM is not master PM, reset it. */
                    _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));
                    _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 0));
                } else {

                    /*Hard reset tsc core*/
                    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &port_ctrl_reg_val));
                    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &port_ctrl_reg_val, RSTB_HWf, 0);
                    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, port_ctrl_reg_val));

                    sal_usleep(10);

                    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &port_ctrl_reg_val));
                    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &port_ctrl_reg_val, RSTB_HWf, 1);
                    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, port_ctrl_reg_val));

                    sal_usleep(10);
                }
            }

            /* Bring MAC OOR */
            _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 0);
            if (PORTMOD_PORT_ADD_F_SYS_16B_INTF_MODE_GET(add_info)) {
                if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr,
                                        SYS_16B_INTF_MODEf)) {
                    soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val,
                                      SYS_16B_INTF_MODEf, 1);
                }
                _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
            }
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
        } else { /* disable */
            /* put MAC in reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));

            if (pm_ver_is_d0 == 1){ /* At PM4x25 D0 and beyond - put MAC in reset - here */
                /* put MAC in reset */
                _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
                soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val, XMAC0_RESETf, 1);
                _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
            }

            /* Put Serdes in reset*/
            if(is_master_pll != TRUE) {  /* This PM is not master PM, reset it. */
                _SOC_IF_ERR_EXIT(pm4x25_port_tsc_reset_set(unit, phy_acc, 1));
            }

            /* Turn on Power Save */
            _SOC_IF_ERR_EXIT(READ_CLPORT_POWER_SAVEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_POWER_SAVEr, &reg_val, XPORT_CORE0f, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_POWER_SAVEr(unit, phy_acc, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_to_cpm_pm_enable(int unit, int port, pm_info_t pm_info, int port_index,
                            const portmod_port_add_info_t* add_info,
                            int enable) {
    int rv;
    SOC_INIT_FUNC_DEFS;
    rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, add_info, enable);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_pm_port_init(int unit, int port, pm_info_t pm_info, int internal_port,
                         const portmod_port_add_info_t* add_info, int enable)
{
    int rv, phy_acc;
    uint32 reg_val, flags;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};
    uint32 rsv_mask, rsv_pfc_mask;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = _pm4x25td_pm_port_init(unit, port, internal_port, add_info, enable);
        _SOC_IF_ERR_EXIT(rv);
    } else 
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        if (internal_port == -1)  {
            _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Invalid internal Port %d"), 
                                    internal_port));
        }   
        if(enable) {

            /* RSV Mask */
            rsv_mask = 0;
            rsv_pfc_mask = 0;
            _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_RSV_MASKr(unit, phy_acc, &rsv_pfc_mask));
            if (rsv_pfc_mask & PFC_FRAME_DETECTED_RSV_MASK) {
                rsv_mask |= PFC_FRAME_DETECTED_RSV_MASK;
            }
            SHR_BITSET(&rsv_mask, 3); /* Receive terminate/code error */
            SHR_BITSET(&rsv_mask, 4); /* CRC error */
            SHR_BITSET(&rsv_mask, 6); /* Truncated/Frame out of Range */
            SHR_BITSET(&rsv_mask, 17); /* RUNT detected*/
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MAC_RSV_MASKr(unit, phy_acc, rsv_mask));

            /* Soft reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

            /* Port enable */
            _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

            /* Init MAC */
            flags = 0;
            if(PORTMOD_PORT_ADD_F_RX_SRIP_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_RX_STRIP_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_APPEND_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_APPEND_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_REPLACE_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_REPLACE_CRC;
            }

            if(PORTMOD_PORT_ADD_F_TX_PASS_THROUGH_CRC_GET(add_info)) {
                flags |= CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE;
            }

            if (IS_HG_PORT(unit, port)){
                flags |= CLMAC_INIT_F_IPG_CHECK_DISABLE;
            } else {
                flags &= ~CLMAC_INIT_F_IPG_CHECK_DISABLE;
            }

            if(PHYMOD_INTF_MODES_HIGIG_GET(&(add_info->interface_config))) {
                flags |= CLMAC_INIT_F_IS_HIGIG;
            }
            rv = clmac_init(unit, port, flags);
            _SOC_IF_ERR_EXIT(rv);

            /* LSS */
            _SOC_IF_ERR_EXIT(READ_CLPORT_FAULT_LINK_STATUSr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, REMOTE_FAULTf, 1);
            soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &reg_val, LOCAL_FAULTf, 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_FAULT_LINK_STATUSr(unit, phy_acc, reg_val)); 

            /* Counter MAX size */ 
            _SOC_IF_ERR_EXIT(READ_CLPORT_CNTMAXSIZEr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_CNTMAXSIZEr, &reg_val, CNTMAXSIZEf, 1518);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_CNTMAXSIZEr(unit, phy_acc, reg_val));
            
            /* Reset MIB counters */ 
            _SOC_IF_ERR_EXIT(READ_CLPORT_MIB_RESETr(unit, phy_acc, &reg_val));
            /* coverity[ptr_arith:FALSE] */
            SHR_BITSET(&reg_val, internal_port);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val));
            SHR_BITCLR((uint32 *)&reg_val, internal_port);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, reg_val));

            /* Disable MAC timestamp by default */
            if (PORTMOD_PM4x25_IS_GEN3(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
                _SOC_IF_ERR_EXIT(pm4x25_port_mac_timestamp_enable_set(unit, port, pm_info, 0));
            }
        } else {
            /* Port disable */
            _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &reg_val, port_fields[internal_port], 0);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

             /* Soft reset */
            _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
            soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[internal_port], 1);
            _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_an_mode_update (int unit, int port, pm_info_t pm_info, int port_index, int num_lanes, int an_cl73)
{
    int is_hg, rv;
    phymod_an_mode_type_t an_mode;

    SOC_INIT_FUNC_DEFS;

    rv = PM4x25_IS_HG_GET(unit, pm_info, &is_hg, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_AN_MODE_GET(unit, pm_info, &an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    if(is_hg) {
        an_mode = phymod_AN_MODE_CL73BAM;
    } else if (an_cl73) {
        switch (an_cl73) {
            case PORTMOD_CL73_W_BAM:
                an_mode = phymod_AN_MODE_CL73BAM;
                break ;
            case PORTMOD_CL73_WO_BAM:
                an_mode = phymod_AN_MODE_CL73;
                break ;
            case PORTMOD_CL73_HPAM_VS_SW:
                an_mode = phymod_AN_MODE_HPAM;
                break ;  /* against TD+ */
            case PORTMOD_CL73_HPAM:
                an_mode = phymod_AN_MODE_HPAM;
                break ;  /* against TR3 */
            case PORTMOD_CL73_CL37:
                an_mode = phymod_AN_MODE_CL73;
                break ;
            case PORTMOD_MSA:
                an_mode = phymod_AN_MODE_MSA;
                break;
            case PORTMOD_CL73_MSA:
                an_mode = phymod_AN_MODE_CL73_MSA;
            default:
                break;
        }
    } else {
        an_mode = phymod_AN_MODE_CL73;  /* default mode */
    }

    rv = PM4x25_AN_MODE_SET(unit, pm_info, an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_ext_phy_interface_check(int unit, int port, pm_info_t pm_info, phymod_phy_access_t *phy_acc,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t interface, int *is_valid)
{
    int rv = 0;
    int num_lanes = 0, data_rate = 0;
    uint32_t interface_mode = 0;
    phymod_interface_t  phymod_intf;
    int is_legacy_phy = 0;
    int xphy_id;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (pm4x25_port_lane_count_get(unit, port, pm_info,
                                    PORTMOD_SIDE_LINE, &num_lanes));
    interface_mode = interface_config->interface_modes;
    data_rate      = interface_config->speed;

    xphy_id = phy_acc->access.addr;
    _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

    if (is_legacy_phy) {
        /* if its a legacy PHY let the PHY driver decide the valid interface types */
        *is_valid = 1;
        rv = SOC_E_NONE;
        SOC_EXIT;
    }

    /* convert portmod interface tophymod interface */
    rv = portmod_intf_to_phymod_intf(unit, data_rate, interface, &phymod_intf);
    _SOC_IF_ERR_EXIT(rv);

    rv = portmod_port_line_interface_is_valid(unit, phy_acc, port, data_rate, phymod_intf,
                                            num_lanes, interface_mode, is_valid);
    _SOC_IF_ERR_EXIT(rv);

    if (!*is_valid) {
        LOG_ERROR(BSL_LS_BCM_PORT,
              (BSL_META_UP(unit, port,
               "ERROR: u=%d p=%d interface type %d not supported by external PHY for this speed %d \n"),
               unit, port, interface, data_rate));
        rv = SOC_E_PARAM;
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_interface_check(int unit, int port,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t interface, int *is_valid)
{
    int is_higig;
    int num_lanes, nof_cores = 0;
    phymod_core_info_t   core_info;
    uint32_t rev_letter = 0;
    phymod_core_access_t core_acc[1+MAX_NUM_CORES];
    SOC_INIT_FUNC_DEFS;

    *is_valid = 0;
    num_lanes = interface_config->port_num_lanes;
    is_higig = ((interface_config->encap_mode == SOC_ENCAP_HIGIG2) ||
                 (interface_config->encap_mode == SOC_ENCAP_HIGIG) ||
                 PHYMOD_INTF_MODES_HIGIG_GET(interface_config)) ? 1 : 0;

    if (SOC_PORT_IF_ILKN == interface) {
        *is_valid = 1;
        return SOC_E_NONE;
    }

    switch (interface_config->speed) {
        case 0:
            /* When AN=1 the input speed to portmod may be 0. At
             * this moment the speed is still undecided so there
             * is no reason to invalidate the interface type.
             */
            *is_valid = 1;
            break;
        case 1000:
            if (num_lanes == 1) {
                if ((SOC_PORT_IF_SGMII == interface) ||
                    (SOC_PORT_IF_GMII == interface) ||
                    (SOC_PORT_IF_KX == interface)) {
                    *is_valid = 1;
                }
            }
            break;
         case 2500:
            if (num_lanes == 1) {
                if ((SOC_PORT_IF_SGMII == interface) ||
                    (SOC_PORT_IF_GMII == interface)) {
                    *is_valid = 1;
                }
            }
            break;
        case 5000:
            if (num_lanes == 1) {
                if ((SOC_PORT_IF_KR == interface) ||
                     (SOC_PORT_IF_CR == interface)) {
                    *is_valid = 1;
                }
            }
            break;
        case 10000:
        case 11000:
        case 12000:
            if (is_higig) {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_XFI == interface) ||
                        (SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface)) {
                        *is_valid = 1;
                    }
                }
            } else {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_SFI == interface) ||
                        (SOC_PORT_IF_XFI == interface) ||
                        (SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface) ||
                        (SOC_PORT_IF_SR == interface) ||
                        (SOC_PORT_IF_LR == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 4) {
                    _SOC_IF_ERR_EXIT
                        (portmod_port_core_access_get(unit, port, 0,
                                                      (1+MAX_NUM_CORES),
                                                      core_acc, &nof_cores, NULL));
                    _SOC_IF_ERR_EXIT(phymod_core_info_get(core_acc, &core_info));
                    rev_letter = (core_info.serdes_id & 0xc000) >> 14;
                    /* 10G XAUI is only avialable in TSCF Rev C0 of the PM4x25. */
                    if (((core_info.serdes_id & 0x3f) == 0x14) && (rev_letter == 2)) {
                        if ((SOC_PORT_IF_DNX_XAUI == interface) ||
                            (SOC_PORT_IF_XAUI == interface)) {
                            *is_valid = 1;
                        }
                    }
                }
            }
            break;
        case 20000:
        case 21000:
            if (is_higig) {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface)) {
                        *is_valid = 1;
                    }
                }
            } else {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface) ||
                        (SOC_PORT_IF_SR == interface) ||
                        (SOC_PORT_IF_LR == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface) ||
                        (SOC_PORT_IF_SR2 == interface) ||
                        (SOC_PORT_IF_LR2 == interface)) {
                        *is_valid = 1;
                    }
                }
            }
            break;
        case 25000:
        case 27000:
            if (is_higig) {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface)) {
                        *is_valid = 1;
                    }
                }
            } else {
                if (num_lanes == 1) {
                    if ((SOC_PORT_IF_KR == interface) ||
                        (SOC_PORT_IF_CR == interface) ||
                        (SOC_PORT_IF_SR == interface) ||
                        (SOC_PORT_IF_LR == interface) ||
                        (SOC_PORT_IF_XFI == interface) ||
                        (SOC_PORT_IF_SFI == interface)) {
                        *is_valid = 1;
                    }
                }
            }
            break;
        case 40000:
        case 42000:
            if (is_higig) {
                if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface)) {
                        *is_valid = 1;
                    }
                }
            } else {
                if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface) ||
                        (SOC_PORT_IF_SR4 == interface) ||
                        (SOC_PORT_IF_XLAUI == interface) ||
                        (SOC_PORT_IF_LR4 == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface) ||
                        (SOC_PORT_IF_SR2 == interface) ||
                        (SOC_PORT_IF_XLAUI2 == interface) ||
                        (SOC_PORT_IF_LR2 == interface)) {
                        *is_valid = 1;
                    }
                }
            }
            break;
        case 48000:
            if (!is_higig && num_lanes == 4) {
                if ((SOC_PORT_IF_XLAUI == interface) ||
                    (SOC_PORT_IF_CR4 == interface) ||
                    (SOC_PORT_IF_KR4 == interface)) {
                    *is_valid = 1;
                }
            }
            break;
        case 50000:
        case 53000:
            if (is_higig) {
                if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface)) {
                        *is_valid = 1;
                    }
                } else {
                    LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_UP(unit, port,
                               "ERROR: u=%d p=%d number of lane %d not supported at speed %d\n"),
                                unit, port, num_lanes, interface_config->speed));
                    return SOC_E_PARAM;
                }
            } else {
                if (num_lanes == 2) {
                    if ((SOC_PORT_IF_KR2 == interface) ||
                        (SOC_PORT_IF_CR2 == interface) ||
                        (SOC_PORT_IF_SR2 == interface) ||
                        (SOC_PORT_IF_XLAUI2 == interface) ||
                        (SOC_PORT_IF_LR2 == interface)) {
                        *is_valid = 1;
                    }
                } else if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface)) {
                        *is_valid = 1;
                    }
                } else {
                    LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_UP(unit, port,
                               "ERROR: u=%d p=%d number of lane %d not supported at speed %d\n"),
                                unit, port, num_lanes, interface_config->speed));
                    return SOC_E_PARAM;
                }
            }
            break;
        case 100000:
        case 106000:
            if (is_higig) {
                if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface)) {
                        *is_valid = 1;
                    }
                }
            } else {
                if (num_lanes == 4) {
                    if ((SOC_PORT_IF_KR4 == interface) ||
                        (SOC_PORT_IF_CR4 == interface) ||
                        (SOC_PORT_IF_SR4 == interface) ||
                        (SOC_PORT_IF_CAUI4 == interface) ||
                        (SOC_PORT_IF_CAUI == interface) ||
                        (SOC_PORT_IF_LR4 == interface)) {
                        *is_valid = 1;
                    }
                }
            }
            break;
        default:
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_default_interface_get(int unit, int port,
                               const portmod_port_interface_config_t *interface_config,
                               soc_port_if_t *interface)
{
    int is_higig;
    int num_lanes;

    *interface = SOC_PORT_IF_NULL;
    num_lanes = interface_config->port_num_lanes;
    is_higig = ((interface_config->encap_mode == SOC_ENCAP_HIGIG2) ||
                 (interface_config->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    switch (interface_config->speed) {
        case 1000:
            *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                    SOC_PORT_IF_GMII: SOC_PORT_IF_SGMII;
            break;
        case 2500:
            *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                        SOC_PORT_IF_GMII: SOC_PORT_IF_SGMII;
            break;
        case 5000:
            *interface = SOC_PORT_IF_KR;
            break;
        case 10000:
            if (is_higig) {
                *interface = SOC_PORT_IF_CR;
            } else {
                *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                        SOC_PORT_IF_SFI : SOC_PORT_IF_XFI;
            }
            break;
        case 11000:
        case 12000:
            *interface = SOC_PORT_IF_XFI;
            break;
        case 20000:
        case 21000:
            if (is_higig) {
                if (num_lanes == 2) {
                    *interface = SOC_PORT_IF_CR2;
                } else {
                    *interface = SOC_PORT_IF_CR;
                }
            } else {
                if (num_lanes == 2) {
                    *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                        SOC_PORT_IF_SR2 : SOC_PORT_IF_CR2;
                } else {
                    *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                        SOC_PORT_IF_SR : SOC_PORT_IF_CR;
                }
            }
            break;
        case 25000:
        case 27000:
            if (is_higig) {
                *interface = SOC_PORT_IF_CR;
            } else {
                *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                    SOC_PORT_IF_SR : SOC_PORT_IF_CR;
            }
            break;
        case 40000:
        case 42000:
            if (is_higig) {
                if (num_lanes == 4) {
                    *interface = SOC_PORT_IF_KR4;
                } else {
                    *interface = SOC_PORT_IF_CR2;
                }
            } else {
                if (num_lanes == 4) {
                    *interface = SOC_PORT_IF_XLAUI;
                } else {
                    *interface = SOC_PORT_IF_XLAUI2;
                }
            }
            break;
        case 50000:
        case 53000:
            if (is_higig) {
                if (num_lanes == 4) {
                    *interface = SOC_PORT_IF_KR4;
                } else if (num_lanes == 2){
                    *interface = SOC_PORT_IF_CR2;
                } else {
                    LOG_ERROR(BSL_LS_BCM_PORT, (BSL_META_UP(unit, port,
                               "ERROR: u=%d p=%d number of lane %d not supported at speed %d\n"),
                                unit, port, num_lanes, interface_config->speed));
                    return SOC_E_PARAM;
                }
            } else {
                *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                    SOC_PORT_IF_SR2 : SOC_PORT_IF_CR2;
                if (num_lanes == 4) {
                    *interface = SOC_PORT_IF_CR4;
                }
            }
            break;
        case 100000:
        case 106000:
            if (is_higig) {
                *interface = SOC_PORT_IF_CR4;
            } else {
                *interface = SOC_PORT_IF_CAUI4;
            }
            break;
        default:
            if (num_lanes == 1) {
                if (interface_config->speed >= 10000) {
                    *interface = SOC_PORT_IF_CR;
                } else {
                    *interface = PHYMOD_INTF_MODES_FIBER_GET(interface_config)?
                                    SOC_PORT_IF_GMII: SOC_PORT_IF_SGMII;
                }
            } else if (num_lanes == 2) {
                *interface = SOC_PORT_IF_CR2;
            } else if (num_lanes == 4) {
                if (interface_config->speed >= 100000) {
                    if (is_higig){
                        *interface = SOC_PORT_IF_CR4;
                    } else{
                        *interface = SOC_PORT_IF_CAUI4;
                    }
                } else {
                    *interface = SOC_PORT_IF_CR4;
                }
            } else {
                *interface = SOC_PORT_IF_ILKN;
            }
            break;
    }

    return SOC_E_NONE;
}

/* Argument phy_init_flags indicates whether this function is called during the
 * port attach process or not. When phy_init_flags == PORTMOD_INIT_F_ALL_PHYS 
 * it means that this function is being called during the port attach process.
 * In this case the default interface type of a pm4x25 port can be changed here.
 */
STATIC
int _pm4x25_line_side_phymod_interface_get(int unit, int port, 
                                   pm_info_t pm_info, int phy_init_flags,
                                   const portmod_port_interface_config_t *config,
                                   phymod_interface_t *interface_type)
{
    int port_index = 0;
    int is_valid = 0, nof_phys = 0;
    uint32 bitmap;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

   
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index,
                                            &bitmap));

    _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access_arr ,(1+MAX_PHYN),
                                               &nof_phys));

    _SOC_IF_ERR_EXIT(portmod_line_intf_from_config_get(config, &interface));

    /* 5G is 64b/66b encoding in pm4x25 Gen3 */
    /* Change 5G default inteface to SOC_PORT_IF_KR */
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
    if (PORTMOD_PM4x25_IS_GEN3(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
        if (config->speed == 5000) {
            if (interface == SOC_PORT_IF_GMII) {
                interface = SOC_PORT_IF_KR;
            }
        }
    }

    if (nof_phys > 1) {
        /* EXT PHY interface */
        _SOC_IF_ERR_EXIT
            (_pm4x25_ext_phy_interface_check(unit, port, pm_info, &phy_access_arr[nof_phys-1], config, interface,
                                                 &is_valid));
    } else {
        _SOC_IF_ERR_EXIT
            (_pm4x25_interface_check(unit, port, config, interface, &is_valid));
    }

    if (!is_valid) {
         /* interface check validation for internal sedes failed */ 
        LOG_ERROR(BSL_LS_BCM_PORT,
              (BSL_META_UP(unit, port,
               "ERROR: u=%d p=%d interface type %d not supported by internal SERDES for this speed %d \n"),
               unit, port, interface, config->speed));
        _SOC_IF_ERR_EXIT(SOC_E_PARAM);    
    }


    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, config->speed,
                            interface, interface_type));


exit:
    SOC_FUNC_RETURN;
}

int _pm4x25_phymod_interface_mode_set(int unit, int port,pm_info_t pm_info,
                                   const portmod_port_interface_config_t *config,
                                   phymod_phy_inf_config_t *phy_config,
                                   int is_legacy_phy_callback)
{
    int port_index = 0;
    uint32 bitmap;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index,
                                            &bitmap));
    _SOC_IF_ERR_EXIT(PM4x25_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index));

    phy_config->interface_modes = config->interface_modes;

    /*
     * tsce/tscf relies on the interface type and interface mode specified in phy_config
     * to determine the exact interface type written into or read from the registers. For
     * instance, SR4 and KR4 share the same speed ID (reg value) from TSCE's perspective
     * and their difference is the medium type specified in the interface mode. SR4 is
     * optical while KR4 is not. Below we prevent updating the medium type in the port
     * mode provided to the internal PHY if phy_config is to be provided to the internal
     * PHY in the legacy ext PHY callback scenario. Reason is because interface used in
     * the function call below is meant for the outmost PHY instead of the internal PHY.
     * portmod_port_phychain_interface_config_set() has a similar protection mechanism
     * to change the medium type of the port mode based on internal PHY's interface type.
     * However, that only works when the config property "serdes_if_type" is specified.
     */
    if (!is_legacy_phy_callback) {
    _SOC_IF_ERR_EXIT(portmod_media_type_from_port_intf(unit, interface,
                                                       phy_config));
    }
exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pm4x25_port_timestamp_adjust_wb_get(int unit, int port, pm_info_t pm_info,
                                            int speed, portmod_port_ts_adjust_t *ts_adjust)
{
    int rv = 0;
    int port_index, adjust_index;
    uint32_t bitmap;
    portmod_port_ts_adjust_t port_ts_adjust;

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));
    for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM; adjust_index++) {
         rv = PM4x25_PORT_TIME_STAMP_ADJUST_GET(unit, pm_info, &port_ts_adjust, adjust_index, port_index);
         if ((port_ts_adjust.speed == speed) ||
              (port_ts_adjust.speed == -1)) {
             sal_memcpy(ts_adjust, &port_ts_adjust, sizeof(portmod_port_ts_adjust_t));
             break;
         }
    }

    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int port_index = -1, init_all=0;
    int rv = 0, port_i, my_i, found_first_active_lane;
    int phy, ii, nof_phys, fec_enable = 0;
    int phys_count = 0, first_phy = -1, is_interface_restore;
    uint32 pm_is_active = 0;
    uint32 pm_is_bypassed = 0, port_dynamic_state;
    portmod_pbmp_t port_phys_in_pm;
    phymod_phy_init_config_t init_config;
    phymod_phy_inf_config_t *phy_config = &init_config.interface;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_access_t phy_access_lane;
    portmod_port_ability_t port_ability;
    const portmod_port_interface_config_t *config = &add_info->interface_config;
    portmod_port_interface_config_t local_config = add_info->interface_config;
    phymod_interface_t phymod_interface = phymodInterfaceCount;
    soc_port_if_t local_default_interface;
    int is_higig, itf_update = 0, usr_cfg_idx, three_ports_mode;
    int adjust_index = 0;
    pm_version_t pm_version;
    uint32 interface_modes;
    uint32 vsd_active_group[NUM_VSD_ACTIVE_GROUP_WORD]={0};
    portmod_link_recovery_t link_recovery;
    uint32 *pcs_lane_map;
    uint32 txpi_lane_select[MAX_PORTS_PER_PM4X25]={0};
    int pcs_lane;
    SOC_INIT_FUNC_DEFS;

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    if((add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_CPRI) ||
       (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_RSVD4)){
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_SOC_MSG("Requested  %s Encap Type is not supported for port %d"),
                            (add_info->interface_config.encap_mode == _SHR_PORT_ENCAP_CPRI)?"CPRI":"RSVD4",port));
    }


    /* Get the first phy related to this port */
    if (PM_4x25_INFO(pm_info)->in_pm12x10) {
        port_index = 0;
        rv = PM4x25_LANE2PORT_SET(unit, pm_info, port_index, port);
        _SOC_IF_ERR_EXIT(rv);
        PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, PM_4x25_INFO(pm_info)->first_phy){break;} /*just get first phy*/
    } else {
        PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
        PORTMOD_PBMP_AND(port_phys_in_pm, PM_4x25_INFO(pm_info)->phys);
        PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);
    
        ii = 0;
        PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy){
            if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
                rv = PM4x25_LANE2PORT_SET(unit, pm_info, ii, port);
                _SOC_IF_ERR_EXIT(rv);
                first_phy = (first_phy == -1) ? phy : first_phy;
                port_index = (port_index == -1 ? ii : port_index);
            }
            ii++;
        }
        PM_4x25_INFO(pm_info)->first_phy = first_phy;
    }

    if(port_index<0 || port_index>=PM4X25_LANES_PER_CORE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("can't attach port %d. can't find relevant phy"), port));
    }

    /* init data for port configuration */
    rv = PM4x25_IS_HG_SET(unit, pm_info, add_info->init_config.is_hg, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_AN_MODE_SET(unit, pm_info, add_info->init_config.an_mode, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_FS_CL72_SET(unit, pm_info, add_info->init_config.fs_cl72, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_CX4_10G_SET(unit, pm_info, add_info->init_config.cx4_10g, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_AN_CL72_SET(unit, pm_info, add_info->init_config.an_cl72, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_AN_FEC_SET(unit, pm_info, add_info->init_config.an_fec, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_AN_MASTER_LANE_SET(unit, pm_info, add_info->init_config.an_master_lane, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_MAX_SPEED_SET(unit, pm_info, add_info->interface_config.max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_INTERFACE_TYPE_SET(unit, pm_info, add_info->interface_config.interface, port_index);
    _SOC_IF_ERR_EXIT(rv);
    
    rv = PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, add_info->interface_config.interface, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, add_info->interface_config.interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_PORT_PLL_DIVIDER_REQ_SET(unit, pm_info, add_info->init_config.pll_divider_req, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_PORT_1000X_AT_25G_SET(unit, pm_info, add_info->init_config.serdes_1000x_at_25g_vco, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_PORT_10G_AT_25G_SET(unit, pm_info, add_info->init_config.serdes_10g_at_25g_vco, port_index);
    _SOC_IF_ERR_EXIT(rv);

    rv = PM4x25_CPRI_PORT_VSD_ACTIVE_GROUP_SET(unit, pm_info, vsd_active_group, port_index);
    _SOC_IF_ERR_EXIT(rv);

    is_interface_restore = (add_info->interface_config.flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) ? 1 : 0 ;
    rv = PM4x25_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index);
    _SOC_IF_ERR_EXIT(rv);

     for (adjust_index = 0; adjust_index < PORTMOD_TS_ADJUST_NUM; adjust_index++) {
          rv = PM4x25_PORT_TIME_STAMP_ADJUST_SET(unit, pm_info,
                                      &add_info->init_config.port_ts_adjust[adjust_index],
                                      adjust_index, port_index);
          _SOC_IF_ERR_EXIT(rv);
     }

     link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK;
     link_recovery.enabled = TRUE;
     link_recovery.tick_cnt = 0;
     rv = PM4x25_PORT_LINK_RECOVERY_SET(unit, pm_info, link_recovery, port_index);
     _SOC_IF_ERR_EXIT(rv);

    if (PM_4x25_INFO(pm_info)->in_pm12x10) {
        if (add_info->init_config.fw_load_method_overwrite == 1) {
            PM_4x25_INFO(pm_info)->fw_load_method = phymodFirmwareLoadMethodNone;
        }
    } else {
        if (add_info->init_config.polarity_overwrite == 1) {
            sal_memcpy(&PM_4x25_INFO(pm_info)->polarity,
                &add_info->init_config.polarity[0], sizeof(phymod_polarity_t));
        }
        if (add_info->init_config.lane_map_overwrite == 1) {
            sal_memcpy(&PM_4x25_INFO(pm_info)->lane_map,
                &add_info->init_config.lane_map[0], sizeof(phymod_lane_map_t));
        }
        if (add_info->init_config.fw_load_method_overwrite == 1) {
            PM_4x25_INFO(pm_info)->fw_load_method =
                add_info->init_config.fw_load_method[0];
        }
    }

    if (add_info->init_config.ref_clk_overwrite == 1) {
        PM_4x25_INFO(pm_info)->ref_clk = add_info->init_config.ref_clk;
    }

    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    init_all = (!PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info) &&
                !PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) ? 1 : 0;


    if (!PORTMOD_PORT_ADD_F_AUTONEG_CONFIG_SKIP_GET(add_info))
    {
        /* update an information before proceeding further */
        SOC_IF_ERROR_RETURN(_pm4x25_an_mode_update (unit, port, pm_info, port_index, add_info->interface_config.port_num_lanes, add_info->init_config.an_cl73));
    }

    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    /* if not active - initalize PM */
    if(!pm_is_active){
        if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || (init_all)) {
            three_ports_mode = (add_info->init_config.port_mode_aux_info == portmodModeInfoThreePorts) ? 1 : 0;
            rv = PM4x25_THREE_PORTS_MODE_SET(unit, pm_info, three_ports_mode);
            _SOC_IF_ERR_EXIT(rv);

            /*init the PM*/
            rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, add_info, 1);
            _SOC_IF_ERR_EXIT(rv);
        }

        pm_is_active = 1;

        if(PORTMOD_PM4x25_IS_GEN3_28nm(pm_version)){
            /* Set tsc and fclk */
           _SOC_IF_ERR_EXIT(_pm4x25_pm_ext_tsc_clk_set(unit, port, pm_info, add_info));
        }

        rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, pm_is_active);
        _SOC_IF_ERR_EXIT(rv);
    }


    if ((pm_is_active) && (PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) &&
        (PM_4x25_INFO(pm_info)->int_core_access.type == phymodDispatchTypeInvalid)){
        _SOC_IF_ERR_EXIT(_pm4x25_pm_ext_tsc_clk_set(unit, port, pm_info, add_info));
    }

    /* probe core internal serdes and ext phys if there is any. */
    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0){
        /* only at  CORE PROBE phase need probe */
        if (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info) || init_all) {
            rv = _pm4x25_pm_core_probe(unit, port, pm_info, add_info);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

    if (!(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) && 
        (PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_GET(add_info))) {
        return (rv);
    }

    /* probe core and core config internal serdes and ext phys if there is any. */
    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0 ){
        rv = _pm4x25_pm_core_init(unit, port, pm_info, add_info, 1);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        return (rv);
    }

    /* initalize port */
    rv = _pm4x25_pm_port_init(unit, port, pm_info, port_index, add_info, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* initialze phys */
    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0) {

        _SOC_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

        params.phyn = 0;
        _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, &phy_access_lane, &nof_phys, NULL));

        init_config.an_en = add_info->autoneg_en;
        init_config.cl72_en = add_info->link_training_en;
        init_config.op_mode = add_info->interface_config.port_op_mode;

        if (add_info->init_config.an_cl72_tx_init_skip_on_restart) {
            PHYMOD_PHY_INIT_F_AN_CL72_TX_INIT_SKIP_ON_RESTART_SET(&init_config);
        }

        _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                           phy_access ,(1+MAX_PHYN),
                                                           &nof_phys));

        my_i = 0, usr_cfg_idx = 0;
        found_first_active_lane = 0;
        for(ii=0 ; ii<PM4X25_LANES_PER_CORE ; ii++) {
            rv = PM4x25_LANE2PORT_GET(unit, pm_info, ii, &port_i);
            _SOC_IF_ERR_EXIT(rv);

            if(port_i != port) {
                /* There is a difference between XGS and DNX devices. For DNX, a port can consist inconsecutive physical lanes.
                 * For example, a port can consist lane 1 and 3 of the same core. For XGS, on the other hand, a port always consist
                 * consecutive physical lanes. Therefore my_i below needs not be incremented for XGS devices.
                 */
                if(PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_GET(phy_access[0].device_op_mode) && found_first_active_lane){
                    my_i++;
                }
                continue;
            } else{
                found_first_active_lane = 1;
            }

            if(SHR_BITGET(&(PM_4x25_INFO(pm_info)->polarity.tx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.tx_polarity, my_i);
            }

            if(SHR_BITGET(&(PM_4x25_INFO(pm_info)->polarity.rx_polarity),ii)) {
                SHR_BITSET(&init_config.polarity.rx_polarity, my_i);
            }

            _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access_lane, phymodMediaTypeChipToChip, &init_config.tx[my_i]));

            if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                init_config.tx[my_i].pre = add_info->init_config.tx_params[usr_cfg_idx].pre;
                init_config.tx[my_i].main = add_info->init_config.tx_params[usr_cfg_idx].main;
                init_config.tx[my_i].post = add_info->init_config.tx_params[usr_cfg_idx].post;
            }

            if (PORTMOD_USER_SET_TX_AMP_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                init_config.tx[my_i].amp = add_info->init_config.tx_params[usr_cfg_idx].amp;
            }

            if (PORTMOD_USER_SET_TX_PREEMPHASIS_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])
                || PORTMOD_USER_SET_TX_AMP_BY_CONFIG_GET(add_info->init_config.tx_params_user_flag[usr_cfg_idx])) {
                port_dynamic_state = 0;
                PORTMOD_PORT_DEFAULT_TX_PARAMS_UPDATED_SET(port_dynamic_state);

                PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info, port_dynamic_state, port_index);
            }

            usr_cfg_idx++;
            my_i++;
            
        }

        phy_config->data_rate = config->speed;
        phy_config->pll_divider_req = add_info->init_config.pll_divider_req;
        phy_config->ref_clock = PM_4x25_INFO(pm_info)->ref_clk;

        _SOC_IF_ERR_EXIT(_pm4x25_line_side_phymod_interface_get(unit, port, pm_info, PORTMOD_INIT_F_ALL_PHYS, config, &phymod_interface));
        phy_config->interface_type = phymod_interface;
        _SOC_IF_ERR_EXIT(_pm4x25_phymod_interface_mode_set(unit, port, pm_info, config, phy_config, 0));
        rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
        _SOC_IF_ERR_EXIT(rv);
        if (PHYMOD_INTF_MODES_FIBER_GET(phy_config)) {
            interface_modes &= ~(PHYMOD_INTF_MODES_COPPER | PHYMOD_INTF_MODES_BACKPLANE);
            interface_modes |= PHYMOD_INTF_MODES_FIBER;
        } else if (PHYMOD_INTF_MODES_COPPER_GET(phy_config)){
            interface_modes &= ~(PHYMOD_INTF_MODES_FIBER | PHYMOD_INTF_MODES_BACKPLANE);
            interface_modes |= PHYMOD_INTF_MODES_COPPER;
        } else if (PHYMOD_INTF_MODES_BACKPLANE_GET(phy_config)){
            interface_modes &= ~(PHYMOD_INTF_MODES_COPPER | PHYMOD_INTF_MODES_FIBER);
            interface_modes |= PHYMOD_INTF_MODES_BACKPLANE;
        }
        rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
        _SOC_IF_ERR_EXIT(rv);
 
        _SOC_IF_ERR_EXIT(portmod_port_phychain_phy_init(unit, phy_access, nof_phys,
                                                            &init_config));
    }

    /* enable fec if set, should be done before setting the speed */
    fec_enable = add_info->init_config.serdes_fec_enable;

    if (fec_enable) {
        int value = 0;

        if ((fec_enable != PORTMOD_PORT_FEC_CL91) &&
            (fec_enable != PORTMOD_PORT_FEC_CL74) &&
            (fec_enable != PORTMOD_PORT_FEC_CL108)) {
            return (SOC_E_PARAM);
        }

        if (fec_enable == PORTMOD_PORT_FEC_CL91) {
            PHYMOD_FEC_CL91_SET(value);
        } else if (fec_enable == PORTMOD_PORT_FEC_CL108) {
            PHYMOD_FEC_CL108_SET(value);
        }

        value |= TRUE;
        rv = portmod_port_fec_enable_set(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, value);
        _SOC_IF_ERR_EXIT(rv);
    }

    /* The default interface type given by add_info is filled by soc_esw_portctrl_config_get()
     * and should be updated here locally since soc_esw_portctrl_config_get() only gives coarse-
     * grained default interface type without considering whether the underlying device is
     * pm4x10 or pm4x25, with or without ext PHY, etc.
     */
    if (nof_phys == 1) {
        if (config->serdes_interface != SOC_PORT_IF_NULL) {
            _SOC_IF_ERR_EXIT(_pm4x25_interface_check(unit, port, config, config->serdes_interface, &itf_update));
            if (itf_update) {
                local_config.interface = config->serdes_interface;
            }
        }

    is_higig = ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
               (config->encap_mode == SOC_ENCAP_HIGIG) ||
               PHYMOD_INTF_MODES_HIGIG_GET(config)) ? 1 : 0;

        if ((config->speed == 42000 || config->speed == 40000) && is_higig && !itf_update) {
        local_default_interface = SOC_PORT_IF_NULL;
        _SOC_IF_ERR_EXIT(
                pm4x25_port_default_interface_get(unit, port, pm_info, config,
                                                  &local_default_interface));
        if(local_default_interface != SOC_PORT_IF_NULL){
            local_config.interface = local_default_interface;
                itf_update = 1;
            }
        }

        if (itf_update) {
            _SOC_IF_ERR_EXIT(
                PM4x25_INTERFACE_TYPE_SET(unit, pm_info, local_config.interface, port_index));
            _SOC_IF_ERR_EXIT(
                PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, local_config.interface, port_index));
        }
    }
    if (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
        local_config.flags |= PHYMOD_INTF_F_SINGLE_PLL_ONLY;
    }
    rv = pm4x25_port_interface_config_set(unit, port, pm_info, &local_config, PORTMOD_INIT_F_ALL_PHYS);
    _SOC_IF_ERR_EXIT(rv);

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] > 0) {
        if(add_info->init_config.fs_cl72) {
            /* config port_init_cl72 is used to enable fs cl72 only in the serdes,
             * do not enable cl72 on external phys. Set the no of phys argument to
             * 1 to avoid setting cl72 enable on ext phys
             */
            _SOC_IF_ERR_EXIT(portmod_port_phychain_cl72_set(unit, port, phy_access, 1, 1));
        }
    }

    if(!(PM_4x25_INFO(pm_info)->in_pm12x10)) {
        /* set the default advert ability */
        _SOC_IF_ERR_EXIT
            (pm4x25_port_ability_local_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &port_ability));
        _SOC_IF_ERR_EXIT
            (pm4x25_port_ability_advert_set(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &port_ability));
    }

    if (PORTMOD_PM4x25_IS_GEN3_28nm(pm_version) || PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
        /* Set PMD TXPI mode */
        _SOC_IF_ERR_EXIT(_pm4x25_port_phy_txpi_mode_set(unit, port, pm_info, add_info->init_config.txpi_mode));

        /* Set txpi lane select when port is added */
        pcs_lane_map = (uint32 *)add_info->init_config.lane_map[0].lane_map_tx;
        for (pcs_lane = port_index; pcs_lane < (port_index + add_info->interface_config.port_num_lanes); pcs_lane++) {
            txpi_lane_select[pcs_lane_map[pcs_lane]] = port_index;
            _SOC_IF_ERR_EXIT(_pm4x25_txpi_lane_select_set(unit, port, pm_info, txpi_lane_select, pcs_lane_map[pcs_lane]));
        }
    }

    /* De-Assert SOFT_RESET */
    _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
    
    
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable; 
    int invalid_port = -1;
    int tmp_port;
    int i = 0, flags = 0;
    int rv = 0;
    int is_last_one = TRUE;
    int port_index = -1;
    int phytype = 0, nof_phys;
    uint32 inactive = 0;
    phymod_phy_access_t serdes_acc;
    portmod_access_get_params_t params;
#ifdef PHYMOD_TSCF16_SUPPORT
    phymod_phy_reset_t phy_reset;
#endif
#ifdef PORTMOD_CPM4X25_SUPPORT
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    int vco_freq;
    pm_version_t pm_version;
#endif
    SOC_INIT_FUNC_DEFS;

    if (PM_4x25_INFO(pm_info)->nof_phys[0] == 0) {
        /* There are no phys.., so only mac needs to be checked */
        PORTMOD_PORT_ENABLE_MAC_SET(flags);
    } 

    _SOC_IF_ERR_EXIT(pm4x25_port_enable_get(unit, port, pm_info, flags, &enable));
    if(enable){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }
    
    if (!PM_4x25_INFO(pm_info)->in_pm12x10) {
        _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                                     &params, 1, &serdes_acc, &nof_phys, NULL));
    }

    /*remove from array and check if it was the last one*/
    for( i = 0 ; i < MAX_PORTS_PER_PM4X25; i++){
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            port_index = (port_index == -1 ? i : port_index);
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            _SOC_IF_ERR_EXIT(rv);
        } else if (tmp_port != -1){
            is_last_one = FALSE;
        }
    }

    if ((PM_4x25_INFO(pm_info)->nof_phys[0]) && (port_index == -1))  {
        _SOC_EXIT_WITH_ERR(SOC_E_PORT, (_SOC_MSG("Port %d wasn't found"), port));
    }   

    rv = _pm4x25_pm_port_init(unit, port, pm_info, port_index, NULL, 0);
    _SOC_IF_ERR_EXIT(rv);

#ifdef PHYMOD_TSCF16_SUPPORT
    if (!PM_4x25_INFO(pm_info)->in_pm12x10 && (PM_4x25_INFO(pm_info)->int_core_access.type == phymodDispatchTypeTscf16)) {
        phymod_phy_reset_t_init(&phy_reset);
        _SOC_IF_ERR_EXIT(phymod_phy_reset_set(&serdes_acc, &phy_reset));
    }
#endif

    /*deinit PM in case of last one*/
    if(is_last_one){
       rv = _pm4x25_pm_enable(unit, port, pm_info, port_index, NULL, 0);
       _SOC_IF_ERR_EXIT(rv);

       PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(PM_4x25_INFO(pm_info)->int_core_access.device_op_mode);

       rv = PM4x25_IS_ACTIVE_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = PM4x25_IS_INITIALIZED_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, inactive);
       _SOC_IF_ERR_EXIT(rv);

       /* Updating phymod type in pm_info structure and warmboot engine */
       phytype = phymodDispatchTypeInvalid;
       rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[phy_type], &phytype, 0);
       _SOC_IF_ERR_EXIT(rv);
       PM_4x25_INFO(pm_info)->int_core_access.type = phymodDispatchTypeInvalid;

#ifdef PORTMOD_CPM4X25_SUPPORT
       vco_freq = -1; 
       _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
       if(PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
            _SOC_IF_ERR_EXIT(PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, vco_freq));
            _SOC_IF_ERR_EXIT(PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, vco_freq));
       }
#endif
    }
#ifdef PORTMOD_CPM4X25_SUPPORT
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
    if(PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
        _SOC_IF_ERR_EXIT(PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap));
        _SOC_IF_ERR_EXIT(PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap));
        pll0_active_lane_bitmap &= ~(serdes_acc.access.lane_mask);
        pll1_active_lane_bitmap &= ~(serdes_acc.access.lane_mask);
        _SOC_IF_ERR_EXIT(PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap));
        _SOC_IF_ERR_EXIT(PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap));
    }
#endif

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int i, tmp_port;
    int rv = 0;       
    SOC_INIT_FUNC_DEFS;
    

    /* replace old port with new port */
    for(i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
        rv = PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        _SOC_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = PM4x25_LANE2PORT_SET(unit, pm_info, i, new_port);
            _SOC_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int rv = 0, bypass_enable_get;
    uint32 pm_is_active = 0;
    SOC_INIT_FUNC_DEFS;

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &bypass_enable_get);
    _SOC_IF_ERR_EXIT(rv);

    if (bypass_enable_get != bypass_enable) {
        rv = PM4x25_IS_ACTIVE_GET(unit, pm_info, &pm_is_active);
        _SOC_IF_ERR_EXIT(rv);

        if(pm_is_active) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't chenge bypass mode for active pm")));
        }

        rv = PM4x25_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        _SOC_IF_ERR_EXIT(rv);
    }
    
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{   
    SOC_INIT_FUNC_DEFS;
    
    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    if(phyn >= PM_4x25_INFO(pm_info)->nof_phys[0]){ 
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), PM_4x25_INFO(pm_info)->nof_phys[0]-1, phyn));
    }
    if(phyn < 0) {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[0] - 1; /* last one */
    }
    core_info->ref_clk = PM_4x25_INFO(pm_info)->ref_clk;
    sal_memcpy(&core_info->lane_map, &(PM_4x25_INFO(pm_info)->lane_map), sizeof(phymod_lane_map_t));

exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
	SOC_INIT_FUNC_DEFS;
	PORTMOD_PBMP_ASSIGN(*phys, PM_4x25_INFO(pm_info)->phys);
	SOC_FUNC_RETURN;
}

int pm4x25_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{

    uint32 is_bypassed = 0, phychain_flag;  
    int rv = 0, rst_flags = 0;
    int actual_flags = flags;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_power_t phy_power; 
    int nof_phys = 0, clmac_flags = 0;
    SOC_INIT_FUNC_DEFS;


    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

     /* if MAC is set and either RX or TX set is invalid combination */
    if( (PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) ) {
        if((!PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) || (!PORTMOD_PORT_ENABLE_RX_GET(actual_flags))){
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC RX and TX can't be enabled separately")));
        }
    }
    /* phychain_flag is meant to be used if we want only part of the phychin to be affected*/
    phychain_flag = PORTMOD_PORT_ENABLE_INTERNAL_PHY_ONLY_GET(flags) ? 
        PORTMOD_INIT_F_INTERNAL_SERDES_ONLY : 
        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY ;

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    phy_power.rx = phymodPowerNoChange;
    phy_power.tx = phymodPowerNoChange;

    if(PORTMOD_PORT_ENABLE_RX_GET(actual_flags)){
        phy_power.rx = (enable) ? phymodPowerOn : phymodPowerOff;
        clmac_flags |= CLMAC_ENABLE_SET_FLAGS_RX_EN;
    }
    if(PORTMOD_PORT_ENABLE_TX_GET(actual_flags)){
        phy_power.tx = (enable) ? phymodPowerOn : phymodPowerOff;
        clmac_flags |= CLMAC_ENABLE_SET_FLAGS_TX_EN;
    }

    if(enable){
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)){
            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                rst_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
            }
            rv = clmac_enable_set(unit, port, rst_flags, 1);
            _SOC_IF_ERR_EXIT(rv);
            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
            }
        }
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){
            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));

                if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_set(unit, port,  phy_access, nof_phys, phychain_flag,
                                                           phymodTxSquelchOff));
                }
                if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag,
                                                          phymodRxSquelchOff));
                }
            }

    }
    else{
        if((PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) && (!(PM_4x25_INFO(pm_info)->in_pm12x10))){

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

                if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_tx_lane_control_set(unit, port,  phy_access, nof_phys, phychain_flag, 
                                                           phymodTxSquelchOn));
                }
                if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_rx_lane_control_set(unit, port, phy_access, nof_phys, phychain_flag, 
                                                          phymodRxSquelchOn));  
                }

            _SOC_IF_ERR_EXIT (portmod_port_phychain_power_set(unit, port, phy_access, nof_phys, phychain_flag, &phy_power));
        }  
        if((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags))  && (!is_bypassed)){
            rv = clmac_enable_set(unit, port, clmac_flags, 0);
            _SOC_IF_ERR_EXIT(rv);
        }
    }
        
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    phymod_phy_tx_lane_control_t tx_control = phymodTxSquelchOn;
    phymod_phy_rx_lane_control_t rx_control = phymodRxSquelchOn;
    int phy_enable = 1, mac_enable = 1;
    int mac_rx_enable = 0, mac_tx_enable = 0;
    uint32 is_bypassed = 0, phychain_flag;
    int rv;
    int actual_flags = flags;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(actual_flags);
        PORTMOD_PORT_ENABLE_TX_SET(actual_flags);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(actual_flags);
        PORTMOD_PORT_ENABLE_MAC_SET(actual_flags);
    }

    /* phychain_flag is meant to be used if we want only part of the phychin to be affected*/
    phychain_flag = PORTMOD_PORT_ENABLE_INTERNAL_PHY_ONLY_GET(flags) ? 
        PORTMOD_INIT_F_INTERNAL_SERDES_ONLY : 
        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY ;

    if (PORTMOD_PORT_ENABLE_PHY_GET(actual_flags)) {

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_tx_lane_control_get(unit, port, phy_access, nof_phys, phychain_flag, &tx_control));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_rx_lane_control_get(unit, port, phy_access, nof_phys, phychain_flag, &rx_control));

        phy_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            phy_enable |= (rx_control == phymodRxSquelchOn) ? 0 : 1;
        } 
        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            phy_enable |= (tx_control == phymodTxSquelchOn) ? 0 : 1;
        }
    }

    if ((PORTMOD_PORT_ENABLE_MAC_GET(actual_flags)) && (!is_bypassed)) {

        mac_enable = 0;

        if (PORTMOD_PORT_ENABLE_RX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(clmac_enable_get(unit, port, CLMAC_ENABLE_SET_FLAGS_RX_EN, &mac_rx_enable));
            mac_enable |= (mac_rx_enable) ? 1 : 0;
        }
        if (PORTMOD_PORT_ENABLE_TX_GET(actual_flags)) {
            _SOC_IF_ERR_EXIT(clmac_enable_get(unit, port, CLMAC_ENABLE_SET_FLAGS_TX_EN, &mac_tx_enable));
            mac_enable |= (mac_tx_enable) ? 1 : 0;
        }
    }

    *enable = (mac_enable && phy_enable); 

exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_interface_check(int unit, int port, pm_info_t pm_info,
                                const portmod_port_interface_config_t* config,
                                int is_ext_phy, soc_port_if_t interface,
                                int* is_valid)
{
    int nof_phys = 0;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    if (is_ext_phy && nof_phys < 2) {
        return SOC_E_PARAM;
    }

    if (is_ext_phy) {
        _SOC_IF_ERR_EXIT(_pm4x25_ext_phy_interface_check(unit, port, pm_info, &phy_access[nof_phys-1],
                                                         config, interface, is_valid));
    } else {
        _SOC_IF_ERR_EXIT(_pm4x25_interface_check(unit, port, config, interface, is_valid));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_interface_config_set(int unit, int port, pm_info_t pm_info, 
                                     const portmod_port_interface_config_t* config,
                                     int phy_init_flags)
{
    uint32 pm_is_bypassed = 0;
    phymod_phy_inf_config_t phy_config, cur_phy_config;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    int rv = 0, flags = 0;
    int port_index=0;
    int is_higig = 0;
    uint32 bitmap;
    uint32 is_interface_restore, interface_modes;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    phymod_interface_t   phymod_serdes_interface = phymodInterfaceCount;
    phymod_interface_t   phymod_interface = phymodInterfaceCount;
    phymod_tx_t updated_tx_params[PHYMOD_MAX_LANES_PER_CORE];
    int is_legacy_phy_callback = 0, i;
    portmod_port_ts_adjust_t ts_adjust;
    int pll_divider_req = 0;
    int serdes_10g_at_25g_vco = 0;
    int serdes_1000x_at_25g_vco = 0;
    pm_version_t pm_version;
    int32 mac_version = 0;
    uint32 port_dynamic_state;
    int mac_ts_enable;
#ifdef PORTMOD_CPM4X25_SUPPORT
    int pll_idx;
    uint32_t pll_multiplier=0, pll_multiplier0=0, pll_multiplier1=0;
    int pll0_active_lane_bitmap=0, pll1_active_lane_bitmap=0;
    int vco_rate;
#endif
    SOC_INIT_FUNC_DEFS;

    sal_memset(&ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));

    rv = PM4x25_IS_BYPASSED_GET(unit, pm_info, &pm_is_bypassed);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
    _SOC_IF_ERR_EXIT(clmac_version_get(unit, port, &mac_version));
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, &port_dynamic_state, port_index));
    if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, CLMAC_TS_DISABLEf)) {
        _SOC_IF_ERR_EXIT
                (pm4x25_port_mac_timestamp_enable_get(unit, port, pm_info, &mac_ts_enable));
    } else {
        mac_ts_enable = 1;
    }

    if (config->flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
        int serdes_speed = 0, mac_only = 0;

        _SOC_IF_ERR_EXIT(portmod_port_speed_get(unit, port, &serdes_speed));
        if (config->speed == serdes_speed) {
            mac_only = 1;
        }

        flags = CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        _SOC_IF_ERR_EXIT(clmac_speed_set(unit, port, flags, config->speed));

        if (mac_ts_enable) {
            /*Set MAC Timestamp Delay*/
            if (!PORTMOD_PORT_IS_DEFAULT_TIMESTAMP_ADJUSTMENT_UPDATED(port_dynamic_state)) {
                _SOC_IF_ERR_EXIT(_pm4x25_port_timestamp_adjust_wb_get(unit, port, pm_info, config->speed, &ts_adjust));
                _SOC_IF_ERR_EXIT(clmac_timestamp_delay_set(unit, port, ts_adjust));
            }
            /* PM VERSION and CLMAC BYTE ADJUST aren't valid before TH2, QAX_B0 used GEN1_PM + MAC_A027 */
            if (!PORTMOD_PM4x25_IS_GEN1(pm_version) || (CLMAC_VERSION_A027 == mac_version)) {
                _SOC_IF_ERR_EXIT(clmac_timestamp_byte_adjust_set(unit, port, 0, config->speed));
            }
        }
        if (mac_only) {
            /* line side speed and SerDes speed are equal, no need update SerDes configuration */
            SOC_FUNC_RETURN;
        }

        phy_init_flags = PORTMOD_INIT_F_ALL_PHYS;
    }


    if(!(PM_4x25_INFO(pm_info)->in_pm12x10)) {
        _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                         &bitmap));
        if ((config->encap_mode == SOC_ENCAP_HIGIG2) ||
            (config->encap_mode == SOC_ENCAP_HIGIG)  ||
            (PHYMOD_INTF_MODES_HIGIG_GET(config))) {
            rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
            _SOC_IF_ERR_EXIT(rv);
            interface_modes |= PHYMOD_INTF_MODES_HIGIG;
            rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
            _SOC_IF_ERR_EXIT(rv);
        } else if ((config->encap_mode == SOC_ENCAP_IEEE)) {
            rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
            _SOC_IF_ERR_EXIT(rv);
            interface_modes &= ~PHYMOD_INTF_MODES_HIGIG;
            rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        /* Will skip when init, only called when executing command "port xe<n> if=xxxx" */
        if (config->flags & PHYMOD_INTF_F_INTF_PARAM_SET_ONLY) {
            /* only flag saved by interface_config->flags */
            interface = config->interface;

            /* Update the software state of the interface type. Note that currently
             * the legacy ext PHY does not raise the PHYMOD_INTF_F_INTF_PARAM_SET_ONLY
             * flag so that the following function call cannot be reached when a legacy
             * ext PHY uses the portmod callback function to set the interface type
             * of the internal PHY.
             */
                rv = PM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index);
                _SOC_IF_ERR_EXIT(rv);

                is_interface_restore = 1;
                rv = PM4x25_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index);
                _SOC_IF_ERR_EXIT(rv);

            SOC_FUNC_RETURN;
        }

        rv = PM4x25_IS_INTERFACE_RESTORE_GET(unit, pm_info, &is_interface_restore, port_index);
        _SOC_IF_ERR_EXIT(rv);
        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access,(1+MAX_PHYN),
                                           &nof_phys));
        /*
         * if the interface is already set with PARAM_SET_ONLY
         * h/w interface needs to be updated during speed set.
         * So load the interface information and clear the flag;
         */
        /* After command "port xe<n> if=xxxx; port xe<x> sp=xxx" is issued, the following segment of code will be called
         * If ext phy is present, then target interface is retrieved and passed into ext phy interface_set function
         * is_legacy_phy_callback is set as 1 when we want ext phy to update interface, which relays on interface_type;
         * If ext phy is not present, the target interface is retrieved and passed into internal phy interface_config_set function
         * is_legacy_phy_callback is set as 0 when we want internal phy to update interface, which relays on interface_mode
         */
        if(is_interface_restore) {
            rv = PM4x25_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
            _SOC_IF_ERR_EXIT(rv);
            ((portmod_port_interface_config_t*)config)->interface = interface;
            is_interface_restore = 0;
            rv = PM4x25_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index);
            _SOC_IF_ERR_EXIT(rv);
             if ((phy_init_flags == PORTMOD_INIT_F_ALL_PHYS) && (nof_phys > 1)) {
                  is_legacy_phy_callback = 1;
             } else if (nof_phys == 1) {
                 is_legacy_phy_callback = 0;
             }
        } else {
             /* Speed change only */
             if ((phy_init_flags == PORTMOD_INIT_F_ALL_PHYS) && (nof_phys > 1)) {
                  is_legacy_phy_callback = 1;
             }
             /* Internal PHY speed change */
             if (phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
                 is_legacy_phy_callback = 1;
             }
        }
    }

    if (!pm_is_bypassed){
        rv =  portmod_port_higig2_mode_set(unit, port, PHYMOD_INTF_MODES_HIGIG_GET(config));
        _SOC_IF_ERR_EXIT(rv);

        flags = CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
        rv = clmac_speed_set(unit, port, flags, config->speed);
        _SOC_IF_ERR_EXIT(rv);

        if (mac_ts_enable) {
            /*Set MAC Timestamp Delay*/
            if (!PORTMOD_PORT_IS_DEFAULT_TIMESTAMP_ADJUSTMENT_UPDATED(port_dynamic_state)) {
                _SOC_IF_ERR_EXIT(_pm4x25_port_timestamp_adjust_wb_get(unit, port, pm_info, config->speed, &ts_adjust));
                _SOC_IF_ERR_EXIT(clmac_timestamp_delay_set(unit, port, ts_adjust));
            }
            if (!PORTMOD_PM4x25_IS_GEN1(pm_version) || (CLMAC_VERSION_A027 == mac_version)) {
                _SOC_IF_ERR_EXIT(clmac_timestamp_byte_adjust_set(unit, port, 0, config->speed));
            }
        }

        flags = CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS;

        rv = clmac_encap_set(unit, port, 0, PHYMOD_INTF_MODES_HIGIG_GET(config) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE);
        _SOC_IF_ERR_EXIT(rv);

        is_higig =  PHYMOD_INTF_MODES_HIGIG_GET(config) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE;
        rv = clmac_strict_preamble_set(unit, port, (!is_higig && (config->speed >= 10000)));
        _SOC_IF_ERR_EXIT(rv);

        /* De-Assert SOFT_RESET */
        rv = pm4x25_port_soft_reset(unit, port, pm_info, 0);
        _SOC_IF_ERR_EXIT(rv);
    }

    if(!(PM_4x25_INFO(pm_info)->in_pm12x10)){
        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));
        _SOC_IF_ERR_EXIT(_pm4x25_line_side_phymod_interface_get(unit, port, pm_info, phy_init_flags, config, &phymod_interface));
        phy_config.interface_type = phymod_interface;
        /* If is_legacy_phy_callback = 0, then interface mode will be set based on inerface type, which is needed for tscf */
        _SOC_IF_ERR_EXIT(_pm4x25_phymod_interface_mode_set(unit, port, pm_info, config, &phy_config, is_legacy_phy_callback));

        /* interface_mode is updated with phy_config->interface_mode */
        rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
        _SOC_IF_ERR_EXIT(rv);
        if (PHYMOD_INTF_MODES_FIBER_GET(&phy_config)) {
            interface_modes &= ~(PHYMOD_INTF_MODES_COPPER | PHYMOD_INTF_MODES_BACKPLANE);
            interface_modes |= PHYMOD_INTF_MODES_FIBER;
        } else if (PHYMOD_INTF_MODES_COPPER_GET(&phy_config)){
            interface_modes &= ~(PHYMOD_INTF_MODES_FIBER | PHYMOD_INTF_MODES_BACKPLANE);
            interface_modes |= PHYMOD_INTF_MODES_COPPER;
        } else if (PHYMOD_INTF_MODES_BACKPLANE_GET(&phy_config)){
            interface_modes &= ~(PHYMOD_INTF_MODES_COPPER | PHYMOD_INTF_MODES_FIBER);
            interface_modes |= PHYMOD_INTF_MODES_BACKPLANE;
        }
        rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
        _SOC_IF_ERR_EXIT(rv);

        phy_config.data_rate = config->speed;
        phy_config.ref_clock = PM_4x25_INFO(pm_info)->ref_clk;
        rv = PM4x25_PORT_PLL_DIVIDER_REQ_GET(unit, pm_info, &pll_divider_req, port_index);
        _SOC_IF_ERR_EXIT(rv);
        rv = PM4x25_PORT_10G_AT_25G_GET(unit, pm_info, &serdes_10g_at_25g_vco, port_index);
        _SOC_IF_ERR_EXIT(rv);
        rv = PM4x25_PORT_1000X_AT_25G_GET(unit, pm_info, &serdes_1000x_at_25g_vco, port_index);
        _SOC_IF_ERR_EXIT(rv);
        /*The pll div is set to 165 when 10G working at 25G is supported*/
        if (phy_config.data_rate == 10000) {
            if (serdes_10g_at_25g_vco) {
                pll_divider_req = 165;
            } else {
                pll_divider_req = 132;
            }
        }
        if (phy_config.data_rate == 1000) {
            if (serdes_1000x_at_25g_vco) {
                pll_divider_req = 165;
            } else {
                pll_divider_req = 132;
            }
        }
        phy_config.pll_divider_req = pll_divider_req;

        if (config->serdes_interface != SOC_PORT_IF_NULL) {
            _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit,
                             config->speed,
                             config->serdes_interface,
                             &phymod_serdes_interface));
        }

        /* internal phy preemphasis and current */
        if (!(config->flags & (PHYMOD_INTF_F_SET_SPD_TRIGGER | PHYMOD_INTF_F_UPDATE_SPEED_LINKUP))) {

            /* if the tx params are updated from non default value read the non dflt value from the HW and update it */
            rv = PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info,
                                          &port_dynamic_state, port_index);
            _SOC_IF_ERR_EXIT(rv);

            if (!PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(port_dynamic_state)) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                                0, PM_4x25_INFO(pm_info)->ref_clk,
                                                                &cur_phy_config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
                if ((phy_config.data_rate != cur_phy_config.data_rate) ||
                    (phy_config.interface_modes != cur_phy_config.interface_modes)) {
                    for(i=0; i<PHYMOD_MAX_LANES_PER_CORE; i++){
                        _SOC_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access[0], phymodMediaTypeChipToChip,
                                                                      &(updated_tx_params[i])));
                    }
                    _SOC_IF_ERR_EXIT(portmod_port_phychain_tx_set(unit, phy_access, 1, updated_tx_params));
                }
            }
        }

       _SOC_IF_ERR_EXIT
           (portmod_port_phychain_interface_config_set(unit, port, phy_access, nof_phys,
                                                    config->flags,
                                                    &phy_config,
                                                    phymod_serdes_interface,
                                                    PM_4x25_INFO(pm_info)->ref_clk,
                                                    phy_init_flags));

        /* The interface type software state needs to be stored even if the config flags
         * does not include PHYMOD_INTF_F_INTF_PARAM_SET_ONLY. For example, when changing
         * the encap, portctrl tries to config the PHY interface type and speed in one
         * shot without setting the PHYMOD_INTF_F_INTF_PARAM_SET_ONLY flag. However,
         * when the current function is called by the legacy ext PHY through the portmod
         * callback function, the interface type software state should not be modified
         * since PM4x10_INTERFACE_TYPE_GET() should return the outmost PHY interface type.
         */
        if (!(nof_phys > 1 && phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY)) {
            _SOC_IF_ERR_EXIT
                (portmod_intf_from_phymod_intf(unit, phy_config.interface_type, &interface)); 
            _SOC_IF_ERR_EXIT
                (PM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index));
            _SOC_IF_ERR_EXIT
                (PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index));
        }
#ifdef PORTMOD_CPM4X25_SUPPORT
        if(PORTMOD_CPM4x25_IS_GEN3_28nm(pm_version)) {
            pll_idx = phy_access[0].access.pll_idx;
            /* Get pll_multiplier for pll0 */
            phy_access[0].access.pll_idx = 0;
            _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access[0], &pll_multiplier0));
            /* Get pll_multiplier for pll1 */
            phy_access[0].access.pll_idx = 1;
            _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access[0], &pll_multiplier1));
            phy_access[0].access.pll_idx = pll_idx;

            _SOC_IF_ERR_EXIT(_pm4x25_speed_to_vco(unit, config->speed, config->port_num_lanes,
                                                  serdes_1000x_at_25g_vco, serdes_10g_at_25g_vco, &vco_rate, &pll_multiplier));
            /* PLL0 is use for the port */
            if(pll_multiplier == pll_multiplier0) {
                _SOC_IF_ERR_EXIT(PM4x25_PLL0_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll0_active_lane_bitmap));
                pll0_active_lane_bitmap |= (phy_access[0].access.lane_mask);
                _SOC_IF_ERR_EXIT(PM4x25_PLL0_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll0_active_lane_bitmap));
                _SOC_IF_ERR_EXIT(PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, vco_rate));
            /* PLL1 is use for the port */
            } else if(pll_multiplier == pll_multiplier1) {
                _SOC_IF_ERR_EXIT(PM4x25_PLL1_ACTIVE_LANE_BITMAP_GET(unit, pm_info, &pll1_active_lane_bitmap));
                pll1_active_lane_bitmap |= (phy_access[0].access.lane_mask);
                _SOC_IF_ERR_EXIT(PM4x25_PLL1_ACTIVE_LANE_BITMAP_SET(unit, pm_info, pll1_active_lane_bitmap));
                _SOC_IF_ERR_EXIT(PM4x25_PLL1_VCO_RATE_SET(unit, pm_info, vco_rate));
                /* Read PLL0_VCO_RATE from WB variable if not set set it */
                PM4x25_PLL0_VCO_RATE_GET(unit, pm_info, &vco_rate);
                if((pll_multiplier0 == 132) && (vco_rate <=0)) { /* pll0 is set by default to 20G by pm4x25 */
                    vco_rate = portmodVCO20G;
                    _SOC_IF_ERR_EXIT(PM4x25_PLL0_VCO_RATE_SET(unit, pm_info, vco_rate));
                }
            } else {
                _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Right PLL is not selected for port %d"), port));
            }
        }
#endif
    }

exit:
    SOC_FUNC_RETURN;    
}

int pm4x25_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    soc_port_if_t interface;
    phymod_phy_inf_config_t phy_config;
    int           flags = 0, nof_phys = 0, rv, port_index, max_speed;
    uint32_t      bitmap;
    phymod_interface_t           prev_phymod_if;
    SOC_INIT_FUNC_DEFS;

    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("can't get interface config for PM4X25 within PM12X10")));
    }

    if (_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap))
        return (SOC_E_PARAM);
   
    rv = PM4x25_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_OUTMOST_PHY_INTERFACE_TYPE_GET(unit, pm_info, &interface, port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));

    phy_config.data_rate = max_speed;

    _SOC_IF_ERR_EXIT(portmod_intf_to_phymod_intf(unit, max_speed, interface, &prev_phymod_if));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                    0, PM_4x25_INFO(pm_info)->ref_clk,
                                                    &phy_config, phy_init_flags));

    /* It is possible that the underlying phymod interface get API does not
     * distinguish among certain interface types. For example, CR4 and KR4
     * are treated the same from TSCF's perspective. In these cases portmod
     * has to rely on the software state to retrieve the most-recently used
     * interface type written into phymod.
     */
    if (phy_config.interface_type != prev_phymod_if &&
        prev_phymod_if != phymodInterfaceCount &&
        phy_init_flags != PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        phy_config.interface_type = prev_phymod_if;
    }

    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->flags = 0;

    rv = portmod_intf_from_phymod_intf(unit, phy_config.interface_type, &(config->interface));
    _SOC_IF_ERR_EXIT(rv);

    rv = clmac_encap_get (unit, port, &flags, &config->encap_mode);
    _SOC_IF_ERR_EXIT(rv);

    /* coverity[Out-of-bounds access:FALSE] */
    SHR_BITCOUNT_RANGE(&bitmap, config->port_num_lanes, 0, PM4X25_LANES_PER_CORE);
    rv = PM4x25_MAX_SPEED_GET(unit, pm_info, &config->max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &config->interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);


exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_default_interface_get(int unit, int port, pm_info_t pm_info,
                                      const portmod_port_interface_config_t* config,
                                      soc_port_if_t* interface)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0;
    int is_legacy_phy;
    int xphy_id;
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (nof_phys==1) {
        _SOC_IF_ERR_EXIT
            (_pm4x25_default_interface_get(unit, port, config, interface));
    } else {
        xphy_id = phy_access[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

        if (!is_legacy_phy) {
            _SOC_IF_ERR_EXIT
                (portmod_port_phymod_xphy_default_interface_get(unit, &(phy_access[nof_phys-1]),
                                                                config, interface));
        } else {
            interface_config = *config;
            _SOC_IF_ERR_EXIT
                (portmod_common_default_interface_get(&interface_config));
            *interface = interface_config.interface;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_set(unit, port, phy_access, nof_phys, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_get(unit, phy_access, nof_phys, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_cl72_status_get(int unit, int port, pm_info_t pm_info, phymod_cl72_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if (!SAL_BOOT_SIMULATION) {
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_cl72_status_get(unit, phy_access, nof_phys, status));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_loopback_get(int unit, int port, pm_info_t pm_info,
                 portmod_loopback_mode_t loopback_type, int* enable)
{   
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    uint32_t tmp_enable=0;
    phymod_loopback_mode_t phymod_lb_type;
    int rv = PHYMOD_E_NONE;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    switch(loopback_type){
        case portmodLoopbackMacOuter:
            _SOC_IF_ERR_EXIT(clmac_loopback_get(unit, port, loopback_type, enable));
            break;

        case portmodLoopbackPhyGloopPMD:
        case portmodLoopbackPhyRloopPMD: /*slide*/
        case portmodLoopbackPhyGloopPCS: /*slide*/
             if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
                (*enable) = 0; /* No phy --> no phy loopback*/
             } else {
                _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));
                _SOC_IF_ERR_EXIT
                    (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                      phy_access ,(1+MAX_PHYN),
                                                      &nof_phys));

                rv = portmod_port_phychain_loopback_get(unit, port, phy_access,
                                                    nof_phys,
                                                    phymod_lb_type,
                                                    &tmp_enable);
                if (rv == PHYMOD_E_UNAVAIL) {
                    rv = PHYMOD_E_NONE;
                    tmp_enable = 0;
                }

                _SOC_IF_ERR_EXIT(rv);
                *enable = tmp_enable;

             }
             break;
        default:
            (*enable) = 0; /* not supported --> no loopback */
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                 nof_phys;
    int                 is_clmac_in_soft_reset;
    phymod_loopback_mode_t phymod_lb_type;
    int port_index;
    uint32 bitmap;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacOuter:

        /*Get SOFT_RESET status*/
        _SOC_IF_ERR_EXIT(clmac_soft_reset_get(unit, port, &is_clmac_in_soft_reset));

        _SOC_IF_ERR_EXIT(portmod_port_pm_type_get(unit, port, &port, &pm_type));

        /* According to MAC designer - Do MAC SOFT RESET before loopback change */
        if (!is_clmac_in_soft_reset){
            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                /* Since SOFT_RESET sends credits which might cause FIFO overflow, we need to do a procedural pm4x25_port_soft_reset
                 * (with in_out = 1 to put CLMAC in-to SOFT_RESET) */
                _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 1));
            } else {
#ifdef PORTMOD_PM12X10_SUPPORT
                if (enable && (pm_type == portmodDispatchTypePm12x10)) {
                   /*
                    * CLMAC loopback enable
                    * CLMAC_CTRL.SOFT_RESET=1
                    * CLMAC_CTRL.SOFT_RESET=0
                    * CLMAC_CTRL.LOCAL_LPBK=0
                    * CLMAC_CTRL.LOCAL_LPBK=1
                    */
                    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 1));
                    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 0));
                    _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, loopback_type, 0));
                }
#endif
            }
        }
        /*Open / close loopback*/
        _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, loopback_type, enable));


        if (!is_clmac_in_soft_reset){
            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                /* Get SOFT_RESET out-of-reset, using procedural pm4x25_port_soft_reset with in_out = 0 to get CLMAC out-of SOFT_RESET */
                _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
            } else {
#ifdef PORTMOD_PM12X10_SUPPORT
                if ((!enable) && (pm_type == portmodDispatchTypePm12x10)) {
                   /*
                    * CLMAC loopback disable
                    * CLMAC_CTRL.LOCAL_LPBK=0
                    * CLMAC_CTRL.SOFT_RESET=1
                    * CLMAC_CTRL.SOFT_RESET=0
                    */
                    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 1));
                    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 0));
                }
#endif
            }
        }
        break;

    case portmodLoopbackPhyRloopPCS:
    case portmodLoopbackPhyRloopPMD: /*slide*/
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyGloopPCS: /*slide*/
        if(PM_4x25_INFO(pm_info)->nof_phys[port_index] != 0) {
            if (enable) {
                _SOC_IF_ERR_EXIT(clmac_loopback_set(unit, port, portmodLoopbackMacOuter, 0));
            }

            _SOC_IF_ERR_EXIT(portmod_commmon_portmod_to_phymod_loopback_type(unit,
                                                loopback_type, &phymod_lb_type));

            _SOC_IF_ERR_EXIT
                (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                  phy_access ,(1+MAX_PHYN),
                                                  &nof_phys));

            _SOC_IF_ERR_EXIT(portmod_port_phychain_loopback_set(unit, port, phy_access,
                                                                nof_phys,
                                                                phymod_lb_type,
                                                                enable));
         }
         break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (
                  _SOC_MSG("unsupported loopback type %d"), loopback_type));
            break;
    }
    if (enable)
        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));

exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_port_rx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_rx_enable_set(unit, port, enable));
exit:
    SOC_FUNC_RETURN; 
}

int pm4x25_port_rx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_rx_enable_get(unit, port, enable));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_tx_mac_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_tx_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_tx_mac_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_tx_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_ability_local_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, portmod_port_ability_t* ability)
{
    int                 rv;
    portmod_port_ability_t legacy_phy_ability;
    int                 port_index;
    int                 max_speed = 100000;
    int                 num_of_lanes = 0;
    int                 loopback = SOC_PA_LB_PHY | SOC_PA_LB_MAC;
    uint32              bitmap, tmpbit, interface_modes;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, xphy_id, is_legacy_phy;

    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);

    sal_memset(ability, 0, sizeof(portmod_port_ability_t));
    sal_memset(&legacy_phy_ability, 0, sizeof(portmod_port_ability_t));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys));

    _SOC_IF_ERR_EXIT
        (_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));
   
    rv = PM4x25_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
    _SOC_IF_ERR_EXIT(rv);

    tmpbit = bitmap;
    while(tmpbit) {
        num_of_lanes += ((tmpbit&1)?1:0) ;
        tmpbit >>= 1 ;
    }


    if (SAL_BOOT_SIMULATION) {
        loopback = SOC_PA_LB_MAC;
    }
    ability->loopback  = loopback;
    ability->medium    = SOC_PA_MEDIUM_FIBER | SOC_PA_MEDIUM_COPPER | SOC_PA_MEDIUM_BACKPLANE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->fec       = SOC_PA_FEC_NONE | SOC_PA_FEC_CL74 | SOC_PA_FEC_CL91;
    ability->flags     = SOC_PA_AUTONEG;
    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->encap     = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG | SOC_PA_ENCAP_HIGIG2;
    ability->channel   = SOC_PA_CHANNEL_LONG | SOC_PA_CHANNEL_SHORT;

    if (num_of_lanes == 1) {
        if (interface_modes & PHYMOD_INTF_MODES_HIGIG) {
            switch(max_speed) {
                case 10000: max_speed = 11000 ; break ;
                case 20000: max_speed = 21000 ; break ;
                case 25000: max_speed = 27000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        } else {
            ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            switch(max_speed) {
                case 11000: max_speed = 10000 ; break ;
                case 21000: max_speed = 20000 ; break ;
                case 27000: max_speed = 25000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }
        switch(max_speed) {  /* must include all the supported speedss */
            case 27000:
            case 26500:
                ability->speed_full_duplex |= SOC_PA_SPEED_27GB;
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            case 11000:
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            case 10000:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            case 5000:
                ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
            case 2500:
                ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
        if (max_speed >= 10000) {
            ability->interface |= SOC_PA_INTF_XGMII;
        }
        ability->loopback  = loopback;
        ability->flags = SOC_PA_AUTONEG;
    } else if (num_of_lanes == 2) {
        if (interface_modes & PHYMOD_INTF_MODES_HIGIG) {
            switch(max_speed) {
                case 20000: max_speed = 21000 ; break ;
                case 40000: max_speed = 42000 ; break ;
                case 50000: max_speed = 53000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        } else {
            switch(max_speed) {
                case 21000: max_speed = 20000 ; break ;
                case 42000: max_speed = 40000 ; break ;
                case 53000: max_speed = 50000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }
        switch(max_speed) {  /* must include all the supported speedss */
            case 53000:
                ability->speed_full_duplex |= SOC_PA_SPEED_53GB;
            case 50000:
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
            case 42000:
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            default:
                break;
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->loopback  = loopback;
        ability->flags = 0;
    } else {
        if (interface_modes & PHYMOD_INTF_MODES_HIGIG) {
            switch(max_speed) {
                case 40000: max_speed = 42000 ; break ;
                case 50000: max_speed = 53000 ; break ;
                case 100000: max_speed = 106000 ; break ;
                case 120000: max_speed = 127000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        } else {
            switch(max_speed) {
                case 42000: max_speed = 40000 ; break ;
                case 53000: max_speed = 50000 ; break ;
                case 106000: max_speed = 100000 ; break ;
                case 127000: max_speed = 120000 ; break ;
                default:  break ;
            }
            rv = PM4x25_MAX_SPEED_SET(unit, pm_info, max_speed, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }
        switch(max_speed) {
            case 127000:
                ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
            case 120000:
                ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
            case 106000:
                ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            case 100000:
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            case 53000:
                ability->speed_full_duplex |= SOC_PA_SPEED_53GB;
            case 50000:
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
            case 42000:
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            case 40000:
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            default:
                break; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->loopback  = loopback;
        ability->flags     = SOC_PA_AUTONEG;
    }

    /* If legacy external phy presents, retrieve the local ability from the
     * external phy as it has more variants and the ability might vary for each.
     * However, if this function is called by the legacy ext PHY driver through
     * the PORTMOD callback function portmod_ext_to_int_cmd_get(), the returned
     * data should be of the internal SERDES. portmod_ext_to_int_cmd_get() uses
     * PORTMOD_USER_ACC_CMD_FOR_PHY_SET() to hint its callees that the target
     * of data retrieving is the internal SERDES.
     */
    if (nof_phys > 1 && !(phy_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY)) {
        /* check if the legacy phy present */
        xphy_id = phy_access[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
        if(is_legacy_phy) {
            /* call portmod phy chain function to retrieve the ability */
            _SOC_IF_ERR_EXIT
                (portmod_port_phychain_local_ability_get(unit, port, phy_access,
                                                         nof_phys, &legacy_phy_ability));
            /* PHY driver returns all the speeds supported by PHY
             * Advertise only the speeds supported by both PortMacro and the PHY
             */
            legacy_phy_ability.speed_full_duplex &= ability->speed_full_duplex;
            /* PHY driver may not return all supported encap modes
             * Advertise encap modes supported by both PortMacro and the PHY
             */
            legacy_phy_ability.encap |= ability->encap;
            *ability = legacy_phy_ability;
        } else {
            /* We might need to consider calling portmod phy chain for
             * non legacy phys as well*/
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_tscf_ability_local_get:unit=%d p=%d sp=%08x\n"),
              unit, port, ability->speed_full_duplex));

exit:
    SOC_FUNC_RETURN;
}

static int _pm4x25_nof_lanes_get(int unit, int port, pm_info_t pm_info)
{
    int port_index, rv;
    uint32_t bitmap, bcnt = 0;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap);
    if (rv) return (0); /* bit count is zero will flag error */

    while (bitmap) {
        if (bitmap & 0x1) bcnt++;
        bitmap >>= 1;
    }
    return (bcnt);
}

int pm4x25_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                            const phymod_autoneg_control_t* an)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, rv;
    int port_index, fs_cl72;
    uint32_t bitmap, an_done, port_dynamic_state;
    phymod_autoneg_control_t  *pAn = (phymod_autoneg_control_t*)an;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {
        if (pAn->num_lane_adv == 0) {
            pAn->num_lane_adv = _pm4x25_nof_lanes_get(unit, port, pm_info);
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            rv = PM4x25_AN_MODE_GET(unit, pm_info, &pAn->an_mode, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        if (pAn->an_mode == phymod_AN_MODE_NONE) {
            pAn->an_mode = phymod_AN_MODE_SGMII;
        }

        rv = PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info,
                                           &port_dynamic_state, port_index);
        _SOC_IF_ERR_EXIT(rv);

        if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)) {
            rv = PM4x25_AN_MODE_GET(unit, pm_info, &pAn->an_mode, port_index);
            _SOC_IF_ERR_EXIT(rv);

            PORTMOD_PORT_AUTONEG_MODE_UPDATED_CLR(port_dynamic_state);
            rv = PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info,
                                               port_dynamic_state, port_index);
            _SOC_IF_ERR_EXIT(rv);
        }

        /* next chenck if cl72 force bit is set, if set, don't allow AN to */
        /* change cl72 config */
        rv = PM4x25_FS_CL72_GET(unit, pm_info, &fs_cl72, port_index);
        _SOC_IF_ERR_EXIT(rv);
        if (fs_cl72) {
            PHYMOD_AN_F_ALLOW_CL72_CONFIG_CHANGE_CLR(pAn);
        } else {
            PHYMOD_AN_F_ALLOW_CL72_CONFIG_CHANGE_SET(pAn);
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                              phy_access ,(1+MAX_PHYN),
                                              &nof_phys));

        /* In case of warm boot, get autoneg informaton from hardware */
        if(SOC_WARM_BOOT(unit)) {
            if (pAn->an_mode >= phymod_AN_MODE_Count) {
                _SOC_IF_ERR_EXIT
                    (portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, phy_flags, pAn, &an_done));
            }
        }

        rv = portmod_port_phychain_autoneg_set(unit, port, phy_access, nof_phys, phy_flags, an);
        /* Because the returned value PHYMOD_E_UNAVAIL is -12 from tscf phymod driver */
        if (rv == PHYMOD_E_UNAVAIL) {
            rv = SOC_E_UNAVAIL;
        }
        _SOC_IF_ERR_EXIT(rv);
    }
    
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an)
{      
    int rv, port_index, nof_phys;
    uint32_t bitmap, an_done;
    phymod_phy_access_t phy_access[1+MAX_PHYN];

    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("AN isn't supported")));
    } else {

        if (an->num_lane_adv == 0) {
            an->num_lane_adv = _pm4x25_nof_lanes_get(unit, port, pm_info);
        }
        rv = PM4x25_AN_MODE_GET(unit, pm_info, &an->an_mode, port_index);
        _SOC_IF_ERR_EXIT(rv);
        if (an->an_mode == phymod_AN_MODE_NONE) {
            an->an_mode = phymod_AN_MODE_SGMII;
        }

        _SOC_IF_ERR_EXIT
            (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, phy_flags, an, &an_done));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_autoneg_status_get (int unit, int port, pm_info_t pm_info, 
                                    phymod_autoneg_status_t* an_status)
{
    int port_index;
    uint32 bitmap;
    portmod_port_interface_config_t port_intf_config;
    phymod_interface_t phy_intf;

    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT
        (_pm4x25_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("Invalid number of Phys")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_phy_port_autoneg_status_get (unit, port, pm_info, an_status));
   
    _SOC_IF_ERR_EXIT
        (pm4x25_port_interface_config_get(unit, port, pm_info, &port_intf_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

    _SOC_IF_ERR_EXIT
        (portmod_intf_to_phymod_intf(unit, port_intf_config.speed, port_intf_config.interface, &phy_intf));
    
    if (an_status->enabled && an_status->locked) {
        an_status->data_rate = port_intf_config.speed;
        an_status->interface = phy_intf;
    } else {
        /* upper layer should not rely on the 
         *  data rate and interface if the AN is not locked
         */
        an_status->data_rate = 0;
        an_status->interface = SOC_PORT_IF_NULL;        
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_link_get(int unit, int port, pm_info_t pm_info, int flags, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32_t cl91_enable = 0, cl108_enable = 0, an_done = 0;
    uint32 bitmap;
    int port_index=0;
    uint32 interface_modes;
    phymod_autoneg_control_t an;
    portmod_link_recovery_t link_recovery;
    int rv_cl91 = 0, rv_cl108 = 0;
    uint32_t prbs_enable = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(unit, port, phy_access, nof_phys, flags, (uint32_t*) link));
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM4x25_PORT_LINK_RECOVERY_GET(unit, pm_info, &link_recovery, port_index));

    if ((nof_phys == 1) && (link_recovery.enabled)) {
        /* link recovery for internal serdes only for optical application MLD based */
        uint32_t signal_detect;

        /* first get the medium type from internal serdes */
        _SOC_IF_ERR_EXIT(PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index));

        if ((interface_modes & (PHYMOD_INTF_MODES_FIBER | PHYMOD_INTF_MODES_BACKPLANE))) {
            /* Get CL91 */
            PHYMOD_FEC_CL91_SET(cl91_enable);
            rv_cl91 = phymod_phy_fec_enable_get(&phy_access[0], &cl91_enable);
            /* Get CL108 */
            PHYMOD_FEC_CL108_SET(cl108_enable);
            rv_cl108 = phymod_phy_fec_enable_get(&phy_access[0], &cl108_enable);
            if (((rv_cl91 == PHYMOD_E_NONE) && cl91_enable) || ((rv_cl108 == PHYMOD_E_NONE) && cl108_enable)) {
                /* Get AN */
                _SOC_IF_ERR_EXIT
                    (phymod_phy_autoneg_get(&phy_access[0], &an, &an_done));
                /* Pcs link is impacted once prbs test is enabled.
                 * The WAR could be triggered to do link recovery to reset PMA/PMD.
                 * The prbs test may fail in this condition. Make sure this WAR won't be triggered when prbs is enabled.
                 */
                _SOC_IF_ERR_EXIT
                    (phymod_phy_prbs_enable_get(&phy_access[0], 0, &prbs_enable));
                if ((!an.enable) && (!prbs_enable)) {
                    switch (link_recovery.state) {

                            case PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK:
                                /* link is good */
                                if (*link) {
                                    link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD;
                                } else {
                                    /* no link yet, keep wait for link */
                                    link_recovery.tick_cnt += 1;
                                    if ((link_recovery.tick_cnt) > PORTMOD_LINK_RECOVERY_LINK_WAIT_CNT_LIMIT) {
                                        /* no link after all the try, reset the rx patha again */
                                        link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_RESET_RX;
                                    }
                                }
                                break;
                            case PORTMOD_LINK_RECOVERY_STATE_RESET_RX:
                                 link_recovery.tick_cnt = 0;
                                /*first check link status */
                                if (*link) {
                                    link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD;
                                } else {
                                    /* first reset the rx path */
                                    signal_detect = 0;

                                    /* check if there is signal detect */
                                    _SOC_IF_ERR_EXIT(phymod_phy_rx_signal_detect_get(&phy_access[0], &signal_detect));

                                    /* only toggle rx reset path only if there is signal on the line */
                                    if (signal_detect) {
                                         /* first put the rx into reset */
                                         _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access[0], phymodRxDisable));

                                        /* add some delay */
                                        sal_usleep(5000);

                                         /* toggle CDR lock */
                                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access[0], phymodRxSquelchOn));
                                        /* add some delay */
                                        sal_usleep(5000);
                                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access[0], phymodRxSquelchOff));

                                        sal_usleep(10000);
                                        /* release the rx reset */
                                        _SOC_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access[0], phymodRxReset));

                                        sal_usleep(10000);
                                    }
                                    link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_WAIT_FOR_LINK;
                                }
                                break;

                            case PORTMOD_LINK_RECOVERY_STATE_LINK_GOOD:
                                /* no link, need to reset the rx path */
                                if (*link == 0) {
                                    link_recovery.state = PORTMOD_LINK_RECOVERY_STATE_RESET_RX;
                                }
                                break;
                    }
                    _SOC_IF_ERR_EXIT(PM4x25_PORT_LINK_RECOVERY_SET(unit, pm_info, link_recovery, port_index));
                }
            }
        }
    }/* end of link recovery */


exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_link_latch_down_get(int unit, int port, pm_info_t pm_info, uint32 flags, int* link)
{
    int first_index, rv;
    uint32 reg32_val, bitmap;
    soc_field_t status_field, clear_field;
    SOC_INIT_FUNC_DEFS;
    
    *link = 0;

    rv = _pm4x25_port_index_get(unit, port, pm_info, &first_index, &bitmap);
    _SOC_IF_ERR_EXIT(rv);
    
     switch(first_index) {
        case 0:
            status_field = PORT0_LINKSTATUSf;
            clear_field = PORT0_LINKDOWN_CLEARf;
            break;

        case 1:
            status_field = PORT1_LINKSTATUSf;
            clear_field = PORT1_LINKDOWN_CLEARf;
            break;

        case 2:
            status_field = PORT2_LINKSTATUSf;
            clear_field = PORT2_LINKDOWN_CLEARf;
            break;

        case 3:
            status_field = PORT3_LINKSTATUSf;
            clear_field = PORT3_LINKDOWN_CLEARf;
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

    rv = READ_CLPORT_LINKSTATUS_DOWNr(unit, port, &reg32_val);
    _SOC_IF_ERR_EXIT(rv);

    (*link) = soc_reg_field_get(unit, CLPORT_LINKSTATUS_DOWNr, 
                                        reg32_val, status_field);

    if (PORTMOD_PORT_LINK_LATCH_DOWN_F_CLEAR & flags) {

        rv = READ_CLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, &reg32_val);
        _SOC_IF_ERR_EXIT(rv);

        soc_reg_field_set(unit, CLPORT_LINKSTATUS_DOWN_CLEARr, 
                                        &reg32_val, clear_field, 1);

        rv = WRITE_CLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, reg32_val);
        _SOC_IF_ERR_EXIT(rv);

        sal_usleep(10);

        soc_reg_field_set(unit, CLPORT_LINKSTATUS_DOWN_CLEARr, 
                                        &reg32_val, clear_field, 0);

        /* reset default register value */
        rv = WRITE_CLPORT_LINKSTATUS_DOWN_CLEARr(unit, port, reg32_val);
        _SOC_IF_ERR_EXIT(rv);

    }

        
exit:
    SOC_FUNC_RETURN;
}

/*
    The field STRICT_PREAMBLE is used to check preamble+SFD strictly.
    The preamble will be shrinked when some external PHY works at 1G,
    so there are packet errors if STRICT_PREAMBLE is enabled.
*/
int _pm4x25_port_strict_preamble_set(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index=0;
    uint32 bitmap;
    phymod_phy_inf_config_t phy_config;
    int is_higig = 0;
    uint32 interface_modes;
    int strict_preamble_enable=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                         &bitmap));
    _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_config));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                    0, PM_4x25_INFO(pm_info)->ref_clk,
                                                    &phy_config, PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
    _rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
    _SOC_IF_ERR_EXIT(_rv);
    is_higig =  interface_modes & PHYMOD_INTF_MODES_HIGIG ? 1 : 0;
    if (!is_higig && (phy_config.data_rate>= 10000)) {
        strict_preamble_enable = 1;
    }
    _rv = clmac_strict_preamble_set(unit, port, strict_preamble_enable);
    _SOC_IF_ERR_EXIT(_rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_link_up_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _rv = portmod_port_phychain_phy_link_up_event(unit, port, phy_access, nof_phys);
    _SOC_IF_ERR_EXIT(_rv);

    _SOC_IF_ERR_EXIT(_pm4x25_port_strict_preamble_set(unit, port, pm_info));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_link_down_event(int unit, int port, pm_info_t pm_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _rv = portmod_port_phychain_phy_link_down_event(unit, port, phy_access, nof_phys);

exit:
    SOC_FUNC_RETURN;

}

int pm4x25_port_mac_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    uint32 reg_val, bitmap;
    int port_index, phy_acc;
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
   
    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* coverity[callee_ptr_arith:FALSE] */
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, (uint32*)(&bitmap)));

    switch(port_index) {
        case 0:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN0_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN0_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 1:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN1_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN1_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 2:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN2_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN2_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        case 3:
            _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_LN3_STATUS0_REGr(unit, phy_acc, &reg_val));
            (*link) = soc_reg_field_get(unit, CLPORT_XGXS0_LN3_STATUS0_REGr, reg_val, LINK_STATUSf);
            break;

        default:
            _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL,
                     (_SOC_MSG("Port %d, failed to get port index"), port));
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_prbs_config_set(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                const phymod_prbs_t* config)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_config_set(unit, port, pm_info, mode, flags, config));
}

int pm4x25_port_prbs_config_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                phymod_prbs_t* config)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    }

    return (portmod_phy_port_prbs_config_get(unit, port, pm_info, mode, flags, config));
}


int pm4x25_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags,
                                int enable)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_enable_set(unit, port, pm_info, mode, flags, enable));
}

int pm4x25_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags,
                                int* enable)
{ 
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_enable_get(unit, port, pm_info, mode, flags, enable));
}


int pm4x25_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                                portmod_prbs_mode_t mode, int flags, 
                                phymod_prbs_status_t* status)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if(PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    }
    return (portmod_phy_port_prbs_status_get(unit, port, pm_info, mode, flags, status));
}


int pm4x25_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int *nof_lanes)
{
    int temp_nof_lanes = 0;
    SOC_INIT_FUNC_DEFS;

    temp_nof_lanes = _pm4x25_nof_lanes_get(unit, port, pm_info);
    if(0 == temp_nof_lanes){
        SOC_EXIT;
    }
    else{
        *nof_lanes = temp_nof_lanes;
    }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_firmware_mode_set(int unit, int port, pm_info_t pm_info,
                                  phymod_firmware_mode_t fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */
     
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    /* Place your code here */

        
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_runt_threshold_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_runt_threshold_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_runt_threshold_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_runt_threshold_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}



int pm4x25_port_max_packet_size_set(int unit, int port, pm_info_t pm_info, int value)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_rx_max_size_set(unit, port , value));      
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_max_packet_size_get(int unit, int port, pm_info_t pm_info, int* value)
{
        
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_rx_max_size_get(unit, port , value));
exit:
    SOC_FUNC_RETURN; 
    
}


int pm4x25_port_pad_size_set(int unit, int port, pm_info_t pm_info, int value)
{        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    _SOC_IF_ERR_EXIT(clmac_pad_size_set(unit, port , value));
    
exit:
    SOC_FUNC_RETURN;     
}

int pm4x25_port_pad_size_get(int unit, int port, pm_info_t pm_info, int* value)
{        
    SOC_INIT_FUNC_DEFS;
    SOC_NULL_CHECK(pm_info);
    
    _SOC_IF_ERR_EXIT(clmac_pad_size_get(unit, port , value));
        
exit:
    SOC_FUNC_RETURN;     
}


int pm4x25_port_tx_drop_on_local_fault_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    control.drop_tx_on_fault  = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_tx_drop_on_local_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{       
    portmod_local_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;
exit:
    SOC_FUNC_RETURN; 
}


int pm4x25_port_tx_drop_on_remote_fault_set(int unit, int port,
                                            pm_info_t pm_info, int enable)
{        
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_get(unit, port, pm_info, &control));
    control.drop_tx_on_fault = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_tx_drop_on_remote_fault_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;    
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);   
    _SOC_IF_ERR_EXIT(pm4x25_port_remote_fault_control_get(unit, port, pm_info, &control));
    *enable = control.drop_tx_on_fault;
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_remote_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_remote_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port , &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_set(unit, port , &control));       
exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_remote_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_remote_fault_control_t control;   
    SOC_INIT_FUNC_DEFS;

    portmod_remote_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_get(unit, port , &control));
    *enable = control.enable;    
exit:
    SOC_FUNC_RETURN; 
    
}

int pm4x25_port_local_fault_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);

    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    control.enable = enable;
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_set(unit, port, pm_info, &control));
exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_local_fault_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    portmod_local_fault_control_t control;
    SOC_INIT_FUNC_DEFS;

    portmod_local_fault_control_t_init(unit, &control);
    
    _SOC_IF_ERR_EXIT(pm4x25_port_local_fault_control_get(unit, port, pm_info, &control));
    *enable = control.enable;

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_status_get(int unit, int port,
                                       pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_local_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_local_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_local_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_status_get(int unit, int port,
                                        pm_info_t pm_info, int* value)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_status_get(unit, port, 0, value);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_control_set(int unit, int port, pm_info_t pm_info,
                    const portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_control_set(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_remote_fault_control_get(int unit, int port, pm_info_t pm_info,
                    portmod_remote_fault_control_t* control)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = clmac_remote_fault_control_get(unit, port, control);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pause_control_set(int unit, int port, pm_info_t pm_info,
                                  const portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pause_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_pause_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pause_control_get(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pfc_control_set(int unit, int port,
           pm_info_t pm_info, const portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pfc_control_get(int unit, int port, pm_info_t pm_info, portmod_pfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_get(unit, port, control));     
exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_llfc_control_set(int unit, int port,
            pm_info_t pm_info, const portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_set(unit, port, control));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_llfc_control_get(int unit, int port,
            pm_info_t pm_info, portmod_llfc_control_t* control)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_get(unit, port, control));

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_core_access_get (int unit, int port,
                                 pm_info_t pm_info, int phyn, int max_cores,
                                 phymod_core_access_t* core_access_arr,
                                 int* nof_cores,
                                 int* is_most_ext)
{
    int port_index;
    uint32 bitmap;
    int index;
    int done=0;
    int i,j, is_legacy_phy;
    uint32 xphy_id;
    phymod_core_access_t core_access;

    SOC_INIT_FUNC_DEFS;

    if(PM_4x25_INFO(pm_info)->in_pm12x10){
        _SOC_EXIT_WITH_ERR(SOC_E_INTERNAL, (_SOC_MSG("core access get cannot be called for PM4X25 which is part of PM12X10")));
    }

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if(phyn > PM_4x25_INFO(pm_info)->nof_phys[port_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, 
               (_SOC_MSG("phyn exceeded. max allowed %d. got %d"), 
                       PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1, phyn));
    }

    if(phyn < 0)
    {
        phyn = PM_4x25_INFO(pm_info)->nof_phys[port_index] - 1;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    if( phyn == 0 ){
        sal_memcpy(&core_access_arr[0], &(PM_4x25_INFO(pm_info)->int_core_access),
                     sizeof(phymod_core_access_t));
        *nof_cores = 1;
    } else {
        index = 0;
        done = 0;
        for(i=0 ; (i< MAX_PORTS_PER_PM4X25); i++) {
            if(bitmap & (0x1U << i)) {
                xphy_id = PM_4x25_INFO(pm_info)->lane_conn[phyn-1][i].xphy_id;
                if(xphy_id != PORTMOD_XPHY_ID_INVALID){
                    if(index == 0){
                        _SOC_IF_ERR_EXIT(portmod_xphy_core_access_get(unit, xphy_id, &core_access, &is_legacy_phy));
                        sal_memcpy(&core_access_arr[index], &(core_access),
                                   sizeof(phymod_core_access_t));
                        index++;
                    } else {
                        for( j = 0 ; (j < index) &&(!done) ; j++) {
                             if((!sal_memcmp (&core_access_arr[j], &core_access,
                                               sizeof(phymod_core_access_t)))) { /* found a match */
                                done = 1;
                            }
                        }
                        if((!done) && (index < max_cores)){                        
                            sal_memcpy(&core_access_arr[index], &(core_access),
                                       sizeof(phymod_core_access_t));
                            index++;
                        }
                    }
                }
            }
        }
        *nof_cores = index;
    }
    if (is_most_ext) {
        if (phyn == PM_4x25_INFO(pm_info)->nof_phys[port_index]-1) {
            *is_most_ext = 1;
        } else {
            *is_most_ext = 0;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_port_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x25_port_frame_spacing_stretch_set (int unit, int port, 
                                           pm_info_t pm_info,int spacing)
{
    return(clmac_frame_spacing_stretch_set(unit, port, spacing));
}

/*! 
 * pm4x25_port_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int pm4x25_port_frame_spacing_stretch_get (int unit, int port,
                                            pm_info_t pm_info, 
                                            const int *spacing)
{
    return (clmac_frame_spacing_stretch_get(unit, port, (int *)spacing));
}

/*! 
 * pm4x25_port_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int pm4x25_port_diag_fifo_status_get (int unit, int port,pm_info_t pm_info, 
                                 const portmod_fifo_status_t* diag_info)
{
    return(clmac_diag_fifo_status_get(unit, port, diag_info));
}

int _clport_pfc_config_set(int unit, int port, uint32 value)
{
    int rv = SOC_E_UNAVAIL;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,CLPORT_MAC_RSV_MASKr)) {
        rv = READ_CLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* CLPORT_MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            if(value) {
                rval32 &= ~(1 << 18);
            } else {
                rval32 |= (1 << 18);
            }
            rv = WRITE_CLPORT_MAC_RSV_MASKr(unit, port, rval32);
        }
    }

    return rv;
}

int _clport_pfc_config_get(int unit, int port, uint32* value)
{
    int rv = SOC_E_NONE;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,CLPORT_MAC_RSV_MASKr)) {
        rv = READ_CLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* CLPORT_MAC_RSV_MASK: Bit 18 PFC frame detected
             * Enable  PFC Frame : Set 0. Go through
             * Disable PFC Frame : Set 1. Purged.
             */
            *value = ((rval32 & 0x40000) >> 18) ? 0 : 1;
        }
    }

    return rv;
}

/*! 
 * pm4x25_port_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x25_port_pfc_config_set (int unit, int port,pm_info_t pm_info,
                                const portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    if (pfc_cfg->classes != 8) {
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT(clmac_pfc_config_set(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(_clport_pfc_config_set(unit, port, pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN; 
}

/*! 
 * pm4x25_port_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int pm4x25_port_pfc_config_get (int unit, int port,pm_info_t pm_info, 
                                portmod_pfc_config_t* pfc_cfg)
{
    SOC_INIT_FUNC_DEFS;

    pfc_cfg->classes = 8;
    _SOC_IF_ERR_EXIT(clmac_pfc_config_get(unit, port, pfc_cfg));
    _SOC_IF_ERR_EXIT(_clport_pfc_config_get(unit, port, &pfc_cfg->rxpass));
exit:
    SOC_FUNC_RETURN; 
}

/*!
 * pm4x25_port_eee_set
 *
 * @brief set EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_set(int unit, int port, pm_info_t pm_info,const portmod_eee_t* eee)
{
    return (clmac_eee_set(unit, port, eee));
}

/*!
 * pm4x25_port_eee_get
 *
 * @brief get EEE control and timers
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_get(int unit, int port, pm_info_t pm_info,portmod_eee_t* eee)
{
    return (clmac_eee_get(unit, port, eee));
}

/*! 
 * pm4x25_port_vlan_tag_set
 *
 * @brief vlan tag set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_vlan_tag_set(int unit, int port, pm_info_t pm_info,const portmod_vlan_tag_t* vlan_tag)
{
    return (clmac_rx_vlan_tag_set (unit, port, vlan_tag->outer_vlan_tag,
                                   vlan_tag->inner_vlan_tag));
}


/*! 
 * pm4x25_port_vlan_tag_get
 *
 * @brief vlan tag get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_vlan_tag_get(int unit, int port, pm_info_t pm_info, portmod_vlan_tag_t* vlan_tag)
{
    return (clmac_rx_vlan_tag_get (unit, port, (int*)&vlan_tag->outer_vlan_tag,
                                   (int*)&vlan_tag->inner_vlan_tag));
}

/*! 
 * pm4x25_port_duplex_set
 *
 * @brief duplex set. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable        - 
 */
int pm4x25_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return (clmac_duplex_set (unit, port, enable)); 
}


/*! 
 * pm4x25_port_duplex_get
 *
 * @brief duplex get. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  vlan_tag        - 
 */
int pm4x25_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    return (clmac_duplex_get (unit, port, enable)); 
}

/*!
 * pm4x25_port_speed_get
 *
 * @brief duplex get.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  speed         -
 */
int pm4x25_port_speed_get(int unit, int port, pm_info_t pm_info, int* speed)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t config;
    int nof_phys, rv;

    rv = portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys);
    if (rv) return(rv);

    rv = phymod_phy_interface_config_get(&phy_access[0], 0,0,  &config);
    if (rv) return (rv);

    *speed = config.data_rate;
    return (rv);
}

int pm4x25_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_read(unit, port, phy_access,
                                        nof_phys, lane, flags, reg_addr,value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_reg_write(unit, port, phy_access,
                                         nof_phys, lane, flags, reg_addr,value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_reset_set (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int direction)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if (PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    } 
    return(portmod_phy_port_reset_set (unit, port, pm_info, reset_mode, opcode, direction));
}

int pm4x25_port_reset_get (int unit, int port, pm_info_t pm_info, 
                           int reset_mode, int opcode, int* direction)
{
    int port_index, rv;
    uint32 bitmap;

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
         return (rv);
    }

    if (PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        return(SOC_E_UNAVAIL);
    } 
    return(portmod_phy_port_reset_get (unit, port, pm_info, reset_mode, opcode, direction));
}

/*Port remote Adv get*/
int pm4x25_port_adv_remote_get (int unit, int port, pm_info_t pm_info, 
                                int* ability_mask)
{
    return (0);
}

/*get port fec enable*/
int pm4x25_port_fec_enable_get(int unit, int port, pm_info_t pm_info, 
                               uint32 phy_flags,  uint32_t* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_fec_enable_get(unit, port, phy_access, nof_phys, phy_flags, enable));

exit:
    SOC_FUNC_RETURN;
}

/*set port fec enable*/
int pm4x25_port_fec_enable_set(int unit, int port, pm_info_t pm_info, 
                               uint32 phy_flags, uint32_t enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_fec_enable_set(unit, port, phy_access, nof_phys, phy_flags, enable));

exit:
    SOC_FUNC_RETURN;
}

/*get port auto negotiation local ability*/
int pm4x25_port_ability_advert_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags,
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;
    phymod_autoneg_ability_t    an_ability;
    portmod_port_ability_t port_ability;
    int phy_index, xphy_id, is_legacy_phy = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_get(unit, port, phy_access,
                                                       nof_phys, phy_flags, &an_ability,
                                                       &port_ability));
    phy_index = nof_phys -1;
    if (phy_index) {
        xphy_id = phy_access[phy_index].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
        if (is_legacy_phy) {
            sal_memcpy(ability, &port_ability, sizeof(portmod_port_ability_t));
        }
    } 
    if (!is_legacy_phy) {
        portmod_common_phy_to_port_ability(&an_ability, ability);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *     _pm4x25_port_to_phy_ability
 * Purpose:
 *      This function will translate the portAbility request, which passed
 *      from BCM api to anAbility format required by phymod api.
 *
 * Parameters:
 *      portAbility         -(IN)  port an ability request from BCM Api
 *      anAbility           -(OUT) phymod an ability to phymod api.
 *      port_num_lanes      -(IN)  number of lane
 *      line_interface      -(IN)  line interface
 *      cx4_10g             -(IN)  cx4 port
 *      an_cl72             -(IN)  an cl72 configuration
 *      an_fec              -(IN)  fec configuration
 *      an_hg_mode          -(IN)  hg_mode
 * Returns:
 *      None
 */
void _pm4x25_port_to_phy_ability (portmod_port_ability_t *portAbility,
                                  phymod_autoneg_ability_t *anAbility,
                                  int port_num_lanes,
                                  int line_interface,
                                  int cx4_10g,
                                  int an_cl72,
                                  int an_fec,
                                  int hg_mode,
                                  phymod_an_mode_type_t an_mode)
{
    uint32_t                  an_tech_ability   = 0;
    uint32_t                  an_bam37_ability  = 0;
    uint32_t                  an_bam73_ability  = 0;
    _shr_port_mode_t          speed_full_duplex = portAbility->speed_full_duplex;

    phymod_autoneg_ability_t_init(anAbility);

    /*
     * an_tech_ability
     */
    if (port_num_lanes == 4) {

        if (speed_full_duplex & SOC_PA_SPEED_100GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR4 ) {
                    PHYMOD_AN_CAP_100G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_100G_KR4_SET(an_tech_ability);
                }
            }
        }

        if (speed_full_duplex & SOC_PA_SPEED_40GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR4) {
                    PHYMOD_AN_CAP_40G_CR4_SET(an_tech_ability);
                } else {
                    PHYMOD_AN_CAP_40G_KR4_SET(an_tech_ability);
                }
            }
        }

        if (speed_full_duplex & SOC_PA_SPEED_10GB) {
            PHYMOD_AN_CAP_10G_KX4_SET(an_tech_ability);
        }

    } else if (port_num_lanes == 1) {
        /* When in MSA AN mode : We do not want to populate the base page with IEEE mode.
          * If we do, then IEEE takes the higher priority. Hence we add the following exception
          * block for each 25G case below: if(phymod_AN_MODE_MSA!=an_mode)  */
        if ((speed_full_duplex & SOC_PA_SPEED_25GB) && (phymod_AN_MODE_MSA != an_mode)) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    if (portAbility->channel) {
                        if (portAbility->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                        if (portAbility->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                    } else {
                        PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                        PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                    }
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    if (portAbility->channel) {
                        if (portAbility->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        }
                        if (portAbility->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        }
                    } else {
                        PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                        PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                    }
                } else {
                    /*
                     *  This is essentially a Fiber mode which is not
                     *  a valid case for normal operation, but port
                     *  but during intialization time, it will
                     *  use fiber mode as it is default.
                     */
                    if (portAbility->channel) {
                        if (portAbility->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                        if (portAbility->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                    } else {
                        PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                        PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                    }
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR ) {
                    if (portAbility->channel) {
                        if (portAbility->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        }
                        if (portAbility->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                            PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                        }
                    } else {
                        PHYMOD_AN_CAP_25G_CR1_SET(an_tech_ability);
                        PHYMOD_AN_CAP_25G_CRS1_SET(an_tech_ability);
                    }
                } else {
                    if (portAbility->channel) {
                        if (portAbility->channel & SOC_PA_CHANNEL_SHORT) {
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                        if (portAbility->channel & SOC_PA_CHANNEL_LONG) {
                            PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                            PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                        }
                    } else {
                        PHYMOD_AN_CAP_25G_KR1_SET(an_tech_ability);
                        PHYMOD_AN_CAP_25G_KRS1_SET(an_tech_ability);
                    }
                }
            }
        }

        if (speed_full_duplex & SOC_PA_SPEED_10GB) {
            PHYMOD_AN_CAP_10G_KR_SET(an_tech_ability);
        }

        if (speed_full_duplex & SOC_PA_SPEED_1000MB) {
            PHYMOD_AN_CAP_1G_KX_SET(an_tech_ability);
        }

        if (speed_full_duplex & SOC_PA_SPEED_5000MB) {
            PHYMOD_AN_CAP_5G_KR1_SET(an_tech_ability);
        }
        if (speed_full_duplex & SOC_PA_SPEED_2500MB) {
            PHYMOD_AN_CAP_2P5G_X_SET(an_tech_ability);
        }
    }

    /*
     * an_bam73_ability
     */
    if (port_num_lanes == 4) {
        if (speed_full_duplex & SOC_PA_SPEED_50GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_bam73_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_50G_CR4_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_bam73_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR4 ) {
                    PHYMOD_BAM_CL73_CAP_50G_CR4_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_50G_KR4_SET(an_bam73_ability);
                }
            }
        }
    } else if (port_num_lanes == 2) {
        if (speed_full_duplex & SOC_PA_SPEED_20GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR2) {
                    PHYMOD_BAM_CL73_CAP_20G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_20G_KR2_SET(an_bam73_ability);
                }
            }
        }
        if (speed_full_duplex & SOC_PA_SPEED_50GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR2) {
                    PHYMOD_BAM_CL73_CAP_50G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_50G_KR2_SET(an_bam73_ability);
                }
            }
        }
        if (speed_full_duplex & SOC_PA_SPEED_40GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR2) {
                    PHYMOD_BAM_CL73_CAP_40G_CR2_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_40G_KR2_SET(an_bam73_ability);
                }
            }
        }
    } else {
        /* port_num_lanes == 1 */
        if (speed_full_duplex & SOC_PA_SPEED_25GB) {
            if (portAbility->medium) {
                if (portAbility->medium & SOC_PA_MEDIUM_BACKPLANE) {
                    PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
                } else if (portAbility->medium & SOC_PA_MEDIUM_COPPER) {
                    PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
                }
            } else {
                if (line_interface == SOC_PORT_IF_CR) {
                    PHYMOD_BAM_CL73_CAP_25G_CR1_SET(an_bam73_ability);
                } else {
                    PHYMOD_BAM_CL73_CAP_25G_KR1_SET(an_bam73_ability);
                }
            }

        }
    }

    /*
     * an_bam37_ability is not supported in PM4x25 (TSCF)
     */

    anAbility->an_cap = an_tech_ability;
    anAbility->cl73bam_cap = an_bam73_ability;
    anAbility->cl37bam_cap = an_bam37_ability;

    switch (portAbility->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(anAbility);
        break;
    case SOC_PA_PAUSE_RX:
        /* an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE; */
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(anAbility);
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(anAbility);
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(anAbility);
        break;
    }

    if ((speed_full_duplex & SOC_PA_SPEED_10MB) ||
        (speed_full_duplex & SOC_PA_SPEED_100MB) ||
        (speed_full_duplex & SOC_PA_SPEED_1000MB)) {
        if (portAbility->medium) {
            if (!(portAbility->medium & SOC_PA_MEDIUM_FIBER)) {
                PHYMOD_AN_CAP_SGMII_SET(anAbility);
            } else {
                if (portAbility->speed_full_duplex & SOC_PA_SPEED_100MB) {
                    PHYMOD_AN_CAP_100M_SET(anAbility->an_cap);
                }
            }
        } else {
            if (SOC_PORT_IF_SGMII == line_interface) {
                PHYMOD_AN_CAP_SGMII_SET(anAbility);
            } else {
                if (portAbility->speed_full_duplex & SOC_PA_SPEED_100MB) {
                    PHYMOD_AN_CAP_100M_SET(anAbility->an_cap);
                }
            }
        }
    }

    /* next check if we need to set cl37 attribute */
    if (an_cl72) {
        anAbility->an_cl72 = 1;
    }

    if (hg_mode) {
        anAbility->an_hg2 = 1;
    }

    /*
     * check if Cl74 fec is requested from AN BCM api.
     * SOC_PA_FEC_NONE mean that FEC be off.  If that bit
     * is set, it will ignore both FEC_CL74 and FEC_CL91 setting.
     */
    if (portAbility->fec & SOC_PA_FEC_NONE) {
        PHYMOD_AN_FEC_OFF_SET(anAbility->an_fec);
    } else {
        if ((an_fec == 1) || (portAbility->fec & SOC_PA_FEC_CL74)) {
            /* The 'phy_an_fec=1' and FEC CL74 ability are all the phymod FEC CL74. */
            PHYMOD_AN_FEC_CL74_SET(anAbility->an_fec);
        }

        if ((an_fec == 2) || (portAbility->fec & SOC_PA_FEC_CL91)) {
            /* The 'phy_an_fec=2' and FEC CL91 ability are all the phymod FEC CL74. */
            PHYMOD_AN_FEC_CL91_SET(anAbility->an_fec);
        }
    }

}

int pm4x25_port_ability_advert_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                                 portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys = 0, port_index = 0, rv = 0;    
    int port_num_lanes, line_interface, cx4_10g;
    int an_cl72, an_fec, hg_mode;    
    portmod_port_ability_t      port_ability;
    phymod_autoneg_ability_t    an_ability;
    uint32_t bitmap = 0, interface_modes;
    int                       line_side;
    phymod_an_mode_type_t  an_mode;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));   

    _SOC_IF_ERR_EXIT
        (pm4x25_port_ability_local_get(unit, port, pm_info, phy_flags, &port_ability));

    /* Make sure to advertise only abilities supported by the port */
    port_ability.pause             &= ability->pause;
    port_ability.interface         &= ability->interface;
    port_ability.medium            &= ability->medium;
    port_ability.eee               &= ability->eee;
    port_ability.loopback          &= ability->loopback;
    port_ability.flags             &= ability->flags;
    port_ability.speed_half_duplex &= ability->speed_half_duplex;
    port_ability.speed_full_duplex &= ability->speed_full_duplex;
    port_ability.fec               &= ability->fec;
    port_ability.channel           &= ability->channel;

    /* coverity[Out-of-bounds access:FALSE] */
    SHR_BITCOUNT_RANGE(&bitmap, port_num_lanes, 0, PM4X25_LANES_PER_CORE);
    rv = PM4x25_CX4_10G_GET(unit, pm_info, &cx4_10g, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_AN_CL72_GET(unit, pm_info, &an_cl72, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_AN_FEC_GET(unit, pm_info, &an_fec, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_TYPE_GET(unit, pm_info, &line_interface, port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes,
                                    port_index);
    _SOC_IF_ERR_EXIT(rv);
    rv = PM4x25_AN_MODE_GET(unit,pm_info,&an_mode,port_index);
    hg_mode = (interface_modes & PHYMOD_INTF_MODES_HIGIG)? 1: 0;
    
    /* 
     * Get the number of line side lanes.
     */

    line_side = TRUE ;
    _SOC_IF_ERR_EXIT
        (pm4x25_port_lane_count_get(unit, port, pm_info,
                                    line_side, &port_num_lanes));
    
    _pm4x25_port_to_phy_ability(&port_ability, &an_ability,
                                       port_num_lanes, line_interface, cx4_10g,
                                an_cl72, an_fec, hg_mode, an_mode);

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    rv = PM4x25_AN_MASTER_LANE_GET(unit, pm_info, &an_ability.an_master_lane, port_index);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_ability_set(unit, port, phy_access,
                                          nof_phys, phy_flags, &an_ability,
                                          &port_ability));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                     "Speed(HD=0x%08x, FD=0x%08x) Pause=0x%08x orig(HD=0x%08x, FD=0x%08x) \n"
                     "Interface=0x%08x Medium=0x%08x Loopback=0x%08x Flags=0x%08x\n"),
                     port_ability.speed_half_duplex,
                     port_ability.speed_full_duplex,
                     port_ability.pause,
                     ability->speed_half_duplex,
                     ability->speed_full_duplex,
                     port_ability.interface,
                     port_ability.medium, port_ability.loopback,
                     port_ability.flags));
exit:
    SOC_FUNC_RETURN;
}

/*Port ability remote Adv get*/
int pm4x25_port_ability_remote_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, 
                                       portmod_port_ability_t* ability)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int                         nof_phys;
    phymod_autoneg_ability_t    an_ability;
    int phy_index, xphy_id, is_legacy_phy = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                          phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    phy_index = nof_phys -1;
    if (phy_index) {
        xphy_id = phy_access[phy_index].access.addr;
        _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
    }
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_remote_ability_get(unit, port,  
                    phy_access, nof_phys, phy_flags, &an_ability, ability));
    if (!is_legacy_phy) {
    portmod_common_phy_to_port_ability(&an_ability, ability);
    }

exit:
    SOC_FUNC_RETURN;
}

int _clport_rx_control_set(int unit, int port, int value)
{
    int rv = SOC_E_UNAVAIL;
    uint32 rval32 = 0;

    if (SOC_REG_IS_VALID(unit,CLPORT_MAC_RSV_MASKr)) {
        rv = READ_CLPORT_MAC_RSV_MASKr(unit, port, &rval32);
        if (SOC_SUCCESS(rv)) {
            /* CLPORT_MAC_RSV_MASK: Bit 11 Control Frame recieved
             * Enable  Control Frame : Set 0. Packet go through
             * Disable Control Frame : Set 1. Packet is purged.
             */
            if(value) {
                rval32 &= ~(1 << 11);
            } else {
                rval32 |= (1 << 11);
            }
            rv = WRITE_CLPORT_MAC_RSV_MASKr(unit, port, rval32);
        }
    }

    return rv;
}

int pm4x25_port_rx_control_set (int unit, int port, pm_info_t pm_info,
                                const portmod_rx_control_t* rx_ctrl)
{
    int phy_acc;
    uint32 reg_val;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_RSV_MASKr(unit, phy_acc, &reg_val));

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_CONTROL_FRAME) {
        SOC_IF_ERROR_RETURN(clmac_pass_control_frame_set(unit, port, rx_ctrl->pass_control_frames));

        if(rx_ctrl->pass_control_frames){
            reg_val &= ~(1 << 11);
        } else{
            reg_val |= (1 << 11);
        }
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PFC_FRAME) {
        SOC_IF_ERROR_RETURN(clmac_pass_pfc_frame_set(unit, port, rx_ctrl->pass_pfc_frames));

        if(rx_ctrl->pass_pfc_frames){
            reg_val &= ~(1 << 18);
        } else{
            reg_val |= (1 << 18);
        }
    }

    if (rx_ctrl->flags & PORTMOD_MAC_PASS_PAUSE_FRAME) {
        SOC_IF_ERROR_RETURN(clmac_pass_pause_frame_set(unit, port, rx_ctrl->pass_pause_frames));

        if(rx_ctrl->pass_pause_frames){
            reg_val &= ~(1 << 12);
        } else{
            reg_val |= (1 << 12);
        }
    }

    SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_RSV_MASKr(unit, phy_acc, reg_val));

    return (SOC_E_NONE);
}

int pm4x25_port_rx_control_get (int unit, int port, pm_info_t pm_info,
                                portmod_rx_control_t* rx_ctrl)
{
    int control, pause, pfc;

    SOC_IF_ERROR_RETURN(clmac_pass_control_frame_get(unit, port, &control));
    SOC_IF_ERROR_RETURN(clmac_pass_pause_frame_get(unit, port, &pause));
    SOC_IF_ERROR_RETURN(clmac_pass_pfc_frame_get(unit, port, &pfc));

    rx_ctrl->pass_control_frames = (uint32)control;
    rx_ctrl->pass_pause_frames   = (uint32)pause;
    rx_ctrl->pass_pfc_frames     = (uint32)pfc;  return (SOC_E_NONE);
}

int pm4x25_port_tx_average_ipg_set (int unit, int port, pm_info_t pm_info, int value)
{
    return (clmac_tx_average_ipg_set(unit, port, value));
}

int pm4x25_port_tx_average_ipg_get (int unit, int port, pm_info_t pm_info, int* value)
{
    return (clmac_tx_average_ipg_get(unit, port, value));
}

/*Port tx taps set\get*/
int pm4x25_port_tx_set(int unit, int port, pm_info_t pm_info, const phymod_tx_t* tx)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT(phymod_phy_tx_set(&phy_access[0], tx));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_tx_get(int unit, int port, pm_info_t pm_info, phymod_tx_t* tx)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int   nof_phys = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT(phymod_phy_tx_get(&phy_access[0], tx));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int link, flags = 0;
    int duplex = 0, tx_pause = 0, rx_pause = 0;
    phymod_autoneg_status_t autoneg;
    portmod_pause_control_t pause_control;
    portmod_port_ability_t local_advert, remote_advert;
    portmod_port_interface_config_t interface_config;
    int port_index, ref_clock, max_speed;
    uint32_t                    bitmap;
    phymod_phy_inf_config_t     phy_interface_config;
    soc_port_if_t   interface = SOC_PORT_IF_NULL;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, is_clmac_en;
    portmod_port_ts_adjust_t ts_adjust;
    pm_version_t pm_version;
    int32 mac_version = 0;
    uint32 port_dynamic_state = 0;
    int mac_ts_enable;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_autoneg_status_t_init(&autoneg));
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    sal_memset(&local_advert, 0, sizeof(portmod_port_ability_t));
    sal_memset(&remote_advert, 0, sizeof(portmod_port_ability_t));
    sal_memset(&ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    if(update_control->link_status == -1) {
        _SOC_IF_ERR_EXIT(pm4x25_port_link_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &link));
    } else {
        link = update_control->link_status;
    }

    _SOC_IF_ERR_EXIT(pm4x25_port_autoneg_status_get(unit, port, pm_info, &autoneg));

    /*
     * Update the interface type stored in the SW database when the port is in the AN mode
     * and link is up. The update has to be done before any possible early return in this
     * function or portmod_port_interface_config_get() will get outdated interface type
     * causing "BCM.0> ps" to show outdated interface types in the AN mode.
     */
    if (link && autoneg.enabled && autoneg.locked) {
        _SOC_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&phy_interface_config));
        _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));
        _SOC_IF_ERR_EXIT(PM4x25_MAX_SPEED_GET(unit, pm_info, &max_speed, port_index));

        ref_clock = PM_4x25_INFO(pm_info)->ref_clk;
        phy_interface_config.data_rate =  max_speed;
        phy_interface_config.ref_clock =  ref_clock;
        _SOC_IF_ERR_EXIT
            (portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                                        0, ref_clock, &phy_interface_config,
                                                        PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
        _SOC_IF_ERR_EXIT
            (portmod_intf_from_phymod_intf(unit, phy_interface_config.interface_type, &interface));
        _SOC_IF_ERR_EXIT
            (PM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index));
        _SOC_IF_ERR_EXIT
            (PM4x25_OUTMOST_PHY_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index));
    }

    if (PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK_GET(update_control)){
        /*
         * PORTMOD_PORT_UPDATE_F_UPDATE_SERDES_LINK is set for XGS devices. Since XGS devices
         * already update their MAC registers elsewhere, there is an early return below to
         * avoid redundant works.
         */
        return (SOC_E_NONE);
    }

    if(!link) {
        /* PHY is down.  Disable the MAC. */
        if (PORTMOD_PORT_UPDATE_F_DISABLE_MAC_GET(update_control)) {
            _SOC_IF_ERR_EXIT(clmac_enable_set(unit, port, 0, 0));
        }
    } else {
        if (PORTMOD_PORT_UPDATE_F_DISABLE_MAC_GET(update_control)) {
            _SOC_IF_ERR_EXIT(clmac_enable_get(unit, port, 0, &is_clmac_en));
            if (is_clmac_en == 0) {
                _SOC_IF_ERR_EXIT(clmac_enable_set(unit, port, 0, 1));
                if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                    _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
                }
            }
        }
        /* In case of AN - need to update MAC settings*/
        if(autoneg.enabled && autoneg.locked) {
            _SOC_IF_ERR_EXIT(pm4x25_port_interface_config_get(unit, port, pm_info, &interface_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
            /* update MAC */
            flags = CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS;
            _SOC_IF_ERR_EXIT(clmac_speed_set(unit, port, flags, interface_config.speed));

            /*Set MAC Timestamp Delay*/
            if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, CLMAC_TS_DISABLEf)) {
                _SOC_IF_ERR_EXIT
                    (pm4x25_port_mac_timestamp_enable_get(unit, port, pm_info, &mac_ts_enable));
            } else {
                mac_ts_enable = 1;
            }
            if (mac_ts_enable) {
                _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, &bitmap));
                _SOC_IF_ERR_EXIT(PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, &port_dynamic_state, port_index));
                if (!PORTMOD_PORT_IS_DEFAULT_TIMESTAMP_ADJUSTMENT_UPDATED(port_dynamic_state)) {
                    _SOC_IF_ERR_EXIT(_pm4x25_port_timestamp_adjust_wb_get(unit, port, pm_info, interface_config.speed, &ts_adjust));
                    _SOC_IF_ERR_EXIT(clmac_timestamp_delay_set(unit, port, ts_adjust));
                }
                _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
                _SOC_IF_ERR_EXIT(clmac_version_get(unit, port, &mac_version));
                if (!PORTMOD_PM4x25_IS_GEN1(pm_version) || (CLMAC_VERSION_A027 == mac_version)) {
                    _SOC_IF_ERR_EXIT(clmac_timestamp_byte_adjust_set(unit, port, 0, interface_config.speed));
                }
            }

            if(PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
                _SOC_IF_ERR_EXIT(pm4x25_port_soft_reset(unit, port, pm_info, 0));
            }
            /* update pause in MAC on the base of local and remote pause ability*/
            _SOC_IF_ERR_EXIT(pm4x25_port_duplex_get(unit, port, pm_info, &duplex));
            _SOC_IF_ERR_EXIT(pm4x25_port_ability_advert_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &local_advert));
            _SOC_IF_ERR_EXIT(pm4x25_port_ability_remote_get(unit, port, pm_info, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &remote_advert));
            /* IEEE 802.3 Flow Control Resolution.
                   * Please see $SDK/doc/pause-resolution.txt for more information. */
            if (duplex) {
                tx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    !(remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    (local_advert.pause & SOC_PA_PAUSE_TX)); 
                rx_pause = \
                    ((remote_advert.pause & SOC_PA_PAUSE_RX) && \
                    (local_advert.pause & SOC_PA_PAUSE_RX)) || \
                    ((local_advert.pause & SOC_PA_PAUSE_RX) && \
                    (remote_advert.pause & SOC_PA_PAUSE_TX) && \
                    !(local_advert.pause & SOC_PA_PAUSE_TX));
            }
            else {
                rx_pause = tx_pause = 0;
            }
            _SOC_IF_ERR_EXIT(pm4x25_port_pause_control_get(unit, port, pm_info, &pause_control));
            if ((pause_control.rx_enable != rx_pause) || (pause_control.tx_enable != tx_pause)) {
                pause_control.rx_enable = rx_pause;
                pause_control.tx_enable = tx_pause;
                _SOC_IF_ERR_EXIT(pm4x25_port_pause_control_set(unit, port, pm_info, &pause_control));
            }
        }
        _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_lag_failover_status_toggle(unit, port, pm_info));
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_drv_name_get (int unit, int port, pm_info_t pm_info, 
                              char* buf, int len)
{
    strncpy(buf, "PM4X25 Driver", len);
    return (SOC_E_NONE);
}

int pm4x25_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (lcl_fault) {
        SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_set(unit, port, pm_info, 0, rmt_fault)); 
    }
    return (pm4x25_port_clear_rx_lss_status_set (unit, port, pm_info, 1, rmt_fault));

}

int pm4x25_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int lcl_fault, rmt_fault;

    SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_get (unit, port, pm_info, &lcl_fault, &rmt_fault));
    if (rmt_fault) {
        SOC_IF_ERROR_RETURN (pm4x25_port_clear_rx_lss_status_set(unit, port, pm_info, lcl_fault, 0)); 
    }
    return (pm4x25_port_clear_rx_lss_status_set (unit, port, pm_info, lcl_fault, 1));
}

int pm4x25_port_clear_rx_lss_status_set (int unit, soc_port_t port, 
                           pm_info_t pm_info, int lcl_fault, int rmt_fault)
{
    return (clmac_clear_rx_lss_status_set (unit, port, lcl_fault, rmt_fault));
}

int pm4x25_port_clear_rx_lss_status_get (int unit, soc_port_t port, 
                           pm_info_t pm_info, int *lcl_fault, int *rmt_fault)
{
    return (clmac_clear_rx_lss_status_get (unit, port, lcl_fault, rmt_fault));
}

int pm4x25_port_lag_failover_status_toggle (int unit, soc_port_t port, pm_info_t pm_info)
{
    int phy_acc;
    uint32_t rval;
    int link = 0;
    soc_timeout_t to;

    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* Link status to IPIPE is updated by H/W and driven based on both the
     * rising edge of CLPORT_LAG_FAILOVER_CONFIG.LINK_STATUS_UP and
     * actual link up status from Serdes.
     * In some loopback scenarios it may take longer time to see Serdes link up status.
     */
    soc_timeout_init(&to, 10000, 0);

    do {
        _SOC_IF_ERR_EXIT(pm4x25_port_mac_link_get(unit, port, pm_info, &link));
        if (link) {
            break;
        }
    } while(!soc_timeout_check(&to));

    /* Toggle link_status_up bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, &rval));
    soc_reg_field_set(unit, CLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));
    soc_reg_field_set(unit, CLPORT_LAG_FAILOVER_CONFIGr, &rval, LINK_STATUS_UPf, 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_LAG_FAILOVER_CONFIGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_lag_failover_loopback_set (int unit, soc_port_t port,
                                        pm_info_t pm_info, int value)
{
    return (clmac_lag_failover_loopback_set(unit, port, value));
}

int pm4x25_port_lag_failover_loopback_get (int unit, soc_port_t port,
                                        pm_info_t pm_info, int *value)
{
    return(clmac_lag_failover_loopback_get(unit, port, value));
}

int pm4x25_port_tx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_tx_mac_sa_set(unit, port, mac_addr));
}

int pm4x25_port_tx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_tx_mac_sa_get(unit, port, mac_addr));
}

int pm4x25_port_rx_mac_sa_set(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_rx_mac_sa_set(unit, port, mac_addr));
}

int pm4x25_port_rx_mac_sa_get(int unit, int port, pm_info_t pm_info, sal_mac_addr_t mac_addr)
{
    return (clmac_rx_mac_sa_get(unit, port, mac_addr));
}

int pm4x25_port_mode_get(int unit, soc_port_t port,
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int port_index, rv, phy_acc;
    uint32 bitmap;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* coverity[callee_ptr_arith:FALSE] */
    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (rv) return (rv);

    return (_clport_mode_get(unit, phy_acc, port_index, &mode->cur_mode, &mode->lanes));
}

static int _clport_mode_set(int unit, int phy_acc, portmod_core_port_mode_t cur_mode)
{
    uint32_t rval;
    int      mode;

    SOC_INIT_FUNC_DEFS;

    switch (cur_mode) {
        case portmodPortModeQuad:
            mode = 0;
            break;

        case portmodPortModeTri012:
            mode = 1;
            break;

        case portmodPortModeTri023:
            mode = 2;
            break;

        case portmodPortModeDual:
            mode = 3;
            break;

        case portmodPortModeSingle:
            mode = 4;
            break;

        default:
            return SOC_E_FAIL;
    }

    /* Toggle link bit to notify IPIPE on link up */
    _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &rval));

    soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf, mode);
    soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf, mode);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_MODE_REGr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_mode_set(int unit, soc_port_t port,
                     pm_info_t pm_info, const portmod_port_mode_info_t* mode)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(_clport_mode_set(unit, phy_acc, mode->cur_mode));

exit:
    SOC_FUNC_RETURN;
}

int
pm4x25_port_trunk_hwfailover_config_set(int unit, soc_port_t port, pm_info_t pm_info, int hw_count)
{
    int    old_failover_en=0, new_failover_en = 0;
    int    old_link_status_sel=0, new_link_status_sel=0;
    int    old_reset_flow_control=0, new_reset_flow_control = 0;
    int    lag_failover_lpbk;

    if (hw_count) {
        new_failover_en        = 1;
        new_link_status_sel    = 1;
        new_reset_flow_control = 1;
    }

    SOC_IF_ERROR_RETURN (clmac_lag_failover_loopback_get(unit, port, &lag_failover_lpbk));
    if (lag_failover_lpbk) return (SOC_E_NONE);


    SOC_IF_ERROR_RETURN (clmac_lag_failover_en_get(unit, port, &old_failover_en));
    SOC_IF_ERROR_RETURN (clmac_sw_link_status_select_get (unit, port, &old_link_status_sel));

    if (old_failover_en     != new_failover_en ||
        old_link_status_sel != new_link_status_sel) {

        SOC_IF_ERROR_RETURN (clmac_sw_link_status_select_set (unit, port, new_link_status_sel));
        SOC_IF_ERROR_RETURN (clmac_lag_failover_en_set(unit, port, new_failover_en));
    }

    SOC_IF_ERROR_RETURN (clmac_reset_fc_timers_on_link_dn_get(unit,
                                       port, &old_reset_flow_control));
    if (old_reset_flow_control != new_reset_flow_control) {
        SOC_IF_ERROR_RETURN (clmac_reset_fc_timers_on_link_dn_set(unit,
                                        port, new_reset_flow_control));
    }

    return 0;
}

int
pm4x25_port_trunk_hwfailover_config_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *enable)
{
    return (clmac_lag_failover_en_get(unit, port, enable));
}


int
pm4x25_port_trunk_hwfailover_status_get(int unit, soc_port_t port, pm_info_t pm_info, 
                                        int *loopback)
{
    return (clmac_lag_failover_loopback_get(unit, port, loopback));
}

int pm4x25_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    portmod_access_get_params_t params;


    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));

    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { /* most external is default */  
        params.phyn = -1 ;  
    } 

    if( PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ) {
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* most external is default */
        params.sys_side = PORTMOD_SIDE_LINE ;
    }

    SOC_IF_ERROR_RETURN(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                    &params, 1, phy_access, &nof_phys, NULL));

    return (portmod_phy_port_diag_ctrl(unit, port, phy_access, nof_phys, inst, op_type, op_cmd, arg));
}

int
pm4x25_port_ref_clk_get(int unit, soc_port_t port, pm_info_t pm_info, int *ref_clk)
{
    *ref_clk = pm_info->pm_data.pm4x25_db->ref_clk;

    return (SOC_E_NONE);
}

int pm4x25_port_lag_remove_failover_lpbk_get(int unit, int port, pm_info_t pm_info, int *val)
{
    return (clmac_lag_remove_failover_lpbk_get(unit, port, val));
}

int pm4x25_port_lag_remove_failover_lpbk_set(int unit, int port, pm_info_t pm_info, int val)
{
    return (clmac_lag_remove_failover_lpbk_set(unit, port, val));
}

int pm4x25_port_lag_failover_disable(int unit, int port, pm_info_t pm_info)
{
    return (clmac_lag_failover_disable(unit, port));
}

int pm4x25_port_mac_ctrl_set(int unit, int port,
                   pm_info_t pm_info, uint64 ctrl)
{
    return (clmac_mac_ctrl_set(unit, port, ctrl));
}

int pm4x25_port_drain_cell_get(int unit, int port, 
           pm_info_t pm_info, portmod_drain_cells_t *drain_cells)
{
    return (clmac_drain_cell_get(unit, port, drain_cells));
}

int pm4x25_port_drain_cell_stop (int unit, int port, 
           pm_info_t pm_info, const portmod_drain_cells_t *drain_cells)
{
    return (clmac_drain_cell_stop (unit, port, drain_cells));
               
}

int pm4x25_port_drain_cell_start(int unit, int port, pm_info_t pm_info)
{
    return (clmac_drain_cell_start(unit, port));
}

int pm4x25_port_txfifo_cell_cnt_get(int unit, int port,
                       pm_info_t pm_info, uint32* fval)
{
    return (clmac_txfifo_cell_cnt_get(unit, port, fval));
}

int pm4x25_port_egress_queue_drain_get(int unit, int port,
                   pm_info_t pm_info, uint64 *ctrl, int *rx)
{
    return (clmac_egress_queue_drain_get(unit, port, ctrl, rx));
}

int pm4x25_port_drain_cells_rx_enable(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (clmac_drain_cells_rx_enable(unit, port, rx_en));
}

int pm4x25_port_egress_queue_drain_rx_en(int unit, int port,
                   pm_info_t pm_info, int rx_en)
{
    return (clmac_egress_queue_drain_rx_en(unit, port, rx_en));
}

int pm4x25_port_mac_reset_set(int unit, int port,
                   pm_info_t pm_info, int val)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    /* if callback defined, go to local soft reset function */
    rv = (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) ? pm4x25_port_soft_reset(unit, port, pm_info, val) : clmac_soft_reset_set(unit, port, val);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_mac_reset_get(int unit, int port, pm_info_t pm_info, int* val)
{
    return (clmac_soft_reset_get(unit, port, val));
}

int pm4x25_port_mac_reset_check(int unit, int port, pm_info_t pm_info,
                                int enable, int* reset)
{
    return (clmac_reset_check(unit, port, enable, reset));
}

int pm4x25_port_core_num_get(int unit, int port, pm_info_t pm_info,
                                int* core_num)
{
    *core_num = PM_4x25_INFO(pm_info)->core_num;
    return (SOC_E_NONE);
}

/* in_out: 1= in reset, 0= in and out of reset */
STATIC
int pm4x25_port_soft_reset(int unit, int port, pm_info_t pm_info, int in_out)
{
    int rv, rx_enable = 0;
    int phy_port, block, bindex, i;
    portmod_drain_cells_t drain_cells;
    uint32 cell_count;
    uint64 mac_ctrl;
    soc_timeout_t to;
    SOC_INIT_FUNC_DEFS;

    /* Callback soft reset function */
    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePre);
        _SOC_IF_ERR_EXIT(rv);
    }

    if (in_out == 0) {
	    rv = clmac_egress_queue_drain_get(unit, port, &mac_ctrl, &rx_enable); 
	    _SOC_IF_ERR_EXIT(rv);

	    /* Drain cells */
	    rv = clmac_drain_cell_get(unit, port, &drain_cells);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Start TX FIFO draining */
	    rv = clmac_drain_cell_start(unit, port);
	    _SOC_IF_ERR_EXIT(rv);

	    /* De-assert SOFT_RESET to let the drain start */
	    rv = clmac_soft_reset_set(unit, port, 0);           
	    _SOC_IF_ERR_EXIT(rv);

	    /* Wait until TX fifo cell count is 0 */
	    if (!SAL_BOOT_SIMULATION) {
	        soc_timeout_init(&to, 250000, 0);
	        for (;;) {
	            rv = clmac_txfifo_cell_cnt_get(unit, port, &cell_count);
	            _SOC_IF_ERR_EXIT(rv);
	            if (cell_count == 0) {
	                break;
	            }
	            if (soc_timeout_check(&to)) {
	                LOG_ERROR(BSL_LS_SOC_PORT,(BSL_META_UP(unit, port, "ERROR: u=%d p=%d timeout draining TX FIFO (%d cells remain)\n"), unit, port, cell_count));
	                return SOC_E_INTERNAL;
	            }
	        }
	    }
   
	    /* Stop TX FIFO draining */
	    rv = clmac_drain_cell_stop(unit, port, &drain_cells);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Reset port FIFO */
	    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
	    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
	        block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        	if ((SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLPORT) ||
        	    (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLG2PORT)) {
	            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
        	    _SOC_IF_ERR_EXIT(PM_DRIVER(pm_info)->f_portmod_port_soft_reset_toggle(unit, port, pm_info, bindex));
	            break;
        	}
    	    }
    }

    /* Put port into SOFT_RESET */
    rv = clmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

    if(in_out == 0) {

	    /* Callback soft reset function */
    	    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        	rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypeDuring);
	        	_SOC_IF_ERR_EXIT(rv);
	        	soc_reg64_field32_set (unit, CLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0); /*make sure restored data wont put mac back in reset*/
    	    }

	    /* Enable TX, restore RX, de-assert SOFT_RESET */
	    rv = clmac_egress_queue_drain_rx_en(unit, port, rx_enable);
	    _SOC_IF_ERR_EXIT(rv);

	    /* Restore clmac_CTRL to original value */
	    rv = clmac_mac_ctrl_set(unit, port, mac_ctrl);
	    _SOC_IF_ERR_EXIT(rv);
    }

    /* Callback soft reset function */
    if (PM_4x25_INFO(pm_info)->portmod_mac_soft_reset) {
        rv = PM_4x25_INFO(pm_info)->portmod_mac_soft_reset(unit, port, portmodCallBackActionTypePost);
        _SOC_IF_ERR_EXIT(rv);
    }
 
exit:
    SOC_FUNC_RETURN;
}

/*Port discard set*/
int pm4x25_port_discard_set(int unit, int port, pm_info_t pm_info, int discard)
{
    return(clmac_discard_set(unit, port, discard));
}

/*Port soft reset set set*/
int pm4x25_port_soft_reset_set(int unit, int port, pm_info_t pm_info, int idx,
                               int val, int flags)
{
    return (_pm4x25_port_soft_reset_set(unit, port, pm_info, idx, val));
}

/*Port tx_en=0 and softreset mac*/
int pm4x25_port_tx_down(int unit, int port, pm_info_t pm_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(clmac_tx_enable_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(clmac_discard_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(clmac_soft_reset_set(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pgw_reconfig (int unit, int port, pm_info_t pm_info, 
                              const portmod_port_mode_info_t *pmode,
                              int phy_port, int flags)
{
    int rv, idx=0, physical_port, i;
    int phy_acc, block;
    uint32 reg_val;
    soc_field_t port_fields[] = {PORT0f, PORT1f, PORT2f, PORT3f};

    SOC_INIT_FUNC_DEFS; 

    physical_port = SOC_INFO(unit).port_l2p_mapping[port];
    for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
    	block = SOC_PORT_IDX_BLOCK(unit, physical_port, i);
   	if ((SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLPORT) ||
   	    (SOC_BLOCK_INFO(unit, block).type == SOC_BLK_CLG2PORT)) {
        	idx = SOC_PORT_IDX_BINDEX(unit, physical_port, i);
            	break;
        }
    }
    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    /* 1.a st SOFT_RESET field in CLMAC_CTRL for new ports that use XLMAC */
    rv = clmac_soft_reset_set(unit, port, 1);
    _SOC_IF_ERR_EXIT(rv);

    /* 1.a set PORT[subport] field in CLPORT_SOFT_RESET for all new ports */
    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

    /* Port enable */
    _SOC_IF_ERR_EXIT(READ_CLPORT_ENABLE_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set (unit, CLPORT_ENABLE_REGr, &reg_val,
                           port_fields[idx], 1);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_ENABLE_REGr(unit, phy_acc, reg_val));

 
    /* set port mode */

    /* Release CLPORT SOFT RESET */
    _SOC_IF_ERR_EXIT(READ_CLPORT_SOFT_RESETr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &reg_val, port_fields[idx], 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_SOFT_RESETr(unit, phy_acc, reg_val));

    /*Release Mac Soft Reset */
    rv = clmac_soft_reset_set(unit, port, 0);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm4x25_port_interrupt_enable_get (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 *val)
{
    uint32 reg_val;
    int phy_acc;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MIB_TX_MEM_ERRf);
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MIB_RX_MEM_ERRf);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_TX_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_RX_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxTsCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_RX_TS_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, TSC_ERRf);
             break;
        case portmodIntrTypePmdErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, PMD_ERRf);
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, RX_FLOWCONTROL_REQ_FULLf);
             break;
        case portmodIntrTypeLinkdown:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, LINK_DOWNf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacErr:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_ENABLEr, reg_val, MAC_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTxPktUnderflow :
        case portmodIntrTypeTxPktOverflow :
        case portmodIntrTypeTxLlfcMsgOverflow :
        case portmodIntrTypeTxTSFifoOverflow :
        case portmodIntrTypeRxPktOverflow :
        case portmodIntrTypeRxMsgOverflow :
        case portmodIntrTypeTxCdcSingleBitErr :
        case portmodIntrTypeTxCdcDoubleBitErr :
        case portmodIntrTypeRxCdcSingleBitErr :
        case portmodIntrTypeRxCdcDoubleBitErr :
        case portmodIntrTypeRxTsMemSingleBitErr :
        case portmodIntrTypeRxTsMemDoubleBitErr :
        case portmodIntrTypeLocalFaultStatus :
        case portmodIntrTypeRemoteFaultStatus :
        case portmodIntrTypeLinkInterruptionStatus :
        case portmodIntrTypeTsEntryValid :
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                 _SOC_IF_ERR_EXIT(clmac_interrupt_enable_get(unit, port, intr_type, val));
                 break;
             }
             else {
                 return SOC_E_UNAVAIL;
             }
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/* set interrupt value. */
int pm4x25_port_interrupt_enable_set (int unit, int port, pm_info_t pm_info,
                                    int intr_type, uint32 val)
{
    uint32 reg_val;
    int phy_acc;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_ENABLEr(unit, phy_acc, &reg_val));
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MIB_TX_MEM_ERRf, val);
             break;
        case portmodIntrTypeMibRxMemErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MIB_RX_MEM_ERRf, val);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_TX_CDC_MEM_ERRf, val);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_RX_CDC_MEM_ERRf, val);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxTsCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_RX_TS_CDC_MEM_ERRf, val);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTscErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, TSC_ERRf, val);
             break;
        case portmodIntrTypePmdErr:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, PMD_ERRf, val);
             break;
        case portmodIntrTypeRxFcReqFull:
             soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, RX_FLOWCONTROL_REQ_FULLf, val);
             break;
        case portmodIntrTypeLinkdown:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, LINK_DOWNf, val);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacErr:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                soc_reg_field_set(unit, CLPORT_INTR_ENABLEr, &reg_val, MAC_ERRf, val);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTxPktUnderflow :
        case portmodIntrTypeTxPktOverflow :
        case portmodIntrTypeTxLlfcMsgOverflow :
        case portmodIntrTypeTxTSFifoOverflow :
        case portmodIntrTypeRxPktOverflow :
        case portmodIntrTypeRxMsgOverflow :
        case portmodIntrTypeTxCdcSingleBitErr :
        case portmodIntrTypeTxCdcDoubleBitErr :
        case portmodIntrTypeRxCdcSingleBitErr :
        case portmodIntrTypeRxCdcDoubleBitErr :
        case portmodIntrTypeRxTsMemSingleBitErr :
        case portmodIntrTypeRxTsMemDoubleBitErr :
        case portmodIntrTypeLocalFaultStatus :
        case portmodIntrTypeRemoteFaultStatus :
        case portmodIntrTypeLinkInterruptionStatus :
        case portmodIntrTypeTsEntryValid :
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                _SOC_IF_ERR_EXIT(clmac_interrupt_enable_set(unit, port, intr_type, val));
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_INTR_ENABLEr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/* get interrupt value. */
int pm4x25_port_interrupt_get (int unit, int port, pm_info_t pm_info,
                               int intr_type, uint32* val)
{
    uint32 reg_val;
    int phy_acc;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    switch(intr_type) {
        case portmodIntrTypeMibTxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf);
             break;
        case portmodIntrTypeMibRxMemErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf);
             break;
        case portmodIntrTypeMacTxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacRxTsCdcMemErr:
             if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_TS_CDC_MEM_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTscErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, TSC_ERRf);
             break;
        case portmodIntrTypePmdErr:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, PMD_ERRf);
             break;
        case portmodIntrTypeRxFcReqFull:
             *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf);
             break;
        case portmodIntrTypeLinkdown:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, LINK_DOWNf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeMacErr:
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                *val = soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_ERRf);
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        case portmodIntrTypeTxPktUnderflow :
        case portmodIntrTypeTxPktOverflow :
        case portmodIntrTypeTxLlfcMsgOverflow :
        case portmodIntrTypeTxTSFifoOverflow :
        case portmodIntrTypeRxPktOverflow :
        case portmodIntrTypeRxMsgOverflow :
        case portmodIntrTypeTxCdcSingleBitErr :
        case portmodIntrTypeTxCdcDoubleBitErr :
        case portmodIntrTypeRxCdcSingleBitErr :
        case portmodIntrTypeRxCdcDoubleBitErr :
        case portmodIntrTypeRxTsMemSingleBitErr :
        case portmodIntrTypeRxTsMemDoubleBitErr :
        case portmodIntrTypeLocalFaultStatus :
        case portmodIntrTypeRemoteFaultStatus :
        case portmodIntrTypeLinkInterruptionStatus :
        case portmodIntrTypeTsEntryValid :
             if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
                _SOC_IF_ERR_EXIT(clmac_interrupt_status_get(unit, port, intr_type, val));
                break;
             }
             else {
                return SOC_E_UNAVAIL;
             }
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_interrupts_get (int unit, int port, pm_info_t pm_info,
                                int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 reg_val, cnt=0;
    int phy_acc;
    uint32 mac_intr_cnt = 0;
    pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(READ_CLPORT_INTR_STATUSr(unit, phy_acc, &reg_val));
    _SOC_IF_ERR_EXIT(_pm4x25_pm_version_get(unit, pm_info, &pm_version));

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_TX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibTxMemErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MIB_RX_MEM_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibRxMemErr;
    }

    if(PORTMOD_PM4x25_IS_GEN1(pm_version)) {
        if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_TX_CDC_MEM_ERRf)) {
             if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
             }
             intr_arr[cnt++] = portmodIntrTypeMacTxCdcMemErr;
        }

        if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_CDC_MEM_ERRf)) {
             if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
             }
             intr_arr[cnt++] = portmodIntrTypeMacRxCdcMemErr;
        }

        if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_RX_TS_CDC_MEM_ERRf)) {
             if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
             }
             intr_arr[cnt++] = portmodIntrTypeMacRxTsCdcMemErr;
        }
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, TSC_ERRf)) {
        intr_arr[cnt++] = portmodIntrTypeTscErr;
    }

    if (soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, RX_FLOWCONTROL_REQ_FULLf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxFcReqFull;
    }

    if(!PORTMOD_PM4x25_IS_GEN1(pm_version)) {
        if(soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, LINK_DOWNf)) {
             if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
             }
             intr_arr[cnt++] = portmodIntrTypeLinkdown;
        }

        if(soc_reg_field_get(unit, CLPORT_INTR_STATUSr, reg_val, MAC_ERRf)) {
             if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
             }
             intr_arr[cnt++] = portmodIntrTypeMacErr;
        }

        _SOC_IF_ERR_EXIT(clmac_interrupts_status_get(unit, port, (arr_max_size- cnt), (intr_arr + cnt), &mac_intr_cnt));
        cnt += mac_intr_cnt;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_port_eee_clock_set
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             -
 */
int pm4x25_port_eee_clock_set(int unit, int port, pm_info_t pm_info,
                              const portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_CLOCK_GATEr(unit, phy_acc, eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, eee_clk->timer_pulse));

exit:
    SOC_FUNC_RETURN;
}


/*!
 * portmod_port_eee_clock_get
 *
 * @brief set EEE Config.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  eee             -
 */
int pm4x25_port_eee_clock_get (int unit, int port, pm_info_t pm_info,
                               portmod_eee_clock_t* eee_clk)
{
    int phy_acc;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_CLOCK_GATEr(unit, phy_acc, &eee_clk->clock_gate));
    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr(unit, phy_acc, &eee_clk->clock_count));
    _SOC_IF_ERR_EXIT(READ_CLPORT_EEE_DURATION_TIMER_PULSEr(unit, phy_acc, &eee_clk->timer_pulse));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_check_legacy_phy (int unit, int port, pm_info_t pm_info, int *legacy_phy)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys, xphy_id, is_legacy_phy;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    *legacy_phy = 0;

    if (PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy reset is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
     while (nof_phys > 1) {
         --nof_phys;
         xphy_id = phy_access[nof_phys].access.addr;
         _SOC_IF_ERR_EXIT(portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));
         if (is_legacy_phy) {
             *legacy_phy = 1;
             break;
         }
     }

exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_failover_mode_set(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_set(unit, phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_failover_mode_get(int unit, int port, pm_info_t pm_info, phymod_failover_mode_t* failover)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    int port_index;
    uint32 bitmap;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index,
                                             &bitmap));

    if (PM_4x25_INFO(pm_info)->nof_phys[port_index] == 0) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                           (_SOC_MSG("phy failover is not supported")));
    }

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_failover_mode_get(unit, phy_access, nof_phys, failover));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_flow_control_set (int unit, int port, pm_info_t pm_info,
                                  int merge_mode_en, int parallel_fc_en)
{
    int phy_acc, rv;
    uint32 rval;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_CLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, &rval);
    if (SOC_FAILURE(rv)) return (rv);

    soc_reg_field_set(unit, CLPORT_FLOW_CONTROL_CONFIGr, &rval, MERGE_MODE_ENf, merge_mode_en);
    soc_reg_field_set(unit, CLPORT_FLOW_CONTROL_CONFIGr, &rval, PARALLEL_FC_ENf, parallel_fc_en);

    return(WRITE_CLPORT_FLOW_CONTROL_CONFIGr(unit, phy_acc, rval));
}

int pm4x25_port_e2ecc_hdr_set (int unit, int port, pm_info_t pm_info,
                               const portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (clmac_e2ecc_hdr_set (unit, port, (uint32_t *)e2ecc_hdr->words));
}


int pm4x25_port_e2ecc_hdr_get (int unit, int port, pm_info_t pm_info,
                               portmod_port_higig_e2ecc_hdr_t* e2ecc_hdr)
{
    return (clmac_e2ecc_hdr_get (unit, port, e2ecc_hdr->words));
}

int pm4x25_port_e2e_enable_set (int unit, int port, pm_info_t pm_info, int enable)
{
    return (clmac_e2e_enable_set(unit, port, enable));
}


int pm4x25_port_e2e_enable_get (int unit, int port, pm_info_t pm_info, int *enable)
{
    return (clmac_e2e_enable_get(unit, port, enable));
}

int pm4x25_port_notify(int unit, int port, pm_info_t pm_info, int link) {
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    portmod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    /* If nof phys > 1 change the internal phy speed based on the external
    link speed if the link is up*/
    if (nof_phys > 1) {
        if (link) {
            _SOC_IF_ERR_EXIT(portmod_port_interface_config_get(unit,
                                                               port,
                                                               &interface_config,
                                                               PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
            /* Set teh interface config to internal phy */
            _SOC_IF_ERR_EXIT(portmod_port_interface_config_set(unit,
                                                               port,
                                                               &interface_config, 
                                                               PORTMOD_INIT_F_INTERNAL_SERDES_ONLY));
        } else {
            /*TBD*/
        }
    }
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_mac_rsv_mask_set(int unit, int port, pm_info_t pm_info, uint32 rsv_mask)
{
        return(WRITE_CLPORT_MAC_RSV_MASKr(unit, port, rsv_mask));
}

int pm4x25_port_mib_reset_toggle(int unit, int port, pm_info_t pm_info, int port_index)
{
    uint32 reg_val[1];
    int    rv = 0, phy_acc;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    rv = READ_CLPORT_MIB_RESETr(unit, phy_acc, reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITSET(reg_val, port_index);

    rv = WRITE_CLPORT_MIB_RESETr(unit, phy_acc, *reg_val);
    if (SOC_FAILURE(rv)) return (rv);
    SHR_BITCLR(reg_val, port_index);

    return(WRITE_CLPORT_MIB_RESETr(unit, phy_acc, *reg_val));
}

int pm4x25_port_warmboot_db_restore(int unit, int port, pm_info_t pm_info,
                         const portmod_port_interface_config_t *f_intf_config,
                         const portmod_port_init_config_t      *f_init_config,
                         const phymod_operation_mode_t phy_op_mode)
{
    pm4x25_t pm4x25_data = NULL;
    phymod_phy_access_t                 phy_access[1+MAX_PHYN];
    phymod_phy_inf_config_t             phy_intf_config;
    portmod_encap_t                     encap_mode;
    phymod_autoneg_control_t            an;
    soc_port_if_t                       interface;
    int     phy, nof_phys, port_index, rv = 0, flags = 0, port_num_lanes = 0, is_interface_restore = 0;
    uint32  bitmap, an_done, interface_modes = 0;

    /*
     * if no warmboot db restore is required return
     */
    if(PM_4x25_INFO(pm_info)->warmboot_skip_db_restore){
        return SOC_E_NONE;
    }

    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) return (rv);

    pm4x25_data      = pm_info->pm_data.pm4x25_db;

    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy) {
        pm4x25_data->first_phy = phy;
        break;
    }

    rv = PM4x25_INTERFACE_TYPE_SET(unit, pm_info, f_intf_config->interface, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_MAX_SPEED_SET(unit, pm_info, f_intf_config->max_speed, port_index);
    if (SOC_FAILURE(rv)) return (rv);

    rv = PM4x25_IS_HG_SET(unit, pm_info, f_init_config->is_hg, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_AN_MODE_SET(unit, pm_info, f_init_config->an_mode, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_FS_CL72_SET(unit, pm_info, f_init_config->fs_cl72, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_AN_CL72_SET(unit, pm_info, f_init_config->an_cl72, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_AN_FEC_SET(unit, pm_info, f_init_config->an_fec, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_AN_MASTER_LANE_SET(unit, pm_info, f_init_config->an_master_lane, port_index);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_chain_phy_access_get (unit, port, pm_info, phy_access,
                                           (1+MAX_PHYN), &nof_phys);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_phychain_interface_config_get(unit, port, phy_access, nof_phys,
                                   0, phymodRefClk156Mhz, &phy_intf_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY);
    if (SOC_FAILURE(rv)) return (rv);

    rv = PM4x25_IS_INTERFACE_RESTORE_SET(unit, pm_info, is_interface_restore, port_index);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, phy_intf_config.interface_modes, port_index);
    if (SOC_FAILURE(rv)) return (rv);

    rv = portmod_port_interface_type_get (unit, port, &interface);
    if (SOC_FAILURE(rv)) return (rv);
    rv = PM4x25_INTERFACE_TYPE_SET(unit, pm_info, interface, port_index);
    if (SOC_FAILURE(rv)) return (rv);

    clmac_encap_get(unit, port, &flags, &encap_mode);
    if ((encap_mode == SOC_ENCAP_HIGIG2) || (encap_mode == SOC_ENCAP_HIGIG)) {
        rv = PM4x25_INTERFACE_MODES_GET(unit, pm_info, &interface_modes, port_index);
        if (SOC_FAILURE(rv)) return (rv);
        interface_modes |= PHYMOD_INTF_MODES_HIGIG;
        rv = PM4x25_INTERFACE_MODES_SET(unit, pm_info, interface_modes, port_index);
        if (SOC_FAILURE(rv)) return (rv);
    }

    rv = portmod_port_phychain_autoneg_get(unit, port, phy_access, nof_phys, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &an, &an_done);
    if (SOC_FAILURE(rv)) return (rv);

    if(an.an_mode != phymod_AN_MODE_NONE) {
        rv = PM4x25_AN_MODE_SET(unit, pm_info, an.an_mode, port_index);
        if (SOC_FAILURE(rv)) return (rv);
    }
    else {
        /* update an information before proceeding further */
        SOC_IF_ERROR_RETURN(_pm4x25_an_mode_update (unit, port, pm_info, port_index, port_num_lanes, f_init_config->an_cl73));
    }

    return (SOC_E_NONE);
}

int pm4x25_port_timesync_config_set(int unit, int port, pm_info_t pm_info, const portmod_phy_timesync_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_config_set(unit, port, phy_access,
                                                                  nof_phys,config));
    exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_timesync_config_get(int unit, int port, pm_info_t pm_info, portmod_phy_timesync_config_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_config_get(unit, port, phy_access,
                                                                  nof_phys,config));
    exit:
    SOC_FUNC_RETURN;  
}

int pm4x25_port_timesync_enable_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_enable_set(unit, phy_access,
                                                                  nof_phys,enable));
    exit:
    SOC_FUNC_RETURN;   
}

int pm4x25_port_timesync_enable_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_enable_get(unit, phy_access,
                                                                  nof_phys,enable));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_nco_addend_set(int unit, int port, pm_info_t pm_info, uint32 freq_step)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_nco_addend_set(unit, phy_access,
                                                                      nof_phys,freq_step));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_nco_addend_get(int unit, int port, pm_info_t pm_info, uint32* freq_step)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_nco_addend_get(unit, phy_access,
                                                                      nof_phys,freq_step));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_framesync_mode_set(int unit, int port, pm_info_t pm_info, 
                                            const portmod_timesync_framesync_t* framesync)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_framesync_mode_set(unit, phy_access,
                                                                          nof_phys,framesync));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_framesync_mode_get(int unit, int port, pm_info_t pm_info, 
                                            portmod_timesync_framesync_t* framesync)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_framesync_mode_get(unit, phy_access,
                                                                      nof_phys,framesync));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_local_time_set(int unit, int port, pm_info_t pm_info, const uint64 local_time)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_local_time_set(unit, phy_access,
                                                                      nof_phys,local_time));
    exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_timesync_local_time_get(int unit, int port, pm_info_t pm_info, uint64* local_time)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_local_time_get(unit, phy_access, nof_phys,
                                                                      local_time));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_load_ctrl_set(int unit, int port, pm_info_t pm_info, 
                                       uint32 load_once, uint32 load_always)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_load_ctrl_set(unit, phy_access, nof_phys,
                                                                     load_once, load_always));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_load_ctrl_get(int unit, int port, pm_info_t pm_info, 
                                       uint32* load_once, uint32* load_always)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_load_ctrl_get(unit, phy_access, nof_phys,
                                                                     load_once, load_always));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_tx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_tx_timestamp_offset_set(unit, phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_tx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_tx_timestamp_offset_get(unit, phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_rx_timestamp_offset_set(int unit, int port, pm_info_t pm_info, uint32 ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_rx_timestamp_offset_set(unit, phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_timesync_rx_timestamp_offset_get(int unit, int port, pm_info_t pm_info, uint32* ts_offset)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_timesync_rx_timestamp_offset_get(unit, phy_access, nof_phys,
                                                                               ts_offset));
    exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_port_timestamp_adjust_set
 *
 *Purpose:
 *  It sets tsts,osts adjustment into MAC registers.
*/
int
pm4x25_port_timestamp_adjust_set(int unit, int port, pm_info_t pm_info,
                         const soc_port_timestamp_adjust_t *ts_adjust)
{
    int port_index;
    uint32_t bitmap, port_dynamic_state;
    portmod_port_ts_adjust_t portmod_ts_adjust;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&portmod_ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));
    portmod_ts_adjust.osts_adjust = ts_adjust->osts_adjust;
    portmod_ts_adjust.tsts_adjust = ts_adjust->tsts_adjust;
    _SOC_IF_ERR_EXIT(clmac_timestamp_delay_set(unit, port, portmod_ts_adjust));

    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &port_index, &bitmap));
    _SOC_IF_ERR_EXIT(PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, &port_dynamic_state, port_index));
    PORTMOD_PORT_DEFAULT_TIMESTAMP_ADJUSTMENT_UPDATED_SET(port_dynamic_state);
    _SOC_IF_ERR_EXIT(PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info, port_dynamic_state, port_index));

    exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_port_timestamp_adjust_get
 *
 *Purpose:
 *  Get the tsts,osts adjustment from MAC register for current speed.
 */
int
pm4x25_port_timestamp_adjust_get(int unit, int port, pm_info_t pm_info,
                          soc_port_timestamp_adjust_t *ts_adjust)
{
    portmod_port_ts_adjust_t portmod_ts_adjust;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&portmod_ts_adjust, 0, sizeof(portmod_port_ts_adjust_t));

    _SOC_IF_ERR_EXIT(clmac_timestamp_delay_get(unit, port, &portmod_ts_adjust));
    ts_adjust->osts_adjust = portmod_ts_adjust.osts_adjust;
    ts_adjust->tsts_adjust = portmod_ts_adjust.tsts_adjust;

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_control_phy_timesync_set(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64 value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_control_phy_timesync_set(unit, port, phy_access, nof_phys,
                                                                       config, value));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_control_phy_timesync_get(int unit, int port, pm_info_t pm_info, const portmod_port_control_phy_timesync_t config, uint64* value)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    SOC_IF_ERROR_RETURN(portmod_port_phychain_control_phy_timesync_get(unit, port, phy_access, nof_phys,
                                                                       config, value));

    exit:
    SOC_FUNC_RETURN;
}

int pm4x25_xphy_lane_attach_to_pm (int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_conn)
{   
    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    uint32 xphy_id;
    int lane_mask;
    uint32 active_lanes;
    SOC_INIT_FUNC_DEFS;

    /* Validate parameters */
    _SOC_IF_ERR_EXIT(portmod_xphy_lane_connection_t_validate(unit,lane_conn));

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info,&pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x25_INFO(pm_info)->lane_conn[phyn-1][lane_index]);
    sal_memcpy(candidate_lane_conn, lane_conn, sizeof(portmod_xphy_lane_connection_t));
    rv = PM4x25_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, candidate_lane_conn, phyn-1, lane_index);
    _SOC_IF_ERR_EXIT(rv);

    xphy_id = candidate_lane_conn->xphy_id;
    lane_mask = candidate_lane_conn->ss_lane_mask;

    /*
     * Keeping track of the xphy lane usage.  If the xphy lane is connected
     * it will set the corresponding bit to mark as lane is active.
     */

    if ((lane_mask != -1) && (xphy_id != PORTMOD_XPHY_ID_INVALID)) {
        _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_get(unit, xphy_id, &active_lanes));
        active_lanes |= lane_mask;
        _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_set(unit,xphy_id, active_lanes));
    }

    /* increment the nof_phys for that lane */
    if (PM_4x25_INFO(pm_info)->nof_phys[lane_index] <= phyn){
        PM_4x25_INFO(pm_info)->nof_phys[lane_index] = phyn+1;
        rv = PM4x25_PORT_NUM_PHYS_SET(unit, pm_info, PM_4x25_INFO(pm_info)->nof_phys[lane_index], lane_index);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x25_xphy_lane_detach_from_pm (int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_conn)
{

    portmod_xphy_lane_connection_t *candidate_lane_conn = NULL;
    int lane_index = 0, phy_index = 0;
    int pm_is_active = 0, rv = 0;
    uint32 xphy_id;
    int lane_mask;
    uint32 active_lanes;
    SOC_INIT_FUNC_DEFS;

    if (MAX_PHYN <= phyn){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid Ext PHY location.")));
    }
    /* Making sure the port macro is not active */
    rv = PM4x25_IS_ACTIVE_GET(unit, pm_info,&pm_is_active);
    _SOC_IF_ERR_EXIT(rv);

    /* Attaching the external phy to the phy chain of the port macro */

    /* find lane index corresponding to internal physical port */
    lane_index = 0;
    PORTMOD_PBMP_ITER(PM_4x25_INFO(pm_info)->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }

    candidate_lane_conn = &(PM_4x25_INFO(pm_info)->lane_conn[phyn-1][lane_index]);

    xphy_id = candidate_lane_conn->xphy_id;
    lane_mask = candidate_lane_conn->ss_lane_mask;

    /*
     * Keeping track of the xphy lane usage.  If the xphy lane is disconnected
     * it will clear the corresponding bit to mark as lane is not active.
     * When none of the lane are active, it clear intialization status,
     * phy_type and probe status.  When it probe again, it will re-initialize
     * this xphy again.
     */

    if ((lane_mask != -1) && (xphy_id != PORTMOD_XPHY_ID_INVALID)) {

        _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_get(unit, xphy_id, &active_lanes));
        active_lanes &= ~lane_mask;
        _SOC_IF_ERR_EXIT(portmod_xphy_active_lane_set(unit, xphy_id, active_lanes));

        if (active_lanes == 0) { /* none of the lanes are active */
            _SOC_IF_ERR_EXIT(portmod_xphy_core_probed_set(unit, xphy_id, 0));
            _SOC_IF_ERR_EXIT(portmod_xphy_phy_type_set(unit,xphy_id,
                                                  phymodDispatchTypeInvalid));
            _SOC_IF_ERR_EXIT(portmod_xphy_is_initialized_set(unit, xphy_id,0));
        }
    }

    sal_memcpy( lane_conn, candidate_lane_conn, sizeof(portmod_xphy_lane_connection_t));
    portmod_xphy_lane_connection_t_init(unit, candidate_lane_conn); 
    rv = PM4x25_PORT_LANE_CONNECTION_DB_SET(unit, pm_info, candidate_lane_conn, phyn-1, lane_index);
    _SOC_IF_ERR_EXIT(rv);

    /* decrement the nof_phys for that lane */
    if (phyn > 0){
        PM_4x25_INFO(pm_info)->nof_phys[lane_index] = phyn;
        rv = PM4x25_PORT_NUM_PHYS_SET(unit, pm_info, PM_4x25_INFO(pm_info)->nof_phys[lane_index], lane_index);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
   SOC_FUNC_RETURN;
}

int pm4x25_port_update_dynamic_state(int unit, int port, pm_info_t pm_info, uint32_t port_dynamic_state) {
    int rv = 0;
    int port_index;
    uint32_t bitmap;
    phymod_an_mode_type_t an_mode;
    uint32_t an_updated = FALSE, tmp_state;
    SOC_INIT_FUNC_DEFS;
   
    rv = _pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap);
    if (SOC_FAILURE(rv)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unable to retrieve the port index")));
    }

    if (PORTMOD_PORT_IS_AUTONEG_MODE_UPDATED(port_dynamic_state)) {
        rv = PM4x25_PORT_DYNAMIC_STATE_GET(unit, pm_info, &tmp_state,
                                           port_index);
        _SOC_IF_ERR_EXIT(rv);
        /* we need to change the data base An_mode based on the new value */
        switch (port_dynamic_state >> 16 & 0xffff) {
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37:
            an_mode = phymod_AN_MODE_CL37;
            an_updated = TRUE;
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL37_BAM:
            an_mode = phymod_AN_MODE_CL37BAM;
            an_updated = TRUE;
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73:
            an_mode = phymod_AN_MODE_CL73;
            an_updated = TRUE;
            break;
        case SOC_PHY_CONTROL_AUTONEG_MODE_CL73_BAM:
            an_mode = phymod_AN_MODE_CL73BAM;
            an_updated = TRUE;
            break;
        default:
            break;
        }
    } else if (PORTMOD_PORT_IS_DEFAULT_TX_PARAMS_UPDATED(port_dynamic_state)) {
        rv = PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info,
                                      port_dynamic_state, port_index);
        _SOC_IF_ERR_EXIT(rv);
    }
    if (an_updated == TRUE) {
        rv = PM4x25_AN_MODE_SET(unit, pm_info, an_mode, port_index);
        _SOC_IF_ERR_EXIT(rv);
        PORTMOD_PORT_AUTONEG_MODE_UPDATED_SET(tmp_state);
        rv = PM4x25_PORT_DYNAMIC_STATE_SET(unit, pm_info, tmp_state,
                                           port_index);
        _SOC_IF_ERR_EXIT(rv);
    }

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_phy_op_mode_get(int unit, int port, pm_info_t pm_info, phymod_operation_mode_t* val)
{
    /* there is no opmode config for internal PHYS return 0*/
    *val = 0;
    return (SOC_E_NONE);
}

int pm4x25_port_medium_config_set(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_config_set(unit, port, medium, config);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x25_port_medium_config_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t medium, soc_phy_config_t* config)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_config_get(unit, port, medium, config);
    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x25_port_medium_get(int unit, int port, pm_info_t pm_info, soc_port_medium_t* medium)
{
    int rv;
    SOC_INIT_FUNC_DEFS;

    rv = portmod_port_phychain_medium_get(unit, port, medium);

    if (SOC_FAILURE(rv)) return (rv);

    SOC_FUNC_RETURN;
}

int pm4x25_port_multi_get(int unit, int port, pm_info_t pm_info, portmod_multi_get_t* multi_get)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info, 
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_multi_get(unit, port, phy_access, nof_phys, multi_get));

exit:
    SOC_FUNC_RETURN;

}

int pm4x25_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    PM_4x25_INFO(pm_info)->lane_map = *lane_map;


    SOC_FUNC_RETURN;
}

int pm4x25_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags, phymod_lane_map_t* lane_map)
{
    SOC_INIT_FUNC_DEFS;

    *lane_map = PM_4x25_INFO(pm_info)->lane_map;


    SOC_FUNC_RETURN;
}

int pm4x25_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    PM_4x25_INFO(pm_info)->polarity = *polarity;


    SOC_FUNC_RETURN;
}

int pm4x25_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    SOC_INIT_FUNC_DEFS;

    *polarity = PM_4x25_INFO(pm_info)->polarity;


    SOC_FUNC_RETURN;
}

int pm4x25_pm_is_in_pm12x10(int unit, pm_info_t pm_info, int* in_pm12x10)
{
    SOC_INIT_FUNC_DEFS;

    if (PM_4x25_INFO(pm_info)->in_pm12x10){
        *in_pm12x10 = 1;
    } else {
        *in_pm12x10 = 0;
    }

    SOC_FUNC_RETURN;
}

/*
 * This function will be called during speed change or flexport.
 * This function returns pll_div value by looking up the static table.
 * In flexport scenario, this funciton might be called before port is attached. The port number may be invalid(-1).
 * So this function can't access registers by logical port number, or access Warmboot DB.
 */
int pm4x25_port_pll_div_get(int unit, int port, pm_info_t pm_info,
                            const portmod_port_resources_t* port_resource,
                            uint32_t* pll_div)
{
    int higig = SOC_ENCAP_IEEE;

    SOC_INIT_FUNC_DEFS;

    higig = ((port_resource->encap_mode == SOC_ENCAP_HIGIG2) ||
             (port_resource->encap_mode == SOC_ENCAP_HIGIG)) ? 1 : 0;

    switch (port_resource->speed){
       case 1000:
           if (PORTMOD_SERDES_1000X_AT_25G_GET(port_resource->flag)) {
               *pll_div = phymod_TSCF_PLL_DIV165;
           } else {
               *pll_div = phymod_TSCF_PLL_DIV132;
           }
            break;
       case 2500:
           *pll_div = phymod_TSCF_PLL_DIV165;
            break;
       case 5000:
           *pll_div = phymod_TSCF_PLL_DIV132;
            break;
       case 10000:
            if (PORTMOD_SERDES_10G_AT_25G_GET(port_resource->flag)) {
               *pll_div = phymod_TSCF_PLL_DIV165;
            } else {
               *pll_div = phymod_TSCF_PLL_DIV132;
            }
            break;
       case 11000:
           *pll_div = phymod_TSCF_PLL_DIV140;
            break;
       case 12000:
           *pll_div = phymod_TSCF_PLL_DIV160;
            break;
       case 20000:
           *pll_div = phymod_TSCF_PLL_DIV132;
            break;
       case 21000:
       case 21200:
           *pll_div = phymod_TSCF_PLL_DIV140;
            break;
       case 25000:
           *pll_div = phymod_TSCF_PLL_DIV165;
            break;
       case 27000:
           *pll_div = phymod_TSCF_PLL_DIV175;
            break;
       case 40000:
           *pll_div = phymod_TSCF_PLL_DIV132;
            break;
       case 42000:
           *pll_div = phymod_TSCF_PLL_DIV140;
            break;
       case 48000:
           *pll_div = phymod_TSCF_PLL_DIV160;
            break;
       case 50000:
           *pll_div = phymod_TSCF_PLL_DIV165;
            break;
       case 53000:
            if (port_resource->num_lane == 2) {
                *pll_div = phymod_TSCF_PLL_DIV175;
            } else {
                *pll_div = phymod_TSCF_PLL_DIV165;
            }
            break;
       case 100000:
           if (higig){
               *pll_div = phymod_TSCF_PLL_DIV160;
            } else {
               *pll_div = phymod_TSCF_PLL_DIV165;
            }
            break;
       case 106000:
           *pll_div = phymod_TSCF_PLL_DIV175;
            break;
       default:
           *pll_div = phymod_TSCF_PLL_DIV175;
            break;
    }

    SOC_FUNC_RETURN;
}

int pm4x25_port_master_set(int unit, int port, pm_info_t pm_info, int master_mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_phychain_master_set(unit, port, pm_info, master_mode));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_master_get(int unit, int port, pm_info_t pm_info, int* master_mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_phychain_master_get(unit, port, pm_info, master_mode));

exit:
    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      _pm4x25_preemption_rx_frag_size_set
 * Purpose:
 *      This function configures preemption rx fragment size
 *      of final and non-final fragments.
 *
 * Parameters:
 *      unit                -(IN) Device unit number
 *      port                -(IN) logical port
 *      final_frag          -(IN) if set indicates final fragment
 *      value               -(IN) value of fragment size
 */
STATIC
int _pm4x25_preemption_rx_frag_size_set(int unit, int port, pm_info_t pm_info,
                                        uint32 frag, uint32 value)
{
    int phy_acc;
    uint32_t rval;
    soc_field_t field;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_MIB_MIN_FRAG_SIZEr(unit, phy_acc, &rval));

    if (frag == PORTMOD_PREEMPTION_FINAL_FRAG) {
        field = MIN_FINAL_FRAG_SIZEf;
    } else {
        field = MIN_NON_FINAL_FRAG_SIZEf;
    }

    soc_reg_field_set(unit, CLPORT_MIB_MIN_FRAG_SIZEr, &rval, field, value);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_MIB_MIN_FRAG_SIZEr(unit, phy_acc, rval));

exit:
    SOC_FUNC_RETURN;

}

/*
 * Function:
 *      _pm4x25_preemption_rx_frag_size_get
 * Purpose:
 *      This function configures preemption rx fragment size
 *      of final and non-final fragments.
 *
 * Parameters:
 *      unit                -(IN)  Device unit number
 *      port                -(IN)  logical port
 *      final_frag          -(IN)  if set indicates final fragment
 *      value               -(OUT) value of fragment size
 */
STATIC
int _pm4x25_preemption_rx_frag_size_get(int unit, int port, pm_info_t pm_info,
                                        uint32 frag, uint32 *value)
{
    int phy_acc;
    uint32_t rval;
    soc_field_t field;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_MIB_MIN_FRAG_SIZEr(unit, phy_acc, &rval));

    if (frag == PORTMOD_PREEMPTION_FINAL_FRAG) {
        field = MIN_FINAL_FRAG_SIZEf;
    } else {
        field = MIN_NON_FINAL_FRAG_SIZEf;
    }

    *value = soc_reg_field_get(unit, CLPORT_MIB_MIN_FRAG_SIZEr, rval, field);

exit:
    SOC_FUNC_RETURN;

}

/*
 * Function:
 *      pm4x25_preemption_control_set
 * Purpose:
 *      This function configures preemption feature parameters
 *      The application layer has to call this function with
 *      associated preemption-controls to configure parameters.
 * Parameters:
 *      unit                -(IN) Device unit number
 *      port                -(IN) logical port
 *      type                -(IN) preemption parameter to be configured
 *      value               -(IN) value
 * Returns:
 *      Status
 */
int pm4x25_preemption_control_set(int unit, int port, pm_info_t pm_info,
                                  portmod_preemption_control_t type,
                                  uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    switch(type) {
        case  portmodPreemptionControlInvalid:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
        case  portmodPreemptionControlPreemptionSupport:
            /* Enable/disable port preemption capability support */
            _SOC_IF_ERR_EXIT(clmac_preemption_mode_set(unit, port, value));
            break;
        case  portmodPreemptionControlEnableTx:
            /*
             * Enable/disable port preemption on tx side
             * if verify_enable = 1
             * this function call blocks until, verify operation is complete
             * if verify_enable = 0
             * means static preemption, function call returns immediately
             * after enabling preemption in the hardware in tx direction
             */
            _SOC_IF_ERR_EXIT(clmac_preemption_tx_enable_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyEnable:
            /*
             * Enable/disable verify operation.
             * Before enabling the verify operation on a port,
             * the number of attempts for a veriy operation and
             * the timeout for each verify operation should be configured
             */
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_enable_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyTime:
            /*
             * Configuring the verify timeout is valid only when verify
             * operation is enabled.
             * The timeout here means, if verify operation is not complete
             * success/failure/inprogress after the (timeout + 1) millisecs
             * the MAC sends another verify message to the link partner
             */
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_time_set(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyAttempts:
            /*
             * Configuring the number of verify attempts is valid only when
             * verify operation is enabled.
             * If verify operation is not complete success/failure/inprogress
             * after the (timeout + 1) millisecs the MAC sends another verify
             * to the link partner. The MAC tries for (num_verify_attempts + 1)
             * to complete the verify operation.
             */
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_attempts_set(unit, port, value)); 
            break;
        case  portmodPreemptionControlNonFinalFragSizeRx:
            /* configure non-final rx fragment size */
            _SOC_IF_ERR_EXIT(
                _pm4x25_preemption_rx_frag_size_set(unit, port, pm_info,
                                      PORTMOD_PREEMPTION_NON_FINAL_FRAG,
                                      value));
            break;
        case  portmodPreemptionControlFinalFragSizeRx:
            /* configure final rx fragment size */
            _SOC_IF_ERR_EXIT(
                _pm4x25_preemption_rx_frag_size_set(unit, port, pm_info,
                                          PORTMOD_PREEMPTION_FINAL_FRAG,
                                          value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;

}

/*
 * Function:
 *      pm4x25_preemption_control_get
 * Purpose:
 *      This function gets the value of preemption parameters
 *      configured. The application layer has to call this function with
 *      associated preemption-controls to configure parameters.
 * Parameters:
 *      unit                -(IN) Device unit number
 *      port                -(IN) logical port
 *      type                -(IN) preemption parameter to be configured
 *      value               -(OUT) value returned from hardware
 * Returns:
 *      Status
 */
int pm4x25_preemption_control_get(int unit, int port, pm_info_t pm_info,
                                  portmod_preemption_control_t type,
                                  uint32 *value)
{
    SOC_INIT_FUNC_DEFS;

    switch(type) {
        case  portmodPreemptionControlInvalid:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
        case  portmodPreemptionControlPreemptionSupport:
            _SOC_IF_ERR_EXIT(clmac_preemption_mode_get(unit, port, value));
            break;
        case  portmodPreemptionControlEnableTx:
            _SOC_IF_ERR_EXIT(clmac_preemption_tx_enable_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyEnable:
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_enable_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyTime:
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_time_get(unit, port, value));
            break;
        case  portmodPreemptionControlVerifyAttempts:
            _SOC_IF_ERR_EXIT(
                clmac_preemption_verify_attempts_get(unit, port, value));
            break;
        case  portmodPreemptionControlNonFinalFragSizeRx:
            /* get non-final rx fragment size */
            _SOC_IF_ERR_EXIT(
                _pm4x25_preemption_rx_frag_size_get(unit, port, pm_info,
                                      PORTMOD_PREEMPTION_NON_FINAL_FRAG,
                                      value));
            break;
        case  portmodPreemptionControlFinalFragSizeRx:
            /* get final rx fragment size */
            _SOC_IF_ERR_EXIT(
                _pm4x25_preemption_rx_frag_size_get(unit, port, pm_info,
                                          PORTMOD_PREEMPTION_FINAL_FRAG,
                                          value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid preemption control type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_preemption_tx_status_get
 *
 * @brief Get the preemption status on the transmit side Indicates if
 *        preemption on a port is
 *        Inactive: (Verifiy is enabled but its status is either failed
 *                  or not initiated) or (preemption is not enabled)
 *        Verify in progress: Preemption is enabled and Verify is
 *                            enabled and its operation is in progress
 *        Active: Preemption is enabled and ((Verify is enabled and
 *                succeeded) or (Verify is NOT enabled))
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] value           - indicates preemption status in tx direction.
 */
int pm4x25_preemption_tx_status_get(int unit, int port,
                                    pm_info_t pm_info,
                                    uint32* value)
{
    return (clmac_preemption_tx_status_get(unit, port,
                                           CLMAC_PREEMPTION_TX_STATUS,
                                           value));
}

/*!
 * pm4x25_preemption_verify_status_get
 *
 * @brief Get the preemption verify status.
 *        This function gets verify operation status or verify count
 *        status based on the verify status type argument.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  type            - portmod_preemption_verify_status_type_t
 * @param [out] value           - indicates preemption verify op status
 */
int pm4x25_preemption_verify_status_get(int unit, int port,
                                 pm_info_t pm_info,
                                 portmod_preemption_verify_status_type_t type,
                                 uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionVerifyStatus:
            _SOC_IF_ERR_EXIT(clmac_preemption_tx_status_get(unit, port,
                                       CLMAC_PREEMPTION_VERIFY_STATUS,
                                       value));
            break;
        case portmodPreemptionVerifyCountStatus:
            _SOC_IF_ERR_EXIT(clmac_preemption_tx_status_get(unit, port,
                                  CLMAC_PREEMPTION_VERIFY_CNT_STATUS,
                                  value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid verify status type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * portmod_preemption_force_rx_eop_mode_set
 *
 * @brief Set preemption forced rx lost eop(end of packet) sequence mode
 * setting. The rx lost eop sequence can be either software or hardware
 * enforced.
 * If HW based rx lost eop is enabled, link down or rx disable even
 * forces rx_lost_eop sequence after some internal delay if preempt
 * packet EOP was not sent.
 * If Sw based rx lost eop is enabled,  the application rx eop sequence
 * When SW rx lost EOP is enabled(done by application layer), rising
 * edge MAC forces rx_lost_eop sequence after some internal delay if
 * preempt packet EOP was not sent.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enable/disable
 * @param [in]  timeout_cnt     - timeout in millisecs
 */
int pm4x25_preemption_force_rx_eop_mode_set(int unit, int port,
                              pm_info_t pm_info,
                              portmod_preemption_force_rx_eop_mode_t type,
                              uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionForceSwRxEop:
            _SOC_IF_ERR_EXIT(clmac_preemption_force_rx_lost_eop_set(unit, port,
                                              CLMAC_PREEMPTION_FORCE_SW_RX_EOP,
                                              value));
            break;
        case portmodPreemptionForceAutoHwEop:
            _SOC_IF_ERR_EXIT(clmac_preemption_force_rx_lost_eop_set(unit, port,
                                         CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP,
                                         value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid Force RX lost EOP type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_preemption_force_rx_eop_mode_get
 *
 * @brief Get preemption forced rx lost eop(end of packet) sequence mode
 * setting. The rx lost eop sequence can be either software or hardware
 * enforced.
 * This functin returns the rx lost eop setting based on the type
 * of lost eop is enforced.
 *
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out] type            - portmod_preemption_force_rx_eop_mode_t
 * @param [out] value           - timeout in millisecs
 */
int pm4x25_preemption_force_rx_eop_mode_get(int unit, int port,
                                 pm_info_t pm_info,
                                 portmod_preemption_force_rx_eop_mode_t type,
                                 uint32 *value)
{
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case portmodPreemptionForceSwRxEop:
            _SOC_IF_ERR_EXIT(clmac_preemption_force_rx_lost_eop_get(unit, port,
                                              CLMAC_PREEMPTION_FORCE_SW_RX_EOP,
                                              value));
            break;
        case portmodPreemptionForceAutoHwEop:
            _SOC_IF_ERR_EXIT(clmac_preemption_force_rx_lost_eop_get(unit, port,
                                         CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP,
                                         value));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
              (_SOC_MSG("Invalid Force RX lost EOP type(%d)"), type));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}
int pm4x25_port_lane_count_get (int unit, int port, pm_info_t pm_info, int line_side, int* num_lanes)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys;
    int is_outer_most = 0;
    SOC_INIT_FUNC_DEFS;

    /* coverity[check_return] */
    portmod_access_get_params_t_init(unit, &params);

    if(line_side){
        params.sys_side =  PORTMOD_SIDE_LINE;
    } else {
        params.sys_side =  PORTMOD_SIDE_SYSTEM;
    }

    _SOC_IF_ERR_EXIT(pm4x25_port_phy_lane_access_get(unit, port, pm_info,
                                                     &params, 1 , &phy_access,
                                                     &nof_phys, &is_outer_most));

    *num_lanes = 0;
    PORTMOD_BIT_COUNT(phy_access.access.lane_mask,*num_lanes);

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x25_preemption_rx_timeout_set
 *
 * @brief Set specified preemption rx timeout and timeout count.
 * This function should only be called during device init.
 * There are 2 rx parameters rx timeout enable, rx timeout value in microsec
 * that can be configured using this function.
 * If set, enables TIMEOUT mechanism for Rx preempt traffic.
 * Timeout value in microsec, this configuration indicates link idle
 * time(in microsec) after which Rx timeout will force rx_lost_eop sequence
 * if preempt packet EOP was not sent.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enables timeout for rx preempt traffic
 * @param [in]  timeout_cnt     - timeout in microsecs
 */
int pm4x25_preemption_rx_timeout_set(int unit, int port, pm_info_t pm_info,
                                     uint32 enable, uint32 timeout_cnt)
{
    return (clmac_preemption_rx_timeout_info_set(unit, port, enable,
                                                 timeout_cnt));
}

/*!
 * pm4x25_preemption_rx_timeout_set
 *
 * @brief Get specified preemption rx timeout and timeout count.
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  enable          - enables timeout for rx preempt traffic
 * @param [out] timeout_cnt     - timeout in millisec
 */
int pm4x25_preemption_rx_timeout_get(int unit, int port, pm_info_t pm_info,
                                     uint32 *enable, uint32 *timeout_cnt)
{
    return (clmac_preemption_rx_timeout_info_get(unit, port, enable,
                                                 timeout_cnt));
}

STATIC
int _pm4x25_phy_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    phymod_ts_fifo_status_t phy_ts_tx_info;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(phymod_ts_fifo_status_t_init(&phy_ts_tx_info));

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    _SOC_IF_ERR_EXIT(portmod_port_phychain_timesync_tx_info_get(unit, port, phy_access,
                                                                  nof_phys, &phy_ts_tx_info));

    tx_info->timestamps_in_fifo = phy_ts_tx_info.ts_in_fifo_lo;
    tx_info->timestamps_in_fifo_hi = phy_ts_tx_info.ts_in_fifo_hi;
    tx_info->sequence_id = phy_ts_tx_info.ts_seq_id;
    tx_info->timestamp_sub_nanosec = phy_ts_tx_info.ts_sub_nanosec;

exit:
    SOC_FUNC_RETURN;
}

/*!
 * pm4x10_port_timesync_tx_info_get
 *
 * @brief get port timestamps in fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [inout]  tx_info         - timestamp and seq id form fifo
 */

int pm4x25_port_timesync_tx_info_get(int unit, int port, pm_info_t pm_info, portmod_fifo_status_t* tx_info)
{
    int phy_acc, mac_ts_disable;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    mac_ts_disable = 0;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, CLMAC_TS_DISABLEf)) {
        _SOC_IF_ERR_EXIT(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
        mac_ts_disable = soc_reg_field_get(unit, CLPORT_MAC_CONTROLr, reg_val, CLMAC_TS_DISABLEf);
    }

   /*
    * 1 = disable MAC timestamp, use TSC for timestamp.
    * 0 = legacy mode, use MAC for timestamp.
    */
    if (mac_ts_disable) {
        _SOC_IF_ERR_EXIT(_pm4x25_phy_timesync_tx_info_get(unit, port, pm_info, tx_info));
    } else {
        _SOC_IF_ERR_EXIT(clmac_timesync_tx_info_get(unit, port, tx_info));
    }

exit:
    SOC_FUNC_RETURN;
}

#ifdef FW_BCAST_DOWNLOAD
STATIC int
_pm4x25_sbus_control_set(int unit, const phymod_core_access_t* core_acc, int bcast_id, int chain_last)
{
    portmod_default_user_access_t *user_acc = (portmod_default_user_access_t*)core_acc->access.user_acc;
    int reg_acc_idx = user_acc->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;
    soc_reg_t sbus_control_reg = CLPORT_SBUS_CONTROLr;
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, sbus_control_reg, reg_acc_idx, 0, &reg_val));
    soc_reg_field_set(unit, sbus_control_reg, &reg_val,
                      SBUS_BCAST_BLOCK_IDf, bcast_id);
    soc_reg_field_set(unit, sbus_control_reg, &reg_val,
                      SBUS_CHAIN_LASTf, chain_last);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, sbus_control_reg, reg_acc_idx, 0, reg_val));

    return SOC_E_NONE;
}

STATIC int
_pm4x25_tsc_acc_mode_set(int unit, const phymod_core_access_t* core_acc, int acc_mem)
{
    portmod_default_user_access_t *user_acc = (portmod_default_user_access_t*)core_acc->access.user_acc;;
    int reg_acc_idx = user_acc->blk_id | SOC_REG_ADDR_BLOCK_ID_MASK;
    soc_reg_t ucmem_ctrl_reg = CLPORT_WC_UCMEM_CTRLr;
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, ucmem_ctrl_reg, reg_acc_idx, 0, &reg_val));
    soc_reg64_field32_set(unit, ucmem_ctrl_reg, &reg_val, ACCESS_MODEf, acc_mem);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, ucmem_ctrl_reg, reg_acc_idx, 0, reg_val));

    return SOC_E_NONE;
}
#endif

int pm4x25_fw_bcast(int unit, pm_info_t pm_info, const portmod_sbus_bcast_config_t* config)
{
#ifdef FW_BCAST_DOWNLOAD
    phymod_core_access_t* core_access = &PM_4x25_INFO(pm_info)->int_core_access;
    phymod_core_init_config_t core_conf;
    phymod_core_status_t core_status;
    portmod_default_user_access_t* user_acc;
    int flag = config->flag;

    SOC_IF_ERROR_RETURN(phymod_core_init_config_t_init(&core_conf));
    SOC_IF_ERROR_RETURN(phymod_core_status_t_init(&core_status));

    core_conf.flags = 0;
    PHYMOD_CORE_INIT_F_SERDES_FW_BCAST_SET(&core_conf);
    if (flag & PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_conf);
        flag &= ~PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY;
    }

    core_conf.firmware_loader = pm4x25_default_fw_loader;
    core_status.pmd_active = 0;

    switch (flag) {
    case PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD:
        PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_SET(&core_conf);
        SOC_IF_ERROR_RETURN(phymod_core_init(core_access, &core_conf, &core_status));
        PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_CLR(&core_conf);
        break;

    case PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD:
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&core_conf);
        SOC_IF_ERROR_RETURN(phymod_core_init(core_access, &core_conf, &core_status));
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_CLR(&core_conf);

        /* program broadcast ID and chain last */
        _pm4x25_sbus_control_set(unit, core_access, config->bcast_id, config->chain_last);

        /* access TSC ucode memory */
        _pm4x25_tsc_acc_mode_set(unit, core_access, 1);
        break;

    case PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD:
        /*
         * phymod_core_init still need to access register to check whether UC is enabled before download firmware
         * TSC access mode will be changed to memory mode finally in fw_loader that will be called in phymod_core_init
         */
        _pm4x25_tsc_acc_mode_set(unit, core_access, 0);

        user_acc = (portmod_default_user_access_t*)core_access->access.user_acc;
        PORTMOD_USER_ACCESS_FW_LOAD_BROADCAST_SET(user_acc);

        PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_SET(&core_conf);
        SOC_IF_ERROR_RETURN(phymod_core_init(core_access, &core_conf, &core_status));
        PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_CLR(&core_conf);

        PORTMOD_USER_ACCESS_FW_LOAD_BROADCAST_CLR(user_acc);
        break;

    case PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD:
        /* access TSC register */
        _pm4x25_tsc_acc_mode_set(unit, core_access, 0);

        /* clear broadcast ID and chain last */
        _pm4x25_sbus_control_set(unit, core_access, 0x7f, 0);

        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_conf);
        SOC_IF_ERROR_RETURN(phymod_core_init(core_access, &core_conf, &core_status));
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_CLR(&core_conf);
        break;

    case PHYMOD_CORE_INIT_F_FW_LOAD_END:
        PHYMOD_CORE_INIT_F_FW_LOAD_END_SET(&core_conf);
        SOC_IF_ERROR_RETURN(phymod_core_init(core_access, &core_conf, &core_status));
        PHYMOD_CORE_INIT_F_FW_LOAD_END_CLR(&core_conf);
        break;

    default:
        break;
    }

    return SOC_E_NONE;
#else
    return SOC_E_UNAVAIL;
#endif
}


STATIC int
_pm4x25_init_plls_get(int unit, int port, pm_info_t pm_info, portmod_vcos_speed_config_t *speed_config, int size, phymod_dual_plls_t *dual_plls)
{
    portmod_dual_vcos_t dual_vcos;

    portmod_dual_vcos_t_init(unit, &dual_vcos);
    size = 1;
    pm4x25_port_vcos_get(unit, port, pm_info, speed_config, size, &dual_vcos);

    if (speed_config->port_type == portmodCpmPrtEthOfMixedMode) {
        /* PLL1 is used for ETH ports */
        dual_plls->pll0_div = 0;
        dual_plls->pll1_div = (dual_vcos.vco_1 == portmodVCO20G)? phymod_TSCF_PLL_DIV132 : phymod_TSCF_PLL_DIV165;
    } else {
        switch (dual_vcos.vco_0) {
            case portmodVCO20G:
                dual_plls->pll0_div = phymod_TSCF_PLL_DIV132;
                break;
            case portmodVCO21G:
                dual_plls->pll0_div = phymod_TSCF_PLL_DIV140;
                break;
            case portmodVCO27G:
                dual_plls->pll0_div = phymod_TSCF_PLL_DIV175;
                break;
            default:
                dual_plls->pll0_div = phymod_TSCF_PLL_DIV132;
                break;
       }
       /* PLL1 is always 25G */
       dual_plls->pll1_div = phymod_TSCF_PLL_DIV165;
   }

   return SOC_E_NONE;
}


#define VCO_20G      0x1
#define VCO_21G      0x2
#define VCO_25G      0x4
#define VCO_27G      0x8
#define VCO_20G_25G  0x5
int
pm4x25_port_vcos_get(int unit, int port, pm_info_t pm_info, const portmod_vcos_speed_config_t* speed_config_list, int size, portmod_dual_vcos_t* dual_vco)
{
    int index = 0;
    int vco_bitmap[4] = {0};
    int vco_bmp = 0;

    if (size == 0) return SOC_E_NONE;
    for (index = 0; index < size; index++){
        /* Check if eth port is in the quad. If yes, PLL1 vco is either 20G or 25G */
        switch (speed_config_list[index].num_lanes){
            case 1:
                if(speed_config_list[index].speed == 1000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 2500){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 5000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 10000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 20000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 25000){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 11000){
                    vco_bitmap[index] = VCO_21G;
                } else if (speed_config_list[index].speed == 21000){
                    vco_bitmap[index] = VCO_21G;
                } else if(speed_config_list[index].speed == 27000){
                    vco_bitmap[index] = VCO_27G;
                }
                break;
            case 2:
                if(speed_config_list[index].speed == 20000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 40000){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 50000){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 21000){
                    vco_bitmap[index] = VCO_21G;
                } else if (speed_config_list[index].speed == 42000){
                    vco_bitmap[index] = VCO_21G;
                } else if(speed_config_list[index].speed == 53000){
                    vco_bitmap[index] = VCO_27G;
                }
                break;
            case 4:
                if(speed_config_list[index].speed == 40000){
                    vco_bitmap[index] = VCO_20G;
                }else if (speed_config_list[index].speed == 50000){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 100000){
                    vco_bitmap[index] = VCO_25G;
                }else if (speed_config_list[index].speed == 42000){
                    vco_bitmap[index] = VCO_21G;
                } else if (speed_config_list[index].speed == 53000){
                    vco_bitmap[index] = VCO_27G;
                } else if(speed_config_list[index].speed == 106000){
                    vco_bitmap[index] = VCO_27G;
                }
                break;
            default:
                 LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
                   speed_config_list[index].num_lanes, speed_config_list[index].speed));
                return SOC_E_PARAM;
        }
        vco_bmp |= vco_bitmap[index];
    }
    if (speed_config_list->port_type == portmodCpmPrtEthOfMixedMode) {
        switch (vco_bmp){
            case VCO_20G:
                dual_vco->vco_1 = portmodVCO20G;
            break;
            case VCO_25G:
                dual_vco->vco_1 = portmodVCO25G;
            break;
            default:
                 LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "fail to get vcos -- no vcos matches in ETH vco_bmp 0x%x\n"), vco_bmp));
                return SOC_E_PARAM;
        }
    } else {
        switch (vco_bmp){
            case VCO_20G:
            case VCO_25G:
            case VCO_20G_25G:
                dual_vco->vco_0 = portmodVCO20G;  /* IEEE */
                dual_vco->vco_1 = portmodVCO25G;
            break;
            case VCO_21G:
                dual_vco->vco_0 = portmodVCO21G;  /* HiGig */
                dual_vco->vco_1 = portmodVCO25G;
                break;
            case VCO_27G:
                dual_vco->vco_0 = portmodVCO27G;  /* HiGig */
                dual_vco->vco_1 = portmodVCO25G;
                break;
            default:
                 LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "fail to get vcos -- no vcos combination vco_bmp 0x%x\n"), vco_bmp));
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;

}

STATIC
int _pm4x25_ext_tsc_clk_set(int unit, int phy_acc, int ext_clk, int pll_vcoclk)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, EXT_TSC_CLK_ENf, ext_clk? 1 : 0);
    if (!ext_clk){
        soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, TSC_CLK_SELf, pll_vcoclk ? 1 : 0);
    }
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;

}

STATIC
int _pm4x25_pm_ext_tsc_clk_set(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int phy_acc;
    int ext_clk = 0;
    int pll_vcoclk = 0;
    portmod_vcos_speed_config_t speed_config;
    phymod_dual_plls_t init_plls;

    SOC_INIT_FUNC_DEFS;
    portmod_vcos_speed_config_t_init(unit, &speed_config);
    speed_config.higig_mode = add_info->init_config.is_hg;
    speed_config.speed = add_info->interface_config.speed;
    speed_config.num_lanes = add_info->interface_config.port_num_lanes;
    if (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri) {
        speed_config.port_type = portmodCpmPrtEthOfMixedMode;
    }
    _pm4x25_init_plls_get(unit, 0 , pm_info, &speed_config, 1, &init_plls);

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    if ((add_info->init_config.cpm_core_mode == portmodCpmAllIEEE) || (add_info->init_config.cpm_core_mode == portmodCpmAllHIGIG)
         || (add_info->init_config.cpm_core_mode == portmodCpmMixEthCpri)) {
        ext_clk = 1;

    } else if (add_info->init_config.cpm_core_mode == portmodCpmAllCpri) {
        LOG_ERROR(BSL_LS_SOC_PORT,
            (BSL_META_U(unit, " Wrong port _mode from CPM4x25 %d\n"), portmodCpmAllCpri));
    } else {
        if(add_info->init_config.is_hg) {
            ext_clk = 0;
            pll_vcoclk = 0;
            /* Check if port is HIGIG mode with IEEE speed */
            if((init_plls.pll0_div == phymod_TSCF_PLL_DIV132) && (init_plls.pll1_div == phymod_TSCF_PLL_DIV165)) {
                pll_vcoclk = 1;
            }
        } else {
            ext_clk = 0;
            pll_vcoclk = 1;
        }
#if 0
        if(add_info->init_config.txpi_mode){
            ext_clk = 1;
        }
#endif
    }

    _SOC_IF_ERR_EXIT
        (_pm4x25_ext_tsc_clk_set(unit, phy_acc, ext_clk, pll_vcoclk));

exit:

    SOC_FUNC_RETURN;
}

int pm4x25_port_mac_timestamp_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    int phy_acc;
    uint32 reg_val;
    pm_version_t pm_version;
    int32 mac_version = 0;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, CLMAC_TS_DISABLEf)) {
        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &reg_val,
                                CLMAC_TS_DISABLEf, (enable) ? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, phy_acc, reg_val));
    } else {
        return SOC_E_UNAVAIL;
    }

    if (!enable) {
        /* Disable XLMAC byte_adjust if MAC timestamp is disabled.
         * Otherwise, it will affect timestamp value of TSC timestamping.
         */
        SOC_IF_ERROR_RETURN(_pm4x25_pm_version_get(unit, pm_info, &pm_version));
        SOC_IF_ERROR_RETURN(clmac_version_get(unit, port, &mac_version));
        /* PM VERSION and CLMAC BYTE ADJUST aren't valid before TH2. QAX_B0 used GEN1_PM + MAC_A027 */
        if (!PORTMOD_PM4x25_IS_GEN1(pm_version) || (CLMAC_VERSION_A027 == mac_version)) {
            SOC_IF_ERROR_RETURN
                (clmac_timestamp_byte_adjust_set(unit, port, CLMAC_TIMESTAMP_BYTE_ADJUST_CLEAR, 0));
        }

    }
    return (SOC_E_NONE);
}

int pm4x25_port_mac_timestamp_enable_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    int phy_acc, tmp;
    uint32 reg_val;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, CLMAC_TS_DISABLEf)) {
        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, phy_acc, &reg_val));
        tmp = soc_reg_field_get(unit, CLPORT_MAC_CONTROLr, reg_val, CLMAC_TS_DISABLEf);
    } else {
        return SOC_E_UNAVAIL;
    }

    if (tmp) {
        *enable = FALSE;
    } else {
        *enable = TRUE;
    }

    return (SOC_E_NONE);
}

/*
 *Function:
 *  _pm4x25_txpi_sdm_scheme_set
 *
 *Purpose:
 *  Set Portmacro TXPI DSM scheme type
 *  Default type is portmodTxPiSdmType1StOderWithFloor
 *  This is a core level API
 *  SDM scheme can't be changed if the port is active
*/
int _pm4x25_txpi_sdm_scheme_set(int unit, int port, pm_info_t pm_info, portmod_txpi_sdm_type_t type)
{
    int phy_acc;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_TXPI_CSR_CTRL1r(unit, phy_acc, &reg_val));
    soc_reg_field_set(unit, CLPORT_TXPI_CSR_CTRL1r, &reg_val, TXPI_CSR_CTRL_SDM_TYPEf, type);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_TXPI_CSR_CTRL1r(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_txpi_lane_select_set
 *
 *Purpose:
 *  Set Portmacro TXPI lane select
 *  Defaults to using TXPI for it's own lane
 *  This is a core level API
*/
int _pm4x25_txpi_lane_select_set(int unit, int port, pm_info_t pm_info, uint32_t *lane_select, int physical_lane)
{
    int phy_acc, lane = physical_lane;
    uint32 reg_val = 0;
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

    _SOC_IF_ERR_EXIT(READ_CLPORT_TXPI_LANE_SELECTIONr(unit, phy_acc, &reg_val));
    switch (lane) {
            case 3:
                soc_reg_field_set(unit, CLPORT_TXPI_LANE_SELECTIONr, &reg_val, TXPI_LANE_SELECTION_3f, lane_select[3]);
                break;
            case 2:
                soc_reg_field_set(unit, CLPORT_TXPI_LANE_SELECTIONr, &reg_val, TXPI_LANE_SELECTION_2f, lane_select[2]);
                break;
            case 1:
                soc_reg_field_set(unit, CLPORT_TXPI_LANE_SELECTIONr, &reg_val, TXPI_LANE_SELECTION_1f, lane_select[1]);
                break;
            case 0:
            default:
                soc_reg_field_set(unit, CLPORT_TXPI_LANE_SELECTIONr, &reg_val, TXPI_LANE_SELECTION_0f, lane_select[0]);
                break;
    }
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_TXPI_LANE_SELECTIONr(unit, phy_acc, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  _pm4x25_port_phy_txpi_mode_set
 *
 *Purpose:
 *  Set PMD TXPI mode: normal or external PD
 *  This is a port level API
*/
int _pm4x25_port_phy_txpi_mode_set(int unit, int port, pm_info_t pm_info, uint32 mode)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    phymod_txpi_config_t config;
    SOC_INIT_FUNC_DEFS;

    config.enable = mode ? 1 : 0;
    config.mode   = mode;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));
    SOC_IF_ERROR_RETURN(portmod_port_phychain_txpi_config_set(unit, phy_access, nof_phys, &config));

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_port_txpi_override_set
 *
 *Purpose:
 *  Overwrite Portmacro TXPI SDM value
 *  Defaults to using value from CMIC
 *  This is a port level API
*/
int pm4x25_port_txpi_override_set (int unit, int port, pm_info_t pm_info, const portmod_txpi_override_t *override)
{
    int phy_acc;
    uint32 reg_val_en, bitmap;
    int i, first_index;
    int64 reg_value;

    soc_field_t field[MAX_PORTS_PER_PM4X25] = {TXPI_CSR_OVER_EN_0f, TXPI_CSR_OVER_EN_1f, TXPI_CSR_OVER_EN_2f, TXPI_CSR_OVER_EN_3f};
    soc_reg_t over_reg_low[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_CSR_OVER_VAL_0_LOWr, CLPORT_TXPI_CSR_OVER_VAL_1_LOWr,
                                                    CLPORT_TXPI_CSR_OVER_VAL_2_LOWr, CLPORT_TXPI_CSR_OVER_VAL_3_LOWr};
    soc_reg_t over_reg_high[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_CSR_OVER_VAL_0_HIGHr, CLPORT_TXPI_CSR_OVER_VAL_1_HIGHr,
                                                    CLPORT_TXPI_CSR_OVER_VAL_2_HIGHr, CLPORT_TXPI_CSR_OVER_VAL_3_HIGHr};
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /* Enable TXPI DSM override */
    _SOC_IF_ERR_EXIT(READ_CLPORT_TXPI_CSR_OVER_ENr(unit, phy_acc, &reg_val_en));
    for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
        if (bitmap & (0x1 << i)) {
            soc_reg_field_set(unit, CLPORT_TXPI_CSR_OVER_ENr, &reg_val_en, field[i], override->enable ? TRUE : FALSE);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_TXPI_CSR_OVER_ENr(unit, phy_acc, reg_val_en));

    /* Write TXPI DSM override value. Low 32 bits must be wroten before high register
     * reg_value = txpi_dsm_value_1ppm * override->value;
    */
    COMPILER_64_SET(reg_value, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
    COMPILER_64_UMUL_32(reg_value, override->value);

    for (i = 0; i < MAX_PORTS_PER_PM4X25; i++) {
        if (bitmap & (0x1 << i)) {
            soc_reg32_set(unit, over_reg_low[i], phy_acc, 0, override->enable ? COMPILER_64_LO(reg_value) : 0);
            soc_reg32_set(unit, over_reg_high[i], phy_acc, 0, override->enable ? (COMPILER_64_HI(reg_value) & 0xffff) : 0);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_port_txpi_override_get
 *
 *Purpose:
 *  Get Portmacro TXPI SDM override status and value
 *  Defaults to using value from CMIC
 *  This is a port level API
*/
int pm4x25_port_txpi_override_get (int unit, int port, pm_info_t pm_info, portmod_txpi_override_t *override)
{
    int phy_acc;
    uint32 reg_val_en, bitmap;
    int first_index;
    uint32 reg_value_lo, reg_value_hi;
    int64 tmp_reg_val64, txpi_dsm_value_1ppm;

    soc_field_t field[MAX_PORTS_PER_PM4X25] = {TXPI_CSR_OVER_EN_0f, TXPI_CSR_OVER_EN_1f, TXPI_CSR_OVER_EN_2f, TXPI_CSR_OVER_EN_3f};
    soc_reg_t over_reg_low[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_CSR_OVER_VAL_0_LOWr, CLPORT_TXPI_CSR_OVER_VAL_1_LOWr,
                                                    CLPORT_TXPI_CSR_OVER_VAL_2_LOWr, CLPORT_TXPI_CSR_OVER_VAL_3_LOWr};
    soc_reg_t over_reg_high[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_CSR_OVER_VAL_0_HIGHr, CLPORT_TXPI_CSR_OVER_VAL_1_HIGHr,
                                                    CLPORT_TXPI_CSR_OVER_VAL_2_HIGHr, CLPORT_TXPI_CSR_OVER_VAL_3_HIGHr};
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
     _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /*Get enable status of TXPI DSM override */
    _SOC_IF_ERR_EXIT(READ_CLPORT_TXPI_CSR_OVER_ENr(unit, phy_acc, &reg_val_en));
    override->enable = soc_reg_field_get(unit, CLPORT_TXPI_CSR_OVER_ENr, reg_val_en, field[first_index]);

    /* Get TXPI DSM override value */
    if (override->enable == TRUE) {
        /*  The LOW register should be read first before reading from the HIGH register. */
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, over_reg_low[first_index],
                                       phy_acc, 0, &reg_value_lo));
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, over_reg_high[first_index],
                         phy_acc, 0, &reg_value_hi));
        /*
         * bit47 : 0 -- positive; 1 -- negative.
         * If it is negative, the high 16 bits must be filled with 1.
         */
        reg_value_hi = (reg_value_hi & 0x8000) ? (reg_value_hi | 0xffff0000) : (reg_value_hi & 0xffff);
        /*
         *Use COMPILER_64_XXX macros to put together 64 bit variables txpi_dsm_value_1ppm and tmp_reg_val64
         *and then generate the override->value
         */
        COMPILER_64_SET(txpi_dsm_value_1ppm, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
        COMPILER_64_SET(tmp_reg_val64, reg_value_hi, reg_value_lo);
        COMPILER_64_UDIV_64(tmp_reg_val64, txpi_dsm_value_1ppm);

        COMPILER_64_TO_32_LO(override->value, tmp_reg_val64);
    } else {
        override->value = 0;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 *Function:
 *  pm4x25_port_txpi_sdm_data_get
 *
 *Purpose:
 *  Get Portmacro TXPI SDM value of resolution
 *  This is a port level API
*/
int pm4x25_port_txpi_sdm_data_get(int unit, int port, pm_info_t pm_info, int *value)

{
    int phy_acc;
    uint32 bitmap;
    int first_index;
    uint32 reg_value_lo, reg_value_hi;
    int64 tmp_reg_val64, txpi_dsm_value_1ppm;

    soc_reg_t data_reg_low[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_DATA_OUT_STS_0_LOWr, CLPORT_TXPI_DATA_OUT_STS_1_LOWr,
                                                    CLPORT_TXPI_DATA_OUT_STS_2_LOWr, CLPORT_TXPI_DATA_OUT_STS_3_LOWr};
    soc_reg_t data_reg_high[MAX_PORTS_PER_PM4X25] = {CLPORT_TXPI_DATA_OUT_STS_0_HIGHr, CLPORT_TXPI_DATA_OUT_STS_1_HIGHr,
                                                    CLPORT_TXPI_DATA_OUT_STS_2_HIGHr, CLPORT_TXPI_DATA_OUT_STS_3_HIGHr};
    SOC_INIT_FUNC_DEFS;

    PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);
    _SOC_IF_ERR_EXIT(_pm4x25_port_index_get(unit, port, pm_info, &first_index, &bitmap));

    /* get the first sub-port if the port is dual-lane or quad-lane mode.
     * The LOW register should be read first before reading from the HIGH register.
     */
    _SOC_IF_ERR_EXIT(soc_reg32_get(unit, data_reg_low[first_index],
                                   phy_acc, 0, &reg_value_lo));
    _SOC_IF_ERR_EXIT(soc_reg32_get(unit, data_reg_high[first_index],
                                   phy_acc, 0, &reg_value_hi));
    /*
     * bit47 : 0 -- positive; 1 -- negative.
     * If it is negative, the high 16 bits must be filled with 1.
     */
    reg_value_hi = (reg_value_hi & 0x8000) ? (reg_value_hi | 0xffff0000) : (reg_value_hi & 0xffff);

    /*
     *Use COMPILER_64_XXX macros to put together 64 bit variables txpi_dsm_value_1ppm and tmp_reg_val64
     *and then generate the override->value
     */
    COMPILER_64_SET(txpi_dsm_value_1ppm, TXPI_DSM_VALUE_1PPM_HI, TXPI_DSM_VALUE_1PPM_LO);
    COMPILER_64_SET(tmp_reg_val64, reg_value_hi, reg_value_lo);
    COMPILER_64_UDIV_64(tmp_reg_val64, txpi_dsm_value_1ppm);

    COMPILER_64_TO_32_LO(*value, tmp_reg_val64);
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_synce_clk_ctrl_set(int unit, int port, pm_info_t pm_info,
                                   const portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);
    phy_synce_cfg.stg0_mode = cfg->stg0_mode;
    phy_synce_cfg.stg1_mode = cfg->stg1_mode;
    phy_synce_cfg.sdm_val = cfg->sdm_val;

    _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_set(&phy_access[0],
                                                   phy_synce_cfg));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_synce_clk_ctrl_get(int unit, int port, pm_info_t pm_info,
                                   portmod_port_synce_clk_ctrl_t* cfg)
{
    phymod_synce_clk_ctrl_t phy_synce_cfg;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    phymod_synce_clk_ctrl_t_init(&phy_synce_cfg);

    _SOC_IF_ERR_EXIT(phymod_phy_synce_clk_ctrl_get(&phy_access[0],
                                                    &phy_synce_cfg));

    cfg->stg0_mode = phy_synce_cfg.stg0_mode;
    cfg->stg1_mode = phy_synce_cfg.stg1_mode;
    cfg->sdm_val = phy_synce_cfg.sdm_val;

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get the VCO rates from given PCS bypassed port speed
 */
STATIC
int _pm4x25_shared_pcs_bypassed_vco_get(int speed, portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    switch (speed) {
        case 28125:
            *vco = portmodVCO28P125G;
            break;
        case 27343:
            *vco = portmodVCO27P343G;
            break;
        case 25781:
            *vco = portmodVCO25P781G;
            break;
        case 25000:
        case 12500:
            *vco = portmodVCO25G;
            break;
        case 20625:
        case 10312:
            *vco = portmodVCO20P625G;
            break;
        default:
            *vco = portmodVCOInvalid;
            break;
    }
    SOC_FUNC_RETURN;
}

/*
 * given the port speed, number of lanes, and FEC type, return the VCO specified in
 * pm4x25_fs_ability_table in the associated entry; VCO should be portmodVCOInvalid
 * if the ability is invalid
 */
STATIC
int _pm4x25_fs_ability_table_vco_get(uint32 speed, uint32 num_lanes, portmod_vco_type_t* vco)
{
    int i;
    SOC_INIT_FUNC_DEFS;

    *vco = portmodVCOInvalid;
    for (i = 0; i < PM4X25_FS_ABILITY_TABLE_SIZE; i++)
    {
        if (pm4x25_fs_ability_table[i].speed == speed &&
            pm4x25_fs_ability_table[i].num_lanes == num_lanes)
        {
            *vco = pm4x25_fs_ability_table[i].vco;
            break;
        }
    }

    SOC_FUNC_RETURN;
}

/* This function will check the speed config is valid or not, and return required vco for valid input */
/* The supported speed config in this function is based on PM8x50 Portmod Spec */
STATIC
int _pm4x25_port_speed_config_to_vco_get(int unit, const portmod_speed_config_t* speed_config,
                                         int speed_for_pcs_bypass_port,
                                         portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    if (speed_for_pcs_bypass_port)
    {
        _SOC_IF_ERR_EXIT(_pm4x25_shared_pcs_bypassed_vco_get(speed_config->speed, vco));
    }
    else
    {
        _SOC_IF_ERR_EXIT(_pm4x25_fs_ability_table_vco_get(speed_config->speed, speed_config->num_lane, vco));
    }
    /*
     * when *vco == portmodVCOInvalid, it means the entered combination of
     * port speed, number of lanes, and FEC type is not supported.
     */
    if (*vco == portmodVCOInvalid)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG,(_SOC_MSG("Can not support speed: %d num_lane: %d"), speed_config->speed, speed_config->num_lane));
    }
exit:
    SOC_FUNC_RETURN;
}
#ifdef PORTMOD_CPM4X25_SUPPORT

STATIC int _pm4x25_speed_to_vco(int unit, int speed, int num_lanes, int serdes_1000x_at_25g_vco, int serdes_10g_at_25g_vco, int *vco, uint32_t *pll_mul)
{
    switch (num_lanes){
        case 1:
            if(speed == 1000){
                if(serdes_1000x_at_25g_vco) {
                    *vco = portmodVCO25G;
                    *pll_mul = 165;
                } else {
                    *vco = portmodVCO20G;
                    *pll_mul = 132;
                }
            }else if (speed == 2500){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 5000){
                *vco = portmodVCO20G;
                *pll_mul = 132;
            }else if (speed == 10000){
                if(serdes_10g_at_25g_vco) {
                    *vco = portmodVCO25G;
                    *pll_mul = 165;
                } else {
                    *vco = portmodVCO20G;
                    *pll_mul = 132;
                }
            }else if (speed == 20000){
                *vco = portmodVCO20G;
                *pll_mul = 132;
            }else if (speed == 25000){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 11000){
                *vco = portmodVCO21G;
                *pll_mul = 140;
            } else if (speed == 21000){
                *vco = portmodVCO21G;
                *pll_mul = 140;
            } else if(speed == 27000){
                *vco = portmodVCO27G;
                *pll_mul = 175;
            }
            break;
        case 2:
            if(speed == 20000){
                *vco = portmodVCO20G;
                *pll_mul = 132;
            }else if (speed == 40000){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 50000){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 21000){
                *vco = portmodVCO21G;
                *pll_mul = 140;
            } else if (speed == 42000){
                *vco = portmodVCO21G;
                *pll_mul = 140;
            } else if(speed == 53000){
                *vco = portmodVCO27G;
                *pll_mul = 175;
            }
            break;
        case 4:
            if(speed == 40000){
                *vco = portmodVCO20G;
                *pll_mul = 132;
            }else if (speed == 50000){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 100000){
                *vco = portmodVCO25G;
                *pll_mul = 165;
            }else if (speed == 42000){
                *vco = portmodVCO21G;
                *pll_mul = 140;
            } else if (speed == 53000){
                *vco = portmodVCO27G;
                *pll_mul = 175;
            } else if(speed == 106000){
                *vco = portmodVCO27G;
                *pll_mul = 175;
            }
            break;
        default:
             LOG_ERROR(BSL_LS_SOC_PORT,
              (BSL_META_U(unit, "fail to get vcos -- no vco matches lane %d speed %d\n"),
               num_lanes, speed));
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

#endif

/*
 * Validate if a set of speed can be configured for given PORTMACRO without
 * affecting the other active ports.
 *
 * Inputs:
 *     unit:           unit number;
 *     pm_id           portmacro ID;
 *     pm_info:        pm_info data structure;
 *     ports:          Logical port bitmaps which reqiring the input vcos;
 *     flag:           PLL switch flag.
 *     vco_setting:    information about the speeds to validate: num_speeds, speed_config_list, port_starting_lane_list, speed_for_pcs_bypass_port
 *
 * Output:
 *     vco_setting:    If validation pass, vco_setting will indicating
 *                     what's the new vco rates and whether they are new or not; tvco, is_tvco_new (not using the OVCO fields)
 */
int pm4x25_pm_speed_config_validate(int unit,
                                    int pm_id,
                                    pm_info_t pm_info,
                                    const portmod_pbmp_t* ports,
                                    int flag,
                                    portmod_pm_vco_setting_t* vco_setting)
{
    portmod_vco_type_t vco = portmodVCOInvalid, new_vco = portmodVCOInvalid, current_vco;
    portmod_pbmp_t pm_all_ports;
    int all_pm_ports_on_list = TRUE;
    int port_i, tmp_port, i;
    SOC_INIT_FUNC_DEFS;

    PORTMOD_PBMP_CLEAR(pm_all_ports);

    /** make sure all of the speeds in the list are supported by the same VCO */
    for (i = 0; i < vco_setting->num_speeds; i++)
    {
        _SOC_IF_ERR_EXIT(_pm4x25_port_speed_config_to_vco_get(unit, &(vco_setting->speed_config_list[i]),
                                                  vco_setting->speed_for_pcs_bypass_port[i], &vco));
        if (new_vco == portmodVCOInvalid)
        {
            /** first VCO. we save it in order to make sure the entire list is supported using this VCO */
            new_vco = vco;
        }
        else if (vco != new_vco)
        {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Too many VCOs given in the speed config list")));
        }
    }

    /** Create a bitmap of all ports on the PM */
    for(i = 0 ;i < MAX_PORTS_PER_PM4X25; ++i)
    {
        _SOC_IF_ERR_EXIT(PM4x25_LANE2PORT_GET(unit, pm_info, i, &tmp_port));
        if (tmp_port != -1)
        {
            PORTMOD_PBMP_PORT_ADD(pm_all_ports, tmp_port);
        }
    }

    /** Check if all PMs ports are on the list */
    PORTMOD_PBMP_ITER(pm_all_ports, port_i)
    {
        if (!PORTMOD_PBMP_MEMBER(*ports, port_i))
        {
            all_pm_ports_on_list = FALSE;
        }
    }

    /** get current VCO */
    _SOC_IF_ERR_EXIT(PM4x25_VCO_GET(unit, pm_info, &current_vco));

    if (all_pm_ports_on_list || current_vco == portmodVCOInvalid || current_vco == new_vco)
    {
        vco_setting->tvco = new_vco;
        vco_setting->is_tvco_new = (current_vco == new_vco) ? FALSE : TRUE;
    }
    else
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("VCO Mismatch! current VCO is:%d while requested VCO is: %d"), current_vco, new_vco));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * This API reconfig the VCO rates of the PM.
 *
 * 'pm_speed_config_validate' need to be called before this function.
 * Only when pm_speed_config_validate reuturns E_NONE and indicates
 * VCO change requirement, user can call this API to change VCOs.
 *
 * here we only update the TVCO, since pm4x25 has only one VCO.
 * vco[0]: TVCO.
 */
int pm4x25_pm_vco_reconfig(int unit,
                           int pm_id,
                           pm_info_t pm_info,
                           const portmod_vco_type_t* vco)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(PM4x25_VCO_SET(unit, pm_info, vco[0]));
exit:
    SOC_FUNC_RETURN;
}

STATIC
int pm4x25_port_speed_config_set_verify(int unit,
                                 int port,
                                 pm_info_t pm_info,
                                 const portmod_speed_config_t* speed_config)
{
    int lane_config = speed_config->lane_config;
    SOC_INIT_FUNC_DEFS;

    /** Verify FEC */
    switch (speed_config->fec)
    {
    case _SHR_PORT_PHY_FEC_NONE:
    case _SHR_PORT_PHY_FEC_BASE_R:
    case _SHR_PORT_PHY_FEC_RS_FEC:
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("FEC type %d not supported"), speed_config->fec));
    }

    /** Verify Lane Config */
    if (PORTMOD_PORT_PHY_LANE_CONFIG_BR_DFE_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_ES_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NS_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(lane_config) ||
        PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_GET(lane_config))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unsupported feature used in Lane Config")));
    }

exit:
    SOC_FUNC_RETURN;
}

/** Get portmod interface type according to portmod_speed_config_t */
STATIC
int _pm4x25_port_speed_config_to_portmod_intf_get(int unit,
                                                  const portmod_speed_config_t *speed_config,
                                                  soc_port_if_t *interface)
{
    phymod_firmware_media_type_t media_type;
    SOC_INIT_FUNC_DEFS;

    media_type = PORTMOD_PORT_PHY_LANE_CONFIG_MEDIUM_GET(speed_config->lane_config);

    if (speed_config->speed == 1000)
    {
        /* 1G */
        *interface = SOC_PORT_IF_SGMII;
    }
    else if (media_type == _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE)
    {
        switch (speed_config->num_lane)
        {
            /* Copper */
            case 1:
                *interface = SOC_PORT_IF_CR;
                break;
            case 2:
                *interface = SOC_PORT_IF_CR2;
                break;
            case 4:
                *interface = SOC_PORT_IF_CR4;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unsupported Number of Lanes %d\n"), speed_config->num_lane));
        }
    } else if (media_type == _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS) {
        switch (speed_config->num_lane)
        {
            /* Fiber */
            case 1:
                *interface = SOC_PORT_IF_SR;
                break;
            case 2:
                *interface = SOC_PORT_IF_SR2;
                break;
            case 4:
                *interface = SOC_PORT_IF_SR4;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unsupported Number of Lanes %d\n"), speed_config->num_lane));
        }
    } else {
        switch (speed_config->num_lane)
        {
            /* Backplane */
            case 1:
                *interface = SOC_PORT_IF_KR;
                break;
            case 2:
                *interface = SOC_PORT_IF_KR2;
                break;
            case 4:
                *interface = SOC_PORT_IF_KR4;
                break;
            default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Unsupported Number of Lanes %d\n"), speed_config->num_lane));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* set the speed config for the specified port.*/
int pm4x25_port_speed_config_set(int unit,
                                 int port,
                                 pm_info_t pm_info,
                                 const portmod_speed_config_t* speed_config)
{
    portmod_port_interface_config_t if_config;
    phymod_firmware_media_type_t media_type;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_firmware_lane_config_t fw_config;
    int dfe, lp_dfe, unreliable_los, cl72_polarity, cl72_restart_timeout, value;
    int nof_phys;
    int port_index;
    uint32_t bitmap;
    SOC_INIT_FUNC_DEFS;

    /** Verify requested configuration */
    _SOC_IF_ERR_EXIT(pm4x25_port_speed_config_set_verify(unit, port, pm_info, speed_config));

    /** get current configuration */
    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &if_config));
    _SOC_IF_ERR_EXIT(pm4x25_port_interface_config_get(unit, port, pm_info, &if_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

    /** configure new speed */
    if_config.speed = speed_config->speed;

    /** configure new media type */
    PHYMOD_INTF_MODES_FIBER_CLR(&if_config);
    PHYMOD_INTF_MODES_COPPER_CLR(&if_config);
    PHYMOD_INTF_MODES_BACKPLANE_CLR(&if_config);
    media_type = PORTMOD_PORT_PHY_LANE_CONFIG_MEDIUM_GET(speed_config->lane_config);
    if (media_type == _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE)
    {
        PHYMOD_INTF_MODES_COPPER_SET(&if_config);
    }
    else if (media_type == _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS)
    {
        PHYMOD_INTF_MODES_FIBER_SET(&if_config);
    }
    else /** _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE */
    {
        PHYMOD_INTF_MODES_BACKPLANE_SET(&if_config);
    }

    /*
     * In case of Ethernet interface:
     *      update interface type according to speed and number of lanes
     */
    if (if_config.interface != SOC_PORT_IF_ILKN)
    {
        _SOC_IF_ERR_EXIT(_pm4x25_port_speed_config_to_portmod_intf_get(unit, speed_config, &if_config.interface));
        _SOC_IF_ERR_EXIT(_pm4x25_port_index_get (unit, port, pm_info, &port_index, &bitmap));
        _SOC_IF_ERR_EXIT
            (PM4x25_INTERFACE_TYPE_SET(unit, pm_info, if_config.interface, port_index));
    }

    /** set new speed and media type */
    _SOC_IF_ERR_EXIT(pm4x25_port_interface_config_set(unit, port, pm_info, &if_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

    /** get phy access */
    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    /** DFE */
    dfe = PORTMOD_PORT_PHY_LANE_CONFIG_DFE_GET(speed_config->lane_config);

    /** LP DFE */
    lp_dfe = PORTMOD_PORT_PHY_LANE_CONFIG_LP_DFE_GET(speed_config->lane_config);

    /** unreliable los */
    unreliable_los = PORTMOD_PORT_PHY_LANE_CONFIG_UNRELIABLE_LOS_GET(speed_config->lane_config);

    /** CL72 polarity auto enable */
    cl72_polarity = PORTMOD_PORT_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_GET(speed_config->lane_config);

    /** CL72 restart timeout enbale */
    cl72_restart_timeout = PORTMOD_PORT_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_GET(speed_config->lane_config);

    _SOC_IF_ERR_EXIT(portmod_port_phychain_firmware_lane_config_get(unit, phy_access, nof_phys, &fw_config));
    fw_config.DfeOn = dfe;
    fw_config.LpDfeOn = lp_dfe;
    fw_config.UnreliableLos = unreliable_los;
    fw_config.Cl72AutoPolEn = cl72_polarity;
    fw_config.Cl72RestTO = cl72_restart_timeout;
    _SOC_IF_ERR_EXIT(portmod_port_phychain_firmware_lane_config_set(unit, phy_access, nof_phys, fw_config));

    /** FEC */
    if (speed_config->fec == _SHR_PORT_PHY_FEC_BASE_R)
    {
        /* enable CL74 */
        value = 1;
        PHYMOD_FEC_CL91_CLR(value);
        PHYMOD_FEC_CL108_CLR(value);
    }
    else if (speed_config->fec == _SHR_PORT_PHY_FEC_RS_FEC)
    {
        /* enable CL91 */
        value = 1;
        PHYMOD_FEC_CL91_SET(value);
        PHYMOD_FEC_CL108_CLR(value);
    }
    else /** _SHR_PORT_PHY_FEC_NONE*/
    {
        /* disable CL74 */
        value = 0;
        PHYMOD_FEC_CL91_CLR(value);
        PHYMOD_FEC_CL108_CLR(value);
        _SOC_IF_ERR_EXIT(pm4x25_port_fec_enable_set(unit, port, pm_info, 0, value));

        /* disable CL91 */
        value = 0;
        PHYMOD_FEC_CL91_SET(value);
        PHYMOD_FEC_CL108_CLR(value);

    }
    _SOC_IF_ERR_EXIT(pm4x25_port_fec_enable_set(unit, port, pm_info, 0, value));

    /** link training */
    _SOC_IF_ERR_EXIT(pm4x25_port_cl72_set(unit, port, pm_info, speed_config->link_training));
exit:
    SOC_FUNC_RETURN;
}

/* get the speed config for the specified port.*/
int pm4x25_port_speed_config_get(int unit,
                                 int port,
                                 pm_info_t pm_info,
                                 portmod_speed_config_t* speed_config)
{
    portmod_port_interface_config_t if_config;
    phymod_firmware_media_type_t media_type;
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    phymod_firmware_lane_config_t fw_config;
    int nof_phys;
    uint32 link_training_en;
    int num_lane;
    phymod_autoneg_status_t an_status;
    uint32 cl74_enable = 0, cl91_enable = 0, cl108_enable = 0;
    SOC_INIT_FUNC_DEFS;

    /** clear speed_config */
    _SOC_IF_ERR_EXIT(portmod_speed_config_t_init(unit, speed_config));

    /** Clear lane config */
    speed_config->lane_config = 0;

    /* get number of port lanes */
    _SOC_IF_ERR_EXIT
        (pm4x25_port_lane_count_get(unit, port, pm_info,
                                    PORTMOD_SIDE_LINE, &num_lane));

    _SOC_IF_ERR_EXIT(pm4x25_port_autoneg_status_get(unit, port, pm_info, &an_status));

    if (an_status.enabled && an_status.locked) {
        speed_config->num_lane = an_status.resolved_num_lane;
    } else {
        speed_config->num_lane = num_lane;
    }

    /** get configuration */
    _SOC_IF_ERR_EXIT(pm4x25_port_interface_config_get(unit, port, pm_info, &if_config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

    /** get the speed */
    speed_config->speed = if_config.speed;

    /** get media type */
    if (PHYMOD_INTF_MODES_FIBER_GET(&if_config))
    {
        media_type = _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_OPTICS;
    }
    else if (PHYMOD_INTF_MODES_COPPER_GET(&if_config))
    {
        media_type = _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_COPPER_CABLE;
    }
    else /** _SHR_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE */
    {
        media_type = BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_BACKPLANE;
    }
    PORTMOD_PORT_PHY_LANE_CONFIG_MEDIUM_SET(speed_config->lane_config, media_type);

    /** get phy access */
    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    /** get firmware lane config from HW */
    _SOC_IF_ERR_EXIT(portmod_port_phychain_firmware_lane_config_get(unit, phy_access, nof_phys, &fw_config));

    /** DFE */
    if (fw_config.DfeOn)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_DFE_SET(speed_config->lane_config);
    }

    /** LP DFE */
    if (fw_config.LpDfeOn)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_LP_DFE_SET(speed_config->lane_config);
    }

    /** unreliable los */
    if (fw_config.UnreliableLos)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_UNRELIABLE_LOS_SET(speed_config->lane_config);
    }

    /** CL72 polarity auto enable */
    if (fw_config.Cl72AutoPolEn)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_SET(speed_config->lane_config);
    }

    /** CL72 restart timeout enbale */
    if (fw_config.Cl72RestTO)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(speed_config->lane_config);
    }

    /** Force NRZ */
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_SET(speed_config->lane_config);

    /** FEC */
    _SOC_IF_ERR_EXIT(pm4x25_port_fec_enable_get(unit, port, pm_info, 0, &cl74_enable));
    /* Get CL91 */
    PHYMOD_FEC_CL91_SET(cl91_enable);
    _SOC_IF_ERR_EXIT(pm4x25_port_fec_enable_get(unit, port, pm_info, 0, &cl91_enable));
    /* Get CL108 */
    PHYMOD_FEC_CL108_SET(cl108_enable);
    _SOC_IF_ERR_EXIT(pm4x25_port_fec_enable_get(unit, port, pm_info, 0, &cl108_enable));
    if (cl74_enable)
    {
        /* CL74 enabled */
        speed_config->fec = _SHR_PORT_PHY_FEC_BASE_R;
    }
    else if (cl91_enable || cl108_enable)
    {
        speed_config->fec = _SHR_PORT_PHY_FEC_RS_FEC;
    }
    else
    {
        /** no FEC is active */
        speed_config->fec = _SHR_PORT_PHY_FEC_NONE;
    }

    /** link training */
    _SOC_IF_ERR_EXIT(pm4x25_port_cl72_get(unit, port, pm_info, &link_training_en));
    speed_config->link_training = link_training_en;
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_txpi_rxcdr_clk_get(int unit,
                                   int port,
                                   pm_info_t pm_info,
                                   uint32* txpi_clk, 
                                   uint32* rxcdr_clk)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    uint32_t pll_multiplier, temp_clk_khz;
    phymod_osr_mode_t osr_mode;
    phymod_ref_clk_t ref_clk;
    uint32_t temp_cdr_clk;
    uint64_t temp_cdr_clk_64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info,
                                                       phy_access ,(1+MAX_PHYN),
                                                       &nof_phys));

    /*
     * next get the current port PLL divider.
     * set PLL_INDEX_BY_LANE flag for phymod
     * to get pll_idx from lane number.
     */
    PHYMOD_ACC_F_PLL_INDEX_BY_LANE_SET(&phy_access[0].access);
    _SOC_IF_ERR_EXIT(phymod_phy_pll_multiplier_get(&phy_access[0],
                                                    &pll_multiplier));

    /*next get the current port PLL divider */
    _SOC_IF_ERR_EXIT(phymod_phy_osr_mode_get(&phy_access[0],
                                                    &osr_mode));
    /* get the rf clock */
    _SOC_IF_ERR_EXIT(phymod_phy_ref_clk_get(&phy_access[0],
                                                    &ref_clk));
    /*first calculate rx cdr clock, which is VCO/40 */
    switch (ref_clk)  {
    case phymodRefClk156Mhz:
        temp_clk_khz = 156250;
        break;
    case phymodRefClk125Mhz:
        temp_clk_khz = 125000;
        break;
    default:
        temp_clk_khz = 156250;
        break;
    }

    /*first calculate rx cdr clock, which is VCO/(40 * osr)  expect 1G.
     * 1G should be VCO/40.
     */
    *rxcdr_clk = (temp_clk_khz * 100 * pll_multiplier) / 40;

    /*next convert it to Hz */
    *rxcdr_clk *= 10;

    switch (osr_mode) {
        case phymodOversampleMode1:
            temp_cdr_clk = *rxcdr_clk;
            break;
        case phymodOversampleMode2:
            temp_cdr_clk = *rxcdr_clk / 2;
            break;
        case phymodOversampleMode3:
            temp_cdr_clk = *rxcdr_clk / 3;
            break;
        case phymodOversampleMode3P3:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 3 * 10 + 3);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        case phymodOversampleMode4:
            temp_cdr_clk = *rxcdr_clk / 4;
            break;
        case phymodOversampleMode5:
            temp_cdr_clk = *rxcdr_clk / 5;
            break;
        case phymodOversampleMode7P5:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 7 * 10 + 5);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        case phymodOversampleMode8:
            temp_cdr_clk = *rxcdr_clk / 8;
            break;
        case phymodOversampleMode8P25:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 100);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 8 * 100 + 25);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
           break;
        case phymodOversampleMode10:
            temp_cdr_clk = *rxcdr_clk / 10;
            break;
        case phymodOversampleMode16P5:
        case phymodOversampleMode20P625:
            temp_cdr_clk = *rxcdr_clk;
            break;
        case phymodOversampleMode2P5:
            COMPILER_64_SET(temp_cdr_clk_64, 0, *rxcdr_clk);
            COMPILER_64_UMUL_32(temp_cdr_clk_64, 10);
            COMPILER_64_UDIV_32(temp_cdr_clk_64, 2 * 10 + 5);
            temp_cdr_clk = COMPILER_64_LO(temp_cdr_clk_64);
            break;
        default:
            temp_cdr_clk = *rxcdr_clk;
    }

    *txpi_clk = temp_cdr_clk;
    *rxcdr_clk = temp_cdr_clk;

exit:
    SOC_FUNC_RETURN;

}

int pm4x25_egr_1588_timestamp_config_set(int unit, int port, pm_info_t pm_info,
                          portmod_egr_1588_timestamp_config_t timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    int phy_acc;
#ifdef PORTMOD_PM4X25TD_SUPPORT
    int rv;
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    SOC_INIT_FUNC_DEFS;

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = pm4x25td_egr_1588_timestamp_config_set(unit, port, pm_info, timestamp_config);
        _SOC_IF_ERR_EXIT(rv);
    } else
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

        if (timestamp_config.timestamp_mode == portmodTimestampMode32bit) {
            timestamp_mode = PM4X25_EGR_1588_TIMESTAMP_MODE_32BIT;
        } else if (timestamp_config.timestamp_mode == portmodTimestampMode48bit) {
            timestamp_mode = PM4X25_EGR_1588_TIMESTAMP_MODE_48BIT;
        } else {
            return SOC_E_PARAM;
        }

        _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &reg_val));

        soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val,
                          EGR_1588_TIMESTAMPING_MODEf, timestamp_mode);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &reg_val,
                          EGR_1588_TIMESTAMPING_CMIC_48_ENf,
                          timestamp_config.cmic_48_overr_en ? 1 : 0);
        _SOC_IF_ERR_EXIT(WRITE_CLPORT_MODE_REGr(unit, phy_acc, reg_val));
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_egr_1588_timestamp_config_get(int unit, int port, pm_info_t pm_info,
                         portmod_egr_1588_timestamp_config_t* timestamp_config)
{
    uint32 reg_val, timestamp_mode;
    int phy_acc;
#ifdef PORTMOD_PM4X25TD_SUPPORT
    int  rv;
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    SOC_INIT_FUNC_DEFS;

#ifdef PORTMOD_PM4X25TD_SUPPORT
    if (pm_info->type == portmodDispatchTypePm4x25td) {
        rv = pm4x25td_egr_1588_timestamp_config_get(unit, port, pm_info, timestamp_config);
        _SOC_IF_ERR_EXIT(rv);
    } else
#endif /* PORTMOD_PM4X25TD_SUPPORT */
    {
        PM4x25_PHY_ACCESS_GET(unit, port, pm_info, phy_acc);

        _SOC_IF_ERR_EXIT(READ_CLPORT_MODE_REGr(unit, phy_acc, &reg_val));
        timestamp_mode = soc_reg_field_get(unit, CLPORT_MODE_REGr, reg_val,
                                           EGR_1588_TIMESTAMPING_MODEf);

        if (timestamp_mode == PM4X25_EGR_1588_TIMESTAMP_MODE_32BIT) {
            timestamp_config->timestamp_mode = portmodTimestampMode32bit;
        } else {
            timestamp_config->timestamp_mode = portmodTimestampMode48bit;
        }

        timestamp_config->cmic_48_overr_en = soc_reg_field_get(unit, CLPORT_MODE_REGr, reg_val,
                                                               EGR_1588_TIMESTAMPING_CMIC_48_ENf);
    }
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_pmd_lock_counter_get(int unit, int port, pm_info_t pm_info, uint32* pmd_lock_counter_sum)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    /* get phy access */
    _SOC_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access ,(1+MAX_PHYN), &nof_phys));

    /* get pmd_lock counter */
    _SOC_IF_ERR_EXIT(portmod_port_phychain_rx_pmd_lock_counter_get(unit, phy_access, nof_phys, pmd_lock_counter_sum));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_tx_stall_set(int unit, int port, pm_info_t pm_info, int value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_tx_stall_enable_set(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}


int pm4x25_port_tx_stall_get(int unit, int port, pm_info_t pm_info, int* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(clmac_tx_stall_enable_get(unit, port, value));
exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_fec_error_inject_set(int unit, int port, pm_info_t pm_info,
                                     uint16 error_control_map,
                                     portmod_fec_error_mask_t bit_error_mask)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_set(&phy_access[0], error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

int pm4x25_port_fec_error_inject_get(int unit, int port, pm_info_t pm_info,
                                     uint16* error_control_map,
                                     portmod_fec_error_mask_t* bit_error_mask)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (phymod_phy_fec_error_inject_get(&phy_access[0], error_control_map, bit_error_mask));

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM4X25_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
