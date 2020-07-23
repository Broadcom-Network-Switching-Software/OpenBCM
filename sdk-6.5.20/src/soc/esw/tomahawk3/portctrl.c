/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * portctrl soc routines
 */
#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/tomahawk3.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>
#include <soc/init/tomahawk3_ep_init.h>

#ifdef BCM_TOMAHAWK3_SUPPORT

/* Calculation same as PM addr calculation below, but Management PM is on Bus 11
 * 0x80 | 0x260 | 1
 */
static uint32_t _th3_pm4x10_addr[SOC_TH3_PM4X10_COUNT] = { 0x2E1 };
static uint32_t _th3_pm4x10_port[SOC_TH3_PM4X10_COUNT] = { SOC_TH3_PM4X10_PORT_1};

/* This holds PM VCO info during the flexport operation.  It is used
 * during the validation stage to hold the VCOs prior to writing them
 * back to SOC_INFO and flexing
 */
static soc_pmvco_info_t out_pmvco_info[SOC_MAX_NUM_BLKS];

#define SOC_TH3_PM8X50_PORT_1        1
#define SOC_TH3_PM8X50_PORT_2        9
#define SOC_TH3_PM8X50_PORT_3        17
#define SOC_TH3_PM8X50_PORT_4        25
#define SOC_TH3_PM8X50_PORT_5        33
#define SOC_TH3_PM8X50_PORT_6        41
#define SOC_TH3_PM8X50_PORT_7        49
#define SOC_TH3_PM8X50_PORT_8        57
#define SOC_TH3_PM8X50_PORT_9        65
#define SOC_TH3_PM8X50_PORT_10       73
#define SOC_TH3_PM8X50_PORT_11       81
#define SOC_TH3_PM8X50_PORT_12       89
#define SOC_TH3_PM8X50_PORT_13       97
#define SOC_TH3_PM8X50_PORT_14       105
#define SOC_TH3_PM8X50_PORT_15       113
#define SOC_TH3_PM8X50_PORT_16       121
#define SOC_TH3_PM8X50_PORT_17       129
#define SOC_TH3_PM8X50_PORT_18       137
#define SOC_TH3_PM8X50_PORT_19       145
#define SOC_TH3_PM8X50_PORT_20       153
#define SOC_TH3_PM8X50_PORT_21       161
#define SOC_TH3_PM8X50_PORT_22       169
#define SOC_TH3_PM8X50_PORT_23       177
#define SOC_TH3_PM8X50_PORT_24       185
#define SOC_TH3_PM8X50_PORT_25       193
#define SOC_TH3_PM8X50_PORT_26       201
#define SOC_TH3_PM8X50_PORT_27       209
#define SOC_TH3_PM8X50_PORT_28       217
#define SOC_TH3_PM8X50_PORT_29       225
#define SOC_TH3_PM8X50_PORT_30       233
#define SOC_TH3_PM8X50_PORT_31       241
#define SOC_TH3_PM8X50_PORT_32       249

/* PM Address calculation:
 * 0x80 | bus_id_encoding | (phy port - bus offset)
 * where bus_id_encoding = bit {9,8,6,5}
 *
 * So PM0 = 0x80 | 0x0  | (1 - 0)   = 0x81
 *    PM3 = 0x80 | 0x20 | (25 - 24) = 0xa1
 *    PM6 = 0x80 | 0x40 | (49 - 48) = 0xc1
 *    PM7 = 0x80 | 0x40 | (57 - 48) = 0xc9
 *    PM9 = 0x80 | 0x60 | (73 - 72) = 0xe1
 *    PM12 = 0x80 | 0x100 | (97 - 96) = 0x181
 *    PM15 = 0x80 | 0x120 | (121 - 120) = 0x1a1
 *    PM18 = 0x80 | 0x140 | (145 - 144) = 0x1c1
 *    PM21 = 0x80 | 0x160 | (169 - 168) = 0x1e1
 *    PM24 = 0x80 | 0x200 | (193 - 192) = 0x281
 *    PM27 = 0x80 | 0x220 | (217 - 216) = 0x2a1
 *    PM30 = 0x80 | 0x240 | (241 - 240) = 0x2c1
 */
static uint32_t _th3_pm8x50_addr[SOC_TH3_PM8X50_COUNT] = {
    0x81,  0x89,  0x91,     /* Bus 0 has PM 0-2 */
    0xa1,  0xa9,  0xb1,     /* Bus 1 has PM 3-5 */
    0xc1,  0xc9,  0xd1,     /* Bus 2 has PM 6-8 */
    0xe1,  0xe9,  0xf1,     /* Bus 3 has PM 9-11 */
    0x181, 0x189, 0x191,    /* Bus 4 has PM 12-14 */
    0x1a1, 0x1a9, 0x1b1,    /* Bus 5 has PM 15-17 */
    0x1c1, 0x1c9, 0x1d1,    /* Bus 6 has PM 18-20 */
    0x1e1, 0x1e9, 0x1f1,    /* Bus 7 has PM 21-23 */
    0x281, 0x289, 0x291,    /* Bus 8 has PM 24-26 */
    0x2a1, 0x2a9, 0x2b1,    /* Bus 9 has PM 27-29 */
    0x2c1, 0x2c9            /* Bus 10 has PM 30-31 */
};

static uint32_t _th3_pm8x50_core_num[SOC_TH3_PM8X50_COUNT] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

static uint32_t _th3_pm4x10_core_num[SOC_TH3_PM4X10_COUNT] = {32};

static uint32_t _th3_pm8x50_port[SOC_TH3_PM8X50_COUNT] = {
    SOC_TH3_PM8X50_PORT_1,  SOC_TH3_PM8X50_PORT_2,
    SOC_TH3_PM8X50_PORT_3,  SOC_TH3_PM8X50_PORT_4,
    SOC_TH3_PM8X50_PORT_5,  SOC_TH3_PM8X50_PORT_6,
    SOC_TH3_PM8X50_PORT_7,  SOC_TH3_PM8X50_PORT_8,
    SOC_TH3_PM8X50_PORT_9,  SOC_TH3_PM8X50_PORT_10,
    SOC_TH3_PM8X50_PORT_11, SOC_TH3_PM8X50_PORT_12,
    SOC_TH3_PM8X50_PORT_13, SOC_TH3_PM8X50_PORT_14,
    SOC_TH3_PM8X50_PORT_15, SOC_TH3_PM8X50_PORT_16,
    SOC_TH3_PM8X50_PORT_17, SOC_TH3_PM8X50_PORT_18,
    SOC_TH3_PM8X50_PORT_19, SOC_TH3_PM8X50_PORT_20,
    SOC_TH3_PM8X50_PORT_21, SOC_TH3_PM8X50_PORT_22,
    SOC_TH3_PM8X50_PORT_23, SOC_TH3_PM8X50_PORT_24,
    SOC_TH3_PM8X50_PORT_25, SOC_TH3_PM8X50_PORT_26,
    SOC_TH3_PM8X50_PORT_27, SOC_TH3_PM8X50_PORT_28,
    SOC_TH3_PM8X50_PORT_29, SOC_TH3_PM8X50_PORT_30,
    SOC_TH3_PM8X50_PORT_31, SOC_TH3_PM8X50_PORT_32
};

portmod_pm_instances_t th3_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10, SOC_TH3_PM4X10_COUNT}, 
#endif
#ifdef PORTMOD_PM8X50_SUPPORT
    {portmodDispatchTypePm8x50, SOC_TH3_PM8X50_COUNT},
#endif
};  

portmod_pm_instances_t th3_4pipe_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10, SOC_TH3_PM4X10_COUNT},
#endif
#ifdef PORTMOD_PM8X50_SUPPORT
    {portmodDispatchTypePm8x50, (SOC_TH3_PM8X50_COUNT)},
#endif
};

#define SOC_TH3_MAX_PHYS             (261)    /* (1x4) 4 XLMAC's, (32x8) 256 CDMAC's */

/*
 *  TH3 CMIC to PCS in PM8X50 Port Macros delay in nanoseconds.
 *  60ns: CMIC to PM8x50 Port Macros stage delay.
 *  12ns: Stage delay within Port Macros.
 */
#define SOC_TH3_PM8X50_CMIC_TO_PM_DELAY_IN_NS (72)

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_th3_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM8X50_SUPPORT
static portmod_default_user_access_t *pm8x50_th3_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM8X50_SUPPORT */

soc_portctrl_functions_t soc_th3_portctrl_func = {
    soc_th3_portctrl_pm_portmod_init,
    soc_th3_portctrl_pm_portmod_deinit,
    soc_th3_portctrl_pm_port_config_get,
    soc_th3_portctrl_pm_port_phyaddr_get,
    NULL,
    soc_th3_portctrl_pm_type_get,
    NULL,
    NULL,
    soc_th3_portctrl_pm_vco_store_clear,
    soc_th3_portctrl_pm_vco_store,
    soc_th3_portctrl_pm_vco_fetch,
    soc_th3_portctrl_pm_vco_reconfigure,
    soc_th3_portctrl_sbus_broadcast_setup
};

#if 0
/* Commented out for now.  However, these registers still exist in TH3,
 * so this function may be needed in the future
 */
STATIC int
_soc_th3_portctrl_rescal_calculate(int unit, int *val1, int *val2)
{
    uint32 rval, rescal0, rescal1, rescal2, rescal3;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = PONf;

    SOC_IF_ERROR_RETURN(READ_TOP_RESCAL_0_STATUS_0r(unit,&rval));
    reg = TOP_RESCAL_0_STATUS_0r;
    rescal0 = soc_reg_field_get(unit, reg, rval, field);

    SOC_IF_ERROR_RETURN(READ_TOP_RESCAL_1_STATUS_0r(unit,&rval));
    reg = TOP_RESCAL_1_STATUS_0r;
    rescal1 = soc_reg_field_get(unit, reg, rval, field);

    SOC_IF_ERROR_RETURN(READ_TOP_RESCAL_2_STATUS_0r(unit,&rval));
    reg = TOP_RESCAL_2_STATUS_0r;
    rescal2 = soc_reg_field_get(unit, reg, rval, field);

    SOC_IF_ERROR_RETURN(READ_TOP_RESCAL_3_STATUS_0r(unit,&rval));
    reg = TOP_RESCAL_3_STATUS_0r;
    rescal3 = soc_reg_field_get(unit, reg, rval, field);

    *val1 = (rescal0 + rescal1) / 2;
    *val2 = (rescal2 + rescal3) / 2;

    return SOC_E_NONE;
}
#endif

STATIC int
_soc_th3_portctrl_pm_init(int unit, int *max_phys,
                         portmod_pm_instances_t **pm_types,
                         int *pms_arr_len)
{
    *max_phys = SOC_TH3_MAX_PHYS;
    if (NUM_PIPE(unit) == 4) {
        *pm_types = th3_4pipe_pm_types;
        *pms_arr_len = sizeof(th3_4pipe_pm_types)/sizeof(portmod_pm_instances_t);
    } else {
        *pm_types = th3_pm_types;
        *pms_arr_len = sizeof(th3_pm_types)/sizeof(portmod_pm_instances_t);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th3_portctrl_device_addr_port_get(int unit, int pm_type,
                                      uint32_t **addr,
                                      uint32_t **port,
                                      uint32_t **core_num)
{
    *addr     = NULL;
    *port     = NULL;
    *core_num = NULL;
    if (pm_type == portmodDispatchTypePm4x10) {
        *addr = _th3_pm4x10_addr;
        *port = _th3_pm4x10_port;
        *core_num = _th3_pm4x10_core_num;
    } else if (pm_type == portmodDispatchTypePm8x50){
        *addr = _th3_pm8x50_addr;
        *port = _th3_pm8x50_port;
        *core_num = _th3_pm8x50_core_num;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th3_portctrl_pm_user_acc_flag_set(int unit,
                    portmod_default_user_access_t* pm_user_access)
{
    PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((pm_user_access));
    PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((pm_user_access));
    return SOC_E_NONE;
}

/*
 * Polarity get looks for physical port based config.
 */
STATIC void
_soc_th3_portctrl_pm_port_polarity_get(int unit, int logical_port, int physical_port,
                            int lane, uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity;

    rx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                                        spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0x0);
    *rx_pol = (rx_polarity & 0x1) << lane;

    tx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                                        spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0x0);
    *tx_pol = (tx_polarity & 0x1) << lane;
}

STATIC void
_soc_th3_portctrl_pm_polarity_get(int unit, int physical_port,
                            uint32 *tx_pol, uint32* rx_pol)
{
    *rx_pol = soc_property_phys_port_get(unit, physical_port,
                            spn_SERDES_CORE_RX_POLARITY_FLIP_PHYSICAL, 0);
    *tx_pol = soc_property_phys_port_get(unit, physical_port,
                            spn_SERDES_CORE_TX_POLARITY_FLIP_PHYSICAL, 0);
}


/*
 * lanemap get looks for physical port based config. 
 */
STATIC 
void _soc_th3_portctrl_pm_port_lanemap_get
            (int unit, int logical_port, int physical_port, int core_num, 
            uint32 *txlane_map, uint32* rxlane_map)
{
    /* If this is the management port PM, this is a 4x10, so the lane mapping
     * will be different than the 8x50 */
    if (core_num == 32) {
        *rxlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                    spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0x3210);


        *txlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                    spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0x3210);
    } else {
        *rxlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                    spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0x76543210);


        *txlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                    spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0x76543210);
    }
}


#define NRZ_MAX_SPEED_PER_LANE 28125
STATIC
int _soc_th3_portctrl_port_serdes_tx_taps_get
            (int unit, int logical_port, int *tx_params_user_flag,
             phymod_tx_t *tx_params)
{
    char *config_str, *sub_str, *sub_str_end;
    char str_buf[8];
    int16_t *field = NULL;
    static const char nrz[] = "nrz";
    static const char pam4[] = "pam4";
    soc_info_t *si = &SOC_INFO(unit);
    int i, temp, speed_per_lane, rv = SOC_E_NONE;

    config_str = soc_property_port_get_str(unit, logical_port, spn_SERDES_TX_TAPS);

    /* If this config is not specified, then SDK will use its own default tx taps val */
    if (NULL == config_str) {
        *tx_params_user_flag = 0;
        return rv;
    }

    /* User defined serdes_tx_taps */
    *tx_params_user_flag = 1;

    sub_str = config_str;

    /** Parse signalling mode **/
    /* Copy over nrz or pam4 */
    for (i = 0; i < sizeof(pam4); i++) {
        if (sub_str[i] == ':' || sub_str[i] == '\0') {
            break;
        }
        str_buf[i] = sub_str[i];
    }
    str_buf[i] = '\0';

    /* If pam4 */
    if (!sal_strcmp(str_buf, pam4)) {
        tx_params->sig_method = phymodSignallingMethodPAM4;
    } else if (!sal_strcmp(str_buf, nrz)) {
        /* nrz */
        tx_params->sig_method = phymodSignallingMethodNRZ;
    } else {
        LOG_CLI((BSL_META_U(unit,
                        "SERDES_TX_TAPS: Missing signalling mode: %s\n"),
                        str_buf));
        rv = SOC_E_CONFIG;
        goto end;
    }

    /* Compare signalling mode against speed and # of lanes.
     * NRZ must be less than 28125 per lane.
     * PAM4 must be greater than or equal to 50000 per lane.
     */
    /* following tx taps check only applies to init, after init,
     * this check will be bypassed */
    if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED)) {
        speed_per_lane = si->port_speed_max[logical_port] /
                         si->port_num_lanes[logical_port];
        if ((speed_per_lane > NRZ_MAX_SPEED_PER_LANE &&
                tx_params->sig_method == phymodSignallingMethodNRZ) ||
            (speed_per_lane < NRZ_MAX_SPEED_PER_LANE &&
                tx_params->sig_method == phymodSignallingMethodPAM4)) {
            LOG_CLI((BSL_META_U(unit,
                            "SERDES_TX_TAPS: Port %d speed per lane %d "
                            "not supported for signalling mode %s\n"),
                            logical_port, speed_per_lane, str_buf));
            rv = SOC_E_CONFIG;
            goto end;
        }
    }

    /* Set the sub_str to after the signalling type */
    sub_str = &sub_str[i];

    /* Start off at 3 tap mode.  If it goes to 6, set that later */
    tx_params->tap_mode = phymodTxTapMode3Tap;

    /* Parse each of the different fields after the signalling type */
    for (i = 0; i < 6; i++) {
        /* Set the field to fetch */
        switch(i) {
            case 0:
                field = &tx_params->pre;
                break;
            case 1:
                field = &tx_params->main;
                break;
            case 2:
                field = &tx_params->post;
                break;
            case 3:
                field = &tx_params->pre2;
                break;
            case 4:
                field = &tx_params->post2;
                break;
            case 5:
                field = &tx_params->post3;
                tx_params->tap_mode = phymodTxTapMode6Tap;
                break;
            default:
                /* Shouldn't reach this */
                break;
        }

        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                "Port %d: Bad tx taps string\"%s\"\n"),
                            logical_port, config_str));
                rv = SOC_E_CONFIG;
                goto end;
            }
            sub_str++;
        } else {
            /* Only 3 taps is fine; go to the end */
            if (i == 3) {
                goto end;
            }

            LOG_CLI((BSL_META_U(unit,
                "SERDES_TX_TAPS port %d: tx taps string missing a field\"%s\"\n"),
                        logical_port, config_str));
            rv = SOC_E_CONFIG;
            goto end;
        }

        /* fetch field */
        temp = sal_ctoi(sub_str, &sub_str_end);
        *field = (int16_t)temp;

        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                "SERDES_TX_TAPS port %d: tx taps string missing a field\"%s\"\n"),
                        logical_port, config_str));
            rv = SOC_E_CONFIG;
            goto end;
        }

        sub_str = sub_str_end;
    }

end:

    return rv;
}
#undef NRZ_MAX_SPEED_PER_LANE

STATIC int
_soc_th3_portctrl_pm4x10_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt;
    int port_num_lanes = 0;
    uint32 *pAddr, *pPort, *pCoreNum;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN
        (_soc_th3_portctrl_device_addr_port_get(unit,
                                                portmodDispatchTypePm4x10,
                                                &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                               num_of_instances,
                                               &pm4x10_th3_user_acc[unit]));

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < num_of_instances); pmid++) {

        rv = portmod_pm_create_info_t_init(unit, &pm_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        /* PM info */
        first_port = pPort[pmid];

        /* Only two physical ports are used in management pm4x10, 257, 258.
         * However, each management port can use multiple phy lanes in PM.
         * As a result, we need to initialize multiple phy lanes in Portmod. */
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+1);

        /* Even though we see physical ports 257 and 258 as the management,
         * it's actually more like 257 and 259 in port macro.
         */
        if (NUM_PIPE(unit) == 8) {
            PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+2);
            PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+3);
        }

        /* Check if mgmt ports is configured as 4 lanes */
        if (si->port_l2p_mapping[38] != -1) {
            port_num_lanes = si->port_num_lanes[38];
            if (port_num_lanes == 4) {
                PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+2);
                PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+3);
            }
        }

        found = 0;
        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){
            blk = SOC_PORT_IDX_INFO(unit, first_port, idx).blk;
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        } 

        pm_info.type         = portmodDispatchTypePm4x10;

        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x10_th3_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_th3_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_th3_user_acc[unit][pmid])));

        pm4x10_th3_user_acc[unit][pmid].unit = unit;
        pm4x10_th3_user_acc[unit][pmid].blk_id = blk; 
        pm4x10_th3_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_th3_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_th3_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x10.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x10.access.bus      = NULL; /* Use default bus */
#if !defined (EXCLUDE_PHYMOD_TSCE16_SIM) && defined (PHYMOD_TSCE16_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTsce16,
                                  &(pm_info.pm_specific_info.pm4x10.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_info.pm_specific_info.pm4x10.fw_load_method =
                phymodFirmwareLoadMethodNone;
            /* TSCE sim supports CL45 mode to read/write registers. */
            PHYMOD_ACC_F_CLAUSE45_SET(&pm_info.pm_specific_info.pm4x10.access);
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x10.external_fw_loader = NULL;

        core_cnt = 1; 

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x10.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x10.core_num_int = 0;
            pm_info.pm_specific_info.pm4x10.rescal = -1;
        }

        pm_info.pm_specific_info.pm4x10.portmod_phy_external_reset
            = soc_esw_portctrl_reset_tsc3_cb;

        /* 4x10 is only used in mgmt ports */
        pm_info.pm_specific_info.pm4x10.port_mode_aux_info = portmodModeInfoTwoDualModePorts;
        
        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid = 0; pmid < num_of_instances; pmid++) {
            if (pm4x10_th3_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_th3_user_acc[unit][pmid].mutex);
                pm4x10_th3_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

/*
 * Function:
 *      soc_th3_portctrl_ep_rst_egr_enable_cb
 * Purpose:
 *      Callback function from portmod to perform EP Soft Reset for the port
 *      and to write EGR_ENABLE.
 *
 *      NOTE: This is ONLY for SW 400G Autoneg!
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 *      port             - (IN) Logical Port
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_th3_portctrl_ep_rst_egr_enable_cb(int unit, int port)
{
    int phy_port;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_credit_rst_port(unit, phy_port));

    return SOC_E_NONE;
}

STATIC int
_soc_th3_portctrl_pm8x50_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM8X50_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt;
    uint32 *pAddr, *pPort,*pCoreNum;
    uint16 dev_id;
    uint8 rev_id;

    SOC_IF_ERROR_RETURN
        (_soc_th3_portctrl_device_addr_port_get(unit,
                                                portmodDispatchTypePm8x50,
                                                &pAddr, &pPort,&pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                               num_of_instances,
                                               &pm8x50_th3_user_acc[unit]));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < num_of_instances); pmid++) {

        rv = portmod_pm_create_info_t_init(unit, &pm_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        /* PM info */
        first_port = pPort[pmid];

        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+1); 
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+2);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+3);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+4);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+5);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+6);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port+7);

        found = 0;
        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){ 
            blk = SOC_PORT_IDX_INFO(unit, first_port, idx).blk; 
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CDPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        } 

        pm_info.type         = portmodDispatchTypePm8x50;

        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm8x50_th3_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm8x50_th3_user_acc[unit][pmid])));
        PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((&(pm8x50_th3_user_acc[unit][pmid])));
        pm8x50_th3_user_acc[unit][pmid].unit = unit;
        pm8x50_th3_user_acc[unit][pmid].blk_id = blk; 
        pm8x50_th3_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm8x50_th3_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM8x50 */
        rv = phymod_phy_access_t_init(&pm_info.pm_specific_info.pm8x50.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm8x50.access.access.user_acc =
            &(pm8x50_th3_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm8x50.access.access.addr = pAddr[pmid];
        /* Use default bus */
        pm_info.pm_specific_info.pm8x50.access.access.bus  = NULL;
        /* Use PLL1 as TVCO source */
        pm_info.pm_specific_info.pm8x50.access.access.tvco_pll_index = 1;
#if !defined (EXCLUDE_PHYMOD_TSCBH_SIM) && defined (PHYMOD_TSCBH_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTscbh, 
                            &(pm_info.pm_specific_info.pm8x50.access.access),
                            0, &is_sim);
#else
        is_sim = 0;
#endif

        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_info.pm_specific_info.pm8x50.fw_load_method =
                phymodFirmwareLoadMethodNone;
            /* TSCF sim supports CL45 mode to read/write registers. */
            PHYMOD_ACC_F_CLAUSE45_SET(
                        &pm_info.pm_specific_info.pm8x50.access.access);
            PHYMOD_ACC_F_PHYSIM_SET(
                        &pm_info.pm_specific_info.pm8x50.access.access);
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm8x50.external_fw_loader = NULL;

        core_cnt = 1; 

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm8x50.core_num = pCoreNum? pCoreNum[pmid] : pmid;
        }

        /* Set the TVCO and OVCO for the Port Macro */
        pm_info.pm_specific_info.pm8x50.tvco = SOC_INFO(unit).pm_vco_info[pmid].tvco;
        pm_info.pm_specific_info.pm8x50.ovco = SOC_INFO(unit).pm_vco_info[pmid].ovco;


        /* Tell Portmod which Port Macros are the Master Port Macros, meaning
         * they drive the clock for the slave port macros. */
        if (rev_id == BCM56980_A0_REV_ID) {
            /* In TH3 A0, the "inner" port macros on a pipe are the Masters.
             * PM 0 is driven by PM 1 and PM 3 is driven by PM 2.
             */
            if (pmid == 1 || pmid == 2 || pmid == 5 || pmid == 6 || pmid == 9 ||
                pmid == 10 || pmid == 13 || pmid == 14 || pmid == 17 || pmid == 18 ||
                pmid == 21 || pmid == 22 || pmid == 25 || pmid == 26 || pmid == 29 ||
                pmid == 30) {
                pm_info.pm_specific_info.pm8x50.is_master_pm = 1;
            } else {
                pm_info.pm_specific_info.pm8x50.is_master_pm = 0;
            }
        } else {
            /* In TH3 B0, there is only 1 master PM per pipe */
            if (pmid == 3 || pmid == 4 || pmid == 11 || pmid == 12 ||
                pmid == 19 || pmid == 20 || pmid == 27 || pmid == 28) {
                pm_info.pm_specific_info.pm8x50.is_master_pm = 1;
            } else {
                pm_info.pm_specific_info.pm8x50.is_master_pm = 0;
            }
        }

        /* Add callback function for portmod to reset EP and enable
         * port in 400G AN */
        pm_info.pm_specific_info.pm8x50.portmod_egress_buffer_reset =
            soc_th3_portctrl_ep_rst_egr_enable_cb;

        /* Add CMIC to PCS delay on TH3 in nanoseconds. */
        pm_info.pm_specific_info.pm8x50.pm_offset = SOC_TH3_PM8X50_CMIC_TO_PM_DELAY_IN_NS;

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid = 0; pmid < num_of_instances; pmid++) {
            if (pm8x50_th3_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm8x50_th3_user_acc[unit][pmid].mutex);
                pm8x50_th3_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM8X50_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM8X50_SUPPORT */
    return 0;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Tomahawk3
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;
    int flags = 0;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_th3_portctrl_pm_init(unit, &max_phys,
                                 &pm_types, &pms_arr_len));
    if (pms_arr_len == 0) {
        return SOC_E_UNAVAIL;
    }

    /*
     * If portmod was create - destroy and create again
     * Better way would be to create once and leave it.
     */
    if (SOC_E_NONE == soc_esw_portctrl_init_check(unit)) {
        SOC_IF_ERROR_RETURN(portmod_destroy(unit));
    }

    PORTMOD_CREATE_F_PM_NULL_SET(flags);

    if (soc_property_get(unit, spn_PORTMOD_THREAD_DISABLE, 0)) {
        PORTMOD_CREATE_F_PORTMOD_THREAD_DISABLE_SET(flags);
    }

    SOC_IF_ERROR_RETURN
        (portmod_create(unit, flags, SOC_MAX_NUM_PORTS, max_phys,
                        pms_arr_len, pm_types));

    for (count = 0; SOC_SUCCESS(rv) && (count < pms_arr_len); count++) {
#ifdef PORTMOD_PM4X10_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x10) {
            rv = _soc_th3_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else 
#endif /* PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM8X50_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm8x50) {
            rv = _soc_th3_portctrl_pm8x50_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM8X50_SUPPORT  */
        {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Tomahawk3.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_th3_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_th3_user_acc[unit] != NULL) {
        sal_free(pm4x10_th3_user_acc[unit]);
        pm4x10_th3_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM8X50_SUPPORT
    if (pm8x50_th3_user_acc[unit] != NULL) {
        sal_free(pm8x50_th3_user_acc[unit]);
        pm8x50_th3_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM8X50_SUPPORT */

    SOC_IF_ERROR_RETURN(portmod_destroy(unit));

    return SOC_E_NONE;
}

int
soc_th3_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config)
{
#if defined (PORTMOD_PM4X10_SUPPORT) || defined(PORTMOD_PM8X50_SUPPORT)
    int rv = SOC_E_NONE;
    int pmid = 0, lane, phy, phy_port, logical_port;
    int first_port, core_num, core_count = 0, core_index, is_sim;
    int type = portmodDispatchTypeCount;
    uint32 txlane_map, rxlane_map, i;
    uint32 *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;
    uint32 tx_polarity, rx_polarity;
    portmod_port_init_config_t *port_config_info;
    phymod_firmware_load_method_t fw_load_method;
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map;
    portmod_dispatch_type_t pm_type = portmodDispatchTypeCount;
    int num_lanes = 0;

    port_config_info = (portmod_port_init_config_t *)port_config;

    phy = SOC_INFO(unit).port_l2p_mapping[port];
    /* Tomahawk 3's Management Port Macro requires a spacing between
     * the lanes, so even though the two management ports' physical
     * ports are 257 and 258, the lane needs to be the next one,
     * so logical port 118 being mapped to physical port 258 needs
     * to pass 259 to portmod because portmod only recognizes lane
     * 0 (physical port 257) and lane 2 (physial port 259).
     */
    if (phy == 258) {
        phy = 259;
    }

    rv = portmod_phy_pm_type_get(unit, phy, &pm_type);
    if (rv < 0) {
        return rv;
    }

    core_num = -1;

    switch (pm_type) {
      case portmodDispatchTypePm4x10:
        num_lanes = SOC_PM4X10_NUM_LANES;
        core_num = 32;
        break;
      case portmodDispatchTypePm8x50:
        num_lanes = 8;
        core_num = (phy - 1)/num_lanes ;
        break;
      default:
        return SOC_E_INTERNAL;
        break;
    }

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    for (i=0; i<SOC_TH3_PM4X10_COUNT; i++) {
        if (core_num == _th3_pm4x10_core_num[i]) {
            type = portmodDispatchTypePm4x10;
            SOC_IF_ERROR_RETURN
                (_soc_th3_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x10,
                                                 &pAddr, &pPort, &pCoreNum));
            pmid = i;
            core_count = 1;
            break;
        }
    }
    /* When port dones't find PM4x10, find PM8x50 again. */
    if (i == SOC_TH3_PM4X10_COUNT) {
        for (i=0; i<SOC_TH3_PM8X50_COUNT; i++) {
            if (core_num == _th3_pm8x50_core_num[i]) {
                type = portmodDispatchTypePm8x50;
                SOC_IF_ERROR_RETURN
                    (_soc_th3_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm8x50,
                                                 &pAddr, &pPort, &pCoreNum));
                pmid = i;
                core_count = 1;
                break;
            }
        }
    }

    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Parse TX TAPS info from config file for the port */
    SOC_IF_ERROR_RETURN(_soc_th3_portctrl_port_serdes_tx_taps_get
            (unit, port,
             &port_config_info->tx_params_user_flag[0],
             &port_config_info->tx_params[0]));
    /* Copy the above into each lane for the port */
    for (i = 1; i < SOC_INFO(unit).port_num_lanes[port]; i++) {
        port_config_info->tx_params_user_flag[i] =
                    port_config_info->tx_params_user_flag[0];

        port_config_info->tx_params[i].sig_method =
                    port_config_info->tx_params[0].sig_method;

        port_config_info->tx_params[i].tap_mode =
                    port_config_info->tx_params[0].tap_mode;

        port_config_info->tx_params[i].pre =
                    port_config_info->tx_params[0].pre;

        port_config_info->tx_params[i].main =
                    port_config_info->tx_params[0].main;

        port_config_info->tx_params[i].post =
                    port_config_info->tx_params[0].post;

        port_config_info->tx_params[i].pre2 =
                    port_config_info->tx_params[0].pre2;

        port_config_info->tx_params[i].post2 =
                    port_config_info->tx_params[0].post2;

        port_config_info->tx_params[i].post3 =
                    port_config_info->tx_params[0].post3;
    }


    /* PM info */
    first_port = pPort[pmid];

    for (core_index = 0; core_index < core_count; core_index++) {
        fw_load_method = phymodFirmwareLoadMethodExternal;

        soc_physim_enable_get(unit, &is_sim);

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            fw_load_method = phymodFirmwareLoadMethodNone;
        } else {
            fw_load_method = soc_property_suffix_num_get(unit, pmid,
                                            spn_LOAD_FIRMWARE, "quad",
                                            phymodFirmwareLoadMethodExternal);
            fw_load_method &= 0xff;
        }

        rv = phymod_polarity_t_init(&polarity);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        /* The PM8x50 has a per core polarity.
         * The phy_port is the first port of the port macro */
        if (type == portmodDispatchTypePm8x50) {
            phy_port = first_port;
            _soc_th3_portctrl_pm_polarity_get(unit, phy_port, &tx_polarity, &rx_polarity);
            polarity.rx_polarity = rx_polarity;
            polarity.tx_polarity = tx_polarity;
        } else {
            /* The PM4x10 has a per lane polarity config */
            for (lane = 0; lane < num_lanes; lane++) {
                phy_port = first_port + lane + (core_index*4);
                logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];

                _soc_th3_portctrl_pm_port_polarity_get(unit, logical_port, phy_port, lane, &tx_polarity, &rx_polarity);

                polarity.rx_polarity |= rx_polarity;
                polarity.tx_polarity |= tx_polarity;
            }
        }

        /* Lane map */
        rv = phymod_lane_map_t_init(&lane_map);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        phy_port = first_port + (core_index*4);
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];

        _soc_th3_portctrl_pm_port_lanemap_get(unit, logical_port, phy_port, core_num, &txlane_map, &rxlane_map);

        if (type == portmodDispatchTypePm8x50) {
            lane_map.num_of_lanes = SOC_PM8X50_NUM_LANES;
        } else {
            lane_map.num_of_lanes = SOC_PM4X10_NUM_LANES;
        }

#define BITS_PER_LANE   4
#define LANE_BITMASK    0xF

        /* For a PM8x50, the default lane will come back as 0x76543210, so
         * we're going to store, for example, the number 7 as the 7th lane
         * in lane_map_tx[].  So, shift it right by (lane * 4) bits, which is
         * 7 * 4 = 28.  Then mask those 4 bits to store the 7 into
         * lane_map_tx[7].
         * If the 6 was going to be stored, shift right by 6 * 4 bits, mask
         * those 4 bits, and store the value into lane_map_tx[6].
         *
         * The PM4x10 has a default lane map of 0x3210 */
        for(lane = 0 ; lane < lane_map.num_of_lanes; lane++) {
            lane_map.lane_map_tx[lane] =
                (txlane_map >> (lane * BITS_PER_LANE)) &
                LANE_BITMASK;
            lane_map.lane_map_rx[lane] =
                (rxlane_map >> (lane * BITS_PER_LANE)) &
                LANE_BITMASK;
        }
#undef BITS_PER_LANE
#undef LANE_BITMASK

        sal_memcpy(&port_config_info->fw_load_method[core_index], &fw_load_method,
                    sizeof(phymod_firmware_load_method_t));
        port_config_info->fw_load_method_overwrite = 1;
        sal_memcpy(&port_config_info->polarity[core_index], &polarity,
                    sizeof(phymod_polarity_t));
        port_config_info->polarity_overwrite = 1;
        sal_memcpy(&port_config_info->lane_map[core_index], &lane_map,
                    sizeof(phymod_lane_map_t));
        port_config_info->lane_map_overwrite = 1;
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

int
soc_th3_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int phy, core_index;
    uint32 i, *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = (phy - 1) / SOC_PM4X10_NUM_LANES;
#ifdef PORTMOD_PM4X10_SUPPORT
    for (i=0; i<SOC_TH3_PM4X10_COUNT; i++) {
        if (core_index == _th3_pm4x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th3_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM8X50_SUPPORT
    /* When port dones't find PM4x10, find PM8x50 again. */
    for (i=0; i<SOC_TH3_PM8X50_COUNT; i++) {
        if (core_index == _th3_pm8x50_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th3_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm8x50,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM8X50_SUPPORT */

    return -1;
}

int
soc_th3_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type)
{
    int rv;

    *pm_type = -1;

    if (phy_port >= 1 && phy_port <= 256) {
#ifdef PORTMOD_PM8X50_SUPPORT
        *pm_type = portmodDispatchTypePm8x50;
        rv = SOC_E_NONE;
#else
        rv = SOC_E_UNAVAIL;
#endif
    } else if (phy_port == 257 /* _TH3_PHY_PORT_MNG0 */ ||
               phy_port == 258 /* _TH3_PHY_PORT_MNG1 */ ||
               phy_port == 259 /* mark 259 as management port because of pm4x10
                                  lane issue */) {
#ifdef PORTMOD_PM4X10_SUPPORT
        *pm_type = portmodDispatchTypePm4x10;
        rv = SOC_E_NONE;
#else
        rv = SOC_E_UNAVAIL;
#endif
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_vco_reconfigure
 * Purpose:
 *      Reconfigure the VCOs of Port Macros that need to be reconfigured.
 *      This normally occurs after a flexport operation. For Port Macros that
 *      have their TVCO changed, the entire port macro needs to be brought down
 *      and back up, so it is NOT necessary to call pm_vco_reconfig for those
 *      Port Macros.
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_pm_vco_reconfigure(int unit)
{
    int num_vco;
    int pm_id, portmod_pm_id;
    int base_phy_port, phy_port, logical_port;
    int is_init;
    portmod_vco_type_t vco[2];

    soc_info_t *si = &SOC_INFO(unit);

    /* Iterate through the affected ports in the PM */
    for (pm_id = 0; pm_id < _TH3_PBLKS_PER_DEV; pm_id++) {
        base_phy_port = (pm_id * _TH3_PORTS_PER_PBLK) + 1;

        logical_port = -1;

        for (phy_port = base_phy_port;
             phy_port < base_phy_port + _TH3_PORTS_PER_PBLK;
             phy_port++) {
            logical_port = si->port_p2l_mapping[phy_port];
            if (SOC_PBMP_PORT_VALID(logical_port)) {
                break;
            }
        }

        /* no logical port found on this PM. Continue to next */
        if (!SOC_PBMP_PORT_VALID(logical_port)) {
            continue;
        }
                    
        sal_memset(vco, 0, sizeof(vco));

        num_vco = 0;
        if ((si->pm_vco_info[pm_id].is_tvco_new == 1) &&
            (si->pm_vco_info[pm_id].is_ovco_new == 1)) {
            /* Reconfigure VCOs only when VCOs in database are newly updated */
            si->pm_vco_info[pm_id].is_tvco_new = 0;
            vco[num_vco] = si->pm_vco_info[pm_id].tvco;
            num_vco++;
            si->pm_vco_info[pm_id].is_ovco_new = 0;
            vco[num_vco] = si->pm_vco_info[pm_id].ovco;
            num_vco++;
        }

        /* No New VCOs; skip this PM */
        if (num_vco == 0) {
            continue;
        }

        /* Fetch the portmod pm id since indexing is different than sdk */
        SOC_IF_ERROR_RETURN(portmod_phy_pm_id_get(unit, base_phy_port,
                    &portmod_pm_id));

        /* Check if the core is initialized. If it's NOT initialized,
         * we don't need to call vco reconfigure because initializing
         * the core will set the VCOs
         */
        SOC_IF_ERROR_RETURN(portmod_pm_is_initialized(unit, portmod_pm_id, &is_init));

        /* If the core isn't initialized, skip this, since core init will
         * set VCO
         */
        if (!PORTMOD_CORE_INIT_FLAG_INITIALZIED_GET(is_init)) {
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
        (BSL_META_U(unit,
                    "  PM VCO reconfigure "
                    "PM=%d TVCO=%d OVCO=%d\n"),
         pm_id, vco[0], vco[1]));

        /* Reconfigure the VCO for this port macro
         * and then move onto the other port macros
         */
        SOC_IF_ERROR_RETURN(portmod_pm_vco_reconfig(unit,
                    portmod_pm_id, vco));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_portctrl_vco_soc_info_update
 * Purpose:
 *      Update the SOC_INFO's VCO information.
 *
 *      NOTE: This is only called at init!
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_vco_soc_info_update(int unit)
{
    int blk_idx;
    int phy_port;
    int pm_type;
    int index;
    int port_list[_TH3_PORTS_PER_PBLK];
    uint32 flags = 0;

    for (blk_idx = 0; blk_idx < _TH3_PBLKS_PER_DEV; blk_idx++) {
        /* clear the port list for each port macro and the index
         * so that it can be filled for each port macro */
        memset(port_list, 0, sizeof(port_list));
        index = 0;
        pm_type = -1;
        for (phy_port = (blk_idx * _TH3_PORTS_PER_PBLK) + 1;
                phy_port <= (blk_idx + 1)*_TH3_PORTS_PER_PBLK;
                phy_port++) {

            /* Set the device specific pm type. This is necessary since
             * portmod has NOT been initialized yet, so querying portmod
             * what kind of pm type it has WILL NOT work. */
            if (pm_type < 0) {
                SOC_IF_ERROR_RETURN(soc_th3_portctrl_pm_type_get(unit, phy_port,
                                &pm_type));
            }

            port_list[index] = phy_port;
            index++;
        }

        /* Set PM version flag for PM8x50 */
        if (pm_type == portmodDispatchTypePm8x50) {
            PORTMOD_PM8X50_REV0_16NM_SET(flags);
        }
        /* Update the tvco and ovco for this port macro so that the info
         * can be passed to portmod during portmod init */
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_pm_update_vco_soc_info(unit,
                                            port_list,
                                            _TH3_PORTS_PER_PBLK,
                                            blk_idx,
                                            pm_type,
                                            flags));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_vco_store_clear
 * Purpose:
 *      Clear the calculated VCO from a flexport operation
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_pm_vco_store_clear(int unit)
{
    soc_pmvco_info_t vco_info;
    int i;

    vco_info.tvco = -1;
    vco_info.ovco = -1;
    vco_info.is_tvco_new = 0;
    vco_info.is_ovco_new = 0;

    for (i = 0; i < SOC_TH3_PM8X50_COUNT; i++) {
        sal_memcpy(&out_pmvco_info[i], &vco_info, sizeof(soc_pmvco_info_t));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_vco_store
 * Purpose:
 *      Store the calculated VCOs from a flexport operation
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 *      index            - (IN) Port macro index
 *      pm_vco_info      - (IN) VCO info
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_pm_vco_store(int unit, int index, const void *pm_vco_info)
{
    soc_pmvco_info_t *vco_info = (soc_pmvco_info_t *)pm_vco_info;

    if (index >= SOC_MAX_NUM_BLKS) {
        return SOC_E_PARAM;
    }
    sal_memcpy(&out_pmvco_info[index], vco_info, sizeof(soc_pmvco_info_t));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_vco_fetch
 * Purpose:
 *      Fetch the calculated VCOs from a flexport operation
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 *      index            - (IN) Port Macro index
 *      pm_vco_info      - (OUT) VCO info
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th3_portctrl_pm_vco_fetch(int unit, int index, void *pm_vco_info)
{
    if (index >= SOC_MAX_NUM_BLKS) {
        return SOC_E_PARAM;
    }

    if (pm_vco_info == NULL) {
        return SOC_E_PARAM;
    }

    sal_memcpy(pm_vco_info, &out_pmvco_info[index], sizeof(soc_pmvco_info_t));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_portctrl_pm_is_active
 * Purpose:
 *      Given a physical port, find out if the PM it is on is active 
 *
 * Parameters:
 *      unit             - (IN) Unit number.
 *      phy_port         - (IN) Physical port
 *      is_active        - (OUT) PM is active or not
 * Returns:
 *      SOC_E_XXX
 */
int soc_th3_portctrl_pm_is_active(int unit, int phy_port, int *is_active)
{
    int portmod_pm_id, rv;

    /* if Portmod isn't up yet, just say the PM is not active */
    if (SOC_E_NONE != soc_esw_portctrl_init_check(unit)) {
        *is_active = 0;
        return SOC_E_NONE;
    }

    if (phy_port < 1 || phy_port > SOC_TH3_PHY_PORTS_PER_DEV) {
        return SOC_E_PORT;
    }

    rv = portmod_phy_pm_id_get(unit, phy_port, &portmod_pm_id);

    if (SOC_SUCCESS(rv)) {
        rv = portmod_pm_is_initialized(unit, portmod_pm_id, is_active);
    }

    return rv;
}

STATIC
int _soc_th3_portctrl_pm8x50_get_active_pms(int unit, int *active_pms, int size)
{
    int blk, port, phy_port, i, pm;
    uint32 pipe_map;
    uint8 rev_id;
    uint16 dev_id;
    soc_info_t *si = &SOC_INFO(unit);

    if (size < _TH3_PBLKS_PER_DEV) {
        return SOC_E_PARAM;
    }

    sal_memset(active_pms, 0, size * sizeof(int));

    /* Mark the PMs that have mappings */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CDPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        phy_port = si->port_l2p_mapping[port];
        /* mark that this PM is active */
        active_pms[(phy_port-1)/_TH3_PORTS_PER_PBLK] = 1;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    /* Mark the Master PMs */
    if (rev_id == BCM56980_A0_REV_ID) {
        for (i = 0; i < 2; i++) {
            for (pm = 1 + i; pm < _TH3_PBLKS_PER_DEV; pm+=4) {
                /* skip if the pm has already been marked */
                if (active_pms[pm] == 1) {
                    continue;
                }

                /* skip a PM if the pipe it is located in is disabled */
                if ((pipe_map & (1 << (pm / _TH3_PBLKS_PER_PIPE))) == 0) {
                    continue;
                }

                active_pms[pm] = 1;
            }
        }
    } else {
        /* In TH3 B0 the master PMs are 3, 4, 11, 12, 19, 20, 27, 28 */
        for (pm = 0; pm < _TH3_PBLKS_PER_DEV; pm++) {
            /* mark if a master PM */
            if ( (pm == 3 || pm == 4 || pm == 11 || pm == 12 ||
                   pm == 19 || pm == 20 || pm == 27 || pm == 28) ) {

                /* skip a PM if the pipe it is located in is disabled */
                if ((pipe_map & (1 << (pm / _TH3_PBLKS_PER_PIPE))) == 0) {
                    continue;
                }

                active_pms[pm] = 1;
            }
        }
    }
    return SOC_E_NONE;
}

STATIC
int _soc_th3_portctrl_sbus_bcast_set_bcast_block(int unit, int phy_port, int value)
{
    uint8 at;
    uint32 raddr, reg_val;
    soc_block_t block;

    raddr = soc_reg_addr_get(unit, CDPORT_SBUS_CONTROLr, phy_port, 0,
            SOC_REG_ADDR_OPTION_PRESERVE_PORT, &block, &at);
    SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, block, at, raddr, &reg_val));

    soc_reg_field_set(unit, CDPORT_SBUS_CONTROLr, &reg_val,
            SBUS_BCAST_BLOCK_IDf, value);

    SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, block, at, raddr, reg_val));

    return SOC_E_NONE;
}

STATIC
int _soc_th3_portctrl_sbus_bcast_assign_bcast_blocks(int unit, int *active_pms, int size)
{
    int i;

    if (size < _TH3_PBLKS_PER_DEV) {
        return SOC_E_PARAM;
    }

    /* Assign the broadcast block for SBUS ring 4 */
    for (i = 0; i < 16; i++) {
        /* Clear bcast block to a disabled PM */
        if (active_pms[i] == 0) {
            SOC_IF_ERROR_RETURN(
                _soc_th3_portctrl_sbus_bcast_set_bcast_block(unit,
                                                1 + i*_TH3_PORTS_PER_PBLK,
                                                _TH3_SBUS_BCAST_BLOCK_DEFAULT));
        } else {
            SOC_IF_ERROR_RETURN(
                _soc_th3_portctrl_sbus_bcast_set_bcast_block(unit,
                                                1 + i*_TH3_PORTS_PER_PBLK,
                                                _TH3_SBUS_BCAST_BLOCK_SBUS4));
        }
    }
    /* Assign the broadcast block for SBUS ring 5 */
    for (i = 16; i < 32; i++) {
        /* Clear bcast block to a disabled PM */
        if (active_pms[i] == 0) {
            SOC_IF_ERROR_RETURN(
                _soc_th3_portctrl_sbus_bcast_set_bcast_block(unit,
                                                1 + i*_TH3_PORTS_PER_PBLK,
                                                _TH3_SBUS_BCAST_BLOCK_DEFAULT));
        }
        SOC_IF_ERROR_RETURN(
                _soc_th3_portctrl_sbus_bcast_set_bcast_block(unit,
                                                1 + i*_TH3_PORTS_PER_PBLK,
                                                _TH3_SBUS_BCAST_BLOCK_SBUS5));
    }

    return SOC_E_NONE;
}

STATIC
int _soc_th3_portctrl_sbus_bcast_assign_last_pm(int unit, int *active_pms, int size)
{
    uint8 at;
    uint32 raddr, reg_val;
    soc_block_t block;
    int phy_port_sbus4, phy_port_sbus5, i, last_pm_active;

    if (size < _TH3_PBLKS_PER_DEV) {
        return SOC_E_PARAM;
    }

    /* Clear all the last PM bits to start off with.  Then set the real
     * last one */
    for (i = 0; i < _TH3_PBLKS_PER_DEV; i++) {
        raddr = soc_reg_addr_get(unit, CDPORT_SBUS_CONTROLr, 1 + (i * _TH3_PORTS_PER_PBLK), 0,
                SOC_REG_ADDR_OPTION_PRESERVE_PORT, &block, &at);
        SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, block, at, raddr, &reg_val));

        soc_reg_field_set(unit, CDPORT_SBUS_CONTROLr, &reg_val, SBUS_CHAIN_LASTf, 0);
    }

    /* Find out which PM is the last active PM for each SBUS */

    /* SBUS 4 starts at 0 and the last PM is 15 */
    last_pm_active = -1;

    /* Ring 4's PM order is 7,6,5,4,3,2,1,0, 8,9,10,11,12,13,14,15, with
     * 7 being the absolute first and 15 being the absolute last possible PM
     * on the ring, so iterate through the PMs like that
     */
    for (i = 7; i >= 0; i--) {
        if (active_pms[i]) {
            last_pm_active = i;
        }
    }

    for (i = 8; i < 16; i++) {
        if (active_pms[i]) {
            last_pm_active = i;
        }
    }
    if (last_pm_active > -1 && last_pm_active < 16) {
        phy_port_sbus4 = 1 + (last_pm_active * _TH3_PORTS_PER_PBLK);
    } else {
        phy_port_sbus4 = -1;
    }

    /* SBUS 5 starts at 31 and the last PM is 16 */
    last_pm_active = 32;

    /* Ring 5's PM order is 24,25,26,27,28,29,30,31, 23,22,21,20,19,18,17,16,
     * with 24 being the absolute first and 16 being the absolute last possible
     * PM on the ring, so iterate through the PMs like that
     */
    for (i = 24; i < 32; i++) {
        if (active_pms[i]) {
            last_pm_active = i;
        }
    }

    for (i = 23; i > 15; i--) {
        if (active_pms[i]) {
            last_pm_active = i;
        }
    }

    if (last_pm_active > 15 && last_pm_active < 32) {
        phy_port_sbus5 = 1 + (last_pm_active * _TH3_PORTS_PER_PBLK);
    } else {
        phy_port_sbus5 = -1;
    }

    /* Setup broadcast indexing with a physical port from the "last PM" */
    if (phy_port_sbus4 > 0) {
        raddr = soc_reg_addr_get(unit, CDPORT_SBUS_CONTROLr, phy_port_sbus4, 0,
                SOC_REG_ADDR_OPTION_PRESERVE_PORT, &block, &at);
        SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, block, at, raddr, &reg_val));

        soc_reg_field_set(unit, CDPORT_SBUS_CONTROLr, &reg_val, SBUS_CHAIN_LASTf, 1);
    }
    SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, block, at, raddr, reg_val));

    if (phy_port_sbus5 > 0) {
        raddr = soc_reg_addr_get(unit, CDPORT_SBUS_CONTROLr, phy_port_sbus5, 0,
                SOC_REG_ADDR_OPTION_PRESERVE_PORT, &block, &at);
        SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, block, at, raddr, &reg_val));

        soc_reg_field_set(unit, CDPORT_SBUS_CONTROLr, &reg_val, SBUS_CHAIN_LASTf, 1);
    }
    SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, block, at, raddr, reg_val));

    return SOC_E_NONE;
}

int
soc_th3_portctrl_sbus_broadcast_setup(int unit)
{
    int active_pms[_TH3_PBLKS_PER_DEV];

    /* Get the active Port Macros in the device */
    SOC_IF_ERROR_RETURN(_soc_th3_portctrl_pm8x50_get_active_pms(unit,
                        active_pms, _TH3_PBLKS_PER_DEV));

    /* Assign Broadcast Blocks to the active Port Macros */
    SOC_IF_ERROR_RETURN(_soc_th3_portctrl_sbus_bcast_assign_bcast_blocks(unit,
                        active_pms, _TH3_PBLKS_PER_DEV));

    /* Set the Last PM bit to the last active PM on the SBUS */
    SOC_IF_ERROR_RETURN(_soc_th3_portctrl_sbus_bcast_assign_last_pm(unit,
                        active_pms, _TH3_PBLKS_PER_DEV));
    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */
