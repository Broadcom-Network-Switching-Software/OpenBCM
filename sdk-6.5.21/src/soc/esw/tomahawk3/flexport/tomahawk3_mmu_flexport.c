/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  tomahawk2_mmu_flexport.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#include <soc/tomahawk.h>
#include <soc/init/tomahawk3_ep_init.h>
#include <soc/init/tomahawk3_tdm.h>

#include <soc/flexport/tomahawk3_flexport.h>


/*** START SDK API COMMON CODE ***/

/*! @file tomahawk3_mmu_flexport.c
 *  @brief
 */


/*! @struct soc_th3_mmu_thdi_pg_hdrm_cfg_s
 *  @brief
 */
typedef struct soc_th3_mmu_thdi_pg_hdrm_cfg_s {
    int speed;
    uint32 line_rate_cells;
    uint32 oversub_cells;
} soc_th3_mmu_thdi_pg_hdrm_cfg_t;

static const soc_th3_mmu_thdi_pg_hdrm_cfg_t soc_th3_mmu_thdi_pg_hdrm_cfg_tbl[]
    = {
    {10000, 165, 246},
    {25000, 194, 275},
    {40000, 301, 382},
    {50000, 301, 382},
    {100000, 498, 690},
    {200000, 948, 1332},
    {400000, 1889, 2657}
    };


/* THDO config */
/*! @struct soc_th3_mmu_thdo_cfg_s
 *  @brief
 */
typedef struct soc_th3_mmu_thdo_cfg_s {
    int speed;
    /* Per Queue Thresholds */
    uint32 qgroup_valid;
    uint32 q_spid;
    uint32 q_cfg_disable_queueing;
    uint32 q_cfg_use_qgroup_min;
    uint32 q_cfg_limit_enable;
    uint32 q_cfg_color_enable;
    uint32 q_cfg_limit_red;
    uint32 q_cfg_limit_yellow;
    uint32 q_cfg_color_limit_mode;
    uint32 q_cfg_limit_dynamic;
    uint32 q_cfg_min_limit;
    uint32 q_cfg_shared_limit_alpha;
    uint32 resume_offset;
    /* Per Queue Group Thresholds */
    uint32 qgrp_cfg_min_limit;
    /* Per Port, Service Pool Thresholds */
    uint32 color_limit_enable;
    uint32 red_yellow_shared_limits;
    /* NOTE: DON'T ALTER FIELD ORDER */
} soc_th3_mmu_thdo_cfg_t;

/* THDO Config Table  */
static const soc_th3_mmu_thdo_cfg_t
    soc_th3_mmu_thdo_cfg_tbl[] = {
    { -1, /* All Speeds have same values */
        1, 0,
        0, 1, 1, 0, 0, 0, 1, 1, 0, 8,
        2, 14, 0, 132105
    }
};



/*! @fn int soc_tomahawk3_mmu_thdi_get_pg_hdrm_setting(int speed, int line_rate,
 *              uint32* setting)
 *  @param speed port speed
 *  @param line_rate whether the port is line rate or oversub
 *  @param setting pointer to the limit setting, contains returned value
 *  @brief API to provide PG headroom limit setting based on port speed and
 *         lr/os (for lossless mode)
 */
int
soc_tomahawk3_mmu_thdi_get_pg_hdrm_setting(int unit, int speed, int line_rate,
    uint32* setting)
{
    int index;
    int found = 0;
    for (index = 0; index < TH3_MMU_NUM_SPEEDS; index++) {
        if (soc_th3_mmu_thdi_pg_hdrm_cfg_tbl[index].speed == speed) {
            *setting =
                line_rate ?
                    soc_th3_mmu_thdi_pg_hdrm_cfg_tbl[index].line_rate_cells
                    : soc_th3_mmu_thdi_pg_hdrm_cfg_tbl[index].oversub_cells;
            found = 1;
            break;
        }
    }
    if (!found) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "In soc_tomahawk3_mmu_thdi_get_pg_hdrm_setting function"
            "Unsupported speed %0d for HDRM settings\n"),speed));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_get_num_queues_for_port(int unit, int pipe,
 *              int lcl_port, int *number_of_queues)
 *  @param unit Device number
 *  @param pipe Pipe Number
 *  @param lcl_port Pipe's local mmu port number
 *  @param *number_of_queues  Returned Number of Queues for that port
 *  @brief API to provide number of Queues per a Pipe's Local MMU Port.
 */
int
soc_tomahawk3_mmu_get_num_queues_for_port(int unit, int pipe, int lcl_port,
    int *number_of_queues)
{
    if (lcl_port != TH3_MMU_CPU_PORT_NUM && lcl_port <
        TH3_MMU_MAX_PORTS_PER_PIPE) {
        *number_of_queues = TH3_MMU_NUM_Q_PER_HSP_PORT;
    }
    else if ((TH3_MMU_IS_CPU_PORT(pipe, lcl_port))) {
        *number_of_queues = TH3_MMU_NUM_Q_FOR_CPU_PORT;
    }
    else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid MMU Port Specified for"
                              "Getting Num Queues."
                              "Specified value is greater than %0d."
                              "Input is lcl_port %0d, pipe %0d.\n"),
                              TH3_MMU_MAX_PORTS_PER_PIPE, lcl_port, pipe));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_get_valid_pipes_for_itm(int unit, int itm,
 *              int *pipes)
 *  @param unit Device number
 *  @param itm ITM number
 *  @param *pipes Returns Valid pipes for ITM
 *  @brief API to provide a ITM's valid pipe numbers
 *         Returns an error if itm num > 1
 */
int
soc_tomahawk3_mmu_get_valid_pipes_for_itm(int unit, int itm,
    int *pipes)
{
    int i;

    for (i = 0; i < SOC_MAX_NUM_PIPES; i++)
    {
        pipes[i] = 0;
    }

    if (itm == 0)
    {
        pipes[0] = 1;
        pipes[1] = 1;
        pipes[6] = 1;
        pipes[7] = 1;
    }
    else if (itm == 1)
    {
        pipes[2] = 1;
        pipes[3] = 1;
        pipes[4] = 1;
        pipes[5] = 1;
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid ITM number specified."
                              "Specified ITM greater than 1.\n")));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_get_itm_from_pipe_num(int unit, int pipe,
 *              int *itm_num)
 *  @param unit Device number
 *  @param Pipe Pipe number
 *  @param *itm_num Returns ITM for this Pipe
 *  @brief API to provide the ITM for the given Pipe
 */
int
soc_tomahawk3_mmu_get_itm_from_pipe_num(int unit, int pipe,
    int *itm_num)
{
   if (pipe == 0 || pipe == 1 || pipe == 6 || pipe == 7)
   {
      *itm_num = 0;
   }
   else if (pipe == 2 || pipe == 3 || pipe == 4 || pipe == 5)
   {
      *itm_num = 1;
   }
   else
   {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Invalid Pipe number specified."
                              "Specified Pipe greater than 7.\n")));
        return SOC_E_FAIL;
   }
    return SOC_E_NONE;
}

/*! @fn void soc_tomahawk3_mmu_get_q_base_num_for_pipe(int unit, int pipe,
 *              int *pipe_q_base_num)
 *  @param unit Device number
 *  @param Pipe Pipe number
 *  @param *pipe_q_base_num Returns Q Base Num for this Pipe
 *  @brief API to provide a given Pipe's Queue base number
 */
void
soc_tomahawk3_mmu_get_q_base_num_for_pipe(int unit, int pipe,
    int *pipe_q_base_num)
{
    int pipe0_num_queues, pipe0_q_base_num;
    int pipe1_num_queues, pipe1_q_base_num;
    int pipe2_num_queues, pipe2_q_base_num;
    int pipe3_num_queues, pipe3_q_base_num;
    int pipe4_num_queues, pipe4_q_base_num;
    int pipe5_num_queues, pipe5_q_base_num;
    int pipe6_num_queues, pipe6_q_base_num;
    int pipe7_q_base_num;

    pipe0_num_queues = 19 * TH3_MMU_NUM_Q_PER_HSP_PORT
                            + TH3_MMU_NUM_Q_FOR_CPU_PORT;
    pipe1_num_queues = 20 * TH3_MMU_NUM_Q_PER_HSP_PORT;
    pipe2_num_queues = 19 * TH3_MMU_NUM_Q_PER_HSP_PORT;
    pipe3_num_queues = 19 * TH3_MMU_NUM_Q_PER_HSP_PORT;
    pipe4_num_queues = 19 * TH3_MMU_NUM_Q_PER_HSP_PORT;
    pipe5_num_queues = 20 * TH3_MMU_NUM_Q_PER_HSP_PORT;
    pipe6_num_queues = 20 * TH3_MMU_NUM_Q_PER_HSP_PORT;

    pipe0_q_base_num = 0;
    pipe1_q_base_num = pipe0_q_base_num + pipe0_num_queues;
    pipe2_q_base_num = pipe1_q_base_num + pipe1_num_queues;
    pipe3_q_base_num = pipe2_q_base_num + pipe2_num_queues;
    pipe4_q_base_num = pipe3_q_base_num + pipe3_num_queues;
    pipe5_q_base_num = pipe4_q_base_num + pipe4_num_queues;
    pipe6_q_base_num = pipe5_q_base_num + pipe5_num_queues;
    pipe7_q_base_num = pipe6_q_base_num + pipe6_num_queues;

    if (pipe == 0) *pipe_q_base_num = pipe0_q_base_num;
    if (pipe == 1) *pipe_q_base_num = pipe1_q_base_num;
    if (pipe == 2) *pipe_q_base_num = pipe2_q_base_num;
    if (pipe == 3) *pipe_q_base_num = pipe3_q_base_num;
    if (pipe == 4) *pipe_q_base_num = pipe4_q_base_num;
    if (pipe == 5) *pipe_q_base_num = pipe5_q_base_num;
    if (pipe == 6) *pipe_q_base_num = pipe6_q_base_num;
    if (pipe == 7) *pipe_q_base_num = pipe7_q_base_num;
}

/*! @fn int soc_tomahawk3_mmu_get_chip_q_num_base_for_port(int unit, int pipe,
 *              int local_mmu_port,
 *              int *chip_q_base_num_for_port)
 *  @param unit Device number
 *  @param Pipe Pipe number
 *  @param local_mmu_port local mmu port number
 *  @param *pipe_q_base_num Returns Q Base Num for this Port
 *  @brief API to provide a given Port's Queue base number
 */
int
soc_tomahawk3_mmu_get_chip_q_num_base_for_port(int unit, int pipe,
    int local_mmu_port,
    int *chip_q_base_num_for_port)
{
    int pipe_q_base_index;
    int num_queues;

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_num_queues_for_port(unit,
                                        pipe, local_mmu_port, &num_queues));

    soc_tomahawk3_mmu_get_q_base_num_for_pipe(unit, pipe, &pipe_q_base_index);
    *chip_q_base_num_for_port = pipe_q_base_index + (local_mmu_port * num_queues);

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_mmu_get_thdi_bst_mode(int unit, int *thdi_bst_mode)
 *  @param unit Device number
 *  @param *thdi_bst_mode Return value thdi bst mode
 *  @brief API to return the THDI bst mode programmed
 */
int
soc_tomahawk3_mmu_get_thdi_bst_mode(int unit,
    int *thdi_bst_mode)
{
    uint32 rval;
    int inst = 0;

    rval = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, MMU_THDI_BSTCONFIGr,
                                            inst, 0, &rval));
    *thdi_bst_mode = soc_reg_field_get(unit, MMU_THDI_BSTCONFIGr, rval,
                                  TRACKING_MODEf);
    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_get_thdo_bst_mode(int unit, int *thdo_bst_mode)
 *  @param unit Device number
 *  @param *thdo_bst_mode Return value thdo bst mode
 *  @brief API to return the thdo bst mode programmed
 */
int
soc_tomahawk3_mmu_get_thdo_bst_mode(int unit,
    int *thdo_bst_mode)
{
    uint32 rval;
    int inst = 0;

    rval = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, MMU_THDO_BST_CONFIGr,
                                            inst, 0, &rval));
    *thdo_bst_mode = soc_reg_field_get(unit, MMU_THDO_BST_CONFIGr, rval,
                                  TRACKING_MODEf);

    return SOC_E_NONE;
}


/*! @fn void soc_tomahawk3_mmu_get_pipe_flexed_status(int unit,
 *              soc_port_resource_t *port_resource_t, int pipe, int *pipe_flexed)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param pipe Pipe to check
 *  @param *pipe_flexed Pipe Flexed status,
 *             0 = 'No change on any Ports in this Pipe'
 *             1 = 'One of more Ports in this Pipe has changed configuration'
 *  @brief API to return the Flexed status of a particular pipe
 */
void
soc_tomahawk3_mmu_get_pipe_flexed_status(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe,
    int *pipe_flexed)
{
    int port;
    int dev_port, phy_port, mmu_port;
    int pipe_id;

    *pipe_flexed = 0;

    for (port = 0; port < port_schedule_state_t->nport; port++) {
        
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            pipe_id = TH3_MMU_PIPE_NUM(mmu_port);
        }
        else {
            pipe_id = port_schedule_state_t->resource[port].pipe;
        }

        if (pipe_id == pipe) {
            *pipe_flexed = 1;
            break;
        }
    }
}


/*! @fn int soc_tomahawk3_mmu_get_itm_flexed_status(int unit,
 *              soc_port_resource_t *port_resource_t, int itm, int *itm_flexed)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param itm ITM to check
 *  @param *itm_flexed ITM Flexed status,
 *             0 = 'No change on any Ports in this ITM'
 *             1 = 'One of more Ports in this ITM has changed configuration'
 *  @brief API to return the Flexed status of a particular ITM
 */
int
soc_tomahawk3_mmu_get_itm_flexed_status(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int itm,
    int *itm_flexed)
{
    int port;
    int dev_port, phy_port, mmu_port;
    int pipe_id;
    int itm_pipes[SOC_MAX_NUM_PIPES];

    *itm_flexed = 0;

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_valid_pipes_for_itm(unit, itm, itm_pipes));

    for (port = 0; port < port_schedule_state_t->nport; port++) {
        
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            pipe_id = TH3_MMU_PIPE_NUM(mmu_port);
        }
        else {
            pipe_id = port_schedule_state_t->resource[port].pipe;
        }
        
         if (itm_pipes[pipe_id] == 1) {
             *itm_flexed = 1;
             break;
         }
    }

    return SOC_E_NONE;

}

/*! @fn int soc_tomahawk3_get_max_src_ct_speed(int unit, int dst_speed,
 *              int ct_mode,
 *              uint32 *max_speed)
 *  @param unit Device number
 *  @param dst_speed Dst port speed value
 *  @param *max_speed max src speed
 *  @brief API to return the Maximum src port speed allowed to
 *         cut-through to a destination port.
 */
int
soc_tomahawk3_get_max_src_ct_speed(int unit, int dst_speed,
    int ct_mode,
    uint32 *max_speed)
{
    uint32 max_speed_val;

    if (ct_mode == _SOC_ASF_MODE_SAF) {
        max_speed_val = 0;
    }
    else if (ct_mode == _SOC_ASF_MODE_SAME_SPEED) {
        switch (dst_speed) {
            case 10000  : max_speed_val = 40000; break;
            case 25000  : max_speed_val = 100000; break;
            case 40000  : max_speed_val = 100000; break;
            case 50000  : max_speed_val = 200000; break;
            case 100000 : max_speed_val = 400000; break;
            case 200000 : max_speed_val = 400000; break;
            case 400000 : max_speed_val = 400000; break;
            default     : max_speed_val = 0; break;
        }
    }
    else {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
            "soc_tomahawk3_get_max_src_ct_speed()"
            "Unsupported SOC_ASF_MODE\n")));
        return SOC_E_PARAM;
    }

    *max_speed = max_speed_val;

    return SOC_E_NONE;
}

/*! @fn void soc_tomahawk3_mmu_get_speed_decode(int unit, int speed,
 *              int *speed_decode)
 *  @param unit Device number
 *  @param speed Actual speed value
 *  @param *speed_decode Decoded speed ID value
 *  @brief API to return the decoded speed value, used to program
 *         EBCTM registers
 */
void
soc_tomahawk3_mmu_get_speed_decode(int unit, int speed,
    uint32 *speed_decode)
{
    switch (speed) {
        case 10000  : *speed_decode = 1; break;
        case 25000  : *speed_decode = 2; break;
        case 40000  : *speed_decode = 3; break;
        case 50000  : *speed_decode = 4; break;
        case 100000 : *speed_decode = 5; break;
        case 200000 : *speed_decode = 6; break;
        case 400000 : *speed_decode = 7; break;
        default     : *speed_decode = 0; break;
    }

}

/*! @fn int soc_tomahawk3_flex_mmu_port_flush(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to flush MMU during flexport port down operation.
 */
int
soc_tomahawk3_flex_mmu_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    uint32 temp32;
    int pipe, itm;
    int pipe_flexed, itm_flexed;

    temp32 = TH3_MMU_FLUSH_ON;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "flex_mmu_port_flush() :: setting FLUSH_ON\n")));

    /* Per-Pipe configuration */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Pipe %0d Flexed status = %0d\n"), pipe, pipe_flexed));

        if (pipe_flexed == 1) {

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_qsch_port_flush\n")));
            soc_tomahawk3_mmu_qsch_port_flush(unit, port_schedule_state_t,
                pipe, temp32);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_ebqs_port_flush\n")));
            soc_tomahawk3_mmu_ebqs_port_flush(unit, port_schedule_state_t,
                pipe, temp32);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_mtro_port_refresh\n")));
            soc_tomahawk3_mmu_mtro_port_metering_config(unit, port_schedule_state_t,
                pipe, temp32);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_intfo_clr_port_en\n")));
            soc_tomahawk3_mmu_intfo_clr_port_en(unit, port_schedule_state_t,
                pipe);
        }
    }


    /* Per-ITM configuration */
    for (itm = 0; itm < _TH3_ITMS_PER_DEV; itm++) {

        soc_tomahawk3_mmu_get_itm_flexed_status(
            unit, port_schedule_state_t, itm, &itm_flexed);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "ITM %0d Flexed status = %0d\n"), itm, itm_flexed));

        if (itm_flexed == 1) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_rqe_port_flush\n")));
            SOC_IF_ERROR_RETURN(
                soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 1));
            /* Call a second time due to workaround needed for RQE */
            SOC_IF_ERROR_RETURN(
                soc_tomahawk3_mmu_rqe_port_flush(unit, itm, 2));
        }
    }
    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "flex_mmu_port_flush() :: after FLUSH_ON set\n")));

    /* Wait for EBCTM empty status */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Pipe %0d Flexed status = %0d\n"), pipe, pipe_flexed));
        
        if (pipe_flexed == 1) {
            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_wait_ebctm_empty\n")));
            SOC_IF_ERROR_RETURN(
                soc_tomahawk3_mmu_wait_ebctm_empty(unit,
                                            port_schedule_state_t, pipe));
        }
    }


    temp32 = TH3_MMU_FLUSH_OFF;

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "flex_mmu_port_flush() :: setting FLUSH_OFF\n")));

    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_qsch_port_flush\n")));
            soc_tomahawk3_mmu_qsch_port_flush(unit, port_schedule_state_t,
                pipe, temp32);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before mmu_ebqs_port_flush\n")));
            soc_tomahawk3_mmu_ebqs_port_flush(unit, port_schedule_state_t,
                pipe, temp32);
        }
    }

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "flex_mmu_port_flush() :: after FLUSH_OFF set\n")));


    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_flex_mmu_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port down operation.
 */
int
soc_tomahawk3_flex_mmu_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    int port;
    int logical_port, phy_port, mmu_port;

    /* MMU Port Flush API */
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_flex_mmu_port_flush(unit, port_schedule_state_t));

    /* MMU Port Flush API */
    /* Call a second time due to workaround needed for RQE */
    /*SOC_IF_ERROR_RETURN(
        soc_tomahawk3_flex_mmu_port_flush(unit, port_schedule_state_t));*/

    /* EBCTM settings for each Port Down */
    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            logical_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[logical_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];

            soc_tomahawk3_mmu_ebctm_reconfig(
                    unit, &port_schedule_state_t->resource[port],
                    port_schedule_state_t->cutthru_prop.asf_modes[logical_port],
                    mmu_port);
        }
    }

    /* Per-Port configuration : Port mappings for Port Down */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            soc_tomahawk3_mmu_set_mmu_port_mappings(
                unit, port_schedule_state_t, &port_schedule_state_t->resource[port]);
        }
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_port_down(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to update all MMU related functionalities during
 *         port down operation.
 */
int
soc_tomahawk3_mmu_port_down(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    /* MMU Port Flush API */
    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_flex_mmu_port_flush(unit, port_schedule_state_t));

    /* MMU Port Flush API */
    /* Call a second time due to workaround needed for RQE */
    /*SOC_IF_ERROR_RETURN(
        soc_tomahawk3_flex_mmu_port_flush(unit, port_schedule_state_t));*/

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_port_up(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to update all MMU related functionalities during
 *         port up operation.
 */
int
soc_tomahawk3_mmu_port_up(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    int pipe;
    int pipe_flexed;

    /* Per-Pipe configuration */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);
        if (pipe_flexed == 1) {
            soc_tomahawk3_mmu_mtro_port_metering_config(unit,
                port_schedule_state_t, pipe, 0);
        }
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_mmu_reconfigure_phase1(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t) {
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to reconfigure TDM calendars for MMU.
 */
int
soc_tomahawk3_flex_mmu_reconfigure_phase1(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_mmu_flexport(
        unit, port_schedule_state_t));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_set_mmu_port_mappings(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule State Struct
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the MMU port mappings for various blocks
 *         during port up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_mmu_port_mappings(int unit, 
    soc_port_schedule_state_t *port_schedule_state_t, 
    soc_port_resource_t *port_resource_t)
{

    /* When a physical port goes down, the port resource struct phy port number
     * becomes -1. At that time, we need to invalidate the phy port  When
     * a port is coming back up, the struct is populated correctly.
     */

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_crb_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_crb_port_mapping(unit,
                                                            port_resource_t));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_rqe_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_rqe_port_mapping(unit,
                                                            port_resource_t));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_ebcfp_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_ebcfp_port_mapping(unit,
                                        port_schedule_state_t, port_resource_t));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_intfi_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_intfi_port_mapping(unit,
                                        port_schedule_state_t, port_resource_t));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_intfo_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_intfo_port_mapping(unit,
                                        port_schedule_state_t, port_resource_t));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "before soc_tomahawk3_mmu_set_thdo_port_mapping\n")));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_set_thdo_port_mapping(unit,
                                        port_schedule_state_t, port_resource_t));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_crb_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the CRB during mmu flexport reconfiguration
 */
int
soc_tomahawk3_mmu_crb_reconfig(int unit,
    soc_port_resource_t *port_resource_t)
{
    uint32 port_speed_new;
    soc_reg_t reg1;
    uint32 rval1;
    int inst1;

    /* Update SRC Port speed */

    reg1 = MMU_CRB_SRC_PORT_CFGr;

    /*READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    rval1 = 0;

    inst1 = port_resource_t->mmu_port;

    switch (port_resource_t->speed) {
        case 10000  : port_speed_new = 1;
                      break;
        case 25000  : port_speed_new = 2;
                      break;
        case 40000  : port_speed_new = 3;
                      break;
        case 50000  : port_speed_new = 4;
                      break;
        case 100000 : port_speed_new = 5;
                      break;
        case 200000 : port_speed_new = 6;
                      break;
        case 400000 : port_speed_new = 7;
                      break;
        default     : port_speed_new = 0;
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "MMU_CRB_RECONFIG : Incorrect speed specified "
                              "%0d\n"), port_resource_t->speed));
                      return SOC_E_FAIL;
                      break;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg1, inst1, 0, &rval1));
    soc_reg_field_set(unit, reg1, &rval1, PORT_SPEEDf, port_speed_new);

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg1, inst1, 0, rval1));


    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk3_mmu_scb_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the SCB during mmu flexport reconfiguration
 */
int
soc_tomahawk3_mmu_scb_reconfig(int unit,
    soc_port_resource_t *port_resource_t)
{
    uint32 port_speed_new;
    soc_reg_t reg1;
    uint32 rval1;
    int inst1;


    reg1 = MMU_SCB_SOURCE_PORT_CFGr;

    /*READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    rval1 = 0;

    inst1 = port_resource_t->mmu_port;

    switch (port_resource_t->speed) {
        case 10000  : port_speed_new = 1;
                      break;
        case 25000  : port_speed_new = 2;
                      break;
        case 40000  : port_speed_new = 3;
                      break;
        case 50000  : port_speed_new = 4;
                      break;
        case 100000 : port_speed_new = 5;
                      break;
        case 200000 : port_speed_new = 6;
                      break;
        case 400000 : port_speed_new = 7;
                      break;
        default     : port_speed_new = 0;
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "MMU_SCB_RECONFIG : Incorrect speed specified "
                              "%0d\n"), port_resource_t->speed));
                      return SOC_E_FAIL;
                      break;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg1, inst1, 0, &rval1));
    soc_reg_field_set(unit, reg1, &rval1, PORT_SPEEDf, port_speed_new);

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg1, inst1, 0, rval1));

    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk3_mmu_oqs_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the OQS during mmu flexport reconfiguration
 */
int
soc_tomahawk3_mmu_oqs_reconfig(int unit,
    soc_port_resource_t *port_resource_t)
{
    uint32 port_speed_new;
    soc_reg_t reg;
    uint32 rval;
    int inst;

    reg = MMU_OQS_AGER_DST_PORT_MAPr;

    inst = port_resource_t->mmu_port;

    switch (port_resource_t->speed) {
        case 10000  : port_speed_new = 0;
                      break;
        case 25000  : port_speed_new = 0;
                      break;
        case 40000  : port_speed_new = 0;
                      break;
        case 50000  : port_speed_new = 0;
                      break;
        case 100000 : port_speed_new = 1;
                      break;
        case 200000 : port_speed_new = 2;
                      break;
        case 400000 : port_speed_new = 3;
                      break;
        default     : port_speed_new = 0;
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "MMU_OQS_RECONFIG : Incorrect speed specified "
                              "%0d\n"), port_resource_t->speed));
                      return SOC_E_FAIL;
                      break;
    }

    /* Read-Modify-Write */
    rval = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, inst, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, PORT_SPEEDf, port_speed_new);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk3_mmu_ebctm_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t,
 *              soc_asf_mode_e ct_mode, int mmu_port)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @param ct_mode CT Mode
 *  @param mmu_port The mmu_port for this api call
 *  @brief API to update the EBCTM config during
 *         port up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_ebctm_reconfig(int unit,
    soc_port_resource_t  *port_resource_t,
    soc_asf_mode_e        ct_mode,
    int mmu_port)
{
    int dst_port_speed;
    uint32 max_src_speed;
    uint32 dst_speed_id, max_src_speed_id;
    soc_reg_t ct_reg;
    soc_reg_t tct_reg;
    uint32 rval;
    int enable;
    int offset;
    int inst;

    ct_reg   = MMU_EBCTM_EPORT_CT_CFGr;
    tct_reg  = MMU_EBCTM_EPORT_TCT_CFGr;

    inst = mmu_port;

    if (port_resource_t->physical_port == -1) {
        enable = 0;
        offset = 0;
        dst_port_speed = 0;
    }
    else {
        if (ct_mode == _SOC_ASF_MODE_SAF) {
            enable = 0;
            offset = 0;
            dst_port_speed = 0;
        }
        else if (ct_mode == _SOC_ASF_MODE_SAME_SPEED) {
            enable = 1;
            offset = TH3_MMU_TCT_ON_TO_CT_OFFSET;
            dst_port_speed = port_resource_t->speed;
        }
        else {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "soc_tomahawk3_mmu_ebctm_reconfig()"
                "Unsupported SOC_ASF_MODE\n")));
            return SOC_E_PARAM;
        }
    }


    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "soc_tomahawk3_mmu_ebctm_reconfig():"
                    "physical_port=%d log_port=%3d mmu_port=%3d ct_mode=%d\n"),
                    port_resource_t->physical_port,
                    port_resource_t->logical_port, mmu_port, ct_mode));


    /* CT Config */

    rval = 0;
    SOC_IF_ERROR_RETURN
            (soc_reg32_rawport_get(unit, ct_reg, inst, 0, &rval));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_get_max_src_ct_speed(unit,
                                dst_port_speed, ct_mode, &max_src_speed));

    soc_tomahawk3_mmu_get_speed_decode(unit, dst_port_speed, &dst_speed_id);
    soc_tomahawk3_mmu_get_speed_decode(unit, max_src_speed, &max_src_speed_id);

    soc_reg_field_set(unit, ct_reg, &rval, CT_ENABLEf, enable);
    soc_reg_field_set(unit, ct_reg, &rval, DST_PORT_SPEEDf, dst_speed_id);
    soc_reg_field_set(unit, ct_reg, &rval, MAX_SRC_PORT_SPEEDf, max_src_speed_id);

    SOC_IF_ERROR_RETURN
            (soc_reg32_rawport_set(unit, ct_reg, inst, 0, rval));

    /* TCT Config */

    rval = 0;
    SOC_IF_ERROR_RETURN
            (soc_reg32_rawport_get(unit, tct_reg, inst, 0, &rval));

    soc_reg_field_set(unit, tct_reg, &rval, TCT_ENABLEf, enable);
    soc_reg_field_set(unit, tct_reg, &rval, TCT_ON_TO_CT_OFFSETf, offset);
    SOC_IF_ERROR_RETURN
            (soc_reg32_rawport_set(unit, tct_reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_ebpcc_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the EBPCC during mmu flexport reconfiguration
 */
int
soc_tomahawk3_mmu_ebpcc_reconfig(int unit,
    soc_port_resource_t *port_resource_t) 
{
    soc_reg_t reg;
    uint32 rval;
    int inst;
    int port_speed;

    reg = MMU_EBPCC_EPORT_CFGr;

    inst = port_resource_t->mmu_port;

    switch (port_resource_t->speed) {
        case 10000  : port_speed = 1;
                      break;
        case 25000  : port_speed = 2;
                      break;
        case 40000  : port_speed = 3;
                      break;
        case 50000  : port_speed = 4;
                      break;
        case 100000 : port_speed = 5;
                      break;
        case 200000 : port_speed = 6;
                      break;
        case 400000 : port_speed = 7;
                      break;
        default     : port_speed = 0;
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "MMU_EBPCC_RECONFIG : Incorrect speed specified "
                              "%0d\n"), port_resource_t->speed));
                      return SOC_E_FAIL;
                      break;
    }
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, DST_PORT_SPEEDf, port_speed);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;

}


/*! @fn int soc_tomahawk3_mmu_eb_sch_reconfig(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the EBQS during mmu flexport reconfiguration
 */
int
soc_tomahawk3_mmu_eb_sch_reconfig(int unit,
    soc_port_resource_t *port_resource_t) 
{
    soc_reg_t reg1;
    uint32 rval1;
    int inst;
    uint32 ebqs_port_speed;

    reg1 = MMU_EBQS_PORT_CFGr;

    inst = port_resource_t->mmu_port;

    switch (port_resource_t->speed) {
        case 10000  : ebqs_port_speed = 1;
                      break;
        case 25000  : ebqs_port_speed = 2;
                      break;
        case 40000  : ebqs_port_speed = 3;
                      break;
        case 50000  : ebqs_port_speed = 4;
                      break;
        case 100000 : ebqs_port_speed = 5;
                      break;
        case 200000 : ebqs_port_speed = 6;
                      break;
        case 400000 : ebqs_port_speed = 7;
                      break;
        default     : ebqs_port_speed = 0;
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                              "MMU_EB_SCH_RECONFIG : Incorrect speed specified "
                              "%0d\n"), port_resource_t->speed));
                      return SOC_E_FAIL;
                      break;
    }

    /* Read-Modify-Write */
    rval1 = 0;
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg1, inst, 0, &rval1));
    soc_reg_field_set(unit, reg1, &rval1, PORT_SPEEDf, ebqs_port_speed);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg1, inst, 0, rval1));

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_mmu_set_crb_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the CRB's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_crb_port_mapping(int unit,
    soc_port_resource_t *port_resource_t)
{
    uint32 mmu_port_new;
    soc_reg_t reg;
    uint32 rval;
    int inst;

    /* Port Mapping update */

    inst = port_resource_t->logical_port;

    if (port_resource_t->physical_port == -1) {
        mmu_port_new = TH3_INVALID_MMU_PORT;
        /*Invalid entries should be programmed to 0xFF.*/
    }
    else {
        mmu_port_new = port_resource_t->mmu_port;
    }

    reg = MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr;
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, MMU_PORTf, mmu_port_new);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_set_rqe_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the RQE's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_rqe_port_mapping(int unit,
    soc_port_resource_t *port_resource_t)
{
    uint32 mmu_port_new;
    uint32 mem_entry_data[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    int mem_indx;

    if (port_resource_t->physical_port == -1) {
        mmu_port_new = TH3_INVALID_MMU_PORT; /* All 1's */
    }
    else {
        mmu_port_new = port_resource_t->mmu_port;
    }

    mem = MMU_RQE_DEVICE_TO_MMU_PORT_MAPPINGm;
    sal_memset(mem_entry_data, 0, sizeof(mem_entry_data));

    mem_indx = port_resource_t->logical_port;

    soc_mem_field_set(unit, mem, mem_entry_data, MMU_PORTf,
                      &mmu_port_new);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      mem_indx, mem_entry_data));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_set_ebcfp_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the EBCFP's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_ebcfp_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t)
{
    soc_reg_t reg;
    uint32 rval;
    int inst;
    int device_port_new;
    int dev_port, phy_port, mmu_port;

    dev_port = port_resource_t->logical_port;

    if (port_resource_t->physical_port == -1) {
        phy_port = port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
        mmu_port = port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
    }
    else {
        phy_port = port_resource_t->physical_port;
        mmu_port = port_resource_t->mmu_port;
    }

    inst = mmu_port;

    if (port_resource_t->physical_port == -1) {
        device_port_new = TH3_INVALID_DEV_PORT; /*Resetval*/
    }
    else {
        device_port_new = dev_port;
    }

    reg = MMU_EBCFP_MMU_PORT_TO_DEVICE_PORT_MAPPINGr;
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, DEVICE_PORTf, device_port_new);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_set_intfi_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the INTFI's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_intfi_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t)
{
    uint32 phy_port_new;
    soc_reg_t reg;
    uint32 rval;
    int inst;
    int dev_port, phy_port, mmu_port;
    int pipe;

    dev_port = port_resource_t->logical_port;

    if (port_resource_t->physical_port == -1) {
        phy_port = port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
        mmu_port = port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
        pipe = TH3_MMU_PIPE_NUM(mmu_port);
    }
    else {
        phy_port = port_resource_t->physical_port;
        mmu_port = port_resource_t->mmu_port;
        pipe = port_resource_t->pipe;
    }

    inst = mmu_port;

    if (port_resource_t->physical_port == -1) {
        phy_port_new = 63; /*Resetval*/
    }
    else {
        if (pipe == 0) {
            phy_port_new = phy_port % TH3_PHY_PORT_PIPE_OFFSET;
        } else {
            phy_port_new = (phy_port - 1) % TH3_PHY_PORT_PIPE_OFFSET;
        }
    }

    reg = MMU_INTFI_MMU_PORT_TO_PHY_PORT_MAPPINGr;
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, PHY_PORT_NUMf, phy_port_new);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_set_intfo_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the INTFO's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_intfo_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t)
{
    uint32 phy_port_new;
    soc_reg_t reg;
    uint32 rval;
    int inst;
    int dev_port, phy_port, mmu_port;

    dev_port = port_resource_t->logical_port;

    if (port_resource_t->physical_port == -1) {
        phy_port = port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
        mmu_port = port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
    }
    else {
        phy_port = port_resource_t->physical_port;
        mmu_port = port_resource_t->mmu_port;
    }

    inst = mmu_port;

    if (port_resource_t->physical_port == -1) {
        phy_port_new = 0; /*Resetval*/
    }
    else {
        phy_port_new = phy_port;
    }

    reg = MMU_INTFO_MMU_PORT_TO_PHY_PORT_MAPPINGr;
    rval = 0;
    soc_reg_field_set(unit, reg, &rval, PHY_PORT_NUMf, phy_port_new);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_set_thdo_port_mapping(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to update the THDO's port mapping during port
 *         up and port down operations in flexport.
 */
int
soc_tomahawk3_mmu_set_thdo_port_mapping(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    soc_port_resource_t *port_resource_t)
{
    soc_mem_t mem;
    uint32    thdo_port_map_data[SOC_MAX_MEM_WORDS];

    int inst;
    uint32 device_port_new;
    int dev_port, phy_port, mmu_port;
    int pipe, local_mmu_port;
    int chip_port_num;

    dev_port = port_resource_t->logical_port;

    if (port_resource_t->physical_port == -1) {
        phy_port = port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
        mmu_port = port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
        pipe = TH3_MMU_PIPE_NUM(mmu_port);
    }
    else {
        phy_port = port_resource_t->physical_port;
        mmu_port = port_resource_t->mmu_port;
        pipe = port_resource_t->pipe;
    }

    local_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

    /* THDO uses Chip Port Num */
    chip_port_num = (pipe * TH3_MMU_NUM_PORTS_PER_PIPE_L) + local_mmu_port;
    inst = chip_port_num;

    if (port_resource_t->physical_port == -1) {
        device_port_new = TH3_THDO_INVALID_DEV_PORT; /*Resetval*/
    }
    else {
        device_port_new = dev_port;
    }

    mem = MMU_THDO_DEVICE_PORT_MAPm;
    sal_memset(thdo_port_map_data, 0, sizeof(thdo_port_map_data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                        MEM_BLOCK_ANY, inst,
                                        thdo_port_map_data));

    soc_mem_field_set(unit, mem, thdo_port_map_data,
                        DEVICE_PORTf, &device_port_new);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                        MEM_BLOCK_ANY, inst,
                                        thdo_port_map_data));

    return SOC_E_NONE;
}



/*! @fn int soc_tomahawk3_mmu_qsch_port_flush(int unit,
 *      soc_port_schedule_state_t *port_schedule_state_t, int pipe, uint32 set_val)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param set_val Value to be set to the QSCH port flush registers
 *  @brief API to Set the QSCH Port Flush registers
 */
int
soc_tomahawk3_mmu_qsch_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 enable_val;
    int port;
    int pipe_for_port_down;
    int dev_port, phy_port;
    int mmu_port, lcl_mmu_port;
    int inst;
    uint32 new_val;
    uint32 temp;

    reg = MMU_QSCH_PORT_FLUSHr;

    inst = pipe;
    
    /*READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    rval = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, inst, 0, &rval));
    enable_val = soc_reg_field_get(unit, reg, rval, ENABLEf);

    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {

            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            pipe_for_port_down = TH3_MMU_PIPE_NUM(mmu_port);

            if (pipe_for_port_down == pipe) {

                new_val = 1;

                lcl_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

                temp = TH3_MMU_FLUSH_ON;

                new_val <<= lcl_mmu_port;


                if (set_val == temp) {
                    enable_val |= new_val;
                } else {
                    new_val = ~new_val;
                    enable_val &= new_val;
                }

                rval = 0;
                soc_reg_field_set(unit, reg, &rval, ENABLEf, enable_val);

            }
        }
    }

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}




/*! @fn int soc_tomahawk3_mmu_intfo_clr_port_en(int unit,
 *      soc_port_schedule_state_t *port_schedule_state_t, int pipe)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param pipe Pipe number
 *  @brief API to clear the oobport enable bits in INTFO for a disabled port
 */
int
soc_tomahawk3_mmu_intfo_clr_port_en(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe)
{
    soc_mem_t mem_oobport_mapping_thdi, mem_oobport_mapping_thdo;
    uint32    entry_oobport_mapping_thdi[SOC_MAX_MEM_WORDS];
    uint32    entry_oobport_mapping_thdo[SOC_MAX_MEM_WORDS];

    soc_reg_t reg_ing_port_en0, reg_ing_port_en1, reg_ing_port_en2;
    soc_reg_t reg_eng_port_en0, reg_eng_port_en1, reg_eng_port_en2;
    soc_reg_t reg_cong_st_en0, reg_cong_st_en1, reg_cong_st_en2;

    uint64 rval_ing_port_en0, rval_ing_port_en1; uint32 rval_ing_port_en2;
    uint64 rval_eng_port_en0, rval_eng_port_en1; uint32 rval_eng_port_en2;
    uint64 rval_cong_st_en0, rval_cong_st_en1; uint32 rval_cong_st_en2;

    uint64 ing_port_en_field_val0;
    uint64 ing_port_en_field_val1;
    uint32 ing_port_en_field_val2;
    uint64 eng_port_en_field_val0;
    uint64 eng_port_en_field_val1;
    uint32 eng_port_en_field_val2;
    uint64 cong_st_en_field_val0;
    uint64 cong_st_en_field_val1;
    uint32 cong_st_en_field_val2;

    int port, inst;
    int pipe_for_port_down;
    int dev_port, phy_port, mmu_port;
    uint32 ing_oob_port_fldval[1];
    uint32 eng_oob_port_fldval[1];
    uint32 cng_oob_port_fldval[1];
    uint32 ing_oob_port;
    uint32 eng_oob_port;
    uint32 cng_oob_port;

    reg_ing_port_en0 = MMU_INTFO_OOBFC_ING_PORT_EN0_64r;
    reg_ing_port_en1 = MMU_INTFO_OOBFC_ING_PORT_EN1_64r;
    reg_ing_port_en2 = MMU_INTFO_OOBFC_ING_PORT_EN2r;
    reg_eng_port_en0 = MMU_INTFO_OOBFC_ENG_PORT_EN0_64r;
    reg_eng_port_en1 = MMU_INTFO_OOBFC_ENG_PORT_EN1_64r;
    reg_eng_port_en2 = MMU_INTFO_OOBFC_ENG_PORT_EN2r;
    reg_cong_st_en0  = MMU_INTFO_OOBFC_CONGST_ST_EN0_64r;
    reg_cong_st_en1  = MMU_INTFO_OOBFC_CONGST_ST_EN1_64r;
    reg_cong_st_en2  = MMU_INTFO_OOBFC_CONGST_ST_EN2r;

    mem_oobport_mapping_thdi = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDIm;
    mem_oobport_mapping_thdo = MMU_INTFO_MMU_PORT_TO_OOB_PORT_MAPPING_THDO0m;

    inst = 0;

    /*READ MODIFY WRITE*/

    COMPILER_64_ZERO(rval_ing_port_en0);
    COMPILER_64_ZERO(rval_ing_port_en1);
    rval_ing_port_en2 = 0;
    COMPILER_64_ZERO(rval_eng_port_en0);
    COMPILER_64_ZERO(rval_eng_port_en1);
    rval_eng_port_en2 = 0;
    COMPILER_64_ZERO(rval_cong_st_en0);
    COMPILER_64_ZERO(rval_cong_st_en1);
    rval_cong_st_en2 = 0;

    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_ing_port_en0, inst, 0, &rval_ing_port_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_ing_port_en1, inst, 0, &rval_ing_port_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                            reg_ing_port_en2, inst, 0, &rval_ing_port_en2));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_eng_port_en0, inst, 0, &rval_eng_port_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_eng_port_en1, inst, 0, &rval_eng_port_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                            reg_eng_port_en2, inst, 0, &rval_eng_port_en2));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_cong_st_en0, inst, 0, &rval_cong_st_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_get(unit,
                            reg_cong_st_en1, inst, 0, &rval_cong_st_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit,
                            reg_cong_st_en2, inst, 0, &rval_cong_st_en2));


    ing_port_en_field_val0 = soc_reg64_field_get(unit,
                                    reg_ing_port_en0, rval_ing_port_en0,
                                                        ING_PORT_ENf);
    ing_port_en_field_val1 = soc_reg64_field_get(unit,
                                    reg_ing_port_en1, rval_ing_port_en1,
                                                        ING_PORT_ENf);
    ing_port_en_field_val2 = soc_reg_field_get(unit,
                                    reg_ing_port_en2, rval_ing_port_en2,
                                                        ING_PORT_ENf);
    eng_port_en_field_val0 = soc_reg64_field_get(unit,
                                    reg_eng_port_en0, rval_eng_port_en0,
                                                        ENG_PORT_ENf);
    eng_port_en_field_val1 = soc_reg64_field_get(unit,
                                    reg_eng_port_en1, rval_eng_port_en1,
                                                        ENG_PORT_ENf);
    eng_port_en_field_val2 = soc_reg_field_get(unit,
                                    reg_eng_port_en2, rval_eng_port_en2,
                                                        ENG_PORT_ENf);
    cong_st_en_field_val0 = soc_reg64_field_get(unit,
                                    reg_cong_st_en0, rval_cong_st_en0,
                                                        CONGST_ST_ENf);
    cong_st_en_field_val1 = soc_reg64_field_get(unit,
                                    reg_cong_st_en1, rval_cong_st_en1,
                                                        CONGST_ST_ENf);
    cong_st_en_field_val2 = soc_reg_field_get(unit,
                                    reg_cong_st_en2, rval_cong_st_en2,
                                                        CONGST_ST_ENf);


    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {

            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            pipe_for_port_down = TH3_MMU_PIPE_NUM(mmu_port);

            if (pipe_for_port_down == pipe) {

                sal_memset(entry_oobport_mapping_thdi, 0,
                    sizeof(entry_oobport_mapping_thdi));
                sal_memset(entry_oobport_mapping_thdo, 0,
                    sizeof(entry_oobport_mapping_thdo));
                sal_memset(ing_oob_port_fldval, 0, sizeof(ing_oob_port_fldval));
                sal_memset(eng_oob_port_fldval, 0, sizeof(eng_oob_port_fldval));
                sal_memset(cng_oob_port_fldval, 0, sizeof(cng_oob_port_fldval));


                SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                        mem_oobport_mapping_thdi, MEM_BLOCK_ALL,
                                        dev_port, entry_oobport_mapping_thdi));
                soc_mem_field_get(unit, mem_oobport_mapping_thdi,
                                        entry_oobport_mapping_thdi, OOB_PORT_NUMf,
                                        ing_oob_port_fldval);

                SOC_IF_ERROR_RETURN(soc_mem_read(unit,
                                        mem_oobport_mapping_thdo, MEM_BLOCK_ALL,
                                        dev_port, entry_oobport_mapping_thdo));
                soc_mem_field_get(unit, mem_oobport_mapping_thdo,
                                        entry_oobport_mapping_thdo, OOB_PORT_NUMf,
                                        eng_oob_port_fldval);

                ing_oob_port = ing_oob_port_fldval[0];
                eng_oob_port = eng_oob_port_fldval[0];
                cng_oob_port = ing_oob_port_fldval[0];


                /* Reg0 is OOBPORT 0-63*/
                if (ing_oob_port < 64) {
                    COMPILER_64_BITCLR(ing_port_en_field_val0, ing_oob_port);
                }
                /* Reg1 is OOBPORT 64-127*/
                else if (ing_oob_port < 128) {
                    COMPILER_64_BITCLR(ing_port_en_field_val1, (ing_oob_port - 64));
                }
                /* Reg2 is OOBPORT 128-154*/
                else {
                    ing_port_en_field_val2 &= ~(1 << (ing_oob_port - 128));
                }


                /* Reg0 is OOBPORT 0-63*/
                if (eng_oob_port < 64) {
                    COMPILER_64_BITCLR(eng_port_en_field_val0, eng_oob_port);
                }
                /* Reg1 is OOBPORT 64-127*/
                else if (eng_oob_port < 128) {
                    COMPILER_64_BITCLR(eng_port_en_field_val1, (eng_oob_port - 64));
                }
                /* Reg2 is OOBPORT 128-154*/
                else {
                    eng_port_en_field_val2 &= ~(1 << (eng_oob_port - 128));
                }


                /* Reg0 is OOBPORT 0-63*/
                if (cng_oob_port < 64) {
                    COMPILER_64_BITCLR(cong_st_en_field_val0, cng_oob_port);
                }
                /* Reg1 is OOBPORT 64-127*/
                else if (cng_oob_port < 128) {
                    COMPILER_64_BITCLR(cong_st_en_field_val1, (cng_oob_port - 64));
                }
                /* Reg2 is OOBPORT 128-154*/
                else {
                    cong_st_en_field_val2 &= ~(1 << (cng_oob_port - 128));
                }


            }
        }
    }

    soc_reg64_field_set(unit, reg_ing_port_en0, &rval_ing_port_en0,
                                ING_PORT_ENf, ing_port_en_field_val0);
    soc_reg64_field_set(unit, reg_ing_port_en1, &rval_ing_port_en1,
                                ING_PORT_ENf, ing_port_en_field_val1);
    soc_reg_field_set(unit, reg_ing_port_en2, &rval_ing_port_en2,
                                ING_PORT_ENf, ing_port_en_field_val2);
    soc_reg64_field_set(unit, reg_eng_port_en0, &rval_eng_port_en0,
                                ENG_PORT_ENf, eng_port_en_field_val0);
    soc_reg64_field_set(unit, reg_eng_port_en1, &rval_eng_port_en1,
                                ENG_PORT_ENf, eng_port_en_field_val1);
    soc_reg_field_set(unit, reg_eng_port_en2, &rval_eng_port_en2,
                                ENG_PORT_ENf, eng_port_en_field_val2);
    soc_reg64_field_set(unit, reg_cong_st_en0, &rval_cong_st_en0,
                                CONGST_ST_ENf, cong_st_en_field_val0);
    soc_reg64_field_set(unit, reg_cong_st_en1, &rval_cong_st_en1,
                                CONGST_ST_ENf, cong_st_en_field_val1);
    soc_reg_field_set(unit, reg_cong_st_en2, &rval_cong_st_en2,
                                CONGST_ST_ENf, cong_st_en_field_val2);

    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_ing_port_en0, inst, 0, rval_ing_port_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_ing_port_en1, inst, 0, rval_ing_port_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                            reg_ing_port_en2, inst, 0, rval_ing_port_en2));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_eng_port_en0, inst, 0, rval_eng_port_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_eng_port_en1, inst, 0, rval_eng_port_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                            reg_eng_port_en2, inst, 0, rval_eng_port_en2));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_cong_st_en0, inst, 0, rval_cong_st_en0));
    SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit,
                            reg_cong_st_en1, inst, 0, rval_cong_st_en1));
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit,
                            reg_cong_st_en2, inst, 0, rval_cong_st_en2));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_ebqs_port_flush(int unit,
 *      soc_port_schedule_state_t *port_schedule_state_t, int pipe, uint32 set_val)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param set_val Value to be set to the EBQS port flush registers
 *  @brief API to Set the EBQS Port Flush registers
 */
int
soc_tomahawk3_mmu_ebqs_port_flush(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 enable_val;
    int port;
    int pipe_for_port_down;
    int dev_port, phy_port;
    int mmu_port, lcl_mmu_port;
    int inst;
    uint32 new_val;
    uint32 temp;

    reg = MMU_EBQS_PORT_FLUSHr;

    inst = pipe;

    /*READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    rval = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, inst, 0, &rval));
    enable_val = soc_reg_field_get(unit, reg, rval, ENABLEf);

    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port == -1) {

            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            pipe_for_port_down = TH3_MMU_PIPE_NUM(mmu_port);

            if (pipe_for_port_down == pipe) {

                new_val = 1;

                lcl_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

                temp = TH3_MMU_FLUSH_ON;

                new_val <<= lcl_mmu_port;


                if (set_val == temp) {
                    enable_val |= new_val;
                } else {
                    new_val = ~new_val;
                    enable_val &= new_val;
                }

                rval = 0;
                soc_reg_field_set(unit, reg, &rval, ENABLEf, enable_val);

            }
        }
    }

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_rqe_port_flush(int unit, int itm, int call_iter)
 *  @param unit Device number
 *  @param itm ITM number to flush
 *  @brief API to Set the RQE Snapshot register to Flush out packets in the
 *         RQE Replication FIFO
 */
int
soc_tomahawk3_mmu_rqe_port_flush(int unit, int itm, int call_iter)
{

    soc_reg_t reg_flush, reg_int_stat;
    uint32 rval;
    uint32 wr_val;
    int count = 0;
    uint32 rd_val;
    int inst;

    reg_flush    = MMU_RQE_QUEUE_SNAPSHOT_ENr;
    reg_int_stat = MMU_RQE_INT_STATr;

    inst = itm;

    /* Set MMU_RQE_QUEUE_FLUSH_EN.VALUE to 1 */

    wr_val = 1;
    rval = 0;
    soc_reg_field_set(unit, reg_flush, &rval, VALUEf, wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg_flush, inst, 0, rval));


    /* Wait for MMU_RQE_INT_STAT.FLUSH_DONE interrupt to be set */

    while (1) {
        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg_int_stat,
                                                    inst, 0, &rval));
        rd_val = soc_reg_field_get(unit, reg_int_stat,
                                    rval, FLUSH_DONEf);
        if (rd_val == 1) {
            break;
        }
        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 50000) {
            if (call_iter >= 2) {
                SOC_IF_ERROR_RETURN(
                    soc_tomahawk3_cpu_tx_rqe_queues_for_flush(unit, inst));
            } else {
                break;
            }
        }
    }

    /* Reset MMU_RQE_QUEUE_FLUSH_EN.VALUE to 0 */

    wr_val = 0;
    rval = 0;
    soc_reg_field_set(unit, reg_flush, &rval, VALUEf, wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg_flush, inst, 0, rval));


    /* Clear MMU_RQE_INT_STAT.FLUSH_DONE interrupt */

    wr_val = 1; /*w1tc*/
    rval = 0;
    soc_reg_field_set(unit, reg_int_stat, &rval, FLUSH_DONEf, wr_val);
    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg_int_stat, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_wait_ebctm_empty(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t, int pipe)
 *  @param unit Device number
 *  @param pipe Pipe number to check
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @brief API to poll MMU EBCTM Port Empty register
 */
int
soc_tomahawk3_mmu_wait_ebctm_empty(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe)
{
    soc_reg_t reg;
    uint32 rval;
    uint32 dst_port_empty_val;
    int count = 0;
    int port;
    int pipe_for_port_down;
    int dev_port, phy_port;
    int mmu_port, lcl_mmu_port;
    int inst;
    int port_empty, all_ports_empty;

    reg = MMU_EBCTM_PORT_EMPTY_STSr;

    inst = pipe;

    while (1) {

        rval = 0;

        SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, inst, 0, &rval));
        dst_port_empty_val = soc_reg_field_get(unit, reg, rval, DST_PORT_EMPTYf);

        all_ports_empty = 1;

        for (port = 0; port < port_schedule_state_t->nport; port++) {
            if (port_schedule_state_t->resource[port].physical_port == -1) {

                dev_port = port_schedule_state_t->resource[port].logical_port;
                phy_port =
                    port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
                mmu_port =
                    port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
                pipe_for_port_down = TH3_MMU_PIPE_NUM(mmu_port);

                if (pipe_for_port_down == pipe) {

                    lcl_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

                    port_empty =
                        ( ( dst_port_empty_val & (1 << lcl_mmu_port) ) != 0 );

                    all_ports_empty &= port_empty;

                    if (all_ports_empty == 0) {
                        break;
                    }

                }
            }
        }

        if (all_ports_empty == 1) {
            break;
        }

        sal_usleep(1 + (SAL_BOOT_QUICKTURN ? 1 : 0) * EMULATION_FACTOR);
        count++;
        if (count > 30000) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                           "WAIT_EBCTM_EMPTY : Ports not empty even after 30000 "
                           "iterations\n")));
            return SOC_E_FAIL;
        }

    }

    return SOC_E_NONE;
}




/*! @fn int soc_tomahawk3_mmu_mtro_port_metering_config(int unit,
 *      soc_port_schedule_state_t *port_schedule_state_t, int pipe, uint32 set_val_mtro)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule_State struct
 *  @param set_val_mtro Value to be set in the per-port DISABLE bitmap
 *  @brief This function is used to set the MTRO Port Entity Disable register to
 *         ignore shaper information on a port during flexport operation
 */
int
soc_tomahawk3_mmu_mtro_port_metering_config(int unit,
    soc_port_schedule_state_t *port_schedule_state_t,
    int pipe, uint32 set_val_mtro)
{

    soc_reg_t reg;
    uint32 rval;
    int port;
    int pipe_for_port;
    int dev_port, phy_port;
    int mmu_port, lcl_mmu_port;
    int inst;
    uint32 bitmap_val;

    reg = MMU_MTRO_PORT_ENTITY_DISABLEr;

    inst = pipe;

    /*READ MODIFY WRITE IN SW ... Hence get Register
       Value and Then Write ... */

    rval = 0;

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_get(unit, reg, inst, 0, &rval));
    bitmap_val = soc_reg_field_get(unit, reg, rval, METERING_DISABLEf);


    /* Loop through each Flex'd Port */
    for (port = 0; port < port_schedule_state_t->nport; port++) {

        /* First, get it's Pipe */
        if (port_schedule_state_t->resource[port].physical_port == -1) {
            /* Port Down */
            dev_port = port_schedule_state_t->resource[port].logical_port;
            phy_port =
                port_schedule_state_t->in_port_map.port_l2p_mapping[dev_port];
            mmu_port =
                port_schedule_state_t->in_port_map.port_p2m_mapping[phy_port];
            lcl_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

            pipe_for_port = TH3_MMU_PIPE_NUM(mmu_port);
        }
        else {
            /* Port Up */
            mmu_port = port_schedule_state_t->resource[port].mmu_port;
            lcl_mmu_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

            pipe_for_port = port_schedule_state_t->resource[port].pipe;
        }

        /* Now, Set or Clear disable bit depending on function input */
        if (pipe_for_port == pipe) {

            if (set_val_mtro == 1) { /* Disable refresh - on all UP/DOWN Ports */
                /* set the 'lcl_mmu_port' bit */
                uint32 temp;
                temp = 1;
                temp <<= lcl_mmu_port;
                bitmap_val |= temp;
            }
            else { /* Enable refresh */
                /* clear the 'lcl_mmu_port' bit */
                uint32 temp;
                temp = 1;
                temp <<= lcl_mmu_port;
                temp = ~temp;
                bitmap_val &= temp;
            }

        }

    }


    rval = 0;
    soc_reg_field_set(unit, reg, &rval, METERING_DISABLEf, bitmap_val);

    SOC_IF_ERROR_RETURN(soc_reg32_rawport_set(unit, reg, inst, 0, rval));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_clear_mtro_bucket_mems(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear MTRO bucket count memories to ensure that a port
 *         doesn't start out of profile in the new configuration.
 */
int
soc_tomahawk3_mmu_clear_mtro_bucket_mems(int unit,
    soc_port_resource_t *port_resource_t)
{
    int mmu_port;
    int pipe_number;
    uint32 entry_mtro_prt_bkt[SOC_MAX_MEM_WORDS];
    uint32 entry_mtro_l0_bkt[SOC_MAX_MEM_WORDS];
    uint32 mem_indx;
    int lcl_port;
    int cos;
    uint32 mem_fld;
    soc_mem_t mem_mtro_prt_bkt, mem_mtro_l0_bkt;


    mmu_port = port_resource_t->mmu_port;
    pipe_number = port_resource_t->pipe;
    lcl_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

    mem_mtro_prt_bkt = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                           MMU_MTRO_EGRMETERINGBUCKETm, pipe_number);
    mem_mtro_l0_bkt  = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                           MMU_MTRO_BUCKET_L0m, pipe_number);

    sal_memset(entry_mtro_prt_bkt, 0, sizeof(entry_mtro_prt_bkt));
    sal_memset(entry_mtro_l0_bkt, 0, sizeof(entry_mtro_l0_bkt));

    /* PORT BUCKET Memory */
    mem_indx = lcl_port;

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_prt_bkt, MEM_BLOCK_ALL,
                                    mem_indx, entry_mtro_prt_bkt));
    mem_fld = 0;
    soc_mem_field_set(unit, mem_mtro_prt_bkt, entry_mtro_prt_bkt, BUCKETf,
                        &mem_fld);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_prt_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_prt_bkt));

    /* L0 Bucket Memory */
    for (cos = 0; cos < TH3_MMU_NUM_COS; cos++) {

        mem_indx = (lcl_port * TH3_MMU_NUM_COS) + cos;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem_mtro_l0_bkt, MEM_BLOCK_ALL,
                                        mem_indx, entry_mtro_l0_bkt));
        mem_fld = 0;
        soc_mem_field_set(unit, mem_mtro_l0_bkt, entry_mtro_l0_bkt, MIN_BUCKETf,
                          &mem_fld);
        soc_mem_field_set(unit, mem_mtro_l0_bkt, entry_mtro_l0_bkt, MAX_BUCKETf,
                          &mem_fld);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem_mtro_l0_bkt, MEM_BLOCK_ALL,
                                          mem_indx, entry_mtro_l0_bkt));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_thdi_bst_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear port/PG hdrm and shared BST counters for a given MMU
 *         source port
 */
int
soc_tomahawk3_mmu_thdi_bst_clr(int unit, soc_port_resource_t *port_resource_t)
{
    /*Variables*/
    int local_mmu_port;
    int pipe_number;
    uint32 data[SOC_MAX_MEM_WORDS];

    /*Memories*/
    soc_mem_t mmu_thdi_port_pg_bst;
    soc_mem_t mmu_thdi_port_sp_bst;
    soc_mem_t mmu_thdi_port_pg_hdrm_bst;

    /*Set up pipe & port information*/
    pipe_number = port_resource_t->pipe;
    local_mmu_port = (port_resource_t->mmu_port) % TH3_MMU_PORT_PIPE_OFFSET;

    sal_memset(data, 0, sizeof(data));



    mmu_thdi_port_pg_bst = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                    MMU_THDI_PORT_PG_SHARED_BSTm, pipe_number);
    mmu_thdi_port_sp_bst = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                    MMU_THDI_PORTSP_BSTm, pipe_number);
    mmu_thdi_port_pg_hdrm_bst = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                    MMU_THDI_PORT_PG_HDRM_BSTm, pipe_number);

    /*Writing 0 to all fields*/
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdi_port_pg_bst,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        data));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdi_port_pg_hdrm_bst,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        data));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_thdi_port_sp_bst,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        data));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_thdi_setup(int unit,
                soc_port_resource_t *port_resource_t, int lossy)
 *  @param unit Device number
 *  @param *port_resource_t Port resource struct
 *  @param lossy If the thresholds are to be reconfigured in lossy
 *          or lossless mode
 *  @brief API to reset THDI port-PG thresholds for a given source port
 */
int
soc_tomahawk3_mmu_thdi_setup(int unit, soc_port_resource_t *port_resource_t,
    int lossy)
{
    /*Variables*/
    int local_mmu_port;
    int pg;
    int pipe;
    uint32 pg_min_setting[TH3_MMU_NUM_PGS];
    uint32 pg_hdrm_setting[TH3_MMU_NUM_PGS];


    /*Memories*/
    soc_mem_t thdi_port_pg_min_config;
    soc_mem_t thdi_port_pg_hdrm_config;
    uint32    port_pg_min_config_data[SOC_MAX_MEM_WORDS];
    uint32    port_pg_hdrm_config_data[SOC_MAX_MEM_WORDS];

    pipe           = port_resource_t->pipe;
    local_mmu_port = (port_resource_t->mmu_port) % TH3_MMU_PORT_PIPE_OFFSET;

    thdi_port_pg_min_config = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                              MMU_THDI_PORT_PG_MIN_CONFIGm, pipe);
    thdi_port_pg_hdrm_config = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                              MMU_THDI_PORT_PG_HDRM_CONFIGm, pipe);

    sal_memset(port_pg_min_config_data, 0, sizeof(port_pg_min_config_data));
    sal_memset(port_pg_hdrm_config_data, 0, sizeof(port_pg_hdrm_config_data));


    for (pg = 0; pg < TH3_MMU_NUM_PGS; pg++) {
        if (pg != TH3_MMU_PAUSE_PG) {
            pg_min_setting[pg]  = TH3_MMU_THDI_PG_MIN_LIMIT_LOSSY;
            pg_hdrm_setting[pg] = TH3_MMU_THDI_PG_HDRM_LIMIT_LOSSY;
        }
        else {

            pg_min_setting[pg] =
                (!lossy) ? TH3_MMU_THDI_PG_MIN_LIMIT_LOSSLESS :
                            TH3_MMU_THDI_PG_MIN_LIMIT_LOSSY;
            if (!lossy) {
                SOC_IF_ERROR_RETURN(
                    soc_tomahawk3_mmu_thdi_get_pg_hdrm_setting(unit,
                        port_resource_t->speed,
                        (1 - port_resource_t->oversub),
                        &pg_hdrm_setting[pg]));
            } else {
                pg_hdrm_setting[pg] = TH3_MMU_THDI_PG_HDRM_LIMIT_LOSSY;
            }

        }
    }

    /* MIN */

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdi_port_pg_min_config,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        port_pg_min_config_data));
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG7_MIN_LIMITf, &pg_min_setting[7]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG6_MIN_LIMITf, &pg_min_setting[6]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG5_MIN_LIMITf, &pg_min_setting[5]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG4_MIN_LIMITf, &pg_min_setting[4]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG3_MIN_LIMITf, &pg_min_setting[3]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG2_MIN_LIMITf, &pg_min_setting[2]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG1_MIN_LIMITf, &pg_min_setting[1]);
    soc_mem_field_set(unit, thdi_port_pg_min_config, port_pg_min_config_data,
                        PG0_MIN_LIMITf, &pg_min_setting[0]);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdi_port_pg_min_config,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        port_pg_min_config_data));


    /* HRDM */

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdi_port_pg_hdrm_config,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        port_pg_hdrm_config_data));
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG7_HDRM_LIMITf, &pg_hdrm_setting[7]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG6_HDRM_LIMITf, &pg_hdrm_setting[6]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG5_HDRM_LIMITf, &pg_hdrm_setting[5]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG4_HDRM_LIMITf, &pg_hdrm_setting[4]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG3_HDRM_LIMITf, &pg_hdrm_setting[3]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG2_HDRM_LIMITf, &pg_hdrm_setting[2]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG1_HDRM_LIMITf, &pg_hdrm_setting[1]);
    soc_mem_field_set(unit, thdi_port_pg_hdrm_config, port_pg_hdrm_config_data,
                        PG0_HDRM_LIMITf, &pg_hdrm_setting[0]);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdi_port_pg_hdrm_config,
                                        MEM_BLOCK_ANY, local_mmu_port,
                                        port_pg_hdrm_config_data));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_thdo_bst_clr_queue(int unit,
 *                                   int pipe, int global_mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_mmu_port global mmu port
 *  @brief Clears thdo bst total queue counters of the port
 */
int
soc_tomahawk3_mmu_thdo_bst_clr_queue(int unit, int pipe, int global_mmu_port)
{
    int local_mmu_port;
    int itm;
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_total_queue_bst_mem;

    sal_memset(data, 0, sizeof(data));

    local_mmu_port = global_mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    thdo_total_queue_bst_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                MMU_THDO_BST_TOTAL_QUEUEm, itm);

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "In soc_tomahawk3_mmu_thdo_bst_clr_queue :"
                " pipe = %0d, global_mmu_port = %0d\n"),
                pipe, global_mmu_port));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_num_queues_for_port(unit,
                                        pipe, local_mmu_port, &num_queues));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_chip_q_num_base_for_port(unit,
                                 pipe, local_mmu_port, &chip_q_num_base_for_port));

    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++)
    {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "In soc_tomahawk3_mmu_thdo_bst_clr_queue :"
                    " curr_idx = %0d, mem_idx = %0d\n"),
                    curr_idx, mem_idx));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_total_queue_bst_mem,
                                            MEM_BLOCK_ANY, mem_idx, data));
        mem_idx++;
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_thdo_bst_clr_port(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port counters of the port
 */
int
soc_tomahawk3_mmu_thdo_bst_clr_port(int unit, int pipe, int global_device_port)
{
    int itm;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_bst_shared_port_mem;

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    thdo_bst_shared_port_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                    MMU_THDO_BST_SHARED_PORTm, itm);

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_bst_shared_port_mem,
                            MEM_BLOCK_ANY, global_device_port, data));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_thdo_bst_clr_portsp(int unit,
 *                                   int pipe, int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe in which port exists
 *  @param int global_device_port global device port
 *  @brief Clears thdo bst port/sp counters of the port
 */
int
soc_tomahawk3_mmu_thdo_bst_clr_portsp(int unit, int pipe, int global_device_port)
{
    int itm;
    int sp;
    int mem_idx;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_bst_shared_portsp_mc_mem;

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    thdo_bst_shared_portsp_mc_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                        MMU_THDO_BST_SHARED_PORTSP_MCm, itm);

    for (sp = 0; sp < TH3_MMU_NUM_SPS; sp++)
    {
        mem_idx = (global_device_port * 4) + sp;
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_bst_shared_portsp_mc_mem,
                                MEM_BLOCK_ANY, mem_idx, data));
    }

    return SOC_E_NONE;
}




/*! @fn int soc_tomahawk3_mmu_thdo_bst_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
           epipe and port info
 *  @brief Clears thdo queue/port bst counters of an egress port
 */
int
soc_tomahawk3_mmu_thdo_bst_clr(int unit, soc_port_resource_t *port_resource_t)
{
    int global_mmu_port;
    int global_device_port;
    int pipe;

    pipe = port_resource_t->pipe;
    global_mmu_port = port_resource_t->mmu_port;
    global_device_port = port_resource_t->logical_port;

    /* Clear per-queue memories */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_thdo_bst_clr_queue(unit, pipe,
                                                        global_mmu_port));
    /* Clear per-port memories */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_thdo_bst_clr_port(unit, pipe,
                                                        global_device_port));
    /* Clear per-port-per-spid memories */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_thdo_bst_clr_portsp(unit, pipe,
                                                        global_device_port));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_thdo_pktstat_clr_queue(int unit, int pipe,
 *              int global_mmu_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_mmu_port Global mmu port
 *  @brief Internal function. Clears thdo stats counters of all queues
 *         in the egress port
 */
int
soc_tomahawk3_mmu_thdo_pktstat_clr_queue(int unit, int pipe, int global_mmu_port)
{
    int local_mmu_port;
    int itm;
    int curr_idx;
    int num_queues;
    int mem_idx;
    int chip_q_num_base_for_port;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_total_queue_bst_mem;

    sal_memset(data, 0, sizeof(data));

    local_mmu_port = global_mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    thdo_total_queue_bst_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                MMU_THDO_QUEUE_DROP_COUNTm, itm);

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_num_queues_for_port(unit,
                                        pipe, local_mmu_port, &num_queues));

    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "In soc_tomahawk3_mmu_thdo_pktstat_clr_queue :"
                " pipe = %0d, global_mmu_port = %0d\n"), pipe, global_mmu_port));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_chip_q_num_base_for_port(unit,
                                 pipe, local_mmu_port, &chip_q_num_base_for_port));

    mem_idx = chip_q_num_base_for_port;

    for (curr_idx = 0; curr_idx < num_queues; curr_idx++)
    {
        LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "In soc_tomahawk3_mmu_thdo_pktstat_clr_queue :"
                    " curr_idx = %0d, mem_idx = %0d\n"), curr_idx, mem_idx));

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_total_queue_bst_mem,
                                MEM_BLOCK_ANY, mem_idx, data));
        mem_idx++;
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_thdo_pktstat_clr_port(int unit, int pipe,
 *              int global_device_port)
 *  @param int unit Chip unit number
 *  @param int pipe Egress pipe
 *  @param int global_device_port Global device port
 *  @brief Internal function. Clears thdo stats counters of
 *         the egress ports
 */
int
soc_tomahawk3_mmu_thdo_pktstat_clr_port(int unit, int pipe,
    int global_device_port)
{
    int itm;
    int local_device_port;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_src_port_drop_count_mem;
    soc_mem_t thdo_port_drop_count_mc_mem;
    soc_mem_t thdo_port_drop_count_uc_mem;

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    thdo_src_port_drop_count_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                        MMU_THDO_SRC_PORT_DROP_COUNTm, pipe);
    thdo_port_drop_count_mc_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                        MMU_THDO_PORT_DROP_COUNT_MCm, itm);
    thdo_port_drop_count_uc_mem = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                        MMU_THDO_PORT_DROP_COUNT_UCm, itm);

    local_device_port = global_device_port % TH3_MMU_NUM_PORTS_PER_PIPE_L;

    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_src_port_drop_count_mem,
                            MEM_BLOCK_ANY, local_device_port, data));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_port_drop_count_mc_mem,
                            MEM_BLOCK_ANY, global_device_port, data));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_port_drop_count_uc_mem,
                            MEM_BLOCK_ANY, global_device_port, data));

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_mmu_thdo_pktstat_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 *         epipe and port info
 *  @brief Clears thdo stats counters of all queues/ports in the egress port
 */
int
soc_tomahawk3_mmu_thdo_pktstat_clr(int unit,
    soc_port_resource_t *port_resource_t)
{
    int global_mmu_port;
    int global_device_port;
    int pipe;

    pipe = port_resource_t->pipe;
    global_mmu_port = port_resource_t->mmu_port;
    global_device_port = port_resource_t->logical_port;

    /* Clear per-queue memories */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_thdo_pktstat_clr_queue(unit, pipe,
                                                          global_mmu_port));
    /* Clear per-port memories */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_thdo_pktstat_clr_port(unit, pipe,
                                                          global_device_port));

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_thdo_config(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param int unit Chip unit number
 *  @param soc_port_resource_t *port_resource_t Port resource struct containing
 *             epipe and port info
 *  @brief Program THDO for new ports
 */
int
soc_tomahawk3_mmu_thdo_config(
    int unit, soc_port_resource_t *port_resource_t)
{
    uint32 uc_qgrp_min_limit;
    uint32 mc_qgrp_min_limit;
    uint32 qgroup_valid;
    uint32 q_spid;
    int global_mmu_port, local_mmu_port;
    int pipe;
    int num_queues;
    int chip_q_num_base_for_port;
    int q_idx;
    int chip_port_num, mem_index;
    uint32 data[SOC_MAX_MEM_WORDS];
    soc_mem_t thdo_config_uc_qgrp0_mem;
    soc_mem_t thdo_config_uc_qgrp1_mem;
    soc_mem_t thdo_config_uc_qgrp2_mem;
    soc_mem_t thdo_config_mc_qgrp_mem;
    soc_mem_t thdo_port_queue_service_pool_mem;
    soc_mem_t thdo_q_to_qgrp_mapd0_mem;
    soc_mem_t thdo_queue_config_mem;

    thdo_config_uc_qgrp0_mem = MMU_THDO_CONFIG_UC_QGROUP0m;
    thdo_config_uc_qgrp1_mem = MMU_THDO_CONFIG_UC_QGROUP1m;
    thdo_config_uc_qgrp2_mem = MMU_THDO_CONFIG_UC_QGROUP2m;
    thdo_config_mc_qgrp_mem  = MMU_THDO_CONFIG_MC_QGROUPm;
    thdo_port_queue_service_pool_mem = MMU_THDO_PORT_QUEUE_SERVICE_POOLm;
    thdo_q_to_qgrp_mapd0_mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
    thdo_queue_config_mem = MMU_THDO_QUEUE_CONFIGm;

    uc_qgrp_min_limit        = soc_th3_mmu_thdo_cfg_tbl[0].qgrp_cfg_min_limit;
    mc_qgrp_min_limit        = soc_th3_mmu_thdo_cfg_tbl[0].qgrp_cfg_min_limit;
    qgroup_valid             = soc_th3_mmu_thdo_cfg_tbl[0].qgroup_valid;
    q_spid                   = soc_th3_mmu_thdo_cfg_tbl[0].q_spid;

    pipe = port_resource_t->pipe;
    global_mmu_port = port_resource_t->mmu_port;
    local_mmu_port = global_mmu_port % TH3_MMU_PORT_PIPE_OFFSET;

    /* THDO uses Chip Port Num */
    chip_port_num = (pipe * TH3_MMU_NUM_PORTS_PER_PIPE_L) + local_mmu_port;


    /* Per Port Memories */
    mem_index = chip_port_num;

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_config_uc_qgrp0_mem,
                            MEM_BLOCK_ANY, mem_index, data));
    soc_mem_field_set(unit, thdo_config_uc_qgrp0_mem, data,
                            MIN_LIMITf, &uc_qgrp_min_limit);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_config_uc_qgrp0_mem,
                            MEM_BLOCK_ANY, mem_index, data));

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_config_uc_qgrp1_mem,
                            MEM_BLOCK_ANY, mem_index, data));
    soc_mem_field_set(unit, thdo_config_uc_qgrp1_mem, data,
                            MIN_LIMITf, &uc_qgrp_min_limit);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_config_uc_qgrp1_mem,
                            MEM_BLOCK_ANY, mem_index, data));

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_config_uc_qgrp2_mem,
                            MEM_BLOCK_ANY, mem_index, data));
    soc_mem_field_set(unit, thdo_config_uc_qgrp2_mem, data,
                            MIN_LIMITf, &uc_qgrp_min_limit);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_config_uc_qgrp2_mem,
                            MEM_BLOCK_ANY, mem_index, data));

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_config_mc_qgrp_mem,
                            MEM_BLOCK_ANY, mem_index, data));
    soc_mem_field_set(unit, thdo_config_mc_qgrp_mem, data,
                            MIN_LIMITf, &mc_qgrp_min_limit);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_config_mc_qgrp_mem,
                            MEM_BLOCK_ANY, mem_index, data));

    sal_memset(data, 0, sizeof(data));

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_port_queue_service_pool_mem,
                            MEM_BLOCK_ANY, mem_index, data));
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE0_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE1_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE2_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE3_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE4_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE5_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE6_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE7_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE8_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE9_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE10_SPIDf, &q_spid);
    soc_mem_field_set(unit, thdo_port_queue_service_pool_mem, data,
                            QUEUE11_SPIDf, &q_spid);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_port_queue_service_pool_mem,
                            MEM_BLOCK_ANY, mem_index, data));



    /* Per Queue Memories */

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_num_queues_for_port(unit,
                                        pipe, local_mmu_port, &num_queues));

    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_get_chip_q_num_base_for_port(unit,
                                 pipe, local_mmu_port, &chip_q_num_base_for_port));

    for (q_idx = 0; q_idx < num_queues; q_idx++)
    {
        mem_index = chip_q_num_base_for_port + q_idx;

        sal_memset(data, 0, sizeof(data));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_q_to_qgrp_mapd0_mem,
                                MEM_BLOCK_ANY, mem_index, data));
        soc_mem_field_set(unit, thdo_q_to_qgrp_mapd0_mem, data,
                                QGROUP_VALIDf, &qgroup_valid);
        soc_mem_field_set(unit, thdo_q_to_qgrp_mapd0_mem, data,
                                Q_SPIDf, &q_spid);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_q_to_qgrp_mapd0_mem,
                                MEM_BLOCK_ANY, mem_index, data));

        sal_memset(data, 0, sizeof(data));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, thdo_queue_config_mem,
                                MEM_BLOCK_ANY, mem_index, data));
        soc_mem_field_set(unit, thdo_queue_config_mem, data,
                                USE_QGROUP_MINf, &qgroup_valid);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, thdo_queue_config_mem,
                                MEM_BLOCK_ANY, mem_index, data));

    }


    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_clear_qsch_credit_memories(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear QSCH credit memories and accum memories for the
 *         ports that are flexing during flexport.
 */
int
soc_tomahawk3_mmu_clear_qsch_credit_memories(
    int unit,soc_port_resource_t *port_resource_t)
{
    int itm;
    int mmu_port;
    int lcl_port;
    int pipe_number;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_cr_l0[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_cr_l1[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_cr_l2[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_acc_l0[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_acc_l1[SOC_MAX_MEM_WORDS];
    uint32 entry_qsch_acc_l2[SOC_MAX_MEM_WORDS];
    uint32 data[SOC_MAX_MEM_WORDS];

    uint32 mem_indx;
    int cos, schq;
    soc_mem_t qsch_l0_credit_mem;
    soc_mem_t qsch_l1_credit_mem;
    soc_mem_t qsch_l2_credit_mem;

    soc_mem_t qsch_l0_accum_comp_mem;
    soc_mem_t qsch_l1_accum_comp_mem;
    soc_mem_t qsch_l2_accum_comp_mem;


    mmu_port = port_resource_t->mmu_port;
    lcl_port = mmu_port % TH3_MMU_PORT_PIPE_OFFSET;
    pipe_number = port_resource_t->pipe;

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe_number, &itm));


    qsch_l0_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                            MMU_QSCH_L0_CREDIT_MEMm, pipe_number);
    qsch_l1_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                            MMU_QSCH_L1_CREDIT_MEMm, pipe_number);
    qsch_l2_credit_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                            MMU_QSCH_L2_CREDIT_MEMm, pipe_number);

    qsch_l0_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                MMU_QSCH_L0_ACCUM_COMP_MEMm, pipe_number);
    qsch_l1_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                MMU_QSCH_L1_ACCUM_COMP_MEMm, pipe_number);
    qsch_l2_accum_comp_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit,
                                MMU_QSCH_L2_ACCUM_COMP_MEMm, pipe_number);

    sal_memset(entry_qsch_cr_l0, 0, sizeof(entry_qsch_cr_l0));
    sal_memset(entry_qsch_cr_l1, 0, sizeof(entry_qsch_cr_l1));
    sal_memset(entry_qsch_cr_l2, 0, sizeof(entry_qsch_cr_l2));
    sal_memset(entry_qsch_acc_l0, 0, sizeof(entry_qsch_acc_l0));
    sal_memset(entry_qsch_acc_l1, 0, sizeof(entry_qsch_acc_l1));
    sal_memset(entry_qsch_acc_l2, 0, sizeof(entry_qsch_acc_l2));

    sal_memset(entry, 0, sizeof(entry));

    sal_memset(data, 0, sizeof(data));


    /*Writing 0 to all fields*/

    for (cos = 0; cos < TH3_MMU_NUM_COS; cos++) {

        mem_indx = (lcl_port * TH3_MMU_NUM_COS) + cos;

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l0_credit_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l0_accum_comp_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
    }

    for (schq = 0; schq < TH3_MMU_NUM_L0_NODES_PER_HSP_PORT; schq++) {

        mem_indx = (lcl_port * TH3_MMU_NUM_L0_NODES_PER_HSP_PORT) + schq;

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l1_credit_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l1_accum_comp_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
    }

    for (schq = 0; schq < TH3_MMU_NUM_L0_NODES_PER_HSP_PORT; schq++) {

        mem_indx =
            (((lcl_port * TH3_MMU_NUM_L0_NODES_PER_HSP_PORT) + schq) * 2) + itm;

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l2_credit_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, qsch_l2_accum_comp_mem,
                                MEM_BLOCK_ANY, mem_indx, data));
    }

    return SOC_E_NONE;
}

/*! @fn int soc_tomahawk3_mmu_wred_clr(int unit,
 *              soc_port_resource_t *port_resource_t)
 *  @param unit Device number
 *  @param *port_resource_t Port Resource Struct
 *  @brief API to clear the WRED memories to clear stale counts for new ports

 */
int
soc_tomahawk3_mmu_wred_clr(int unit, soc_port_resource_t *port_resource_t)
{
    soc_mem_t mmu_wred_avg_qsize, mmu_wred_avg_portsp_size;
    int cosq_mem_index, portsp_mem_index;
    uint32 wred_q_data[SOC_MAX_MEM_WORDS];
    uint32 wred_portsp_data[SOC_MAX_MEM_WORDS];
    int q, sp;
    int global_device_port;
    int pipe, itm;
    uint32 mem_data;

    /* Calculate base indexes for the set of UC queues and
      port SPs for the given port
     */
    global_device_port = port_resource_t->logical_port;

    pipe = port_resource_t->pipe;

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_mmu_get_itm_from_pipe_num(unit, pipe, &itm));

    sal_memset(wred_q_data, 0, sizeof(wred_q_data));
    sal_memset(wred_portsp_data, 0, sizeof(wred_portsp_data));

    mmu_wred_avg_portsp_size = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                MMU_WRED_AVG_PORTSP_SIZEm, itm);
    mmu_wred_avg_qsize = SOC_MEM_UNIQUE_ACC_ITM(unit,
                                MMU_WRED_AVG_QSIZEm, itm);

    mem_data = 0;

    /*Per-UCQ loop*/
    for (q = 0; q < TH3_MMU_NUM_Q_PER_HSP_PORT; q++) {

        cosq_mem_index = (global_device_port * TH3_MMU_NUM_Q_PER_HSP_PORT) + q;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_wred_avg_qsize,
                                MEM_BLOCK_ANY, cosq_mem_index,
                                wred_q_data));
        soc_mem_field_set(unit, mmu_wred_avg_qsize, wred_q_data,
                                AVG_QSIZE_FRACTIONf, &mem_data);
        soc_mem_field_set(unit, mmu_wred_avg_qsize, wred_q_data,
                                AVG_QSIZEf, &mem_data);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_wred_avg_qsize,
                                MEM_BLOCK_ANY, cosq_mem_index,
                                wred_q_data));
    }

    /*Per-SP loop*/
    for (sp = 0; sp < TH3_MMU_NUM_SPS; sp++) {

        portsp_mem_index = (global_device_port * TH3_MMU_NUM_SPS) + sp;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mmu_wred_avg_portsp_size,
                                MEM_BLOCK_ANY, portsp_mem_index,
                                wred_portsp_data));
        soc_mem_field_set(unit, mmu_wred_avg_portsp_size, wred_portsp_data,
                                AVG_QSIZE_FRACTIONf, &mem_data);
        soc_mem_field_set(unit, mmu_wred_avg_portsp_size, wred_portsp_data,
                                AVG_QSIZEf, &mem_data);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mmu_wred_avg_portsp_size,
                                MEM_BLOCK_ANY, portsp_mem_index,
                                wred_portsp_data));
    }

    return SOC_E_NONE;
}


/*! @fn int soc_tomahawk3_flex_mmu_reconfigure_phase2(int unit,
 *              soc_port_schedule_state_t *port_schedule_state_t)
 *  @param unit Device number
 *  @param *port_schedule_state_t Port Schedule Struct
 *  @brief API to update all MMU related functionalities during flexport
 *         port up operation.
 */
int
soc_tomahawk3_flex_mmu_reconfigure_phase2(int unit,
    soc_port_schedule_state_t *port_schedule_state_t)
{
    int port, pipe, pipe_flexed;
    int logical_port;
    int lossy;
    int thdi_bst_mode;
    int thdo_bst_mode;

    lossy = !(port_schedule_state_t->lossless);

    soc_tomahawk3_mmu_get_thdi_bst_mode(unit, &thdi_bst_mode);
    soc_tomahawk3_mmu_get_thdo_bst_mode(unit, &thdo_bst_mode);

    /* Per-Pipe configuration */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
            soc_tomahawk3_mmu_mtro_port_metering_config(unit,
                    port_schedule_state_t, pipe, TH3_MMU_FLUSH_ON);
        }
    }


    /* Per-Port configuration */
    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_crb_reconfig\n")));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_crb_reconfig(unit,
                                    &port_schedule_state_t->resource[port]));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_scb_reconfig\n")));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_scb_reconfig(unit,
                                    &port_schedule_state_t->resource[port]));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_oqs_reconfig\n")));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_oqs_reconfig(unit,
                                    &port_schedule_state_t->resource[port]));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_ebpcc_reconfig\n")));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_ebpcc_reconfig(unit,
                                    &port_schedule_state_t->resource[port]));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_eb_sch_reconfig\n")));
            SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_eb_sch_reconfig(unit,
                                    &port_schedule_state_t->resource[port]));

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_clear_mtro_bucket_mems\n")));
            /* Clear MTRO bucket memories */
            soc_tomahawk3_mmu_clear_mtro_bucket_mems(unit,
                            &port_schedule_state_t->resource[port]);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_clear_qsch_credit_memories\n")));
            /* Clear QSCH credit memories*/
            soc_tomahawk3_mmu_clear_qsch_credit_memories(unit,
                            &port_schedule_state_t->resource[port]);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_wred_clr\n")));
            /* Clear WRED Avg_Qsize instead of waiting for background process*/
            soc_tomahawk3_mmu_wred_clr(unit,
                            &port_schedule_state_t->resource[port]);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_thdi_setup\n")));
            soc_tomahawk3_mmu_thdi_setup(unit,
                            &port_schedule_state_t->resource[port], lossy);

            LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                        "before soc_tomahawk3_mmu_thdo_config\n")));
            soc_tomahawk3_mmu_thdo_config(unit,
                            &port_schedule_state_t->resource[port]);

            if (thdi_bst_mode == TH3_MMU_BST_WMARK_MODE) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "before soc_tomahawk3_mmu_thdi_bst_clr\n")));
                    /* Clear THDI BST in watermark mode..*/
                    soc_tomahawk3_mmu_thdi_bst_clr(unit,
                            &port_schedule_state_t->resource[port]);
            }
            if (thdo_bst_mode == TH3_MMU_BST_WMARK_MODE) {
                    LOG_DEBUG(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                "before soc_tomahawk3_mmu_thdo_bst_clr\n")));
                    /* Clear THDO BST counters in watermark mode*/
                    soc_tomahawk3_mmu_thdo_bst_clr(unit,
                            &port_schedule_state_t->resource[port]);
                    /* Clear PktStat counters in THDO for Queues*/
                    soc_tomahawk3_mmu_thdo_pktstat_clr(unit,
                            &port_schedule_state_t->resource[port]);
            }

        }
    }

    /* EBCTM settings for each Port Up */
    for (port = 0; port < port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            logical_port = port_schedule_state_t->resource[port].logical_port;

            soc_tomahawk3_mmu_ebctm_reconfig(
                    unit, &port_schedule_state_t->resource[port],
                    port_schedule_state_t->cutthru_prop.asf_modes[logical_port],
                    port_schedule_state_t->resource[port].mmu_port);
        }
    }


    /* Per-Port configuration : Port mappings for Port Up */
    for (port=0; port<port_schedule_state_t->nport; port++) {
        if (port_schedule_state_t->resource[port].physical_port != -1) {
            soc_tomahawk3_mmu_set_mmu_port_mappings(
                unit, port_schedule_state_t, &port_schedule_state_t->resource[port]);
        }
    }


    /* Per-Pipe configuration */
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        soc_tomahawk3_mmu_get_pipe_flexed_status(
            unit, port_schedule_state_t, pipe, &pipe_flexed);

        if (pipe_flexed == 1) {
            soc_tomahawk3_mmu_mtro_port_metering_config(unit,
                    port_schedule_state_t, pipe, 0);
        }
    }


    return SOC_E_NONE;
}



/*** END SDK API COMMON CODE ***/


#endif /* BCM_TOMAHAWK3_SUPPORT */

