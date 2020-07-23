/*

 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        trident2p.c
 * Purpose:
 * Requires:
 */
#include <shared/bsl.h>
#include <sal/core/boot.h>

#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/mspi.h>
#include <soc/l2x.h>
#include <soc/soc_ser_log.h>
#include <soc/devids.h>


#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <soc/trident2.h>
#include <soc/td2_td2p.h>
#include <soc/tdm/core/tdm_top.h>
#include <soc/esw/portctrl.h>
#include <soc/scache.h>

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))


#define BCM_TD2P_MAX_BANK_SIZE      128
#define BCM_TD2P_SMALL_BANK_SIZE    64


#define _TD2P_PROB_DROP_SOP_XOFF          80
#define _TD2P_PROB_DROP_SOP_XON_ABOVE_THR 65
#define _TD2P_PROB_DROP_SOP_XON_BELOW_THR 0


#define _SOC_TRIDENT2P_UPDATE_TDM_PRE  (0)
#define _SOC_TRIDENT2P_UPDATE_TDM_POST (1)

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r
};

STATIC soc_reg_t mmu_ovs_group_wt_regs_td2p_local[2][_MMU_OVS_WT_GROUP_COUNT] = {
    {
        ES_PIPE0_OVR_SUB_GRP_WT0r, ES_PIPE0_OVR_SUB_GRP_WT1r,
        ES_PIPE0_OVR_SUB_GRP_WT2r, ES_PIPE0_OVR_SUB_GRP_WT3r
    },
    {
        ES_PIPE1_OVR_SUB_GRP_WT0r, ES_PIPE1_OVR_SUB_GRP_WT1r,
        ES_PIPE1_OVR_SUB_GRP_WT2r, ES_PIPE1_OVR_SUB_GRP_WT3r
    }
};

STATIC soc_reg_t mmu_3dbg_c_regs[2] = {
    ES_PIPE0_MMU_3DBG_Cr,
    ES_PIPE1_MMU_3DBG_Cr
};

STATIC const soc_field_t mmu_ovr_group_wt_select_fields[] = {
    GRP0f, GRP1f, GRP2f, GRP3f,
    GRP4f, GRP5f, GRP6f, GRP7f
};

typedef struct _soc_td2p_ovr_sub_mmu_tdm_s {
    int grp_wt_sel[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT];
    int tdm_update[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
    int grp_wt[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
    int mmu_port[TD2P_PIPES_PER_DEV][MAX_MMU_OVS_GROUP_COUNT]
                                    [_MMU_OVS_GROUP_TDM_LENGTH];
} _soc_td2p_ovr_sub_mmu_tdm_t;

typedef struct _soc_td2p_ovr_sub_tdm_s {
    _soc_td2p_ovr_sub_mmu_tdm_t mmu_tdm;
} _soc_td2p_ovr_sub_tdm_t;

enum _td2p_ovs_wt_speed_group {
    td2p_ovs_wt_group_speed_10G,
    td2p_ovs_wt_group_speed_20G,
    td2p_ovs_wt_group_speed_40G,
    td2p_ovs_wt_group_speed_NA
};

static _soc_td2p_flexport_port_q_cb td2p_flexport_cb;
static _soc_td2p_ets_mode_q_cb      td2p_ets_mode_cb;


#define NUM_TSC4_PORT_MACRO 32
static int 
soc_td2p_pm_to_1st_port_tsc12[NUM_TSC4_PORT_MACRO] = 
                                        {1 , 1 , 1 , 13, /* X Pipe */
                                         17, 21, 21, 21,
                                         33, 33, 33, 45,
                                         49, 53, 53, 53, 
                                         65, 65, 65, 77, /* Y Pipe */
                                         81, 85, 85, 85, 
                                         97, 97, 97, 109, 
                                         113,117, 117, 117};

static int 
soc_td2p_pm_to_1st_port_tsc4[NUM_TSC4_PORT_MACRO] = 
                                        {1 , 5 , 9 , 13, /* X Pipe */
                                         17, 21, 25, 29, 
                                         33, 37, 41, 45, 
                                         49, 53, 57, 61, 
                                         65, 69, 73, 77, /* Y Pipe */
                                         81, 85, 89, 93, 
                                         97, 101, 105, 109, 
                                         113,117, 121, 125};

/* index 0 is for X pipe, index 1 is for Y pipe */
static int num_hsp_to_reserve[_TD2_PIPES_PER_DEV] = {0, 0};     

/* defines for Reserved Buffer for Flex Port Macro when new Flex Port is
   enabled */
/* lossless defines */
#define _TD2P_RSVD_BUF_FLEX_HDRM_LOSSLESS 182

#define _TD2P_RSVD_BUF_FLEX_QGRP 16
#define _TD2P_RSVD_BUF_FLEX_ASF 4

/* defines for Reserved Buffer for Ports when Flex Port for the macro is NOT
   enabled*/
#define _TD2P_RSVD_BUF_NON_FLEX_100G_HDRM_LOSSLESS 558
#define _TD2P_RSVD_BUF_NON_FLEX_40G_HDRM_LOSSLESS 461
#define _TD2P_RSVD_BUF_NON_FLEX_20G_HDRM_LOSSLESS 262
#define _TD2P_RSVD_BUF_NON_FLEX_10G_HDRM_LOSSLESS 182

#define _TD2P_RSVD_BUF_NON_FLEX_PGMIN 8
#define _TD2P_RSVD_BUF_NON_FLEX_QGRP 16
#define _TD2P_RSVD_BUF_NON_FLEX_ASF 16

#define _TD2P_RSVD_BUF_NON_FLEX_10G_ASF 4

#define _TD2P_NUM_TSC4_PER_PIPE 16
#define _TD2P_NUM_PORTS_PER_TSC4 4

int get_mmu_mode (int unit)
{
    int is16Mmode = 0;
    char *str, *str_end;
    int mmu_size = 0;

    if (SAL_BOOT_QUICKTURN) {
        if ((str = soc_property_get_str(unit, "mmu_mem_size")) != NULL) {
            mmu_size = sal_ctoi(str, &str_end);
            if (str != str_end) {
                if ( mmu_size == 16 ) {
                    is16Mmode = 1;
                }
            } 
        }
    } else {
        is16Mmode = 1;
    }

    return is16Mmode;
}

mmu_ovs_group_wt_regs_t 
get_mmu_ovs_group_wt_regs (int unit, mmu_ovs_group_wt_regs_t old)
{
    if (SOC_IS_TD2P_TT2P(unit)) {
        return mmu_ovs_group_wt_regs_td2p_local;
    }
    return old;
}

int soc_td2p_if_full_chip (int unit)
{
    int full_chip = 1;

    return full_chip;
}

int soc_td2p_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, avg;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval)); 
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 4);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_sleep(1);
    avg = 0;

    /* Read Voltages. Ignores result of PVTMON8 */
    for (index = 0; index < COUNTOF(pvtmon_result_reg) - 1; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        /* (PVT_DATA * 880) / 1024 * 0.7 */
        fval = (fval * 880 * 10) / (1024 * 7);
        avg += fval;
        LOG_CLI((BSL_META_U(unit,
                            "Voltage monitor %d: voltage = %d.%03dV\n"),
                 index, (fval/1000), (fval %1000)));
    }

    avg /= (COUNTOF(pvtmon_result_reg) - 1);
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}

int soc_td2p_set_obm_registers (int unit, soc_reg_t reg, int speed, int index,
                int obm_inst,  int lossless , int default_flag)
{
    int xon, xoff;
    uint64 ctrl_rval64;
    int maxt, mint = 0;
    int lowprit;
    int div;

    if (PGW_OBM_INIT_HW_DEFAULT == default_flag) {
        maxt = 0;
        xoff = 0;
        xon  = 0;
        lowprit = 0;
    } else {
        if ( speed > 20000) {
            div = 1;
            xoff = 0x2C3;
            xon  = 0x2AB;
            lowprit = 0x2AE;
        } else if (speed > 10000) {
            div = 2;
            xoff = 0x13B;
            xon  = 0x12F;
            lowprit = 0x132;
        } else {
            div = 4;
            xoff = 0x4F;
            xon  = 0x49;
            lowprit = 0x4c;
        }
        maxt = GET_NUM_CELLS_PER_OBM(unit) / div;

        if ( !lossless ) {
            xon = xoff = 0x7FF;
            lowprit = maxt;
        }
    }

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          MIN_THRESHOLDf, mint);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          LOW_PRI_THRESHOLDf, lowprit);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          MAX_THRESHOLDf, maxt);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          FLOW_CTRL_XONf, xon);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          FLOW_CTRL_XOFFf, xoff);

    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));


    return SOC_E_NONE;
}

/* Probablistic Drop configuration for TD2PLUS:
 * These settings cannot be changed dynamically, According to Architecture team
 * these settings are optimized for all the OverSubscription ratios. Hence not
 * giving any configuration options for these fields. 
 * OverSub feature is enabled by default for TD2PLUS.
 */
int soc_td2p_obm_prob_drop_default_set (int unit, int xlp, int index,
                                         int obm_inst, int default_flag)
{
    static const soc_reg_t pgw_obm_prob_drop_regs[] = {
                PGW_OBM0_LOW_PRI_DROP_PROBr, PGW_OBM1_LOW_PRI_DROP_PROBr,
                PGW_OBM2_LOW_PRI_DROP_PROBr, PGW_OBM3_LOW_PRI_DROP_PROBr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;

    reg = pgw_obm_prob_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    if (PGW_OBM_INIT_HW_DEFAULT == default_flag) {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, EN_DROP_PROBf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XOFFf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_ABOVE_THRf, 0);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_BELOW_THRf, 0);
    } else {
        /* By default enable Probabilistic dropping */
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, EN_DROP_PROBf, 1);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XOFFf,
                              _TD2P_PROB_DROP_SOP_XOFF);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_ABOVE_THRf,
                              _TD2P_PROB_DROP_SOP_XON_ABOVE_THR);
        soc_reg64_field32_set(unit, reg, &ctrl_rval64, SOP_XON_BELOW_THRf,
                              _TD2P_PROB_DROP_SOP_XON_BELOW_THR);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

/* Cut through is allowed if usage is less than or equal to
 * CUT_THROUGH_OKf threshold.  Granularity is 16B cell.
 */
int soc_td2p_obm_cut_through_threshold_set (int unit, int xlp, int index,
                                            int obm_inst, int default_flag)
{
    static const soc_reg_t pgw_obm_threshold2_regs[] = {
        PGW_OBM0_THRESHOLD2r, PGW_OBM1_THRESHOLD2r,
        PGW_OBM2_THRESHOLD2r, PGW_OBM3_THRESHOLD2r
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;

    reg = pgw_obm_threshold2_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    if ( PGW_OBM_INIT_HW_DEFAULT == default_flag ) {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                              CUT_THROUGH_OKf, 0);
    } else {
        soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                              CUT_THROUGH_OKf, 2);
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));
    return SOC_E_NONE;
}

STATIC int
soc_td2p_pgw_mib_reset_reg_default_set(int unit, int xlp, int port_index,
                                       int obm_inst)
{
    const soc_reg_t reg = PGW_MIB_RESETr;
    const soc_field_t field = CLR_CNTf;
    int port_index_pgw;
    uint64 ctrl_rval64;
    uint32 reg_val;

    port_index_pgw = xlp * 4 + port_index;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, port_index,
                                    &ctrl_rval64));
    reg_val = soc_reg64_field32_get(unit, reg, ctrl_rval64, field);
    /* coverity[ptr_arith] */
    SHR_BITSET(&reg_val, port_index_pgw);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          field, reg_val);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, port_index, ctrl_rval64));

    /* coverity[ptr_arith] */
    SHR_BITCLR(&reg_val, port_index_pgw);
    soc_reg64_field32_set(unit, reg, &ctrl_rval64,
                          field, reg_val);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, port_index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_high_pri_byte_drop_default_set (int unit, int xlp, int index,
                                             int obm_inst)
{
    static const soc_reg_t pgw_obm_high_pri_byte_drop_regs[] = {
                PGW_OBM0_HIGH_PRI_BYTE_DROPr, PGW_OBM1_HIGH_PRI_BYTE_DROPr,
                PGW_OBM2_HIGH_PRI_BYTE_DROPr, PGW_OBM3_HIGH_PRI_BYTE_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_high_pri_byte_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_high_pri_pkt_drop_default_set (int unit, int xlp, int index,
                                            int obm_inst)
{
    static const soc_reg_t pgw_obm_high_pri_pkt_drop_regs[] = {
                PGW_OBM0_HIGH_PRI_PKT_DROPr, PGW_OBM1_HIGH_PRI_PKT_DROPr,
                PGW_OBM2_HIGH_PRI_PKT_DROPr, PGW_OBM3_HIGH_PRI_PKT_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_high_pri_pkt_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_low_pri_byte_drop_default_set (int unit, int xlp, int index,
                                            int obm_inst)
{
    static const soc_reg_t pgw_obm_low_pri_byte_drop_regs[] = {
                PGW_OBM0_LOW_PRI_BYTE_DROPr, PGW_OBM1_LOW_PRI_BYTE_DROPr,
                PGW_OBM2_LOW_PRI_BYTE_DROPr, PGW_OBM3_LOW_PRI_BYTE_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_low_pri_byte_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_low_pri_pkt_drop_default_set (int unit, int xlp, int index,
                                           int obm_inst)
{
    static const soc_reg_t pgw_obm_low_pri_pkt_drop_regs[] = {
                PGW_OBM0_LOW_PRI_PKT_DROPr, PGW_OBM1_LOW_PRI_PKT_DROPr,
                PGW_OBM2_LOW_PRI_PKT_DROPr, PGW_OBM3_LOW_PRI_PKT_DROPr
    };
    soc_reg_t reg;
    uint64 ctrl_rval64;
    reg = pgw_obm_low_pri_pkt_drop_regs[xlp];
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm_inst, index,
                                    &ctrl_rval64));
    soc_reg64_field32_set(unit, reg, &ctrl_rval64, COUNTERf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_set(unit, reg, obm_inst, index, ctrl_rval64));

    return SOC_E_NONE;
}


int soc_td2p_mmu_delay_insertion_set (int unit, int port, int speed)
{
    int phy_port, mmu_port;    
    int pipe;
    int val = 0;
    uint32 rval;
    soc_info_t *si;

    si = &SOC_INFO(unit);      

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port] & 0x3F;

    if ( mmu_port < SOC_TD2P_MAX_MMU_PORTS_PER_PIPE ) {
        pipe = SOC_PBMP_MEMBER(si->ypipe_pbm, port) ? 1 : 0;

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, 
                    mmu_3dbg_c_regs [ pipe ], REG_PORT_ANY, mmu_port, &rval));
        /* register format is the same for PIPE0 and PIPE1 */
        if (IS_OVERSUB_PORT(unit, port )) {
            if ( speed <= 10000 ) {
                val = 15;
            } else if ( speed <= 20000 ) {
                val = 30;
            } else if ( speed <= 25000 ) {
                val = 40;
            } else if ( speed <= 40000 ) {
                val = 60;
            } else {
            }
        } else {
            /* linerate port */
        }
        soc_reg_field_set(unit, ES_PIPE0_MMU_3DBG_Cr, &rval, FIELD_Af, val);

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                    mmu_3dbg_c_regs [ pipe ], REG_PORT_ANY, mmu_port, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_subport_init
 * Purpose:
 *      Setup the pbm fields from config variables for the COE application
 * Parameters:
 *      unit    -  (IN) Device number.
 * Returns:
 *      NONE
 */
void soc_td2p_subport_init(int unit)
{
    soc_port_t  port;
    soc_info_t *si=&SOC_INFO(unit);
    soc_pbmp_t  pbmp_subport;
    int         num_subport = 0;

    SOC_PBMP_CLEAR(si->subtag_pbm);
    SOC_PBMP_CLEAR(si->subtag_allowed_pbm);
    SOC_PBMP_CLEAR(si->subtag.bitmap);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        si->port_num_subport[port] = 0;
    }

    pbmp_subport = soc_property_get_pbmp(unit, spn_PBMP_SUBPORT, 0);

    SOC_PBMP_ASSIGN(si->subtag_allowed_pbm, pbmp_subport);

    if (SOC_PBMP_NOT_NULL(si->subtag_allowed_pbm)) {
        /* Iterate through SubTag ports and reserve subport indices */
        SOC_PBMP_ITER(si->subtag_allowed_pbm, port) {

            
            num_subport =
                soc_property_port_get(unit, port, spn_NUM_SUBPORTS, 0);

            if (num_subport > TD2P_MAX_SUBPORT_COE_PORT_PER_CASPORT) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "\nCONFIG ERROR\n"
                              "num_subports_%d on port %d exceed max value,"
                              "setting it to the max allowed value %d\n\n"), 
                              num_subport, port,
                              TD2P_MAX_SUBPORT_COE_PORT_PER_CASPORT));
                num_subport = TD2P_MAX_SUBPORT_COE_PORT_PER_CASPORT;
            }

            if (port < SOC_MAX_NUM_PORTS) {
                si->port_num_subport[port] = num_subport;

            }
        }
    }

    /* Initialize the number of COE modules */
    si->num_coe_modules = TD2P_MAX_COE_MODULES;
}

int
soc_td2p_mem_config(int unit, uint16 dev_id, uint8 rev_id)
{
    soc_persist_t *sop;
    sop_memstate_t *memState;

    int         index_max;
    int         ipmc_tbl [] = { L3_IPMC_1m, L3_IPMC_REMAPm, L3_IPMCm, 
                                EGR_IPMCm};
    int         ipmc_tindex;
            
    
    sop = SOC_PERSIST(unit);
    memState = sop->memState;

    switch ( dev_id ) {
        case BCM56832_DEVICE_ID :
            index_max = 8 * 1024;
            break;
        default :
            index_max = 16 * 1024;
            break;
    }
    index_max--; /* index is a last element so = size - 1 */

    for (ipmc_tindex = 0; ipmc_tindex < sizeof(ipmc_tbl)/sizeof(ipmc_tbl[0]); 
                          ipmc_tindex++ ) {
        memState[ipmc_tbl[ipmc_tindex]].index_max = index_max;
    }

    if (dev_id == BCM56832_DEVICE_ID) { 
        index_max = 8 * 1024;
    } else {
        index_max = 12 * 1024;
    }
    index_max--;
    memState[L3_IIFm].index_max = index_max;

    /* Disable CoE related memories for appr. SKUs */
    if ((dev_id != BCM56866_DEVICE_ID) && (dev_id != BCM56867_DEVICE_ID)) {
        memState[SUBPORT_TAG_SGPP_MAPm].index_max = -1;
        memState[SUBPORT_TAG_SGPP_MAP_ONLYm].index_max = -1;
        memState[SUBPORT_TAG_SGPP_MAP_DATA_ONLYm].index_max = -1;
        memState[EGR_LPORT_PROFILEm].index_max = -1;
        memState[EGR_SUBPORT_TAG_DOT1P_MAPm].index_max = -1;
    }

    switch (dev_id) {
       case BCM56868_DEVICE_ID:
            /* L3 Forwarding disabled */
            memState[L3_ECMPm].index_max = -1;
            memState[L3_ECMP_COUNTm].index_max = -1;
            memState[ING_L3_NEXT_HOPm].index_max = -1;
            memState[INITIAL_L3_ECMPm].index_max = -1;
            memState[INITIAL_L3_ECMP_GROUPm].index_max = -1;
            memState[INITIAL_ING_L3_NEXT_HOPm].index_max = -1;
            memState[L3_DEFIP_AUX_HITBIT_UPDATEm].index_max = -1;
            memState[L3_DEFIP_AUX_SCRATCHm].index_max = -1;
            memState[L3_DEFIP_AUX_TABLEm].index_max = -1;
            memState[L3_TUNNELm].index_max = -1;
            memState[L3_TUNNEL_DATA_ONLYm].index_max = -1;
            memState[L3_TUNNEL_ONLYm].index_max = -1;

            /* MPLS related memories */
            memState[ING_MPLS_EXP_MAPPINGm].index_max = -1;
            memState[EGR_MPLS_EXP_PRI_MAPPINGm].index_max = -1;
            memState[EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm].index_max = -1;
            memState[EGR_MPLS_EXP_PRI_MAPPINGm].index_max = -1;
            memState[EGR_IP_TUNNEL_MPLSm].index_max = -1;
            memState[MPLS_ENTROPY_LABEL_DATAm].index_max = -1;
            memState[MPLS_ENTRYm].index_max = -1;

            /* coverity[fallthrough:FALSE] */
        case BCM56832_DEVICE_ID:
        case BCM56833_DEVICE_ID:
        case BCM56836_DEVICE_ID:
        case BCM56867_DEVICE_ID:
            /* FCoE */
            memState[ING_FC_HEADER_TYPEm].index_max = -1;
            memState[ING_VFT_PRI_MAPm].index_max = -1;
            memState[ING_VSANm].index_max = -1;
            memState[FC_MAP_PROFILEm].index_max = -1;
            memState[FCOE_HOP_COUNT_FNm].index_max = -1;
            memState[EGR_VSAN_INTPRI_MAPm].index_max = -1;
            memState[EGR_FCOE_CONTROL_1m].index_max = -1;
            memState[EGR_VFT_FIELDS_PROFILEm].index_max = -1;
            memState[EGR_VFT_PRI_MAPm].index_max = -1;
            memState[EGR_FC_HEADER_TYPEm].index_max = -1;

            /* NAT */
            memState[ING_SNATm].index_max = -1;
            memState[ING_SNAT_DATA_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLYm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Xm].index_max = -1;
            memState[ING_SNAT_HIT_ONLY_Ym].index_max = -1;
            memState[ING_SNAT_ONLYm].index_max = -1;
            memState[ING_DNAT_ADDRESS_TYPEm].index_max = -1;
            memState[EGR_NAT_PACKET_EDIT_INFOm].index_max = -1;

            /* TRILL */
            memState[ING_TRILL_PARSE_CONTROLm].index_max = -1;
            memState[ING_TRILL_PAYLOAD_PARSE_CONTROLm].index_max = -1;
            memState[EGR_TRILL_PARSE_CONTROLm].index_max = -1;
            memState[EGR_TRILL_PARSE_CONTROL_2m].index_max = -1;
            memState[EGR_TRILL_RBRIDGE_NICKNAMESm].index_max = -1;
            memState[EGR_TRILL_TREE_PROFILEm].index_max = -1;
            break;
        case BCM56860_DEVICE_ID:
            if (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_NONE) {
                if (SOC_SWITCH_BYPASS_MODE(unit) ==
                        SOC_SWITCH_BYPASS_MODE_L3_AND_FP) {
    
                    /* IFP */
                    memState[FP_RANGE_CHECKm].index_max = -1;
                    memState[FP_GLOBAL_MASK_TCAMm].index_max = -1;
                    memState[FP_GM_FIELDSm].index_max = -1;
                    memState[FP_TCAMm].index_max = -1;
                    memState[FP_SLICE_MAPm].index_max = -1;
                    memState[FP_POLICY_TABLEm].index_max = -1;
                    memState[FP_METER_TABLEm].index_max = -1;
                    memState[FP_COUNTER_TABLEm].index_max = -1;
                    memState[FP_COUNTER_TABLE_Xm].index_max = -1;
                    memState[FP_COUNTER_TABLE_Ym].index_max = -1;
                    memState[FP_STORM_CONTROL_METERSm].index_max = -1;
                    memState[FP_SLICE_KEY_CONTROLm].index_max = -1;
                    memState[FP_PORT_METER_MAPm].index_max = -1;
                    memState[FP_I2E_CLASSID_SELECTm].index_max = -1;
                    memState[FP_HG_CLASSID_SELECTm].index_max = -1;
                     
                    /* EFP */
                    memState[EFP_TCAMm].index_max = -1;
                    memState[EFP_POLICY_TABLEm].index_max = -1;
                    memState[EFP_METER_TABLEm].index_max = -1;
                    memState[EGR_PW_INIT_COUNTERSm].index_max = -1;

                    /* IVXLT */
                    memState[VLAN_PROTOCOLm].index_max = -1;
                    memState[VLAN_PROTOCOL_DATAm].index_max = -1;
                    memState[VLAN_SUBNETm].index_max = -1;
                    memState[VLAN_SUBNET_ONLYm].index_max = -1;
                    memState[VLAN_SUBNET_DATA_ONLYm].index_max = -1;
                    memState[VLAN_MACm].index_max = -1;
                    memState[VLAN_XLATEm].index_max = -1;
                    memState[VFP_TCAMm].index_max = -1;
                    memState[VFP_POLICY_TABLEm].index_max = -1;
                    memState[ING_VLAN_TAG_ACTION_PROFILEm].index_max = -1;
                    memState[MPLS_ENTRYm].index_max = -1;
                    memState[UDF_CONDITIONAL_CHECK_TABLE_CAMm].index_max = -1;
                    memState[UDF_CONDITIONAL_CHECK_TABLE_RAMm].index_max = -1;
                    memState[ING_ETAG_PCP_MAPPINGm].index_max = -1;
                    memState[VLAN_XLATE_ECCm].index_max = -1;
                    memState[MPLS_ENTROPY_LABEL_DATAm].index_max = -1;
                    memState[ING_ETAG_PCP_MAPPINGm].index_max = -1;
                }
            }
            break;
        default:
            ;

    }

    return SOC_E_NONE;
}

int
soc_td2p_get_shared_bank_size(int unit, uint16 dev_id, uint8 rev_id)
{
    int bank_size = BCM_TD2P_SMALL_BANK_SIZE;


    switch ( dev_id ) {
    case BCM56867_DEVICE_ID :
        bank_size = BCM_TD2P_MAX_BANK_SIZE;
        break;
    }

    return bank_size;
}

uint32* soc_td2p_mmu_params_arr_get (uint16 dev_id, uint8 rev_id)
{
    switch (dev_id) {
        case BCM56860_DEVICE_ID:
        case BCM56861_DEVICE_ID:
        case BCM56862_DEVICE_ID:
        case BCM56864_DEVICE_ID:
        case BCM56865_DEVICE_ID:
        case BCM56866_DEVICE_ID:
        case BCM56867_DEVICE_ID:
        case BCM56868_DEVICE_ID:
        case BCM56836_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_16MB];
            break;
        case BCM56832_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_9MB];
            break;
        case BCM56833_DEVICE_ID:
            return soc_mmu_pkt_buf_size_arr[_MMU_PKT_BUF_SIZE_12MB];
            break;
    }

    return NULL;
}

STATIC uint32 trident2p_write_data[] = {
    0x1fffffc0, 0xbffffbfe, 0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1dfcfe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0x1dfc7e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000, 0x40080000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcde80, 0xbffffbfe,
    0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x80000180, 0x1dfc9e80, 0xbffffbfe, 0x44000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xbfcbe80, 0xbffffbfe, 0x40000000, 0x40001000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000, 0x40001000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc3e80, 0xbffffbfe,
    0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xbfcde80, 0xbffffbfe, 0x40000000, 0x41000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfc3e80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcde80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x17fc7e80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x17fc3e80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x13fcbe80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x13fc7e80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xffcfe80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xffcbe80,
    0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc5e80,
    0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80, 0xbffffbfe,
    0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80, 0xbffffbfe, 0x40000000,
    0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcbe80,
    0xbffffbfe, 0x40000000, 0x48000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xdfc5e80, 0xbffffbfe, 0x40000000, 0x40000002, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xdfc1e80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfcfe80, 0xbffffbfe, 0x40000000,
    0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xbfc9e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0x1dfcbe80, 0xbffffbfe, 0x40000000, 0x40000200, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcfe80,
    0xbffffbfe, 0x40000000, 0x40000200, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcfe80, 0xbffffbfe,
    0x40000000, 0x40000200, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc7e80, 0xbffffbfe, 0x40000000,
    0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfc7e80,
    0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc3e80, 0xbffffbfe, 0x40000000,
    0x40080000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfcde80,
    0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfcde80, 0xbffffbfe, 0x44000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1dfc9e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1dfc9e80,
    0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfcbe80, 0xbffffbfe, 0x40000000,
    0x40001000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfcbe80,
    0xbffffbfe, 0x40000000, 0x40001000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000, 0x40001000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfc7e80, 0xbffffbfe, 0x40000000,
    0x40001000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc3e80,
    0xbffffbfe, 0x40000000, 0x40080000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xbfc3e80, 0xbffffbfe, 0x40000000, 0x40080000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfcde80, 0xbffffbfe, 0x40000000,
    0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000,
    0x40000000, 0x80000180, 0xbfcde80, 0xbffffbfe, 0x40000000, 0x41000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1bfc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1bfc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1bfcde80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1bfcde80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x17fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x17fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x17fc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x17fc3e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x13fcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x13fcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x13fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x13fc7e80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xffcfe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xffcfe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xffcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0xffcbe80, 0xbffffbfe, 0x40000000, 0x40000008, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1ffc5e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0x1ffc5e80, 0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80,
    0xbffffbfe, 0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1ffc1e80, 0xbffffbfe,
    0x44000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc5e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1dfc1e80, 0xbffffbfe, 0x40000000,
    0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80, 0xbffffbfe, 0x40000000,
    0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0x1bfcfe80,
    0xbffffbfe, 0x40000000, 0x48000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x80000180, 0x1bfcbe80, 0xbffffbfe, 0x40000000, 0x48000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0x1bfcbe80, 0xbffffbfe, 0x40000000,
    0x48000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xdfc5e80,
    0xbffffbfe, 0x40000000, 0x40000002, 0x40000000, 0x40000000, 0x41000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xdfc5e80, 0xbffffbfe,
    0x40000000, 0x40000002, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xdfc1e80, 0xbffffbfe, 0x40000000,
    0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xdfc1e80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfcfe80, 0xbffffbfe, 0x40000000,
    0x40200000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x41000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180,
    0xbfcfe80, 0xbffffbfe, 0x40000000, 0x40200000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x80000180, 0xbfc9e80, 0xbffffbfe, 0x40000000,
    0x40000008, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x41000000,
    0x40000000, 0x40000000, 0x40000000, 0x80000180, 0xbfc9e80, 0xbffffbfe,
    0x40000000, 0x40000008, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x40000000,
    0x40000000, 0x40000000, 0x40000000, 0x40000000, 0x80000180
};

/*
 * Function:
 *      trident2p_ovstb_toggle
 * Purpose:
 *      Execute the OVST sequence for Trident2Plus_A0 devices
 * Parameters:
 *      unit - StrataSwitch unit #
*/
void
trident2p_ovstb_toggle(int unit)
{
    int i = 0;
    int array_length = 0;
    int write_data_count = 0;
    uint32 read_data = 0;
      
    array_length = sizeof(trident2p_write_data) / sizeof(uint32); 
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
    READ_TOP_UC_TAP_CONTROLr(unit, &read_data);

    for ( i=0; i < array_length; i++)
    {
        WRITE_TOP_UC_TAP_WRITE_DATAr(unit, trident2p_write_data[write_data_count]);
        WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
        READ_TOP_UC_TAP_READ_DATAr(unit, &read_data);
        write_data_count++;
    
    }
      
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2f);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2e);
    WRITE_TOP_UC_TAP_CONTROLr(unit, 0x2c);

    return;    
}


/*
 * Function:
 *      soc_td2p_idb_buf_reset
 * Purpose:
 *      Poll until Cell Assembly and OBM buffers are empty.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    soc_field_t field;
    uint64 rval64;
    int phy_port;
    int pgw;
    int xlp;
    int port_index;
    int obm;
    soc_timeout_t to;
    int use_count;
    int fifo_empty;
    static const soc_reg_t pgw_obm_use_counter_regs[] = {
        PGW_OBM0_USE_COUNTERr, PGW_OBM1_USE_COUNTERr,
        PGW_OBM2_USE_COUNTERr, PGW_OBM3_USE_COUNTERr
    };
    static const soc_field_t pgw_obm_use_counter_fields[] = {
        PORT0_USE_COUNTf, PORT1_USE_COUNTf,
        PORT2_USE_COUNTf, PORT3_USE_COUNTf
    };

    /* If not reset, nothing to do */
    if (!reset) {
        return SOC_E_NONE;
    }

    /* Get physical port PGW information */
    phy_port = si->port_l2p_mapping[port];
    SOC_IF_ERROR_RETURN
        (soc_td2p_phy_port_pgw_info_get(unit, phy_port,
                                        &pgw, &xlp, &port_index));

    /* Assume values returned above are valid */
    reg = pgw_obm_use_counter_regs[xlp];
    field = pgw_obm_use_counter_fields[port_index];
    obm = pgw | SOC_REG_ADDR_INSTANCE_MASK;

    if (!SAL_BOOT_SIMULATION) {
        /* Poll until Cell Assembly and OBM buffers are empty */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            /* Get PGW use counter */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, obm, port_index,
                                            &rval64));
            use_count = soc_reg64_field32_get(unit, reg, rval64, field);

            /* Get PGW fifo empty flag */
            SOC_IF_ERROR_RETURN(READ_PGW_BOD_STATUS1r(unit, port, &rval64));
            fifo_empty = soc_reg64_field32_get(unit, PGW_BOD_STATUS1r,
                                               rval64, PGW_BOD_FIFO_EMPTYf);

            if ((use_count == 0) && (fifo_empty == 1)) {
                break;
            }

            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "PGW buffer reset timeout: port %d, %s, "
                                      "timeout (use_count: %d) "
                                      "(pgw_bod_fifo_emty:%d)\n"),
                           port, SOC_PORT_NAME(unit, port),
                           use_count, fifo_empty));
                return SOC_E_INTERNAL;
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_td2p_port_credit_size_get(int unit, soc_port_t port, int *ep_credit_size)
{
    int speed_max;

    if (ep_credit_size == NULL) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_td2p_port_speed_get(unit, port, &speed_max));

    if (IS_OVERSUB_PORT(unit, port)) {
      if (speed_max >= 100000) {
         return SOC_E_PARAM;
      } else if (speed_max >= 40000) {
         *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(40G);
      } else if (speed_max >= 20000) {
         *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(20G);
      } else {
         *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(10G);
      }
    } else {
       if (speed_max >= 100000) {
          *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(100G);
       } else if (speed_max >= 40000) {
          *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(40G);
       } else if (speed_max >= 20000) {
          *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(20G);
       } else {
          *ep_credit_size = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(10G);
       }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_mmu_ep_credit_set
 * Purpose:
 *      Initalize EP credits in MMU so EP is in charge
 *      of distributing the correct number of credits.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_mmu_ep_credit_set(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int mmu_port;
    int ep_credit_size = 0;
    soc_reg_t reg;
    uint32 rval;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Get MMU port */
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port == -1) {
        return SOC_E_INTERNAL;
    }
    mmu_port &= 0x3F;    /* Convert mmu port number for pipe */

    /* Wait until credits are initialised at non-optimum value
     * EP passes MMU 1 credit per cycle, so worse case delay is 95 cycles after EGR_ENABLE is set.
     * @793.75MHz, this is ~120ns.
     */
    sal_usleep(1);

    if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) {
        reg = ES_PIPE0_MMU_PORT_CREDITr;
    } else {
        reg = ES_PIPE1_MMU_PORT_CREDITr;
    }

    SOC_IF_ERROR_RETURN(_soc_td2p_port_credit_size_get(unit, port, &ep_credit_size));
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg, &rval, INIT_CREDITf, ep_credit_size);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_mmu_ep_credit_reset
 * Purpose:
 *      Reset EP credits
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_mmu_ep_credit_reset(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int mmu_port;
    soc_reg_t reg;
    uint32 rval;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Get MMU port */
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port == -1) {
        return SOC_E_INTERNAL;
    }
    mmu_port &= 0x3F;    /* Convert mmu port number for pipe */

    if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) {
        reg = ES_PIPE0_MMU_PORT_CREDITr;
    } else {
        reg = ES_PIPE1_MMU_PORT_CREDITr;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg, &rval, INIT_CREDITf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_edb_buf_reset
 * Purpose:
 *      Initialize EP credits in MMU, release EDB port buffer and
 *      enable cell request in EP.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port;
    int use_pfc_optimized_settings = FALSE;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    if (reset) {
        /*
         * Hold EDB port buffer in reset state and disable cell
         * request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));
    } else {

        /* Initialize EP credits in MMU */
        SOC_IF_ERROR_RETURN(_soc_td2p_mmu_ep_credit_reset(unit, port));

        /* If system is able to use optimized settings, set
           credit limit accordingly, else use default */
        use_pfc_optimized_settings = soc_td2p_use_pfc_optimized_settings(unit);
        if (use_pfc_optimized_settings) {
           SOC_IF_ERROR_RETURN(soc_td2p_set_mmu_credit_limit(unit, port));
        }

        /*
         * Release EDB port buffer reset and
         * enable cell request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                            entry, ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));

        /* If system is able to use optimized settings, set
           watermark thresh accordingly, else use default */
        if (use_pfc_optimized_settings) {
           SOC_IF_ERROR_RETURN(_soc_td2p_mmu_ep_credit_set(unit, port));
           SOC_IF_ERROR_RETURN(
           soc_td2p_set_edb_pfc_watermark_threshold(unit, port));
        }
    }

    return SOC_E_NONE;
}


STATIC int
_soc_td2p_resource_data_check(int unit, int num_res, soc_port_resource_t *res)
{
    int i, j;
    /* null check for all resource data before accessing register */
    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];
        if (NULL == p) {
            /* 
            * COVERITY 
            * It is kept intentionally as a defensive check. 
            */
            /* coverity[dead_error_line] */
            return SOC_E_PARAM;
        }
        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];
            if (NULL == p_lane_info) {
                return SOC_E_PARAM;
            }
        }
    }
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_pgw_obm_default_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_CONTROL
 *          PGW_OBM[0-3]_HIGH_PRI_BYTE_DROP[]
 *          PGW_OBM[0-3]_HIGH_PRI_PKT_DROP[]
 *          PGW_OBM[0-3]_LOW_PRI_BYTE_DROP[]
 *          PGW_OBM[0-3]_LOW_PRI_DROP_PROB[]
 *          PGW_OBM[0-3]_LOW_PRI_PKT_DROP[]
 *          PGW_OBM[0-3]_USE_COUNTER
 *          PGW_OBM[0-3]_SHARED_CONFIG
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_INIT_SW_DEFAULT
 *                          Use SW default value
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_td2p_pgw_obm_default_set(int unit, int num_res, soc_port_resource_t *res,
                             int default_flag)
{
    int obm_inst;
    int xlp, port_index;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp = p_lane_info->xlp;
            port_index = p_lane_info->port_index;
            obm_inst = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;

            /* PGW_OBM[0-3]_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_ctrl_reg_default_set(unit, xlp,
                               port_index, obm_inst, p->oversub, default_flag));
            /* PGW_OBM[0-3]_LOW_PRI_DROP_PROB[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_prob_drop_default_set (unit, xlp,
                               port_index, obm_inst, default_flag));
            /* PGW_OBM[0-3]_HIGH_PRI_BYTE_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_high_pri_byte_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_HIGH_PRI_PKT_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_high_pri_pkt_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_LOW_PRI_BYTE_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_low_pri_byte_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_LOW_PRI_PKT_DROP[] */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_low_pri_pkt_drop_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_USE_COUNTER */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_use_counter_reg_default_set(unit, xlp,
                               port_index, obm_inst));
            /* PGW_OBM[0-3]_SHARED_CONFIG */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_shared_config_reg_default_set(unit, xlp,
                               port_index, obm_inst, default_flag));
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_td2p_pgw_obm_hw_default_set(int unit, int num_res, soc_port_resource_t *res)
{
    int rv;
    rv = soc_td2p_pgw_obm_default_set(unit, num_res, res,
                                      PGW_OBM_INIT_HW_DEFAULT);
    return rv;
}

STATIC int
soc_td2p_pgw_obm_sw_default_set(int unit, int num_res, soc_port_resource_t *res)
{
    int rv;
    rv = soc_td2p_pgw_obm_default_set(unit, num_res, res,
                                      PGW_OBM_INIT_SW_DEFAULT);
    return rv;
}

/*
 * Function:
 *      soc_td2p_pgw_obm_set
 * Purpose:
 *      Reconfigure PGW_OBM registers in flex port sequence
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_pgw_obm_set(int unit,
                     int pre_num_res,  soc_port_resource_t *pre_res,
                     int post_num_res, soc_port_resource_t *post_res)
{
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, post_num_res, post_res));

    SOC_IF_ERROR_RETURN(soc_td2p_pgw_obm_hw_default_set(unit,
                                                        pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(soc_td2p_pgw_obm_sw_default_set(unit,
                                                        post_num_res, post_res));
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_obm_threshold_default_set
 * Purpose:
 *      Reconfigure PGW Threshold in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_THRESHOLD
 *          PGW_OBM[0-3]_THRESHOLD2
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      lossless     - (IN) Lossless configuration of device
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_INIT_SW_DEFAULT
 *                          Use SW default value
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC
soc_error_t soc_td2p_obm_threshold_default_set(int unit,
                                               int num_res,
                                               soc_port_resource_t *res,
                                               int lossless,
                                               int default_flag)
{
    pgw_obm_threshold_regs_t pgw_obm_threshold_regs =
                             soc_trident2_pgw_obm_threshold_regs_get(unit);
    soc_reg_t  reg;
    int obm_inst;
    int xlp, port_index, speed;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        speed = p->speed;
        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp        = p_lane_info->xlp;
            port_index = p_lane_info->port_index;
            obm_inst   = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;

            reg = pgw_obm_threshold_regs[xlp];
            /* PGW_OBM[0-3]_THRESHOLD */
            SOC_IF_ERROR_RETURN
                (soc_td2p_set_obm_registers(unit, reg, speed, port_index,
                                            obm_inst, lossless, default_flag));

            /* PGW_OBM[0-3]_THRESHOLD2 */
            SOC_IF_ERROR_RETURN
                (soc_td2p_obm_cut_through_threshold_set(unit, xlp, port_index,
                                                     obm_inst, default_flag));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_threshold_hw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{

    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               PGW_OBM_INIT_HW_DEFAULT));
    return SOC_E_NONE;
}

STATIC int
soc_td2p_obm_threshold_sw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               PGW_OBM_INIT_SW_DEFAULT));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_obm_threshold_set
 * Purpose:
 *  Reconfigure PGW registers in flex port sequence
 *  In this function PGW OBM threshold registers will be initialized.
 *  This function will do two things,
 *  1. clear all registers for old ports which exists in pre-Flex status
 *  2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *  unit         - (IN) Unit number.
 *  pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *  post_num_res - (IN) Number of resource data which exist in post-Flex status
 *  pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *  post_res     - (IN) Resource data structure pointer in post-Flex status
 *  lossless     - (IN) mmu lossless information
 *
 * Returns:
 *  SOC_E_XXX
 *
 * Note : For 100G port, this functions expects the ovsersub field to be
 * disabled.
 */
soc_error_t soc_td2p_obm_threshold_set(int unit,
                         int pre_num_res, soc_port_resource_t *pre_res,
                         int post_num_res, soc_port_resource_t *post_res,
                         int lossless)
{
    
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, pre_num_res,
                                                      pre_res));
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, post_num_res,
                                                      post_res));
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_hw_default_set(unit,
                        pre_num_res, pre_res, lossless));

    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_sw_default_set(unit, 
                        post_num_res, post_res, lossless));

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_pgw_port_default_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_MAX_USAGE:
 *          PGW_OBM_PORT[0-15]_FC_CONFIG
 *          PGW_OBM[0-3]_PRIORITY_MAP[]
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_FLEXPORT_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_FLEXPORT_INIT_SW_DEFAULT
 *                          Use SW default value
 *      lossless     - (IN) mmu lossless information
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. master/slave pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw+1)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_td2p_pgw_port_default_set(int unit, int num_res, soc_port_resource_t *res,
                              int lossless, int default_flag)
{
    int obm_inst;
    int xlp, port_index;
    int i, j;

    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];

        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];

            xlp = p_lane_info->xlp;
            port_index = p_lane_info->port_index;

            obm_inst = p_lane_info->pgw | SOC_REG_ADDR_INSTANCE_MASK;

            /* PGW_MIB_RESET */
            SOC_IF_ERROR_RETURN(soc_td2p_pgw_mib_reset_reg_default_set(
                                    unit, xlp, port_index, obm_inst));

            /* PGW_OBM[0-3]_MAX_USAGE */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_max_usage_reg_default_set(unit, xlp,
                                       port_index, obm_inst));

            if (!lossless) {
                continue;
            }

            /* PGW_OBM_PORT[0-15]_FC_CONFIG */
            SOC_IF_ERROR_RETURN
                (soc_td2_obm_fc_config_reg_default_set(unit, xlp, port_index,
                                       obm_inst, p->logical_port, p->oversub,
                                       default_flag));
        }
    }

    return SOC_E_NONE;
}

STATIC int
soc_td2p_pgw_port_hw_default_set(int unit, int num_res, soc_port_resource_t *res,
                                 int lossless)
{
    int rv;
    rv = soc_td2p_pgw_port_default_set(unit, num_res, res,
                                       lossless, PGW_OBM_INIT_HW_DEFAULT);
    return rv;
}

STATIC int
soc_td2p_pgw_port_sw_default_set(int unit, int num_res, soc_port_resource_t *res,
                                 int lossless)
{
    int rv;
    rv = soc_td2p_pgw_port_default_set(unit, num_res, res,
                                       lossless, PGW_OBM_INIT_SW_DEFAULT);
    return rv;
}


STATIC
int soc_td2p_ovr_sub_tdm_config_get(int unit,
                                    _soc_td2p_ovr_sub_tdm_t *ovs_tdm)
{
    int pipe;
    int group;
    int slot;
    uint32 wt_group_select;
    soc_reg_t reg;
    uint32 rval;
    int index;
    mmu_ovs_group_wt_regs_t mmu_ovs_group_wt_regs =
                                        mmu_ovs_group_wt_regs_td2p_local;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    mmu_ovs_group_regs_t mmu_ovs_group_regs =
                           soc_trident2_mmu_ovs_group_regs_get(unit);
    mmu_ovs_group_wt_select_regs_t mmu_ovs_group_wt_select_regs =
                           soc_trident2_mmu_ovs_group_wt_select_regs_get(unit);

    /* Initialize MMU OverSub TDM Ports Database */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                /* Invalid MMU Port 0x3f */
                mmu_tdm->mmu_port[pipe][group][slot] = 0x3f;
            }
        }
    }

    /* Extract MMU Over Subscription TDM */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        wt_group_select = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, mmu_ovs_group_wt_select_regs[pipe],
                               REG_PORT_ANY, 0, &wt_group_select));

        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            reg = mmu_ovs_group_regs[pipe][group];
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
                mmu_tdm->mmu_port[pipe][group][slot] =
                                soc_reg_field_get(unit, reg, rval, MMU_PORTf);
            }
        }
        /* TD2P supports 4 oversub speeds only 10/20 and 40*/
        for (group = 0; group < _MMU_OVS_WT_GROUP_COUNT; group++) {
            reg = mmu_ovs_group_wt_regs[pipe][group];
            for (index = 0; index < _MMU_OVS_GROUP_TDM_LENGTH; index++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
                mmu_tdm->grp_wt[pipe][group][index] =
                     soc_reg_field_get(unit, reg, rval, WEIGHTf);
            }
        }
            /*Read the Group Weight Sel fields, Used later while
             * configuring the MMU TDM.
             */
        for (group = 0; group < MAX_MMU_OVS_GROUP_COUNT; group++) {
            mmu_tdm->grp_wt_sel[pipe][group] =
                soc_reg_field_get(unit,  mmu_ovs_group_wt_select_regs[pipe],
                                  wt_group_select,
                                  mmu_ovr_group_wt_select_fields[group]);
        }
    }

    return SOC_E_NONE;
}


/* TD2P OverSub Scheduler */
/* For Egress MMU OvrSub schedular configuration, we need to create weight
 * group for each port speed.
 * If we didn't have a Port speed setup in the config file and during flex
 * operation, if we want to setup a port with new speed(Eg:40) we need to
 * create a new group. This functions sets up the Weight configuration.
 */
STATIC
int soc_td2p_ovr_sub_tdm_mmu_group_create(int unit, int pipe, int speed_max,
                                          _soc_td2p_ovr_sub_tdm_t *ovs_tdm,
                                          int *group_new)
{
    mmu_ovs_group_regs_t mmu_ovs_group_regs =
                          soc_trident2_mmu_ovs_group_regs_get(unit);
    mmu_ovs_group_wt_select_regs_t mmu_ovs_group_wt_select_regs =
                           soc_trident2_mmu_ovs_group_wt_select_regs_get(unit);
    mmu_ovs_group_wt_regs_t mmu_ovs_group_wt_regs =
                                        mmu_ovs_group_wt_regs_td2p_local;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    int index;
    int group; 
    int slot;
    int wt_group = 0;
    soc_reg_t reg;
    uint32 rval;
    int weight;

    for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
        if (mmu_tdm->grp_wt_sel[pipe][group] == td2p_ovs_wt_group_speed_NA) {
            /* This is an empty group, Initialize the group */
            break;
        }
    }

    if (group == GET_MMU_OVS_GCOUNT(unit)) {
        /* No Groups Available, Hence return SOC_E_RESOURCE */
        return SOC_E_RESOURCE;
    }
    reg = mmu_ovs_group_regs[pipe][group];

    /* Initialize the MMU port to 0x3f for all the slots */
    for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, MMU_PORTf, 0x3f);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
    }
    
    if ( speed_max <= 11000 ) {
        wt_group = td2p_ovs_wt_group_speed_10G;
    } else if ( speed_max <= 21000 ) {
        wt_group = td2p_ovs_wt_group_speed_20G;
    } else if ( speed_max <= 42000 ) {
        wt_group = td2p_ovs_wt_group_speed_40G;
    } else {
        wt_group = td2p_ovs_wt_group_speed_NA; /* "not" valid group */
    }
    /* use 2500M as weight unit */
    weight = (speed_max > 2500 ? speed_max : 2500) / 2500;

    /* Read the Current OverSub Group Select Register per pipe, and assign the
     * group_weight for this group*/
    rval = 0;   
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, mmu_ovs_group_wt_select_regs [pipe], 
                       REG_PORT_ANY, 0, &rval));
    /* Clear the Existing Weight, and append the new weight */
    soc_reg_field_set(unit, mmu_ovs_group_wt_select_regs [pipe],
                      &rval, mmu_ovr_group_wt_select_fields[group], wt_group);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, mmu_ovs_group_wt_select_regs [pipe], 
                       REG_PORT_ANY, 0, rval));

    /* For the new Group, set the weight for all it's entries */
    rval = 0;   
    reg = mmu_ovs_group_wt_regs[pipe][wt_group];
    for (index = 0; index < _MMU_OVS_GROUP_TDM_LENGTH; index++) {
        soc_reg_field_set(unit, reg, &rval, WEIGHTf,
                          weight * (index + 1));
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, index, rval));
    }
    *group_new = group;
    return SOC_E_NONE;
}




/* TD2P OverSub Scheduler */
/* This function expects the oversub field in port_resource_t structure to be 0
 * for 100G ports. Since oversub on 100G ports is not supported for TD2Plus.
 *
 * This function Updates the Over Sub TDM calender.
 */
STATIC
int soc_td2p_ovr_sub_tdm_db_update(int unit, int num_port_res,
                                   soc_port_resource_t *port_res,
                                   _soc_td2p_ovr_sub_tdm_t *ovs_tdm,
                                   int db_type)
{
    int pipe, res_count;
    int group;
    int slot = 0;
    int count;
    int speed_max;
    int group_wt;
    int mmu_port, mmu_port_update;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;

    for ( res_count = 0; res_count < num_port_res; res_count++) {
        soc_port_resource_t *p = &port_res[res_count];

        /* If OBM is not enabled then continue */
        if (!p->oversub) {
            continue;
        }

        if (p->flags & SOC_PORT_RESOURCE_INACTIVE) {
            continue;
        }

        speed_max   = p->speed;
        pipe        = p->pipe;

        if (_SOC_TRIDENT2P_UPDATE_TDM_PRE  == db_type) {
            mmu_port        = p->mmu_port & 0x3f;
            mmu_port_update = 0x3f; /* Invalid MMU Port */
        } else {
            mmu_port        = 0x3f;
            mmu_port_update = p->mmu_port & 0x3f;      /* Valid MMU Port */
        }

        if ( speed_max <= 11000 ) {
            group_wt = td2p_ovs_wt_group_speed_10G;
        } else if ( speed_max <= 21000 ) {
            group_wt = td2p_ovs_wt_group_speed_20G;
        } else if ( speed_max <= 42000 ) {
            group_wt = td2p_ovs_wt_group_speed_40G;
        } else {
            group_wt = td2p_ovs_wt_group_speed_NA; /* "not" valid group */
        }

        /* Update MMU OverSub TDM Ports Database */
        count = 0;
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            /* Identify the group that needs to be updated */
            if (mmu_tdm->grp_wt_sel[pipe][group] != group_wt) {
                continue;
            }
            /* Update MMU OverSub TDM Ports Database */
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                /* Invalid MMU Port 0x3f */
                if (mmu_tdm->mmu_port[pipe][group][slot] == mmu_port) {
                    count++;
                    break;
                }
            }
            if (count) {
                break;
            }
        }

        if (group ==  GET_MMU_OVS_GCOUNT(unit) && 
            (_SOC_TRIDENT2P_UPDATE_TDM_PRE  != db_type)) {
            /* Group (or) Free slot not found.
             * Need to allocate a new group if available */

            SOC_IF_ERROR_RETURN(
                         soc_td2p_ovr_sub_tdm_mmu_group_create(unit, pipe,
                                              speed_max, ovs_tdm, &group));

            /* Start from the begining slot */
            mmu_tdm->grp_wt_sel[pipe][group] = group_wt;
            slot = 0;
            count = 1;

            /* MMU Port should be configured only once */
            if (1 != count) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                           "MMU port %d cannot be found in MMU OVS GROUP TABLE: "
                           "ES_PIPE_OVR_SUB_GRP_TBL\n"),
                           mmu_port));
                return SOC_E_PARAM;
            }
        }
        if (group < GET_MMU_OVS_GCOUNT(unit)) {
            /* Update the MMU_PORT */
            mmu_tdm->tdm_update[pipe][group][slot] = 1;
            mmu_tdm->mmu_port[pipe][group][slot] = mmu_port_update;
        }
    }
    return SOC_E_NONE;
}

STATIC
int soc_td2p_ovr_sub_tdm_hw_update(int unit,
                                   _soc_td2p_ovr_sub_tdm_t *ovs_tdm)
{
    int pgw_master;
    int pipe;
    int group;
    int slot;
    int index = 0;
    int pgw;
    int base;
    soc_reg_t reg;
    uint32 rval;
    uint64 rval64, ctrl_rval64;
    int obm_inst;
    int phy_port;
    int count = 0;
    _soc_td2p_ovr_sub_mmu_tdm_t *mmu_tdm = &ovs_tdm->mmu_tdm;
    mmu_ovs_group_regs_t mmu_ovs_group_regs =
                           soc_trident2_mmu_ovs_group_regs_get(unit);
    pgw_ovs_tdm_regs_t pgw_ovs_tdm_regs =
                           soc_trident2_pgw_ovs_tdm_regs_get(unit);
    pgw_tdm_fields_t pgw_tdm_fields =
                           soc_trident2_pgw_tdm_fields_get(unit);
    pgw_ovs_spacing_regs_t pgw_ovs_spacing_regs =
                           soc_trident2_pgw_ovs_spacing_regs_get(unit);
    pgw_spacing_fields_t pgw_spacing_fields =
                           soc_trident2_pgw_spacing_fields_get(unit);
    _soc_trident2_tdm_t *tdm;

    tdm = soc_td2_td2p_tdm_sched_info_get(unit, 0);
    if (tdm == NULL) {
        return SOC_E_INIT;
    }
    

    /* Initialize MMU OverSub TDM Ports Database */
    for (pipe = 0; pipe < TD2P_PIPES_PER_DEV; pipe++) {
        for (group = 0; group < GET_MMU_OVS_GCOUNT(unit); group++) {
            for (slot = 0; slot < _MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                if (mmu_tdm->tdm_update[pipe][group][slot]) {
                    /* Invalid MMU Port 0x3f */
                    reg = mmu_ovs_group_regs[pipe][group];

                    SOC_IF_ERROR_RETURN
                     (soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));

                    soc_reg_field_set(unit, reg, &rval, MMU_PORTf,
                             mmu_tdm->mmu_port[pipe][group][slot] & 0x3f);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
                }
            }
        }
    }

    /* Extract PGW Over Subscription TDM */
    for (pgw = 0; pgw < TD2P_PGWS_PER_DEV; pgw += TD2P_PGWS_PER_QUAD) {
        pgw_master = pgw;

        if (( pgw == 2 ) || ( pgw == 6 )) {
            pgw_master = pgw + 1;
        }

        obm_inst = pgw_master | SOC_REG_ADDR_INSTANCE_MASK;

        /* Update PGW oversubscription ports TDM */
        count = 0;
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, obm_inst, 0,
                                        &ctrl_rval64));
        for (base = 0; base < _PGW_TDM_OVS_SIZE;
                       base += _PGW_TDM_SLOTS_PER_REG) {
            reg = pgw_ovs_tdm_regs[base / _PGW_TDM_SLOTS_PER_REG];
            COMPILER_64_ZERO(rval64);
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                phy_port = tdm->pgw_ovs_tdm[pgw_master / 2][slot];
                if (phy_port == NUM_EXT_PORTS) {
                    phy_port = 0xff;
                } else {
                    count++;
                }
                soc_reg64_field32_set(unit, reg, &rval64, pgw_tdm_fields[slot],
                                      phy_port);
            }
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, obm_inst, 0,
                                                            rval64));
        }
        soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                              OS_TDM_ENABLEf, count ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, PGW_TDM_CONTROLr, obm_inst, 0, ctrl_rval64));

        /* Configure PGW oversubscription port spacing */
        for (base = 0; base < _PGW_TDM_OVS_SIZE;
             base += _PGW_TDM_SLOTS_PER_REG) {
            reg = pgw_ovs_spacing_regs[base / _PGW_TDM_SLOTS_PER_REG];
            COMPILER_64_ZERO(rval64);
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                phy_port = tdm->pgw_ovs_spacing[pgw_master / 2][slot];
                soc_reg64_field32_set(unit, reg, &rval64,
                                      pgw_spacing_fields[slot], phy_port);
            }
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, obm_inst, 0, rval64));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_ovr_sub_tdm_update
 * Description:
 *      Update PGW and MMU TDM Over subscription programming for flex port
 * Parameters:
 *      unit          - Device number
 *      pre_num_res   - Number of Previous resources
 *      pre_res       - Previous resource information.
 *      post_num_res  - Number of Current resource configured
 *      post_res      - Current port resource information.
 */
int
soc_td2p_ovr_sub_tdm_update(int unit, 
                         int pre_num_res, soc_port_resource_t *pre_res,
                         int post_num_res, soc_port_resource_t *post_res)
{
    _soc_td2p_ovr_sub_tdm_t *ovr_sub_tdm;
    int rv;

    
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, pre_num_res,
                                                      pre_res));
    SOC_IF_ERROR_RETURN(_soc_td2p_resource_data_check(unit, post_num_res,
                                                      post_res));

    ovr_sub_tdm = sal_alloc(sizeof(_soc_td2p_ovr_sub_tdm_t), "ovr_sub_tdm");
    if (ovr_sub_tdm == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(ovr_sub_tdm, 0, sizeof(_soc_td2p_ovr_sub_tdm_t));

    /* Recover the Over_Sub PGW and MMU TDM configuration from registers.
     * Construct software database.
     */
    rv = soc_td2p_ovr_sub_tdm_config_get(unit, ovr_sub_tdm);
    if (SOC_FAILURE(rv)) {
        sal_free(ovr_sub_tdm);
        return rv;
    }

    /* Update the Pre OBM TDM configuration
     * Free's the pre-occupied slots*/
    rv = soc_td2p_ovr_sub_tdm_db_update(unit, pre_num_res, pre_res, ovr_sub_tdm,
                                              _SOC_TRIDENT2P_UPDATE_TDM_PRE);
    if (SOC_FAILURE(rv)) {
        sal_free(ovr_sub_tdm);
        return rv;
    }

    /* Update the Post OBM TDM configuration
     * Updates the Empty slots*/
    rv = soc_td2p_ovr_sub_tdm_db_update(unit, post_num_res, post_res, ovr_sub_tdm,
                                              _SOC_TRIDENT2P_UPDATE_TDM_POST);
    if (SOC_FAILURE(rv)) {
        sal_free(ovr_sub_tdm);
        return rv;
    }

    /* Updates the OBM TDM configuration in the Hardware */
    rv = soc_td2p_ovr_sub_tdm_hw_update(unit, ovr_sub_tdm);
    if (SOC_FAILURE(rv)) {
        sal_free(ovr_sub_tdm);
        return rv;
    }

    sal_free(ovr_sub_tdm);
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_egr_edb_xmit_ctrl_set
 *
 * Description:
 *      Program EGR_EDB_XMIT_CTRL memory by considering
 *      Oversub/Linerate mode and Cut-Through/SAF mode
 *
 * Parameters:
 *      unit            - Device number
 *      phy_port        - Physical port
 *      speed           - Port speed
 *      oversub         - Oversub is enabled or not
 *      cut_thru_enable - Cut through mode is enabled or not
 */
int soc_td2p_egr_edb_xmit_ctrl_set(int unit, int phy_port, int speed,
                                   int oversub, int cut_thru_enable)
{
    soc_info_t *si;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t reg_field;
    int         start_cnt_val = 0;

    si = &SOC_INFO(unit);
    sal_memset(entry, 0, sizeof(egr_edb_xmit_ctrl_entry_t));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_EDB_XMIT_CTRLm, MEM_BLOCK_ALL,
                                     phy_port, &entry));
    if (!cut_thru_enable) {
        if (oversub) {
            reg_field = WAIT_FOR_2ND_MOPf;
        } else {
            reg_field = WAIT_FOR_MOPf;
        }
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, START_CNTf, 0);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, reg_field, 1);
    } else {
        if (oversub) {
            start_cnt_val = _TD2_MMU_XMIT_START_CNT_OVERSUB;
        } else {
            start_cnt_val = _TD2_MMU_XMIT_START_CNT_LINERATE(speed);
        }
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, START_CNTf,
                            start_cnt_val);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, WAIT_FOR_MOPf, 0);
        soc_mem_field32_set(unit, EGR_EDB_XMIT_CTRLm, entry, WAIT_FOR_2ND_MOPf,
                            0);
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_EDB_XMIT_CTRLm,
                                      MEM_BLOCK_ALL,
                                      phy_port, &entry));
    return SOC_E_NONE;
}

/*
 * This function is used to enable or disable cut through when a port is 
 * operating in store and forward mode. 
 * Caller must pass the correct port_info pointer, corresponding to the port
 * being configured (and sent by the BCM layer)
 * To enable cut-through, pass enable = 1
 * To disable cut-through, pass enable = 0
 */
STATIC int 
soc_td2p_cut_thru_enable_disable(int unit,
                                            soc_port_resource_t *port_info,
                                            int enable)
{
    uint32 rval;
    uint32 speed;
    int use_pfc_optimized_settings = FALSE;
    egr_pfc_control_entry_t pfc_control;

    /* Mask unnecessary bits */
    enable &= 0x1;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port_info->logical_port,
                        &rval));

    speed = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, ASF_PORT_SPEEDf);

    /* Set port speed to 0 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                        rval));

    if (!enable) {
        /* Wait 8ms for draining all cells */
        sal_usleep(8000);
    }

    SOC_IF_ERROR_RETURN(soc_td2p_egr_edb_xmit_ctrl_set(
                            unit, port_info->physical_port, port_info->speed,
                            port_info->oversub, enable));

    /* Set UC_ASF_ENABLE, MC_ASF_ENABLE bits to 1 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, UC_ASF_ENABLEf, enable);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, MC_ASF_ENABLEf, enable);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                                            rval));

    if (enable) {
        /* Wait 1 micro-sec */
        sal_usleep(1);
    }

    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, speed);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port_info->logical_port,
                                            rval));

    /* If system is able to use optimized settings,
       disable EDB watermark to avoid credit underrun.
       MMU mechanism will be used to optimize PFC response. */
    use_pfc_optimized_settings = soc_td2p_use_pfc_optimized_settings(unit);
    if (use_pfc_optimized_settings) {
        SOC_IF_ERROR_RETURN(READ_EGR_PFC_CONTROLm(unit, MEM_BLOCK_ALL,
            port_info->physical_port, &pfc_control));
        soc_mem_field32_set(unit, EGR_PFC_CONTROLm, &pfc_control,
            ENABLEf, enable ? 0 : 1);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PFC_CONTROLm(unit, MEM_BLOCK_ALL,
            port_info->physical_port, &pfc_control));
    }

    return SOC_E_NONE;
}

int soc_td2p_max_speed_port_find(int phy_port, 
                                        soc_port_resource_t *port_info,
                                        int port_info_arr_size) 
{
    int idx, index, speed;
  
    index = 0;
    speed = port_info[index].speed;

    for (idx = 1; idx < port_info_arr_size; idx++) {
        /* If all speeds are equal, '>' will return the 1st index,
         * '>=' will return the last index
         */
        if (port_info[idx].speed > speed) {
            speed = port_info[idx].speed;
            index = idx;
        }
    }

    return index;
}

/* This function fetches the current egress scheduler's TDM table number being
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_egr_tdm_table_get(int unit, int pipe,
                                           uint32 *table_num)
{
    soc_reg_t reg;
    uint32 rval;

    /* Return value of CURR_CALf in table_num */

    reg = (pipe == 0) ? ES_PIPE0_TDM_CONFIGr : ES_PIPE1_TDM_CONFIGr;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    *table_num = soc_reg_field_get(unit, reg, rval, CURR_CALf);

    return SOC_E_NONE;
}

/* This function fetches the current ingress scheduler's TDM table number being
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_ingr_tdm_table_get(int unit, int pgw_inst,
                                            uint32 *table_num)
{
    uint64 rval64;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                                    &rval64));

    /* Return value of LR_TDM_SEL in table_num */
    *table_num = soc_reg64_field32_get(unit, PGW_TDM_CONTROLr, rval64, 
                                       LR_TDM_SELf);

    return SOC_E_NONE;
}

/* This function sets the new egress scheduler's TDM table number which will be
 * used by the hardware. This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_egr_tdm_table_set(int unit, int pipe, 
                                            uint32 table_num)
{
    uint32 rval;
    soc_reg_t reg;

    reg = (pipe == 0) ? ES_PIPE0_TDM_CONFIGr : ES_PIPE1_TDM_CONFIGr;

    SOC_IF_ERROR_RETURN (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    /* Mask unwanted bits */
    table_num &= 0x1;

    /* Set CURR_CALf */
    soc_reg_field_set(unit, reg, &rval, CURR_CALf, table_num);

    SOC_IF_ERROR_RETURN (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/* This function fetches the current ingress scheduler's TDM table number being
 * used by the hardware.  This function will be used during flexing, but its use
 * is not restricted to flexing operations.
 */
STATIC int soc_td2p_curr_ingr_tdm_table_set(int unit, int pgw_inst,
                                            int table_num)
{
    uint64 rval64;

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                                    &rval64));

    /* Mask unwanted bits */
    table_num &= 0x1;

    /* Set LR_TDM_SEL */
    soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &rval64, LR_TDM_SELf,
                          table_num);

    SOC_IF_ERROR_RETURN (soc_reg_set(unit, PGW_TDM_CONTROLr, pgw_inst, 0,
                         rval64));

    return SOC_E_NONE;
}

/*
 * This function checks if cut-through is enabled for a given port.
 * If either unicast or multicast cut-through is enabled, the function
 * returns TRUE else FALSE is returned
 */
int soc_td2p_is_cut_thru_enabled(int unit, soc_port_t port, int *enable)
{
    uint32 rval;
    uint32 uc_enable, mc_enable;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));

    uc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, UC_ASF_ENABLEf);
    mc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, MC_ASF_ENABLEf);

    /* This is for ensuring that the behavior of cut-through setting get is
     * same as in _bcm_td2_port_asf_enable_get called by application
     * (upper layer software), through bcm_esw_switch_control_port_get()
     */
    if (uc_enable & mc_enable) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * This function is used to clear various counters in EP block. The 
 * counters to clear are mentioned in 'Reconfigure EP' section of TD2+ Flexport
 */
STATIC int soc_td2p_ep_counters_clear(int unit, soc_port_resource_t *port_info)
{
    uint64 val64;
    uint32 val = 0;

    COMPILER_64_ZERO(val64);
    SOC_IF_ERROR_RETURN(WRITE_EGR_TRILL_TX_PKTSr(unit, port_info->logical_port,
                        val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_TRILL_TX_ACCESS_PORT_TRILL_PKTS_DISCARDEDr(unit, port_info->logical_port, val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_DELIMITER_ERROR_FRAMESr(unit,
                        port_info->logical_port, val));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_INVALID_CRC_FRAMESr(unit, 
                        port_info->logical_port, val));

    SOC_IF_ERROR_RETURN(WRITE_TPCEr(unit, port_info->logical_port, val64));

    SOC_IF_ERROR_RETURN(WRITE_TDBGC0r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC1r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC2r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC3r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC4r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC5r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC6r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC7r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC8r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC9r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC10r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC11r(unit, port_info->logical_port, val64));

    return SOC_E_NONE;
}

STATIC int soc_td2p_port_resource_tdm_calculate(int unit, 
                                            int curr_port_info_size,
                                            soc_port_resource_t *curr_port_info,
                                            int new_port_info_size,
                                            soc_port_resource_t *new_port_info,
                                            soc_td2p_info_t *si_info)
{
#ifdef BCM_56860_A0
    int rv, i;
    int num_of_subport;
    soc_info_t *si;
    _soc_trident2_tdm_t *tdm;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    int phy_port;
    int port;
    int speed_max, index, slot;
    soc_pbmp_t pbmp;

    /* soc info has new values */
    si = &SOC_INFO(unit);

    tdm = soc_td2_td2p_tdm_sched_info_get(unit, 1);
    if (tdm == NULL) {
        return SOC_E_MEMORY;
    }

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }

        /* Since soc info structure has been updated before this function is
         * called, we use the information provided by it
         */
        phy_port = si->port_l2p_mapping[port];

        tdm->speed[phy_port] = si->port_speed_max[port];
        if (si->port_speed_max[port] < 1000) {
            tdm->speed[phy_port] = 1000;
        }
        tdm->port_state[phy_port] = SOC_PBMP_MEMBER(si->oversub_pbm, port) ?
                                                    PORT_STATE_OVERSUB : 
                                                    PORT_STATE_LINERATE;

        num_of_subport = 0;
        if (tdm->speed[phy_port] >= 120000) {
            num_of_subport = 11;
        } else if (tdm->speed[phy_port] >= 100000) {
            num_of_subport = 9;
        } else if (tdm->speed[phy_port] >= 40000) {
            num_of_subport = 3;
        } else if (tdm->speed[phy_port] >= 20000) {
            num_of_subport = 1;
        }
        for (i = 0; i < num_of_subport; i++) {
            tdm->port_state[phy_port + i + 1] = PORT_STATE_SUBPORT;
        }
    }

    tdm->speed[0] = 1000;
    tdm->speed[129] = 1000;
    tdm->tdm_bw = si->bandwidth / 1000;

    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->xpipe_pbm);
    if (tdm->tdm_bw == 720) { /* 720g special handling */
        /* tell tdm code the pipe is oversub only if all ports are oversub */
        SOC_PBMP_PORT_ADD(pbmp, 0); /* cpu port */
        tdm->pipe_ovs_state[0] = SOC_PBMP_EQ(pbmp, si->xpipe_pbm);
    } else {
        /* tell tdm code the pipe is oversub if any ports is oversub */
        tdm->pipe_ovs_state[0] = SOC_PBMP_NOT_NULL(pbmp);
    }
    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->ypipe_pbm);
    if (tdm->tdm_bw == 720) { /* 720g special handling */
        /* tell tdm code the pipe is oversub only if all ports are oversub */
        SOC_PBMP_PORT_ADD(pbmp, 105); /* loopback port */
        tdm->pipe_ovs_state[1] = SOC_PBMP_EQ(pbmp, si->ypipe_pbm);
    } else {
        /* tell tdm code the pipe is oversub if any ports is oversub */
        tdm->pipe_ovs_state[1] = SOC_PBMP_NOT_NULL(pbmp);
    }

    tdm->manage_port_type = 0;
    if (SOC_PBMP_NOT_NULL(si->management_pbm)) {
        speed_max = 0;
        for (phy_port = 13; phy_port <= 16; phy_port++) {
            port = si->port_p2l_mapping[phy_port];
            if (port == -1) {
                continue;
            }
            if (speed_max < si->port_speed_max[port]) {
                speed_max = si->port_speed_max[port];
            }
        }
        if (speed_max <= 1000) {
            tdm->manage_port_type = 1; /* 4 x 1g */
        } else if (speed_max <= 2500) {
            tdm->manage_port_type = 2; /* 4 * 2.5g */
        } else {
            tdm->manage_port_type = 3; /* 1 x 10g */
        }
        if (tdm->manage_port_type > 0) {
            tdm->pgw_tdm[0][0] = 1234; /* magic number used by tdm code */
        }
    }
    if (tdm->manage_port_type != 0) {
        _chip_pkg.soc_vars.td2p.tdm_mgmt_en = 1;
    } else {
        _chip_pkg.soc_vars.td2p.tdm_mgmt_en = 0;
    }

    for (slot = 0; slot <= _MMU_TDM_LENGTH; slot++) {
        tdm->mmu_tdm[0][slot] = NUM_EXT_PORTS;
        tdm->mmu_tdm[1][slot] = NUM_EXT_PORTS;
    }

    /* Configure ingress scheduler back up calendar */
    _chip_pkg.unit = unit;
    _chip_pkg.num_ext_ports = TD2P_NUM_EXT_PORTS;
    _chip_pkg.state=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *), "port state list");
    _chip_pkg.speed=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *), "port speed list");
    for (index=1; index<(_chip_pkg.num_ext_ports); index++) {
        _chip_pkg.state[index] = tdm->port_state[index];
    }
    _chip_pkg.state[0]=1; /* enable cpu port */
    _chip_pkg.state[_chip_pkg.num_ext_ports-1]=1; /* enable loopback port */

    /* This pointer is used internally by set tdm table API, and is
     * initialized in tdm_td2p_init (src/soc/esw/tdm/set_tdm.c).
     * _soc_set_tdm_tbl calls functions in the array of function pointers
     * (_chip_exec)
     * This pointer is not used by SDK code.
     */
    /* coverity[uninit_use_in_call] */
    _chip_pkg.pmap = NULL;
    for (index=0; index<(_chip_pkg.num_ext_ports); index++) {
        _chip_pkg.speed[index] = tdm->speed[index];
    }

    /*
     * Map detected core clk frequency to TDM algorithm internal code
     * value.
     */
    switch (si->frequency) {
           case 790:
           case 760:
               _chip_pkg.clk_freq = 760;
               break;
           case 635:
               _chip_pkg.clk_freq = 608;
               break;
           case 537:
               _chip_pkg.clk_freq = 518;
               break;
           case 421:
               _chip_pkg.clk_freq = 415;
               break;
           default:
               _chip_pkg.clk_freq = (si->frequency > 760) ? (760)
                                       : si->frequency;
    }

    /* This function initializes only the fields which the caller of
     * _soc_set_tdm_tbl should fill. The remaining fields of _chip_pkg
     * structure are initialized and used internally within _soc_set_tdm_tbl
     * (and other tdm library functions)
     */
    /* coverity[uninit_use_in_call] */
    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));
    if (!_tdm_pkg) {
        LOG_CLI((BSL_META_U(unit,
                "Unsupported config for TDM calendar generation\n")));
        return SOC_E_FAIL;
    }

    sal_memcpy(tdm->pgw_tdm[0], _tdm_pkg->_chip_data.cal_0.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[0], _tdm_pkg->_chip_data.cal_0.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[0], _tdm_pkg->_chip_data.cal_0.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[1], _tdm_pkg->_chip_data.cal_1.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[1], _tdm_pkg->_chip_data.cal_1.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[1], _tdm_pkg->_chip_data.cal_1.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[2], _tdm_pkg->_chip_data.cal_2.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[2], _tdm_pkg->_chip_data.cal_2.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[2], _tdm_pkg->_chip_data.cal_2.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_tdm[3], _tdm_pkg->_chip_data.cal_3.cal_main, sizeof(int)*MAX_PGW_TDM_LENGTH);
    sal_memcpy(tdm->pgw_ovs_tdm[3], _tdm_pkg->_chip_data.cal_3.cal_grp[0], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->pgw_ovs_spacing[3], _tdm_pkg->_chip_data.cal_3.cal_grp[1], sizeof(int)*_PGW_TDM_OVS_SIZE);
    sal_memcpy(tdm->mmu_tdm[0], _tdm_pkg->_chip_data.cal_4.cal_main, sizeof(int)*_MMU_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][0], _tdm_pkg->_chip_data.cal_4.cal_grp[0], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][1], _tdm_pkg->_chip_data.cal_4.cal_grp[1], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][2], _tdm_pkg->_chip_data.cal_4.cal_grp[2], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][3], _tdm_pkg->_chip_data.cal_4.cal_grp[3], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][4], _tdm_pkg->_chip_data.cal_4.cal_grp[4], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][5], _tdm_pkg->_chip_data.cal_4.cal_grp[5], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][6], _tdm_pkg->_chip_data.cal_4.cal_grp[6], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[0][7], _tdm_pkg->_chip_data.cal_4.cal_grp[7], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_tdm[1], _tdm_pkg->_chip_data.cal_5.cal_main, sizeof(int)*_MMU_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][0], _tdm_pkg->_chip_data.cal_5.cal_grp[0], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][1], _tdm_pkg->_chip_data.cal_5.cal_grp[1], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][2], _tdm_pkg->_chip_data.cal_5.cal_grp[2], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][3], _tdm_pkg->_chip_data.cal_5.cal_grp[3], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][4], _tdm_pkg->_chip_data.cal_5.cal_grp[4], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][5], _tdm_pkg->_chip_data.cal_5.cal_grp[5], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][6], _tdm_pkg->_chip_data.cal_5.cal_grp[6], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(tdm->mmu_ovs_group_tdm[1][7], _tdm_pkg->_chip_data.cal_5.cal_grp[7], sizeof(int)*_MMU_OVS_GROUP_TDM_LENGTH);
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);
    _tdm_pkg->_chip_exec[TDM_CHIP_EXEC__FREE](_tdm_pkg);
    sal_free(_tdm_pkg);

    rv = tdm_td2p_set_iarb_tdm_table(tdm->tdm_bw,
                                     tdm->pipe_ovs_state[0],
                                     tdm->pipe_ovs_state[1],
                                     tdm->manage_port_type == 1,
                                     tdm->manage_port_type == 2,
                                     tdm->manage_port_type == 3,
                                     &tdm->iarb_tdm_wrap_ptr[0],
                                     &tdm->iarb_tdm_wrap_ptr[1],
                                     tdm->iarb_tdm[0],
                                     tdm->iarb_tdm[1]);

    if (rv == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "Unsupported config for TDM calendar generation\n")));
        return SOC_E_FAIL;
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_use_pfc_optimized_settings
 * Purpose:
 *      Determines whether to use pfc optimized settings or not.
 *      PFC Optimized Settings can only be used at max clock frequency,
 *      which enables bandwidth of 960G. Its use is also dependent on
 *      the oversub ratio, which must be < 1.5. Configurations that are
 *      all linerate or all oversub do not exceed this threshold. However,
 *      mixed configurations of linerate and oversubbed ports can exceed
 *      1.5, so the oversub ratio is calculated at the bottom of the
 *      function.
 *  
 * Parameters:
 *      unit     - (IN) Unit number.
 *  
 * Returns:
 *      TRUE or FALSE
 */
int
soc_td2p_use_pfc_optimized_settings(int unit)
{
   soc_info_t *si;
   soc_pbmp_t pbmp;

   int x_denominator = 0, y_denominator = 0;

   int all_x_oversub = 0, any_x_oversub = 0;
   int all_y_oversub = 0, any_y_oversub = 0;

   int x_linerate_bw = 0, x_oversub_bw = 0;
   int y_linerate_bw = 0, y_oversub_bw = 0;
   int x_core_bw = 0, y_core_bw = 0;
   int max_quad_core_bandwidth, quad_linerate_bandwidth, quad_oversub_bandwidth;

   /* soc info has new values */
   si = &SOC_INFO(unit);
   /* if not max frequency, use non-optimized settings */
   if (si->bandwidth < 960000) {
      return FALSE;
   }

   SOC_PBMP_CLEAR(pbmp);
   SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
   SOC_PBMP_AND(pbmp, si->xpipe_pbm);

   /* find if any X ports are oversub */
   any_x_oversub = SOC_PBMP_NOT_NULL(pbmp);

   /* find if all X ports are oversub */
   SOC_PBMP_PORT_ADD(pbmp, 0); /* cpu port */
   all_x_oversub = SOC_PBMP_EQ(pbmp, si->xpipe_pbm);



   SOC_PBMP_CLEAR(pbmp);
   SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
   SOC_PBMP_AND(pbmp, si->ypipe_pbm);

   /* find if any Y ports are oversub */
   any_y_oversub = SOC_PBMP_NOT_NULL(pbmp);

   /* find if all Y ports are oversub */
   SOC_PBMP_PORT_ADD(pbmp, 105); /* loopback port */
   all_y_oversub = SOC_PBMP_EQ(pbmp, si->ypipe_pbm);

   /* if all ports are oversub, use pfc optimized settings;
      oversub ratio < 1.5 */
   if (all_x_oversub == 1 && all_y_oversub == 1) {
      return TRUE;
   }

   /* if all the ports are linerate, use pfc optimized settings;
      oversub ratio < 1.5 */
   if (any_x_oversub == 0 && any_y_oversub == 0) {
      return TRUE;
   }

   /* in the mixed line rate/oversub case, we need to calculate
      the oversub ratio:
    
      oversubbed port bandwidth / remaining bw after guaranteed linerate
    
      Example: if 2 100G line rate ports, subtract 200 from core bandwidth
      of 480 (per pipe) because that's guaranteed, so you're left with 280.
    
      Now if you have 8 40G ports, that's 320G of bandwidth, so you MUST
      use oversub for those since you only have 280G left.
    
      Thus your Oversub ratio is 320/280.  This value MUST be < 1.5 in order
      to use PFC Optimized Settings.
      */

   /* Calculate total linerate and oversub bandwidth for X Pipe */
   _soc_trident2_quad_bandwidth_calculate(unit, 0, &max_quad_core_bandwidth, 
                                      &quad_linerate_bandwidth,
                                      &quad_oversub_bandwidth);
   x_core_bw     += max_quad_core_bandwidth;
   x_linerate_bw += quad_linerate_bandwidth;
   x_oversub_bw  += quad_oversub_bandwidth;

   _soc_trident2_quad_bandwidth_calculate(unit, 1, &max_quad_core_bandwidth, 
                                      &quad_linerate_bandwidth,
                                      &quad_oversub_bandwidth);
   x_core_bw     += max_quad_core_bandwidth;
   x_linerate_bw += quad_linerate_bandwidth;
   x_oversub_bw  += quad_oversub_bandwidth;

   /* Calculate total linerate and oversub bandwidth for Y Pipe */
   _soc_trident2_quad_bandwidth_calculate(unit, 2, &max_quad_core_bandwidth, 
                                      &quad_linerate_bandwidth, 
                                      &quad_oversub_bandwidth);
   y_core_bw     += max_quad_core_bandwidth;
   y_linerate_bw += quad_linerate_bandwidth;
   y_oversub_bw  += quad_oversub_bandwidth;

   _soc_trident2_quad_bandwidth_calculate(unit, 3, &max_quad_core_bandwidth,
                                      &quad_linerate_bandwidth,
                                      &quad_oversub_bandwidth);
   y_core_bw     += max_quad_core_bandwidth;
   y_linerate_bw += quad_linerate_bandwidth;
   y_oversub_bw  += quad_oversub_bandwidth;

   /* Calculate Oversub Ratio per pipe. Oversub ratio must be < 1.5.
      Since we can't use floats, the math
      used here is to multiply the numerator by 2 and the denominator by 3
      and compare the results.
      numerator = oversub bandwidth
      denominator = core bandwidth - linerate bandwidth
      */
   x_denominator = (x_core_bw - x_linerate_bw);
   y_denominator = (y_core_bw - y_linerate_bw);

   /* If either pipe has an oversub ratio > 1.5, use normal settings. */
   if  ((2 * x_oversub_bw) >= (3 * x_denominator) ||
        (2 * y_oversub_bw) >= (3 * y_denominator)) {
      return FALSE;
   }

   /* Use PFC Optimized Settings */
   return TRUE;
}



/*
 * Function:
 *      soc_td2p_set_mmu_credit_limit
 * Purpose:
 *      Programs egress port credit size depending on port speed and whether
 *      oversub is enabled.  Programs it for the worst case scenario.
 *  
 *      EP credit sizing is used to reduce the usable size of Edatabuf (EDB)
 *      to limit accumulation.
 *  
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical Port
 *  
 * Returns:
 *      SOC_E_XXX
 */
int 
soc_td2p_set_mmu_credit_limit(int unit, soc_port_t port)
{
   uint32 enable = 1;
   uint32 credit_limit = 0;
   uint32 entry[SOC_MAX_MEM_WORDS];
   int phy_port, speed_max, hw_index;
   soc_mem_t mem;
   soc_info_t *si = &SOC_INFO(unit);

   phy_port = si->port_l2p_mapping[port];
   if (phy_port == -1) {
       return SOC_E_INTERNAL;
   }

   SOC_IF_ERROR_RETURN(
      READ_EGR_MMU_CREDIT_LIMITm(unit, MEM_BLOCK_ALL, phy_port, entry));

   /* Get the maximum speed of the port */
   speed_max = si->port_speed_max[port];

   /* There are different credit limits depending on if
      oversub is enabled, and the port speed. */
   if (IS_OVERSUB_PORT(unit, port)) {
      if (speed_max >= 100000) {
         return SOC_E_PARAM;
      } else if (speed_max >= 40000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(40G);
      } else if (speed_max >= 20000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(20G);
      } else if (speed_max >= 10000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_OVERSUB(10G);
      } else {
         return SOC_E_NONE;
      }
   } else {
      if (speed_max >= 100000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(100G);
      } else if (speed_max >= 40000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(40G);
      } else if (speed_max >= 20000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(20G);
      } else if (speed_max >= 10000) {
         credit_limit = _TD2P_MMU_EGR_MMU_CREDIT_LIMIT_LINERATE(10G);
      } else {
         return SOC_E_NONE;
      }
   }

    mem = SOC_TD2_PMEM(unit, port, EGR_MMU_CREDIT_LIMIT_Xm, EGR_MMU_CREDIT_LIMIT_Ym);
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }

    hw_index = phy_port;
    SOC_IF_ERROR_RETURN
             (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));

    soc_mem_field32_set(unit, mem, entry, ENABLEf, enable);
    soc_mem_field32_set(unit, mem, entry, CELL_COUNTSf, credit_limit);

    SOC_IF_ERROR_RETURN
             (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, entry));


   return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_set_edb_pfc_watermark_threshold
 * Purpose:
 *      Programs EDB PFC Watermark Threshold depending on port speed and whether
 *      oversub is enabled. Programs it for the worst case scenario.
 *  
 *      PFC Watermark in EDB stalls EP credit return to the MMU in order
 *      to limit outstanding credits.
 *  
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical Port
 *  
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_set_edb_pfc_watermark_threshold(int unit, soc_port_t port)
{

   uint32 enable = 1;
   uint32 pfc_watermark_thresh = 0;
   uint32 entry[SOC_MAX_MEM_WORDS];
   int phy_port, speed_max, ct_enable = 0;
   soc_info_t *si = &SOC_INFO(unit);

   phy_port = si->port_l2p_mapping[port];
   if (phy_port == -1) {
       return SOC_E_INTERNAL;
   }

   SOC_IF_ERROR_RETURN(
      READ_EGR_PFC_CONTROLm(unit, MEM_BLOCK_ALL, phy_port, entry));

   /* Get the maximum speed of the port */
   speed_max = si->port_speed_max[port];

   /* There are different watermarks depending on if
      oversub is enabled, and the port speed. */
      /* cut through is not supported */
   if (IS_OVERSUB_PORT(unit, port)) {
      if (speed_max >= 100000) {
         return SOC_E_PARAM;
      } else if (speed_max >= 40000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB(40G);
      } else if (speed_max >= 20000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB(20G);
      } else if (speed_max >= 10000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_OVERSUB(10G);
      } else {
         return SOC_E_NONE;
      }
   } else {
      if (speed_max >= 100000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE(100G);
      } else if (speed_max >= 40000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE(40G);
      } else if (speed_max >= 20000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE(20G);
      } else if (speed_max >= 10000) {
         pfc_watermark_thresh = _TD2P_EGR_PFC_WATERMARK_THRESH_LINERATE(10G);
      } else {
         return SOC_E_NONE;
      }
   }

   SOC_IF_ERROR_RETURN(
      soc_td2p_is_cut_thru_enabled(unit, port, &ct_enable));
   if (ct_enable) {
      enable = 0;
   }
   /* modify ENABLE and CELL_COUNTS in the EGR_MMU_CREDIT_LIMIT */
   soc_EGR_PFC_CONTROLm_field_set(unit, entry, ENABLEf, &enable);
   soc_EGR_PFC_CONTROLm_field_set(unit, entry, WATERMARKf, &pfc_watermark_thresh);   

   SOC_IF_ERROR_RETURN(
      WRITE_EGR_PFC_CONTROLm(unit, MEM_BLOCK_ALL, phy_port, entry));

   return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_tdm_set
 * Purpose:
 *      Reconfigure the ingress and egress schedulers for
 *      the new port configuration.
 *
 *      This includes reconfiguration of:
 *      - OBM Thresholds
 *      - EDB Prebuffer
 *      - Ingress Oversub Scheduler
 *      - Egress Oversub Scheduler
 *      - Line-Rate Scheduler
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      curr_port_info_size  - (IN) Size of current port information array
 *      curr_port_info       - (IN) Per-port array with current flexing info
 *      new_port_info_size   - (IN) Size of new port information array
 *      new_port_info        - (IN) Per-port array with new flexing info
 *      si_info              - (IN) Contains subset of fields from
 *                             soc_info_t structure
 *                             prior to the flexing operation
 *      lossless             - (IN) MMU lossless information
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, that is, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_tdm_set(int unit, int curr_port_info_size,
                                   soc_port_resource_t *curr_port_info,
                                   int new_port_info_size,
                                   soc_port_resource_t *new_port_info,
                                   soc_td2p_info_t *si_info,
                                   int lossless)
{
    soc_info_t *si;
    _soc_trident2_tdm_t *tdm;
    int pipe;
    int phy_port;
    int count;
    uint32 table_num = 0;
    uint64 rval64, ctrl_rval64;
    uint32 rval;
    int idx, mmu_port, length, profile_x, profile_y, port;
    int  index, pgw, pgw_master, pgw_slave, obm_inst, base, slot;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_reg_t reg;
    soc_field_t fields[4];
    uint32 values[4];
    soc_pbmp_t pbmp;
    /* Used to store per-port cut-through setting */
    int cut_thru_en[SOC_MAX_NUM_PORTS];
    soc_port_resource_t res;

    /* Egress scheduler calendar memories */
    static const soc_mem_t mmu_tdm_mems_0[2] ={
        ES_PIPE0_TDM_TABLE_0m, ES_PIPE1_TDM_TABLE_0m
    };

    static const soc_mem_t mmu_tdm_mems_1[2] ={
        ES_PIPE0_TDM_TABLE_1m, ES_PIPE1_TDM_TABLE_1m
    };

    static const soc_mem_t mmu_tdm_regs[2] ={
        ES_PIPE0_TDM_CONFIGr, ES_PIPE1_TDM_CONFIGr
    };

    static const soc_mem_t iarb_tdm_mems[2] ={
        IARB_MAIN_TDM_Xm, IARB_MAIN_TDM_Ym
    };

    static soc_reg_t pgw_tdm_regs[] = {
        PGW_LR_TDM_REG_0r, PGW_LR_TDM_REG_1r,
        PGW_LR_TDM_REG_2r, PGW_LR_TDM_REG_3r,
        PGW_LR_TDM_REG_4r, PGW_LR_TDM_REG_5r,
        PGW_LR_TDM_REG_6r, PGW_LR_TDM_REG_7r,
        PGW_LR_TDM_REG_8r,  PGW_LR_TDM_REG_9r,
        PGW_LR_TDM_REG_10r, PGW_LR_TDM_REG_11r,
        PGW_LR_TDM_REG_12r, PGW_LR_TDM_REG_13r,
        PGW_LR_TDM_REG_14r, PGW_LR_TDM_REG_15r
    };

    /* Used for programming back up calender */
    static soc_reg_t pgw_tdm2_regs[] = {
        PGW_LR_TDM2_REG_0r, PGW_LR_TDM2_REG_1r,
        PGW_LR_TDM2_REG_2r, PGW_LR_TDM2_REG_3r,
        PGW_LR_TDM2_REG_4r, PGW_LR_TDM2_REG_5r,
        PGW_LR_TDM2_REG_6r, PGW_LR_TDM2_REG_7r,
        PGW_LR_TDM2_REG_8r,  PGW_LR_TDM2_REG_9r,
        PGW_LR_TDM2_REG_10r, PGW_LR_TDM2_REG_11r,
        PGW_LR_TDM2_REG_12r, PGW_LR_TDM2_REG_13r,
        PGW_LR_TDM2_REG_14r, PGW_LR_TDM2_REG_15r
    };

    static const soc_field_t pgw_tdm_fields[] = {
        TDM_ENTRY0_PORT_IDf, TDM_ENTRY1_PORT_IDf,
        TDM_ENTRY2_PORT_IDf, TDM_ENTRY3_PORT_IDf,
        TDM_ENTRY4_PORT_IDf, TDM_ENTRY5_PORT_IDf,
        TDM_ENTRY6_PORT_IDf, TDM_ENTRY7_PORT_IDf,
        TDM_ENTRY8_PORT_IDf, TDM_ENTRY9_PORT_IDf,
        TDM_ENTRY10_PORT_IDf, TDM_ENTRY11_PORT_IDf,
        TDM_ENTRY12_PORT_IDf, TDM_ENTRY13_PORT_IDf,
        TDM_ENTRY14_PORT_IDf, TDM_ENTRY15_PORT_IDf,
        TDM_ENTRY16_PORT_IDf, TDM_ENTRY17_PORT_IDf,
        TDM_ENTRY18_PORT_IDf, TDM_ENTRY19_PORT_IDf,
        TDM_ENTRY20_PORT_IDf, TDM_ENTRY21_PORT_IDf,
        TDM_ENTRY22_PORT_IDf, TDM_ENTRY23_PORT_IDf,
        TDM_ENTRY24_PORT_IDf, TDM_ENTRY25_PORT_IDf,
        TDM_ENTRY26_PORT_IDf, TDM_ENTRY27_PORT_IDf,
        TDM_ENTRY28_PORT_IDf, TDM_ENTRY29_PORT_IDf,
        TDM_ENTRY30_PORT_IDf, TDM_ENTRY31_PORT_IDf,
        TDM_ENTRY32_PORT_IDf, TDM_ENTRY33_PORT_IDf,
        TDM_ENTRY34_PORT_IDf, TDM_ENTRY35_PORT_IDf,
        TDM_ENTRY36_PORT_IDf, TDM_ENTRY37_PORT_IDf,
        TDM_ENTRY38_PORT_IDf, TDM_ENTRY39_PORT_IDf,
        TDM_ENTRY40_PORT_IDf, TDM_ENTRY41_PORT_IDf,
        TDM_ENTRY42_PORT_IDf, TDM_ENTRY43_PORT_IDf,
        TDM_ENTRY44_PORT_IDf, TDM_ENTRY45_PORT_IDf,
        TDM_ENTRY46_PORT_IDf, TDM_ENTRY47_PORT_IDf,
        TDM_ENTRY48_PORT_IDf, TDM_ENTRY49_PORT_IDf,
        TDM_ENTRY50_PORT_IDf, TDM_ENTRY51_PORT_IDf,
        TDM_ENTRY52_PORT_IDf, TDM_ENTRY53_PORT_IDf,
        TDM_ENTRY54_PORT_IDf, TDM_ENTRY55_PORT_IDf,
        TDM_ENTRY56_PORT_IDf, TDM_ENTRY57_PORT_IDf,
        TDM_ENTRY58_PORT_IDf, TDM_ENTRY59_PORT_IDf,
        TDM_ENTRY60_PORT_IDf, TDM_ENTRY61_PORT_IDf,
        TDM_ENTRY62_PORT_IDf, TDM_ENTRY63_PORT_IDf
    };

    if ((!SOC_IS_TRIDENT2PLUS(unit) && !SOC_IS_TITAN2PLUS(unit))) {
        /* Do nothing */
        return SOC_E_NONE;
    }

    /* Reconfigure EDB prebuffer */

    /* Note both pipelines' registers are written by hardware internally */
    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));

    /* Reconfigure EDB prebuffer back up calendar, for line rate
     * TDM calendar. Oversub related portion will be added later
     */
    SOC_IF_ERROR_RETURN(soc_td2p_port_resource_tdm_calculate(unit,
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info, si_info));

    /* Reconfigure oversub scheduler.
     * Function internally determines if oversub is enabled/disabled
     */
    SOC_IF_ERROR_RETURN(soc_td2p_ovr_sub_tdm_update(unit, 
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info));

    tdm = soc_td2_td2p_tdm_sched_info_get(unit, 0);
    if (tdm == NULL) {
        return SOC_E_INIT;
    }

    /* soc info has new values */
    si = &SOC_INFO(unit);

    
    /* Configure ingress scheduler back up calendar */
    for (pgw = 0; pgw < _TD2_PGWS_PER_DEV; pgw += _TD2_PGWS_PER_QUAD) {
        pgw_master = pgw;
        pgw_slave  = pgw + 1;
        if ((pgw == 2) || (pgw == 6)) {
            pgw_master = pgw + 1;
            pgw_slave  = pgw;
        }

        /* coverity[negative_returns : FALSE] */
        if (!si->block_valid[PGW_CL_BLOCK(unit, pgw_master)] &&
            !si->block_valid[PGW_CL_BLOCK(unit, pgw_slave)]) {
            /* Both master and slave are not in use */
            continue;
        }

        obm_inst = pgw_master | SOC_REG_ADDR_INSTANCE_MASK;

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, PGW_TDM_CONTROLr, obm_inst, 0,
                                        &ctrl_rval64));

        SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_get(unit,
                                                            obm_inst,
                                                            &table_num)); 
        /* Configure PGW line rate TDM backup calendar */
        count = 0;
        for (base = 0; base < GET_PGW_TDM_LENGTH(unit);
            base += _PGW_TDM_SLOTS_PER_REG) {

            /* Point to back up calendar */
            if (table_num == 0) {
                reg = pgw_tdm2_regs[base / _PGW_TDM_SLOTS_PER_REG];
            } else {
                reg = pgw_tdm_regs[base / _PGW_TDM_SLOTS_PER_REG];
            }

            COMPILER_64_ZERO(rval64);
            for (index = 0; index < _PGW_TDM_SLOTS_PER_REG; index++) {
                slot = base + index;
                phy_port = tdm->pgw_tdm[pgw_master / 2][slot];
                if (phy_port == NUM_EXT_PORTS) {
                    break;
                }
                soc_reg64_field32_set(unit, reg, &rval64, pgw_tdm_fields[slot],
                                      phy_port);
                count++;
            }

            if (index != 0) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, obm_inst, 0, rval64));
            }
            if (index != _PGW_TDM_SLOTS_PER_REG) {
                /* Due to hardware limitation, line-rate TDM wrap pointer
                 * must be larger than 0
                 */
                if (count == 1) {

                    count = 2;

                    soc_reg64_field32_set(unit, reg, &rval64,
                                          pgw_tdm_fields[slot],
                                          PGW_LR_UNUSED_SLOT);

                    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, obm_inst, 0,
                                                    rval64));
                }
                break;
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                       (BSL_META_U(unit,
                       "pgw_master %d, count %d\n"), pgw_master, count));

        /*
         * When switching from current calendar to new calendar, there is an
         * intermediate state where hardware waits until end of current calendar
         * to switch to the new celandar. This happens when a moving table
         * pointer (ptr) inside the hardware reaches the value LR_TDMx_WRAP_PTR.
         * In the case where was no port on this pgw before flexing,
         * the hardware pointer will not increment to reach LR_TDMx_WRAP_PTR
         * value. In this case we set the value of wrap pointer to 0, so that,
         * inside hardware, calendar switching happens.
         */
        if (table_num == 0) {
            /* Program new calendar bits */
            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM2_WRAP_PTRf, count ? (count - 1) : 0);

            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM2_ENABLEf, count ? 1 : 0);
        } else {
            /* Program new calendar bits */
            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM_WRAP_PTRf, count ? (count - 1) : 0);

            soc_reg64_field32_set(unit, PGW_TDM_CONTROLr, &ctrl_rval64,
                                  LR_TDM_ENABLEf, count ? 1 : 0);
        }

        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, PGW_TDM_CONTROLr, obm_inst, 0, ctrl_rval64));
    }

    /* Configure ingress scheduler back up calendar */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {

        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_get(unit, pipe,
                                                            &table_num)); 

        reg = mmu_tdm_regs[pipe];

        /* Program the _other_ (back up) TDM table (not the one in use)*/
        if (table_num == 0) {
            mem = mmu_tdm_mems_1[pipe];
        } else {
            mem = mmu_tdm_mems_0[pipe];
        }

        length = _MMU_TDM_LENGTH;
        for (slot = _MMU_TDM_LENGTH - 1; slot >= 0; slot--) {
            if (tdm->mmu_tdm[pipe][slot] != NUM_EXT_PORTS) {
                length = slot + 1;
                break;
            }
        }
        
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < length; slot += 2) {
            phy_port = tdm->mmu_tdm[pipe][slot];
            if (phy_port == GET_OVS_TOKEN(unit)) {
                mmu_port = 57; /* oppurtunist port */
            } else if (phy_port == GET_IDL_TOKEN(unit)) {
                mmu_port = 58; /* scheduler will not pick anything */
            } else if (phy_port >= NUM_EXT_PORTS) {
                mmu_port = 0x3f;
            } else {
                /* The BCM layer has updated the p2m mapping before calling
                 * tdm_set, so we use it. Other way may be to find mmu_port
                 * from the subset of new ports in new_port_info.
                 * For other ports, updated p2m mapping may be used 
                 */
                mmu_port = si->port_p2m_mapping[phy_port];
            }

            soc_mem_field32_set(unit, mem, entry, PORT_NUM_EVENf,
                                mmu_port & 0x3F);

            phy_port = tdm->mmu_tdm[pipe][slot + 1];
            if (phy_port == GET_OVS_TOKEN(unit)) {
                mmu_port = 57; /* oppurtunist port */
            } else if (phy_port == GET_IDL_TOKEN(unit)) {
                mmu_port = 58; /* scheduler will not pick anything */
            } else if (phy_port >= NUM_EXT_PORTS) {
                mmu_port = 0x3f;
            } else {
                /* See comment above for EVENf slots */
                mmu_port = si->port_p2m_mapping[phy_port];
            }
            soc_mem_field32_set(unit, mem, entry, PORT_NUM_ODDf,
                                mmu_port & 0x3f);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
        }

        /* We are programming the back up table, so we set it's
         * 'end' field (that is, not the current table's end field)
         */
        if (table_num == 0) {
            soc_reg_field_set(unit, reg, &rval, CAL1_ENDf, slot / 2);

            if (length & 1) {
                soc_reg_field_set(unit, reg, &rval, CAL1_END_SINGLEf, 1);
            }
        } else {
            soc_reg_field_set(unit, reg, &rval, CAL0_ENDf, slot / 2);

            if (length & 1) {
                soc_reg_field_set(unit, reg, &rval, CAL0_END_SINGLEf, 1);
            }
        }

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    /* Configure IARB TDM */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        mem = iarb_tdm_mems[pipe];
        sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
        for (slot = 0; slot < _IARB_TDM_LENGTH; slot++) {
            if (slot > tdm->iarb_tdm_wrap_ptr[pipe]) {
                break;
            }
            soc_mem_field32_set(unit, mem, entry, TDM_SLOTf,
                                tdm->iarb_tdm[pipe][slot]);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot, entry));
        }
    }

    /* Both pipe are expected to have same IARB TDM table length */
    fields[0] = TDM_WRAP_PTRf;
    values[0] = tdm->iarb_tdm_wrap_ptr[0];
    fields[1] = DISABLEf;
    values[1] = 0;
    fields[2] = AUX_CMICM_SLOT_ENf;
    values[2] = 1;
    fields[3] = AUX_EP_LB_SLOT_ENf;
    values[3] = 1;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, IARB_TDM_CONTROLr, REG_PORT_ANY, 4,
                                 fields, values));

    /* Find ports with speeds >= 100G in any or both of the pipes */ 
    /* In the new flex config, check if we are adding >= 100G speeds in a pipe,
     * or removing >=100G speeds from a pipe. If we are removing, profile_ will 
     * set to '1'. If we are adding, profile_ will be set to '0' in the code
     * below. If there's no change in the pipe as far as speeds >= 100G is
     * concerned, we will set the same value of profile_ again.
     */

    profile_x = 1; /* For enforcing a minimum of 11 cycle same-port spacing */
    profile_y = 1; /* For enforcing a minimum of 11 cycle same-port spacing */

    for (idx = 0; idx < new_port_info_size; idx++) {
        if (new_port_info[idx].speed >= 100000) {
            /* Set profile_* value for enforcing a minimum of 4 cycle
             * same-port spacing for speeds >= 100G
             */
            if (new_port_info[idx].pipe == 0) {
                /* Port belongs to x-pipe */
                profile_x = 0;
            } else {
                /* Port belongs to y-pipe */
                profile_y = 0;
            }
        }
    }

    /* If there was any port >= 100G configured during a past flexing within a 
     * pipe, then we need to retain the same-port spacing value for 100G (0)
     * Search in the bigger (whole) set of ports
     */
    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        pbmp = pipe ? si->ypipe_pbm : si->xpipe_pbm;
        SOC_PBMP_ITER(pbmp, port) {
            
            if (si->port_l2p_mapping[port] == -1) {
                continue;
            }

            if (si->port_speed_max[port] >= 100000) {
                if (pipe == 0) {
                    profile_x = 0;
                } else {
                    profile_y = 0;
                }
            }
        }
    }

    /* X-pipe */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ES_PIPE0_MIN_SPACINGr,
                        REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, ES_PIPE0_MIN_SPACINGr, &rval, PROFILEf, profile_x);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ES_PIPE0_MIN_SPACINGr,
                                      REG_PORT_ANY, 0, rval));

    /* Y-pipe */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ES_PIPE1_MIN_SPACINGr,
                        REG_PORT_ANY, 0, &rval));
    soc_reg_field_set(unit, ES_PIPE1_MIN_SPACINGr, &rval, PROFILEf, profile_y);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ES_PIPE1_MIN_SPACINGr,
                                      REG_PORT_ANY, 0, rval));

    /* Configure ingress scheduler back up calendar */

    /* Safety check to make sure the array bounds are not crossed. If the input
     * array size exceeds, then we can increase size of cut_thru_en[]
     */
    if (new_port_info_size > SOC_MAX_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                   "Input array size %d (new_port_info_size) exceeds"
                   " SOC_MAX_NUM_PORTS\n"),
                   new_port_info_size));
        return SOC_E_INTERNAL;
    }

    /* Initialize cut-through settings array */
    sal_memset(cut_thru_en, 0, sizeof(cut_thru_en));

    sal_memset(&res, 0, sizeof(soc_port_resource_t));
    PBMP_PORT_ITER(unit, port) {

        if (IS_LB_PORT(unit, port) || IS_CPU_PORT (unit, port)) {
            continue;
        }

        /* Skip inactive ports, and ports with no mapping */
        if ((SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) ||
            (si->port_l2p_mapping[port] == -1)) {
            continue;
        }

        /* Cut-through setting should be manipulated for line rate ports only.
         * Also, only speeds <= 21000 need to be considered
         */
        if ((SOC_PBMP_MEMBER(si->oversub_pbm, port)) ||
            (si->port_speed_max[port] > 21000)) {
            continue;
        }

        /* Manipulate cut-through settings for the ports */
        SOC_IF_ERROR_RETURN(soc_td2p_is_cut_thru_enabled(unit,
                            port, &cut_thru_en[port]));

        /* If cut-through is enabled, then we disable it here and re-enable
         * it again later in this function. If cut-through is disabled,
         * we don't program anything related to cut-through, here
         */
        if (cut_thru_en[port] == TRUE) {
            res.logical_port = port;
            res.physical_port = si->port_l2p_mapping[port];
            res.speed = si->port_speed_max[port];
            res.oversub = 0;
            /* Disable cut-through for this port */
            SOC_IF_ERROR_RETURN(soc_td2p_cut_thru_enable_disable(unit, &res,
                                                                 0));
        }

    }

    /* Configure ingress scheduler back up calendar */
    /* Note we are providing 80Usec delay only once, not twice
     */
    sal_usleep(80);
    idx = 0;

    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        int masters[_TD2_PGWS_PER_DEV / 2] = {0, 3, 4, 7}; /* pgw masters */
        int masters_per_pipe = (sizeof(masters) / sizeof(int)) /
                                _TD2_PIPES_PER_DEV;
         int pgw_num;

        /* Switch over egress calendar */
        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_get(unit, pipe,
                                                            &table_num));
        table_num ^= 0x1;
        
        SOC_IF_ERROR_RETURN(soc_td2p_curr_egr_tdm_table_set(unit, pipe,
                                                            table_num));

        for (pgw_num = 0; pgw_num  < masters_per_pipe; pgw_num++, idx++) { 
            obm_inst = masters[idx] | SOC_REG_ADDR_INSTANCE_MASK;

            /* Switch over ingress calendar */

            SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_get(unit,
                                obm_inst, &table_num));
            table_num ^= 0x1;

            SOC_IF_ERROR_RETURN(soc_td2p_curr_ingr_tdm_table_set(unit,
                                obm_inst, table_num));
            }
    }

    /* Configure ingress scheduler back up calendar */
    sal_memset(&res, 0, sizeof(soc_port_resource_t));
    PBMP_PORT_ITER(unit, port) {

        if (IS_LB_PORT(unit, port) || IS_CPU_PORT(unit, port)) {
            continue;
        }

        /* Skip inactive ports, and ports with no mapping */
        if ((SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) ||
            (si->port_l2p_mapping[port] == -1)) {
            continue;
        }

        /* Cut-through setting should be manipulated for line rate ports only.
         * Also, only speeds <= 21000 need to be considered
         */
        if ((SOC_PBMP_MEMBER(si->oversub_pbm, port)) ||
            (si->port_speed_max[port] > 21000)) {
            continue;
        }

        /* Cut-through was enabled before entering this function, so we
         * restore it. If cut-through was disabled before entering this
         * function, we don't perform any programming related to it.
         */
        if (cut_thru_en[port] == TRUE) {
            res.logical_port = port;
            res.physical_port = si->port_l2p_mapping[port];
            res.speed = si->port_speed_max[port];
            res.oversub = 0;
            /* Re-enable cut-through for this port */
            SOC_IF_ERROR_RETURN(soc_td2p_cut_thru_enable_disable(unit, &res,
                                                                 1));
        }

    }

    /* Reconfigure OBM thresholds */
    SOC_IF_ERROR_RETURN(soc_td2p_obm_threshold_set(unit,
                        curr_port_info_size, curr_port_info,
                        new_port_info_size, new_port_info,
                        lossless));

    /* Reconfigure EDB prebuffer */

    sal_usleep(1);

    /* Note both pipelines' registers are written by hardware internally */
    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));


    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_pgw_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex
 *         status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex
 *                          status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex
 *                          status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *      lossless     - (IN) MMU lossless information
 *
 * Returns:
 *      SOC_E_XXX
 * Note: 
 *      - Assumes FlexPort cases are valid.
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int
soc_td2p_port_resource_pgw_set(int unit,
                      int pre_num_res,  soc_port_resource_t *pre_res,
                      int post_num_res, soc_port_resource_t *post_res,
                      int lossless)
{
    soc_esw_portctrl_pgw_t data;
    int i;

    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_td2p_resource_data_check(unit, post_num_res, post_res));

    /*
     * NOTE: Mode is set in a per-XLPORT register, so
     *       we only need to update the mode in a port.
     */
    for (i = 0; i < post_num_res; i++) {
        data.mode       = post_res[i].mode;
        data.lanes      = post_res[i].num_lanes;
        data.port_index = post_res[i].lane_info[0]->port_index;
        data.flags      = 0x0;

        SOC_IF_ERROR_RETURN(soc_esw_portctrl_pgw_reconfigure(unit,
                            post_res[i].logical_port,
                            post_res[i].physical_port, &data));
    }


    SOC_IF_ERROR_RETURN
        (soc_td2p_pgw_obm_set(unit,
                                        pre_num_res, pre_res,
                                        post_num_res, post_res));

    SOC_IF_ERROR_RETURN(soc_td2p_pgw_port_hw_default_set(unit,
                                                         pre_num_res, pre_res,
                                                         lossless));
    SOC_IF_ERROR_RETURN(soc_td2p_pgw_port_sw_default_set(unit,
                                                         post_num_res, post_res,
                                                         lossless));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_ip_set
 * Purpose:
 *      Reconfigure IP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_ip_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_td2p_info_t *si_info)
{
    soc_info_t *si;
    soc_mem_t mem;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        sal_memset(&entry, 0, sizeof(entry));

        /* The p2l mapping has been updated before this function is called */
        port = si->port_p2l_mapping[phy_port];

        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x7F : port);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port,
                                          &entry));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_ep_set
 * Purpose:
 *      Reconfigure EP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the 
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_td2p_port_resource_ep_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_td2p_info_t *si_info)
{
    soc_info_t *si;
    uint32 rval;
    int num_port, port, idx, phy_port;

    si = &SOC_INFO(unit);

    /* Ingress logical to physical port mapping */
    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        /*
         * Use logical to physical number mapping from soc info structure
         * since it has been updated before this function is called
         */
        phy_port = si->port_l2p_mapping[port];

        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0xFF : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }

    /* Clear counters for the newly added ports */
    for (idx = 0; idx < new_port_info_size; idx++) {
        SOC_IF_ERROR_RETURN(soc_td2p_ep_counters_clear(unit,
                                                       &new_port_info[idx]));
    }

    return SOC_E_NONE;
}

/*
 * Function: 
 *      soc_td2p_port_resource_mmu_mapping_set
 * Purpose:
 *      Set the HW MMU Port mappings.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      nport     - (IN) Number of elements in array resource.
 *      resource  - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_td2p_port_resource_mmu_mapping_set(int unit, int nport,
                                       soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    uint32 rval;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MMU Port Mappings Set\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            logic_port = 0x7F;
            phy_port = 0xFF;

            /* Clear MMU port info */
            SOC_IF_ERROR_RETURN(
            soc_td2p_mmu_port_resource_clear(unit, 
                                             pr->logical_port));
        } else {
            logic_port = pr->logical_port;
            phy_port = pr->physical_port;
        }

        /*
         * Skip for legacy inactive configurations since
         * MMU mappings are kept the same.
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "  Skip port with inactive configuration, "
                                    "mappings are kept the same, "
                                    "mmu=%d logical=%d physical=%d\n"),
                         si->port_p2m_mapping
                         [si->port_l2p_mapping[pr->logical_port]],
                         pr->logical_port,
                         si->port_l2p_mapping[pr->logical_port]));
            continue;
        }


        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                          PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr,
                           pr->logical_port, 0, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, &rval,
                          LOGIC_PORTf, logic_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr,
                           pr->logical_port, 0, rval));

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  MMU port mappings: "
                                "mmu=%d logical=%d physical=%d\n"),
                     si->port_p2m_mapping
                     [si->port_l2p_mapping[pr->logical_port]],
                     logic_port, phy_port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_resource_mmu_set
 * Purpose:
 *      Called to reconfigure the MMU hardware.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN) An array of port changes that are being done
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_td2p_port_resource_mmu_set(int unit, int nport,
                               soc_port_resource_t *flexport_data)
{
    int i, rv = SOC_E_NONE;
    int * ports;

    /* Before we initialize LLS for the newly flexed ports,
     * let's take a moment to update _td2_invalid_ptr since
     * flex operation could have added new mmu ports.
     * And also update the LLS memories with INVALID PARENT
     * values
 */
    SOC_IF_ERROR_RETURN(soc_td2p_lls_reinit_invalid_pointers(unit));
    SOC_IF_ERROR_RETURN(soc_td2p_lls_reset_flex(unit));

    /* Update MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_mapping_set(unit, nport,
                                                flexport_data));

    /* Configure all MMU registers for new ports */
    for (i = 0; i < nport; i++) {
        if (flexport_data[i].physical_port != -1) {
            /* a physical port of -1 indicates it's being deleted, so here
               we're adding */
            SOC_IF_ERROR_RETURN(
            soc_td2p_mmu_port_resource_set(unit, flexport_data[i].logical_port,
                                           flexport_data[i].speed,
                          (flexport_data[i].flags & SOC_PORT_RESOURCE_I_MAP)));
        } 
    }

    /* Copy the logical ports into a bcm_ports_t array because
       bcm_td2p_update_base_queue_num in src/bcm/esw/trident2/cosq.c has no
       concept of what a soc_port_resource_t is. */
    ports = sal_alloc(sizeof(int) * nport, "flexport_ports");
    if (ports == NULL) {
        return SOC_E_MEMORY;
    }
    for (i = 0; i < nport; i++) {
        ports[i] = flexport_data[i].logical_port;
    }

    /* Update the logical port and queue mappings in memory and registers */
    if (td2p_flexport_cb) {
        rv = td2p_flexport_cb(unit, nport, ports);
    }

    /* free up the ports array */
    sal_free(ports);

    return rv;
}

void
soc_td2p_num_hsp_reserved_reset(int unit)
{
    num_hsp_to_reserve[0] = 0;
    num_hsp_to_reserve[1] = 0;
    return;
}

/*
 * Function:
 *      soc_td2p_num_hsp_reserved_per_pipe
 * Purpose:
 *   This function calculates the number of HSP ports to reserve per pipe.
 *   It does by using the following algorithm:
 *   - device speed
 *   - for High clock frequency systems ( > 760), it takes into consideration
 *      the number of TSC-12's that are flex enabled
 *   - for Lower clock frequency systems, it looks at the number of port macros
 *      (TSC-4's) that are flex enabled * Parameters:
 * Parameters 
 *      unit           - (IN) Device Number
 *      pipe           - (IN) The pipe that will be checked to see how many
 *                            HSP need to be reserved.
 *      num_hsp        - (OUT) The number of HSP that need to be reserved
 *                             for the specified pipe.
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 * Note:
 */
int 
soc_td2p_num_hsp_reserved_per_pipe(int unit, int pipe, int *num_hsp)
{
    int port_macro, pm_flex_enabled = 0, temp_num_hsp = 0, port = 0;
    int i, phy_port, skip_port;
    soc_info_t *si;
    soc_pbmp_t pbmp;
    int phy_ports_x[] = {1, 21, 33, 53};
    int phy_ports_y[] = {65, 85, 97, 117};
    int * phy_ports = NULL;

    si = &SOC_INFO(unit);

    if (num_hsp == NULL) {
        return SOC_E_PARAM;
    }

    *num_hsp = 0;

    /* If we're at a sufficiently high frequency,
       only check the first physical port of every TSC-12 PM:
       physical ports 1, 21, 33, 53, 65, 85, 97, 117.
       This is because only 100G+ ports are considered HSP. */
    if (si->frequency >= 760) {
        if (pipe == 0) {
            phy_ports = phy_ports_x;
        } else {
            phy_ports = phy_ports_y;
        }
        /* If a PGW is flexport capable or if it's 100G but not flex capable,
           a HSP MMU Port needs to be saved for it; it may not be given
           to a non-hsp port. */
        for(i = 0; i < _TD2_PGWS_PER_PIPE; i++) {
            if(soc_td2p_is_port_flex_enable(unit, phy_ports[i])) {
                port = si->port_p2l_mapping[phy_ports[i]];
                temp_num_hsp++;
            } else {
                port = si->port_p2l_mapping[phy_ports[i]];
                /* the PGW is NOT port flex enabled, but still requires a HSP
                   so mark it as reserved. */
                if (si->port_speed_max[port] >= 100000 ||
                    soc_property_port_get(unit, port, spn_PORT_SCHED_HSP, 0)) {
                    temp_num_hsp++;
                }
            }
        }
        /* Go through all the logical ports, skipping the ones mapped to
           the first of the PGWs
           since they were checked above, and see if they are port_sched_hsp */
        pbmp = pipe ? si->ypipe_pbm : si->xpipe_pbm;
        SOC_PBMP_ITER(pbmp, port) {
            phy_port = si->port_l2p_mapping[port];
            skip_port = 0;

            /* skip the port if it was already counted above */
            for(i = 0; i < _TD2_PGWS_PER_PIPE; i++) {
                if(phy_port == phy_ports[i]) {
                    skip_port = 1;
                    break;
                }
            }
            if(skip_port == 0) {
                /* if the logical port was declared port_sched_hsp,
                   reserve HSP */
                if(soc_property_port_get(unit, port, spn_PORT_SCHED_HSP, 0)) {
                    temp_num_hsp++;
                }
            }
        }
    } else {
        /* Otherwise, check each PM on the specified pipe because
           we'll need to at least reserve 1 HSP port per port macro.
           This function does NOT determine if a T2OQ needs to be
           reserved too. */
        for (port_macro = pipe * _TD2P_NUM_TSC4_PER_PIPE; 
             port_macro < (pipe * _TD2P_NUM_TSC4_PER_PIPE) + 
                  _TD2P_NUM_TSC4_PER_PIPE; port_macro++) {
            SOC_IF_ERROR_RETURN(
               soc_td2p_port_macro_flex_enabled(unit, port_macro, &pm_flex_enabled));

            /* If a PM is flexport capable or if it's 40G but not flex capable,
               a HSP MMU Port needs to be saved for it; it may not be given
               to a non-hsp port. */
            if (pm_flex_enabled == 1) {
                temp_num_hsp++;
            } else {
                phy_port = soc_td2p_pm_to_1st_port_tsc4[port_macro];
                port = si->port_p2l_mapping[phy_port];
                /* the PM is NOT port flex enabled, but still requires a HSP
                   so mark it as reserved. */
                if (si->port_speed_max[port] >= 40000 ||
                    soc_property_port_get(unit, port, spn_PORT_SCHED_HSP, 0)) {
                    temp_num_hsp++;
                }
            }
        }

        /* Go through all the logical ports, skipping the ones mapped to
           the port macros
           since they were checked above, and see if they are port_sched_hsp */
        pbmp = pipe ? si->ypipe_pbm : si->xpipe_pbm;
        SOC_PBMP_ITER(pbmp, port) {
            phy_port = si->port_l2p_mapping[port];
            skip_port = 0;

            /* skip the port if it was already counted above */
            for (port_macro = pipe * _TD2P_NUM_TSC4_PER_PIPE; 
                 port_macro < (pipe * _TD2P_NUM_TSC4_PER_PIPE) + 
                      _TD2P_NUM_TSC4_PER_PIPE; port_macro++) {
                if(phy_port == 
                   soc_td2p_pm_to_1st_port_tsc4[port_macro]) {
                    skip_port = 1;
                    break;
                }
            }
            if(skip_port == 0) {
                /* if the logical port was declared port_sched_hsp,
                   reserve HSP */
                if(soc_property_port_get(unit, port, spn_PORT_SCHED_HSP, 0)) {
                    temp_num_hsp++;
                }
            }
        }
    }
    /* Only allow 16 HSP maximum per pipe */
    if(temp_num_hsp > SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE) {
        *num_hsp = SOC_TD2_NUM_HSP_MMU_PORTS_PER_PIPE;
    } else {
        *num_hsp = temp_num_hsp;
    }

    return SOC_E_NONE;
}



/* Tells if a port macro (indexed by TSC4, not TSC12) is flex port enabled.
 *  
 * Port macro flex on or off is determined by the first port in that macro. 
 * For TSC12 it's the first port in the first TSC4. For a TSC4 that is NOT 
 * part of a TSC12, it's also the first port of that port macro.  For a TSC4 
 * that is a part of a TSC12, refer to the first port of the first port macro 
 * for that TSC12. 
 *  
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port_macro    - (IN) port macro desired
 *      flex_enabled  - (OUT) whether the port macro is enabled or disabled
 *  
 *  Returns: 
 *      SOC_E_xxx
 */
int
soc_td2p_port_macro_flex_enabled(int unit, int port_macro, int * flex_enabled)
{
    soc_info_t *si;
    int port_num = -1;

    si = &SOC_INFO(unit);

    if ((port_macro < 0) || (port_macro > 31)) {
        return SOC_E_PARAM;
    }

    /* If SKU is max frequency, check flexport for TSC-12 and then
            individual TSC-4.
       Else, check each individual TSC-4 (not grouped as TSC-12). */
    if (si->frequency >= 760) {
        port_num = soc_td2p_pm_to_1st_port_tsc12[port_macro];
    } else {
        port_num = soc_td2p_pm_to_1st_port_tsc4[port_macro];
    }

    *flex_enabled = soc_td2p_is_port_flex_enable(unit, port_num);

    return SOC_E_NONE;
}

/* Function to reserve cells for CutThru(CT) and Flex ports
 * Parameters:
 *      unit          - (IN) Unit number.
 *      pipe          - (IN) Pipe number
 *      flex          - (IN) Config has Flex ports or Not
 *      rsvd_buffer   - (OUT) Buffers reserved per resource (Ing/Egr/ASF)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_td2p_mmu_additional_buffer_reserve(int unit, int pipe, int flex,
                                     _soc_mmu_rsvd_buffer_t *rsvd_buffer)
{
    soc_info_t *si;
    int pg_min_cells = 0, qgrp_min_cells = 0, hdrm_cells = 0, asf_cells = 0;
    int total_reserve = 0;
    int lossless = 0;
    int mcq_entry = 0;

    int i, j, start_port, logical_port, start_pm, end_pm;
    int pm_flex_enabled = 0;

    COMPILER_REFERENCE(si);
    si = &SOC_INFO(unit);

    if ((pipe >= NUM_PIPE(unit)) || (!rsvd_buffer)) {
        return SOC_E_PARAM;
    }

    /* returning if flex is true.  flex true indicates the legacy flexport ':i' case is
       enabled */
    if (flex) {
        return SOC_E_NONE;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    /* if we're on pipe 1, start at the 2nd half of the macros.  Otherwise,
       start at the 1st half */
    if(pipe) {
        start_pm = _TD2P_NUM_TSC4_PER_PIPE;
        end_pm = 2*_TD2P_NUM_TSC4_PER_PIPE;
    } else {
        start_pm = 0;
        end_pm = _TD2P_NUM_TSC4_PER_PIPE;
    }
    
    /* Per pipe, calculate the memory required on a per port macro (TSC4) basis */    
    for (i = start_pm; i < end_pm; i++) {
        SOC_IF_ERROR_RETURN(
            soc_td2p_port_macro_flex_enabled(unit, i, &pm_flex_enabled));

        /* if this port macro is flex enabled, allocate memory for it accordingly */
        if (pm_flex_enabled == 1) {
            /* Reserved Buffer for Flex Port Macro when Flex Port is enabled in
               device */
            if (lossless) {
                hdrm_cells      = (_TD2P_RSVD_BUF_FLEX_HDRM_LOSSLESS 
                                   * _TD2P_NUM_PORTS_PER_TSC4);
                pg_min_cells    = (_TD2P_MMU_RSVD_BUF_FLEX_PGMIN_LOSSLESS
                                   * _TD2P_NUM_PORTS_PER_TSC4);
                qgrp_min_cells  = 0;
                asf_cells       = (_TD2P_RSVD_BUF_FLEX_ASF
                                   * _TD2P_NUM_PORTS_PER_TSC4);
            } else {
                hdrm_cells      = 0;
                pg_min_cells    = 0;
                qgrp_min_cells  = (_TD2P_RSVD_BUF_FLEX_QGRP 
                                   * _TD2P_NUM_PORTS_PER_TSC4);

                asf_cells       = (_TD2P_RSVD_BUF_FLEX_ASF
                                   * _TD2P_NUM_PORTS_PER_TSC4);
            }
            mcq_entry = _TD2P_MMU_NUM_MCQ_PORT_MACRO * 
                _TD2P_MMU_PER_COSQ_EGRESS_QENTRY_RSVD *
                _TD2P_NUM_PORTS_PER_TSC4;
        } else {
            /*For port modules that are in non-flex mode, buffer is reserved 
              per port (not port macro).
             
              Here we are calculating the buffer requirements for this TSC4 on
              a per port basis. */
            hdrm_cells = 0;
            pg_min_cells = 0;
            qgrp_min_cells = 0;
            asf_cells = 0;
            mcq_entry = 0;

            start_port = (i * _TD2P_NUM_PORTS_PER_TSC4) + 1;
            for (j = start_port; j < start_port + _TD2P_NUM_PORTS_PER_TSC4; j++) {
                /* figure out from the logical port what the speed of this port is */
                logical_port = si->port_p2l_mapping[j];
                if (logical_port == -1) {
                    continue;
                }

                if (si->port_speed_max[logical_port] >= 100000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_100G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else if (si->port_speed_max[logical_port] >= 40000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_40G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else if (si->port_speed_max[logical_port] >= 20000) {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_20G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_ASF;
                    }
                } else {
                    if (lossless) {
                        hdrm_cells      += _TD2P_RSVD_BUF_NON_FLEX_10G_HDRM_LOSSLESS;
                        pg_min_cells    += _TD2P_RSVD_BUF_NON_FLEX_PGMIN;
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_10G_ASF;
                    } else {
                        qgrp_min_cells  += _TD2P_RSVD_BUF_NON_FLEX_QGRP;
                        asf_cells       += _TD2P_RSVD_BUF_NON_FLEX_10G_ASF;
                    }
                }
                mcq_entry += soc_td2p_mcq_entries_rsvd_port(unit, logical_port);
            }
        }

        /* Add what was calculated for the port macro to the total */
        rsvd_buffer->ing_rsvd_cells += (hdrm_cells + pg_min_cells);
        rsvd_buffer->egr_rsvd_cells += qgrp_min_cells;
        rsvd_buffer->asf_rsvd_cells += asf_cells;
        rsvd_buffer->mcq_entry_rsvd += mcq_entry;
    }

    total_reserve = rsvd_buffer->ing_rsvd_cells;
    total_reserve += rsvd_buffer->egr_rsvd_cells;
    total_reserve += rsvd_buffer->asf_rsvd_cells;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config: Cells rsvd for Pipe %d,"
                            " Flex/ASF per pipe: %d, Ing: %d,"
                            " Egr: %d, ASF: %d MCQ %d\n"),
                            pipe, total_reserve, rsvd_buffer->ing_rsvd_cells,
                            rsvd_buffer->egr_rsvd_cells,
                            rsvd_buffer->asf_rsvd_cells,
                            rsvd_buffer->mcq_entry_rsvd));

    return SOC_E_NONE;
}

STATIC int
soc_td2p_mmu_config_init_port(int unit, int port)
{
    int rv;
    int lossless;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    _soc_td2_mmu_init_dev_config(unit, &devcfg, lossless);
    /* Calculate the buf - global as well as per port
     * but _soc_td2_mmu_config_buf_set_hw_port is only made
     * for that port - since it is flex operation - we don't
     * touch any other port */
    _soc_td2_mmu_config_buf_default(unit, buf, &devcfg, lossless);
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU config: Use default setting\n")));
        _soc_td2_mmu_config_buf_default_global(unit, buf, &devcfg, lossless);
        _soc_td2_mmu_config_buf_default_port(unit, port, buf, &devcfg, lossless);
        _soc_mmu_cfg_buf_calculate(unit, buf, &devcfg);
    }
    rv = _soc_td2_mmu_config_buf_set_hw_port(unit, port, buf, &devcfg, lossless);

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}


/*
 * Function:
 *      soc_td2p_mmu_flexport_map_validate
 * Purpose:
 *      Validate and allocate/deallocate MMU port assignments.
 *      New MMU port assignments are returned in argument.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN/OUT) An array of port changes that are being done.
 *                       Returns MMU ports assignment.
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 */
int
soc_td2p_mmu_flexport_map_validate(int unit, int nport, 
                                   soc_port_resource_t * flexport_data)
{
    return soc_td2p_mmu_flexport_allocate_deallocate_ports(unit, nport, 
                                                           flexport_data);
}

/*
 * Function:
 *      soc_td2p_mmu_flexport_allocate_deallocate_ports
 * Purpose:
 *      Allocate or deallocate ports in software.  This function frees up
 *      mmu ports or allocates them in software.  When apply is 0,
 *      flexport_data[x].mmu_port needs to be filled in.  When apply is 1,
 *      flexport_data[x].mmu_port is valid and needs to be assigned in the
 *      port_p2m_mapping[].
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN/OUT) An array of port changes that are being done
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise. 
 */
int
soc_td2p_mmu_flexport_allocate_deallocate_ports(int unit, int nport, 
                             soc_port_resource_t * flexport_data)
{
    int i, j;
    int phy_port;
    int mmu_port;
    int pipe;
    int start_mmu_port;
    int end_mmu_port;
    int port_max_speed = 0;
    int mmu_ports_used[TD2P_PIPES_PER_DEV * _TD2_PORTS_PER_PIPE];
    int flexport_max_speed = -1;
    int flexport_max_speed_pipe[_TD2_PIPES_PER_DEV];
    int t2oq_reserved[2];
    int port_sched_hsp_exists = 0;

    soc_info_t *si = &SOC_INFO(unit);

    t2oq_reserved[0] = 0;
    t2oq_reserved[1] = 0;

    sal_memset(mmu_ports_used, 0, sizeof(int) * COUNTOF(mmu_ports_used));

    /* Iterate through all logical ports and see if any of them are marked
       with port_sched_hsp */
    PBMP_ALL_ITER(unit, i) {
        if(soc_property_port_get(unit, i, spn_PORT_SCHED_HSP, 0)) {
            port_sched_hsp_exists = 1;
        }
    }

    /* Iterate through all the MMU ports and mark the used mmu ports */
    for (j = 1; j <= TD2P_PIPES_PER_DEV * _TD2_PORTS_PER_PIPE; j++) {

        mmu_port = si->port_p2m_mapping[j];

        /* if the mmu port is assigned, mark it as used */
        if(mmu_port != -1) {
            mmu_ports_used[mmu_port] = 1;
        }
    }

    /* Find out how many port macros are capable of flexing per pipe. Only
       really needs to be called and set once. */
    if(num_hsp_to_reserve[0] == 0 && num_hsp_to_reserve[1] == 0) {
        SOC_IF_ERROR_RETURN(
           soc_td2p_num_hsp_reserved_per_pipe(unit, 0, &num_hsp_to_reserve[0]));
        SOC_IF_ERROR_RETURN(
           soc_td2p_num_hsp_reserved_per_pipe(unit, 1, &num_hsp_to_reserve[1]));
    }

    /* Iterate through the array to deallocate and allocate ports */
    for (i = 0; i < nport; i++) {
        if ((flexport_data[i].flags & SOC_PORT_RESOURCE_I_MAP) ||
            (flexport_data[i].physical_port == -1)) {

            /*
             * Get the current physical port and MMU Port associated
             * with the logical port passed in */
            phy_port = si->port_l2p_mapping[flexport_data[i].logical_port];

            if (phy_port == -1) { 
                LOG_VERBOSE(BSL_LS_SOC_MMU,
                    (BSL_META_U(unit,
                                "MMU port deallocation failure: logical "
                                "port %d is not mapped to a physical port\n"), 
                                flexport_data[i].logical_port));

                return SOC_E_PARAM;
            }

            mmu_port = si->port_p2m_mapping[phy_port];

            /* Use current MMU port assignments for inactive mapping */
            if (flexport_data[i].flags & SOC_PORT_RESOURCE_I_MAP) {
                if (mmu_port == -1) { 
                    LOG_ERROR(BSL_LS_SOC_MMU,
                              (BSL_META_U(unit,
                                          "Invalid MMU port on inactive "
                                          "port configuration: "
                                          "port=%d mmu=%d\n"),
                               flexport_data[i].logical_port, mmu_port));
                    return SOC_E_INTERNAL;
                }

                flexport_data[i].mmu_port = mmu_port;
                LOG_VERBOSE(BSL_LS_SOC_MMU,
                            (BSL_META_U(unit,
                                        "Inactive port configuration, "
                                        "keep same mapping: port=%d mmu=%d\n"),
                             flexport_data[i].logical_port, mmu_port));
                continue;
            }

            /*
             * Continue with next case.
             * If the phy_port == -1, we're deallocating if we're told to
             */

            /* Check to make sure we aren't indexing into mmu_ports_used with
               a negative mmu port number. Skip this if p2m returns negative. */
            if (mmu_port < 0) {
                continue;
            }
            /* Mark the MMU port unused so that it can be allocated again
               in this routine */
            mmu_ports_used[si->port_p2m_mapping[phy_port]] = 0;


        } else {
            /* We're allocating a port.
               1. Find out if we need to allocate a HSP given the port speed
               and the clock frequency
               2. Assign a free MMU port appropriate to the pipe the physical
               port is on.  A phy port on X
                  pipe must have an MMU port on X pipe, Y on Y.  */

            SOC_IF_ERROR_RETURN(soc_td2p_port_resource_speed_max_get(unit, &flexport_max_speed));


            /* Find out what pipe we're going to assign to */
            if (flexport_data[i].physical_port < si->phy_port_base[1]) { /* X-pipe */
                pipe = 0;
            } else {
                pipe = 1;
            }

            /* If a per pipe flex speed has been defined, use that instead.
               Must have been defined for both X and Y Pipe. */
            flexport_max_speed_pipe[0] = 0;
            flexport_max_speed_pipe[1] = 0;

            SOC_IF_ERROR_RETURN(soc_td2p_port_resource_speed_max_x_get(unit, &flexport_max_speed_pipe[0]));
            SOC_IF_ERROR_RETURN(soc_td2p_port_resource_speed_max_y_get(unit, &flexport_max_speed_pipe[1]));

            /* If both X and Y pipes have had their flex speed specified, select
               the one from the appropriate pipe; it takes priority over the
               global flex speed. */
            if ((flexport_max_speed_pipe[0] > 0) && (flexport_max_speed_pipe[1] > 0)) {
                flexport_max_speed = flexport_max_speed_pipe[pipe];
            }

            /* 1. Find the first unused mmu port and map it to our phy
               port.  If we aren't allocating a HSP, skip MMU Ports 0-15
               (start at 16). If we are, start at 0 and end at 15.
             
               We use SOC_TD2_NUM_MMU_PORTS_PER_PIPE instead of
               _TD2_PORTS_PER_PIPE because
               the last 11 MMU ports in each pipe are not used.
             
               **** THIS IS PER PIPE!!! ****
             
               */
            if (si->bandwidth >= 960000) {
                if(flexport_data[i].speed >= 100000) {
                    start_mmu_port = 0;
                    end_mmu_port = num_hsp_to_reserve[pipe];
                } else {
                    /* If any port is capable of flexing above HG[42],
                       then we've reserved HSP and T2OQ */
                    if(flexport_max_speed > 42000 || port_sched_hsp_exists == 1) {
                        start_mmu_port = num_hsp_to_reserve[pipe];
                        /* 53 MMU ports per pipe, MMU port 52 is CPU port,
                           MMU port 116 is Loopback port*/
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;

                        /* Reserving T2OQ per pipe to match with HSP */
                        t2oq_reserved[pipe] = num_hsp_to_reserve[pipe];
                    } else {
                        /* No HSP.  All ports are treated as normal */
                        start_mmu_port = 0;
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;
                    }
                }
            } else if (si->bandwidth >= 480000) {
                /* both 480G and 720G SKU need a HSP at 40Gb and above */
                if (flexport_data[i].speed >= 40000) {
                    start_mmu_port = 0;
                    end_mmu_port = num_hsp_to_reserve[pipe];
                } else {
                    /* If any port is capable of flexing above 40G,
                       then we've reserved HSP and T2OQ */
                    if(flexport_max_speed >= 40000 || port_sched_hsp_exists == 1) {
                        start_mmu_port = num_hsp_to_reserve[pipe];
                        /* 53 MMU ports per pipe, MMU port 52 is CPU port,
                           MMU port 116 is Loopback port */
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;

                        /* Reserving T2OQ per pipe to match with HSP */
                        t2oq_reserved[pipe] = num_hsp_to_reserve[pipe];
                    } else {
                        start_mmu_port = 0;
                        end_mmu_port = SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1;
                    }
                }
            } else {
                LOG_VERBOSE(BSL_LS_SOC_MMU,
                    (BSL_META_U(unit,
                                "MMU port allocation failure: "
                                "SKU %d is not supported\n"), 
                                si->bandwidth));
                return SOC_E_INTERNAL;
            }

            /* 2. check pipe for a free mmu port */
            /* Add the necessary offset for the pipe we're in */
            start_mmu_port += (pipe * _TD2_PORTS_PER_PIPE);
            end_mmu_port += (pipe * _TD2_PORTS_PER_PIPE);

            /* loop from start mmu port to end mmu port to find
               a free mmu port
               0-51 is range for X Pipe
               52 is CPU port
               53-63 are NOT used
               64-115 is range for Y Pipe
               116 is Loopback port */
            for(j = start_mmu_port; j < end_mmu_port; j++) {
                /* Case where SKU == 720G and
                   40 Gig <= max flexed speed < 100 Gig doesn't
                   require T2OQ to be reserved, so we
                   can use mmu ports 36 - 51 in that case,
                   but MUST skip for all others */
                SOC_IF_ERROR_RETURN
                    (soc_td2p_port_resource_speed_max_get(unit,
                                                          &port_max_speed));

                /* Only want to skip the amount of T2OQ needed to match with
                   HSP. If we reserved 4 HSP, need to reserve 4 T2OQ max.
                   If we reserved 16 HSP, need to reserve 16 T2OQ.
                   j < (36 + t2oq_reserved + (pipe * _TD2_PORTS_PER_PIPE)).
                   sets the upper limit of T2OQ we reserve since j >= 36...  .
                   is the base. */
                if (!((si->bandwidth == 720000) &&
                      (port_max_speed >= 40000) &&
                      (port_max_speed < 100000))){
                    if ((j >= 36 + (pipe * _TD2_PORTS_PER_PIPE)) &&
                        j < (36 + t2oq_reserved[pipe] + (pipe * _TD2_PORTS_PER_PIPE))) {
                        continue;
                    }
                }

                /* We found an unused MMU port! Let's assign it to
                   a phy port! */
                if (mmu_ports_used[j] == 0) {
                    /* Mark the port as used. This is done in case
                       we're passed multiple ports
                       to allocate to make sure we have enough ports. */
                    mmu_ports_used[j] = 1;
                    flexport_data[i].mmu_port = j;
                    break;
                }
            }

            /* if we didn't find a free mmu port, return error. */
            if (j == end_mmu_port) {
                return SOC_E_RESOURCE;
            }
        }
    }
    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_port_asf_set
* Purpose:
*     Enable/Disable CT/ASF sequence
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
*     enable  - (IN) Enable or Disable.
* Returns:
*     SOC_E_XXX
 */
int
soc_td2p_port_asf_set (int unit, soc_port_t port, int enable)
{
    soc_port_resource_t res;
    soc_info_t *si;
    si = &SOC_INFO(unit);

    sal_memset(&res, 0, sizeof(soc_port_resource_t));
    res.logical_port = port;
    res.physical_port = si->port_l2p_mapping[port];
    res.speed = si->port_speed_max[port];
    res.oversub = IS_OVERSUB_PORT(unit, port);

    /* Enable/disable cut-through for this port */
    return soc_td2p_cut_thru_enable_disable(unit, &res, enable);
}

/*
* Function:
*     soc_td2p_port_icc_width_set
* Purpose:
*     Program PORT_INITIAL_COPY_COUNT_WIDTHr
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
 */
STATIC int
soc_td2p_port_icc_width_set(int unit, soc_port_t port)
{
    int num_lanes;
    int count_width = 0;

    if (SOC_REG_IS_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr)) {
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        switch (num_lanes) {
            case 1: count_width = 1;
                    break;
            case 2: count_width = 2;
                    break;
            case 4: count_width = 3;
                    break;
            default: count_width = 0;
                     break;
        }
        if (SOC_REG_FIELD_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, 
                    BIT_WIDTHf)) {
            SOC_IF_ERROR_RETURN(
                    soc_reg_field32_modify(unit, PORT_INITIAL_COPY_COUNT_WIDTHr,
                        port, BIT_WIDTHf, count_width ? (count_width - 1) : 0));                    
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_td2p_repl_port_agg_map_init
 * Purpose:
 *     Initialize L3MC Port Agg Map.
 * Parameters:
 *     unit    - (IN) Unit number.
 *     port    - (IN) Logical SOC port number.
 * Returns:
 *     SOC_E_xxx
 */
STATIC int
soc_td2p_repl_port_agg_map_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    int phy_port, mmu_port;
    uint32 regval;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    /* configure mmu port to repl aggregateId map */
    regval = 0;
    soc_reg_field_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, &regval,
            L3MC_PORT_AGG_IDf, mmu_port % 64);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr,
                port,0,regval));

    regval = 0;
    soc_reg_field_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, mmu_port);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, port, 0, regval));

    regval = 0;
    soc_reg_field_set(unit, MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, mmu_port);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr(unit,
                        port, regval));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_ipmc_repl_init
* Purpose:
*     Initialize IPMC Replication settings
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
 */
STATIC int
soc_td2p_ipmc_repl_init(int unit, soc_port_t port)
{
    /* Program MMU_TOQ_REPL_PORT_AGG_MAP
 */
    SOC_IF_ERROR_RETURN(soc_td2p_repl_port_agg_map_init(unit, port));

    return SOC_E_NONE;
}
/*
* Function:
*     soc_trident2p_num_cosq_init_port
* Purpose:
*     Assign the base queues and number of queues for the
*     specified port.  These are based upon MMU Port number to avoid
*     reallocation or running into other queues during post device init
*     port init.  This is modeled after soc_trident2_num_cosq_init
*     but only for individual ports.
* Parameters:
*     unit         - (IN) Unit number.
*     logical_port - (IN) logical port num
* Returns:
*     SOC_E_XXX
*
*
* cpu port (mmu port 0): 48 queues (2000-2047)
* loopback port (mmu port 116): 8 queues (4048-4055)
*/
STATIC int
soc_trident2p_num_cosq_init_port(int unit, int logical_port)
{
    soc_info_t *si;
    int port, phy_port, mmu_port, mmu_port_offset, pipe;
    int uc_cosq_base, mc_cosq_base, uc_cosq_per_port;
    int apply = 0;

    si = &SOC_INFO(unit);
/*
    if (soc_feature(unit, soc_feature_cmic_reserved_queues)) {
        si->port_num_cosq[CMIC_PORT(unit)] = 44;
    } else {
        si->port_num_cosq[CMIC_PORT(unit)] = 48;
    }
    si->port_cosq_base[CMIC_PORT(unit)] = 520; */
/*
    si->port_num_cosq[LB_PORT(unit)] = 9;
    si->port_cosq_base[LB_PORT(unit)] = 1088;*/

    for (pipe = 0; pipe < _TD2_PIPES_PER_DEV; pipe++) {
        if (pipe) { /* Y-pipe */
            uc_cosq_base = 1480;
            mc_cosq_base = 568;
        } else { /* X-pipe */
            uc_cosq_base = 0;
            mc_cosq_base = 0;
        }
        for (mmu_port_offset = 0; mmu_port_offset < _TD2_MMU_PORTS_PER_PIPE;
             mmu_port_offset++) {
            mmu_port = si->mmu_port_base[pipe] + mmu_port_offset;
            phy_port = si->port_m2p_mapping[mmu_port];
            if (phy_port == -1) {
                continue;
            }
            port = si->port_p2l_mapping[phy_port];

            if(port == logical_port) {
                apply = 1;
            }

            if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                continue;
            }

            if (SOC_PBMP_MEMBER(si->eq_pbm, port)) { /* VBS (HSP) port */
                if(apply == 1) {
                    si->port_num_cosq[port] = 10;
                    si->port_cosq_base[port] = mc_cosq_base + (mmu_port_offset * si->port_num_cosq[port]);
                    si->port_num_uc_cosq[port] = 10;
                    /* Made port_uc_cosq_base a function of mmu_port number
                     * rather than a running number (as calculated by
                     * uc_cosq_base before). If it is a running number, &
                     * the port numbers come & go dynamically (as in flex port)
                     * uc_cosq_base overlap. With port_uc_cosq_base a function
                     * of mmu_port, each port has a fixed offset
 */
                    si->port_uc_cosq_base[port] = uc_cosq_base + (mmu_port_offset * 10);
                }
            } else {
                uc_cosq_per_port =
                    soc_property_port_get(unit, port, spn_LLS_NUM_L2UC, 10);
                if (uc_cosq_per_port < 10 || uc_cosq_per_port > 16) {
                    uc_cosq_per_port = 10;
                }
                uc_cosq_per_port = (uc_cosq_per_port + 3) & ~3;

                if (apply == 1) {
                    si->port_num_cosq[port] = 10;
                    si->port_cosq_base[port] = mc_cosq_base + (mmu_port_offset * si->port_num_cosq[port]);
                    si->port_num_uc_cosq[port] = uc_cosq_per_port;
                }
                /* uc_cosq_base for LLS port must be
                 * divisible by 4 for PFC to work */
                uc_cosq_base = (uc_cosq_base + 3) & (~3);

                if (apply == 1) {
                    /* Made port_uc_cosq_base a function of mmu_port number
                     * rather than a running number (as calculated by
                     * uc_cosq_base before). If it is a running number, &
                     * the port numbers come & go dynamically (as in flex port)
                     * uc_cosq_base overlap. With port_uc_cosq_base a function
                     * of mmu_port, each port has a fixed offset
 */
                    si->port_uc_cosq_base[port] = uc_cosq_base + (mmu_port_offset * uc_cosq_per_port);

                    return SOC_E_NONE;
                }
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 * soc_td2p_hsp_sched_global_config_set
 * Purpose:
 * Set HSP_SCHED_GLOBAL_CONFIG for the given port
 * Parameters:
 * unit     - (IN) Unit number.
 * port     - (IN) Logical SOC port number.
 * Returns:
 * SOC_E_XXX
 */
STATIC int soc_td2p_hsp_sched_global_config_set(int unit, soc_port_t port)
{
    uint32 rval, fval;
    uint32 field;
    int mmu_port, pipe;
    int hsp_port_index;

    /* Only set this register for the port if the port is HSP */
    if(_soc_trident2_port_sched_type_get(unit, port)
                != SOC_TD2_SCHED_HSP) {
        return SOC_E_NONE;
    }

    rval = fval = 0;

    SOC_IF_ERROR_RETURN(
            soc_td2_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    field = pipe ? IS_HSP_PORT_IN_YPIPEf : IS_HSP_PORT_IN_XPIPEf;
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, field);
    hsp_port_index = mmu_port - pipe * _TD2_PORTS_PER_PIPE;
    fval |= (1 << hsp_port_index);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, field, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));

    return SOC_E_NONE;
}

/*
* Function:
* soc_td2p_hsp_sched_global_config_clear
* Purpose:
* Clear HSP_SCHED_GLOBAL_CONFIG for the given port
* Parameters:
* unit    - (IN) Unit number.
* port    - (IN) Logical SOC port number.
* Returns:
* SOC_E_XXX
 */
STATIC int soc_td2p_hsp_sched_global_config_clear(int unit, soc_port_t port)
{
    uint32 rval, fval;
    uint32 field;
    int mmu_port, pipe;
    int hsp_port_index;

    rval = fval = 0;

    SOC_IF_ERROR_RETURN(
            soc_td2_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    field = pipe ? IS_HSP_PORT_IN_YPIPEf : IS_HSP_PORT_IN_XPIPEf;
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, field);
    hsp_port_index = mmu_port - pipe * _TD2_PORTS_PER_PIPE;
    fval &= ~(1 << hsp_port_index);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, field, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));

    return SOC_E_NONE;
}

/*
* Function:
* soc_td2p_mmu_port_resource_set
* Purpose:
* Reconfigure MMU for flexport operation
* Parameters:
* unit       - (IN) Unit number.
* port       - (IN) Logical SOC port number.
* speed      - (IN) port speed. 
* legacyFlex - (IN) Whether this is a legacy flex or new flex 
* Returns:
* SOC_E_XXX
 */
int soc_td2p_mmu_port_resource_set(int unit, soc_port_t port, int speed, int legacyFlex)
{
    int ets_mode = 0;
    /* Flex port operation not allowed on CPU or loopback port
 */
     if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
         return SOC_E_PARAM;
     }

    /* Update the logical port and queue mappings in memory and registers */
    if (td2p_ets_mode_cb) {
        ets_mode = td2p_ets_mode_cb(unit, port);
    }

    /* Alloc the bitmaps for L0, L1, L2 nodes for this port */
    SOC_IF_ERROR_RETURN(soc_trident2_lls_bmap_alloc_port(unit, port, ets_mode));

    SOC_IF_ERROR_RETURN(soc_trident2p_num_cosq_init_port(unit, port));

    /* Configure all MMU registers for new ports whose setting is
     * based on port speed.
 */
    SOC_IF_ERROR_RETURN(soc_trident2_port_speed_update(unit, port, speed));

    /* Program PORT_INITIAL_COPY_COUNT_WIDTH
 */
    SOC_IF_ERROR_RETURN(soc_td2p_port_icc_width_set(unit, port));
    

    if (!ets_mode) {
        /* Default Scheduling on the port.  The function
         * below also sets HSP_SCHED_GLOBAL_CONFIG for high speed ports.
         * This needs to be done regardless of if it's default or
         * custom scheduling. Custom scheduling is handled below.
 */
        SOC_IF_ERROR_RETURN(soc_trident2_port_sched_set(unit, port));
    } else {
        /* Custom scheduling on the port.
         * If the port is HSP, set HSP_SCHED_GLOBAL_CONFIG so the 
         * device knows to use HSP scheduling for that port. The
         * function below checks if the port is HSP internally.
 */
         SOC_IF_ERROR_RETURN(
             soc_td2p_hsp_sched_global_config_set(unit, port));
    }

    /* Reconfigure IPMC replication
 */
    SOC_IF_ERROR_RETURN(soc_td2p_ipmc_repl_init(unit, port));

    /* Reconfigure THDI, THDO settings
 */
    /* legacy flex has already set up these pg and queue settings for the port
       on device init. Calling this will overwrite any settings with
       default settings. */
    if(legacyFlex == 0) {
        SOC_IF_ERROR_RETURN(soc_td2p_mmu_config_init_port(unit, port));
    }

    return SOC_E_NONE;
}

/*
* Function:
* soc_td2p_mmu_port_resource_clear
* Purpose:
* Reconfigure MMU for flexport operation
* Parameters:
* unit    - (IN) Unit number.
* port    - (IN) Logical SOC port number.
* Returns:
* SOC_E_XXX
 */
int soc_td2p_mmu_port_resource_clear(int unit, soc_port_t port)
{
    int ets_mode = 0;
    soc_trident2_sched_type_t sched_type;

    if (td2p_ets_mode_cb) {
        ets_mode = td2p_ets_mode_cb(unit, port);
    }

    sched_type = _soc_trident2_port_sched_type_get(unit, port);
    if (sched_type == SOC_TD2_SCHED_HSP) {
        SOC_IF_ERROR_RETURN(soc_td2p_hsp_sched_global_config_clear(unit, port));
    }

    /* In ETS mode, SDK does not initialize or deletes the default LLS hierarchy */
    if (!ets_mode) {
        SOC_IF_ERROR_RETURN(soc_td2_lls_port_uninit(unit, port));
    }
    SOC_IF_ERROR_RETURN(soc_td2_lls_reset_port(unit, port));

    SOC_IF_ERROR_RETURN(soc_td2p_port_sched_hsp_set(unit, port, 0));
    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_is_any_port_flex_enable
* Purpose:
*     Find out if
*     port_flex_enable=1 OR
*     port_flex_enable{physical port num}=1
*     config is present in config.bcm
* Parameters:
*     unit    - (IN) Unit number.
* Returns:
*     TRUE or FALSE
*     If such a config is present, this function
*     returns 1 - implying that flex is enable on
*     at least 1 port (port macro) in the system
*     This function returns 0, if flex is not enabled
*     on any port (port macro) in the system
 */
int soc_td2p_is_any_port_flex_enable(int unit)
{
    int phy_port, num_phy_port;

    num_phy_port = 130;

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        if (soc_property_phys_port_get(unit,
                                       phy_port,
                                       spn_PORT_FLEX_ENABLE, 0)) {
            return TRUE;
        }
    }

    return FALSE;
}

/*
* Function:
*     soc_td2p_is_port_flex_enable
* Purpose:
*     Find out if port_flex_enable{physical port num} config
*     is present on the given physical port in config.bcm
* Parameters:
*     unit    - (IN) Unit number.
*     phy_port - (IN) physical port num
* Returns:
*     TRUE or FALSE
 */
int soc_td2p_is_port_flex_enable(int unit, int phy_port)
{
    int enable = 0;

    /* This property does not apply to cpu port &
     * loopback port
 */

    if (phy_port > 0 && phy_port < 129) {
        enable = soc_property_phys_port_get(unit,
                                            phy_port,
                                            spn_PORT_FLEX_ENABLE, 0);
    }

    return enable;
}

/*
* Function:
*     soc_td2p_set_flexport_port_q_callback
* Purpose:
*     Register a callback for the bcm layer so that the soc layer can call it
*     to update logical port and queue associations
* Parameters:
*     unit    - (IN) Unit number.
*     cb      - (IN) Function callback
* Returns:
*     SOC_E_XXX
 */
int soc_td2p_set_flexport_port_q_callback(int unit, 
                                          _soc_td2p_flexport_port_q_cb cb)
{
   td2p_flexport_cb = cb;
   return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_set_ets_mode_q_callback
* Purpose:
*     Register a callback for the bcm layer so that the soc layer can call it
*     and check if ets mode is enabled or not
* Parameters:
*     unit    - (IN) Unit number.
*     cb      - (IN) Function callback
* Returns:
*     SOC_E_XXX
 */
int soc_td2p_set_ets_mode_q_callback(int unit, 
                                          _soc_td2p_ets_mode_q_cb cb)
{
   td2p_ets_mode_cb = cb;
   return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of TSC4
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each TSC4 can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  10G  10G  10G  10G
 *   tri_012 port  10G  10G  20G   x
 *   tri_023 port  20G   x   10G  10G
 *      dual port  20G   x   20G   x
 *    single port  40G   x    x    x
 */
int
soc_td2p_port_mode_get(int unit, int logical_port, int *mode)
{
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[_TD2_PORTS_PER_XLP];

    si = &SOC_INFO(unit);
    first_phyport = si->port_l2p_mapping[logical_port];

    for (lane = 0; lane < _TD2_PORTS_PER_XLP; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TD2_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_TD2_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_TD2_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_TD2_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_TD2_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_mcq_entries_rsvd_port
* Purpose:
*     For a given port, soc_td2p_mcq_entries_rsvd_port calculates
*     the number of MC q entries (Reserved Egress Queue Entries).
*     This function only gives MC q entries for
*     for flex disabled ports.
*     For flex enabled port macros, this function is not used.
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) logical port num
* Returns:
*     Reserved Egress Queue Entries for a given port
 */
int soc_td2p_mcq_entries_rsvd_port(int unit, int port)
{
    int port_num_cosq;
    soc_info_t *si;
    int mcq_entry = 0;

    si = &SOC_INFO(unit);
    port_num_cosq = si->port_num_cosq[port];

    if (IS_LB_PORT(unit, port)) {
        /* Loopback port has 9 queues (including QCN)
         * For calculation of Egress queue entries,
         * loopback port is considered having 8 queues
         * (Why? :- ask Architecture)
 */
        port_num_cosq = si->port_num_cosq[port] - 1;
    }

    mcq_entry = port_num_cosq * _TD2P_MMU_PER_COSQ_EGRESS_QENTRY_RSVD;
    return mcq_entry;
}

/*
* Function:
*     soc_td2p_egr_buf_rsvd_port
* Purpose:
*     For Cpu port or loopback port,
*     soc_td2p_egr_buf_rsvd_port calculates
*     the reserved queue min/guarantee. 
*     This function only gives cumulative (for all queues)
*     queue guarantee for CPU port or Loopback port.
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) logical port num for CPU port or LB port
* Returns:
*     Reserved Cumulative (total for all queues) Queue Min/Guarantee value
 */
int soc_td2p_egr_buf_rsvd_port(int unit, int port, int default_mtu_cells)
{
    int port_num_cosq;
    soc_info_t *si;
    int egr_rsvd = 0;
    int min_cell_per_mcq;

    if (!IS_CPU_PORT(unit, port) && !IS_LB_PORT(unit, port)) {
        return 0;
    }

    si = &SOC_INFO(unit);

    port_num_cosq = si->port_num_cosq[port];

    if (IS_LB_PORT(unit, port)) {
        /* Loopback port has 9 queues (including QCN)
         * For calculation of Egress queue entries,
         * loopback port is considered having 8 queues
         * (Why? :- ask Architecture)
 */
        port_num_cosq = si->port_num_cosq[port] - 1;
    }

    if (soc_feature(unit, soc_feature_min_cell_per_queue)) {
        min_cell_per_mcq = _soc_td2_min_cell_rsvd_per_mcq(unit, port, default_mtu_cells);
    } else {
        min_cell_per_mcq = default_mtu_cells;
    }

    egr_rsvd = port_num_cosq * min_cell_per_mcq;
    return egr_rsvd;
}

/*
 * Function:
 *      _soc_td2p_lb_port_flush
 * Purpose:
 *      Poll until LB port gets back all the credits from EP
 * Parameters:
 *      unit           - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_lb_port_flush(int unit)
{
#define MMU_LB_PORT_BOOT_TIME_CREDITS 36

    soc_info_t *si = &SOC_INFO(unit);
    soc_timeout_t to;
    int lb_port_credits;
    uint32 lbval;
    int mmu_lb_port;

    mmu_lb_port = si->port_p2m_mapping[si->port_l2p_mapping[si->lb_port]];
    /* Adjust for pipe */
    mmu_lb_port = mmu_lb_port &0x3f;

    soc_timeout_init(&to, 250000, 0);

    /* Poll until EDB buffer is empty */
    for (;;) {
        SOC_IF_ERROR_RETURN
            (READ_ES_PIPE1_MMU_PORT_CURRENT_CREDITr(unit, mmu_lb_port, &lbval));
        lb_port_credits = soc_reg_field_get(unit, ES_PIPE1_MMU_PORT_CURRENT_CREDITr,
                lbval, CREDITf);
        if (lb_port_credits >= MMU_LB_PORT_BOOT_TIME_CREDITS) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "EDB buffer drain timeout: "
                                "port %d, %s, "
                                "timeout port %d credit %d\n"),
                     unit, SOC_PORT_NAME(unit, si->lb_port), mmu_lb_port, lb_port_credits));
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

#define _TD2P_XPIPE (0)
#define _TD2P_YPIPE (1)
/*
 * Function:
 *      _soc_td2p_tdm_cpulb_port_indices_get
 * Purpose:
 *      This function finds & returns the slot number (location of) 
 *      loopback port in the active TDM calendar
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pipe           - (IN) pipe number. 0 - xpipe, 1 - ypipe.
 *      table_num      - (IN) Active TDM Table number - 0 or 1
 *      slot_len       - (IN) Current length of the Active TDM table
 *      *arr           - (OUT) contains location (slot num) of lb port in TDM Cal
 *      count          - (OUT) number(count) of locations of LB port in TDM Cal
 *                             - valid number of entries in *arr
 * Returns:
 *      SOC_E_XXX
 * Note:
 *     This function assumes that the output array (arr) being passed
 *     is initialized & has sufficient memory (_MMU_TDM_LENGTH) to store all possible
 *     locations of loopback port in the TDM Calendar. In short,
 *     before calling this function, please allocate the memory
 *     (uint32 * _MMU_TDM_LENGTH) for arr
 */
STATIC int
_soc_td2p_tdm_cpulb_port_indices_get(int unit, int pipe, int table_num,
                                  int slot_len, uint32 *arr, uint32 *count)
{
    static const soc_mem_t mmu_tdm_mems[2][2] = {
        {ES_PIPE0_TDM_TABLE_0m, ES_PIPE0_TDM_TABLE_1m},
        {ES_PIPE1_TDM_TABLE_0m, ES_PIPE1_TDM_TABLE_1m}
    };
    static const soc_mem_t mmu_tdm_fields[2] = {
        PORT_NUM_EVENf, PORT_NUM_ODDf};

    soc_info_t *si;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t field;
    soc_mem_t mem;
    uint32 fval;
    int phy_port, log_port, mmu_port;
    int slot, index;
    int temp_count;

    si = &SOC_INFO(unit);

    if (!arr || !count) {
        return SOC_E_NONE;
    }
    if ((pipe != _TD2P_XPIPE) && (pipe != _TD2P_YPIPE)) {
        return SOC_E_PARAM;
    }

    temp_count = 0;
    mem = mmu_tdm_mems[pipe][table_num];
    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Cal GET Pipe %d table_num %d slot_len %d mem %d \n"),
             pipe, table_num, slot_len, mem));

    for (slot = 0; slot < slot_len; slot += 2) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
        for (index = 0; index < 2; index++) {
            if (slot + index >= slot_len){
                break;
            }
            field = mmu_tdm_fields[index];
            fval = soc_mem_field32_get(unit, mem, entry, field);
            mmu_port = fval + si->mmu_port_base[pipe];
            phy_port = si->port_m2p_mapping[mmu_port];
            if (phy_port != -1) {
                log_port = si->port_p2l_mapping[phy_port];
                if (((pipe == _TD2P_XPIPE) && IS_CPU_PORT(unit, log_port)) ||
                    ((pipe == _TD2P_YPIPE) && IS_LB_PORT(unit, log_port))) {
                    LOG_VERBOSE(BSL_LS_SOC_MMU,
                            (BSL_META_U(unit,
                                        "Pipe %d TDM Calendar %d slot %d"
                                        " mmu %d phy %d log %d fval 0x%x\n"),
                             pipe, table_num,
                             slot+index,
                             mmu_port,
                             phy_port, log_port, fval));
                    arr[temp_count] = slot+index;
                    temp_count++;
                }
            }
        }
    }
    *count = temp_count;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_tdm_cpulb_port_indices_set
 * Purpose:
 *      This function sets the given slot number (locations)
 *      in active TDM calendar with the value passed in the arguments.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pipe           - (IN) pipe number. 0 - xpipe, 1 - ypipe.
 *      table_num      - (IN) Active TDM Table number - 0 or 1
 *      slot_len       - (IN) Current length of the Active TDM table
 *      *arr           - (IN) contains location (slot num) of TDM Cal
 *      count          - (IN) number(count) of valid locations of TDM Cal
 *                             - valid number of entries in *arr
 *      value          - (IN) value to be set
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_tdm_cpulb_port_indices_set(int unit, int pipe, int table_num, int slot_len,
                                  uint32 *arr, uint32 count, int value)
{

    static const soc_mem_t mmu_tdm_mems[2][2] = {
        {ES_PIPE0_TDM_TABLE_0m, ES_PIPE0_TDM_TABLE_1m},
        {ES_PIPE1_TDM_TABLE_0m, ES_PIPE1_TDM_TABLE_1m}
    };
    static const soc_mem_t mmu_tdm_fields[2] ={
        PORT_NUM_EVENf, PORT_NUM_ODDf
    };

    int i, offset;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_t field;
    soc_mem_t mem;
    int slot, index;

    if (!arr) {
        return SOC_E_NONE;
    }
    if ((pipe != _TD2P_XPIPE) && (pipe != _TD2P_YPIPE)) {
        return SOC_E_PARAM;
    }

    mem = mmu_tdm_mems[pipe][table_num];
    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Cal SET Pipe %d table_num %d slot_len %d mem %d \n"),
             pipe, table_num, slot_len, mem));

    for (i = 0; i < count; i++) {
        if (arr[i] >= slot_len){
            break;
        }
        index = arr[i];
        offset = index % 2;
        slot = index - offset;
        LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "Writing TDM Cal %d slot %d value 0x%x\n"),
                 table_num, slot+offset, value));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, slot/2, entry));
        field = mmu_tdm_fields[offset];
        soc_mem_field32_set(unit, mem, entry, field, value);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot / 2, entry));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_mmu_map_phy_port_to_cpulb
 * Purpose:
 *      This function temporarily maps old physical port to
 *      CPU/loopback port in the mmu port mapping registers
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pipe           - (IN) Pipe number.
 *      count          - (IN) Number of ports to map.
 *      pport          - (IN) Physical port number list.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_mmu_map_phy_port_to_cpulb(int unit, int pipe,
                                 int count, int *pport)
{
    int i;
    uint32 rval;
    int old_phy_port;
    int cpulb_port;
    soc_info_t *si = &SOC_INFO(unit);

    cpulb_port = (pipe == _TD2P_XPIPE)? si->cmic_port : si->lb_port;
    for (i = 0; i < count; i++) {
        LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU RQE Phy port remapping "
                            "old physical_port=%d\n"),
                            pport[i]));

        /* MMU_PORT_TO_PHY_PORT_MAPPING [MMU CPU/LB port number]
         * = old physical port number
 */
        old_phy_port = pport[i];

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                PHY_PORTf, old_phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr,
                           cpulb_port, 0, rval));

    }

    /* MMU_PORT_TO_PHY_PORT_MAPPING [MMU CPU/LB port number]
     * = physical CPU/LB port number */
    rval = 0;
    soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
            PHY_PORTf, si->port_l2p_mapping[cpulb_port]);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr,
                       cpulb_port, 0, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_mmu_rqe_phy_port_mapping_update
 * Purpose:
 *      This function maps the old physical ports
 *      temporarily to the CPU/Loopback port
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pipe           - (IN) pipe number of the ports. 0 - xpipe, 1 - ypipe.
 *      count          - (IN) number of ports.
 *      phy_port       - (IN) list of physical port number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Ports in the list should be in the same pipe.
 *      - The goal of this function is to minimize the time CPU/loopback port
 *        is down. Hence, minimal operations are done between
 *        Turning off Opportunistic Scheduling for the CPU/Loopback port
 *        and
 *        Turning ON Opportunistic Scheduling for the CPU/Loopback port
 */
STATIC int
soc_td2p_mmu_rqe_phy_port_mapping_update(int unit, int pipe,
                                         int count, int *phy_port)
{
    static const soc_reg_t mmu_tdm_regs[2] = {
        ES_PIPE0_TDM_CONFIGr, ES_PIPE1_TDM_CONFIGr};
    static const soc_mem_t cal_end_field[2] = {
        CAL0_ENDf, CAL1_ENDf };
    static const soc_mem_t cal_end_single_field[2] = {
        CAL0_END_SINGLEf, CAL1_END_SINGLEf};
    soc_reg_t mmu_tdm_reg;
    uint32 cpulb_port_tdm_indices[_MMU_TDM_LENGTH];
    uint32 mmu_tdm_reg_rval;
    soc_info_t *si;
    uint32 cnt = 0;
    int cpulb_lport;
    int mmu_cpulb_port;
    int table_num;
    int slot_len;


    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Start MMU RQE phy port remapping\n")));

    if ((pipe != _TD2P_XPIPE) && (pipe != _TD2P_YPIPE)) {
        return SOC_E_PARAM;
    }
    /* Get all the fields here from Pipe TDM CONFIG
     * to avoid SBUS access in every
     * subsequent function to essentially
     * get the same information */
    mmu_tdm_reg = mmu_tdm_regs[pipe];
    mmu_tdm_reg_rval = 0;
    SOC_IF_ERROR_RETURN(
            soc_reg32_get(unit, mmu_tdm_reg, REG_PORT_ANY, 0, &mmu_tdm_reg_rval));
    table_num = soc_reg_field_get(
                unit, mmu_tdm_reg, mmu_tdm_reg_rval, CURR_CALf);
    slot_len = (soc_reg_field_get(
                unit, mmu_tdm_reg,
                mmu_tdm_reg_rval, cal_end_field[table_num]) + 1) * 2;
    slot_len -= soc_reg_field_get(
                unit, mmu_tdm_reg,
                mmu_tdm_reg_rval, cal_end_single_field[table_num]);

    si = &SOC_INFO(unit);
    cpulb_lport = (pipe == _TD2P_XPIPE)? si->cmic_port : si->lb_port;
    mmu_cpulb_port = si->port_p2m_mapping[si->port_l2p_mapping[cpulb_lport]];
    /* Adjust for pipe */
    mmu_cpulb_port = mmu_cpulb_port &0x3f;

    /* Get the location of (slot numbers) CPU/loopback port in the active TDM calendar */
    SOC_IF_ERROR_RETURN(
            _soc_td2p_tdm_cpulb_port_indices_get(unit, pipe, table_num,
                slot_len, cpulb_port_tdm_indices,
                &cnt));

    /* Turn off Opportunistic Scheduling for the CPU/Loopback port */
    soc_reg_field_set(unit, mmu_tdm_reg, &mmu_tdm_reg_rval, OPP_CPULB_ENf, 0);
    SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, mmu_tdm_reg,
                          REG_PORT_ANY, 0, mmu_tdm_reg_rval));

    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Writing 0x3f to CPU/LB port locations\n")));

    /* Write to the active TDM calendar table,
     * changing all instances of the CPU/LB port to 0x3f */
    SOC_IF_ERROR_RETURN(
            _soc_td2p_tdm_cpulb_port_indices_set(unit, pipe, table_num, slot_len,
                cpulb_port_tdm_indices, cnt, 0x3f));

    if (pipe == _TD2P_XPIPE) {
        /* Stop EDB drain to the CMIC */
        (void) WRITE_CMIC_RXBUF_BLOCK_DATABUF_ALLOCr(unit, 1);
    } else {
        /* Waiting for LB port to be empty */
        SOC_IF_ERROR_RETURN(
                _soc_td2p_lb_port_flush(unit));
    }

    /* Temporarily map each old physical port number to the CPU/Loopback Port MMU port */
    SOC_IF_ERROR_RETURN(
            _soc_td2p_mmu_map_phy_port_to_cpulb(unit, pipe, count, phy_port));

    if (pipe == _TD2P_XPIPE) {
        /* Resume EDB drain to the CMIC */
        (void) WRITE_CMIC_RXBUF_BLOCK_DATABUF_ALLOCr(unit, 0);
    }

    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Restoring TDM Cal to original\n\n")));
    /* Write to the active TDM calendar table,
     * changing all instances of the LB port to mmu port for LB port */
    SOC_IF_ERROR_RETURN(
            _soc_td2p_tdm_cpulb_port_indices_set(unit, pipe, table_num, slot_len,
                cpulb_port_tdm_indices, cnt, mmu_cpulb_port));


    /* Turn ON Opportunistic Scheduling for the Loopback port */
    soc_reg_field_set(unit, mmu_tdm_reg, &mmu_tdm_reg_rval, OPP_CPULB_ENf, 1);
    SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit,
                          mmu_tdm_reg, REG_PORT_ANY, 0, mmu_tdm_reg_rval));


    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "End MMU RQE phy port remapping\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_mmu_rqe_phy_port_mapping_set
 * Purpose:
 *      This function maps the old physical ports
 *      (that are either being deleted or remapped
 *      in the flexport sequence)
 *      temporarily to the CPU/Loopback port
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 *      - The goal of this function is to minimize the time loopback port
 *        is down. Hence, minimal operations are done between
 *        Turning off Opportunistic Scheduling for the CPU/Loopback port
 *        and
 *        Turning ON Opportunistic Scheduling for the CPU/Loopback port
 */
int
soc_td2p_mmu_rqe_phy_port_mapping_set(int unit,
                                  int pre_count,
                                  soc_port_resource_t *pre_resource)
{
    int *pport;
    int i, px_cnt = 0, py_cnt = 0;
    soc_port_resource_t *pr;
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_MMU,
            (BSL_META_U(unit,
                        "Start MMU RQE phy port remapping\n")));
    if (pre_count <= 0) {
        return SOC_E_NONE;
    }
    /* sort pre_resource based on port pipe. */
    pport = sal_alloc(pre_count * sizeof(int), "phy port list");
    if (!pport) {
        return SOC_E_MEMORY;
    }
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {
        if (pr->physical_port > _TD2_PORTS_PER_PIPE) {
            py_cnt++;
            pport[pre_count - py_cnt] = pr->physical_port;
        } else {
            pport[px_cnt] = pr->physical_port;
            px_cnt++;
        }
    }

    /* Do remapping for ports in x-pipe and y-pipe respectively. */
    if (px_cnt > 0) {
        rv = soc_td2p_mmu_rqe_phy_port_mapping_update(unit, _TD2P_XPIPE,
                                                     px_cnt, pport);
    }
    if (SOC_FAILURE(rv)) {
        goto cleanup;
    }

    if (py_cnt > 0) {
        rv = soc_td2p_mmu_rqe_phy_port_mapping_update(unit, _TD2P_YPIPE,
                                                     py_cnt, pport + px_cnt);
    }

cleanup:
    sal_free(pport);

    if (SOC_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "Error happen during MMU RQE phy port remapping\n")));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "End MMU RQE phy port remapping\n")));
    }

    return rv;
}

#endif /* BCM_TRIDENT2PLUS_SUPPORT */
