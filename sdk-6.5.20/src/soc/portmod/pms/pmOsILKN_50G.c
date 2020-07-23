/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/wb_engine.h>
        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM_OS_ILKN_50G_SUPPORT

#include <soc/portmod/pmOsILKN_50G.h>
#include <soc/portmod/pmOsILKN_shared.h>

#define OS_ILKN_BURST_SHORT_32B_VAL                   (0)
#define OS_ILKN_BURST_SHORT_64B_VAL                   (1)
#define OS_ILKN_BURST_SHORT_96B_VAL                   (3)
#define OS_ILKN_BURST_SHORT_128B_VAL                  (6)

#define OS_ILKN_FABRIC_PHY_BASE                       (96)
#define OS_ILKN_NOF_FEC_INSTANCE                      (6)

#define OS_ILKN_PM_NOT_FOUND    (-1)

#define OS_ILKN_PHY_ADDR_PAIRS_IS_CROSS(a, b)    ((a) == (b) + 1)

extern soc_reg_t remap_lanes_rx_regs[];
extern soc_reg_t remap_lanes_tx_regs[];
extern soc_field_t remap_lanes_rx_fields[];
extern soc_field_t remap_lanes_tx_fields[];


STATIC int _pmOsILKN_50G_port_fec_enable_get(int unit, int port, pm_info_t pm_info, int *fec_enable, int *fec_bypass_enable);

/**
 * \brief - get number of pipes to configure ilkn core based on OS formula
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM information
 * \param [out] num_pipes - number of pipes
 *
 * \return
 *   STATIC int - see SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_port_num_pipes_get(int unit, int port, pm_info_t pm_info, uint32* num_pipes)
{
    pmOsIlkn_internal_t* ilkn_data;
    portmod_speed_config_t speed_config;
    int nof_lanes, serdes_speed=-1, core_id, i, is_pm_aggregated=0;
    uint32 core_clk_khz;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    core_clk_khz = ilkn_data->core_clk_khz;

    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    _SOC_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config));

    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(
                    __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_speed_config_get(unit, port, ilkn_data->pms[i], &speed_config));
            break;
        }
    }

    serdes_speed = speed_config.speed;

    /* pipe number formula by OS */
    *num_pipes = PORTMOD_DIV_ROUND_UP( (10 * serdes_speed * nof_lanes ) , ( (core_clk_khz / 100) * 67) );

    if (*num_pipes > OS_ILKN_SLE_MAX_NOF_PIPES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("pipe number exceeded limit (%d)"), *num_pipes));
    }

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - ILKN retransmit Configuration. Static method to be 
 *        called only from port attach / detach methods, and
 *        only when retransmit is enabled.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] pm_info - pm specific DB
 * \param [in] rx_retransmit - is retransmit enabled on RX side
 * \param [in] tx_retransmit - is retransmit enabled on TX side
 * \param [in] reserved_channel_rx - channel reserved for RX 
 *        retransmit
 * \param [in] reserved_channel_tx - channel reserved for TX 
 *        retransmit
 *   
 * \return
 *   STATIC int - see _SHR_E_ *
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_retransmit_config_set(int unit, int port, pm_info_t pm_info, uint32 rx_retransmit, uint32 tx_retransmit, uint32 reserved_channel_rx, uint32 reserved_channel_tx)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_USERf, rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_USERf, tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val)); 

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_RETRANSMIT_CONFIGr, &reg_val, SLE_RX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_RETRANSMIT_CONFIGr, &reg_val, SLE_TX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_RX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, reserved_channel_rx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_TX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, reserved_channel_tx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;

}

/**
 * \brief - set ILKN enabled lanes to HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enabled_lanes - uint32 to represent enabled lanes 
 *        bitmap.
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_enabled_lanes_set(int unit, int port, uint32 enabled_rx_serdes, uint32 enabled_tx_serdes)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* The register has reverse logic  - so we negate the lanes bitmap */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_LANEr, &reg_val, SLE_TX_LANE_DISABLEf, ~enabled_tx_serdes);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_LANEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_LANEr, &reg_val, SLE_RX_LANE_DISABLEf, ~enabled_rx_serdes);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_LANEr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Stall selection configuration for ILKN. stall 
 *        selection tells the ILKN core to sync all lanes
 *        according to a specific lane. if it is enabled it is
 *        suggested to set to the last enabled lane - it can
 *        help prevent deskew problems
 * 
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] stall_en - enable stalling according to specific 
 *        lane
 * \param [in] lane - which lane should be used for stalling
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOSILKN_50G_ilkn_stall_selection_set(int unit, int port, int stall_en, int lane)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_LANEf, lane); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_ENf, stall_en);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Configure ILKN lanes. this is a sub function of port 
 *        attach method.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - pm information DB
 * \param [in] lane_map - lane map info for ILKN core
 *   
 * \return
 *   STATIC int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_lanes_config(int unit, int port, pm_info_t pm_info, pmOsIlkn_core_lane_map_t *lane_map)
{
    pmOsIlkn_internal_t* ilkn_data;
    portmod_pbmp_t serdes_rx_bmp, serdes_tx_bmp;
    int core_id, lane, lane_pos, last_lane_pos, last_lane;
    uint32 rx_reg_val, tx_reg_val, lane_count;
    int nof_lanes;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    /* Clear rx and tx serdes bmp */
    PORTMOD_PBMP_CLEAR(serdes_rx_bmp);
    PORTMOD_PBMP_CLEAR(serdes_tx_bmp);

    for (lane = 0 ; lane < nof_lanes ; lane++)
    {
        PORTMOD_PBMP_PORT_ADD(serdes_rx_bmp, lane_map->rx_lane_map[lane]);
        PORTMOD_PBMP_PORT_ADD(serdes_tx_bmp, lane_map->tx_lane_map[lane]);
    }

    rx_reg_val = SOC_PBMP_WORD_GET(serdes_rx_bmp, 0);
    tx_reg_val = SOC_PBMP_WORD_GET(serdes_tx_bmp, 0);
    /* Set enabled lanes in HW */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, rx_reg_val, tx_reg_val));

    sal_usleep(1000);

    /* Get last lane for stall selection - Choose the last enabled lane in order to prevent skew probles between lanes*/
    PORTMOD_PBMP_COUNT(serdes_tx_bmp, lane_count);
    last_lane_pos = lane_count - 1;
    lane_pos = 0;
    PORTMOD_PBMP_ITER(serdes_tx_bmp, last_lane)
    {
        if (lane_pos == last_lane_pos) 
        {
            break; 
        }
        ++lane_pos;
    }
    /* Set stall slection in HW */
    _SOC_IF_ERR_EXIT(_pmOSILKN_50G_ilkn_stall_selection_set(unit, port, 1, last_lane));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Configure ILKN bursts in HW.
 * 
 * \param [in] unit - chip unit ID.
 * \param [in] port - logical port
 * \param [in] pm_info - pm info DB
 * \param [in] burst_max - burst max in bytes
 * \param [in] burst_min - burst min in bytes
 * \param [in] burst_short - burst short in bytes
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_burst_config_set(int unit, int port, pm_info_t pm_info, int burst_max, int burst_min, int burst_short)
{
    uint32 reg_val, burst_short_val, burst_max_val, burst_min_val;
    SOC_INIT_FUNC_DEFS;

    switch (burst_short)
    {
    case 32:
        burst_short_val = OS_ILKN_BURST_SHORT_32B_VAL;
        break;
    case 64: 
        burst_short_val = OS_ILKN_BURST_SHORT_64B_VAL;
        break;
    case 96:
        burst_short_val = OS_ILKN_BURST_SHORT_96B_VAL;
        break;
    case 128:
        burst_short_val = OS_ILKN_BURST_SHORT_128B_VAL;
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Burst short value %d for port %d is invalid"), burst_short, port));
    }
    /*burst min has the following calculation in HW: (burst_min_bytes = (burst_min_val+1) * 32bytes). 
      must be lesser or equal to burst_max/2 and must be higher or equal to burst_short */
    burst_min_val = burst_min / 32 - 1;
    /*burst max has the following calculation in HW: (burst_max_bytes = (burst_max_val+1) * 64bytes). 
      i.e 0000 = 64 bytes   0001 = 128 bytes ... and so on*/
    burst_max_val = burst_max / 64 - 1;
    
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_BURSTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_BURSTr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - configure metaframe period in HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] metaframe_period - metaframe period in bytes
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_metaframe_period_set(int unit, int port, pm_info_t pm_info, int metaframe_period)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_METAFRAMEr, &reg_val, SLE_TX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_METAFRAMEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_METAFRAMEr, &reg_val, SLE_RX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_METAFRAMEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - set number of segments in HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] nof_segments - number of segements for the ILKN 
 *        port. (2/4 or 0 to disable)
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_nof_segments_set(int unit, int port, pm_info_t pm_info, uint32 nof_segments)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* Segments are a resourse of the ILKN, which indicates the BW the ILKN port can pass. 
     * There are 4 segments in the ILKN core, which has to be divided between all ports of the same ILKN core.
     * there can be two divisions:
     * 1. 4 segments to ILKN0 (ILKN1 is not active)
     * 2. 2 segments to ILKN0 and 2 segments to ILKN1.
     * calling this function with 0 segments should only be done when the port is removed.
     */

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_0f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_1f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_2f, nof_segments > 2 ? 1 : 0); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_3f, nof_segments > 2 ? 1 : 0);
    if (nof_segments > 4)
    {
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_4f, 1);
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_5f, 1);
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_6f, 1);
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_7f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_0f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_1f, nof_segments > 0 ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f, nof_segments > 2 ? 1 : 0); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_3f, nof_segments > 2 ? 1 : 0);
    if (nof_segments > 4)
    {
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_4f, 1);
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_5f, 1);
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_6f, 1);
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_7f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - set watermarks values to HW
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] wm_high - watermark high indication
 * \param [in] wm_low - watermark low indication
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_watermarks_set(int unit, int port, pm_info_t pm_info, uint32 wm_high, uint32 wm_low)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FIFO_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_HIGHf, wm_high);
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_LOWf, wm_low);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FIFO_CFGr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Enable inband flow control in ILKN core
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] tx_cal_len - Tx calender len
 * \param [in] rx_cal_len - Rx calender len
 *
 * \return
 *   STATIC int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_inband_fc_set(int unit, int port, pm_info_t pm_info, int tx_cal_len, int rx_cal_len)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_INBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_INBANDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_INBANDr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_INBANDr, &reg_val, SLE_TX_CAL_INBAND_LASTf, ((tx_cal_len-1 >= 0) ? (tx_cal_len-1) : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_INBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_INBANDr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_INBANDr, &reg_val, SLE_RX_CAL_INBAND_LASTf, ((rx_cal_len-1 >= 0) ? (rx_cal_len-1) : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_INBANDr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - clear all counters in the ILKN core - for all 
 *        channels.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC 
int _pmOsILKN_50G_ilkn_counters_clear(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    int ch;
    SOC_INIT_FUNC_DEFS;

    for (ch = 0; ch < OS_ILKN_MAX_NOF_CHANNELS; ++ch) 
    {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, ch);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, ch);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));
        
        sal_usleep(1000);
    }
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - ILKN general configuration to HW. here we want to 
 *        clear the configuration register just in case before
 *        we start configuring the requested values.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_general_cfg_set(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* SLE default values depend on LA-mode (on/off).
       just to make sure, we set these regs to 0, than configure required fields.*/
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, 0));

    /* this configuration tells the ILKN core how many cycles to wait before accessing the memory for the statistic counters*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val)); 
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Set Data pack indication in HW. Data pack allows the 
 *        core to pack the bursts to a specified size.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] is_data_pack - enable / disable data pack enabled
 *        logic.
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_data_pack_set(int unit, int port, pm_info_t pm_info, uint32 is_data_pack)
{
    uint32 reg_val;
    pmOsIlkn_revision_t revision;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    _SOC_IF_ERR_EXIT(pmOsILKN_revision_get(unit, port, &revision));
    /*
     * In SIM, the revision is not retrieved correctly
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (OS_ILKN_IS_REVISION_8(revision)) {
            soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_TX_DATA_PCKf, is_data_pack);
        } else {
            soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_PCKf, is_data_pack);
        }
    }

    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Set the default lane order for ILKN core.
 *       There is no logical lane swapping by default.
 *       lane_order[0] = 0, lane_order[1] = 1.....
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_map - ILKN core logical to physical mapping info
 *
 *   
 * \return
 *   STATIC int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
STATIC
int _pmOsILKN_50G_ilkn_logical_lane_order_init(int unit, int port, pm_info_t pm_info)
{
    int core_id;
    int lane_count;
    int lane_order[OS_ILKN_TOTAL_LANES_PER_CORE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    pmOsIlkn_internal_t* ilkn_data;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    PORTMOD_PBMP_COUNT(ilkn_data->phys, lane_count);

    /* Set the default Lane order - No remapping*/
    _SOC_IF_ERR_EXIT(pmOsILKN_50G_port_logical_lane_order_set(unit, port, pm_info, lane_order, lane_count));

exit:
    SOC_FUNC_RETURN;
}

/* update the Aggregated PM list attached for a specific ilkn port in the SW-state */
int pmOsILKN_50G_pm_aggregated_update(int unit,  pm_info_t pm_info, int core_id, int nof_aggregated_pms,  const pm_info_t* pms, const int* pm_ids)
{
    pmOsIlkn_internal_t *ilkn_data ;
    int pm_index, temp_enable;
    SOC_INIT_FUNC_DEFS;

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (pm_index = 0; pm_index < nof_aggregated_pms; ++pm_index) {
        ilkn_data->pms[pm_index] = pms[pm_index];
        ilkn_data->pm_ids[pm_index] = pm_ids[pm_index];

        if(!SOC_WARM_BOOT(unit)){
            temp_enable = 1;
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, core_id, pm_index));
            /* Store the Ilkn PM-Id in the WB SW-state */
            _SOC_IF_ERR_EXIT(PM_ILKN_AGGREGATED_PM_ID_SET(unit, pm_info, pm_ids[pm_index], core_id, pm_index));
        }


    }
exit:
    SOC_FUNC_RETURN;
}


/**
 * \brief - attach a new ILKN port. this is the method to 
 *        configure a new port in the ILKN port
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] add_info - information about the new port from 
 *        the user.
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{        
    pmOsIlkn_internal_t* ilkn_data;
    pmOsIlkn_core_lane_map_t lane_map;
    uint32 data_pack, ilkn_core_id;
    portmod_port_add_info_t add_info_copy;
    int pm, nof_lanes_temp, is_pm_aggregated = 0;
    int first_phy = 0;
    portmod_pbmp_t phys_temp;
    SOC_INIT_FUNC_DEFS;

    /* ILKN core id is the port index inside the ILKN core. (in JR2 core_id will always be 0) */
    ilkn_core_id = add_info->ilkn_core_id;
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[ilkn_core_id]);

    /* Connect the logical port to the ILKN core interface id in WB Engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, port, ilkn_core_id));

    /* Set ILKN over fabric indication in WB Engine */
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, add_info->ilkn_port_is_over_fabric, ilkn_core_id));

    PORTMOD_PBMP_ASSIGN(ilkn_data->phys, add_info->phys);

    /* Get the ILKN core lane map */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_port_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    /**** Configure OpenSilicon ILKN core ****/
    /* Update the Active lanes of the ILKN port */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_lanes_config(unit, port, pm_info, &lane_map));

    /* Common CFG regs */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_general_cfg_set(unit, port, pm_info));

    /* Configure Retransmit if enabled */
    if (add_info->rx_retransmit || add_info->tx_retransmit)
    {
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_retransmit_config_set(unit, port, pm_info, add_info->rx_retransmit, 
                                                                 add_info->tx_retransmit, add_info->reserved_channel_rx, add_info->reserved_channel_tx)); 
    }

    /* Metaframe period */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_metaframe_period_set(unit, port, pm_info, add_info->ilkn_metaframe_period));
    
    /* Segments enable */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_nof_segments_set(unit, port, pm_info, add_info->ilkn_nof_segments));

    /* Set Watermarks */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_watermarks_set(unit, port, pm_info, ilkn_data->wm_high, ilkn_data->wm_low));

    /* Set Flow Control */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_inband_fc_set(unit, port, pm_info, add_info->ilkn_inb_cal_len_tx, add_info->ilkn_inb_cal_len_rx));

    /* Reset all SLE counters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_counters_clear(unit, port, pm_info));

    /* Configure PMs below */
    sal_memcpy(&add_info_copy, add_info, sizeof(portmod_port_add_info_t));
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, ilkn_core_id, pm));
        if (is_pm_aggregated) {
            /* Get  the phys of this specific pm */
            PORTMOD_PBMP_CLEAR(phys_temp);
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_phys_get(unit, ilkn_data->pms[pm], &phys_temp));
            _SHR_PBMP_FIRST(phys_temp, first_phy);
            PORTMOD_PBMP_COUNT(phys_temp, nof_lanes_temp);
            /* Get the ILKN core lane map */
            _SOC_IF_ERR_EXIT(
                pm_info->pm_data.pmOsIlkn_db->ilkn_pm_lane_map_get(unit, port, nof_lanes_temp, first_phy,  add_info_copy.init_config.lane_map));

            add_info_copy.init_config.lane_map_overwrite=1;

            if (__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_bypass_set)
            {
                /* set PM into bypass mode */
                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_pm_bypass_set(unit, ilkn_data->pms[pm], 1));
            }
            if (__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_core_add)
            {
                int core_add_flags = 0;

                PORTMOD_CORE_ADD_SKIP_MAC_SET(core_add_flags);
                /* call PM below core add method */

                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_core_add(unit, ilkn_data->pm_ids[pm], ilkn_data->pms[pm], core_add_flags, &add_info_copy));
            }
            /* Call PM below port attach method */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_attach(unit, port, ilkn_data->pms[pm], &add_info_copy));
        }
    }
    /* Configure Burst parameters */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_burst_config_set(unit, port, pm_info, add_info->ilkn_burst_max, add_info->ilkn_burst_min, add_info->ilkn_burst_short));

    /* Set Data Pack for ELK ports */
    data_pack = (PORTMOD_PORT_ADD_F_ELK_GET(add_info)) ? 1 : 0;
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_data_pack_set(unit, port, pm_info, data_pack));

    /* Set the default ILKN logcial lane mapping */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_logical_lane_order_init(unit, port, pm_info));

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Detach an existing ILKN port from the ILKN core.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_detach(int unit, int port, pm_info_t pm_info)
{        
    int enable, core_id, pm, is_pm_aggregated, temp_enable, burst_max, burst_min, burst_short;
    int wm_high, wm_low, metaframe_period;
    pmOsIlkn_internal_t* ilkn_data;
    uint64 reg_default;
    uint32 reg_val, burst_short_reg;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    /* Set Data Pack to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_data_pack_set(unit, port, pm_info, 0));

    /* Configure Burst parameters  to default values */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_RX_BURSTr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    burst_short_reg = soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_SHORTf);
    switch (burst_short_reg)
    {
    case OS_ILKN_BURST_SHORT_32B_VAL:
        burst_short = 32;
        break;
    case OS_ILKN_BURST_SHORT_64B_VAL:
        burst_short = 64;
        break;
    case OS_ILKN_BURST_SHORT_96B_VAL:
        burst_short = 96;
        break;
    case OS_ILKN_BURST_SHORT_128B_VAL:
        burst_short = 128;
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Burst short value %d for port %d is invalid"), burst_short_reg, port));
    }
    burst_min = ((soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_MINf))+1) * 32 ;
    burst_max = ((soc_reg_field_get(unit, ILKN_SLE_RX_BURSTr, reg_val, SLE_RX_BURST_MAXf))+1) * 64 ;
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_burst_config_set(unit, port, pm_info, burst_max, burst_min, burst_short));

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_detach(unit, port, ilkn_data->pms[pm])); 
        }
        temp_enable = 0;
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, core_id, pm));
    }

    /* Set Watermarks */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_TX_FIFO_CFGr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    wm_high = soc_reg_field_get(unit, ILKN_SLE_TX_FIFO_CFGr, reg_val, SLE_TX_FIFO_CFG_WM_HIGHf);
    wm_low = soc_reg_field_get(unit, ILKN_SLE_TX_FIFO_CFGr, reg_val, SLE_TX_FIFO_CFG_WM_LOWf);
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_watermarks_set(unit, port, pm_info, wm_high, wm_low));

    /* Segments enable */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_nof_segments_set(unit, port, pm_info, 0));

    /* Metaframe period */
    SOC_REG_RST_VAL_GET(unit, ILKN_SLE_TX_METAFRAMEr, reg_default);
    reg_val = COMPILER_64_LO(reg_default);
    metaframe_period = soc_reg_field_get(unit, ILKN_SLE_TX_METAFRAMEr, reg_val, SLE_TX_METAFRAME_PERIODf);
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_metaframe_period_set(unit, port, pm_info, metaframe_period));

    /* Configure Retransmit Properties to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_retransmit_config_set(unit, port, pm_info, 0, 0, 0, 0)); 

    /* Update the Active lanes of the ILKN port to 0 */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_ilkn_enabled_lanes_set(unit, port, 0, 0));
    _SOC_IF_ERR_EXIT(_pmOSILKN_50G_ilkn_stall_selection_set(unit, port, 0, 0));

    /* Disable - clean DB */
    PORTMOD_PBMP_CLEAR(ilkn_data->phys);
    /* Clear WB engine */
    temp_enable = -1; /* -1 = not_applicable */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, temp_enable, core_id));
    temp_enable = 0;
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, temp_enable, core_id));
        
exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - method to retrieve phy access information, which can 
 *        be used to call Phymod APIs
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] params - specifications for access structure to 
 *        be returned
 * \param [in] max_phys - how many phy_access elements are given 
 *        in output array
 * \param [out] access - output phy access array
 * \param [out] nof_phys - how many phy access elements were 
 *        returned
 * \param [out] is_most_ext - this pointer can be NULL. it is 
 *        expected to return an indication whether the output
 *        phy access include the most external phy as well. in
 *        JR2 there is no account for external phys so this
 *        indication will always return true (internal phy is
 *        always the most external..)
 *   
 * \return
 *   int - see _SHR_E*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{        
    int core_id, arr_inx, rv, is_pm_aggregated=0;
    int  phy_iter = 0, required_phy = 0, pm_index, lane_pm_found = FALSE, active_lane_in_ilkn_port = 0, active_lane_in_pm = 0, ilkn_port_nof_lanes;
    pmOsIlkn_internal_t *ilkn_data;
    portmod_access_get_params_t params_copy;    
    portmod_pbmp_t phys_temp;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* validate that the given lane is not bigger than the maximum number of ilkn lanes */
    PORTMOD_PBMP_COUNT(ilkn_data->phys,ilkn_port_nof_lanes);
    if (params->lane >= ilkn_port_nof_lanes)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("provided lane %d is to big"), params->lane));
    }

    sal_memcpy(&params_copy, params, sizeof(portmod_access_get_params_t));

    if (is_most_ext) 
    {
        /* Jr2 and above does not account for external phys.
           Hence, internal_phy is always the most external */
        *is_most_ext = 1;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&access[0]));
    *nof_phys = arr_inx = 0;

    active_lane_in_ilkn_port = 0;

    if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE)
    {
        PORTMOD_PBMP_ITER(ilkn_data->phys, phy_iter )
        {
            if (active_lane_in_ilkn_port == params->lane)
            {
                required_phy = phy_iter;
                break;
            }
            active_lane_in_ilkn_port++;
        }
    }

    /* call PMs below - aggregate here */
    for (pm_index = 0; pm_index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm_index)
    {
       _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm_index));
       if (is_pm_aggregated)
       {
           if (arr_inx >= max_phys)
           {
              _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("access size %d isn't big enough"), max_phys));
           }

           if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE)
           {
               PORTMOD_PBMP_CLEAR(phys_temp);
               /* Take the PM phys */
               _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm_index]->type]->f_portmod_pm_phys_get(unit, ilkn_data->pms[pm_index], &phys_temp));

               if (!PORTMOD_PBMP_MEMBER(phys_temp, required_phy))
               {
                   /* Phy is not a member of this PM */
                   continue;
               }
               lane_pm_found = TRUE;
               active_lane_in_pm = 0;
               PORTMOD_PBMP_AND(phys_temp, ilkn_data->phys);
               PORTMOD_PBMP_ITER(phys_temp, phy_iter )
               {
                  if (phy_iter == required_phy)
                  {
                      params_copy.lane = active_lane_in_pm;
                      break;
                  }
                  active_lane_in_pm++;
               }

           }

           rv = __portmod__dispatch__[ilkn_data->pms[pm_index]->type]->f_portmod_port_phy_lane_access_get(unit, port,
                                                                                   ilkn_data->pms[pm_index], &params_copy, max_phys - arr_inx, access + arr_inx, nof_phys, NULL);
           _SOC_IF_ERR_EXIT(rv);

           arr_inx += *nof_phys;
       }
    }

    if ((params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE) && (!lane_pm_found))
    {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("No PM was found for lane %d of port %d"), params->lane, port));
    }

    *nof_phys = arr_inx; 

exit:
    SOC_FUNC_RETURN;
}


/**
 * \brief - Get the logical lane order info.
 *    Using lane order of Rx side is enough, as we can get the
 *    same order from tx side.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_order_max_size - max lanes of the ILKN port
 * \param [out] lane_order - lane order array.
 * \param [out] lane_order_actual_size - actual lane order size
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_logical_lane_order_get(int unit, int port, pm_info_t pm_info, int lane_order_max_size, int* lane_order, int* lane_order_actual_size)
{
    pmOsIlkn_internal_t* ilkn_data;
    int field_index, reg_index;
    int core_id, lane_id, serdes_id, count, nof_lanes;
    int tx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int reverse_tx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int active_tx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    uint32 reg_val;
    pmOsIlkn_core_lane_map_t lane_map;
    portmod_pbmp_t active_tx_serdes_bmp;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    /*
     * Set the default mapping, one to one mapping
     * This can ensure it is still one to one mapping after
     * reversing rx_lane_order.
     */
    for (serdes_id = 0; serdes_id < OS_ILKN_TOTAL_LANES_PER_CORE; ++serdes_id)
    {
        tx_lane_order[serdes_id] = serdes_id;
    }
    /*
     * Get the lane map info for ILKN core
     */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_port_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    PORTMOD_PBMP_CLEAR(active_tx_serdes_bmp);
    count = 0;
    for (lane_id = 0 ; lane_id < nof_lanes ; lane_id++)
    {
        /* calc relevant reg and field */
        field_index = lane_map.tx_lane_map[lane_id];
        reg_index = field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;

        /* get tx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[reg_index], port, 0, &reg_val));
        tx_lane_order[field_index] = soc_reg_field_get(unit, remap_lanes_tx_regs[reg_index], reg_val, remap_lanes_tx_fields[field_index]);
        /*
         * Get the active Tx serdes bitmap
         */
        PORTMOD_PBMP_PORT_ADD(active_tx_serdes_bmp, lane_map.tx_lane_map[lane_id]);
        count++;
    }

    /*
     * Reverse the tx serdes lane order to conclude the logical lane order,
     * only using tx mapping is enough, as we can get the same result using
     * rx mapping.
     */
    for (serdes_id = 0; serdes_id < OS_ILKN_TOTAL_LANES_PER_CORE; ++serdes_id)
    {
        reverse_tx_lane_order[tx_lane_order[serdes_id]] = serdes_id;
    }
    /*
     * Set the serdes array by order
     * For exampe, the active serdes bitmap is b'101010,
     * the following info should be stored:
     * active_rx_serdes[1] = 0;
     * active_rx_serdes[3] = 1;
     * active_rx_serdes[5] = 2;
     */
    count = 0;
    PORTMOD_PBMP_ITER(active_tx_serdes_bmp, serdes_id)
    {
        /* coverity protection */
        if (serdes_id > OS_ILKN_TOTAL_LANES_PER_CORE - 1) {
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("Tx serdes_id %d is out of bounds"), serdes_id));
        }
        active_tx_serdes[serdes_id] = count;
        count++;
    }
    /*
     * Get the ILKN mapping info
     */
    count = 0;
    for (lane_id = 0 ; lane_id < nof_lanes ; lane_id++)
    {
        serdes_id = reverse_tx_lane_order[lane_map.tx_lane_map[lane_id]];
        lane_order[count] = active_tx_serdes[serdes_id];
        count++;
    }
    _SOC_IF_ERR_EXIT(portmod_port_nof_lanes_get(unit, port, &nof_lanes));
    *lane_order_actual_size = lane_order_max_size < nof_lanes ? lane_order_max_size : nof_lanes;

exit:
    SOC_FUNC_RETURN;
}

/**
 * \brief - Write the rx/tx lane order info to registers.
 *     All the invalid lanes should be one to one mapping.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM info DB
 * \param [in] lane_order - lane order array, logical lane order
 *     For example, lane_oder[0] represents for the 1st lane of
 *     the ILKN port.
 * \param [in] lane_order_size - active lanes in for ILKN port
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_50G_port_logical_lane_order_set(int unit, int port, pm_info_t pm_info, const int* lane_order, int lane_order_size)
{
    pmOsIlkn_internal_t* ilkn_data;
    int pre_rx_field_index, rx_field_index, pre_rx_reg_index, rx_reg_index;
    int tx_field_index, tx_reg_index;
    int core_id, pre_lane_id = 0, lane_id, serdes_id;
    int count, pre_remap_lane,remap_lane;
    int active_rx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    int active_tx_serdes[OS_ILKN_TOTAL_LANES_PER_CORE] = { 0 };
    int rx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int tx_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int nof_lanes;
    uint32 reg_val;
    pmOsIlkn_core_lane_map_t lane_map;
    portmod_pbmp_t active_rx_serdes_bmp, active_tx_serdes_bmp;
    int fec_enable = 0;
    int fec_bypass_enable = 0;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    /*
     * Get the lane map info for ILKN core
     */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_port_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    /*
     * Convert the active lanes to active serdes bitmap
     */
    PORTMOD_PBMP_CLEAR(active_rx_serdes_bmp);
    PORTMOD_PBMP_CLEAR(active_tx_serdes_bmp);
    for (lane_id = 0 ; lane_id < nof_lanes ; lane_id++)
    {
        /* Store rx and tx serdes bmp */
        PORTMOD_PBMP_PORT_ADD(active_rx_serdes_bmp, lane_map.rx_lane_map[lane_id]);
        PORTMOD_PBMP_PORT_ADD(active_tx_serdes_bmp, lane_map.tx_lane_map[lane_id]);
    }

    /*
     * Store the serdes ID by order
     * For exampe, the active serdes bitmap is b'101010,
     * the following info should be stored:
     * active_rx_serdes[0] = 1;
     * active_rx_serdes[1] = 3;
     * active_rx_serdes[2] = 5;
     */
    count = 0;
    PORTMOD_PBMP_ITER(active_rx_serdes_bmp, serdes_id)
    {
        active_rx_serdes[count] = serdes_id;
        count++;
    }
    count = 0;
    PORTMOD_PBMP_ITER(active_tx_serdes_bmp, serdes_id)
    {
        active_tx_serdes[count] = serdes_id;
        count++;
    }

    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_fec_enable_get(unit, port, pm_info, &fec_enable, &fec_bypass_enable));
    count = 0;
    for (lane_id = 0 ; lane_id < nof_lanes ; lane_id++)
    {
        /*
         * Get the remap logical lane id
         * For exampe:
         * lane_order[4] = 2 (all the values are zero-based)
         * this means the 5th lane of the ILKN port is remapped
         * to the 3rd lane.
         * remap_lane = 2
 */
        remap_lane = lane_order[count];
        /*
         * Set the lane order according to the remapped lane id
         */

        rx_lane_order[active_rx_serdes[remap_lane]] = lane_map.rx_lane_map[lane_id];
        tx_lane_order[active_tx_serdes[remap_lane]] = lane_map.tx_lane_map[lane_id];

        /*
         * Config lane remap register, rx and tx register should be configured seperately
         */
        rx_field_index = active_rx_serdes[remap_lane];
        tx_field_index = active_tx_serdes[remap_lane];

        rx_reg_index = rx_field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;
        tx_reg_index = tx_field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;
         /*Need to adjust previous RX remap due to ILKN FEC doing the Rx swap automatically.*/
        if (fec_enable && (count % 2 == 1) && OS_ILKN_PHY_ADDR_PAIRS_IS_CROSS( \
            lane_map.rx_lane_map[pre_lane_id],lane_map.rx_lane_map[lane_id])) {

            pre_remap_lane = lane_order[count - 1];
            pre_rx_field_index = active_rx_serdes[pre_remap_lane];
            pre_rx_reg_index = pre_rx_field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;
            /*configure current ilkn lane id rx map*/
            _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, &reg_val));
            soc_reg_field_set(unit, remap_lanes_rx_regs[rx_reg_index], &reg_val, remap_lanes_rx_fields[rx_field_index], rx_lane_order[pre_rx_field_index]);
            _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, reg_val));

            /*Adjust the previous ilkn lane id rx remap */
            _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[pre_rx_reg_index], port, 0, &reg_val));
            soc_reg_field_set(unit, remap_lanes_rx_regs[pre_rx_reg_index], &reg_val, remap_lanes_rx_fields[pre_rx_field_index], rx_lane_order[rx_field_index]);
            _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[pre_rx_reg_index], port, 0, reg_val));

        } else {
            /* configure rx remap */
            _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, &reg_val));
            soc_reg_field_set(unit, remap_lanes_rx_regs[rx_reg_index], &reg_val, remap_lanes_rx_fields[rx_field_index], rx_lane_order[rx_field_index]);
            _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[rx_reg_index], port, 0, reg_val));
        }
        /* configure tx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[tx_reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_tx_regs[tx_reg_index], &reg_val, remap_lanes_tx_fields[tx_field_index], tx_lane_order[tx_field_index]);
        _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_tx_regs[tx_reg_index], port, 0, reg_val));

        count++;
        pre_lane_id = lane_id;
    }
exit:
    SOC_FUNC_RETURN;

}
/*fec enable get*/
STATIC int _pmOsILKN_50G_port_fec_enable_get(int unit, int port, pm_info_t pm_info, int *fec_enable, int *fec_bypass_enable)
{
    uint32 reg_val, is_tx_enable, is_rx_enable;
    int fec_bypass_valid;
    SOC_INIT_FUNC_DEFS;

    fec_bypass_valid = pm_info->pm_data.pmOsIlkn_db->fec_disable_by_bypass;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    is_tx_enable = soc_reg_field_get(unit, ILKN_SLE_TX_CFGr, reg_val, SLE_TX_CFG_FEC_ENf);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    is_rx_enable = soc_reg_field_get(unit, ILKN_SLE_RX_CFGr, reg_val, SLE_RX_CFG_FEC_ENf);

    *fec_enable = (is_tx_enable && is_rx_enable) ? 1 : 0;

    if (fec_bypass_valid)
    {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
        is_tx_enable = soc_reg_field_get(unit, ILKN_SLE_TX_CFGr, reg_val, SLE_TX_CFG_FEC_BYPASSf);

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
        is_rx_enable = soc_reg_field_get(unit, ILKN_SLE_RX_CFGr, reg_val, SLE_RX_CFG_FEC_BYPASSf);

        *fec_bypass_enable = (is_tx_enable && is_rx_enable) ? 1 : 0;
    }

exit:
    SOC_FUNC_RETURN;
}

/*fec enable set*/
STATIC int _pmOsILKN_50G_port_fec_enable_set(int unit, int port, pm_info_t pm_info, int fec_enable, int fec_bypass_enable)
{
    uint32 reg_val;
    pmOsIlkn_revision_t revision;
    int lane_order_size;
    int lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int fec_bypass_valid;

    SOC_INIT_FUNC_DEFS;

    fec_bypass_valid = pm_info->pm_data.pmOsIlkn_db->fec_disable_by_bypass;
    _SOC_IF_ERR_EXIT(pmOsILKN_revision_get(unit, port, &revision));

    _SOC_IF_ERR_EXIT(pm_info->pm_data.pmOsIlkn_db->ilkn_port_fec_units_set(unit, port, FALSE, fec_enable, fec_bypass_valid, fec_bypass_enable));

    if (OS_ILKN_IS_REVISION_8(revision)) {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_FEC_DEC_PRE_CFGr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_FEC_DEC_PRE_CFGr, &reg_val, SLE_FEC_DEC_PRE_CFG_I_AM_PERIOD_CONFIGf, 0xaaaaaa);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_FEC_DEC_PRE_CFGr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_FEC_ENC_PER_CFGr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_FEC_ENC_PER_CFGr, &reg_val, SLE_FEC_ENC_PER_CFG_I_AM_PERIOD_CONFIGf, 0xaaaaaa);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_FEC_ENC_PER_CFGr(unit, port, reg_val));
     }
    /* get the lane order*/
    _SOC_IF_ERR_EXIT(pmOsILKN_50G_port_logical_lane_order_get(unit, port, pm_info,
                              OS_ILKN_TOTAL_LANES_PER_CORE, lane_order, &lane_order_size));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_FEC_ENf, fec_enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_FEC_ENf, fec_enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    if (fec_bypass_valid)
    {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_FEC_BYPASSf, fec_bypass_enable);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_FEC_BYPASSf, fec_bypass_enable);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));
    }

    /* After the fec status change, need to reconfig the lane order due to FEC enable or not
     * in the consider list in lane_order set().Because the FEC BLOCK is aligning Rx by itself.
     */
    _SOC_IF_ERR_EXIT(pmOsILKN_50G_port_logical_lane_order_set(unit, port, pm_info, lane_order, lane_order_size));

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_50G_port_speed_config_set(int unit, int port, pm_info_t pm_info, const portmod_speed_config_t* speed_config)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, is_pm_aggregated = 0;
    uint32 nof_pipes = 0;
    int fec_enable = 0;
    int fec_bypass_enable = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_speed_config_set(unit, port, ilkn_data->pms[pm], speed_config));
        }
    }

    if (!PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_GET(speed_config))
    {

        if (!pm_info->pm_data.pmOsIlkn_db->fec_disable_by_bypass)
        {
            if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544) {
                fec_enable = 1;
            } else {
                fec_enable = 0;
            }
        }
        else
        {
            /*
             * FEC Bypass configuration is required. Enable FEC and set FEC Bypass mode.
             */
            fec_enable = 1;

            if (speed_config->fec == PORTMOD_PORT_PHY_FEC_RS_544) {
                fec_bypass_enable = 0;
            } else {
                fec_bypass_enable = 1;
            }
        }

        /*fec enable*/
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_fec_enable_set(unit, port, pm_info, fec_enable, fec_bypass_enable));
    }

    /* pipe enable */
    /* enable num_lanes pipes, or all pipes */
    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_num_pipes_get(unit, port, pm_info, &nof_pipes));
    _SOC_IF_ERR_EXIT(pmOsILKN_port_pipe_config(unit, port, pm_info, nof_pipes));

exit:
	SOC_FUNC_RETURN;
}

int pmOsILKN_50G_port_speed_config_get(int unit, int port, pm_info_t pm_info, portmod_speed_config_t* speed_config)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, is_pm_aggregated = 0;
    int pm_found = 0;
    int fec_enable = 0;
    int fec_bypass_enable = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_speed_config_get(unit, port, ilkn_data->pms[pm], speed_config));
            pm_found = 1;
            break;
        }
    }

    _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_fec_enable_get(unit, port, pm_info, &fec_enable, &fec_bypass_enable));

    if (!pm_info->pm_data.pmOsIlkn_db->fec_disable_by_bypass)
    {
        if (fec_enable) {
            speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_544;
        } else {
            speed_config->fec = PORTMOD_PORT_PHY_FEC_NONE;
        }
    }
    else
    {
        if (!fec_bypass_enable) {
            speed_config->fec = PORTMOD_PORT_PHY_FEC_RS_544;
        } else {
            speed_config->fec = PORTMOD_PORT_PHY_FEC_NONE;
        }
    }

    if (!pm_found) {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("aggregated PM not found for port %d"), port));
    }

exit:
	SOC_FUNC_RETURN;
}

STATIC
int _pmOsILKN_50G_port_logical_lane_order_align(int unit, int port, pm_info_t pm_info, int is_to_align)
{
    pmOsIlkn_internal_t* ilkn_data;
    int field_index, reg_index, lane;
    int core_id, lane_id,  nof_lanes;
    int rx_lane_lookup[OS_ILKN_TOTAL_LANES_PER_CORE];
    uint32 reg_val, tx_hw_val, rx_hw_val;
    portmod_pbmp_t lanes_bmp;
    pmOsIlkn_core_lane_map_t lane_map;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    /* Get the lane map info for ILKN core */
    _SOC_IF_ERR_EXIT(
        pm_info->pm_data.pmOsIlkn_db->ilkn_port_lane_map_get(unit, port, OS_ILKN_TOTAL_LANES_PER_CORE, lane_map.rx_lane_map, lane_map.tx_lane_map));

    PORTMOD_PBMP_CLEAR(lanes_bmp);

    /*Fill the Rx lanes lookup Array, whereas the index is the Tx mapping of the lane, the value is the Rx mapping of the same lane */
    for (lane_id = 0 ; lane_id < nof_lanes ; lane_id++)
    {
        rx_lane_lookup[lane_map.tx_lane_map[lane_id]]=lane_map.rx_lane_map[lane_id];
        PORTMOD_PBMP_PORT_ADD(lanes_bmp, lane_map.tx_lane_map[lane_id]);
    }

    /*For each lane set Rx mapping in Hw. if is_to_align is set (loopback mode) RX map = Tx map, otherwise set according to mapping taken from lane order*/
    PORTMOD_PBMP_ITER(lanes_bmp, lane)
    {
        /* calc relevant reg and field */
        field_index = lane % OS_ILKN_TOTAL_LANES_PER_CORE; /*coverity */
        reg_index = field_index / OS_ILKN_LANE_MAP_REGISTER_RESOLUTION;

        /* get tx remap */
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[reg_index], port, 0, &reg_val));
        tx_hw_val = soc_reg_field_get(unit, remap_lanes_tx_regs[reg_index], reg_val, remap_lanes_tx_fields[field_index]);

        rx_hw_val = (is_to_align) ? tx_hw_val :rx_lane_lookup[tx_hw_val];
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_rx_regs[reg_index], &reg_val, remap_lanes_rx_fields[field_index], rx_hw_val);
       _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[reg_index], port, 0, reg_val));

    }

exit:
    SOC_FUNC_RETURN;
}

STATIC int _pmOsILKN_50G_port_loopback_set(int unit, int port, int enable)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

STATIC int _pmOsILKN_50G_port_loopback_get(int unit, int port, int *enable)
{
    uint32 reg_val, is_tx, is_rx;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    is_tx = soc_reg_field_get(unit, ILKN_SLE_TX_CFGr, reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    is_rx = soc_reg_field_get(unit, ILKN_SLE_RX_CFGr, reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf);

    *enable = (is_tx && is_rx) ? 1 : 0;

exit:
    SOC_FUNC_RETURN;
}



int pmOsILKN_50G_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int i, core_id, rv, is_pm_aggregated=0;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_logical_lane_order_align(unit, port, pm_info, enable));
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_loopback_set(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:

        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                type = ilkn_data->pms[i]->type;
                if ((type != portmodDispatchTypePmOsILKN) &&
                    (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_set)){
                    rv = __portmod__dispatch__[type]->f_portmod_port_loopback_set(unit, port, ilkn_data->pms[i], loopback_type, enable);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }


exit:
    SOC_FUNC_RETURN;

}

int pmOsILKN_50G_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    int i, core_id, rv, is_pm_aggregated=0;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_50G_port_loopback_get(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                type = ilkn_data->pms[i]->type;
                if ((type != portmodDispatchTypePmOsILKN) &&
                    (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_get)){
                    rv = __portmod__dispatch__[type]->f_portmod_port_loopback_get(unit, port, ilkn_data->pms[i], loopback_type, enable);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        (*enable) = 0; /* not supported --> no loopback */
        break;
    default:
        (*enable) = 0; /* not supported --> no loopback */
    }

exit:
    SOC_FUNC_RETURN;
}

/* enable credits for ILKN over fabric port - credits from SerDes Tx to ILKN core */
int pmOsILKN_50G_port_credits_enable_set(int unit, int port, pm_info_t pm_info, int enable)
{
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, pm, is_pm_aggregated = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++pm) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_credits_enable_set(unit, port, ilkn_data->pms[pm], enable));
        }
    }

exit:
	SOC_FUNC_RETURN;
}

int pmOsILKN_50G_port_pmd_lock_counter_get(int unit, int port, pm_info_t pm_info, uint32* pmd_lock_counter_sum)
{

    int core_id, index, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    uint32 pmd_lock_counter_single = 0;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    *pmd_lock_counter_sum = 0;
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (index = 0; index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++index) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, index));
        if (is_pm_aggregated) {
            /* assume all PMs are equal */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_pmd_lock_counter_get(unit, port, ilkn_data->pms[index], &pmd_lock_counter_single));
            *pmd_lock_counter_sum += pmd_lock_counter_single;
        }
    }

exit:
    SOC_FUNC_RETURN;

}
#endif /* PORTMOD_PM_OS_ILKN_50G_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
