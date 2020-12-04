/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3_set_tdm.c
 * Purpose:
 * Requires:
 */


#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>

#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <soc/init/tomahawk3_tdm.h>


/*** START SDK API COMMON CODE ***/

#define TH3_TDM_MAX_FREQ 1325

/* MIN_SPACING_* table */
static const struct soc_tomahawk3_tdm_min_spacing_s {
    int def;
    int min;
} soc_tomahawk3_tdm_min_spacing_settings[4][4] = {
    { /* one cell */
        {12 , 6},    /* 50GE  */
        { 6,  6},    /* 100GE */
        { 3,  3},    /* 200GE */
        { 3,  3}     /* 400GE */
    },
    { /* two cell */
        {20 , 6},    /* 50GE  */
        { 8,  6},    /* 100GE */
        { 3,  3},    /* 200GE */
        { 3,  3}     /* 400GE */
    },
    { /* three cell */
        {32 , 6},    /* 50GE  */
        {14,  6},    /* 100GE */
        { 5,  3},    /* 200GE */
        { 3,  3}     /* 400GE */
    },
    { /* four cell */
        {44 , 6},    /* 50GE  */
        {20,  6},    /* 100GE */
        { 8,  3},    /* 200GE */
        { 3,  3}     /* 400GE */
    }
};



/*! @fn int soc_tomahawk3_tdm_set_cal_config(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port CAL_CONFIG
 * Description:
 *      API to initialize/clear per port CAL_CONFIG
 *      Regs/Mems configured: IDB/MMU SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_cal_config(
    int unit, 
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    uint64 this_port_cal_slot_bitmap;
    int this_port_number;
    int this_slot_valid;
    int this_port_speed;
    int log_port;
    soc_port_map_type_t *port_map;
    uint64 fldval64, rval64;
    int is_special_slot;
    int inst, slot;
    soc_reg_t reg;
    int port_down_slot_pos_table[_TH3_TDM_LENGTH];

    static const soc_reg_t idb_cal_config_regs[] = {
        IS_CAL_CONFIG_PIPE0r, IS_CAL_CONFIG_PIPE1r,
        IS_CAL_CONFIG_PIPE2r, IS_CAL_CONFIG_PIPE3r,
        IS_CAL_CONFIG_PIPE4r, IS_CAL_CONFIG_PIPE5r,
        IS_CAL_CONFIG_PIPE6r, IS_CAL_CONFIG_PIPE7r
    };

    static const soc_reg_t idb_pksch_cal_config_regs[] = {
        IS_PKSCH_CAL_CONFIG_PIPE0r, IS_PKSCH_CAL_CONFIG_PIPE1r,
        IS_PKSCH_CAL_CONFIG_PIPE2r, IS_PKSCH_CAL_CONFIG_PIPE3r,
        IS_PKSCH_CAL_CONFIG_PIPE4r, IS_PKSCH_CAL_CONFIG_PIPE5r,
        IS_PKSCH_CAL_CONFIG_PIPE6r, IS_PKSCH_CAL_CONFIG_PIPE7r
    };

    /* Set reg's variables */

    if (is_port_down == 1) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    is_special_slot = (phy_port == (SOC_MAX_NUM_PORTS + 1)) ? 1 : 0;

    this_port_number = (is_port_down == 1) ? 0 :
        ((is_special_slot == 1) ? _TH3_TDM_SCHED_SPECIAL_SLOT :
        port_map->port_p2m_mapping[phy_port] & 0x1f);

    this_slot_valid  = (is_port_down == 1) ? 0 : 1;

    if (is_port_down == 1) {
        this_port_speed = 0;
    } else if (is_special_slot == 1) {
        this_port_speed = 3; /* speed 50G for Aux ports */
    } else {
        log_port = port_map->port_p2l_mapping[phy_port];
        switch (port_map->log_port_speed[log_port]) {
        case SPEED_400G:
            this_port_speed = 0;
            break;
        case SPEED_200G:
            this_port_speed = 1;
            break;
        case SPEED_100G:
            this_port_speed = 2;
            break;
        case SPEED_50G:
            this_port_speed = 3;
            break;
        default: /* speed < 50G */
            this_port_speed = 3;
            break;
        }
    }

    COMPILER_64_ZERO(this_port_cal_slot_bitmap);
    if (is_port_down == 0) {
        for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
            if (port_schedule_state->tdm_ingress_schedule_pipe[pipe_num].tdm_schedule_slice[0].linerate_schedule[slot] ==
                phy_port) {
                COMPILER_64_BITSET(
                    this_port_cal_slot_bitmap, slot);
            }
        }
    }

    /* Select the right reg based on sched instance */
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_cal_config_regs[pipe_num];
        break;
    case TDM_IDB_PKT_SCHED:
        reg = idb_pksch_cal_config_regs[pipe_num];
        break;
    case TDM_MMU_MAIN_SCHED:
        reg = MMU_PTSCH_CAL_CONFIGr;
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_CAL_CONFIGr;
        break;
    case TDM_MMU_EB_PKT_SCHED:
        reg = MMU_EBPTS_PKSCH_CAL_CONFIGr;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_cal_config()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    /* Set reg's field variables and configure */
    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(fldval64, 0, this_port_number);
    soc_reg64_field_set(unit, reg, &rval64, THIS_PORT_NUMBERf, fldval64);
    soc_reg64_field_set(unit, reg, &rval64, THIS_PORT_CAL_SLOT_BITMAPf, this_port_cal_slot_bitmap);
    COMPILER_64_SET(fldval64, 0, this_slot_valid);
    soc_reg64_field_set(unit, reg, &rval64, THIS_SLOT_VALIDf, fldval64);
    COMPILER_64_SET(fldval64, 0, this_port_speed);
    soc_reg64_field_set(unit, reg, &rval64, THIS_PORT_SPEEDf, fldval64);

    sal_memset(port_down_slot_pos_table, 0, sizeof(port_down_slot_pos_table));
    if (is_port_down == 1) {
        if (is_special_slot == 0) { /* FP ports */
            log_port =  port_map->port_p2l_mapping[phy_port];
            /* get the slots in the scheduler table that need to be invalidated */
            soc_tomahawk3_set_tdm_slot_pos_tbl(phy_port,
                port_map->log_port_speed[log_port], port_down_slot_pos_table);
        } else { /* special slot */
            port_down_slot_pos_table[_TH3_TDM_LENGTH - 1] = 1;
        }
    }


    /* Configure with proper set_reg API */
    for (slot = 0; slot < _TH3_TDM_LENGTH; slot++) {
        if (((is_port_down == 0) && /* add port to scheduler */
            (port_schedule_state->tdm_ingress_schedule_pipe[pipe_num].tdm_schedule_slice[0].linerate_schedule[slot] ==
                 phy_port)) ||
            ((is_port_down == 1) &&  /* remove port from scheduler */
            (port_down_slot_pos_table[slot] == 1)) ) {
            switch (sched_inst_name) {
            case TDM_IDB_PORT_SCHED:
            case TDM_IDB_PKT_SCHED:
                SOC_IF_ERROR_RETURN(soc_reg64_set(
                   unit, reg, REG_PORT_ANY, slot, rval64));
                break;
            case TDM_MMU_MAIN_SCHED:
            case TDM_MMU_EB_PORT_SCHED:
            case TDM_MMU_EB_PKT_SCHED:
                inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
                SOC_IF_ERROR_RETURN(soc_reg64_set(
                   unit, reg, inst, slot, rval64));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "soc_tomahawk3_tdm_set_cal_config()"
                    "Unsupported sched_inst_name 2nd\n")));
                return SOC_E_PARAM;
                break;
            }
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_tdm_set_max_spacing(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port CAL_CONFIG
 * Description:
 *      API to initialize/clear per port CAL_CONFIG
 *      Regs/Mems configured: IDB/MMU SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_max_spacing(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int max_spacing_all_speeds;
    int max_spacing_special_slot;
    uint32 rval;
    int skip_instance;
    int inst;
    soc_reg_t reg = INVALIDr;

    static const soc_reg_t idb_max_spacing_regs[] = {
        IS_MAX_SPACING_PIPE0r, IS_MAX_SPACING_PIPE1r,
        IS_MAX_SPACING_PIPE2r, IS_MAX_SPACING_PIPE3r,
        IS_MAX_SPACING_PIPE4r, IS_MAX_SPACING_PIPE5r,
        IS_MAX_SPACING_PIPE6r, IS_MAX_SPACING_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_max_spacing_regs[pipe_num];
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_MAX_SPACINGr;
        break;
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_max_spacing()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    /*
     * max_spacing = (52 * core_freq / 1325)
     * where 52 is the max spacing for 50G at 1325 MHz
     */

    max_spacing_all_speeds =
        soc_tomahawk3_div_round(52 * port_schedule_state->frequency, TH3_TDM_MAX_FREQ);
    max_spacing_special_slot = 33;

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, MAX_SPACING_ALL_SPEEDSf, max_spacing_all_speeds);
    soc_reg_field_set(unit, reg, &rval, MAX_SPACING_SPECIAL_SLOTf, max_spacing_special_slot);

    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    case TDM_MMU_EB_PORT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_max_spacing()"
            "Unsupported sched_inst_name 2nd \n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_feature_ctrl(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port FEATURE_CTRL
 * Description:
 *      API to initialize/clear per port FEATURE_CTRL
 *      Regs/Mems configured: IDB/MMU PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_feature_ctrl(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int disable_max_spacing;
    int enable_bubble_mop;
    int skip_instance;
    int inst;
    uint32 rval;
    soc_reg_t reg;

    static const soc_reg_t idb_feature_ctrl_regs[] = {
        IS_FEATURE_CTRL_PIPE0r, IS_FEATURE_CTRL_PIPE1r,
        IS_FEATURE_CTRL_PIPE2r, IS_FEATURE_CTRL_PIPE3r,
        IS_FEATURE_CTRL_PIPE4r, IS_FEATURE_CTRL_PIPE5r,
        IS_FEATURE_CTRL_PIPE6r, IS_FEATURE_CTRL_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;
    disable_max_spacing = 0;
    enable_bubble_mop = 1;
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_feature_ctrl_regs[pipe_num];
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_FEATURE_CTRLr;
        enable_bubble_mop = 0; /* It's don't care; set to 0 */
        break;
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_feature_ctrl()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, DISABLE_MAX_SPACINGf,
                      disable_max_spacing);
    soc_reg_field_set(unit, reg, &rval, ENABLE_BUBBLE_MOPf,
                      enable_bubble_mop);

    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    case TDM_MMU_EB_PORT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_feature_ctrl()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_tdm_set_cbmg_value(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port CBMG_VALUE
 * Description:
 *      API to initialize/clear per port CBMG_VALUE
 *      Regs/Mems configured: IDB/MMU PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_cbmg_value(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int terminal_counter_value;
    int skip_instance;
    int inst;
    uint32 rval = 0;
    soc_reg_t reg;
    int null_bw;

    static const soc_reg_t idb_cbmg_value_regs[] = {
        IS_CBMG_VALUE_PIPE0r, IS_CBMG_VALUE_PIPE1r,
        IS_CBMG_VALUE_PIPE2r, IS_CBMG_VALUE_PIPE3r,
        IS_CBMG_VALUE_PIPE4r, IS_CBMG_VALUE_PIPE5r,
        IS_CBMG_VALUE_PIPE6r, IS_CBMG_VALUE_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;

    null_bw = 12;
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_cbmg_value_regs[pipe_num];
        null_bw = 12; /* 12 MHz */
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_CBMG_VALUEr;
        null_bw = 5; /* 5 MHz */
        break;
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_cbmg_value()"
                  "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    /* TH3 bandwidth requirements are roughly as follows:
     * For minimum guarantee of 8.1G(IP) & 3.4(EP) computed at 64B packets,
     * Null/IDLE slots should be picked with an average TDM of once every
     * configurable terminal_counter_value cycles.
     * CBMG_value = [(64+20)*8] * CoreFreq(MHz) / BW(Mbps)
     * IP: IFP Refresh 8.2MHz + 3.8MHz SBUS = 12 MHz total
     * EP: 3.8Mhz SBUS + 1.2MHz TCAM parity scan = 5MHz total
     * Simplified formula:
     * CBMG_value = FLOOR[ CoreFreq(MHz) / NullRate(MHz) ]
     */
    terminal_counter_value = port_schedule_state->frequency / null_bw;

    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_get(
           unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval,
                      TERMINAL_COUNTER_VALUEf,
                      terminal_counter_value);
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    case TDM_MMU_EB_PORT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_get(
           unit, reg, inst, 0, &rval));
        soc_reg_field_set(unit, reg, &rval,
                      TERMINAL_COUNTER_VALUEf,
                      terminal_counter_value);
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_cbmg_value()"
                  "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_urg_cell_spacing(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port URG_CELL_SPACING
 * Description:
 *      API to initialize/clear per port / per speed URG_CELL_SPACING
 *      Regs/Mems configured: IDB/MMU PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_urg_cell_spacing(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int urg_pick_50g_spacing;
    int urg_pick_100g_spacing;
    int urg_pick_200g_spacing;
    int urg_pick_400g_spacing;
    int skip_instance;
    int inst;
    uint32 rval;
    soc_reg_t reg;
    int core_clock_freq;

    static const soc_reg_t idb_urg_cell_spacing_regs[] = {
        IS_URG_CELL_SPACING_PIPE0r, IS_URG_CELL_SPACING_PIPE1r,
        IS_URG_CELL_SPACING_PIPE2r, IS_URG_CELL_SPACING_PIPE3r,
        IS_URG_CELL_SPACING_PIPE4r, IS_URG_CELL_SPACING_PIPE5r,
        IS_URG_CELL_SPACING_PIPE6r, IS_URG_CELL_SPACING_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_urg_cell_spacing_regs[pipe_num];
        break;
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_URG_CELL_SPACINGr;
        break;
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_urg_cell_spacing()"
                  "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    /*
     * FLOOR[(CoreFreq * 84 * 8) / 400000] * (400000 / SPEED);
     */
    core_clock_freq = port_schedule_state->frequency;
    if (core_clock_freq >= TH3_TDM_MAX_FREQ) {
        urg_pick_400g_spacing = (core_clock_freq * 84 * 8) / 400000;
        urg_pick_200g_spacing = 2 * urg_pick_400g_spacing;
        urg_pick_100g_spacing = 4 * urg_pick_400g_spacing;
        urg_pick_50g_spacing  = 8 * urg_pick_400g_spacing;
    } else {
        urg_pick_200g_spacing = (core_clock_freq * 84 * 8) / 200000;
        urg_pick_100g_spacing = 2 * urg_pick_200g_spacing;
        urg_pick_50g_spacing  = 4 * urg_pick_200g_spacing;
        urg_pick_400g_spacing = urg_pick_200g_spacing; /* N/A */
    }

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, URG_PICK_50G_SPACINGf,
                      urg_pick_50g_spacing);
    soc_reg_field_set(unit, reg, &rval, URG_PICK_100G_SPACINGf,
                      urg_pick_100g_spacing);
    soc_reg_field_set(unit, reg, &rval, URG_PICK_200G_SPACINGf,
                      urg_pick_200g_spacing);
    soc_reg_field_set(unit, reg, &rval, URG_PICK_400G_SPACINGf,
                      urg_pick_400g_spacing);

    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    case TDM_MMU_EB_PORT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_urg_cell_spacing()"
                  "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_tdm_set_pksch_pkt_credits_per_pipe(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port CBMG_VALUE
 * Description:
 *      API to initialize/clear per port CBMG_VALUE
 *      Regs/Mems configured: IDB/MMU PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_pksch_pkt_credits_per_pipe(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int pkt_credit_count;
    int skip_instance;
    int inst;
    uint32 rval;
    soc_reg_t reg;

    static const soc_reg_t idb_pkt_credits_regs[] = {
        IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE0r, IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE1r,
        IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE2r, IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE3r,
        IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE4r, IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE5r,
        IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE6r, IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;

    /* TH3 recommended
     */
    pkt_credit_count = 21;
    switch (sched_inst_name) {
    case TDM_IDB_PKT_SCHED:
        reg = idb_pkt_credits_regs[pipe_num];
        break;
    case TDM_MMU_EB_PKT_SCHED:
        reg = MMU_EBPTS_PKSCH_PKT_CREDITS_PER_PIPEr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PORT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_pksch_pkt_credits_per_pipe()"
                  "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, PKT_CREDIT_COUNTf,
                      pkt_credit_count);

    switch (sched_inst_name) {
    case TDM_IDB_PKT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    case TDM_MMU_EB_PKT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                  "soc_tomahawk3_tdm_set_pksch_pkt_credits_per_pipe()"
                  "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}

/* Max EB credit configurations for EB prt shced to MMU Main prt sched*/
/* Indexed by speed: 50G:0, 100G:1; 200G:2; 400G:3*/
static const int soc_th3_tdm_mmu_eb_credit_config_vals[] = {
      47, 55, 71, 104};

/*! @fn int soc_tomahawk3_tdm_set_eb_credit_config(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port EB_CREDIT_CONFIG
 * Description:
 *      API to initialize/clear per port EB_CREDIT_CONFIG
 *      Regs/Mems configured: MMU MAIN SCHED EB_CREDIT_CONFIG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_eb_credit_config(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    int mmu_port;
    int num_eb_credits;
    soc_port_map_type_t *port_map;
    int log_port;
    int skip_instance;
    int inst;
    uint32 rval;
    uint64 rval64;
    soc_reg_t reg;

    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        reg = MMU_PTSCH_EB_CREDIT_CONFIGr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_EB_PORT_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_eb_credit_config()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    /* Only MMU main prt scheduler gets this reg configured */
    if (skip_instance == 1) {return SOC_E_NONE;}

    /* Set reg's variables */
    if (is_port_down == 1) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    mmu_port = port_map->port_p2m_mapping[phy_port];

    if (is_port_down == 1) {
        num_eb_credits = 0;
    } else {
        log_port = port_map->port_p2l_mapping[phy_port];
        switch (port_map->log_port_speed[log_port]) {
        case SPEED_400G:
            num_eb_credits = soc_th3_tdm_mmu_eb_credit_config_vals[3];
            break;
        case SPEED_200G:
            num_eb_credits = soc_th3_tdm_mmu_eb_credit_config_vals[2];
            break;
        case SPEED_100G:
            num_eb_credits = soc_th3_tdm_mmu_eb_credit_config_vals[1];
            break;
        default: /* any speed <= 50G; */
            num_eb_credits = soc_th3_tdm_mmu_eb_credit_config_vals[0];
            break;
        }
    }

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, NUM_EB_CREDITSf,
                      num_eb_credits);

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        inst = mmu_port;
        COMPILER_64_SET(rval64, 0, rval);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval64));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_eb_credit_config()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_ebshp_port_cfg(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port MMU_EBPTS_EBSHP_PORT_CFG
 * Description:
 *      API to initialize/clear per port MMU_EBPTS_EBSHP_PORT_CFG
 *      Regs/Mems configured: MMU MAIN SCHED MMU_EBPTS_EBSHP_PORT_CFG
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_ebshp_port_cfg(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    int mmu_port;
    int port_speed = 0;
    soc_port_map_type_t *port_map;
    int log_port;
    int skip_instance;
    int inst;
    uint32 rval;
    uint64 rval64;
    soc_reg_t reg;

    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_EBSHP_PORT_CFGr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_ebshp_port_cfg()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    /* Only MMU main prt scheduler gets this reg configured */
    if (skip_instance == 1) {return SOC_E_NONE;}

    /* Set reg's variables */
    if (is_port_down == 1) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    mmu_port = port_map->port_p2m_mapping[phy_port];

    if (is_port_down == 1) {
        port_speed = 0;
    } else {
        log_port = port_map->port_p2l_mapping[phy_port];
        switch (port_map->log_port_speed[log_port]) {
        case SPEED_400G: port_speed = 7; break;
        case SPEED_200G: port_speed = 6; break;
        case SPEED_100G: port_speed = 5; break;
        case SPEED_50G : port_speed = 4; break;
        case SPEED_40G : port_speed = 3; break;
        case SPEED_25G : port_speed = 2; break;
        case SPEED_10G : port_speed = 1; break;
        default:
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "soc_tomahawk3_tdm_set_ebshp_port_cfg()"
                "Unsupported speed\n")));
            return SOC_E_PARAM;
            break;
        }
    }

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, PORT_SPEEDf,
                      port_speed);

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        inst = mmu_port;
        COMPILER_64_SET(rval64, 0, rval);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval64));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_ebshp_port_cfg()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_clear_edb_credit_counter_per_port(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param is_port_down. boolean to indicate if port down
 *  @brief API to clear per port EDB_CREDIT_COUNTER
 * Description:
 *      API to initialize/clear per port EDB_CREDIT_COUNTER
 *      Regs/Mems configured: MMU EB_PORT_SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_clear_edb_credit_counter_per_port(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    int mmu_port;
    int num_edb_credits;
    soc_port_map_type_t *port_map;
    int skip_instance;
    int inst;
    uint32 rval;
    uint64 rval64;
    soc_reg_t reg;

    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_EDB_CREDIT_COUNTERr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_clear_edb_credit_counter_per_port()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    /* Only EB prt scheduler gets this reg configured */
    if (skip_instance == 1) {return SOC_E_NONE;}

    /* Set reg's variables */
    if (is_port_down == 1) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    mmu_port = port_map->port_p2m_mapping[phy_port];
    num_edb_credits = 0;

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, NUM_EDB_CREDITSf,
                      num_edb_credits);

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        inst = mmu_port;
        COMPILER_64_SET(rval64, 0, rval);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval64));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_clear_edb_credit_counter_per_port()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_clear_edb_credit_counter(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to clear per port EDB_CREDIT_COUNTER
 * Description:
 *      API to initialize/clear per port EDB_CREDIT_COUNTER
 *      Regs/Mems configured: MMU EB_PORT_SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_clear_edb_credit_counter(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, logical_port, physical_port;
    int pipe_num, is_port_down;

    is_port_down = 1; /* that is, disable port */
    for (i = 0; i < port_schedule_state->nport; i++) {
        if (port_schedule_state->resource[i].physical_port == -1) { /* port DOWN */
            logical_port = port_schedule_state->resource[i].logical_port;
            pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
            physical_port =
                port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
            SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_clear_edb_credit_counter_per_port(unit, port_schedule_state,
                                TDM_MMU_EB_PORT_SCHED, pipe_num, physical_port, is_port_down));
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_eb_satisfied_threshold(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port EB_SATISFIED_THRESHOLD
 * Description:
 *      API to initialize/clear per port EB_SATISFIED_THRESHOLD
 *      Regs/Mems configured: MMU MAIN PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_eb_satisfied_threshold(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int num_satisfaction_credits_200g;
    int num_satisfaction_credits_100g;
    int num_satisfaction_credits_50g;
    int num_satisfaction_credits_400g;
    int skip_instance;
    int inst;
    uint32 rval;
    soc_reg_t reg;


    /* Select the right reg based on sched instance */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        reg = MMU_PTSCH_EB_SATISFIED_THRESHOLDr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_EB_PORT_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_eb_satisfied_threshold()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    /* 50% from EB_CREDIT_CONFIG */
    num_satisfaction_credits_50g  = soc_th3_tdm_mmu_eb_credit_config_vals[0] / 2;
    num_satisfaction_credits_100g = soc_th3_tdm_mmu_eb_credit_config_vals[1] / 2;
    num_satisfaction_credits_200g = soc_th3_tdm_mmu_eb_credit_config_vals[2] / 2;
    num_satisfaction_credits_400g = soc_th3_tdm_mmu_eb_credit_config_vals[3] / 2;

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, NUM_SATISFACTION_CREDITS_200Gf,
                      num_satisfaction_credits_200g);
    soc_reg_field_set(unit, reg, &rval, NUM_SATISFACTION_CREDITS_100Gf,
                      num_satisfaction_credits_100g);
    soc_reg_field_set(unit, reg, &rval, NUM_SATISFACTION_CREDITS_50Gf,
                      num_satisfaction_credits_50g);
    soc_reg_field_set(unit, reg, &rval, NUM_SATISFACTION_CREDITS_400Gf,
                      num_satisfaction_credits_400g);

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, inst, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_eb_satisfied_threshold()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_tdm_set_min_spacing(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize MIN_SPACING_* regs
 * Description:
 *      API to initialize MIN_SPACING_* regs
 *      Regs/Mems configured: MMU MAIN PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_min_spacing(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int one_cell_400g;
    int one_cell_200g;
    int one_cell_100g;
    int one_cell_50g;
    int two_cell_400g;
    int two_cell_200g;
    int two_cell_100g;
    int two_cell_50g;
    int three_cell_400g;
    int three_cell_200g;
    int three_cell_100g;
    int three_cell_50g;
    int four_cell_400g;
    int four_cell_200g;
    int four_cell_100g;
    int four_cell_50g;
    int skip_instance;
    int inst;
    uint32 one_cell_rval, two_cell_rval,
           three_cell_rval, four_cell_rval;
    soc_reg_t one_cell_reg, two_cell_reg,
              three_cell_reg, four_cell_reg;
    soc_reg_t pick_to_same_port_pick_reg;
    soc_reg_t pick_to_same_port_cpu_reg;
    uint64 pick_to_same_port_pick_val;
    uint32 pick_to_same_port_cpu_val;
    uint64 pick_min_four_cell_50g, pick_min_four_cell_100g;
    uint64 pick_min_one_to_three_cell_50g, pick_min_one_to_three_cell_100g;
    uint32 pick_min_four_cell_cpu, pick_min_one_to_three_cell_cpu;
    static const soc_reg_t cpu_min_spacing_regs[] = {
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB0r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB1r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB2r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB3r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB4r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB5r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB6r,
        MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACING_EB7r
    };



    /* Select the right reg based on sched instance */
    skip_instance = 0;

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        one_cell_reg = MMU_PTSCH_MIN_SPACING_ONE_CELLr;
        two_cell_reg = MMU_PTSCH_MIN_SPACING_TWO_CELLr;
        three_cell_reg = MMU_PTSCH_MIN_SPACING_THREE_CELLr;
        four_cell_reg = MMU_PTSCH_MIN_SPACING_FOUR_CELLr;
        pick_to_same_port_pick_reg = MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_PICK_MIN_SPACINGr;
        pick_to_same_port_cpu_reg = cpu_min_spacing_regs[pipe_num]; /*MMU_PTSCH_PIPE_PICK_TO_SAME_PORT_CPU_MIN_SPACINGr;*/
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_EB_PORT_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_min_spacing()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}


    one_cell_50g  = (soc_tomahawk3_tdm_min_spacing_settings[0][0].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    one_cell_50g = (one_cell_50g < soc_tomahawk3_tdm_min_spacing_settings[0][0].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[0][0].min : one_cell_50g;
    one_cell_100g  = (soc_tomahawk3_tdm_min_spacing_settings[0][1].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    one_cell_100g = (one_cell_100g < soc_tomahawk3_tdm_min_spacing_settings[0][1].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[0][1].min : one_cell_100g;
    one_cell_200g  = (soc_tomahawk3_tdm_min_spacing_settings[0][2].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    one_cell_200g = (one_cell_200g < soc_tomahawk3_tdm_min_spacing_settings[0][2].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[0][2].min : one_cell_200g;
    one_cell_400g  = (soc_tomahawk3_tdm_min_spacing_settings[0][3].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    one_cell_400g = (one_cell_400g < soc_tomahawk3_tdm_min_spacing_settings[0][3].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[0][3].min : one_cell_400g;

    two_cell_50g  = (soc_tomahawk3_tdm_min_spacing_settings[1][0].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    two_cell_50g = (two_cell_50g < soc_tomahawk3_tdm_min_spacing_settings[1][0].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[1][0].min : two_cell_50g;
    two_cell_100g  = (soc_tomahawk3_tdm_min_spacing_settings[1][1].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    two_cell_100g = (two_cell_100g < soc_tomahawk3_tdm_min_spacing_settings[1][1].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[1][1].min : two_cell_100g;
    two_cell_200g  = (soc_tomahawk3_tdm_min_spacing_settings[1][2].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    two_cell_200g = (two_cell_200g < soc_tomahawk3_tdm_min_spacing_settings[1][2].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[1][2].min : two_cell_200g;
    two_cell_400g  = (soc_tomahawk3_tdm_min_spacing_settings[1][3].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    two_cell_400g = (two_cell_400g < soc_tomahawk3_tdm_min_spacing_settings[1][3].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[1][3].min : two_cell_400g;

    three_cell_50g  = (soc_tomahawk3_tdm_min_spacing_settings[2][0].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    three_cell_50g = (three_cell_50g < soc_tomahawk3_tdm_min_spacing_settings[2][0].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[2][0].min : three_cell_50g;
    three_cell_100g  = (soc_tomahawk3_tdm_min_spacing_settings[2][1].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    three_cell_100g = (three_cell_100g < soc_tomahawk3_tdm_min_spacing_settings[2][1].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[2][1].min : three_cell_100g;
    three_cell_200g  = (soc_tomahawk3_tdm_min_spacing_settings[2][2].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    three_cell_200g = (three_cell_200g < soc_tomahawk3_tdm_min_spacing_settings[2][2].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[2][2].min : three_cell_200g;
    three_cell_400g  = (soc_tomahawk3_tdm_min_spacing_settings[2][3].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    three_cell_400g = (three_cell_400g < soc_tomahawk3_tdm_min_spacing_settings[2][3].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[2][3].min : three_cell_400g;

    four_cell_50g  = (soc_tomahawk3_tdm_min_spacing_settings[3][0].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    four_cell_50g = (four_cell_50g < soc_tomahawk3_tdm_min_spacing_settings[3][0].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[3][0].min : four_cell_50g;
    four_cell_100g  = (soc_tomahawk3_tdm_min_spacing_settings[3][1].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    four_cell_100g = (four_cell_100g < soc_tomahawk3_tdm_min_spacing_settings[3][1].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[3][1].min : four_cell_100g;
    four_cell_200g  = (soc_tomahawk3_tdm_min_spacing_settings[3][2].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    four_cell_200g = (four_cell_200g < soc_tomahawk3_tdm_min_spacing_settings[3][2].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[3][2].min : four_cell_200g;
    four_cell_400g  = (soc_tomahawk3_tdm_min_spacing_settings[3][3].def
                     * port_schedule_state->frequency) / TH3_TDM_MAX_FREQ;
    four_cell_400g = (four_cell_400g < soc_tomahawk3_tdm_min_spacing_settings[3][3].min)
                    ? soc_tomahawk3_tdm_min_spacing_settings[3][3].min : four_cell_400g;


    one_cell_rval = 0;
    soc_reg_field_set(unit, one_cell_reg, &one_cell_rval, ONE_CELL_50G_MIN_SPACINGf,
                      one_cell_50g);
    soc_reg_field_set(unit, one_cell_reg, &one_cell_rval, ONE_CELL_100G_MIN_SPACINGf,
                      one_cell_100g);
    soc_reg_field_set(unit, one_cell_reg, &one_cell_rval, ONE_CELL_200G_MIN_SPACINGf,
                      one_cell_200g);
    soc_reg_field_set(unit, one_cell_reg, &one_cell_rval, ONE_CELL_400G_MIN_SPACINGf,
                      one_cell_400g);

    two_cell_rval = 0;
    soc_reg_field_set(unit, two_cell_reg, &two_cell_rval, TWO_CELL_50G_MIN_SPACINGf,
                      two_cell_50g);
    soc_reg_field_set(unit, two_cell_reg, &two_cell_rval, TWO_CELL_100G_MIN_SPACINGf,
                      two_cell_100g);
    soc_reg_field_set(unit, two_cell_reg, &two_cell_rval, TWO_CELL_200G_MIN_SPACINGf,
                      two_cell_200g);
    soc_reg_field_set(unit, two_cell_reg, &two_cell_rval, TWO_CELL_400G_MIN_SPACINGf,
                      two_cell_400g);

    three_cell_rval = 0;
    soc_reg_field_set(unit, three_cell_reg, &three_cell_rval, THREE_CELL_50G_MIN_SPACINGf,
                      three_cell_50g);
    soc_reg_field_set(unit, three_cell_reg, &three_cell_rval, THREE_CELL_100G_MIN_SPACINGf,
                      three_cell_100g);
    soc_reg_field_set(unit, three_cell_reg, &three_cell_rval, THREE_CELL_200G_MIN_SPACINGf,
                      three_cell_200g);
    soc_reg_field_set(unit, three_cell_reg, &three_cell_rval, THREE_CELL_400G_MIN_SPACINGf,
                      three_cell_400g);

    four_cell_rval = 0;
    soc_reg_field_set(unit, four_cell_reg, &four_cell_rval, FOUR_CELL_50G_MIN_SPACINGf,
                      four_cell_50g);
    soc_reg_field_set(unit, four_cell_reg, &four_cell_rval, FOUR_CELL_100G_MIN_SPACINGf,
                      four_cell_100g);
    soc_reg_field_set(unit, four_cell_reg, &four_cell_rval, FOUR_CELL_200G_MIN_SPACINGf,
                      four_cell_200g);
    soc_reg_field_set(unit, four_cell_reg, &four_cell_rval, FOUR_CELL_400G_MIN_SPACINGf,
                      four_cell_400g);


    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, one_cell_reg, inst, 0, one_cell_rval));
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, two_cell_reg, inst, 0, two_cell_rval));
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, three_cell_reg, inst, 0, three_cell_rval));
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, four_cell_reg, inst, 0, four_cell_rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_min_spacing()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    switch (sched_inst_name) {
    case TDM_MMU_MAIN_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;

        SOC_IF_ERROR_RETURN(soc_reg64_get(
           unit, pick_to_same_port_pick_reg, inst, 0,
           &pick_to_same_port_pick_val));
        COMPILER_64_SET(pick_min_four_cell_50g, 0, 2);
        COMPILER_64_SET(pick_min_four_cell_100g, 0, 2);
        COMPILER_64_SET(pick_min_one_to_three_cell_50g, 0, 2);
        COMPILER_64_SET(pick_min_one_to_three_cell_100g, 0, 2);
        soc_reg64_field_set(unit, pick_to_same_port_pick_reg,
            &pick_to_same_port_pick_val,
            FOUR_CELL_50G_SPACINGf, pick_min_four_cell_50g);
        soc_reg64_field_set(unit, pick_to_same_port_pick_reg,
            &pick_to_same_port_pick_val,
            FOUR_CELL_100G_SPACINGf, pick_min_four_cell_100g);
        soc_reg64_field_set(unit, pick_to_same_port_pick_reg,
            &pick_to_same_port_pick_val,
            ONE_TO_THREE_CELL_50G_SPACINGf, pick_min_one_to_three_cell_50g);
        soc_reg64_field_set(unit, pick_to_same_port_pick_reg,
            &pick_to_same_port_pick_val,
            ONE_TO_THREE_CELL_100G_SPACINGf, pick_min_one_to_three_cell_100g);
        SOC_IF_ERROR_RETURN(soc_reg64_set(
           unit, pick_to_same_port_pick_reg, inst, 0,
           pick_to_same_port_pick_val));

        if ((pipe_num == 1) ||
            (pipe_num == 5)) { /* that is, MGM ports */
            SOC_IF_ERROR_RETURN(soc_reg32_get(
               unit, pick_to_same_port_cpu_reg, REG_PORT_ANY, 0,
               &pick_to_same_port_cpu_val));
            pick_min_four_cell_cpu = 2;
            pick_min_one_to_three_cell_cpu = 2;
            soc_reg_field_set(unit, pick_to_same_port_cpu_reg,
                &pick_to_same_port_cpu_val,
                FOUR_CELL_CPU_SPACINGf, pick_min_four_cell_cpu);
            soc_reg_field_set(unit, pick_to_same_port_cpu_reg,
                &pick_to_same_port_cpu_val,
                ONE_TO_THREE_CELL_CPU_SPACINGf, pick_min_one_to_three_cell_cpu);
            SOC_IF_ERROR_RETURN(soc_reg32_set(
               unit, pick_to_same_port_cpu_reg, REG_PORT_ANY, 0,
               pick_to_same_port_cpu_val));
        }
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_min_spacing()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_tdm_set_min_port_pick_spacing(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize MIN_PORT_PICK_SPACING_WITHIN_PKT
 * Description:
 *      API to initialize MIN_PORT_PICK_SPACING_WITHIN_PKT
 *      Regs/Mems configured: IDB/MMU SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_min_port_pick_spacing(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int mid_pkt_50g_min_spacing;
    int mid_pkt_100g_min_spacing;
    int mid_pkt_200g_min_spacing;
    int mid_pkt_400g_min_spacing;
    uint32 rval;
    int skip_instance;
    soc_reg_t reg;

    static const soc_reg_t idb_min_port_pick_regs[] = {
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE0r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE1r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE2r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE3r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE4r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE5r,
        IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE6r, IS_MIN_PORT_PICK_SPACING_WITHIN_PKT_PIPE7r
    };

    /* Select the right reg based on sched instance */
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        reg = idb_min_port_pick_regs[pipe_num];
        break;
    case TDM_MMU_EB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_min_port_pick_spacing()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    if(port_schedule_state->frequency > 1000) {
        mid_pkt_50g_min_spacing = 12;
        mid_pkt_100g_min_spacing = 12;
        mid_pkt_200g_min_spacing = 8;
        mid_pkt_400g_min_spacing = 4;
    } else {
        mid_pkt_50g_min_spacing = 11;
        mid_pkt_100g_min_spacing = 11;
        mid_pkt_200g_min_spacing = 5;
        mid_pkt_400g_min_spacing = 4;
    }

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, MID_PKT_50G_MIN_SPACINGf, mid_pkt_50g_min_spacing);
    soc_reg_field_set(unit, reg, &rval, MID_PKT_100G_MIN_SPACINGf, mid_pkt_100g_min_spacing);
    soc_reg_field_set(unit, reg, &rval, MID_PKT_200G_MIN_SPACINGf, mid_pkt_200g_min_spacing);
    soc_reg_field_set(unit, reg, &rval, MID_PKT_400G_MIN_SPACINGf, mid_pkt_400g_min_spacing);


    switch (sched_inst_name) {
    case TDM_IDB_PORT_SCHED:
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg, REG_PORT_ANY, 0, rval));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_min_port_pick_spacing()"
            "Unsupported sched_inst_name 2nd \n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}




/*! @fn int soc_tomahawk3_tdm_set_mmu_edb_credit_threshold(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe_num)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @brief API to initialize/clear per port EDB_PORT_CREDIT_THRESHOLD_0/1
 * Description:
 *      API to initialize/clear per port EDB_PORT_CREDIT_THRESHOLD_0/1
 *      Regs/Mems configured: MMU EB PRT SCHED
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_mmu_edb_credit_threshold(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num)
{
    int edb_credit_threshold_50g;
    int edb_credit_threshold_100g;
    int edb_credit_threshold_200g;
    int edb_credit_threshold_400g;
    int skip_instance;
    int inst;
    uint32 rval_0, rval_1;
    soc_reg_t reg_0, reg_1;


    /* Select the right reg based on sched instance */
    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg_0 = MMU_EBPTS_EDB_PORT_CREDIT_THRESHOLD_0r;
        reg_1 = MMU_EBPTS_EDB_PORT_CREDIT_THRESHOLD_1r;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_mmu_edb_credit_threshold()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    if (skip_instance == 1) {return SOC_E_NONE;}

    /* 50% from EDB to MMU CREDITs for that speed
     */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
        unit, port_schedule_state, SPEED_50G, &edb_credit_threshold_50g));
    edb_credit_threshold_50g  = edb_credit_threshold_50g / 2;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
        unit, port_schedule_state, SPEED_100G, &edb_credit_threshold_100g));
    edb_credit_threshold_100g  = edb_credit_threshold_100g / 2;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
        unit, port_schedule_state, SPEED_200G, &edb_credit_threshold_200g));
    edb_credit_threshold_200g  = edb_credit_threshold_200g / 2;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_get_ep2mmu_credit_per_speed(
        unit, port_schedule_state, SPEED_400G, &edb_credit_threshold_400g));
    edb_credit_threshold_400g  = edb_credit_threshold_400g / 2;

    rval_0 = 0;
    rval_1 = 0;
    soc_reg_field_set(unit, reg_0, &rval_0, EDB_CREDIT_THRESHOLD_50Gf,
                      edb_credit_threshold_50g);
    soc_reg_field_set(unit, reg_0, &rval_0, EDB_CREDIT_THRESHOLD_100Gf,
                      edb_credit_threshold_100g);
    soc_reg_field_set(unit, reg_1, &rval_1, EDB_CREDIT_THRESHOLD_200Gf,
                      edb_credit_threshold_200g);
    soc_reg_field_set(unit, reg_1, &rval_1, EDB_CREDIT_THRESHOLD_400Gf,
                      edb_credit_threshold_400g);

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        inst = pipe_num | SOC_REG_ADDR_INSTANCE_MASK;
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg_0, inst, 0, rval_0));
        SOC_IF_ERROR_RETURN(soc_reg32_set(
           unit, reg_1, inst, 0, rval_1));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_mmu_edb_credit_threshold()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_tdm_global_to_local_phy_num(
 *    int phy_port,
 *    int *local_phy_num)
 *  @param phy_port phy_port to configure.
 *  @param *local_phy_num local phy number.
 *  @brief API to get local physical number based on global physical number
 * Description:
 *      API to get local physical number based on global physical number
 * Return Value:
 *      SOC_E_*
 */
STATIC int soc_tomahawk3_tdm_global_to_local_phy_num(
    int phy_port,
    int *local_phy_num)
{
    *local_phy_num = 0x3f;

    if ((0 <= phy_port) && (phy_port <= _TH3_GPHY_PORTS_PER_PIPE)) {
        *local_phy_num = phy_port;
    } else if (((_TH3_GPHY_PORTS_PER_PIPE + 1) <= phy_port) &&
               (phy_port <= (_TH3_GPHY_PORTS_PER_PIPE * _TH3_PIPES_PER_DEV))) {
        *local_phy_num = ((phy_port - 1) % _TH3_GPHY_PORTS_PER_PIPE);
    } else if ((_TH3_PHY_PORT_MNG0 <= phy_port) && (phy_port <= _TH3_PHY_PORT_MNG1)) {
        *local_phy_num = 32;
    } else if ((_TH3_PHY_PORT_LPBK0 <= phy_port) && (phy_port <= _TH3_PHY_PORT_LPBK7)) {
        *local_phy_num = 33;
    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping(
 *    int unit,
 *    soc_port_schedule_state_t *port_schedule_state,
 *    tdm_sched_inst_name_e sched_inst_name,
 *    int pipe,
 *    int phy_port,
 *    int is_port_down)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name name enum of instance
 *  @param pipe pipe number.
 *  @param phy_port phy_port to configure.
 *  @param phy_port is_port_down. boolean to indicate if port down
 *  @brief API to initialize/clear per port MMU_PORT_TO_PHY_PORT_MAPPING
 * Description:
 *      API to initialize/clear per port MMU_PORT_TO_PHY_PORT_MAPPING
 *      Regs/Mems configured: MMU EB PRT
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int pipe_num,
    int phy_port,
    int is_port_down)
{
    int mmu_port;
    int phy_port_num;
    soc_port_map_type_t *port_map;
    int skip_instance;
    int inst;
    uint32 rval;
    uint64 rval64;
    soc_reg_t reg;

    skip_instance = 0;
    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        reg = MMU_EBPTS_MMU_PORT_TO_PHY_PORT_MAPPINGr;
        break;
    case TDM_IDB_PORT_SCHED:
    case TDM_IDB_PKT_SCHED:
    case TDM_MMU_MAIN_SCHED:
    case TDM_MMU_EB_PKT_SCHED:
        skip_instance = 1;
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping()"
            "Unsupported sched_inst_name\n")));
        return SOC_E_PARAM;
        break;
    }

    /* Only MMU main prt scheduler gets this reg configured */
    if (skip_instance == 1) {return SOC_E_NONE;}

    /* Set reg's variables */
    if (is_port_down == 1) { /* in_port_map used at port down */
        port_map = &port_schedule_state->in_port_map;
    } else {                 /* out_port_map used at port up */
        port_map = &port_schedule_state->out_port_map;
    }

    mmu_port = port_map->port_p2m_mapping[phy_port];

    if (is_port_down == 1) {
        phy_port_num = 0x3f;
    } else {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_global_to_local_phy_num(phy_port, &phy_port_num));
    }

    rval = 0;
    soc_reg_field_set(unit, reg, &rval, PHY_PORT_NUMf,
                      phy_port_num);

    switch (sched_inst_name) {
    case TDM_MMU_EB_PORT_SCHED:
        inst = mmu_port;
        COMPILER_64_SET(rval64, 0, rval);
        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, inst, 0, rval64));
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping()"
            "Unsupported sched_inst_name 2nd\n")));
        return SOC_E_PARAM;
        break;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_set_out_port_map(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t struct variable
 *  @brief Helper function to calculate port_schedule_state->out_port_map from
 *         port_schedule_state->out_port_map and port_schedule_state->resource[]
 * Description:
 *      Helper function
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int
soc_tomahawk3_tdm_set_out_port_map(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int i, logical_port, physical_port;
    int prev_physical_port, prev_mmu_port;

    /* Copy in_port_map in out_port_map */
    for (i= 0; i< SOC_MAX_NUM_PORTS; i++) {
        port_schedule_state->out_port_map.log_port_speed[i]   =
            port_schedule_state->in_port_map.log_port_speed[i];
        port_schedule_state->out_port_map.port_num_lanes[i]   =
            port_schedule_state->in_port_map.port_num_lanes[i];
    }

    sal_memcpy(port_schedule_state->out_port_map.port_p2l_mapping,
               port_schedule_state->in_port_map.port_p2l_mapping,
               sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(port_schedule_state->out_port_map.port_l2p_mapping,
               port_schedule_state->in_port_map.port_l2p_mapping,
               sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(port_schedule_state->out_port_map.port_p2m_mapping,
               port_schedule_state->in_port_map.port_p2m_mapping,
               sizeof(int) * SOC_MAX_NUM_PORTS);
    sal_memcpy(port_schedule_state->out_port_map.port_m2p_mapping,
               port_schedule_state->in_port_map.port_m2p_mapping,
               sizeof(int) * SOC_MAX_NUM_MMU_PORTS);
    sal_memcpy(port_schedule_state->out_port_map.port_l2i_mapping,
               port_schedule_state->in_port_map.port_l2i_mapping,
               sizeof(int) * SOC_MAX_NUM_PORTS);

    sal_memcpy(&port_schedule_state->out_port_map.physical_pbm,
               &port_schedule_state->in_port_map.physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.hg2_pbm,
               &port_schedule_state->in_port_map.hg2_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.management_pbm,
               &port_schedule_state->in_port_map.management_pbm, sizeof(pbmp_t));
    sal_memcpy(&port_schedule_state->out_port_map.oversub_pbm,
               &port_schedule_state->in_port_map.oversub_pbm, sizeof(pbmp_t));


    if (1 == port_schedule_state->is_flexport) {
        /* Update for ports that are down
         * Remove all the mappings and bitmaps
         */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (-1 == physical_port) { /* that is, port down */
                port_schedule_state->out_port_map.log_port_speed[logical_port] = 0;
                port_schedule_state->out_port_map.port_num_lanes[logical_port] = 0;
                /* Remove previous log_port to phy_port mapping */
                port_schedule_state->out_port_map.port_l2p_mapping[logical_port] = -1;
                /* Remove previous log_port to idb_port mapping */
                port_schedule_state->out_port_map.port_l2i_mapping[logical_port] = -1;
                /* Remove previous phy_port to log_port mapping */
                prev_physical_port =
                    port_schedule_state->in_port_map.port_l2p_mapping[logical_port];
                port_schedule_state->out_port_map.port_p2l_mapping[prev_physical_port] = -1;
                /* Remove previous phy_port to mmu_port mapping */
                port_schedule_state->out_port_map.port_p2m_mapping[prev_physical_port] = -1;
                /* Remove previous mmu_port to phy_port mapping */
                prev_mmu_port =
                    port_schedule_state->in_port_map.port_p2m_mapping[prev_physical_port];
                port_schedule_state->out_port_map.port_m2p_mapping[prev_mmu_port] = -1;
                /* Remove corresponding bitmaps */
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.physical_pbm,
                    prev_physical_port);
                SOC_PBMP_PORT_REMOVE(port_schedule_state->out_port_map.hg2_pbm,
                                     logical_port);
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.oversub_pbm, logical_port);
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->out_port_map.management_pbm,
                    logical_port);    /* That's optional */
            }
        }

        /* Update for ports that are brought up */
        for (i = 0; i < port_schedule_state->nport; i++) {
            logical_port = port_schedule_state->resource[i].logical_port;
            physical_port = port_schedule_state->resource[i].physical_port;
            if (-1 != physical_port) { /* that is, port up */
                port_schedule_state->out_port_map.log_port_speed[logical_port]
                    = port_schedule_state->resource[i].speed;
                port_schedule_state->out_port_map.port_num_lanes[logical_port]
                    = port_schedule_state->resource[i].num_lanes;
                /* Add log_port to phy_port mapping */
                port_schedule_state->out_port_map.port_l2p_mapping[logical_port]
                    = physical_port;
                /* Add log_port to idb_port mapping */
                port_schedule_state->out_port_map.port_l2i_mapping[logical_port]
                    = port_schedule_state->resource[i].idb_port;
                /* Add phy_port to log_port mapping */
                port_schedule_state->out_port_map.port_p2l_mapping[physical_port]
                    = logical_port;
                /* Add phy_port to mmu_port mapping */
                port_schedule_state->out_port_map.port_p2m_mapping[physical_port]
                    = port_schedule_state->resource[i].mmu_port;
                /* Add mmu_port to phy_port mapping */
                port_schedule_state->out_port_map.port_m2p_mapping[
                    port_schedule_state->resource[i].mmu_port] = physical_port;
                /* Add corresponding bitbams */
                SOC_PBMP_PORT_ADD(
                    port_schedule_state->out_port_map.physical_pbm,
                    physical_port);
                if (_SHR_PORT_ENCAP_HIGIG2 ==
                    port_schedule_state->resource[i].encap) {
                    SOC_PBMP_PORT_ADD(port_schedule_state->out_port_map.hg2_pbm,
                                      logical_port);
                }
                if (1 == port_schedule_state->resource[i].oversub) {
                    SOC_PBMP_PORT_ADD(
                        port_schedule_state->out_port_map.oversub_pbm,
                        logical_port);
                }
                if ((_TH3_PHY_PORT_MNG0 == physical_port) ||
                    (_TH3_PHY_PORT_MNG1 == physical_port) ) {
                    SOC_PBMP_PORT_ADD(
                        port_schedule_state->out_port_map.management_pbm,
                        logical_port);    /* That's optional */
                }
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn void soc_tomahawk3_get_pipe_map(int unit,
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
soc_tomahawk3_get_pipe_map(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    uint32 *                   pipe_map)
{
    uint32 port, pipe, i;

    *pipe_map = 0;
    if (port_schedule_state->is_flexport == 0) { /* Init */
        for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
            if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
                pipe = port / _TH3_DEV_PORTS_PER_PIPE;
                *pipe_map |= 1 << pipe;
            }
        }
    } else { /* FlexPort */
        for (i = 0; i < port_schedule_state->nport; i++) {
            port  = port_schedule_state->resource[i].logical_port;
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            *pipe_map |= 1 << pipe;
        }
    }
}


/*! @fn int soc_tomahawk3_tdm_sched_init(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              tdm_sched_inst_name_e sched_inst_name )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name Instance name enum.
 *  @brief API to initialize the TH3 scheduler instance.
 * Description:
 *      API to initialize the TH3 scheduler instance.
 *      Regs/Mems configured: TH3 scheduler instance.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_sched_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name)
{
    uint32 port, pipe;
    uint32 pipe_map;
    int phy_port;
    int is_port_down;

    is_port_down = 0;
    soc_tomahawk3_get_pipe_map(unit, port_schedule_state, &pipe_map);

    /* Configure per port registers */
    for (port = 0; port < _TH3_DEV_PORTS_PER_DEV; port++) {
        if (port_schedule_state->out_port_map.log_port_speed[port] > 0 ) {
            pipe = port / _TH3_DEV_PORTS_PER_PIPE;
            phy_port = port_schedule_state->out_port_map.port_l2p_mapping[port];
            if (_TH3_PHY_IS_FRONT_PANEL_PORT(phy_port)) {
                soc_tomahawk3_tdm_set_cal_config(unit, port_schedule_state,
                      sched_inst_name, pipe, phy_port, is_port_down);
                soc_tomahawk3_tdm_set_ebshp_port_cfg(unit, port_schedule_state,
                      sched_inst_name, pipe, phy_port, is_port_down); /* MMU EB   PRT scheduler */
            }
            /* This is for MMU schedulers; they will be skipped for IDB or PKT schedulers */
            soc_tomahawk3_tdm_set_eb_credit_config(unit, port_schedule_state,
                      sched_inst_name, pipe, phy_port, is_port_down); /* MMU MAIN PRT scheduler */
            soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping(unit, port_schedule_state,
                      sched_inst_name, pipe, phy_port, is_port_down); /* MMU EB   PRT scheduler */
        }
    }

    /* Per pipe configs; at init time only; not part of FlexPort */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (!(pipe_map & (1 << pipe))) {
            continue;
        }
        /* Configure special slot in CALC_CONFIG */
        soc_tomahawk3_tdm_set_cal_config(unit, port_schedule_state,
                   sched_inst_name, pipe, (SOC_MAX_NUM_PORTS + 1), is_port_down);
        /* Takes effect only in IDB/MMU_EB PRT SCHED*/
        soc_tomahawk3_tdm_set_max_spacing(unit,
            port_schedule_state, sched_inst_name, pipe);
        soc_tomahawk3_tdm_set_feature_ctrl(unit,
            port_schedule_state, sched_inst_name, pipe);
        soc_tomahawk3_tdm_set_cbmg_value(unit,
            port_schedule_state, sched_inst_name, pipe);
        /* Takes effect only in MMU MAIN PRT scheduler */
        soc_tomahawk3_tdm_set_eb_satisfied_threshold(unit,
            port_schedule_state, sched_inst_name, pipe);
        soc_tomahawk3_tdm_set_min_spacing(unit,
            port_schedule_state, sched_inst_name, pipe);
        /* Takes effect only in IDB PRT scheduler */
        soc_tomahawk3_tdm_set_min_port_pick_spacing(unit,
            port_schedule_state, sched_inst_name, pipe);
        /* Takes effect only in MMU EB   PRT scheduler */
        soc_tomahawk3_tdm_set_mmu_edb_credit_threshold(unit,
            port_schedule_state, sched_inst_name, pipe);
        /* Takes effect only in MMU EB   PRT scheduler */
        soc_tomahawk3_tdm_set_urg_cell_spacing(unit,
            port_schedule_state, sched_inst_name, pipe);
        /* Takes effect only in PKT SCH*/
        soc_tomahawk3_tdm_set_pksch_pkt_credits_per_pipe(unit,
            port_schedule_state, sched_inst_name, pipe);
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_idb_init(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to initialize the TH3 IDB schedulers.
 * Description:
 *      API to initialize the TH3 IDB schedulers.
 *      Regs/Mems configured: TH3 IDB schedulers.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_idb_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_set_tdm_tbl(
        unit, port_schedule_state));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_IDB_PORT_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_IDB_PKT_SCHED));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_tdm_mmu_init(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to initialize the TH3 MMU schedulers.
 * Description:
 *      API to initialize the TH3 MMU schedulers.
 *      Regs/Mems configured: TH3 MMU schedulers.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_mmu_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_set_tdm_tbl(
        unit, port_schedule_state));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
       unit, port_schedule_state, TDM_MMU_MAIN_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_MMU_EB_PORT_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_MMU_EB_PKT_SCHED));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_init(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to initialize the TH3 IDB/MMU schedulers.
 * Description:
 *      API to initialize the TH3 IDB/MMU schedulers.
 *      Regs/Mems configured: TH3 IDB/MMU schedulers.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_init(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_set_tdm_tbl(
        unit, port_schedule_state));

    /* Ingress Schedulers */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_IDB_PORT_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_IDB_PKT_SCHED));

    /* MMU Schedulers */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_MMU_MAIN_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_MMU_EB_PORT_SCHED));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_init(
        unit, port_schedule_state, TDM_MMU_EB_PKT_SCHED));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_sched_flexport(int unit,
 *              soc_port_schedule_state_t *port_schedule_state
 *              tdm_sched_inst_name_e sched_inst_name )
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @param sched_inst_name Instance name enum.
 *  @param ports_down Ports down or up
 *  @brief API to flex the TH3 scheduler instance.
 * Description:
 *      API to flex the TH3 scheduler instance.
 *      Regs/Mems configured: TH3 scheduler instance.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_sched_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state,
    tdm_sched_inst_name_e sched_inst_name,
    int ports_down)
{
    int i, logical_port, pipe_num, phy_port;
    soc_port_map_type_t *port_map;

    if (1 == ports_down) { /* remove ports from scheduler */
        port_map = &port_schedule_state->in_port_map;
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (port_schedule_state->resource[i].physical_port == -1) {
                logical_port = port_schedule_state->resource[i].logical_port;
                pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
                phy_port =
                    port_map->port_l2p_mapping[logical_port];
                /* Removes slots allocated to this port from scheduler */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_cal_config(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Clears EDB to MMU credits counter */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_clear_edb_credit_counter_per_port(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Removes port from mmu to phy mapping */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Clears port_speed in EB SHAPER */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_ebshp_port_cfg(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
            }
        }
    } else {               /* add     ports to scheduler */
        port_map = &port_schedule_state->out_port_map;
        for (i = 0; i < port_schedule_state->nport; i++) {
            if (port_schedule_state->resource[i].physical_port != -1) {
                logical_port = port_schedule_state->resource[i].logical_port;
                pipe_num = logical_port / _TH3_DEV_PORTS_PER_PIPE;
                phy_port = port_schedule_state->resource[i].physical_port;
                /* Add slots allocated to this port in scheduler */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_cal_config(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Configure per port EB credits */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_eb_credit_config(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Adds port to mmu to phy mapping */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_mmu_port_to_phy_port_mapping(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
                /* Configures port_speed in EB SHAPER */
                SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_ebshp_port_cfg(
                      unit, port_schedule_state,
                      sched_inst_name, pipe_num, phy_port, ports_down));
            }
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_idb_flexport(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to FLEX the TH3 IDB schedulers.
 * Description:
 *      API to FLEX the TH3 IDB schedulers.
 *      Regs/Mems configured: TH3 IDB schedulers.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_idb_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int ports_down;

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_set_tdm_tbl(
        unit, port_schedule_state));

    /* Remove DOWN ports */
    ports_down = 1;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_IDB_PORT_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_IDB_PKT_SCHED, ports_down));

    /* Add UP ports */
    ports_down = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_IDB_PORT_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_IDB_PKT_SCHED, ports_down));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_tdm_mmu_flexport(int unit,
 *              soc_port_schedule_state_t *port_schedule_state)
 *  @param unit Chip unit number.
 *  @param port_schedule_state Pointer to a soc_port_schedule_state_t
 *             struct variable.
 *  @brief API to FLEX the TH3 MMU schedulers.
 * Description:
 *      API to FLEX the TH3 MMU schedulers.
 *      Regs/Mems configured: TH3 MMU schedulers.
 * Parameters:
 *      unit - Device number
 *      *port_schedule_state_t - Agreed structure between SDK and DV
 * Return Value:
 *      SOC_E_*
 */
int soc_tomahawk3_tdm_mmu_flexport(
    int unit,
    soc_port_schedule_state_t *port_schedule_state)
{
    int ports_down;

    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_set_out_port_map(
        unit, port_schedule_state));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_set_tdm_tbl(
        unit, port_schedule_state));

    /* Remove DOWN ports */
    ports_down = 1;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
       unit, port_schedule_state, TDM_MMU_MAIN_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_MMU_EB_PORT_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_MMU_EB_PKT_SCHED, ports_down));

    /* Add UP ports */
    ports_down = 0;
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
       unit, port_schedule_state, TDM_MMU_MAIN_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_MMU_EB_PORT_SCHED, ports_down));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_sched_flexport(
        unit, port_schedule_state, TDM_MMU_EB_PKT_SCHED, ports_down));

    return SOC_E_NONE;
}


/*** END SDK API COMMON CODE ***/

#endif /* BCM_TOMAHAWK3_SUPPORT */
