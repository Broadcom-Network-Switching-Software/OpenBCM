/*

 * 

 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>


#include <soc/mcm/driver.h>     
#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/linkctrl.h>

#include <soc/dcmn/fabric.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/dcmn/dcmn_cmic.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mem.h>

#ifdef BCM_88750_SUPPORT
#include <soc/dfe/fe1600/fe1600_drv.h>
#include <soc/dfe/fe1600/fe1600_link.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#endif



soc_interrupt_fn_t dfe_intr_fn = soc_intr;


int soc_dfe_tbl_is_dynamic(int unit, soc_mem_t mem)
{
    return MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_tbl_is_dynamic, (unit, mem));
}


void soc_dfe_tbl_mark_cachable(int unit) {
    soc_mem_t mem;
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        };
        
        if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem))
        {
            continue;
        }
        
        if (soc_dfe_tbl_is_dynamic(unit,mem))
        {
            continue;
        }
        

        
        if (mem == RTP_MCTm) {
            continue;
        }
        if (mem == RTP_SLSCTm) {
            if(soc_dfe_load_balancing_mode_destination_unreachable != SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
                continue;
            }
        }
        if (mem == FSRD_FSRD_WL_EXT_MEMm) {
            continue;
        }

        
        if ((mem == SER_ACC_TYPE_MAPm) ||
            (mem ==  SER_MEMORYm) ||
            (mem == SER_RESULT_0m) ||
            (mem == SER_RESULT_1m) ||
            (mem == SER_RESULT_DATA_0m) ||
            (mem ==  SER_RESULT_DATA_1m) ||
            (mem ==  SER_RESULT_EXPECTED_0m) ||
            (mem == SER_RESULT_EXPECTED_1m)) {
            continue;
        }
        SOC_MEM_INFO(unit, mem).flags |= SOC_MEM_FLAG_CACHABLE;
    }
}


void dfe_local_soc_intr(void *_unit)
{
    dfe_intr_fn(_unit);
}


extern soc_controlled_counter_t soc_dfe_controlled_counter[];


int
soc_dfe_misc_init(int unit)
{
    return SOC_E_NONE;
}

int
soc_dfe_mmu_init(int unit)
{
    return SOC_E_NONE;
}

soc_functions_t soc_dfe_drv_funs = {
    soc_dfe_misc_init,
    soc_dfe_mmu_init,
    NULL,
    NULL,
    NULL,
};

STATIC int
soc_dfe_info_soc_properties(int unit) 
{

    SOCDNX_INIT_FUNC_DEFS;    

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_general_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_chip_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_multicast_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_fabric_pipes_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_access_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_port_read(unit));

    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_fabric_cell_read(unit));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_drv_soc_properties_fabric_routing_read(unit));

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_drv_soc_properties_validate));
exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_fabric_pipes_read(int unit)
{    
    int nof_pipes;
    int i;
    int j;
    soc_dcmn_fabric_pipe_map_t fabric_pipe_map_valid_config[SOC_DCMN_FABRIC_PIPE_NUM_OF_FABRIC_PIPE_VALID_CONFIGURATIONS];
    int is_valid=1;
    char *str;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DFE_CONFIG(unit).is_dual_mode = soc_dfe_property_get(unit, spn_IS_DUAL_MODE,  0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).system_is_dual_mode_in_system = soc_dfe_property_get(unit, spn_SYSTEM_IS_DUAL_MODE_IN_SYSTEM,  0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).system_is_single_mode_in_system = soc_dfe_property_get(unit, spn_SYSTEM_IS_SINGLE_MODE_IN_SYSTEM,  0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).system_contains_multiple_pipe_device = soc_dfe_property_get(unit, spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE,  0, SOC_DFE_PROPERTY_UNAVAIL);
    str = soc_dfe_property_get_str(unit, spn_FABRIC_TDM_PRIORITY_MIN,0,NULL);
    rv = soc_dfe_property_str_to_enum(unit, spn_FABRIC_TDM_PRIORITY_MIN, soc_dfe_property_str_enum_fabric_tdm_priority_min, str, &SOC_DFE_CONFIG(unit).fabric_tdm_priority_min);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dcmn_fabric_pipe_map_initalize_valid_configurations(unit, 
                                                                     SOC_DFE_CONFIG(unit).fabric_tdm_priority_min == SOC_DFE_FABRIC_TDM_PRIORITY_NONE ? SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES : SOC_DFE_CONFIG(unit).fabric_tdm_priority_min, 
                                                                     fabric_pipe_map_valid_config);
    SOCDNX_IF_ERR_EXIT(rv);
    if (!SOC_IS_BCM88776(unit))  {      
        nof_pipes=soc_dfe_property_get(unit,spn_FABRIC_NUM_PIPES,0, SOC_DFE_PROPERTY_UNAVAIL);
    } else {
        nof_pipes=1;
    }
    SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes=nof_pipes;

    if (nof_pipes == SOC_DFE_PROPERTY_UNAVAIL) 
    {
        SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes=(SOC_DFE_CONFIG(unit).is_dual_mode? 2:1);
        SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type = (SOC_DFE_CONFIG(unit).is_dual_mode? soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm:soc_dcmn_fabric_pipe_map_single);

        for (i=0;i<SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES;i++)
        {
            SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[i]=SOC_DFE_PROPERTY_UNAVAIL;
            SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[i]=SOC_DFE_PROPERTY_UNAVAIL;
        }

        SOC_EXIT; 
    }

    if (nof_pipes == 1)
    {
        
        for (i=0;i<SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES;i++)
        {
            SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[i]=0;
            SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[i]=0;
        }
        SOC_EXIT;
    }
    
    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[0] = soc_dfe_property_suffix_num_get(unit, 0, spn_FABRIC_PIPE_MAP, "uc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[0] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 0 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[1] = soc_dfe_property_suffix_num_get(unit, 1, spn_FABRIC_PIPE_MAP, "uc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[1] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 1 isn't configued- if number of pipes > 1, all priorities must be configued")));
    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[2] = soc_dfe_property_suffix_num_get(unit, 2, spn_FABRIC_PIPE_MAP, "uc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[2] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 2 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[3] = soc_dfe_property_suffix_num_get(unit, 3, spn_FABRIC_PIPE_MAP, "uc",1, SOC_DFE_PROPERTY_UNAVAIL); 
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[3] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("unicast priority 3 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[0] = soc_dfe_property_suffix_num_get(unit, 0, spn_FABRIC_PIPE_MAP, "mc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[0] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 0 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[1] = soc_dfe_property_suffix_num_get(unit, 1, spn_FABRIC_PIPE_MAP, "mc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[1] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 1 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[2] = soc_dfe_property_suffix_num_get(unit, 2, spn_FABRIC_PIPE_MAP, "mc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[2] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 2 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }

    SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[3] = soc_dfe_property_suffix_num_get(unit, 3, spn_FABRIC_PIPE_MAP, "mc",1, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[3] == SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("multicast priority 3 isn't configued- if number of pipes > 1, all priorities must be configued")));

    }


    
    for (i=0;i<4;i++)
    {
        if ( (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[i] >= nof_pipes) || (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[i] >= nof_pipes) ||
             (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[i] < 0) || (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[i] < 0)  )
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid pipe number - more than configured")));
        }
    }
    

    if (nof_pipes!=1)
    {
        
        for (i=0;i<SOC_DCMN_FABRIC_PIPE_NUM_OF_FABRIC_PIPE_VALID_CONFIGURATIONS;i++)
        {
            if (nof_pipes==fabric_pipe_map_valid_config[i].nof_pipes)
            {
                is_valid=1;
                for (j=0;j<SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES;j++)
                {
                    is_valid= (is_valid && (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[j]==fabric_pipe_map_valid_config[i].config_uc[j]) &&
                                           (SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[j]==fabric_pipe_map_valid_config[i].config_mc[j]));
                }
                if (is_valid)
                {
                    SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type = fabric_pipe_map_valid_config[i].mapping_type;
                    break;
                }
            }
        }
        
    }

    if (!is_valid) 
    {
        if (SOC_DFE_CONFIG(unit).custom_feature_lab==0)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid configuration")));
        }
        else
        {
            SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type = (nof_pipes == 2) ? soc_dcmn_fabric_pipe_map_dual_custom:soc_dcmn_fabric_pipe_map_triple_custom;
            LOG_CLI((BSL_META_U(unit,
                                "warning: using an invalid configuration for fabric pipes")));
        }
    }

    if (SOC_DFE_FABRIC_PIPES_CONFIG(unit).mapping_type == soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc &&
        SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable == 1)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid Configuration - cannot configure triple pipe uc,hp-mc,lp-mc mode & mc priority map")));
    }
    


exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_access_read(int unit)
{
       int rv;
    SOCDNX_INIT_FUNC_DEFS;

    
    soc_sand_access_conf_get(unit);
    SOC_DFE_CONFIG(unit).mdio_int_dividend = soc_dfe_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).mdio_int_divisor = soc_dfe_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 0, SOC_DFE_PROPERTY_UNAVAIL);

    
    if ((SOC_DFE_CONFIG(unit).run_mbist = soc_dfe_property_get(unit, spn_BIST_ENABLE, 0, SOC_DFE_PROPERTY_UNAVAIL))) {
        if (SOC_DFE_CONFIG(unit).run_mbist != 2) {
            SOC_DFE_CONFIG(unit).run_mbist = 1;
        }
    }

    
    SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable = soc_dfe_property_get(unit, spn_FABRIC_CELL_FIFO_DMA_ENABLE, 0, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable != SOC_DFE_PROPERTY_UNAVAIL && SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable)
    {
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size = soc_dfe_property_get(unit, spn_FABRIC_CELL_FIFO_DMA_BUFFER_SIZE, 0, SOC_DFE_PROPERTY_UNAVAIL);
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout = soc_dfe_property_get(unit, spn_FABRIC_CELL_FIFO_DMA_TIMEOUT, 0, SOC_DFE_PROPERTY_UNAVAIL);
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold = soc_dfe_property_get(unit, spn_FABRIC_CELL_FIFO_DMA_THRESHOLD, 0, SOC_DFE_PROPERTY_UNAVAIL);
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fifo_dma_fabric_cell_validate, (unit));
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size  = SOC_DFE_PROPERTY_UNAVAIL;
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout = SOC_DFE_PROPERTY_UNAVAIL;
        SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold = SOC_DFE_PROPERTY_UNAVAIL;
    }



exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_chip_read(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (soc_property_get_str(unit, spn_SYSTEM_REF_CORE_CLOCK) != NULL) {
        
        SOC_DFE_CONFIG(unit).system_ref_core_clock = soc_dfe_property_get(unit, spn_SYSTEM_REF_CORE_CLOCK, 0, SOC_DFE_PROPERTY_UNAVAIL) * 1000;
    } else {
        
        SOC_DFE_CONFIG(unit).system_ref_core_clock = soc_dfe_property_suffix_num_get(unit, 0, spn_SYSTEM_REF_CORE_CLOCK, "khz",1 , SOC_DFE_PROPERTY_UNAVAIL); 
        if (SOC_DFE_CONFIG(unit).system_ref_core_clock ==  SOC_DFE_PROPERTY_UNAVAIL) 
        {
            SOC_DFE_CONFIG(unit).system_ref_core_clock = soc_dfe_property_get(unit, spn_SYSTEM_REF_CORE_CLOCK, 0, SOC_DFE_PROPERTY_UNAVAIL) * 1000;
        }
    }

    if (soc_property_get_str(unit, spn_CORE_CLOCK_SPEED) != NULL) {
        
        SOC_DFE_CONFIG(unit).core_clock_speed  = soc_dfe_property_get(unit, spn_CORE_CLOCK_SPEED, 0, SOC_DFE_PROPERTY_UNAVAIL) * 1000;
    } else {
        
        SOC_DFE_CONFIG(unit).core_clock_speed = soc_dfe_property_suffix_num_get(unit,0, spn_CORE_CLOCK_SPEED, "khz", 1 , SOC_DFE_PROPERTY_UNAVAIL);
        if (SOC_DFE_CONFIG(unit).core_clock_speed ==  SOC_DFE_PROPERTY_UNAVAIL ) 
        {
            SOC_DFE_CONFIG(unit).core_clock_speed = soc_dfe_property_get(unit, spn_CORE_CLOCK_SPEED, 0, SOC_DFE_PROPERTY_UNAVAIL ) * 1000;
        }
    }


    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_multicast_read(int unit)
{
    char *str;
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    str = soc_dfe_property_get_str(unit, spn_FABRIC_MULTICAST_MODE,0,NULL);
    rv = soc_dfe_property_str_to_enum(unit, spn_FABRIC_MULTICAST_MODE, soc_dfe_property_str_enum_fabric_multicast_mode, str, &SOC_DFE_CONFIG(unit).fabric_multicast_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    if (soc_feature(unit, soc_feature_fe_mc_id_range))
    {
        str = soc_dfe_property_get_str(unit, spn_FE_MC_ID_RANGE,0,NULL);
        rv = soc_dfe_property_str_to_enum(unit, spn_FE_MC_ID_RANGE, soc_dfe_property_str_enum_fe_mc_id_range, str, &SOC_DFE_CONFIG(unit).fe_mc_id_range);
    }
    else
    {
        SOC_DFE_CONFIG(unit).fe_mc_id_range = soc_dfe_multicast_table_mode_64k;
    }
    SOCDNX_IF_ERR_EXIT(rv);
    if (soc_feature(unit, soc_feature_fe_mc_priority_mode_enable))
    {
        SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable = soc_dfe_property_get(unit, spn_FE_MC_PRIORITY_MAP_ENABLE,  0,  SOC_DFE_PROPERTY_UNAVAIL);
    }
    else
    {
        SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable = 1; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_port_read(int unit)
{
    int lcpll, rc;
    SOCDNX_INIT_FUNC_DEFS;

    for (lcpll = 0; lcpll < SOC_DFE_NOF_LCPLL; lcpll++)
    {
        SOC_DFE_CONFIG(unit).fabric_port_lcpll_in[lcpll] = soc_dfe_property_suffix_num_get(unit, lcpll, spn_SERDES_FABRIC_CLK_FREQ, "in", 0, SOC_DFE_PROPERTY_UNAVAIL);
        SOC_DFE_CONFIG(unit).fabric_port_lcpll_out[lcpll] = soc_dfe_property_suffix_num_get(unit, lcpll, spn_SERDES_FABRIC_CLK_FREQ, "out", 0, SOC_DFE_PROPERTY_UNAVAIL);
    }

    if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) || SOC_DFE_IS_FE2(unit))
    {
        SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_26 = soc_dfe_property_suffix_num_get_only_suffix(unit, 26, spn_SERDES_FABRIC_CLK_FREQ, "in_quad", 1, -1); 
        SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_35 = soc_dfe_property_suffix_num_get_only_suffix(unit, 35, spn_SERDES_FABRIC_CLK_FREQ, "in_quad", 1, -1);
        SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_26 = soc_dfe_property_suffix_num_get_only_suffix(unit, 26, spn_SERDES_FABRIC_CLK_FREQ, "out_quad", 1, -1); 
        SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_35 = soc_dfe_property_suffix_num_get_only_suffix(unit, 35, spn_SERDES_FABRIC_CLK_FREQ, "out_quad", 1, -1);
        if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_26 != SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_26) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Fabric clock in frequency must be equal to out frequency for quad 26")));
        }
        if (SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_35 != SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_35) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Fabric clock in frequency must be equal to out frequency for quad 35")));
        }
    }

    SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable = soc_dfe_property_get(unit, spn_SERDES_MIXED_RATE_ENABLE,  0, SOC_DFE_PROPERTY_UNAVAIL);
    if (SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable !=  SOC_DFE_PROPERTY_UNAVAIL) 
    {
        if (SOC_IS_FE1600_A0(unit) && SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Property %s is not supported by Fe1600_A0"),spn_SERDES_MIXED_RATE_ENABLE));
        }
    }

    SOC_DFE_CONFIG(unit).fabric_optimize_patial_links = soc_dfe_property_get(unit, spn_FABRIC_OPTIMIZE_PARTIAL_LINKS, 0, SOC_DFE_PROPERTY_UNAVAIL);

    SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate = soc_dfe_property_get(unit, spn_FABRIC_MAC_BUCKET_FILL_RATE,  0, SOC_DFE_PROPERTY_UNAVAIL);

    if (SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate != SOC_DFE_PROPERTY_UNAVAIL)
    {
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_bucket_fill_rate_validate,(unit, SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate)));
    }

    rc = _soc_dfe_drv_soc_property_serdes_qrtt_read(unit);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_drv_soc_properties_fabric_cell_read(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;


    SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system = soc_dfe_property_get(unit, spn_SYSTEM_IS_VCS_128_IN_SYSTEM,  0, SOC_DFE_PROPERTY_UNAVAIL);

    SOC_DFE_CONFIG(unit).system_is_fe600_in_system = soc_dfe_property_get(unit, spn_SYSTEM_IS_FE600_IN_SYSTEM,  0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).fabric_merge_cells = soc_dfe_property_get(unit, spn_FABRIC_MERGE_CELLS,  0, SOC_DFE_PROPERTY_UNAVAIL);
    SOC_DFE_CONFIG(unit).fabric_TDM_fragment = soc_dfe_property_get(unit, spn_FABRIC_TDM_FRAGMENT,  0, SOC_DFE_PROPERTY_UNAVAIL);

    SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe = soc_dfe_property_get(unit, spn_FABRIC_TDM_OVER_PRIMARY_PIPE,  0, SOC_DFE_PROPERTY_UNAVAIL);

    if (!SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe) {
        SOC_DFE_CONFIG(unit).vcs128_unicast_priority = soc_dfe_property_get(unit, spn_VCS128_UNICAST_PRIORITY, 1,  DFE_VCS128_UNICAST_PRIORITY_DEFAULT);
        if ( SOC_DFE_CONFIG(unit).vcs128_unicast_priority != SOC_DFE_PROPERTY_UNAVAIL) 
        {
            if(!DFE_IS_PRIORITY_VALID(SOC_DFE_CONFIG(unit).vcs128_unicast_priority)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Illegal priority %d"),SOC_DFE_CONFIG(unit).vcs128_unicast_priority));
            } else if (SOC_DFE_CONFIG(unit).vcs128_unicast_priority >= soc_dfe_fabric_priority_3){
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Illegal priority %d - TDM priority"),SOC_DFE_CONFIG(unit).vcs128_unicast_priority));
            }
        }
    } else {
        SOC_DFE_CONFIG(unit).vcs128_unicast_priority = soc_dfe_property_get(unit, spn_VCS128_UNICAST_PRIORITY, 1,  DFE_VCS128_UNICAST_PRIORITY_TDM_OVER_PRIMARY_PIPE_DEFAULT);
        if ( SOC_DFE_CONFIG(unit).vcs128_unicast_priority != SOC_DFE_PROPERTY_UNAVAIL) 
        {
            if(!DFE_IS_PRIORITY_VALID(SOC_DFE_CONFIG(unit).vcs128_unicast_priority)) 
            {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Illegal priority %d"),SOC_DFE_CONFIG(unit).vcs128_unicast_priority));
            } else if (SOC_DFE_CONFIG(unit).vcs128_unicast_priority >= soc_dfe_fabric_priority_2) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Illegal priority %d - TDM priority"),SOC_DFE_CONFIG(unit).vcs128_unicast_priority));
            }
        }
    }

    if (SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system !=  SOC_DFE_PROPERTY_UNAVAIL && SOC_DFE_CONFIG(unit).system_is_fe600_in_system != SOC_DFE_PROPERTY_UNAVAIL) 
    {
        if (SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system==0 && SOC_DFE_CONFIG(unit).system_is_fe600_in_system==1) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unsupported properties: is_fe600=1 & dpp_arad->init.fabric.is_128_in_system=0\n")));
        }
    }

    if(SOC_DFE_CONFIG(unit).fabric_TDM_fragment != SOC_DFE_PROPERTY_UNAVAIL){ 
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_TDM_fragment_validate,(unit, SOC_DFE_CONFIG(unit).fabric_TDM_fragment)));
    }

    if (SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe != SOC_DFE_PROPERTY_UNAVAIL)
    {
        if (SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe && !SOC_DFE_CONFIG(unit).is_dual_mode) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TDM_OVER_PRIMARY_PIPE: available only in and dual pipe device")));
        }
    }



exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_dfe_drv_soc_properties_general_read(int unit)
{
    char *str;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = dcmn_property_get_str(unit, spn_FABRIC_DEVICE_MODE, &str);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_dfe_property_str_to_enum(unit, spn_FABRIC_DEVICE_MODE, soc_dfe_property_str_enum_fabric_device_mode, str, &SOC_DFE_CONFIG(unit).fabric_device_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    str = soc_dfe_property_get_str(unit, spn_FABRIC_LOAD_BALANCING_MODE,0,NULL);
    rv = soc_dfe_property_str_to_enum(unit, spn_FABRIC_LOAD_BALANCING_MODE, soc_dfe_property_str_enum_fabric_load_balancing_mode, str, &SOC_DFE_CONFIG(unit).fabric_load_balancing_mode);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_DFE_CONFIG(unit).custom_feature_lab=soc_dfe_property_suffix_num_get_only_suffix(unit,0,spn_CUSTOM_FEATURE,"lab",1,0);

    SOC_DFE_CONFIG(unit).mesh_topology_fast= soc_dfe_property_suffix_num_get_only_suffix(unit,0,spn_CUSTOM_FEATURE,"mesh_topology_fast",1,1);


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_drv_soc_properties_fabric_routing_read(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_DFE_IS_FE13(unit))
    {
        SOC_DFE_CONFIG(unit).fabric_local_routing_enable = soc_dfe_property_get(unit, spn_FABRIC_LOCAL_ROUTING_ENABLE, 0, SOC_DFE_PROPERTY_UNAVAIL);
    }
    else
    {
        SOC_DFE_CONFIG(unit).fabric_local_routing_enable = 0;
    }

    SOCDNX_FUNC_RETURN;
}

int
_soc_dfe_drv_soc_property_serdes_qrtt_read(int unit)
{
    soc_pbmp_t          pbmp_disabled;
    int                 quad, disabled_port, quad_index, port, rc, dis;
    uint32              quad_active;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(pbmp_disabled);
    for (quad = 0; quad < SOC_DFE_DEFS_GET(unit, nof_links)/4; quad++) {

       
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_quad_disabled, (unit, quad, &dis));
        SOCDNX_IF_ERR_EXIT(rc);

        if (dis == 1) {
           
            SOCDNX_IF_ERR_EXIT(dcmn_property_suffix_num_get(unit, quad, spn_SERDES_QRTT_ACTIVE, "", 0, &quad_active));
        } else {
            quad_active = soc_property_suffix_num_get(unit, quad, spn_SERDES_QRTT_ACTIVE, "", 1); 
             if (quad_active) {
                 quad_active = soc_property_suffix_num_get(unit, quad, spn_PB_SERDES_QRTT_ACTIVE, "", 1);
             }
        }
         if (!quad_active) {
            for (quad_index = 0; quad_index < 4; quad_index++) {
                SOC_PBMP_PORT_ADD(pbmp_disabled, quad*4 + quad_index);
            }
         }
    }

    for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links) ; port++) {
        disabled_port = FALSE;
        if (PBMP_MEMBER(pbmp_disabled, port)) {
            disabled_port = TRUE;
        }
        
        if (SOC_IS_FE1600_A0(unit) ||  !disabled_port) {
            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi));
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port));            
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all));            
            SOCDNX_IF_ERR_EXIT(rc);

            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi_disabled));           
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port_disabled));            
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all_disabled));
            SOCDNX_IF_ERR_EXIT(rc);
            
        }
        if (disabled_port) {
            
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_sfi_disabled));            
            SOCDNX_IF_ERR_EXIT(rc);

             rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_port_disabled));            
            SOCDNX_IF_ERR_EXIT(rc);

            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 1, soc_dfe_port_update_type_all_disabled));            
            SOCDNX_IF_ERR_EXIT(rc);

            
            if (!SOC_IS_FE1600_A0(unit))
            {
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_sfi));            
                SOCDNX_IF_ERR_EXIT(rc);

                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_port));                
                SOCDNX_IF_ERR_EXIT(rc);

                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_dynamic_port_update, (unit, port, 0, soc_dfe_port_update_type_all));                
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
        
    }
exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_dfe_control_init(int unit)
{
    int rv;
    uint16 dev_id;
    uint8 rev_id;
    soc_dfe_control_t    *dfe = NULL;
    SOCDNX_INIT_FUNC_DEFS;

    
    
    dfe = SOC_DFE_CONTROL(unit);
    if (dfe == NULL) {
      dfe = (soc_dfe_control_t *) sal_alloc(sizeof(soc_dfe_control_t), 
                                            "soc_dfe_control");
      if (dfe == NULL) {
          SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_dfe_control")));
      }
      sal_memset(dfe, 0, sizeof (soc_dfe_control_t));
    
      SOC_CONTROL(unit)->drv = dfe;      
   }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    rv = soc_dfe_info_config(unit, dev_id);
    SOCDNX_IF_ERR_CONT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int dfe_tbl_mem_cache_mem_set(int unit, soc_mem_t mem, void* en)
{
    int rc;
    int enable = *(int *)en;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    if(!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
    {
        return SOC_E_NONE;
    }

    if ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0) {

       rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, enable);
       SOCDNX_IF_ERR_EXIT(rc);

    }

exit:
    SOCDNX_FUNC_RETURN;

}

int
dfe_tbl_mem_cache_enable_parity_tbl(int unit, soc_mem_t mem, void* en)
{
    int rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_MEM_FIELD_VALID(unit, mem, PARITYf))
      rc = dfe_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "parity memory %s cache\n"),SOC_MEM_NAME(unit, mem)));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;

}

int
dfe_tbl_mem_cache_enable_ecc_tbl(int unit, soc_mem_t mem, void* en)
{
    int rc = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_MEM_FIELD_VALID(unit, mem, ECCf))
      rc = dfe_tbl_mem_cache_mem_set(unit, mem, en);

    LOG_INFO(BSL_LS_SOC_MEM, (BSL_META_U(unit, "ecc memory %s cache\n"),SOC_MEM_NAME(unit, mem)));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_cache_enable_init(int unit)
{
    uint32 cache_enable = 1;
    int enable_all, enable_parity, enable_ecc;

    SOCDNX_INIT_FUNC_DEFS;

    enable_all = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "all", 0);
    enable_parity = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "parity", 1);
    enable_ecc = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "ecc", 1);

    if (enable_all) {
        SOCDNX_IF_ERR_EXIT(soc_mem_iterate(unit, dfe_tbl_mem_cache_mem_set, &cache_enable));
    }

    if (enable_parity)
    {
        SOCDNX_IF_ERR_EXIT(soc_mem_iterate(unit, dfe_tbl_mem_cache_enable_parity_tbl, &cache_enable));
    }

    if (enable_ecc)
    {
        SOCDNX_IF_ERR_EXIT(soc_mem_iterate(unit, dfe_tbl_mem_cache_enable_ecc_tbl, &cache_enable));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_cache_enable_deinit(int unit)
{
    uint32 cache_enable = 0;
    int enable_all, enable_parity, enable_ecc;
    SOCDNX_INIT_FUNC_DEFS;

    enable_all = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "all", 0);
    enable_parity = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "parity", 0);
    enable_ecc = soc_property_suffix_num_get(unit, 0, spn_MEM_CACHE_ENABLE, "ecc", 0);

    if (enable_all) {
        SOCDNX_IF_ERR_CONT(soc_mem_iterate(unit, dfe_tbl_mem_cache_mem_set, &cache_enable));
    }

    if (enable_parity)
    {
        SOCDNX_IF_ERR_CONT(soc_mem_iterate(unit, dfe_tbl_mem_cache_enable_parity_tbl, &cache_enable));
    }

    if (enable_ecc)
    {
        SOCDNX_IF_ERR_CONT(soc_mem_iterate(unit, dfe_tbl_mem_cache_enable_ecc_tbl, &cache_enable));
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_init_reset(int unit)
{
    int rc = SOC_E_NONE;
    soc_control_t* soc;

    SOCDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit); 

    soc = SOC_CONTROL(unit);

    
    soc_endian_config(unit);

    
    rc = soc_dfe_info_soc_properties(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    soc_dfe_tbl_mark_cachable(unit);

    rc = soc_dfe_cache_enable_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = soc_dfe_warm_boot_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_linkctrl_init);
    SOCDNX_IF_ERR_EXIT(rc);
    
    
    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        SOC_DFE_DRV_INIT_LOG(unit, "Device Reset");
        rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_reset_device);
        SOCDNX_IF_ERR_EXIT(rc);

        
        SOC_DFE_DRV_INIT_LOG(unit, "Device Isolate");
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_set,(unit, soc_dcmn_isolation_status_isolated)); 
        SOCDNX_IF_ERR_EXIT(rc);      
        
    } else {
        
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_interrupts_init, (unit));
        SOCDNX_IF_ERR_CONT(rc);
    }

    
    rc = soc_counter_attach(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_port_soc_init); 
    SOCDNX_IF_ERR_EXIT(rc);
    if(!SOC_IS_FE3200(unit)){
        rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_drv_sw_ver_set); 
        SOCDNX_IF_ERR_EXIT(rc);
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        
        rc = MBCM_DFE_DRIVER_CALL_NO_ARGS(unit, mbcm_dfe_ser_init);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    soc->soc_flags |= SOC_F_INITED;

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    if (SOCDNX_FUNC_ERROR)
    {
        soc_dfe_deinit(unit);
    }
    SOCDNX_FUNC_RETURN;

}

int
soc_dfe_init_no_reset(int unit)
{
    int rc = SOC_E_NONE;
    soc_control_t* soc;

    soc_pbmp_t pbmp_enabled, pbmp_unisolated;
    soc_port_t port;
    int enable;
    soc_dcmn_isolation_status_t link_isolation_status, device_isolation_status;


    SOCDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);


    soc = SOC_CONTROL(unit);
    soc_endian_config(unit);

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RELOADING(unit))
    {
        SOC_PBMP_CLEAR(pbmp_enabled);
        SOC_PBMP_CLEAR(pbmp_unisolated);
        SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
        {
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_phy_enable_get, (unit, port, &enable));
            SOCDNX_IF_ERR_EXIT(rc);
            if (enable) {
                SOC_PBMP_PORT_ADD(pbmp_enabled, port);
            }
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_isolate_get, (unit, port, &link_isolation_status));
            SOCDNX_IF_ERR_EXIT(rc);
            if (link_isolation_status == soc_dcmn_isolation_status_active)
            {
                SOC_PBMP_PORT_ADD(pbmp_unisolated, port);
            }
        }

        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_isolate_get, (unit, &device_isolation_status));
        SOCDNX_IF_ERR_EXIT(rc);

        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_graceful_shutdown_set, 
                                  (unit, pbmp_enabled, 1, pbmp_unisolated, 0));
        SOCDNX_IF_ERR_EXIT(rc);
        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, SOC_DCMN_RESET_ACTION_INOUT_RESET));
        SOCDNX_IF_ERR_EXIT(rc);

        sal_usleep(10000); 

        rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_graceful_shutdown_set, 
                                  (unit, pbmp_enabled, 0, pbmp_unisolated, device_isolation_status == soc_dcmn_isolation_status_isolated ? 1:0));
        SOCDNX_IF_ERR_EXIT(rc);
        

    }

    soc->soc_flags |= SOC_F_INITED;

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit); 
    SOCDNX_FUNC_RETURN;

}


int
soc_dfe_init(int unit, int reset) 
{
    int rc = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS; 
    
    if (reset)
    {
        rc=soc_dfe_init_reset(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        rc=soc_dfe_init_no_reset(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
     
    SOCDNX_FUNC_RETURN;
}


soc_driver_t *
soc_dfe_chip_driver_find(int unit , uint16 pci_dev_id, uint8 pci_rev_id)
{
    uint16              driver_dev_id;
    uint16              driver_rev_id;

    if (soc_cm_get_id_driver(pci_dev_id, pci_rev_id, &driver_dev_id, &driver_rev_id) < 0) {
        return NULL;
    }

     switch(driver_dev_id)
     {
#if defined(BCM_88750_SUPPORT)
     case BCM88750_DEVICE_ID:
        if (pci_rev_id == BCM88750_A0_REV_ID) {
           return &soc_driver_bcm88750_a0;
        } else if (pci_rev_id == BCM88750_B0_REV_ID) {
            return &soc_driver_bcm88750_b0;
        } else {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
         break;
     case BCM88753_DEVICE_ID:
        if (pci_rev_id == BCM88753_A0_REV_ID) {
           return &soc_driver_bcm88750_a0;
        } else if (pci_rev_id == BCM88753_B0_REV_ID) {
            return &soc_driver_bcm88750_b0;
        } else {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
         break;
     case BCM88755_DEVICE_ID:
        if (pci_rev_id == BCM88755_B0_REV_ID) {
           return &soc_driver_bcm88750_b0;
        } else {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
         break;
#ifdef BCM_88754_A0 
     case BCM88754_DEVICE_ID:
            if (pci_rev_id == BCM88754_A0_REV_ID) {
               return &soc_driver_bcm88754_a0;
            } else {
                  LOG_ERROR(BSL_LS_SOC_INIT,
                            (BSL_META_U(unit,
                                        "soc_dfe_chip_driver_find: driver in devid table "
                                        "not in soc_base_driver_table\n")));
            }
            break;
#endif 

        case BCM88752_DEVICE_ID:
        if (pci_rev_id == BCM88750_A0_REV_ID) {
           return &soc_driver_bcm88750_a0;
        } else if (pci_rev_id == BCM88750_B0_REV_ID) {
            return &soc_driver_bcm88750_b0;
        }  else {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
        break;
#endif 

#ifdef BCM_88950_A0
     case BCM88950_DEVICE_ID:
         if (pci_rev_id == BCM88950_A0_REV_ID || pci_rev_id == BCM88950_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88770_DEVICE_ID:
         if (pci_rev_id == BCM88770_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88953_DEVICE_ID:
     case BCM88773_DEVICE_ID:
         if (pci_rev_id == BCM88773_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88954_DEVICE_ID:
     case BCM88774_DEVICE_ID:
         if (pci_rev_id == BCM88774_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88955_DEVICE_ID:
     case BCM88775_DEVICE_ID:
         if (pci_rev_id == BCM88775_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88956_DEVICE_ID:
     case BCM88776_DEVICE_ID:
         if (pci_rev_id == BCM88776_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
     case BCM88777_DEVICE_ID:
         if (pci_rev_id == BCM88777_A1_REV_ID)
         {
             return &soc_driver_bcm88950_a0;
         }
         else
         {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
         }
         break;
    case BCM88952_DEVICE_ID:
        if (pci_rev_id == BCM88952_A0_REV_ID || pci_rev_id == BCM88952_A1_REV_ID)
        {
            return &soc_driver_bcm88950_a0;
        } else 
        {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
        break;
    case BCM88772_DEVICE_ID:
        if (pci_rev_id == BCM88772_A1_REV_ID)
        {
            return &soc_driver_bcm88950_a0;
        } else 
        {
             LOG_ERROR(BSL_LS_SOC_INIT,
                       (BSL_META_U(unit,
                                   "soc_dfe_chip_driver_find: driver in devid table "
                                   "not in soc_base_driver_table\n")));
        }
        break;
            
#endif
       default:
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "soc_dfe_chip_driver_find: driver in devid table "
                                  "not in soc_base_driver_table\n")));
           break;         
     }
  
    return NULL;
}

int
soc_dfe_info_config(int unit, int dev_id)
{
    soc_info_t          *si;
    soc_control_t       *soc;
    int                 mem, blk, blktype, rv;
    char                instance_string[3];
    int port, phy_port, bindex;

    SOCDNX_INIT_FUNC_DEFS;

    soc = SOC_CONTROL(unit);

    
    switch (dev_id) {
        case BCM88750_DEVICE_ID:
        case BCM88753_DEVICE_ID:
        case BCM88755_DEVICE_ID:
#ifdef BCM_88754_A0
       case BCM88754_DEVICE_ID:
#endif
        case BCM88752_DEVICE_ID:
            SOC_CHIP_STRING(unit) = "fe1600";
            break;
        case BCM88770_DEVICE_ID:
        case BCM88773_DEVICE_ID:
        case BCM88774_DEVICE_ID:
        case BCM88775_DEVICE_ID:
        case BCM88776_DEVICE_ID:
        case BCM88777_DEVICE_ID:
        case BCM88950_DEVICE_ID:
        case BCM88953_DEVICE_ID:
        case BCM88954_DEVICE_ID:
        case BCM88955_DEVICE_ID:
        case BCM88956_DEVICE_ID:
        case BCM88772_DEVICE_ID:
        case BCM88952_DEVICE_ID:
            SOC_CHIP_STRING(unit) = "fe3200";
            break;

        default:
            SOC_CHIP_STRING(unit) = "???";
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "soc_dfe_info_config: driver device %04x unexpected\n"),
                                 dev_id));
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("failed to find device id")));
    }


    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    rv = soc_dfe_defines_init(unit);
    SOCDNX_IF_ERR_CONT(rv);

    rv = soc_dfe_implementation_defines_init(unit);
    SOCDNX_IF_ERR_CONT(rv);


    si->fe.min          = si->fe.max          = -1;         si->fe.num = 0;
    si->ge.min          = si->ge.max          = -1;         si->ge.num = 0;
    si->xe.min          = si->xe.max          = -1;         si->xe.num = 0;
    si->hg.min          = si->hg.max          = -1;         si->hg.num = 0;
    si->hg_subport.min  = si->hg_subport.max  = -1;         si->hg_subport.num = 0;
    si->hl.min          = si->hl.max          = -1;         si->hl.num = 0;
    si->st.min          = si->st.max          = -1;         si->st.num = 0;
    si->gx.min          = si->gx.max          = -1;         si->gx.num = 0;
    si->xg.min          = si->xg.max          = -1;         si->xg.num = 0;
    si->spi.min         = si->spi.max         = -1;         si->spi.num = 0;
    si->spi_subport.min = si->spi_subport.max = -1;         si->spi_subport.num = 0;
    si->sci.min         = si->sci.max         = -1;         si->sci.num = 0;
    si->sfi.min         = si->sfi.max         = -1;         si->sfi.num = 0;
    si->port.min        = si->port.max        = -1;         si->port.num = 0;
    si->ether.min       = si->ether.max       = -1;         si->ether.num = 0;
    si->all.min         = si->all.max         = -1;         si->all.num = 0;
    
    
    si->port_num = 0;

    sal_memset(si->has_block, 0, sizeof(soc_block_t) * COUNTOF(si->has_block));

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk++) {
        si->block_port[blk] = REG_PORT_ANY;
        si->block_valid[blk] = 0;
    }

    SOC_PBMP_CLEAR(si->cmic_bitmap);

    si->cmic_block = -1;
    for (blk = 0; blk < SOC_MAX_NUM_OTPC_BLKS; blk++) {
        si->otpc_block[blk] = -1;
    }

    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        blktype = SOC_BLOCK_INFO(unit, blk).type;

        if(blk >= SOC_MAX_NUM_BLKS)
        {
              SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much blocks for device \n")));
        }
        si->has_block[blk] = blktype;
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     SOC_BLOCK_INFO(unit, blk).number);


        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_block_valid_get, (unit, blktype, SOC_BLOCK_INFO(unit, blk).number, &(si->block_valid[blk])));
        SOCDNX_IF_ERR_EXIT(rv);
        
        switch(blktype)
        {
            case SOC_BLK_ECI:
                si->eci_block = blk;
                break;
            case SOC_BLK_CMIC:
                si->cmic_block = blk;
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                si->mesh_topology_block = blk;
                break;
            case SOC_BLK_FMAC:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FMAC_BLKS) {
                    si->fmac_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much FMAC blocks")));
                }
                break;
            case SOC_BLK_OTPC:
                if (SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_OTPC_BLKS) {
                    si->otpc_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much OTPC blocks")));
                }
                break;
            case SOC_BLK_FSRD:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_FSRD_BLKS) {
                    si->fsrd_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much FSRD blocks")));
                }
                break;
            case SOC_BLK_RTP:
                si->rtp_block = blk;
                break;
            case SOC_BLK_OCCG:
                si->occg_block = blk;
                break;
            case SOC_BLK_DCH:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCH_BLKS) {
                    si->dch_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                     SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much DCH blocks")));
                }
                break;
            case SOC_BLK_DCL:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCL_BLKS) {
                    si->dcl_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much DCL blocks")));
                }
                break;
            case SOC_BLK_DCMA:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCMA_BLKS) {
                    si->dcma_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much DCMA blocks")));
                }
                break;
            case SOC_BLK_DCMB:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_DCMB_BLKS) {
                    si->dcmb_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much DCMB blocks")));
                }
                break;
            case SOC_BLK_DCMC:
                si->dcmc_block = blk;
                break;
            case SOC_BLK_CCS:
                if(SOC_BLOCK_INFO(unit, blk).number < SOC_MAX_NUM_CCS_BLKS) {
                    si->ccs_block[SOC_BLOCK_INFO(unit, blk).number] = blk;
                    si->block_port[blk] = SOC_BLOCK_INFO(unit, blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much CCS blocks")));
                }
                break;
            case SOC_BLK_BRDC_FMACH:
                si->brdc_fmach_block = blk;
                break;
            case SOC_BLK_BRDC_FMACL:
                si->brdc_fmacl_block = blk;
                break;
            case SOC_BLK_BRDC_FSRD:
                si->brdc_fsrd_block = blk;
                break;
            case SOC_BLK_DCM:
                if (SOC_BLOCK_INFO(unit,blk).number < SOC_MAX_NUM_DCM_BLKS) {
                    si->dcm_block[SOC_BLOCK_INFO(unit,blk).number] = blk;
                    si->block_port[blk]= SOC_BLOCK_INFO(unit,blk).number | SOC_REG_ADDR_INSTANCE_MASK;
                }
                else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: too much DCM blocks")));

                }
                break;
            case SOC_BLK_BRDC_DCH:
                si->brdc_dch_block= blk;
                break;
            case SOC_BLK_BRDC_DCM:
                si->brdc_dcm_block=blk;
                break;
            case SOC_BLK_BRDC_DCL:
                si->brdc_dcl_block=blk;
                break;
            case SOC_BLK_BRDC_CCS:
                si->brdc_ccs_block=blk;
                break;
            case SOC_BLK_BRDC_FMAC_AC:
                si->brdc_fmac_ac_block=blk;
                break;
            case SOC_BLK_BRDC_FMAC_BD:
                si->brdc_fmac_bd_block=blk;
                break;
            case SOC_BLK_IPROC:
                si->iproc_block=blk;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dfe_info_config: unknown block type")));
                break;            
        }
              

        sal_snprintf(si->block_name[blk], sizeof(si->block_name[blk]),
                     "%s%s",
                     soc_block_name_lookup_ext(blktype, unit),
                     instance_string);
    }
    si->block_num = blk;

    
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        si->mem_block_any[mem] = -1;
        if (SOC_MEM_IS_VALID(unit, mem)) {
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                si->mem_block_any[mem] = blk;
                break;
            }
        }
    }

    for (phy_port = 0; ; phy_port++) {
        
        blk = SOC_DRIVER(unit)->port_info[phy_port].blk;
        bindex = SOC_DRIVER(unit)->port_info[phy_port].bindex;
        if (blk < 0 && bindex < 0) { 
            break;
        }
       
        port = phy_port;
        
        if (blk < 0 ) { 
            blktype = 0; 
        } else {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
        }
        
        if (blktype == 0) {
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                         "sfi%d", port);
            si->port_offset[port] = port;
            continue;
        }
        
        switch (blktype) {
            case SOC_BLK_CMIC:
                si->cmic_port = port;
                sal_sprintf(SOC_PORT_NAME(unit, port),"CMIC");
                SOC_PBMP_PORT_ADD(si->cmic_bitmap, port);
                break;
        default:
                si->port_num_lanes[port] = 1;
                sal_sprintf(SOC_PORT_NAME(unit, port),"sfi%d",port);
                sal_sprintf(SOC_PORT_NAME_ALTER(unit, port),"fabric%d",port);
                SOC_PORT_NAME_ALTER_VALID(unit, port) = 1;
                DFE_ADD_PORT(sfi, port);
                DFE_ADD_PORT(port, port);
                DFE_ADD_PORT(all, port);
                break;
        }

        si->port_type[phy_port] = blktype;
        
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_detach(int unit)
{
    soc_control_t       *soc;
    int                  mem,rc;
    int                  cmc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        SOC_EXIT;
    }


    if (soc->miimMutex) {
        sal_mutex_destroy(soc->miimMutex);
        soc->miimMutex = NULL;
    }

    if (soc->counterMutex) {
        sal_mutex_destroy(soc->counterMutex);
        soc->counterMutex = NULL;
    }

    if (soc->schanMutex) {
        sal_mutex_destroy(soc->schanMutex);
        soc->schanMutex = NULL;
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if (soc->schanIntr[cmc]) {
            sal_sem_destroy(soc->schanIntr[cmc]);
            soc->schanIntr[cmc] = NULL;
        }
    }

    (void)soc_sbusdma_lock_deinit(unit);

    
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {
            
            if (soc->memState[mem].lock) {
                 sal_mutex_destroy(soc->memState[mem].lock);
                 soc->memState[mem].lock = NULL;
            }
        }
    }

    if (soc->miimIntr) {
        sal_sem_destroy(soc->miimIntr);
        soc->miimIntr = NULL;
    }

    if (SOC_PERSIST(unit)) {
        sal_free(SOC_PERSIST(unit));
        SOC_PERSIST(unit) = NULL;
    }

    if (soc->socControlMutex) {
        sal_mutex_destroy(soc->socControlMutex);
        soc->socControlMutex = NULL;
    }

    if (_bcm_lock[unit] != NULL) {
        sal_mutex_destroy(_bcm_lock[unit]);
        _bcm_lock[unit] = NULL;
    }
    if (soc->schan_wb_mutex != NULL) {
        sal_mutex_destroy(soc->schan_wb_mutex);
        soc->schan_wb_mutex = NULL;
    }



    rc = soc_dfe_implementation_defines_deinit(unit);
    SOCDNX_IF_ERR_CONT(rc);

    rc = soc_dfe_defines_deinit(unit);
    SOCDNX_IF_ERR_CONT(rc);

    if (SOC_CONTROL(unit)->drv != NULL)
    {
    sal_free((soc_dfe_control_t *)SOC_CONTROL(unit)->drv);
    SOC_CONTROL(unit)->drv = NULL;
    }

    sal_free(soc);
    SOC_CONTROL(unit) = NULL;

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_chip_type_set(int unit, uint16 dev_id)
{
    soc_info_t           *si; 
    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    
    switch (dev_id) {
       case BCM88750_DEVICE_ID:
       case BCM88753_DEVICE_ID:
#ifdef BCM_88754_A0
       case BCM88754_DEVICE_ID:
#endif
        case BCM88755_DEVICE_ID:
        case BCM88752_DEVICE_ID:     
            si->chip_type = SOC_INFO_CHIP_TYPE_FE1600;
            break;
        case BCM88770_DEVICE_ID:
        case BCM88773_DEVICE_ID:
        case BCM88774_DEVICE_ID:
        case BCM88775_DEVICE_ID:
        case BCM88776_DEVICE_ID:
        case BCM88777_DEVICE_ID:
        case BCM88950_DEVICE_ID:
        case BCM88953_DEVICE_ID:
        case BCM88954_DEVICE_ID:
        case BCM88955_DEVICE_ID:
        case BCM88956_DEVICE_ID:
        case BCM88772_DEVICE_ID:
        case BCM88952_DEVICE_ID:
            si->chip_type = SOC_INFO_CHIP_TYPE_FE3200;
            break;

        default:
            si->chip_type = 0;
           LOG_ERROR(BSL_LS_SOC_INIT,
                     (BSL_META_U(unit,
                                 "soc_dfe_chip_type_set: driver device %04x unexpected\n"),
                                 dev_id));
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("failed to find device id")));
    }

exit:  
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_attach(int unit)
{
    soc_control_t        *soc;
    soc_persist_t        *sop;
    soc_info_t           *si; 
    uint16               dev_id;
    uint8                rev_id;
    int                  rc = SOC_E_NONE, mem;
    int                  cmc;
    SOCDNX_INIT_FUNC_DEFS;

    
    soc = SOC_CONTROL(unit);
    if (soc == NULL) {
        soc = sal_alloc(sizeof (soc_control_t), "soc_control");
        if (soc == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_control")));
        }
        sal_memset(soc, 0, sizeof (soc_control_t));
        SOC_CONTROL(unit) = soc;
    } else {
        if (soc->soc_flags & SOC_F_ATTACHED) {
            SOC_EXIT;
        }
    }

    soc->soc_functions = &soc_dfe_drv_funs;

    soc->soc_link_pause = 0;

    SOC_PCI_CMCS_NUM(unit) = soc_property_uc_get(unit, 0, spn_PCI_CMCS_NUM, 1);

	#ifdef BCM_SBUSDMA_SUPPORT
    if (!SOC_IS_FE1600(unit)) {
        SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DCMN_MAX_SBUSDMA_CHANNELS;
        SOC_CONTROL(unit)->tdma_ch              = SOC_DCMN_TDMA_CHANNEL;
        SOC_CONTROL(unit)->tslam_ch             = SOC_DCMN_TSLAM_CHANNEL;
        SOC_CONTROL(unit)->desc_ch              = SOC_DCMN_DESC_CHANNEL;
        
        SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, 
            soc_property_get(unit, spn_MEM_CLEAR_CHUNK_SIZE, SOC_DCMN_MEM_CLEAR_CHUNK_SIZE));
    }
	#endif


    
    SOCDNX_IF_ERR_EXIT(soc_dma_attach(unit, 1 ));

    
    SOCDNX_IF_ERR_EXIT(soc_dcmn_cmic_pcie_userif_purge_ctrl_init(unit));

    

    if ((soc->miimMutex = sal_mutex_create("MIIM")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM lock")));
    }
        
    if ((soc->miimIntr = sal_sem_create("MIIM interrupt", sal_sem_BINARY, 0)) == NULL)  {
         SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate MIIM semaphore")));
    }

    if (_bcm_lock[unit] == NULL) {
        if ((_bcm_lock[unit] = sal_mutex_create("bcm_dfe_config_lock")) == NULL) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate _bcm_lock")));
        }
    }

    if (_bcm_lock[unit] == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate DFE lock")));
    }

    if ((soc->socControlMutex = sal_mutex_create("SOC_CONTROL")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate socControlMutex")));
    }

    soc->counterMutex = sal_mutex_create("Counter");
    if (soc->counterMutex == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate counterMutex")));
    }

    soc->schanMutex = sal_mutex_create("SCHAN");
    if (soc->schanMutex == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate schanMutex")));
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit) + 1; cmc++) {
        if ((soc->schanIntr[cmc] =
             sal_sem_create("SCHAN interrupt", sal_sem_BINARY, 0)) == NULL) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate schanSem")));
        }
    }
    
    SOC_PERSIST(unit) = sal_alloc(sizeof (soc_persist_t), "soc_persist");
    if (SOC_PERSIST(unit) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate soc_persist")));
    }
    sal_memset(SOC_PERSIST(unit), 0, sizeof (soc_persist_t));
    sop = SOC_PERSIST(unit);
    sop->version = 1;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SOCDNX_IF_ERR_EXIT(soc_dfe_chip_type_set(unit, dev_id));

    
    soc->chip_driver = soc_dfe_chip_driver_find(unit, dev_id, rev_id);
    if (soc->chip_driver == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("unit has no driver (device 0x%04x rev 0x%02x)"),dev_id, rev_id));
    }

    
    soc_feature_init(unit);

    si  = &SOC_INFO(unit);
    si->driver_type = soc->chip_driver->type;
    si->driver_group = soc_chip_type_map[si->driver_type];

    
    
    rc = mbcm_dfe_init(unit);
    if (rc != SOC_E_NONE) {
         LOG_INFO(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dfe_init error in mbcm_dfe_init\n")));
    }
    SOCDNX_IF_ERR_EXIT(rc);    


    


    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_controlled_counter_set,(unit));
    SOCDNX_IF_ERR_EXIT(rc);


    
    
    rc = soc_dfe_control_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    soc->soc_flags |= SOC_F_ATTACHED;

    

    
    rc = soc_sbusdma_lock_init(unit);
    if (rc != SOC_E_NONE) {
        SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to Initialize SBUSDMA Locks")));
    }
#ifdef BCM_SBUSDMA_SUPPORT
    
    if (soc_feature(unit, soc_feature_sbusdma)) {
        rc = soc_sbusdma_init(unit, soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC, 0),
                                  soc_property_get(unit, spn_DMA_DESC_INTR_ENABLE, 0));
        if (rc != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to Initialize SBUSDMA")));
        }
        rc = (sand_init_fill_table(unit));
        if (rc != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to Initialize Fill table init")));
        }
    }
#endif

    
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (SOC_MEM_IS_VALID(unit, mem)) {
            sop->memState[mem].index_max = SOC_MEM_INFO(unit, mem).index_max;
            
            if ((soc->memState[mem].lock =
                 sal_mutex_create(SOC_MEM_NAME(unit, mem))) == NULL) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("failed to allocate memState lock")));
            }
    
            
            sal_memset(soc->memState[mem].cache,
                       0,
                       sizeof (soc->memState[mem].cache));
        } else {
            sop->memState[mem].index_max = -1;
        }
    }

    if ((soc->schan_wb_mutex = sal_mutex_create("SchanWB")) == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate SchanWB")));
    }

    
    rc = soc_schan_init(unit);
    if (rc != SOC_E_NONE) {
       SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to Initialize SCHAN")));
    }

exit:  
    if(SOCDNX_FUNC_ERROR) {
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META_U(unit,
                             "soc_dfe_attach: unit %d failed (%s)\n"), 
                             unit, soc_errmsg(rc)));
        soc_dfe_detach(unit);
    }

    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_dump(int unit, char *pfx)
{
    soc_control_t       *soc;
    soc_persist_t       *sop;
    soc_stat_t          *stat;
    uint16              dev_id;
    uint8               rev_id;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    soc = SOC_CONTROL(unit);
    sop = SOC_PERSIST(unit);

    stat = &soc->stat;

    LOG_CLI((BSL_META_U(unit,
                        "%sUnit %d Driver Control Structure:\n"), pfx, unit));

    soc_cm_get_id(unit, &dev_id, &rev_id);

    LOG_CLI((BSL_META_U(unit,
                        "%sChip=%s Rev=0x%02x Driver=%s\n"),
             pfx,
             soc_dev_name(unit),
             rev_id,
             SOC_CHIP_NAME(soc->chip_driver->type)));
    LOG_CLI((BSL_META_U(unit,
                        "%sFlags=0x%x:"),
             pfx, soc->soc_flags));
    if (soc->soc_flags & SOC_F_ATTACHED) {
        LOG_CLI((BSL_META_U(unit,
                            " attached")));
    }
    if (soc->soc_flags & SOC_F_INITED) {
        LOG_CLI((BSL_META_U(unit,
                            " initialized")));
    }
    if (soc->soc_flags & SOC_F_LSE) {
        LOG_CLI((BSL_META_U(unit,
                            " link-scan")));
    }
    if (soc->soc_flags & SOC_F_SL_MODE) {
        LOG_CLI((BSL_META_U(unit,
                            " sl-mode")));
    }
    if (soc->soc_flags & SOC_F_POLLED) {
        LOG_CLI((BSL_META_U(unit,
                            " polled")));
    }
    if (soc->soc_flags & SOC_F_URPF_ENABLED) {
        LOG_CLI((BSL_META_U(unit,
                            " urpf")));
    }
    if (soc->soc_flags & SOC_F_MEM_CLEAR_USE_DMA) {
        LOG_CLI((BSL_META_U(unit,
                            " mem-clear-use-dma")));
    }
    if (soc->soc_flags & SOC_F_IPMCREPLSHR) {
        LOG_CLI((BSL_META_U(unit,
                            " ipmc-repl-shared")));
    }
    if (soc->remote_cpu) {
        LOG_CLI((BSL_META_U(unit,
                            " rcpu")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "; board type 0x%x"), soc->board_type));
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "%s"), pfx));
    soc_cm_dump(unit);

    LOG_CLI((BSL_META_U(unit,
                        "%sDisabled: reg_flags=0x%x mem_flags=0x%x\n"),
             pfx,
             soc->disabled_reg_flags, soc->disabled_mem_flags));
    LOG_CLI((BSL_META_U(unit,
                        "%sSchanOps=%d MMUdbg=%d LinkPause=%d\n"),
             pfx,
             stat->schan_op,
             sop->debugMode, soc->soc_link_pause));
    LOG_CLI((BSL_META_U(unit,
                        "%sCounter: int=%dus per=%dus dmaBuf=%p\n"),
             pfx,
             soc->counter_interval,
             SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev),
             (void *)soc->counter_buf32));
    LOG_CLI((BSL_META_U(unit,
                        "%sTimeout: Schan=%d(%dus) MIIM=%d(%dus)\n"),
             pfx,
             stat->err_sc_tmo, soc->schanTimeout,
             stat->err_mii_tmo, soc->miimTimeout));
    LOG_CLI((BSL_META_U(unit,
                        "%sIntr: Total=%d Sc=%d ScErr=%d MMU/ARLErr=%d\n"
             "%s      LinkStat=%d PCIfatal=%d PCIparity=%d\n"
                        "%s      ARLdrop=%d ARLmbuf=%d ARLxfer=%d ARLcnt0=%d\n"
                        "%s      TableDMA=%d TSLAM-DMA=%d\n"
                        "%s      MemCmd[BSE]=%d MemCmd[CSE]=%d MemCmd[HSE]=%d\n"
                        "%s      ChipFunc[0]=%d ChipFunc[1]=%d ChipFunc[2]=%d\n"
                        "%s      ChipFunc[3]=%d ChipFunc[4]=%d\n"
                        "%s      I2C=%d MII=%d StatsDMA=%d Desc=%d Chain=%d\n"),
             pfx, stat->intr, stat->intr_sc, stat->intr_sce, stat->intr_mmu,
             pfx, stat->intr_ls,
             stat->intr_pci_fe, stat->intr_pci_pe,
             pfx, stat->intr_arl_d, stat->intr_arl_m,
             stat->intr_arl_x, stat->intr_arl_0,
             pfx, stat->intr_tdma, stat->intr_tslam,
             pfx, stat->intr_mem_cmd[0],
             stat->intr_mem_cmd[1], stat->intr_mem_cmd[2],
             pfx, stat->intr_chip_func[0], stat->intr_chip_func[1],
             stat->intr_chip_func[2],
             pfx, stat->intr_chip_func[3], stat->intr_chip_func[4],
             pfx, stat->intr_i2c, stat->intr_mii, stat->intr_stats,
             stat->intr_desc, stat->intr_chain));

        soc_dfe_drv_soc_properties_dump(unit);
   
exit:
    SOCDNX_FUNC_RETURN;
} 
  
void 
soc_dfe_drv_soc_properties_dump(int unit)
{
    int i;
    int lcpll;
    SOCDNX_INIT_FUNC_DEFS;

    LOG_CLI((BSL_META_U(unit,
                        "\nSoC properties dump\n")));
    LOG_CLI((BSL_META_U(unit,
                        "-------------------\n")));

    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, spn_FABRIC_DEVICE_MODE, soc_dfe_property_str_enum_fabric_device_mode, SOC_DFE_CONFIG(unit).fabric_device_mode);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, spn_FABRIC_MULTICAST_MODE, soc_dfe_property_str_enum_fabric_multicast_mode, SOC_DFE_CONFIG(unit).fabric_multicast_mode);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, spn_FABRIC_LOAD_BALANCING_MODE, soc_dfe_property_str_enum_fabric_load_balancing_mode, SOC_DFE_CONFIG(unit).fabric_load_balancing_mode);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, spn_FE_MC_ID_RANGE, soc_dfe_property_str_enum_fe_mc_id_range, SOC_DFE_CONFIG(unit).fe_mc_id_range);


    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_IS_DUAL_MODE, SOC_DFE_CONFIG(unit).is_dual_mode);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_IS_VCS_128_IN_SYSTEM, SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_IS_DUAL_MODE_IN_SYSTEM, SOC_DFE_CONFIG(unit).system_is_dual_mode_in_system);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_IS_SINGLE_MODE_IN_SYSTEM, SOC_DFE_CONFIG(unit).system_is_single_mode_in_system);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE, SOC_DFE_CONFIG(unit).system_contains_multiple_pipe_device);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_IS_FE600_IN_SYSTEM, SOC_DFE_CONFIG(unit).system_is_fe600_in_system);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_MERGE_CELLS, SOC_DFE_CONFIG(unit).fabric_merge_cells);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_TDM_FRAGMENT, SOC_DFE_CONFIG(unit).fabric_TDM_fragment);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_STR_DUMP(unit, spn_FABRIC_TDM_PRIORITY_MIN, soc_dfe_property_str_enum_fabric_tdm_priority_min, SOC_DFE_CONFIG(unit).fabric_tdm_priority_min);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_VCS128_UNICAST_PRIORITY, SOC_DFE_CONFIG(unit).vcs128_unicast_priority);

    
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_MAC_BUCKET_FILL_RATE, SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SERDES_MIXED_RATE_ENABLE, SOC_DFE_CONFIG(unit).serdes_mixed_rate_enable);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_OPTIMIZE_PARTIAL_LINKS, SOC_DFE_CONFIG(unit).fabric_optimize_patial_links);

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FE_MC_PRIORITY_MAP_ENABLE, SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable);

    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_TDM_OVER_PRIMARY_PIPE, SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe);

    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_NUM_PIPES, SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes);
    for (i=0;i<SOC_DFE_MAX_NUM_OF_PRIORITIES ;i++)
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_FABRIC_PIPE_MAP, "uc", i,SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_uc[i]);
    }
    for (i=0;i<SOC_DFE_MAX_NUM_OF_PRIORITIES;i++)
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_FABRIC_PIPE_MAP, "mc", i,SOC_DFE_FABRIC_PIPES_CONFIG(unit).config_mc[i]);
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SYSTEM_REF_CORE_CLOCK, SOC_DFE_CONFIG(unit).system_ref_core_clock);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_CORE_CLOCK_SPEED, SOC_DFE_CONFIG(unit).core_clock_speed);
    for (lcpll = 0; lcpll < SOC_DFE_NOF_LCPLL; lcpll++)
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "in", lcpll ,SOC_DFE_CONFIG(unit).fabric_port_lcpll_in[lcpll]);
    }

    for (lcpll = 0; lcpll < SOC_DFE_NOF_LCPLL; lcpll++)
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "out", lcpll ,SOC_DFE_CONFIG(unit).fabric_port_lcpll_out[lcpll]);
    }
    
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_BIST_ENABLE, SOC_DFE_CONFIG(unit).run_mbist);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_INT_DUMP(unit, spn_CUSTOM_FEATURE, "lab", SOC_DFE_CONFIG(unit).custom_feature_lab);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_INT_DUMP(unit, spn_CUSTOM_FEATURE, "mesh_topology_fast", SOC_DFE_CONFIG(unit).mesh_topology_fast);

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SCHAN_TIMEOUT_USEC, SOC_CONTROL(unit)->schanTimeout);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_SCHAN_INTR_ENABLE, SOC_CONTROL(unit)->schanIntrEnb);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_MIIM_TIMEOUT_USEC, SOC_CONTROL(unit)->miimTimeout);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_MIIM_INTR_ENABLE, SOC_CONTROL(unit)->miimIntrEnb);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_RATE_INT_MDIO_DIVIDEND, SOC_DFE_CONFIG(unit).mdio_int_dividend);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_RATE_INT_MDIO_DIVISOR, SOC_DFE_CONFIG(unit).mdio_int_divisor);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_TDMA_TIMEOUT_USEC, SOC_CONTROL(unit)->tableDmaTimeout);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_TDMA_INTR_ENABLE, SOC_CONTROL(unit)->tableDmaIntrEnb);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_TSLAM_TIMEOUT_USEC, SOC_CONTROL(unit)->tslamDmaTimeout);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_TSLAM_INTR_ENABLE, SOC_CONTROL(unit)->tslamDmaIntrEnb);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_CELL_FIFO_DMA_ENABLE, SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_CELL_FIFO_DMA_BUFFER_SIZE, SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_buffer_size);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_CELL_FIFO_DMA_TIMEOUT, SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_timeout);
    SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_CELL_FIFO_DMA_THRESHOLD, SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_threshold);

    if (SOC_DFE_IS_FE13(unit))
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_INT_DUMP(unit, spn_FABRIC_LOCAL_ROUTING_ENABLE, SOC_DFE_CONFIG(unit).fabric_local_routing_enable);
    }

    if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) || SOC_DFE_IS_FE2(unit))
    {
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "in_quad", 26, SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_26);
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "in_quad", 35, SOC_DFE_CONFIG(unit).fabric_clk_freq_in_quad_35);
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "out_quad", 26, SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_26);
        SOC_DFE_DRV_SUPPORTED_SOC_PROPERTY_SUFFIX_NUM_INT_DUMP(unit, spn_SERDES_FABRIC_CLK_FREQ, "out_quad", 35, SOC_DFE_CONFIG(unit).fabric_clk_freq_out_quad_35);
    }

    LOG_CLI((BSL_META_U(unit,
                        "-------------------\n")));

    SOCDNX_FUNC_RETURN_VOID;
}

void
soc_dfe_chip_dump(int unit, soc_driver_t *d)
{
    soc_info_t          *si;
    int                 i, count = 0;
    soc_port_t          port;
    char                pfmt[SOC_PBMP_FMT_LEN];
    uint16              dev_id;
    uint8               rev_id;
    int                 blk, bindex;
    char                instance_string[3], block_name[14];

    if (d == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "unit %d: no driver attached\n"), unit));
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "driver %s (%s)\n"), SOC_CHIP_NAME(d->type), d->chip_string));
    LOG_CLI((BSL_META_U(unit,
                        "\tregsfile\t\t%s\n"), d->origin));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci identifier\t\tvendor 0x%04x device 0x%04x rev 0x%02x\n"),
             d->pci_vendor, d->pci_device, d->pci_revision));
    LOG_CLI((BSL_META_U(unit,
                        "\tclasses of service\t%d\n"), d->num_cos));
    LOG_CLI((BSL_META_U(unit,
                        "\tmaximums\t\tblock %d ports %d mem_bytes %d\n"),
             SOC_MAX_NUM_BLKS, SOC_MAX_NUM_PORTS, SOC_MAX_MEM_BYTES));

    if (unit < 0) {
        return;
    }
    si = &SOC_INFO(unit);
    for (blk = 0; d->block_info[blk].type >= 0; blk++) {
        sal_snprintf(instance_string, sizeof(instance_string), "%d",
                     d->block_info[blk].number);
        if (d->block_info[blk].type == SOC_BLK_PORT_GROUP4 ||
            d->block_info[blk].type == SOC_BLK_PORT_GROUP5) {
            sal_strncpy(instance_string,
                        d->block_info[blk].number ? "_y" : "_x", 2);
            instance_string[2] = '\0';
        }
        sal_snprintf(block_name, sizeof(block_name), "%s%s",
                     soc_block_name_lookup_ext(d->block_info[blk].type, unit),
                     instance_string);
        LOG_CLI((BSL_META_U(unit,
                            "\tblk %d\t\t%-14s schan %d cmic %d\n"),
                 blk,
                 block_name,
                 d->block_info[blk].schan,
                 d->block_info[blk].cmic));
    }
    for (port = 0; ; port++) {
          blk = d->port_info[port].blk;
          bindex = d->port_info[port].bindex;
          if (blk < 0 && bindex < 0) {            
              break;
          }
          if (blk < 0) {                          
              continue;
          }
          LOG_CLI((BSL_META_U(unit,
                              "\tport %d\t\t%s\tblk %d %s%d.%d\n"),
                   soc_feature(unit, soc_feature_logical_port_num) ?
                   si->port_p2l_mapping[port] : port,
                   soc_block_port_name_lookup_ext(d->block_info[blk].type, unit),
                   blk,
                   soc_block_name_lookup_ext(d->block_info[blk].type, unit),
                   d->block_info[blk].number,
                   bindex));
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    LOG_CLI((BSL_META_U(unit,
                        "unit %d:\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "\tpci\t\t\tdevice %04x rev %02x\n"), dev_id, rev_id));
    LOG_CLI((BSL_META_U(unit,
                        "\tdriver\t\t\ttype %d (%s) group %d (%s)\n"),
             si->driver_type, SOC_CHIP_NAME(si->driver_type),
             si->driver_group, soc_chip_group_names[si->driver_group]));
    LOG_CLI((BSL_META_U(unit,
                        "\tchip\t\t\t\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\tGE ports\t%d\t%s (%d:%d)\n"),
             si->ge.num, SOC_PBMP_FMT(si->ge.bitmap, pfmt),
             si->ge.min, si->ge.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tXE ports\t%d\t%s (%d:%d)\n"),
             si->xe.num, SOC_PBMP_FMT(si->xe.bitmap, pfmt),
             si->xe.min, si->xe.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tHG ports\t%d\t%s (%d:%d)\n"),
             si->hg.num, SOC_PBMP_FMT(si->hg.bitmap, pfmt),
             si->hg.min, si->hg.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tST ports\t%d\t%s (%d:%d)\n"),
             si->st.num, SOC_PBMP_FMT(si->st.bitmap, pfmt),
             si->st.min, si->st.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tETHER ports\t%d\t%s (%d:%d)\n"),
             si->ether.num, SOC_PBMP_FMT(si->ether.bitmap, pfmt),
             si->ether.min, si->ether.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tPORT ports\t%d\t%s (%d:%d)\n"),
             si->port.num, SOC_PBMP_FMT(si->port.bitmap, pfmt),
             si->port.min, si->port.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tALL ports\t%d\t%s (%d:%d)\n"),
             si->all.num, SOC_PBMP_FMT(si->all.bitmap, pfmt),
             si->all.min, si->all.max));
    LOG_CLI((BSL_META_U(unit,
                        "\tIPIC port\t%d\tblock %d\n"), si->ipic_port, si->ipic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tCMIC port\t%d\t%s block %d\n"), si->cmic_port,
             SOC_PBMP_FMT(si->cmic_bitmap, pfmt), si->cmic_block));
    LOG_CLI((BSL_META_U(unit,
                        "\tother blocks\t\tARL %d MMU %d MCU %d\n"),
             si->arl_block, si->mmu_block, si->mcu_block));
    LOG_CLI((BSL_META_U(unit,
                        "\t            \t\tIPIPE %d IPIPE_HI %d EPIPE %d EPIPE_HI %d BSAFE %d ESM %d OTPC %d\n"),
             si->ipipe_block, si->ipipe_hi_block,
             si->epipe_block, si->epipe_hi_block, si->bsafe_block, si->esm_block, si->otpc_block[0]));

    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            count++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\thas blocks\t%d\t"), count));
    for (i = 0; i < COUNTOF(si->has_block); i++) {
        if (si->has_block[i]) {
            LOG_CLI((BSL_META_U(unit,
                                "%s "), soc_block_name_lookup_ext(si->has_block[i], unit)));
            if ((i) && !(i%6)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n\t\t\t\t")));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\tport names\t\t")));
    for (port = 0; port < si->port_num; port++) {
        if (port > 0 && (port % 5) == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "\n\t\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%d=%s\t"),
                 port, si->port_name[port]));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    i = 0;
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        if (SOC_PBMP_IS_NULL(si->block_bitmap[blk])) {
            continue;
        }
        if (++i == 1) {
            LOG_CLI((BSL_META_U(unit,
                                "\tblock bitmap\t")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "\n\t\t\t")));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%-2d  %-14s %s (%d ports)"),
                 blk,
                 si->block_name[blk],
                 SOC_PBMP_FMT(si->block_bitmap[blk], pfmt),
                 si->block_valid[blk]));
    }
    if (i > 0) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }

    {
        soc_feature_t f;

        LOG_CLI((BSL_META_U(unit,
                            "\tfeatures\t")));
        i = 0;
        for (f = 0; f < soc_feature_count; f++) {
            if (soc_feature(unit, f)) {
                if (++i > 3) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n\t\t\t")));
                    i = 1;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%s "), soc_feature_name[f]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    }
}

int
soc_dfe_nof_interrupts(int unit, int* nof_interrupts) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    SOCDNX_NULL_CHECK(nof_interrupts);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_nof_interrupts,(unit, nof_interrupts));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_dfe_nof_block_instances(int unit,  soc_block_types_t block_types, int *nof_block_instances) 
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    SOCDNX_NULL_CHECK(nof_block_instances);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_nof_block_instance,(unit, block_types, nof_block_instances));
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_dfe_deinit(int unit)
{
    int rc;
    soc_control_t* soc;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit); 


    soc = SOC_CONTROL(unit);

    soc->soc_flags &= ~SOC_F_INITED;

    if(SOC_FAILURE(soc_linkctrl_deinit(unit))){
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Failed in soc_linkctrl_deinit\n")));
    }
    
    rc = soc_dfe_warm_boot_deinit(unit);
    SOCDNX_IF_ERR_CONT(rc);

    rc = soc_dfe_cache_enable_deinit(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_interrupts_deinit, (unit));
    SOCDNX_IF_ERR_CONT(rc);

    
    rc = soc_counter_detach(unit);
    if(SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "Failed to detach counter\n")));
    }


    
    
    rc = soc_dfe_control_init(unit);
    SOCDNX_IF_ERR_CONT(rc);

#ifdef BCM_SBUSDMA_SUPPORT
    
    if (soc_feature(unit, soc_feature_sbusdma)) {
        rc = (sand_deinit_fill_table(unit));
        if (rc != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(rc, (_BSL_SOCDNX_MSG("failed to De-Initialize Fill table init")));
        }
        rc = soc_sbusdma_desc_detach(unit);
        if (rc != SOC_E_NONE) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_sbusdma_desc_detach returns error.")));
        }
    }
#endif

exit:

    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit); 

    
    if(SOC_UNIT_NUM_VALID(unit)) {
        SOC_DETACH(unit,0);
    }


    SOCDNX_FUNC_RETURN;
}

int 
soc_dfe_avs_value_get(int unit, uint32* avs_val)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("invalid unit")));
    }
    
    SOCDNX_NULL_CHECK(avs_val);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_avs_value_get,(unit, avs_val));
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN;    
}

int soc_dfe_compiler_64_div_32(uint64 x, uint32 y, uint32 *result)
{
    uint64 rem;
    uint64 b;
    uint64 res, d;
    uint32 high;

    COMPILER_64_SET(rem, COMPILER_64_HI(x), COMPILER_64_LO(x));
    COMPILER_64_SET(b, 0, y);
    COMPILER_64_SET(d, 0, 1);

    high = COMPILER_64_HI(rem);

    COMPILER_64_ZERO(res);
    if (high >= y) {
        
       LOG_ERROR(BSL_LS_SOC_INIT,
                 (BSL_META("soc_dfe_compiler_64_div_32: result > 32bits\n")));
        return SOC_E_PARAM;
    }

    while ((!COMPILER_64_BITTEST(b, 63)) &&
       (COMPILER_64_LT(b, rem)) ) {
        COMPILER_64_ADD_64(b,b);
        COMPILER_64_ADD_64(d,d);
    }

    do {
    if (COMPILER_64_GE(rem, b)) {
        COMPILER_64_SUB_64(rem, b);
        COMPILER_64_ADD_64(res, d);
    }
        COMPILER_64_SHR(b, 1);
        COMPILER_64_SHR(d, 1);
    } while (!COMPILER_64_IS_ZERO(d));

    *result = COMPILER_64_LO(res);

    return SOC_E_NONE;
}


int
soc_dfe_drv_mbist(int unit, int skip_errors)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_drv_mbist,(unit, skip_errors));
    SOCDNX_IF_ERR_EXIT(rc);
        
exit:
    SOCDNX_FUNC_RETURN;   
}

int soc_dfe_device_reset(
    int unit,
    int mode,
    int action)
{
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    switch (mode) {
    case SOC_DCMN_RESET_MODE_BLOCKS_RESET:
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_blocks_reset, (unit, action , NULL));
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DCMN_RESET_MODE_BLOCKS_SOFT_RESET:
        rv = soc_dfe_init(unit, 0);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DCMN_RESET_MODE_INIT_RESET:
        rv = soc_dfe_init(unit, 1);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DCMN_RESET_MODE_REG_ACCESS:
        
        rv = soc_dfe_info_soc_properties(unit);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_reg_access_only_reset, (unit));
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    case SOC_DCMN_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET:
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_soft_init, (unit, action));
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Unknown/Unsupported Reset Mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_dfe_drv_link_to_dch_block(int unit, int link, int *block_instance, int *inner_link)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping , (unit, link, block_instance, inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

