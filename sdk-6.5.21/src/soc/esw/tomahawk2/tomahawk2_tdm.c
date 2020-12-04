/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk2_tdm.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk2_tdm.h>


/*** START SDK API COMMON CODE ***/


/*! @file tomahawk2_tdm.c
 *  @brief Scheduler TDM init for Tomahawk2.
 *  Details are shown below.
 */


/*! @fn int _soc_tomahawk2_tdm_mmu_calendar_set(int 
 *              unit, soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t 
 *         struct variable.
 *  @brief API to initialize the TH2 MMU main calendar.
 * Description:
 *      API to initialize the TH2 MMU main calendar.
 *      Regs/Mems configured: MMU TDM_CALENDAR, TDM_CONFIG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_mmu_calendar_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    uint32 pipe_map;
    uint32 cal_len;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length, calendar_id;
    int phy_port, mmu_port, inst;
    uint32 rval;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mem_indx;
    static const soc_mem_t calendar_mems[] = {
        TDM_CALENDAR0_PIPE0m, TDM_CALENDAR0_PIPE1m, TDM_CALENDAR0_PIPE2m,
        TDM_CALENDAR0_PIPE3m,
        TDM_CALENDAR1_PIPE0m, TDM_CALENDAR1_PIPE1m, TDM_CALENDAR1_PIPE2m,
        TDM_CALENDAR1_PIPE3m
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);
    /* TDM Calendar is always in slice 0 */

    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, TDM_CALENDAR0_PIPE0m));

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        cal_len =
            port_schedule_state->tdm_egress_schedule_pipe[pipe].
            tdm_schedule_slice[0
            ].cal_len;

        for (length = cal_len; length > 0; length--) {
            if (port_schedule_state->tdm_egress_schedule_pipe[pipe].
                tdm_schedule_slice[0].linerate_schedule[length - 1] !=
                TH2_NUM_EXT_PORTS) {
                break;
            }
        }

        reg = TDM_CONFIGr;
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        if (port_schedule_state->is_flexport == 1) { /* choose the other one */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        } else {
            calendar_id = 0; /* At init use calendar_id=0*/
            rval = 0;
        }
        mem = calendar_mems[(calendar_id * _TH2_PIPES_PER_DEV) + pipe];
        field = calendar_end_fields[calendar_id];
        for (slot = 0; slot < length; slot += 2) {
            /* EVEN */
            phy_port =
                port_schedule_state->tdm_egress_schedule_pipe[pipe].
                tdm_schedule_slice
                [0].linerate_schedule[slot];
            if (phy_port == TH2_OVSB_TOKEN) {
                mmu_port = _SOC_TH2_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH2_IDL1_TOKEN) {
                mmu_port = _SOC_TH2_TDM_IDL1_TOKEN;
            } else if (phy_port == TH2_IDL2_TOKEN) {
                mmu_port = _SOC_TH2_TDM_UNUSED_TOKEN; /* _SOC_TH2_TDM_IDL2_TOKEN; */
            } else if (phy_port == TH2_NULL_TOKEN) {
                mmu_port = _SOC_TH2_TDM_NULL_TOKEN;
            } else if (phy_port >= TH2_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH2_TDM_UNUSED_TOKEN;
            } else if (phy_port == TH2_CMIC_TOKEN ||
                       phy_port == TH2_MGM1_TOKEN ||
                       phy_port == TH2_MGM2_TOKEN) {
                mmu_port = _SOC_TH2_TDM_CPU_MNG_TOKEN;
            } else if (phy_port == TH2_LPB0_TOKEN ||
                       phy_port == TH2_LPB1_TOKEN ||
                       phy_port == TH2_LPB2_TOKEN ||
                       phy_port == TH2_LPB3_TOKEN) {
                mmu_port = _SOC_TH2_TDM_LB_TOKEN;
            } else {
                mmu_port =
                    port_schedule_state->out_port_map.port_p2m_mapping[phy_port]
                    & 0x3f;
            }
            if (mmu_port > 31) { /* Not a front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
            }
            memfld = mmu_port & 0x3f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_EVENf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_EVENf, &memfld);
            /* ODDD */
            phy_port =
                port_schedule_state->tdm_egress_schedule_pipe[pipe].
                tdm_schedule_slice
                [0].linerate_schedule[slot + 1];
            if (phy_port == TH2_OVSB_TOKEN) {
                mmu_port = _SOC_TH2_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH2_IDL1_TOKEN) {
                mmu_port = _SOC_TH2_TDM_IDL1_TOKEN;
            } else if (phy_port == TH2_IDL2_TOKEN) {
                mmu_port = _SOC_TH2_TDM_UNUSED_TOKEN; /* _SOC_TH2_TDM_IDL2_TOKEN; */
            } else if (phy_port == TH2_NULL_TOKEN) {
                mmu_port = _SOC_TH2_TDM_NULL_TOKEN;
            } else if (phy_port >= TH2_NUM_EXT_PORTS) {
                mmu_port = _SOC_TH2_TDM_UNUSED_TOKEN;
            } else if (phy_port == TH2_CMIC_TOKEN ||
                       phy_port == TH2_MGM1_TOKEN ||
                       phy_port == TH2_MGM2_TOKEN) {
                mmu_port = _SOC_TH2_TDM_CPU_MNG_TOKEN;
            } else if (phy_port == TH2_LPB0_TOKEN ||
                       phy_port == TH2_LPB1_TOKEN ||
                       phy_port == TH2_LPB2_TOKEN ||
                       phy_port == TH2_LPB3_TOKEN) {
                mmu_port = _SOC_TH2_TDM_LB_TOKEN;
            } else {
                mmu_port =
                    port_schedule_state->out_port_map.port_p2m_mapping[phy_port
                    ] &
                    0x3f;
            }
            if (mmu_port > 31) { /* Not a front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
            }
            memfld = mmu_port & 0x3f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_ODDf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_ODDf, &memfld);
            mem_indx = slot / 2;
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_indx, entry));
            if (port_schedule_state->tdm_egress_schedule_pipe[pipe].
                tdm_schedule_slice[0].linerate_schedule[slot + 2] ==
                TH2_NUM_EXT_PORTS) {
                soc_reg_field_set(
                    unit, reg, &rval, field,
                    phy_port == TH2_NUM_EXT_PORTS ? slot : slot +
                    1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_idb_calendar_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to initialize the TH2 IDB main calendar.
 * Description:
 *      API to initialize the TH2 IDB main calendar.
 *      Regs/Mems configured: IDB TDM_CALENDAR, TDM_CONFIG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_idb_calendar_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    uint32 pipe_map;
    uint32 cal_len;
    soc_mem_t mem;
    soc_reg_t reg;
    soc_field_t field;
    int pipe, slot, id, length, calendar_id;
    int phy_port, idb_port;
    int port;
    uint32 rval;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mem_indx;
    static const soc_mem_t idb_calendar_mems[_TH2_PIPES_PER_DEV][2] = {
        {IS_TDM_CALENDAR0_PIPE0m, IS_TDM_CALENDAR1_PIPE0m},
        {IS_TDM_CALENDAR0_PIPE1m, IS_TDM_CALENDAR1_PIPE1m},
        {IS_TDM_CALENDAR0_PIPE2m, IS_TDM_CALENDAR1_PIPE2m},
        {IS_TDM_CALENDAR0_PIPE3m, IS_TDM_CALENDAR1_PIPE3m}
    };
    static const soc_field_t calendar_end_fields[] = {
        CAL0_ENDf, CAL1_ENDf
    };
    static const soc_reg_t idb_tdm_cfg_reg[_TH2_PIPES_PER_DEV] = {
        IS_TDM_CONFIG_PIPE0r, IS_TDM_CONFIG_PIPE1r,
        IS_TDM_CONFIG_PIPE2r, IS_TDM_CONFIG_PIPE3r
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    sal_memset(entry, 0, sizeof(uint32) *
               soc_mem_entry_words(unit, IS_TDM_CALENDAR0_PIPE0m));

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        reg = idb_tdm_cfg_reg[pipe];
        if ((pipe_map & (1 << pipe)) == 0) {
            /* At init, even if the pipe doesn't have active ports,
             * enable scheduler for sbus accesses to PP IPIPE
             */
            if (port_schedule_state->is_flexport == 0) {
                rval = 0;
                soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0,
                                                  rval));
            }
            continue;
        }
        /* TDM Calendar is always taken from slice 0 */
        cal_len =
            port_schedule_state->tdm_ingress_schedule_pipe[pipe].
            tdm_schedule_slice[
                0].cal_len;

        for (length = cal_len; length > 0; length--) {
            if (port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                tdm_schedule_slice[0].linerate_schedule[length - 1] !=
                TH2_NUM_EXT_PORTS) {
                break;
            }
        }
        if (port_schedule_state->is_flexport == 1) { /* choose other calendar */
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            calendar_id = soc_reg_field_get(unit, reg, rval, CURR_CALf) ^ 1;
        } else {
            /* At init use calendar_id=0*/
            calendar_id = 0;
            /* Also, before writing the TDM calendar entries, make sure 
             * calendar is disabled again; 
             * IDB sheduler may be already enabled at this point to allow
             * sbus access to IPIPE for reset purposes (Sec. 3.1.2 SW_init doc)
             */
            rval = 0;
            soc_reg_field_set(unit, reg, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        mem = idb_calendar_mems[pipe][calendar_id];
        field = calendar_end_fields[calendar_id];

        for (slot = 0; slot < length; slot += 2) {
            /* EVEN */
            phy_port =
                port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                tdm_schedule_slice
                [0].linerate_schedule[slot];
            if (phy_port == TH2_OVSB_TOKEN) {
                idb_port = _SOC_TH2_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH2_IDL1_TOKEN) {
                idb_port = _SOC_TH2_TDM_IDL1_TOKEN;
            } else if (phy_port == TH2_IDL2_TOKEN) {
                idb_port = _SOC_TH2_TDM_UNUSED_TOKEN; /* _SOC_TH2_TDM_IDL2_TOKEN; */
            } else if (phy_port == TH2_NULL_TOKEN) {
                idb_port = _SOC_TH2_TDM_NULL_TOKEN;
            } else if (phy_port >= TH2_NUM_EXT_PORTS) {
                idb_port = _SOC_TH2_TDM_UNUSED_TOKEN;
            } else if (phy_port == TH2_CMIC_TOKEN ||
                       phy_port == TH2_MGM1_TOKEN ||
                       phy_port == TH2_MGM2_TOKEN) {
                idb_port = _SOC_TH2_TDM_CPU_MNG_TOKEN;
            } else if (phy_port == TH2_LPB0_TOKEN ||
                       phy_port == TH2_LPB1_TOKEN ||
                       phy_port == TH2_LPB2_TOKEN ||
                       phy_port == TH2_LPB3_TOKEN) {
                idb_port = _SOC_TH2_TDM_LB_TOKEN;
            } else {
                port =
                    port_schedule_state->out_port_map.port_p2l_mapping[phy_port];
                idb_port =
                    port_schedule_state->out_port_map.port_l2i_mapping[port]
                    & 0x3f;
            }
            if (idb_port > 31) { /* Not a front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
            }
            memfld = idb_port & 0x3f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_EVENf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_EVENf, &memfld);
            /* ODDD */
            phy_port =
                port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                tdm_schedule_slice
                [0].linerate_schedule[slot + 1];
            if (phy_port == TH2_OVSB_TOKEN) {
                idb_port = _SOC_TH2_TDM_OVERSUB_TOKEN;
            } else if (phy_port == TH2_IDL1_TOKEN) {
                idb_port = _SOC_TH2_TDM_IDL1_TOKEN;
            } else if (phy_port == TH2_IDL2_TOKEN) {
                idb_port = _SOC_TH2_TDM_UNUSED_TOKEN; /* _SOC_TH2_TDM_IDL2_TOKEN; */
            } else if (phy_port == TH2_NULL_TOKEN) {
                idb_port = _SOC_TH2_TDM_NULL_TOKEN;
            } else if (phy_port >= TH2_NUM_EXT_PORTS) {
                idb_port = _SOC_TH2_TDM_UNUSED_TOKEN;
            } else if (phy_port == TH2_CMIC_TOKEN ||
                       phy_port == TH2_MGM1_TOKEN ||
                       phy_port == TH2_MGM2_TOKEN) {
                idb_port = _SOC_TH2_TDM_CPU_MNG_TOKEN;
            } else if (phy_port == TH2_LPB0_TOKEN ||
                       phy_port == TH2_LPB1_TOKEN ||
                       phy_port == TH2_LPB2_TOKEN ||
                       phy_port == TH2_LPB3_TOKEN) {
                idb_port = _SOC_TH2_TDM_LB_TOKEN;
            } else {
                port =
                    port_schedule_state->out_port_map.port_p2l_mapping[phy_port
                    ];
                idb_port =
                    port_schedule_state->out_port_map.port_l2i_mapping[port] &
                    0x3f;
            }
            if (idb_port > 31) { /* Not a front panel port */
                id = 0xf;
            } else {
                id = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
            }
            memfld = idb_port & 0x3f;
            soc_mem_field_set(unit, mem, entry, PORT_NUM_ODDf, &memfld);
            memfld = id & 0xf;
            soc_mem_field_set(unit, mem, entry, PHY_PORT_ID_ODDf, &memfld);
            mem_indx = slot / 2;
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_indx, entry));
            if (port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                tdm_schedule_slice[0].linerate_schedule[slot + 2] ==
                TH2_NUM_EXT_PORTS) {
                soc_reg_field_set(
                    unit, reg, &rval, field,
                    phy_port == TH2_NUM_EXT_PORTS ? slot : slot +
                    1);
                break;
            }
        }
        soc_reg_field_set(unit, reg, &rval, CURR_CALf, calendar_id);
        soc_reg_field_set(unit, reg, &rval, ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_oversub_group_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to initialize the TH2 MMU & IDB OVS tables.
 * Description:
 *      API to initialize the TH2 MMU/IDB OVS tables.
 *      Regs/Mems configured: IDB HPIPE0/1_OVR_SUB_GRP_CFG,
 *                            HPIPE0/1_OVR_SUB_GRP_CFG
 *                            PBLK<n>_CALENDAR
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_oversub_group_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    return (_soc_tomahawk2_tdm_oversub_group_set_sel(unit, port_schedule_state,
                                                     1, 1));
}


/*! @fn int _soc_tomahawk2_tdm_oversub_group_set_sel(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state, int set_idb, 
 *              int set_mmu)
 *  @param unit Chip unit number.
 *  @param port_schedule_state : Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param set_idb if 1 then IDB OVS tables are configured
 *  @param set_mmu if 1 then MMU OVS tables are configured
 *  @brief API to initialize the TH2 MMU &/or IDB OVS tables.
 * Description:
 *      API to initialize the TH2 MMU/IDB OVS tables.
 *      Regs/Mems configured: IDB HPIPE0/1_OVR_SUB_GRP_CFG,
 *                            HPIPE0/1_OVR_SUB_GRP_CFG
 *                            PBLK<n>_CALENDAR
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_oversub_group_set_sel(
    int unit, soc_port_schedule_state_t *port_schedule_state, int set_idb,
    int set_mmu)
{
    uint32 pipe_map, ovs_pipe_map;
    soc_reg_t reg, reg1;
    int pipe, hpipe, group, lane, slot, id = 0, mode;
    int port, phy_port, phy_port_base, idb_port, mmu_port, inst;
    int speed_max;
    uint32 rval, rval1;
    int pblks[_TH2_PIPES_PER_DEV][2][_TH2_PBLKS_PER_HPIPE];
    int pm_num, pblk_cal_idx;
    int pblk_valid, pblk_spacing;
    int ovs_class, ovs_same_spacing, ovs_sister_spacing, is_hsp;
    int l_speed, l_num_lanes, prev_or_new;

    static const soc_reg_t idb_grp_cfg_regs[_TH2_PIPES_PER_DEV][2] = {
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE0r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE0r},
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE1r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE1r},
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE2r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE2r},
        {IS_HPIPE0_OVR_SUB_GRP_CFG_PIPE3r, IS_HPIPE1_OVR_SUB_GRP_CFG_PIPE3r},
    };

    static const soc_reg_t mmu_grp_cfg_regs[2] = {
        HPIPE0_OVR_SUB_GRP_CFGr, HPIPE1_OVR_SUB_GRP_CFGr
    };

    static const soc_reg_t idb_grp_ovs_regs[_TH2_PIPES_PER_DEV][2][6] = {
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE0r,
             IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE0r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE0r,
             IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE0r }
        },
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE1r,
             IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE1r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE1r,
             IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE1r }
        },
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE2r,
             IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE2r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE2r,
             IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE2r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE2r,
             IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE2r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE2r,
             IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE2r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE2r,
             IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE2r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE2r,
             IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE2r }
        },
        {
            {IS_HPIPE0_OVR_SUB_GRP0_TBL_PIPE3r,
             IS_HPIPE0_OVR_SUB_GRP1_TBL_PIPE3r,
             IS_HPIPE0_OVR_SUB_GRP2_TBL_PIPE3r,
             IS_HPIPE0_OVR_SUB_GRP3_TBL_PIPE3r,
             IS_HPIPE0_OVR_SUB_GRP4_TBL_PIPE3r,
             IS_HPIPE0_OVR_SUB_GRP5_TBL_PIPE3r },
            {IS_HPIPE1_OVR_SUB_GRP0_TBL_PIPE3r,
             IS_HPIPE1_OVR_SUB_GRP1_TBL_PIPE3r,
             IS_HPIPE1_OVR_SUB_GRP2_TBL_PIPE3r,
             IS_HPIPE1_OVR_SUB_GRP3_TBL_PIPE3r,
             IS_HPIPE1_OVR_SUB_GRP4_TBL_PIPE3r,
             IS_HPIPE1_OVR_SUB_GRP5_TBL_PIPE3r },
        }
    };

    static const soc_reg_t mmu_grp_ovs_regs[2][6] = {
        {HPIPE0_OVR_SUB_GRP0_TBLr, HPIPE0_OVR_SUB_GRP1_TBLr,
         HPIPE0_OVR_SUB_GRP2_TBLr, HPIPE0_OVR_SUB_GRP3_TBLr,
         HPIPE0_OVR_SUB_GRP4_TBLr, HPIPE0_OVR_SUB_GRP5_TBLr },
        {HPIPE1_OVR_SUB_GRP0_TBLr, HPIPE1_OVR_SUB_GRP1_TBLr,
         HPIPE1_OVR_SUB_GRP2_TBLr, HPIPE1_OVR_SUB_GRP3_TBLr,
         HPIPE1_OVR_SUB_GRP4_TBLr, HPIPE1_OVR_SUB_GRP5_TBLr }
    };

    static const soc_reg_t idb_pblk_cal_regs[_TH2_PIPES_PER_DEV][2][8] = {
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE0r, IS_HPIPE0_PBLK1_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE0r, IS_HPIPE0_PBLK3_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE0r, IS_HPIPE0_PBLK5_CALENDAR_PIPE0r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE0r, IS_HPIPE0_PBLK7_CALENDAR_PIPE0r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE0r, IS_HPIPE1_PBLK1_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE0r, IS_HPIPE1_PBLK3_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE0r, IS_HPIPE1_PBLK5_CALENDAR_PIPE0r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE0r, IS_HPIPE1_PBLK7_CALENDAR_PIPE0r }
        },
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE1r, IS_HPIPE0_PBLK1_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE1r, IS_HPIPE0_PBLK3_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE1r, IS_HPIPE0_PBLK5_CALENDAR_PIPE1r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE1r, IS_HPIPE0_PBLK7_CALENDAR_PIPE1r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE1r, IS_HPIPE1_PBLK1_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE1r, IS_HPIPE1_PBLK3_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE1r, IS_HPIPE1_PBLK5_CALENDAR_PIPE1r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE1r, IS_HPIPE1_PBLK7_CALENDAR_PIPE1r }
        },
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE2r, IS_HPIPE0_PBLK1_CALENDAR_PIPE2r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE2r, IS_HPIPE0_PBLK3_CALENDAR_PIPE2r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE2r, IS_HPIPE0_PBLK5_CALENDAR_PIPE2r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE2r, IS_HPIPE0_PBLK7_CALENDAR_PIPE2r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE2r, IS_HPIPE1_PBLK1_CALENDAR_PIPE2r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE2r, IS_HPIPE1_PBLK3_CALENDAR_PIPE2r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE2r, IS_HPIPE1_PBLK5_CALENDAR_PIPE2r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE2r, IS_HPIPE1_PBLK7_CALENDAR_PIPE2r }
        },
        {
            {IS_HPIPE0_PBLK0_CALENDAR_PIPE3r, IS_HPIPE0_PBLK1_CALENDAR_PIPE3r,
             IS_HPIPE0_PBLK2_CALENDAR_PIPE3r, IS_HPIPE0_PBLK3_CALENDAR_PIPE3r,
             IS_HPIPE0_PBLK4_CALENDAR_PIPE3r, IS_HPIPE0_PBLK5_CALENDAR_PIPE3r,
             IS_HPIPE0_PBLK6_CALENDAR_PIPE3r, IS_HPIPE0_PBLK7_CALENDAR_PIPE3r },
            {IS_HPIPE1_PBLK0_CALENDAR_PIPE3r, IS_HPIPE1_PBLK1_CALENDAR_PIPE3r,
             IS_HPIPE1_PBLK2_CALENDAR_PIPE3r, IS_HPIPE1_PBLK3_CALENDAR_PIPE3r,
             IS_HPIPE1_PBLK4_CALENDAR_PIPE3r, IS_HPIPE1_PBLK5_CALENDAR_PIPE3r,
             IS_HPIPE1_PBLK6_CALENDAR_PIPE3r, IS_HPIPE1_PBLK7_CALENDAR_PIPE3r }
        }
    };


    static const soc_reg_t mmu_pblk_cal_regs[2][8] = {
        {HPIPE0_PBLK0_CALENDARr, HPIPE0_PBLK1_CALENDARr,
         HPIPE0_PBLK2_CALENDARr, HPIPE0_PBLK3_CALENDARr,
         HPIPE0_PBLK4_CALENDARr, HPIPE0_PBLK5_CALENDARr,
         HPIPE0_PBLK6_CALENDARr, HPIPE0_PBLK7_CALENDARr },
        {HPIPE1_PBLK0_CALENDARr, HPIPE1_PBLK1_CALENDARr,
         HPIPE1_PBLK2_CALENDARr, HPIPE1_PBLK3_CALENDARr,
         HPIPE1_PBLK4_CALENDARr, HPIPE1_PBLK5_CALENDARr,
         HPIPE1_PBLK6_CALENDARr, HPIPE1_PBLK7_CALENDARr }
    };


    static int pblk_slots[SOC_TH_PORT_RATIO_COUNT][7] = {
        { 0, -1,  0,  0, -1,  0, -1 }, /* SOC_TH_PORT_RATIO_SINGLE */
        { 0, -1,  2,  0, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_1 */
        { 0,  0,  2,  0,  0,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_2_1 */
        { 0,  2,  2,  0,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_DUAL_1_2 */
        { 0, -1,  2,  0, -1,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_2_1_1 */
        { 0,  0,  2,  0,  0,  3, -1 }, /* SOC_TH_PORT_RATIO_TRI_023_4_1_1 */
        { 0, -1,  2,  1, -1,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_2 */
        { 0,  2,  2,  1,  2,  2, -1 }, /* SOC_TH_PORT_RATIO_TRI_012_1_1_4 */
        { 0, -1,  2,  1, -1,  3, -1 }  /* SOC_TH_PORT_RATIO_QUAD */
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);
    soc_tomahawk2_ovs_pipe_map_get(unit, port_schedule_state, &ovs_pipe_map);

    /* OVS tables  */
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < _TH2_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (slot = 0; slot < _TH2_OVS_GROUP_TDM_LENGTH; slot++) {
                    phy_port =
                        port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                        tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (phy_port >= TH2_NUM_EXT_PORTS) {
                        idb_port = 0x3f;
                        mmu_port = 0x3f;
                        id = 0x7;
                    } else {
                        port =
                            port_schedule_state->in_port_map.port_p2l_mapping[
                                phy_port];
                        idb_port =
                            port_schedule_state->in_port_map.port_l2i_mapping[
                                port]
                            & 0x3f;
                        mmu_port =
                            port_schedule_state->in_port_map.port_p2m_mapping[
                                phy_port] & 0x3f;
                        pm_num = (phy_port - 1)/_TH2_PORTS_PER_PBLK;
                        id =
                            port_schedule_state->out_port_map.
                            port_p2PBLK_inst_mapping
                            [phy_port]; /* this goes in PHY_PORT_IDf */
                    }
                    /* IDB OverSub Groups */
                    if (set_idb == 1) {
                        rval = 0;
                        reg = idb_grp_ovs_regs[pipe][hpipe][group];
                        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
                        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, idb_port);
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
                    }
                    if (set_mmu == 1) {
                        /* MMU OverSub Groups */
                        rval = 0;
                        reg = mmu_grp_ovs_regs[hpipe][group];
                        soc_reg_field_set(unit, reg, &rval, PHY_PORT_IDf, id);
                        soc_reg_field_set(unit, reg, &rval, PORT_NUMf, mmu_port);
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, reg, inst, slot, rval));
                    }
                } /*  for (slot = 0; slot < _TH2_... */

                /* Handle the case where the first element in the group
                 * is invalid but other valid ports exist in the group.
                 */
                for (slot = 0; slot < _TH2_OVS_GROUP_TDM_LENGTH; slot++) {
                    phy_port =
                        port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                        tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (phy_port < TH2_NUM_EXT_PORTS) {
                        break;
                    }
                }
                /* No valid ports found, skip group */
                if (_TH2_OVS_GROUP_TDM_LENGTH == slot) {
                    ovs_class = 0;
                    speed_max = 0;
                    ovs_same_spacing = 0;
                    ovs_sister_spacing = 0;
                } else {
                    ovs_class = 0;
                    port =
                        port_schedule_state->in_port_map.port_p2l_mapping[
                            phy_port];
                    prev_or_new = 1;
                    soc_tomahawk2_tdm_get_tdm_speed(unit, port_schedule_state,
                        port, prev_or_new, &l_speed, &l_num_lanes);
                    speed_max = l_speed;
                    _soc_tomahawk2_speed_to_ovs_class_mapping(unit, speed_max,
                                                              &ovs_class);
                    ovs_sister_spacing = 4;
                }
                if (set_idb == 1) {
                    rval = 0;
                    reg = idb_grp_cfg_regs[pipe][hpipe];
                    /* on ingress, all ports have HSP same spacing */
                    soc_tomahawk2_get_hsp_info(unit, speed_max,
                                   1, 0, &is_hsp, &ovs_same_spacing);
                    soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf, 
                                                        ovs_same_spacing); 
                    soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
                                      ovs_sister_spacing);
                    soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, REG_PORT_ANY, group, rval));
                }
                if (set_mmu == 1) {
                    rval = 0;
                    soc_tomahawk2_get_hsp_info(unit, speed_max,
                                   0, 1, &is_hsp, &ovs_same_spacing);
                    reg = mmu_grp_cfg_regs[hpipe];
                    soc_reg_field_set(unit, reg, &rval, SAME_SPACINGf,
                                      ovs_same_spacing);
                    soc_reg_field_set(unit, reg, &rval, SISTER_SPACINGf,
                                      ovs_sister_spacing);
                    soc_reg_field_set(unit, reg, &rval, SPEEDf, ovs_class);
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, reg, inst, group, rval));
                }
            }
        }
    }


    /* 16 Port-blocks are supported, with each instance of Oversubscription
     * Cell Scheduler supports up to 8 Portblocks. */

    /* First get pblk_cal_idx to pm_num mapping from
     * pm_num to pblk_cal_idx
     * and **oversub_schedule
     */
    sal_memset(pblks, -1, sizeof(pblks));
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }
        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (group = 0; group < _TH2_OVS_GROUP_COUNT_PER_HPIPE; group++) {
                for (slot = 0; slot < _TH2_OVS_GROUP_TDM_LENGTH; slot++) {
                    phy_port =
                        port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                        tdm_schedule_slice[hpipe].oversub_schedule[group][slot];
                    if (phy_port < TH2_NUM_EXT_PORTS) {
                        pm_num = (phy_port - 1) / _TH2_PORTS_PER_PBLK;
                        pblk_cal_idx =
                            port_schedule_state->out_port_map.
                            port_p2PBLK_inst_mapping
                            [phy_port];
                        if (pblk_cal_idx < 0) {
                            return SOC_E_FAIL; /* should be a valid mapping */
                        }
                        pblks[pipe][hpipe][pblk_cal_idx] = pm_num;
                    }
                }
            }
        }
    }


    /* Configure PBLKs */
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(ovs_pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            for (pblk_cal_idx = 0; pblk_cal_idx < _TH2_PBLKS_PER_HPIPE;
                 pblk_cal_idx++) {

                pm_num = pblks[pipe][hpipe][pblk_cal_idx];
                phy_port_base = 1 + (pm_num * _TH2_PORTS_PER_PBLK);
                /* Get port ratio for this Port Macro */
                soc_tomahawk2_port_ratio_get(unit, port_schedule_state, pm_num,
                                             &mode,
                                             1);
                reg  = idb_pblk_cal_regs[pipe][hpipe][pblk_cal_idx];
                reg1 = mmu_pblk_cal_regs[hpipe][pblk_cal_idx];
                if ((pm_num < 0) || (pm_num > _TH2_PBLKS_PER_DEV)) {
                    /* To speed-up init this branch could be skipped */
                    for (slot = 0; slot < 6; slot++) {
                        rval = 0;
                        rval1 = 0;
                        if (set_idb == 1) {
                            soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf,
                                              0x3f);
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg, REG_PORT_ANY, slot,
                                               rval));
                        }
                        if (set_mmu == 1) {
                            soc_reg_field_set(unit, reg1,  &rval1,  PORT_NUMf,
                                              0x3f);
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg1, inst, slot, rval1));
                        }
                    }
                } else {
                    for (slot = 0; slot < 6; slot++) {
                        /* only first 6 out of 7 could be valid */
                        rval = 0;
                        rval1 = 0;
                        lane = pblk_slots[mode][slot];
                        if (lane == -1) {
                            pblk_valid = 0;
                            pblk_spacing = 0;
                            mmu_port = 0x3f;
                            idb_port = 0x3f;
                        } else {
                            pblk_valid = 1;
                            mmu_port =
                                port_schedule_state->in_port_map.
                                port_p2m_mapping[
                                    phy_port_base + lane] & 0x3f;
                            port =
                                port_schedule_state->in_port_map.
                                port_p2l_mapping[
                                    phy_port_base + lane];
                            idb_port =
                                port_schedule_state->in_port_map.
                                port_l2i_mapping[
                                    port] & 0x3f;
                            prev_or_new = 1;
                            soc_tomahawk2_tdm_get_tdm_speed (unit, port_schedule_state, 
                                port, prev_or_new, &l_speed, &l_num_lanes);
                            soc_tomahawk2_get_hsp_info(unit, 
                                   l_speed,
                                   0, 1, &is_hsp, &pblk_spacing);
                            /* same spacing used only for MMU */
                        }
                        soc_reg_field_set(unit, reg,  &rval,  VALIDf,
                                          pblk_valid);
                        soc_reg_field_set(unit, reg,  &rval,  SPACINGf,  6);
                        soc_reg_field_set(unit, reg1, &rval1, VALIDf,
                                          pblk_valid);
                        soc_reg_field_set(unit, reg1, &rval1, SPACINGf,
                                          pblk_spacing);
                        soc_reg_field_set(unit, reg,  &rval,  PORT_NUMf,
                                          idb_port);
                        soc_reg_field_set(unit, reg1, &rval1, PORT_NUMf,
                                          mmu_port);
                        if (set_idb == 1) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg, REG_PORT_ANY, slot,
                                               rval));
                        }
                        if (set_mmu == 1) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, reg1, inst, slot, rval1));
                        }
                    }
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn void _soc_tomahawk2_speed_to_ovs_class_mapping(int unit, int speed, 
 *               int *ovs_class)
 *  @param unit   Chip unit number.
 *  @param speed Speed
 *  @param *ovs_class : ovs_class to be returned
 *  @brief Help function to calculate ovs class
 * Description:
 *     Help functiont to calculate ovs class
 * Oversubscription group speed class encoding
 * 0 - 0
 * 1 - 2 (10G)
 * 2 - 4 (20G)
 * 3 - 5 (25G)
 * 4 - 8 (40G)
 * 5 - 10 (50G)
 * 6 - 20 (100G)
 */
void
_soc_tomahawk2_speed_to_ovs_class_mapping(int unit, int speed, int *ovs_class)
{
    *ovs_class = 0;

    if (speed >= 40000) {
        if (speed >= 100000) {
            *ovs_class = 6;
        } else if (speed >= 50000) {
            *ovs_class = 5;
        } else {
            *ovs_class = 4;
        }
    } else {
        if (speed >= 25000) {
            *ovs_class = 3;
        } else if (speed >= 20000) {
            *ovs_class = 2;
        } else {
            *ovs_class = 1;
        }
    }
}


/*! @fn void soc_tomahawk2_get_hsp_info(
 *   int  unit,
 *   int  speed,
 *   int  is_idb,
 *   int  is_mmu,
 *   int *is_hsp,
 *   int *same_spacing);
 *  @param unit   Chip unit number.
 *  @param speed Speed
 *  @param is_idb : get for idb
 *  @param is_mmu : get for mmu
 *  @param is_hsp : if 1 is HSP
 *  @param same_spacing : same spacing
 *  @brief Help function to return if is hsp and sam spacing
 * Description:
 *     Help function to calculate HSP info
 */
void
soc_tomahawk2_get_hsp_info(
    int  unit,
    int  speed,
    int  is_idb,
    int  is_mmu,
    int *is_hsp,
    int *same_spacing)
{
    *is_hsp = 0;
    *same_spacing = 0;

    if (speed > 0) {
        if ((is_idb == 1) ||
            ((is_mmu == 1) &&
             ((speed == 20000) || 
              (speed >= 40000))) ) {
            *is_hsp = 1;
            *same_spacing = 6;
        } else {
            *is_hsp = 0;
            *same_spacing = 10;
        }
    }

}



/*! @fn void soc_tomahawk2_port_ratio_get(int unit, soc_port_schedule_state_t 
 *               *port_schedule_state, int clport, int *mode, int prev_or_new)
 *  @param unit   Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param clport PM block_id (0 to 63)
 *  @param *mode  mode to be returned
 *  @param prev_or_new function looks in in_port_map or out_port_map
 *  @brief Help function to get port ratio
 * Description:
 *     Help function to get port ratio
 */

/*
 * #1   single: 100  -   x  -   x  -   x  SOC_TH_PORT_RATIO_SINGLE
 * #2   single:  40  -   x  -   x  -   x  SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #3   dual:    50  -   x  -  50  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #4   dual:    40  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #5   dual:    20  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_1_1
 * #6   dual:    40  -   x  -  20  -   x  SOC_TH_PORT_RATIO_DUAL_2_1
 * #7   dual:    20  -   x  -  40  -   x  SOC_TH_PORT_RATIO_DUAL_1_2
 * #8   tri:     50  -   x  - 25/x - 25/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #9   tri:     20  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     40  -   x  - 10/x - 10/x SOC_TH_PORT_RATIO_TRI_023_4_1_1
 * #11  tri:    25/x - 25/x -  50  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    10/x - 10/x -  20  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_2
 * #13  tri:    10/x - 10/x -  40  -   x  SOC_TH_PORT_RATIO_TRI_012_1_1_4
 * #14  quad:   25/x - 25/x - 25/x - 25/x SOC_TH_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TH_PORT_RATIO_QUAD
 */
void
soc_tomahawk2_port_ratio_get(int unit,
                             soc_port_schedule_state_t *port_schedule_state,
                             int clport, int *mode,
                             int prev_or_new)
{
    int port, phy_port_base, lane;
    int num_lanes[_TH2_PORTS_PER_PBLK];
    int speed_max[_TH2_PORTS_PER_PBLK];
    soc_port_map_type_t *port_map;
    int l_speed, l_num_lanes;

    if (prev_or_new == 1) {
        port_map = &port_schedule_state->in_port_map;
    } else {
        port_map = &port_schedule_state->out_port_map;
    }

    phy_port_base = 1 + (clport * _TH2_PORTS_PER_PBLK);
    for (lane = 0; lane < _TH2_PORTS_PER_PBLK; lane += 2) {
        port = port_map->port_p2l_mapping[phy_port_base + lane];
        if ( (port == -1) || (port_map->log_port_speed[port]==0) ) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            soc_tomahawk2_tdm_get_tdm_speed (unit, port_schedule_state, port,
                prev_or_new, &l_speed, &l_num_lanes);
            num_lanes[lane] = l_num_lanes;
            speed_max[lane] = l_speed;
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_TH_PORT_RATIO_SINGLE;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        if (speed_max[0] == speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else if (speed_max[0] > speed_max[2]) {
            *mode = SOC_TH_PORT_RATIO_DUAL_2_1;
        } else {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_2;
        }
    } else if (num_lanes[0] == 2) {
        if (num_lanes[2] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[0] == 20000 || speed_max[0] == 21000 ||
                     speed_max[0] == 50000 || speed_max[0] == 53000) ?
                    SOC_TH_PORT_RATIO_TRI_023_2_1_1 :
                    SOC_TH_PORT_RATIO_TRI_023_4_1_1;
        }
    } else if (num_lanes[2] == 2) {
        if (num_lanes[0] == -1) {
            *mode = SOC_TH_PORT_RATIO_DUAL_1_1;
        } else {
            *mode = (speed_max[2] == 20000 || speed_max[2] == 21000 ||
                     speed_max[2] == 50000 || speed_max[2] == 53000) ?
                    SOC_TH_PORT_RATIO_TRI_012_1_1_2 :
                    SOC_TH_PORT_RATIO_TRI_012_1_1_4;
        }
    } else {
        *mode = SOC_TH_PORT_RATIO_QUAD;
    }
}


/*! @fn int _soc_tomahawk2_tdm_pkt_shaper_calendar_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to initialize the TH2 IDB Packet Shaper calendar
 * Description:
 *      API to initialize the TH2 IDB Packet Shaper calendar
 *      Regs/Mems configured: IDB PKT_SCH_CALENDAR
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_pkt_shaper_calendar_set(
    int unit, soc_port_schedule_state_t *port_schedule_state)
{
    uint32 pipe_map;
    soc_mem_t mem;
    int pipe, slot, hpipe;
    int port, phy_port, idb_port;
    uint32 memfld;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const soc_mem_t idb_pkt_shaper_mems[_TH2_PIPES_PER_DEV][2] = {
        {PKT_SCH_CALENDAR0_PIPE0m, PKT_SCH_CALENDAR1_PIPE0m},
        {PKT_SCH_CALENDAR0_PIPE1m, PKT_SCH_CALENDAR1_PIPE1m},
        {PKT_SCH_CALENDAR0_PIPE2m, PKT_SCH_CALENDAR1_PIPE2m},
        {PKT_SCH_CALENDAR0_PIPE3m, PKT_SCH_CALENDAR1_PIPE3m}
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        /* Configure packet shaper calendar */
        for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
            mem = idb_pkt_shaper_mems[pipe][hpipe];
            for (slot = 0; slot < SHAPING_GRP_LEN; slot++) {
                phy_port =
                    port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                    tdm_schedule_slice[hpipe].pkt_sch_or_ovs_space[0][slot];
                if ((phy_port >= TH2_NUM_EXT_PORTS) || (phy_port <= 0)) {
                    idb_port = _SOC_TH_TDM_UNUSED_TOKEN;
                } else {
                    port =
                        port_schedule_state->in_port_map.port_p2l_mapping[
                            phy_port];
                    idb_port =
                        port_schedule_state->in_port_map.port_l2i_mapping[port];
                }
                sal_memset(entry, 0, sizeof(uint32) *
                           soc_mem_entry_words(unit, PKT_SCH_CALENDAR0_PIPE0m));
                memfld = idb_port  & 0x3f;
                soc_mem_field_set(unit, mem, entry, PORT_NUMf, &memfld);
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                                  slot, entry));
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_idb_opportunistic_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int cpu_op_en, int lb_opp_en, int opp1_port_en,
 *              int opp2_port_en, int opp_ovr_sub_en)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param cpu_op_en
 *  @param lb_opp_en
 *  @param opp1_port_en
 *  @param opp2_port_en
 *  @param opp_ovr_sub_en
 *  @brief API to initialize the TH2 IDB Opportunistic related registers
 * Description:
 *      API to initialize the TH2 IDB Opportunistic related registers
 *      Regs/Mems configured: IS_CPU_LB_OPP_CFG_PIPE<pipe>, 
 *      IS_OPP_SCHED_CFG_PIPE<pipe>
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_idb_opportunistic_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cpu_op_en,
    int lb_opp_en,
    int opp1_port_en,
    int opp2_port_en,
    int opp_ovr_sub_en)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;
    static const soc_reg_t idb_cpu_lb_opp_cfg[_TH2_PIPES_PER_DEV] = {
        IS_CPU_LB_OPP_CFG_PIPE0r, IS_CPU_LB_OPP_CFG_PIPE1r,
        IS_CPU_LB_OPP_CFG_PIPE2r, IS_CPU_LB_OPP_CFG_PIPE3r
    };
    static const soc_reg_t idb_opp_sched_cfg[_TH2_PIPES_PER_DEV] = {
        IS_OPP_SCHED_CFG_PIPE0r, IS_OPP_SCHED_CFG_PIPE1r,
        IS_OPP_SCHED_CFG_PIPE2r, IS_OPP_SCHED_CFG_PIPE3r
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if ((pipe_map & (1 << pipe)) == 0) {
            /* Enable OPP1 to enable sbus access to the rest of IPIPE*/
            reg = idb_opp_sched_cfg[pipe];
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
                              (opp1_port_en == 1) ? 1 : 0);
            soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            continue;
        }

        reg = idb_cpu_lb_opp_cfg[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf,
                          (cpu_op_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf,
                          (lb_opp_en == 1) ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        reg = idb_opp_sched_cfg[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
                          (opp1_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf,
                          (opp2_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_SPACINGf, 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf,
                          (opp_ovr_sub_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_STRICT_PRIf, 1);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 35); /* NULL port */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_mmu_opportunistic_set(int unit, 
 *             soc_port_schedule_state_t *port_schedule_state, int cpu_op_en,
 *             int lb_opp_en, int opp1_port_en, int opp2_port_en, 
 *             int opp_ovr_sub_en)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param cpu_op_en
 *  @param lb_opp_en
 *  @param opp1_port_en
 *  @param opp2_port_en
 *  @param opp_ovr_sub_en
 *  @brief API to initialize the TH2 MMU Opportunistic related registers
 * Description:
 *      API to initialize the TH2 MMU Opportunistic related registers
 *      Regs/Mems configured: CPU_LB_OPP_CFG, OPP_SCHED_CFG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_mmu_opportunistic_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int cpu_op_en, int lb_opp_en, int opp1_port_en, 
    int opp2_port_en, int opp_ovr_sub_en)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int pipe;
    int inst;
    uint32 rval;

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }

        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;

        reg = CPU_LB_OPP_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, CPU_OPP_ENf,
                          (cpu_op_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, LB_OPP_ENf,
                          (lb_opp_en == 1) ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));

        reg = OPP_SCHED_CFGr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, OPP1_PORT_ENf,
                          (opp1_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP1_SPACINGf, 6);
        soc_reg_field_set(unit, reg, &rval, OPP2_PORT_ENf,
                          (opp2_port_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP2_SPACINGf, 0);
        soc_reg_field_set(unit, reg, &rval, OPP_OVR_SUB_ENf,
                          (opp_ovr_sub_en == 1) ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, OPP_STRICT_PRIf, 0);
        soc_reg_field_set(unit, reg, &rval, DISABLE_PORT_NUMf, 35); /* NULL port */
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_idb_hsp_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to initialize the TH2 IDB HSP.
 * Description:
 *      API to initialize the TH2 IDB HSP.
 *      Regs/Mems configured: IS_TDM_HSP_PIPE<pipe>
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_idb_hsp_set(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 port_map[_TH2_PIPES_PER_DEV]; /* works if regular number of logical 
                                          * ports per pipe <=32
                                          */
    uint32 rval;
    static const soc_reg_t idb_tdm_hsp[_TH2_PIPES_PER_DEV] = {
        IS_TDM_HSP_PIPE0r, IS_TDM_HSP_PIPE1r,
        IS_TDM_HSP_PIPE2r, IS_TDM_HSP_PIPE3r
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    /* IDB always configures it to 0xFFFFFFFF independent of the port's speed */
    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        port_map[pipe] = 0xffffffff;
        rval = 0;
        reg = idb_tdm_hsp[pipe];
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_mmu_hsp_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to initialize the TH2 MMU HSP.
 * Description:
 *      API to initialize the TH2 MMU HSP.
 *      Regs/Mems configured: TDM_HSP_PIPE
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_mmu_hsp_set(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe, inst;
    int port, mmu_port, phy_port;
    int is_hsp, same_spacing;
    uint32 port_map[_TH2_PIPES_PER_DEV]; /* it works if the regular number 
                                          * of logical ports per pipe <=32
                                          */
    uint32 rval;
    uint32 max_phy_ports;
    int l_speed, l_num_lanes, prev_or_new;

    max_phy_ports = _TH2_PORTS_PER_PBLK * _TH2_PBLKS_PER_PIPE *
                    _TH2_PIPES_PER_DEV;  /* 4*16*4=256 */

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        port_map[pipe] = 0;
    }

    for (port=0; port<_TH2_DEV_PORTS_PER_DEV; port++) {
        /* Set if port speed is 20G or higher than 40G */
        prev_or_new = 0;
        soc_tomahawk2_tdm_get_tdm_speed (unit, port_schedule_state, port,
                                   prev_or_new, &l_speed, &l_num_lanes);
        soc_tomahawk2_get_hsp_info(unit, l_speed,
                                   0, 1, &is_hsp, &same_spacing);
        if (is_hsp == 1) {
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if ((1 <= phy_port) && (phy_port <= max_phy_ports)) {
                pipe =
                    (phy_port - 1) / (_TH2_PBLKS_PER_PIPE * _TH2_PORTS_PER_PBLK);   
                mmu_port =
                    port_schedule_state->out_port_map.port_p2m_mapping[phy_port
                    ] &
                    0x3f;
                port_map[pipe] |= 1 << mmu_port;
            }
        }
    }

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = TDM_HSPr;
        rval = 0;
        soc_reg_field_set(unit, reg, &rval, PORT_BMPf, port_map[pipe]);
        inst = pipe | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, inst, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_idb_ppe_credit_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state,
 *              int full_credit_threshold_0, int opp_credit_threshold_0,
 *              int full_credit_threshold_1, int opp_credit_threshold_1 )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param full_credit_threshold_0
 *  @param opp_credit_threshold_0
 *  @param full_credit_threshold_1
 *  @param opp_credit_threshold_1
 *  @brief API to initialize the TH2 IDB PPE Credit Config register.
 * Description:
 *      API to initialize the TH2 IDB PPE Credit Config register.
 *      Regs/Mems configured:PPE_CREDIT_CONFIG_PIPE<pipe>
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_idb_ppe_credit_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int full_credit_threshold_0,
    int opp_credit_threshold_0,
    int full_credit_threshold_1,
    int opp_credit_threshold_1 )
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;
    static const soc_reg_t idb_ppe_credit[_TH2_PIPES_PER_DEV] = {
        PPE_CREDIT_CONFIG_PIPE0r, PPE_CREDIT_CONFIG_PIPE1r,
        PPE_CREDIT_CONFIG_PIPE2r, PPE_CREDIT_CONFIG_PIPE3r
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = idb_ppe_credit[pipe];
        rval = 0;
        /*SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));*/
        soc_reg_field_set(unit, reg, &rval, FULL_CREDIT_THRESHOLD_0f,
                          full_credit_threshold_0);
        soc_reg_field_set(unit, reg, &rval, OPP_CREDIT_THRESHOLD_0f,
                          opp_credit_threshold_0);
        soc_reg_field_set(unit, reg, &rval, FULL_CREDIT_THRESHOLD_1f,
                          full_credit_threshold_1);
        soc_reg_field_set(unit, reg, &rval, OPP_CREDIT_THRESHOLD_1f,
                          opp_credit_threshold_1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_idb_dpp_ctrl_set(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state, int credits)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct 
 *         variable.
 *  @param credits
 *  @brief API to initialize the TH2 IDB_DPP_CTRL register.
 */
int
_soc_tomahawk2_tdm_idb_dpp_ctrl_set(
    int unit, soc_port_schedule_state_t *port_schedule_state,
    int credits)
{
    soc_reg_t reg;
    uint32 pipe_map;
    int pipe;
    uint32 rval;
    static const soc_reg_t idb_dpp_ctrl[_TH2_PIPES_PER_DEV] = {
        IDB_DPP_CTRL_PIPE0r, IDB_DPP_CTRL_PIPE1r,
        IDB_DPP_CTRL_PIPE2r, IDB_DPP_CTRL_PIPE3r
    };

    soc_tomahawk2_pipe_map_get(unit, port_schedule_state, &pipe_map);

    for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        reg = idb_dpp_ctrl[pipe];
        rval = 0;
        /*SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));*/
        soc_reg_field_set(unit, reg, &rval, MAX_REF_CNTf, 4);
        soc_reg_field_set(unit, reg, &rval, MAX_SBUS_CNTf, 4);
        soc_reg_field_set(unit, reg, &rval, CREDITSf, credits);
        soc_reg_field_set(unit, reg, &rval, DONEf, 0);
        soc_reg_field_set(unit, reg, &rval, STARTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}


/*! @fn int _soc_tomahawk2_tdm_calculation(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *             variable.
 *  @brief API to calculate TDM tables
 * Description:
 *      API to calculate TDM tables
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
_soc_tomahawk2_tdm_calculation(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state)
{
    int i, port, phy_port, pm_num;
    int pipe, hpipe;
    int lane, group, index, cal_id, cal_grp;
    int *tdm_idb_pipe_main, *tdm_mmu_pipe_main;
    int **tdm_pipe_group;
    tdm_soc_t _chip_pkg;
    tdm_mod_t *_tdm_pkg;
    int mgmt_pm_hg = 0;
    uint32 speed[_TH2_PHY_PORTS_PER_DEV];
    uint32 port_state[_TH2_PHY_PORTS_PER_DEV]; /* enum port_state_e */
    int l_speed, l_num_lanes, prev_or_new;

    sal_memset(speed, 0, _TH2_PHY_PORTS_PER_DEV*sizeof(uint32));
    sal_memset(port_state, 0, _TH2_PHY_PORTS_PER_DEV*sizeof(uint32));

    /* Remap input speeds to ETH bitrates */
    soc_tomahawk2_port_schedule_speed_remap(unit, port_schedule_state);

    for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->in_port_map.log_port_speed[port] > 0 ) {
            phy_port = port_schedule_state->in_port_map.port_l2p_mapping[port];
            prev_or_new = 1;
            soc_tomahawk2_tdm_get_tdm_speed (unit, port_schedule_state, port,
                prev_or_new, &l_speed, &l_num_lanes);
            if (SOC_PBMP_MEMBER(port_schedule_state->in_port_map.hg2_pbm,
                                port)) {
                port_state[phy_port] =
                    SOC_PBMP_MEMBER(
                        port_schedule_state->in_port_map.oversub_pbm, port) ?
                    PORT_STATE__OVERSUB_HG : PORT_STATE__LINERATE_HG;
                for (lane = 1;
                     lane <
                     l_num_lanes;
                     lane++) {
                    port_state[phy_port + lane] = PORT_STATE__COMBINE_HG;
                }
            } else {
                port_state[phy_port] =
                    SOC_PBMP_MEMBER(
                        port_schedule_state->in_port_map.oversub_pbm, port) ?
                    PORT_STATE__OVERSUB : PORT_STATE__LINERATE;
                for (lane = 1;
                     lane <
                     l_num_lanes;
                     lane++) {
                    port_state[phy_port + lane] = PORT_STATE__COMBINE;
                }
            }
            speed[phy_port] = l_speed;
        }
    }

    /* Management ports */
    SOC_PBMP_ITER(port_schedule_state->in_port_map.management_pbm, port) {
        if (SOC_PBMP_MEMBER(port_schedule_state->in_port_map.hg2_pbm,port)) {
            mgmt_pm_hg = 1; /* HIGIG2 : ETHERNET */
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "frequency: %dMHz\n"),
                  port_schedule_state->frequency));
        LOG_DEBUG(BSL_LS_SOC_PORT,  (BSL_META_U(unit, "port speed:")));
        for (index = 0; index < TH2_NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT,  (BSL_META_U(unit,
                      "port=%d speed=%6dG"), index, speed[index]/1000));
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "port state map:")));
        for (index = 0; index < TH2_NUM_EXT_PORTS; index++) {
            if (index % 16 == 0) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
            }
            if (index == 0 || index == (TH2_NUM_EXT_PORTS - 1)) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " ---")));
            } else {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "port=%d state=%3d"), index,
                          port_state[index]));
            }
        }
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
    }

    /* Inputs for SW TDM Generation:
     * unit, num_ext_ports, speed, state, clk_freq, mgmt_pm_hg */
    _chip_pkg.unit = unit;
    _chip_pkg.num_ext_ports = TH2_NUM_EXT_PORTS;
    _chip_pkg.state=sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *),
                              "port state list");
    if (_chip_pkg.state == NULL) {
        return SOC_E_MEMORY;
    }
    _chip_pkg.speed = sal_alloc((_chip_pkg.num_ext_ports)*sizeof(int *),
                                "port speed list");
    if (_chip_pkg.speed == NULL) {
        sal_free(_chip_pkg.state);
        return SOC_E_MEMORY;
    }
    for (index=0; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.speed[index] = speed[index];
    }
    /* Shift state to left one position; required by C_TDM */
    for (index=1; index < _chip_pkg.num_ext_ports; index++) {
        _chip_pkg.state[index-1] = port_state[index];
    }

    _chip_pkg.clk_freq = port_schedule_state->frequency;
    _chip_pkg.soc_vars.th2.mgmt_pm_hg= mgmt_pm_hg;
    _chip_pkg.soc_vars.th2.cal_universal_en =
                    (port_schedule_state->calendar_type == 0) ? 1 : 0;
    _chip_pkg.tvec_size = 0;
    _chip_pkg.flex_port_en = port_schedule_state->is_flexport;
    _chip_pkg.pmap = NULL; /* fix COVERITY error UNINIT */
    /* coverity[uninit_use_in_call : FALSE] */
    _tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&_chip_pkg));
    sal_free(_chip_pkg.state);
    sal_free(_chip_pkg.speed);

    if (!_tdm_pkg) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "Unsupported config for TDM calendar generation\n")));
        return SOC_E_FAIL;
    }
    else {
        /* IDB calendar (0-3), MMU calendar (4-7) */
        for (cal_id = 0; cal_id < _TH2_PIPES_PER_DEV; cal_id++) {
            switch (cal_id) {
            case 0:
                tdm_pipe_group      = _tdm_pkg->_chip_data.cal_0.cal_grp;
                tdm_idb_pipe_main   = _tdm_pkg->_chip_data.cal_0.cal_main;
                tdm_mmu_pipe_main   = _tdm_pkg->_chip_data.cal_4.cal_main;
                break;
            case 1:
                tdm_pipe_group      = _tdm_pkg->_chip_data.cal_1.cal_grp;
                tdm_idb_pipe_main   = _tdm_pkg->_chip_data.cal_1.cal_main;
                tdm_mmu_pipe_main   = _tdm_pkg->_chip_data.cal_5.cal_main;
                break;
            case 2:
                tdm_pipe_group      = _tdm_pkg->_chip_data.cal_2.cal_grp;
                tdm_idb_pipe_main   = _tdm_pkg->_chip_data.cal_2.cal_main;
                tdm_mmu_pipe_main   = _tdm_pkg->_chip_data.cal_6.cal_main;
                break;
            case 3:
                tdm_pipe_group      = _tdm_pkg->_chip_data.cal_3.cal_grp;
                tdm_idb_pipe_main   = _tdm_pkg->_chip_data.cal_3.cal_main;
                tdm_mmu_pipe_main   = _tdm_pkg->_chip_data.cal_7.cal_main;
                break;
            default:
                tdm_pipe_group      = NULL;
                tdm_idb_pipe_main   = NULL;
                tdm_mmu_pipe_main   = NULL;
                return SOC_E_FAIL;
            }
            /* IDB TDM main calendar */
            sal_memcpy(
                port_schedule_state->tdm_ingress_schedule_pipe[cal_id].
                tdm_schedule_slice[0].linerate_schedule,
                tdm_idb_pipe_main, sizeof(int)*_TH2_TDM_LENGTH);

            /* MMU TDM main calendar */
            sal_memcpy(
                port_schedule_state->tdm_egress_schedule_pipe[cal_id].
                tdm_schedule_slice[0].linerate_schedule,
                tdm_mmu_pipe_main, sizeof(int)*_TH2_TDM_LENGTH);

            /* IDB and MMU OverSub Groups */
            for (index = 0; index < _TH2_OVS_GROUP_COUNT_PER_PIPE;
                 index++) {
                /* Group 0:5 -> hpipe 0, Group 6:11 -> hpipe1 */
                hpipe = index / _TH2_OVS_GROUP_COUNT_PER_HPIPE;
                cal_grp = index % _TH2_OVS_GROUP_COUNT_PER_HPIPE;
                sal_memcpy(
                    port_schedule_state->tdm_ingress_schedule_pipe[
                        cal_id].tdm_schedule_slice[hpipe].oversub_schedule[
                        cal_grp],
                    tdm_pipe_group[index], sizeof(int) *
                    _TH2_OVS_GROUP_TDM_LENGTH);
                sal_memcpy(
                    port_schedule_state->tdm_egress_schedule_pipe[cal_id].
                    tdm_schedule_slice[hpipe].oversub_schedule[cal_grp],
                    tdm_pipe_group[index], sizeof(int) *
                    _TH2_OVS_GROUP_TDM_LENGTH);
            }

            /* Packet shaper */
            for (hpipe = 0; hpipe < 2; hpipe++) {
                sal_memcpy(
                    port_schedule_state->tdm_ingress_schedule_pipe[
                        cal_id].tdm_schedule_slice[hpipe].pkt_sch_or_ovs_space[
                        0],
                    tdm_pipe_group[hpipe+SHAPING_GRP_IDX_0],
                    sizeof(int) * SHAPING_GRP_LEN);
            }
        }
        /* Re-evaluate this at FlexPort;
         * Before calling FlexPort port_schedule_state->
         * in_port_map.port_p2PBLK_inst_mapping should be copied in pm_num_to_pblk
         * After FlexPort pm_num_to_pblk should be copied back to 
         * port_schedule_state->out_port_map.port_p2PBLK_inst_mapping
         */
        sal_memset(port_schedule_state->out_port_map.port_p2PBLK_inst_mapping,
                   0,
                   sizeof(int) * SOC_MAX_NUM_PORTS);
        for (pm_num = 0; pm_num < TH2_NUM_PHY_PM; pm_num++) {
             /* _TH2_PBLKS_PER_PIPE * _TH2_PIPES_PER_DEV */
            for (port = 0; port < _TH2_PORTS_PER_PBLK; port++) {
                port_schedule_state->out_port_map.port_p2PBLK_inst_mapping[(
                                         pm_num * _TH2_PORTS_PER_PBLK)
                                         + 1 + port] =
                    _tdm_pkg->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[
                        pm_num];
            }
        }
        /* Free the memory allocated in TDM algorithm code */
        /* This function frees all pointers allocated in _soc_set_tdm_tbl call */
        tdm_th2_dealloc(_tdm_pkg);
        sal_free(_tdm_pkg);
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pipe = 0; pipe < _TH2_PIPES_PER_DEV; pipe++) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Pipe %d idb_tdm:"), pipe));
            for (index = 0; index < _TH2_TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                          port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                          tdm_schedule_slice[0].linerate_schedule[index] ));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
            for (hpipe = 0; hpipe < _TH2_OVS_HPIPE_COUNT_PER_PIPE; hpipe++) {
                for (group = 0; group < _TH2_OVS_GROUP_COUNT_PER_HPIPE;
                     group++) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                              "Pipe %d hpipe %d group %d ovs_tdm"),
                              pipe, hpipe, group));
                    for (index = 0; index < _TH2_OVS_GROUP_TDM_LENGTH;
                         index++) {
                        if (index == 0) {
                            LOG_DEBUG(BSL_LS_SOC_PORT,
                                      (BSL_META_U(unit, "\n    ")));
                        }
                        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                                  port_schedule_state->tdm_ingress_schedule_pipe
                                  [pipe].tdm_schedule_slice[hpipe].
                                  oversub_schedule[group]
                                  [index]));
                    }
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Pipe %d hpipe %d IDB PKT SCHEDULER\n"),
                          pipe, hpipe));
                for (index = 0; index < SHAPING_GRP_LEN; index++) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                              port_schedule_state->tdm_ingress_schedule_pipe[
                                 pipe].tdm_schedule_slice[hpipe].
                              pkt_sch_or_ovs_space[0][
                                 index]));
                    if (index % 16 == 0) {
                        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
                    }
                }
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Pipe %d mmu_tdm:"), pipe));
            for (index = 0; index < _TH2_TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n    ")));
                }
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, " %3d"),
                          port_schedule_state->tdm_egress_schedule_pipe[pipe].
                          tdm_schedule_slice[0].linerate_schedule[index] ));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\n")));
        }
    }


    if (port_schedule_state->is_flexport == 0) {
        /* If init then just copy in_port_map to out_port_map*/
        for (i= 0; i< SOC_MAX_NUM_PORTS; i++) {
            port_schedule_state->out_port_map.log_port_speed[i]   =
                port_schedule_state->in_port_map.log_port_speed[i];
            port_schedule_state->out_port_map.port_p2l_mapping[i] =
                port_schedule_state->in_port_map.port_p2l_mapping[i];
            port_schedule_state->out_port_map.port_l2p_mapping[i] =
                port_schedule_state->in_port_map.port_l2p_mapping[i];
            port_schedule_state->out_port_map.port_p2m_mapping[i] =
                port_schedule_state->in_port_map.port_p2m_mapping[i];
            port_schedule_state->out_port_map.port_num_lanes[i]   =
                port_schedule_state->in_port_map.port_num_lanes[i];
            port_schedule_state->out_port_map.port_l2i_mapping[i] =
                port_schedule_state->in_port_map.port_l2i_mapping[i];
        }
        for (i = 0; i < SOC_MAX_NUM_MMU_PORTS; i++) {
            port_schedule_state->out_port_map.port_m2p_mapping[i] =
                port_schedule_state->in_port_map.port_m2p_mapping[i];
        }
        for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
            port_schedule_state->out_port_map.physical_pbm.pbits[i]   =
                port_schedule_state->in_port_map.physical_pbm.pbits[i];
            port_schedule_state->out_port_map.hg2_pbm.pbits[i]        =
                port_schedule_state->in_port_map.hg2_pbm.pbits[i];
            port_schedule_state->out_port_map.management_pbm.pbits[i] =
                port_schedule_state->in_port_map.management_pbm.pbits[i];
            port_schedule_state->out_port_map.oversub_pbm.pbits[i]    =
                port_schedule_state->in_port_map.oversub_pbm.pbits[i];
        }
    }
    /*
     * else {
     *    HERE future handling of FlexPort between in_port_map to out_port_map
     * }
     */

    return SOC_E_NONE;
}


/*! @fn void soc_tomahawk2_pipe_map_get(int unit, 
 *               soc_port_schedule_state_t *port_schedule_state, uint32 *pipe_map)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct 
 *         variable.
 *  @param pipe_map Returned pipe map.
 *  @brief Handler that returns pipe map
 * Description:
 *      Gets pipe map;
 * If init ,then a pipe gets 1 if any port active within that pipe
 * If flexport, then a pipe gets 1 if any port flexes (UP/DOWN) within that pipe
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      *pipe_map - returned pipe map
 * Return Value:
 *      SOC_E_*
 */
void
soc_tomahawk2_pipe_map_get(int                        unit,
                           soc_port_schedule_state_t *port_schedule_state,
                           uint32 *                   pipe_map)
{
    uint32 port, pipe, i;

    *pipe_map = 0;
    if (port_schedule_state->is_flexport == 0) { /* Init */
        for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
            if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
                pipe = port / _TH2_DEV_PORTS_PER_PIPE;
                *pipe_map |= 1 << pipe;
            }
        }
    } else { /* FlexPort */
        for (i = 0; i < port_schedule_state->nport; i++) {
            port  = port_schedule_state->resource[i].logical_port;
            pipe = port / _TH2_DEV_PORTS_PER_PIPE;
            *pipe_map |= 1 << pipe;
        }
    }
}


/*! @fn void soc_tomahawk2_ovs_pipe_map_get(int unit, 
 *               soc_port_schedule_state_t *port_schedule_state,
 *               uint32 *ovs_pipe_map)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable.
 *  @param ovs_pipe_map Returned pipe map.
 *  @brief Handler that returns OVS pipe map
 * Description:
 *      Gets OVS pipe map;
 * If init ,then a pipe gets 1 if any port active within that pipe
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 *      *ovs_pipe_map - returned pipe map
 * Return Value:
 *      SOC_E_*
 */
void
soc_tomahawk2_ovs_pipe_map_get(int                        unit,
                               soc_port_schedule_state_t *port_schedule_state,
                               uint32 *                   ovs_pipe_map)
{
    uint32 port, pipe;
    int is_ovs_map;

    *ovs_pipe_map = 0;
    if (port_schedule_state->is_flexport == 0) { /* Init */
        for (port = 0; port < _TH2_DEV_PORTS_PER_DEV; port++) {
            is_ovs_map = SOC_PBMP_MEMBER(
                port_schedule_state->out_port_map.oversub_pbm, port) ? 1 : 0;
            if ((port_schedule_state->out_port_map.log_port_speed[port] > 0) &&
                (1 == is_ovs_map) ) {
                pipe = port / _TH2_DEV_PORTS_PER_PIPE;
                *ovs_pipe_map |= 1 << pipe;
            }
        }
    }
}


/*! @fn int soc_tomahawk2_tdm_init(int unit, 
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief Main API for TDM init
 * Description:
 *      Main API for TDM init
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk2_tdm_init(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_calculation(unit,
                                                       port_schedule_state));

    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_idb_calendar_set(unit,
                                                            port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_idb_hsp_set(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_pkt_shaper_calendar_set(unit,
                                                                   port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_idb_ppe_credit_set(unit,
                                                              port_schedule_state,
                                                              16, 8, 16, 8));
    /* IDB opp1_port_en should always be 1 to enable sbus access to the 
     * rest of IPIPE
     */
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_idb_opportunistic_set(unit,
                                                                 port_schedule_state,
                                                                 1, 1, 1, 1, 1));

    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_oversub_group_set(unit,
                                                             port_schedule_state));

    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_mmu_calendar_set(unit,
                                                            port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_mmu_hsp_set(unit,
                                                       port_schedule_state));
    SOC_IF_ERROR_RETURN(_soc_tomahawk2_tdm_mmu_opportunistic_set(unit,
                                                                 port_schedule_state,
                                                                 1, 1, 1, 1, 1));

    return SOC_E_NONE;
}


/*! @fn void soc_print_port_schedule_state(int unit, 
 *               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief Debug API to print port_schedule_state
 */
void
soc_print_port_schedule_state(int                        unit,
                              soc_port_schedule_state_t *port_schedule_state)
{
    int i, pipe, hpipe, print_tdm;
    print_tdm = 1;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "\n\nPRINTING soc_port_schedule_state_t \n")));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "nport= %3d\n"), port_schedule_state->nport));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "frequency= %4dMHz\n"), port_schedule_state->frequency));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "bandwidth= %4d\n"), port_schedule_state->bandwidth));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "io_bandwidth= %4d\n"),
              port_schedule_state->io_bandwidth));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "lossless= %1d\n"), port_schedule_state->lossless));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "is_flexport= %1d\n"), port_schedule_state->is_flexport));
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "calendar_type= %1d\n"), port_schedule_state->calendar_type));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "\nPRINTING soc_port_schedule_state_t::in_port_map\n")));
    soc_print_port_map(unit, &port_schedule_state->in_port_map);

    if (port_schedule_state->is_flexport == 1) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\nPRINTING soc_port_schedule_state_t::out_port_map\n")));
        soc_print_port_map(unit, &port_schedule_state->out_port_map);
    }

    if (print_tdm == 1) {
        for (pipe = 0; pipe< SOC_MAX_NUM_PIPES; pipe++) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "\n====== TDM Tables PIPE=%1d INGRESS\n"), pipe));
            for (hpipe = 0; hpipe< MAX_SCH_SLICES; hpipe++) {
                soc_print_tdm_schedule_slice(
                    unit,
                    &port_schedule_state->tdm_ingress_schedule_pipe[pipe].
                    tdm_schedule_slice
                    [hpipe], hpipe, 1);
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "\n====== TDM Tables PIPE=%1d  EGRESS\n"), pipe));
            for (hpipe = 0; hpipe< MAX_SCH_SLICES; hpipe++) {
                soc_print_tdm_schedule_slice(
                    unit,
                    &port_schedule_state->tdm_egress_schedule_pipe[pipe].
                    tdm_schedule_slice
                    [hpipe],  hpipe, 0);
            }
        }
    }


    if ((port_schedule_state->is_flexport == 1) &&
        (port_schedule_state->nport > 0)) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\n====== FLEXPORT RESOURCES ===== num entries= %d = \n"),
                  port_schedule_state->nport));
        for (i = 0; i < port_schedule_state->nport; i++) {
            soc_print_port_resource(unit, &port_schedule_state->resource[i], i);
        }
    }

}

/*! @fn void soc_print_port_map(int unit, soc_port_map_type_t *port_map)
 *  @param unit Chip unit number.
 *  @param port_map Pointer to a soc_port_map_type_t struct variable.
 *  @brief Debug API to print soc_port_map_type_t
 */
void
soc_print_port_map(int unit, soc_port_map_type_t *port_map)
{
    int port;
    int is_hg2, is_ovs, is_mgm;
    int speed, phy_port, idb_port, mmu_port, num_lanes, pblk;
    int phy_bmap, p2l_map, m2p_map;

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (port_map->log_port_speed[port] > 0) {
            speed     = port_map->log_port_speed[port]/1000;
            phy_port  = port_map->port_l2p_mapping[port];
            idb_port  = port_map->port_l2i_mapping[port];
            mmu_port  = port_map->port_p2m_mapping[phy_port];
            num_lanes = port_map->port_num_lanes[port];
            pblk      = port_map->port_p2PBLK_inst_mapping[phy_port];
            phy_bmap  =
                SOC_PBMP_MEMBER(port_map->physical_pbm, phy_port) ? 1 : 0;
            /* coverity[overrun-local] */
            is_hg2    = SOC_PBMP_MEMBER(port_map->hg2_pbm,port) ? 1 : 0;
            /* coverity[overrun-local] */
            is_ovs    = SOC_PBMP_MEMBER(port_map->oversub_pbm, port) ? 1 : 0;
            p2l_map   = port_map->port_p2l_mapping[phy_port];
            m2p_map   = port_map->port_m2p_mapping[mmu_port];
            /* coverity[overrun-local] */
            is_mgm       =
                SOC_PBMP_MEMBER(port_map->management_pbm, port) ? 1 : 0;
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "log_port=%3d phy_port=%3d speed=%3dG  %s %s "
                      "idb_port=%3d mmu_port=%3d num_lanes=%1d pblk=%3d "
                      "phy_bmap=%1d p2l_map=%3d "),
                      port, phy_port, speed,
                      (is_hg2==1) ? "HG2" : "ETH", (is_ovs==1) ? "OVS" : " LR",
                      idb_port, mmu_port, num_lanes, pblk, phy_bmap, p2l_map));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "m2p_map=%3d %s\n"),  m2p_map,
                      (is_mgm==1) ? "MGM" : "" ));
        }
    }

}


/*! @fn void soc_print_tdm_schedule_slice(int unit, 
 *               soc_tdm_schedule_t *tdm_schedule, int hpipe, int is_ing)
 *  @param unit Chip unit number.
 *  @param tdm_schedule Pointer to a soc_tdm_schedule_t struct variable.
 *  @param hpipe Half Pipe.
 *  @param is_ing distinguishes between IP and EP schedulers
 *  @brief Debug API to print soc_tdm_schedule_t
 */
void
soc_print_tdm_schedule_slice(int unit, soc_tdm_schedule_t *tdm_schedule,
                             int hpipe,
                             int is_ing)
{
    int cal_length, j, m;

    if (hpipe == 0) {
        for (cal_length = tdm_schedule->cal_len; cal_length > 0;
             cal_length--) {
            if (tdm_schedule->linerate_schedule[cal_length - 1] !=
                TH2_NUM_EXT_PORTS) {
                break;
            }
        }

        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\tMAIN CALENDAR: cal_length=%3d \n"), cal_length ));
        for (j=0; j < cal_length; j++) {
            if (j%32==0) {
              LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"\n %3d : %3d\t"),
                        j, j+31));
            }
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"%5d"),
                      tdm_schedule->linerate_schedule[j]));
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
              "\n\n\tOVS TABLES: HPIPE= %3d \n"), hpipe ));
    for (j=0; j< tdm_schedule->num_ovs_groups; j++) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"\nOVS_CAL[%d]:\t"), j ));
        for (m=0; m< tdm_schedule->ovs_group_len; m++) {
            if (tdm_schedule->oversub_schedule[j][m] != TH2_NUM_EXT_PORTS) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "%d\t"),
                          tdm_schedule->oversub_schedule[j][m] ));
            } else {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"---\t")));
            }
        }
    }

    if (is_ing == 1) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "\n\n\tPKT_SCHEDULER: HPIPE= %3d \n"), hpipe ));
        for (j=0; j<tdm_schedule->pkt_sch_or_ovs_space_len; j++) {
            if (j%16==0) {
              LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"\n %3d : %3d\t"),
                        j, j+15 ));
            }
            if (tdm_schedule->pkt_sch_or_ovs_space[0][j] !=
                TH2_NUM_EXT_PORTS) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit, "\t%d"),
                          tdm_schedule->pkt_sch_or_ovs_space[0][j] ));
            } else {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"\t---")));
            }
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,"\n")));
}


/*! @fn void soc_print_port_resource(int unit, soc_port_resource_t *port_resource,
 *               int entry_num)
 *  @param unit Chip unit number.
 *  @param port_resource Pointer to a soc_port_resource_t struct variable.
 *  @param entry_num entry number.
 *  @brief Debug API to print soc_port_resource_t
 */
void
soc_print_port_resource(int unit, soc_port_resource_t *port_resource,
                        int entry_num)
{
    int is_hg2, lane;

    is_hg2 = 0;
    if (port_resource->encap == _SHR_PORT_ENCAP_HIGIG2) {is_hg2=1; }

    if (port_resource->physical_port == -1) {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "nport=%3d DOWN: logical_port=%3d physical_port=%3d\n"),
                  entry_num, port_resource->logical_port,
                  port_resource->physical_port));
    } else {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "nport=%3d  UP : logical_port=%3d physical_port=%3d mmu_port=%3d "),
                  entry_num, port_resource->logical_port,
                  port_resource->physical_port, port_resource->mmu_port ));
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "idb_port=%3d pipe=%3d speed=%3d num_lanes=%1d  %s  %s\n"),
                  port_resource->idb_port,
                  port_resource->pipe, port_resource->speed/1000,
                  port_resource->num_lanes, (is_hg2==1) ? "HG2" : "ETH",
                  (port_resource->oversub==1) ? "OVS" : " LR" ));
        for (lane = 0; lane < SOC_PORT_RESOURCE_LANES_MAX; lane++) {
            if (port_resource->lane_info[lane] != NULL) {
                LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "nport=%3d lane_info[%d]: pgw=%3d xlp=%3d port_index=%3d\n"),
                          entry_num, lane, port_resource->lane_info[lane]->pgw,
                          port_resource->lane_info[lane]->xlp,
                          port_resource->lane_info[lane]->port_index));
            }
        }
    }
}


/*! @fn int soc_tomahawk2_port_speed_higig2eth(int speed)
 *  @param int speed
 *  @brief Map port speed to ETH bitrates
 */
static int
soc_tomahawk2_port_speed_higig2eth(int speed)
{
    switch (speed) {
    case 11000:
        /* 10G */
        return 10000;
    case 21000:
        /* 20G */
        return 20000;
    case 27000:
        /* 25G */
        return 25000;
    case 42000:
        /* 40G */
        return 40000;
    case 53000:
        /* 50G */
        return 50000;
    case 106000:
        /* 100G */
        return 100000;
    default:
        return speed;
    }
}


/*! @fn void soc_tomahawk2_port_schedule_speed_remap(int unit, 
 *               soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @brief API to remap speeds to ETH bitrates
 */
void
soc_tomahawk2_port_schedule_speed_remap(
    int  unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, port;

    /* Speed remap for in_port_map*/
    for (port=0; port < SOC_MAX_NUM_PORTS; port++) {
        port_schedule_state->in_port_map.log_port_speed[port] = 
            soc_tomahawk2_port_speed_higig2eth(port_schedule_state->in_port_map.log_port_speed[port]);
    }

    if (port_schedule_state->is_flexport == 1) {
        /* Speed remap for out_port_map*/
        for (port=0; port < SOC_MAX_NUM_PORTS; port++) {
            port_schedule_state->out_port_map.log_port_speed[port] = 
                soc_tomahawk2_port_speed_higig2eth(port_schedule_state->out_port_map.log_port_speed[port]);
        }
        /* Speed remap for resource */
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (-1 != port_schedule_state->resource[i].physical_port) { /* that is, port up */
                port_schedule_state->resource[i].speed = 
                    soc_tomahawk2_port_speed_higig2eth(port_schedule_state->resource[i].speed);
            }
        }
    }
}


/*! @fn int soc_tomahawk2_tdm_get_tdm_speed(int unit,
 *   soc_port_schedule_state_t *port_schedule_state,
 *  int log_port,
 *  int prev_or_new,
 *  int *speed,
 *  int *num_lanes)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct
 *         variable.
 *  @param log_port: logical port
 *  @param prev_or_new: if = 1 use in_port_map
 *  int *speed: port speed returned
 *  int *num_lanes: num lanes returned
 *  @brief helper func to get the scheduler speed and num lanes
 */
int
soc_tomahawk2_tdm_get_tdm_speed(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    int log_port,
    int prev_or_new,
    int *speed,
    int *num_lanes)
{
    soc_port_map_type_t *port_map;

    if (prev_or_new == 1) {
        port_map = &port_schedule_state->in_port_map;
    } else {
        port_map = &port_schedule_state->out_port_map;
    }

    *speed = port_map->log_port_speed[log_port];
    *num_lanes = port_map->port_num_lanes[log_port];

    /* If 25G should act as 50G in the scheduler change speed and num_lanes */
    if (SOC_PBMP_MEMBER(port_map->physical_pbm,log_port) &&
        (*speed == 25000) ) {
        *speed = 50000;
        *num_lanes = 2;
    }

    return SOC_E_NONE;
}

/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK2_SUPPORT */
