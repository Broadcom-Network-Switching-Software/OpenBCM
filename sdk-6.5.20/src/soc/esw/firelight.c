/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firelight.c
 * Purpose:
 * Requires:
 */
#include <soc/bradley.h>
#include <soc/hurricane3.h>
#include <soc/firelight.h>
#include <soc/firelight_tdm.h>
#include <soc/hash.h>
#include <soc/bondoptions.h>
#include <soc/scache.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif /* BCM_CMICX_SUPPORT */

#ifdef BCM_FIRELIGHT_SUPPORT

#define ETHER_MODE                      0
#define Q_MODE                          1

#define SOC_FL_PM4X10_QTC_COUNT         3
#define FL_PORTS_PER_TSC                4

#define SOC_MIN_PHY_PORT_NUM         FL_MIN_PHY_PORT_NUM
#define SOC_MAX_PHY_PORTS            FL_NUM_PHY_PORT
#define SOC_MAX_LOGICAL_PORTS        66
#define SOC_MAX_MMU_PORTS            66

#define PORT_IS_FALCON(unit, phy_port) \
        ((phy_port >= 62) && (phy_port <= 77)) ? 1 : 0
#define PORT_IS_QTCE(unit, phy_port) \
        ((phy_port >= 50) && (phy_port <= 61)) ? 1 : 0

static int fl_port_speed[SOC_MAX_NUM_DEVICES][SOC_MAX_PHY_PORTS];

/* Port config related : p2l, max_speed and TDM */
#define FL_TDM_SLOT_MAX            512
/* Initial TDM table */
uint32 fl_tdm_table_default[FL_TDM_SLOT_MAX] = {0};

#define FL_MACSEC_MAX_PORT_NUM       48
static int fl_macsec_port[SOC_MAX_NUM_DEVICES][SOC_MAX_PHY_PORTS] = {{-1}};
static int fl_macsec_cfg_port[SOC_MAX_NUM_DEVICES][SOC_MAX_PHY_PORTS] = {{-1}};
static int fl_macsec_valid[SOC_MAX_NUM_DEVICES][FL_MACSEC_MAX_PORT_NUM];
static int macsec_count[SOC_MAX_NUM_DEVICES] = {0};

int fl_p2l_mapping_template_op[] = {
    0, -1,
    /* TSC4Q 0 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSC4Q 1 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSC4Q 2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSCE 0~2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSF 0~3*/
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1
};
int fl_port_speed_max_template_op[] = {
    0, -1,
    /* TSC4Q 0 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSC4Q 1 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSC4Q 2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSCE 0~2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSF 0~3*/
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
};

/* OPTION_02_5, 48P 2.5G + 12P 25G */
static int p2l_mapping_op02_5[] = {
    0, -1,
    /* TSC4Q 0 */
    2, 3, 4, 5,
    6, 7, 8, 9,
    10, 11, 12, 13,
    14, 15, 16, 17,
    /* TSC4Q 1 */
    18, 19, 20, 21,
    22, 23, 24, 25,
    26, 27, 28, 29,
    30, 31, 32, 33,
    /* TSC4Q 2 */
    34, 35, 36, 37,
    38, 39, 40, 41,
    42, 43, 44, 45,
    46, 47, 48, 49,
    /* TSCE 0~2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSF 0~3*/
    50, 51, 52, 53,
    54, 55, 56, 57,
    58, 59, 60, 61,
    -1, -1, -1, -1
};
static int port_speed_max_op02_5[] = {
    0, -1,
    /* TSC4Q 0 */
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    /* TSC4Q 1 */
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    /* TSC4Q 2 */
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    25, 25, 25, 25,
    /* TSCE 0~2 */
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    -1, -1, -1, -1,
    /* TSF 0~3*/
    250, 250, 250, 250,
    250, 250, 250, 250,
    250, 250, 250, 250,
    -1, -1, -1, -1,
};

/* System Clock Freq (MHz) */
#define _FL_SYSTEM_FREQ_700          (700) /* 700 MHz */
#define _FL_SYSTEM_FREQ_469          (469) /* 468.75 MHz */
#define _FL_SYSTEM_FREQ_344          (344) /* 343.75 MHz */
#define _FL_SYSTEM_FREQ_219          (219) /* 218.75 MHz */

#define _FL_56070_SYSTEM_FREQ        _FL_SYSTEM_FREQ_700

#define _FL_MAX_TSC_COUNT              (7)
#define _FL_MAX_TSCF_COUNT             (4)
#define _FL_FIRST_PORT_QTC0            (2)
#define _FL_MAX_QTC_COUNT              (3)
#define _FL_PORT_COUNT_PER_TSC         (4)
#define _FL_PORT_COUNT_PER_QTC         (16)
#define _FL_TSC_TSCF0_IDX              (3)

/* 7 TSC (TSC0~2, TSCF0~3) */
static const int tsc_phy_port[] = {50, 54, 58, 62, 66, 70, 74};

/* 3 QTC */
static const int qtc_phy_port[] = {2, 18, 34};

/* Option string */
#define OPTION_1 "op1"
#define OPTION_2 "op2"
#define OPTION_3 "op3"
#define OPTION_4 "op4"
#define OPTION_5 "op5"
#define OPTION_6 "op6"

static int matched_option_idx[SOC_MAX_NUM_DEVICES];

typedef struct _fl_tsc_info_s {
    int     port_count;
    uint8   valid;
    int     phy_port_base;
} _fl_tsc_info_t;

typedef struct _fl_option_info_s {
    char        *config_op; /* option string */
    int         freq;
    int   *p2l_mapping;
    int   *speed_max;
    uint32      *tdm_table;
    int         tdm_table_size;
    uint32      disabled_tscq_bmp; /* 3 bits: [tscq2~tscq0] */
    uint32      disabled_tscf_bmp; /* 4 bits: [tscf3~tscf0] */
    int         default_port_ratio[_FL_MAX_TSC_COUNT];
} _fl_option_info_t;

#define SOC_FL_PORT_CONFIG_OPTION_MAX     7       /* 6 option + 1 NULL */
static _fl_option_info_t
_fl_option_port_config[SOC_MAX_NUM_DEVICES][SOC_FL_PORT_CONFIG_OPTION_MAX];
static int _fl_p2l_mapping[SOC_MAX_NUM_DEVICES][SOC_MAX_PHY_PORTS];
static int _fl_speed_max[SOC_MAX_NUM_DEVICES][SOC_MAX_PHY_PORTS];
static uint32 _fl_tdm_table[SOC_MAX_NUM_DEVICES][FL_TDM_SLOT_MAX];

static _fl_tsc_info_t _fl_tsc[SOC_MAX_NUM_DEVICES][_FL_MAX_TSC_COUNT];

STATIC _fl_option_info_t
_fl_a0_flex_option_port_template[SOC_FL_PORT_CONFIG_OPTION_MAX] = {
/* OPTION_1 */
{OPTION_1, _FL_SYSTEM_FREQ_700,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
/* OPTION_2 */
{OPTION_2, _FL_SYSTEM_FREQ_700,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
/* OPTION_3 */
{OPTION_3, _FL_SYSTEM_FREQ_700,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
/* OPTION_4 */
{OPTION_4, _FL_SYSTEM_FREQ_700,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
/* OPTION_5 */
{OPTION_5, _FL_SYSTEM_FREQ_700,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
/* OPTION_6 */
{OPTION_6, _FL_SYSTEM_FREQ_219,
    NULL, NULL,
    NULL, FL_TDM_SLOT_MAX,
    0x0, 0x0,
    {SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD, SOC_FL_PORT_RATIO_QUAD,
     SOC_FL_PORT_RATIO_QUAD}},
{NULL, 0,
    0, 0,
    0, 0,
    0, 0,
    {-1, -1, -1, -1, -1, -1, -1}},
};

typedef struct _fl_sku_option_list_s {
    uint16      dev_id;
    char        *option_string; /* option */
    int         default_option; /* 1 : the default option of the SKU */
} _fl_sku_option_list_t;

_fl_sku_option_list_t *_fl_sku_option_support_list;

STATIC _fl_sku_option_list_t _fl_a0_flex_sku_option_support_list[] = {
    /* BCM56070 */
    {BCM56070_DEVICE_ID,  OPTION_1, 0},
    {BCM56070_DEVICE_ID,  OPTION_2, 1},
    {BCM56070_DEVICE_ID,  OPTION_3, 0},
    {BCM56070_DEVICE_ID,  OPTION_4, 0},
    {BCM56070_DEVICE_ID,  OPTION_5, 0},
    {BCM56070_DEVICE_ID,  OPTION_6, 0},
    /* BCM56071 */
    {BCM56071_DEVICE_ID,  OPTION_5, 1},
    {BCM56071_DEVICE_ID,  OPTION_6, 0},
    /* BCM56072 */
    {BCM56072_DEVICE_ID,  OPTION_1, 0},
    {BCM56072_DEVICE_ID,  OPTION_2, 1},
    {BCM56072_DEVICE_ID,  OPTION_3, 0},
    {BCM56072_DEVICE_ID,  OPTION_4, 0},
    {BCM56072_DEVICE_ID,  OPTION_5, 0},
    {BCM56072_DEVICE_ID,  OPTION_6, 0},

    {0, "not existed", 0},
};

#define FLEX_HG42_GEN2          0 /* F.HG[42] */
#define FLEX_HG42_UL_GEN3       1 /* F.HG[42]-GEN3 */
#define FLEX_2HG53_GEN3         2 /* F.2HG53-GEN3 */
#define FLEX_CAUI_GEN3          3 /* F.CAUI */
#define FLEX_XLAUI_QXG          4 /* F.XLAUI or F.QXG */
#define FLEX_XLAUI_QSG          5 /* F.XLAUI or F.QSG */
#define FLEX_MGL_GEN2           6 /* F.MGL-GEN2 */
#define FLEX_MGL_GEN3           7 /* F.MGL-GEN3 */
#define FLEX_QXGMII             8 /* F.QXG */
#define FLEX_QSGMII             9 /* F.QSG */
#define FLEX_OFF             (-1) /* core is off */

typedef struct _fl_option_tsc_flex_mode_s {
    char        *config_op; /* option string */
    int         mode[_FL_MAX_TSC_COUNT];
} _fl_option_tsc_flex_mode_t;

/* Supported flex port speed of each TSC */
STATIC _fl_option_tsc_flex_mode_t _fl_option_tsc_flex_modes[] = {
    /* OPTION_1 */
    {OPTION_1, {FLEX_HG42_GEN2, FLEX_HG42_GEN2, FLEX_HG42_GEN2,
                FLEX_2HG53_GEN3, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_2HG53_GEN3}},
    /* OPTION_2 */
    {OPTION_2, {FLEX_XLAUI_QXG, FLEX_XLAUI_QXG, FLEX_XLAUI_QXG,
                FLEX_OFF, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_2HG53_GEN3}},
    /* OPTION_3 */
    {OPTION_3, {FLEX_OFF, FLEX_OFF, FLEX_OFF,
                FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3}},
    /* OPTION_4 */
    {OPTION_4, {FLEX_HG42_GEN2, FLEX_HG42_GEN2, FLEX_HG42_GEN2,
                FLEX_HG42_UL_GEN3, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_HG42_UL_GEN3}},
    /* OPTION_5 */
    {OPTION_5, {FLEX_XLAUI_QXG, FLEX_XLAUI_QXG, FLEX_XLAUI_QSG,
                FLEX_OFF, FLEX_CAUI_GEN3, FLEX_CAUI_GEN3, FLEX_OFF}},
    /* OPTION_6 */
    {OPTION_6, {FLEX_MGL_GEN2, FLEX_MGL_GEN2, FLEX_MGL_GEN2,
                FLEX_MGL_GEN3, FLEX_HG42_UL_GEN3, FLEX_HG42_UL_GEN3, FLEX_MGL_GEN3}},
    /* invalid entry */
    {NULL, {-1, -1, -1, -1, -1, -1, -1}},
};

/* MMU related */
#define FL_MMU_CBP_FULL_SIZE 0x3fff

#define CMIC_PARITY_MMU_TO_CMIC_MEMFAIL_INTR_BIT    0x0001
#define CMIC_PARITY_EP_TO_CMIC_PERR_INTR_BIT        0x0002
#define CMIC_PARITY_IP0_TO_CMIC_PERR_INTR_BIT       0x0004
#define CMIC_PARITY_IP1_TO_CMIC_PERR_INTR_BIT       0x0008
#define CMIC_PARITY_IP2_TO_CMIC_PERR_INTR_BIT       0x0010

#define _SOC_FL_MMU_IPMC_GROUP_MAX     66
static soc_mem_t ipmc_mems[_SOC_FL_MMU_IPMC_GROUP_MAX] = {
        INVALIDm,  INVALIDm,
        MMU_IPMC_GROUP_TBL2m,  MMU_IPMC_GROUP_TBL3m,
        MMU_IPMC_GROUP_TBL4m,  MMU_IPMC_GROUP_TBL5m,
        MMU_IPMC_GROUP_TBL6m,  MMU_IPMC_GROUP_TBL7m,
        MMU_IPMC_GROUP_TBL8m,  MMU_IPMC_GROUP_TBL9m,
        MMU_IPMC_GROUP_TBL10m,  MMU_IPMC_GROUP_TBL11m,
        MMU_IPMC_GROUP_TBL12m,  MMU_IPMC_GROUP_TBL13m,
        MMU_IPMC_GROUP_TBL14m,  MMU_IPMC_GROUP_TBL15m,
        MMU_IPMC_GROUP_TBL16m,  MMU_IPMC_GROUP_TBL17m,
        MMU_IPMC_GROUP_TBL18m,  MMU_IPMC_GROUP_TBL19m,
        MMU_IPMC_GROUP_TBL20m,  MMU_IPMC_GROUP_TBL21m,
        MMU_IPMC_GROUP_TBL22m,  MMU_IPMC_GROUP_TBL23m,
        MMU_IPMC_GROUP_TBL24m,  MMU_IPMC_GROUP_TBL25m,
        MMU_IPMC_GROUP_TBL26m,  MMU_IPMC_GROUP_TBL27m,
        MMU_IPMC_GROUP_TBL28m,  MMU_IPMC_GROUP_TBL29m,
        MMU_IPMC_GROUP_TBL30m,  MMU_IPMC_GROUP_TBL31m,
        MMU_IPMC_GROUP_TBL32m,  MMU_IPMC_GROUP_TBL33m,
        MMU_IPMC_GROUP_TBL34m,  MMU_IPMC_GROUP_TBL35m,
        MMU_IPMC_GROUP_TBL36m,  MMU_IPMC_GROUP_TBL37m,
        MMU_IPMC_GROUP_TBL38m,  MMU_IPMC_GROUP_TBL39m,
        MMU_IPMC_GROUP_TBL40m,  MMU_IPMC_GROUP_TBL41m,
        MMU_IPMC_GROUP_TBL42m,  MMU_IPMC_GROUP_TBL43m,
        MMU_IPMC_GROUP_TBL44m,  MMU_IPMC_GROUP_TBL45m,
        MMU_IPMC_GROUP_TBL46m,  MMU_IPMC_GROUP_TBL47m,
        MMU_IPMC_GROUP_TBL48m,  MMU_IPMC_GROUP_TBL49m,
        MMU_IPMC_GROUP_TBL50m,  MMU_IPMC_GROUP_TBL51m,
        MMU_IPMC_GROUP_TBL52m,  MMU_IPMC_GROUP_TBL53m,
        MMU_IPMC_GROUP_TBL54m,  MMU_IPMC_GROUP_TBL55m,
        MMU_IPMC_GROUP_TBL56m,  MMU_IPMC_GROUP_TBL57m,
        MMU_IPMC_GROUP_TBL58m,  MMU_IPMC_GROUP_TBL59m,
        MMU_IPMC_GROUP_TBL60m,  MMU_IPMC_GROUP_TBL61m,
        MMU_IPMC_GROUP_TBL62m,  MMU_IPMC_GROUP_TBL63m,
        MMU_IPMC_GROUP_TBL64m,  MMU_IPMC_GROUP_TBL65m
};

static int
soc_firelight_pipe_mem_clear(int unit)
{
    uint32              rval;
    int                 pipe_init_usec, index;
    soc_timeout_t       to;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                     BOOT_F_PLISIM },
        { EFP_TCAMm,                        BOOT_F_PLISIM },
        { FP_GLOBAL_MASK_TCAMm,             BOOT_F_PLISIM },
        { FP_TCAMm,                         BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                   BOOT_F_PLISIM },
        { L2_USER_ENTRY_ONLYm,              BOOT_F_PLISIM },
        { L3_DEFIPm,                        BOOT_F_PLISIM },
        { L3_DEFIP_ONLYm,                   BOOT_F_PLISIM },
        { VFP_TCAMm,                        BOOT_F_PLISIM },
        { VLAN_SUBNETm,                     0 },/* VLAN API needs all 0 mask */
        { VLAN_SUBNET_ONLYm,                0 }
    };

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table, L2_ENTRYm */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x8000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /*
     * Set count to # entries in largest EPIPE table,
     * SR_FLOW_EGR_COUNTER_CONTROLm
     */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0x2000);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Processing tables that are not handled by hardware reset */
    if (!SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM) {
        /* TCAM tables are not handled by hardware reset control */
        for (index = 0; index < sizeof(cam_list) / sizeof(cam_list[0]);
             index++) {
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }
            if (((cam_list[index].mem == EFP_TCAMm) ||
                (cam_list[index].mem == VFP_TCAMm)) &&
                (!soc_feature(unit, soc_feature_field_multi_stage))) {
                continue;
            }

            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }

        /* MMU_IPMC_VLAN_TBL */
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, TRUE));

        /* MMU_IPMC_GROUP_TBLn : n is 2 ~ 65 in FL */
        for (index = 0; index < _SOC_FL_MMU_IPMC_GROUP_MAX; index++) {
            if (ipmc_mems[index] != INVALIDm) {
                SOC_IF_ERROR_RETURN(soc_mem_clear(unit,
                                    ipmc_mems[index], COPYNO_ALL, TRUE));
            }
        }

    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_firelight_gmac_init()
 * Purpose:
 *      To init GMAC (should be moved to mac)
 */
STATIC int
soc_firelight_gmac_init(int unit)
{
    uint32 rval;
    uint32 prev_reg_addr;
    int port;

    rval = 0;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 1);
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, GPORT_ENf, 1);
    prev_reg_addr = 0xffffffff;
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XL_PORT(unit, port) || IS_CL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }
    prev_reg_addr = 0xffffffff;
    soc_reg_field_set(unit, GPORT_CONFIGr, &rval, CLR_CNTf, 0);
    PBMP_E_ITER(unit, port) {
        uint32  reg_addr;
        if (IS_XL_PORT(unit, port) || IS_CL_PORT(unit, port)) {
            continue;
        }
        reg_addr = soc_reg_addr(unit, GPORT_CONFIGr, port, 0);
        if (reg_addr != prev_reg_addr) {
            SOC_IF_ERROR_RETURN(WRITE_GPORT_CONFIGr(unit, port, rval));
            prev_reg_addr = reg_addr;
        }
    }

    return SOC_E_NONE;
}

/*
 * Func : soc_firelight_pgw_encap_field_modify
 *  - To write to PGW interface on GX/XL/CL port on indicated field
 */
int
soc_firelight_pgw_encap_field_modify(int unit,
                                soc_port_t lport,
                                soc_field_t field,
                                uint32 val)
{
    int     pport = SOC_INFO(unit).port_l2p_mapping[lport];

    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_XLPORT) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                    PGW_XL_CONFIGr, lport, field, val));
    } else if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_CLPORT) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                    PGW_CL_CONFIGr, lport, field, val));
    } else {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                    PGW_GE_CONFIGr, lport, field, val));
    }

    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_firelight_higig_mode_init()
 * Purpose:
 *      To init the default Higig mode for HG port
 */
STATIC int
soc_firelight_higig_mode_init(int unit)
{
    uint32 rval;
    int port;

    PBMP_PORT_ITER(unit, port) {
        if (!IS_HG_PORT(unit, port)) {
            continue;
        }
        if (!IS_XL_PORT(unit, port) && !IS_CL_PORT(unit, port)) {
            continue;
        }

        /* Section below is used to config XLPORT and PGW related HiGig encap
         * setting for normal init process.
         */
        if (IS_XL_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
        }
        if (IS_CL_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_CLPORT_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_CONFIGr(unit, port, rval));
        }

        /* only HG ports allows PGW encap setting below */
        SOC_IF_ERROR_RETURN(soc_firelight_pgw_encap_field_modify(unit,
                port, HIGIG_MODEf, 1));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_firelight_mib_reset()
 * Purpose:
 *      Reset MIB counter
 */
STATIC int
soc_firelight_mib_reset(int unit)
{
    uint32 rval, fval;
    int blk, bindex;
    soc_info_t *si = &SOC_INFO(unit);
    int blk_port, phy_port;

    /*
     * Reset XLPORT and CLPORT MIB counter
     * (registers implemented in memory).
     * The clear function is implemented with read-modify-write,
     * parity needs to be disabled
     */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        phy_port = si->port_l2p_mapping[blk_port];
        fval = 0;
        for (bindex = 0; bindex < 4; bindex++) {
            if (si->port_p2l_mapping[phy_port + bindex] != -1) {
                fval |= 1 << bindex;
            }
        }
        rval = 0;
        soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, fval);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, blk_port, 0));
    }
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        blk_port = SOC_BLOCK_PORT(unit, blk);
        if (blk_port < 0) {
            continue;
        }
        phy_port = si->port_l2p_mapping[blk_port];
        fval = 0;
        for (bindex = 0; bindex < 4; bindex++) {
            if (si->port_p2l_mapping[phy_port + bindex] != -1) {
                fval |= 1 << bindex;
            }
        }
        rval = 0;
        soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, fval);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, blk_port, rval));
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, blk_port, 0));
    }

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
soc_fl_port_init_speed_scache_allocate(int unit)
{
    int     rv;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32  alloc_get = 0;
    uint32  alloc_size = 0;

    alloc_size = sizeof(int) * SOC_MAX_PHY_PORTS;
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_scache_alloc(unit, scache_handle, alloc_size);
    if (rv == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    SOC_IF_ERROR_RETURN(
        soc_scache_ptr_get(unit, scache_handle, &scache_ptr, &alloc_get));

    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }

    return SOC_E_NONE;
}

int
soc_fl_port_init_speed_scache_sync(int unit)
{
    int                 rv, phy_port;
    uint8               *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get = 0, alloc_size = 0, scache_offset = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    alloc_size = sizeof(int) * SOC_MAX_PHY_PORTS;
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        sal_memcpy(&scache_ptr[scache_offset],
                   &fl_port_speed[unit][phy_port],
                   sizeof(int));
        scache_offset += sizeof(int);
    }
    return SOC_E_NONE;
}

STATIC int
soc_fl_port_init_speed_scache_recovery(int unit)
{
    int                 rv, phy_port;
    uint8               *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32              alloc_get = 0, alloc_size = 0, scache_offset = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    rv = soc_scache_ptr_get(unit, scache_handle,
                            &scache_ptr, &alloc_get);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    alloc_size = sizeof(int) * SOC_MAX_PHY_PORTS;
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        return SOC_E_INTERNAL;
    }
    if (NULL == scache_ptr) {
        return SOC_E_MEMORY;
    }
    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        sal_memcpy(&fl_port_speed[unit][phy_port],
                   &scache_ptr[scache_offset],
                   sizeof(int));
        scache_offset += sizeof(int);
    }
    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

STATIC int
soc_firelight_ledup_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int freq, qtc_mode;
    uint32 rval, clk_half_period, refresh_period;

    /* freq in KHz */
    freq = (si->frequency) * 1000;

    /* For LED refresh period = 33 ms (about 30Hz)  */

    /* refresh period
       = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (freq * 33);

    rval = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_REFRESH_CTRLr(unit, rval));

    /* For LED clock period */
    /* LEDCLK_HALF_PERIOD
       = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)

       Where M0SS freqency is 858MHz and
       Typical LED clock period is 200ns(5MHz) = 2*10^-7
    */
    freq = 667 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;

    rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval,
                      LEDCLK_HALF_PERIODf, clk_half_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval));


    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval));

    soc_fl_port_qsgmii_mode_get_by_qtc_num(unit, 0, &qtc_mode);

    if (qtc_mode) {
        /* If PM4x10Q0 in Q mode */
        soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                          LAST_PORTf, 15);
    } else {
        /* If PM4x10Q0 in ethernet mode */
        soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval,
                          LAST_PORTf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));

    /* To initialize M0s for LED or Firmware Link Scan*/
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));
    }

    return SOC_E_NONE;
}

int
soc_firelight_init_port_mapping(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int logical_port, phy_port, mmu_port;
    int num_port, num_phy_port;
    uint32 fval[3];
    egr_tdm_port_map_entry_t egr_tdm_port_map_entry;
    int idx;
    int shift;
    int port, speed;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        logical_port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            logical_port == -1 ? 0x7f : logical_port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }
    num_port = soc_mem_index_count(unit, PORT_TABm);

    /* Egress logical to physical port mapping */
    for (logical_port = 0; logical_port < num_port; logical_port++) {
        phy_port = si->port_l2p_mapping[logical_port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x7f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit,
                                                                logical_port,
                                                                rval));
    }

    /* EGR_TDM_PORT_MAPm */
    sal_memset(&egr_tdm_port_map_entry, 0, sizeof(egr_tdm_port_map_entry));
    sal_memset(fval, 0, sizeof(fval));
    mem = EGR_TDM_PORT_MAPm;
    for (logical_port = 0; logical_port < num_port; logical_port++) {
        phy_port = si->port_l2p_mapping[logical_port];
        if (logical_port == 1) {
            /* skip port 1 */
            continue;
        }
        idx = phy_port / 32;
        shift = phy_port - idx * 32;
        fval[idx] |= (1 << shift);
    }
    soc_mem_field_set(unit, EGR_TDM_PORT_MAPm,
        (void *)&egr_tdm_port_map_entry, BITMAPf, fval);
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, &egr_tdm_port_map_entry));

    /* MMU to physical port mapping and MMU to logical port mapping */
    for (mmu_port = 0; mmu_port < SOC_MAX_MMU_PORTS; mmu_port++) {
        /* MMU to Physical  port */
        phy_port = si->port_m2p_mapping[mmu_port];
        /* Physical to Logical port */
        logical_port = si->port_p2l_mapping[phy_port];

        if (phy_port == 1) {
            /* skip port 1 */
            continue;
        }
        if (phy_port == -1) {
            /* skip not mapped mmu port */
            continue;
        }
        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                          PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_PORT_TO_PHY_PORT_MAPPINGr(unit, logical_port, rval));

        if (logical_port == -1) {
            logical_port = 1;
        }
        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, &rval,
                          LOGIC_PORTf, logical_port);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_PORT_TO_LOGIC_PORT_MAPPINGr(unit, logical_port, rval));
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(
            soc_fl_port_init_speed_scache_recovery(unit));
    } else {
        SOC_IF_ERROR_RETURN(
            soc_fl_port_init_speed_scache_allocate(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Initialize the si->port_init_speed */
    PBMP_ALL_ITER(unit, port) {
        /*
         * Let the port_init_speed as the max supported speed of xe
         * while the port_speed_max would be the max supported speed of hg
         */
        switch (si->port_speed_max[port]) {
            case 11000:
                speed = 10000;
                break;
            case 21000:
                speed = 20000;
                break;
            case 27000:
                speed = 25000;
                break;
            case 42000:
                speed = 40000;
                break;
            case 53000:
                speed = 50000;
                break;
            default:
                speed = si->port_speed_max[port];
                break;
        }
        si->port_init_speed[port] = speed;
        if (!SOC_WARM_BOOT(unit)) {
            fl_port_speed[unit][si->port_l2p_mapping[port]] = speed;
        }
    }
    return SOC_E_NONE;
}

/* Provide the max speed mapping from xe to hg */
int
soc_firelight_port_hg_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_info_t *si;

    si = &SOC_INFO(unit);

    switch (si->port_init_speed[port]) {
        case 10000:
            *speed = 11000;
            break;
        case 20000:
            *speed = 21000;
            break;
        case 25000:
            *speed = 27000;
            break;
        case 40000:
            *speed = 42000;
            break;
        case 50000:
            *speed = 53000;
            break;
        default:
            *speed = si->port_init_speed[port];
            break;
    }

    if (*speed > si->port_speed_max[port]) {
        *speed = si->port_speed_max[port];
    }
    return SOC_E_NONE;
}

/*
 * Check if the configured port speed matched.
 *
 * return value
 *    1 : check passed
 *    0 : check failed
 */
static int
flex_lane_speed_check(int speed, int flex_mode, int lane_count)
{
    /* Check invalid lane count */
    if ((lane_count < 1) || (lane_count > 4)) {
        return 0;
    }
    if (lane_count == 3) {
        return 0;
    }

    if (flex_mode == FLEX_QXGMII) {
        if (lane_count == 1) {
            /* 1G, 2.5G */
            if ((speed == 10) || (speed == 25)) {
                return 1;
            }
        }
    } else if (flex_mode == FLEX_QSGMII) {
        if (lane_count == 1) {
            /* 1G */
            if (speed == 10) {
                return 1;
            }
        }
    } else if ((flex_mode == FLEX_HG42_GEN2) ||
               (flex_mode == FLEX_HG42_UL_GEN3)) {
        if (lane_count == 1) {
            /* 1G, 2.5G, 5G, 10G, 11G */
            if ((speed == 10) || (speed == 25) || (speed == 50) || \
                (speed == 100) || (speed == 110)) {
                return 1;
            }
        }
        if (lane_count == 2) {
            /* 20G, 21G */
            if ((speed == 200) || (speed == 210)) {
                return 1;
            }
        }
        if (lane_count == 4) {
            /* 40G, 42G */
            if ((speed == 400) || (speed == 420)) {
                return 1;
            }
        }
    } else if ((flex_mode == FLEX_CAUI_GEN3) ||
               (flex_mode == FLEX_2HG53_GEN3)) {
        if (lane_count == 1) {
            /* 1G, 2.5G, 5G, 10G, 11G, 25G, 27G */
            if ((speed == 10) || (speed == 25) || (speed == 50) || \
                (speed == 100) || (speed == 110) || \
                (speed == 250) || (speed == 270)) {
                return 1;
            }
        }
        if (lane_count == 2) {
            /* 20G, 21G, 50G, 53G */
            if ((speed == 200) || (speed == 210) ||
                (speed == 500) || (speed == 530)) {
                return 1;
            }
        }
        if (lane_count == 4) {
            /* 40G, 42G, 100G & GEN3 */
            if ((speed == 400) || (speed == 420) ||
                ((flex_mode == FLEX_CAUI_GEN3) && (speed == 1000))) {
                return 1;
            }
        }
    } else if ((flex_mode == FLEX_MGL_GEN2) ||
               (flex_mode == FLEX_MGL_GEN3)) {
        if (lane_count == 1) {
            /* 1G, 2.5G */
            if ((speed == 10) || (speed == 25)) {
                return 1;
            }
        }
        if ((lane_count == 2) || (lane_count == 4)) {
            return 0;
        }
    } else {
        return 0;
    }
    return 0;
}

static int
soc_fl_flex_qxg_qsg_checker(int unit,
                        int cfg_match_id,
                        int tsc_idx,
                        int *port_ratio,
                        int qsg)
{
    _fl_option_info_t *matched_port_config = NULL;
    int *p2l_mapping, *speed_max;
    int index;
    int flex_mode = FLEX_QXGMII;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    p2l_mapping = matched_port_config->p2l_mapping;
    speed_max = matched_port_config->speed_max;

    if (qsg) {
        flex_mode = FLEX_QSGMII;
    }

    if (SOC_BOND_INFO(unit) != NULL) {
        if (flex_mode == FLEX_QXGMII) {
            if (SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, \
                            _FL_MAX_TSC_COUNT + _FL_MAX_QTC_COUNT + tsc_idx)) {
                /* QXGMII is disabled on the core */
                return SOC_E_FAIL;
            }
        } else {
            if (SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, \
                           _FL_MAX_TSC_COUNT + tsc_idx)) {
                /* QSGMII is disabled on the core */
                return SOC_E_FAIL;
            }
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) ||
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) ||
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) ||
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {
        /* TSCE port should be disabled on the core */
        return SOC_E_FAIL;
    }

    if ((p2l_mapping[qtc_phy_port[tsc_idx]] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+3] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+4] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+5] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+6] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+7] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+8] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+9] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+10] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+11] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+12] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+13] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+14] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+15] == -1)) {
        /* No lanes enabled on the core */
        *port_ratio = SOC_FL_PORT_RATIO_QUAD;
        return SOC_E_NONE;
    }

    if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+4] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+5] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+6] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+7] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+8] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+9] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+10] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+11] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+12] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+13] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+14] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+15] == -1)) {

        /* 1st lane enabled on the core */
        for (index = 0; index < 4; index++) {
            if (flex_lane_speed_check
                (speed_max[qtc_phy_port[tsc_idx] + index], flex_mode, 1) == 0){
                return SOC_E_FAIL;
            }
        }
    }

    if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+4] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+5] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+6] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+7] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+8] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+9] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+10] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+11] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+12] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+13] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+14] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+15] == -1)) {

        /* 1st and 2nd lanes enabled on the core */
        for (index = 0; index < 8; index++) {
            if (flex_lane_speed_check
                (speed_max[qtc_phy_port[tsc_idx] + index], flex_mode, 1) == 0){
                return SOC_E_FAIL;
            }
        }
    }

    if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+4] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+5] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+6] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+7] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+8] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+9] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+10] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+11] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+12] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+13] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+14] == -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+15] == -1)) {

        /* 1st, 2nd and 3rd lanes enabled on the core */
        for (index = 0; index < 12; index++) {
            if (flex_lane_speed_check
                (speed_max[qtc_phy_port[tsc_idx] + index], flex_mode, 1) == 0){
                return SOC_E_FAIL;
            }
        }
    }

    if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+4] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+5] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+6] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+7] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+8] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+9] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+10] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+11] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+12] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+13] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+14] != -1) &&
        (p2l_mapping[qtc_phy_port[tsc_idx]+15] != -1)) {

        /* all lanes enabled on the core */
        for (index = 0; index < 16; index++) {
            if (flex_lane_speed_check
                (speed_max[qtc_phy_port[tsc_idx] + index], flex_mode, 1) == 0){
                return SOC_E_FAIL;
            }
        }
    }

    *port_ratio = SOC_FL_PORT_RATIO_QUAD;
    return SOC_E_NONE;
}

static int
soc_fl_flex_hg42_checker(int unit,
                       int cfg_match_id,
                       int tsc_idx,
                       int *port_ratio,
                       int gen3)
{
    _fl_option_info_t *matched_port_config = NULL;
    int *p2l_mapping, *speed_max;
    int rv = SOC_E_NONE;
    int ratio;
    int flex_mode = FLEX_HG42_GEN2;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    p2l_mapping = matched_port_config->p2l_mapping;
    speed_max = matched_port_config->speed_max;

    if (gen3) {
        flex_mode = FLEX_HG42_UL_GEN3;
    }

    if (tsc_idx < _FL_MAX_QTC_COUNT) {
        if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+4] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+5] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+6] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+7] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+8] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+9] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+10] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+11] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+12] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+13] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+14] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+15] != -1)) {
            /* QTC port should be disabled on the core */
            return SOC_E_FAIL;
        }
    }

    ratio = -1;
    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {

        /* If 5G is required on a port macro,
           all ports on that port macro must be configured as 5GE */
        if (gen3 && ((speed_max[tsc_phy_port[tsc_idx]] == 50) ||
                     (speed_max[tsc_phy_port[tsc_idx] + 1] == 50) ||
                     (speed_max[tsc_phy_port[tsc_idx] + 2] == 50) ||
                     (speed_max[tsc_phy_port[tsc_idx] + 3] == 50))) {

            if ((speed_max[tsc_phy_port[tsc_idx]] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 1] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 2] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 3] != 50)) {
                return SOC_E_FAIL;
            }
        }

        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 1], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 3], flex_mode, 1)) {
            ratio = SOC_FL_PORT_RATIO_QUAD;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 2) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 3], flex_mode, 1)) {
            ratio = SOC_FL_PORT_RATIO_TRI_023;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 1], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 2)) {
            ratio = SOC_FL_PORT_RATIO_TRI_012;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 2) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 2)) {
            ratio = SOC_FL_PORT_RATIO_DUAL_2_2;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 4)) {
            ratio = SOC_FL_PORT_RATIO_SINGLE;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        /* No lanes enabled on the core, assign default value */
        ratio = SOC_FL_PORT_RATIO_QUAD;
    }

    if ((ratio >= SOC_FL_PORT_RATIO_SINGLE) &&
        (ratio < SOC_FL_PORT_RATIO_COUNT)) {
        *port_ratio = ratio;
        rv = SOC_E_NONE;
    } else {
        rv = SOC_E_FAIL;
    }
    return rv;
}

static int
soc_fl_flex_d50_caui_checker(int unit,
                       int cfg_match_id,
                       int tsc_idx,
                       int *port_ratio,
                       int caui)
{
    _fl_option_info_t *matched_port_config = NULL;
    int *p2l_mapping, *speed_max;
    int rv = SOC_E_NONE;
    int ratio;
    int flex_mode = FLEX_2HG53_GEN3;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    p2l_mapping = matched_port_config->p2l_mapping;
    speed_max = matched_port_config->speed_max;

    if (caui) {
        flex_mode = FLEX_CAUI_GEN3;
    }

    ratio = -1;
    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {

        /* If 5G is required on a port macro,
           all ports on that port macro must be configured as 5GE */
        if ((speed_max[tsc_phy_port[tsc_idx]] == 50) ||
            (speed_max[tsc_phy_port[tsc_idx] + 1] == 50) ||
            (speed_max[tsc_phy_port[tsc_idx] + 2] == 50) ||
            (speed_max[tsc_phy_port[tsc_idx] + 3] == 50)) {

            if ((speed_max[tsc_phy_port[tsc_idx]] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 1] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 2] != 50) ||
                (speed_max[tsc_phy_port[tsc_idx] + 3] != 50)) {
                return SOC_E_FAIL;
            }
        }

        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 1], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 3], flex_mode, 1)) {
            ratio = SOC_FL_PORT_RATIO_QUAD;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 2) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 3], flex_mode, 1)) {
            ratio = SOC_FL_PORT_RATIO_TRI_023;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 1], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 2)) {
            ratio = SOC_FL_PORT_RATIO_TRI_012;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 2) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 2)) {
            ratio = SOC_FL_PORT_RATIO_DUAL_2_2;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 4)) {
            ratio = SOC_FL_PORT_RATIO_SINGLE;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        /* No lanes enabled on the core, assign default value */
        ratio = SOC_FL_PORT_RATIO_QUAD;
    }

    if ((ratio >= SOC_FL_PORT_RATIO_SINGLE) &&
        (ratio < SOC_FL_PORT_RATIO_COUNT)) {
        *port_ratio = ratio;
        rv = SOC_E_NONE;
    } else {
        rv = SOC_E_FAIL;
    }
    return rv;
}

static int
soc_fl_flex_mgl_checker(int unit,
                          int cfg_match_id,
                          int tsc_idx,
                          int *port_ratio,
                          int gen3)
{
    _fl_option_info_t *matched_port_config = NULL;
    int *p2l_mapping, *speed_max;
    int rv = SOC_E_NONE;
    int ratio;
    int flex_mode = FLEX_MGL_GEN2;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    p2l_mapping = matched_port_config->p2l_mapping;
    speed_max = matched_port_config->speed_max;

    if (tsc_idx < _FL_MAX_QTC_COUNT) {
        if ((p2l_mapping[qtc_phy_port[tsc_idx]] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+1] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+2] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+3] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+4] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+5] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+6] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+7] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+8] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+9] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+10] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+11] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+12] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+13] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+14] != -1) ||
            (p2l_mapping[qtc_phy_port[tsc_idx]+15] != -1)) {
            /* QTC port should be disabled on the core */
            return SOC_E_FAIL;
        }
    }

    if (gen3) {
        flex_mode = FLEX_MGL_GEN3;
    }

    ratio = -1;
    if ((p2l_mapping[tsc_phy_port[tsc_idx]] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] != -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] != -1)) {
        if (flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx]], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 1], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 2], flex_mode, 1) &&
            flex_lane_speed_check
                (speed_max[tsc_phy_port[tsc_idx] + 3], flex_mode, 1)) {
            ratio = SOC_FL_PORT_RATIO_QUAD;
        }
    }

    if ((p2l_mapping[tsc_phy_port[tsc_idx]] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        /* No lanes enabled on the core, assign default value */
        ratio = SOC_FL_PORT_RATIO_QUAD;
    }

    if ((ratio >= SOC_FL_PORT_RATIO_SINGLE) &&
        (ratio < SOC_FL_PORT_RATIO_COUNT)) {
        *port_ratio = ratio;
        rv = SOC_E_NONE;
    } else {
        rv = SOC_E_FAIL;
    }
    return rv;
}

static int
soc_fl_flex_off_checker(int unit,
                            int cfg_match_id,
                            int tsc_idx,
                            int *port_ratio)
{
    _fl_option_info_t *matched_port_config = NULL;
    int *p2l_mapping;
    int rv = SOC_E_NONE;
    int ratio;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    p2l_mapping = matched_port_config->p2l_mapping;

    ratio = -1;
    if ((p2l_mapping[tsc_phy_port[tsc_idx]] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+1] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+2] == -1) &&
        (p2l_mapping[tsc_phy_port[tsc_idx]+3] == -1)) {
        /* No lanes enabled on the core, assign default value */
        ratio = SOC_FL_PORT_RATIO_QUAD;
    }

    if ((ratio >= SOC_FL_PORT_RATIO_SINGLE) &&
        (ratio < SOC_FL_PORT_RATIO_COUNT)) {
        *port_ratio = ratio;
        rv = SOC_E_NONE;
    } else {
        rv = SOC_E_FAIL;
    }
    return rv;
}


/*
 * Check portmap configured data matches the flex mode.
 */
static int
soc_fl_port_cfg_checker(int unit, int cfg_match_id, int flex_mode_id)
{
    /*
     * Validate each core
     * Check if p2l and speed matched flex port mode of the TSC core.
     *    If matched, return the PORT_RATIO_TYPE
     *    If not matched, put warning message and return error.
     */
    _fl_option_info_t *matched_port_config = NULL;
    _fl_option_tsc_flex_mode_t *tsc_flex_modes = NULL;
    int rv = SOC_E_NONE;
    int i;
    int port_ratio = SOC_FL_PORT_RATIO_QUAD;
    int flex_mode;
    uint16 dev_id = 0;
    uint8  rev_id = 0;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];
    tsc_flex_modes = &_fl_option_tsc_flex_modes[flex_mode_id];

    soc_cm_get_id(unit, &dev_id, &rev_id);
    for (i = 0; i < _FL_MAX_TSC_COUNT; i++) {
        flex_mode = tsc_flex_modes->mode[i];
        if ((dev_id == BCM56071_DEVICE_ID) &&
            ((i == _FL_TSC_TSCF0_IDX) || (i == (_FL_TSC_TSCF0_IDX + 3)))) {
            /* Falcon #0 and #3 are disabled on 56071 */
            flex_mode = FLEX_OFF;
        }
        switch (flex_mode) {
            case FLEX_XLAUI_QXG:
                rv = soc_fl_flex_hg42_checker(unit, cfg_match_id,
                                            i, &port_ratio, 0);
                if (rv != SOC_E_NONE) {
                    rv = soc_fl_flex_qxg_qsg_checker(unit, cfg_match_id,
                                                 i, &port_ratio, 0);
                }
                break;
            case FLEX_XLAUI_QSG:
                rv = soc_fl_flex_hg42_checker(unit, cfg_match_id,
                                            i, &port_ratio, 0);
                if (rv != SOC_E_NONE) {
                    rv = soc_fl_flex_qxg_qsg_checker(unit, cfg_match_id,
                                                 i, &port_ratio, 1);
                }
                break;
            case FLEX_HG42_GEN2:
                rv = soc_fl_flex_hg42_checker(unit, cfg_match_id,
                                            i, &port_ratio, 0);
                break;
            case FLEX_HG42_UL_GEN3:
                rv = soc_fl_flex_hg42_checker(unit, cfg_match_id,
                                            i, &port_ratio, 1);
                break;
            case FLEX_2HG53_GEN3:
                rv = soc_fl_flex_d50_caui_checker(unit, cfg_match_id,
                                            i, &port_ratio, 0);
                break;
            case FLEX_CAUI_GEN3:
                rv = soc_fl_flex_d50_caui_checker(unit, cfg_match_id,
                                            i, &port_ratio, 1);
                break;
            case FLEX_MGL_GEN2:
                rv = soc_fl_flex_mgl_checker(unit, cfg_match_id,
                                               i, &port_ratio, 0);
                break;
            case FLEX_MGL_GEN3:
                rv = soc_fl_flex_mgl_checker(unit, cfg_match_id,
                                               i, &port_ratio, 1);
                break;
            case FLEX_OFF:
                rv = soc_fl_flex_off_checker(unit, cfg_match_id,
                                              i, &port_ratio);
                break;
            default:
                if (i < _FL_TSC_TSCF0_IDX) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "PORTMAP config is invalid "
                                        "on TSCE #%d.\n"), i));
                } else {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "PORTMAP config is invalid "
                                        "on TSCF #%d.\n"), i-_FL_TSC_TSCF0_IDX));
                }
                return SOC_E_FAIL;
        }
        if (rv != SOC_E_NONE) {
            if (i < _FL_TSC_TSCF0_IDX) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Incorrect PORTMAP config "
                                    "on TSCE #%d.\n"), i));
            } else {
                LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Incorrect PORTMAP config "
                                    "on TSCF #%d.\n"), i-_FL_TSC_TSCF0_IDX));
            }
            return rv;
        }
        matched_port_config->default_port_ratio[i] = port_ratio;
    }

    return rv;
}

/* Update port cfg, p2l and speed, based on spn_PORTMAP */
static int
soc_fl_port_mapping_update(int unit,
                                 int cfg_match_id,
                                 int flex_mode_id,
                                 int *updated)
{
    int lport;
    int pport;
    int port_bw;
    _fl_option_info_t *matched_port_config = NULL;
    int rv = SOC_E_NONE;
    int renew;
    char *config_str;
    char *sub_str;
    char *sub_str_end;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    renew = 0;
    /* Scan spn_PORTMAP and update to p2l_mapping and speed max tables */
    for (lport = 2; lport < SOC_MAX_LOGICAL_PORTS; lport++) {
        config_str = soc_property_port_get_str(unit, lport, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

        /*
         * portmap.<port>=<physical port number>:<bandwidth in Gb>
         */
        sub_str = config_str;

        /* Parsing physical port number */
        pport = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "Port %d: Missing physical port "
                                 "information \"%s\"\n"),
                                 lport, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (pport < SOC_MIN_PHY_PORT_NUM || \
            pport >= SOC_MAX_PHY_PORTS) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "Port %d: Invalid physical port "
                                 "number %d\n"),
                                 lport, pport));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, "Port %d: Bad config "
                                     "string \"%s\"\n"),
                                     lport, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parsing bandwidth */
        port_bw = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "Port %d: Missing bandwidth "
                                 "information \"%s\"\n"),
                                 lport, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        matched_port_config->p2l_mapping[pport] = lport;
        matched_port_config->speed_max[pport] = port_bw * 10;
        /* Special case for 2.5G config */
        if (!sal_strcmp(sub_str, "2.5")) {
            matched_port_config->speed_max[pport] = 25;
        }

        if (!renew) {
            renew = 1;
        }
    }

    *updated = renew;

    return rv;
}

STATIC
void soc_fl_port_user_config_update(int unit, int cfg_match_id)
{
    int origin_p2l_mapping[SOC_MAX_PHY_PORTS];
    int origin_speed_max[SOC_MAX_PHY_PORTS];
    int rv = SOC_E_NONE;
    _fl_option_info_t *matched_port_config = NULL;
    int i;
    int flex_mode_id = -1;
    int updated;
    char *port_ratio_str;
    int port_ratio;

    matched_port_config = &_fl_option_port_config[unit][cfg_match_id];

    /* Get the default TSC flex modes */
    for (i = 0; _fl_option_tsc_flex_modes[i].config_op; i++) {
        if (!sal_strcmp(matched_port_config->config_op,
                        _fl_option_tsc_flex_modes[i].config_op)) {
            flex_mode_id = i;
            break;
        }
    }
    if (flex_mode_id == -1) {
        /* No matched entry found */
        return;
    }

    /* Save copy of port cfg templates */
    sal_memcpy(origin_p2l_mapping,
               matched_port_config->p2l_mapping,
               SOC_MAX_PHY_PORTS * sizeof(int));
    sal_memcpy(origin_speed_max,
               matched_port_config->speed_max,
               SOC_MAX_PHY_PORTS * sizeof(int));

    /*
     * Update flex port configuration (p2l mapping and speed)
     * based on spn_PORTMAP user config
     */
    updated = 0;
    if (SOC_SUCCESS(rv)) {
        rv = soc_fl_port_mapping_update(unit, cfg_match_id, flex_mode_id,
                                         &updated);
    }

    /* If success, check p2l mapping and speed based on supported flex mode */
    if (SOC_SUCCESS(rv) && updated) {
        rv = soc_fl_port_cfg_checker(unit, cfg_match_id, flex_mode_id);
    }

    /* Any failure, restore the port cfg templates */
    if ((rv != SOC_E_NONE) || (!updated)){
        sal_memcpy(matched_port_config->p2l_mapping,
                   origin_p2l_mapping,
                   SOC_MAX_PHY_PORTS * sizeof(int));
        sal_memcpy(matched_port_config->speed_max,
                   origin_speed_max,
                   SOC_MAX_PHY_PORTS * sizeof(int));
        if (rv != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Option %s port config scan failed.\n"),
                                matched_port_config->config_op));
        }
        if (!updated) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Option %s port config not updated.\n"),
                                matched_port_config->config_op));
        }
    }

    /* Dump the final p2l mapping table */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"Physical to Logical port mapping : "
                            "[P] L")));
    for (i = 0; i < SOC_MAX_PHY_PORTS; i++) {
        if (!(i % 10)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"\n")));
        }
        if (matched_port_config->p2l_mapping[i] == -1) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"[%2d]   "), i));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"[%2d]%2d "),
                                    i, matched_port_config->p2l_mapping[i]));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"\n")));
    /* Dump the final port max speed table */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"Physical port max speed : "
                            "[P] Speed(Gb)")));
    for (i = 0; i < SOC_MAX_PHY_PORTS; i++) {
        if (!(i % 10)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"\n")));
        }
        if (matched_port_config->speed_max[i] == -1) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"[%2d]   "), i));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,"[%2d]%2d "),
                                    i, matched_port_config->speed_max[i] / 10));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"\n")));
    /* Dump port ratio mode of each TSC */
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"TSC (0~2: TSCE, 3~6: TSCF) "
                            "port ratio mode :\n")));
    for (i = 0; i < (_FL_MAX_TSC_COUNT); i++) {
        port_ratio = matched_port_config->default_port_ratio[i];
        if (port_ratio == SOC_FL_PORT_RATIO_SINGLE) {
            port_ratio_str = "SINGLE";
        } else if (port_ratio == SOC_FL_PORT_RATIO_SINGLE_XAUI) {
            port_ratio_str = "XAUI";
        } else if (port_ratio == SOC_FL_PORT_RATIO_DUAL_2_2) {
            port_ratio_str = "DUAL";
        } else if (port_ratio == SOC_FL_PORT_RATIO_TRI_012) {
            port_ratio_str = "TRI 012";
        } else if (port_ratio == SOC_FL_PORT_RATIO_TRI_023) {
            port_ratio_str = "TRI 023";
        } else if (port_ratio == SOC_FL_PORT_RATIO_QUAD) {
            port_ratio_str = "QUAD";
        } else {
            port_ratio_str = "INVALID";
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,"TSCE #%d port mode : %s\n"),
                                i, port_ratio_str));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"\n")));
}

/*
 *  Check the user configured macsec index
 *  return: SDK_E_NONE/SDK_E_FAIL
 */
STATIC int
_soc_fl_macsec_mapping_check(int unit)
{
    int rv = SOC_E_NONE;
    int pport;
    uint32 gmask = 0x0, xlmask = 0x0, clmask = 0x0;

    /*
     * Checking per port macsec index is correct.
     * With checking the macsec_index for multiple lanes.
     */
    for (pport = 0; pport < SOC_MAX_PHY_PORTS; pport++) {

        if (fl_macsec_cfg_port[unit][pport] == -1) {
            continue;
        }

        if (pport < FL_PHY_PORT_OFFSET_TSCE0) {
            /* GE port */
            if (fl_macsec_cfg_port[unit][pport] != (pport - 2)) {
                rv = SOC_E_CONFIG;
            }
            if ((gmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                gmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCE1) {
            /* XL port 0 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCE0 + 4)) {
                rv = SOC_E_CONFIG;
            }
            if ((xlmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                xlmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCE2) {
            /* XL port 1 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCE1 + 20)) {
                rv = SOC_E_CONFIG;
            }
            if ((xlmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                xlmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCF0) {
            /* XL port 2 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCE2 + 36)) {
                rv = SOC_E_CONFIG;
            }
            if ((xlmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                xlmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCF1) {
            /* CL port 0 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCF0 + 44)) {
                rv = SOC_E_CONFIG;
            }
            if ((clmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                clmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCF2) {
            /* CL port 1 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCF1 + 32)) {
                rv = SOC_E_CONFIG;
            }
            if ((clmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                clmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else if (pport < FL_PHY_PORT_OFFSET_TSCF3) {
            /* CL port 2 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCF2 + 40)) {
                rv = SOC_E_CONFIG;
            }
            if ((clmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                clmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        } else {
            /* CL port 3 */
            if (fl_macsec_cfg_port[unit][pport] != (pport - FL_PHY_PORT_OFFSET_TSCF3 + 16)) {
                rv = SOC_E_CONFIG;
            }
            if ((clmask & (1 << SOC_PORT_BLOCK_NUMBER(unit, pport))) == 0) {
                clmask |= 1 << SOC_PORT_BLOCK_NUMBER(unit, pport);
            }
        }
        if (rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "Physical port %d has incorrect "
                         "macsec index %d\n"),
                         pport, fl_macsec_cfg_port[unit][pport]));
        }
    }

    /*
     * Checking exclude port group per port block.
     */
    /* GPORT0/1 exclude XLPORT0 */
    if ((gmask & (1 << 0)) || (gmask & (1 << 1))) {
        if (xlmask & (1 << 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "GPORT0/1 has conflict with XLPORT0.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* GPORT2/3 exclude XLPORT1/CLPORT3 */
    if ((gmask & (1 << 2)) || (gmask & (1 << 3))) {
        if ((xlmask & (1 << 1)) || (clmask & (1 << 3))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "GPORT2/3 has conflict with XLPORT1/3.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* GPORT4/5 exclude XLPORT2/CLPORT0/1/2 */
    if ((gmask & (1 << 4)) || (gmask & (1 << 5))) {
        if ((xlmask & (1 << 1)) || (clmask & (1 << 0)) || (clmask & (1 << 1)) ||
            (clmask & (1 << 2))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "GPORT4/5 has conflict with XLPORT1/CLPORT0/1/2.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* XLPORT0 exclude GPORT0/1 */
    if (xlmask & (1 << 0)) {
        if ((gmask & (1 << 0)) || (gmask & (1 << 1))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "XLPORT0 has conflict with GPORT0/1.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* XLPORT1 exclude GPORT2/3 */
    if (xlmask & (1 << 1)) {
        if ((gmask & (1 << 2)) || (gmask & (1 << 3))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "XLPORT1 has conflict with GPORT2/3.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* XLPORT2 exclude GPORT4/5 */
    if (xlmask & (1 << 2)) {
        if ((gmask & (1 << 4)) || (gmask & (1 << 5))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "XLPORT2 has conflict with GPORT4/5.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* CLPORT0/1/2 exclude GPORT4/5 */
    if ((clmask & (1 << 0)) || (clmask & (1 << 1)) || (clmask & (1 << 2))) {
        if ((gmask & (1 << 4)) || (gmask & (1 << 5))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "CLPORT0/1/2 has conflict with GPORT4/5.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* CLPORT3 exclude GPORT2/3 */
    if (clmask & (1 << 3)) {
        if ((gmask & (1 << 2)) || (gmask & (1 << 3))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "CLPORT3 has conflict with GPORT2/3.\n")));
            return SOC_E_CONFIG;
        }
    }

    /*
     * Checking exclude port between MerlinQ and Falcon.
     */
    /* MerlinQ #0,#1,#2 in Quad mode exclude Falcon #0,#1,#2,#3 */
    if ((gmask & 0x3f) == 0x3f) {
        if ((clmask & (1 << 0)) || (clmask & (1 << 1)) ||
            (clmask & (1 << 2)) || (clmask & (1 << 3))) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "TSCE0-2 has conflict with TSCF0-3.\n")));
            return SOC_E_CONFIG;
        }
    }
    /* MerlinQ #0,#1 in Quad mode execlude Falcon #3 */
    if ((gmask & 0xf) == 0xf) {
        if (clmask & (1 << 3)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "TSCE0-1 has conflict with TSCF3.\n")));
            return SOC_E_CONFIG;
        }
    }

    return rv;
}

/*
 *  FL port mapping
 * cpu port number is fixed: physical 0, logical 0, mmu 0
 */
int
soc_firelight_port_config_init(int unit, uint16 dev_id)
{

    soc_info_t *si;
    int rv = SOC_E_NONE;
    int phy_port;
    int *p2l_mapping = 0, *speed_max = 0;
    char *option;
    int match = -1, i;
    int valid_sku_option;
    char *default_option_string = NULL;
    _fl_option_info_t *matched_port_config = NULL;
    int freq = _FL_56070_SYSTEM_FREQ;

    option = soc_property_get_str(unit, spn_INIT_PORT_CONFIG_OPTION);
    if (option == NULL) {
        /* Backward compatible */
        option = soc_property_get_str(unit, "bcm56070_init_port_config");
    }

    _fl_sku_option_support_list = _fl_a0_flex_sku_option_support_list;

    for (i = 0; i < SOC_FL_PORT_CONFIG_OPTION_MAX; i++) {
        _fl_option_port_config[unit][i] =
                _fl_a0_flex_option_port_template[i];
        _fl_option_port_config[unit][i].p2l_mapping = _fl_p2l_mapping[unit];
        _fl_option_port_config[unit][i].speed_max = _fl_speed_max[unit];
        _fl_option_port_config[unit][i].tdm_table = _fl_tdm_table[unit];
        if (i == 0) {
            sal_memcpy(_fl_option_port_config[unit][i].p2l_mapping,
                        fl_p2l_mapping_template_op,
                        sizeof(fl_p2l_mapping_template_op));
            sal_memcpy(_fl_option_port_config[unit][i].speed_max,
                        fl_port_speed_max_template_op,
                        sizeof(fl_port_speed_max_template_op));
            sal_memcpy(_fl_option_port_config[unit][i].tdm_table,
                        fl_tdm_table_default,
                        sizeof(fl_tdm_table_default));
        }
    }
    matched_option_idx[unit] = -1;

    if (option == NULL) {
        /* option = OPTION_2; */
        option = OPTION_2;
    }

    valid_sku_option = 0;
    for (i = 0; _fl_sku_option_support_list[i].dev_id; i++) {
        if (dev_id == _fl_sku_option_support_list[i].dev_id) {
            /* save the default option in case nothing matched later */
            if ((default_option_string == NULL) &&
                _fl_sku_option_support_list[i].default_option) {
                default_option_string =
                    _fl_sku_option_support_list[i].option_string;
            }
            /* compare if the optoin is suppported in the SKU */
            if (!sal_strcmp(_fl_sku_option_support_list[i].option_string,
                   option)) {
                valid_sku_option = 1;
                break;
            }
        }
    }
    if (!valid_sku_option && default_option_string) {
        LOG_WARN(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                        "Warning: option %s "
                        "is not supported in %s \n"), option,
                        soc_dev_name(unit)));
        LOG_WARN(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                        "Use default option %s "
                        "instead\n"), default_option_string));
        /* assign the default option if nothing matched */
        option = default_option_string;
    }

    for (i = 0; _fl_option_port_config[unit][i].config_op; i++) {
        if (!sal_strcmp(_fl_option_port_config[unit][i].config_op, option)) {
            match = i;
            break;
        }
    }

    /* Flex port configuration update */
    soc_fl_port_user_config_update(unit, (match == -1) ? 0 : match);

    for (i = 0; i < _FL_MAX_TSC_COUNT; i++) {
        _fl_tsc[unit][i].port_count = 0;
        _fl_tsc[unit][i].valid = 0;
        _fl_tsc[unit][i].phy_port_base = tsc_phy_port[i];
        if (match == -1) {
            _fl_tsc[unit][i].port_count =
                _fl_option_port_config[unit][0].default_port_ratio[i];
        } else {
            _fl_tsc[unit][i].port_count =
                _fl_option_port_config[unit][match].default_port_ratio[i];
        }
    }

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        fl_port_speed[unit][phy_port] = 0;
    }

    if (match == -1) {
        /* take opton 2a as default config */
        p2l_mapping = p2l_mapping_op02_5;
        speed_max = port_speed_max_op02_5;
        freq = _FL_56070_SYSTEM_FREQ;
        LOG_WARN(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,"%s: no device_id matched\n"), FUNCTION_NAME()));
    } else {
        matched_option_idx[unit] = match;
        matched_port_config = &_fl_option_port_config[unit][match];

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,"%s: option matched %s\n"),
            FUNCTION_NAME(), matched_port_config->config_op));

        p2l_mapping = matched_port_config->p2l_mapping;
        speed_max = matched_port_config->speed_max;
        freq = matched_port_config->freq;
    }

    si = &SOC_INFO(unit);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        if ((speed_max[phy_port] == -1) && (phy_port != 0)) {
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->max_port_p2m_mapping[phy_port] = -1;
        } else {
            si->port_p2l_mapping[phy_port] = p2l_mapping[phy_port];
            si->port_p2m_mapping[phy_port] = -1;
            si->max_port_p2m_mapping[phy_port] = -1;
            if (p2l_mapping[phy_port] >= 0) {
                si->port_num_lanes[p2l_mapping[phy_port]] = 1;
            }
        }
        if (speed_max[phy_port] != -1) {
            si->port_speed_max[si->port_p2l_mapping[phy_port]] =
                speed_max[phy_port] * 100;
        }
        if (p2l_mapping[phy_port] >= 0) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"%s: Port %d, lane number %d.\n"),
                FUNCTION_NAME(), p2l_mapping[phy_port],
                si->port_num_lanes[p2l_mapping[phy_port]]));
        }
    }

    /* physical to mmu port mapping */
    {
        int phy_port_cpu;
        int phy_port_loopback;
        int num_mmu_port = SOC_MAX_MMU_PORTS;
        int mmu_port_max;
        int mmu_port;
        int port;

        phy_port_cpu = 0;
        phy_port_loopback = 1;
        si->port_p2m_mapping[phy_port_cpu] = 0;
        si->port_p2m_mapping[phy_port_loopback] = 1;

        mmu_port_max = num_mmu_port - 1;

        /* Assign MMU port for all ports with speed >= 100G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 100000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all ports with speed >= 50G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 50000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all ports with speed >= 40G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 40000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all ports with speed >= 25G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 25000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all ports with speed >= 20G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 20000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all ports with speed >= 10G */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if ((port != -1) && (si->port_speed_max[port] >= 10000)) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /* Assign MMU port for all remaining ports */
        for (phy_port = (SOC_MAX_PHY_PORTS - 1);
              phy_port >= SOC_MIN_PHY_PORT_NUM; phy_port--) {
              if (si->port_p2m_mapping[phy_port] != -1) {
                  continue;
              }

             port = si->port_p2l_mapping[phy_port];
             if (port != -1) {
                 si->port_p2m_mapping[phy_port] = mmu_port_max;
                 mmu_port_max--;
             }
        }

        /*
         * reset port_m2p_mapping, actual value is setup in
         * soc_info_config() later
         */
        for (mmu_port = 0; mmu_port < num_mmu_port; mmu_port++) {
            si->port_m2p_mapping[mmu_port] = -1;
        }
    }

    /* Get physical to macsec port mapping */
    {
        int macsec_index;
        int gport_macsec = -1;

        for (i = 0; i < SOC_MAX_PHY_PORTS; i++) {
            fl_macsec_port[unit][i] = -1;
            fl_macsec_cfg_port[unit][i] = -1;
        }

        for (i = 0; i < FL_MACSEC_MAX_PORT_NUM; i++) {
            fl_macsec_valid[unit][i] = -1;
        }

        for (i = 0; i < SOC_MAX_PHY_PORTS; i++) {
            macsec_index = soc_property_port_get(unit, i, spn_MACSEC_PORT_INDEX, -1);
            if (macsec_index >= FL_MACSEC_MAX_PORT_NUM) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Warning: exceed macsec index %d\n"),
                        (FL_MACSEC_MAX_PORT_NUM - 1)));
                continue;
            }

            if (macsec_index != -1) {
                if (fl_macsec_valid[unit][macsec_index] != -1 &&
                    fl_macsec_valid[unit][macsec_index] != i) {
                    LOG_WARN(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "macsec port %d for physical port "
                            "%d has conflict with port %d\n"),
                            macsec_index, i, fl_macsec_valid[unit][macsec_index]));
                    continue;
                } else {
                    fl_macsec_valid[unit][macsec_index] = i;
                    macsec_count[unit]++;
                }
            }

            fl_macsec_port[unit][i] = macsec_index;
            fl_macsec_cfg_port[unit][i] = macsec_index;
        }

        /* Check the macsec index is valid */
        SOC_IF_ERROR_RETURN(_soc_fl_macsec_mapping_check(unit));

        /* No macsec port input, set 0~FL_MACSEC_MAX_PORT_NUM ports as default */
        phy_port = SOC_MIN_PHY_PORT_NUM;
        if (!macsec_count[unit]) {
            for (i = 0; i < FL_MACSEC_MAX_PORT_NUM; i++) {
                for (; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
                    if (si->port_p2l_mapping[phy_port] != -1) {
                        break;
                    }
                }
                if (phy_port >= SOC_MAX_PHY_PORTS) {
                    break;
                }
                if ((gport_macsec != -1) &&
                    (phy_port >= tsc_phy_port[0])) {
                    break;
                }
                fl_macsec_valid[unit][i] = phy_port;
                fl_macsec_port[unit][phy_port] = i;
                if ((gport_macsec == -1) &&
                    (phy_port < tsc_phy_port[0])) {
                    gport_macsec = 0;
                }
                phy_port++;
            }
        }
    }

    /* early update for port_l2p_mapping and port_serdes */
    {
        int port;

        rv = SOC_E_NONE;

        for (port = 0; port < SOC_MAX_PHY_PORTS; port++) {
            si->port_l2p_mapping[port] = -1;
            si->port_serdes[port] = -1;
            if (si->port_p2l_mapping[port] != -1) {
                si->port_l2p_mapping[si->port_p2l_mapping[port]] = port;
            }
        }

        for (port = 0; port < SOC_MAX_LOGICAL_PORTS; port++) {
            if (si->port_l2p_mapping[port] == -1) {
                continue;
            }
            phy_port = si->port_l2p_mapping[port];
            if (phy_port <= 49) {
                si->port_serdes[port] = (phy_port - 2) / _FL_PORTS_PER_PMQ_PBLK;
            } else {
                si->port_serdes[port] = ((phy_port - 2) / _FL_PORTS_PER_PBLK) - 12;
            }
        }
    }

    /* system core clock */
    si->frequency = freq;

    return rv;
}

int
_soc_firelight_autotdm_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_schedule_state_t *port_schedule_state;
    soc_port_map_type_t *in_portmap;
    int port;
    int rv = SOC_E_NONE;

    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "Firelight soc_port_schedule_state_t");
    if (port_schedule_state == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    /* Core clock frequency */
    port_schedule_state->frequency = si->frequency;

    /* Construct in_port_map */
    in_portmap = &port_schedule_state->in_port_map;

    PBMP_PORT_ITER(unit, port) {
        in_portmap->log_port_speed[port] = si->port_speed_max[port];
        in_portmap->port_num_lanes[port] = si->port_num_lanes[port];
    }

    /* DV requires CPU port speed 1G */
    port = 0;
    in_portmap->log_port_speed[port] = SPEED_1G;

    sal_memcpy(in_portmap->port_p2l_mapping, si->port_p2l_mapping,
               sizeof(int) * SOC_MAX_PHY_PORTS);
    sal_memcpy(in_portmap->port_l2p_mapping, si->port_l2p_mapping,
               sizeof(int) * SOC_MAX_LOGICAL_PORTS);
    sal_memcpy(in_portmap->port_p2m_mapping, si->port_p2m_mapping,
               sizeof(int) * SOC_MAX_PHY_PORTS);
    sal_memcpy(in_portmap->port_m2p_mapping, si->port_m2p_mapping,
               sizeof(int) * SOC_MAX_MMU_PORTS);
    sal_memcpy(&in_portmap->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_portmap->hg2_pbm, &si->hg.bitmap, sizeof(pbmp_t));

    rv = soc_fl_tdm_init(unit, port_schedule_state);
    if (rv != SOC_E_NONE) {
        LOG_CLI((BSL_META_U(unit,
                            "TDM calendar generation failed.\n")));
        sal_free(port_schedule_state);
        return rv;
    }

    sal_free(port_schedule_state);

    return rv;
}

STATIC int fl_ceiling_func(uint32 numerators, uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;
    if (numerators % denominator != 0) {
        result++;
    }
    return result;
}

STATIC int fl_floor_func(uint32 numerators, uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;

    return result;
}


STATIC int _soc_firelight_mmu_init_helper_standalone_lossy(int unit)
{
    uint32 rval;
    int port, phy_port, mmu_port;
    int index;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t pbmp_cpu;
    soc_pbmp_t pbmp_uplink;
    soc_pbmp_t pbmp_uplink_6kxq;
    soc_pbmp_t pbmp_uplink_2kxq;
    soc_pbmp_t pbmp_downlink;
    int speed;
    int standard_jumbo_frame;
    int cell_size;
    int ethernet_mtu_cell;
    int standard_jumbo_frame_cell;
    int total_physical_memory;
    int total_cell_memory_for_admission;
    int number_of_used_memory_banks;
    int reserved_for_cfap;
    int skidmarker;
    int prefetch;
    int total_cell_memory;
    int cfapfullsetpoint;
    int total_advertised_cell_memory;
    int number_of_uplink_ports;
    int number_of_downlink_ports;
    int queue_port_limit_ratio;
    int egress_queue_min_reserve_uplink_ports_lossy;
    int egress_queue_min_reserve_downlink_ports_lossy;
    int egress_queue_min_reserve_uplink_ports_lossless;
    int egress_queue_min_reserve_downlink_ports_lossless;
    int egress_queue_min_reserve_cpu_ports;
    int egress_xq_min_reserve_lossy_ports;
    int egress_xq_min_reserve_lossless_uplink_ports;
    int egress_xq_min_reserve_lossless_downlink_ports;
    int num_active_pri_group_lossless;
    int num_lossy_queues;
    int num_lossless_queues;
    int mmu_xoff_pkt_threshold_uplink_ports;
    int mmu_xoff_pkt_threshold_downlink_ports;
    int mmu_xoff_cell_threshold_1g_port_downlink_ports;
    int mmu_xoff_cell_threshold_all_uplink_ports;
    int num_cpu_queues;
    int num_cpu_ports;
    int numxqs_per_uplink_ports_6kxq;
    int numxqs_per_uplink_ports_2kxq;
    int numxqs_per_downlink_ports_and_cpu_port;
    int headroom_for_1g_port;
    int xoff_cell_thresholds_per_port_1g_port_downlink_ports;
    int xoff_cell_threshold_all_uplink_ports;
    int xoff_packet_thresholds_per_port_uplink_port;
    int xoff_packet_thresholds_per_port_downlink_port;
    int discard_limit_per_port_pg_uplink_1g_port;
    int discard_limit_per_port_pg_downlink_port;
    int total_reserved_cells_for_uplink_ports;
    int total_reserved_cells_for_downlink_ports;
    int total_reserved_cells_for_cpu_port;
    int total_reserved;
    int shared_space_cells;
    int reserved_xqs_per_uplink_port;
    int shared_xqs_per_6kxq_uplink_port;
    int shared_xqs_per_2kxq_uplink_port;
    int reserved_xqs_per_downlink_port;
    int shared_xqs_per_downlink_port;
    int cfapfullthreshold_cfapfullsetpoint_up;
    int gbllimitsetlimit_gblcellsetlimit_up;
    int totaldyncellsetlimit_totaldyncellsetlimit_up;
    int holcosminxqcnt_qlayer8_holcosminxqcnt_up;
    int lwmcoscellsetlimit0_cellsetlimit_up;
    int lwmcoscellsetlimit7_cellsetlimit_up;
    int lwmcoscellsetlimit_qlayer0_cellsetlimit_up;
    int lwmcoscellsetlimit_qlayer7_cellsetlimit_up;
    int lwmcoscellsetlimit_qlayer8_cellsetlimit_up;
    int holcoscellmaxlimit0_cellmaxlimit_up;
    int holcoscellmaxlimit7_cellmaxlimit_up;
    int holcoscellmaxlimit_qlayer0_cellmaxlimit_up;
    int holcoscellmaxlimit_qlayer7_cellmaxlimit_up;
    int holcoscellmaxlimit_qlayer8_cellmaxlimit_up;
    int dyncelllimit_dyncellsetlimit_up;
    int dynxqcntport_dynxqcntport_down_1;
    int lwmcoscellsetlimit0_cellsetlimit_down_1;
    int lwmcoscellsetlimit7_cellsetlimit_down_1;
    int holcoscellmaxlimit0_cellmaxlimit_down_1;
    int holcoscellmaxlimit7_cellmaxlimit_down_1;
    int dyncelllimit_dyncellsetlimit_down_1;
    int holcosminxqcnt0_holcosminxqcnt_cpu;
    int holcosminxqcnt7_holcosminxqcnt_cpu;
    int dynxqcntport_dynxqcntport_cpu;
    int lwmcoscellsetlimit0_cellsetlimit_cpu;
    int lwmcoscellsetlimit7_cellsetlimit_cpu;
    int holcoscellmaxlimit0_cellmaxlimit_cpu;
    int holcoscellmaxlimit7_cellmaxlimit_cpu;
    int dyncelllimit_dyncellsetlimit_cpu;
    int cl_downlink_option = 0;
    int count_ge = 0, count_xl = 0;
    soc_pbmp_t pbmp_downlink_backplane;

    SOC_PBMP_CLEAR(pbmp_downlink_backplane);
    SOC_PBMP_COUNT(PBMP_GE_ALL(unit), count_ge);
    SOC_PBMP_COUNT(PBMP_XL_ALL(unit), count_xl);
    if ((count_ge + count_xl) == 0) {
        cl_downlink_option = 1;
        pbmp_downlink_backplane = soc_property_get_pbmp(unit,
                                              spn_DOWNLINK_BACKPLANE_PBMP, 0);
    }

    /* setup port bitmap according the port max speed for lossy
     *   TSC/TSCF    : uplink port
     *   QGMII/SGMII : downlink port
     */
    num_cpu_ports = 0;
    number_of_uplink_ports = 0;
    number_of_downlink_ports = 0;
    SOC_PBMP_CLEAR(pbmp_cpu);
    SOC_PBMP_CLEAR(pbmp_uplink);
    SOC_PBMP_CLEAR(pbmp_uplink_6kxq);
    SOC_PBMP_CLEAR(pbmp_uplink_2kxq);
    SOC_PBMP_CLEAR(pbmp_downlink);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            continue;
        }
        if (IS_CPU_PORT(unit, port)) {
            num_cpu_ports++;
            SOC_PBMP_PORT_ADD(pbmp_cpu, port);
            continue;
        }
        speed = si->port_init_speed[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (speed <= 0 ) {
            continue; /* this user port has not been mapping in this sku */
        } else if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            continue; /* this user port has been masked out by pbmp_valid */
        }

        if (IS_CL_PORT(unit, port) && cl_downlink_option &&
            !SOC_PBMP_MEMBER(pbmp_downlink_backplane, port)) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        }

        if (IS_CL_PORT(unit, port) && cl_downlink_option &&
            (SOC_PBMP_MEMBER(pbmp_uplink, port))) {
            number_of_uplink_ports++;
            if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
            } else {
                SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
            }
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                number_of_uplink_ports++;
                if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
                } else {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
                }
            }
        } else {
            number_of_downlink_ports++;
            SOC_PBMP_PORT_ADD(pbmp_downlink, port);
        }
    }

    standard_jumbo_frame = 9216;
    cell_size = 144;
    ethernet_mtu_cell = fl_ceiling_func(15 * 1024 / 10, cell_size);
    standard_jumbo_frame_cell = fl_ceiling_func(standard_jumbo_frame, cell_size);
    total_physical_memory = 16 * 1024;
    total_cell_memory_for_admission = 14 * 1024;
    number_of_used_memory_banks = 8;
    reserved_for_cfap = (65) * 2 + number_of_used_memory_banks * 4;
    skidmarker = 7;
    prefetch = 64 + 4;
    total_cell_memory = total_cell_memory_for_admission;
    cfapfullsetpoint = total_physical_memory - reserved_for_cfap;
    total_advertised_cell_memory = total_cell_memory;
    queue_port_limit_ratio = 8;
    egress_queue_min_reserve_uplink_ports_lossy = ethernet_mtu_cell;
    egress_queue_min_reserve_downlink_ports_lossy = ethernet_mtu_cell;
    egress_queue_min_reserve_uplink_ports_lossless = 0;
    egress_queue_min_reserve_downlink_ports_lossless = 0;
    egress_queue_min_reserve_downlink_ports_lossless = 0;
    egress_queue_min_reserve_cpu_ports = ethernet_mtu_cell;
    egress_xq_min_reserve_lossy_ports
          = ethernet_mtu_cell;
    egress_xq_min_reserve_lossless_uplink_ports = 0;
    egress_xq_min_reserve_lossless_downlink_ports = 0;
    num_active_pri_group_lossless = 1;
    num_lossless_queues = 0;
    num_lossy_queues = 8;
    mmu_xoff_pkt_threshold_uplink_ports = total_advertised_cell_memory;
    mmu_xoff_pkt_threshold_downlink_ports = total_advertised_cell_memory;
    mmu_xoff_cell_threshold_1g_port_downlink_ports
          = total_advertised_cell_memory;
    mmu_xoff_cell_threshold_all_uplink_ports = total_advertised_cell_memory;
    num_cpu_queues = 8;
    num_cpu_ports = 1;
    numxqs_per_uplink_ports_6kxq = 6 * 1024;
    numxqs_per_uplink_ports_2kxq = 2 * 1024;
    numxqs_per_downlink_ports_and_cpu_port = 2 * 1024;
    headroom_for_1g_port = 0;
    xoff_cell_thresholds_per_port_1g_port_downlink_ports
          = mmu_xoff_cell_threshold_1g_port_downlink_ports;
    xoff_cell_threshold_all_uplink_ports
          = mmu_xoff_cell_threshold_all_uplink_ports;
    xoff_packet_thresholds_per_port_uplink_port
          = mmu_xoff_pkt_threshold_uplink_ports;
    xoff_packet_thresholds_per_port_downlink_port
          = mmu_xoff_pkt_threshold_downlink_ports;
    discard_limit_per_port_pg_uplink_1g_port
        = xoff_cell_thresholds_per_port_1g_port_downlink_ports
          + headroom_for_1g_port;
    discard_limit_per_port_pg_downlink_port = total_advertised_cell_memory;
    total_reserved_cells_for_uplink_ports
        = egress_queue_min_reserve_uplink_ports_lossy
          * number_of_uplink_ports * num_lossy_queues
          + number_of_uplink_ports
          * egress_queue_min_reserve_uplink_ports_lossless
          * num_lossless_queues;
    total_reserved_cells_for_downlink_ports
        = number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossy
          * (num_lossy_queues) + number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossless
          * num_active_pri_group_lossless;
    total_reserved_cells_for_cpu_port
        = num_cpu_ports * egress_queue_min_reserve_cpu_ports
          * num_cpu_queues;
    total_reserved
        = total_reserved_cells_for_uplink_ports
          + total_reserved_cells_for_downlink_ports
          + total_reserved_cells_for_cpu_port;
    shared_space_cells = total_advertised_cell_memory - total_reserved;
    reserved_xqs_per_uplink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_uplink_ports
          * num_active_pri_group_lossless;
    shared_xqs_per_6kxq_uplink_port
          = numxqs_per_uplink_ports_6kxq - reserved_xqs_per_uplink_port;
    shared_xqs_per_2kxq_uplink_port
          = numxqs_per_uplink_ports_2kxq - reserved_xqs_per_uplink_port;
    reserved_xqs_per_downlink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_downlink_ports
          * num_active_pri_group_lossless;
    shared_xqs_per_downlink_port
        = numxqs_per_downlink_ports_and_cpu_port
          - reserved_xqs_per_downlink_port;
    cfapfullthreshold_cfapfullsetpoint_up = cfapfullsetpoint;
    gbllimitsetlimit_gblcellsetlimit_up = total_cell_memory_for_admission;
    totaldyncellsetlimit_totaldyncellsetlimit_up = shared_space_cells;
    holcosminxqcnt_qlayer8_holcosminxqcnt_up = 0;
    lwmcoscellsetlimit0_cellsetlimit_up
          = egress_queue_min_reserve_uplink_ports_lossy;
    lwmcoscellsetlimit7_cellsetlimit_up
          = egress_queue_min_reserve_uplink_ports_lossy;
    lwmcoscellsetlimit_qlayer0_cellsetlimit_up
          = egress_queue_min_reserve_uplink_ports_lossy;
    lwmcoscellsetlimit_qlayer7_cellsetlimit_up
          = egress_queue_min_reserve_uplink_ports_lossy;
    lwmcoscellsetlimit_qlayer8_cellsetlimit_up = 0;
    holcoscellmaxlimit0_cellmaxlimit_up
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit0_cellsetlimit_up;
    holcoscellmaxlimit7_cellmaxlimit_up
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit7_cellsetlimit_up;
    holcoscellmaxlimit_qlayer0_cellmaxlimit_up
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit_qlayer0_cellsetlimit_up;
    holcoscellmaxlimit_qlayer7_cellmaxlimit_up
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit_qlayer7_cellsetlimit_up;
    holcoscellmaxlimit_qlayer8_cellmaxlimit_up
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit_qlayer8_cellsetlimit_up;
    dyncelllimit_dyncellsetlimit_up = shared_space_cells;
    dynxqcntport_dynxqcntport_down_1
          = shared_xqs_per_downlink_port - skidmarker - prefetch;
    lwmcoscellsetlimit0_cellsetlimit_down_1 =
              egress_queue_min_reserve_downlink_ports_lossy;
    lwmcoscellsetlimit7_cellsetlimit_down_1 =
              egress_queue_min_reserve_downlink_ports_lossy;
    holcoscellmaxlimit0_cellmaxlimit_down_1
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit0_cellsetlimit_down_1;
    holcoscellmaxlimit7_cellmaxlimit_down_1
        = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio)
          + lwmcoscellsetlimit7_cellsetlimit_down_1;
    dyncelllimit_dyncellsetlimit_down_1 = shared_space_cells;
    holcosminxqcnt0_holcosminxqcnt_cpu = egress_queue_min_reserve_cpu_ports;
    holcosminxqcnt7_holcosminxqcnt_cpu = egress_queue_min_reserve_cpu_ports;
    dynxqcntport_dynxqcntport_cpu =
              shared_xqs_per_downlink_port - skidmarker - prefetch;
    lwmcoscellsetlimit0_cellsetlimit_cpu = egress_queue_min_reserve_cpu_ports;
    lwmcoscellsetlimit7_cellsetlimit_cpu = egress_queue_min_reserve_cpu_ports;
    holcoscellmaxlimit0_cellmaxlimit_cpu =
              fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
              lwmcoscellsetlimit0_cellsetlimit_cpu;
    holcoscellmaxlimit7_cellmaxlimit_cpu =
              fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
              lwmcoscellsetlimit7_cellsetlimit_cpu;
    dyncelllimit_dyncellsetlimit_cpu = shared_space_cells;

    if ((shared_space_cells * cell_size)/1024 <= 800) {
        LOG_CLI((BSL_META_U(unit,
                 "Shared Pool Is Small,\
                 should be larger than 800 (value=%d)\n"),
                 (shared_space_cells * cell_size)/1024));
        return SOC_E_PARAM;
    }

    /* system-based */
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLSETPOINTf,
                           cfapfullsetpoint);
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLRESETPOINTf,
                           cfapfullthreshold_cfapfullsetpoint_up -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, GBLLIMITSETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLSETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, GBLLIMITRESETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLRESETLIMITf,
                           gbllimitsetlimit_gblcellsetlimit_up);
    soc_reg_field32_modify(unit, TOTALDYNCELLSETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLSETLIMITf,
                           shared_space_cells);
    soc_reg_field32_modify(unit, TOTALDYNCELLRESETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLRESETLIMITf,
                           totaldyncellsetlimit_totaldyncellsetlimit_up -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, TWO_LAYER_SCH_MODEr,
                           REG_PORT_ANY,
                           SCH_MODEf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           MULTIPLE_ACCOUNTING_FIX_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           CNG_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYN_XQ_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           HOL_CELL_SOP_DROP_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYNAMIC_MEMORY_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           SKIDMARKERf,
                           3);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_0r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_1r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_2r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           3);

    soc_reg32_set(unit, E2ECC_MODEr, REG_PORT_ANY, 0, 0);
    soc_reg32_set(unit, E2ECC_HOL_ENr, REG_PORT_ANY, 0, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    /* E2ECC_TX_ENABLE_BMPr, index 0 ~ 7 */
    for (index = 0; index <= 7; index++) {
        soc_reg32_set(unit, E2ECC_TX_ENABLE_BMPr, REG_PORT_ANY, index, 0);
    }
    soc_reg32_set(unit, E2ECC_TX_PORTS_NUMr, REG_PORT_ANY, 0, 0);

    /* port-based : uplink 6kxq*/
    SOC_PBMP_ITER(pbmp_uplink_6kxq, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker
                              - prefetch
                              + egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker
                              - prefetch
                              + holcosminxqcnt_qlayer8_holcosminxqcnt_up);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker
                              - prefetch
                              + egress_xq_min_reserve_lossy_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker
                              - prefetch
                              + egress_xq_min_reserve_lossy_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf,
                          shared_xqs_per_6kxq_uplink_port  - skidmarker
                          - prefetch
                          + egress_xq_min_reserve_lossy_ports - 1);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker
                              - prefetch
                              + holcosminxqcnt_qlayer8_holcosminxqcnt_up - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_uplink_ports_6kxq - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_uplink_ports_6kxq - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_uplink_ports_6kxq - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_uplink_ports_6kxq - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker
                          - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker
                          - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf, 0);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf, 0);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf,
                              fl_ceiling_func(shared_space_cells,
                                           queue_port_limit_ratio)
                              + lwmcoscellsetlimit_qlayer0_cellsetlimit_up);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf,
                          fl_ceiling_func(shared_space_cells,
                                          queue_port_limit_ratio)
                          + lwmcoscellsetlimit_qlayer7_cellsetlimit_up);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf,
                              fl_ceiling_func(shared_space_cells,
                                              queue_port_limit_ratio) +
                              lwmcoscellsetlimit_qlayer8_cellsetlimit_up);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              holcoscellmaxlimit_qlayer0_cellmaxlimit_up
                              - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf,
                          holcoscellmaxlimit_qlayer7_cellmaxlimit_up
                          - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              holcoscellmaxlimit_qlayer8_cellmaxlimit_up
                              - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellsetlimit_up
                          - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                        port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf,
                              numxqs_per_uplink_ports_6kxq - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_uplink_ports_6kxq - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_uplink_ports_6kxq - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_uplink_ports_6kxq - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf,
                              (total_advertised_cell_memory - 1));
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1
                              - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : uplink 2kxq*/
    SOC_PBMP_ITER(pbmp_uplink_2kxq, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_2kxq_uplink_port - skidmarker
                              - prefetch
                              + egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_2kxq_uplink_port  - skidmarker
                              - prefetch
                              + egress_xq_min_reserve_lossy_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                          PKTRESETLIMITf,
                          shared_xqs_per_2kxq_uplink_port  - skidmarker
                          - prefetch
                          + egress_xq_min_reserve_lossy_ports - 1);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, rval);

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_uplink_ports_2kxq - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_uplink_ports_2kxq - 1);

            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_uplink_ports_2kxq - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_uplink_ports_2kxq - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_2kxq_uplink_port - skidmarker
                          - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_2kxq_uplink_port - skidmarker
                          - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf,
                              fl_ceiling_func(shared_space_cells,
                                           queue_port_limit_ratio)
                              + lwmcoscellsetlimit0_cellsetlimit_up);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                          CELLMAXLIMITf,
                          fl_ceiling_func(shared_space_cells,
                                       queue_port_limit_ratio)
                          + lwmcoscellsetlimit7_cellsetlimit_up);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, rval);

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              holcoscellmaxlimit0_cellmaxlimit_up
                              - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                          CELLMAXRESUMELIMITf,
                          holcoscellmaxlimit7_cellmaxlimit_up
                          - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, rval);

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellsetlimit_up
                          - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : downlink */
    SOC_PBMP_ITER(pbmp_downlink, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_downlink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                          CELLSETLIMITf,
                          xoff_cell_thresholds_per_port_1g_port_downlink_ports);
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                          CELLRESETLIMITf,
                          xoff_cell_thresholds_per_port_1g_port_downlink_ports);
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 7 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf,
                          discard_limit_per_port_pg_uplink_1g_port);
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 7, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNTr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker
                                - prefetch
                                + egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                            port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          dynxqcntport_dynxqcntport_down_1 - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf,
                                  fl_ceiling_func(shared_space_cells,
                                                  queue_port_limit_ratio)
                                  + lwmcoscellsetlimit0_cellsetlimit_down_1);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  holcoscellmaxlimit0_cellmaxlimit_down_1
                                  - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf,
                                  fl_ceiling_func(shared_space_cells,
                                                  queue_port_limit_ratio));
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  fl_ceiling_func(shared_space_cells,
                                                  queue_port_limit_ratio)
                                  - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf,
                                fl_ceiling_func(shared_space_cells,
                                            queue_port_limit_ratio)
                                + lwmcoscellsetlimit0_cellsetlimit_down_1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf,
                            fl_ceiling_func(shared_space_cells,
                                        queue_port_limit_ratio)
                            + lwmcoscellsetlimit7_cellsetlimit_down_1);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf,
                                holcoscellmaxlimit0_cellmaxlimit_down_1
                                - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              holcoscellmaxlimit7_cellmaxlimit_down_1
                              - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, 7, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellsetlimit_down_1
                          - (ethernet_mtu_cell * 2));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 - 1*/
            for (index = 0; index <= 1; index++) {
                soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                COLOR_DROP_ENf,
                                0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                PKTSETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                PKTRESETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXLIMITf,
                                (total_advertised_cell_memory - 1));
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXRESUMELIMITf,
                                total_advertised_cell_memory - 1
                                - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                            COLOR_DROP_ENf,
                            0);
            soc_reg32_set(unit, COLOR_DROP_ENr,
                        port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : cpu port*/
    SOC_PBMP_ITER(pbmp_cpu, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker
                              - prefetch
                              + holcosminxqcnt0_holcosminxqcnt_cpu);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          shared_xqs_per_downlink_port  - skidmarker
                          - prefetch
                          + holcosminxqcnt7_holcosminxqcnt_cpu);
        soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                      port, 7, rval);

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker
                              - prefetch
                              + holcosminxqcnt0_holcosminxqcnt_cpu - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                          PKTRESETLIMITf,
                          shared_xqs_per_downlink_port  - skidmarker
                          - prefetch
                          + holcosminxqcnt7_holcosminxqcnt_cpu - 1);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, rval);

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          dynxqcntport_dynxqcntport_cpu - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf,
                              fl_ceiling_func(shared_space_cells,
                                              queue_port_limit_ratio)
                              + lwmcoscellsetlimit0_cellsetlimit_cpu);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                          CELLMAXLIMITf,
                          fl_ceiling_func(shared_space_cells,
                                       queue_port_limit_ratio) +
                          lwmcoscellsetlimit7_cellsetlimit_cpu);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, rval);

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              holcoscellmaxlimit0_cellmaxlimit_cpu -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                          CELLMAXRESUMELIMITf,
                          holcoscellmaxlimit7_cellmaxlimit_cpu -
                          ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                      port, 7, rval);

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          dyncelllimit_dyncellsetlimit_cpu -
                          ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7*/
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    return SOC_E_NONE;
}

STATIC int _soc_firelight_mmu_init_helper_standalone_lossless_pause(int unit)
{
    uint32 rval, temp_val;
    int port, phy_port, mmu_port;
    int index;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t pbmp_cpu;
    soc_pbmp_t pbmp_uplink;
    soc_pbmp_t pbmp_uplink_6kxq;
    soc_pbmp_t pbmp_uplink_2kxq;
    soc_pbmp_t pbmp_downlink;
    int speed;
    int standard_jumbo_frame;
    int cell_size;
    int ethernet_mtu_cell;
    int standard_jumbo_frame_cell;
    int total_physical_memory;
    int total_cell_memory_for_admission;
    int number_of_used_memory_banks;
    int reserved_for_cfap;
    int skidmarker;
    int prefetch;
    int total_cell_memory;
    int cfapfullsetpoint;
    int cfapfullresetpoint;
    int total_advertised_cell_memory;
    int numxqs_6kxq_uplink_ports;
    int numxqs_2kxq_uplink_ports;
    int numxqs_per_downlink_ports_and_cpu_port;
    int number_of_uplink_ports;
    int number_of_downlink_ports;
    int egress_queue_min_reserve_uplink_ports_lossy;
    int egress_queue_min_reserve_downlink_ports_lossy;
    int egress_queue_min_reserve_uplink_ports_lossless;
    int egress_queue_min_reserve_downlink_ports_lossless;
    int egress_queue_min_reserve_cpu_ports;
    int egress_xq_min_reserve_lossy_ports;
    int egress_xq_min_reserve_lossless_uplink_ports;
    int egress_xq_min_reserve_lossless_downlink_ports;
    int num_lossy_queues;
    int num_lossless_queues;
    int mmu_xoff_cell_threshold_1g_port_downlink_ports;
    int mmu_xoff_cell_threshold_2dot5g_port_downlink_ports;
    int mmu_xoff_cell_threshold_10g_port_downlink_ports;
    int mmu_xoff_cell_threshold_20g_port_downlink_ports;
    int mmu_xoff_cell_threshold_25g_port_downlink_ports;
    int mmu_xoff_cell_threshold_40g_port_downlink_ports;
    int mmu_xoff_cell_threshold_50g_port_downlink_ports;
    int mmu_xoff_cell_threshold_100g_port_downlink_ports;
    int mmu_xoff_cell_thresholds_per_port_10g_uplink_ports;
    int mmu_xoff_cell_thresholds_per_port_20g_uplink_ports;
    int mmu_xoff_cell_thresholds_per_port_25g_uplink_ports;
    int mmu_xoff_cell_thresholds_per_port_40g_uplink_ports;
    int mmu_xoff_cell_thresholds_per_port_50g_uplink_ports;
    int mmu_xoff_cell_thresholds_per_port_100g_uplink_ports;
    int num_cpu_queues;
    int num_10g_ports_uplink_ports;
    int num_20g_ports_uplink_ports;
    int num_25g_ports_uplink_ports;
    int num_40g_ports_uplink_ports;
    int num_50g_ports_uplink_ports;
    int num_100g_ports_uplink_ports;
    int num_1g_ports_downlink_ports;
    int num_2dot5g_ports_downlink_ports;
    int num_10g_ports_downlink_ports;
    int num_20g_ports_downlink_ports;
    int num_25g_ports_downlink_ports;
    int num_40g_ports_downlink_ports;
    int num_50g_ports_downlink_ports;
    int num_100g_ports_downlink_ports;
    int num_cpu_ports;
    int headroom_for_1g_port;
    int headroom_for_2dot5g_port;
    int headroom_for_10g_port;
    int headroom_for_20g_port;
    int headroom_for_25g_port;
    int headroom_for_40g_port;
    int headroom_for_50g_port;
    int headroom_for_100g_port;
    int xoff_cell_thresholds_per_port_1g_port_downlink_ports;
    int xoff_cell_thresholds_per_port_2dot5g_downlink_ports;
    int xoff_cell_thresholds_per_port_10g_downlink_ports;
    int xoff_cell_thresholds_per_port_20g_downlink_ports;
    int xoff_cell_thresholds_per_port_25g_downlink_ports;
    int xoff_cell_thresholds_per_port_40g_downlink_ports;
    int xoff_cell_thresholds_per_port_50g_downlink_ports;
    int xoff_cell_thresholds_per_port_100g_downlink_ports;
    int xoff_cell_threshold_all_downlink_ports;
    int xoff_cell_threshold_all_uplink_ports;
    int xoff_cell_thresholds_per_port_10g_uplink_ports;
    int xoff_cell_thresholds_per_port_20g_uplink_ports;
    int xoff_cell_thresholds_per_port_25g_uplink_ports;
    int xoff_cell_thresholds_per_port_40g_uplink_ports;
    int xoff_cell_thresholds_per_port_50g_uplink_ports;
    int xoff_cell_thresholds_per_port_100g_uplink_ports;
    int xoff_packet_thresholds_per_port_uplink_port;
    int discard_limit_per_port_pg_uplink_port;
    int discard_limit_per_port_pg_downlink_port;
    int total_reserved_cells_for_uplink_ports;
    int total_reserved_cells_for_downlink_ports;
    int total_reserved_cells_for_cpu_port;
    int total_reserved;
    int shared_space_cells;
    int reserved_xqs_per_uplink_port;
    int shared_xqs_per_6kxq_uplink_port;
    int shared_xqs_per_2kxq_uplink_port;
    int reserved_xqs_per_downlink_port;
    int shared_xqs_per_downlink_port;
    int gbllimitsetlimit_gblcellsetlimit_up;
    int cl_downlink_option = 0;
    int count_ge = 0, count_xl = 0;
    soc_pbmp_t pbmp_downlink_backplane;

    SOC_PBMP_COUNT(PBMP_GE_ALL(unit), count_ge);
    SOC_PBMP_COUNT(PBMP_XL_ALL(unit), count_xl);
    if ((count_ge + count_xl) == 0) {
        cl_downlink_option = 1;
        pbmp_downlink_backplane = soc_property_get_pbmp(unit,
                                              spn_DOWNLINK_BACKPLANE_PBMP, 0);
    }

    /* setup port bitmap according the port max speed for lossy
     *   TSC/TSCF    : uplink port
     *   QGMII/SGMII : downlink port
     */
    num_cpu_ports = 0;
    num_10g_ports_uplink_ports = 0;
    num_20g_ports_uplink_ports = 0;
    num_25g_ports_uplink_ports = 0;
    num_40g_ports_uplink_ports = 0;
    num_50g_ports_uplink_ports = 0;
    num_100g_ports_uplink_ports = 0;
    num_1g_ports_downlink_ports = 0;
    num_2dot5g_ports_downlink_ports = 0;
    num_10g_ports_downlink_ports = 0;
    num_20g_ports_downlink_ports = 0;
    num_25g_ports_downlink_ports = 0;
    num_40g_ports_downlink_ports = 0;
    num_50g_ports_downlink_ports = 0;
    num_100g_ports_downlink_ports = 0;
    number_of_uplink_ports = 0;
    number_of_downlink_ports = 0;
    SOC_PBMP_CLEAR(pbmp_cpu);
    SOC_PBMP_CLEAR(pbmp_uplink);
    SOC_PBMP_CLEAR(pbmp_uplink_6kxq);
    SOC_PBMP_CLEAR(pbmp_uplink_2kxq);
    SOC_PBMP_CLEAR(pbmp_downlink);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            continue; /* this user port has not been mapping in this sku */
        }
        if (IS_CPU_PORT(unit, port)) {
            num_cpu_ports++;
            SOC_PBMP_PORT_ADD(pbmp_cpu, port);
            continue;
        }
        speed = si->port_speed_max[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (speed <= 0) {
            continue; /* this user port has not been mapping in this sku */
        } else if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            continue; /* this user port has been masked out by pbmp_valid */
        }

        if (IS_CL_PORT(unit, port)  && cl_downlink_option &&
            !SOC_PBMP_MEMBER(pbmp_downlink_backplane, port)) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        }

        if (IS_CL_PORT(unit, port)  && cl_downlink_option &&
            (SOC_PBMP_MEMBER(pbmp_uplink, port))) {
            number_of_uplink_ports++;
            if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
            } else {
                SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
            }
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                number_of_uplink_ports++;
                if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
                } else {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
                }
            }
        } else {
            SOC_PBMP_PORT_ADD(pbmp_downlink, port);
            number_of_downlink_ports++;
            if (SOC_INFO(unit).port_init_speed[port] > 100000) {
                LOG_CLI((BSL_META_U(unit,
                        "for downlink port %d,\
                        the max speed cannot exceed 100G (value=%d)\n"),
                        port, SOC_INFO(unit).port_init_speed[port]));
                return SOC_E_PARAM;
            } else if (SOC_INFO(unit).port_init_speed[port] > 50000) {
                num_100g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 40000) {
                num_50g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 25000) {
                num_40g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 20000) {
                num_25g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 10000) {
                num_20g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 2500) {
                num_10g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_init_speed[port] > 1000) {
                num_2dot5g_ports_downlink_ports++;
            } else {
                num_1g_ports_downlink_ports++;
            }
        }
    }

    SOC_PBMP_ITER(pbmp_uplink, port) {
        if (SOC_INFO(unit).port_init_speed[port] > 100000) {
            LOG_CLI((BSL_META_U(unit,
                     "for backplane port %d,\
                     the max speed cannot exceed 100G (value=%d)\n"),
                     port, SOC_INFO(unit).port_init_speed[port]));
            return SOC_E_PARAM;
        } else if (SOC_INFO(unit).port_init_speed[port] > 50000) {
            num_100g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_init_speed[port] > 40000) {
            num_50g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_init_speed[port] > 25000) {
            num_40g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_init_speed[port] > 20000) {
            num_25g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_init_speed[port] > 10000) {
            num_20g_ports_uplink_ports++;
        } else {
            num_10g_ports_uplink_ports++;
        }
    }

    standard_jumbo_frame = 9216;
    cell_size = 144;
    ethernet_mtu_cell = fl_ceiling_func(15 * 1024 / 10, cell_size);
    standard_jumbo_frame_cell = fl_ceiling_func(standard_jumbo_frame, cell_size);
    total_physical_memory = 16 * 1024;
    total_cell_memory_for_admission = 14 * 1024;
    number_of_used_memory_banks = 8;
    reserved_for_cfap = (65) * 2 + number_of_used_memory_banks * 4;
    skidmarker = 7;
    prefetch = 64 + 4;
    total_cell_memory = total_cell_memory_for_admission;
    cfapfullsetpoint = total_physical_memory - reserved_for_cfap;
    cfapfullresetpoint = cfapfullsetpoint - standard_jumbo_frame_cell * 2;
    total_advertised_cell_memory = total_cell_memory;
    numxqs_6kxq_uplink_ports = 6 * 1024;
    numxqs_2kxq_uplink_ports = 2 * 1024;
    numxqs_per_downlink_ports_and_cpu_port = 2 * 1024;
    egress_queue_min_reserve_uplink_ports_lossy = 0;
    egress_queue_min_reserve_downlink_ports_lossy = 0;
    egress_queue_min_reserve_uplink_ports_lossless = ethernet_mtu_cell;
    egress_queue_min_reserve_downlink_ports_lossless = ethernet_mtu_cell;
    egress_queue_min_reserve_cpu_ports = ethernet_mtu_cell;
    egress_xq_min_reserve_lossy_ports = 0;
    egress_xq_min_reserve_lossless_uplink_ports
          = ethernet_mtu_cell;
    egress_xq_min_reserve_lossless_downlink_ports = ethernet_mtu_cell;
    num_lossy_queues = 0;
    num_lossless_queues = 8;
    mmu_xoff_cell_threshold_1g_port_downlink_ports = 30;
    mmu_xoff_cell_threshold_2dot5g_port_downlink_ports = 34;
    mmu_xoff_cell_threshold_10g_port_downlink_ports = 74;
    mmu_xoff_cell_threshold_20g_port_downlink_ports = 112;
    mmu_xoff_cell_threshold_25g_port_downlink_ports = 168;
    mmu_xoff_cell_threshold_40g_port_downlink_ports = 186;
    mmu_xoff_cell_threshold_50g_port_downlink_ports = 264;
    mmu_xoff_cell_threshold_100g_port_downlink_ports = 486;
    mmu_xoff_cell_thresholds_per_port_10g_uplink_ports = 74;
    mmu_xoff_cell_thresholds_per_port_20g_uplink_ports = 112;
    mmu_xoff_cell_thresholds_per_port_25g_uplink_ports = 168;
    mmu_xoff_cell_thresholds_per_port_40g_uplink_ports = 186;
    mmu_xoff_cell_thresholds_per_port_50g_uplink_ports = 264;
    mmu_xoff_cell_thresholds_per_port_100g_uplink_ports = 486;
    num_cpu_queues = 8;
    num_cpu_ports = 1;
    headroom_for_1g_port = 30;
    headroom_for_2dot5g_port = 34;
    headroom_for_10g_port = 74;
    headroom_for_20g_port = 112;
    headroom_for_25g_port = 168;
    headroom_for_40g_port = 186;
    headroom_for_50g_port = 264;
    headroom_for_100g_port = 486;
    xoff_cell_thresholds_per_port_1g_port_downlink_ports
          = mmu_xoff_cell_threshold_1g_port_downlink_ports;
    xoff_cell_thresholds_per_port_2dot5g_downlink_ports
          = mmu_xoff_cell_threshold_2dot5g_port_downlink_ports;
    xoff_cell_thresholds_per_port_10g_downlink_ports
          = mmu_xoff_cell_threshold_10g_port_downlink_ports;
    xoff_cell_thresholds_per_port_20g_downlink_ports
          = mmu_xoff_cell_threshold_20g_port_downlink_ports;
    xoff_cell_thresholds_per_port_25g_downlink_ports
          = mmu_xoff_cell_threshold_25g_port_downlink_ports;
    xoff_cell_thresholds_per_port_40g_downlink_ports
          = mmu_xoff_cell_threshold_40g_port_downlink_ports;
    xoff_cell_thresholds_per_port_50g_downlink_ports
          = mmu_xoff_cell_threshold_50g_port_downlink_ports;
    xoff_cell_thresholds_per_port_100g_downlink_ports
          = mmu_xoff_cell_threshold_100g_port_downlink_ports;
    xoff_packet_thresholds_per_port_uplink_port = 11;
    xoff_cell_thresholds_per_port_10g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_10g_uplink_ports;
    xoff_cell_thresholds_per_port_20g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_20g_uplink_ports;
    xoff_cell_thresholds_per_port_25g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_25g_uplink_ports;
    xoff_cell_thresholds_per_port_40g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_40g_uplink_ports;
    xoff_cell_thresholds_per_port_50g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_50g_uplink_ports;
    xoff_cell_thresholds_per_port_100g_uplink_ports =
    mmu_xoff_cell_thresholds_per_port_100g_uplink_ports;
    xoff_cell_threshold_all_downlink_ports = total_advertised_cell_memory;
    xoff_cell_threshold_all_uplink_ports = total_advertised_cell_memory;
    discard_limit_per_port_pg_downlink_port = total_advertised_cell_memory;
    discard_limit_per_port_pg_uplink_port = total_advertised_cell_memory;
    total_reserved_cells_for_uplink_ports
        = egress_queue_min_reserve_uplink_ports_lossy
          * number_of_uplink_ports * num_lossy_queues
          + number_of_uplink_ports * egress_queue_min_reserve_uplink_ports_lossless
          * num_lossless_queues;
    total_reserved_cells_for_downlink_ports
        = number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossy * num_lossy_queues
          + number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossless
          * num_lossless_queues;
    total_reserved_cells_for_cpu_port
        = num_cpu_ports * egress_queue_min_reserve_cpu_ports
          * num_cpu_queues;
    total_reserved = total_reserved_cells_for_uplink_ports
        + total_reserved_cells_for_downlink_ports
        + total_reserved_cells_for_cpu_port;
    shared_space_cells = total_advertised_cell_memory - total_reserved;
    reserved_xqs_per_uplink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_uplink_ports
          * num_lossless_queues;
    shared_xqs_per_6kxq_uplink_port
          = numxqs_6kxq_uplink_ports - reserved_xqs_per_uplink_port;
    shared_xqs_per_2kxq_uplink_port
          = numxqs_2kxq_uplink_ports - reserved_xqs_per_uplink_port;
    reserved_xqs_per_downlink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_downlink_ports
          * num_lossless_queues;
    shared_xqs_per_downlink_port
        = numxqs_per_downlink_ports_and_cpu_port
          - reserved_xqs_per_downlink_port;
    gbllimitsetlimit_gblcellsetlimit_up = total_cell_memory_for_admission;

    if ((shared_space_cells * cell_size)/1024 <= 800) {
        LOG_CLI((BSL_META_U(unit,
                 "Shared Pool Is Small,\
                 should be larger than 800 (value=%d)\n"),
                 (shared_space_cells * cell_size)/1024));
        return SOC_E_PARAM;
    }

    /* system-based */
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLSETPOINTf,
                           cfapfullsetpoint);
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLRESETPOINTf,
                           cfapfullresetpoint);
    soc_reg_field32_modify(unit, GBLLIMITSETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLSETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, GBLLIMITRESETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLRESETLIMITf,
                           gbllimitsetlimit_gblcellsetlimit_up);
    soc_reg_field32_modify(unit, TOTALDYNCELLSETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLSETLIMITf,
                           shared_space_cells);
    soc_reg_field32_modify(unit, TOTALDYNCELLRESETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLRESETLIMITf,
                           shared_space_cells -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, TWO_LAYER_SCH_MODEr,
                           REG_PORT_ANY,
                           SCH_MODEf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           MULTIPLE_ACCOUNTING_FIX_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           CNG_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYN_XQ_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           HOL_CELL_SOP_DROP_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYNAMIC_MEMORY_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           SKIDMARKERf,
                           3);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_0r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_1r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_2r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           3);

    soc_reg32_set(unit, E2ECC_MODEr, REG_PORT_ANY, 0, 0);
    soc_reg32_set(unit, E2ECC_HOL_ENr, REG_PORT_ANY, 0, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    /* E2ECC_TX_ENABLE_BMPr, index 0 ~ 7 */
    for (index = 0; index <= 7; index++) {
        soc_reg32_set(unit, E2ECC_TX_ENABLE_BMPr, REG_PORT_ANY, index, 0);
    }
    soc_reg32_set(unit, E2ECC_TX_PORTS_NUMr, REG_PORT_ANY, 0, 0);

    /* port-based : uplink 6kxq*/
    SOC_PBMP_ITER(pbmp_uplink_6kxq, port) {
        speed = si->port_init_speed[port];
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0x80);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }
        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        if (speed == 10000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_10g_uplink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_20g_uplink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_25g_uplink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_40g_uplink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_50g_uplink_ports);
        } else{
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                PKTSETLIMITf, xoff_cell_thresholds_per_port_100g_uplink_ports);
        }
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }
        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        if (speed == 10000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_10g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_10g_uplink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_20g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_20g_uplink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_25g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_25g_uplink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_40g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_40g_uplink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_50g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_50g_uplink_ports);
        } else{
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_100g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_100g_uplink_ports);
        }
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_uplink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_uplink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossless_uplink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossless_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossless);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              0);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossless);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              0);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        temp_val =
        (xoff_cell_thresholds_per_port_1g_port_downlink_ports + headroom_for_1g_port) *
        num_1g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_2dot5g_downlink_ports + headroom_for_2dot5g_port) *
        num_2dot5g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_downlink_ports + headroom_for_10g_port) *
        num_10g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_20g_downlink_ports + headroom_for_20g_port) *
        num_20g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_25g_downlink_ports + headroom_for_25g_port) *
        num_25g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_40g_downlink_ports + headroom_for_40g_port) *
        num_40g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_50g_downlink_ports + headroom_for_50g_port) *
        num_50g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_100g_downlink_ports + headroom_for_100g_port) *
        num_100g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_uplink_ports + headroom_for_10g_port) *
        num_10g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_20g_uplink_ports + headroom_for_20g_port) *
        num_20g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_25g_uplink_ports + headroom_for_25g_port) *
        num_25g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_40g_uplink_ports + headroom_for_40g_port) *
        num_40g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_50g_uplink_ports + headroom_for_50g_port) *
        num_50g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_100g_uplink_ports + headroom_for_100g_port) *
        num_100g_ports_uplink_ports;
        temp_val = fl_ceiling_func(temp_val, 1);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, temp_val);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf,
                            0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                        port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf,
                              (total_advertised_cell_memory - 1));
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : uplink 2kxq*/
    SOC_PBMP_ITER(pbmp_uplink_2kxq, port) {
        speed = si->port_init_speed[port];
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0x80);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }
        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        if (speed == 10000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_10g_uplink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_20g_uplink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_25g_uplink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_40g_uplink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_50g_uplink_ports);
        } else{
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_100g_uplink_ports);
        }
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }
        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        if (speed == 10000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_10g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_10g_uplink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_20g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_20g_uplink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_25g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_25g_uplink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_40g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_40g_uplink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_50g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_50g_uplink_ports);
        } else{
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_100g_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_100g_uplink_ports);
        }
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_uplink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_uplink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_2kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossless_uplink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_2kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossless_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_2kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_2kxq_uplink_ports - 1);

            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_2kxq_uplink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_2kxq_uplink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossless);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossless);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
        temp_val =
        (xoff_cell_thresholds_per_port_1g_port_downlink_ports + headroom_for_1g_port) *
        num_1g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_2dot5g_downlink_ports + headroom_for_2dot5g_port) *
        num_2dot5g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_downlink_ports + headroom_for_10g_port) *
        num_10g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_20g_downlink_ports + headroom_for_20g_port) *
        num_20g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_25g_downlink_ports + headroom_for_25g_port) *
        num_25g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_40g_downlink_ports + headroom_for_40g_port) *
        num_40g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_50g_downlink_ports + headroom_for_50g_port) *
        num_50g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_100g_downlink_ports + headroom_for_100g_port) *
        num_100g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_uplink_ports + headroom_for_10g_port) *
        num_10g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_20g_uplink_ports + headroom_for_20g_port) *
        num_20g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_25g_uplink_ports + headroom_for_25g_port) *
        num_25g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_40g_uplink_ports + headroom_for_40g_port) *
        num_40g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_50g_uplink_ports + headroom_for_50g_port) *
        num_50g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_100g_uplink_ports + headroom_for_100g_port) *
        num_100g_ports_uplink_ports;
        temp_val = fl_ceiling_func(temp_val, 1);
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, temp_val);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }


    /* port-based : downlink */
    SOC_PBMP_ITER(pbmp_downlink, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        speed = si->port_init_speed[port];

        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0x80);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }
        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        if (speed == 1000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_1g_port_downlink_ports);
        } else if (speed == 2500) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_2dot5g_downlink_ports);
        } else if (speed == 10000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_10g_downlink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_20g_downlink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_25g_downlink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_40g_downlink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_50g_downlink_ports);
        } else {
            soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf,
                          xoff_cell_thresholds_per_port_100g_downlink_ports);
        }
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }
        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        if (speed == 1000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_1g_port_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_1g_port_downlink_ports);
        } else if (speed == 2500) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_2dot5g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_2dot5g_downlink_ports);
        } else if (speed == 10000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_10g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_10g_downlink_ports);
        } else if (speed == 20000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_20g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_20g_downlink_ports);
        } else if (speed == 25000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_25g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_25g_downlink_ports);
        } else if (speed == 40000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_40g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_40g_downlink_ports);
        } else if (speed == 50000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_50g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_50g_downlink_ports);
        } else{
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_100g_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_100g_downlink_ports);
        }
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_uplink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossless_uplink_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker - prefetch
                                + egress_xq_min_reserve_lossless_uplink_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker -
                                prefetch);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker - prefetch
                                + egress_xq_min_reserve_lossless_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf,
                                  shared_xqs_per_downlink_port  - skidmarker -
                                  prefetch - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                              port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossless_uplink_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNTr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker - prefetch
                                + egress_xq_min_reserve_lossless_uplink_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker - prefetch
                                + egress_xq_min_reserve_lossless_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);

                soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                            port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_uplink_ports_lossless);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_uplink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            temp_val =
            (xoff_cell_thresholds_per_port_1g_port_downlink_ports + headroom_for_1g_port) *
            num_1g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_2dot5g_downlink_ports + headroom_for_2dot5g_port) *
            num_2dot5g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_10g_downlink_ports + headroom_for_10g_port) *
            num_10g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_20g_downlink_ports + headroom_for_20g_port) *
            num_20g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_25g_downlink_ports + headroom_for_25g_port) *
            num_25g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_40g_downlink_ports + headroom_for_40g_port) *
            num_40g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_50g_downlink_ports + headroom_for_50g_port) *
            num_50g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_100g_downlink_ports + headroom_for_100g_port) *
            num_100g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_10g_uplink_ports + headroom_for_10g_port) *
            num_10g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_20g_uplink_ports + headroom_for_20g_port) *
            num_20g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_25g_uplink_ports + headroom_for_25g_port) *
            num_25g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_40g_uplink_ports + headroom_for_40g_port) *
            num_40g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_50g_uplink_ports + headroom_for_50g_port) *
            num_50g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_100g_uplink_ports + headroom_for_100g_port) *
            num_100g_ports_uplink_ports;
            temp_val = fl_ceiling_func(temp_val, 1);
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, temp_val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  temp_val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_uplink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_uplink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            temp_val =
            (xoff_cell_thresholds_per_port_1g_port_downlink_ports + headroom_for_1g_port) *
            num_1g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_2dot5g_downlink_ports + headroom_for_2dot5g_port) *
            num_2dot5g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_10g_downlink_ports + headroom_for_10g_port) *
            num_10g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_20g_downlink_ports + headroom_for_20g_port) *
            num_20g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_25g_downlink_ports + headroom_for_25g_port) *
            num_25g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_40g_downlink_ports + headroom_for_40g_port) *
            num_40g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_50g_downlink_ports + headroom_for_50g_port) *
            num_50g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_100g_downlink_ports + headroom_for_100g_port) *
            num_100g_ports_downlink_ports +
            (xoff_cell_thresholds_per_port_10g_uplink_ports + headroom_for_10g_port) *
            num_10g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_20g_uplink_ports + headroom_for_20g_port) *
            num_20g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_25g_uplink_ports + headroom_for_25g_port) *
            num_25g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_40g_uplink_ports + headroom_for_40g_port) *
            num_40g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_50g_uplink_ports + headroom_for_50g_port) *
            num_50g_ports_uplink_ports +
            (xoff_cell_thresholds_per_port_100g_uplink_ports + headroom_for_100g_port) *
            num_100g_ports_uplink_ports;
            temp_val = fl_ceiling_func(temp_val, 1);
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, temp_val);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf,
                                temp_val -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                PKTSETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                PKTRESETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXLIMITf,
                                (total_advertised_cell_memory - 1));
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXRESUMELIMITf,
                                total_advertised_cell_memory - 1 -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr,
                          port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }


    /* port-based : cpu port*/
    SOC_PBMP_ITER(pbmp_cpu, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker - prefetch
                              + egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker - prefetch
                              + egress_queue_min_reserve_cpu_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        temp_val =
        (xoff_cell_thresholds_per_port_1g_port_downlink_ports + headroom_for_1g_port) *
        num_1g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_2dot5g_downlink_ports + headroom_for_2dot5g_port) *
        num_2dot5g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_downlink_ports + headroom_for_10g_port) *
        num_10g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_20g_downlink_ports + headroom_for_20g_port) *
        num_20g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_25g_downlink_ports + headroom_for_25g_port) *
        num_25g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_40g_downlink_ports + headroom_for_40g_port) *
        num_40g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_50g_downlink_ports + headroom_for_50g_port) *
        num_50g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_100g_downlink_ports + headroom_for_100g_port) *
        num_100g_ports_downlink_ports +
        (xoff_cell_thresholds_per_port_10g_uplink_ports + headroom_for_10g_port) *
        num_10g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_20g_uplink_ports + headroom_for_20g_port) *
        num_20g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_25g_uplink_ports + headroom_for_25g_port) *
        num_25g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_40g_uplink_ports + headroom_for_40g_port) *
        num_40g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_50g_uplink_ports + headroom_for_50g_port) *
        num_50g_ports_uplink_ports +
        (xoff_cell_thresholds_per_port_100g_uplink_ports + headroom_for_100g_port) *
        num_100g_ports_uplink_ports;
        temp_val = fl_ceiling_func(temp_val, 1);
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, temp_val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    return SOC_E_NONE;
}

STATIC int _soc_firelight_mmu_init_helper_standalone_lossless_pfc(int unit)
{
    uint32 rval, temp_val;
    int port, phy_port, mmu_port;
    int index;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t pbmp_cpu;
    soc_pbmp_t pbmp_uplink;
    soc_pbmp_t pbmp_downlink;
    soc_pbmp_t pbmp_downlink_lossy;
    soc_pbmp_t pbmp_downlink_lossless;
    int speed;
    int standard_jumbo_frame;
    int cell_size;
    int ethernet_mtu_cell;
    int standard_jumbo_frame_cell;
    int total_physical_memory;
    int total_cell_memory_for_admission;
    int number_of_used_memory_banks;
    int reserved_for_cfap;
    int skidmarker;
    int prefetch;
    int total_cell_memory;
    int cfapfullsetpoint;
    int cfapfullresetpoint;
    int total_advertised_cell_memory;
    int numxqs_6kxq_uplink_ports;
    int numxqs_per_downlink_ports_and_cpu_port;
    int number_of_uplink_ports;
    int number_of_lossless_downlink_ports;
    int number_of_downlink_ports;
    int queue_port_limit_ratio;
    int egress_queue_min_reserve_uplink_ports_lossy;
    int egress_queue_min_reserve_downlink_ports_lossy;
    int egress_queue_min_reserve_uplink_ports_lossless;
    int egress_queue_min_reserve_downlink_ports_lossless;
    int egress_queue_min_reserve_cpu_ports;
    int egress_xq_min_reserve_lossy_ports;
    int egress_xq_min_reserve_lossless_uplink_ports;
    int egress_xq_min_reserve_lossless_downlink_ports;
    int num_lossy_queues;
    int num_lossless_queues;
    int mmu_xoff_pkt_threshold_uplink_ports;
    int mmu_xoff_pkt_threshold_downlink_ports;
    int mmu_xoff_cell_threshold_1g_port_downlink_ports;
    int mmu_xoff_cell_threshold_2dot5g_port_downlink_ports;
    int mmu_xoff_cell_threshold_10g_port_downlink_ports;
    int mmu_xoff_cell_threshold_20g_port_downlink_ports;
    int mmu_xoff_cell_threshold_25g_port_downlink_ports;
    int mmu_xoff_cell_threshold_40g_port_downlink_ports;
    int num_cpu_queues;
    int num_10g_ports_uplink_ports;
    int num_20g_ports_uplink_ports;
    int num_25g_ports_uplink_ports;
    int num_40g_ports_uplink_ports;
    int num_50g_ports_uplink_ports;
    int num_100g_ports_uplink_ports;
    int num_1g_ports_downlink_ports;
    int num_2dot5g_ports_downlink_ports;
    int num_10g_ports_downlink_ports;
    int num_20g_ports_downlink_ports;
    int num_25g_ports_downlink_ports;
    int num_40g_ports_downlink_ports;
    int num_cpu_ports;
    int headroom_for_1g_port;
    int headroom_for_2dot5g_port;
    int headroom_for_10g_port;
    int headroom_for_20g_port;
    int headroom_for_25g_port;
    int headroom_for_40g_port;
    int xoff_cell_thresholds_per_port_1g_port_downlink_ports;
    int xoff_cell_thresholds_per_port_2dot5g_port_downlink_ports;
    int xoff_cell_thresholds_per_port_10g_port_downlink_ports;
    int xoff_cell_threshold_all_downlink_ports;
    int xoff_cell_threshold_all_uplink_ports;
    int xoff_packet_thresholds_per_port_uplink_port;
    int xoff_packet_thresholds_per_port_downlink_port;
    int discard_limit_per_port_pg_downlink_1g_port;
    int discard_limit_per_port_pg_downlink_2dot5g_port;
    int discard_limit_per_port_pg_downlink_10g_port;
    int discard_limit_per_port_pg_uplink_port;
    int discard_limit_per_port_pg_downlink_port;
    int total_reserved_cells_for_uplink_ports;
    int total_reserved_cells_for_downlink_ports;
    int total_reserved_cells_for_cpu_port;
    int total_reserved;
    int shared_space_cells;
    int reserved_xqs_per_uplink_port;
    int shared_xqs_per_6kxq_uplink_port;
    int reserved_xqs_per_downlink_port;
    int shared_xqs_per_downlink_port;
    int gbllimitsetlimit_gblcellsetlimit_up;
    int sum_40g_val;
    int sum_25g_val;
    int sum_20g_val;
    int sum_10g_val;
    int sum_2dot5g_val;
    int sum_1g_val;
    int temp_25g_val;
    int temp_20g_val;
    int temp_10g_val;
    int temp_2dot5g_val;
    int temp_1g_val;
    int remind_num_25g_ports_downlink_ports;
    int remind_num_20g_ports_downlink_ports;
    int remind_num_10g_ports_downlink_ports;
    int remind_num_2dot5g_ports_downlink_ports;
    int remind_num_1g_ports_downlink_ports;
    int cl_downlink_option = 0;
    int count_ge = 0, count_xl = 0;
    soc_pbmp_t pbmp_downlink_backplane;

    SOC_PBMP_COUNT(PBMP_GE_ALL(unit), count_ge);
    SOC_PBMP_COUNT(PBMP_XL_ALL(unit), count_xl);
    if ((count_ge + count_xl) == 0) {
        cl_downlink_option = 1;
        pbmp_downlink_backplane = soc_property_get_pbmp(unit,
                                              spn_DOWNLINK_BACKPLANE_PBMP, 0);
    }

    /* setup port bitmap according the port max speed for lossy
     *   TSC/TSCF    : uplink port
     *   QGMII/SGMII : downlink port
     */
    num_cpu_ports = 0;
    num_10g_ports_uplink_ports = 0;
    num_20g_ports_uplink_ports = 0;
    num_25g_ports_uplink_ports = 0;
    num_40g_ports_uplink_ports = 0;
    num_50g_ports_uplink_ports = 0;
    num_100g_ports_uplink_ports = 0;
    num_1g_ports_downlink_ports = 0;
    num_2dot5g_ports_downlink_ports = 0;
    num_10g_ports_downlink_ports = 0;
    num_20g_ports_downlink_ports = 0;
    num_25g_ports_downlink_ports = 0;
    num_40g_ports_downlink_ports = 0;
    number_of_uplink_ports = 0;
    number_of_downlink_ports = 0;
    number_of_lossless_downlink_ports = 0;
    SOC_PBMP_CLEAR(pbmp_cpu);
    SOC_PBMP_CLEAR(pbmp_uplink);
    SOC_PBMP_CLEAR(pbmp_downlink);
    SOC_PBMP_CLEAR(pbmp_downlink_lossy);
    SOC_PBMP_CLEAR(pbmp_downlink_lossless);

    pbmp_downlink_lossless = soc_property_get_pbmp(unit, spn_MMU_LOSSLESS_PBMP, 0);
    SOC_PBMP_COUNT(pbmp_downlink_lossless, temp_val);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            continue; /* this user port has not been mapping in this sku */
        }
        if (IS_CPU_PORT(unit, port)) {
            num_cpu_ports++;
            SOC_PBMP_PORT_ADD(pbmp_cpu, port);
            continue;
        }
        speed = si->port_speed_max[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (speed <= 0) {
            continue; /* this user port has not been mapping in this sku */
        } else if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            continue; /* this user port has been masked out by pbmp_valid */
        }

        if (IS_CL_PORT(unit, port)  && cl_downlink_option &&
            !SOC_PBMP_MEMBER(pbmp_downlink_backplane, port)) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        }

        if (IS_CL_PORT(unit, port)  && cl_downlink_option &&
            SOC_PBMP_MEMBER(pbmp_uplink, port)) {
            number_of_uplink_ports++;
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                number_of_uplink_ports++;
            }
        } else {
            SOC_PBMP_PORT_ADD(pbmp_downlink, port);
            number_of_downlink_ports++;
            if (temp_val == 0) {
                /* While no specify pbmp_downlink_lossless,
                 * pbmp_downlink_lossless = pbmp_downlink
                 */
                 SOC_PBMP_PORT_ADD(pbmp_downlink_lossless, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_downlink_lossless, port)) {
                number_of_lossless_downlink_ports++;
                if (SOC_INFO(unit).port_speed_max[port] > 10000) {
                    LOG_CLI((BSL_META_U(unit,
                            "for downlink lossless port %d,\
                            the max speed cannot exceed 10G (value=%d)\n"),
                            port, SOC_INFO(unit).port_speed_max[port]));
                    return SOC_E_PARAM;
                } else if (SOC_INFO(unit).port_speed_max[port] > 2500) {
                    num_10g_ports_downlink_ports++;
                } else if (SOC_INFO(unit).port_speed_max[port] > 1000) {
                    num_2dot5g_ports_downlink_ports++;
                } else {
                    num_1g_ports_downlink_ports++;
                }
            } else {
                /* lossly port */
                SOC_PBMP_PORT_ADD(pbmp_downlink_lossy, port);
            }
        }
    }

    if (number_of_uplink_ports > 6) {
        LOG_CLI((BSL_META_U(unit,
                 "Num of Uplink Ports Error,\
                  should be less than or equal to 6 (value=%d)\n"),
                 number_of_uplink_ports));
        return SOC_E_PARAM;
    }

    if (number_of_lossless_downlink_ports > 24) {
        LOG_CLI((BSL_META_U(unit,
                 "Num of Lossless Downlink Ports Error,\
                  should be less than or equal to 24 (value=%d)\n"),
                 number_of_lossless_downlink_ports));
        return SOC_E_PARAM;
    }

    SOC_PBMP_ITER(pbmp_uplink, port) {
        speed = si->port_speed_max[port];
        if (SOC_INFO(unit).port_speed_max[port] > 100000) {
            LOG_CLI((BSL_META_U(unit,
                     "for backplane port %d,\
                     the max speed cannot exceed 100G (value=%d)\n"),
                     port, SOC_INFO(unit).port_speed_max[port]));
            return SOC_E_PARAM;
        } else if (SOC_INFO(unit).port_speed_max[port] > 50000) {
            num_100g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 40000) {
            num_50g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 25000) {
            num_40g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 20000) {
            num_25g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 10000) {
            num_20g_ports_uplink_ports++;
        } else {
            num_10g_ports_uplink_ports++;
        }
    }

    standard_jumbo_frame = 9216;
    cell_size = 144;
    ethernet_mtu_cell = fl_ceiling_func(15 * 1024 / 10, cell_size);
    standard_jumbo_frame_cell = fl_ceiling_func(standard_jumbo_frame, cell_size);
    total_physical_memory = 16 * 1024;
    total_cell_memory_for_admission = 14 * 1024;
    number_of_used_memory_banks = 8;
    reserved_for_cfap = (65) * 2 + number_of_used_memory_banks * 4;
    skidmarker = 7;
    prefetch = 64 + 4;
    total_cell_memory = total_cell_memory_for_admission;
    cfapfullsetpoint = total_physical_memory - reserved_for_cfap;
    cfapfullresetpoint = cfapfullsetpoint - standard_jumbo_frame_cell * 2;
    total_advertised_cell_memory = total_cell_memory;
    numxqs_6kxq_uplink_ports = 6 * 1024;
    numxqs_per_downlink_ports_and_cpu_port = 2 * 1024;
    queue_port_limit_ratio = 8;
    egress_queue_min_reserve_uplink_ports_lossy = ethernet_mtu_cell;
    egress_queue_min_reserve_downlink_ports_lossy = ethernet_mtu_cell;
    egress_queue_min_reserve_downlink_ports_lossless = 0;
    egress_queue_min_reserve_cpu_ports = ethernet_mtu_cell;
    egress_xq_min_reserve_lossy_ports = ethernet_mtu_cell;
    egress_xq_min_reserve_lossless_downlink_ports = 0;
    num_lossless_queues = 1;
    num_lossy_queues = 7;
    mmu_xoff_pkt_threshold_uplink_ports = total_advertised_cell_memory;
    mmu_xoff_pkt_threshold_downlink_ports = total_advertised_cell_memory;
    mmu_xoff_cell_threshold_1g_port_downlink_ports = 30;
    mmu_xoff_cell_threshold_2dot5g_port_downlink_ports = 34;
    mmu_xoff_cell_threshold_10g_port_downlink_ports = 74;
    mmu_xoff_cell_threshold_20g_port_downlink_ports = 112;
    mmu_xoff_cell_threshold_25g_port_downlink_ports = 168;
    mmu_xoff_cell_threshold_40g_port_downlink_ports = 186;
    num_cpu_queues = 8;
    num_cpu_ports = 1;
    headroom_for_1g_port = 30;
    headroom_for_2dot5g_port = 34;
    headroom_for_10g_port = 74;
    headroom_for_20g_port = 112;
    headroom_for_25g_port = 168;
    headroom_for_40g_port = 186;
    remind_num_25g_ports_downlink_ports = number_of_lossless_downlink_ports - num_40g_ports_downlink_ports;
    remind_num_20g_ports_downlink_ports = remind_num_25g_ports_downlink_ports - num_25g_ports_downlink_ports;
    remind_num_10g_ports_downlink_ports = remind_num_20g_ports_downlink_ports - num_20g_ports_downlink_ports;
    remind_num_2dot5g_ports_downlink_ports = remind_num_10g_ports_downlink_ports - num_10g_ports_downlink_ports;
    remind_num_1g_ports_downlink_ports = remind_num_2dot5g_ports_downlink_ports - num_2dot5g_ports_downlink_ports;
    sum_40g_val = mmu_xoff_cell_threshold_40g_port_downlink_ports + headroom_for_40g_port;
    sum_25g_val = mmu_xoff_cell_threshold_25g_port_downlink_ports + headroom_for_25g_port;
    sum_20g_val = mmu_xoff_cell_threshold_20g_port_downlink_ports + headroom_for_20g_port;
    sum_10g_val = mmu_xoff_cell_threshold_10g_port_downlink_ports + headroom_for_10g_port;
    sum_2dot5g_val = mmu_xoff_cell_threshold_2dot5g_port_downlink_ports + headroom_for_2dot5g_port;
    sum_1g_val = mmu_xoff_cell_threshold_1g_port_downlink_ports + headroom_for_1g_port;

    if (remind_num_1g_ports_downlink_ports < 0) {
        remind_num_1g_ports_downlink_ports = 0;
    }
    if (num_1g_ports_downlink_ports > remind_num_1g_ports_downlink_ports) {
        temp_1g_val = remind_num_1g_ports_downlink_ports * sum_1g_val;
    } else {
        temp_1g_val = num_1g_ports_downlink_ports * sum_1g_val;
    }

    if (remind_num_2dot5g_ports_downlink_ports < 0) {
        remind_num_2dot5g_ports_downlink_ports = 0;
    }

    if (num_2dot5g_ports_downlink_ports > remind_num_2dot5g_ports_downlink_ports) {
        temp_2dot5g_val = remind_num_2dot5g_ports_downlink_ports * sum_2dot5g_val;
    } else {
        temp_2dot5g_val = num_2dot5g_ports_downlink_ports * sum_2dot5g_val+temp_1g_val;
    }

    if (remind_num_10g_ports_downlink_ports < 0) {
        remind_num_10g_ports_downlink_ports = 0;
    }

    if (num_10g_ports_downlink_ports > remind_num_10g_ports_downlink_ports) {
        temp_10g_val = remind_num_10g_ports_downlink_ports * sum_10g_val;
    } else {
        temp_10g_val = num_10g_ports_downlink_ports *  sum_10g_val + temp_2dot5g_val;
    }

    if (remind_num_20g_ports_downlink_ports < 0) {
        remind_num_20g_ports_downlink_ports = 0;
    }

    if (num_20g_ports_downlink_ports > remind_num_20g_ports_downlink_ports) {
        temp_20g_val = remind_num_20g_ports_downlink_ports * sum_20g_val;
    } else {
        temp_20g_val = num_20g_ports_downlink_ports * sum_20g_val + temp_10g_val;
    }

    if (remind_num_25g_ports_downlink_ports < 0) {
        remind_num_25g_ports_downlink_ports = 0;
    }

    if (num_25g_ports_downlink_ports > remind_num_25g_ports_downlink_ports) {
        temp_25g_val = remind_num_25g_ports_downlink_ports * sum_25g_val;
    } else {
        temp_25g_val = num_25g_ports_downlink_ports * sum_25g_val + temp_20g_val;
    }

    if (num_40g_ports_downlink_ports > number_of_lossless_downlink_ports) {
        egress_xq_min_reserve_lossless_uplink_ports
            = number_of_lossless_downlink_ports * sum_40g_val;
    } else {
        egress_xq_min_reserve_lossless_uplink_ports
            = num_40g_ports_downlink_ports * sum_40g_val + temp_25g_val;
    }
    egress_queue_min_reserve_uplink_ports_lossless = egress_xq_min_reserve_lossless_uplink_ports;

    xoff_cell_thresholds_per_port_1g_port_downlink_ports
          = mmu_xoff_cell_threshold_1g_port_downlink_ports;
    xoff_cell_thresholds_per_port_2dot5g_port_downlink_ports
          = mmu_xoff_cell_threshold_2dot5g_port_downlink_ports;
    xoff_cell_thresholds_per_port_10g_port_downlink_ports
          = mmu_xoff_cell_threshold_10g_port_downlink_ports;
    xoff_packet_thresholds_per_port_uplink_port = mmu_xoff_pkt_threshold_uplink_ports;
    xoff_packet_thresholds_per_port_downlink_port = mmu_xoff_pkt_threshold_downlink_ports;
    discard_limit_per_port_pg_downlink_1g_port
          = xoff_cell_thresholds_per_port_1g_port_downlink_ports +
            headroom_for_1g_port;
    discard_limit_per_port_pg_downlink_2dot5g_port
          = xoff_cell_thresholds_per_port_2dot5g_port_downlink_ports +
            headroom_for_2dot5g_port;
    discard_limit_per_port_pg_downlink_10g_port
          = xoff_cell_thresholds_per_port_10g_port_downlink_ports +
            headroom_for_10g_port;
    xoff_cell_threshold_all_downlink_ports = total_advertised_cell_memory;
    xoff_cell_threshold_all_uplink_ports = total_advertised_cell_memory;
    discard_limit_per_port_pg_downlink_port = total_advertised_cell_memory;
    discard_limit_per_port_pg_uplink_port = total_advertised_cell_memory;
    total_reserved_cells_for_uplink_ports
        = egress_queue_min_reserve_uplink_ports_lossy
          * number_of_uplink_ports * num_lossy_queues
          + number_of_uplink_ports * egress_queue_min_reserve_uplink_ports_lossless
          * num_lossless_queues;
    total_reserved_cells_for_downlink_ports
        = number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossy * num_lossy_queues
          + number_of_downlink_ports
          * egress_queue_min_reserve_downlink_ports_lossless
          * num_lossless_queues;
    total_reserved_cells_for_cpu_port
        = num_cpu_ports * egress_queue_min_reserve_cpu_ports
          * num_cpu_queues;
    total_reserved = total_reserved_cells_for_uplink_ports
        + total_reserved_cells_for_downlink_ports
        + total_reserved_cells_for_cpu_port;
    shared_space_cells = total_advertised_cell_memory - total_reserved;
    reserved_xqs_per_uplink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_uplink_ports
          * num_lossless_queues;
    shared_xqs_per_6kxq_uplink_port
          = numxqs_6kxq_uplink_ports - reserved_xqs_per_uplink_port;
    reserved_xqs_per_downlink_port
        = egress_xq_min_reserve_lossy_ports
          * num_lossy_queues + egress_xq_min_reserve_lossless_downlink_ports
          * num_lossless_queues;
    shared_xqs_per_downlink_port
        = numxqs_per_downlink_ports_and_cpu_port
          - reserved_xqs_per_downlink_port;
    gbllimitsetlimit_gblcellsetlimit_up = total_cell_memory_for_admission;

    if ((shared_space_cells * cell_size)/1024 <= 500) {
        LOG_CLI((BSL_META_U(unit,
                 "Shared Pool Is Small,\
                 should be larger than 500 (value=%d)\n"),
                 (shared_space_cells * cell_size)/1024));
        return SOC_E_PARAM;
    }

    /* system-based */
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLSETPOINTf,
                           cfapfullsetpoint);
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLRESETPOINTf,
                           cfapfullresetpoint);
    soc_reg_field32_modify(unit, GBLLIMITSETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLSETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, GBLLIMITRESETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLRESETLIMITf,
                           gbllimitsetlimit_gblcellsetlimit_up);
    soc_reg_field32_modify(unit, TOTALDYNCELLSETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLSETLIMITf,
                           shared_space_cells);
    soc_reg_field32_modify(unit, TOTALDYNCELLRESETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLRESETLIMITf,
                           shared_space_cells -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, TWO_LAYER_SCH_MODEr,
                           REG_PORT_ANY,
                           SCH_MODEf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           MULTIPLE_ACCOUNTING_FIX_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           CNG_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYN_XQ_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           HOL_CELL_SOP_DROP_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYNAMIC_MEMORY_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           SKIDMARKERf,
                           3);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_0r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_1r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_2r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           3);

    soc_reg32_set(unit, E2ECC_MODEr, REG_PORT_ANY, 0, 0);
    soc_reg32_set(unit, E2ECC_HOL_ENr, REG_PORT_ANY, 0, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    /* E2ECC_TX_ENABLE_BMPr, index 0 ~ 7 */
    for (index = 0; index <= 7; index++) {
        soc_reg32_set(unit, E2ECC_TX_ENABLE_BMPr, REG_PORT_ANY, index, 0);
    }
    soc_reg32_set(unit, E2ECC_TX_PORTS_NUMr, REG_PORT_ANY, 0, 0);

    /* port-based : uplink*/
    SOC_PBMP_ITER(pbmp_uplink, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_uplink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                          HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_uplink_ports);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                          PKTSETLIMITf,
                          shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                          + egress_xq_min_reserve_lossless_uplink_ports);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossy_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf,
                          shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch
                          + egress_xq_min_reserve_lossless_uplink_ports - 1);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_uplink_port  - skidmarker - prefetch - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_6kxq_uplink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_uplink_ports_lossless);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_uplink_ports_lossless);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                      port, 7, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              0);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              0);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, temp_val +
                              egress_queue_min_reserve_uplink_ports_lossy);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val +
                              egress_queue_min_reserve_uplink_ports_lossy -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, temp_val +
                          egress_queue_min_reserve_uplink_ports_lossless);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf,
                          temp_val +
                          egress_queue_min_reserve_uplink_ports_lossless -
                          ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                      port, 7, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, temp_val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf,
                              (total_advertised_cell_memory - 1));
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7*/
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }


    /* port-based : downlink lossless */
    SOC_PBMP_ITER(pbmp_downlink_lossless, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0x80);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_downlink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          128);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 7 */
        speed = si->port_speed_max[port];
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        if (speed == 1000) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_1g_port_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_1g_port_downlink_ports);
        } else if (speed == 2500) {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_2dot5g_port_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_2dot5g_port_downlink_ports);
        } else {
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_thresholds_per_port_10g_port_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_thresholds_per_port_10g_port_downlink_ports);
        }
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 7 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 7, &rval);
        if (speed == 1000) {
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_1g_port);
        } else if (speed == 2500) {
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_2dot5g_port);
        } else {
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_10g_port);
        }
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 7, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                            HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                        port, 7, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf,
                                  shared_xqs_per_downlink_port - skidmarker -
                                  prefetch + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                            PKTSETLIMITf,
                            shared_xqs_per_downlink_port - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                        port, 7, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf,
                                  shared_xqs_per_downlink_port - skidmarker -
                                  prefetch);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf, shared_xqs_per_downlink_port -
                                skidmarker - prefetch +
                                egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                            PKTRESETLIMITf, shared_xqs_per_downlink_port -
                            skidmarker - prefetch +
                            egress_xq_min_reserve_lossless_downlink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, 7, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf, shared_xqs_per_downlink_port -
                                skidmarker - prefetch - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNTr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNTr, index 7 */
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                            HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                        port, 7, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker - prefetch
                                + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                            PKTSETLIMITf,
                            shared_xqs_per_downlink_port - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                        port, 7, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker - prefetch
                                + egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                            PKTRESETLIMITf,
                            shared_xqs_per_downlink_port  - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                            port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, 7, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, 7, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXLIMITf, temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXRESUMELIMITf,
                                temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                            CELLMAXLIMITf, temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                            CELLMAXRESUMELIMITf,
                            temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless -
                            ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, 7, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, temp_val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  temp_val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, 7, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, 7, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf,
                                temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf,
                            temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless -
                            ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                PKTSETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                PKTRESETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  (total_advertised_cell_memory - 1));
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXRESUMELIMITf,
                                total_advertised_cell_memory - 1 -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                            COLOR_DROP_ENf,
                            0);
            soc_reg32_set(unit, COLOR_DROP_ENr,
                        port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : downlink lossy */
    SOC_PBMP_ITER(pbmp_downlink_lossy, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }
        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          0);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_downlink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                            HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                        port, 7, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf,
                                  shared_xqs_per_downlink_port - skidmarker -
                                  prefetch + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                            PKTSETLIMITf,
                            shared_xqs_per_downlink_port - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                        port, 7, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf,
                                  shared_xqs_per_downlink_port - skidmarker -
                                  prefetch);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf, shared_xqs_per_downlink_port -
                                skidmarker - prefetch +
                                egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                            PKTRESETLIMITf, shared_xqs_per_downlink_port -
                            skidmarker - prefetch +
                            egress_xq_min_reserve_lossless_downlink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, 7, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf, shared_xqs_per_downlink_port -
                                skidmarker - prefetch - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNTr,
                            port, index, rval);
            }

            /* HOLCOSMINXQCNTr, index 7 */
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                            HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                        port, 7, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_downlink_port - skidmarker - prefetch
                                + egress_xq_min_reserve_lossy_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                            PKTSETLIMITf,
                            shared_xqs_per_downlink_port - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                        port, 7, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_downlink_port  - skidmarker - prefetch
                                + egress_xq_min_reserve_lossy_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                            PKTRESETLIMITf,
                            shared_xqs_per_downlink_port  - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_downlink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);

                soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                            port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, 7, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, 7, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXLIMITf, temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXRESUMELIMITf,
                                temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                            CELLMAXLIMITf, temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                            CELLMAXRESUMELIMITf,
                            temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless -
                            ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, 7, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, temp_val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  temp_val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                              port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, 7, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports_lossless);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, 7, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf,
                                temp_val +
                                egress_queue_min_reserve_downlink_ports_lossy -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf,
                            temp_val +
                            egress_queue_min_reserve_downlink_ports_lossless -
                            ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                        port, 7, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - 2 * ethernet_mtu_cell);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                PKTSETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                PKTRESETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  (total_advertised_cell_memory - 1));
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXRESUMELIMITf,
                                total_advertised_cell_memory - 1 -
                                ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                            COLOR_DROP_ENf,
                            0);
            soc_reg32_set(unit, COLOR_DROP_ENr,
                        port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    /* port-based : cpu port*/
    SOC_PBMP_ITER(pbmp_cpu, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf,
                          1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf,
                          2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf,
                          3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf,
                          4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf,
                          5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf,
                          6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf,
                          7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf,
                          7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          3);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg_downlink_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossy_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          shared_xqs_per_downlink_port  - skidmarker - prefetch
                          + egress_xq_min_reserve_lossless_downlink_ports);
        soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                      port, 7, rval);

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_downlink_port  - skidmarker - prefetch
                              + egress_xq_min_reserve_lossy_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 7 */
        soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                          PKTRESETLIMITf,
                          shared_xqs_per_downlink_port  - skidmarker - prefetch
                          + egress_xq_min_reserve_lossless_downlink_ports - 1);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                      port, 7, rval);

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_downlink_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        temp_val= fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, temp_val +
                              egress_queue_min_reserve_cpu_ports);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              temp_val +
                              egress_queue_min_reserve_cpu_ports -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf,
                          0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7*/
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf,
                            255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf,
                            0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf,
                            0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, 0);
        }
    }

    return SOC_E_NONE;
}

STATIC int _soc_firelight_mmu_init_helper_chassis(int unit)
{
    uint32 rval;
    int port, phy_port, mmu_port;
    int index;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t pbmp_cpu;
    soc_pbmp_t pbmp_downlink;
    soc_pbmp_t pbmp_uplink_6kxq;
    soc_pbmp_t pbmp_uplink_2kxq;
    soc_pbmp_t pbmp_active;
    int check_uplink_speed_type;
    int check_downlink_speed_type;
    int speed = 0;
    int standard_jumbo_frame;
    int cell_size;
    int ethernet_mtu_cell;
    int standard_jumbo_frame_cell;
    int total_cell_memory_for_admission;
    int skidmarker;
    int prefetch;
    int total_cell_memory;
    int total_buffer_space_for_downstream_traffic;
    int total_buffer_space_for_upstream_channelized_fc_message;
    int total_buffer_space_for_upstream_data_high_traffic;
    int total_buffer_space_for_cpu_traffic;
    int total_headroom_space_for_downstream_traffic;
    int cfapfullsetpoint;
    int total_advertised_cell_memory;
    int number_of_active_uplink_ports;
    int number_of_downlink_ports;
    int headroom_for_backplane_port;
    int num_1g_ports_downlink_ports;
    int num_2dot5g_ports_downlink_ports;
    int num_5g_ports_downlink_ports;
    int num_10g_ports_downlink_ports;
    int num_100g_ports_uplink_ports;
    int num_50g_ports_uplink_ports;
    int num_40g_ports_uplink_ports;
    int num_25g_ports_uplink_ports;
    int num_10g_ports_uplink_ports;
    int num_data_classes;
    int mmu_xoff_pkt_threshold_uplink_ports;
    int mmu_xoff_pkt_threshold_downlink_ports;
    int mmu_xoff_cell_threshold_all_downlink_ports;
    int mmu_xoff_cell_threshold_all_uplink_ports;
    int xoff_cell_threshold_all_downlink_ports;
    int egress_queue_min_reserve_uplink_ports;
    int egress_queue_min_reserve_downlink_ports;
    int egress_queue_min_reserve_cpu_ports;
    int egress_xq_min_reserve_backplane_ports;
    int egress_xq_min_reserve_access_ports;
    int egress_xq_min_reserve_cpu_ports;
    int num_backplane_queues;
    int num_access_queues;
    int num_active_egress_queues_per_access_port;
    int num_cpu_queues;
    int num_cpu_ports;
    int numxqs_per_backplane_ports;
    int numxqs_per_downlink_ports_and_cpu_port;
    int xoff_cell_threshold_all_uplink_ports;
    int xoff_cell_threshold_per_cpu_port;
    int xoff_packet_thresholds_per_port_uplink_port;
    int xoff_packet_thresholds_per_port_downlink_port;
    int discard_limit_per_port_pg_backplane_port;
    int discard_limit_per_port_pg0_access_port;
    int discard_limit_per_port_pg1_access_port;
    int discard_limit_per_port_pg7_access_port;
    int discard_limit_per_queue_access_port;
    int total_reserved_cells_for_uplink_ports;
    int total_reserved_cells_for_downlink_ports;
    int total_reserved_cells_for_cpu_port;
    int total_reserved;
    int shared_space_cells;
    int reserved_xqs_per_6kxq_backplane_port;
    int reserved_xqs_per_2kxq_backplane_port;
    int shared_xqs_per_6kxq_backplane_port;
    int shared_xqs_per_2kxq_backplane_port;
    int shared_xqs_per_access_port;
    int reserved_xqs_per_access_port;
    int gbllimitsetlimit_gblcellsetlimit_up;
    int cl_downlink_option = 0;
    int count_ge = 0;
    int count_xl = 0;
    soc_pbmp_t pbmp_downlink_backplane;

    SOC_PBMP_COUNT(PBMP_GE_ALL(unit), count_ge);
    SOC_PBMP_COUNT(PBMP_XL_ALL(unit), count_xl);
    if ((count_ge + count_xl) == 0) {
        cl_downlink_option = 1;
        pbmp_downlink_backplane = soc_property_get_pbmp(unit,
                                              spn_DOWNLINK_BACKPLANE_PBMP, 0);
    }

    num_cpu_ports = 0;
    num_data_classes = 2;
    num_access_queues = 2;
    total_buffer_space_for_downstream_traffic = 5040;
    number_of_active_uplink_ports = 0;
    number_of_downlink_ports = 0;
    num_1g_ports_downlink_ports = 0;
    num_2dot5g_ports_downlink_ports = 0;
    num_5g_ports_downlink_ports = 0;
    num_10g_ports_downlink_ports = 0;
    num_100g_ports_uplink_ports = 0;
    num_50g_ports_uplink_ports = 0;
    num_40g_ports_uplink_ports = 0;
    num_25g_ports_uplink_ports = 0;
    num_10g_ports_uplink_ports = 0;
    SOC_PBMP_CLEAR(pbmp_cpu);
    SOC_PBMP_CLEAR(pbmp_downlink);
    SOC_PBMP_CLEAR(pbmp_uplink_6kxq);
    SOC_PBMP_CLEAR(pbmp_uplink_2kxq);
    SOC_PBMP_CLEAR(pbmp_active);

    pbmp_active = soc_property_get_pbmp(unit, spn_ACTIVE_BACKPLANE_PBMP, 0);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            continue; /* this user port has not been mapping in this sku */
        }
        mmu_port = si->port_p2m_mapping[phy_port];
        if (IS_CPU_PORT(unit, port)) {
            num_cpu_ports++;
            SOC_PBMP_PORT_ADD(pbmp_cpu, port);
            continue;
        }
        speed = si->port_speed_max[port];
        if (speed == 0) {
            continue; /* this user port has not been mapping in this sku */
        } else if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            continue; /* this user port has been masked out by pbmp_valid */
        }
        if (IS_CL_PORT(unit, port) && cl_downlink_option &&
            !SOC_PBMP_MEMBER(pbmp_downlink_backplane, port)) {
            if (SOC_PBMP_MEMBER(pbmp_active, port)) {
                number_of_active_uplink_ports++;
                if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
                } else {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
                }
            }
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            if (SOC_PBMP_MEMBER(pbmp_active, port)) {
                number_of_active_uplink_ports++;
                if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_6kxq, port);
                } else {
                    SOC_PBMP_PORT_ADD(pbmp_uplink_2kxq, port);
                }
            }
        } else {
            number_of_downlink_ports++;
            SOC_PBMP_PORT_ADD(pbmp_downlink, port);
            if (SOC_INFO(unit).port_speed_max[port] > 5000) {
                num_10g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_speed_max[port] > 2500) {
                num_5g_ports_downlink_ports++;
            } else if (SOC_INFO(unit).port_speed_max[port] > 1000) {
                num_2dot5g_ports_downlink_ports++;
            } else {
                num_1g_ports_downlink_ports++;
            }
        }
    }

    if (number_of_active_uplink_ports == 0) {
        LOG_CLI((BSL_META_U(unit,
                 "Wrong config of the active packplane ports\n")));
        return SOC_E_PARAM;
    }

    /* Check all active backplane ports has the same speed */
    SOC_PBMP_ITER(pbmp_active, port) {
        speed = si->port_speed_max[port];
        if (SOC_INFO(unit).port_speed_max[port] > 100000) {
            LOG_CLI((BSL_META_U(unit,
                     "for backplane port %d,\
                     the max speed cannot exceed 100G (value=%d)\n"),
                     port, SOC_INFO(unit).port_speed_max[port]));
            return SOC_E_PARAM;
        } else if (SOC_INFO(unit).port_speed_max[port] < 10000) {
            SOC_PBMP_PORT_REMOVE(pbmp_active, port);
        } else if (SOC_INFO(unit).port_speed_max[port] > 50000) {
            num_100g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 40000) {
            num_50g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 25000) {
            num_40g_ports_uplink_ports++;
        } else if (SOC_INFO(unit).port_speed_max[port] > 10000) {
            num_25g_ports_uplink_ports++;
        } else {
            num_10g_ports_uplink_ports++;
        }
    }

    check_uplink_speed_type = 0;
    check_uplink_speed_type = (num_100g_ports_uplink_ports == 0 ? 0 : 1) +
        (num_50g_ports_uplink_ports == 0 ? 0 : 1) +
        (num_40g_ports_uplink_ports == 0 ? 0 : 1) +
        (num_25g_ports_uplink_ports == 0 ? 0 : 1) +
        (num_10g_ports_uplink_ports == 0 ? 0 : 1);
    if (check_uplink_speed_type > 1) {
        LOG_CLI((BSL_META_U(unit,
                 "The backplane ports have the different port speed\n")));
        return SOC_E_PARAM;
    }

    check_downlink_speed_type = 0;
    check_downlink_speed_type = (num_10g_ports_downlink_ports == 0 ? 0 : 1) +
        (num_5g_ports_downlink_ports == 0 ? 0 : 1) +
        (num_2dot5g_ports_downlink_ports == 0 ? 0 : 1) +
        (num_1g_ports_downlink_ports == 0 ? 0 : 1);
    if (check_downlink_speed_type > 1) {
        LOG_CLI((BSL_META_U(unit,
                 "The access ports have the different port speed\n")));
        return SOC_E_PARAM;
    }

    standard_jumbo_frame = 9216;
    cell_size = 144;
    ethernet_mtu_cell = fl_ceiling_func(15 * 1024 / 10, cell_size);
    standard_jumbo_frame_cell =
          fl_ceiling_func(standard_jumbo_frame, cell_size);
    total_cell_memory_for_admission = 14336;
    skidmarker = 7;
    prefetch = 64 + 4;
    total_cell_memory = total_cell_memory_for_admission;
    cfapfullsetpoint = 16222;
    total_advertised_cell_memory = total_cell_memory;
    mmu_xoff_pkt_threshold_uplink_ports = total_advertised_cell_memory;
    mmu_xoff_pkt_threshold_downlink_ports = total_advertised_cell_memory;
    mmu_xoff_cell_threshold_all_uplink_ports = total_advertised_cell_memory;
    mmu_xoff_cell_threshold_all_downlink_ports =
        total_buffer_space_for_downstream_traffic/number_of_active_uplink_ports;
    num_active_egress_queues_per_access_port = num_access_queues;
    egress_queue_min_reserve_uplink_ports = 0;
    egress_queue_min_reserve_downlink_ports = 0;
    egress_queue_min_reserve_cpu_ports = 0;
    egress_xq_min_reserve_backplane_ports = 0;
    egress_xq_min_reserve_access_ports = 0;
    egress_xq_min_reserve_cpu_ports = 0;
    num_backplane_queues = num_data_classes + 1;
    total_buffer_space_for_upstream_channelized_fc_message = 96;
    total_buffer_space_for_upstream_data_high_traffic = 1248;
    total_buffer_space_for_cpu_traffic = 30;
    total_headroom_space_for_downstream_traffic = 1032;
    num_cpu_queues = 8;
    numxqs_per_backplane_ports = 6 * 1024;
    numxqs_per_downlink_ports_and_cpu_port = 2 * 1024;
    headroom_for_backplane_port =
        total_headroom_space_for_downstream_traffic/number_of_active_uplink_ports;
    xoff_cell_threshold_all_downlink_ports =
          mmu_xoff_cell_threshold_all_downlink_ports;
    xoff_cell_threshold_all_uplink_ports =
          mmu_xoff_cell_threshold_all_uplink_ports;
    xoff_cell_threshold_per_cpu_port =
        mmu_xoff_cell_threshold_all_uplink_ports;
    xoff_packet_thresholds_per_port_uplink_port =
          mmu_xoff_pkt_threshold_uplink_ports;
    xoff_packet_thresholds_per_port_downlink_port =
          mmu_xoff_pkt_threshold_downlink_ports;
    discard_limit_per_port_pg_backplane_port =
        headroom_for_backplane_port + xoff_cell_threshold_all_downlink_ports;
    discard_limit_per_port_pg0_access_port = fl_floor_func
        ((total_advertised_cell_memory - total_buffer_space_for_downstream_traffic - total_headroom_space_for_downstream_traffic - total_buffer_space_for_upstream_channelized_fc_message - total_buffer_space_for_upstream_data_high_traffic - total_buffer_space_for_cpu_traffic), number_of_downlink_ports);
    discard_limit_per_port_pg1_access_port = fl_floor_func
        (total_buffer_space_for_upstream_data_high_traffic, number_of_downlink_ports);
    discard_limit_per_port_pg7_access_port = fl_floor_func
        (total_buffer_space_for_upstream_channelized_fc_message, number_of_downlink_ports);
    discard_limit_per_queue_access_port = fl_floor_func
        (total_buffer_space_for_downstream_traffic, number_of_downlink_ports);
    total_reserved_cells_for_uplink_ports =
          egress_queue_min_reserve_uplink_ports *
          number_of_active_uplink_ports * num_data_classes;
    total_reserved_cells_for_downlink_ports =
          number_of_downlink_ports *
          egress_queue_min_reserve_downlink_ports * num_access_queues;
    total_reserved_cells_for_cpu_port =
          num_cpu_ports * egress_queue_min_reserve_cpu_ports * num_cpu_queues;
    total_reserved =
          total_reserved_cells_for_uplink_ports +
          total_reserved_cells_for_downlink_ports +
          total_reserved_cells_for_cpu_port;
    shared_space_cells = total_advertised_cell_memory - total_reserved;
    reserved_xqs_per_6kxq_backplane_port =
        egress_xq_min_reserve_backplane_ports * num_backplane_queues;
    reserved_xqs_per_2kxq_backplane_port =
        egress_xq_min_reserve_backplane_ports * num_backplane_queues;
    shared_xqs_per_6kxq_backplane_port =
        numxqs_per_backplane_ports - reserved_xqs_per_6kxq_backplane_port;
    shared_xqs_per_2kxq_backplane_port =
        numxqs_per_downlink_ports_and_cpu_port - reserved_xqs_per_2kxq_backplane_port;
    reserved_xqs_per_access_port =
        num_active_egress_queues_per_access_port * egress_xq_min_reserve_access_ports;
    shared_xqs_per_access_port =
        numxqs_per_downlink_ports_and_cpu_port - reserved_xqs_per_access_port;
    gbllimitsetlimit_gblcellsetlimit_up = total_cell_memory_for_admission;

    /* system-based */
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLSETPOINTf,
                           cfapfullsetpoint);
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLRESETPOINTf,
                           cfapfullsetpoint -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, GBLLIMITSETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLSETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, GBLLIMITRESETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLRESETLIMITf,
                           gbllimitsetlimit_gblcellsetlimit_up);
    soc_reg_field32_modify(unit, TOTALDYNCELLSETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLSETLIMITf,
                           shared_space_cells);
    soc_reg_field32_modify(unit, TOTALDYNCELLRESETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLRESETLIMITf,
                           shared_space_cells -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, TWO_LAYER_SCH_MODEr,
                           REG_PORT_ANY,
                           SCH_MODEf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           MULTIPLE_ACCOUNTING_FIX_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           CNG_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYN_XQ_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           HOL_CELL_SOP_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYNAMIC_MEMORY_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           SKIDMARKERf,
                           3);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_0r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_1r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_2r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           3);
    soc_reg32_set(unit, E2ECC_MODEr, REG_PORT_ANY, 0, 1);
    soc_reg32_set(unit, E2ECC_HOL_ENr, REG_PORT_ANY, 0, 1);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, TIMERf, 12);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, TIMERf, 12);
    /* E2ECC_TX_ENABLE_BMPr, index 0 ~ 7 */
    for (index = 0; index <= 7; index++) {
        soc_reg32_set(unit, E2ECC_TX_ENABLE_BMPr, REG_PORT_ANY, index, 15);
    }
    soc_reg32_set(unit, E2ECC_TX_PORTS_NUMr, REG_PORT_ANY, 0,
                  number_of_downlink_ports + 2);

    /* port-based : 6kxq backplane ports */
    SOC_PBMP_ITER(pbmp_uplink_6kxq, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          128);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 0);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf, 1);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                          CELLSETLIMITf,
                          xoff_cell_threshold_all_downlink_ports);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                          CELLRESETLIMITf,
                          xoff_cell_threshold_all_downlink_ports
                          - ethernet_mtu_cell);
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              total_advertised_cell_memory);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 7 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf,
                          discard_limit_per_port_pg_backplane_port);
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 7, rval);

        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_backplane_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_6kxq_backplane_port - skidmarker -
                              prefetch + egress_xq_min_reserve_backplane_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_6kxq_backplane_port - skidmarker -
                              prefetch + egress_xq_min_reserve_backplane_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_backplane_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_backplane_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_backplane_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_backplane_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_6kxq_backplane_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_6kxq_backplane_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf,
                              cfapfullsetpoint);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf,
                              cfapfullsetpoint - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          cfapfullsetpoint);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          cfapfullsetpoint - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                        port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf,
                              numxqs_per_backplane_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf, 6142);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, 6143);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, 6143);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, cfapfullsetpoint);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              cfapfullsetpoint - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                          port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr,
                      port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, cfapfullsetpoint);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, cfapfullsetpoint);
        }
    }

    /* port-based : 2kxq backplane ports */
    SOC_PBMP_ITER(pbmp_uplink_2kxq, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          128);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf,
                              0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* PG2TCr, index 7 */
        soc_reg32_get(unit, PG2TCr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PG2TCr, &rval,
                          PG_BMPf,
                          128);
        soc_reg32_set(unit, PG2TCr,
                      port, 7, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_uplink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf,
                          0);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf,
                          1);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf,
                          128);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGCELLLIMITr, index 7 */
        soc_reg32_get(unit, PGCELLLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                      CELLSETLIMITf,
                      xoff_cell_threshold_all_downlink_ports);
        soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                      CELLRESETLIMITf,
                      xoff_cell_threshold_all_downlink_ports
                      - ethernet_mtu_cell);
        soc_reg32_set(unit, PGCELLLIMITr,
                      port, 7, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              total_advertised_cell_memory);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 7 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf,
                          discard_limit_per_port_pg_backplane_port);
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 7, rval);

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_backplane_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_2kxq_backplane_port - skidmarker -
                              prefetch + egress_xq_min_reserve_backplane_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_2kxq_backplane_port - skidmarker -
                              prefetch + egress_xq_min_reserve_backplane_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_2kxq_backplane_port - skidmarker -
                          prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_2kxq_backplane_port - skidmarker -
                          prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_uplink_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_uplink_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf,
                              cfapfullsetpoint);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              cfapfullsetpoint - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          cfapfullsetpoint);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          cfapfullsetpoint - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, cfapfullsetpoint);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, cfapfullsetpoint);
        }
    }

    /* port-based : downlink */
    SOC_PBMP_ITER(pbmp_downlink, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf, 1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf, 2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf, 3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf, 4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf, 5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf, 6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf, 0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_downlink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 0);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

      /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf, 255);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_all_uplink_ports);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf, discard_limit_per_port_pg0_access_port);
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 0, rval);

        /* PGDISCARDSETLIMITr, index 1 ~ 6 */
        for (index = 1; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              discard_limit_per_port_pg1_access_port);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }
        /* PGDISCARDSETLIMITr, index 7 */
        soc_reg32_get(unit, PGDISCARDSETLIMITr,
                      port, 7, &rval);
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf, discard_limit_per_port_pg7_access_port);
        soc_reg32_set(unit, PGDISCARDSETLIMITr,
                      port, 7, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_access_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_access_port - skidmarker -
                                prefetch + egress_xq_min_reserve_access_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_access_port - skidmarker -
                                prefetch + egress_xq_min_reserve_access_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                                HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_access_ports);
                soc_reg32_set(unit, HOLCOSMINXQCNTr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                                PKTSETLIMITf,
                                shared_xqs_per_access_port - skidmarker -
                                prefetch + egress_xq_min_reserve_access_ports);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                            PKTSETLIMITf,
                            shared_xqs_per_access_port - skidmarker -
                            prefetch + egress_xq_min_reserve_access_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                        port, 7, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                PKTRESETLIMITf,
                                shared_xqs_per_access_port - skidmarker -
                                prefetch + egress_xq_min_reserve_access_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                            PKTRESETLIMITf,
                            shared_xqs_per_access_port - skidmarker -
                            prefetch + egress_xq_min_reserve_access_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                        port, 7, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                            port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_access_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_access_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXLIMITf,
                                cfapfullsetpoint);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                CELLMAXRESUMELIMITf,
                                cfapfullsetpoint - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr,
                            port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLSETLIMITf,
                                egress_queue_min_reserve_downlink_ports);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                            CELLSETLIMITf,
                            egress_queue_min_reserve_downlink_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                        port, 7, rval);

            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                CELLRESETLIMITf,
                                egress_queue_min_reserve_downlink_ports);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                            port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                        port, 7, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                            CELLRESETLIMITf,
                            egress_queue_min_reserve_downlink_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                        port, 7, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf,
                                discard_limit_per_queue_access_port);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf,
                                discard_limit_per_queue_access_port
                                - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                            port, index, rval);
            }
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          cfapfullsetpoint);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          cfapfullsetpoint - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                            port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                PKTSETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                PKTRESETLIMITf,
                                numxqs_per_downlink_ports_and_cpu_port - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                CNGPKTSETLIMIT0f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                            port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                CNGPKTSETLIMIT1f,
                                numxqs_per_downlink_ports_and_cpu_port - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                            port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXLIMITf, cfapfullsetpoint);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                CELLMAXRESUMELIMITf,
                                cfapfullsetpoint - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                            port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                        port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr,
                        port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr,
                        port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr,
                      port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_CELL_ENf, 255);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, cfapfullsetpoint);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        rval = fl_floor_func
            (total_buffer_space_for_upstream_channelized_fc_message + total_buffer_space_for_upstream_data_high_traffic,
            number_of_downlink_ports) + 1;
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }
    }

    /* port-based : cpu port*/
    SOC_PBMP_ITER(pbmp_cpu, port) {
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PPFC_PG_ENf,
                          0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI0_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI1_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI2_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI3_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI4_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI5_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI6_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval,
                          PRI7_GRPf, 0);
        soc_reg32_set(unit, PG_CTRL0r,
                      port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI8_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI9_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI10_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI11_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI12_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI13_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI14_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL1r, &rval,
                          PRI15_GRPf, 0);
        soc_reg32_set(unit, PG_CTRL1r,
                      port, 0, rval);

        /* PG2TCr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PG2TCr,
                          port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval,
                              PG_BMPf, 0);
            soc_reg32_set(unit, PG2TCr,
                          port, index, rval);
        }

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          PKTSETLIMITf,
                          xoff_packet_thresholds_per_port_downlink_port);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval,
                          RESETLIMITSELf, 0);
        soc_reg32_set(unit, IBPPKTSETLIMITr,
                      port, 0, rval);

      /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval,
                          MMU_FC_RX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_RX_ENr,
                      port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval,
                          MMU_FC_TX_ENABLEf, 0);
        soc_reg32_set(unit, MMU_FC_TX_ENr,
                      port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGCELLLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLSETLIMITf,
                              xoff_cell_threshold_per_cpu_port);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval,
                              CELLRESETLIMITf,
                              xoff_cell_threshold_per_cpu_port);
            soc_reg32_set(unit, PGCELLLIMITr,
                          port, index, rval);
        }

        /* PGDISCARDSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                              DISCARDSETLIMITf,
                              total_buffer_space_for_cpu_traffic);
            soc_reg32_set(unit, PGDISCARDSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf,
                              shared_xqs_per_access_port - skidmarker -
                              prefetch + egress_xq_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf,
                              shared_xqs_per_access_port - skidmarker -
                              prefetch + egress_xq_min_reserve_cpu_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r,
                          port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_downlink_ports_and_cpu_port - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r,
                          port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval,
                          CNGPORTPKTLIMIT0f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r,
                      port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r,
                      port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval,
                          CNGPORTPKTLIMIT1f,
                          numxqs_per_downlink_ports_and_cpu_port - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r,
                      port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval,
                          DYNXQCNTPORTf,
                          shared_xqs_per_access_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr,
                      port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval,
                          DYNRESETLIMPORTf,
                          shared_xqs_per_access_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr,
                      port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf,
                              total_buffer_space_for_cpu_traffic);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr,
                          port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_buffer_space_for_cpu_traffic - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr,
                          port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLSETLIMITf,
                          cfapfullsetpoint);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval,
                          DYNCELLRESETLIMITf,
                          cfapfullsetpoint - (2 * ethernet_mtu_cell));
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr,
                      port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_ENr,
                      port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr,
                        port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr,
                        port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr,
                      port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval,
                          COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr,
                      port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr,
                      port, 0, cfapfullsetpoint);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr,
                          port, index, cfapfullsetpoint);
        }
    }

    return SOC_E_NONE;
}

STATIC int _soc_firelight_mmu_init_bump_on_wire(int unit)
{
    uint32 rval, val;
    int port, phy_port, mmu_port;
    int index;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t pbmp_cpu;
    soc_pbmp_t pbmp_uplink;
    soc_pbmp_t pbmp_6kxq;
    soc_pbmp_t pbmp_2kxq;
    soc_pbmp_t pbmp_downlink;
    soc_pbmp_t pbmp_all;
    soc_pbmp_t pbmp_1g;
    soc_pbmp_t pbmp_2dot5g;
    soc_pbmp_t pbmp_10g_6kxq;
    soc_pbmp_t pbmp_10g_2kxq;
    soc_pbmp_t pbmp_20g_6kxq;
    soc_pbmp_t pbmp_20g_2kxq;
    soc_pbmp_t pbmp_25g_6kxq;
    soc_pbmp_t pbmp_25g_2kxq;
    soc_pbmp_t pbmp_40g_6kxq;
    soc_pbmp_t pbmp_40g_2kxq;
    soc_pbmp_t pbmp_50g_6kxq;
    soc_pbmp_t pbmp_50g_2kxq;
    soc_pbmp_t pbmp_100g_6kxq;
    soc_pbmp_t pbmp_100g_2kxq;
    int speed;
    int standard_jumbo_frame;
    int cell_size;
    int ethernet_mtu_cell;
    int standard_jumbo_frame_cell;
    int total_physical_memory;
    int total_cell_memory_for_admission;
    int number_of_used_memory_banks;
    int reserved_for_cfap;
    int skidmarker;
    int prefetch;
    int total_cell_memory;
    int cfapfullsetpoint;
    int total_advertised_cell_memory;
    int number_of_uplink_ports;
    int number_of_downlink_ports;
    int num_1g_ports_uplink_ports;
    int num_2dot5g_ports_uplink_ports;
    int num_10g_ports_uplink_ports;
    int num_20g_ports_uplink_ports;
    int num_25g_ports_uplink_ports;
    int num_40g_ports_uplink_ports;
    int num_50g_ports_uplink_ports;
    int num_100g_ports_uplink_ports;
    int num_1g_ports_downlink_ports;
    int num_2dot5g_ports_downlink_ports;
    int num_10g_ports_downlink_ports;
    int num_20g_ports_downlink_ports;
    int num_25g_ports_downlink_ports;
    int num_40g_ports_downlink_ports;
    int num_50g_ports_downlink_ports;
    int num_100g_ports_downlink_ports;
    int queue_port_limit_ratio;
    int egress_queue_min_reserve_lossy;
    int egress_queue_min_reserve_lossless_1g_port;
    int egress_queue_min_reserve_lossless_2dot5g_port;
    int egress_queue_min_reserve_lossless_10g_port;
    int egress_queue_min_reserve_lossless_20g_port;
    int egress_queue_min_reserve_lossless_25g_port;
    int egress_queue_min_reserve_lossless_40g_port;
    int egress_queue_min_reserve_lossless_50g_port;
    int egress_queue_min_reserve_lossless_100g_port;
    int egress_queue_min_reserve_cpu_ports;
    int egress_xq_min_reserve_lossy;
    int egress_xq_min_reserve_lossless_1g_port;
    int egress_xq_min_reserve_lossless_2dot5g_port;
    int egress_xq_min_reserve_lossless_10g_port;
    int egress_xq_min_reserve_lossless_20g_port;
    int egress_xq_min_reserve_lossless_25g_port;
    int egress_xq_min_reserve_lossless_40g_port;
    int egress_xq_min_reserve_lossless_50g_port;
    int egress_xq_min_reserve_lossless_100g_port;
    int num_lossy_queues;
    int num_lossless_queues;
    int mmu_xoff_pkt_threshold_uplink_ports;
    int mmu_xoff_pkt_threshold_downlink_ports;
    int mmu_xoff_cell_threshold_1g_port;
    int mmu_xoff_cell_threshold_2dot5g_port;
    int mmu_xoff_cell_threshold_10g_port;
    int mmu_xoff_cell_threshold_20g_port;
    int mmu_xoff_cell_threshold_25g_port;
    int mmu_xoff_cell_threshold_40g_port;
    int mmu_xoff_cell_threshold_50g_port;
    int mmu_xoff_cell_threshold_100g_port;
    int num_cpu_queues;
    int num_cpu_ports;
    int numxqs_per_6kxq_uplink_ports;
    int numxqs_per_2kxq_uplink_ports;
    int headroom_for_1g_port;
    int headroom_for_2dot5g_port;
    int headroom_for_10g_port;
    int headroom_for_20g_port;
    int headroom_for_25g_port;
    int headroom_for_40g_port;
    int headroom_for_50g_port;
    int headroom_for_100g_port;
    int total_required_pg_headroom;
    int xoff_cell_thresholds_per_port_1g_port;
    int xoff_cell_thresholds_per_port_2dot5g_port;
    int xoff_cell_thresholds_per_port_10g_port;
    int xoff_cell_thresholds_per_port_20g_port;
    int xoff_cell_thresholds_per_port_25g_port;
    int xoff_cell_thresholds_per_port_40g_port;
    int xoff_cell_thresholds_per_port_50g_port;
    int xoff_cell_thresholds_per_port_100g_port;
    int xoff_cell_threshold_downlink_ports;
    int xoff_packet_thresholds_per_port_uplink_port;
    int xoff_packet_thresholds_per_port_downlink_port;
    int discard_limit_per_port_pg_uplink_port;
    int discard_limit_per_port_pg_downlink_port;
    int discard_limit_per_port_pg_1g_port;
    int discard_limit_per_port_pg_2dot5g_port;
    int discard_limit_per_port_pg_10g_port;
    int discard_limit_per_port_pg_20g_port;
    int discard_limit_per_port_pg_25g_port;
    int discard_limit_per_port_pg_40g_port;
    int discard_limit_per_port_pg_50g_port;
    int discard_limit_per_port_pg_100g_port;
    int total_reserved_cells_for_uplink_ports;
    int total_reserved_cells_for_downlink_ports;
    int total_reserved_cells_for_cpu_port;
    int total_reserved;
    int shared_space_cells;
    int reserved_xqs_per_1g_port;
    int reserved_xqs_per_2dot5g_port;
    int reserved_xqs_per_10g_port;
    int reserved_xqs_per_20g_port;
    int reserved_xqs_per_25g_port;
    int reserved_xqs_per_40g_port;
    int reserved_xqs_per_50g_port;
    int reserved_xqs_per_100g_port;
    int reserved_xqs_per_cpu_port;
    int shared_xqs_per_2kxq_port_1g;
    int shared_xqs_per_2kxq_port_2dot5g;
    int shared_xqs_per_2kxq_port_10g;
    int shared_xqs_per_2kxq_port_20g;
    int shared_xqs_per_2kxq_port_25g;
    int shared_xqs_per_2kxq_port_40g;
    int shared_xqs_per_2kxq_port_50g;
    int shared_xqs_per_2kxq_port_100g;
    int shared_xqs_per_6kxq_port_10g;
    int shared_xqs_per_6kxq_port_20g;
    int shared_xqs_per_6kxq_port_25g;
    int shared_xqs_per_6kxq_port_40g;
    int shared_xqs_per_6kxq_port_50g;
    int shared_xqs_per_6kxq_port_100g;
    int shared_xqs_cpu_port;
    int cl_downlink_option = 0;
    int count_ge = 0, count_xl = 0;
    soc_pbmp_t pbmp_downlink_backplane;

    SOC_PBMP_COUNT(PBMP_GE_ALL(unit), count_ge);
    SOC_PBMP_COUNT(PBMP_XL_ALL(unit), count_xl);
    if ((count_ge + count_xl) == 0) {
        cl_downlink_option = 1;
        pbmp_downlink_backplane = soc_property_get_pbmp(unit,
                                              spn_DOWNLINK_BACKPLANE_PBMP, 0);
    }

    /* setup port bitmap according the port max speed for lossy
     *   TSC/TSCF    : uplink port
     *   QGMII/SGMII : downlink port
     */
    num_cpu_ports = 0;
    number_of_uplink_ports = 0;
    number_of_downlink_ports = 0;
    num_1g_ports_uplink_ports = 0;
    num_2dot5g_ports_uplink_ports = 0;
    num_10g_ports_uplink_ports = 0;
    num_20g_ports_uplink_ports = 0;
    num_25g_ports_uplink_ports = 0;
    num_40g_ports_uplink_ports = 0;
    num_50g_ports_uplink_ports = 0;
    num_100g_ports_uplink_ports = 0;
    num_1g_ports_downlink_ports = 0;
    num_2dot5g_ports_downlink_ports = 0;
    num_10g_ports_downlink_ports = 0;
    num_20g_ports_downlink_ports = 0;
    num_25g_ports_downlink_ports = 0;
    num_40g_ports_downlink_ports = 0;
    num_50g_ports_downlink_ports = 0;
    num_100g_ports_downlink_ports = 0;
    SOC_PBMP_CLEAR(pbmp_cpu);
    SOC_PBMP_CLEAR(pbmp_uplink);
    SOC_PBMP_CLEAR(pbmp_6kxq);
    SOC_PBMP_CLEAR(pbmp_2kxq);
    SOC_PBMP_CLEAR(pbmp_downlink);
    SOC_PBMP_CLEAR(pbmp_all);
    SOC_PBMP_CLEAR(pbmp_1g);
    SOC_PBMP_CLEAR(pbmp_2dot5g);
    SOC_PBMP_CLEAR(pbmp_10g_6kxq);
    SOC_PBMP_CLEAR(pbmp_10g_2kxq);
    SOC_PBMP_CLEAR(pbmp_20g_6kxq);
    SOC_PBMP_CLEAR(pbmp_20g_2kxq);
    SOC_PBMP_CLEAR(pbmp_25g_6kxq);
    SOC_PBMP_CLEAR(pbmp_25g_2kxq);
    SOC_PBMP_CLEAR(pbmp_40g_6kxq);
    SOC_PBMP_CLEAR(pbmp_40g_2kxq);
    SOC_PBMP_CLEAR(pbmp_50g_6kxq);
    SOC_PBMP_CLEAR(pbmp_50g_2kxq);
    SOC_PBMP_CLEAR(pbmp_100g_6kxq);
    SOC_PBMP_CLEAR(pbmp_100g_2kxq);

    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (IS_CPU_PORT(unit, port)) {
            num_cpu_ports++;
            SOC_PBMP_PORT_ADD(pbmp_cpu, port);
            continue;
        }
        speed = si->port_speed_max[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        if (port < 0 || speed <= 0 ) {
            continue; /* this user port has not been mapping in this sku */
        } else if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
            continue; /* this user port has been masked out by pbmp_valid */
        }

        if (IS_CL_PORT(unit, port) && cl_downlink_option) {
            if (!SOC_PBMP_MEMBER(pbmp_downlink_backplane, port)) {
                SOC_PBMP_PORT_ADD(pbmp_uplink, port);
                number_of_uplink_ports++;
            } else {
                SOC_PBMP_PORT_ADD(pbmp_downlink, port);
                number_of_downlink_ports++;
            }
            if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
                SOC_PBMP_PORT_ADD(pbmp_6kxq, port);
            } else {
                SOC_PBMP_PORT_ADD(pbmp_2kxq, port);
            }
        } else if (IS_CL_PORT(unit, port) && !cl_downlink_option) {
            SOC_PBMP_PORT_ADD(pbmp_uplink, port);
        }
    }
    if (number_of_uplink_ports != number_of_downlink_ports) {
        LOG_CLI((BSL_META_U(unit,
                 "Num of Uplink and Downlink ports should be equal\n")));
        return SOC_E_PARAM;
    }

    SOC_PBMP_OR(pbmp_all, pbmp_cpu);
    SOC_PBMP_OR(pbmp_all, pbmp_uplink);
    SOC_PBMP_OR(pbmp_all, pbmp_downlink);

    SOC_PBMP_ITER(pbmp_all, port) {
        speed = si->port_init_speed[port];
        if (speed > 100000) {
            LOG_CLI((BSL_META_U(unit,
                     "for uplink port %d,\
                     the max speed cannot exceed 100G (value=%d)\n"),
                     port, speed));
            return SOC_E_PARAM;
        } else if (speed > 50000) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_100g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_100g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_100g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_100g_ports_downlink_ports++;
            }
        } else if (speed > 40000) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_50g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_50g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_50g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_50g_ports_downlink_ports++;
            }
        } else if (speed > 25000) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_40g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_40g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_40g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_40g_ports_downlink_ports++;
            }
        } else if (speed > 20000) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_25g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_25g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_25g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_25g_ports_downlink_ports++;
            }
        } else if (speed > 10000) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_20g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_20g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_20g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_20g_ports_downlink_ports++;
            }
        } else if (speed > 2500) {
            if (SOC_PBMP_MEMBER(pbmp_6kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_10g_6kxq, port);
            } else if (SOC_PBMP_MEMBER(pbmp_2kxq, port)) {
                SOC_PBMP_PORT_ADD(pbmp_10g_2kxq, port);
            }
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_10g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_10g_ports_downlink_ports++;
            }
        } else if (speed > 1000) {
            SOC_PBMP_PORT_ADD(pbmp_2dot5g, port);
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_2dot5g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_2dot5g_ports_downlink_ports++;
            }
        } else {
            SOC_PBMP_PORT_ADD(pbmp_1g, port);
            if (SOC_PBMP_MEMBER(pbmp_uplink, port)) {
                num_1g_ports_uplink_ports++;
            } else if (SOC_PBMP_MEMBER(pbmp_downlink, port)) {
                num_1g_ports_downlink_ports++;
            }
        }
    }

    standard_jumbo_frame = 9216;
    cell_size = 144;
    standard_jumbo_frame_cell = fl_ceiling_func(standard_jumbo_frame, cell_size);
    ethernet_mtu_cell = fl_ceiling_func(15 * 1024 / 10, cell_size);
    total_cell_memory_for_admission = 14 * 1024;
    total_physical_memory = 16 * 1024;
    number_of_used_memory_banks = 8;
    reserved_for_cfap = 65 * 2 + number_of_used_memory_banks * 4;
    cfapfullsetpoint = total_physical_memory - reserved_for_cfap;
    total_cell_memory = total_cell_memory_for_admission;
    total_advertised_cell_memory = total_cell_memory;
    skidmarker = 7;
    prefetch = 64 + 4;
    queue_port_limit_ratio = 1;
    egress_queue_min_reserve_lossy = 0;
    egress_queue_min_reserve_cpu_ports = ethernet_mtu_cell;
    num_lossless_queues = 1;
    num_lossy_queues = 7;
    num_cpu_queues = 8;
    numxqs_per_6kxq_uplink_ports = 6 * 1024;
    numxqs_per_2kxq_uplink_ports = 2 * 1024;
    headroom_for_1g_port = 118;
    headroom_for_2dot5g_port = 120;
    headroom_for_10g_port = 160;
    headroom_for_20g_port = 198;
    headroom_for_25g_port = 256;
    headroom_for_40g_port = 274;
    headroom_for_50g_port = 352;
    headroom_for_100g_port = 574;
    mmu_xoff_pkt_threshold_uplink_ports = total_advertised_cell_memory;
    mmu_xoff_pkt_threshold_downlink_ports = total_advertised_cell_memory;
    xoff_cell_threshold_downlink_ports = total_advertised_cell_memory;
    xoff_packet_thresholds_per_port_uplink_port
          = mmu_xoff_pkt_threshold_uplink_ports;
    xoff_packet_thresholds_per_port_downlink_port
          = mmu_xoff_pkt_threshold_downlink_ports;
    discard_limit_per_port_pg_uplink_port = total_advertised_cell_memory;
    discard_limit_per_port_pg_downlink_port = total_advertised_cell_memory;
    total_required_pg_headroom
        = headroom_for_1g_port *
          (num_1g_ports_uplink_ports + num_1g_ports_downlink_ports) +
          headroom_for_2dot5g_port *
          (num_2dot5g_ports_uplink_ports + num_2dot5g_ports_downlink_ports) +
          headroom_for_10g_port *
          (num_10g_ports_uplink_ports + num_10g_ports_downlink_ports) +
          headroom_for_20g_port *
          (num_20g_ports_uplink_ports + num_20g_ports_downlink_ports) +
          headroom_for_25g_port *
          (num_25g_ports_uplink_ports + num_25g_ports_downlink_ports) +
          headroom_for_40g_port *
          (num_40g_ports_uplink_ports + num_40g_ports_downlink_ports) +
          headroom_for_50g_port *
          (num_50g_ports_uplink_ports + num_50g_ports_downlink_ports) +
          headroom_for_100g_port *
          (num_100g_ports_uplink_ports + num_100g_ports_downlink_ports);
    if ((num_1g_ports_uplink_ports + num_1g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_1g_port = headroom_for_1g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_1g_port / headroom_for_100g_port;
        if (headroom_for_1g_port < val) {
            val = headroom_for_1g_port;
        }
        mmu_xoff_cell_threshold_1g_port = fl_floor_func(val, 1);
    }
    if ((num_2dot5g_ports_uplink_ports + num_2dot5g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_2dot5g_port = headroom_for_2dot5g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_2dot5g_port / total_required_pg_headroom;
        if (headroom_for_2dot5g_port < val) {
            val = headroom_for_2dot5g_port;
        }
        mmu_xoff_cell_threshold_2dot5g_port = fl_floor_func(val, 1);
    }
    if ((num_10g_ports_uplink_ports + num_10g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_10g_port = headroom_for_10g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_10g_port / total_required_pg_headroom;
        if (headroom_for_10g_port < val) {
            val = headroom_for_10g_port;
        }
        mmu_xoff_cell_threshold_10g_port = fl_floor_func(val, 1);
    }
    if ((num_20g_ports_uplink_ports + num_20g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_20g_port = headroom_for_20g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_20g_port / total_required_pg_headroom;
        if (headroom_for_20g_port < val) {
            val = headroom_for_20g_port;
        }
        mmu_xoff_cell_threshold_20g_port = fl_floor_func(val, 1);
    }
    if ((num_25g_ports_uplink_ports + num_25g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_25g_port = headroom_for_25g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_25g_port / total_required_pg_headroom;
        if (headroom_for_25g_port < val) {
            val = headroom_for_25g_port;
        }
        mmu_xoff_cell_threshold_25g_port = fl_floor_func(val, 1);
    }
    if ((num_40g_ports_uplink_ports + num_40g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_40g_port = headroom_for_40g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_40g_port / total_required_pg_headroom;
        if (headroom_for_40g_port < val) {
            val = headroom_for_40g_port;
        }
        mmu_xoff_cell_threshold_40g_port = fl_floor_func(val, 1);
    }
    if ((num_50g_ports_uplink_ports + num_50g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_50g_port = headroom_for_50g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_50g_port / total_required_pg_headroom;
        if (headroom_for_50g_port < val) {
            val = headroom_for_50g_port;
        }
        mmu_xoff_cell_threshold_50g_port = fl_floor_func(val, 1);
    }
    if ((num_100g_ports_uplink_ports + num_100g_ports_downlink_ports) == 0) {
        mmu_xoff_cell_threshold_100g_port = headroom_for_100g_port;
    } else {
        val = (total_cell_memory_for_admission - total_required_pg_headroom -
              (number_of_uplink_ports + number_of_downlink_ports) * 72 - 88) *
              headroom_for_100g_port / total_required_pg_headroom;
        if (headroom_for_100g_port < val) {
            val = headroom_for_100g_port;
        }
        mmu_xoff_cell_threshold_100g_port = fl_floor_func(val, 1);
    }
    xoff_cell_thresholds_per_port_1g_port
          = mmu_xoff_cell_threshold_1g_port;
    xoff_cell_thresholds_per_port_2dot5g_port
          = mmu_xoff_cell_threshold_2dot5g_port;
    xoff_cell_thresholds_per_port_10g_port
          = mmu_xoff_cell_threshold_10g_port;
    xoff_cell_thresholds_per_port_20g_port
          = mmu_xoff_cell_threshold_20g_port;
    xoff_cell_thresholds_per_port_25g_port
          = mmu_xoff_cell_threshold_25g_port;
    xoff_cell_thresholds_per_port_40g_port
          = mmu_xoff_cell_threshold_40g_port;
    xoff_cell_thresholds_per_port_50g_port
          = mmu_xoff_cell_threshold_50g_port;
    xoff_cell_thresholds_per_port_100g_port
          = mmu_xoff_cell_threshold_100g_port;
    discard_limit_per_port_pg_1g_port
        = xoff_cell_thresholds_per_port_1g_port + headroom_for_1g_port;
    discard_limit_per_port_pg_2dot5g_port
        = xoff_cell_thresholds_per_port_2dot5g_port
          + headroom_for_2dot5g_port;
    discard_limit_per_port_pg_10g_port
        = xoff_cell_thresholds_per_port_10g_port
          + headroom_for_10g_port;
    discard_limit_per_port_pg_20g_port
        = xoff_cell_thresholds_per_port_20g_port
          + headroom_for_20g_port;
    discard_limit_per_port_pg_25g_port
        = xoff_cell_thresholds_per_port_25g_port
          + headroom_for_25g_port;
    discard_limit_per_port_pg_40g_port
        = xoff_cell_thresholds_per_port_40g_port
          + headroom_for_40g_port;
    discard_limit_per_port_pg_50g_port
        = xoff_cell_thresholds_per_port_50g_port
          + headroom_for_50g_port;
    discard_limit_per_port_pg_100g_port
        = xoff_cell_thresholds_per_port_100g_port
          + headroom_for_100g_port;
    egress_xq_min_reserve_lossy = 0;
    egress_xq_min_reserve_lossless_1g_port
        = mmu_xoff_cell_threshold_1g_port + headroom_for_1g_port;
    egress_queue_min_reserve_lossless_1g_port
        = egress_xq_min_reserve_lossless_1g_port;
    egress_xq_min_reserve_lossless_2dot5g_port
        = mmu_xoff_cell_threshold_2dot5g_port + headroom_for_2dot5g_port;
    egress_queue_min_reserve_lossless_2dot5g_port
        = egress_xq_min_reserve_lossless_2dot5g_port;
    egress_xq_min_reserve_lossless_10g_port
        = mmu_xoff_cell_threshold_10g_port + headroom_for_10g_port;
    egress_queue_min_reserve_lossless_10g_port
        = egress_xq_min_reserve_lossless_10g_port;
    egress_xq_min_reserve_lossless_20g_port
        = mmu_xoff_cell_threshold_20g_port + headroom_for_20g_port;
    egress_queue_min_reserve_lossless_20g_port
        = egress_xq_min_reserve_lossless_20g_port;
    egress_xq_min_reserve_lossless_25g_port
        = mmu_xoff_cell_threshold_25g_port + headroom_for_25g_port;
    egress_queue_min_reserve_lossless_25g_port
        = egress_xq_min_reserve_lossless_25g_port;
    egress_xq_min_reserve_lossless_40g_port
        = mmu_xoff_cell_threshold_40g_port + headroom_for_40g_port;
    egress_queue_min_reserve_lossless_40g_port
        = egress_xq_min_reserve_lossless_40g_port;
    egress_xq_min_reserve_lossless_50g_port
        = mmu_xoff_cell_threshold_50g_port + headroom_for_50g_port;
    egress_queue_min_reserve_lossless_50g_port
        = egress_xq_min_reserve_lossless_50g_port;
    egress_xq_min_reserve_lossless_100g_port
        = mmu_xoff_cell_threshold_100g_port + headroom_for_100g_port;
    egress_queue_min_reserve_lossless_100g_port
        = egress_xq_min_reserve_lossless_100g_port;
    reserved_xqs_per_1g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_1g_port*num_lossless_queues;
    reserved_xqs_per_2dot5g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + num_lossless_queues * egress_xq_min_reserve_lossless_2dot5g_port;
    reserved_xqs_per_10g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_10g_port*num_lossless_queues;
    reserved_xqs_per_20g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_20g_port*num_lossless_queues;
    reserved_xqs_per_25g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_25g_port*num_lossless_queues;
    reserved_xqs_per_40g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_40g_port*num_lossless_queues;
    reserved_xqs_per_50g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_50g_port*num_lossless_queues;
    reserved_xqs_per_100g_port
        = egress_xq_min_reserve_lossy * num_lossy_queues
          + egress_xq_min_reserve_lossless_100g_port*num_lossless_queues;
    reserved_xqs_per_cpu_port
        = num_lossless_queues * egress_queue_min_reserve_cpu_ports
          + num_lossy_queues * egress_queue_min_reserve_cpu_ports;
    shared_xqs_per_2kxq_port_1g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_1g_port;
    shared_xqs_per_2kxq_port_2dot5g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_2dot5g_port;
    shared_xqs_per_2kxq_port_10g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_10g_port;
    shared_xqs_per_2kxq_port_20g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_20g_port;
    shared_xqs_per_2kxq_port_25g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_25g_port;
    shared_xqs_per_2kxq_port_40g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_40g_port;
    shared_xqs_per_2kxq_port_50g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_50g_port;
    shared_xqs_per_2kxq_port_100g
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_100g_port;
    shared_xqs_per_6kxq_port_10g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_10g_port;
    shared_xqs_per_6kxq_port_20g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_20g_port;
    shared_xqs_per_6kxq_port_25g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_25g_port;
    shared_xqs_per_6kxq_port_40g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_40g_port;
    shared_xqs_per_6kxq_port_50g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_50g_port;
    shared_xqs_per_6kxq_port_100g
        = numxqs_per_6kxq_uplink_ports - reserved_xqs_per_100g_port;
    shared_xqs_cpu_port
        = numxqs_per_2kxq_uplink_ports - reserved_xqs_per_cpu_port;
    total_reserved_cells_for_uplink_ports
        = egress_queue_min_reserve_lossy
          * number_of_uplink_ports * num_lossy_queues
          + num_1g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_1g_port
          + num_2dot5g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_2dot5g_port
          + num_10g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_10g_port
          + num_20g_ports_uplink_ports
          * egress_queue_min_reserve_lossless_20g_port * num_lossless_queues
          + num_25g_ports_uplink_ports
          * egress_queue_min_reserve_lossless_25g_port * num_lossless_queues
          + num_40g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_40g_port
          + num_50g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_50g_port
          + num_100g_ports_uplink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_100g_port;
    total_reserved_cells_for_downlink_ports
        = number_of_downlink_ports * egress_queue_min_reserve_lossy
          * num_lossy_queues + num_1g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_1g_port
          + num_2dot5g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_2dot5g_port
          + num_10g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_10g_port
          + num_20g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_20g_port
          + num_25g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_25g_port
          + num_40g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_40g_port
          + num_50g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_50g_port
          + num_100g_ports_downlink_ports * num_lossless_queues
          * egress_queue_min_reserve_lossless_100g_port;
    total_reserved_cells_for_cpu_port
        = num_cpu_ports * egress_queue_min_reserve_cpu_ports
          * num_cpu_queues;
    total_reserved
        = total_reserved_cells_for_uplink_ports
          + total_reserved_cells_for_downlink_ports
          + total_reserved_cells_for_cpu_port;
    shared_space_cells = total_advertised_cell_memory - total_reserved;

    if ((shared_space_cells * cell_size)/1024 <= 500) {
        LOG_CLI((BSL_META_U(unit,
                 "Shared Pool Is Small,\
                 should be larger than 500 (value=%d)\n"),
                 (shared_space_cells * cell_size)/1024));
        return SOC_E_PARAM;
    }

    /* system-based */
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLSETPOINTf,
                           cfapfullsetpoint);
    soc_reg_field32_modify(unit, CFAPFULLTHRESHOLDr,
                           REG_PORT_ANY,
                           CFAPFULLRESETPOINTf,
                           cfapfullsetpoint -
                           (standard_jumbo_frame_cell * 2));
    soc_reg_field32_modify(unit, GBLLIMITSETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLSETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, GBLLIMITRESETLIMITr,
                           REG_PORT_ANY,
                           GBLCELLRESETLIMITf,
                           total_cell_memory_for_admission);
    soc_reg_field32_modify(unit, TOTALDYNCELLSETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLSETLIMITf,
                           shared_space_cells);
    soc_reg_field32_modify(unit, TOTALDYNCELLRESETLIMITr,
                           REG_PORT_ANY,
                           TOTALDYNCELLRESETLIMITf,
                           shared_space_cells - standard_jumbo_frame_cell * 2);
    soc_reg_field32_modify(unit, TWO_LAYER_SCH_MODEr,
                           REG_PORT_ANY,
                           SCH_MODEf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           MULTIPLE_ACCOUNTING_FIX_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           CNG_DROP_ENf,
                           0);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYN_XQ_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           HOL_CELL_SOP_DROP_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           DYNAMIC_MEMORY_ENf,
                           1);
    soc_reg_field32_modify(unit, MISCCONFIGr,
                           REG_PORT_ANY,
                           SKIDMARKERf,
                           3);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_0r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_1r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           0xFFFFFFFF);
    soc_reg_field32_modify(unit, MMUPORTTXENABLE_2r,
                           REG_PORT_ANY,
                           MMUPORTTXENABLEf,
                           3);

    soc_reg32_set(unit, E2ECC_MODEr, REG_PORT_ANY, 0, 0);
    soc_reg32_set(unit, E2ECC_HOL_ENr, REG_PORT_ANY, 0, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MIN_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, LGf, 0);
    soc_reg_field32_modify(unit, E2ECC_MAX_TX_TIMERr, REG_PORT_ANY, TIMERf, 0);
    /* E2ECC_TX_ENABLE_BMPr, index 0 ~ 7 */
    for (index = 0; index <= 7; index++) {
        soc_reg32_set(unit, E2ECC_TX_ENABLE_BMPr, REG_PORT_ANY, index, 0);
    }
    soc_reg32_set(unit, E2ECC_TX_PORTS_NUMr, REG_PORT_ANY, 0, 0);

    /* port-based : uplink downlink*/
    SOC_PBMP_ITER(pbmp_all, port) {
        speed = si->port_init_speed[port];
        /* PG_CTRL0r, index 0 */
        soc_reg32_get(unit, PG_CTRL0r, port, 0, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = 0;
        } else {
            val = 0x80;
        }
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PPFC_PG_ENf, val);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI0_GRPf, 0);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI1_GRPf, 1);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI2_GRPf, 2);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI3_GRPf, 3);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI4_GRPf, 4);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI5_GRPf, 5);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI6_GRPf, 6);
        soc_reg_field_set(unit, PG_CTRL0r, &rval, PRI7_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL0r, port, 0, rval);

        /* PG_CTRL1r, index 0 */
        soc_reg32_get(unit, PG_CTRL1r, port, 0, &rval);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI8_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI9_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI10_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI11_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI12_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI13_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI14_GRPf, 7);
        soc_reg_field_set(unit, PG_CTRL1r, &rval, PRI15_GRPf, 7);
        soc_reg32_set(unit, PG_CTRL1r, port, 0, rval);

        /* PG2TCr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PG2TCr, port, index, &rval);
            soc_reg_field_set(unit, PG2TCr, &rval, PG_BMPf, 0);
            soc_reg32_set(unit, PG2TCr, port, index, rval);
        }
        /* PG2TCr, index 7 */
        index = 7;
        soc_reg32_get(unit, PG2TCr, port, index, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = 0;
        } else {
            val = 0x80;
        }
        soc_reg_field_set(unit, PG2TCr, &rval, PG_BMPf, val);
        soc_reg32_set(unit, PG2TCr, port, index, rval);

        /* IBPPKTSETLIMITr, index 0 */
        soc_reg32_get(unit, IBPPKTSETLIMITr, port, 0, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = xoff_packet_thresholds_per_port_uplink_port;
        } else {
            val = xoff_packet_thresholds_per_port_downlink_port;
        }
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, PKTSETLIMITf, val);
        soc_reg_field_set(unit, IBPPKTSETLIMITr, &rval, RESETLIMITSELf, 3);
        soc_reg32_set(unit, IBPPKTSETLIMITr, port, 0, rval);

        /* MMU_FC_RX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_RX_ENr, port, 0, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = 0;
        } else {
            val = 0x80;
        }
        soc_reg_field_set(unit, MMU_FC_RX_ENr, &rval, MMU_FC_RX_ENABLEf, val);
        soc_reg32_set(unit, MMU_FC_RX_ENr, port, 0, rval);

        /* MMU_FC_TX_ENr, index 0 */
        soc_reg32_get(unit, MMU_FC_TX_ENr, port, 0, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = 0;
        } else {
            val = 0x80;
        }
        soc_reg_field_set(unit, MMU_FC_TX_ENr, &rval, MMU_FC_TX_ENABLEf, val);
        soc_reg32_set(unit, MMU_FC_TX_ENr, port, 0, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr, port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval, CELLSETLIMITf,
                              xoff_cell_threshold_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr, port, index, rval);
        }
        /* PGCELLLIMITr, index 7 */
        index = 7;
        soc_reg32_get(unit, PGCELLLIMITr, port, index, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = xoff_cell_threshold_downlink_ports;
        } else if (speed == 1000) {
            val = xoff_cell_thresholds_per_port_1g_port;
        } else if (speed == 2500) {
            val = xoff_cell_thresholds_per_port_2dot5g_port;
        } else if (speed == 10000) {
            val = xoff_cell_thresholds_per_port_10g_port;
        } else if (speed == 20000) {
            val = xoff_cell_thresholds_per_port_20g_port;
        } else if (speed == 25000) {
            val = xoff_cell_thresholds_per_port_25g_port;
        } else if (speed == 40000) {
            val = xoff_cell_thresholds_per_port_40g_port;
        } else if (speed == 50000) {
            val = xoff_cell_thresholds_per_port_50g_port;
        } else {
            val = xoff_cell_thresholds_per_port_100g_port;
        }
        soc_reg_field_set(unit, PGCELLLIMITr, &rval, CELLSETLIMITf, val);
        soc_reg32_set(unit, PGCELLLIMITr, port, index, rval);

        /* PGCELLLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGCELLLIMITr, port, index, &rval);
            soc_reg_field_set(unit, PGCELLLIMITr, &rval, CELLRESETLIMITf,
                              xoff_cell_threshold_downlink_ports);
            soc_reg32_set(unit, PGCELLLIMITr, port, index, rval);
        }
        /* PGCELLLIMITr, index 7 */
        index = 7;
        soc_reg32_get(unit, PGCELLLIMITr, port, index, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = xoff_cell_threshold_downlink_ports;
        } else if (speed == 1000) {
            val = xoff_cell_thresholds_per_port_1g_port;
        } else if (speed == 2500) {
            val = xoff_cell_thresholds_per_port_2dot5g_port;
        } else if (speed == 10000) {
            val = xoff_cell_thresholds_per_port_10g_port;
        } else if (speed == 20000) {
            val = xoff_cell_thresholds_per_port_20g_port;
        } else if (speed == 25000) {
            val = xoff_cell_thresholds_per_port_25g_port;
        } else if (speed == 40000) {
            val = xoff_cell_thresholds_per_port_40g_port;
        } else if (speed == 50000) {
            val = xoff_cell_thresholds_per_port_50g_port;
        } else {
            val = xoff_cell_thresholds_per_port_100g_port;
        }
        soc_reg_field_set(unit, PGCELLLIMITr, &rval, CELLRESETLIMITf, val);
        soc_reg32_set(unit, PGCELLLIMITr, port, index, rval);

        /* PGDISCARDSETLIMITr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, PGDISCARDSETLIMITr, port, index, &rval);
            if (speed <= 40000) {
                soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                                  DISCARDSETLIMITf,
                                  discard_limit_per_port_pg_uplink_port);
            } else {
                soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                                  DISCARDSETLIMITf,
                                  discard_limit_per_port_pg_downlink_port);
            }
            soc_reg32_set(unit, PGDISCARDSETLIMITr, port, index, rval);
        }
        /* PGDISCARDSETLIMITr, index 7 */
        index = 7;
        soc_reg32_get(unit, PGDISCARDSETLIMITr, port, index, &rval);
        if (IS_CPU_PORT(unit, port)) {
            val = discard_limit_per_port_pg_downlink_port;
        } else if (speed == 1000) {
            val = discard_limit_per_port_pg_1g_port;
        } else if (speed == 2500) {
            val = discard_limit_per_port_pg_2dot5g_port;
        } else if (speed == 10000) {
            val = discard_limit_per_port_pg_10g_port;
        } else if (speed == 20000) {
            val = discard_limit_per_port_pg_20g_port;
        } else if (speed == 25000) {
            val = discard_limit_per_port_pg_25g_port;
        } else if (speed == 40000) {
            val = discard_limit_per_port_pg_40g_port;
        } else if (speed == 50000) {
            val = discard_limit_per_port_pg_50g_port;
        } else {
            val = discard_limit_per_port_pg_100g_port;
        }
        soc_reg_field_set(unit, PGDISCARDSETLIMITr, &rval,
                          DISCARDSETLIMITf, val);
        soc_reg32_set(unit, PGDISCARDSETLIMITr, port, index, rval);
    }

    SOC_PBMP_ITER(pbmp_1g, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_1g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_1g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_1g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }

        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_1g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_1g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch
                            + egress_xq_min_reserve_lossless_1g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_1g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_1g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_1g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                  egress_queue_min_reserve_lossless_1g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_1g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_1g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                            egress_queue_min_reserve_lossless_1g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_2dot5g, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_2dot5g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_2dot5g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_2dot5g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }

            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_2dot5g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_2dot5g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_2dot5g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_2dot5g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_2dot5g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_2dot5g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                  egress_queue_min_reserve_lossless_2dot5g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }
            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_2dot5g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_2dot5g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                egress_queue_min_reserve_lossless_2dot5g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_10g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }
        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_10g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_10g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_10g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_10g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }
        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_10g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_10g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_10g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                          port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_10g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }

            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_10g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                  0);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_10g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_10g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 63; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_10g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }
            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_10g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }
            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_10g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_10g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_10g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_10g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossless_10g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_10g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_10g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                egress_queue_min_reserve_lossless_10g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_20g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }
        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_20g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_20g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_20g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_20g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_20g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_20g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_20g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_20g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_20g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_20g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          &rval);
            val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_20g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_20g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }
            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_20g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }
            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_20g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_20g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_20g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_20g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossless_20g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_20g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_20g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                egress_queue_min_reserve_lossless_20g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_25g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }
        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_25g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_25g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_25g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_25g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_25g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_25g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_25g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_25g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_25g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_25g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          &rval);
            val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_25g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_25g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }
            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_25g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_25g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_25g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_25g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_25g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                  egress_queue_min_reserve_lossless_25g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }
            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_25g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_25g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                egress_queue_min_reserve_lossless_25g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_40g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_40g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_40g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_40g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_40g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_40g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_40g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_40g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_40g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_40g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_40g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          &rval);
            val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_40g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }

            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_40g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                  val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_40g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                              val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_40g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                              val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }

            /* CNGPORTPKTLIMIT0r, index 0 */
            soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
            soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                              numxqs_per_2kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                        numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_40g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_40g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_40g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossless_40g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_40g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_40g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossless_40g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_50g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }
        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_50g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_50g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_50g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_50g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_50g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_50g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_50g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_50g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_50g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_50g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          &rval);
            val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_50g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_50g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_50g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_50g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_50g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_50g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_50g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                  egress_queue_min_reserve_lossless_50g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_50g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_50g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossless_50g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_100g_6kxq, port) {
        /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossy);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }
        /* HOLCOSMINXQCNT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval, HOLCOSMINXQCNTf,
                          egress_xq_min_reserve_lossless_100g_port);
        soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf, 0);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_100g_port;
        soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval, PKTSETLIMITf,
                          val);
        soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
        val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
              + egress_xq_min_reserve_lossless_100g_port - 1;
        soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                          PKTRESETLIMITf, val);
        soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch
                  + egress_xq_min_reserve_lossy - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                              CNGPKTSETLIMIT0f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
        for (index = 0; index <= 63; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                              CNGPKTSETLIMIT1f,
                              numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_6kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_6kxq_port_100g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLSETLIMITf,
                          egress_queue_min_reserve_lossless_100g_port);
        soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                          CELLRESETLIMITf,
                          egress_queue_min_reserve_lossless_100g_port);
        soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

        /* LWMCOSCELLSETLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossy);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
        for (index = 0; index <= 6; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
        index = 7;
        soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr,port, index, &rval);
        val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                              egress_queue_min_reserve_lossless_100g_port;
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXLIMITf, val);
        soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                          CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
        soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

        /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
        for (index = 8; index <= 63; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_lossy;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
        for (index = 0; index <= 1; index++) {
            soc_reg32_get(unit, COLOR_DROP_EN_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                            COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                              PKTSETLIMITf, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, port, index, rval);
        }

        /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                              PKTRESETLIMITf,
                              numxqs_per_6kxq_uplink_ports - 1 - 1);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                              CNGPKTSETLIMIT0f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                              CNGPKTSETLIMIT1f, numxqs_per_6kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXLIMITf, total_advertised_cell_memory - 1);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                              CELLMAXRESUMELIMITf,
                              total_advertised_cell_memory - 1 -
                              ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, port, index, rval);
        }

        /* COLOR_DROP_EN_QGROUPr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                          COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0-7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                              SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* SHARED_POOL_CTRL_EXT1r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

        /* SHARED_POOL_CTRL_EXT2r */
        soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
        soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                        DYNAMIC_COS_DROP_ENf, 0xffffffff);
        soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_get(unit, E2ECC_PORT_CONFIGr, port, 0, &rval);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_CELL_ENf, 0);
        soc_reg_field_set(unit, E2ECC_PORT_CONFIGr, &rval, COS_PKT_ENf, 0);
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, rval);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    SOC_PBMP_ITER(pbmp_100g_2kxq, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* HOLCOSMINXQCNT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf,
                                  egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSMINXQCNT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                              HOLCOSMINXQCNTf,
                              egress_xq_min_reserve_lossless_100g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);

            /* HOLCOSMINXQCNT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNT_QLAYERr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNT_QLAYERr, &rval,
                                  HOLCOSMINXQCNTf, 0);
                soc_reg32_set(unit, HOLCOSMINXQCNT_QLAYERr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_100g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTSETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, &rval,
                                  PKTSETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch +
                      egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                          &rval);
            val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch +
                  egress_xq_min_reserve_lossless_100g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSPKTRESETLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch- 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, &rval,
                                  PKTRESETLIMITf, val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* CNGCOSPKTLIMIT0_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QLAYERr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QLAYERr, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QLAYERr, index 0 ~ 63 */
            for (index = 0; index <= 63; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QLAYERr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QLAYERr, port, index, rval);
            }
        } else {
            /* HOLCOSMINXQCNTr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
                soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                                egress_xq_min_reserve_lossy);
                soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
            }
            /* HOLCOSMINXQCNTr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                            egress_xq_min_reserve_lossless_100g_port);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);

            /* HOLCOSPKTSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_100g_port;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval, PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr, port, index, rval);

            /* HOLCOSPKTRESETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
                val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch
                    + egress_xq_min_reserve_lossy - 1;
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                                val);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);
            }

            /* HOLCOSPKTRESETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr, port, index, &rval);
            val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch
                + egress_xq_min_reserve_lossless_100g_port - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval, PKTRESETLIMITf,
                            val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr, port, index, rval);

            /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
            }

            /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                                numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
            }
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        val = shared_xqs_per_2kxq_port_100g - skidmarker - prefetch;
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf, val);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          val - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* LWMCOSCELLSETLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf,
                                  egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* LWMCOSCELLSETLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLSETLIMITf,
                              egress_queue_min_reserve_lossless_100g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                              CELLRESETLIMITf,
                              egress_queue_min_reserve_lossless_100g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index, rval);

            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLSETLIMITf, 0);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, &rval,
                                  CELLRESETLIMITf, 0);
                soc_reg32_set(unit, LWMCOSCELLSETLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio) +
                      egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                  egress_queue_min_reserve_lossless_100g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index, rval);

            /* HOLCOSCELLMAXLIMIT_QLAYERr, index 8 ~ 63 */
            for (index = 8; index <= 63; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              &rval);
                val = fl_ceiling_func(shared_space_cells,
                                      queue_port_limit_ratio);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, &rval,
                                  CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QLAYERr, port, index,
                              rval);
            }
        } else {
            /* LWMCOSCELLSETLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                                egress_queue_min_reserve_lossy);
                soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
            }

            /* LWMCOSCELLSETLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                            egress_queue_min_reserve_lossless_100g_port);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                            egress_queue_min_reserve_lossless_100g_port);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);

            /* HOLCOSCELLMAXLIMITr, index 0 ~ 6 */
            for (index = 0; index <= 6; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
                val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                    egress_queue_min_reserve_lossy;
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXLIMITf, val);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                                CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
            }

            /* HOLCOSCELLMAXLIMITr, index 7 */
            index = 7;
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                egress_queue_min_reserve_lossless_100g_port;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                            CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr, port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr, port, 0, rval);

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* COLOR_DROP_EN_QLAYERr, index 0 ~ 1 */
            for (index = 0; index <= 1; index++) {
                soc_reg_field_set(unit, COLOR_DROP_EN_QLAYERr, &rval,
                                  COLOR_DROP_ENf, 0);
                soc_reg32_set(unit, COLOR_DROP_EN_QLAYERr,
                              port, index, rval);
            }

            /* HOLCOSPKTSETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTSETLIMIT_QGROUPr, &rval,
                                  PKTSETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, HOLCOSPKTSETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSPKTRESETLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr, &rval,
                                  PKTRESETLIMITf,
                                  numxqs_per_2kxq_uplink_ports - 1 - 1);
                soc_reg32_set(unit, HOLCOSPKTRESETLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT0_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT0_QGROUPr, &rval,
                                  CNGPKTSETLIMIT0f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT0_QGROUPr,
                              port, index, rval);
            }

            /* CNGCOSPKTLIMIT1_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, CNGCOSPKTLIMIT1_QGROUPr, &rval,
                                  CNGPKTSETLIMIT1f,
                                  numxqs_per_2kxq_uplink_ports - 1);
                soc_reg32_set(unit, CNGCOSPKTLIMIT1_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXLIMITf,
                                  total_advertised_cell_memory - 1);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* HOLCOSCELLMAXLIMIT_QGROUPr, index 0 ~ 7 */
            for (index = 0; index <= 7; index++) {
                soc_reg32_get(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, &rval);
                soc_reg_field_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr, &rval,
                                  CELLMAXRESUMELIMITf,
                                  total_advertised_cell_memory - 1 -
                                  ethernet_mtu_cell);
                soc_reg32_set(unit, HOLCOSCELLMAXLIMIT_QGROUPr,
                              port, index, rval);
            }

            /* COLOR_DROP_EN_QGROUPr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_EN_QGROUPr, &rval,
                              COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_EN_QGROUPr,
                          port, 0, rval);
        } else {
            /* COLOR_DROP_ENr, index 0 */
            soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
            soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
            soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);
        }

        /* SHARED_POOL_CTRLr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        if (mmu_port >= (SOC_MAX_MMU_PORTS - 8)) {
            /* SHARED_POOL_CTRL_EXT1r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT1r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT1r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT1r, port, 0, rval);

            /* SHARED_POOL_CTRL_EXT2r */
            soc_reg32_get(unit, SHARED_POOL_CTRL_EXT2r, port, 0, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRL_EXT2r, &rval,
                            DYNAMIC_COS_DROP_ENf, 0xffffffff);
            soc_reg32_set(unit, SHARED_POOL_CTRL_EXT2r, port, 0, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    /* port-based : cpu port*/
    SOC_PBMP_ITER(pbmp_cpu, port) {
        /* HOLCOSMINXQCNTr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSMINXQCNTr, port, index, &rval);
            soc_reg_field_set(unit, HOLCOSMINXQCNTr, &rval, HOLCOSMINXQCNTf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, HOLCOSMINXQCNTr, port, index, rval);
        }

        /* HOLCOSPKTSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTSETLIMITr,
                          port, index, &rval);
            val = shared_xqs_cpu_port - skidmarker - prefetch +
                  egress_queue_min_reserve_cpu_ports;
            soc_reg_field_set(unit, HOLCOSPKTSETLIMITr, &rval,
                              PKTSETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTSETLIMITr,
                          port, index, rval);
        }

        /* HOLCOSPKTRESETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSPKTRESETLIMITr,
                          port, index, &rval);
            val = shared_xqs_cpu_port - skidmarker - prefetch +
                  egress_queue_min_reserve_cpu_ports - 1;
            soc_reg_field_set(unit, HOLCOSPKTRESETLIMITr, &rval,
                              PKTRESETLIMITf, val);
            soc_reg32_set(unit, HOLCOSPKTRESETLIMITr,
                          port, index, rval);
        }

        /* CNGCOSPKTLIMIT0r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT0r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT0r, &rval, CNGPKTSETLIMIT0f,
                              numxqs_per_2kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT0r, port, index, rval);
        }

        /* CNGCOSPKTLIMIT1r, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, CNGCOSPKTLIMIT1r, port, index, &rval);
            soc_reg_field_set(unit, CNGCOSPKTLIMIT1r, &rval, CNGPKTSETLIMIT1f,
                              numxqs_per_2kxq_uplink_ports - 1);
            soc_reg32_set(unit, CNGCOSPKTLIMIT1r, port, index, rval);
        }

        /* CNGPORTPKTLIMIT0r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT0r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT0r, &rval, CNGPORTPKTLIMIT0f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT0r, port, 0, rval);

        /* CNGPORTPKTLIMIT1r, index 0 */
        soc_reg32_get(unit, CNGPORTPKTLIMIT1r, port, 0, &rval);
        soc_reg_field_set(unit, CNGPORTPKTLIMIT1r, &rval, CNGPORTPKTLIMIT1f,
                          numxqs_per_2kxq_uplink_ports - 1);
        soc_reg32_set(unit, CNGPORTPKTLIMIT1r, port, 0, rval);

        /* DYNXQCNTPORTr, index 0 */
        soc_reg32_get(unit, DYNXQCNTPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNXQCNTPORTr, &rval, DYNXQCNTPORTf,
                          shared_xqs_cpu_port - skidmarker - prefetch);
        soc_reg32_set(unit, DYNXQCNTPORTr, port, 0, rval);

        /* DYNRESETLIMPORTr, index 0 */
        soc_reg32_get(unit, DYNRESETLIMPORTr, port, 0, &rval);
        soc_reg_field_set(unit, DYNRESETLIMPORTr, &rval, DYNRESETLIMPORTf,
                          shared_xqs_cpu_port - skidmarker - prefetch - 2);
        soc_reg32_set(unit, DYNRESETLIMPORTr, port, 0, rval);

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLSETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
        }

        /* LWMCOSCELLSETLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, LWMCOSCELLSETLIMITr, port, index, &rval);
            soc_reg_field_set(unit, LWMCOSCELLSETLIMITr, &rval, CELLRESETLIMITf,
                              egress_queue_min_reserve_cpu_ports);
            soc_reg32_set(unit, LWMCOSCELLSETLIMITr, port, index, rval);
        }

        /* HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, HOLCOSCELLMAXLIMITr, port, index, &rval);
            val = fl_ceiling_func(shared_space_cells, queue_port_limit_ratio) +
                                  egress_queue_min_reserve_cpu_ports;
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXLIMITf, val);
            soc_reg_field_set(unit, HOLCOSCELLMAXLIMITr, &rval,
                              CELLMAXRESUMELIMITf, val - ethernet_mtu_cell);
            soc_reg32_set(unit, HOLCOSCELLMAXLIMITr, port, index, rval);
        }

        /* DYNCELLLIMITr, index 0 */
        soc_reg32_get(unit, DYNCELLLIMITr,
                      port, 0, &rval);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLSETLIMITf,
                          shared_space_cells);
        soc_reg_field_set(unit, DYNCELLLIMITr, &rval, DYNCELLRESETLIMITf,
                          shared_space_cells - ethernet_mtu_cell * 2);
        soc_reg32_set(unit, DYNCELLLIMITr,
                      port, 0, rval);

        /* COLOR_DROP_ENr, index 0 */
        soc_reg32_get(unit, COLOR_DROP_ENr, port, 0, &rval);
        soc_reg_field_set(unit, COLOR_DROP_ENr, &rval, COLOR_DROP_ENf, 0);
        soc_reg32_set(unit, COLOR_DROP_ENr, port, 0, rval);

        /* SHARED_POOL_CTRLr, index 0 ~ 7*/
        for (index = 0; index <= 7; index++) {
            soc_reg32_get(unit, SHARED_POOL_CTRLr, port, index, &rval);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            DYNAMIC_COS_DROP_ENf, 255);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_DISCARD_ENf, 0);
            soc_reg_field_set(unit, SHARED_POOL_CTRLr, &rval,
                            SHARED_POOL_XOFF_ENf, 0);
            soc_reg32_set(unit, SHARED_POOL_CTRLr, port, index, rval);
        }

        /* E2ECC_PORT_CONFIGr, index 0 */
        soc_reg32_set(unit, E2ECC_PORT_CONFIGr, port, 0, 0);

        /* EARLY_DYNCELLLIMITr, index 0 */
        soc_reg32_set(unit, EARLY_DYNCELLLIMITr, port, 0, 0);

        /* EARLY_HOLCOSCELLMAXLIMITr, index 0 ~ 7 */
        for (index = 0; index <= 7; index++) {
            soc_reg32_set(unit, EARLY_HOLCOSCELLMAXLIMITr, port, index, 0);
        }
    }

    return SOC_E_NONE;
}


#define  FL_NUM_COS  8


STATIC int
_soc_firelight_mmu_init(int unit)
{
    int         cos;
    soc_port_t  port;
    uint32      val, oval, cfap_max_idx;
    int         age[FL_NUM_COS], max_age, min_age;
    int         age_enable, disabled_age;
    uint16      dev_id;
    uint8       rev_id;
    soc_info_t *si = &SOC_INFO(unit);
    soc_pbmp_t mmu_pbmp;
    int phy_port;
    int mmu_port;
    int qgroup;
    int active_backplane_pbmp_count, pbmp_lossless_count;
    int bump_on_wire;
    soc_pbmp_t active_backplane_pbmp, pbmp_lossless;
    int idx;
    egr_edb_xmit_ctrl_entry_t edb_xmit_ctrl_entry;


    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((dev_id & 0xF000) == 0x8000) {
        dev_id &= 0xFF0F;
    }

    SOC_IF_ERROR_RETURN(_soc_firelight_autotdm_init(unit));

    cfap_max_idx = FL_MMU_CBP_FULL_SIZE;

    SOC_IF_ERROR_RETURN(READ_CFAPCONFIGr(unit, &val));
    oval = val;
    soc_reg_field_set(unit, CFAPCONFIGr, &val, CFAPPOOLSIZEf, cfap_max_idx);
    if (oval != val) {
        SOC_IF_ERROR_RETURN(WRITE_CFAPCONFIGr(unit, val));
    }

    /* Enable IP to CMICM credit transfer */
    val = 0;
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr,
                      &val, TRANSFER_ENABLEf, 1);
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr,
                      &val, NUM_OF_CREDITSf, 32);
    SOC_IF_ERROR_RETURN(WRITE_IP_TO_CMICM_CREDIT_TRANSFERr(unit, val));

    for (idx = 0; idx <= soc_mem_index_max(unit, EGR_EDB_XMIT_CTRLm); idx++) {
        sal_memset(&edb_xmit_ctrl_entry, 0, sizeof(egr_edb_xmit_ctrl_entry_t));
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, &edb_xmit_ctrl_entry,
                            WAIT_FOR_MOPf, 1);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, &edb_xmit_ctrl_entry,
                            MIN_NON_FINAL_FRAG_SIZEf, 0xf);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, &edb_xmit_ctrl_entry,
                            MIN_FINAL_FRAG_SIZEf, 0x10);
        SOC_IF_ERROR_RETURN(WRITE_EGR_EDB_XMIT_CTRLm(unit, MEM_BLOCK_ALL, idx,
                                                     &edb_xmit_ctrl_entry));
    }

    bump_on_wire = soc_property_get(unit, spn_MMU_BUMP_IN_THE_WIRE, 0);

    active_backplane_pbmp_count = 0;
    active_backplane_pbmp = soc_property_get_pbmp(unit, spn_ACTIVE_BACKPLANE_PBMP, 0);
    SOC_PBMP_COUNT(active_backplane_pbmp, active_backplane_pbmp_count);

    if (active_backplane_pbmp_count) {
        /* Chassis mode */
        SOC_IF_ERROR_RETURN(_soc_firelight_mmu_init_helper_chassis(unit));
    } else if (bump_on_wire) {
        /* Bump on wire */
        SOC_IF_ERROR_RETURN(_soc_firelight_mmu_init_bump_on_wire(unit));
    } else {
        /* Standalone mode */
        if (soc_property_get(unit, spn_MMU_LOSSLESS, 0)) {
            SOC_PBMP_CLEAR(pbmp_lossless);
            pbmp_lossless = soc_property_get_pbmp_default(unit, spn_MMU_LOSSLESS_PBMP,
                                                          pbmp_lossless);
            SOC_PBMP_COUNT(pbmp_lossless, pbmp_lossless_count);
            if ((pbmp_lossless_count > 0) && (pbmp_lossless_count <= 24)) {
                /* Lossy+Lossless(PFC) mode. Maximum 24 support ports lossless. */
                SOC_IF_ERROR_RETURN(_soc_firelight_mmu_init_helper_standalone_lossless_pfc(unit));
            } else {
                /* Lossless(Pause) mode. Legacy lossless mode.*/
                SOC_IF_ERROR_RETURN(_soc_firelight_mmu_init_helper_standalone_lossless_pause(unit));
            }
        } else {
            /* lossy */
            SOC_IF_ERROR_RETURN(_soc_firelight_mmu_init_helper_standalone_lossy(unit));
        }
    }

    /* Enable E2EFC in mmu block */
    SOC_IF_ERROR_RETURN(READ_E2EFC_IBP_ENr(unit, &val));
    soc_reg_field_set(unit, E2EFC_IBP_ENr,
                      &val, SEND_RX_E2E_BKP_ENf, 1);
    soc_reg_field_set(unit, E2EFC_IBP_ENr,
                      &val, ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_E2EFC_IBP_ENr(unit, val));

    /*
     * Configure per-XQ packet aging for the various COSQs.
     *
     * The shortest age allowed by H/W is 250 microseconds.
     * The longest age allowed is 7.162 seconds (7162 msec).
     * The maximum ratio between the longest age and the shortest
     * (nonzero) age is 7:2.
     */
    age_enable = disabled_age = max_age = 0;
    min_age = 7162;
    for (cos = 0; cos < NUM_COS(unit); cos++) {
        if ((age[cos] =
             soc_property_suffix_num_get(unit, cos, spn_MMU_XQ_AGING,
                                         "cos",  0)) > 0) {
            age_enable = 1;
            if (age[cos] > 7162) {
                age[cos] = 7162;
            }
            if (age[cos] < min_age) {
                min_age = age[cos];
            }
        } else {
            disabled_age = 1;
            age[cos] = 0;
        }
        if (age[cos] > max_age) {
            max_age = age[cos];
        }
    }
    if (!age_enable) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        for (qgroup = 0; qgroup < 8; qgroup++) {
            SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT_r(unit, qgroup, 0));
        }
    } else {
        uint32 regval = 0;
        uint32 timerval;

        /* Enforce the 7:2 ratio between min and max values */
        if ((((max_age * 2) + 6) / 7) > min_age) {
            /* Keep requested max age; make min_age comply */
            min_age = ((max_age * 2) + 6) / 7;
        }

        /*
         * Give up granularity for range, if we need to
         * "disable" (max out) aging for any COSQ(s).
         */
        if (disabled_age) {
            /* Max range */
            max_age = min_age * 7 / 2;
        }

        /*
         * Compute shortest duration of one PKTAGINGTIMERr cycle.
         * This duration is 1/7th of the longest packet age.
         * This duration is in units of 125 usec (msec * 8).
         */
        timerval = ((8 * max_age) + 6) / 7;
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, timerval));

        /* rPKTAGINGTIMER value 0 means aging disabled */
        if (timerval != 0) {
            for (cos = 0; cos < NUM_COS(unit); cos++) {
                if (!age[cos]) {
                    /*
                     * Requested to be disabled, but cannot disable individual
                     * COSQs once packet aging is enabled. Therefore, mark
                     * this COSQ's aging duration as maxed out.
                     */
                    age[cos] = -1;
                } else if (age[cos] < min_age) {
                    age[cos] = min_age;
                }

                /* Normalize each "age" into # of PKTAGINGTIMERr cycles. */
                if (age[cos] > 0) {
                    age[cos] = ((8 * age[cos]) + timerval - 1) / timerval;
                }
                else {
                    age[cos] = 7;
                }
                /* Format each "age" for its appropriate field */
                regval |= ((7 - age[cos]) << (cos * 3));
            }
        } else {
            regval = 0;
        }
        for (qgroup = 0; qgroup < 8; qgroup++) {
            SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT_r(unit, qgroup, regval));
        }
    }

    /*
     * MMU Port enable
     */
    SOC_PBMP_CLEAR(mmu_pbmp);
    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        SOC_PBMP_PORT_ADD(mmu_pbmp, mmu_port);
    }
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLE_0r, &val, MMUPORTENABLEf,
                        SOC_PBMP_WORD_GET(mmu_pbmp, 0));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLE_0r(unit, val));
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLE_1r, &val, MMUPORTENABLEf,
                        SOC_PBMP_WORD_GET(mmu_pbmp, 1));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLE_1r(unit, val));
    val = 0;
    soc_reg_field_set(unit, MMUPORTENABLE_2r, &val, MMUPORTENABLEf,
                        SOC_PBMP_WORD_GET(mmu_pbmp, 2));
    SOC_IF_ERROR_RETURN(WRITE_MMUPORTENABLE_2r(unit, val));

    return SOC_E_NONE;
}

/*
 *  PM4X10_QTC core has two work modes: Q(QTC) mode and Ethernet(XLPORT) mode.
 *  In QTC mode, QTC is reset via register PMQ_XGXS0_CTRL_REGr which is belong to SOC_BLK_PMQPORT block.
 *  In Eth mode, TSC is reset via register XLPORT_XGXS0_CTRL_REGr which is belong to SOC_BLK_XLPORT block.
 *  To reset QTC or TSC core, the work mode must be identified by register CHIP_CONFIGr which is configured in soc_firelight_chip_reset.
 */
int
soc_firelight_tsc_reset(int unit)
{
    int blk, port;
    uint32 rval;
    uint64 rval64;
    uint32 index, qtc_mode = ETHER_MODE;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    /* Power off CLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on CLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));
    }

    /* Power off XLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on XLPORT blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (IS_QSGMII_PORT(unit, port)) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, XLPORT_XGXS0_CTRL_REGr, port, 0, &rval64));
        soc_reg64_field32_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval64, REFSELf, 5);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, XLPORT_XGXS0_CTRL_REGr, port, 0, rval64));
    }

    /* Power off PMQ blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PMQPORT) {
        port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        index = SOC_BLOCK_NUMBER(unit, blk);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, index, 0, &rval64));
        qtc_mode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);
        if (qtc_mode == Q_MODE) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 1));
        }
    }
    sal_usleep(sleep_usec + 10000);
    /* Power on PMQ blocks */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PMQPORT) {
        port = blk | SOC_REG_ADDR_BLOCK_ID_MASK;
        index = SOC_BLOCK_NUMBER(unit, blk);

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, index, 0, &rval64));
        qtc_mode = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);
        if (qtc_mode == Q_MODE) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_power_mode(unit, port, 0, 0));

            SOC_IF_ERROR_RETURN(soc_reg_get(unit, PMQ_XGXS0_CTRL_REGr, port, 0, &rval64));
            soc_reg64_field32_set(unit, PMQ_XGXS0_CTRL_REGr, &rval64, REFSELf, 5);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, PMQ_XGXS0_CTRL_REGr, port, 0, rval64));
        }
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        if (soc_reg_field_valid(unit, CLPORT_MAC_CONTROLr, SYS_16B_INTF_MODEf) &&
            soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
            soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, SYS_16B_INTF_MODEf, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    return SOC_E_NONE;
}

#define INVALID_PPORT_ID    -1
/*
 * Func : _soc_firelight_mdio_addr_to_port
 *  - To report pport_id from given phy_addr(for INT-PHY)
 *
 * INFO :
 *  - This is HW dependent design in TOP Spec.
 *      1. Int-MDIO-Bus1 : TSCF0 ~ TSCF3 (each for 4 ports)
 *      2. Int-MDIO-Bus2 :
 *          2.1. TSCE0 ~ TSCE2 (each for 4 ports)
 *          2.2. QTC0 ~ QTC2 (each for 16 ports) in QMODE
 *  - Param of "phy_addr" here has been constructed by phymod driver.
 */
STATIC int
_soc_firelight_mdio_addr_to_port(int unit, uint32 phy_addr_int)
{
    int bus, offset = 0;
    int mdio_addr;
    soc_info_t *si;

    /* Must be internal MDIO address */
    if ((phy_addr_int & 0x80) == 0) {
        return INVALID_PPORT_ID;
    }

    bus = PHY_ID_BUS_NUM(phy_addr_int);
    mdio_addr = phy_addr_int & 0x1f;

    if (bus == 1) {
        /* for TSCF0 ~ TSCF3 */
        if (mdio_addr <= 16) {
            offset = 61;
        } else {
            return INVALID_PPORT_ID;
        }
    } else if (bus == 2) {
        si = &SOC_INFO(unit);

        if (mdio_addr <= 4) {
            if (si->port_p2l_mapping[2] != -1 ) {
                /* for QTC0 */
                offset = 1;
            } else {
                /* for TSCE0 */
                offset = 49;
            }
        } else if (mdio_addr <= 8) {
            if (si->port_p2l_mapping[18] != -1 ) {
                /* for QTC1 */
                offset = 13;
            } else {
                /* for TSCE1 */
                offset = 49;
            }
        } else if (mdio_addr <= 12) {
            if (si->port_p2l_mapping[34] != -1 ) {
                /* for QTC2 */
                offset = 25;
            } else {
                /* for TSCE2 */
                offset = 49;
            }
        } else {
            return INVALID_PPORT_ID;
        }
    } else {
        return INVALID_PPORT_ID;
    }

    return mdio_addr + offset;
}

/*
 * PHY_REG address decode for TSCx phy access :
 *  - TSC_ADDR[31:0]={DEVICE(ADVAD)[31:27],??[26:24],TSC_ID(PHYAD)[23:19],
 *      PORT_MODE(LANE)[18:16], IEEE_BIT[15], BLOCK[14:4], REG[3:0]}
 */
#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

/*
 * Func :  soc_firelight_sbus_tsc_block()
 *  - to report the valid blk of this port for wc_ucmem_data
 *      (i.e. PMQPORT/XLPORT/CLPORT_WC_UCMEM_DATAm)access process.
 *
 *  - Parameters :
 *      phy_port(IN) : physical port.
 *      blk(OUT) : the valid block
 */
int
soc_firelight_sbus_tsc_block(int unit, int phy_port, int *blk)
{
    int blk_i, blktype, port;
    int qmod = ETHER_MODE, inst;
    uint64 rval64;

    *blk = -1;
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    *blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_firelight_sbus_tsc_block[%d]: %d/%d/%d\n"),
              unit, phy_port, port, *blk));

    if(phy_port <= 49){
        inst = (phy_port - 2) / 16;
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, CHIP_CONFIGr, inst, 0, &rval64));
        qmod = soc_reg64_field32_get(unit, CHIP_CONFIGr, rval64, QMODEf);

        for (blk_i = 0; SOC_BLOCK_INFO(unit, blk_i).type >= 0; blk_i++) {
            blktype = SOC_BLOCK_INFO(unit, blk_i).type;
            if ((blktype == (qmod ? SOC_BLK_PMQPORT : -1)) && (SOC_BLOCK_INFO(unit, blk_i).number == inst)) {
                *blk = blk_i;
                break;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_firelight_sbus_tsc_block[%d]: %d/%d/%d\n"),
              unit, phy_port, port, *blk));
    if(*blk == -1) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

/*
 * Func : _soc_firelight_tscx_reg_read
 *  - FL function to serve SBus MDIO read on TSCx
 */
STATIC int
_soc_firelight_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_firelight_mdio_addr_to_port(unit, phy_addr);

    if (phy_port == INVALID_PPORT_ID) {
        LOG_WARN(BSL_LS_SOC_MII, (BSL_META_U(unit,
                "unit%d: phy_addr=%d is unable for Sbus-MDIO read!\n"),
                unit, phy_addr));
        return SOC_E_PARAM;
    }

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];

    SOC_IF_ERROR_RETURN
                (soc_firelight_sbus_tsc_block(unit, phy_port, &blk));

    LOG_INFO(BSL_LS_SOC_MII, (BSL_META_U(unit,
         "_soc_firelight_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
         unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);

    return rv;
}

/*
 * Func : _soc_firelight_tscx_reg_write
 *  - FL function to serve SBus MDIO write on TSCx
 */
STATIC int
_soc_firelight_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_firelight_mdio_addr_to_port(unit, phy_addr);

    if (phy_port == INVALID_PPORT_ID) {
        LOG_WARN(BSL_LS_SOC_MII, (BSL_META_U(unit,
                "unit%d: phy_addr=%d is unable for Sbus-MDIO write!\n"),
                unit, phy_addr));
        return SOC_E_PARAM;
    }

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];

    SOC_IF_ERROR_RETURN
                (soc_firelight_sbus_tsc_block(unit, phy_port, &blk));

    LOG_INFO(BSL_LS_SOC_MII, (BSL_META_U(unit,
         "_soc_firelight_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
         unit, phy_addr, phy_port, port, blk));
    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

/*
 * Func : soc_firelight_pgw_encap_field_get
 *  - To write to PGW interface on GX/XL/CL port on indicated field
 */
int
soc_firelight_pgw_encap_field_get(int unit,
                                soc_port_t lport,
                                soc_field_t field,
                                uint32 *val)
{
    uint32  reg_val = 0;
    int     pport = SOC_INFO(unit).port_l2p_mapping[lport];

    *val = 0;
    if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_XLPORT) {
        SOC_IF_ERROR_RETURN(READ_PGW_XL_CONFIGr(unit, lport, &reg_val));
        *val = soc_reg_field_get(unit, PGW_XL_CONFIGr, reg_val, field);
    } else if (SOC_PORT_BLOCK_TYPE(unit, pport) == SOC_BLK_CLPORT) {
        SOC_IF_ERROR_RETURN(READ_PGW_CL_CONFIGr(unit, lport, &reg_val));
        *val = soc_reg_field_get(unit, PGW_CL_CONFIGr, reg_val, field);
    } else {
        SOC_IF_ERROR_RETURN(READ_PGW_GE_CONFIGr(unit, lport, &reg_val));
        *val = soc_reg_field_get(unit, PGW_GE_CONFIGr, reg_val, field);

    }

    return SOC_E_NONE;
}

/*
 * Interrupt handler functions
 */
STATIC soc_fl_oam_ser_handler_t
fl_oam_ser_handler[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC soc_fl_oam_handler_t fl_oam_handler[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC soc_fl_tsn_event_handler_t fl_intr_sr_h[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC soc_fl_cosq_event_handler_t
fl_intr_tas_h[SOC_MAX_NUM_DEVICES] = {NULL};
soc_fl_taf_event_handler_t
fl_intr_taf_h[SOC_MAX_NUM_DEVICES] = {NULL};
STATIC soc_fl_macsec_handler_t
fl_intr_macsec_h[SOC_MAX_NUM_DEVICES] = {NULL};

STATIC soc_ser_functions_t fl_ser_functions[SOC_MAX_NUM_DEVICES];
static uint8 fl_event_handler_init[SOC_MAX_NUM_DEVICES] = { 0 };

/* Interrupt types on Firelight */
#define FL_INTR_PARITY         (1)
#define FL_INTR_L2_OVERFLOW    (2)
#define FL_INTR_OAM            (3)
#define FL_INTR_TAS            (4)
#define FL_INTR_SR             (5)
#define FL_INTR_TAF            (6)
#define FL_INTR_MACSEC         (7)

#define FL_LP_INTR_REG0           (0)
#define FL_LP_INTR_REG1           (1)

/* FL_INTR_PARITY: ICFG_CHIP_LP_INTR_STATUS_REG0r bit 0 -18 */
#define FL_REG0_SER_INTR_MASK (0x0007FF7F)

#define FL_REG0_SER_ENGINE_INTR_MASK (0x00040000)

/* FL_INTR_L2_OVERFLOW: IP1_TO_CMIC_PERR_INTR (ICFG_CHIP_LP_INTR_STATUS_REG0r bit 3) */
#define FL_REG0_L2_OVERFLOW_INTR_MASK (0x8)

/* FL_INTR_OAM: IP2_TO_CMIC_PERR_INTR  (ICFG_CHIP_LP_INTR_STATUS_REG0r : bit 4) */
#define FL_REG0_OAM_INTR_MASK              (0x10)

/* FL_INTR_TAS: MMU_TO_CMIC_MEMFAIL_INTR (ICFG_CHIP_LP_INTR_STATUS_REG0r : bit 0) */
#define FL_REG0_TAS_INTR_MASK              (0x1)

/* FL_INTR_MACSEC: MACSEC_TO_CMIC_INTR (ICFG_CHIP_LP_INTR_STATUS_REG1r : bit 5) */
#define FL_REG1_MACSEC_INTR_MASK           (0x20)

/*
 * FL_INTR_SR:
 * EP_TO_CMIC_PERR_INTR (ICFG_CHIP_LP_INTR_STATUS_REG0r : bit 1)
 * IP2_TO_CMIC_PERR_INTR  (ICFG_CHIP_LP_INTR_STATUS_REG0r : bit 4)
 * IP2_SR_TH_TO_CMIC_INTR  (ICFG_CHIP_LP_INTR_STATUS_REG1r : bit 3)
 */
#define FL_REG0_SR_INTR_MASK               (0x12)
#define FL_REG1_SR_INTR_MASK               (0x8)

/* TAF_GSG_TO_CMIC_INTR  (ICFG_CHIP_LP_INTR_STATUS_REG1r : bit 4) */
#define FL_REG1_TAF_INTR_MASK              (0x10)

/* Fields for OAM SER and event */
soc_field_t _soc_fl_oam_interrupt_fields[] = {
    SOME_RMEP_CCM_DEFECT_INTRf,
    SOME_RDI_DEFECT_INTRf,
    ANY_RMEP_TLV_PORT_DOWN_INTRf,
    ANY_RMEP_TLV_PORT_UP_INTRf,
    ANY_RMEP_TLV_INTERFACE_DOWN_INTRf,
    ANY_RMEP_TLV_INTERFACE_UP_INTRf,
    XCON_CCM_DEFECT_INTRf,
    ERROR_CCM_DEFECT_INTRf,
    INVALIDf
};

/* Fields mask for TAS evnets bit 22,21,20,19,17,16 in MEMFAILINTSTATUSr */
#define FL_TAS_INTR_STATUS_MASK     (0x7B0000)

/* Fields mask for TAF evnets bit 22,21,20,19,17,16 in TAF_MEMFAILINTSTATUSr */
#define FL_TAF_INTR_STATUS_MASK     (0x7B0000)

/* Fields mask for SR events bit 43,44 in IP2_INTR_STATUSr */
#define FL_IP2_SR_LAN_INTR_STATUS_HI_MASK     (0x1800)

/* Fields mask for L2 overflow bit 8,9 in IP1_INTR_STATUSr */
#define FL_IP1_L2_INTR_STATUS_MASK     (0x300)

STATIC int _soc_ser_info_init = 0;
STATIC _soc_generic_ser_info_t
 *_soc_fl_tcam_ser_info[SOC_MAX_NUM_DEVICES];

#define _FL_SER_TEST_TCAM       (1)

static _soc_generic_ser_info_t _soc_fl_tcam_ser_info_template[] = {
#if _FL_SER_TEST_TCAM
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {2, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 69}, {1, 69}, {70, 138}, {71, 138} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 66}, {1, 66}, {67, 132}, {68, 132} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 77}, {1, 77}, {78, 154}, {79, 154} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 72}, {1, 72}, {73, 144}, {74, 144} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {2, 235}, {236, 452}, {237, 452} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA},
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {2, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 78}, {1, 78}, {79, 156}, {80, 156} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { SVC_METER_OFFSET_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 90}, {1, 90}, {91, 180}, {92, 180} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
#ifdef INCLUDE_XFLOW_MACSEC
    { ISEC_SC_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 78}, {1, 78}, {79, 156}, {80, 156} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
#endif
#endif /* _FL_SER_TEST_TCAM */
    /* End of list */
    { INVALIDm },
};

int
soc_fl_tcam_ser_init(int unit)
{
    int alloc_size = 0;

    /* First, make per-unit copy of the master TCAM list */
    alloc_size = sizeof(_soc_fl_tcam_ser_info_template);

    if (_soc_ser_info_init == 0) {
        int u;
        for (u = 0; u < SOC_MAX_NUM_DEVICES; u++) {
            _soc_fl_tcam_ser_info[u] = NULL;
        }
        _soc_ser_info_init = 1;
    }

    if (NULL == _soc_fl_tcam_ser_info[unit]) {
        if ((_soc_fl_tcam_ser_info[unit] =
             sal_alloc(alloc_size, "fl tcam list")) == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(_soc_fl_tcam_ser_info[unit], 0, alloc_size);
    }

    /* Make a fresh copy of the TCAM template info */
    sal_memcpy(
               _soc_fl_tcam_ser_info[unit],
               &(_soc_fl_tcam_ser_info_template),
               alloc_size);

#ifdef INCLUDE_MEM_SCAN
    soc_mem_scan_ser_list_register(unit, TRUE,
                                   _soc_fl_tcam_ser_info[unit]);
#endif
    return soc_generic_ser_init(unit, _soc_fl_tcam_ser_info[unit]);
}

void
soc_fl_ser_fail(int unit)
{
    soc_generic_ser_process_error(unit, _soc_fl_tcam_ser_info[unit],
                                  _SOC_PARITY_TYPE_SER);
}

int
soc_fl_ser_enable_all(int unit, int enable)
{
    _soc_fl_ser_route_block_t *rb;
    uint8       rbi;
    int         block_info_idx;
    int         port = REG_PORT_ANY;
    uint64      rval64;
    int         rv = SOC_E_NONE;

    /* Loop jh each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        rb = soc_fl_ser_route_blocks_get(rbi);
        if (rb->cmic_bit == 0) {
            /* End of table */
            break;
        }
        port = REG_PORT_ANY;
        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }
        if (rb->enable_reg != INVALIDr) {
            if (!SOC_REG_IS_VALID(unit, rb->enable_reg)) {
                continue;
            }
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rval64));
        }
        rv = soc_fl_ser_enable_info(unit, block_info_idx, rb->id, port,
                                      rb->enable_reg, &rval64,
                                      rb->info, INVALIDm, enable);
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rval64));
        }
    }

    /* Write CMIC enable register */
    soc_fl_ser_cmic_interrupt_enable(unit, enable);

    return SOC_E_NONE;
}

/* Helper function to config the interrupt mask */
STATIC int
soc_fl_lp_interrupt_enable(int unit, int intr_type, int enable)
{
    uint32 reg0_old, reg1_old;
    uint32 reg0, reg1;
    int rv = SOC_E_NONE;

    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r, REG_PORT_ANY, 0),
                               &reg0_old);
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r, REG_PORT_ANY, 0),
                               &reg1_old);

    reg0 = 0;
    reg1 = 0;
    switch (intr_type) {
        case FL_INTR_PARITY:
            reg0 = FL_REG0_SER_INTR_MASK;
            break;
        case FL_INTR_L2_OVERFLOW:
            reg0 = FL_REG0_L2_OVERFLOW_INTR_MASK;
            break;
        case FL_INTR_OAM:
            reg0 = FL_REG0_OAM_INTR_MASK;
            break;
        case FL_INTR_TAS:
            reg0 = FL_REG0_TAS_INTR_MASK;
            break;
        case FL_INTR_SR:
            reg0 = FL_REG0_SR_INTR_MASK;
            reg1 = FL_REG1_SR_INTR_MASK;
            break;
        case FL_INTR_MACSEC:
            reg1 = FL_REG1_MACSEC_INTR_MASK;
            break;
        case FL_INTR_TAF:
            reg1 = FL_REG1_TAF_INTR_MASK;
            break;
        default :
            return SOC_E_PARAM;
    }

    if (enable) {
        if (reg0) {
            reg0_old |= reg0;
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        REG_PORT_ANY, 0), reg0_old));
        }
        if (reg1) {
            reg1_old |= reg1;
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                        REG_PORT_ANY, 0), reg1_old));
        }
    } else {
        /*
         * Each bit in REG0 might be shared with multiple types
         * Remove bits from disable mask if it is required for
         * the valid/working types.
         */
        if (SOC_CONTROL(unit)->l2_overflow_enable) {
            /* remove l2 mask */
            reg0 &= ~FL_REG0_L2_OVERFLOW_INTR_MASK;
        }
        if (fl_intr_sr_h[unit]) {
            /* remove sr mask */
            reg0 &= ~FL_REG0_SR_INTR_MASK;
        }
        if (fl_intr_tas_h[unit]) {
            /* remove tas mask */
            reg0 &= ~FL_REG0_TAS_INTR_MASK;
        }
        if (fl_oam_handler[unit]) {
            /* remove oam mask */
            reg0 &= ~FL_REG0_OAM_INTR_MASK;
        }
        if (fl_ser_functions[unit]._soc_ser_fail_f) {
            /* remove ser mask */
            reg0 &= ~FL_REG0_SER_INTR_MASK;
        }
        if (fl_intr_taf_h[unit]) {
            /* remove taf mask */
            reg1 &= ~FL_REG1_TAF_INTR_MASK;
        }
        if (fl_intr_macsec_h[unit]) {
            /* remove macsec mask */
            reg1 &= ~FL_REG1_MACSEC_INTR_MASK;
        }

        if (reg0) {
            reg0_old &= ~ reg0;
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        REG_PORT_ANY, 0), reg0_old));
        }
        if (reg1) {
            reg1_old &= ~ reg1;
            SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                        REG_PORT_ANY, 0), reg1_old));
        }
    }
    return rv;
}

/* SER */
void
soc_fl_ser_cmic_interrupt_enable(int unit, int enable)
{
    (void)soc_fl_lp_interrupt_enable(unit, FL_INTR_PARITY, enable);
}

/* OAM SER */
void
soc_fl_oam_ser_handler_register(int unit, soc_fl_oam_ser_handler_t handler)
{
    fl_oam_ser_handler[unit] = handler;
}

int
soc_fl_oam_ser_process(int unit, soc_mem_t mem, int index)
{
    if (fl_oam_ser_handler[unit]) {
        return fl_oam_ser_handler[unit](unit, mem, index);
    } else {
        return SOC_E_UNAVAIL;
    }
}

/* OAM event */
void
soc_fl_oam_handler_register(int unit, soc_fl_oam_handler_t handler)
{
    uint64 rval;
    int rv, id = 0;
    uint32 enable = 0;

    fl_oam_handler[unit] = handler;

    if (handler != NULL) {
        enable = 1;
    }
    rv = READ_IP2_INTR_ENABLEr(unit, &rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error reading %s reg !!\n"),
                   unit, SOC_REG_NAME(unit, IP2_INTR_ENABLEr)));
    }
    while (_soc_fl_oam_interrupt_fields[id] != INVALIDf) {
        soc_reg64_field32_set(unit, IP2_INTR_ENABLEr, &rval,
                              _soc_fl_oam_interrupt_fields[id], enable);
        id++;
    }
    rv = WRITE_IP2_INTR_ENABLEr(unit, rval);
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d: Error writing %s reg !!\n"),
                   unit, SOC_REG_NAME(unit, IP1_INTR_ENABLEr)));
    }

    (void)soc_fl_lp_interrupt_enable(unit, FL_INTR_OAM, enable);
}

STATIC int
_soc_fl_process_oam_interrupt(int unit)
{
    uint64 rval;
    int id = 0;
    soc_fl_oam_handler_t oam_handler_snapshot = fl_oam_handler[unit];

    SOC_IF_ERROR_RETURN(READ_IP2_INTR_STATUSr(unit, &rval));

    while (_soc_fl_oam_interrupt_fields[id] != INVALIDf) {
        if (soc_reg64_field32_get(unit, IP2_INTR_STATUSr, rval,
                                  _soc_fl_oam_interrupt_fields[id])) {
            if (oam_handler_snapshot != NULL) {
                (void)(oam_handler_snapshot(unit,
                                            _soc_fl_oam_interrupt_fields[id]));
            }
        }
        id++;
    }

    return SOC_E_NONE;
}

/* TAS interrupt/evnet */
int
soc_fl_cosq_event_handler_register(
    int unit,
    soc_fl_cosq_event_handler_t handler)
{
    int rv = SOC_E_NONE;

    fl_intr_tas_h[unit] = handler;

    if (handler != NULL) {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_TAS, 1);
    } else {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_TAS, 0);
    }
    if (SOC_FAILURE(rv)) {
        fl_intr_tas_h[unit] = NULL;
    }
    return rv;
}

STATIC void
soc_fl_process_tas_intr(int unit)
{
    uint32 rval;

    rval = 0;
    /* check if this belong to interested TAS interrupt */
    if (SOC_FAILURE(READ_MEMFAILINTSTATUSr(unit, &rval))) {
        return;
    }

    if (rval & FL_TAS_INTR_STATUS_MASK) {
        if (fl_intr_tas_h[unit] != NULL) {
            fl_intr_tas_h[unit](unit);
        }
    }
    return;
}

/* Macsec interrupt */
int
soc_fl_macsec_handler_register(
    int unit,
    soc_fl_macsec_handler_t handler)
{
    int rv = SOC_E_NONE;

    fl_intr_macsec_h[unit] = handler;

    if (handler != NULL) {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_MACSEC, 1);
    } else {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_MACSEC, 0);
    }
    if (SOC_FAILURE(rv)) {
        fl_intr_macsec_h[unit] = NULL;
    }
    return rv;
}

void
soc_fl_process_macsec_intr(int unit)
{
    if (fl_intr_macsec_h[unit] != NULL) {
        fl_intr_macsec_h[unit](unit);
    }
}

/* TAF interrupt/evnet */
int
soc_fl_taf_event_handler_register(
    int unit,
    soc_fl_taf_event_handler_t handler)
{
    int rv = SOC_E_NONE;

    fl_intr_taf_h[unit] = handler;

    if (handler != NULL) {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_TAF, 1);
    } else {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_TAF, 0);
    }
    if (SOC_FAILURE(rv)) {
        fl_intr_taf_h[unit] = NULL;
    }
    return rv;
}

STATIC void
soc_fl_process_taf_intr(int unit)
{
    uint32 rval;

    rval = 0;
    /* check if this belong to interested TAS interrupt */
    if (SOC_FAILURE(READ_TAF_MEMFAILINTSTATUSr(unit, &rval))) {
        return;
    }

    if (rval & FL_TAF_INTR_STATUS_MASK) {
        if (fl_intr_taf_h[unit] != NULL) {
            fl_intr_taf_h[unit](unit
                );
        }
    }
    return;
}

/* TSN/SR interrupt/event */
int
soc_fl_tsn_event_handler_register(
    int unit,
    soc_fl_tsn_event_handler_t handler)
{
    int rv;

    fl_intr_sr_h[unit] = handler;

    if (handler != NULL) {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_SR, 1);
    } else {
        rv = soc_fl_lp_interrupt_enable(unit, FL_INTR_SR, 0);
    }
    if (SOC_FAILURE(rv)) {
        fl_intr_sr_h[unit] = NULL;
    }
    return rv;
}

STATIC int
soc_fl_process_sr_intr(int unit, int type)
{
    uint64  rval64;
    uint32  rval32;
    uint8   lan_err = 0, ip_cnt_err = 0, ep_cnt_err = 0;

    if (type == FL_LP_INTR_REG0) {
        SOC_IF_ERROR_RETURN(READ_IP2_INTR_STATUSr(unit, &rval64));
        if (COMPILER_64_HI(rval64) & FL_IP2_SR_LAN_INTR_STATUS_HI_MASK) {
            lan_err = 1;
        }
        SOC_IF_ERROR_RETURN(READ_EGR_MISC_INTR_STATUSr(unit, &rval32));
        if (rval32) {
            ep_cnt_err = 1;
        }
    }
    if (type == FL_LP_INTR_REG1) {
        SOC_IF_ERROR_RETURN(READ_IP2_SR_TH_INTR_STATUSr(unit, &rval64));
        if (COMPILER_64_HI(rval64) || COMPILER_64_LO(rval64)) {
            ip_cnt_err = 1;
        }
    }

    if (lan_err || ep_cnt_err || ip_cnt_err) {
        if (fl_intr_sr_h[unit] != NULL) {
            fl_intr_sr_h[unit](unit, lan_err, ip_cnt_err, ep_cnt_err);
        }
    }
    return SOC_E_NONE;
}

STATIC void
soc_fl_process_l2_intr(int unit)
{
    uint32 rval;

    if (SOC_FAILURE(READ_IP1_INTR_STATUSr(unit, &rval))) {
        return;
    }

    if (rval & FL_IP1_L2_INTR_STATUS_MASK) {
        (void)_soc_hr3_process_l2_overflow(unit);
    }
    return;
}

#if defined(SER_TR_TEST_SUPPORT)
_soc_generic_ser_info_t *soc_fl_ser_tcam_ser_info_get(int unit) {
    return _soc_fl_tcam_ser_info[unit];
}
#endif /* SER_TR_TEST_SUPPORT */

/* Registered interrupt handler */
/*
 * Function:
 *      soc_fl_lp_intr_dpc
 * Purpose:
 *      This function is the main low priority interrupt handler. L2 learn cache
 *      SER and other low priority interrupts should be handled by this
 *      function.
 * Parameters:
 *      unit  - SOC unit #
 * Returns:
 *     Nothing
 */
void soc_fl_lp_intr_dpc(void *unit_vp, void *d1, void *d2,
                                void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32 regval = 0;
    soc_reg_t reg[] = { ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r
                      };


    soc_fl_ser_process_all(unit, PTR_TO_INT(d3), PTR_TO_INT(d4));

    /* Note CHIP_INTR_LOW_PRIORITY is disabled in the calling handler function
     * (from CMICx layer)
     */
    /* SER and any other modules should check the interrupt bits they will
     * handle, add their handlers here
     */

    /* Check interrupts in reg[0] */
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[0], REG_PORT_ANY, 0),
                           &regval);
    if (regval & FL_REG0_TAS_INTR_MASK) {
        soc_fl_process_tas_intr(unit);
    }

    if (regval & FL_REG0_SR_INTR_MASK) {
        soc_fl_process_sr_intr(unit, FL_LP_INTR_REG0);
    }

    if (regval & FL_REG0_OAM_INTR_MASK) {
        (void)_soc_fl_process_oam_interrupt(unit);
    }

    if (regval & FL_REG0_SER_ENGINE_INTR_MASK) {
        soc_generic_ser_process_error(unit, _soc_fl_tcam_ser_info[unit],
                                      _SOC_PARITY_TYPE_SER);
    }

    if (regval & FL_REG0_L2_OVERFLOW_INTR_MASK) {
        soc_fl_process_l2_intr(unit);
    }

    regval = 0;
    /* Check interrupts in reg[1] */
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[1], REG_PORT_ANY, 0),
                           &regval);

    if (regval & FL_REG1_SR_INTR_MASK) {
        soc_fl_process_sr_intr(unit, FL_LP_INTR_REG1);
    }

    if (regval & FL_REG1_TAF_INTR_MASK) {
        soc_fl_process_taf_intr(unit);
    }

   /* Re-enable main CMICx interrupt */
   soc_ser_top_intr_reg_enable(unit, PTR_TO_INT(d3), PTR_TO_INT(d4), 1);
   soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}


/* Interrupt initialization function */
STATIC void
soc_fl_interrupt_init(int unit)
{
    soc_cmic_intr_handler_t handle;

    /* Initialize event handlers */
    if (!fl_event_handler_init[unit]) {
        fl_oam_ser_handler[unit] = NULL;
        fl_oam_handler[unit] = NULL;
        fl_intr_sr_h[unit] = NULL;
        fl_intr_tas_h[unit] = NULL;
        fl_intr_taf_h[unit] = NULL;

        fl_event_handler_init[unit] = 1;
    }

    /* Clear low priority interrupt mask */
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                     REG_PORT_ANY, 0), 0x0);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                     REG_PORT_ANY, 0), 0x0);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                     REG_PORT_ANY, 0), 0x0);
    soc_iproc_setreg(unit,
                     soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG3r,
                     REG_PORT_ANY, 0), 0x0);

    /* register the SER callback function sets */
    sal_memset(&fl_ser_functions[unit], 0, sizeof(soc_ser_functions_t));
    fl_ser_functions[unit]._soc_ser_fail_f = &soc_fl_ser_fail;
    fl_ser_functions[unit]._soc_ser_parity_error_cmicx_intr_f =
        &soc_fl_lp_intr_dpc;
    soc_ser_function_register(unit, &fl_ser_functions[unit]);

    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_ser_cmicx_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);

    /* Turn on low priority interrupts for actual h/w and emulation */
    /* Note the individual modules should check if they will be working in
     * interrupt mode, and enable/disable module level interrupts accordingly
     */
    soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}

/*
 * End of interrupt handler functions
 */

#define FL_FLOW_TYPE_SR  1
#define FL_FLOW_TYPE_TSN 2

STATIC int
_soc_firelight_misc_init(int unit)
{
#define NUM_SUBPORT 4 /* number of subport of a XL or CL port */
    static const soc_field_t port_field[] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    uint32              rval, l2_ovf_enable, l2_tbl_size, fval;
    uint64              reg64;
    int                 port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t         fields[3];
    uint32              values[3];
    int blk, bindex, mode;
    int phy_port_base;
    soc_info_t *si = &SOC_INFO(unit);
    int index, count, sub_sel, offset;
    static int rtag7_field_width[] = { 16, 16, 4, 16, 8, 8, 16, 16 };
    int port_count = 0;
    int i = 0;
    int flow_type;
    int parity_enable;

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {
        /* Clear IPIPE/EIPIE Memories */
        SOC_IF_ERROR_RETURN(soc_firelight_pipe_mem_clear(unit));

        /* CHECKME : FL needs the same programming as HR3 */
        /* _soc_hurricane3_gport_tdm_mode_init(unit) */

        /* Clear MIB counter */
        SOC_IF_ERROR_RETURN(soc_firelight_mib_reset(unit));
    }

    SOC_IF_ERROR_RETURN(soc_firelight_init_port_mapping(unit));

    soc_fl_interrupt_init(unit);
    /* Do SER init and enable/disable parity according to config */
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_fl_tcam_ser_init(unit));
        /* Enabling the packet drop when parity error occurred */
        SOC_IF_ERROR_RETURN(READ_IPIPE_PERR_CONTROLr(unit, &rval));
        fval = 1;
        soc_reg_field_set(unit, IPIPE_PERR_CONTROLr, &rval, DROPf, fval);
        SOC_IF_ERROR_RETURN(WRITE_IPIPE_PERR_CONTROLr(unit, rval));
        SOC_IF_ERROR_RETURN(soc_fl_ser_enable_all(unit, TRUE));
#if defined(SER_TR_TEST_SUPPORT)
        soc_fl_ser_test_register(unit);
#endif
    } else {
        SOC_IF_ERROR_RETURN(soc_fl_ser_enable_all(unit, FALSE));
    }

    /* Enable L2 overflow bucket */
    l2_tbl_size = soc_property_get(unit, spn_L2_TABLE_SIZE, 0);
    if (l2_tbl_size == (soc_mem_index_count(unit, L2Xm) +
                        soc_mem_index_count(unit, L2_ENTRY_OVERFLOWm))) {
        /*
         * if l2 table size equals to l2 table size + l2 overflow table size,
         * it indicates the l2 overflow table is enabled.
         */
        l2_ovf_enable = 1;
    } else {
        l2_ovf_enable = 0;
    }
    SOC_IF_ERROR_RETURN(READ_L2_LEARN_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, L2_LEARN_CONTROLr, &rval,
                      OVERFLOW_BUCKET_ENABLEf, l2_ovf_enable);
    SOC_IF_ERROR_RETURN(WRITE_L2_LEARN_CONTROLr(unit, rval));
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_bucket_enable = l2_ovf_enable;
    SOC_CONTROL_UNLOCK(unit);

    /* Enable L2 overflow interrupt */
    if (soc_property_get(unit, spn_L2_OVERFLOW_EVENT, FALSE)) {

        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->l2_overflow_enable = TRUE;
        SOC_CONTROL_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_start(unit));
        /* Enable the l2 overflow interrupt */
        soc_fl_lp_interrupt_enable(unit, FL_INTR_L2_OVERFLOW, 1);
    } else {
        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->l2_overflow_enable = FALSE;
        SOC_CONTROL_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_stop(unit));
        /* Disable the l2 overflow interrupt */
        soc_fl_lp_interrupt_enable(unit, FL_INTR_L2_OVERFLOW, 0);
    }


    if (!soc_feature(unit, soc_feature_portmod))
    {
        /* GMAC init  */
        SOC_IF_ERROR_RETURN(soc_firelight_gmac_init(unit));
    }

    /* HG init */
    SOC_IF_ERROR_RETURN(soc_firelight_higig_mode_init(unit));

    /* XLPORT and CLPORT blocks init */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        phy_port_base = si->port_l2p_mapping[port];

        /* Assert XLPORT soft reset */
        port_count = -1;
        rval = 0;
        for (bindex = 0; bindex < NUM_SUBPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        for (i = 0; i < _FL_MAX_QTC_COUNT ; i++) {
            if (_fl_tsc[unit][i].phy_port_base == phy_port_base) {
                port_count = _fl_tsc[unit][i].port_count;
                break;
            }
        }
        if (port_count == SOC_FL_PORT_RATIO_QUAD) {
            mode = SOC_FL_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
            si->port_num_lanes[port + 3] = 1;
        } else if ((port_count == SOC_FL_PORT_RATIO_SINGLE) ||
            (port_count == SOC_FL_PORT_RATIO_SINGLE_XAUI)) {
            mode = SOC_FL_PORT_MODE_SINGLE;
            si->port_num_lanes[port] = 4;
        } else if (port_count == SOC_FL_PORT_RATIO_DUAL_2_2) {
            mode = SOC_FL_PORT_MODE_DUAL;
            si->port_num_lanes[port] = 2;
            si->port_num_lanes[port + 1] = 2;
        } else if (port_count == SOC_FL_PORT_RATIO_TRI_012) {
            mode = SOC_FL_PORT_MODE_TRI_012;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 2;
        } else if (port_count == SOC_FL_PORT_RATIO_TRI_023) {
            mode = SOC_FL_PORT_MODE_TRI_023;
            si->port_num_lanes[port] = 2;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
        } else {
            mode = SOC_FL_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
            si->port_num_lanes[port + 3] = 1;
        }

        rval = 0;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_CORE_PORT_MODEf, mode);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval,
                          XPORT0_PHY_PORT_MODEf, mode);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        for (bindex = 0; bindex < NUM_SUBPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }
        phy_port_base = si->port_l2p_mapping[port];

        /* Assert XLPORT soft reset */
        port_count = -1;
        rval = 0;
        for (bindex = 0; bindex < NUM_SUBPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, CLPORT_SOFT_RESETr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, rval));

        for (i = _FL_MAX_QTC_COUNT; i < _FL_MAX_TSC_COUNT ; i++) {
            if (_fl_tsc[unit][i].phy_port_base == phy_port_base) {
                port_count = _fl_tsc[unit][i].port_count;
                break;
            }
        }
        if (port_count == SOC_FL_PORT_RATIO_QUAD) {
            mode = SOC_FL_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
            si->port_num_lanes[port + 3] = 1;
        } else if ((port_count == SOC_FL_PORT_RATIO_SINGLE) ||
            (port_count == SOC_FL_PORT_RATIO_SINGLE_XAUI)) {
            mode = SOC_FL_PORT_MODE_SINGLE;
            si->port_num_lanes[port] = 4;
        } else if (port_count == SOC_FL_PORT_RATIO_DUAL_2_2) {
            mode = SOC_FL_PORT_MODE_DUAL;
            si->port_num_lanes[port] = 2;
            si->port_num_lanes[port + 1] = 2;
        } else if (port_count == SOC_FL_PORT_RATIO_TRI_012) {
            mode = SOC_FL_PORT_MODE_TRI_012;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 2;
        } else if (port_count == SOC_FL_PORT_RATIO_TRI_023) {
            mode = SOC_FL_PORT_MODE_TRI_023;
            si->port_num_lanes[port] = 2;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
        } else {
            mode = SOC_FL_PORT_MODE_QUAD;
            si->port_num_lanes[port] = 1;
            si->port_num_lanes[port + 1] = 1;
            si->port_num_lanes[port + 2] = 1;
            si->port_num_lanes[port + 3] = 1;
        }

        rval = 0;
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval,
                          XPORT0_CORE_PORT_MODEf, mode);
        soc_reg_field_set(unit, CLPORT_MODE_REGr, &rval,
                          XPORT0_PHY_PORT_MODEf, mode);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        for (bindex = 0; bindex < NUM_SUBPORT; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] != -1) {
                soc_reg_field_set(unit, CLPORT_ENABLE_REGr, &rval,
                                  port_field[bindex], 1);
            }
        }
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_ENABLE_REGr(unit, port, rval));
    }

    /* Using lane number to check the macsec valid config */
    for (i = 0; i < SOC_MAX_PHY_PORTS; i++) {
        if ((si->port_num_lanes[si->port_p2l_mapping[i]] == -1) &&
            (fl_macsec_cfg_port[unit][i] != -1)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                         "Physical port %d has incorrect "
                         "macsec index %d\n"),
                         i, fl_macsec_cfg_port[unit][i]));
            return SOC_E_CONFIG;
        }
    }

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

    /* Enable dual hash on L2 and L3 tables */
    fields[0] = ENABLEf;
    values[0] = 1;
    fields[1] = HASH_SELECTf;
    values[1] = FB_HASH_CRC32_LOWER;
    fields[2] = INSERT_LEAST_FULL_HALFf;
    values[2] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_AUX_HASH_CONTROLr, REG_PORT_ANY, 3,
                                 fields, values));

    /*
     * Egress Enable
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL,
                               SOC_INFO(unit).port_l2p_mapping[port], entry));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, EPC_LINK_BMAP_LO_64r, &reg64, PORT_BITMAP_LOf,
                         SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0));
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAP_LO_64r(unit, reg64));
    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &reg64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          ARP_VALIDATION_ENf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                          IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    if (SAL_BOOT_BCMSIM) {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &reg64,
                              SVL_ENABLEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, reg64));

    SOC_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
    soc_reg_field_set(unit, EGR_CONFIG_1r, &rval, RING_MODEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    rval = 0;
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, VT_MISS_UNTAGf, 0);

    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT)));

    /* Enable vrf based l3 lookup by default. */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, VRF_MASKr, REG_PORT_ANY, MASKf, 0));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    fields[1] = LAG_RES_ENf;
    values[0] = values[1] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                 REG_PORT_ANY, 2, fields, values));

    /* Populate and enable RTAG7 Macro flow offset table */
    if (soc_mem_is_valid(unit, RTAG7_FLOW_BASED_HASHm)) {
        count = soc_mem_index_max(unit, RTAG7_FLOW_BASED_HASHm);
        sal_memset(entry, 0, sizeof(rtag7_flow_based_hash_entry_t));
        for (index = 0; index < count; ) {
            for (sub_sel = 0; sub_sel < 8 && index < count; sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        SUB_SEL_ECMPf, sub_sel);
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        OFFSET_ECMPf, offset);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, RTAG7_FLOW_BASED_HASHm,
                                       MEM_BLOCK_ANY, index, &entry));
                    index++;
                }
            }
        }
        rval = 0;
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval, USE_FLOW_SEL_ECMPf, 1);
        SOC_IF_ERROR_RETURN(WRITE_RTAG7_HASH_SELr(unit, rval));
    }

    /* Directed Mirroring ON by default */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, EGR_PORT_64r,
                              &reg64, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORT_64r(unit, port, reg64));
        SOC_IF_ERROR_RETURN(READ_IEGR_PORT_64r(unit, port, &reg64));
        soc_reg64_field32_set(unit, IEGR_PORT_64r,
                              &reg64, EM_SRCMOD_CHANGEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IEGR_PORT_64r(unit, port, reg64));
    }

#ifdef INCLUDE_AVS
    soc_hr3_avs_init(unit);
#endif /* INCLUDE_AVS  */

    /* Major for L3_IIFm.TRUST_DSCP_PTRf init */
    SOC_IF_ERROR_RETURN(_soc_hr3_l3iif_hw_mem_init(unit, TRUE));

    /* Select flow type to not eligible for cut-through */
    flow_type = soc_property_get(unit, spn_FLOW_TYPE_FOR_CUT_THROUGH_CONTROL,
                                 FL_FLOW_TYPE_SR);
    SOC_IF_ERROR_RETURN(READ_CUT_THROUGH_PRIORITYr(unit, &rval));
    if (flow_type == FL_FLOW_TYPE_TSN) {
        soc_reg_field_set(unit, CUT_THROUGH_PRIORITYr, &rval, CT_PRIORITYf, 1);
    } else {
        /* Default select SR flow type */
        soc_reg_field_set(unit, CUT_THROUGH_PRIORITYr, &rval, CT_PRIORITYf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_CUT_THROUGH_PRIORITYr(unit, rval));

    /*
     * macsec increase the jitter between cells egress to PM,
     * so increase the value of ASF_PKT_SIZE to avoid underrun
     */
    rval = 4;
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_ASF_PKT_SIZEr(unit, port, rval));
    }

    /* Reset the TOP_TMON_CTRL
     * Before release reset, change the TMON BG_ADJ to 0x0 from default value 0x1
     */
    SOC_IF_ERROR_RETURN(READ_TOP_TMON_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON0_RESET_Nf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON1_RESET_Nf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON0_BG_ADJf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON1_BG_ADJf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TMON_CTRLr(unit, rval));
    sal_usleep(10);
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON0_RESET_Nf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRLr, &rval, TMON1_RESET_Nf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TMON_CTRLr(unit, rval));

    /* LEDuP init */
    SOC_IF_ERROR_RETURN(soc_firelight_ledup_init(unit));

    return SOC_E_NONE;
}

STATIC int
_soc_firelight_misc_deinit (int unit)
{
    if (_soc_ser_info_init && _soc_fl_tcam_ser_info[unit] != NULL) {
        sal_free(_soc_fl_tcam_ser_info[unit]);
        _soc_fl_tcam_ser_info[unit] = NULL;
    }

    return SOC_E_NONE;
}

/* soc_firelight_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_firelight_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    soc_persist_t *sop = SOC_PERSIST(unit);
    uint16 dev = 0;
    uint8 rev = 0;

    SOC_CONTROL(unit)->l3_defip_max_tcams = 2;
    SOC_CONTROL(unit)->l3_defip_tcam_size = 512; /* or 64 per OTP */

    if (SAL_BOOT_QUICKTURN) {
        soc_cm_get_id(unit, &dev, &rev);

        /* QuickTurn with limited table entries */
        sop->memState[VLAN_SUBNETm].index_max = 14;
        sop->memState[VLAN_SUBNET_DATA_ONLYm].index_max = 14;
        sop->memState[VLAN_SUBNET_ONLYm].index_max = 14;
        /* sop->memState[VFP_TCAMm].index_max = 1; */
        sop->memState[MY_STATION_TCAMm].index_max = 31;
        sop->memState[MY_STATION_TCAM_DATA_ONLYm].index_max = 31;
        sop->memState[MY_STATION_TCAM_ENTRY_ONLYm].index_max = 31;
        /* sop->memState[EFP_TCAMm].index_max = 3; */
        sop->memState[L2_USER_ENTRYm].index_max = 30;
        sop->memState[L2_USER_ENTRY_DATA_ONLYm].index_max = 30;
        sop->memState[L2_USER_ENTRY_ONLYm].index_max = 30;
        sop->memState[L3_DEFIPm].index_max = 31;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 31;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 31;
        sop->memState[L3_DEFIP_ONLYm].index_max = 31;
        sop->memState[CPU_COS_MAPm].index_max = 30;
        sop->memState[CPU_COS_MAP_DATA_ONLYm].index_max = 30;
        sop->memState[CPU_COS_MAP_ONLYm].index_max = 30;
        /* sop->memState[FP_TCAMm].index_max = 3;
        sop->memState[FP_GLOBAL_MASK_TCAMm].index_max = 3; */
        sop->memState[SVC_METER_OFFSET_TCAMm].index_max = 3;
    }
    return rv;
}

/*
 * Function:
 *  soc_firelight_serdes_disabled
 * Purpose:
 *  Get the disabled serdes core from strap status register
 */
STATIC int
soc_firelight_serdes_disabled(int unit,
                               uint32 *disabled_tscf,
                               uint32 *disabled_tscq)
{
    uint32 strap_sts_1 = 0;
    int power_off = 0;
    int i = 0;
    soc_field_t tscq_fields[] = {
        STRAP_TSCQ0_PWR_OFFf,
        STRAP_TSCQ1_PWR_OFFf,
        STRAP_TSCQ2_PWR_OFFf,
        INVALIDf
    };
    soc_field_t tscf_fields[] = {
        STRAP_TSCF0_PWR_OFFf,
        STRAP_TSCF1_PWR_OFFf,
        STRAP_TSCF2_PWR_OFFf,
        STRAP_TSCF3_PWR_OFFf,
        INVALIDf
    };

    /* parameter validation */
    if ((disabled_tscf == NULL) || (disabled_tscq == NULL)) {
        return SOC_E_PARAM;
    }

    /*
     * For simulation, starp status register is empty.
     * Return all QTC and TSC not disabled.
     */
    if  (SAL_BOOT_SIMULATION) {
        *disabled_tscf = 0;
        *disabled_tscq = 0;
        return SOC_E_NONE;
    } else {
        if (soc_feature(unit, soc_feature_untethered_otp)) {
            /* read OTP information */
            /* tscq */
            for (i = 0; i < _FL_MAX_QTC_COUNT; i++) {
                if (SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, i)) {
                    *disabled_tscq |= (1 << i);
                }
            }
            /* tscf */
            for (i = 0; i < _FL_MAX_TSCF_COUNT; i++) {
                if (SHR_BITGET(SOC_BOND_INFO(unit)->tsc_disabled, _FL_MAX_QTC_COUNT + i)) {
                    *disabled_tscf |= (1 << i);
                }
            }
        } else {
            *disabled_tscf = 0;
            *disabled_tscq = 0;
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts_1));

    /* Get the disabled QTC cores */
    i = 0;
    while (tscq_fields[i] != INVALIDf) {
        power_off = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                      strap_sts_1, tscq_fields[i]);
        if (power_off) {
            *disabled_tscq |= (1 << i);
        }
        i++;
    };

    /* Get the disabled TSC cores */
    i = 0;
    while (tscf_fields[i] != INVALIDf) {
        power_off = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                      strap_sts_1, tscf_fields[i]);
        if (power_off) {
            *disabled_tscf |= (1 << i);
        }
        i++;
    };

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,"%s: disabled QTC(0x%x), "
                            "disabled TSC(0x%x)\n"),
                 FUNCTION_NAME(), *disabled_tscq, *disabled_tscf));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_firelight_core_disable
 * Purpose:
 *      Re-config the port info for the core disabled
 */
STATIC int
_soc_firelight_core_disable(int unit, int core_type, uint32 disable_core_bmp)
{
    int i, j, idx, core_count;
    int phy_port, port, blk, num_port;
    soc_info_t *si;
    const int *core_phy_port;
    int mmu_port;

    si = &SOC_INFO(unit);

    switch (core_type) {
        case SOC_FL_PORT_CORE_TYPE_TSCE:
            num_port = _FL_PORT_COUNT_PER_TSC; /* 4 */
            core_phy_port = &tsc_phy_port[0];
            core_count = _FL_MAX_QTC_COUNT; /* 3 */
            break;
        case SOC_FL_PORT_CORE_TYPE_TSCF:
            num_port = _FL_PORT_COUNT_PER_TSC; /* 4 */
            core_phy_port = &tsc_phy_port[_FL_MAX_QTC_COUNT];
            core_count = _FL_MAX_TSCF_COUNT; /* 4 */
            break;
        case SOC_FL_PORT_CORE_TYPE_QTC:
            num_port = _FL_PORT_COUNT_PER_QTC; /* 16 */
            core_phy_port = &qtc_phy_port[0];
            core_count = _FL_MAX_QTC_COUNT; /* 3 */
            break;
        default:
            return (SOC_E_PARAM);
    }
    for (i = 0; i < core_count; i++) {
        if ((1 << i) & disable_core_bmp){
            for (j = 0; j < num_port; j++) {
                phy_port = core_phy_port[i] + j;
                port = si->port_p2l_mapping[phy_port];
                mmu_port = si->port_p2m_mapping[phy_port];

                si->port_p2l_mapping[phy_port] = -1;
                /*
                 * port_p2m_mapping value 127 for non-enabled port program in
                 * MMU_ARB_TDM_TABLE.PORT_NUMf
                 */
                si->port_p2m_mapping[phy_port] = 127;
                si->max_port_p2m_mapping[phy_port] = -1;

                /* skip native unmapped mmu ports */
                if (mmu_port != -1) {
                    si->port_m2p_mapping[mmu_port] = -1;
                }

                /* skip native unmapped logical ports */
                if (port != -1) {
                    for (idx = 0; idx < SOC_DRIVER(unit)->port_num_blktype; idx++) {
                        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, idx);
                        if (blk < 0) { /* end of block list of each port */
                            break;
                        }
                        if(si->block_port[blk] == port){
                            si->block_port[blk] = -1;
                        }
                        si->block_valid[blk] = 0;
                        SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], port);
                    }

                    si->port_l2p_mapping[port] = -1;
                    SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->xl.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->cl.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->all.bitmap, port);
                    SOC_PBMP_PORT_REMOVE(si->port.bitmap, port);
                    sal_snprintf(si->port_name[port],
                                 sizeof(si->port_name[port]),
                                 "?%d", port);
                    si->port_offset[port] = port;
                }
            }
        }
    }

#define RECONFIG_PORT_INFO(ptype,str) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, port) { \
        if (sal_strcmp(str,"")) {\
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),\
            "%s%d", str, si->ptype.num);\
        }\
        si->ptype.port[si->ptype.num++] = port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = port; \
        } \
        if (port > si->ptype.max) { \
            si->ptype.max = port; \
        } \
    }

    /* coverity[pointless_string_compare] */
    RECONFIG_PORT_INFO(ether,"");
    /* coverity[pointless_string_compare] */
    RECONFIG_PORT_INFO(st,"");
    RECONFIG_PORT_INFO(hg,"hg");
    RECONFIG_PORT_INFO(xe,"xe");
    RECONFIG_PORT_INFO(ge,"ge");
#undef RECONFIG_PORT_INFO

    return SOC_E_NONE;
}

#define TOP_STRAP_LCPLL1_REFCLK_SEL(val)    (((val) >> 18) & 0x1)

/*
 * Function:
 *      soc_firelight_chip_reset
 * Purpose:
 *      Special reset sequencing for BCM56070
 */
int
soc_firelight_chip_reset(int unit)
{
    uint32 rval, to_usec;
    _fl_option_info_t *matched_opt_info;
    uint32 disabled_bmp;
    uint32 disabled_tscf = 0;
    uint32 disabled_tscq = 0;
    int i, init_usec_timeout, rv = SOC_E_NONE;
    soc_info_t *si;
    uint16 dev_id;
    uint8  rev_id;
    uint32 core_pll_ndiv;
    uint32 core_pll_ch0;
    uint32 core_pll_ch1;
    uint32 core_pll_ch2;
    uint32 core_pll_ch3;
    uint32 core_pll_ch4;
    int ms_clk_freq;
    int ms_port_en[SOC_MAX_LOGICAL_PORTS];
    uint32 pgw_pll_frefeff_info;
    uint32 pgw_pll_ndiv_int;
    uint32 pgw_pll_pdiv;
    uint32 pgw_pll_ch0_mdiv;
    uint32 pgw_pll_ch5_mdiv;
    soc_reg_t reg;
    soc_field_t fld;
    int blk;
    uint32 ge_xl_sel = 0, ge_xl_bypass = 0x7;
    uint32 cl_en = 0, cl_bypass = 0xf;
    int  phy_port_base, port, bindex;
    int  pport, port_cnt;
    uint32 strap_sts;
    int macsec_ing_byp = 0, macsec_egr_byp = 0;
    int bond_macsec_enable = 0;
    int pll_lock_usec = 0;
    soc_timeout_t to;
    uint32 lcpll1_i_refclk_sel;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

/*     $CMIC_SBUS_RING_MAP_5_RESETVAL = 0x0000_1111;
    $CMIC_SBUS_RING_MAP_4_RESETVAL = 0x1111_1111;
    $CMIC_SBUS_RING_MAP_3_RESETVAL = 0x0000_0000;
    $CMIC_SBUS_RING_MAP_2_RESETVAL = 0x2222_6554;
    $CMIC_SBUS_RING_MAP_1_RESETVAL = 0x0043_0070;
    $CMIC_SBUS_RING_MAP_0_RESETVAL = 0x0111_0000; */

    WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x01110000); /* block 7  - 0 */
    WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x00430070); /* block 15 - 8 */
    WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x22226554); /* block 23 - 16 */
    WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00000000); /* block 31 - 24 */
    WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x11111111); /* block 39 - 32 */
    WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00001111); /* block 40 - 47 */
    WRITE_CMIC_TOP_SBUS_TIMEOUTr(unit, 0x7d0);

    sal_usleep(to_usec);


    /* core clock PLL setting */
    si = &SOC_INFO(unit);

    /* core clock frequency override for internal use case */
    si->frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY,
                                     si->frequency);

    switch (si->frequency) {
        case _FL_SYSTEM_FREQ_219:
            core_pll_ndiv = 70;
            core_pll_ch0 = 8;
            core_pll_ch1 = 14;
            core_pll_ch2 = 7;
            core_pll_ch3 = 4;
            core_pll_ch4 = 4;
            break;
        case _FL_SYSTEM_FREQ_344:
            core_pll_ndiv = 110;
            core_pll_ch0 =8;
            core_pll_ch1 = 22;
            core_pll_ch2 = 11;
            core_pll_ch3 = 6;
            core_pll_ch4 = 6;
            break;
        case _FL_SYSTEM_FREQ_469:
            core_pll_ndiv = 150;
            core_pll_ch0 = 8;
            core_pll_ch1 = 30;
            core_pll_ch2 = 15;
            core_pll_ch3 = 8;
            core_pll_ch4 = 8;
            break;
        case _FL_SYSTEM_FREQ_700:
        default:
            core_pll_ndiv = 140;
            core_pll_ch0 = 5;
            core_pll_ch1 = 28;
            core_pll_ch2 = 14;
            core_pll_ch3 = 7;
            core_pll_ch4 = 7;
            break;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL3r,
                                 REG_PORT_ANY, MSTR_NDIV_INTf, core_pll_ndiv));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL4r,
                                 REG_PORT_ANY, MSTR_CH0_MDIVf, core_pll_ch0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL4r,
                                 REG_PORT_ANY, MSTR_CH1_MDIVf, core_pll_ch1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL5r,
                                 REG_PORT_ANY, MSTR_CH2_MDIVf, core_pll_ch2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_CTRL6r,
                                 REG_PORT_ANY, MSTR_CH3_MDIVf, core_pll_ch3));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_CORE_PLL_DEBUG_CTRL_2r,
                                 REG_PORT_ANY, RESERVEDf, core_pll_ch4));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                CMIC_TO_CORE_PLL_LOADf, 1));

    if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_LCPLL_SOFT_RESETf, 0x1);
        WRITE_TOP_SOFT_RESET_REGr(unit, rval);

        /* configure both BSPLL0 and BSPLL1 */
        /* BSPLL0 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, PDIVf, 2);
        WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC0_LCPLL_FBDIV_0f, 0x0000);
        WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC0_LCPLL_FBDIV_1f, 0x1E00);
        WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, CH0_MDIVf, 0x96);
        WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r, REG_PORT_ANY, BROAD_SYNC0_LCPLL_HO_BYP_ENABLEf, 0x1));

        /* BSPLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, PDIVf, 2);
        WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r, &rval, BROAD_SYNC1_LCPLL_FBDIV_0f, 0x0000);
        WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC1_LCPLL_FBDIV_1f, 0x1E00);
        WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, CH0_MDIVf, 0x96);
        WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval);

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r, REG_PORT_ANY, BROAD_SYNC1_LCPLL_HO_BYP_ENABLEf, 0x1));

        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_LCPLL_SOFT_RESETf, 0x0);
        WRITE_TOP_SOFT_RESET_REGr(unit, rval);

        /* BSPLL0 */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC0_LCPLL_FBDIV_1f, 0x1E00);
        WRITE_TOP_BROAD_SYNC0_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r, REG_PORT_ANY, BROAD_SYNC0_LCPLL_HO_BYP_ENABLEf, 0x0));

        /* BSPLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r, &rval, BROAD_SYNC1_LCPLL_FBDIV_1f, 0x1E00);
        WRITE_TOP_BROAD_SYNC1_LCPLL_FBDIV_CTRL_1r(unit, rval);

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r, REG_PORT_ANY, BROAD_SYNC1_LCPLL_HO_BYP_ENABLEf, 0x0));

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL0_SW_OVWRf, 0);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, CMIC_TO_BS_PLL1_SW_OVWRf, 0);
        WRITE_TOP_MISC_CONTROL_1r(unit, rval);
    }
    /*
     * Get ms_port_en from property macsec_port_index and macsec_enable.
     */
    sal_memset(ms_port_en, 0, sizeof(ms_port_en));

    for (pport = SOC_MIN_PHY_PORT_NUM ;
         pport < SOC_MAX_PHY_PORTS ;
         pport++) {

         port = si->port_p2l_mapping[pport];
         if (port != -1) {
             if (soc_fl_macsec_idx_get(unit, port) != -1) {
                 ms_port_en[port] = 1;
             }
         }
     }

     /* PGW PLL setting */

     /*
      * 1. Calculate ms_clk_freq from
      * (a) If any 100G or 50G or 25G speed in CLPORT are used,
      *      MACsec clock = 950MHz (ms_bandwidth = 600Gbps)
      * (b) if any 40G or 20G or 10G speed in XLPORT are used,
      *      MACsec clock = 380MHz (ms_bandwidth = 240Gbps)
      * (c) If only GPORT is used,
      *      MACsec clock = 190MHz (ms_bandwidth = 120Gbps)
      */
     ms_clk_freq = 190;
     port_cnt = 0;
     for (pport = SOC_MIN_PHY_PORT_NUM ;
          pport < SOC_MAX_PHY_PORTS ;
          pport++) {

         port = si->port_p2l_mapping[pport];

         if (port == -1) {
            continue;
         }

         if (IS_CL_PORT(unit, port) &&
              ms_port_en[port]) {
             if ((si->port_speed_max[port] >= 50000) ||
                 (si->port_speed_max[port] == 25000) ||
                 (si->port_speed_max[port] == 27000)) {
                 ms_clk_freq = 950;
                 break;
             }
          } else if (IS_XL_PORT(unit, port) &&
              ms_port_en[port]) {
             if ((si->port_speed_max[port] == 40000) ||
                 (si->port_speed_max[port] == 42000) ||
                 (si->port_speed_max[port] == 20000) ||
                 (si->port_speed_max[port] == 21000) ||
                 (si->port_speed_max[port] == 10000) ||
                 (si->port_speed_max[port] == 11000)) {
                 ms_clk_freq = 380;
             }
          }
     }

     pgw_pll_frefeff_info = 25;
     pgw_pll_ndiv_int = 228;
     pgw_pll_pdiv = 2;
     pgw_pll_ch0_mdiv = 5;

     switch (ms_clk_freq) {
         case 950:
             pgw_pll_ch5_mdiv = 6;
             break;
         case 380:
             pgw_pll_ch5_mdiv = 15;
             break;
         case 190:
         default:
             pgw_pll_ch5_mdiv = 30;
             break;
     }

     SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, TOP_PGW_PLL_OPR_CTRL_2r,
                                  REG_PORT_ANY, FREFEFF_INFOf, pgw_pll_frefeff_info));
     SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, TOP_PGW_PLL_CTRL_3r,
                                  REG_PORT_ANY, NDIV_INTf, pgw_pll_ndiv_int));
     SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, TOP_PGW_PLL_CTRL_1r,
                                  REG_PORT_ANY, PDIVf, pgw_pll_pdiv));
     SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, TOP_PGW_PLL_CTRL_0r,
                                  REG_PORT_ANY, CH0_MDIVf, pgw_pll_ch0_mdiv));
     SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, TOP_PGW_PLL_CTRL_2r,
                                  REG_PORT_ANY, CH5_MDIVf, pgw_pll_ch5_mdiv));

    /* De-assert the PGW PLL reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_PGW_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r,
                      &rval, TOP_PGW_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Check if PGW PLL locked */
    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pll_lock_usec = 10000000;
    } else {
        pll_lock_usec = 50000;
    }
    soc_timeout_init(&to, pll_lock_usec, 0);

    /* Wait for TOP_PGW_PLL_LOCK done. */
    do {
        SOC_IF_ERROR_RETURN(READ_TOP_PGW_PLL_STATUSr(unit, &rval));
        if (soc_reg_field_get(unit, TOP_PGW_PLL_STATUSr, rval, TOP_PGW_PLL_LOCKf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : TOP_PGW_PLL_LOCK timeout\n"), unit));
            break;
        }
    } while (TRUE);

    /* LCPLL initialization procedure for 50MHz internal refclk path */
    strap_sts=0;
    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));

    lcpll1_i_refclk_sel = (TOP_STRAP_LCPLL1_REFCLK_SEL(strap_sts) == 1);
    if (lcpll1_i_refclk_sel && !SAL_BOOT_SIMULATION) {
        /* Put PLL in reset state */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                    REG_PORT_ANY, TOP_LCPLL_SOFT_RESETf, 0x1));

        /* Setup LCPLL0 clock path */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_3r,
                                    REG_PORT_ANY, RATE_MNGR_MODEf, 0x1));

        /* Output LCPLL CML (REFCLK) */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_8r,
                                    REG_PORT_ANY, D2C2_CTRLf, 0xE0));

        /* Enable SW overwrite to LCPLL0 settings */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r,
                                    REG_PORT_ANY, CMIC_TO_XG_PLL0_SW_OVWRf, 0x1));

        /* Divide down the VCO frequency to get target CH0 clock (156.25MHz) */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG_PLL0_CTRL_0r,
                                    REG_PORT_ANY, CH0_MDIVf, 0x28));


        /* Update LCPLL0 FBDIV via bypass holdover logic */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG0_LCPLL_FBDIV_CTRL_0r,
                                    REG_PORT_ANY, XG0_LCPLL_FBDIV_0f, 0x0000));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG0_LCPLL_FBDIV_CTRL_1r,
                                    REG_PORT_ANY, XG0_LCPLL_FBDIV_1f, 0x1F40));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r,
                                    REG_PORT_ANY, XG0_LCPLL_HO_BYP_ENABLEf, 0x1));

        /* Release PLL reset */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                    REG_PORT_ANY, TOP_LCPLL_SOFT_RESETf, 0x0));


        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_XG0_LCPLL_FBDIV_CTRL_1r,
                                    REG_PORT_ANY, XG0_LCPLL_FBDIV_1f, 0x1F40));

        /* Wait for TOP_XGPLL_LOCK done. */
        pll_lock_usec = 1000000;
        soc_timeout_init(&to, pll_lock_usec, 0);

        do {
            SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_STATUSr(unit, &rval));
            if (soc_reg_field_get(unit, TOP_XG_PLL0_STATUSr, rval, TOP_XGPLL_LOCKf)) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : TOP_XGPLL_LOCK timeout\n"), unit));
                break;
            }
        } while (TRUE);

        /* Release bypass holdover logic */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_3r,
                                    REG_PORT_ANY, XG0_LCPLL_HO_BYP_ENABLEf, 0x0));
    }

    /*
     * TSCx or QTCx to be disabled in some sku.
     */
    /* To be reviewed on chip */
    if (matched_option_idx[unit] == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
           (BSL_META_U(unit, "Warning: soc_firelight_port_config_init should "
                        "be invoked first! Choose bcm56070 port config.\n")));
        matched_option_idx[unit] = 0;
    }

    matched_opt_info = &_fl_option_port_config[unit][matched_option_idx[unit]];
    disabled_tscf = 0;
    disabled_tscq = 0;
    SOC_IF_ERROR_RETURN(
        soc_firelight_serdes_disabled(unit,
                                       &disabled_tscf, &disabled_tscq));
    disabled_bmp = matched_opt_info->disabled_tscf_bmp;
    for (i = 0; i < _FL_MAX_TSCF_COUNT; i++) {
        if ((1 << i) & disabled_bmp) {
            disabled_tscf |= (1 << i);
        }
    }

    disabled_bmp = matched_opt_info->disabled_tscq_bmp;
    for (i = 0; i < _FL_MAX_QTC_COUNT; i++) {
        if ((1 << i) & disabled_bmp) {
            disabled_tscq |= (1 << i);
        }
    }

    if (disabled_tscf) {
        SOC_IF_ERROR_RETURN(
            _soc_firelight_core_disable(unit, SOC_FL_PORT_CORE_TYPE_TSCF,
                                         disabled_tscf));
    }
    if (disabled_tscq) {
        SOC_IF_ERROR_RETURN(
            _soc_firelight_core_disable(unit, SOC_FL_PORT_CORE_TYPE_QTC,
                                         disabled_tscq));
        SOC_IF_ERROR_RETURN(
            _soc_firelight_core_disable(unit, SOC_FL_PORT_CORE_TYPE_TSCE,
                                         disabled_tscq));
    }

    SOC_IF_ERROR_RETURN(READ_PGW_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, PGW_CTRL_0r, &rval, SW_PM4X10Q_DISABLEf,
                      disabled_tscq & 0x7);
    soc_reg_field_set(unit, PGW_CTRL_0r, &rval, SW_PM4X25_DISABLEf,
                      disabled_tscf & 0xf);
    SOC_IF_ERROR_RETURN(WRITE_PGW_CTRL_0r(unit, rval));
    sal_usleep(to_usec);

    /* Write to MACSEC MUX register */
    for (blk = 0; blk < _FL_MAX_QTC_COUNT; blk++) {
        /* If no macsec configure, keep the default value and bypass macsec */
        if (!macsec_count[unit]) {
            break;
        }
        phy_port_base = blk * _FL_PORT_COUNT_PER_QTC + _FL_FIRST_PORT_QTC0;
        port = si->port_p2l_mapping[phy_port_base];

        if (IS_XL_PORT(unit, port) || IS_CL_PORT(unit, port)) {
            continue;
        }

        for (bindex = 0; bindex < 16; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] == -1) {
                continue;
            }
            if (fl_macsec_cfg_port[unit][phy_port_base + bindex] != -1) {
                ge_xl_bypass &= ~(1 << blk);
                continue;
            }
        }
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        phy_port_base = si->port_l2p_mapping[port];

        /* If no macsec configure, keep the default value and bypass macsec */
        if (!macsec_count[unit]) {
            break;
        }
        for (bindex = 0; bindex < 4; bindex++) {
            if (si->port_p2l_mapping[phy_port_base + bindex] == -1) {
                continue;
            }

            if (fl_macsec_cfg_port[unit][phy_port_base + bindex] != -1) {
                if ((ge_xl_sel & (1 << SOC_BLOCK_NUMBER(unit, blk))) == 0) {
                    /* ge_xl_sel default is 0: ge, set to '1' for XL */
                    ge_xl_sel |= 1 << SOC_BLOCK_NUMBER(unit, blk);
                }
                if ((ge_xl_bypass & (1 << SOC_BLOCK_NUMBER(unit, blk))) ==
                    (1 << SOC_BLOCK_NUMBER(unit, blk))) {
                    ge_xl_bypass &= ~(1 << SOC_BLOCK_NUMBER(unit, blk));
                }
            }
        }
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (port == -1 || port == REG_PORT_ANY) {
            continue;
        }

        /* If no macsec configure, keep the default value and bypass macsec */
        if (!macsec_count[unit]) {
            break;
        }

        phy_port_base = si->port_l2p_mapping[port];
        for (bindex = 0; bindex < 4; bindex++) {
            if (fl_macsec_cfg_port[unit][phy_port_base + bindex] != -1) {
                if ((cl_en & (1 << SOC_BLOCK_NUMBER(unit, blk))) == 0) {
                    cl_en |= 1 << SOC_BLOCK_NUMBER(unit, blk);
                }

                if ((cl_bypass & (1 << SOC_BLOCK_NUMBER(unit, blk))) ==
                   (1 << SOC_BLOCK_NUMBER(unit, blk))) {
                    cl_bypass &= ~(1 << SOC_BLOCK_NUMBER(unit, blk));
                    continue;
                }
            }
        }
    }

    SOC_IF_ERROR_RETURN(READ_TOP_MACSEC_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_MACSEC_CTRLr, &rval, MACSEC_GE_XL_SELf, ge_xl_sel);
    soc_reg_field_set(unit, TOP_MACSEC_CTRLr, &rval, MACSEC_CL_ENf, cl_en);
    soc_reg_field_set(unit, TOP_MACSEC_CTRLr, &rval, MACSEC_PM4X10Q_BYPf, ge_xl_bypass);
    soc_reg_field_set(unit, TOP_MACSEC_CTRLr, &rval, MACSEC_PM4X25_BYPf, cl_bypass);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MACSEC_CTRLr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MACSEC_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    strap_sts=0;
    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));
    macsec_ing_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_ING_BYP_ENf);
    macsec_egr_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_EGR_BYP_ENf);
    if (SAL_BOOT_SIMULATION) {
        bond_macsec_enable = 1;
    } else {
        bond_macsec_enable = !SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoMacsec);
    }
    if ((!macsec_ing_byp || !macsec_egr_byp) && bond_macsec_enable) {
        port_cnt = 0;
        for (pport = SOC_MIN_PHY_PORT_NUM ;
             pport < SOC_MAX_PHY_PORTS ;
             pport++) {
            if (port_cnt >= FL_MACSEC_MAX_PORT_NUM) {
                break;
            }
            port = si->port_p2l_mapping[pport];
            if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
                continue;
            }
            if (soc_fl_macsec_idx_get(unit, port) == -1) {
                continue;
            }

            SOC_IF_ERROR_RETURN(READ_MACSEC_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, MACSEC_CTRLr, &rval, BYPASS_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_MACSEC_CTRLr(unit, port, rval));
            port_cnt++;
        }
    }

    /*
     * Bring port blocks out of reset
     */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GEP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GEP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_GEP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_XLP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CLP0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CLP1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CLP2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_CLP3_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Bring TSCQ and TSCF blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCQ0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCQ1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCQ2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCF0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCF1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCF2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TSCF3_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* PM4x10Q QSGMII mode control
     */
    if (soc_property_phys_port_get(unit, 2, spn_PORT_GMII_MODE, 0)) {
        soc_reg_field32_modify(unit, CHIP_CONFIGr, 0, QMODEf, 1);
    }
    if (soc_property_phys_port_get(unit, 18, spn_PORT_GMII_MODE, 0)) {
        soc_reg_field32_modify(unit, CHIP_CONFIGr, 1, QMODEf, 1);
    }
    if (soc_property_phys_port_get(unit, 34, spn_PORT_GMII_MODE, 0)) {
        soc_reg_field32_modify(unit, CHIP_CONFIGr, 2, QMODEf, 1);
    }

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r,
                      &rval, TOP_TS_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Initialize cmic, ep, port local timers to same value. */
    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));

    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

    SOC_IF_ERROR_RETURN(
        READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
    soc_reg_field_set(
        unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

    /* Reset cmic, ep, port local timers with same value. */
    SOC_BLOCK_ITER(unit, blk, SOC_BLK_GPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, GPORT_TS_TIMER_47_32_REGr,
                            port, 0, &rval));
        soc_reg_field_set(unit, GPORT_TS_TIMER_47_32_REGr, &rval,
                          TS_TIMER_47_32_VALUEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, GPORT_TS_TIMER_47_32_REGr,
                            port, 0, rval));
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    /* Bring network sync out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(10000);

    /* Reset egress 1588 timer value. */
    SOC_IF_ERROR_RETURN(
        READ_EGR_1588_TIMER_VALUEr(unit, &rval));
    soc_reg_field_set(unit, EGR_1588_TIMER_VALUEr, &rval,
                      TIMESTAMP_47_32f, 0);
    SOC_IF_ERROR_RETURN(
        WRITE_EGR_1588_TIMER_VALUEr(unit, rval));

    if ((!macsec_ing_byp || !macsec_egr_byp) && bond_macsec_enable) {
        port_cnt = 0;
        for (pport = SOC_MIN_PHY_PORT_NUM ;
             pport < SOC_MAX_PHY_PORTS ;
             pport++) {
            port = si->port_p2l_mapping[pport];
            if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
                continue;
            }
            if (fl_macsec_port[unit][pport] == -1) {
                continue;
            }

            SOC_IF_ERROR_RETURN(READ_MACSEC_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, MACSEC_CTRLr, &rval, SOFT_RESETf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MACSEC_CTRLr(unit, port, rval));
        }

        /* Only doing GPORT, CLPORT/XLPORT will do in bcm init */
        for (pport = SOC_MIN_PHY_PORT_NUM;
             pport < tsc_phy_port[0];
             pport++) {
            if (port_cnt >= FL_MACSEC_MAX_PORT_NUM) {
                break;
            }
            port = si->port_p2l_mapping[pport];
            if (!SOC_PBMP_MEMBER(PBMP_ALL(unit), port)) {
                continue;
            }
            if (fl_macsec_port[unit][pport] == -1) {
                continue;
            }
            if (soc_fl_macsec_idx_get(unit, port) == -1) {
                continue;
            }
            SOC_IF_ERROR_RETURN(READ_MACSEC_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, MACSEC_CTRLr, &rval, SOFT_RESETf, 0);
            SOC_IF_ERROR_RETURN(WRITE_MACSEC_CTRLr(unit, port, rval));
            port_cnt++;
        }

        if (!SAL_BOOT_SIMULATION) {
            rval = 0;
            reg = MACSEC_HW_RESET_CONTROLr;
            fld = MEM_TABLE_INITf;
            soc_reg_field_set(unit, reg, &rval, fld, 1);
            fld = MIB_INITf;
            soc_reg_field_set(unit, reg, &rval, fld, 1);
            fld = TCAM_INITf;
            soc_reg_field_set(unit, reg, &rval, fld, 1);
            rv = WRITE_MACSEC_HW_RESET_CONTROLr(unit, rval);
            if (SOC_FAILURE(rv)) {
                return rv;
            }

            init_usec_timeout = 1000000;
            soc_timeout_init(&to, init_usec_timeout, 0);
            fld = ISEC_MIB_SA_INIT_DONEf;
            do {
                rv = READ_MACSEC_HW_RESET_CONTROLr(unit, &rval);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                if (soc_reg_field_get(unit, reg, rval, fld)) {
                    rval = 0;
                    rv = WRITE_MACSEC_HW_RESET_CONTROLr(unit, rval);
                    if (SOC_FAILURE(rv)) {
                        return rv;
                    }
                    break;
                }
                if (soc_timeout_check(&to)) {
                    return SOC_E_TIMEOUT;
                }
            } while (TRUE);
        }
    }

    return SOC_E_NONE;
}


STATIC int
_soc_firelight_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &value));
    *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_ENAf);
    *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, value, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_firelight_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds =
        soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

STATIC int
_soc_firelight_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, L2_AGE_TIMERr, &value, AGE_VALf, age_seconds);
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, value));

    return SOC_E_NONE;
}

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_TMON0_RESULTr,
    TOP_TMON1_RESULTr,
};

int
soc_fl_temperature_monitor_get(int unit,
          int temperature_max,
          soc_switch_temperature_monitor_t *temperature_array,
          int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval[2], val;
    int fval, cur, peak;
    int num_entries_out;

    *temperature_count = 0;
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }

    /* Read from TOP_TMON0/1_RESULT */
    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval[index]));
    }
    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];

        cur = -1;
        peak = -1;
        if (soc_reg_field_get(unit, reg, rval[index], TMON_DATA_VALIDf)) {
            fval = soc_reg_field_get(unit, reg, rval[index], TMON_DATAf);
            cur = (45799000 - (55010 * fval)) / 10000;
            peak = cur;
        }
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &val));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &val,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, val));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &val,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, val));

    *temperature_count = num_entries_out;
    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_fl_macsec_port_reg_blk_idx_get
 * Purpose:
 *  Get the block and port index within macsec registers.
 * Parameters:
 *  unit (IN): unit number.
 *  pport (IN): physical port.
 *  blktype (IN): port block.
 *  block (OUT): blk used to access the reg.
 *  index (OUT): port index of the reg.
 * Return:
 *   SOC_E_NONE: register matched and the block/index has been override.
 *   SOC_E_NOT_FOUND : port cannot match the invalid macsec port.
 *   SOC_E_PARAM : invalid paramter
 */
int
soc_fl_macsec_port_reg_blk_idx_get(
    int unit, int pport, int blktype, int *block, int *index)
{
    int blk = -1;
    int macsec_idx = -1;

    if (blktype != SOC_BLK_MACSEC) {
        return SOC_E_PARAM;
    }

    if (pport == REG_PORT_ANY) {
        blk = MACSEC_BLOCK(unit, 0);
        macsec_idx = 0;
    } else if (fl_macsec_port[unit][pport] != -1) {
        blk = MACSEC_BLOCK(unit, 0);
        macsec_idx = fl_macsec_port[unit][pport];
    }

    if (block != NULL) {
        *block = blk;
    }

    if (index != NULL) {
        *index = macsec_idx;
    }

    if (macsec_idx == -1 || blk == -1) {
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *  soc_fl_macsec_idx_get
 * Purpose:
 *  Get the macsec index from logic port.
 * Parameters:
 *  unit (IN): unit number.
 *  port (IN): logical port.
 * Return:
 *  macsec port: macsec port index.
 *          -1 : no configured macsec port or invalid.
 */
int
soc_fl_macsec_idx_get(int unit, int port)
{
    int phy_port = -1;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if ((phy_port < SOC_MIN_PHY_PORT_NUM) || (phy_port >= SOC_MAX_PHY_PORTS)) {
        return -1;
    }
    return fl_macsec_cfg_port[unit][phy_port];
}

/*
 * Function:
 *  soc_fl_macsec_ctrl_reset
 * Purpose:
 *  Put port into MACSEC SOFT_RESET.
 * Parameters:
 *  unit (IN): unit number.
 *  port (IN): logical port.
 *  enable (IN): enable/disable.
 * Return:
 *   SOC_E_NONE: register matched and the block/index has been override.
 */
int
soc_fl_macsec_ctrl_reset(int unit, int port, int enable)
{
    uint32 macsec_ctrl;
    uint32 strap_sts = 0;
    int macsec_ing_byp = 0, macsec_egr_byp = 0;
    int bond_macsec_enable = 0;
    int pport = SOC_INFO(unit).port_l2p_mapping[port];

    /* GE port doesn't do MACSEC_CTRL in portctrl, GE MACSEC_CTRLr.SOFT_RESETf
       handled in chip_reset */
    if ((SOC_PORT_BLOCK_TYPE(unit, pport) != SOC_BLK_XLPORT) &&
        (SOC_PORT_BLOCK_TYPE(unit, pport) != SOC_BLK_CLPORT)) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_STRAP_STATUSr(unit, &strap_sts));
    macsec_ing_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_ING_BYP_ENf);
    macsec_egr_byp = soc_reg_field_get(unit, TOP_STRAP_STATUSr,
                                  strap_sts, STRAP_MACSEC_EGR_BYP_ENf);
    if (SAL_BOOT_SIMULATION) {
        bond_macsec_enable = 1;
    } else {
        bond_macsec_enable = !SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureNoMacsec);
    }
    if ((!macsec_ing_byp || !macsec_egr_byp) && bond_macsec_enable) {
        if (soc_fl_macsec_idx_get(unit, port) != -1) {
            SOC_IF_ERROR_RETURN(READ_MACSEC_CTRLr(unit, port,
                                                  &macsec_ctrl));
            soc_reg_field_set(unit, MACSEC_CTRLr, &macsec_ctrl, SOFT_RESETf,
                              enable);
            SOC_IF_ERROR_RETURN(WRITE_MACSEC_CTRLr(unit, port,
                                                   macsec_ctrl));
        }
    }
    return SOC_E_NONE;
}

int
_soc_firelight_features(int unit, soc_feature_t feature)
{
    int flow_type = 0;

    flow_type = soc_property_get(unit, spn_FLOW_TYPE_FOR_CUT_THROUGH_CONTROL,
                                 FL_FLOW_TYPE_SR);

    switch (feature) {
        /* FL specific feature (different from GH2) */
        case soc_feature_field_slices12:
        case soc_feature_field_meter_pools12:
        case soc_feature_cmicm:
        case soc_feature_cmicd_v2:
        case soc_feature_cmicm_extended_interrupts:
        case soc_feature_preemption: /* no preemption in FL */
        case soc_feature_preemption_cnt:
        case soc_feature_dcb_type37:
        case soc_feature_rcpu_priority:
        case soc_feature_rcpu_tc_mapping:
        case soc_feature_rcpu_1: /* RCPU related register CMIC_PKT_CTRL is invalid in CMICx register */
        case soc_feature_rroce:
        case soc_feature_vxlan_lite_riot:
        case soc_feature_fifo_dma_hu2:
        case soc_feature_multi_sbus_cmds:
            return FALSE;

        case soc_feature_field_slices8:
        case soc_feature_field_meter_pools8:
        case soc_feature_cmicx:
        case soc_feature_fl:
        case soc_feature_dcb_type40:
        case soc_feature_clmac_16byte_interface_mode:
        case soc_feature_xy_tcam_lpt:
        case soc_feature_xflow_macsec:
        case soc_feature_xflow_macsec_stat:
        case soc_feature_xflow_macsec_inline:
        case soc_feature_xflow_macsec_crypto_aes_256:
        case soc_feature_xflow_macsec_restricted_move:
        /* GPORT EEE LPI counter tx/rx registers swapped */
        case soc_feature_gport_eee_lpi_tx_rx_swap:
        case soc_feature_ser_hw_bg_read:
        case soc_feature_portmod:
        case soc_feature_iddq_new_default:
        case soc_feature_use_smbus0_for_i2c:
        case soc_feature_no_higig_plus:
        case soc_feature_pdelay_req:
        case soc_feature_tscf_gen3_pcs_timestamp:
            return TRUE;

        /* GH2 B0 feature */
        case soc_feature_arl_hashed:
        case soc_feature_arl_insert_ovr:
        case soc_feature_cfap_pool:
        case soc_feature_cos_rx_dma:
        case soc_feature_ingress_metering:
        case soc_feature_egress_metering:
        case soc_feature_l3_lookup_cmd:
        case soc_feature_led_proc:
        case soc_feature_led_data_offset_a0:
        case soc_feature_schmsg_alias:
        case soc_feature_stack_my_modid:
        case soc_feature_stat_dma:
        case soc_feature_cpuport_stat_dma:
        case soc_feature_table_dma:
        case soc_feature_tslam_dma:
        case soc_feature_stg:
        case soc_feature_stg_xgs:
        case soc_feature_remap_ut_prio:
        case soc_feature_xgxs_v7:
        case soc_feature_phy_cl45:
        case soc_feature_aging_extended:
        case soc_feature_modmap:
        case soc_feature_l2_hashed:
        case soc_feature_l2_lookup_cmd:
        case soc_feature_l2_lookup_retry:
        case soc_feature_l2_user_table:
        case soc_feature_schan_hw_timeout:
        case soc_feature_mdio_enhanced:
        case soc_feature_dodeca_serdes:
        case soc_feature_rxdma_cleanup:
        case soc_feature_fe_maxframe:
        case soc_feature_l2x_parity:
        case soc_feature_l3x_parity:
        case soc_feature_l2_modfifo:
        case soc_feature_vlan_mc_flood_ctrl:
        case soc_feature_vlan_translation:
        case soc_feature_parity_err_tocpu:
        case soc_feature_nip_l3_err_tocpu:
        case soc_feature_l3mtu_fail_tocpu:
        case soc_feature_meter_adjust:
        case soc_feature_xgxs_power:
        case soc_feature_src_modid_blk:
        case soc_feature_src_modid_blk_ucast_override:
        case soc_feature_src_modid_blk_opcode_override:
        case soc_feature_egress_blk_ucast_override:
        case soc_feature_stat_jumbo_adj:
        case soc_feature_stat_xgs3:
        case soc_feature_port_trunk_index:
        case soc_feature_port_flow_hash:
        case soc_feature_cpuport_switched:
        case soc_feature_cpuport_mirror:
        case soc_feature_higig2:
        case soc_feature_color:
        case soc_feature_color_inner_cfi:
        case soc_feature_color_prio_map:
        case soc_feature_untagged_vt_miss:
        case soc_feature_module_loopback:
        case soc_feature_dscp_map_per_port:
        case soc_feature_egr_dscp_map_per_port:
        case soc_feature_dscp_map_mode_all:
        case soc_feature_higig_lookup:
        case soc_feature_egr_mirror_path:
        case soc_feature_trunk_extended:
        case soc_feature_hg_trunking:
        case soc_feature_hg_trunk_override:
        case soc_feature_egr_vlan_check:
        case soc_feature_cpu_proto_prio:
        case soc_feature_hg_trunk_failover:
        case soc_feature_trunk_egress:
        case soc_feature_force_forward:
        case soc_feature_port_egr_block_ctl:
        case soc_feature_bucket_support:
        case soc_feature_remote_learn_trust:
        case soc_feature_src_mac_group:
        case soc_feature_storm_control:
        case soc_feature_hw_stats_calc:
        case soc_feature_mac_learn_limit:
        case soc_feature_linear_drr_weight:
        case soc_feature_igmp_mld_support:
        case soc_feature_basic_dos_ctrl:
        case soc_feature_enhanced_dos_ctrl:
        case soc_feature_proto_pkt_ctrl:
        case soc_feature_vlan_ctrl:
        case soc_feature_big_icmpv6_ping_check:
        case soc_feature_trunk_group_overlay:
        case soc_feature_xport_convertible:
        case soc_feature_dual_hash:
        case soc_feature_dscp:
        case soc_feature_unimac:
        case soc_feature_ifg_wb_include_unimac:
        case soc_feature_xlmac:
        case soc_feature_clmac:
        case soc_feature_generic_table_ops:
        case soc_feature_vlan_translation_range:
        case soc_feature_static_pfm:
        case soc_feature_sgmii_autoneg:
        case soc_feature_mem_push_pop:
        case soc_feature_dcb_reason_hi:
        case soc_feature_new_sbus_format:
        case soc_feature_new_sbus_old_resp:
        case soc_feature_sbus_format_v4:
        case soc_feature_fifo_dma:
        case soc_feature_fifo_dma_active:
        case soc_feature_l2_pending:
        case soc_feature_internal_loopback:
        case soc_feature_vlan_action:
        case soc_feature_mac_based_vlan:
        case soc_feature_ip_subnet_based_vlan:
        case soc_feature_packet_rate_limit:
        case soc_feature_system_mac_learn_limit:
        case soc_feature_field:
        case soc_feature_field_mirror_ovr:
        case soc_feature_field_udf_higig:
        case soc_feature_field_udf_ethertype:
        case soc_feature_field_udf_offset_hg_114B:
        case soc_feature_field_udf_offset_hg2_110B:
        case soc_feature_field_comb_read:
        case soc_feature_field_wide:
        case soc_feature_field_slice_enable:
        case soc_feature_field_cos:
        case soc_feature_field_color_indep:
        case soc_feature_field_qual_drop:
        case soc_feature_field_qual_IpType:
        case soc_feature_field_qual_Ip6High:
        case soc_feature_field_qual_vlanformat_reverse:
        case soc_feature_field_ingress_global_meter_pools:
        case soc_feature_field_ingress_ipbm:
        case soc_feature_field_egress_flexible_v6_key:
        case soc_feature_field_egress_global_counters:
        case soc_feature_field_ing_egr_separate_packet_byte_counters:
        case soc_feature_field_egress_metering:
        case soc_feature_field_intraslice_double_wide:
        case soc_feature_field_virtual_slice_group:
        case soc_feature_field_virtual_queue:
        case soc_feature_field_action_timestamp:
        case soc_feature_field_action_l2_change:
        case soc_feature_field_action_redirect_nexthop:
        case soc_feature_field_action_pfc_class:
        case soc_feature_field_action_redirect_ipmc:
        case soc_feature_field_slice_dest_entity_select:
        case soc_feature_field_packet_based_metering:
        case soc_feature_lport_tab_profile:
        case soc_feature_field_oam_actions:
        case soc_feature_oam:
        case soc_feature_ignore_cmic_xgxs_pll_status:
        case soc_feature_use_double_freq_for_ddr_pll:
        case soc_feature_counter_parity:
        case soc_feature_extended_pci_error:
        case soc_feature_qos_profile:
        case soc_feature_rx_timestamp:
        case soc_feature_rx_timestamp_upper:
        case soc_feature_logical_port_num:
        case soc_feature_timestamp_counter:
        case soc_feature_modport_map_profile:
        case soc_feature_modport_map_dest_is_hg_port_bitmap:
        case soc_feature_eee:
        case soc_feature_xy_tcam:
        case soc_feature_xy_tcam_direct:
        case soc_feature_xy_tcam_28nm:
        case soc_feature_vlan_egr_it_inner_replace:
        case soc_feature_iproc:
        case soc_feature_iproc_7:
        case soc_feature_unified_port:
        case soc_feature_sbusdma:
        case soc_feature_mirror_encap_profile:
        case soc_feature_higig_misc_speed_support:
        case soc_feature_vpd_profile:
        case soc_feature_color_prio_map_profile:
        case soc_feature_mem_parity_eccmask:
        case soc_feature_l2_no_vfi:
        case soc_feature_gmii_clkout:
        case soc_feature_proxy_port_property:
        case soc_feature_system_reserved_vlan:
        case soc_feature_ser_parity:
        case soc_feature_mem_cache:
        case soc_feature_mem_wb_cache_reload:
        case soc_feature_ser_engine:
        case soc_feature_ser_system_scrub:
        case soc_feature_regs_as_mem:
        case soc_feature_int_common_init:
        case soc_feature_inner_tpid_enable:
        case soc_feature_no_tunnel:
        case soc_feature_eee_bb_mode:
        case soc_feature_hg_no_speed_change:
        case soc_feature_src_modid_base_index:
        case soc_feature_avs:
        case soc_feature_l2_overflow:
        case soc_feature_l2_overflow_bucket:
        case soc_feature_l2_learn_stats:
        case soc_feature_miml:
        case soc_feature_custom_header:
        case soc_feature_ing_capwap_parser:
        case soc_feature_port_extension:
        case soc_feature_niv:
        case soc_feature_gh2_rtag7:
        case soc_feature_no_vlan_vrf:
        case soc_feature_tscf:
        case soc_feature_sync_port_lport_tab:
            /* GH2 Service Metering Block is a simple version of Apache */
        case soc_feature_global_meter_v2:
        case soc_feature_global_meter_source_l2:
        case soc_feature_separate_ing_lport_rtag7_profile:
        case soc_feature_rtag7_port_profile:
        case soc_feature_no_vrf_stats:
        case soc_feature_flowcnt:
        case soc_feature_high_portcount_register:
        case soc_feature_asf:
        case soc_feature_hg2_light_in_portmacro:
        case soc_feature_tas:
        case soc_feature_use_local_1588:
        case soc_feature_common_egr_ip_tunnel_ser:
        case soc_feature_memory_2bit_ecc_ser:
        case soc_feature_switch_match:
        case soc_feature_pktpri_as_dot1p:
        case soc_feature_cpureg_dump:
        case soc_feature_hr2_dual_hash:
        case soc_feature_discard_ability:
        case soc_feature_wred_drop_counter_per_port:
        case soc_feature_ecn_wred:
        case soc_feature_port_encap_speed_max_config:
        case soc_feature_extended_cmic_error:
        case soc_feature_xlmac_speed_display_above_10g:
        case soc_feature_xlmac_timestamp_disable:
        case soc_feature_serdes_firmware_pos_by_host_endian:
        case soc_feature_dport_update_hl:
        case soc_feature_phy_interface_wb:
        case soc_feature_port_enable_encap_restore:
            return TRUE;
        case soc_feature_tsn:
        case soc_feature_tsn_sr:
        case soc_feature_tsn_sr_hsr:
        case soc_feature_tsn_sr_prp:
        case soc_feature_tsn_sr_802_1cb:
        case soc_feature_tsn_mtu_stu:
        case soc_feature_uc_mhost:
        case soc_feature_ptp:
        case soc_feature_timesync_nanosync:
        case soc_feature_nanosync_time_capture:
        case soc_feature_use_local_tod:
            return TRUE;
        case soc_feature_tsn_sr_auto_learn:
            if (NUM_PORT(unit) < 3) {
                /*
                 * At least 3 ports are required to setup SR Auto Learn table.
                 */
                return FALSE;
            } else {
                return TRUE;
            }
        case soc_feature_uc:
            if (soc_property_get(unit, spn_UC_VALID_BMP, 1) != 0) {
                return TRUE;
            } else {
                return FALSE;
            }
        case soc_feature_tsn_taf:
            return TRUE;
        case soc_feature_tsn_sr_flow_no_asf_select:
            if (flow_type == FL_FLOW_TYPE_SR) {
                return TRUE;
            } else {
                return FALSE;
            }
            break;
        case soc_feature_tsn_flow_no_asf_select:
            if (flow_type == FL_FLOW_TYPE_TSN) {
                return TRUE;
            } else {
                return FALSE;
            }
            break;
        case soc_feature_time_v3_no_bs:
            return FALSE;
        case soc_feature_e2ecc:
        case soc_feature_priority_flow_control:
        case soc_feature_gh_style_pfc_config:
            return TRUE;
        case soc_feature_time_support:
        case soc_feature_time_v3:
        case soc_feature_timesync_support:
        case soc_feature_timesync_v3:
        case soc_feature_timesync_timestampingmode:
            if (SAL_BOOT_BCMSIM) {
                return FALSE;
            } else {
                return TRUE;
            }
        case soc_feature_gh2_my_station:
            return TRUE;
        case soc_feature_preempt_mib_support:
        case soc_feature_clmib_support:
        case soc_feature_ecmp_hash_bit_count_select:
            return TRUE;
        case soc_feature_field_multi_stage:
            return TRUE;
        case soc_feature_field_slice_size128:
            /* Ranger3/Quartz has 256 entries per slice; GH2 has 128. */
            return FALSE;
        case soc_feature_l3_no_ecmp:
        case soc_feature_miml_no_l3:
#if 1
            if (!SAL_BOOT_SIMULATION) {
                return FALSE;
            } else {
#endif
                return FALSE;
            }
            break;
        case soc_feature_e2efc:
            return TRUE;
        /* L3 related features */
        case soc_feature_l3:
        case soc_feature_l3_ip6:
        case soc_feature_l3_entry_key_type:
        case soc_feature_lpm_tcam:
        case soc_feature_ip_mcast:
        case soc_feature_ip_mcast_repl:
        case soc_feature_ipmc_unicast:
        case soc_feature_ipmc_use_configured_dest_mac:
        case soc_feature_urpf:
        case soc_feature_l3mc_use_egress_next_hop:
        case soc_feature_l3_sgv:
        case soc_feature_l3_dynamic_ecmp_group:
        case soc_feature_l3_ingress_interface:
        case soc_feature_l3_iif_zero_invalid:
        case soc_feature_l3_iif_under_4k:
        case soc_feature_vxlan_lite:
        case soc_feature_tunnel_any_in_6:
#if 1
            if (!SAL_BOOT_SIMULATION) {
                return TRUE;
            } else {
#endif
                return TRUE;
            }
            break;
        case soc_feature_untethered_otp:
#if 1
            return (!SAL_BOOT_SIMULATION);
#endif
        case soc_feature_trunk_group_size:
            return TRUE;
        case soc_feature_global_meter:
            if (SAL_BOOT_QUICKTURN) {
                return FALSE;
            } else {
                return TRUE;
            }
        default:
            return FALSE;
    }
}

/*
 * Function:
 *      soc_fl_tdm_size_get
 * Purpose:
 *      Get the TDM array size.
 * Parameters:
 *      unit - unit number.
 *      tdm_size (OUT) - TDM array size
 * Returns:
 *      SOC_E_XXX
 */
int soc_fl_tdm_size_get(int unit, int *tdm_size)
{
    _fl_option_info_t *matched_opt_info;

    /* The function should be called only if the TDM is programmed */
    if (matched_option_idx[unit] == -1) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, "Warning: soc_firelight_port_config_init "
                             "should be invoked first! \n")));
        return SOC_E_INIT;
    }

    /* Return the TDM size */
    matched_opt_info = &_fl_option_port_config[unit][matched_option_idx[unit]];
    *tdm_size = matched_opt_info->tdm_table_size;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fl_tdm_size_set
 * Purpose:
 *      Set the TDM array size.
 * Parameters:
 *      unit - unit number.
 *      tdm_size (OUT) - TDM array size
 * Returns:
 *      SOC_E_XXX
 */
int soc_fl_tdm_size_set(int unit, int tdm_size)
{
    _fl_option_info_t *matched_opt_info;

    matched_opt_info = &_fl_option_port_config[unit][matched_option_idx[unit]];

    /* AutoTDM option */
    matched_opt_info->tdm_table_size = tdm_size;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fl_64q_port_check
 * Purpose:
 *      Check if the port(logical) is capable of configured up to 64 COSQ.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      port - logical port.
 *      is_64q (OUT) - with 64 COSQ or not
 * Returns:
 *      BCM_E_XXX
 */
int soc_fl_64q_port_check(int unit, soc_port_t port, int *is_64q_port)
{
    int phy_port;
    int mmu_port;

    /* Array access protection. */
    if ((port < 0) || (port >= SOC_MAX_LOGICAL_PORTS)){
        return SOC_E_PORT;
    }

    *is_64q_port = 0;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    if ((mmu_port >= SOC_FL_64Q_MMU_PORT_IDX_MIN) &&
        (mmu_port <= SOC_FL_64Q_MMU_PORT_IDX_MAX)) {
        *is_64q_port = 1;
    } else {
        *is_64q_port = 0;
    }
    return SOC_E_NONE;
}

/*
 * For calculate FL MMU_MAX/MIN_BUCKET_QLAYER table index.
 *    - mmu port 0~57 : 8 entries
 *    - mmu port 58~65 : 64 entries
 *  (IN) p : logical port
 *  (IN) q : cosq
 *  (OUT) idx : the entry index of MMU_MAX/MIN_BUCKET_QLAYER table
 */
int
soc_fl_mmu_bucket_qlayer_index(int unit, int p, int q, int *idx)
{
    int is_64q;
    int phy_port;
    int mmu_port;

    is_64q = 0;
    phy_port = SOC_INFO(unit).port_l2p_mapping[p];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];

    SOC_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, p, &is_64q));


    if (!is_64q) {
        *idx = (mmu_port * SOC_FL_LEGACY_QUEUE_NUM) + q;
    }  else {
        *idx = (SOC_FL_64Q_MMU_PORT_IDX_MIN * SOC_FL_LEGACY_QUEUE_NUM) + \
               ((mmu_port-SOC_FL_64Q_MMU_PORT_IDX_MIN) * \
                 SOC_FL_QLAYER_COSQ_PER_PORT_NUM) + \
               q;
    }
    return SOC_E_NONE;
}

/*
 * For calculate FL MMU_MAX/MIN_BUCKET_QGROUP table index.
 *    - mmu port 0~57 : not available
 *    - mmu port 58~65 : 8 entries
 *  (IN) p : logical port
 *  (IN) g : queue group id
 *  (OUT) idx : the entry index of MMU_MAX/MIN_BUCKET_QGROUP table
 */
int
soc_fl_mmu_bucket_qgroup_index(int unit, int p, int g, int *idx)
{
    int is_64q = 0;
    int phy_port;
    int mmu_port;

    SOC_IF_ERROR_RETURN(soc_fl_64q_port_check(unit, p, &is_64q));
    if (!is_64q) {
        /* MMU port 0~57 do not have QGROUP tables */
        return SOC_E_PARAM;
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[p];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    *idx = ((mmu_port - SOC_FL_64Q_MMU_PORT_IDX_MIN) * \
             SOC_FL_QGROUP_PER_PORT_NUM) + g;

    return SOC_E_NONE;
}

/*
 * Firelight chip driver functions.
 */
soc_functions_t soc_firelight_drv_funs = {
    _soc_firelight_misc_init,
    _soc_firelight_mmu_init,
    _soc_firelight_age_timer_get,
    _soc_firelight_age_timer_max_get,
    _soc_firelight_age_timer_set,
    NULL,
    _soc_firelight_tscx_reg_read,
    _soc_firelight_tscx_reg_write,
    soc_firelight_bond_info_init,
    _soc_firelight_misc_deinit
};

soc_error_t
soc_firelight_granular_speed_get(int unit, soc_port_t port, int *speed)
{
    soc_info_t  *si = &SOC_INFO(unit);

    /* CLMAC and XLMAC only */
    if (!IS_CL_PORT(unit, port) && !IS_XL_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    *speed = fl_port_speed[unit][si->port_l2p_mapping[port]];

    return SOC_E_NONE;
}

int
soc_firelight_port_speed_update(int unit, soc_port_t port, int speed)
{
    soc_info_t  *si = &SOC_INFO(unit);

    fl_port_speed[unit][si->port_l2p_mapping[port]] = speed;

    return SOC_E_NONE;
}

static int fl_qmode_config_init[SOC_MAX_NUM_DEVICES] = { 0 };
static int fl_qmode_config[SOC_MAX_NUM_DEVICES][SOC_FL_PM4X10_QTC_COUNT] = { { 0 } };

/* Get qmode by lport */
void
soc_fl_port_qsgmii_mode_get(int unit, soc_port_t port, int* qsgmii_mode)
{
    soc_info_t *si;
    int phy_port, base_port;

    si = &SOC_INFO(unit);

    if (!fl_qmode_config_init[unit]) {
        fl_qmode_config[unit][0] = soc_property_phys_port_get(unit, 2, spn_PORT_GMII_MODE, 0);
        fl_qmode_config[unit][1] = soc_property_phys_port_get(unit, 18, spn_PORT_GMII_MODE, 0);
        fl_qmode_config[unit][2] = soc_property_phys_port_get(unit, 34, spn_PORT_GMII_MODE, 0);

        fl_qmode_config_init[unit] = 1;
    }

    phy_port = si->port_l2p_mapping[port];
    base_port = SOC_FL_PORT_BLOCK_BASE_PORT(port);

    if ((phy_port >= 2) && (phy_port <= 49)) {
        if (base_port == 2) {
            *qsgmii_mode = fl_qmode_config[unit][0];
        } else if (base_port == 18) {
            *qsgmii_mode = fl_qmode_config[unit][1];
        } else if (base_port == 34) {
            *qsgmii_mode = fl_qmode_config[unit][2];
        }
    } else {
        *qsgmii_mode = 0;
    }

    return;
}


void
soc_fl_port_qsgmii_mode_get_by_qtc_num(int unit, int qtc_num, int* qsgmii_mode)
{
    if ((qtc_num < 0) && (qtc_num > SOC_FL_PM4X10_QTC_COUNT)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                    "unit %d: Error qtc_num(%d) in %s\n"),
             unit, qtc_num, __func__));
        return;
    }
    if (!fl_qmode_config_init[unit]) {
        fl_qmode_config[unit][0] = soc_property_phys_port_get(unit, 2, spn_PORT_GMII_MODE, 0);
        fl_qmode_config[unit][1] = soc_property_phys_port_get(unit, 18, spn_PORT_GMII_MODE, 0);
        fl_qmode_config[unit][2] = soc_property_phys_port_get(unit, 34, spn_PORT_GMII_MODE, 0);

        fl_qmode_config_init[unit] = 1;
    }

    *qsgmii_mode = fl_qmode_config[unit][qtc_num];

    return;
}

/*
 * Function:
 *      soc_firelight_port_mode_get
 * Description:
 *      Get port mode in a TSC core by logical port number.
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each TSC3 can be configured into following 5 mode:
 *                     Lane number    return mode
 *                  0    1    2    3
 *   ------------  ---  ---  ---  --- -----------------------
 *      quad port  10G  10G  10G  10G SOC_FL_PORT_MODE_SINGLE
 *   tri_012 port  10G  10G  20G   x  SOC_FL_PORT_MODE_TRI_012
 *   tri_023 port  20G   x   10G  10G SOC_FL_PORT_MODE_TRI_023
 *      dual port  20G   x   20G   x  SOC_FL_PORT_MODE_DUAL
 *    single port  40G   x    x    x  SOC_FL_PORT_MODE_QUAD
 */
int
soc_firelight_port_mode_get(int unit, int logical_port, int *mode)
{
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[FL_PORTS_PER_TSC];

    si = &SOC_INFO(unit);
    /* Each core is 4 lane core to get the first divide by 4 then multiply by 4
     * physical port is 2 => ((2-2)/4)*4+2 = 2
     * physical port is 7 => ((7-2)/4)*4+2 = 6
     */
    first_phyport = (((si->port_l2p_mapping[logical_port]-2)/4)*4)+2;

    for (lane = 0; lane < FL_PORTS_PER_TSC; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_FL_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_FL_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_FL_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_FL_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_FL_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_firelight_support_speeds
 * Purpose:
 *      Get the supported speed of port for specified lanes
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      ports                - (IN)  Logical port number.
 *      speed_mask           - (IN)  Bitmap for supported speed.
 * Returns:
 *      BCM_E_xxx
 */
int
soc_firelight_support_speeds(int unit, int port, uint32 *speed_mask)
{
    uint32 speed_valid;
    int phy_port, lanes;
    soc_info_t *si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    lanes = si->port_num_lanes[port];

    switch(lanes) {
    case 1:
        /* Single Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB;
        if (PORT_IS_FALCON(unit, phy_port)) {
            speed_valid |= SOC_PA_SPEED_25GB | SOC_PA_SPEED_27GB;
            if (!IS_HG_PORT(unit,port)) {
                speed_valid |= SOC_PA_SPEED_2500MB | SOC_PA_SPEED_5000MB;
            }
        } else {
            speed_valid |=  SOC_PA_SPEED_10MB | SOC_PA_SPEED_100MB |
                            SOC_PA_SPEED_2500MB | SOC_PA_SPEED_5000MB;
        }
        break;
    case 2:
        /* Dual Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB;
        if (PORT_IS_FALCON(unit, phy_port)) {
            speed_valid |= SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                           SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB;
        } else {
            speed_valid |=  SOC_PA_SPEED_10GB | SOC_PA_SPEED_11GB;
        }
        break;
    case 4:
        /* Quad Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                      SOC_PA_SPEED_100GB;
        break;
    default:
        return SOC_E_PARAM;
    }

    *speed_mask = speed_valid;

    return SOC_E_NONE;
}

#endif /* BCM_FIRELIGHT_SUPPORT */
