/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_ep_flexport.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>
#include <soc/flexport/tomahawk2_flexport.h>


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk2_ep_flexport.c
 *  @brief EP (EDB) FlexPort for Tomahawk2.
 *  Details are shown below.
 */


/* ASF Core Config Table  */
static const soc_th2_ep_core_cfg_t
    soc_th2_ep_core_cfg_tbl[] = {
    /* speed   ep2mmu  ep2_mmu_max    L2_lat      L3 lat       full_lat
     * LR  3:2 2:1   LR  3:2 2:1   LR  3:2 2:1
     */
    {    -1,  0,  0, { 7, 12, 16}, { 7, 12, 16}, { 7, 12, 16} },   /* SAF     */
    { 10000, 10,  9, {29, 40, 48}, {29, 40, 48}, {29, 40, 48} },   /* 10GE    */
    { 11000, 10,  9, {29, 40, 48}, {29, 40, 48}, {29, 40, 48} },   /* HG[11]  */
    { 20000, 17, 11, {30, 41, 49}, {35, 41, 49}, {28, 41, 49} },   /* 20GE    */
    { 21000, 17, 11, {30, 41, 49}, {35, 41, 49}, {28, 41, 49} },   /* HG[21]  */
    { 25000, 17, 13, {30, 41, 49}, {35, 41, 49}, {25, 47, 56} },   /* 25GE    */
    { 27000, 17, 13, {30, 41, 49}, {35, 41, 49}, {25, 47, 56} },   /* HG[27]  */
    { 40000, 26, 17, {38, 44, 52}, {18, 52, 62}, {13, 52, 44} },   /* 40GE    */
    { 42000, 26, 17, {38, 44, 52}, {18, 52, 62}, {13, 52, 44} },   /* HG[42]  */
    { 50000, 28, 19, {38, 50, 59}, {14, 52, 62}, {13, 40, 39} },   /* 50GE    */
    { 53000, 28, 19, {38, 50, 59}, {14, 52, 62}, {13, 40, 39} },   /* HG[53]  */
    {100000, 48, 31, {28, 64, 74}, {13, 34, 34}, {13, 24, 31} },   /* 100GE   */
    {106000, 48, 31, {28, 64, 74}, {13, 34, 34}, {13, 24, 31} }    /* HG[106] */
};


/*! @fn int soc_tomahawk2_ep_flexport_get_max_ovs_ratio(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int *old_max_ovs_ratio, int *new_max_ovs_ratio )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param old_max_ovs_ratio returned pointer to old_max_ovs_ratio.
 *  @param new_max_ovs_ratio returned pointer to new_max_ovs_ratio.
 *  @brief Compute old and new ovs ratio across all Half Pipes
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_get_max_ovs_ratio(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int *old_max_ovs_ratio, int *new_max_ovs_ratio)
{
    int pipe, hpipe;
    int told_max_ovs_ratio, tnew_max_ovs_ratio;
    soc_tomahawk2_flex_scratch_t *cookie;

    /* Find old/new max ovs ratio among the whole half pipes in the device */
    cookie = port_schedule_state->cookie;
    told_max_ovs_ratio = 0;
    tnew_max_ovs_ratio = 0;
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            if (tnew_max_ovs_ratio <
                cookie->new_hpipe_ovs_ratio[pipe][hpipe]) {
                tnew_max_ovs_ratio = cookie->new_hpipe_ovs_ratio[pipe][hpipe];
            }
            if (told_max_ovs_ratio <
                cookie->prev_hpipe_ovs_ratio[pipe][hpipe]) {
                told_max_ovs_ratio = cookie->prev_hpipe_ovs_ratio[pipe][hpipe];
            }
        }
    }

    *old_max_ovs_ratio = told_max_ovs_ratio;
    *new_max_ovs_ratio = tnew_max_ovs_ratio;

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk2_ep_get_ep2mmu_credit(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Dev port num
 *  @param ovs_ratio ovs_ratio inside {10-LR, 15-OVS 3:2, 20- OVS 2:1}
 *  @brief Helper function to get EP2MMU credit for a port;
 * This API assumes that credits were retrieved for active ports after FlexPort
 */
int
soc_tomahawk2_ep_get_ep2mmu_credit(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    int ct_class = 0;
    int speed;
    int old_max_ovs_ratio, new_max_ovs_ratio;
    soc_tomahawk2_flex_scratch_t *cookie;

    soc_tomahawk2_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    /* Get ct_class */
    cookie = port_schedule_state->cookie;
    speed = cookie->exact_out_log_port_speed[logical_port];
    ct_class = soc_tomahawk2_get_ct_class(speed);

    /* At MAX FREQ, the ovs_ratio will always be less than 3:2; 
     * Still add that check
     */
    if ((1700 == port_schedule_state->frequency) &&
        (new_max_ovs_ratio < 20) ) {
        return soc_th2_ep_core_cfg_tbl[ct_class].egr_mmu_credit_max_freq;
    } else {
        return soc_th2_ep_core_cfg_tbl[ct_class].egr_mmu_credit;
    }
}


/*! @fn int soc_tomahawk2_ep_get_ct_xmit_start_cnt(int ct_class, int latency_mode,
 *              int ovs_ratio)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct 
 *         variable
 *  @param latency_mode Latency mode 0-FULL; 1-L3; 2-L2.
 *  @param ovs_ratio ovs_ratio inside {10-LR, 15-OVS 3:2, 20- OVS 2:1}
 *  @brief Helper function to get XMIT_START_CNT value
 */
/*
 * On BCM56960, BCM56970, the switch latency bypass modes availabe are:
 * 0 - normal operation (FULL)
 * 1 - balanced latency L2 + L3 (L3)
 * 2 - low latency L2  (L2)
 */
int
soc_tomahawk2_ep_get_ct_xmit_start_cnt(int ct_class, int latency_mode,
                                       int ovs_ratio)
{
    int xmit_cnt;

    switch (latency_mode) {
    /* FULL latency */
    case 0:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_full_latency.
                oversub_2_1;
        }
        break;
    case 1:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l3_latency.
                oversub_2_1;
        }
        break;
    case 2:
        if (ovs_ratio <= 10) {     /* LR */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.line_rate;
        } else if (ovs_ratio <= 15) {     /* OVS 3:2 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.
                oversub_3_2;
        } else {     /* OVS 2:1 */
            xmit_cnt =
                soc_th2_ep_core_cfg_tbl[ct_class].xmit_cnt_l2_latency.
                oversub_2_1;
        }
        break;
    default: xmit_cnt = 0; break;
    }

    return xmit_cnt;
}


/*! @fn int soc_tomahawk2_ep_get_xmit_start_count(int unit,
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
soc_tomahawk2_ep_get_xmit_start_count(
    int  unit, soc_port_schedule_state_t *port_schedule_state,
    int  logical_port, int *xmit_start_cnt)
{
    int dst_ct_class, src_ct_class;
    int speed;
    int latency_mode;
    int saf_xmit_start_cnt, dst_ct_xmit_start_cnt;
    int old_max_ovs_ratio, new_max_ovs_ratio;
    soc_tomahawk2_flex_scratch_t *cookie;

    cookie = port_schedule_state->cookie;
    speed = cookie->exact_out_log_port_speed[logical_port];
    dst_ct_class = soc_tomahawk2_get_ct_class(speed);

    soc_tomahawk2_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    latency_mode = port_schedule_state->cutthru_prop.switch_bypass_mode;

    /* Populate xmit_start_cnt array */
    saf_xmit_start_cnt = soc_tomahawk2_ep_get_ct_xmit_start_cnt(
                                      0, latency_mode, new_max_ovs_ratio);                                                            /* 0 means SAF */
    dst_ct_xmit_start_cnt = soc_tomahawk2_ep_get_ct_xmit_start_cnt(
        dst_ct_class, latency_mode, new_max_ovs_ratio);
    for (src_ct_class=0; src_ct_class < 13; src_ct_class++) {
        if (src_ct_class < dst_ct_class) {
            xmit_start_cnt[src_ct_class] = saf_xmit_start_cnt;
        } else {
            xmit_start_cnt[src_ct_class] = dst_ct_xmit_start_cnt;
        }
    }
    xmit_start_cnt[13] = 0;
    xmit_start_cnt[14] = 0;
    xmit_start_cnt[15] = 0;

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_sft_rst_pm_intf(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int rst_on)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *             variable
 *  @param rst_on rst_on==1-> ASSERT reset; rst_on==0-> DEASSERT reset
 *  @brief Soft resets the EDB PM interface; EGR_PORT_BUFFER_SFT_RESET_0
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_sft_rst_pm_intf(
    int unit, soc_port_schedule_state_t *port_schedule_state, int rst_on)
{
    uint32 pipe_map;
    int i, pipe, pm_indx, pm_num, subport;
    int logical_port, physical_port, num_lanes;
    uint32 pm_rst_values[_TH2_PIPES_PER_DEV][_TH2_PBLKS_PER_PIPE];

    uint64 rval64, fldval64;
    soc_reg_t reg;
    static const soc_reg_t edb_sft_rst_regs[_TH2_PIPES_PER_DEV] = {
        EGR_PORT_BUFFER_SFT_RESET_0_PIPE0r, EGR_PORT_BUFFER_SFT_RESET_0_PIPE1r,
        EGR_PORT_BUFFER_SFT_RESET_0_PIPE2r, EGR_PORT_BUFFER_SFT_RESET_0_PIPE3r
    };


    /* pipe_map indicates which pipe has flexing ports */
    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    sal_memset(pm_rst_values, 0, sizeof(pm_rst_values));

    /* Compute per PM sft reset;
     * only for PMs that have ports going UP or DOWN
     */
    if (1 == rst_on) {
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port  = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            pipe = logical_port / _TH2_DEV_PORTS_PER_PIPE;
            if (-1 == physical_port) { /* port DOWN; look in prev OVS port map */
                physical_port =
                    port_schedule_state->in_port_map.port_l2p_mapping[
                        logical_port];
                num_lanes =
                    port_schedule_state->in_port_map.port_num_lanes[
                        logical_port];
            } else { /* port UP; look in new OVS port map */
                physical_port =
                    port_schedule_state->out_port_map.port_l2p_mapping[
                        logical_port];
                num_lanes =
                    port_schedule_state->out_port_map.port_num_lanes[
                        logical_port];
            }
            pm_num = (physical_port - 1) / _TH2_PORTS_PER_PBLK; /* device pm_num*/
            pm_indx = (pm_num % _TH2_PBLKS_PER_PIPE);
            /* pm_indx is pipe local PM indx and swapped for odd pipes */
            pm_indx =
                ((pipe % 2) == 0) ? pm_indx : (_TH2_PBLKS_PER_PIPE-1 - pm_indx);
            subport = (physical_port - 1) % _TH2_PORTS_PER_PBLK;
            switch (subport) {
            case 0:
            case 1:
                if (num_lanes == 4) {
                    pm_rst_values[pipe][pm_indx] |= 3;    /* 3'b011 -> 3'b100 */
                } else {
                    pm_rst_values[pipe][pm_indx] |= 1;    /* 3'b001 */
                }
                break;
            case 2:
            case 3:
                pm_rst_values[pipe][pm_indx] |= 2;    /* 3'b010 */
                break;
            default: break;
            }
        }

        /* Make  3'b011 -> 3'b100; that is, full PM rst */
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
            for (pm_indx = 0; pm_indx < _TH2_PBLKS_PER_PIPE; pm_indx++) {
                if (3 == pm_rst_values[pipe][pm_indx]) {
                    pm_rst_values[pipe][pm_indx] = 4;
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                    "pm_rst_values[%1d][%2d]=%2d\n"), pipe,
                         pm_indx,
                         pm_rst_values[pipe][pm_indx]));
            }
        }
    }

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = edb_sft_rst_regs[pipe];
        COMPILER_64_ZERO(rval64);

        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][0]);
        soc_reg64_field_set(unit, reg, &rval64, PM0_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][1]);
        soc_reg64_field_set(unit, reg, &rval64, PM1_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][2]);
        soc_reg64_field_set(unit, reg, &rval64, PM2_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][3]);
        soc_reg64_field_set(unit, reg, &rval64, PM3_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][4]);
        soc_reg64_field_set(unit, reg, &rval64, PM4_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][5]);
        soc_reg64_field_set(unit, reg, &rval64, PM5_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][6]);
        soc_reg64_field_set(unit, reg, &rval64, PM6_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][7]);
        soc_reg64_field_set(unit, reg, &rval64, PM7_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][8]);
        soc_reg64_field_set(unit, reg, &rval64, PM8_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][9]);
        soc_reg64_field_set(unit, reg, &rval64, PM9_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][10]);
        soc_reg64_field_set(unit, reg, &rval64, PM10_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][11]);
        soc_reg64_field_set(unit, reg, &rval64, PM11_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][12]);
        soc_reg64_field_set(unit, reg, &rval64, PM12_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][13]);
        soc_reg64_field_set(unit, reg, &rval64, PM13_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][14]);
        soc_reg64_field_set(unit, reg, &rval64, PM14_RESETf,
                            fldval64);
        COMPILER_64_SET(fldval64, 0, pm_rst_values[pipe][15]);
        soc_reg64_field_set(unit, reg, &rval64, PM15_RESETf,
                            fldval64);

        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, REG_PORT_ANY, 0,
                                                rval64));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_sft_rst_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *         struct variable
 *  @param rst_on rst_on==1-> ASSERT reset; rst_on==0-> DEASSERT reset
 *  @brief Main API that sft resets all flexing ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_sft_rst_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state, int rst_on)
{
    int i;
    int logical_port, physical_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    soc_mem_t mem;
    int port_rst_serviced[_TH2_PHY_PORTS_PER_DEV];

    mem = EGR_PER_PORT_BUFFER_SFT_RESETm;

    if (rst_on == 1) {
        memfld = 1;
    } else {
        memfld = 0;
    }

    /* For ports going DOWN  or UP do:
     * Assert(rst_on=1)/De-assert(rst_on=0) per port sft reset
     */
    sal_memset(port_rst_serviced, 0, sizeof(port_rst_serviced));
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        /* If port down, take the actual physical port from in_port_map */
        if (-1 == physical_port) { /* that is, port DOWN */
            physical_port =
                port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
        }
        /* If physical_port index was written once don't do it again
         * Note that there may be two writes to the same physical port
         * More sbus efficiency by tracking which phy indexes thar are
         * already written
         */
        if (0 == port_rst_serviced[physical_port]) {
            sal_memset(entry, 0, sizeof(entry));
            soc_mem_field_set(unit, mem, entry, ENABLEf, &memfld);
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                              physical_port, entry));
            port_rst_serviced[physical_port] = 1;
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_enable_disable(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - port disabled; else port enabled.
 *  @brief Enables or disables EDB port; configures EGR_ENABLE table
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_enable_disable(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int physical_port;
    soc_mem_t mem;

    if (0 == down_or_up ) { /* port down */
        memfld = 0;
        physical_port =
            port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
    } else { /* port up */
        memfld = 1;
        physical_port =
            port_schedule_state->out_port_map.port_l2p_mapping[logical_port];
    }
    mem = EGR_ENABLEm;
    sal_memset(entry, 0, sizeof(uint32) * soc_mem_entry_words(unit, EGR_ENABLEm));
    soc_mem_field_set(unit, mem, entry, PRT_ENABLEf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, physical_port,
                                      entry));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_set_egr_mmu_credit(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct 
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - clear credits; else configure credits
 *  @brief Sets or resets EDB 2 MMU credit; configures EGR_MMU_CELL_CREDITm table
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_set_egr_mmu_credit(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int logical_port, int down_or_up)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int physical_port;
    soc_mem_t mem;

    if (0 == down_or_up ) { /* port down */
        memfld = 0;
        physical_port =
            port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
    } else { /* port up */
        memfld = soc_tomahawk2_ep_get_ep2mmu_credit(unit, port_schedule_state,
                                                    logical_port);
        physical_port =
            port_schedule_state->out_port_map.port_l2p_mapping[logical_port];
    }

    mem = EGR_MMU_CELL_CREDITm;
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_MMU_CELL_CREDITm));
    soc_mem_field_set(unit, mem, entry, CREDITf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, physical_port,
                                      entry));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_set_dev_to_phy(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - reset mapping; else configure mapping
 *  @brief Sets or resets EDB 2 MMU credit; configures 
 *         EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_set_dev_to_phy(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 rval;
    int physical_port;
    soc_reg_t reg;

    if (0 == down_or_up ) { /* port down */
        physical_port = 0xFF;
    } else { /* port up */
        physical_port =
            port_schedule_state->out_port_map.port_l2p_mapping[logical_port];
    }

    rval = 0;
    reg = EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr;
    soc_reg_field_set(unit, reg, &rval, PHYSICAL_PORT_NUMBERf, physical_port);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, logical_port, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_set_edb_1dbg_b(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              int logical_port, int down_or_up)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @param down_or_up down_or_up=0 - clear credits; else configure credits
 *  @brief Configures EDB_1DBG_Bm
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_set_edb_1dbg_b(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state,
                                int                        logical_port,
                                int                        down_or_up)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 memfld;
    int physical_port;
    soc_mem_t mem;
    int is_ovs;
    soc_port_map_type_t *port_map;

    if (0 == down_or_up ) { /* port down */
        port_map = &port_schedule_state->in_port_map;
        memfld = 0;
    } else { /* port up */
        port_map = &port_schedule_state->out_port_map;
        memfld =
            (15 * 11875 *
             (port_schedule_state->out_port_map.log_port_speed[logical_port] /
         1000)) / 1000;
    }

    /* Configure only if OVS */
    is_ovs = SOC_PBMP_NOT_NULL(port_map->oversub_pbm) ? 1 : 0;
    if (1 == is_ovs) {
        mem = EDB_1DBG_Bm;
        sal_memset(entry, 0, sizeof(uint32) *
                   soc_mem_entry_words(unit, EDB_1DBG_Bm));
        physical_port = port_map->port_l2p_mapping[logical_port];
        soc_mem_field_set(unit, mem, entry, FIELD_Bf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                          physical_port, entry));
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk2_ep_flexport_extra_holding_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int cell_counts)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct 
 *         variable
 *  @param cell_counts number of additional cell in prebuffering
 *  @brief Configure EDB extra holding reg; EGR_FLEXPORT_EXTRA_HOLDING_PIPE0/1/2/3r
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_extra_holding_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cell_counts)
{
    int pipe;
    uint32 rval;
    uint32 pipe_map;
    soc_reg_t reg;
    static const soc_reg_t extr_hold_regs[_TH2_PIPES_PER_DEV] = {
        EGR_FLEXPORT_EXTRA_HOLDING_PIPE0r, EGR_FLEXPORT_EXTRA_HOLDING_PIPE1r,
        EGR_FLEXPORT_EXTRA_HOLDING_PIPE2r, EGR_FLEXPORT_EXTRA_HOLDING_PIPE3r
    };

    /* pipe_map indicates which pipe has flexing ports */
    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    /* Extra Hold in all PIPEs */
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) { continue; }
        reg = extr_hold_regs[pipe];
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, CELL_COUNTSf, cell_counts);
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0,
                                                  rval));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_xmit_cnt_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @brief Configures EGR_XMIT_START_COUNT_PIPE table; all valid per port 13
 *         entries
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_xmit_cnt_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    soc_mem_t mem;
    int pipe, ct_class, mem_index;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int xmit_start_cnt[16];
    static const soc_mem_t ep_xmit_cnt_mems[_TH2_PIPES_PER_DEV] = {
        EGR_XMIT_START_COUNT_PIPE0m, EGR_XMIT_START_COUNT_PIPE1m,
        EGR_XMIT_START_COUNT_PIPE2m, EGR_XMIT_START_COUNT_PIPE3m
    };

    pipe = logical_port / _TH2_DEV_PORTS_PER_PIPE;

    /* Set all XMIT START CNT values for a port; 0-SAF; 1-12 CT classes;
     * 13-15 reserved 
     */
    mem = ep_xmit_cnt_mems[pipe];
    soc_tomahawk2_ep_get_xmit_start_count(unit, port_schedule_state,
                                          logical_port,
                                          xmit_start_cnt);
    for (ct_class = 0; ct_class < 13; ct_class++) {
        sal_memset(entry, 0, sizeof(uint32) *
                   soc_mem_entry_words(unit, EGR_XMIT_START_COUNT_PIPE0m));
        mem_index = (logical_port % _TH2_DEV_PORTS_PER_PIPE) * 16 + ct_class;
        memfld = xmit_start_cnt[ct_class];
        soc_mem_field_set(unit, mem, entry, THRESHOLDf, &memfld);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index,
                                          entry));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_xmit_cnt_set_all_ports(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief API to configure EGR_XMIT_START_COUNT_PIPE0/1/2/3m for either added
 *         ports or all ports in a pipe (if ovs_ratio changes)
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_xmit_cnt_set_all_ports(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int old_max_ovs_ratio, new_max_ovs_ratio;
    int i, logical_port, physical_port, is_ovs;


    /* Find old/new max ovs ratio among the whole half pipes in the device */
    soc_tomahawk2_ep_flexport_get_max_ovs_ratio(unit,
                                                port_schedule_state,
                                                &old_max_ovs_ratio,
                                                &new_max_ovs_ratio);

    /* ovs_ratio_changed; all ports from device configured */
    if (new_max_ovs_ratio != old_max_ovs_ratio) {
        for (logical_port = 0; logical_port < _TH2_DEV_PORTS_PER_DEV;
             logical_port++) {
            is_ovs = SOC_PBMP_MEMBER(
                port_schedule_state->out_port_map.oversub_pbm,
                logical_port) ? 1 : 0;
            if ((port_schedule_state->out_port_map.log_port_speed[logical_port]
                 > 0 ) &&
                (is_ovs == 1)) {
                soc_tomahawk2_ep_flexport_xmit_cnt_set(unit,
                                                       port_schedule_state,
                                                       logical_port);
            }
        }
    } else { /* ovs_ratio is the same; only added ports are configured */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (-1 != physical_port) { /* that is, port UP */
                /* Configure XMIT_START_CNT entries for added port :
                 * write EGR_XMIT_START_COUNT_PIPE0/1/2/3m
                 */
                soc_tomahawk2_ep_flexport_xmit_cnt_set(unit,
                                                       port_schedule_state,
                                                       logical_port);
            }
        }
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_ct_class_set(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param logical_port Logical port.
 *  @brief Configures EGR_IP_CUT_THRU_CLASS table in all 4 pipes
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_ct_class_set(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    soc_mem_t mem;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int speed;
    soc_tomahawk2_flex_scratch_t *cookie;

    /* Set CT_CLASS for this port in all PIPEs */
    mem = EGR_IP_CUT_THRU_CLASSm;
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_IP_CUT_THRU_CLASSm));
    cookie = port_schedule_state->cookie;
    speed  = cookie->exact_out_log_port_speed[logical_port];
    memfld = soc_tomahawk2_get_ct_class(speed);
    soc_mem_field_set(unit, mem, entry, CUT_THRU_CLASSf, &memfld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, logical_port,
                                      entry));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_ep_flexport_drain_port(int unit,
 *              soc_port_schedule_state_t *port_schedule_state, int logical_port)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param cell_counts number of additional cell in prebuffering
 *  @brief Drain a port that goes down
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_flexport_drain_port(
    int unit, soc_port_schedule_state_t *port_schedule_state, int logical_port)
{
    int pipe;
    uint32 rval_save, rval;
    soc_reg_t reg;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 util;
    int physical_port;
    soc_mem_t mem;
    int timeout, iter;

    static const soc_reg_t misc_ctrl_regs[_TH2_PIPES_PER_DEV] = {
        EGR_EDB_MISC_CTRL_PIPE0r, EGR_EDB_MISC_CTRL_PIPE1r,
        EGR_EDB_MISC_CTRL_PIPE2r, EGR_EDB_MISC_CTRL_PIPE3r
    };

    pipe = logical_port / _TH2_DEV_PORTS_PER_PIPE;

    /* Configure EGR_EDB_MISC_CTRL such that EGR_MAX_USED_ENTRIES returns
     * buffer utilization
     */
    reg = misc_ctrl_regs[pipe];
    rval_save = 0;
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, REG_PORT_ANY, 0,
                                              &rval_save));
    rval = rval_save;
    soc_reg_field_set(unit, reg, &rval, SELECT_CURRENT_USED_ENTRIESf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0, rval));

    /* polling port's buffer utilization until is 0 */
    timeout = 1000;
    iter = 0;
    physical_port =
        /* assumes drain before FlexPort */
        port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, EGR_MAX_USED_ENTRIESm));
    mem = EGR_MAX_USED_ENTRIESm;
    while (iter < timeout) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                          physical_port, entry));
        /* coverity[callee_ptr_arith : FALSE] */
        soc_mem_field_get(unit, mem, entry, LEVELf, &util);
        if (0 == util) { break; }
        iter++;
    }

    /* Configure EGR_EDB_MISC_CTRL back to previous value */
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, REG_PORT_ANY, 0,
                                              rval_save));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_flex_ep_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Helper function to follow EDB port down sequence for flexport operation.
 *         Two main parts:
 *            (1) Poll until EDB buffer is empty
 *            (2) Hold EDB port buffer in reset state and disable cell request
 *                generation in EP
 *  @returns SOC_E_NONE
 */
int
soc_tomahawk2_flex_ep_port_down(int                        unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    int i, rst_on, down_or_up;
    int logical_port, physical_port;

    /*
     * 6. Poll until EDB buffer is empty
     * Set EGR_EDB_MISC_CTRL.SELECT_CURRENT_USED_ENTRIES to 1.
     * Poll until EGR_MAX_USED_ENTRIES[physical_port] is equal to 0.
     */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) {
            logical_port = port_schedule_state->resource[i].logical_port;
            SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_drain_port(unit,
                                    port_schedule_state, logical_port));
        }
    }

    /*
     * 7. Hold EDB port buffer in reset state and disable cell request generation
     *    in EP2
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 0.
     * Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 1.
     */

    down_or_up = 0; /* that is, port DOWN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 == physical_port) { /* that is, port DOWN */
            /* Disable port : write EGR_ENABLEm */
            soc_tomahawk2_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    rst_on = 1;
    SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_sft_rst_ports(unit,
                                                                port_schedule_state,
                                                                rst_on));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_flex_ep_reconfigure_remove(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - remove ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_flex_ep_reconfigure_remove(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port, physical_port;

    /* For ports going DOWN do:
     * 1. Delete Dev 2 Phy mapping
     * 2. Reset max Ep2MMU credits to 0
     * 3. Reset EDB_1DBG_Bm to 0
     */
    down_or_up = 0; /* that is, port DOWN */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 == physical_port) { /* that is, port DOWN */
            /* Reset Logical to phy mapping : write 
             * EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr
             */
            soc_tomahawk2_ep_set_dev_to_phy(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
            /* Reset EP2MMU credits to 0 : write EGR_MMU_CELL_CREDITm */
            soc_tomahawk2_ep_set_egr_mmu_credit(unit, port_schedule_state,
                                                logical_port,
                                                down_or_up);
            /* Reset edb_1dbg_b : write EDB_1DBG_Bm */
            soc_tomahawk2_ep_set_edb_1dbg_b(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    /* Increase EDB pre-buffering during FlexPort by 9 cells (144 Bytes) */
    SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_extra_holding_set(unit,
                            port_schedule_state, 9)); /* 9 as pe FlexPort Spec */

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_flex_ep_reconfigure_add(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that reconfigures all ports from EGR pipe - add ports
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_flex_ep_reconfigure_add(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int i, down_or_up;
    int logical_port, physical_port;


    /* For ports going UP do:
     * 1. Map Dev 2 Phy for added port
     * 2. Configure max Ep2MMU credits to proper value
     * 3. Configure EDB_1DBG_Bm for added port
     * 4. Configure CT_class for added port
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            /* Map Dev 2 Phy : write EGR_DEVICE_TO_PHYSICAL_PORT_NUMBER_MAPPINGr */
            soc_tomahawk2_ep_set_dev_to_phy(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
            /* Configure max Ep2MMU credits to proper value : 
             * write EGR_MMU_CELL_CREDITm
             */
            soc_tomahawk2_ep_set_egr_mmu_credit(unit, port_schedule_state,
                                                logical_port,
                                                down_or_up);
            /* Configure EDB_1DBG_Bm : write EDB_1DBG_Bm */
            soc_tomahawk2_ep_set_edb_1dbg_b(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
            /* Configure CT_class for added port : write EGR_IP_CUT_THRU_CLASSm */
            soc_tomahawk2_ep_flexport_ct_class_set(unit, port_schedule_state,
                                                   logical_port);
        }
    }

    /* 4. Configure XMIT_START_CNT entries for added port OR all ports
     * write EGR_XMIT_START_COUNT_PIPE0/1/2/3m
     */
    SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_xmit_cnt_set_all_ports(unit,
                            port_schedule_state));

    /* Reset EDB pre-buffering after FlexPort back to 0 */
    SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_extra_holding_set(unit,
                            port_schedule_state, 0));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk2_flex_ep_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @brief Main API that adds all ports from EGR pipe
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_flex_ep_port_up(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state)
{
    int i, rst_on, down_or_up;
    int logical_port, physical_port;


    /*
     * Release EDB port buffer reset and enable cell request generation in EP
     * Set EGR_PER_PORT_BUFFER_SFT_RESET[device_port] to 0
     * Set EGR_ENABLE[device_port].PRT_ENABLE to 1
     */

    /* De-assert PM intf sft_reset */
    rst_on = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk2_ep_flexport_sft_rst_ports(unit,
                                                                port_schedule_state,
                                                                rst_on));

    /* Enable Ports going up after PM sft_rst is de-asserted */
    /* For ports going UP do:
     * 1. Enable port; write EGR_ENABLEm
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            /* Enable port; write EGR_ENABLEm */
            soc_tomahawk2_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }

    return SOC_E_NONE;
}


/*** END SDK API COMMON CODE ***/


/*! @fn int soc_tomahawk2_tdm_calculate_ovs(int unit,
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int pipe, int prev_or_new)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable
 *  @param prev_or_new calculate OVS ratios for prev or new OVS tables; 
 *         0 - prev; 1- new
 *  @brief Calculate ovs_ratios per PIPE and HPIPE
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_ep_display_xmit_cnt(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int ct_class, lat, ovs_ratio, port, ep2mmu_cred;
    int xmit_cnt;
    int xmit_cnt_tbl[16];

    /* FULL LATENCY */

    for (ovs_ratio=2; ovs_ratio<=4; ovs_ratio++) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n\nOVS=%d\n"), 5*ovs_ratio));
        for (ct_class=0; ct_class < 13; ct_class++) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "ct_class=%d\tep2mmu=%d\tep2mmu_max_freq=%d\t"),
                     ct_class, soc_th2_ep_core_cfg_tbl[ct_class].egr_mmu_credit,
                     soc_th2_ep_core_cfg_tbl[ct_class].egr_mmu_credit_max_freq));
            for (lat=2; lat>=0; lat--) {
                xmit_cnt = soc_tomahawk2_ep_get_ct_xmit_start_cnt(ct_class, lat,
                                                                  5*ovs_ratio);
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%2d\t"), xmit_cnt));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }


    for (port = 1; port < 136; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
            ep2mmu_cred = soc_tomahawk2_ep_get_ep2mmu_credit(
                unit, port_schedule_state, port);
            LOG_DEBUG(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit, "LOG_PORT=%3d\tSPEED=%3d\tCRED=%2d\t"),
                     port,
                     port_schedule_state->in_port_map.log_port_speed[port]/1000,
                     ep2mmu_cred));
            soc_tomahawk2_ep_get_xmit_start_count(unit, port_schedule_state,
                                                  port,
                                                  xmit_cnt_tbl);
            for (ct_class=0; ct_class < 16; ct_class++) {
                LOG_DEBUG(BSL_LS_SOC_PORT, 
                          (BSL_META_U(unit, "%2d\t"), xmit_cnt_tbl[ct_class]));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }

    return SOC_E_NONE;
}


int
soc_tomahawk2_ep_flexport_remove_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk2_flex_start")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_flex_start(unit, port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk2_tdm_calculation_flexport")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_tdm_calculation_flexport(unit,
                                                               port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk2_flex_ep_port_down")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_flex_ep_port_down(unit,
                                                        port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_remove_ports_shim(): "
                        "calling soc_tomahawk2_ep_flexport_reconfigure_remove")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_flex_ep_reconfigure_remove(unit,
                            port_schedule_state));


    return SOC_E_NONE;
}

/* Comment out these functions from soc_tomahawk2_ep_flexport_add_ports function
 * 1. soc_tomahawk2_ep_flexport_ct_class_set
 * 2. soc_tomahawk2_ep_flexport_xmit_cnt_set_all_ports
 * 3. soc_tomahawk2_ep_enable_disable
 */
int
soc_tomahawk2_ep_flexport_add_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk2_ep_flexport_reconfigure_add")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_flex_ep_reconfigure_add(unit,
                                                              port_schedule_state));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk2_ep_flexport_add_ports")));
    SOC_IF_ERROR_RETURN(soc_tomahawk2_flex_ep_port_up(unit, port_schedule_state));

    return SOC_E_NONE;
}


int
soc_tomahawk2_ep_flexport_enable_ports_shim(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int down_or_up, i, logical_port, physical_port;


    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "soc_tomahawk2_ep_flexport_add_ports_shim(): "
                        "calling soc_tomahawk2_ep_flexport_add_ports")));
    /* Enable Ports going up after PM sft_rst is de-asserted */
    /* For ports going UP do:
     * 6. Enable port; write EGR_ENABLEm
     */
    down_or_up = 1; /* that is, port UP */
    for (i = 0; i < port_schedule_state->nport; i++) {
        logical_port = port_schedule_state->resource[i].logical_port;
        physical_port = port_schedule_state->resource[i].physical_port;
        if (-1 != physical_port) { /* that is, port UP */
            /* Enable port; write EGR_ENABLEm */
            soc_tomahawk2_ep_enable_disable(unit, port_schedule_state,
                                            logical_port,
                                            down_or_up);
        }
    }
    return SOC_E_NONE;
}


#endif /* BCM_TOMAHAWK2_SUPPORT */
