/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifdef BCM_DNX_SUPPORT


#include <soc/dnxc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/drv.h>
#include <soc/drv.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/link/link.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/counter.h>
#if defined(INCLUDE_KBP)
#include <bcm_int/dnx/kbp/kbp_recover.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT




#ifdef JER_2_TO_DO

int soc_dnx_reset_blocks_poll_init_finish(int unit)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);
    
    for(i = 0; i < dnx_data_device.general.nof_cores_get(unit); i++) {
        
        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRE_RESET_STATUS_REGISTERr, i, 0, CTXT_STATUS_INIT_DONEf, 0x1));
        if (!SOC_IS_QAX(unit)) { 
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRE_RESET_STATUS_REGISTERr, i, 0, CTXT_MAP_INIT_DONEf, 0x1));

            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, CONTEXT_STATUS_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, CHUNK_STATUS_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, WORD_INDEX_FIFO_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IDR_RESET_STATUS_REGISTERr, i, 0, FREE_PCB_FIFO_INIT_DONEf, 0x1));

            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, FPF_0_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, FPF_1_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, IS_FPF_0_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, IS_FPF_1_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, DESTINATION_TABLE_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, LAG_MAPPING_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, LAG_TO_LAG_RANGE_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, MCDB_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, STACK_FEC_RESOLVE_INIT_DONEf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IRR_RESET_STATUS_REGISTERr, i, 0, STACK_TRUNK_RESOLVE_INIT_DONEf, 0x1));

            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, IQC_INITf, 0x0));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, STE_INITf, 0x0));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IQM_IQM_INITr, i, 0, PDM_INITf, 0x0));

            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MRPS_INIT_SEQ_ONr, i, 0, MCDA_INIT_SEQ_ONf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MTRPS_EM_INIT_SEQ_ONr, i, 0, MCDA_INIT_SEQ_ONf, 0x1));
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IHB_ISEM_RESET_STATUS_REGISTERr, i, 0, ISEM_KEYT_RESET_DONEf, 0x1));
        }

        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, IPS_IPS_GENERAL_CONFIGURATIONSr, i, 0, IPS_INIT_TRIGGERf, 0x0));
        
        
        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EGQ_EGQ_BLOCK_INIT_STATUSr, i, 0, EGQ_BLOCK_INITf, 0x0));
    }
    
    if (!SOC_IS_FLAIR(unit)) {
        
        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMA_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OEMA_KEYT_RESET_DONEf, 0x1));
        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_A_OEMB_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, OEMB_KEYT_RESET_DONEf, 0x1));

        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, PPDB_B_LARGE_EM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, LARGE_EM_KEYT_RESET_DONEf, 0x1));

        if (!SOC_IS_JERICHO_PLUS_A0(unit)) {
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_GLEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, GLEM_KEYT_RESET_DONEf, 0x1)); 
            SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, EDB_ESEM_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, ESEM_KEYT_RESET_DONEf, 0x1));
        }

        SHR_IF_ERR_EXIT(soc_dpp_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, OAMP_REMOTE_MEP_EXACT_MATCH_RESET_STATUS_REGISTERr, REG_PORT_ANY, 0, REMOTE_MEP_EXACT_MATCH_KEYT_RESET_DONEf, 0x1));
    }

exit:
    SHR_FUNC_EXIT;  
}
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

    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_fabric.links.nof_links_get(unit))
    {
        
        nof_blocks = SOC_MEM_BLOCK_MAX(unit, FMAC_RX_RRR_CTRLm) - SOC_MEM_BLOCK_MIN(unit, FMAC_RX_RRR_CTRLm);
        for (i=0; i<=nof_blocks; i++)
        {
            if (SOC_IS_JERICHO2_A0(unit)) { 
                SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i, (SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan >> 4)));
            } else if (SOC_IS_J2P(unit)) {
                SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FMAC_BROADCAST_IDSr, i, SBUS_BROADCAST_IDf, (SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan)));
            } else {
                SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, i, (SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan)));
            }
        }

        
        nof_blocks = SOC_MEM_BLOCK_MAX(unit, FSRD_FSRD_PROM_MEMm) - SOC_MEM_BLOCK_MIN(unit, FSRD_FSRD_PROM_MEMm);
        for (i=0; i<=nof_blocks; i++)
        {
            if (SOC_IS_JERICHO2_A0(unit)) { 
                SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, (SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan >> 4)));
            } else if (SOC_IS_J2P(unit)) {
                SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, FSRD_BROADCAST_IDSr, i, SBUS_BROADCAST_IDf, (SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan)));
            }else {
                SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, (SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan)));
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
                if (SOC_IS_J2P(unit)) {
                    SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, HBC_BROADCAST_IDSr, 
                                                            channel + hbm_index * nof_channels_in_hbm, 
                                                            SBUS_BROADCAST_IDf, (SOC_BLOCK_INFO(unit,BRDC_HBC_BLOCK(unit)).schan)));
                } else {
                    SHR_IF_ERR_EXIT(WRITE_HBC_SBUS_BROADCAST_IDr(unit, channel + hbm_index * nof_channels_in_hbm, (SOC_BLOCK_INFO(unit,BRDC_HBC_BLOCK(unit)).schan)));
                }
            }
        }

        
        SHR_BIT_ITER(dram_bitmap, nof_hbms, hbm_index)
        {
            SHR_IF_ERR_EXIT(WRITE_HBC_SBUS_LAST_IN_CHAINr(unit, dnx_data_dram.hbm.hbc_last_in_chain_get(unit, dram_bitmap[0], hbm_index)->is_last_in_chain, 1));
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
    bcm_pbmp_t fabric_ports_bitmap;
    bcm_port_t port = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    if (!without_fabric) {
        BCM_PBMP_CLEAR(fabric_ports_bitmap);
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
        BCM_PBMP_ITER(fabric_ports_bitmap, port)
        {
            SHR_IF_ERR_EXIT(portmod_port_electrical_idle_enable_set(unit, port, 1));
        }
    }

    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &data_on));
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_get(unit, bcmFabricControlCellsEnable, &control_cells_on));
    SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &linkscan_interval));
    SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

    if (data_on) {
        SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, 0));
        sal_usleep(2000);
    }

    if (without_fabric && control_cells_on) {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, FALSE, 1));
    }

    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, 0));
    SHR_IF_ERR_EXIT(soc_counter_stop(unit));

    
#if defined(INCLUDE_KBP)
    if (without_ile == 0)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_device_interface_enable_link(unit, 0));
    }
#endif

    if (perform_soft_reset) {
        SHR_IF_ERR_EXIT(soc_dnx_soft_reset(unit));
        SHR_IF_ERR_EXIT(soc_dnx_pll_init(unit));
    }

    SHR_IF_ERR_EXIT(soc_dnx_soft_init(unit, without_fabric, without_ile));

#if defined(INCLUDE_KBP)
    if (without_ile == 0)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_device_interface_enable_link(unit, 1));
    }
#endif


    
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit) || SOC_IS_J2P(unit))
    {
        soc_port_t port;
        soc_pbmp_t nif_ports;
        soc_pbmp_t flexe_mac_ports;
        soc_pbmp_t tm_egress_queuing_ports;
        dnx_algo_port_info_s port_info;
        int enable;

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC, 0, &flexe_mac_ports));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &tm_egress_queuing_ports));
        _SHR_PBMP_OR(nif_ports, flexe_mac_ports);
        _SHR_PBMP_OR(nif_ports, tm_egress_queuing_ports);
        _SHR_PBMP_ITER(nif_ports, port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, port));
            }
            
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info))
            {
                SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port, &enable));
                SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, port, enable));
            }
        }
    }

    SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, linkscan_interval));

    if (without_fabric && control_cells_on) {
        SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, TRUE, 1));
    }

    if (data_on) {
        SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_set(unit, 1));
    }
    if (!without_fabric) {
        BCM_PBMP_ITER(fabric_ports_bitmap, port)
        {
            SHR_IF_ERR_EXIT(portmod_port_electrical_idle_enable_set(unit, port, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

extern int
dnx_init_only_enough_for_access(int unit, int action);


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
        SHR_IF_ERR_EXIT(dnx_init_only_enough_for_access(unit, action));
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

    
    if (SOC_CONTROL(unit)->schanMutex == NULL) {
        SHR_IF_ERR_EXIT(soc_dnx_mutexes_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


#endif
#ifdef JER_2_TO_DO

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/shrextend/shrextend_debug.h>

int
soc_dnx_device_reset(int unit, int mode, int flag, int action)
{
  SHR_FUNC_INIT_VARS(unit);
  SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
exit:
  SHR_FUNC_EXIT;

}
#endif

