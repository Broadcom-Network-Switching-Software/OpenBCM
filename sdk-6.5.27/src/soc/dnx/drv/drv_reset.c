/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */


#ifdef BCM_DNX_SUPPORT


#include <soc/dnxc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/drv.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/pll/pll.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_fabric_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>
#include  <bcm_int/dnx/dram/gddr6/gddr6.h>
#include <soc/dnxc/fabric.h>
#include <soc/counter.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#if defined(INCLUDE_KBP)
#include <bcm_int/dnx/kbp/kbp_recover.h>
#endif



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT

#ifdef BCM_88850_A0
CONST soc_reg_t j2p_per_block_config_reg[] = {
    BDM_RESERVED_MTCPr,
    CDB_RESERVED_MTCPr,
    CDBM_RESERVED_MTCPr,
    CDPM_RESERVED_MTCPr,
    CFC_RESERVED_MTCPr,
    CGM_RESERVED_MTCPr,
    CRPS_RESERVED_MTCPr,
    DDHA_RESERVED_MTCPr,
    DDHB_RESERVED_MTCPr,
    DDP_RESERVED_MTCPr,
    DQM_RESERVED_MTCPr,
    ECGM_RESERVED_MTCPr,
    ECI_RESERVED_MTCPr,
    EDB_RESERVED_MTCPr,
    EPNI_RESERVED_MTCPr,
    EPRE_RESERVED_MTCPr,
    EPS_RESERVED_MTCPr,
    ERPP_RESERVED_MTCPr,
    ESB_RESERVED_MTCPr,
    ETPPA_RESERVED_MTCPr,
    ETPPB_RESERVED_MTCPr,
    ETPPC_RESERVED_MTCPr,
    EVNT_RESERVED_MTCPr,
    FCR_RESERVED_MTCPr,
    FCT_RESERVED_MTCPr,
    FDA_RESERVED_MTCPr,
    FDR_RESERVED_MTCPr,
    FDT_RESERVED_MTCPr,
    FDTL_RESERVED_MTCPr,
    FMAC_RESERVED_MTCPr,
    FQP_RESERVED_MTCPr,
    FSRD_RESERVED_MTCPr,
    
    HBMC_RESERVED_MTCPr,
    HRC_RESERVED_MTCPr,
    ILE_RESERVED_MTCPr,
    IPPA_RESERVED_MTCPr,
    IPPB_RESERVED_MTCPr,
    IPPC_RESERVED_MTCPr,
    IPPD_RESERVED_MTCPr,
    IPPE_RESERVED_MTCPr,
    IPPF_RESERVED_MTCPr,
    IPS_RESERVED_MTCPr,
    IPT_RESERVED_MTCPr,
    IQM_RESERVED_MTCPr,
    IRE_RESERVED_MTCPr,
    ITPP_RESERVED_MTCPr,
    ITPPD_RESERVED_MTCPr,
    KAPS_RESERVED_MTCPr,
    MACT_RESERVED_MTCPr,
    MCP_RESERVED_MTCPr,
    MDB_RESERVED_MTCPr,
    MESH_TOPOLOGY_RESERVED_MTCPr,
    MRPS_RESERVED_MTCPr,
    MSD_RESERVED_MTCPr,
    MSS_RESERVED_MTCPr,
    MTM_RESERVED_MTCPr,
    NMG_RESERVED_MTCPr,
    OAMP_RESERVED_MTCPr,
    OCB_RESERVED_MTCPr,
    OCBM_RESERVED_MTCPr,
    OLP_RESERVED_MTCPr,
    PDM_RESERVED_MTCPr,
    PEM_RESERVED_MTCPr,
    PQP_RESERVED_MTCPr,
    RQP_RESERVED_MTCPr,
    RTP_RESERVED_MTCPr,
    SCH_RESERVED_MTCPr,
    SIF_RESERVED_MTCPr,
    SPB_RESERVED_MTCPr,
    SQM_RESERVED_MTCPr,
    TCAM_RESERVED_MTCPr,
    TDU_RESERVED_MTCPr,
    INVALIDr
};
#endif






int soc_dnx_init_reset_cmic_regs(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(soc_dnxc_mdio_config_set(unit));

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_drv_sbus_broadcast_config(int unit)
{
    int i;
    int nof_blocks;
    int nof_hbms;
    int nof_channels_in_hbm;
    uint32 dram_bitmap[1];
    int hbm_index;
    int schan, chain, block_valid;

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_fabric.links.nof_links_get(unit))
    {
        
        nof_blocks = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
        for (i=0; i<nof_blocks; i++)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_fabric_fmac_block_valid_get(unit, i, &block_valid));
            if (block_valid)
            {
                chain = dnx_data_fabric.blocks.fmac_sbus_chain_get(unit, i)->chain;
                schan = SOC_BLOCK_INFO(unit, SOC_BLOCK_INSTANCES(unit, SOC_BLK_BRDC_FMAC).first_blk_instance + chain).schan;

                if (SOC_IS_JERICHO2_A0(unit)) { 
                    SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i, schan >> 4));
                } else if (SOC_IS_J2P(unit) || SOC_IS_J2X(unit)) {
                    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FMAC_BROADCAST_IDSr, i, SBUS_BROADCAST_IDf, schan));
                } else {
                    SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i, schan));
                }
            }
        }

        
        nof_blocks = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
        for (i=0; i<nof_blocks; i++)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_fabric_fsrd_block_valid_get(unit, i, &block_valid));
            if (block_valid)
            {
                chain = dnx_data_fabric.blocks.fsrd_sbus_chain_get(unit, i)->chain;
                schan = SOC_BLOCK_INFO(unit, SOC_BLOCK_INSTANCES(unit, SOC_BLK_BRDC_FSRD).first_blk_instance + chain).schan;

                if (SOC_IS_JERICHO2_A0(unit)) { 
                    SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, schan >> 4));
                } else if (SOC_IS_J2P(unit) || SOC_IS_J2X(unit)) {
                    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSRD_BROADCAST_IDSr, i, SBUS_BROADCAST_IDf, schan));
                }else {
                    SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, schan));
                }
            }
        }
    }

    if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
    {
        
        
        dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
        nof_hbms = dnx_data_dram.general_info.max_nof_drams_get(unit);
        nof_channels_in_hbm = dnx_data_dram.general_info.nof_channels_get(unit);
        SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_index)
        {
            for(int channel = 0; channel < nof_channels_in_hbm; ++channel)
            {
                chain = dnx_data_dram.hbm.hbc_sbus_chain_get(unit, channel + hbm_index * nof_channels_in_hbm)->sbus_chain;
                schan = SOC_BLOCK_INFO(unit, SOC_BLOCK_INSTANCES(unit, SOC_BLK_BRDC_HBC).first_blk_instance + chain).schan;

                if (SOC_IS_J2P(unit) || SOC_IS_J2X(unit)) {
                    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, HBC_BROADCAST_IDSr, channel + hbm_index * nof_channels_in_hbm,
                                                            SBUS_BROADCAST_IDf, schan));
                } else {
                    SHR_IF_ERR_EXIT(WRITE_HBC_SBUS_BROADCAST_IDr(unit, channel + hbm_index * nof_channels_in_hbm, schan));
                }
            }
        }

        
        SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_index)
        {
            SHR_IF_ERR_EXIT(WRITE_HBC_SBUS_LAST_IN_CHAINr(unit, dnx_data_dram.hbm.hbc_last_in_chain_get(unit, dram_bitmap[0], hbm_index)->is_last_in_chain, 1));
        }
    }
#ifdef BCM_88850_A0
    if (SOC_IS_J2P(unit))
    {
        
        soc_reg_above_64_val_t data;
        for (i = 0; j2p_per_block_config_reg[i] != INVALIDr; i++)
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, j2p_per_block_config_reg[i], REG_PORT_ANY, 0, data));
            soc_reg_above_64_field32_set(unit, j2p_per_block_config_reg[i], data, ITEM_19_30f, 0x4);
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, j2p_per_block_config_reg[i], REG_PORT_ANY, 0, data));
        }
        
        if (dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
        {
            dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;
            nof_hbms = dnx_data_dram.general_info.max_nof_drams_get(unit);
            nof_channels_in_hbm = dnx_data_dram.general_info.nof_channels_get(unit);
            
            SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_index)
            {
                for(int channel = 0; channel < nof_channels_in_hbm; ++channel)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, HBC_RESERVED_MTCPr, channel + hbm_index * nof_channels_in_hbm, 0, data));
                    soc_reg_above_64_field32_set(unit, HBC_RESERVED_MTCPr, data, ITEM_19_30f, 0x4);
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, HBC_RESERVED_MTCPr, channel + hbm_index * nof_channels_in_hbm, 0, data));
                }
            }
        }
    }
#endif
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_dnx2_fmac_rx_gate_enable(
    int unit,
    int enable)
{
    uint32 fmac_rx_gate_reg_val = 0;
    uint32 fmac_id = 0;
    uint32 link_in_fmac = 0;
    int instance_enable = 0;
    uint32 nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    uint32 nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    for (fmac_id = 0; fmac_id < nof_instances_fmac; ++fmac_id)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_fmac_block_valid_get(unit, fmac_id, &instance_enable));
        if (instance_enable)
        {
            for (link_in_fmac = 0; link_in_fmac < nof_links_in_fmac; ++link_in_fmac)
            {
                SHR_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr
                                (unit, fmac_id, link_in_fmac, &fmac_rx_gate_reg_val));
                soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &fmac_rx_gate_reg_val, AUTO_DOC_NAME_53f,
                                enable);
                SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr
                                (unit, fmac_id, link_in_fmac, fmac_rx_gate_reg_val));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnx_fmac_rx_gate_enable(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX2(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx2_fmac_rx_gate_enable(unit, enable));
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_dnx_ilkn_data_port_enable(
    int unit,
    soc_pbmp_t * restore_pbmp,
    int enable)
{
    soc_pbmp_t ilkn_pbmp;
    soc_port_t ilkn_port;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_Q2A(unit))
    {
        if (enable)
        {
            BCM_PBMP_ITER(*restore_pbmp, ilkn_port)
            {
                
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, ilkn_port, IMB_PORT_ENABLE_F_SKIP_FEC|IMB_PORT_ENABLE_F_SKIP_PORTMOD , enable));
            }
        }
        else
        {
            SOC_PBMP_CLEAR(*restore_pbmp);
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                       (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 
                        DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY|DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK, &ilkn_pbmp));
            BCM_PBMP_ITER(ilkn_pbmp, ilkn_port)
            {
                int port_enable;
                SHR_IF_ERR_EXIT(imb_port_enable_get(unit, ilkn_port, &port_enable));
                if (port_enable)
                {
                    
                    SHR_IF_ERR_EXIT(imb_port_enable_set(unit, ilkn_port, IMB_PORT_ENABLE_F_SKIP_FEC|IMB_PORT_ENABLE_F_SKIP_PORTMOD , enable));
                    SOC_PBMP_PORT_ADD(*restore_pbmp, ilkn_port);
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}



int dnx_redo_soft_reset_soft_init(int unit, int perform_soft_reset, int without_fabric, int without_ile)
{
    int control_cells_on, data_on;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;
    int linkscan_interval;
    dnx_hbmc_mrs_values_t mrs_values;
    int temp_monitoring_is_active;
    int temp_monitoring_paused = FALSE;
    dnx_gddr6_per_channel_info_t dynamic_calibration_save;
    int skip_standalone_state_modify = FALSE;
    int enable_traffic = TRUE;
    int fabric_blocks_exist = dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist);
    soc_pbmp_t ilkn_restore_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &linkscan_interval));
    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, 0));
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
    SHR_IF_ERR_EXIT(soc_counter_stop(unit));

    
    SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_is_active(unit, &temp_monitoring_is_active));
    if (temp_monitoring_is_active)
    {
        SHR_IF_ERR_EXIT(dnx_hbmc_temp_monitor_pause(unit));
        temp_monitoring_paused = TRUE;
    }

    
    
    
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_save(unit, &mrs_values));


    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &data_on));
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_on));

    if (data_on) {
        enable_traffic = FALSE;
        SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, enable_traffic, skip_standalone_state_modify));
        sal_usleep(2000);
    }


    
    SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_save_and_disable(unit, &dynamic_calibration_save));

    if (!without_fabric && control_cells_on) {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, FALSE, 1));
    }

    
#if defined(INCLUDE_KBP)
    if (without_ile == 0)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_device_interface_enable_link(unit, 0));
    }
#endif

    if (perform_soft_reset) {
        SHR_IF_ERR_EXIT(soc_dnx_pre_reset_pll_init(unit));
        SHR_IF_ERR_EXIT(soc_dnx_soft_reset(unit));
        SHR_IF_ERR_EXIT(soc_dnx_post_reset_pll_init(unit));
    }

    if (without_fabric == 0 && fabric_blocks_exist)
    {
        
        SHR_IF_ERR_EXIT(soc_dnx_fmac_rx_gate_enable(unit, 1));
    }

    if (without_ile == 0)
    {
        SOC_PBMP_CLEAR(ilkn_restore_pbmp);
        SHR_IF_ERR_EXIT(soc_dnx_ilkn_data_port_enable(unit, &ilkn_restore_pbmp, 0));
    }

    SHR_IF_ERR_EXIT(soc_dnx_soft_init(unit, without_fabric, without_ile));

    if (without_ile == 0)
    {
        SHR_IF_ERR_EXIT(soc_dnx_ilkn_data_port_enable(unit, &ilkn_restore_pbmp, 1));
    }

    if (without_fabric == 0 && fabric_blocks_exist)
    {
        
        SHR_IF_ERR_EXIT(soc_dnx_fmac_rx_gate_enable(unit, 0));
    }

#if defined(INCLUDE_KBP)
    if (without_ile == 0)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_device_interface_enable_link(unit, 1));
    }
#endif


    
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit) || SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
    {
        soc_port_t port;
        soc_pbmp_t nif_ports;
        soc_pbmp_t flexe_mac_ports;
        soc_pbmp_t tm_egress_queuing_ports;
        dnx_algo_port_info_s port_info;
        int has_tx_speed;
        int block_index;
        uint32 reg_val[1];
        soc_reg_t reg_name;
        dnx_algo_port_ethu_access_info_t ethu_access;
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC, 0, &flexe_mac_ports));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &tm_egress_queuing_ports));
        _SHR_PBMP_OR(nif_ports, flexe_mac_ports);
        _SHR_PBMP_OR(nif_ports, tm_egress_queuing_ports);
        _SHR_PBMP_ITER(nif_ports, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));

            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info) && has_tx_speed)
            {
                SHR_IF_ERR_EXIT(dnx_egr_tm_port_credit_init(unit, port));
            }
            
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, FALSE) && !DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_access));
                if (ethu_access.imb_type == imbDispatchTypeImb_cdu)
                {
                    block_index = ethu_access.ethu_id;
                    reg_name = CDU_TX_PORTS_SRSTNr;
                }
                else
                {
                    block_index = ethu_access.ethu_id - dnx_data_nif.eth.cdu_nof_get(unit);
                    reg_name = CLU_TX_PORTS_SRSTNr;
                }
                SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg_name, block_index, 0, reg_val));
                
                if (SHR_BITGET(reg_val, ethu_access.first_lane_in_port))
                {
                    SHR_BITCLR(reg_val, ethu_access.first_lane_in_port);
                    SHR_IF_ERR_EXIT(soc_reg32_set(unit, reg_name, block_index, 0, reg_val[0]));

                    SHR_BITSET(reg_val, ethu_access.first_lane_in_port);
                    SHR_IF_ERR_EXIT(soc_reg32_set(unit, reg_name, block_index, 0, reg_val[0]));
                }
            }
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_gddr6_dynamic_calibration_restore(unit, &dynamic_calibration_save));

    if (!without_fabric && control_cells_on) {
        if (!without_fabric) {
            
            sal_usleep(4000);
            if (dnx_data_fabric.links.feature_get(unit, dnx_data_fabric_links_isolation_needed_before_disable_full_reset) && 
                dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
            {
                sal_usleep(46000);
            }
        }
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, TRUE, 1));
        
        skip_standalone_state_modify = TRUE;
    }

    if (data_on) {
        if (without_fabric == TRUE)
        {
            skip_standalone_state_modify = TRUE;
        }
        enable_traffic = TRUE;
        
        SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, enable_traffic, skip_standalone_state_modify));
    }

    
    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_restore(unit, &mrs_values));

    
    SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, linkscan_interval));

exit:
    
    if (temp_monitoring_paused == TRUE)
    {
        SHR_IF_ERR_CONT(dnx_hbmc_temp_monitor_resume(unit));
    }

    SHR_FUNC_EXIT;
}


int
soc_dnx_device_reset(int unit, int mode, int flag, int action)
{
    int without_ile = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_IS_DNX(unit)) {
        SHR_EXIT_WITH_LOG(_SHR_E_UNAVAIL, "Jericho 2 function. Invalid Device\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    switch (mode) {
    case SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET:
        if ( flag & SOC_DNX_RESET_MODE_FLAG_WITHOUT_ILE)
        {
            without_ile = 1;
        }
        SHR_IF_ERR_EXIT(dnx_redo_soft_reset_soft_init(unit, 0, 1, without_ile));
        break;
    case SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        if ( flag & SOC_DNX_RESET_MODE_FLAG_WITHOUT_ILE)
        {
            without_ile = 1;
        }
        SHR_IF_ERR_EXIT(dnx_redo_soft_reset_soft_init(unit, 0, 0, without_ile));
        break;
    case SOC_DNX_RESET_MODE_REG_ACCESS:
        SHR_IF_ERR_EXIT(soc_dnxc_only_enough_for_access_init(unit));
        break;
    default:
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "Unsupported reset mode, see 'dnx reset' command for more reset options\n"
                "%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

int soc_dnx_init_reset(
    int unit,
    int reset_action)
{
    int disable_hard_reset = 0x0;

    SHR_FUNC_INIT_VARS(unit);

    
    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
    
    if (disable_hard_reset == 0) {
        SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, reset_action));
    }


    
    SHR_IF_ERR_EXIT(soc_dnx_init_reset_cmic_regs(unit));

exit:
    SHR_FUNC_EXIT;
}


#endif
#ifdef JER_2_TO_DO

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <soc/sand/shrextend/shrextend_debug.h>

int
soc_dnx_device_reset(int unit, int mode, int flag, int action)
{
  SHR_FUNC_INIT_VARS(unit);
  SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
exit:
  SHR_FUNC_EXIT;

}
#endif

