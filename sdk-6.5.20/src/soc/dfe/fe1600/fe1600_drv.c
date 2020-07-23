/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 DRV
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <shared/shr_template.h>

#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/mem.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_fabric_topology.h>
#include <soc/dfe/fe1600/fe1600_drv.h>
#include <soc/dfe/fe1600/fe1600_port.h>
#include <soc/dfe/fe1600/fe1600_config_defs.h>
#include <soc/dfe/fe1600/fe1600_config_imp_defs.h>
#include <shared/bitop.h>
#include <soc/register.h>
#include <soc/dcmn/dcmn_mbist.h>

extern char *_build_release;

#if defined(BCM_88750_A0)


#define SOC_FE1600_GRACEFUL_SHUT_DOWN_DISABLE_DELAY 2000000 


#define _SOC_FE1600_PVT_MON_NOF                             (4)
#define _SOC_FE1600_PVT_FACTOR                              (5660)
#define _SOC_FE1600_PVT_BASE                                (4283900)


#define _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE1          (11)
#define _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE1            (2)
#define _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE2          (11)
#define _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE2            (1)
#define _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE3          (12)
#define _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE3            (1)
#define _SOC_FE1600_DRV_COMMA_BURST_PERIOD_REPEATER     (_SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE2)
#define _SOC_FE1600_DRV_COMMA_BURST_SIZE_REPEATER       (_SOC_FE1600_DRV_COMMA_BURST_SIZE_FE2)


static
soc_reg_t fe1600_interrupt_monitor_mem_reg[] = {
    CCS_ECC_ERR_MONITOR_MEM_MASKr,
    DCH_ECC_ERR_MONITOR_MEM_MASKr,
    DCL_ECC_ERR_MONITOR_MEM_MASKr,
    DCMA_ECC_ERR_MONITOR_MEM_MASK_Ar,
    DCMB_ECC_ERR_MONITOR_MEM_MASK_Ar,
    ECI_ECC_ERR_MONITOR_MEM_MASKr,
    BRDC_FMACH_ECC_1B_ERR_MONITOR_MEM_MASKr,
    BRDC_FMACH_ECC_2B_ERR_MONITOR_MEM_MASKr,
    BRDC_FMACL_ECC_1B_ERR_MONITOR_MEM_MASKr,
    BRDC_FMACL_ECC_2B_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_1B_ERR_MONITOR_MEM_MASKr,
    FMAC_ECC_2B_ERR_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    RTP_PAR_ERR_MEM_MASKr,
    INVALIDr
};

static
soc_reg_t fe1600_interrupts_mask_registers[] = {
    CCS_INTERRUPT_MASK_REGISTERr,
    CCS_CAPTURE_FILTER_MASK_0r,
    CCS_CAPTURE_FILTER_MASK_1r,
    DCH_INTERRUPT_MASK_REGISTERr,
    DCH_INTERRUPT_MASK_REGISTER_1r,
    DCH_INTERRUPT_MASK_REGISTER_2r,
    DCH_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_0r,
    DCH_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_1r,
    DCH_PROGRAMMABLE_DATA_CELL_COUNTER_MASK_2r,
    DCH_ERROR_FILTER_MASKr,
    DCH_ERROR_FILTER_MASK_ENr,
    DCL_INTERRUPT_MASK_REGISTERr,
    DCMA_INTERRUPT_MASK_REGISTERr,
    DCMB_INTERRUPT_MASK_REGISTERr,
    ECI_INTERRUPT_MASK_REGISTERr,
    ECI_MAC_INTERRUPT_MASK_REGISTERr,
    ECI_ECI_INTERNAL_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTERr,
    FMAC_INTERRUPT_MASK_REGISTER_1r,
    FMAC_INTERRUPT_MASK_REGISTER_2r,
    FMAC_INTERRUPT_MASK_REGISTER_3r,
    FMAC_INTERRUPT_MASK_REGISTER_4r,
    FMAC_INTERRUPT_MASK_REGISTER_5r,
    FMAC_INTERRUPT_MASK_REGISTER_6r,
    FMAC_INTERRUPT_MASK_REGISTER_7r,
    FMAC_INTERRUPT_MASK_REGISTER_8r,
    FMAC_INTERRUPT_MASK_REGISTER_9r,
    FSRD_INTERRUPT_MASK_REGISTERr,
    FSRD_QUAD_INTERRUPT_MASK_REGISTERr,
    FSRD_WC_UC_MEM_MASK_BITMAPr,
    OCCG_INTERRUPT_MASK_REGISTERr,
    RTP_INTERRUPT_MASK_REGISTERr,
    RTP_DRHP_INTERRUPT_MASK_REGISTERr,
    RTP_DRHS_INTERRUPT_MASK_REGISTERr,
    RTP_CRH_INTERRUPT_MASK_REGISTERr,
    RTP_GENERAL_INTERRUPT_MASK_REGISTERr,
    RTP_ECC_1B_ERR_INTERRUPT_MASK_REGISTERr,
    RTP_ECC_2B_ERR_INTERRUPT_MASK_REGISTERr,
    RTP_PAR_ERR_INTERRUPT_MASK_REGISTERr,
    INVALIDr
};

int
soc_dfe_clean_rtp_table_array(int unit, soc_mem_t mem, soc_reg_above_64_val_t data)
{
    uint32 reg_val32;
    unsigned i, elem_num;
    uint32 current_address, element_skip;        
    uint64  val64;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_MEM_IS_ARRAY_SAFE(unit,mem))
    {
        elem_num = SOC_MEM_NUMELS(unit, mem);
        element_skip = SOC_MEM_ELEM_SKIP(unit, mem);
    }
    else
    {
        elem_num = 1;
        element_skip = 0;
    }
    current_address = SOC_MEM_INFO(unit, mem).base;
    
    for (i = 0; i < elem_num; ++i) { 
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_WR_DATAr(unit, data));
        COMPILER_64_ZERO(val64);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_DATA_INCREMENTr(unit, val64));
    
        reg_val32 = 0;
        soc_reg_field_set(unit, RTP_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_ADDRf, current_address);

        soc_reg_field_set(unit, RTP_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_TYPEf, 0 );
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_ADDRESSr(unit, reg_val32));
    
        reg_val32 = 0;
        soc_reg_field_set(unit, RTP_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_COUNTf, SOC_MEM_INFO(unit, mem).index_max+1);
        soc_reg_field_set(unit, RTP_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_TRIGGERf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMANDr(unit, reg_val32));

        current_address += element_skip;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#define RTP_INDIRECT_COUNT (8*1024)

STATIC int
soc_fe1600_reset_tables(int unit)
{
    soc_reg_above_64_val_t data, field;
    int total_links, active_links;
    uint32 score, entry;
    uint32 bmp[4];
    uint32 totsf_val, slsct_val, score_slsct, links_count, sctinc_val, sctinc;
    int link;
	soc_field_t scrub_en;
	uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    scrub_en = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, SCT_SCRUB_ENABLEf); 
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, 0);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    SOC_REG_ABOVE_64_CLEAR(data);

    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MCLBTPm, data));
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MCLBTSm, data));

    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_RCGLBTm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_TOTSFm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_SLSCTm, data));
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_SCTINCm, data));

    
    SOC_REG_ABOVE_64_CLEAR(field);
    soc_mem_field_set(unit, RTP_MEM_800000m, data, ITEM_1f, field);
    
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MEM_800000m, data));
    SOCDNX_IF_ERR_EXIT(soc_dfe_clean_rtp_table_array(unit, RTP_MEM_900000m, data));

    totsf_val = 0;
    links_count = 1;
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, LINK_NUMf, (uint32*)&links_count);

    slsct_val = 0;
    score_slsct = 0;
    soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, LINK_NUMf, &score_slsct);

    sctinc_val = 0;
    sctinc = 0;
    soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, LINK_NUMf, &sctinc);

    for(link = 0 ; link < SOC_DFE_DEFS_GET(unit, nof_links) ; link++) {
        
        bmp[0] = bmp[1] = bmp[2] = bmp[3] = 0;
        SHR_BITSET(bmp,link);

        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
    }

    
    for(total_links = 1 ; total_links <= 64 ; total_links++) {
        for(active_links = 1 ; active_links <= total_links ; active_links++) {
            score = (SOC_FE1600_MAX_LINK_SCORE * active_links) / total_links;
            if ((SOC_FE1600_MAX_LINK_SCORE * active_links) % total_links != 0) {
                score = score + 1;
            }
            if(active_links < 33) {
                entry=(total_links-1)*32+active_links-1;
            } else {
                entry = (64-total_links)*32+64-active_links; 
            }
             SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_TBm(unit, MEM_BLOCK_ALL, entry, &score));
        }
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, scrub_en);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe1600_reset_cmic_regs(int unit) 
{
    int lcpll, i;
    int dividend, divisor, mdio_delay;
    uint32 rval;
    uint32 lcpll_in, lcpll_out, control_1, control_3;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    for(lcpll=0 ; lcpll<SOC_FE1600_NOF_LCPLL ; lcpll++) {

#ifdef BCM_88754_A0
        if (SOC_IS_BCM88754_A0(unit))
        {
            lcpll_in = 1;
            lcpll_out = 1;
        } else 
#endif 
        {
            lcpll_in = soc_property_suffix_num_get(unit, lcpll, spn_SERDES_FABRIC_CLK_FREQ, "in", 1);
            lcpll_out = soc_property_suffix_num_get(unit, lcpll, spn_SERDES_FABRIC_CLK_FREQ, "out", 1);
        }

        
        switch(lcpll_out) {
            case soc_dcmn_init_serdes_ref_clock_125:
                control_1 = SOC_FE1600_LCPLL_CONTROL1_125_VAL;
                break;
            case soc_dcmn_init_serdes_ref_clock_156_25:
                control_1 = SOC_FE1600_LCPLL_CONTROL1_156_25_VAL;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("lcpll_out: %d is out-of-ranget (use 0=125MHz, 1=156.25MHz"), lcpll_out));
        }

        switch(lcpll_in) {
            case soc_dcmn_init_serdes_ref_clock_125:
                control_3 = SOC_FE1600_LCPLL_CONTROL3_125_VAL;
                break;
            case soc_dcmn_init_serdes_ref_clock_156_25:
                control_3 = SOC_FE1600_LCPLL_CONTROL3_156_25_VAL;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("lcpll_out: %d is out-of-ranget (use 0=125MHz, 1=156.25MHz"), lcpll_out));
        }

        switch(lcpll) {
             case 0:
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS0_PLL_CONTROL_1r(unit, control_1));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS0_PLL_CONTROL_2r(unit, 0x0064c000));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS0_PLL_CONTROL_3r(unit, control_3));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS0_PLL_CONTROL_4r(unit, 0x15c00000));
                break;
             case 1:
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS1_PLL_CONTROL_1r(unit, control_1));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS1_PLL_CONTROL_2r(unit, 0x0064c000));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS1_PLL_CONTROL_3r(unit, control_3));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS1_PLL_CONTROL_4r(unit, 0x15c00000));
                break;
             case 2:
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS2_PLL_CONTROL_1r(unit, control_1));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS2_PLL_CONTROL_2r(unit, 0x0064c000));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS2_PLL_CONTROL_3r(unit, control_3));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS2_PLL_CONTROL_4r(unit, 0x15c00000));
                break;
             case 3:
            default:
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS3_PLL_CONTROL_1r(unit, control_1));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS3_PLL_CONTROL_2r(unit, 0x0064c000));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS3_PLL_CONTROL_3r(unit, control_3));
                SOCDNX_IF_ERR_EXIT(WRITE_CMIC_XGXS3_PLL_CONTROL_4r(unit, 0x15c00000));
                break;
        }
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MISC_CONTROLr(unit, 0x00000f00)); 

    
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SOFT_RESET_REGr(unit, 0x0));
    sal_usleep(20);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SOFT_RESET_REGr(unit, 0xffffffff));


   
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_0r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_0r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_1r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_1r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_2r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_2r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_3r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_3r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_4r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_4r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_5r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_5r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_6r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_6r).rst_val_lo));
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_SBUS_RING_MAP_7r(unit, SOC_REG_INFO(unit, CMIC_SBUS_RING_MAP_7r).rst_val_lo));

     
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 0,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 1,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 2,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 3,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 4,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 5,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 6,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 7,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 8,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 9,  61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 10, 61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 11, 61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 12, 61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 13, 61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 14, 61));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 15, 61));
    
    
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 16, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 17, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 18, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 19, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 20, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 21, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 22, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 23, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 24, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 25, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 26, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 27, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 28, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 29, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 30, 62));
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, 31, 62));

    
#ifdef BCM_88754_A0
    if (!SOC_IS_BCM88754_A0(unit))
    {
#endif
        for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd) ; i++) { 
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, 60));
        }
#ifdef BCM_88754_A0
    }
#endif
    
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 24);
    rval = 0;
    SOCDNX_IF_ERR_EXIT(READ_CMIC_RATE_ADJUST_INT_MDIOr(unit, &rval));
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf, dividend);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval));

    
    rval = 0;
    SOCDNX_IF_ERR_EXIT(READ_CMIC_CONFIGr(unit, &rval));
    mdio_delay = 0x7; 
    soc_reg_field_set(unit, CMIC_CONFIGr, &rval, MDIO_OUT_DELAYf, mdio_delay);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CONFIGr(unit, rval));

     
    rval = 0x1D0003;
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_3r(unit, rval));
    
    sal_usleep(20000);
    rval = 0x41D0003;
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_CORE_PLL3_CTRL_STATUS_REGISTER_3r(unit, rval));
    
    
   SOCDNX_IF_ERR_EXIT(soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_LINK_STAT_MSG_CLR));
   SOCDNX_IF_ERR_EXIT(soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_LINK_SCAN_EN_CLR));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_fe1600_set_operation_mode(int unit) {

    uint32 reg_val32;
    uint64 reg_val64, val64;
    int i;
    uint32 repeater_mode_get;
    uint32 multistage_config;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_1r(unit, &reg_val32));
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, REP_MODEf, SOC_DFE_IS_REPEATER(unit) ? 1 : 0);

    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, FE_13_MODEf, SOC_DFE_IS_FE13(unit)  ? 1 : 0);
    multistage_config = (!SOC_DFE_IS_MULTISTAGE(unit)) ? 0 :                                                                                        
                        (SOC_DFE_IS_FE2(unit) && SOC_IS_FE1600_B0_AND_ABOVE(unit) && SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system) ? 0 : 1;      
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, MULTI_STAGEf, multistage_config);
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, VSC_128_MODEf, SOC_DFE_CONFIG(unit).system_is_fe600_in_system  ? 1 : 0);
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, PETRA_SYSTEMf, SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system  ? 1 : 0);
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, DUAL_PIPE_ENf, SOC_DFE_CONFIG(unit).is_dual_mode  ? 1 : 0);
    if(SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system) {
        soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, FIELD_9_10f, 1);
    } else if ((SOC_DFE_CONFIG(unit).system_is_dual_mode_in_system)) {
        soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, FIELD_9_10f, 2);
    } else {
        soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, FIELD_9_10f, 0);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, M_24L_MODEf, SOC_DFE_CONFIG(unit).fabric_optimize_patial_links  ? 1 : 0);
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, PETRA_ENHANCE_ENf, (SOC_DFE_CONFIG(unit).fabric_merge_cells)  ? 1 : 0);
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg_val32, PETRA_TDM_FRAG_NUMf, SOC_DFE_CONFIG(unit).fabric_TDM_fragment);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_1r(unit, reg_val32));

    
    
    SOCDNX_IF_ERR_EXIT(READ_DCH_REG_005Ar(unit, REG_PORT_ANY, &reg_val32));
    repeater_mode_get = soc_reg_field_get(unit, DCH_REG_005Ar, reg_val32, FIELD_3_3f) ? 1 : 0;
    if (SOC_DFE_IS_REPEATER(unit) != repeater_mode_get  && !SAL_BOOT_PLISIM) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("FABRIC_DEVICE_MODE!=REPEATER -  device for repeater only use")));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    COMPILER_64_SET(val64, 0,(soc_dfe_load_balancing_mode_normal == SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) ? 1 : 0);
    soc_reg64_field_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, DISABLE_RCG_LOAD_BALANCINGf, 
                            val64);                              
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MULTICAST_MODE_SELECTIONr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_INDIRECT_LIST_OF_FAPS_MODEf, 
                        soc_dfe_multicast_mode_indirect == SOC_DFE_CONFIG(unit).fabric_multicast_mode ? 1 : 0);


    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTICAST_MODE_SELECTIONr(unit, reg_val32));
    
    
    reg_val32 = 0xf;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 2, 0x5b, 1, &reg_val32));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 3, 0x5b, 1, &reg_val32));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 4, 0x5b, 1, &reg_val32));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 6, 0x5b, 1, &reg_val32));

    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) {    


        SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0061r(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_REG_0061r, &reg_val32, PETRA_PIPE_BMP_ENf, 1);
        soc_reg_field_set(unit, DCH_REG_0061r, &reg_val32, PETRA_UNI_PRIf, SOC_DFE_CONFIG(unit).vcs128_unicast_priority);
        soc_reg_field_set(unit, DCH_REG_0061r, &reg_val32, PETRA_PIPE_TDM_PRIf,  (SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe ? DFE_TDM_PRIORITY_OVER_PRIMARY_PIPE : DFE_TDM_PRIORITY_OVER_SECONDARY_PIPE));
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0061r(unit, i, reg_val32));
    }

    
    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dcl) ; i++) {   
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, &reg_val32, PETRA_PIPE_TDM_PRIf, (SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe ? DFE_TDM_PRIORITY_OVER_PRIMARY_PIPE : DFE_TDM_PRIORITY_OVER_SECONDARY_PIPE));
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, i, reg_val32));
    }


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe1600_set_fmac_config(int unit) {

    uint32 reg_val32;
    int i;
    int link, blk, inner_link;
    soc_dfe_fabric_link_device_mode_t link_mode;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_FILL_RATEf, SOC_DFE_CONFIG(unit).fabric_mac_bucket_fill_rate);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_UP_THf, 0x20);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_DN_THf, 0x10);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, SIG_DET_BKT_RST_ENAf, 0x1);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, ALIGN_LCK_BKT_RST_ENAf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_LEAKY_BUCKET_CONTROL_REGISTERr(unit, reg_val32));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_LEAKY_BUCKET_CONTROL_REGISTERr(unit, reg_val32));

        
    if (SOC_DFE_IS_FE13(unit))
    {
       PBMP_SFI_ITER(unit, link)
       {
           SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, link, 0, &link_mode)));
           SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

           SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, &reg_val32));
           soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
           if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
           {
               soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE1);
               soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE1);
           } else {
               soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE3);
               soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE3);
           }
           SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, reg_val32));
       }
    } else {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 0, 0, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        if (SOC_DFE_IS_REPEATER(unit)) {
            soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, _SOC_FE1600_DRV_COMMA_BURST_PERIOD_REPEATER);
            soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, _SOC_FE1600_DRV_COMMA_BURST_SIZE_REPEATER);
        } else {
            soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, _SOC_FE1600_DRV_COMMA_BURST_PERIOD_FE2);
            soc_reg_field_set(unit, FMAC_FMAL_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, _SOC_FE1600_DRV_COMMA_BURST_SIZE_FE2);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 0, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 1, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 2, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 3, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 0, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 1, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 2, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_COMMA_BURST_CONFIGURATIONr(unit, 3, reg_val32));
    }
    
    if(SOC_DFE_IS_FE13(unit)) {
        
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_REG_0065r(unit,0xf));
    }

    
    reg_val32 = 0;
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_RX_ENf, 0xf);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_TX_ENf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));
    SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    
    for(i=0; i<SOC_FE1600_NOF_LINKS_IN_MAC; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe1600_set_fsrd_config(int unit) {

    uint32 hv_disable, reg_val;
    int blk_ins, quad, global_quad;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

#ifdef BCM_88754_A0
    
    if (SOC_IS_BCM88754_A0(unit))
    {
        SOC_EXIT;
    }
#endif
    for(blk_ins=0 ; blk_ins<SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd) ; blk_ins++) { 

        for(quad=0 ; quad<SOC_FE1600_NOF_QUADS_IN_FSRD ; quad++)
        {
            
            if(SOC_IS_FE1600_B0_AND_ABOVE(unit) && SOC_PBMP_MEMBER(SOC_CONTROL(unit)->info.sfi.disabled_bitmap, blk_ins*SOC_FE1600_NOF_LINKS_IN_FSRD + quad*SOC_FE1600_NOF_LINKS_IN_QUAD)) {
                continue;
            }
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, &reg_val));
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_POWER_DOWNf, 0);
            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_IDDQf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            sal_usleep(20);

            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_HWf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            sal_usleep(20);

            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_MDIO_REGSf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            sal_usleep(20);

            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_PLLf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            sal_usleep(20);

            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_RSTB_FIFOf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
            sal_usleep(20);
         
            global_quad = blk_ins*SOC_FE1600_NOF_QUADS_IN_FSRD + quad;
            hv_disable = soc_property_suffix_num_get(unit, global_quad, spn_SRD_TX_DRV_HV_DISABLE, "quad", 0);
            LOG_DEBUG(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "%s[FSRD%d, quad %d]=%d \n"),spn_SRD_TX_DRV_HV_DISABLE,blk_ins,quad,hv_disable));

            soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_TX_DRV_HV_DISABLEf, hv_disable);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, blk_ins, quad, reg_val));
     
        }
    }


exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe1600_set_ccs_config(int unit) {
    
    int blk_ins;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    for(blk_ins=0 ; blk_ins<SOC_FE1600_NOF_INSTANCES_CCS ; blk_ins++) {
        SOCDNX_IF_ERR_EXIT(READ_CCS_REG_0124r(unit, blk_ins, &reg_val));       
        soc_reg_field_set(unit, CCS_REG_0124r, &reg_val, FIELD_0_6f, 0x34);
        soc_reg_field_set(unit, CCS_REG_0124r, &reg_val, FIELD_8_14f, 0x31);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0124r(unit, blk_ins, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_set_mesh_topology_config(int unit) {

    uint32 reg_val32, intg, frac; 
    uint64 frac64;
    soc_reg_t mesh_topology_reg_0107;
#ifdef BCM_88950_SUPPORT
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
    uint32 field_zeros =0;
    int gt_size = -1;
#endif
    SOCDNX_INIT_FUNC_DEFS;
    
#ifdef BCM_88950_SUPPORT
    if (SOC_IS_FE3200(unit))
    {
        mesh_topology_reg_0107 = MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r;
    } else
#endif
    {
        mesh_topology_reg_0107  = MESH_TOPOLOGY_REG_0107r;
    }

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, FIELD_27_27f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_2r, &reg_val32, FIELD_4_17f, 2049);    
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_INITr(unit, &reg_val32));
    if(SOC_DFE_IS_FE13(unit)) {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, INITf, 10);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_1f, 2);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 0x0); 
    } else {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 1);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_INITr(unit, reg_val32));

    intg =  SOC_DFE_CONFIG(unit).system_ref_core_clock / SOC_DFE_CONFIG(unit).core_clock_speed;
    COMPILER_64_SET(frac64, 0, SOC_DFE_CONFIG(unit).system_ref_core_clock);
    COMPILER_64_SUB_32(frac64, SOC_DFE_CONFIG(unit).core_clock_speed*intg);
    COMPILER_64_SHL(frac64, 19);
    SOCDNX_IF_ERR_EXIT(soc_dfe_compiler_64_div_32(frac64, SOC_DFE_CONFIG(unit).core_clock_speed, &frac));
    

    soc_reg32_get(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, &reg_val32);
#ifdef BCM_88950_SUPPORT
    if (SOC_IS_FE3200(unit))
    {
        soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_1f, frac);
        soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_2f, intg);
    }
    else 
#endif 
    {
        soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, FIELD_0_18f, frac);
        soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, FIELD_20_23f, intg);
    }
    soc_reg32_set(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, reg_val32);

    


#ifdef BCM_88950_SUPPORT
    if (!SOC_IS_FE1600(unit))
    {
        
        
        gt_size = soc_property_suffix_num_get(unit,0,spn_CUSTOM_FEATURE, "mesh_topology_size", -1);

        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
        if (gt_size == -1) {
            if (SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system) {
                soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 1);
            } else if (SOC_DFE_CONFIG(unit).system_contains_multiple_pipe_device) {
                soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 2);
            } else {
                soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 0);
            }
        } else {
            soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, gt_size);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

        SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_011Br(unit, &reg_val32));
        soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_0f, 5); 
         
        
        soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_1f, 0xc);
        

        soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_2f, SOC_DFE_CONFIG(unit).mesh_topology_fast ? 1 : 0);


        SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_011Br(unit, reg_val32));



        if (SOC_DFE_IS_FE13_ASYMMETRIC(unit))
        {
            SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
            SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
            SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0110r(unit, reg_val_above_64));
            soc_reg_above_64_field_get(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0110r, reg_val_above_64, REG_110_CONFIG_0f, field_val_above_64);
            
            SOC_REG_ABOVE_64_RANGE_COPY(field_val_above_64, 32, &field_zeros, 0, 4);
            SOC_REG_ABOVE_64_RANGE_COPY(field_val_above_64, 68, &field_zeros, 0, 4);
            soc_reg_above_64_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0110r, reg_val_above_64, REG_110_CONFIG_0f, field_val_above_64);
            SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0110r(unit, reg_val_above_64));
        }


    }
#endif 

    sal_usleep(20);

    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
soc_fe1600_set_rtp_config(int unit) {

    uint32 reg_val32; 
    uint64 reg_val64, val64;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 core_clock_speed;
    uint32 rtpwp;
    uint32 wp_at_core_clock_steps;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed;

     
    wp_at_core_clock_steps = ((SOC_FE1600_RTP_REACHABILTY_WATCHDOG_RATE / 1000) * core_clock_speed ) / 1000 ;
    rtpwp = wp_at_core_clock_steps/4096;
    rtpwp = (rtpwp * 4096 < wp_at_core_clock_steps) ? rtpwp+1 : rtpwp; 

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr_REG32(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr, &reg_val32, RTPWPf, rtpwp);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr_REG32(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    if(soc_dfe_load_balancing_mode_destination_unreachable == SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
        COMPILER_64_SET(val64,0,0);
        soc_reg64_field_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, val64);
    } else {
        COMPILER_64_SET(val64,0,1);
        soc_reg64_field_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, val64);
    }
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, UPDATE_BASE_INDEXf, 0x3f); 
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_DRH_LOAD_BALANCING_CONFIGr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_DRH_LOAD_BALANCING_CONFIGr, &reg_val32, LOAD_BALANCE_LEVELS_IGNOREf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_DRH_LOAD_BALANCING_CONFIGr(unit, reg_val32));

    if (SOC_DFE_IS_FE13(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_RTP_REG_0069r(unit, &reg_val32)); 
        soc_reg_field_set(unit, RTP_REG_0069r, &reg_val32, FIELD_10_10f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0069r(unit, reg_val32)); 
    }



    SOC_REG_ABOVE_64_ALLONES(reg_val_above_64);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_LINK_BUNDLE_BITMAPr(unit, 0, reg_val_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}


#define SOC_FE1600_DCH_LLFC_TH_SINGLE_PIPE_DEF                  (100)
#define SOC_FE1600_DCH_LLFC_TH_DUAL_PIPE_DEF                    (40)
#define SOC_FE1600_DCH_IFM_ALL_FULL_TH_SINGLE_PIPE_DEF          (120)
#define SOC_FE1600_DCH_IFM_ALL_FULL_TH_DUAL_PIPE_DEF            (60)
#define SOC_FE1600_DCH_LOW_PRI_CELL_DROP_TH_SINGLE_PIPE_DEF     (64)
#define SOC_FE1600_DCH_LOW_PRI_CELL_DROP_TH_DUAL_PIPE_DEF       (32)
#define SOC_FE1600_DCH_FIELD_31_31_DEF                          (1)
#define SOC_FE1600_DCH_REG_0091_FIELD_4_24_FE3_VSC256_DEF       (0x1200)
#define SOC_FE1600_DCH_REG_0091_FIELD_4_24_FE1_FE2_VSC256_DEF   (0x400)

#define SOC_FE1600_DCH_GCI_TH_DEF                               (129)

STATIC int
soc_fe1600_set_dch_config(int unit) {
    uint32 reg32_val;
    int port;

    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    

    for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) { 
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Pr, &reg32_val, LNK_LVL_FC_TH_0_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCH_LLFC_TH_DUAL_PIPE_DEF : SOC_FE1600_DCH_LLFC_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Pr, &reg32_val, LNK_LVL_FC_TH_1_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCH_LLFC_TH_DUAL_PIPE_DEF : SOC_FE1600_DCH_LLFC_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Pr, &reg32_val, IFM_ALL_FULL_TH_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCH_IFM_ALL_FULL_TH_DUAL_PIPE_DEF : SOC_FE1600_DCH_IFM_ALL_FULL_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Pr, &reg32_val, LOW_PRI_CELL_DROP_TH_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCH_LOW_PRI_CELL_DROP_TH_DUAL_PIPE_DEF: SOC_FE1600_DCH_LOW_PRI_CELL_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Pr, &reg32_val, FIELD_31_31f, SOC_FE1600_DCH_FIELD_31_31_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_Pr(unit, port, reg32_val));
    }

    for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Sr, &reg32_val, LNK_LVL_FC_TH_0_Sf, SOC_FE1600_DCH_LLFC_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Sr, &reg32_val, LNK_LVL_FC_TH_1_Sf, SOC_FE1600_DCH_LLFC_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Sr, &reg32_val, IFM_ALL_FULL_TH_Sf, SOC_FE1600_DCH_IFM_ALL_FULL_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Sr, &reg32_val, LOW_PRI_CELL_DROP_TH_Sf, SOC_FE1600_DCH_LOW_PRI_CELL_DROP_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_Sr, &reg32_val, FIELD_31_31f, SOC_FE1600_DCH_FIELD_31_31_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_Sr(unit, port, reg32_val));
    }

    
    for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0061r(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_REG_0061r, &reg32_val, FIELD_0_7f, 0Xfd);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0061r(unit, port, reg32_val));
    }

    
    for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) { 
        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, &reg32_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, &reg32_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, &reg32_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, &reg32_val, DCH_LOCAL_GCI_EN_Pf, 0);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr, &reg32_val, DCH_LOCAL_MCI_EN_Pf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, &reg32_val, DCH_LOCAL_GCI_0_TYPE_0_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, &reg32_val, DCH_LOCAL_GCI_1_TYPE_0_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, &reg32_val, DCH_LOCAL_GCI_2_TYPE_0_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, &reg32_val, DCH_LOCAL_GCI_EN_Sf, 0);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr, &reg32_val, DCH_LOCAL_MCI_EN_Sf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_0_TH_Sr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, &reg32_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, &reg32_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr, &reg32_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Pf, SOC_FE1600_DCH_GCI_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, &reg32_val, DCH_LOCAL_GCI_0_TYPE_1_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, &reg32_val, DCH_LOCAL_GCI_1_TYPE_1_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        soc_reg_field_set(unit, DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr, &reg32_val, DCH_LOCAL_GCI_2_TYPE_1_TH_Sf, SOC_FE1600_DCH_GCI_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_LOCAL_GCI_TYPE_1_TH_Sr(unit, port, reg32_val));
    }

    
    if (SOC_DFE_IS_REPEATER(unit)) {
        for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) {    
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0091r(unit, port, &reg32_val));
            soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_0_0f, 0X0); 
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0091r(unit, port, reg32_val));
        }

    } else if (SOC_DFE_IS_FE2(unit)) {
        for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) {
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0091r(unit, port, &reg32_val));
            soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_0_0f, 0X1); 
            soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_4_22f, SOC_FE1600_DCH_REG_0091_FIELD_4_24_FE1_FE2_VSC256_DEF);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0091r(unit, port, reg32_val));
        }
    } else if (SOC_DFE_IS_FE13(unit)) {
        
        for (port=0 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch)/2; port++) { 
            SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0091r(unit, port, &reg32_val));
            soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_0_0f, 0X1); 
            soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_4_22f, SOC_FE1600_DCH_REG_0091_FIELD_4_24_FE1_FE2_VSC256_DEF);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0091r(unit, port, reg32_val));
        }
        
        if (SOC_DFE_CONFIG(unit).system_is_vcs_128_in_system)
        {
            for (port=SOC_DFE_DEFS_GET(unit, nof_instances_dch)/2 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) { 
                SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0091r(unit, port, &reg32_val));
                soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_0_0f, 0X0); 
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0091r(unit, port, reg32_val));
            }
        } else {
            for (port=SOC_DFE_DEFS_GET(unit, nof_instances_dch)/2 ; port < SOC_DFE_DEFS_GET(unit, nof_instances_dch); port++) { 
                SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0091r(unit, port, &reg32_val));
                soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_0_0f, 0X1); 
                soc_reg_field_set(unit, DCH_REG_0091r, &reg32_val, FIELD_4_22f, SOC_FE1600_DCH_REG_0091_FIELD_4_24_FE3_VSC256_DEF);
                SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0091r(unit, port, reg32_val));
            }
        }
    }

       
exit:
    SOCDNX_FUNC_RETURN;
}


#define SOC_FE1600_DCM_0_DROP_TH_SINGLE_PIPE_DEF                    (280)
#define SOC_FE1600_DCM_1_DROP_TH_SINGLE_PIPE_DEF                    (290)
#define SOC_FE1600_DCM_2_DROP_TH_SINGLE_PIPE_DEF                    (300)
#define SOC_FE1600_DCM_3_DROP_TH_SINGLE_PIPE_DEF                    (360)

#define SOC_FE1600_DCM_0_DROP_TH_DUAL_PIPE_DEF                      (100)
#define SOC_FE1600_DCM_1_DROP_TH_DUAL_PIPE_DEF                      (110)
#define SOC_FE1600_DCM_2_DROP_TH_DUAL_PIPE_DEF                      (120)
#define SOC_FE1600_DCM_3_DROP_TH_DUAL_PIPE_DEF                      (180)

#define SOC_FE1600_DCM_ALM_FULL_DUAL_PIPE_DEF                       (120)
#define SOC_FE1600_DCM_ALM_FULL_SINGLE_PIPE_DEF                     (320)
#define SOC_FE1600_DCM_FULL_TH_DUAL_PIPE_DEF                        (180)
#define SOC_FE1600_DCM_FULL_TH_SINGLE_PIPE_DEF                      (360)

#define SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF                         (128)
#define SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF                       (256)


STATIC int
soc_fe1600_set_dcm_config(int unit) {
    uint64 reg64_val;
    uint32 reg32_val;
    int port;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    
    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr(unit, port, &reg64_val));
        soc_reg64_field32_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUA_P_0_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_0_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_0_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUA_P_1_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_1_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_1_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUA_P_2_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_2_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_2_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUA_P_3_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_3_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_3_DROP_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMUA_PRIORITY_DROP_THRESHOLDr(unit, port, reg64_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr(unit, port, &reg64_val));
        soc_reg64_field32_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMA_P_0_DROP_THf, SOC_FE1600_DCM_0_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMA_P_1_DROP_THf, SOC_FE1600_DCM_1_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMA_P_2_DROP_THf, SOC_FE1600_DCM_2_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMA_P_3_DROP_THf, SOC_FE1600_DCM_3_DROP_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMMA_PRIORITY_DROP_THRESHOLDr(unit, port, reg64_val));
    }

    
    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMU_ALM_FULL_0r(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMA_DCMU_ALM_FULL_0r, &reg32_val, DCMUA_ALM_FULLf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_ALM_FULL_DUAL_PIPE_DEF : SOC_FE1600_DCM_ALM_FULL_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMU_ALM_FULL_0r, &reg32_val, DCMU_FULL_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_FULL_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_FULL_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMU_ALM_FULL_0r(unit, port, reg32_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMM_ALM_FULL_0r(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMA_DCMM_ALM_FULL_0r, &reg32_val, DCMMA_ALM_FULLf, SOC_FE1600_DCM_ALM_FULL_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMM_ALM_FULL_0r, &reg32_val, DCMM_FULL_THf, SOC_FE1600_DCM_FULL_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMM_ALM_FULL_0r(unit, port, reg32_val));
    }

    

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMUA_GCI_THr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMA_DCMUA_GCI_THr, &reg32_val, GCI_TH_LOWf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMUA_GCI_THr, &reg32_val, GCI_TH_MEDf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMUA_GCI_THr, &reg32_val, GCI_TH_HIGHf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMUA_GCI_THr(unit, port, reg32_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMA; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMA_DCMMA_GCI_THr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMA_DCMMA_GCI_THr, &reg32_val, GCI_TH_LOWf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMMA_GCI_THr, &reg32_val, GCI_TH_MEDf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMA_DCMMA_GCI_THr, &reg32_val, GCI_TH_HIGHf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMA_DCMMA_GCI_THr(unit, port, reg32_val));
    }


    
    
    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr(unit, port, &reg64_val));
        soc_reg64_field32_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUB_P_0_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_0_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_0_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUB_P_1_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_1_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_1_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUB_P_2_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_2_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_2_DROP_TH_SINGLE_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMUB_P_3_DROP_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_3_DROP_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_3_DROP_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMUB_PRIORITY_DROP_THRESHOLDr(unit, port, reg64_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr(unit, port, &reg64_val));
        soc_reg64_field32_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMB_P_0_DROP_THf, SOC_FE1600_DCM_0_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMB_P_1_DROP_THf, SOC_FE1600_DCM_1_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMB_P_2_DROP_THf, SOC_FE1600_DCM_2_DROP_TH_DUAL_PIPE_DEF);
        soc_reg64_field32_set(unit, DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr, &reg64_val, DCMMB_P_3_DROP_THf, SOC_FE1600_DCM_3_DROP_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMMB_PRIORITY_DROP_THRESHOLDr(unit, port, reg64_val));
    }

    
    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUBLM_FULL_0r(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMB_DCMUBLM_FULL_0r, &reg32_val, DCMUB_ALM_FULLf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_ALM_FULL_DUAL_PIPE_DEF : SOC_FE1600_DCM_ALM_FULL_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMUBLM_FULL_0r, &reg32_val, DCMU_FULL_THf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_FULL_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_FULL_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMUBLM_FULL_0r(unit, port, reg32_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMBLM_FULL_0r(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMB_DCMMBLM_FULL_0r, &reg32_val, DCMMB_ALM_FULLf, SOC_FE1600_DCM_ALM_FULL_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMMBLM_FULL_0r, &reg32_val, DCMM_FULL_THf, SOC_FE1600_DCM_FULL_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMMBLM_FULL_0r(unit, port, reg32_val));
    }

    

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMUB_GCI_THr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMB_DCMUB_GCI_THr, &reg32_val, GCI_TH_LOWf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMUB_GCI_THr, &reg32_val, GCI_TH_MEDf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMUB_GCI_THr, &reg32_val, GCI_TH_HIGHf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF : SOC_FE1600_DCM_GCI_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMUB_GCI_THr(unit, port, reg32_val));
    }

    for (port=0 ; port < SOC_FE1600_NOF_INSTANCES_DCMB; port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCMB_DCMMB_GCI_THr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCMB_DCMMB_GCI_THr, &reg32_val, GCI_TH_LOWf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMMB_GCI_THr, &reg32_val, GCI_TH_MEDf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCMB_DCMMB_GCI_THr, &reg32_val, GCI_TH_HIGHf, SOC_FE1600_DCM_GCI_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCMB_DCMMB_GCI_THr(unit, port, reg32_val));
    }

     

exit:
    SOCDNX_FUNC_RETURN;
}



#define SOC_FE1600_DCL_U_LLFC_TH_DEF                                    (322)
#define SOC_FE1600_DCL_M_LLFC_TH_DEF                                    (194)

#define SOC_FE1600_DCL_P_ALM_FULL_TH_DEF                                (322)
#define SOC_FE1600_DCL_S_ALM_FULL_TH_DEF                                (194)

#define SOC_FE1600_DCL_GCI_LOW_TH_P_SINGLE_PIPE_TH_DEF                  (180)
#define SOC_FE1600_DCL_GCI_MED_TH_P_SINGLE_PIPE_TH_DEF                  (220)
#define SOC_FE1600_DCL_GCI_HIGH_TH_P_SINGLE_PIPE_TH_DEF                 (260)
#define SOC_FE1600_DCL_GCI_LOW_TH_P_DUAL_PIPE_TH_DEF                    (80)
#define SOC_FE1600_DCL_GCI_MED_TH_P_DUAL_PIPE_TH_DEF                    (100)
#define SOC_FE1600_DCL_GCI_HIGH_TH_P_DUAL_PIPE_TH_DEF                   (120)
#define SOC_FE1600_DCL_GCI_LOW_TH_S_TH_DEF                              (100)
#define SOC_FE1600_DCL_GCI_MED_TH_S_TH_DEF                              (120)
#define SOC_FE1600_DCL_GCI_HIGH_TH_S_TH_DEF                             (140)

#define SOC_FE1600_DCL_RCI_HIGH_TH_SINGLE_PIPE_DEF                      (200)
#define SOC_FE1600_DCL_RCI_HIGH_TH_DUAL_PIPE_DEF                        (110)

#define SOC_FE1600_DCL_DROP_PRIO_TH_SINGLE_PIPE_DEF                     (320)
#define SOC_FE1600_DCL_DROP_PRIO_TH_P_DUAL_PIPE_DEF                     (128)
#define SOC_FE1600_DCL_DROP_PRIO_TH_S_DUAL_PIPE_DEF                     (192)

STATIC int
soc_fe1600_set_dcl_config(int unit) {
    uint32 reg32_val;
    uint64 reg64_val, val64;
    int port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_FE1600_ONLY(unit);

    
    for (port=0; port < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_LLFC_THr(unit, port, &reg64_val));
        COMPILER_64_SET(val64,0, SOC_FE1600_DCL_U_LLFC_TH_DEF);
        soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg64_val, DCLU_LLFC_TH_TYPE_0f, val64);
        soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg64_val, DCLU_LLFC_TH_TYPE_1f, val64);
        COMPILER_64_SET(val64,0, SOC_FE1600_DCL_M_LLFC_TH_DEF);
        soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg64_val, DCLM_LLFC_TH_TYPE_0f, val64);
        soc_reg64_field_set(unit, DCL_DCL_LLFC_THr, &reg64_val, DCLM_LLFC_TH_TYPE_1f, val64);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_LLFC_THr(unit, port, reg64_val));
    }

    

    for (port=0; port < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_ALM_FULL_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_0_ALM_FULL_Pr, &reg32_val, TYPE_0_ALM_FULL_Pf, SOC_FE1600_DCL_P_ALM_FULL_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_ALM_FULL_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_ALM_FULL_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_1_ALM_FULL_Pr, &reg32_val, TYPE_1_ALM_FULL_Pf, SOC_FE1600_DCL_P_ALM_FULL_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_ALM_FULL_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_ALM_FULL_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_0_ALM_FULL_Sr, &reg32_val, TYPE_0_ALM_FULL_Sf, SOC_FE1600_DCL_S_ALM_FULL_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_ALM_FULL_Sr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_ALM_FULL_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_1_ALM_FULL_Sr, &reg32_val, TYPE_1_ALM_FULL_Sf, SOC_FE1600_DCL_S_ALM_FULL_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_ALM_FULL_Sr(unit, port, reg32_val));
    }

    

    for (port=0; port < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); port++) { 
        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg32_val, GCI_TH_LOW_0_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_LOW_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_LOW_TH_P_SINGLE_PIPE_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg32_val, GCI_TH_MED_0_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_MED_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_MED_TH_P_SINGLE_PIPE_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Pr, &reg32_val, GCI_TH_HIGH_0_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_HIGH_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_HIGH_TH_P_SINGLE_PIPE_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_GCI_TH_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg32_val, GCI_TH_LOW_1_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_LOW_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_LOW_TH_P_SINGLE_PIPE_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg32_val, GCI_TH_MED_1_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_MED_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_MED_TH_P_SINGLE_PIPE_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Pr, &reg32_val, GCI_TH_HIGH_1_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_GCI_HIGH_TH_P_DUAL_PIPE_TH_DEF : SOC_FE1600_DCL_GCI_HIGH_TH_P_SINGLE_PIPE_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_GCI_TH_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_GCI_TH_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg32_val, GCI_TH_LOW_0_Sf, SOC_FE1600_DCL_GCI_LOW_TH_S_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg32_val, GCI_TH_MED_0_Sf, SOC_FE1600_DCL_GCI_MED_TH_S_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_0_GCI_TH_Sr, &reg32_val, GCI_TH_HIGH_0_Sf,SOC_FE1600_DCL_GCI_HIGH_TH_S_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_GCI_TH_Sr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_GCI_TH_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg32_val, GCI_TH_LOW_1_Sf, SOC_FE1600_DCL_GCI_LOW_TH_S_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg32_val, GCI_TH_MED_1_Sf, SOC_FE1600_DCL_GCI_MED_TH_S_TH_DEF);
        soc_reg_field_set(unit, DCL_TYPE_1_GCI_TH_Sr, &reg32_val, GCI_TH_HIGH_1_Sf,SOC_FE1600_DCL_GCI_HIGH_TH_S_TH_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_GCI_TH_Sr(unit, port, reg32_val));
    }

    
    for (port=0; port < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); port++) {
        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Pr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Pr, &reg32_val, RCI_TH_HIGH_0_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_RCI_HIGH_TH_DUAL_PIPE_DEF : SOC_FE1600_DCL_RCI_HIGH_TH_SINGLE_PIPE_DEF);
        soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Pr, &reg32_val, RCI_TH_HIGH_1_Pf, SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_RCI_HIGH_TH_DUAL_PIPE_DEF : SOC_FE1600_DCL_RCI_HIGH_TH_SINGLE_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_01_RCI_TH_Pr(unit, port, reg32_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_01_RCI_TH_Sr(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Sr, &reg32_val, RCI_TH_HIGH_0_Sf, SOC_FE1600_DCL_RCI_HIGH_TH_DUAL_PIPE_DEF);
        soc_reg_field_set(unit, DCL_TYPE_01_RCI_TH_Sr, &reg32_val, RCI_TH_HIGH_1_Sf, SOC_FE1600_DCL_RCI_HIGH_TH_DUAL_PIPE_DEF);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_01_RCI_TH_Sr(unit, port, reg32_val));
    }

    
    for (port=0; port < SOC_DFE_DEFS_GET(unit, nof_instances_dcl); port++) {
        COMPILER_64_SET(val64,0,SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_DROP_PRIO_TH_P_DUAL_PIPE_DEF : SOC_FE1600_DCL_DROP_PRIO_TH_SINGLE_PIPE_DEF);

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PPr(unit, port, &reg64_val));
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg64_val, TYPE_0_DRP_P_0_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg64_val, TYPE_0_DRP_P_1_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg64_val, TYPE_0_DRP_P_2_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PPr, &reg64_val, TYPE_0_DRP_P_3_Pf, val64);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_DRP_PPr(unit, port, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PPr(unit, port, &reg64_val));
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg64_val, TYPE_1_DRP_P_0_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg64_val, TYPE_1_DRP_P_1_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg64_val, TYPE_1_DRP_P_2_Pf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PPr, &reg64_val, TYPE_1_DRP_P_3_Pf, val64);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_DRP_PPr(unit, port, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_0_DRP_PSr(unit, port, &reg64_val));
        COMPILER_64_SET(val64,0,SOC_DFE_CONFIG(unit).is_dual_mode ? SOC_FE1600_DCL_DROP_PRIO_TH_S_DUAL_PIPE_DEF : SOC_FE1600_DCL_DROP_PRIO_TH_SINGLE_PIPE_DEF);

        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg64_val, TYPE_0_DRP_P_0_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg64_val, TYPE_0_DRP_P_1_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg64_val, TYPE_0_DRP_P_2_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_0_DRP_PSr, &reg64_val, TYPE_0_DRP_P_3_Sf, val64);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_0_DRP_PSr(unit, port, reg64_val));

        SOCDNX_IF_ERR_EXIT(READ_DCL_TYPE_1_DRP_PSr(unit, port, &reg64_val));

        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg64_val, TYPE_1_DRP_P_0_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg64_val, TYPE_1_DRP_P_1_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg64_val, TYPE_1_DRP_P_2_Sf, val64);
        soc_reg64_field_set(unit, DCL_TYPE_1_DRP_PSr, &reg64_val, TYPE_1_DRP_P_3_Sf, val64);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_TYPE_1_DRP_PSr(unit, port, reg64_val));


        SOCDNX_IF_ERR_EXIT(READ_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, port, &reg32_val));
        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, &reg32_val, LOW_PR_DROP_EN_Pf, 0xf);
        soc_reg_field_set(unit, DCL_DCL_ENABLERS_REGISTERr, &reg32_val, LOW_PR_DROP_EN_Sf, 0xf);
        SOCDNX_IF_ERR_EXIT(WRITE_DCL_DCL_ENABLERS_REGISTERr_REG32(unit, port, reg32_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}



int soc_fe1600_tbl_is_dynamic(int unit, soc_mem_t mem) {

    switch (mem) {        
    case RTP_DLLUPm:
    case RTP_DLLUSm:
    case RTP_MEM_1100000m:
    case RTP_MEM_800000m:
    case RTP_MEM_900000m:
        return TRUE;
    case RTP_DUCTPm:
    case RTP_DUCTSm:
    case RTP_CUCTm:
    case RTP_RMHMTm:
        if(!SOC_DFE_IS_REPEATER(unit)) {
            return TRUE;
        } else {
            return FALSE;
        }
    case RTP_SLSCTm:
        if (soc_dfe_load_balancing_mode_destination_unreachable != SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
            return TRUE;
        } else {
            return FALSE;
        }

    default:
        return FALSE;
    }
}


STATIC int
soc_fe1600_interrupt_disable(int unit)
{
    uint32 inst_idx;
    soc_block_types_t  block;
    int blk;
    int instance;
    soc_reg_above_64_val_t above_64;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_REG_ABOVE_64_CLEAR(above_64);
    for(inst_idx=0; fe1600_interrupts_mask_registers[inst_idx] != INVALIDr; ++inst_idx) {
        if (SOC_REG_PTR(unit, fe1600_interrupts_mask_registers[inst_idx])== NULL)
        {
            continue;
        }
        block = SOC_REG_INFO(unit, fe1600_interrupts_mask_registers[inst_idx]).block;
        SOC_BLOCKS_ITER(unit, blk, block) {
            instance = SOC_BLOCK_NUMBER(unit, blk);
            rc = soc_reg_above_64_set(unit, fe1600_interrupts_mask_registers[inst_idx], instance, 0, above_64);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }

    
    rc = WRITE_ECI_INTERRUPT_MASK_REGISTERr(unit, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = WRITE_ECI_MAC_INTERRUPT_MASK_REGISTERr(unit, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe1600_ser_init(int unit)
{
    uint32 inst_idx;
    soc_block_types_t  block;
    int blk;
    int instance;
    soc_reg_above_64_val_t above_64;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_REG_ABOVE_64_ALLONES(above_64);
    for(inst_idx=0; fe1600_interrupt_monitor_mem_reg[inst_idx] != INVALIDr; inst_idx++) {
        block = SOC_REG_INFO(unit, fe1600_interrupt_monitor_mem_reg[inst_idx]).block;
        SOC_BLOCKS_ITER(unit, blk, block) {
            instance = SOC_BLOCK_NUMBER(unit, blk);
            rc = soc_reg_above_64_set(unit, fe1600_interrupt_monitor_mem_reg[inst_idx], instance, 0, above_64);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_reset_device(int unit)
{
    uint64 reg_val64;
    uint32 reg_val32, i;
    soc_reg_above_64_val_t reg_val_above_64;
    int rc;
    uint32 ser_test_iters = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "set_test_iters_num", 0);
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);
    
    rc =soc_fe1600_reset_cmic_regs(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    COMPILER_64_SET(reg_val64, 0x7, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit, reg_val64));
    sal_sleep(1);
    COMPILER_64_SET(reg_val64, 0x0, 0x0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit,reg_val64));
    reg_val32 = 0x1ff;
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit,reg_val32));
    sal_usleep(20);

    rc = soc_fe1600_set_operation_mode(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    if (ser_test_iters) { 
        uint32 time_to_wait = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "ser_test_delay_sec", 0);
        if (time_to_wait == 0) { 
            time_to_wait = soc_property_suffix_num_get_only_suffix(unit, -1, spn_BIST_ENABLE, "ser_test_delay_us", 0);
            if (time_to_wait == 0) {
                time_to_wait = 3600 | DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC; 
            }
        } else {
            time_to_wait |= DCMN_MBIST_TEST_LONG_WAIT_DELAY_IS_SEC;
        }
        SOCDNX_IF_ERR_EXIT(soc_bist_fe1600_ser_test(unit, 1, ser_test_iters, time_to_wait));
    }

    
    if (SOC_DFE_CONFIG(unit).run_mbist) {
        SOCDNX_IF_ERR_EXIT(soc_fe1600_drv_mbist(unit, SOC_DFE_CONFIG(unit).run_mbist - 1));

        
        COMPILER_64_SET(reg_val64, 0x7, 0xFFFFFFFF);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit, reg_val64));
        sal_sleep(1);
        COMPILER_64_SET(reg_val64, 0x0, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_RESETr(unit,reg_val64));
        reg_val32 = 0x1ff;
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit,reg_val32));
        sal_usleep(20);

        rc = soc_fe1600_set_operation_mode(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    
    
#ifdef BCM_88754_A0
    if (!SOC_IS_BCM88754_A0(unit))
    {
#endif
        for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd) ; i++) { 
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, i, 60));
        }
#ifdef BCM_88754_A0
    }
#endif

    rc = soc_fe1600_reset_tables(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_fmac_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_fsrd_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_ccs_config(unit);
    SOCDNX_IF_ERR_EXIT(rc); 
    rc = soc_fe1600_set_mesh_topology_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_rtp_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_dch_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_dcm_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_fe1600_set_dcl_config(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    
    for(i=0 ; i<SOC_DFE_DEFS_GET(unit, nof_instances_dch) ; i++) {

        SOCDNX_IF_ERR_EXIT(READ_DCH_REG_0060r(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_REG_0060r, &reg_val32, FIELD_21_24f, 0x4);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_REG_0060r(unit, i, reg_val32));

        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        reg_val_above_64[3] = 0x00180000;
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_ERROR_FILTERr(unit, i, reg_val_above_64));

        SOC_REG_ABOVE_64_ALLONES(reg_val_above_64);
        reg_val_above_64[3] = 0xffe7ffff;
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_ERROR_FILTER_MASKr(unit, i, reg_val_above_64));

        SOCDNX_IF_ERR_EXIT(WRITE_DCH_ERROR_FILTER_MASK_ENr(unit, i, 1));
    }

    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set,(unit, soc_dfe_fabric_priority_0));
    SOCDNX_IF_ERR_EXIT(rc);

    
#ifdef BCM_88750_B0
    if (SOC_IS_FE1600_B0_AND_ABOVE(unit)) {
        rc = soc_fe1600_bo_feature_fixes_enable(unit, 1);
        SOCDNX_IF_ERR_EXIT(rc);
    }
#endif

    
    if (SOC_IS_FE1600_REDUCED(unit)) {
        rc = soc_fe1600_reduced_support_set(unit);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    rc = soc_fe1600_interrupt_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_fe1600_interrupt_disable(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe1600_drv_soft_init(int unit, uint32 soft_reset_mode_flags)
{
    uint64 reg_val64;
    uint32 reg32_val;
    int blk, i;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    for (blk = 0; blk < SOC_DFE_DEFS_GET(unit, nof_instances_mac); blk++) {  
        for(i=0; i<SOC_FE1600_NOF_LINKS_IN_MAC; i++) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk, i, &reg32_val));
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val ,FIELD_8_8f,  1);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk, i, reg32_val));        
        }
    }

    
    COMPILER_64_SET(reg_val64, 0x7, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_INITr(unit, reg_val64));
    
    
    COMPILER_64_SET(reg_val64, 0x0, 0x0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_FE_1600_SOFT_INITr(unit,reg_val64));

    for (blk = 0; blk < SOC_DFE_DEFS_GET(unit, nof_instances_mac); blk++) { 
        for(i=0; i<SOC_FE1600_NOF_LINKS_IN_MAC; i++) {
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk, i, &reg32_val));
            soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val ,FIELD_8_8f,  0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, blk, i, reg32_val));        
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe1600_TDM_fragment_validate(int unit, uint32 tdm_frag) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if(SOC_FAILURE(soc_reg_field_validate(unit, ECI_GLOBAL_1r, PETRA_TDM_FRAG_NUMf, tdm_frag))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("TDM_FRAGMENT: %d is out-of-ranget"), tdm_frag));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_fe1600_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    SOCDNX_NULL_CHECK(nof_block_instances);
    SOCDNX_NULL_CHECK(block_types);

    switch(block_types[0]) {
        case SOC_BLK_FMAC:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_mac); 
            break;
        case SOC_BLK_FSRD:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd);
            break;
        case SOC_BLK_DCH:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
            break;
        case SOC_BLK_CCS:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_CCS;
            break;
        case SOC_BLK_DCL:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_dcl);
            break;
        case SOC_BLK_RTP:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_RTP;
            break;
        case SOC_BLK_OCCG:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_OCCG;
            break;
        case SOC_BLK_ECI:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_ECI;
            break;
        case SOC_BLK_DCMA:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_DCMA;
            break;
        case SOC_BLK_DCMB:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_DCMB;
            break;
        case SOC_BLK_DCMC:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_DCMC;
            break;
        case SOC_BLK_CMIC:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_CMIC;
            break;
        case SOC_BLK_MESH_TOPOLOGY:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_MESH_TOPOLOGY;
            break;
        case SOC_BLK_OTPC:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_OTPC;
            break;
        case SOC_BLK_BRDC_FMACH:
            *nof_block_instances = SOC_DFE_DEFS_GET(unit, nof_instances_brdc_fmach); 
            break;
        case SOC_BLK_BRDC_FMACL:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_BRDC_FMACL;
            break;
        case SOC_BLK_BRDC_FSRD:
            *nof_block_instances = SOC_FE1600_NOF_INSTANCES_BRDC_FSRD;
            break;

        default:
            *nof_block_instances = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_temperature_monitor_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    int i, rv;
    uint32 reg32_val;
    int curr;
    soc_reg_t temp_reg[] = {CMIC_THERMAL_MON_RESULT_0r, CMIC_THERMAL_MON_RESULT_1r, CMIC_THERMAL_MON_RESULT_2r, CMIC_THERMAL_MON_RESULT_3r};
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    
    if (temperature_max < _SOC_FE1600_PVT_MON_NOF)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Array size should be equal or bigger from %d.\n"), _SOC_FE1600_PVT_MON_NOF));
    }

    for (i = 0; i < _SOC_FE1600_PVT_MON_NOF; i++)
    {
        rv = soc_pci_getreg(unit, soc_reg_addr(unit, temp_reg[i], REG_PORT_ANY, 0), &reg32_val);
        SOCDNX_IF_ERR_EXIT(rv);

        curr = soc_reg_field_get(unit, temp_reg[i], reg32_val, TEMP_DATAf);
        
        temperature_array[i].curr =  (_SOC_FE1600_PVT_BASE - curr * _SOC_FE1600_PVT_FACTOR) / 1000;
        temperature_array[i].peak = -1;
    }

    *temperature_count = _SOC_FE1600_PVT_MON_NOF;
        
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int 
soc_fe1600_read_otp_mem_word(int unit, int addr, uint32* rdata)
{

    uint32 data[20];

    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    data[0] = addr;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));

    data[0] = 0x00a80001;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

    data[0] = 0x00e00000;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
   
    sal_usleep(20000);
       
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x00080210, 1, rdata)); 

    data[0] = 0x00a80000;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));  

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
soc_fe1600_otp_prog_en(int unit)
{

    uint32 data[20];

    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    data[0] = 0x00000001;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080200, 1, data));

    data[0] = 0x00000000;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));

    data[0] = 0x00280001;
    
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

    data[0] = 0x00e00000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x00080210, 1, data));

    data[0] = 0x00280000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));
  
    data[0] = 0x0000000f;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));

    data[0] = 0x00000000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x0008022c, 1, data));

    data[0] = 0x00280003;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

    data[0] = 0x00e00000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));

    data[0] = 0x00280002;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));
  
    data[0] = 0x00000004;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));

    data[0] = 0x00000000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x0008022c, 1, data));

    data[0] = 0x00280003;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

    data[0] = 0x00e00000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));

    data[0] = 0x00280002;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));
  
    data[0] = 0x00000008;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));

    data[0] = 0x00000000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x0008022c, 1, data));

    data[0] = 0x00280003;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

    data[0] = 0x00e00000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));

    data[0] = 0x00280002;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));
    
    data[0] = 0x0000000d;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080228, 1, data));
    data[0] = 0x00000000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x0008022c, 1, data));
    data[0] = 0x00280003;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));
    data[0] = 0x00e00000;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080204, 1, data));
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_get(unit, 0x3f, 0x0008020c, 1, data));
    data[0] = 0x00280002;
    SOCDNX_IF_ERR_EXIT(soc_direct_reg_set(unit, 0x3f, 0x00080208, 1, data));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe1600_avs_value_get(int unit, uint32* avs_val)
{
    uint32 rdata;
    int    raddr;

    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    SOCDNX_IF_ERR_EXIT(soc_fe1600_otp_prog_en(unit));  
    
    
    raddr = 10 * 0x20;  
    SOCDNX_IF_ERR_EXIT(soc_fe1600_read_otp_mem_word(unit, raddr, &rdata));

    *avs_val=0;
    SHR_BITCOPY_RANGE(avs_val, 0, &rdata, 10, 3);

exit:
    SOCDNX_FUNC_RETURN;

}

#ifdef BCM_88750_B0
int
soc_fe1600_bo_feature_fixes_enable(int unit, int enable)
{
    uint32 reg32_val;
    int i;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    for(i=0; i< SOC_DFE_DEFS_GET(unit, nof_instances_dch); i++) {
        SOCDNX_IF_ERR_EXIT(READ_DCH_FE_1600_B_0_ENABLERSr(unit, i, &reg32_val));

        
        soc_reg_field_set(unit, DCH_FE_1600_B_0_ENABLERSr, &reg32_val ,PETRA_A_SYS_FIX_ENABLEf, enable ? 1 : 0);

        
        if (SOC_DFE_IS_MULTISTAGE_FE2(unit) || (SOC_DFE_IS_FE13(unit) && i>=2)) {
            soc_reg_field_set(unit, DCH_FE_1600_B_0_ENABLERSr, &reg32_val ,PETRA_TDM_OVER_PRIMARY_ENf, enable && SOC_DFE_CONFIG(unit).fabric_TDM_over_primary_pipe ? 1 : 0);
        }

        SOCDNX_IF_ERR_EXIT(WRITE_DCH_FE_1600_B_0_ENABLERSr(unit, i, reg32_val));
    }

    
    for(i=0; i<SOC_FE1600_NOF_LINKS_IN_MAC; i++) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg32_val));
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, enable ? 1 : 0);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACL_FMAL_GENERAL_CONFIGURATIONr(unit, i, reg32_val));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMACH_FMAL_GENERAL_CONFIGURATIONr(unit, i, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 

int
soc_fe1600_reduced_support_set(int unit)
{
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_SB_RSTN_AND_POWER_DOWNr(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_DCQ_2f, 1);
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_DCQ_3f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit, reg32_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_1r(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_GLOBAL_1r, &reg32_val, DCQ_DOWNf, 0xC); 
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_1r(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_SB_RSTN_AND_POWER_DOWNr(unit, &reg32_val));
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_FMAC_4f, 1);
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_FMAC_5f, 1);
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_FMAC_6f, 1);
    soc_reg_field_set(unit, ECI_SB_RSTN_AND_POWER_DOWNr, &reg32_val, POWER_DOWN_FMAC_7f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit, reg32_val));

    
    SOCDNX_IF_ERR_EXIT(READ_CMIC_MISC_CONTROLr(unit, &reg32_val));
    soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &reg32_val, XGXS2_PLL_PWRDWNf, 1);
    soc_reg_field_set(unit, CMIC_MISC_CONTROLr, &reg32_val, XGXS3_PLL_PWRDWNf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_CMIC_MISC_CONTROLr(unit, reg32_val));

    
    SOC_DFE_DEFS_SET(unit, nof_links, SOC_FE1600_REDUCED_NOF_LINKS);
    SOC_DFE_DEFS_SET(unit, nof_instances_mac, SOC_FE1600_REDUCED_NOF_INSTANCES_MAC);
    SOC_DFE_DEFS_SET(unit, nof_instances_mac_fsrd, SOC_FE1600_REDUCED_NOF_INSTANCES_MAC_FSRD);
    SOC_DFE_DEFS_SET(unit, nof_instances_dch, SOC_FE1600_REDUCED_NOF_INSTANCES_DCH);
    SOC_DFE_DEFS_SET(unit, nof_instances_dcl, SOC_FE1600_REDUCED_NOF_INSTANCES_DCL);
    SOC_DFE_DEFS_SET(unit, nof_instances_brdc_fmach, SOC_FE1600_REDUCED_BLK_NOF_INSTANCES_BRDC_FMACH);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_soc_properties_validate(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    
    if (SOC_IS_FE1600_REDUCED(unit)) {
        
        if (SOC_DFE_CONFIG(unit).fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe2 ||
            SOC_DFE_CONFIG(unit).fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("FE1600_REDUCED do not support multistage ")));
        }
    }

    switch (SOC_DFE_CONFIG(unit).fabric_device_mode)
    {
        case soc_dfe_fabric_device_mode_single_stage_fe2:
        case soc_dfe_fabric_device_mode_multi_stage_fe2:
        case soc_dfe_fabric_device_mode_multi_stage_fe13:
        case soc_dfe_fabric_device_mode_repeater:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("FE1600 does not support this device mode")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_graceful_shutdown_set(int unit, soc_pbmp_t active_links, int shutdown, soc_pbmp_t unisolated_links, int isolate_device) 
{
    int rv;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if (SOC_DFE_IS_FE13(unit)) 
    {
        
        rv =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_fe13_graceful_shutdown_set, (unit, active_links, unisolated_links, shutdown));
        SOCDNX_IF_ERR_EXIT(rv);
    } 
    else 
    {
        
        if (shutdown) {
            
            rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_reachability_mask_set,(unit, active_links,soc_dcmn_isolation_status_isolated));
            SOCDNX_IF_ERR_EXIT(rv);

            sal_usleep(1000000); 

            
            SOC_PBMP_ITER(active_links, port)
            {
                rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_rx_enable_set, (unit, port, 0, 0));
                SOCDNX_IF_ERR_EXIT(rv);
            }
            sal_usleep(20000); 

            
            SOC_PBMP_ITER(active_links, port)
            {
                rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_tx_enable_set, (unit, port, 0));
                SOCDNX_IF_ERR_EXIT(rv);
            }
            sal_usleep(50000); 

            if (!isolate_device)
            {
                
                rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_isolate_set, (unit, soc_dcmn_isolation_status_isolated)); 
                SOCDNX_IF_ERR_EXIT(rv);   
            }
        } else {

            
            if (!isolate_device)
            {
                rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_set,(unit, soc_dcmn_isolation_status_active)); 
                SOCDNX_IF_ERR_EXIT(rv);   
            }

            
            SOC_PBMP_ITER(active_links, port)
            {
                rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_tx_enable_set, (unit, port, 1));
                SOCDNX_IF_ERR_EXIT(rv);
            }

            
            SOC_PBMP_ITER(active_links, port)
            {
                rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_rx_enable_set, (unit, port, 0, 1));
                SOCDNX_IF_ERR_EXIT(rv);
            }

            sal_usleep(SOC_FE1600_GRACEFUL_SHUT_DOWN_DISABLE_DELAY); 

            
            rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_reachability_mask_set,(unit, unisolated_links,soc_dcmn_isolation_status_active));
            SOCDNX_IF_ERR_EXIT(rv);

        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}




int
soc_fe1600_drv_fe13_graceful_shutdown_set(int unit, soc_pbmp_t active_links, soc_pbmp_t unisolated_links, int shutdown) 
{
    int rv;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_FE1600_ONLY(unit);

    if (shutdown)
    {
        rv = soc_fe1600_drv_fe13_isolate_set(unit, unisolated_links, 1);
        SOCDNX_IF_ERR_EXIT(rv);

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_fe1600_port_control_rx_enable_set(unit, port, 0, 0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(20000); 

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_fe1600_port_control_tx_enable_set(unit, port, 0);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(50000); 

    } else { 

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_fe1600_port_control_tx_enable_set(unit, port, 1);
            SOCDNX_IF_ERR_EXIT(rv);
        }

        
        SOC_PBMP_ITER(active_links, port)
        {
            rv = soc_fe1600_port_control_rx_enable_set(unit, port, 0, 1);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        sal_usleep(1000000); 

        rv = soc_fe1600_drv_fe13_isolate_set(unit, unisolated_links, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_fe13_links_bitmap_get(int unit, soc_reg_above_64_val_t *all_links_bitmap, soc_reg_above_64_val_t *fap_links_bitmap, soc_reg_above_64_val_t *fe2_links_bitmap)
{
    soc_port_t port;
    int rv;
    soc_dfe_fabric_link_device_mode_t link_device_mode;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(*all_links_bitmap);
    SOC_REG_ABOVE_64_CLEAR(*fap_links_bitmap);
    SOC_REG_ABOVE_64_CLEAR(*fe2_links_bitmap);
    for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links); port++)
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_link_device_mode_get, (unit, port, 1, &link_device_mode));
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITSET(*all_links_bitmap, port);
        if (link_device_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
        {
            
            SHR_BITSET(*fap_links_bitmap, port);
        } else {
            
            SHR_BITSET(*fe2_links_bitmap, port);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_fe13_isolate_set(int unit, soc_pbmp_t unisolated_links_pbmp, int isolate) 
{
    int rv;
    int ccs_index;
    soc_reg_above_64_val_t reg_above_64_val, all_links_bitmap, fap_links_bitmap, fe2_links_bitmap, unisolated_links;
    uint32 reg32;
    soc_port_t port;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_FE1600_ONLY(unit);

    rv = soc_fe1600_drv_fe13_links_bitmap_get(unit, &all_links_bitmap, &fap_links_bitmap, &fe2_links_bitmap);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(unisolated_links);
    
    SOC_PBMP_ITER(unisolated_links_pbmp, port)
    {
        
        SHR_BITSET(unisolated_links, port);
    }

    if (isolate)
    {
        
        rv = READ_RTP_REG_0063r(unit, &reg32);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, RTP_REG_0063r, &reg32, FIELD_0_7f, 0x0);
        rv = WRITE_RTP_REG_0063r(unit, reg32);
        SOCDNX_IF_ERR_EXIT(rv);

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));
        SOC_REG_ABOVE_64_AND(reg_above_64_val, fe2_links_bitmap);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(READ_CCS_REG_0062r(unit, ccs_index, reg_above_64_val));
            SOC_REG_ABOVE_64_AND(reg_above_64_val, fe2_links_bitmap);
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0062r(unit, ccs_index, reg_above_64_val));
        }       
        
        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0062r(unit, ccs_index, reg_above_64_val));
        }       
        
        sal_usleep(30000); 

        
        rv = READ_RTP_REG_0063r(unit, &reg32);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, RTP_REG_0063r, &reg32, FIELD_0_7f, 0x1);
        rv = WRITE_RTP_REG_0063r(unit, reg32);
        SOCDNX_IF_ERR_EXIT(rv);

        sal_usleep(1000000); 

        
    } else { 

        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_mesh_topology_reset(unit));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_COPY(reg_above_64_val, fe2_links_bitmap);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0062r(unit, ccs_index, reg_above_64_val));
        } 
       
        sal_usleep(30000); 

        SOC_REG_ABOVE_64_COPY(reg_above_64_val, fe2_links_bitmap);
        SOC_REG_ABOVE_64_AND(reg_above_64_val, unisolated_links);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));

        sal_usleep(30000); 

        
        SOC_REG_ABOVE_64_COPY(reg_above_64_val, all_links_bitmap);
        for (ccs_index = 0; ccs_index < SOC_DFE_DEFS_GET(unit, nof_instances_ccs); ccs_index++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0062r(unit, ccs_index, reg_above_64_val));
        }
        
        sal_usleep(30000); 

        SOC_REG_ABOVE_64_COPY(reg_above_64_val, all_links_bitmap);
        SOC_REG_ABOVE_64_AND(reg_above_64_val, unisolated_links);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
soc_fe1600_drv_sw_ver_set(int unit)
{
    int         ver_val[3] = {0,0,0};
    uint32      regval, i, prev_regval;
    char        *ver;
    char        *cur_number_ptr;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
   
    
    regval = 0;
    ver = _build_release;

    
    cur_number_ptr = sal_strstr(ver, "JR2");
    if(cur_number_ptr != NULL) {
        SOC_EXIT;
    }

    cur_number_ptr = sal_strchr(ver, '-');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid Build Release")));
    }
    ++cur_number_ptr;
    ver_val[0] = _shr_ctoi (cur_number_ptr);
    cur_number_ptr = sal_strchr(cur_number_ptr, '.');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid Build Release")));
    }
    ++cur_number_ptr;
    ver_val[1] = _shr_ctoi (cur_number_ptr);
    cur_number_ptr = sal_strchr(cur_number_ptr, '.');
    if(cur_number_ptr == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Invalid Build Release")));
    }
    ++cur_number_ptr;
    ver_val[2] = _shr_ctoi (cur_number_ptr);

   
    SOCDNX_IF_ERR_EXIT(READ_ECI_REG_0053r(unit, &prev_regval));

    for (i=0; i<3; i++) {
       regval = (regval | (0xf & ver_val[i]))<<4;
   }
    
   if (SOC_WARM_BOOT(unit)) {
       regval = regval | 0x8;
   }
   regval = (regval << 16 ) | ( prev_regval & 0xffff);

   SOC_DFE_ALLOW_WARMBOOT_WRITE(WRITE_ECI_REG_0053r(unit, regval), rc);
   SOCDNX_IF_ERR_EXIT(rc);

exit:
  SOCDNX_FUNC_RETURN;;
}
 
#ifdef BCM_88754_A0

#define SOC_FE1600_DRV_BCM88754_SIF_POLLING_TIMEOUT                 (1000000) 
#define SOC_FE1600_DRV_BCM88754_SIF_POLLING_MIN_POLLS               (100)     
#define SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_MASK     (0x1FFFFFF)
#define SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_OFFSET   (3)

int
soc_fe1600_drv_bcm88754_async_fifo_set(int unit, int fmac_index, int lane, soc_field_t field, uint32 data) 
{
    uint64 reg64_val;
    uint32 reg32_val;
    uint32 read_valid;
    soc_timeout_t to;
    uint32 async_fifo_config_read;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    

    

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, 0 , &reg64_val));
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMAND_LANEf, lane);
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMANDf, 0x1); 
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_READ_COMMANDf, 0x0); 
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, 0 , reg64_val));

    
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_ACCESS_TRIGGERr(unit, fmac_index, 0x1));
    sal_usleep(10); 

    
    if (SAL_BOOT_PLISIM) {
        read_valid = 1; 
    } else {
        read_valid = 0;
    }

    soc_timeout_init(&to, SOC_FE1600_DRV_BCM88754_SIF_POLLING_TIMEOUT , SOC_FE1600_DRV_BCM88754_SIF_POLLING_MIN_POLLS);
    reg32_val = 0;
    while (read_valid == 0)
    {
        if (soc_timeout_check(&to)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("FMAC SIF timeout")));
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_ACCESS_READ_DATAr(unit, fmac_index, &reg32_val));
        read_valid = soc_reg_field_get(unit, FMAC_ASYNC_FIFO_ACCESS_READ_DATAr, reg32_val, FMAL_SIF_CPU_COMMAND_DATA_VALIDf);
    }

    
    
    async_fifo_config_read = (reg32_val & SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_MASK) << SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_OFFSET;
    COMPILER_64_SET(reg64_val, 0x0, async_fifo_config_read);

    
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMAND_LANEf, lane);
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_READ_COMMANDf, 0x0);  
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMANDf, 0x0); 
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, field, data);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, 0 , reg64_val));

    
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_ACCESS_TRIGGERr(unit, fmac_index, 0x1));

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_bcm88754_async_fifo_get(int unit, int fmac_index, int lane, int field, uint32 *data) 
{
    uint64 reg64_val;
    uint32 reg32_val;
    uint32 read_valid;
    soc_timeout_t to;
    uint32 async_fifo_config_read;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    

    

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, 0 , &reg64_val));
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMAND_LANEf, lane);
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_COMMANDf, 0x1); 
    soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_SIF_CPU_READ_COMMANDf, 0x0); 
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, 0 , reg64_val));

    
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_ACCESS_TRIGGERr(unit, fmac_index, 0x1));
    sal_usleep(10); 

    
    if (SAL_BOOT_PLISIM) {
        read_valid = 1; 
    } else {
        read_valid = 0;
    }

    soc_timeout_init(&to, SOC_FE1600_DRV_BCM88754_SIF_POLLING_TIMEOUT , SOC_FE1600_DRV_BCM88754_SIF_POLLING_MIN_POLLS);
    reg32_val = 0;
    while (read_valid == 0)
    {
        if (soc_timeout_check(&to)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_TIMEOUT, (_BSL_SOCDNX_MSG("FMAC SIF timeout")));
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_ACCESS_READ_DATAr(unit, fmac_index, &reg32_val));
        read_valid = soc_reg_field_get(unit, FMAC_ASYNC_FIFO_ACCESS_READ_DATAr, reg32_val, FMAL_SIF_CPU_COMMAND_DATA_VALIDf);
    }

    
    
    async_fifo_config_read = (reg32_val & SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_MASK) << SOC_FE1600_DRV_BCM88754_SIF_ASYNC_FIFO_CONFIG_READ_OFFSET;
    COMPILER_64_SET(reg64_val, 0x0, async_fifo_config_read);
    *data = soc_reg64_field32_get(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, reg64_val, field);

exit:
    SOCDNX_FUNC_RETURN;
}
#endif 



int
soc_fe1600_drv_mbist(int unit, int skip_errors)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    SOC_DFE_DRV_INIT_LOG(unit, "Memory Bist");
#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        rc = soc_bist_all_fe1600_bcm88754(unit, skip_errors);
    } else 
#endif 
    {
        rc = soc_bist_all_fe1600(unit, skip_errors);
    }
    SOCDNX_IF_ERR_EXIT(rc);
        
exit:
    SOCDNX_FUNC_RETURN;   
}

int
soc_fe1600_drv_link_to_block_mapping(int unit, int link, int* block_id,int* inner_link, int block_type)
{   
    int nof_links_in_blk, asymmetric_quad = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_DFE_IS_FE13_ASYMMETRIC(unit) && (block_type == SOC_BLK_DCH || block_type == SOC_BLK_DCM))
    {
        MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_asymmetrical_quad_get, (unit, link, &asymmetric_quad));
        if (asymmetric_quad == 0 || asymmetric_quad == 1)
        {
            *block_id = asymmetric_quad; 
            *inner_link = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) + link % SOC_DFE_DEFS_GET(unit, nof_links_in_quad); 
            SOC_EXIT;
        }
    }
    switch (block_type)
    {
       case SOC_BLK_DCH:
       case SOC_BLK_DCM:

           nof_links_in_blk = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);

           *block_id=INT_DEVIDE(link,nof_links_in_blk);
           *inner_link=link % nof_links_in_blk;
           break;

       case SOC_BLK_DCL:
           nof_links_in_blk = SOC_DFE_DEFS_GET(unit, nof_links_in_dcl);

           *block_id=INT_DEVIDE(link,nof_links_in_blk);
           *inner_link=link % nof_links_in_blk;
           break;

       case SOC_BLK_FMAC:
           nof_links_in_blk = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

           *block_id=INT_DEVIDE(link,nof_links_in_blk);
           *inner_link=link % nof_links_in_blk;
           break;


       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("block (%d) - link to block mapiing is not supported"), block_type));
           break;
    }


exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_fe1600_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp)
{   
    int first_link = 0,range = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(*pbmp);

    switch (block_type)
    {
       case SOC_BLK_DCH:
       case SOC_BLK_DCM:
           first_link = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq) * blk_instance;
           range = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);
           
           break;
       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("block (%d) - block pbmp is not supported"), block_type));
           break;
    }

    SOC_PBMP_PORTS_RANGE_ADD(*pbmp, first_link, range);
    SOC_PBMP_AND(*pbmp, PBMP_SFI_ALL(unit));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_fe1600_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap) 
{
    uint64 reg_val64;
    uint32 reg_val32;

    SOCDNX_INIT_FUNC_DEFS;

    if (force_blocks_reset_value)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG_STR("block reset force is not supported")));
    } else {
         
        SOCDNX_IF_ERR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, 0x0));
        sal_usleep(20);
        SOCDNX_IF_ERR_RETURN(WRITE_CMIC_SOFT_RESET_REGr(unit, 0xffffffff));
        sal_usleep(20);
        COMPILER_64_SET(reg_val64, 0x7, 0xFFFFFFFF);
        SOCDNX_IF_ERR_RETURN(WRITE_ECI_FE_1600_SOFT_RESETr(unit, reg_val64));
        sal_sleep(1);
        COMPILER_64_SET(reg_val64, 0x0, 0x0);
        SOCDNX_IF_ERR_RETURN(WRITE_ECI_FE_1600_SOFT_RESETr(unit,reg_val64));
        reg_val32 = 0x1ff;
        SOCDNX_IF_ERR_RETURN(WRITE_ECI_SB_RSTN_AND_POWER_DOWNr(unit,reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;  
}


int
soc_fe1600_drv_test_reg_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch(reg) 
    {
        case ECI_POWER_UP_MACHINEr:
        case CMIC_SOFT_RESET_REGr:
        case CMIC_SOFT_RESET_REG_2r:
        case ECI_FE_1600_SOFT_RESETr:
        case ECI_FE_1600_SOFT_INITr:
        case ECI_SB_RSTN_AND_POWER_DOWNr:
        case FMAC_ASYNC_FIFO_CONFIGURATIONr:
        
        case ECI_INDIRECT_COMMANDr:
        case FSRD_INDIRECT_COMMANDr:
        case RTP_INDIRECT_COMMANDr:
                *is_filtered = 1;
        default:
            break;
    }

#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        switch(reg)
        {
            case ECI_REG_0120r:
                *is_filtered = 1;
            default:
                break;
        }
    }
#endif 

    SOCDNX_FUNC_RETURN; 
}


int
soc_fe1600_drv_test_reg_default_val_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    
    switch(reg) {
        case FMAC_ASYNC_FIFO_CONFIGURATIONr:
        case CCS_REG_0058r:
        case DCL_REG_005Cr:
        case FMAC_SBUS_LAST_IN_CHAINr:
        case FSRD_REG_0058r:
        case ECI_FE_1600_SOFT_RESETr:
        case ECI_FE_1600_SOFT_INITr:
        case ECI_SB_RSTN_AND_POWER_DOWNr:
        case FMAC_SBUS_BROADCAST_IDr:
        case CCS_REG_0080r:
        
        case FMAC_REG_005Ar:
        case DCH_REG_005Ar:
        case DCMA_REG_005Ar:
        case DCMB_REG_005Ar:
        case DCMC_REG_005Ar:
        case DCL_REG_005Ar:
        case RTP_REG_005Ar:
        case MESH_TOPOLOGY_GLOBAL_REGr:
            *is_filtered = 1; 
        default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}



int
soc_fe1600_drv_test_mem_filter(int unit, soc_mem_t mem, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch(mem) 
    {
        case RTP_RMHMTm:
        case RTP_CUCTm:
        case RTP_DUCTPm:
        case RTP_DUCTSm:
        case RTP_MEM_800000m:
        case RTP_MEM_900000m:
            *is_filtered = 1;
        default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}


int
soc_fe1600_drv_test_brdc_blk_filter(int unit, soc_reg_t reg, int *is_filtered)
{
    SOCDNX_INIT_FUNC_DEFS;

    *is_filtered = 0;

    switch(reg) 
    {   
        
        case BRDC_FMACL_ASYNC_FIFO_CONFIGURATIONr:
        case BRDC_FMACL_SBUS_BROADCAST_IDr:
        case BRDC_FMACL_SBUS_LAST_IN_CHAINr:
        case BRDC_FMACL_GTIMER_TRIGGERr:
        
        case BRDC_FMACH_ASYNC_FIFO_CONFIGURATIONr:
        case BRDC_FMACH_SBUS_BROADCAST_IDr:
        case BRDC_FMACH_SBUS_LAST_IN_CHAINr:
        case BRDC_FMACH_GTIMER_TRIGGERr:
        
        case BRDC_FSRD_SBUS_BROADCAST_IDr:
        case BRDC_FSRD_REG_0054r:
        case BRDC_FSRD_REG_0058r:
        case BRDC_FSRD_REG_01E9r:
        case BRDC_FSRD_REG_01EBr:
        case BRDC_FSRD_REG_01ECr:
        case BRDC_FSRD_REG_01EDr:
        case BRDC_FSRD_REG_01EFr:
        case BRDC_FSRD_REG_01F1r:
        case BRDC_FSRD_REG_01F3r:
        case BRDC_FSRD_GTIMER_TRIGGERr:
        case BRDC_FSRD_INDIRECT_COMMANDr:
                *is_filtered = 1;
                break;
        default:
            break;
    }

    SOCDNX_FUNC_RETURN; 
}

int 
soc_fe1600_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size)
{
    int instance;
    int i;
    SOCDNX_INIT_FUNC_DEFS;
    *actual_size = 0;

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMACL;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 2; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMACH;
        for (i = 0, instance = SOC_DFE_DEFS_GET(unit, nof_instances_mac) / 2; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

    
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FSRD;
        for (i = 0, instance = 0; instance < SOC_DFE_DEFS_GET(unit, nof_instances_mac_fsrd); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FSRD_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("Test does not support all device block ids")));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int
soc_fe1600_drv_block_valid_get(int unit, int blktype, int blkid, char *valid)
{
    int nof_block_instances;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_FE1600_REDUCED(unit)) 
    {
        SOCDNX_IF_ERR_EXIT(soc_fe1600_nof_block_instances(unit, &blktype, &nof_block_instances));
        if (blkid >= nof_block_instances)
        {
            *valid = 0;
        } else {
            *valid = 1;
        }
    } else {

        *valid = 1;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

#endif 

#undef _ERR_MSG_MODULE_NAME

