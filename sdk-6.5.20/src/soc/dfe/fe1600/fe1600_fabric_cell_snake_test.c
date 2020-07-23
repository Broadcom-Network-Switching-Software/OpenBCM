/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 FABRIC CELL SNAKE TEST
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
#include <shared/bitop.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/cmn/dfe_port.h>


#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_fabric_cell_snake_test.h>
#include <soc/dfe/fe1600/fe1600_port.h>
#include <soc/phyctrl.h>

#include <soc/dfe/fe1600/fe1600_stack.h>
#include <soc/dfe/fe1600/fe1600_fabric_cell.h>
#include <soc/dfe/fe1600/fe1600_fabric_links.h>
#include <sal/appl/sal.h>


#ifdef BCM_88754_A0
extern int soc_fe1600_fabric_cell_snake_test_bcm887540_links_check(int unit, int reset_unstable_links);
#endif



int
  soc_fe1600_cell_snake_test_prepare(
    int unit, 
    uint32 flags)
{
    uint32 reg_val, field_val[1];
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
    soc_dcmn_port_pcs_t pcs;
    bcm_port_t p, inner_p;
    int blk_id;
    uint64 value;
    uint32 reset_val_lo, reset_val_hi;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

#ifdef BCM_88750_B0
    if ((flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) && SOC_IS_FE1600_B0_AND_ABOVE(unit) && !SOC_IS_BCM88754_A0(unit))
    { 
        for (p=0; p<SOC_DFE_DEFS_GET(unit, nof_links); p++) {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_control_set(unit,p,BCM_PORT_PHY_CONTROL_RX_RESET,1));

        }
        sal_usleep(100*1000);     
    }
#endif

        
#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        int loopback = 0;

        
        if (flags & SOC_DFE_ENABLE_MAC_LOOPBACK)
        {
            loopback = SOC_FE1600_CELL_SNAKE_TEST_BCM88754_MAC_LOOPBACK;
        } else if (flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK)
        {
            loopback = SOC_FE1600_CELL_SNAKE_TEST_BCM88754_EXTERNAL_LOOPBACK;
        } else if (flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK) {
            loopback = SOC_FE1600_CELL_SNAKE_TEST_BCM88754_SIF_CORE_LOOPBACK;
        } else {
            loopback = SOC_FE1600_CELL_SNAKE_TEST_BCM88754_EXTERNAL_LOOPBACK ;
        }
        SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val));
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, LOOP_CONFIGf, loopback);
        SOCDNX_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));

    }
#endif 
    
    if (!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK) && !(flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK) ) 
    {
          
        if(!(flags & SOC_DFE_DONT_TOUCH_MAC_INTERRUPTS)) {
            for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++) {            
                blk_id = INT_DEVIDE(p, 4);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTERr_REG32(unit,blk_id,0x0));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_1r(unit,blk_id,0x0));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_8r(unit,blk_id,0x0));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_9r(unit,blk_id,0x0));            
            }
        }

        for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++) {                                
            if(flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) {
                if (!SOC_IS_BCM88754_A0(unit))
                {
                SOCDNX_IF_ERR_EXIT(soc_fe1600_port_loopback_set(unit, p, soc_dcmn_loopback_mode_none));
                }
            } else {
                
                SOCDNX_IF_ERR_EXIT(soc_fe1600_port_phy_cl72_set(unit, p, 0));
                
                SOCDNX_IF_ERR_EXIT(soc_fe1600_port_loopback_set(unit, p, soc_dcmn_loopback_mode_phy_gloop));
            }

            sal_usleep(20*1000);   
        }

         
        if(!(flags & SOC_DFE_DONT_TOUCH_MAC_INTERRUPTS)) {
            for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++) {                
                blk_id = INT_DEVIDE(p, 4);
                
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTERr_REG32(unit,blk_id,0xFFFFFFFF));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_1r(unit,blk_id,0xFFFFFFFF));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_8r(unit,blk_id,0xFFFFFFFF));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_9r(unit,blk_id,0xFFFFFFFF));

                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTERr_REG32(unit,blk_id,0x181));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_1r(unit,blk_id,0xff));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_8r(unit,blk_id,0xff));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_MASK_REGISTER_9r(unit,blk_id,0x3));                                 
            }
        }


        sal_usleep(100*1000); 
    }


    
    reset_val_lo = 0xFF9999FF;
    reset_val_hi = 0x7;

    if (!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK) && !(flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK))
    { 
        uint32 port_power_flags;

        if (SOC_IS_BCM88754_A0(unit))
        {
            port_power_flags = SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE;
        } else {
            port_power_flags = (flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) ? SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE : 0; 
        }
        for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++) {
            SOCDNX_IF_ERR_EXIT(soc_fe1600_port_control_power_set(unit, p, port_power_flags, soc_dcmn_port_power_off));
        }
    }
    COMPILER_64_SET(value, reset_val_hi, reset_val_lo);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit, value));
    sal_usleep(200); 
    COMPILER_64_SET(value, 0, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit, value));

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit, 0x1ff)); 
    sal_usleep(10); 
    if (!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK) && !(flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK))
    { 
        uint32 port_power_flags;

        if (SOC_IS_BCM88754_A0(unit))
        {
            port_power_flags = SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE;
        } else {
            port_power_flags = (flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) ? SOC_DCMN_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE : 0; 
        }
        for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++) {

            SOCDNX_IF_ERR_EXIT(soc_fe1600_port_control_power_set(unit, p, port_power_flags, soc_dcmn_port_power_on));
        }
    }
    
#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit) && !((flags & SOC_DFE_ENABLE_MAC_LOOPBACK) || (flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK)))
    {
        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_cell_snake_test_bcm887540_links_check(unit, 1));
    }
#endif
    SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val));
      
    *field_val = soc_reg_field_get(unit, OCCG_TEST_MODE_CMD_REGr, reg_val, TEST_MODE_SETTINGf);
    
    
    #define FE1600_SNAKE_TEST_DATA_FIFO_POS 0
    #define FE1600_SNAKE_TEST_DATA_FIFO_LOAD_LOW 0x0
    SHR_BITCLR_RANGE(field_val, FE1600_SNAKE_TEST_DATA_FIFO_POS, 2);
    *field_val |= (FE1600_SNAKE_TEST_DATA_FIFO_LOAD_LOW << FE1600_SNAKE_TEST_DATA_FIFO_POS);
   
    
    #define FE1600_SNAKE_TEST_CONTROL_FIFO_POS 2
    #define FE1600_SNAKE_TEST_CONTROL_FIFO_LOAD_LOW 0x0
    SHR_BITCLR_RANGE(field_val, FE1600_SNAKE_TEST_CONTROL_FIFO_POS, 2);
    *field_val |= (FE1600_SNAKE_TEST_CONTROL_FIFO_LOAD_LOW << FE1600_SNAKE_TEST_CONTROL_FIFO_POS);
    
    SHR_BITCLR_RANGE(field_val, 4, 1);
    *field_val |= (0x1 << 4);

    
    #define FE1600_SNAKE_TEST_RESERVED_POS 5
    SHR_BITCLR_RANGE(field_val, FE1600_SNAKE_TEST_RESERVED_POS, 2);
    *field_val |= (0x0 << FE1600_SNAKE_TEST_RESERVED_POS);

    
    #define FE1600_SNAKE_TEST_FILTER_POS 7
    SHR_BITCLR_RANGE(field_val, FE1600_SNAKE_TEST_FILTER_POS, 2);
    *field_val |= (0x3 << FE1600_SNAKE_TEST_FILTER_POS);

    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, TEST_MODE_SETTINGf, *field_val);

    if (!SOC_IS_BCM88754_A0(unit))
    {
        *field_val = ((!(flags & SOC_DFE_ENABLE_MAC_LOOPBACK)) ? 0x1:0x0);
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, DIS_MAC_IN_LOOPBACKf, *field_val);
    }
          
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, DE_RESET_BEFORE_TEST_MODEf, 0);
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, RESET_REG_AFTER_TEST_MODEf, 0);

    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));  
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 1);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 1);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));
  
    SOCDNX_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
    SOC_REG_ABOVE_64_CREATE_MASK(field_val_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, field_val_above_64);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));   
    
#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit) && !((flags & SOC_DFE_ENABLE_MAC_LOOPBACK) || (flags & SOC_DFE_ENABLE_ASYNC_FIFO_LOOPBACK)))
    {
        int port, speed;
        int fmac_blk_id, fmac_inner_link;

        PBMP_SFI_ITER(unit, port)
        {
            SOCDNX_IF_ERR_EXIT(soc_phyctrl_speed_get(unit, port , &speed));
            if (speed == 6250)
            {
                fmac_blk_id = port / SOC_FE1600_NOF_LINKS_IN_MAC;
                fmac_inner_link = port % SOC_FE1600_NOF_LINKS_IN_MAC;

                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, fmac_inner_link, &reg_val));
                soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_FORCE_SIGNAL_DETECTf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id, fmac_inner_link, reg_val));

                SOCDNX_IF_ERR_EXIT(READ_FMAC_CONTROL_CELL_BURST_REGISTERr(unit, fmac_blk_id, &reg_val));
                soc_reg_field_set(unit, FMAC_CONTROL_CELL_BURST_REGISTERr, &reg_val, MAX_CTRL_CELL_BURSTf, 1);
                soc_reg_field_set(unit, FMAC_CONTROL_CELL_BURST_REGISTERr, &reg_val, CNTRL_BURST_PERIODf, 0xc);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_CONTROL_CELL_BURST_REGISTERr(unit, fmac_blk_id, reg_val));

            }
        }
    }
#endif
    for(p = 0 ; p < SOC_DFE_DEFS_GET(unit, nof_links) ; p++)
    {
        inner_p = p % 4;
        blk_id = INT_DEVIDE(p, 4);

        SOCDNX_IF_ERR_EXIT(soc_fe1600_port_control_pcs_get(unit, p,  &pcs));

        
        reg_val = 0;
        *field_val = 0x1 << inner_p;
        if(soc_dcmn_port_pcs_8_10 == pcs) {
            soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_2r(unit, blk_id, reg_val));
        } else {        
            soc_reg_field_set(unit, FMAC_INTERRUPT_REGISTER_4r, &reg_val, OOF_INTf, *field_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_4r(unit, blk_id, reg_val));
        }      
    }

    sal_usleep(20*1000); 

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_DRH_LOAD_BALANCING_CONFIGr(unit, &reg_val));
    soc_reg_field_set(unit, RTP_DRH_LOAD_BALANCING_CONFIGr, &reg_val, LOAD_BALANCE_LEVELS_IGNOREf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_DRH_LOAD_BALANCING_CONFIGr(unit, reg_val));
        

    
#ifdef BCM_88750_B0
    if ((flags & SOC_DFE_ENABLE_EXTERNAL_LOOPBACK) && SOC_IS_FE1600_B0_AND_ABOVE(unit) && !SOC_IS_BCM88754_A0(unit))
    { 
        int fmac_port, fmac_index;
        
        for (p=0; p < SOC_DFE_DEFS_GET(unit, nof_links); p++) {
            fmac_port = p/SOC_FE1600_NOF_LINKS_IN_MAC;
            fmac_index = p%SOC_FE1600_NOF_LINKS_IN_MAC;
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_port, fmac_index, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FIELD_8_8f, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_port, fmac_index, reg_val));
        }
        
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_RECEIVE_RESET_REGISTERr(unit, 0xf000f));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_RECEIVE_RESET_REGISTERr(unit, 0xf000f));

        sal_usleep(100*1000); 
    }
#endif

     sal_usleep(100*1000); 

exit:
    SOCDNX_FUNC_RETURN;
}


#ifdef BCM_88754_A0
int
soc_fe1600_fabric_cell_snake_test_bcm887540_links_check(int unit, int reset_unstable_links)
{
    int reset_links, fmac_index, recheck_links, iter;
    uint32 reg_val, field_val;
    int p;
    SOCDNX_INIT_FUNC_DEFS;
    
    recheck_links = reset_unstable_links;
    iter = 0;
    
    if (!reset_unstable_links)
    {
        sal_sleep(30); 
    }

     
     for (fmac_index = 0; fmac_index < SOC_FE1600_NOF_INSTANCES_MAC; fmac_index++)
     {
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTERr_REG32(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_1r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_3r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_4r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_5r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_6r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_7r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_8r(unit, fmac_index, -1));
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_9r(unit, fmac_index, -1));
    }
    
    
    while (recheck_links)
    {
        recheck_links = 0;
        for (fmac_index = 0; fmac_index < SOC_FE1600_NOF_INSTANCES_MAC; fmac_index++)
        {
            reset_links = 0;
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTERr_REG32(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_1r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_index, &reg_val));
            field_val = soc_reg_field_get(unit, FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
            if (field_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_3r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_5r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_6r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_7r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_8r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            SOCDNX_IF_ERR_EXIT(READ_FMAC_INTERRUPT_REGISTER_9r(unit, fmac_index, &reg_val));
            if (reg_val != 0)
            {
                reset_links = 1;
            }
            if (reset_links)
            {
                recheck_links = 1;
                
                for(p = 0 ; p < SOC_FE1600_NOF_LINKS_IN_MAC; p++)
                {
                    SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, fmac_index * SOC_FE1600_NOF_LINKS_IN_MAC + p, 0));
                    sal_usleep(20*1000); 
                    SOCDNX_IF_ERR_EXIT(soc_phyctrl_enable_set(unit, fmac_index * SOC_FE1600_NOF_LINKS_IN_MAC + p, 1));
                }
                
                sal_usleep(20*1000); 

                
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTERr_REG32(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_1r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_2r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_3r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_4r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_5r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_6r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_7r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_8r(unit, fmac_index, -1));
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_INTERRUPT_REGISTER_9r(unit, fmac_index, -1));
            }
        }
        
        sal_sleep(10); 
        iter++;
        if (iter  > 2)
        {
            break;
        }                   
    }
    
    if (!reset_unstable_links)
    {
        sal_sleep(20); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}
#endif 
        
    

int
  soc_fe1600_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_fabric_cell_snake_test_results_t* results)
{  
    soc_timeout_t to;
    uint32 reg_val, field_val;
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
    soc_port_t p;

    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);

    sal_memset(results, 0, sizeof(soc_fabric_cell_snake_test_results_t));

#ifdef BCM_88750_B0
    if (SOC_IS_FE1600_B0_AND_ABOVE(unit) && !SOC_IS_BCM88754_A0(unit))
    { 
        int fmac_port, fmac_index;
        
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_RECEIVE_RESET_REGISTERr(unit, 0x0));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_RECEIVE_RESET_REGISTERr(unit, 0x0));
        sal_usleep(100*1000); 
        for (p=0; p<SOC_DFE_DEFS_GET(unit, nof_links); p++) {
            SOCDNX_IF_ERR_EXIT(bcm_port_phy_control_set(unit,p,BCM_PORT_PHY_CONTROL_RX_RESET,0));
        }

        sal_usleep(1000*1000); 

        for (p=0; p < SOC_DFE_DEFS_GET(unit, nof_links); p++) {
            fmac_port = p/SOC_FE1600_NOF_LINKS_IN_MAC;
            fmac_index = p%SOC_FE1600_NOF_LINKS_IN_MAC;
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_port, fmac_index, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FIELD_8_8f, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_port, fmac_index, reg_val));
        }
        
        sal_usleep(100*1000); 
        
    }
#endif

#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_cell_snake_test_bcm887540_links_check(unit, 0));
        sal_sleep(10); 
        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_cell_snake_test_bcm887540_links_check(unit, 0));
    }
#endif

    

    SOCDNX_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit,OCCG_INTERRUPT_REGISTERr,&reg_val,TEST_MODE_CMD_FINISH_INTf,0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_OCCG_INTERRUPT_REGISTERr(unit, reg_val));

    
    if (flags & SOC_DFE_SNAKE_STOP_TEST)
    {
        SOCDNX_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        field_val = soc_reg_field_get(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, reg_val, EXT_WAIT_CELL_PROPf);
        if (field_val != 0x1)
        {
          SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR("test is not on %d"),field_val)); 
        }
    }

    if (!(flags & SOC_DFE_SNAKE_STOP_TEST)) {  
        
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));

        
        soc_intr_block_lo_disable(unit, 0xFFFFFFFF);
        soc_intr_block_hi_disable(unit, 0xFFFFFFFF);

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
        if(SOC_REG_ABOVE_64_IS_ZERO(field_val_above_64)) {
                        
            break;
        }
        
        if (soc_timeout_check(&to)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG_STR("timeout"))); 
        }
    }
    
    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_FAILf, field_val_above_64);
    results->test_failed = field_val_above_64[0];

    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_INT_STATUSf, results->interrupts_status);

    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_STAGE_STATUSf, field_val_above_64);
    field_val = field_val_above_64[0];


    results->failure_stage_flags = 0;

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS 0
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS 1
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS 2
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS 3
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS 4
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND;      
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS 5
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS 6
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND;
    }

    #define FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS 7
    if (field_val & (0x1 << FE1600_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DFE_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND;
    }

    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TDM_LFSR_VALUEf, field_val_above_64);
    results->lfsr_per_pipe[0] = results->tdm_lfsr_value = field_val_above_64[0];
    

    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, NON_TDM_LFSR_VALUEf, field_val_above_64);
    results->lfsr_per_pipe[1] = results->non_tdm_lfsr_value = field_val_above_64[0];

    
    
    SOCDNX_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    field_val = soc_reg_field_get(unit,OCCG_INTERRUPT_REGISTERr,reg_val,TEST_MODE_CMD_FINISH_INTf);
    if (field_val != 1) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("ERROR - configuration error. test did not run\n")));
    }
	
exit:
    if (!(flags & SOC_DFE_SNAKE_INFINITE_RUN))
    {
      
      soc_intr_block_lo_enable(unit, 0xFFFFFFFF);
      soc_intr_block_hi_enable(unit, 0xFFFFFFFF);
    }

    SOCDNX_FUNC_RETURN;
}
 

#endif 

#undef _ERR_MSG_MODULE_NAME

