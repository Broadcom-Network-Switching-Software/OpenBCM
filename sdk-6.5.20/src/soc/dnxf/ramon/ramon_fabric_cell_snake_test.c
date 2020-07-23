/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC CELL SNAKE TEST
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/allenum.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <sal/core/boot.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <soc/dnxf/ramon/ramon_fabric_cell_snake_test.h>
#include <soc/dnxf/ramon/ramon_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_links.h>
#include <soc/dnxf/ramon/ramon_port.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <bcm_int/dnxf_dispatch.h>



#define SOC_RAMON_SKU_CELL_SNAKE_NOF_DATA_CELLS_OVERRIDE    (47)
#define SOC_RAMON_SKU_CELL_SNAKE_NOF_CREDIT_CELLS_OVERRIDE  (47)

#define SOC_RAMON_SKU_CELL_SNAKE_FIRST_ENABLED_CORE         (0)
#define SOC_RAMON_SKU_CELL_SNAKE_DISABLED_CORE_1            (2)
#define SOC_RAMON_SKU_CELL_SNAKE_DISABLED_CORE_2            (4)
#define SOC_RAMON_SKU_CELL_SNAKE_NOF_CELLS_TO_BE_FILTERED   (1008)
#define SOC_RAMON_SKU_CELL_SNAKE_NOF_FILTER_ELEMENTS        (5)

#define SOC_RAMON_REBOOT_REQUIRED_MESSAGE "Warning: Device reboot is required for full functionality.\nSuggest to use 'tr 141' or 'init_dnx NoDeinit=no' commands. \n\n"



STATIC int soc_ramon_cell_snake_test_reg_reset(int unit);
STATIC int soc_ramon_cell_snake_test_sw_configuration_set(int unit);
STATIC int soc_ramon_cell_snake_test_interrupts_clear(int unit);
STATIC int soc_ramon_cell_snake_test_mac_loopback_set(int unit, int port);
STATIC int soc_ramon_cell_snake_test_disabled_fsrd_blocks_power_set(int unit, int enable);


STATIC int soc_ramon_cell_snake_test_occg_override_nof_cells(int unit)
{
    uint64 reg_val64;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(READ_OCCG_REG_0130r(unit, &reg_val64));
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_0_0f, 1);
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_21_21f, 1);
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_42_42f, 1);
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_1_20f,  SOC_RAMON_SKU_CELL_SNAKE_NOF_DATA_CELLS_OVERRIDE);
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_22_41f, SOC_RAMON_SKU_CELL_SNAKE_NOF_DATA_CELLS_OVERRIDE);
    soc_reg64_field_set(unit, OCCG_REG_0130r, &reg_val64, FIELD_43_62f, SOC_RAMON_SKU_CELL_SNAKE_NOF_DATA_CELLS_OVERRIDE);
    SHR_IF_ERR_EXIT(WRITE_OCCG_REG_0130r(unit, reg_val64));


    
    SHR_IF_ERR_EXIT(READ_OCCG_REG_0132r(unit, &reg_val64));

    soc_reg64_field_set(unit, OCCG_REG_0132r, &reg_val64, FIELD_0_0f, 1);
    soc_reg64_field_set(unit, OCCG_REG_0132r, &reg_val64, FIELD_1_20f, SOC_RAMON_SKU_CELL_SNAKE_NOF_CREDIT_CELLS_OVERRIDE);

    SHR_IF_ERR_EXIT(WRITE_OCCG_REG_0132r(unit, reg_val64));

    
    SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val));
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_2_10f, 16);
    SHR_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));


exit:
    SHR_FUNC_EXIT;
}

STATIC int soc_ramon_cell_snake_test_reconfigure_rtp(int unit)
{
    soc_reg_above_64_val_t enabled_links;
    int port;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(enabled_links);
    PBMP_SFI_ITER(unit, port)
    {
        SHR_BITSET(enabled_links, port);
    }
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_SIMPLE_RR_LINKS_CONFIGr(unit, enabled_links));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_AUTO_DOC_NAME_15r(unit, enabled_links));


exit:
    SHR_FUNC_EXIT;
}

STATIC int soc_ramon_cell_snake_test_reconfigure_ocg_filters(int unit)
{
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 filter_core_2[SOC_RAMON_SKU_CELL_SNAKE_NOF_FILTER_ELEMENTS] = {0xffffffff, 0xffffffff, 0x0004ffff, 0x0aa80000, 0xffd0000};
    uint32 filter_core_4[SOC_RAMON_SKU_CELL_SNAKE_NOF_FILTER_ELEMENTS] = {0xffffffff, 0xffffffff, 0x0004ffff, 0x04440000, 0xffd0000};
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_48r(unit, SOC_RAMON_SKU_CELL_SNAKE_DISABLED_CORE_1, 0));
    SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_48r(unit, SOC_RAMON_SKU_CELL_SNAKE_DISABLED_CORE_2, 0));

    SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_48r(unit, SOC_RAMON_SKU_CELL_SNAKE_FIRST_ENABLED_CORE, SOC_RAMON_SKU_CELL_SNAKE_NOF_CELLS_TO_BE_FILTERED));

    
    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    for(i = 0; i < SOC_RAMON_SKU_CELL_SNAKE_NOF_FILTER_ELEMENTS; i++){
        SOC_REG_ABOVE_64_WORD_SET(reg_val_above_64, filter_core_2[i], i);
    }
    SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, SOC_RAMON_SKU_CELL_SNAKE_FIRST_ENABLED_CORE, reg_val_above_64));
    sal_sleep(1);

    
    SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
    for(i = 0; i < SOC_RAMON_SKU_CELL_SNAKE_NOF_FILTER_ELEMENTS; i++){
        SOC_REG_ABOVE_64_WORD_SET(reg_val_above_64, filter_core_4[i], i);
    }
    SHR_IF_ERR_EXIT(WRITE_DCH_MATCH_FILTr(unit, SOC_RAMON_SKU_CELL_SNAKE_FIRST_ENABLED_CORE, reg_val_above_64));
    sal_sleep(1);

exit:
    SHR_FUNC_EXIT;
}


int
  soc_ramon_cell_snake_test_prepare(
    
    int unit, 
    uint32 flags)
{
    
    uint32 reg_val, field_val[1];
    soc_reg_above_64_val_t reg_val_above_64, 
                           block_bitmap_default;
    bcm_port_t port;
    int rv;
    int nof_links;
    int nof_links_in_phy_core, link_idx_in_phy_core, link;
    int nof_phy_cores, phy_core_idx;
    int nof_active_links_in_phy_core = 0;

    SHR_FUNC_INIT_VARS(unit);


    
    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_disabled_fsrd_blocks_power_set( unit, 1));

    sal_usleep(100*10000); 

    nof_links = dnxf_data_port.general.nof_links_get(unit);
    nof_links_in_phy_core = dnxf_data_device.blocks.nof_links_in_phy_core_get(unit);
    nof_phy_cores = nof_links / nof_links_in_phy_core;

    
    for (phy_core_idx = 0; phy_core_idx < nof_phy_cores; ++phy_core_idx)
    {
        nof_active_links_in_phy_core = 0;
        for(link_idx_in_phy_core = 0; link_idx_in_phy_core < nof_links_in_phy_core; ++link_idx_in_phy_core)
        {
            link = phy_core_idx*nof_links_in_phy_core + link_idx_in_phy_core;
            if(SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), link))
            {
                nof_active_links_in_phy_core++;
            }
        }
        if (nof_active_links_in_phy_core != nof_links_in_phy_core && nof_active_links_in_phy_core != 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "the test is supported only if ALL ports in a phy core are active (probed) or ALL ports in the core are deactivated (detached).\n"
                    "in core %d PART of the ports are active and PART deactivated.\n", phy_core_idx);
        }
    }

    
    if(flags & SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK)
    {
        PBMP_SFI_ITER(unit, port)
        {
            rv = soc_dnxc_port_control_cells_fec_bypass_enable_set(unit, port, 0);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_interrupt_all_enable_set, (unit, 0));
    SHR_IF_ERR_EXIT(rv);

    
    if (!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)) 
	{

        PBMP_SFI_ITER(unit, port)
        {
                if(flags & SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK) {
                    rv = soc_dnxf_port_loopback_set(unit, port, portmodLoopbackCount);
                    SHR_IF_ERR_EXIT(rv);
                } else {
                    rv = soc_dnxf_port_loopback_set(unit, port, portmodLoopbackPhyGloopPMD);
                    SHR_IF_ERR_EXIT(rv);
                }

                sal_usleep(20*1000);   
        }

        sal_usleep(100*1000); 
    }

    
   
    if (flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)
    {
        
        SOC_REG_ABOVE_64_ALLONES(block_bitmap_default);
    }
    else
    {
        
        SOC_REG_ABOVE_64_CLEAR(block_bitmap_default);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_0, 0);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_1, 1);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_2, 2);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_3, 3);
    }

    SOC_REG_ABOVE_64_COPY(reg_val_above_64, block_bitmap_default);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_blocks_reset, (unit, 1 , &reg_val_above_64));
    SHR_IF_ERR_EXIT(rv);

    sal_usleep(200);

    if (!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK))
    {
        SHR_IF_ERR_EXIT(soc_ramon_port_link_up_check(unit, PBMP_SFI_ALL(unit)));
    }

    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_reg_reset(unit));

    if (dnxf_data_fabric.hw_snake.feature_get(unit, dnxf_data_fabric_hw_snake_is_sw_config_required) && !(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK))
    {
        SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_sw_configuration_set(unit));
    }

    
    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_init, (unit));
    SHR_IF_ERR_EXIT(rv);

    
    SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val));

    if (dnxf_data_fabric.hw_snake.feature_get(unit, dnxf_data_fabric_hw_snake_is_sw_config_required))
    {
        *field_val = ((flags & SOC_DNXF_ENABLE_MAC_LOOPBACK) ? 0x1:0x0);
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_0_0f, *field_val);
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_12f, *field_val);
    }
    else
    {
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_12f, 1);
    }

    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_2_10f, 0);

    
    *field_val = ((!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)) ? 0x1:0x0);
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_13f, *field_val); 
          
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_1f, 0); 
 
    SHR_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));  
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 0);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 0);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 0);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    
    SHR_IF_ERR_EXIT(WRITE_RTP_GENERAL_INTERRUPT_REGISTERr(unit, -1));
    SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
    soc_reg_above_64_field32_set(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));

    
    if (!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK))
    {
        PBMP_ITER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)
        {
            rv = soc_ramon_cell_snake_test_mac_loopback_set(unit, port);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    sal_usleep(100*10000); 

exit:
    
    if (flags & SOC_DNXF_ENABLE_MAC_LOOPBACK) {
        cli_out(SOC_RAMON_REBOOT_REQUIRED_MESSAGE);
    }
    SHR_FUNC_EXIT;
}
    
    

int
  soc_ramon_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_dnxf_fabric_cell_snake_test_results_t* results)
{  
    soc_timeout_t to;
    uint32 reg_val, field_val;
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
 
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_interrupts_clear(unit));


    sal_memset(results, 0, sizeof(soc_dnxf_fabric_cell_snake_test_results_t));

    
    SHR_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit,OCCG_INTERRUPT_REGISTERr,&reg_val,TEST_MODE_CMD_FINISH_INTf,0x1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_INTERRUPT_REGISTERr(unit, reg_val));

    
    if (flags & SOC_DNXF_SNAKE_STOP_TEST)
    {
        SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        field_val = soc_reg_field_get(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, reg_val, EXT_WAIT_CELL_PROPf);
        if (field_val != 0x0)
        {
          SHR_ERR_EXIT(_SHR_E_PARAM, "test is not on %d",field_val); 
        }
    }
    if (dnxf_data_device.general.device_id_get(unit) == 0x8797) {
        SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_reconfigure_rtp(unit));

        SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_occg_override_nof_cells(unit));
    }

    
    if (!(flags & SOC_DNXF_SNAKE_STOP_TEST)) {  
        SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 1);
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 0);
        SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));
    }
  
          
    if (flags & SOC_DNXF_SNAKE_INFINITE_RUN)
    {
          
          SHR_EXIT();
    }
    else
    {
          
          sal_usleep(1000 * 1000);
    }

    
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    
    sal_usleep(1000 * 1000);

    if (dnxf_data_device.general.device_id_get(unit) == 0x8797) {
        SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_reconfigure_ocg_filters(unit));
    }
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    
    soc_timeout_init(&to, 1000000 , 100);
  
    while(1)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
        soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, field_val_above_64); 
        if(SOC_REG_ABOVE_64_IS_ZERO(field_val_above_64) || SAL_BOOT_PLISIM) {
                        
            break;
        }
        
        if (soc_timeout_check(&to)) {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "timeout - test was not triggered"); 
        }
    }


    
  
    results->test_failed = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_FAILf);
     
    if (results->test_failed)
    {
        soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_INT_STATUSf, results->interrupts_status); 
    }
    
    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_STAGE_STATUSf, field_val_above_64);
    field_val = field_val_above_64[0];


    results->failure_stage_flags = 0;

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS 0
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG;
    }

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS 1
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET;
    }

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS 2
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION;
    }

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS 3
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION;
    }

    
    #define _SOC_RAMON_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS 4
    if (field_val & (0x1 << _SOC_RAMON_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND;      
    }
    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS 5
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND;
    }

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS 6
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND;
    }

    
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS 7
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND;
    }
    
    
    SHR_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    field_val = soc_reg_field_get(unit,OCCG_INTERRUPT_REGISTERr,reg_val,TEST_MODE_CMD_FINISH_INTf);
    if (field_val != 1) 
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR - configuration error. test did not run\n");
    }

exit:

    
    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_disabled_fsrd_blocks_power_set(unit, 0));

    cli_out(SOC_RAMON_REBOOT_REQUIRED_MESSAGE);
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_cell_snake_test_reg_reset(
    int unit)
{

    int i;
    uint32 reg_val32;
    bcm_port_t port;
    int port_enable_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    PORTMOD_PORT_ENABLE_MAC_SET(port_enable_flags);
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 0));
    }

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_AUTO_DOC_NAME_12r(unit, 0xf));

    
    for(i=0; i<dnxf_data_device.blocks.nof_links_in_fmac_get(unit); i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

    
    SHR_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, REG_PORT_ANY, &reg_val32));
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32 ,LNK_LVL_FC_TX_ENf, 0);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32 ,LNK_LVL_FC_RX_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 1));
    }


exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_cell_snake_test_sw_configuration_set(int unit)
{
    int index;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above64_val;
    bcm_port_t port;
    int port_enable_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_INTERRUPT_MASK_REGISTERr(unit,  0x3EF7B87F));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_DCH_ENABLERS_REGISTER_1r(unit, 0x7C00038));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_PIPES_SEPARATION_REGISTERr(unit, 0xA4A4));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_AUTO_DOC_NAME_0r(unit, 0x3));

    for (index = 0 ; index < dnxf_data_fabric.general.max_nof_pipes_get(unit) ; index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_DCH_DCH_ENABLERS_REGISTER_2_Pr, REG_PORT_ANY, index, 0x7E00002));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_DCH_DCH_ENABLERS_REGISTER_3_Pr, REG_PORT_ANY, index, 0x1));
    }

    COMPILER_64_ALLONES(reg64_val);
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, reg64_val));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg64_val));

    
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_GENERAL_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));

    COMPILER_64_SET(reg64_val, 0x00000004, 0x00000000);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, reg64_val));

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_WORD_SET(reg_above64_val, 0x00000000, 0);
    SOC_REG_ABOVE_64_WORD_SET(reg_above64_val, 0x003FC000, 1);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above64_val));

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,  0x0));

    COMPILER_64_SET(reg64_val, 0x0000ffff, 0x00000038);
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_DRH_LOAD_BALANCING_GENERAL_CONFIGr(unit, reg64_val));
    COMPILER_64_SET(reg64_val, 0x00000000, 0x50029FF0);
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr(unit, reg64_val));

    
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_FPC_FREE_ERROR_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_FPC_ALLOC_ERROR_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));

    SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, BRDC_DCML_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg_above64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, BRDC_DCML_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg_above64_val));

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DTML_ENABLERSr(unit, 0x100001));

    
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_INTERRUPT_MASK_REGISTERr(unit, 0x5F9));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_CCH_CONFIGURATIONSr(unit, 0xe));

    
    
    PORTMOD_PORT_ENABLE_MAC_SET(port_enable_flags);
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 0));
    }

    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFEFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFEFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAC_INTERRUPT_MASK_REGISTER_1r(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAC_INTERRUPT_MASK_REGISTER_8r(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_TOPO_MODE_REG_0r(unit, 0xff));

    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_RECEIVE_RESET_REGISTERr(unit,  0x0));

    for (index = 0 ; index < dnxf_data_device.blocks.nof_links_in_fmac_get(unit) ; index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, REG_PORT_ANY, index, 0x80400));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_GENERAL_CONFIGURATIONr, REG_PORT_ANY, index, 0x236c));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, REG_PORT_ANY, index, 0x80000E09));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, REG_PORT_ANY, index,  0x11));
    }

    
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 1));
    }

    COMPILER_64_SET(reg64_val, 0x001FFFE0, 0x0001FF03);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_INTERRUPT_MASK_REGISTERr(unit, reg64_val));
    COMPILER_64_SET(reg64_val, 0x00001800, 0x18999086);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, reg64_val));



exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_cell_snake_test_interrupts_clear(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    
    

    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_cell_snake_test_mac_loopback_set(int unit, int port)
{
    uint32 reg_val;
    uint32 field_val[1];
    int fmac_block, fmac_inner_link;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_drv_link_to_block_mapping(unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC));

    
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMALN_CORE_LOOPBACKf, 1);
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_SRD_BACKPRESSURE_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg_val));

    
    SHR_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    SHR_BITCLR(field_val, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *field_val);
    SHR_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, reg_val));

    
    SHR_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    SHR_BITCLR(field_val, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *field_val);
    SHR_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, reg_val));

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_cell_snake_test_disabled_fsrd_blocks_power_set(int unit, int power_up)
{

    int fsrd_blcok_idx = 0, fmac_block_idx = 0;
    soc_pbmp_t enabled_fmacs_bmp;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_port_enabled_fmac_blocks_get(unit, &enabled_fmacs_bmp));
    for (fsrd_blcok_idx = 0; fsrd_blcok_idx < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); fsrd_blcok_idx++)
    {
        fmac_block_idx = fsrd_blcok_idx * dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit);
        if(!SOC_PBMP_MEMBER(enabled_fmacs_bmp, fmac_block_idx))
        {
            SHR_IF_ERR_EXIT(soc_ramon_port_fsrd_block_power_set(unit, fsrd_blcok_idx * dnxf_data_device.blocks.nof_links_in_fsrd_get(unit), power_up));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

