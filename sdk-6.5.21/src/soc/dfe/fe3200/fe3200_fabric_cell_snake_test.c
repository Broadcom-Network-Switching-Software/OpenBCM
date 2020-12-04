/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC CELL SNAKE TEST
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/allenum.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <sal/core/boot.h>

#if defined(BCM_88950_A0)
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_fabric_cell_snake_test.h>

#include <soc/dfe/fe3200/fe3200_fabric_cell_snake_test.h>
#include <soc/dfe/fe3200/fe3200_drv.h>



STATIC int soc_fe3200_cell_snake_test_reg_reset(int unit);
STATIC int soc_fe3200_cell_snake_test_interrupts_clear(int unit);


int
  soc_fe3200_cell_snake_test_prepare(
    
    int unit, 
    uint32 flags)
{
    
    uint32 reg_val, field_val[1];
    soc_reg_above_64_val_t reg_val_above_64, 
                           block_bitmap_default;
    bcm_port_t port;
    int rv;
    int link_idx;
    SOCDNX_INIT_FUNC_DEFS;

    sal_usleep(100*10000); 

    for(link_idx=0;link_idx<144;link_idx++) {
        DFE_LINK_INPUT_CHECK(unit, link_idx);
        rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_enable_set,(unit, link_idx, 0));
        SOCDNX_IF_ERR_EXIT(rv);
    }
    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_interrupt_all_enable_set, (unit, 0));
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK)) 
	{

        PBMP_SFI_ITER(unit, port)
        {
                if(flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) {
                    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_loopback_set, (unit, port, soc_dcmn_loopback_mode_none));
                    SOCDNX_IF_ERR_EXIT(rv);
                } else {
                    
                    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_phy_cl72_set, (unit, port, 0));
                    SOCDNX_IF_ERR_EXIT(rv);
                    
                    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_loopback_set, (unit, port, soc_dcmn_loopback_mode_phy_gloop));
                    SOCDNX_IF_ERR_EXIT(rv);
                }

                sal_usleep(20*1000);   
        }

        sal_usleep(100*1000); 
    }
        

    



    
   
    
    SOC_REG_ABOVE_64_CLEAR(block_bitmap_default);
    SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_0, 0);
    SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_1, 1);
    SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_FE3200_DRV_BLOCKS_RESET_WITHOUT_FSRD_WORD_2, 2);
    SOC_REG_ABOVE_64_COPY(reg_val_above_64, block_bitmap_default);



    

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_blocks_reset, (unit, 1 , &reg_val_above_64));
    SOCDNX_IF_ERR_EXIT(rv);

    sal_usleep(200);

    SOCDNX_IF_ERR_EXIT(soc_fe3200_cell_snake_test_reg_reset(unit));

    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stat_init, (unit));
    SOCDNX_IF_ERR_EXIT(rv);
   
    
    SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val)); 
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_2_10f, 0); 
 
    
    *field_val = ((!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK)) ? 0x1:0x0);
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_13f, *field_val); 
          
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_12f, 1); 
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_1f, 0); 
 
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));  
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 1);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 1);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));
  
    SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
    soc_reg_above_64_field32_set(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));   

    for(link_idx=0;link_idx<144;link_idx++) {
        rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_enable_set,(unit, link_idx, 1));
        SOCDNX_IF_ERR_EXIT(rv);
    }


    sal_usleep(100*10000); 



exit:
    SOCDNX_FUNC_RETURN;
}
    
    

int
  soc_fe3200_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_fabric_cell_snake_test_results_t* results)
{  
    soc_timeout_t to;
    uint32 reg_val, field_val;
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
 
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_fe3200_cell_snake_test_interrupts_clear(unit));


    sal_memset(results, 0, sizeof(soc_fabric_cell_snake_test_results_t));

    
    SOCDNX_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit,OCCG_INTERRUPT_REGISTERr,&reg_val,TEST_MODE_CMD_FINISH_INTf,0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_INTERRUPT_REGISTERr(unit, reg_val));

    
    if (flags & SOC_DFE_SNAKE_STOP_TEST)
    {
        SOCDNX_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        field_val = soc_reg_field_get(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, reg_val, EXT_WAIT_CELL_PROPf);
        if (field_val != 0x1)
        {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("test is not on %d"),field_val)); 
        }
    }

    
    if (!(flags & SOC_DFE_SNAKE_STOP_TEST)) {  
        SOCDNX_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 0);
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));
    }
  
          
    if (flags & SOC_DFE_SNAKE_INFINITE_RUN)
    {
          
          SOC_EXIT;
    }
    else
    {
          
          sal_usleep(18500);
    }

    
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    
    soc_timeout_init(&to, 1000000 , 100);
  
    while(1)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
        soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, field_val_above_64); 
        if(SOC_REG_ABOVE_64_IS_ZERO(field_val_above_64) || SAL_BOOT_PLISIM) {
                        
            break;
        }
        
        if (soc_timeout_check(&to)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("timeout - test was not triggered"))); 
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

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS 0
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG;
    }

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS 1
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET;
    }

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS 2
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION;
    }

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS 3
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION;
    }

    
    #define _SOC_FE3200_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS 4
    if (field_val & (0x1 << _SOC_FE3200_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND;      
    }
    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS 5
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND;
    }

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS 6
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND;
    }

    
    #define _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS 7
    if (field_val & (0x1 << _SOC_FE3200_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND;
    }

    
    results->lfsr_per_pipe[0] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_0f);
    results->lfsr_per_pipe[1] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_1f);
    results->lfsr_per_pipe[2] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_2f);

    
    
    SOCDNX_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    field_val = soc_reg_field_get(unit,OCCG_INTERRUPT_REGISTERr,reg_val,TEST_MODE_CMD_FINISH_INTf);
    if (field_val != 1) 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("ERROR - configuration error. test did not run\n")));
    }
	
exit:

    cli_out("Warning: Device reset is required for full functionality\n");
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_cell_snake_test_reg_reset(
    int unit)
{

    int i;
    uint32 reg_val32;
    SOCDNX_INIT_FUNC_DEFS;

    
    
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_REG_0102r(unit, 0xf));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_REG_0102r(unit, 0xf));

    
    for(i=0; i<SOC_DFE_DEFS_GET(unit, nof_links_in_mac); i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe3200_cell_snake_test_interrupts_clear(
    int unit)
{

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_DCL_INTERRUPT_REGISTERr(unit, 0xFFFFFFFF));

exit:
    SOCDNX_FUNC_RETURN;
}


#endif 

#undef _ERR_MSG_MODULE_NAME

