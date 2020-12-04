/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:      latency.c
 * Purpose:   Switch Latency Bypass feature support for Tomahawk SKUs
 * Requires:  soc/tomahawk.h
 */

#include <soc/tomahawk.h>

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/property.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <shared/bitop.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/mbcm.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif


#define SBUS_ADDR_BEAT_STAGEID    26

#define _SOC_TH_LATENCY_INIT_VALIDATE(unit, ret)                              \
        if (!_soc_th_latency_ctrl[unit] || !_soc_th_latency_ctrl[unit]->init) \
            return (ret);

#define _SOC_TH_LATENCY_CFG_VALIDATE(cfg, ret)             \
        if (!(((cfg) >= SOC_SWITCH_BYPASS_MODE_NONE) &&    \
              ((cfg) <= SOC_SWITCH_BYPASS_MODE_EFP)))      \
            return (ret);

#define _SOC_SWITCH_BYPASS_MODE_INVALID		0xAB

/* runtime enable/disable knob */
/* temporary, will last only until system testing */
#define _SOC_TH_LATENCY_RT_ENABLE_CHK(unit)                       \
        if (!soc_property_get(unit, "switch_bypass_enable", 1))   \
            return SOC_E_NONE;


extern soc_th_latency_table_cfg_t
soc_th_tbl_cfg[SOC_MAX_NUM_DEVICES];


typedef enum {
    _SOC_TH_IPIPE_STAGE_IDB      =  0,
    _SOC_TH_IPIPE_STAGE_IDB_IS   =  1,
    _SOC_TH_IPIPE_STAGE_IDB_OBM0 =  2,
    _SOC_TH_IPIPE_STAGE_IDB_OBM1 =  3,
    _SOC_TH_IPIPE_STAGE_IDB_OBM2 =  4,
    _SOC_TH_IPIPE_STAGE_IDB_OBM3 =  5,
    _SOC_TH_IPIPE_STAGE_IDB_OBM4 =  6,
    _SOC_TH_IPIPE_STAGE_IDB_OBM5 =  7,
    _SOC_TH_IPIPE_STAGE_IDB_OBM6 =  8,
    _SOC_TH_IPIPE_STAGE_IDB_OBM7 =  9,
    _SOC_TH_IPIPE_STAGE_ICFG     = 10,
    _SOC_TH_IPIPE_STAGE_IPARS    = 11,
    _SOC_TH_IPIPE_STAGE_IVXLT    = 12,
    _SOC_TH_IPIPE_STAGE_IVP      = 13,
    _SOC_TH_IPIPE_STAGE_IFWD     = 14,
    _SOC_TH_IPIPE_STAGE_ISS      = 15,
    _SOC_TH_IPIPE_STAGE_IRSEL1   = 16,
    _SOC_TH_IPIPE_STAGE_ISW1     = 17,
    _SOC_TH_IPIPE_STAGE_IFP      = 18,
    _SOC_TH_IPIPE_STAGE_IRSEL2   = 19,
    _SOC_TH_IPIPE_STAGE_ISW2     = 20,
    _SOC_TH_IPIPE_STAGE_ISW3     = 21,
    _SOC_TH_IPIPE_STAGE_ISBS     = 22,
    _SOC_TH_IPIPE_STAGE_CEV      = 23,
    _SOC_TH_IPIPE_STAGE_L2_MGMT  = 24,
    _SOC_TH_IPIPE_STAGE_FLEX_CTR = 26
} _soc_th_ipipe_stage_e;

typedef enum {
    _SOC_TH_EPIPE_STAGE_EARB     =  0,
    _SOC_TH_EPIPE_STAGE_EL3      =  1,
    _SOC_TH_EPIPE_STAGE_EVLAN    =  2,
    _SOC_TH_EPIPE_STAGE_ESW      =  3,
    _SOC_TH_EPIPE_STAGE_EIPT     =  4,
    _SOC_TH_EPIPE_STAGE_EHCPM    =  5,
    _SOC_TH_EPIPE_STAGE_EPMOD    =  6,
    _SOC_TH_EPIPE_STAGE_EFPPARS  =  7,
    _SOC_TH_EPIPE_STAGE_EFP      =  8,
    _SOC_TH_EPIPE_STAGE_EFPMOD   =  9,
    _SOC_TH_EPIPE_STAGE_EDATABUF = 10,
    _SOC_TH_EPIPE_STAGE_ESBS     = 11
} _soc_th_epipe_stage_e;

typedef enum {
    _SOC_TH2_IPIPE_STAGE_IDB       =  0,
    _SOC_TH2_IPIPE_STAGE_IDB_IS    =  1,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM0  =  2,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM1  =  3,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM2  =  4,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM3  =  5,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM4  =  6,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM5  =  7,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM6  =  8,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM7  =  9,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM8  = 10,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM9  = 11,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM10 = 12,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM11 = 13,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM12 = 14,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM13 = 15,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM14 = 16,
    _SOC_TH2_IPIPE_STAGE_IDB_OBM15 = 17,
    _SOC_TH2_IPIPE_STAGE_ISBS_IDB  = 18,
    _SOC_TH2_IPIPE_STAGE_ICFG      = 19,
    _SOC_TH2_IPIPE_STAGE_IPARS     = 20,
    _SOC_TH2_IPIPE_STAGE_IVXLT     = 21,
    _SOC_TH2_IPIPE_STAGE_IVP       = 22,
    _SOC_TH2_IPIPE_STAGE_IFWD      = 23,
    _SOC_TH2_IPIPE_STAGE_IFWD_2    = 24,
    _SOC_TH2_IPIPE_STAGE_IFWD_3    = 25,
    _SOC_TH2_IPIPE_STAGE_IFWD_4    = 26,
    _SOC_TH2_IPIPE_STAGE_IFWD_5    = 27,
    _SOC_TH2_IPIPE_STAGE_IFWD_6    = 28,
    _SOC_TH2_IPIPE_STAGE_IFWD_7    = 29,
    _SOC_TH2_IPIPE_STAGE_IFWD_8    = 30,
    _SOC_TH2_IPIPE_STAGE_ISS       = 31,
    _SOC_TH2_IPIPE_STAGE_IRSEL1    = 32,
    _SOC_TH2_IPIPE_STAGE_ISW1      = 33,
    _SOC_TH2_IPIPE_STAGE_IFP       = 34,
    _SOC_TH2_IPIPE_STAGE_IRSEL2    = 35,
    _SOC_TH2_IPIPE_STAGE_ISW2      = 36,
    _SOC_TH2_IPIPE_STAGE_ISW3      = 37,
    _SOC_TH2_IPIPE_STAGE_ISBS      = 38,
    _SOC_TH2_IPIPE_STAGE_CEV       = 39,
    _SOC_TH2_IPIPE_STAGE_L2_MGMT   = 40,
    _SOC_TH2_IPIPE_STAGE_L2_MGMT_2 = 41,
    _SOC_TH2_IPIPE_STAGE_ISW4      = 42,
    _SOC_TH2_IPIPE_STAGE_ISBS_ISW4 = 43,
    _SOC_TH2_IPIPE_STAGE_FLEX_CTR  = 44
} _soc_th2_ipipe_stage_e;

typedef enum {
    _SOC_TH2_EPIPE_STAGE_EARB      =  0,
    _SOC_TH2_EPIPE_STAGE_EL3       =  1,
    _SOC_TH2_EPIPE_STAGE_EVLAN     =  2,
    _SOC_TH2_EPIPE_STAGE_ESW       =  3,
    _SOC_TH2_EPIPE_STAGE_EIPT      =  4,
    _SOC_TH2_EPIPE_STAGE_EHCPM     =  5,
    _SOC_TH2_EPIPE_STAGE_EPMOD     =  6,
    _SOC_TH2_EPIPE_STAGE_EFPPARS   =  7,
    _SOC_TH2_EPIPE_STAGE_EFP       =  8,
    _SOC_TH2_EPIPE_STAGE_EFPMOD    =  9,
    _SOC_TH2_EPIPE_STAGE_EDATABUF  = 10,
    _SOC_TH2_EPIPE_STAGE_ESBS      = 11
} _soc_th2_epipe_stage_e;

/* Ingress Latency Config */
typedef struct _soc_th_igr_latency_cfg_s {
    uint32 ipars    : 1;    /* IPARS Latency Mode */
    uint32 ivxlt    : 1;    /* IVXLT Bypass */
    uint32 ivp      : 1;    /* IVP Bypass */
    uint32 ifwd     : 1;    /* IFWD Latency Mode */
    uint32 irsel1   : 1;    /* IRSEL1 Latency Mode */
    uint32 ifp      : 1;    /* IFP Bypass */
    uint32 irsel2   : 2;    /* IRSEL2 Latency Mode */
    uint32 isw2     : 1;    /* ISW2 Latency Mode */
    uint32 ecmp     : 2;    /* ECMP Mode */
    uint32 reserved : 21;   /* reserved */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th_igr_latency_cfg_t;

/* Egress Latency Config */
typedef struct _soc_th_egr_latency_cfg_s {
    uint32 el3              : 1;    /* EL3 Latency Mode */
    uint32 evlan            : 1;    /* EVLAN Latency Mode */
    uint32 efp              : 1;    /* EFP Bypass */
    uint32 otag_action      : 3;    /* Single Outer Tag - action */
    uint32 potag_action     : 3;    /* Pri Outer Tag - action */
    uint32 ut_otag_action   : 3;    /* Untagged - action */
    uint32 outer_pri        : 1;    /* Outer PRI action */
    uint32 outer_cfi        : 1;    /* Outer CFI action */
    uint32 outer_tpid       : 16;   /* Outer Tag - TPID */
    uint32 reserved1        : 2;    /* reserved */
    uint32 profile_ptr      : 4;    /* map PRI-CFI */
    uint32 pri              : 3;    /* PRI for Outer tag */
    uint32 cfi              : 1;    /* CFI for Outer tag */
    uint32 reserved2        : 24;   /* reserved */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th_egr_latency_cfg_t;

/* Latency core config */
typedef struct _soc_th_latency_core_cfg_s {
    _soc_th_igr_latency_cfg_t igr;
    _soc_th_egr_latency_cfg_t egr;
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th_latency_core_cfg_t;

/* Latency ctrl */
typedef struct _soc_th_latency_ctrl_s {
    uint8   init;
    int     latency;
    soc_th_latency_table_cfg_t tbl_cfg;
} _soc_th_latency_ctrl_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* Latency Warmboot */
typedef struct _soc_th_latency_wb_s {
    int unit;
    _soc_th_latency_ctrl_t latency_ctrl;
} _soc_th_latency_wb_t;
#endif


/* Latency Bypass - Core Config Table  */
static const _soc_th_latency_core_cfg_t
_soc_th_latency_cfg_tbl[] = {
    { {0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0x8100, 0, 0, 0, 0, 0} }, /* Full Features */
    { {0, 1, 0, 1, 0, 0, 1, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0, 0x8100, 0, 0, 0, 0, 0} }, /* Balanced L2 + L3 */
    { {1, 1, 1, 1, 1, 1, 2, 1, 2, 0}, {1, 1, 1, 0, 0, 0, 0, 0, 0x8100, 0, 0, 0, 0, 0} },  /* Low Latency L2 */
    { {0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 1, 0, 0, 0, 0, 0, 0x8100, 0, 0, 0, 0, 0} } /* EFP Latency*/
};

/* Latency Control */
static _soc_th_latency_ctrl_t*
_soc_th_latency_ctrl[SOC_MAX_NUM_DEVICES] = {NULL};

/* latency subsystem billboard */
static const char *
lstr[] = {"normal", "balanced (L2/L3)", "low (L2)", "EFP"};


/******************************************************************************
 * Name: _soc_th_igr_latency_config                                           *
 * Description:                                                               *
 *     Configure ingress latency settings on the switch                       *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Latency Mode                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_igr_latency_config(
    int   unit,
    int   latency)
{
    uint32 rval = 0;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_CFG_VALIDATE(latency, SOC_E_PARAM);

    /* ingress latency config */
    if (!SOC_IS_TOMAHAWK3(unit)) {
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IPARS_LATENCY_MODEf,
                          _soc_th_latency_cfg_tbl[latency].igr.ipars);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IVXLT_BYPASSf,
                          _soc_th_latency_cfg_tbl[latency].igr.ivxlt);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IVP_BYPASSf,
                          _soc_th_latency_cfg_tbl[latency].igr.ivp);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IFWD_LATENCY_MODEf,
                      _soc_th_latency_cfg_tbl[latency].igr.ifwd); 

        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IRSEL1_LATENCY_MODEf,
                          _soc_th_latency_cfg_tbl[latency].igr.irsel1);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IFP_BYPASSf,
                          _soc_th_latency_cfg_tbl[latency].igr.ifp);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, IRSEL2_LATENCY_MODEf,
                         _soc_th_latency_cfg_tbl[latency].igr.irsel2);
        soc_reg_field_set(unit, ING_LATENCY_CONTROLr, &rval, ISW2_LATENCY_MODEf,
                         _soc_th_latency_cfg_tbl[latency].igr.isw2);
        SOC_IF_ERROR_RETURN(WRITE_ING_LATENCY_CONTROLr(unit, rval));

        /* ECMP latency config */
        SOC_IF_ERROR_RETURN(READ_ECMP_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf,
                          _soc_th_latency_cfg_tbl[latency].igr.ecmp);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_efp_latency_config                                           *
 * Description:                                                               *
 *     Configure EFP latency settings on the switch                           *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Latency Mode                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_efp_latency_config(
    int   unit,
    int   latency)
{
    uint32 val = 0;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_CFG_VALIDATE(latency, SOC_E_PARAM);

    soc_reg_field_set(unit, EGR_LATENCY_MODEr, &val, EFP_BYPASSf,
                      _soc_th_latency_cfg_tbl[latency].egr.efp);

    SOC_IF_ERROR_RETURN(WRITE_EGR_LATENCY_MODEr(unit, val));

    return SOC_E_NONE;
   
}
/******************************************************************************
 * Name: _soc_th_egr_latency_config                                           *
 * Description:                                                               *
 *     Configure egress latency settings on the switch                        *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Latency Mode                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
STATIC int
_soc_th_egr_latency_config(
    int   unit,
    int   latency)
{
    uint32 rval = 0;
    soc_port_t port;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_CFG_VALIDATE(latency, SOC_E_PARAM);

    if (!SOC_IS_TOMAHAWK3(unit)) {
        soc_reg_field_set(unit, EGR_LATENCY_MODEr, &rval, EL3_LATENCY_MODEf,
                          _soc_th_latency_cfg_tbl[latency].egr.el3);
        soc_reg_field_set(unit, EGR_LATENCY_MODEr, &rval, EVLAN_LATENCY_MODEf,
                          _soc_th_latency_cfg_tbl[latency].egr.evlan);
    }
    soc_reg_field_set(unit, EGR_LATENCY_MODEr, &rval, EFP_BYPASSf,
                      _soc_th_latency_cfg_tbl[latency].egr.efp);

    SOC_IF_ERROR_RETURN(WRITE_EGR_LATENCY_MODEr(unit, rval));

    if (SOC_SWITCH_BYPASS_MODE_NONE != latency) {
        rval = 0;

        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, SOT_OTAG_ACTIONf,
                          _soc_th_latency_cfg_tbl[latency].egr.otag_action);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, SOT_POTAG_ACTIONf,
                          _soc_th_latency_cfg_tbl[latency].egr.potag_action);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, UT_OTAG_ACTIONf,
                          _soc_th_latency_cfg_tbl[latency].egr.ut_otag_action);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, CHANGE_OUTER_PRIf,
                          _soc_th_latency_cfg_tbl[latency].egr.outer_pri);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, CHANGE_OUTER_CFIf,
                          _soc_th_latency_cfg_tbl[latency].egr.outer_cfi);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, OUTER_TPIDf,
                          _soc_th_latency_cfg_tbl[latency].egr.outer_tpid);
        soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr,
                          &rval, OUTER_PRI_CFI_MAPPING_PTRf,
                          _soc_th_latency_cfg_tbl[latency].egr.profile_ptr);

        /* EGR VLAN config */
        PBMP_PORT_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(
                WRITE_EGR_VLAN_TAG_ACTION_FOR_BYPASSr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_latency_resource_avail                                       *
 * Description:                                                               *
 *     Returns if the specified resource is supported/available for the       *
 *     current Latency mode configured on the switch                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Resource Type (Register, Memory, Feature)                            *
 *     - Resource Name (Enumeration)                                          *
 *     - latency mode                                                         *
 * Returns:                                                                   *
 *     - TRUE, if the resource is available                                   *
 *     - FALSE, if the resource is unavailable                                *
 */
STATIC int
_soc_th_latency_resource_avail(
    int                unit,
    soc_th_resource_e  type,
    int                resource,
    int                latency)
{
    uint32 block, pipe_stage;
    uint8  mode;
    uint32 access_latency_mode;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_RT_ENABLE_CHK(unit);

    _SOC_TH_LATENCY_CFG_VALIDATE(latency, SOC_E_NONE);

    if (_SOC_TH_MEM == type) {
        if ((SOC_SWITCH_BYPASS_MODE_NONE == latency) &&
            SOC_MEM_IS_VALID(unit, resource)) {
            return TRUE;
        }

        if (!SOC_MEM_IS_VALID(unit, resource)) {
            return FALSE;
        }
        block = SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, resource));
        pipe_stage =
            (SOC_MEM_INFO(unit, resource).base >> SBUS_ADDR_BEAT_STAGEID);
        access_latency_mode = SOC_MEM_INFO(unit, resource).access_latency_mode;
    } else if (_SOC_TH_REG == type) {
        if ((SOC_SWITCH_BYPASS_MODE_NONE == latency) &&
            SOC_REG_IS_VALID(unit, resource)) {
            return TRUE;
        }

        if (!SOC_REG_IS_VALID(unit, resource)) {
            return FALSE;
        }
        block = SOC_REG_INFO(unit, resource).block[0];
        pipe_stage =
            (SOC_REG_INFO(unit, resource).offset >> SBUS_ADDR_BEAT_STAGEID);
        access_latency_mode = SOC_REG_INFO(unit, resource).access_latency_mode;
    } else {
        return TRUE;
    }

#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        switch(block) {
            case SOC_BLK_IPIPE:
                switch(pipe_stage) {
                    case _SOC_TH2_IPIPE_STAGE_IPARS:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ipars;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IVXLT:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ivxlt;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IVP:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ivp;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IFWD:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ifwd;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IRSEL1:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.irsel1;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IFP:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ifp;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_IRSEL2:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.irsel2;
                        break;
                    case _SOC_TH2_IPIPE_STAGE_ISW2:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.isw2;
                        break;
                    default:
                        return TRUE;
                }
                break;

            case SOC_BLK_EPIPE:
                switch(pipe_stage) {
                    case _SOC_TH2_EPIPE_STAGE_EL3:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.el3;
                        break;
                    case _SOC_TH2_EPIPE_STAGE_EVLAN:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.evlan;
                        break;
                    case _SOC_TH2_EPIPE_STAGE_EFP:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.efp;
                        break;
                    default:
                        return TRUE;
                }
                break;

            default:
                return TRUE;
        }
    } else
#endif
    {
        switch(block) {
            case SOC_BLK_IPIPE:
                switch(pipe_stage) {
                    case _SOC_TH_IPIPE_STAGE_IPARS:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ipars;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IVXLT:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ivxlt;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IVP:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ivp;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IFWD:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ifwd;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IRSEL1:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.irsel1;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IFP:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.ifp;
                        break;
                    case _SOC_TH_IPIPE_STAGE_IRSEL2:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.irsel2;
                        break;
                    case _SOC_TH_IPIPE_STAGE_ISW2:
                        mode = _soc_th_latency_cfg_tbl[latency].igr.isw2;
                        break;
                    default:
                        return TRUE;
                }
                break;

            case SOC_BLK_EPIPE:
                switch(pipe_stage) {
                    case _SOC_TH_EPIPE_STAGE_EL3:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.el3;
                        break;
                    case _SOC_TH_EPIPE_STAGE_EVLAN:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.evlan;
                        break;
                    case _SOC_TH_EPIPE_STAGE_EFP:
                        mode = _soc_th_latency_cfg_tbl[latency].egr.efp;
                        break;
                    default:
                        return TRUE;
                }
                break;

            default:
                return TRUE;
        }
    }

    return (mode <= access_latency_mode);
}

/******************************************************************************
 * Name: _soc_th_latency_mem_filter                                           *
 * Description:                                                               *
 *     Filters out memories/tables unsupported for the current Latency mode   *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 */
STATIC int
_soc_th_latency_mem_filter(int unit)
{
    soc_mem_t mem;
    soc_persist_t *sop;
    SHR_BITDCL *flags;
    int latency;

    sop = SOC_PERSIST(unit);
    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }

        if (!soc_th_latency_mem_avail(unit, mem, latency)) {
            sop->memState[mem].index_max = -1;
            flags = (SHR_BITDCL *) &SOC_MEM_INFO(unit, mem).flags;
            SHR_BITCLR(flags, (SOC_MEM_FLAG_VALID - 1));
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_latency_reg_filter                                           *
 * Description:                                                               *
 *     Filters out registers unsupported for the current Latency mode         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 */
STATIC int
_soc_th_latency_reg_filter(int unit)
{
    soc_reg_t reg;
    int latency;
    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }

        if (!soc_th_latency_reg_avail(unit, reg, latency)) {
            SOC_REG_INFO(unit, reg).regtype = soc_invalidreg;
        }
    }

    if (SOC_SWITCH_BYPASS_MODE(unit) && SOC_REG_IS_VALID(unit, SHARED_TABLE_HASH_CONTROLr)) {
        /*
         * SW WAR to explicitly invalidate SHARED_TABLE_HASH_CONTROL register as this
         * register is not accessible in low latency modes
         */
        SOC_REG_INFO(unit, SHARED_TABLE_HASH_CONTROLr).regtype = soc_invalidreg;
    }

    return SOC_E_NONE;
}
static int mode[SOC_MAX_NUM_DEVICES];
static int is_mode_get[SOC_MAX_NUM_DEVICES] = {0};

/******************************************************************************
 * Name: soc_th_latency_get                                                   *
 * Description:                                                               *
 *     Get the Latency Bypass mode configured on the Switch.                  *
 *     TH ASF is an internal client for this API.                             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 * OUT params:                                                                *
 *     - Latency Mode                                                         *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 */
int
soc_th_latency_get(
    int              unit,
    OUT int* const   latency)
{
    uint16 dev_id = 0;
    int is_td3 = 0;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!latency) {
        return SOC_E_PARAM;
    }

    soc_cm_get_id(unit, &dev_id, NULL);
    if (dev_id >= BCM56870_DEVICE_ID &&
        dev_id <= BCM56870_DEVICE_ID + 10) {
        is_td3 = 1;
    }

    if (_soc_th_latency_ctrl[unit] && _soc_th_latency_ctrl[unit]->init) {
        *latency = _soc_th_latency_ctrl[unit]->latency;
    } else if (!is_mode_get[unit]) {
        /* reqd. for feature & mem checks that happen prior to "init soc" */
        *latency = soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                                    SOC_SWITCH_BYPASS_MODE_NONE);

        if (is_td3 || !((*latency >= SOC_SWITCH_BYPASS_MODE_NONE) &&
            (*latency <= SOC_SWITCH_BYPASS_MODE_EFP))) {
            *latency = SOC_SWITCH_BYPASS_MODE_NONE;
        }

        mode[unit] = *latency;
        is_mode_get[unit] = 1;
    } else {
        *latency = mode[unit];
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_latency_bypassed                                              *
 * Description:                                                               *
 *     Returns if the specified feature is supported/available for the        *
 *     current Latency mode configured on the switch                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Feature (soc_feature_*)                                              *
 * Returns:                                                                   *
 *     - FALSE, if the feature is supported                                   *
 *     - TRUE, if the feature is unsupported                                  *
 */
int
soc_th_latency_bypassed(
    int             unit,
    soc_feature_t   feature)
{
    int latency;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_RT_ENABLE_CHK(unit);

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

    switch(feature) {
        /*
         * features supported only in NORMAL latency mode and NOT supported in either
         * LOW (L2) or BALANCED (L2/L3) Latency modes
         */
        case soc_feature_alpm:
        case soc_feature_ptp:
        case soc_feature_visibility:
        case soc_feature_robust_hash:
        case soc_feature_tunnel_6to4_secure:
        case soc_feature_tunnel_any_in_6:
        case soc_feature_tunnel_gre:
        case soc_feature_tunnel_protocol_match:
        case soc_feature_l2gre:
        case soc_feature_l2gre_default_tunnel:
        case soc_feature_l2_tunnel_transit_payload_tpid_parser:
        case soc_feature_mac_based_vlan:
        case soc_feature_ip_subnet_based_vlan:
        case soc_feature_vfi_zero_invalid:
        case soc_feature_hierarchical_ecmp:
        case soc_feature_l3_ecmp_2k_groups:
        case soc_feature_mpls_lsr_ecmp:
        case soc_feature_sflow_counters:
        case soc_feature_sflow_flex:
        case soc_feature_sflow_ing_mem:
        case soc_feature_sflow_ing_mirror:
        case soc_feature_fp_nw_tcam_prio_order_war:
        case soc_feature_higig2:
        case soc_feature_hg_no_speed_change:
        case soc_feature_xport_convertible:
        /* Please check _bcm_esw_get_fp_mode_global before
         * changing soc_feature_field_multi_stage.
         */
        case soc_feature_field_multi_stage:
            return (SOC_SWITCH_BYPASS_MODE_NONE == latency) ? 0 : \
                    (SOC_SWITCH_BYPASS_MODE_EFP == latency) ? 0 : 1;

        case soc_feature_alpm_flex_stat:
        case soc_feature_alpm_flex_stat_v6_64:
             if (soc_property_get(unit, spn_ALPM_FLEX_STAT_SUPPORT, 0)) {
                 return (SOC_SWITCH_BYPASS_MODE_NONE == latency) ? 0 : \
                         (SOC_SWITCH_BYPASS_MODE_EFP == latency) ? 0 : 1;
             } else {
                  return TRUE;
             }

        case soc_feature_field_compression:
             if ((soc_property_get(unit, "fp_compression_enable", 1)) != 0) {
                 return (SOC_SWITCH_BYPASS_MODE_NONE == latency) ? 0 : \
                         (SOC_SWITCH_BYPASS_MODE_EFP == latency) ? 0 : 1;
             } else {
                  return TRUE;
             }

        case soc_feature_field_exact_match_support:
        case soc_feature_uc_flowtracker_learn:
        case soc_feature_uc_flowtracker_export:
             if ((0 != soc_property_get(unit, "fpem_mem_entries", 0))
                  && ((SOC_MEM_IS_VALID(unit, EXACT_MATCH_2m)
                       && (soc_mem_index_max(unit, EXACT_MATCH_2m) > 0))
                       || (SOC_MEM_IS_VALID(unit, EXACT_MATCH_4m)
                           && (soc_mem_index_max(unit, EXACT_MATCH_4m) > 0)))) {
                 return (SOC_SWITCH_BYPASS_MODE_NONE == latency) ? 0 : \
                         (SOC_SWITCH_BYPASS_MODE_EFP == latency) ? 0 : 1;
             } else {
                  return TRUE;
             }

        /*
         * features supported in NORMAL and BALANCED Latency modes but
         * NOT in LOW (L2) Latency
         */
        case soc_feature_agm:
        case soc_feature_vlan_pri_cfi_action:
        case soc_feature_color_prio_map:
        case soc_feature_bfd:
        case soc_feature_big_icmpv6_ping_check:
        case soc_feature_class_based_learning:
        case soc_feature_ecmp_random:
        case soc_feature_exact_width_match_mode:
        case soc_feature_ecmp_resilient_hash_optimized:
        case soc_feature_ecmp_round_robin:
        case soc_feature_egr_vlan_check:
        case soc_feature_field_ingress_cosq_override:
        case soc_feature_igmp_mld_support:
        case soc_feature_ip_mcast:
        case soc_feature_ip_mcast_repl:
        case soc_feature_ipmc_remap:
        case soc_feature_ipmc_repl_penultimate:
        case soc_feature_ipmc_use_configured_dest_mac:
        case soc_feature_l3:
        case soc_feature_l3_defip_map:
        case soc_feature_l3_shared_defip_table:
        case soc_feature_l3_dynamic_ecmp_group:
        case soc_feature_l3_ecmp_1k_groups:
        case soc_feature_l3_extended_host_entry:
        case soc_feature_l3_host_ecmp_group:
        case soc_feature_l3_iif_profile:
        case soc_feature_l3_iif_zero_invalid:
        case soc_feature_l3_ingress_interface:
        case soc_feature_l3_ip4_options_profile:
        case soc_feature_l3_ip6:
        case soc_feature_l3_lookup_cmd:
        case soc_feature_l3_sgv:
        case soc_feature_l3mtu_fail_tocpu:
        case soc_feature_l3x_parity:
        case soc_feature_lpm_tcam:
        case soc_feature_lpm_prefix_length_max_128:
        case soc_feature_nat:
        case soc_feature_pim_bidir:
        case soc_feature_randomized_load_balance:
        case soc_feature_repl_l3_intf_use_next_hop:
        case soc_feature_round_robin_load_balance:
        case soc_feature_split_repl_group_table:
        case soc_feature_storm_control:
        case soc_feature_udf_hashing:
        case soc_feature_udf_multi_pipe_support:
        case soc_feature_udf_support:
        case soc_feature_shared_trunk_member_table:
        case soc_feature_field:
        case soc_feature_field_wide:
        case soc_feature_field_intraslice_double_wide:
        case soc_feature_field_preselector_support:
        case soc_feature_field_ingress_triple_wide:
        case soc_feature_field_multi_pipe_support:
        case soc_feature_memscan_ifp_slice_adjust:
        case soc_feature_field_action_l2_change:
        case soc_feature_field_action_redirect_ipmc:
        case soc_feature_field_virtual_slice_group:
        case soc_feature_overlaid_address_class:
        case soc_feature_field_action_redirect_ecmp:
        case soc_feature_src_port_consistency_check_skip:
        case soc_feature_vlan_protocol_pkt_ctrl:
        case soc_feature_th_nh_no_overlap_ecmp:
        case soc_feature_vlan_egress_membership_l3_only:
            return (SOC_SWITCH_BYPASS_MODE_LOW == latency);

        /*
         * features supported ONLY in LOW (L2) and BALANCED (L2/L3) Latency
         * mode but NOT in NORMAL Latency
         */
        case soc_feature_no_tunnel:
        case soc_feature_fast_init:
            return (SOC_SWITCH_BYPASS_MODE_NONE == latency) || (SOC_SWITCH_BYPASS_MODE_EFP == latency);

        /*
         * features supported ONLY in LOW (L2) Latency
         */
        case soc_feature_fastlag:
        case soc_feature_fast_egr_vlan_action:
            return (SOC_SWITCH_BYPASS_MODE_LOW != latency);

        /*
         * features supported only in BALANCED (L2/L3) Latency
         */
        case soc_feature_egr_vlan_action:
        case soc_feature_fast_ecmp:
            return (SOC_SWITCH_BYPASS_MODE_BALANCED != latency);

        /*
         * legacy features supported in ALL THREE Latency modes
         */
        case soc_feature_advanced_flex_counter:
        case soc_feature_arl_hashed:
        case soc_feature_arl_insert_ovr:
        case soc_feature_basic_dos_ctrl:
        case soc_feature_bigmac_fault_stat:
        case soc_feature_counter_parity:
        case soc_feature_cpuport_mirror:
        case soc_feature_cpuport_stat_dma:
        case soc_feature_cpuport_switched:
        case soc_feature_dcb_reason_hi:
        case soc_feature_dodeca_serdes:
        case soc_feature_dual_hash:
        case soc_feature_eee:
        case soc_feature_ets:
        case soc_feature_extended_pci_error:
        case soc_feature_fe_maxframe:
        case soc_feature_port_flow_hash:
        case soc_feature_fifo_dma:
        case soc_feature_fifo_dma_active:
        case soc_feature_flex_port:
        case soc_feature_flexible_dma_steps:
        case soc_feature_force_forward:
        case soc_feature_generic_counters:
        case soc_feature_generic_table_ops:
        case soc_feature_gmii_clkout:
        case soc_feature_hw_stats_calc:
        case soc_feature_l2_bulk_control:
        case soc_feature_vlan_queue_map:
        case soc_feature_vlan_mc_flood_ctrl:
        case soc_feature_aging_extended:
        case soc_feature_l2_hashed:
        case soc_feature_l2_hw_aging_bug:
        case soc_feature_l2_lookup_cmd:
        case soc_feature_l2_lookup_retry:
        case soc_feature_l2_modfifo:
        case soc_feature_l2_overflow:
        case soc_feature_l2_pending:
        case soc_feature_l2_user_table:
        case soc_feature_l2x_parity:
        case soc_feature_led_data_offset_a0:
        case soc_feature_led_proc:
        case soc_feature_logical_port_num:
        case soc_feature_lport_tab_profile:
        case soc_feature_mbist:
        case soc_feature_mdio_enhanced:
        case soc_feature_mem_cache:
        case soc_feature_mem_push_pop:
        case soc_feature_min_cell_per_queue:
        case soc_feature_min_resume_limit_1:
        case soc_feature_mirror_control_mem:
        case soc_feature_mirror_cos:
        case soc_feature_mirror_encap_profile:
        case soc_feature_mirror_flexible:
        case soc_feature_mmu_config_property:
        case soc_feature_modmap:
        case soc_feature_modport_map_dest_is_port_or_trunk:
        case soc_feature_module_loopback:
#ifdef INCLUDE_MSTP
        case soc_feature_mstp:
#endif
        case soc_feature_new_sbus_format:
        case soc_feature_phy_cl45:
        case soc_feature_post:
        case soc_feature_qos_profile:
        case soc_feature_regs_as_mem:
        case soc_feature_remap_ut_prio:
        case soc_feature_remote_learn_trust:
        case soc_feature_rx_timestamp:
        case soc_feature_rxdma_cleanup:
        case soc_feature_sbusdma:
        case soc_feature_schan_hw_timeout:
        case soc_feature_schmsg_alias:
        case soc_feature_ser_fifo:
        case soc_feature_ser_parity:
        case soc_feature_sgmii_autoneg:
        case soc_feature_src_modid_base_index:
        case soc_feature_stat_dma:
        case soc_feature_stat_jumbo_adj:
        case soc_feature_stat_xgs3:
        case soc_feature_stg:
        case soc_feature_stg_xgs:
        case soc_feature_table_dma:
        case soc_feature_table_hi:
        case soc_feature_time_support:
        case soc_feature_timesync_support:
        case soc_feature_trunk_group_overlay:
        case soc_feature_trunk_group_size:
        case soc_feature_two_ingress_pipes:
        case soc_feature_unified_port:
        case soc_feature_vlan_ctrl:
        case soc_feature_wesp:
        case soc_feature_xgxs_power:
        case soc_feature_xgxs_v7:
        case soc_feature_xlmac:
        case soc_feature_xy_tcam:
        case soc_feature_xy_tcam_direct:
        case soc_feature_shared_hash_mem:
        case soc_feature_src_modid_blk:
        case soc_feature_src_modid_blk_profile:
        case soc_feature_egr_ipmc_mem_field_l3_payload_valid:
        case soc_feature_tslam_dma:
            return FALSE;

        /*
         * legacy features supported ONLY in NORMAL Latency mode
         */
        default:
            return (((SOC_SWITCH_BYPASS_MODE_NONE == latency) || (SOC_SWITCH_BYPASS_MODE_EFP == latency))
            ? (!soc_features_bcm56850_a0(unit, feature)) : TRUE);
    }

}

/******************************************************************************
 * Name: soc_th2_latency_bypassed                                              *
 * Description:                                                               *
 *     Returns if the specified feature is supported/available for the        *
 *     current Latency mode configured on the switch                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Feature (soc_feature_*)                                              *
 * Returns:                                                                   *
 *     - FALSE, if the feature is supported                                   *
 *     - TRUE, if the feature is unsupported                                  *
 */
int
soc_th2_latency_bypassed(
    int             unit,
    soc_feature_t   feature)
{
    int latency;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_RT_ENABLE_CHK(unit);

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));

    switch(feature) {
        /*
         * features supported only in NORMAL latency mode and NOT supported in either
         * LOW (L2) or BALANCED (L2/L3) Latency modes
         */
        case soc_feature_ecmp_dlb:
        case soc_feature_ecmp_dlb_optimized:
        case soc_feature_dgm:
        case soc_feature_ecn_wred:
        case soc_feature_mpls_ecn:
        case soc_feature_flex_hashing:
        case soc_feature_mpls_frr_lookup:    
            return (SOC_SWITCH_BYPASS_MODE_NONE != latency);

        /*
         * features supported in NORMAL and BALANCED Latency modes but
         * NOT in LOW (L2) Latency
         */
        case soc_feature_lag_dlb:
        case soc_feature_hg_dlb:
        case soc_feature_hgt_lag_dlb_optimized:
            return (SOC_SWITCH_BYPASS_MODE_LOW == latency);

        /*
         * features supported ONLY in LOW (L2) and BALANCED (L2/L3) Latency
         * mode but NOT in NORMAL Latency
         */

        /*
         * features supported ONLY in LOW (L2) Latency
         */

        /*
         * features supported only in BALANCED (L2/L3) Latency
         */

        /*
         * legacy features supported in ALL THREE Latency modes
         */

        /*
         * legacy features untouched
         */
        default:
            return FALSE;
    }

}

/******************************************************************************
 * Name: soc_th_latency_mem_avail                                             *
 * Description:                                                               *
 *     Returns if the specified memory / table is available for the           *
 *     current Latency mode configured on the switch                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Memory/Table                                                         *
 * Returns:                                                                   *
 *     - TRUE, if the memory is available                                     *
 *     - FALSE, if the memory is unavailable                                  *
 */
int
soc_th_latency_mem_avail(
    int         unit,
    soc_mem_t   mem,
    int         latency)
{
    return _soc_th_latency_resource_avail(unit, _SOC_TH_MEM, mem, latency);
}

/******************************************************************************
 * Name: soc_th_latency_reg_avail                                             *
 * Description:                                                               *
 *     Returns if the specified register is available for the                 *
 *     current Latency mode configured on the switch                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Register                                                             *
 * Returns:                                                                   *
 *     - TRUE, if the register is available                                   *
 *     - FALSE, if the register is unavailable                                *
 */
int
soc_th_latency_reg_avail(
    int         unit,
    soc_reg_t   reg,
    int         latency)
{
    return _soc_th_latency_resource_avail(unit, _SOC_TH_REG, reg, latency);
}

/******************************************************************************
 * Name: soc_th_latency_init                                                  *
 * Description:                                                               *
 *     Configure the Switch and the SDK for the Latency Bypass mode           *
 *     specified in config.bcm                                                *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 */
int
soc_th_latency_init(int unit)
{
    int latency;
    uint32 rval = 0;
    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_RT_ENABLE_CHK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* WB recovery happens in _bcm_esw_port_wb_recover context */
        return BCM_E_NONE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (!(_soc_th_latency_ctrl[unit] =
          sal_alloc(sizeof(_soc_th_latency_ctrl_t), "TH Latency Ctrl Area"))) {
        return SOC_E_MEMORY;
    }

    latency = soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                               SOC_SWITCH_BYPASS_MODE_NONE);
    _SOC_TH_LATENCY_CFG_VALIDATE(latency, SOC_E_NONE);

    /* Set MMU in reset */
    if ((latency == SOC_SWITCH_BYPASS_MODE_LOW) ||
        (latency == SOC_SWITCH_BYPASS_MODE_BALANCED)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d Latency mode update. Mode %d. "
                                "Initiate MMU reset\n"),
                                unit, latency));
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    }
    if (SOC_SWITCH_BYPASS_MODE_EFP == latency) {
        /* config EFP latency settings */
        SOC_IF_ERROR_RETURN(_soc_th_efp_latency_config(unit, latency));
    } else { 
        /* config ingress latency settings */
        SOC_IF_ERROR_RETURN(_soc_th_igr_latency_config(unit, latency));
        /* config egress latency settings */
        SOC_IF_ERROR_RETURN(_soc_th_egr_latency_config(unit, latency));
    }

    _soc_th_latency_ctrl[unit]->init = 1;
    _soc_th_latency_ctrl[unit]->latency = latency;
    SOC_SWITCH_BYPASS_MODE(unit) = latency;

    /* apply reg filter for the current latency config */
    SOC_IF_ERROR_RETURN(_soc_th_latency_reg_filter(unit));
    /* apply memory filter for the current latency config */
    SOC_IF_ERROR_RETURN(_soc_th_latency_mem_filter(unit));

    /* Bring MMU out of reset */
    if ((latency == SOC_SWITCH_BYPASS_MODE_LOW) ||
        (latency == SOC_SWITCH_BYPASS_MODE_BALANCED)) {
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d Latency update done. MMU out of reset.\n"),
                                unit));
    }

    sal_memcpy(&_soc_th_latency_ctrl[unit]->tbl_cfg, &soc_th_tbl_cfg[unit],
               sizeof(soc_th_latency_table_cfg_t));

    LOG_DEBUG(BSL_LS_SOC_INIT,
            (BSL_META_U(unit, "*** unit %d: configured for %s latency\n"),
             unit, lstr[latency]));

    return SOC_E_NONE;
}

int
soc_th_latency_deinit(int unit)
{
    if (_soc_th_latency_ctrl[unit]) {
        sal_free(_soc_th_latency_ctrl[unit]);
        _soc_th_latency_ctrl[unit] = NULL;
    }

    return SOC_E_NONE;
}


/***********************************
 * Latency Bypass Warmboot Support *
 */
#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2                SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_8

/* reconfigure latency sw states during wb recovery      */
/* level 0 - reinitializes latency wb control structure  */
/* level 1 - reinitializes reg and memory filters only   */
int
soc_th_latency_wb_reinit(int unit, int level)
{
    int latency_wb, latency_cfg;

    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_INTERNAL;
    }

    /* config.bcm's version of latency */
    latency_cfg = soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                                   SOC_SWITCH_BYPASS_MODE_NONE);

    if (0 == level) {
        _SOC_TH_UNIT_VALIDATE(unit);
        _SOC_TH_LATENCY_RT_ENABLE_CHK(unit);

        if (!_soc_th_latency_ctrl[unit]) {
            return SOC_E_INTERNAL;
        }

        /* rashomon */
        if (1 == _soc_th_latency_ctrl[unit]->init) {
            /* wb's version of latency */
            latency_wb = _soc_th_latency_ctrl[unit]->latency;
        } else {
            return SOC_E_INTERNAL;
        }

        /* ratify the sanity of wb recovery */
        if (latency_cfg == latency_wb) {
            _soc_th_latency_ctrl[unit]->latency = latency_cfg;
        } else {
            return SOC_E_INTERNAL;
        }

        /* reconfiguration section */
        SOC_SWITCH_BYPASS_MODE(unit) = _soc_th_latency_ctrl[unit]->latency;
    }    else {
        if (!latency_cfg) {
            return SOC_E_NONE;
        }
        /* reapply reg filter */
        SOC_IF_ERROR_RETURN(_soc_th_latency_reg_filter(unit));
        /* reapply memory filter */
        SOC_IF_ERROR_RETURN(_soc_th_latency_mem_filter(unit));
    }

    return SOC_E_NONE; /* back to normalcy */
}

int
soc_th_latency_wb_memsz_get(
    int               unit,
    OUT uint32* const mem_sz,
    uint16            scache_ver)
{
    _SOC_TH_UNIT_VALIDATE(unit);
    if (!mem_sz) {
        return SOC_E_PARAM;
    }
    *mem_sz = 0;

    if (!SOC_WARM_BOOT(unit)) {
        if (!_soc_th_latency_ctrl[unit] || !_soc_th_latency_ctrl[unit]->init) {
            return SOC_E_NONE;
        }
    }

    *mem_sz = sizeof(_soc_th_latency_wb_t);

    /* adjust for lower versions */
    if (scache_ver < BCM_WB_VERSION_1_8) {
        *mem_sz -= sizeof(soc_th_latency_table_cfg_t);
    }

    return SOC_E_NONE;
}

int
soc_th_latency_wb_sync(
    int                 unit,
    IN_OUT uint8* const wb_data)
{
    _soc_th_latency_wb_t *wbd;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_latency_ctrl[unit] || !_soc_th_latency_ctrl[unit]->init) {
        return SOC_E_NONE;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_latency_wb_t *) wb_data;
    wbd->unit = unit;
    sal_memcpy(&wbd->latency_ctrl, _soc_th_latency_ctrl[unit],
               sizeof(_soc_th_latency_ctrl_t));

    return SOC_E_NONE;
}

int
soc_th_latency_wb_recover(
    int          unit,
    uint8* const wb_data,
    uint16       scache_ver)
{
    _soc_th_latency_wb_t *wbd;
    int adj = 0; /*scache ver adjustment */

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!SOC_WARM_BOOT(unit)) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_latency_wb_t *) wb_data;
    if (wbd->unit == unit) {
        if (!(_soc_th_latency_ctrl[unit] =
                   sal_alloc(sizeof(_soc_th_latency_ctrl_t),
                             "TH Latency Bypass Ctrl Area"))) {
            return SOC_E_MEMORY;
        }

        /* adjust for lower versions */
        if (scache_ver < BCM_WB_VERSION_1_8) {
            adj = sizeof(soc_th_latency_table_cfg_t);
            /* backfill tbl_cfg from TH driver's global bookkeeping */
            sal_memcpy(&_soc_th_latency_ctrl[unit]->tbl_cfg, &soc_th_tbl_cfg[unit],
                       sizeof(soc_th_latency_table_cfg_t));
        }

        sal_memcpy(_soc_th_latency_ctrl[unit], &wbd->latency_ctrl,
                   (sizeof(_soc_th_latency_ctrl_t) - adj));
    }

    SOC_IF_ERROR_RETURN(soc_th_latency_wb_reinit(unit, 0));

    LOG_CLI((BSL_META_U(unit, "*** unit %d: Latency subsystem warmbooted: "
                        "configured for %s latency\n"), unit,
                        lstr[_soc_th_latency_ctrl[unit]->latency]));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/********************************
 * Latency Bypass Debug Support *
 */
int
soc_th_latency_show(int unit)
{
    int mode;
    char *mode_str[3] = {
        "Normal latency (full features)",
        "Balanced (L2/L3) Latency",
        "Low (L2) Latency"
    };

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_INIT_VALIDATE(unit, SOC_E_INTERNAL);

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &mode));
    LOG_CLI(("switch latency mode : %s\n", mode_str[mode]));

    if (soc_feature(unit, soc_feature_fastlag)) {
        LOG_CLI(("Trunking mode : Fast LAG\n"));
    }
    if (soc_feature(unit, soc_feature_fast_ecmp)) {
        LOG_CLI(("ECMP mode     : Fast ECMP\n"));
    }

    return SOC_E_NONE;
}

int
soc_th_latency_diag(int unit)
{
    int mode;
    char *mode_str[3] = {
        "Normal latency (full features)",
        "Balanced (L2/L3) Latency",
        "Low (L2) Latency"
    };
    uint32 rval;
    uint8 ipars, ivxlt, ivp, ifwd, irsel1, ifp, irsel2, isw2;
    uint8 ecmp;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_LATENCY_INIT_VALIDATE(unit, SOC_E_INTERNAL);

    SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &mode));

    SOC_IF_ERROR_RETURN(READ_ING_LATENCY_CONTROLr(unit, &rval));
    ipars  = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IPARS_LATENCY_MODEf);
    ivxlt  = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IVXLT_BYPASSf);
    ivp    = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IVP_BYPASSf);
    ifwd   = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IFWD_LATENCY_MODEf);
    irsel1 = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IRSEL1_LATENCY_MODEf);
    ifp    = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IFP_BYPASSf);
    irsel2 = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               IRSEL2_LATENCY_MODEf);
    isw2   = soc_reg_field_get(unit, ING_LATENCY_CONTROLr, rval,
                               ISW2_LATENCY_MODEf);

    SOC_IF_ERROR_RETURN(READ_ECMP_CONFIGr(unit, &rval));
    ecmp  = soc_reg_field_get(unit, ECMP_CONFIGr, rval, ECMP_MODEf);

    LOG_CLI(("switch latency mode : %s\n", mode_str[mode]));

    if (soc_feature(unit, soc_feature_fastlag)) {
        LOG_CLI(("Trunking mode : Fast LAG\n"));
    }
    if (soc_feature(unit, soc_feature_fast_ecmp)) {
        LOG_CLI(("ECMP mode     : Fast ECMP\n"));
    }

    LOG_CLI(("Ingress Latency Configurations:\n"));
    LOG_CLI(("ipars  ivxlt  ivp  ifwd  irsel1  ifp  irsel2  isw2  ecmp\n"));
    LOG_CLI(("%5d  %5d  %3d  %4d  %6d  %3d  %6d  %4d  %4d\n",
             ipars, ivxlt, ivp, ifwd, irsel1, ifp, irsel2, isw2, ecmp));

    LOG_CLI(("Egress Latency Configurations:\n"));
    LOG_CLI(("el3  evlan  efp\n"));
    /*
     * due to an RTL issue which left EGR_LATENCY_MODE as NOTEST, we don't
     * have any option but to produce the sw state in lieu of the register values
     */
    LOG_CLI(("%3d  %5d  %3d\n", _soc_th_latency_cfg_tbl[mode].egr.el3,
             _soc_th_latency_cfg_tbl[mode].egr.evlan,
             _soc_th_latency_cfg_tbl[mode].egr.efp));

    /* table configurations */
    LOG_CLI(("l2_entries:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.l2_entries));
    LOG_CLI(("fixed_l2_entries:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.fixed_l2_entries));
    LOG_CLI(("shared_l2_banks:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.shared_l2_banks));
    LOG_CLI(("l3_entries:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.l3_entries));
    LOG_CLI(("fixed_l3_entries:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.fixed_l3_entries));
    LOG_CLI(("shared_l3_banks:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.shared_l3_banks));
    LOG_CLI(("fpem_entries:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.fpem_entries));
    LOG_CLI(("shared_fpem_banks:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.shared_fpem_banks));
    LOG_CLI(("alpm_enable:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.alpm_enable));
    LOG_CLI(("max_tcams:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.max_tcams));
    LOG_CLI(("tcam_depth:%d\n",
             _soc_th_latency_ctrl[unit]->tbl_cfg.tcam_depth));

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

