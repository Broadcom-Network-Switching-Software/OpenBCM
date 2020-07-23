/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_ep_init.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_ep_init.h>
#include <soc/flexport/flexport_common.h>
#include <soc/init/tomahawk3_tdm.h>

/*** START SDK API COMMON CODE ***/


#define TH3_EP_SLOT_LAT_DPP_LATENCY_W_EFP    51
#define TH3_EP_SLOT_LAT_DPP_LATENCY_WO_EFP   37
#define TH3_EP_SLOT_LAT_CREDIT_LOOP          12
#define TH3_EP_SLOT_LAT_EXTRA_LAT_EARB_DPP    3
#define TH3_EP_SLOT_LAT_DOMAIN_CROSSING       3
#define TH3_EP_SLOT_LAT_OUTSTANDING_CREDIT   21
#define TH3_EP_SLOT_LAT_DEF_MARGIN            3
#define TH3_EP_SLOT_LAT_MAX_ACCUM_DPPR_1      4
#define TH3_EP_MAX_CT_CLASSES                16
#define TH3_EP_NUM_CT_CLASSES                 8

/* EP2MMU credits for Aux ports */
#define TH3_EP_EP2MMU_CRED_CPU               13
#define TH3_EP_EP2MMU_CRED_MGM                8
#define TH3_EP_EP2MMU_CRED_LBK               48

#define TH3_EP_XMIT_START_CNT_MGM            12

/*! @file tomahawk3_ep_init.c
 *  @brief EP (EDB) Init/FlexPort APIs for Tomahawk2.
 *  Details are shown below.
 */


/* EDB Credits and XMIT_CNT Table  */
static const soc_th3_ep_core_cfg_t
    soc_th3_ep_core_cfg_tbl[] = {
   /* speed   ct    ep2mmu   ep2mmu     xmit_cnt
   *         class  credits  credits
   *                general optimized   saf  ct
   */
    {     0,   0,     0,       0,     {{ 0,  0}} },   /*  SAF  */
    { 10000,   1,     8,       7,     {{ 4, 23}} },   /* 10GE  */
    { 25000,   2,    15,      11,     {{10, 43}} },   /* 25GE  */
    { 40000,   3,    20,      14,     {{16, 26}} },   /* 40GE  */
    { 50000,   4,    28,      20,     {{20, 85}} },   /* 50GE  */
    {100000,   5,    57,      42,     {{23, 54}} },   /* 100GE */
    {200000,   6,   100,      71,     {{28, 65}} },   /* 200GE */
    {400000,   7,   142,     110,     {{29, 57}} }    /* 400GE */
};


/*! @fn int soc_tomahawk3_div_round_up(
 *    int nominator,
 *    int denominator)
 *  @param int nominator
 *  @param int denominator
 *  @brief API for round int division
 * Description:
 *      round int division
 */
static int soc_tomahawk3_div_round_up(
    int nominator,
    int denominator)
{
    if (denominator == 0) { return 1; }

    return ( (nominator + denominator - 1) / denominator );
}


/*! @fn int soc_tomahawk3_div_round(
 *    int nominator,
 *    int denominator)
 *  @param int nominator
 *  @param int denominator
 *  @brief API for round int division
 * Description:
 *      round int division
 */
int soc_tomahawk3_div_round(
    int nominator,
    int denominator)
{
    int modulo;

    if (denominator == 0) { return 1; }

    modulo = nominator % denominator;

    if (modulo >= ((denominator + 1) / 2)) {
        return ( (nominator + denominator - 1) / denominator );
    } else {
        return ( nominator / denominator );
    }
}


/*! @fn int soc_tomahawk3_get_set_slot_pipeline_latency()
 * Return Value:
 *      SOC_E_*
 * Description:
 * Input parameters:
 * Core frequency
 * Dpp Frequency
 * DPP Latency (DPPL)
 * Extra latency in DPP clk (A)
 * Domain crossing for output event fifo(B)
 * N - Credit loop (12)
 * C - Outstanding Credit (20)
 * Default margin (D)
 * Early retrieval in core clk (E)
 */
int soc_tomahawk3_get_set_slot_pipeline_latency(
    int core_freq,
    int dpp_freq,
    int dppl,
    int A_in,
    int B_in,
    int N_in,
    int C_in,
    int D_in,
    int E_in,
    int *slot_latency)
{
    int max_accum_latency;

    *slot_latency = soc_tomahawk3_div_round(((dppl + A_in + B_in) * core_freq), dpp_freq);

    max_accum_latency  = (core_freq == dpp_freq) ? TH3_EP_SLOT_LAT_MAX_ACCUM_DPPR_1 :
       soc_tomahawk3_div_round_up(core_freq *
       (C_in + TH3_EP_SLOT_LAT_MAX_ACCUM_DPPR_1), dpp_freq)
        - N_in;

    *slot_latency += D_in + E_in + max_accum_latency;

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_set_slot_pipeline_config(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @brief API to initialize EPOST_SLOT_PIPELINE_CONFIG
 * Description:
 *      API to initialize EPOST_SLOT_PIPELINE_CONFIG
 *      Regs/Mems configured: EP-EARB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_slot_pipeline_config(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 rval;
    int dpp_clks_latency = 0;
    int core_clks_latency, bypass_mode = 0;
    int core_clock_freq, dpp_clock_freq;

    reg = EPOST_SLOT_PIPELINE_CONFIGr;

    /*
     * slot_latency = ROUND[ (DPPL + A + B) * R] + M + D
     * DPPL - DPP Latency                     (FULL = 57; EFP BYPASS = 44)
     * A    - Latency due to async interfaces in between Earb and DPP (6)
     * B    - Domain crossing for output event fifo                   (3)
     * M    - Maximum latency  - see formula
     * D    - Default margin. This should be the minimal number that the
     *         DPP can work properly in order to have minimal latency (5)
     * R    - dpp ratio = (Foreq_core / Freq_dpp)
     *
     *
     * M = ( R == 1) ? 4 : ROUND_UP[R*(N*(R-1)/R + C - N  + 4)]
     *    N - Credit loop (12)
     *    C - Outstanding Credit (20)
     */


    /* Set the latency in Slot Pipeline config register, need to be
     * programmed before any traffic going through the EP */
    rval = 0;
    bypass_mode = soc_property_get(unit, spn_SWITCH_BYPASS_MODE, SOC_SWITCH_BYPASS_MODE_NONE);
    if (bypass_mode == 3) {
        /* w/o EFP */
        dpp_clks_latency = TH3_EP_SLOT_LAT_DPP_LATENCY_WO_EFP;
    } else {
        /* w/ EFP */
        dpp_clks_latency = TH3_EP_SLOT_LAT_DPP_LATENCY_W_EFP;
    }

    core_clock_freq = port_schedule_state->frequency;
    dpp_clock_freq = port_schedule_state->in_port_map.port_p2PBLK_inst_mapping[0];

    soc_tomahawk3_get_set_slot_pipeline_latency(
        core_clock_freq,
        dpp_clock_freq,
        dpp_clks_latency,                     /* DPP Latency (DPPL) */
        TH3_EP_SLOT_LAT_EXTRA_LAT_EARB_DPP,   /* Extra latency in DPP clk (A) */
        TH3_EP_SLOT_LAT_DOMAIN_CROSSING,      /* Domain crossing for output event fifo(B) */
        TH3_EP_SLOT_LAT_CREDIT_LOOP,          /* N - Credit loop (12) */
        TH3_EP_SLOT_LAT_OUTSTANDING_CREDIT,   /* C - Outstanding Credit (20) */
        TH3_EP_SLOT_LAT_DEF_MARGIN,           /*  Default margin (D) */
        0,                                    /* Early retrieval in core clk (E); 0 for EP*/
        &core_clks_latency);

    soc_reg_field_set(unit, reg, &rval, WR_ENf, 0);
    soc_reg_field_set(unit, reg, &rval, LATENCYf, core_clks_latency);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    soc_reg_field_set(unit, reg, &rval, WR_ENf, 1);
    soc_reg_field_set(unit, reg, &rval, LATENCYf, core_clks_latency);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_set_dev_to_phy(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPING
 * Description:
 *      API to initialize/clear per port EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPING
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_dev_to_phy(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    soc_mem_t mem;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int logical_port;

    mem = EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm;
    if (0 == is_port_down) {
        logical_port = port_schedule_state->out_port_map.port_p2l_mapping[phy_port];
        memfld = phy_port;
    } else {
        logical_port = port_schedule_state->in_port_map.port_p2l_mapping[phy_port];
        memfld = 0x1FF;
    }
    sal_memset(entry, 0, sizeof(uint32) *
           soc_mem_entry_words(unit, EDB_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGm));
    soc_mem_field_set(unit, mem, entry, DATAf, &memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, logical_port,
                                  entry));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_ep_set_mmu_cell_credit(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EGR_MMU_CELL_CREDIT
 * Description:
 *      API to initialize/clear per port EGR_MMU_CELL_CREDIT
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_mmu_cell_credit(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    if (0 == is_port_down) { /* port up */
        memfld = soc_tomahawk3_ep_get_ep2mmu_credit(unit, port_schedule_state,
                                                    phy_port);
    } else {                /* port down */
        memfld = 0;
    }

    mem = EGR_MMU_CELL_CREDITm;
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_MMU_CELL_CREDITm));
    soc_mem_field_set(unit, mem, entry, CREDITf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port,
                                      entry));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_get_freq_encoding(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  Calculates index into xmit_cnt_per_freq array
 *  Return Value:
 *      SOC_E_*
 */
static int soc_tomahawk3_get_freq_encoding(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int *freq_encoding)
{

    /* All Core/PP frequencies have the same settings for xmit_cnt */
    *freq_encoding = 0;
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_get_speed_encoding(
 *  int speed,
 *  int *speed_encoding)
 *  @param speed Speed
 *  @param *speed_encoding speed encoding
 *  @brief Helper function to get CT class
 */
int
soc_tomahawk3_get_speed_encoding(
    int speed,
    int *speed_encoding)
{
    *speed_encoding = 0;
    switch (speed) {
    case 10000:   *speed_encoding = 1; break;
    case 25000:   *speed_encoding = 2; break;
    case 40000:   *speed_encoding = 3; break;
    case 50000:   *speed_encoding = 4; break;
    case 100000:  *speed_encoding = 5; break;
    case 200000:  *speed_encoding = 6; break;
    case 400000:  *speed_encoding = 7; break;
    default:      return SOC_E_PARAM;  break;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_get_ct_class(int speed)
 *  @param speed Speed
 *  @brief Helper function to get CT class
 */
int
soc_tomahawk3_get_ct_class(int speed)
{
    int speed_encoding;
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_get_speed_encoding(speed, &speed_encoding));
    return soc_th3_ep_core_cfg_tbl[speed_encoding].ct_class;
}

/*! @fn int soc_tomahawk3_ep_get_ep2mmu_credit(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int phy_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Dev port num
 *  @param ovs_ratio ovs_ratio inside {10-LR, 15-OVS 3:2, 20- OVS 2:1}
 *  @brief Helper function to get EP2MMU credit for a port;
 * This API assumes that credits were retrieved for active ports after FlexPort
 */
int
soc_tomahawk3_ep_get_ep2mmu_credit(
    int unit, soc_port_schedule_state_t *port_schedule_state, int phy_port)
{
    int logical_port, speed;
    int ep2mmu_credit = 0;

    if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
        /* Either Init or FlexPort, out_port_map should be used
         * since this function is called only on port up */
        logical_port =
            port_schedule_state->out_port_map.port_p2l_mapping[phy_port];
        speed = port_schedule_state->out_port_map.log_port_speed[logical_port];
        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
                unit, port_schedule_state, speed, &ep2mmu_credit));
    } else if (phy_port == _TH3_PHY_PORT_CPU) {
        ep2mmu_credit = TH3_EP_EP2MMU_CRED_CPU; /* CPU credits for 25G */
    } else if ((phy_port == _TH3_PHY_PORT_MNG0) ||
                 (phy_port == _TH3_PHY_PORT_MNG1) ) {
        ep2mmu_credit = TH3_EP_EP2MMU_CRED_MGM; /* MGM credits for 10G */
    } else if ( (phy_port >=  _TH3_PHY_PORT_LPBK0) &&
                (phy_port <=  _TH3_PHY_PORT_LPBK7) ) {
        ep2mmu_credit = TH3_EP_EP2MMU_CRED_LBK; /* LBK credits for >100G */
    }

    return ep2mmu_credit;
}

/*! @fn int soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int speed, int *credit)
 */
int
soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int speed,
    int *credit)
{
    int speed_encoding;
    int core_clock_freq, dpp_clock_freq;

    core_clock_freq = port_schedule_state->frequency;
    dpp_clock_freq = port_schedule_state->in_port_map.port_p2PBLK_inst_mapping[0];

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_get_speed_encoding(speed, &speed_encoding));

    if ((core_clock_freq == 1325) &&
        (dpp_clock_freq  == 1000)) {  /* PFC optimized for 1325/1000 */
        *credit = soc_th3_ep_core_cfg_tbl[speed_encoding].egr_mmu_credit_pfc_opt;
    } else {
        *credit = soc_th3_ep_core_cfg_tbl[speed_encoding].egr_mmu_credit;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_set_ct_class(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EGR_MMU_CELL_CREDIT
 * Description:
 *      API to initialize/clear per port EGR_MMU_CELL_CREDIT
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_ct_class(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    soc_mem_t mem;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int logical_port, speed;

    /* Only Front Panel ports have CT configured */
    if (!_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) { return SOC_E_NONE; }

    mem = EDB_IP_CUT_THRU_CLASSm;
    logical_port =
        port_schedule_state->out_port_map.port_p2l_mapping[phy_port];
    if (0 == is_port_down) {
        speed = port_schedule_state->out_port_map.log_port_speed[logical_port];
        memfld = soc_tomahawk3_get_ct_class(speed);
    } else {
        /* FlexPort doesn't need to reset the entry when port goes down;
         * needs to be touched when port goes up */
        memfld = 0;
    }
    sal_memset(entry, 0, sizeof(uint32) *
           soc_mem_entry_words(unit, EDB_IP_CUT_THRU_CLASSm));
    soc_mem_field_set(unit, mem, entry, CUT_THRU_CLASSf, &memfld);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, logical_port,
                                  entry));
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_set_xmit_start_cnt(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EDB_XMIT_START_COUNT
 * Description:
 *      API to initialize/clear per port EDB_XMIT_START_COUNT
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_xmit_start_cnt(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    soc_mem_t mem;
    int pipe, ct_class, mem_index;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int xmit_start_cnt[TH3_EP_MAX_CT_CLASSES];
    soc_port_map_type_t *port_map;
    int logical_port;
    static const soc_mem_t ep_xmit_cnt_mems[_TH3_PIPES_PER_DEV] = {
        EDB_XMIT_START_COUNT_PIPE0m, EDB_XMIT_START_COUNT_PIPE1m,
        EDB_XMIT_START_COUNT_PIPE2m, EDB_XMIT_START_COUNT_PIPE3m,
        EDB_XMIT_START_COUNT_PIPE4m, EDB_XMIT_START_COUNT_PIPE5m,
        EDB_XMIT_START_COUNT_PIPE6m, EDB_XMIT_START_COUNT_PIPE7m
    };

    /* Set reg's variables */
    if (1 == is_port_down) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    logical_port = port_map->port_p2l_mapping[phy_port];
    pipe = logical_port / _TH3_DEV_PORTS_PER_PIPE;

    /* Set all XMIT START CNT values for a port; 0-SAF; 1-12 CT classes;
     * 13-15 reserved
     */
    mem = ep_xmit_cnt_mems[pipe];

    if (1 == is_port_down) {
        for (ct_class = 0; ct_class < TH3_EP_MAX_CT_CLASSES; ct_class++) {
            xmit_start_cnt[ct_class] = 0;
        }
    } else {
        soc_tomahawk3_ep_get_xmit_start_count(unit, port_schedule_state,
                                          logical_port,
                                          xmit_start_cnt);
    }

    for (ct_class = 0; ct_class < TH3_EP_NUM_CT_CLASSES; ct_class++) {
        sal_memset(entry, 0, sizeof(uint32) *
                   soc_mem_entry_words(unit, EDB_XMIT_START_COUNT_PIPE0m));
        mem_index = (logical_port % _TH3_DEV_PORTS_PER_PIPE) * TH3_EP_MAX_CT_CLASSES + ct_class;
        memfld = xmit_start_cnt[ct_class];
        soc_mem_field_set(unit, mem, entry, THRESHOLDf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index,
                                          entry));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_get_xmit_start_count(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int logical_port, int *xmit_start_cnt)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *             variable
 *  @param logical_port Dev port num
 *  @param xmit_start_cnt array of 16 entries 0-SAF; 1-12 CT; 13-15 Unused
 *  @brief Helper function to get XMIT_START_CNT array for a port;
 * This API assumes that credits were retrieved for active ports after FlexPort
 */
int
soc_tomahawk3_ep_get_xmit_start_count(
    int  unit, soc_port_schedule_state_t *port_schedule_state,
    int  logical_port, int *xmit_start_cnt)
{
    int dst_ct_class, src_ct_class;
    int speed, speed_encoding, freq_encoding;
    int saf_xmit_start_cnt, dst_ct_xmit_start_cnt;
    int phy_port;

    phy_port = port_schedule_state->out_port_map.port_l2p_mapping[logical_port];

    for (src_ct_class=0; src_ct_class < TH3_EP_MAX_CT_CLASSES; src_ct_class++) {
        xmit_start_cnt[src_ct_class] = 0;
    }

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_get_freq_encoding(unit, port_schedule_state, &freq_encoding));

    /* Front Panel ports have SAF & CT settings */
    if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
        speed = port_schedule_state->out_port_map.log_port_speed[logical_port];
        dst_ct_class = soc_tomahawk3_get_ct_class(speed);

        SOC_IF_ERROR_RETURN(
            soc_tomahawk3_get_speed_encoding(speed, &speed_encoding));

        saf_xmit_start_cnt =
            soc_th3_ep_core_cfg_tbl[speed_encoding].xmit_cnt_per_freq[freq_encoding].saf;
        dst_ct_xmit_start_cnt =
            soc_th3_ep_core_cfg_tbl[speed_encoding].xmit_cnt_per_freq[freq_encoding].ct;

        /* Populate xmit_start_cnt array */
        for (src_ct_class = 0; src_ct_class < TH3_EP_NUM_CT_CLASSES; src_ct_class++) {
            if (src_ct_class < dst_ct_class) {
                xmit_start_cnt[src_ct_class] = saf_xmit_start_cnt;
            } else {
                xmit_start_cnt[src_ct_class] = dst_ct_xmit_start_cnt;
            }
        }
    } else if ((phy_port == _TH3_PHY_PORT_MNG0) ||
               (phy_port == _TH3_PHY_PORT_MNG1) ) { /* 10G SAF */
        xmit_start_cnt[0] =
            TH3_EP_XMIT_START_CNT_MGM;
    } else { /* CMIC & LOOPBACK */
        xmit_start_cnt[0] = 1; /* Low threshold */
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_set_egr_enable(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EGR_ENABLE
 * Description:
 *      API to initialize/clear per port EGR_ENABLE
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_set_egr_enable(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    if (0 == is_port_down ) {
        memfld = 1;
    } else {
        memfld = 0;
    }
    mem = EGR_ENABLEm;
    sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit, EGR_ENABLEm));
    soc_mem_field_set(unit, mem, entry, PRT_ENABLEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port,
                                      entry));
    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_ep_init_egr_enable(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize per CHIP EGR_ENABLE
 * Description:
 *      API to initialize per CHIP EGR_ENABLE
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_init_egr_enable(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    uint32 port, pipe;
    int phy_port;
    int is_port_down;

    is_port_down = 0;

    /* Configure per port registers: Front Panel ports; CPU; Loopback*/
    for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, port_schedule_state,
                   pipe, phy_port, is_port_down));
        }
    }

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_ep_edb_init(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port goes down
 *  @brief API to initialize/clear per port EGR_ENABLE
 * Description:
 *      API to initialize/clear per port EGR_ENABLE
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_edb_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    uint32 port, pipe;
    int phy_port;
    int is_port_down;
    int rst_on;

    /* Toggle per port sft reset first on FP ports that are enabled */
    rst_on = 1; /* assert reset */
    for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0) {
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit,
                                    port_schedule_state, pipe,
                                    phy_port, rst_on));
            }
        }
    }

    rst_on = 0; /* de-assert reset */
    for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0) {
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit,
                                    port_schedule_state, pipe,
                                    phy_port, rst_on));
            }
        }
    }

    is_port_down = 0;
    /* Configure per port registers: Front Panel ports; CPU; Loopback*/
    for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            soc_tomahawk3_ep_set_dev_to_phy(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_mmu_cell_credit(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_ct_class(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_xmit_start_cnt(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
        }
    }

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_ep_init(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to initialize EPIPE
 * Description:
 *      API to initialize EPIPE
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    /* Init EDB; per port config */
    soc_tomahawk3_ep_edb_init(unit, port_schedule_state);

    /* Enable EGR ports */
    soc_tomahawk3_ep_init_egr_enable(unit, port_schedule_state);

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_drain_port(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe_num pipe number.
 *  @param phy_port phy_port to configure.
 *  @brief API to drain phy_port
 * Description:
 *      API to drain phy_port
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_drain_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port)
{
    uint32 rval_save, rval;
    soc_reg_t reg;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 util;
    soc_mem_t mem;
    int timeout_in_us, iter_in_us;

    static const soc_reg_t misc_ctrl_regs[_TH3_PIPES_PER_DEV] = {
        EDB_MISC_CTRL_PIPE0r, EDB_MISC_CTRL_PIPE1r,
        EDB_MISC_CTRL_PIPE2r, EDB_MISC_CTRL_PIPE3r,
        EDB_MISC_CTRL_PIPE4r, EDB_MISC_CTRL_PIPE5r,
        EDB_MISC_CTRL_PIPE6r, EDB_MISC_CTRL_PIPE7r
    };

    /* Configure EGR_EDB_MISC_CTRL such that EGR_MAX_USED_ENTRIES returns
     * current buffer utilization
     */
    reg = misc_ctrl_regs[pipe_num];
    rval_save = 0;
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0,
                                              &rval_save));
    rval = rval_save;
    soc_reg_field_set(unit, reg, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* polling port's buffer utilization until is 0 */
    timeout_in_us = 70; /* account for IDB, MMU, EDB draining */
    iter_in_us = 0;
    sal_memset(entry, 0, sizeof(entry));
    mem = EGR_MAX_USED_ENTRIESm;
    do {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                          phy_port, entry));
        /* coverity[callee_ptr_arith : FALSE] */
        soc_mem_field_get(unit, mem, entry, LEVELf, &util);
        if (0 == util) { break; }

        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        iter_in_us++;
        if (iter_in_us >= timeout_in_us) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "soc_tomahawk3_ep_drain_port()"
                "TIMEOUT of %0d us reached when draining phy_port=%0d\n"), 
                timeout_in_us, phy_port));
            return SOC_E_FAIL;
        }
    } while (0 != util);

    /* Configure EDB_MISC_CTRL back to previous value */
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0,
                                              rval_save));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_ep_credit_rst_port(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe_num pipe number.
 *  @param phy_port phy_port to configure.
 *  @brief API to sft_rst the phy_port
 * Description:
 *      API to reset  EDB credit the phy_port
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_credit_rst_port(
    int unit,
    int phy_port)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    mem = EGR_PORT_CREDIT_RESETm;

    sal_memset(entry, 0, sizeof(entry));
    memfld = 1;
    soc_mem_field_set(unit, mem, entry, VALUEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      phy_port, entry));

    memfld = 0;
    soc_mem_field_set(unit, mem, entry, VALUEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      phy_port, entry));

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_ep_sft_rst_port(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param pipe_num pipe number.
 *  @param phy_port phy_port to configure.
 *  @param rst_on if (rst_on==1) then sft rst the port
 *  @brief API to sft_rst the phy_port
 * Description:
 *      API to sft_rst the phy_port
 *      Regs/Mems configured: EDB
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_ep_sft_rst_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int pipe_num,
    int phy_port,
    int rst_on)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;

    mem = EGR_PER_PORT_BUFFER_SFT_RESETm;

    if (rst_on == 1) {
        memfld = 1;
    } else {
        memfld = 0;
    }

    sal_memset(entry, 0, sizeof(entry));
    soc_mem_field_set(unit, mem, entry, ENABLEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      phy_port, entry));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_ep_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to follow EDB port down sequence for flexport operation.
 *         Does:
 *            (1) Poll until EDB buffer is empty for ports going down
 *            (2) Disables EDB for ports going down
 *            (3) Asserts sft_rst EDB for ports going down
 *
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk3_ep_port_down(int unit,
                           soc_port_schedule_state_t *port_schedule_state)
{
    int i, rst_on, is_port_down;
    int logical_port, physical_port;
    int pipe_num;

    /*
     * Poll until EDB buffer is empty:
     * Set EGR_EDB_MISC_CTRL.SELECT_CURRENT_USED_ENTRIES to 1.
     * Poll until EGR_MAX_USED_ENTRIES[physical_port] is equal to 0.
     */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            /* assumes drain is done before FlexPort */
            physical_port =
                port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_drain_port(unit,
                                    port_schedule_state, pipe_num, physical_port));
        }
    }

    /*
     * Disables EDB to MMU cell request generation
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 0.
     * Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 1.
     */

    is_port_down = 1; /* that is, disable port */
    rst_on = 1;       /* that is, assert sft_rst */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) { /* port DOWN */
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            /* assumes drain is done before FlexPort */
            physical_port =
                port_schedule_state->in_port_map.port_l2p_mapping[logical_port];

            /* Disable port : write EGR_ENABLEm */
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, port_schedule_state,
                                            pipe_num, physical_port, is_port_down));
            /* Assert sft_rst per port */
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, port_schedule_state,
                                            pipe_num, physical_port, rst_on));
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_ep_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that brings up all added ports in EGR pipe
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk3_ep_port_up(int unit,
                         soc_port_schedule_state_t *port_schedule_state)
{
    int i, rst_on, is_port_down;
    int logical_port, physical_port;
    int pipe_num;

    /*
     * Toggle port rst: Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 1
     *                  Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 0
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 1
     */

    rst_on = 1; /* assert reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            /* De-assert sft_rst*/
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, port_schedule_state,
                                            pipe_num, physical_port, rst_on));
        }
    }

    rst_on = 0; /* de-assert reset */
    for (i = 0; i < port_schedule_state->nport; i++) {
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            /* De-assert sft_rst*/
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_sft_rst_port(unit, port_schedule_state,
                                            pipe_num, physical_port, rst_on));
        }
    }

    /* Enable Ports going up after PM sft_rst is de-asserted */
    /* For ports going UP do:
     * 1. Enable port; write EGR_ENABLEm
     */
    is_port_down = 0; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            /* Enable port; write EGR_ENABLEm */
            SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_set_egr_enable(unit, port_schedule_state,
                                            pipe_num, physical_port, is_port_down));
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_ep_reconfigure_remove(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - remove ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk3_flex_ep_reconfigure_remove(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, is_port_down;
    int log_port, phy_port, pipe;

    /* For ports going DOWN do:
     * 1. Delete Dev 2 Phy mapping.
     * 2. Reset max Ep2MMU credits to 0.
     */
    is_port_down = 1; /* that is, port DOWN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        phy_port = port_schedule_state->resource[i].physical_port;
        if (phy_port == -1) { /* that is, port DOWN */
            log_port = port_schedule_state->resource[i].logical_port;
            phy_port = port_schedule_state->in_port_map.port_l2p_mapping[log_port];
            pipe = log_port / _TH3_DEV_PORTS_PER_PIPE;
            soc_tomahawk3_ep_set_dev_to_phy(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_mmu_cell_credit(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_ep_reconfigure_add(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - add ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk3_flex_ep_reconfigure_add(int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, is_port_down;
    int log_port, phy_port, pipe;


    /* For ports going UP do:
     * 1. Map Dev 2 Phy for added port.
     * 2. Configure max Ep2MMU credits to proper value.
     * 3. Configure CT_class for added port.
     * 4. Configure XMIT_START_CNT entries for added port.
     */
    is_port_down = 0; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        phy_port = port_schedule_state->resource[i].physical_port;
        if (phy_port != -1) { /* that is, port UP */
            log_port = port_schedule_state->resource[i].logical_port;
            pipe = log_port / _TH3_DEV_PORTS_PER_PIPE;
            soc_tomahawk3_ep_set_dev_to_phy(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_mmu_cell_credit(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_ct_class(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
            soc_tomahawk3_ep_set_xmit_start_cnt(unit, port_schedule_state,
                   pipe, phy_port, is_port_down);
        }
    }

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */

