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
#include <soc/tomahawk2.h>
#include <soc/phy/phymod_sim.h>
#include <soc/portmod/portmod.h>
#include <soc/bondoptions.h>

#ifdef BCM_TOMAHAWK2_SUPPORT
#define SOC_TH2_PM4X10_COUNT         1
#define SOC_TH2_PM4X25_COUNT         64 

#define SOC_TH2_PM4X10_PORT_1        257  /* xe0 = 66, xe1 = 100 */

static uint32_t _th2_pm4x10_addr[SOC_TH2_PM4X10_COUNT] = { 0x2C1 };
static uint32_t _th2_pm4x10_port[SOC_TH2_PM4X10_COUNT] = { SOC_TH2_PM4X10_PORT_1};

#define SOC_TH2_PM4X25_PORT_1        1
#define SOC_TH2_PM4X25_PORT_2        5
#define SOC_TH2_PM4X25_PORT_3        9
#define SOC_TH2_PM4X25_PORT_4        13
#define SOC_TH2_PM4X25_PORT_5        17
#define SOC_TH2_PM4X25_PORT_6        21
#define SOC_TH2_PM4X25_PORT_7        25
#define SOC_TH2_PM4X25_PORT_8        29
#define SOC_TH2_PM4X25_PORT_9        33
#define SOC_TH2_PM4X25_PORT_10       37
#define SOC_TH2_PM4X25_PORT_11       41
#define SOC_TH2_PM4X25_PORT_12       45
#define SOC_TH2_PM4X25_PORT_13       49
#define SOC_TH2_PM4X25_PORT_14       53
#define SOC_TH2_PM4X25_PORT_15       57
#define SOC_TH2_PM4X25_PORT_16       61
#define SOC_TH2_PM4X25_PORT_17       65
#define SOC_TH2_PM4X25_PORT_18       69
#define SOC_TH2_PM4X25_PORT_19       73
#define SOC_TH2_PM4X25_PORT_20       77
#define SOC_TH2_PM4X25_PORT_21       81
#define SOC_TH2_PM4X25_PORT_22       85
#define SOC_TH2_PM4X25_PORT_23       89
#define SOC_TH2_PM4X25_PORT_24       93
#define SOC_TH2_PM4X25_PORT_25       97
#define SOC_TH2_PM4X25_PORT_26       101
#define SOC_TH2_PM4X25_PORT_27       105
#define SOC_TH2_PM4X25_PORT_28       109
#define SOC_TH2_PM4X25_PORT_29       113
#define SOC_TH2_PM4X25_PORT_30       117
#define SOC_TH2_PM4X25_PORT_31       121
#define SOC_TH2_PM4X25_PORT_32       125
#define SOC_TH2_PM4X25_PORT_33       129
#define SOC_TH2_PM4X25_PORT_34       133
#define SOC_TH2_PM4X25_PORT_35       137
#define SOC_TH2_PM4X25_PORT_36       141
#define SOC_TH2_PM4X25_PORT_37       145
#define SOC_TH2_PM4X25_PORT_38       149
#define SOC_TH2_PM4X25_PORT_39       153
#define SOC_TH2_PM4X25_PORT_40       157
#define SOC_TH2_PM4X25_PORT_41       161
#define SOC_TH2_PM4X25_PORT_42       165
#define SOC_TH2_PM4X25_PORT_43       169
#define SOC_TH2_PM4X25_PORT_44       173
#define SOC_TH2_PM4X25_PORT_45       177
#define SOC_TH2_PM4X25_PORT_46       181
#define SOC_TH2_PM4X25_PORT_47       185
#define SOC_TH2_PM4X25_PORT_48       189
#define SOC_TH2_PM4X25_PORT_49       193
#define SOC_TH2_PM4X25_PORT_50       197
#define SOC_TH2_PM4X25_PORT_51       201
#define SOC_TH2_PM4X25_PORT_52       205
#define SOC_TH2_PM4X25_PORT_53       209
#define SOC_TH2_PM4X25_PORT_54       213
#define SOC_TH2_PM4X25_PORT_55       217
#define SOC_TH2_PM4X25_PORT_56       221
#define SOC_TH2_PM4X25_PORT_57       225
#define SOC_TH2_PM4X25_PORT_58       229
#define SOC_TH2_PM4X25_PORT_59       233
#define SOC_TH2_PM4X25_PORT_60       237
#define SOC_TH2_PM4X25_PORT_61       241
#define SOC_TH2_PM4X25_PORT_62       245
#define SOC_TH2_PM4X25_PORT_63       249
#define SOC_TH2_PM4X25_PORT_64       253

static uint32_t _th2_pm4x25_addr[SOC_TH2_PM4X25_COUNT] = {
    0x81,  0x85,  0x89,  0x8d,
    0x91,  0x95,  0x99,  0xa1,
    0xa5,  0xa9,  0xad,  0xb1,
    0xb5,  0xb9,  0xc1,  0xc5,
    0xc9,  0xcd,  0xe1,  0xe5,
    0xe9,  0xed,  0xf1,  0xf5,
    0xf9,  0x181, 0x185, 0x189,
    0x18d, 0x191, 0x195, 0x199,
    0x1a1, 0x1a5, 0x1a9, 0x1ad,
    0x1b1, 0x1b5, 0x1b9, 0x1c1,
    0x1c5, 0x1c9, 0x1cd, 0x1d1,
    0x1d5, 0x1d9, 0x1e1, 0x1e5,
    0x1e9, 0x1ed, 0x281, 0x285,
    0x289, 0x28d, 0x291, 0x295,
    0x299, 0x2a1, 0x2a5, 0x2a9,
    0x2ad, 0x2b1, 0x2b5, 0x2b9
};

static uint32_t _th2_pm4x25_core_num[SOC_TH2_PM4X25_COUNT] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
};

static uint32_t _th2_pm4x10_core_num[SOC_TH2_PM4X10_COUNT] = {64};

static uint32_t _th2_pm4x25_port[SOC_TH2_PM4X25_COUNT] = {
    SOC_TH2_PM4X25_PORT_1,  SOC_TH2_PM4X25_PORT_2,
    SOC_TH2_PM4X25_PORT_3,  SOC_TH2_PM4X25_PORT_4,
    SOC_TH2_PM4X25_PORT_5,  SOC_TH2_PM4X25_PORT_6,
    SOC_TH2_PM4X25_PORT_7,  SOC_TH2_PM4X25_PORT_8,
    SOC_TH2_PM4X25_PORT_9,  SOC_TH2_PM4X25_PORT_10,
    SOC_TH2_PM4X25_PORT_11, SOC_TH2_PM4X25_PORT_12,
    SOC_TH2_PM4X25_PORT_13, SOC_TH2_PM4X25_PORT_14,
    SOC_TH2_PM4X25_PORT_15, SOC_TH2_PM4X25_PORT_16,
    SOC_TH2_PM4X25_PORT_17, SOC_TH2_PM4X25_PORT_18,
    SOC_TH2_PM4X25_PORT_19, SOC_TH2_PM4X25_PORT_20,
    SOC_TH2_PM4X25_PORT_21, SOC_TH2_PM4X25_PORT_22,
    SOC_TH2_PM4X25_PORT_23, SOC_TH2_PM4X25_PORT_24,
    SOC_TH2_PM4X25_PORT_25, SOC_TH2_PM4X25_PORT_26,
    SOC_TH2_PM4X25_PORT_27, SOC_TH2_PM4X25_PORT_28,
    SOC_TH2_PM4X25_PORT_29, SOC_TH2_PM4X25_PORT_30,
    SOC_TH2_PM4X25_PORT_31, SOC_TH2_PM4X25_PORT_32,
    SOC_TH2_PM4X25_PORT_33, SOC_TH2_PM4X25_PORT_34,
    SOC_TH2_PM4X25_PORT_35, SOC_TH2_PM4X25_PORT_36,
    SOC_TH2_PM4X25_PORT_37, SOC_TH2_PM4X25_PORT_38,
    SOC_TH2_PM4X25_PORT_39, SOC_TH2_PM4X25_PORT_40,
    SOC_TH2_PM4X25_PORT_41, SOC_TH2_PM4X25_PORT_42,
    SOC_TH2_PM4X25_PORT_43, SOC_TH2_PM4X25_PORT_44,
    SOC_TH2_PM4X25_PORT_45, SOC_TH2_PM4X25_PORT_46,
    SOC_TH2_PM4X25_PORT_47, SOC_TH2_PM4X25_PORT_48,
    SOC_TH2_PM4X25_PORT_49, SOC_TH2_PM4X25_PORT_50,
    SOC_TH2_PM4X25_PORT_51, SOC_TH2_PM4X25_PORT_52,
    SOC_TH2_PM4X25_PORT_53, SOC_TH2_PM4X25_PORT_54,
    SOC_TH2_PM4X25_PORT_55, SOC_TH2_PM4X25_PORT_56,
    SOC_TH2_PM4X25_PORT_57, SOC_TH2_PM4X25_PORT_58,
    SOC_TH2_PM4X25_PORT_59, SOC_TH2_PM4X25_PORT_60,
    SOC_TH2_PM4X25_PORT_61, SOC_TH2_PM4X25_PORT_62,
    SOC_TH2_PM4X25_PORT_63, SOC_TH2_PM4X25_PORT_64
};

portmod_pm_instances_t th2_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10, SOC_TH2_PM4X10_COUNT}, 
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
    {portmodDispatchTypePm4x25, SOC_TH2_PM4X25_COUNT},
#endif
};  

portmod_pm_instances_t th2_2pipe_pm_types[] = {
#ifdef PORTMOD_PM4X10_SUPPORT
    {portmodDispatchTypePm4x10, SOC_TH2_PM4X10_COUNT},
#endif
#ifdef PORTMOD_PM4X25_SUPPORT
    {portmodDispatchTypePm4x25, (SOC_TH2_PM4X25_COUNT / 2)},
#endif
};

#define SOC_TH2_MAX_PHYS             (260)    /* (1x4) 4 XLMAC's, (64x4) 256 CLMAC's */

#define SOC_TH2_TSC_GROUP_NONE 0
#define SOC_TH2_TSC_GROUP_1    1
#define SOC_TH2_TSC_GROUP_2    2

/*
 * array index is core number and value indicates this core number belongs to which TSC group
 * core number from 13 to 18 belongs to TSC group 1
 * core number from 45 to 50 belongs to TSC group 2
 */
static int _th2_core_tsc_group[] = {
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 0~3 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 4~7 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 8~11 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_1,    SOC_TH2_TSC_GROUP_1,    SOC_TH2_TSC_GROUP_1,    /* 12~15 */
    SOC_TH2_TSC_GROUP_1,    SOC_TH2_TSC_GROUP_1,    SOC_TH2_TSC_GROUP_1,    SOC_TH2_TSC_GROUP_NONE, /* 16~19 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 20~23 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 24~27 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 28~31 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 32~35 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 36~39 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 40~43 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_2,    SOC_TH2_TSC_GROUP_2,    SOC_TH2_TSC_GROUP_2,    /* 44~47 */
    SOC_TH2_TSC_GROUP_2,    SOC_TH2_TSC_GROUP_2,    SOC_TH2_TSC_GROUP_2,    SOC_TH2_TSC_GROUP_NONE, /* 48~51 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 52~55 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 56~59 */
    SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, SOC_TH2_TSC_GROUP_NONE, /* 60~63 */
    SOC_TH2_TSC_GROUP_NONE                                                                          /* 64 */
};

#ifdef FW_BCAST_DOWNLOAD
/*
 * PM0-PM31(SBUS chain 15-46) belong to SBUS ring 3, PM0 ->...-> PM15 -> PM31 ->...-> PM16
 * PM32-PM63(SBUS chain 47-78) belong to SBUS ring 4, PM63 ->...-> PM48 -> PM32 ->...-> PM47
 */
#define SOC_TH2_SBUS_RING_3_PM_MIN 0
#define SOC_TH2_SBUS_RING_3_PM_MAX 31
#define SOC_TH2_SBUS_RING_4_PM_MIN 32
#define SOC_TH2_SBUS_RING_4_PM_MAX 63

#define SOC_TH2_SBUS_RING_LIST_NUM 2
#define SOC_TH2_SBUS_RING_MAX_PM_NUM 32
#define SOC_TH2_SBUS_RING_DEFAULT_BCAST_ID 0x7f
#define SOC_TH2_SBUS_RING_3_BCAST_ID (SOC_TH2_SBUS_RING_DEFAULT_BCAST_ID - 1)
#define SOC_TH2_SBUS_RING_4_BCAST_ID (SOC_TH2_SBUS_RING_DEFAULT_BCAST_ID - 2)

typedef struct soc_th2_sbus_ring_list_s {
    int core_id[SOC_TH2_SBUS_RING_MAX_PM_NUM];
    int sbus_ring_id;
    int bcast_id;
} soc_th2_sbus_ring_list_t;

static soc_th2_sbus_ring_list_t _th2_sbus_ring_list[SOC_TH2_SBUS_RING_LIST_NUM] = {
    {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16},
     3, SOC_TH2_SBUS_RING_3_BCAST_ID
    },
    {{63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47},
     4, SOC_TH2_SBUS_RING_4_BCAST_ID
    }
};
#endif

#ifdef PORTMOD_PM4X10_SUPPORT
static portmod_default_user_access_t *pm4x10_th2_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
static portmod_default_user_access_t *pm4x25_th2_user_acc[SOC_MAX_NUM_DEVICES];
#endif /* PORTMOD_PM4X25_SUPPORT */

soc_portctrl_functions_t soc_th2_portctrl_func = {
    soc_th2_portctrl_pm_portmod_init,
    soc_th2_portctrl_pm_portmod_deinit,
    soc_th2_portctrl_pm_port_config_get,
    soc_th2_portctrl_pm_port_phyaddr_get,
    soc_th2_portctrl_port_ability_update,
    soc_th2_portctrl_pm_type_get,
#ifdef FW_BCAST_DOWNLOAD
    soc_th2_portctrl_sbus_ring_info_get,
    soc_th2_portctrl_sbus_bcast_id_get
#else
    NULL,
    NULL
#endif
};

STATIC int
_soc_th2_portctrl_rescal_calculate(int unit, int *val1, int *val2)
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

STATIC int
_soc_th2_portctrl_pm_init(int unit, int *max_phys,
                         portmod_pm_instances_t **pm_types,
                         int *pms_arr_len)
{
    *max_phys = SOC_TH2_MAX_PHYS;
    if (NUM_PIPE(unit) == 2) {
        *pm_types = th2_2pipe_pm_types;
        *pms_arr_len = sizeof(th2_2pipe_pm_types)/sizeof(portmod_pm_instances_t);
    } else {
        *pm_types = th2_pm_types;
        *pms_arr_len = sizeof(th2_pm_types)/sizeof(portmod_pm_instances_t);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_th2_portctrl_device_addr_port_get(int unit, int pm_type,
                                      uint32_t **addr,
                                      uint32_t **port,
                                      uint32_t **core_num)
{
    *addr     = NULL;
    *port     = NULL;
    *core_num = NULL;
    if (pm_type == portmodDispatchTypePm4x10) {
        *addr = _th2_pm4x10_addr;
        *port = _th2_pm4x10_port;
        *core_num = _th2_pm4x10_core_num;
    } else if (pm_type == portmodDispatchTypePm4x25){
        *addr = _th2_pm4x25_addr;
        *port = _th2_pm4x25_port;
        *core_num = _th2_pm4x25_core_num;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_th2_portctrl_pm_user_acc_flag_set(int unit,
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
_soc_th2_portctrl_pm_port_polarity_get(int unit, int logical_port, int physical_port, int lane,
                            uint32 *tx_pol, uint32* rx_pol)
{
    uint32 rx_polarity, tx_polarity;

    rx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                                        spn_PHY_CHAIN_RX_POLARITY_FLIP_PHYSICAL, 0x0);
    *rx_pol = (rx_polarity & 0x1) << lane;

    tx_polarity = soc_property_phy_get(unit, physical_port, 0, 0, lane,
                                        spn_PHY_CHAIN_TX_POLARITY_FLIP_PHYSICAL, 0x0);
    *tx_pol = (tx_polarity & 0x1) << lane;

}


/*
 * lanemap get looks for physical port based config. 
 */
STATIC 
void _soc_th2_portctrl_pm_port_lanemap_get
            (int unit, int logical_port, int physical_port, int core_num, 
            uint32 *txlane_map, uint32* rxlane_map)
{

    *rxlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                        spn_PHY_CHAIN_RX_LANE_MAP_PHYSICAL, 0x3210);


    *txlane_map = soc_property_phy_get(unit, physical_port, 0, 0, 0,
                                        spn_PHY_CHAIN_TX_LANE_MAP_PHYSICAL, 0x3210);

}

STATIC int
_soc_th2_portctrl_pm4x10_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt;
    uint32 *pAddr, *pPort, *pCoreNum;

    SOC_IF_ERROR_RETURN
        (_soc_th2_portctrl_device_addr_port_get(unit,
                                                portmodDispatchTypePm4x10,
                                                &pAddr, &pPort, &pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                               num_of_instances,
                                               &pm4x10_th2_user_acc[unit]));

    for (pmid = 0; PORTMOD_SUCCESS(rv) && (pmid < num_of_instances); pmid++) {

        rv = portmod_pm_create_info_t_init(unit, &pm_info);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }
        /* PM info */
        first_port = pPort[pmid];

        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port + 1);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port + 2);
        PORTMOD_PBMP_PORT_ADD(pm_info.phys, first_port + 3);

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
                                                &(pm4x10_th2_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        SOC_IF_ERROR_RETURN(
            _soc_th2_portctrl_pm_user_acc_flag_set(unit,
                &(pm4x10_th2_user_acc[unit][pmid])));

        pm4x10_th2_user_acc[unit][pmid].unit = unit;
        pm4x10_th2_user_acc[unit][pmid].blk_id = blk; 
        pm4x10_th2_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x10_th2_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x10 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x10.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x10.access.user_acc =
            &(pm4x10_th2_user_acc[unit][pmid]);
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
            if (pm4x10_th2_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x10_th2_user_acc[unit][pmid].mutex);
                pm4x10_th2_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X10_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X10_SUPPORT */
}

STATIC int
_soc_th2_portctrl_pm4x25_portmod_init(int unit, int num_of_instances) 
{
#ifdef PORTMOD_PM4X25_SUPPORT
    int rv = SOC_E_NONE;
    portmod_pm_create_info_t pm_info;
    int pmid = 0, blk, is_sim, found;
    int first_port, idx, core_num, core_cnt, rescal_0_1 = -1, rescal_2_3 = -1;
    uint32 *pAddr, *pPort,*pCoreNum;
    int logical_port, mode;

    SOC_IF_ERROR_RETURN
        (_soc_th2_portctrl_device_addr_port_get(unit,
                                                portmodDispatchTypePm4x25,
                                                &pAddr, &pPort,&pCoreNum));
    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
    }

    /* RESCAL should be disabled by default */
    if (soc_feature(unit, soc_feature_rescal)) {
        _soc_th2_portctrl_rescal_calculate(unit, &rescal_0_1, &rescal_2_3);
    }

    /* Allocate PMs */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_user_access_alloc(unit,
                                               num_of_instances,
                                               &pm4x25_th2_user_acc[unit]));

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

        found = 0;
        for(idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++){ 
            blk = SOC_PORT_IDX_INFO(unit, first_port, idx).blk; 
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT){
                found = 1;
                break;
            }
        }

        if (!found) {
            rv = SOC_E_INTERNAL;
            break;
        } 

        pm_info.type         = portmodDispatchTypePm4x25;

        /* Init user_acc for phymod access struct */
        rv = portmod_default_user_access_t_init(unit,
                                                &(pm4x25_th2_user_acc[unit][pmid]));
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        PORTMOD_USER_ACCESS_FW_LOAD_REVERSE_SET((&(pm4x25_th2_user_acc[unit][pmid])));
        PORTMOD_USER_ACCESS_REG_VAL_OFFSET_ZERO_CLR((&(pm4x25_th2_user_acc[unit][pmid])));
        pm4x25_th2_user_acc[unit][pmid].unit = unit;
        pm4x25_th2_user_acc[unit][pmid].blk_id = blk; 
        pm4x25_th2_user_acc[unit][pmid].mutex = sal_mutex_create("core mutex");
        if (pm4x25_th2_user_acc[unit][pmid].mutex == NULL) {
            rv = SOC_E_MEMORY;
            break;
        }

        /* Specific info for PM4x25 */
        rv = phymod_access_t_init(&pm_info.pm_specific_info.pm4x25.access);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        pm_info.pm_specific_info.pm4x25.access.user_acc =
            &(pm4x25_th2_user_acc[unit][pmid]);
        pm_info.pm_specific_info.pm4x25.access.addr     = pAddr[pmid];
        pm_info.pm_specific_info.pm4x25.access.bus      = NULL; /* Use default bus */
#if !defined (EXCLUDE_PHYMOD_TSCF16_SIM) && defined (PHYMOD_TSCF16_SUPPORT)
        rv = soc_physim_check_sim(unit, phymodDispatchTypeTscf16, 
                                  &(pm_info.pm_specific_info.pm4x25.access),
                                  0, &is_sim);
#else
        is_sim = 0;
#endif

        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        if (is_sim) {
            /* Firmward loader : Don't allow FW load on sim enviroment */
            pm_info.pm_specific_info.pm4x25.fw_load_method =
                phymodFirmwareLoadMethodNone;
            /* TSCF sim supports CL45 mode to read/write registers. */
            PHYMOD_ACC_F_CLAUSE45_SET(&pm_info.pm_specific_info.pm4x25.access);
        }

        /* Use default external loader if NULL */
        pm_info.pm_specific_info.pm4x25.external_fw_loader = NULL;

        core_cnt = 1; 

        for (core_num = 0; core_num < core_cnt; core_num++) {
            pm_info.pm_specific_info.pm4x25.core_num = pCoreNum? pCoreNum[pmid] : pmid;
            pm_info.pm_specific_info.pm4x25.core_num_int = 0;
            pm_info.pm_specific_info.pm4x25.rescal = -1;
            if (_th2_core_tsc_group[pm_info.pm_specific_info.pm4x25.core_num] == SOC_TH2_TSC_GROUP_1) {
                pm_info.pm_specific_info.pm4x25.rescal = rescal_0_1;
            } else if (_th2_core_tsc_group[pm_info.pm_specific_info.pm4x25.core_num] == SOC_TH2_TSC_GROUP_2) {
                pm_info.pm_specific_info.pm4x25.rescal = rescal_2_3;
            }
        }

        logical_port = SOC_INFO(unit).port_p2l_mapping[first_port];
        SOC_IF_ERROR_RETURN(soc_th2_port_mode_get(unit, logical_port, &mode));
        if (mode == portmodPortModeTri012 || mode == portmodPortModeTri023) {
            pm_info.pm_specific_info.pm4x25.port_mode_aux_info = portmodModeInfoThreePorts;
        }

        /* Add PM to PortMod */
        rv = portmod_port_macro_add(unit, &pm_info);
    }

    if (PORTMOD_FAILURE(rv)) {
        for (pmid = 0; pmid < num_of_instances; pmid++) {
            if (pm4x25_th2_user_acc[unit][pmid].mutex) {
                sal_mutex_destroy(pm4x25_th2_user_acc[unit][pmid].mutex);
                pm4x25_th2_user_acc[unit][pmid].mutex = NULL;
            }
        }
    }

    return rv;
#else /* PORTMOD_PM4X25_SUPPORT */
    return SOC_E_UNAVAIL;
#endif /* PORTMOD_PM4X25_SUPPORT */
}

/*
 * Function:
 *      soc_th2_portctrl_pm_portmod_init
 * Purpose:
 *      Initialize PortMod and PortMacro for Tomahawk2
 *      Add port macros (PM) to the unit's PortMod Manager (PMM).
 * Parameters:
 *      unit             - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_th2_portctrl_pm_portmod_init(int unit)
{
    int rv = SOC_E_NONE, pms_arr_len = 0, count, max_phys= 0;
    portmod_pm_instances_t *pm_types = NULL;
    int flags = 0;

    /* Call PortMod library initialization */
    SOC_IF_ERROR_RETURN(
        _soc_th2_portctrl_pm_init(unit, &max_phys,
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

    SOC_IF_ERROR_RETURN
        (portmod_create(unit, flags, SOC_MAX_NUM_PORTS, max_phys,
                        pms_arr_len, pm_types));

    for (count = 0; SOC_SUCCESS(rv) && (count < pms_arr_len); count++) {
#ifdef PORTMOD_PM4X10_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x10) {
            rv = _soc_th2_portctrl_pm4x10_portmod_init
                (unit, pm_types[count].instances);
        } else 
#endif /* PORTMOD_PM4X10_SUPPORT  */
#ifdef PORTMOD_PM4X25_SUPPORT
        if (pm_types[count].type == portmodDispatchTypePm4x25) {
            rv = _soc_th2_portctrl_pm4x25_portmod_init
                (unit, pm_types[count].instances);
        } else
#endif /* PORTMOD_PM4X25_SUPPORT  */
        {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_th2_portctrl_pm_portmod_deinit
 * Purpose:
 *      Deinitialize PortMod and PortMacro for Tomahawk2.
 *      Free pm user access data.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_NONE
 */
int
soc_th2_portctrl_pm_portmod_deinit(int unit)
{
    if (SOC_E_INIT == soc_esw_portctrl_init_check(unit)) {
        return SOC_E_NONE;
    }

    /* Free PMs structures */
#ifdef PORTMOD_PM4X10_SUPPORT
    if (pm4x10_th2_user_acc[unit] != NULL) {
        sal_free(pm4x10_th2_user_acc[unit]);
        pm4x10_th2_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    if (pm4x25_th2_user_acc[unit] != NULL) {
        sal_free(pm4x25_th2_user_acc[unit]);
        pm4x25_th2_user_acc[unit] = NULL;
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    SOC_IF_ERROR_RETURN(portmod_destroy(unit));

    return SOC_E_NONE;
}

int
soc_th2_portctrl_pm_port_config_get(int unit, soc_port_t port, void *port_config)
{
#ifdef PORTMOD_PM4X10_SUPPORT
    int rv = SOC_E_NONE;
    int pmid = 0, lane, phy, phy_port, logical_port;
    int first_port, core_num, core_count = 0, core_index, is_sim, type, port_mode;
    uint32 txlane_map, rxlane_map, rxlane_map_l, i;
    uint32 *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;
    uint32 tx_polarity, rx_polarity;
    portmod_port_init_config_t *port_config_info;
    phymod_firmware_load_method_t fw_load_method;
    phymod_polarity_t polarity; /**< Lanes Polarity */
    phymod_lane_map_t lane_map;

    port_config_info = (portmod_port_init_config_t *)port_config;

    SOC_IF_ERROR_RETURN(soc_th2_port_mode_get(unit, port, &port_mode));
    if (port_mode == portmodPortModeTri012 || port_mode == portmodPortModeTri023) {
        port_config_info->port_mode_aux_info = portmodModeInfoThreePorts;
    }

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_num = (phy - 1) / SOC_PM4X10_NUM_LANES;
    for (i=0; i<SOC_TH2_PM4X10_COUNT; i++) {
        if (core_num == _th2_pm4x10_core_num[i]) {
            type = portmodDispatchTypePm4x10;
            SOC_IF_ERROR_RETURN
                (_soc_th2_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x10,
                                                 &pAddr, &pPort, &pCoreNum));
            pmid = i;
            core_count = 1;
            break;
        }
    }
    /* When port dones't find PM4x10, find PM4x25 again. */
    if (i == SOC_TH2_PM4X10_COUNT) {
        for (i=0; i<SOC_TH2_PM4X25_COUNT; i++) {
            if (core_num == _th2_pm4x25_core_num[i]) {
                type = portmodDispatchTypePm4x25;
                SOC_IF_ERROR_RETURN
                    (_soc_th2_portctrl_device_addr_port_get(unit,
                                                 portmodDispatchTypePm4x25,
                                                 &pAddr, &pPort, &pCoreNum));
                /* Because one PM4x25 has three cores and one corresponding pmid. */
                pmid = i;
                core_count = 1;
                break;
            }
        }
    }

    if (!pAddr || !pPort) {
        return SOC_E_INTERNAL;
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

        /* Polarity */
        for (lane = 0; lane < SOC_PM4X10_NUM_LANES; lane++) {
            phy_port = first_port + lane + (core_index*4);
            logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        
            _soc_th2_portctrl_pm_port_polarity_get(unit, logical_port, phy_port, lane, &tx_polarity, &rx_polarity);

            polarity.rx_polarity |= rx_polarity;
            polarity.tx_polarity |= tx_polarity;
        }

        /* Lane map */
        rv = phymod_lane_map_t_init(&lane_map);
        if (PORTMOD_FAILURE(rv)) {
            break;
        }

        phy_port = first_port + (core_index*4);
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];

        _soc_th2_portctrl_pm_port_lanemap_get(unit, logical_port, phy_port, core_num, &txlane_map, &rxlane_map);

        rxlane_map_l = rxlane_map ;
        /* For TSC-E/F family, both lane_map_rx and lane_map_tx are logic lane base */
        /* TD2/TD2+ xgxs_rx_lane_map is phy-lane base */
        if (SOC_IS_TOMAHAWK2(unit)) { /* use TD2 legacy notion system */
            rxlane_map_l = 0 ;
            for (i=0; i<SOC_PM4X10_NUM_LANES; i++) {
                rxlane_map_l |= i << SOC_PM4X10_NUM_LANES *((rxlane_map >> (i*SOC_PM4X10_NUM_LANES))& SOC_PM4X10_LANE_MASK) ;
            }
        }

        if (type == portmodDispatchTypePm4x25) {
            lane_map.num_of_lanes = SOC_PM4X25_NUM_LANES;
        } else {
            lane_map.num_of_lanes = SOC_PM4X10_NUM_LANES;
        }

        for(lane = 0 ; lane < SOC_PM4X10_NUM_LANES; lane++) {
            lane_map.lane_map_tx[lane] =
                (txlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                SOC_PM4X10_LANE_MASK;
            lane_map.lane_map_rx[lane] =
                (rxlane_map >> (lane * SOC_PM4X10_NUM_LANES)) &
                SOC_PM4X10_LANE_MASK;
        }

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
soc_th2_portctrl_pm_port_phyaddr_get(int unit, soc_port_t port)
{
    int phy, core_index;
    uint32 i, *pAddr = NULL, *pPort = NULL, *pCoreNum = NULL;

    phy = SOC_INFO(unit).port_l2p_mapping[port];

    /* Find the port belong to which PM. The port has its own core id, using it to compare PM. */
    core_index = (phy - 1) / SOC_PM4X10_NUM_LANES;
#ifdef PORTMOD_PM4X10_SUPPORT
    for (i=0; i<SOC_TH2_PM4X10_COUNT; i++) {
        if (core_index == _th2_pm4x10_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th2_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x10,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X10_SUPPORT */

#ifdef PORTMOD_PM4X25_SUPPORT
    /* When port dones't find PM4x10, find PM4x25 again. */
    for (i=0; i<SOC_TH2_PM4X25_COUNT; i++) {
        if (core_index == _th2_pm4x25_core_num[i]) {
            SOC_IF_ERROR_RETURN
                (_soc_th2_portctrl_device_addr_port_get(unit,
                                             portmodDispatchTypePm4x25,
                                             &pAddr, &pPort, &pCoreNum));
            return pAddr[i];
        }
    }
#endif /* PORTMOD_PM4X25_SUPPORT */

    return -1;
}

int
soc_th2_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    int phy_port, tsc_id;

    if (!soc_feature(unit, soc_feature_untethered_otp)) {
        return SOC_E_NONE;
    }
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    /* Each bit in FORCE_HG bond option represents one TSC/Port Macro
    * 1 - All ports in specified TSC are Higig only.
    * 0 - No restrictions
    */
    tsc_id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;

    if (SHR_BITGET(SOC_BOND_INFO(unit)->force_hg, tsc_id)) {
        ability->encap &= ~SOC_PA_ENCAP_IEEE;
    }
    return SOC_E_NONE;
}

int
soc_th2_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type)
{
    int rv;

    *pm_type = -1;

    if (phy_port >= 1 && phy_port <= 256) {
#ifdef PORTMOD_PM4X25_SUPPORT
        *pm_type = portmodDispatchTypePm4x25;
        rv = SOC_E_NONE;
#else
        rv = SOC_E_UNAVAIL;
#endif
    } else if (phy_port == _TH2_PHY_PORT_MNG0 ||
               phy_port == _TH2_PHY_PORT_MNG1) {
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

#ifdef FW_BCAST_DOWNLOAD
STATIC int
_soc_th2_sbus_ring_find(soc_th2_sbus_ring_list_t *sbus_ring_list, int core_num, int *pm_position)
{
    int i;

    *pm_position = -1;
    for (i=0; i<SOC_TH2_SBUS_RING_MAX_PM_NUM; i++) {
        if (sbus_ring_list->core_id[i] == core_num) {
            *pm_position = i;
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

int soc_th2_portctrl_sbus_ring_info_get(int unit, soc_port_t port, int* bcast_id, int* position)
{
    int phy, core_num, rv, i, j;

    phy = SOC_INFO(unit).port_l2p_mapping[port];
    core_num = (phy - 1) / SOC_PM4X25_NUM_LANES;

    for (i=0; i<SOC_TH2_SBUS_RING_LIST_NUM; i++) {
        rv = _soc_th2_sbus_ring_find(&_th2_sbus_ring_list[i], core_num, &j);
        if (SOC_SUCCESS(rv)) {
            *bcast_id = _th2_sbus_ring_list[i].bcast_id;
            *position = j;

            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

int soc_th2_portctrl_sbus_bcast_id_get(int unit, int sbus_ch, int* bcast_id)
{
    int i, j;

    for (i=0; i<SOC_TH2_PM4X25_COUNT; i++) {
        j = SOC_BLOCK2SCH(unit, pm4x25_th2_user_acc[unit][i].blk_id);
        if (sbus_ch == j) {
            if (_th2_pm4x25_core_num[i]>=SOC_TH2_SBUS_RING_3_PM_MIN
                && _th2_pm4x25_core_num[i]<=SOC_TH2_SBUS_RING_3_PM_MAX) {
                *bcast_id = _th2_sbus_ring_list[0].bcast_id;
                return SOC_E_NONE;
            } else {
                *bcast_id = _th2_sbus_ring_list[1].bcast_id;
                return SOC_E_NONE;
            }
        }
    }

    return SOC_E_NOT_FOUND;
}
#endif

#endif /* BCM_TOMAHAWK2_SUPPORT */
