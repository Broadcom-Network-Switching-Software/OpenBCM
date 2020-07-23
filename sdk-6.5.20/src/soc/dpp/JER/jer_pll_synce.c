/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */





#include <soc/debug.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>


#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/QAX/qax_nif.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_pll_synce.h>

#if defined PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif


#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#define TSCE_SYNCE_CTRL_REG_ADDR  0x9002
#define TSCF_SYNCE_CTRL_REG_ADDR  0x9001


typedef enum
{
    QAX_PLL_TYPE_BSPLL0 = 0,
    QAX_PLL_TYPE_BSPLL1 = 1
}QAX_BSPLL_TYPE;

typedef enum
{
    QUX_PLL_TYPE_NIF_PML = 0,
    QUX_PLL_TYPE_NIF_PMX = 1
}QUX_NIF_PLL_TYPE;



 
int jer_plus_pll_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    uint32                      ndiv,
                    uint32                      mdiv,
                    uint32                      pdiv,
                    uint32                      is_bypass)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_SYNCE_MASTER_PLL_CONFIGr, ECI_SYNCE_SLAVE_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_MASTER_SYNCE_PLL_STATUSr, ECI_SLAVE_SYNCE_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_7_LOCKEDf, MISC_PLL_8_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);


    
    SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_NDIVf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_CH_0_MDIVf, field_above_64);

    
    val = 6;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_KPf, field_above_64);

    
    val = 4;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_KIf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&pdiv,0,sizeof(pdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PDIVf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_STAT_RESETf, field_above_64);


    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_VCO_FB_DIV_2f, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_VCO_RANGEf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PWRONf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PWRON_LDOf, field_above_64);

    
    if (is_bypass) {
        
        val = 0;
    } else {
        
        val = 1;
    }

    if (eci_srd_pll_config[pll_type] == ECI_SYNCE_MASTER_PLL_CONFIGr) {
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SEL_MASTERf, field_above_64);

        
        val = is_bypass;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_EN_MASTERf, field_above_64);
    }

    if (eci_srd_pll_config[pll_type] == ECI_SYNCE_SLAVE_PLL_CONFIGr) {
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SEL_SLAVEf, field_above_64);

        
        val = is_bypass;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_EN_SLAVEf, field_above_64);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(30);

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

    
    if(!is_bypass){
      if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
          soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type], 1);
          if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
             SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type])));
          }
      }
    }
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_POST_RESET_Bf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int jer_pll_4_set(
                    int                         unit,
                    QAX_BSPLL_TYPE                pll_type)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_BS_0_PLL_CONFIGr, ECI_BS_1_PLL_CONFIGr};

    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_BS_0_PLL_STATUSr, ECI_BS_1_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_1_LOCKEDf, MISC_PLL_2_LOCKEDf};

    const static soc_pll_param_t bs_pll_config[] = {
        
        {25000000,  120,    0,          1,  150},
        {12800000,  250,    0,          1,  160},
        {50000000,  128,    0,          4,  40},
    };

    unsigned ts_pll_ref_freq=25000000; 
    unsigned bs_pll_ref_freq=25000000; 
    unsigned bs_pll_config_idx = 0; 

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    
    if (SOC_IS_QUX(unit)) {
        ts_pll_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 25000000);
        bs_pll_ref_freq = ts_pll_ref_freq;
        for (bs_pll_config_idx = 0; bs_pll_config_idx < sizeof(bs_pll_config)/sizeof(bs_pll_config[0]); bs_pll_config_idx++) {
            if (bs_pll_config[bs_pll_config_idx].ref_freq == bs_pll_ref_freq) {
                break;
            }
        }
        if (bs_pll_config_idx == sizeof(bs_pll_config)/sizeof(bs_pll_config[0])) {
            bs_pll_config_idx = 0; 
        }
    }

    
    if(SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
       val = 120; 
    } else if (SOC_IS_QUX(unit)) {
        val = bs_pll_config[bs_pll_config_idx].ndiv_int;
    } else {
       val = 140;
    }

    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_FBDIV_NDIV_INTf, field_above_64);

    
    if(SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
       val = 150; 
    } else if (SOC_IS_QUX(unit)) {
        val = bs_pll_config[bs_pll_config_idx].mdiv;
    } else {
       val = 140;
    }

    SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CH_0_MDIVf, field_above_64);

    
    val = 3;
    if (SOC_IS_QUX(unit)) {
        if (bs_pll_ref_freq == 50000000) {
            
            val = 5;
        }
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_KPf, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_KIf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_KAf, field_above_64);

    
    val = 1;
    if (SOC_IS_QUX(unit)) {
        val =  bs_pll_config[bs_pll_config_idx].pdiv;
    }

    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_PDIVf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CTRL_VEC_PWM_RATEf, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CTRL_VEC_POST_RESET_SELf, field_above_64);

    
    val = 1;
    if (SOC_IS_QUX(unit)) {
        if (bs_pll_ref_freq == 50000000) {
            
            val = 0;
        }
    }

    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CTRL_VEC_VCO_FB_DIV_2f, field_above_64);

    
    val = 2;
    if (SOC_IS_QUX(unit)) {
        if (bs_pll_ref_freq == 50000000) {
            
            val = 0;
        }
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CTRL_VEC_VCO_RANGEf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CTRL_VEC_LDOf, field_above_64);

    
    if (SOC_IS_QUX(unit)) {
        if  (bs_pll_ref_freq == 12800000) {
            val = 156;
        } else {
            
            val = 160;
        }
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_CLK_PRDf, field_above_64);

    if (SOC_IS_QUX(unit)) {
        
        val = 1;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_BYP_HOLDOVERf, field_above_64);
    }

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_PWRON_LDOf, field_above_64);

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_BS_0_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(1);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_PWR_ONf, field_above_64);

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(30);

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_RESETBf, field_above_64);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, reg_above_64));

    
    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
        soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type-QAX_PLL_TYPE_BSPLL0], 1);
        if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type-QAX_PLL_TYPE_BSPLL0])));
        }
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_BS_0_PLL_CONFIGr, reg_above_64, BS_0_PLL_POST_RESETBf, field_above_64);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-QAX_PLL_TYPE_BSPLL0], REG_PORT_ANY, 0, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_pll_3_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    uint32                      ndiv,
                    uint32                      mdiv,
                    uint32                      pdiv,
                    uint32                      is_bypass)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_FAB_0_PLL_CONFIGr, ECI_FAB_1_PLL_CONFIGr, ECI_NIF_PMH_PLL_CONFIGr, ECI_NIF_PML_0_PLL_CONFIGr, ECI_NIF_PML_1_PLL_CONFIGr, ECI_SYNCE_MASTER_PLL_CONFIGr, ECI_SYNCE_SLAVE_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_FAB_0_PLL_STATUSr, ECI_FAB_1_PLL_STATUSr, ECI_PMH_PLL_STATUSr, ECI_PML_0_PLL_STATUSr, ECI_PML_1_PLL_STATUSr, ECI_MASTER_SYNCE_PLL_STATUSr, ECI_SLAVE_SYNCE_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_2_LOCKEDf, MISC_PLL_3_LOCKEDf, MISC_PLL_4_LOCKEDf, MISC_PLL_5_LOCKEDf, MISC_PLL_6_LOCKEDf, MISC_PLL_7_LOCKEDf, MISC_PLL_8_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    if(!SOC_IS_QAX(unit)) {

       
       SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_NDIVf, field_above_64);

       if (pll_type == JER_PLL_TYPE_FABRIC_0) {
          
          val = 20;
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_CH_4_MDIVf, field_above_64);
       }

       
       SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_CH_0_MDIVf, field_above_64);

       
       val = 6;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KPf, field_above_64);

       
       val = 4;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KIf, field_above_64);

       
       val = 0;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_KPPf, field_above_64);

       
       SHR_BITCOPY_RANGE(field_above_64,0,&pdiv,0,sizeof(pdiv)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PDIVf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_STAT_RESETf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_LC_BOOSTf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_CMLBUF_0_PWRONf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_CMLBUF_1_PWRONf, field_above_64);

       
       val = 2;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

       
       val = 2;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PLL_CTRL_POST_RST_SELf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRONf, field_above_64);

       
       val = 1;
       SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_PWRON_LDOf, field_above_64);

       
       if (is_bypass) {
          
          val = 0;
       } else {
          
          val = 1;
       }
       if (eci_srd_pll_config[pll_type] == ECI_SYNCE_MASTER_PLL_CONFIGr) {
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SELf, field_above_64);

          
          val = is_bypass;
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_ENf, field_above_64);
       }
       if (eci_srd_pll_config[pll_type] == ECI_SYNCE_SLAVE_PLL_CONFIGr) {
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SELf, field_above_64);

          
          val = is_bypass;
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_ENf, field_above_64);
       }

       
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

       sal_usleep(30);

       
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
       SOC_REG_ABOVE_64_CLEAR(field_above_64);
       SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
       soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_RESET_Bf, field_above_64);
       SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

       
       if(((pll_type != JER_PLL_TYPE_SYNCE_0) && (pll_type != JER_PLL_TYPE_SYNCE_1)) || (!is_bypass)){
          if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
              soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type], 1);
              if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
                 SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type])));
              }
          }
       }
      
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
      SOC_REG_ABOVE_64_CLEAR(field_above_64);
      SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
      soc_reg_above_64_field_set(unit, ECI_FAB_0_PLL_CONFIGr, reg_above_64, FAB_0_PLL_CFG_POST_RESET_Bf, field_above_64);
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
   } else { 
      
      SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_NDIVf, field_above_64);

      if (pll_type == JER_PLL_TYPE_FABRIC_0) {
          
          val = 20;
          SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
          soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_CH_4_MDIVf, field_above_64);
      }

      
      SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_CH_0_MDIVf, field_above_64);

      
      val = 6;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_KPf, field_above_64);

      
      val = 4;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_KIf, field_above_64);

      
      val = 0;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_KPPf, field_above_64);

      
      SHR_BITCOPY_RANGE(field_above_64,0,&pdiv,0,sizeof(pdiv)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PDIVf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_STAT_RESETf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_LC_BOOSTf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_CMLBUF_0_PWRONf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_CMLBUF_1_PWRONf, field_above_64);

      
      val = 2;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

      
      val = 2;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PLL_CTRL_POST_RST_SELf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PWRONf, field_above_64);

      
      val = 1;
      SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_PWRON_LDOf, field_above_64);

      
      if (is_bypass) {
         
         val = 0;
      } else {
         
         val = 1;
      }
      if (eci_srd_pll_config[pll_type] == ECI_SYNCE_MASTER_PLL_CONFIGr) {
         SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
         soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_REFRENCE_NIF_SELf, field_above_64);

         
         val = is_bypass;
         SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
         soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_MASTER_PLL_CFG_BYPASS_PLL_ENf, field_above_64);
      }
      if (eci_srd_pll_config[pll_type] == ECI_SYNCE_SLAVE_PLL_CONFIGr) {
         SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
         soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_REFRENCE_NIF_SELf, field_above_64);

         
         val = is_bypass;
         SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
         soc_reg_above_64_field_set(unit, eci_srd_pll_config[pll_type], reg_above_64, SYNCE_SLAVE_PLL_CFG_BYPASS_PLL_ENf, field_above_64);
      }

      
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

      sal_usleep(30);

      
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
      SOC_REG_ABOVE_64_CLEAR(field_above_64);
      SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_RESET_Bf, field_above_64);
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));

      
      if(!is_bypass){
         if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
             soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type], 1);
             if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type])));
             }
          }
      }
      
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));
      SOC_REG_ABOVE_64_CLEAR(field_above_64);
      SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
      soc_reg_above_64_field_set(unit, ECI_SYNCE_MASTER_PLL_CONFIGr, reg_above_64, SYNCE_MASTER_PLL_CFG_POST_RESET_Bf, field_above_64);
      SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type], REG_PORT_ANY, 0, reg_above_64));


   }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int jer_pll_2_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    uint32                      ndiv,
                    uint32                      mdiv,
                    uint32                      icp,
                    uint32                      is_bypass)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_NIF_PMH_PLL_CONFIGr, ECI_NIF_PML_0_PLL_CONFIGr, ECI_NIF_PML_1_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_PMH_PLL_STATUSr, ECI_PML_0_PLL_STATUSr, ECI_PML_1_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_4_LOCKEDf, MISC_PLL_5_LOCKEDf, MISC_PLL_6_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_PWRONf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_PWRON_LDOf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_RESETBf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_POST_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));

    
    sal_usleep(5000);

     

    
    val = 5;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_VCO_GAINf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_SSC_MODEf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CLK_PRDf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CPf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CP_1f, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_PDIVf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CH_0_MDIVf, field_above_64);

    
    val = 0x5;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CH_1_MDIVf, field_above_64);

    
    val = ((JER_PLL_TYPE_NIF_PMH == pll_type) ? 0x15 : 0x4);
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CH_2_MDIVf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CH_3_MDIVf, field_above_64);

    
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency == 325000) {
        
        if (pll_type == JER_PLL_TYPE_NIF_PMH) {
            val = 0x6;
        } else {
            val = 0x8;
        }
    } else {
        val = 0x5;
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CH_4_MDIVf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_RZf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CZf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_MSC_CTRL_VEC_REF_SELf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_MSC_CTRL_VEC_CML_ENf, field_above_64);

    
    val = 0x2a;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_LDO_CTRLf, field_above_64);

    
    val = 0xB0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CTRL_VEC_CPPf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CTRL_VEC_FREQ_DOUBL_DELf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&icp,0,sizeof(icp)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_ICPf, field_above_64);

    
    val = 7;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_RPf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_MSC_CTRL_VEC_TEST_BUF_ENf, field_above_64);

    
    if(SOC_IS_QAX(unit)) {
        val = 0x14; 
    } else {
        val = 0x19;
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_FBDIV_NDIV_INTf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_BYP_HOLDOVERf, field_above_64);

    
    val = 4;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_MSC_CTRL_VEC_D_2C_BIASf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_CTRL_VEC_POST_RES_SELf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_PWRONf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_PWRON_LDOf, field_above_64);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));

    
    sal_usleep(5000);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));

    
    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
         soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type - JER_PLL_TYPE_NIF_PMH], 1);
         if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type - JER_PLL_TYPE_NIF_PMH])));
         }
    }

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_NIF_PMH_PLL_CONFIGr, reg_above_64, PMH_PLL_POST_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - JER_PLL_TYPE_NIF_PMH], REG_PORT_ANY, 0, reg_above_64));

    if(is_bypass){
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: bypass option is not supported\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int qux_pll_2_set(
                    int                         unit,
                    QUX_NIF_PLL_TYPE            pll_type,
                    uint32                      ndiv,
                    uint32                      mdiv,
                    uint32                      icp,
                    uint32                      is_bypass)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_NIF_PML_PLL_CONFIGr, ECI_NIF_PMX_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_PML_PLL_STATUSr, ECI_PMX_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_4_LOCKEDf, MISC_PLL_5_LOCKEDf};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    
    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_PWRONf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_PWRON_LDOf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_RESETBf, field_above_64);
    
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_POST_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));

    
    sal_usleep(5000);

    
    val = 5;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_VCO_GAINf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_SSC_MODEf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CLK_PRDf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CPf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CP_1f, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_PDIVf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&mdiv,0,sizeof(mdiv)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CH_0_MDIVf, field_above_64);

    
    val = 0x5;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CH_1_MDIVf, field_above_64);

    
    val = ((pll_type == QUX_PLL_TYPE_NIF_PML ) ? 4 : 0x50);
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CH_2_MDIVf, field_above_64);

    
    val = 0x7;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CH_4_MDIVf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_RZf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CZf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_MSC_CTRL_VEC_REF_SELf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_MSC_CTRL_VEC_CML_ENf, field_above_64);

    
    val = 0x2a;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_LDO_CTRLf, field_above_64);

    
    val = 0xB0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CTRL_VEC_CPPf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CTRL_VEC_FREQ_DOUBL_DELf, field_above_64);

    
    icp = 24;
    SHR_BITCOPY_RANGE(field_above_64,0,&icp,0,sizeof(icp)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_ICPf, field_above_64);

    
    val = 7;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_RPf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_MSC_CTRL_VEC_TEST_BUF_ENf, field_above_64);

    
    SHR_BITCOPY_RANGE(field_above_64,0,&ndiv,0,sizeof(ndiv)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_FBDIV_NDIV_INTf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_BYP_HOLDOVERf, field_above_64);

    
    val = 4;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_MSC_CTRL_VEC_D_2C_BIASf, field_above_64);

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CTRL_VEC_POST_RES_SELf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_PWRONf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_PWRON_LDOf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CTRL_VEC_RATE_MANGERf, field_above_64);

    
    val = 25;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_CLK_PRDf, field_above_64);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));

    
    sal_usleep(5000);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));

    
    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
         soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type - QUX_PLL_TYPE_NIF_PML], 1);
         if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type - QUX_PLL_TYPE_NIF_PML])));
         }
    }

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    soc_reg_above_64_field_set(unit, ECI_NIF_PML_PLL_CONFIGr, reg_above_64, PML_PLL_POST_RESETBf, field_above_64);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type - QUX_PLL_TYPE_NIF_PML], REG_PORT_ANY, 0, reg_above_64));

    if(is_bypass){
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: bypass option is not supported\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int jer_pll_1_set(
                    int                         unit,
                    JER_PLL_TYPE                pll_type)
{
    uint32 val;
    SOC_SAND_RET soc_sand_ret = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64, field_above_64;

    const static soc_reg_t
        eci_srd_pll_config[] = {ECI_TS_PLL_CONFIGr, ECI_BS_PLL_CONFIGr};
    const static soc_reg_t
        eci_srd_pll_status[] = {ECI_TS_PLL_STATUSr, ECI_BS_PLL_STATUSr};
    const static soc_field_t
        eci_srd_pll_locked[] = {MISC_PLL_0_LOCKEDf, MISC_PLL_1_LOCKEDf};

    const static soc_pll_param_t ts_pll_config[] = {
        
        {25000000,  120,    0,          1},
        {12800000,  234,    393216,     1},
        {50000000,  240,    0,          4},
    };

    unsigned ts_pll_ref_freq=25000000; 
    unsigned ts_pll_config_idx = 0; 

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_CLEAR(field_above_64);

    if (SOC_IS_QAX(unit)) {
        
        
        val = 0;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PWRONf, field_above_64);
        
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PWRON_LDOf, field_above_64);
        
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_RESET_Bf, field_above_64);
        
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_POST_RESET_Bf, field_above_64);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_TS_PLL_CONFIGr, REG_PORT_ANY, 0, reg_above_64));

        
        sal_usleep(1000);
    }

    if (SOC_IS_QUX(unit)) {
        ts_pll_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 25000000);
        for (ts_pll_config_idx = 0; ts_pll_config_idx < sizeof(ts_pll_config)/sizeof(ts_pll_config[0]); ts_pll_config_idx++) {
            if (ts_pll_config[ts_pll_config_idx].ref_freq == ts_pll_ref_freq) {
                break;
            }
        }
        if (ts_pll_config_idx == sizeof(ts_pll_config)/sizeof(ts_pll_config[0])) {
            ts_pll_config_idx = 0; 
        }
    }

#if defined(ENABLE_TS_PLL_HW_WAR)
    if (SOC_IS_QUX(unit)) {
        soc_reg_field32_modify(unit, ECI_OGER_1033r, REG_PORT_ANY, FIELD_0_5f, 0x3f);
        soc_reg_field32_modify(unit, ECI_OGER_1033r, REG_PORT_ANY, FIELD_8_8f, 1);
    }
#endif

    
    if(SOC_IS_QAX(unit)) {
        val = 120; 
    } else {
        val = 140;
    }

     
    if (SOC_IS_QUX(unit)  && (pll_type == JER_PLL_TYPE_TS)) {
        val = ts_pll_config[ts_pll_config_idx].ndiv_int;
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_NDIVf, field_above_64);

    if (SOC_IS_QUX(unit) && (pll_type == JER_PLL_TYPE_TS)) {
        val = ts_pll_config[ts_pll_config_idx].ndiv_frac;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_NDIV_FRACf, field_above_64);
    }

    
    if (pll_type == JER_PLL_TYPE_TS) {
        if(SOC_IS_QAX(unit)) {
           val = 6; 
#if defined(ENABLE_TS_PLL_HW_WAR)
           if (SOC_IS_QUX(unit)) {
              if (pll_type == JER_PLL_TYPE_TS) {
                 val = 4; 
              }
           }
#endif
        } else {
           val = 7;
        }
    } else {
        if(SOC_IS_QMX(unit) ||(SOC_INFO(unit).chip_type  == SOC_INFO_CHIP_TYPE_JERICHO) || (SOC_IS_JERICHO_PLUS(unit))) {
            val = 175; 
        } else {
            val = 140;
        }
    }
    SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_CH_0_MDIVf, field_above_64);

    
    if(SOC_IS_QAX(unit)){
        val = 12;

        SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_CH_1_MDIVf, field_above_64);
    }

    
    val = 3;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_KPf, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_KIf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_KAf, field_above_64);

    
    val = 1;
    if (SOC_IS_QUX(unit) && (pll_type == JER_PLL_TYPE_TS)) {
        val = ts_pll_config[ts_pll_config_idx].pdiv;
    }
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PDIVf, field_above_64);

    
    val = 0;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_PWM_RATEf, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_POST_RESET_SELECTf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_VCO_FB_DIV_2f, field_above_64);

    
    val = 2;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_VCO_RANGEf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_LDOf, field_above_64);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PWRON_LDOf, field_above_64);

    
    if (pll_type == JER_PLL_TYPE_TS) {
        soc_reg_above_64_field32_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PLL_CTRL_FREF_SELf, 1);
    }

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

    sal_usleep(1);

    
    val = 1;
    SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
    soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_PWRONf, field_above_64);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

    
    sal_usleep(1000);

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    if (pll_type == JER_PLL_TYPE_BS) {
        soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, BS_PLL_CFG_RESET_Bf, field_above_64);
    } else {
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_RESET_Bf, field_above_64);
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

    
    if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
         soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type-JER_PLL_TYPE_TS], 1);
         if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type-JER_PLL_TYPE_TS])));
         }
    }

    
    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(field_above_64);
    SOC_REG_ABOVE_64_CREATE_MASK(field_above_64, 1, 0);
    if (pll_type == JER_PLL_TYPE_BS) {
        soc_reg_above_64_field_set(unit, ECI_BS_PLL_CONFIGr, reg_above_64, BS_PLL_CFG_POST_RESET_Bf, field_above_64);
    } else {
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_POST_RESET_Bf, field_above_64);
    }
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

#if defined(ENABLE_TS_PLL_HW_WAR)
   if (SOC_IS_QUX(unit)) {
      if (pll_type == JER_PLL_TYPE_TS) {

        sal_usleep(30);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));
        val = 0;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_POST_RESET_Bf, field_above_64);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

        soc_reg_field32_modify(unit, ECI_OGER_1033r, REG_PORT_ANY, FIELD_0_5f, 0x0d);
        soc_reg_field32_modify(unit, ECI_OGER_1033r, REG_PORT_ANY, FIELD_8_8f, 1);

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

        val = 6; 
        SHR_BITCOPY_RANGE(field_above_64, 0, &val, 0, sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_CH_0_MDIVf, field_above_64);

        val = 1;
        SHR_BITCOPY_RANGE(field_above_64,0,&val,0,sizeof(val)*8);
        soc_reg_above_64_field_set(unit, ECI_TS_PLL_CONFIGr, reg_above_64, TS_PLL_CFG_POST_RESET_Bf, field_above_64);

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, eci_srd_pll_config[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, reg_above_64));

        sal_usleep(30);

        
        if (SOC_DPP_CONFIG(unit)->emulation_system == 0) {
             soc_sand_ret=arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, eci_srd_pll_status[pll_type-JER_PLL_TYPE_TS], REG_PORT_ANY, 0, eci_srd_pll_locked[pll_type-JER_PLL_TYPE_TS], 1);
             if(soc_sand_get_error_code_from_error_word(soc_sand_ret) != SOC_SAND_OK) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Resetting TS PLL to 250MHz ...Polling failed for %s\n"), SOC_REG_NAME(unit, eci_srd_pll_status[pll_type-JER_PLL_TYPE_TS])));
             }
        }

      }
   }
#endif

    if (SOC_IS_QMX(unit)) {
        SOC_IF_ERROR_RETURN(READ_ICFG_IPROC_IOPAD_CTRL_8r(unit, &val));
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_8r, &val,IPROC_BS0_CLK_DRIVEf,0x7);
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_8r, &val,IPROC_BS0_HB_DRIVEf,0x7);
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_8r, &val,IPROC_BS0_TC_DRIVEf,0x7);
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_8r, &val,IPROC_BS1_TC_DRIVEf, 0x7);
        WRITE_ICFG_IPROC_IOPAD_CTRL_8r(unit, val);
        SOC_IF_ERROR_RETURN(READ_ICFG_IPROC_IOPAD_CTRL_7r(unit, &val));
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_7r, &val,IPROC_BS1_CLK_DRIVEf,0x7);
        soc_reg_field_set(unit, ICFG_IPROC_IOPAD_CTRL_7r, &val,IPROC_BS1_HB_DRIVEf,0x7);
        WRITE_ICFG_IPROC_IOPAD_CTRL_7r(unit, val);
    }

exit:
    SOCDNX_FUNC_RETURN;
}
STATIC int jer_pll_clk_div_get(int unit,
                                   soc_dcmn_init_serdes_ref_clock_t  ref_clock_in,
                                   soc_dcmn_init_serdes_ref_clock_t  ref_clock_out,
                                   uint32 *ndiv, uint32 *mdiv, uint32 *icp)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (ref_clock_in == soc_dcmn_init_serdes_ref_clock_125) {
        *ndiv = 25;
        *icp = 19;
    } else if (ref_clock_in == soc_dcmn_init_serdes_ref_clock_156_25) {
        *ndiv = 20;
        
        *icp = 24; 
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (ref_clock_out == soc_dcmn_init_serdes_ref_clock_125) {
        *mdiv = 25;
    } else if (ref_clock_out == soc_dcmn_init_serdes_ref_clock_156_25) {
        *mdiv = 20;
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
exit:
    SOCDNX_FUNC_RETURN;

}

STATIC int qax_pll_binding_set(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_DPP_CONFIG(unit)->jer->pll.is_pll_binding_pml[0]) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_NIF_PML_0_PLL_CONFIGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_above_64_val, PML_0_PLL_MSC_CTRL_VEC_REF_SELf, 0);
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_above_64_val, PML_0_PLL_CTRL_VEC_ON_CHIP_REF_CMLf, 0);
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_0_PLL_CONFIGr, reg_above_64_val, PML_0_PLL_CTRL_VEC_ON_CHIP_REF_SELf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_NIF_PML_0_PLL_CONFIGr(unit, reg_above_64_val));
    } 
    if (SOC_DPP_CONFIG(unit)->jer->pll.is_pll_binding_pml[1]) {
        SOCDNX_IF_ERR_EXIT(READ_ECI_NIF_PML_1_PLL_CONFIGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_above_64_val, PML_1_PLL_MSC_CTRL_VEC_REF_SELf, 0);
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_above_64_val, PML_1_PLL_CTRL_VEC_ON_CHIP_REF_CMLf, 1);
        soc_reg_above_64_field32_set(unit, ECI_NIF_PML_1_PLL_CONFIGr, reg_above_64_val, PML_1_PLL_CTRL_VEC_ON_CHIP_REF_SELf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_NIF_PML_1_PLL_CONFIGr(unit, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int jer_pll_2_3_init(
                    int                         unit,
                    JER_PLL_TYPE                pll_type,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_in,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out)
{
    uint32 ndiv, mdiv, icp;
    uint32 pdiv = 1;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(
        jer_pll_clk_div_get(unit, ref_clock_in, ref_clock_out,
                            &ndiv, &mdiv, &icp));
    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(jer_pll_2_set(unit, pll_type, ndiv, mdiv, icp, FALSE));
        SOCDNX_IF_ERR_EXIT(qax_pll_binding_set(unit));
    } else {
        SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, pll_type, ndiv, mdiv, pdiv, FALSE));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int qux_pll_2_init(
                    int                         unit,
                    QUX_NIF_PLL_TYPE            pll_type,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_in,
                    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out)
{
    uint32 ndiv, mdiv, icp;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(
        jer_pll_clk_div_get(unit, ref_clock_in, ref_clock_out,
                            &ndiv, &mdiv, &icp));

    SOCDNX_IF_ERR_EXIT(qux_pll_2_set(unit, pll_type, ndiv, mdiv, icp, FALSE));

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_synce_config_set(int unit, int synce_index, soc_dcmn_init_serdes_ref_clock_t  ref_clock_out, soc_port_t port)
{
    uint32 ndiv = 0, mdiv = 0, pdiv = 0;
    uint32 reg_val;
    int rv;
    uint32 devide_value = 1;
    ARAD_INIT_SYNCE        *info;
    uint32  is_bypass;
    int is_falcon = FALSE, is_eagle = FALSE;
    uint32 first_phy, lane;
    int speed;
    int synce_reg_addr;
    int enable;
    uint32 num_lanes;
    soc_port_if_t interface_type;
    uint32 synce_div = 2, synce_div_val;
    uint32 synce_cfg_val = 0xaa; 
    SOC_JER_NIF_PLL_TYPE jer_pll_type = SOC_JER_NIF_NOF_PLL_TYPE;
    SOC_QUX_NIF_PLL_TYPE qux_pll_type = SOC_QUX_NIF_NOF_PLL_TYPE;

    soc_reg_above_64_val_t reg_val_above_64;
    soc_reg_t synce_cfg_reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    info = &(SOC_DPP_CONFIG(unit)->arad->init.synce);

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_qux_port_pll_type_get(unit, port, &qux_pll_type));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_pll_type_get(unit, port, &jer_pll_type));
    }

    if (IS_SFI_PORT(unit, port)) {
        is_bypass = 1;
        is_falcon = TRUE;
    } else {
        if((jer_pll_type == SOC_JER_NIF_PLL_TYPE_PMH) ||
            (SOC_IS_JERICHO_PLUS_ONLY(unit) && (jer_pll_type == SOC_JER_NIF_PLL_TYPE_PML1))) {
           is_falcon = TRUE;
        } else if (qux_pll_type == SOC_QUX_NIF_PLL_TYPE_PML || !SOC_IS_QUX(unit)){
           is_eagle = TRUE;
        } else if (qux_pll_type == SOC_QUX_NIF_PLL_TYPE_PMX) {
           SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_lanes));
           if (num_lanes == 1) {
               SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_port_enable_get, (unit, port, 0, &enable)));
               if (!enable) {
                   SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG,
                                       (_BSL_SOCDNX_MSG("SYNCE: can't set synce on a disabled single lane viper port. "
                                                        "Please enable the port first.\n")));
               }
           }
        }
        is_bypass = 0;
    }

    if (is_falcon || is_eagle) {
        
        synce_reg_addr = is_falcon ? TSCF_SYNCE_CTRL_REG_ADDR : TSCE_SYNCE_CTRL_REG_ADDR;
#if defined PORTMOD_SUPPORT
        SOCDNX_IF_ERR_EXIT(portmod_port_phy_reg_read(unit, port, -1, 0, synce_reg_addr, &synce_cfg_val));
#endif 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &first_phy));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, first_phy, &lane)));
        synce_div = (synce_cfg_val >> (2 * ((lane - 1) % NUM_OF_LANES_IN_PM))) & 0x3;

        if (2 == synce_div) {
            synce_div_val = 11; 
            
        } else if (1 == synce_div) {
            synce_div_val = 7;
        } else {
            synce_div_val = 1;
        }

        if (synce_div_val == 7) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: unsupported synce_div_val %d\n"), synce_div_val));
        }

        devide_value = 11 / synce_div_val;
    }
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));

    if (!is_bypass) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        if (is_eagle) {
            switch (speed) {
                case 100:
                case 1000:
                case 2500:
                case 10312:
                case 10000:
                    
                    pdiv = 3;
                    ndiv = 200;
                    if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                        mdiv = 25;
                    } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                        mdiv = 20;
                    } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                        mdiv = 125;
                    } else {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    break;
                case 40000:
                    
                    if (interface_type == SOC_PORT_IF_XLAUI) {
                        pdiv = 3;
                        ndiv = 200;
                        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                            mdiv = 25;
                        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                            mdiv = 20;
                        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                            mdiv = 125;
                        } else {
                            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                        }
                        break;
                    }
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: unsupported speed %d for port %d, interface_type %d\n"), speed, port, interface_type));
            }
        } else if (is_falcon){
            switch (speed) {
                case 1000:
                    pdiv = 3;
                    ndiv = 200;
                    if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                        mdiv = 25;
                    } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                        mdiv = 20;
                    } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                        mdiv = 125;
                    } else {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    break;
                case 10000:
                case 10312:
                    
                    pdiv = 1;
                    if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                        mdiv = 24;
                        ndiv = 128;
                    } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                        mdiv = 18;
                        ndiv = 120;
                    } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                        mdiv = 120;
                        ndiv = 128;
                    } else {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    break;
                case 25000:
                case 25781:
                case 100000:
                    
                    pdiv = 3;
                    ndiv = 160;
                    if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                        mdiv = 25;
                    } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                        mdiv = 20;
                    } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                        mdiv = 125;
                    } else {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    break;
                case 40000:
                    
                    if (interface_type == SOC_PORT_IF_XLAUI2) {
                        pdiv = 1;
                        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                            mdiv = 24;
                            ndiv = 64;
                        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                            mdiv = 18;
                            ndiv = 60;
                        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                            mdiv = 120;
                            ndiv = 64;
                        } else {
                            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                        }
                        break;
                    }
     
                    if (interface_type == SOC_PORT_IF_XLAUI) {
                        pdiv = 1; 
                        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                            mdiv = 24;
                            ndiv = 128;
                        } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                            mdiv = 18;
                            ndiv = 120;
                        } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                            mdiv = 120;
                            ndiv = 128;
                        } else {
                            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                        }
                        break;
                    }
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: unsupported speed %d for port %d, interface_type %d\n"), speed, port, interface_type));
            }
        } else {  
            pdiv = 1;
            ndiv = 50;
            if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                mdiv = 25;
            } else if (soc_dcmn_init_serdes_ref_clock_156_25 == ref_clock_out) {
                mdiv = 20;
            } else if (soc_dcmn_init_serdes_ref_clock_25 == ref_clock_out) {
                mdiv = 125;
            } else {
                SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
            }
            switch (speed) {
                case 2500:
                case 10000:
                    
                    devide_value = 5;
                    break;
                case 1000:
                    if (SOC_IS_QUX(unit)) {
                        
                        if (soc_dcmn_init_serdes_ref_clock_125 == ref_clock_out) {
                            is_bypass = 1;
                            devide_value = 1;
                        } else {
                            devide_value = 2;
                        }
                    } else {
                        
                        devide_value = 2;
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: unsupported speed %d for port %d, interface_type %d\n"), speed, port, interface_type));
            }
        }

        
        rv = handle_sand_result(soc_jer_port_synce_clk_sel_set(unit, synce_index, port));

        SOCDNX_IF_ERR_EXIT(rv);

        
        SOCDNX_IF_ERR_EXIT(READ_ECI_GP_CONTROL_9r(unit, reg_val_above_64));
        soc_reg_above_64_field32_set(unit, ECI_GP_CONTROL_9r, reg_val_above_64, PMH_SYNCE_RSTNf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GP_CONTROL_9r(unit, reg_val_above_64));
        
        
        synce_cfg_reg = SOC_IS_QUX(unit) ? NIF_SYNC_ETH_CFGr : NBIH_SYNC_ETH_CFGr;
        SOCDNX_IF_ERR_EXIT(
            soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_index, &reg_val));

        if (devide_value != 1) {
            
            soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, 2);
            soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_DIVIDER_PHASE_ZERO_Nf, ((devide_value+1)/2) - 1);
            soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_DIVIDER_PHASE_ONE_Nf, (devide_value/2) - 1);
        } else {
            soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_CLOCK_DIV_Nf, 1);
        }
        
        soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, 0);
        
        
        
        
        
        
        soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_LINK_VALID_SEL_Nf, 0);
        SOCDNX_IF_ERR_EXIT(
            soc_reg32_set(unit, synce_cfg_reg, REG_PORT_ANY, synce_index, reg_val));
    }

    
    if (SOC_IS_JERICHO_PLUS_ONLY(unit) || SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(jer_plus_pll_set(unit, synce_index, ndiv, mdiv, pdiv, is_bypass));
    } else {
        SOCDNX_IF_ERR_EXIT(jer_pll_3_set(unit, synce_index + JER_PLL_TYPE_SYNCE_0, ndiv, mdiv, pdiv, is_bypass));
    }
    
    if (info->conf[synce_index].squelch_enable) {
        SOCDNX_IF_ERR_EXIT(
            soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_index, &reg_val));
        soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, 1);
        SOCDNX_IF_ERR_EXIT(
            soc_reg32_set(unit, synce_cfg_reg, REG_PORT_ANY, synce_index, reg_val));
    }

exit:

    SOCDNX_FUNC_RETURN;
}

uint32 jer_synce_clk_div_set(int unit, uint32 synce_idx, ARAD_NIF_SYNCE_CLK_DIV   clk_div)
{
    soc_port_t                  port;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_port_synce_clk_sel_get(unit, synce_idx, &port));
    SOCDNX_IF_ERR_EXIT(jer_synce_config_set(unit, synce_idx, (soc_dcmn_init_serdes_ref_clock_t)clk_div, port));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_synce_clk_div_get(int unit, uint32 synce_idx, ARAD_NIF_SYNCE_CLK_DIV*   clk_div)
{
   uint32 mdiv;
   soc_dcmn_init_serdes_ref_clock_t ref_clock_out;
   soc_reg_t reg[] = {ECI_SYNCE_MASTER_PLL_CONFIGr, ECI_SYNCE_SLAVE_PLL_CONFIGr};
   soc_field_t field[] = {SYNCE_MASTER_PLL_CFG_CH_0_MDIVf, SYNCE_SLAVE_PLL_CFG_CH_0_MDIVf};

   SOCDNX_INIT_FUNC_DEFS;

   if (synce_idx > (JER_PLL_TYPE_SYNCE_1 - JER_PLL_TYPE_SYNCE_0)) {
       SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("SYNCE: invalid value for synce_idx %d\n"), synce_idx));
   }

   SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, reg[synce_idx], REG_PORT_ANY, 0, field[synce_idx], &mdiv));
   switch (mdiv) {
   case 22:
   case 20:
   case 18:
       ref_clock_out = soc_dcmn_init_serdes_ref_clock_156_25;
       break;
   case 24:
   case 25:
       ref_clock_out = soc_dcmn_init_serdes_ref_clock_125;
       break;
   case 125:
   case 120:
       ref_clock_out = soc_dcmn_init_serdes_ref_clock_25;
       break;
   default:
       SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_CONFIG);
   }

   *clk_div = (soc_dcmn_init_serdes_ref_clock_t)ref_clock_out;
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 jer_synce_clk_port_sel_set(int unit, uint32 synce_idx, soc_port_t port)
{
   soc_dcmn_init_serdes_ref_clock_t  ref_clock_out;
   ARAD_NIF_SYNCE_CLK_DIV synce_div;
   int rv;

   SOCDNX_INIT_FUNC_DEFS;

   rv = jer_synce_clk_div_get(unit, synce_idx, &synce_div);
   if(rv != SOC_E_NONE) {
       
       ref_clock_out = soc_dcmn_init_serdes_ref_clock_156_25;
   } else {
       ref_clock_out = (ARAD_NIF_SYNCE_CLK_DIV)synce_div;
   }
   SOCDNX_IF_ERR_EXIT(jer_synce_config_set(unit, synce_idx, ref_clock_out, port));

exit:
   SOCDNX_FUNC_RETURN;
}

uint32 jer_synce_clk_squelch_set(int unit, uint32 synce_idx, int enable)
{
   uint32 reg_val;
   soc_reg_t synce_cfg_reg = INVALIDr;

   SOCDNX_INIT_FUNC_DEFS;
   
   synce_cfg_reg = SOC_IS_QUX(unit) ? NIF_SYNC_ETH_CFGr : NBIH_SYNC_ETH_CFGr;
   SOCDNX_IF_ERR_EXIT(
        soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_idx, &reg_val));

   soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_SQUELCH_EN_Nf, enable ? 1 : 0);
   
   
   
   
   
   
   soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_LINK_VALID_SEL_Nf, enable ? 1 : 0);
   SOCDNX_IF_ERR_EXIT(
        soc_reg32_set(unit, synce_cfg_reg, REG_PORT_ANY, synce_idx, reg_val));

   SOCDNX_IF_ERR_EXIT(
        soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_idx, &reg_val));

exit:
   SOCDNX_FUNC_RETURN;
}

uint32 jer_synce_clk_squelch_get(int unit, uint32 synce_idx, int *enable)
{
   uint32 reg_val;
   soc_reg_t synce_cfg_reg = INVALIDr;

   SOCDNX_INIT_FUNC_DEFS;
   
   synce_cfg_reg = SOC_IS_QUX(unit) ? NIF_SYNC_ETH_CFGr : NBIH_SYNC_ETH_CFGr;
   SOCDNX_IF_ERR_EXIT(
        soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_idx, &reg_val));

   
   *enable = soc_reg_field_get(unit, synce_cfg_reg, reg_val, SYNC_ETH_SQUELCH_EN_Nf);

exit:
   SOCDNX_FUNC_RETURN;
}


int jer_synce_init(int unit)
{
    uint32 reg_val;
    int synce_index;
    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out;
    soc_port_t port;
    ARAD_INIT_SYNCE        *info;
    int is_master;

    SOCDNX_INIT_FUNC_DEFS;

    info = &(SOC_DPP_CONFIG(unit)->arad->init.synce);

    for (synce_index = 0; synce_index <= JER_PLL_TYPE_SYNCE_1 - JER_PLL_TYPE_SYNCE_0; synce_index++) {
        if (info->conf[synce_index].enable) {
            ref_clock_out = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_synce_out[synce_index];
            port = info->conf[synce_index].port_id;
            if (!IS_SFI_PORT(unit, port))
            {
                SOCDNX_IF_ERR_EXIT(jer_synce_config_set(unit, synce_index, ref_clock_out, port));
                if (!SOC_IS_QUX(unit)) {
                    is_master = synce_index? 0 : 1;
                    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, is_master, 0));
                }
            }
        }
    }

    
    reg_val = 0x0;
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_OE_Nf, 0x0 );
    soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_OE_Nf, 0x0 );

    if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_DIFF_CLK) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x0 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x1 );
    } else if (info->mode == ARAD_NIF_SYNCE_MODE_TWO_CLK_AND_VALID) {
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_0_SELECTf, 0x0 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_1_SELECTf, 0x4 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_2_SELECTf, 0x1 );
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, SYNC_ETH_PAD_3_SELECTf, 0x5 );
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


static int flair_pll_init(int unit,
    soc_dcmn_init_serdes_ref_clock_t  ref_clock_in,
    soc_dcmn_init_serdes_ref_clock_t  ref_clock_out)
{
    uint32 value;
    uint32 ndiv, mdiv, icp;
    soc_reg_above_64_val_t reg_above64_val; 

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(
        jer_pll_clk_div_get(unit, ref_clock_in, ref_clock_out,
                            &ndiv, &mdiv, &icp));

    if (!SAL_BOOT_PLISIM) {
        SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
        value = 0x8460000 | (ndiv & 0x3F); 
        SHR_BITCOPY_RANGE(reg_above64_val, 0, &value, 0, 32);
        value = 0x20100000;
        SHR_BITCOPY_RANGE(reg_above64_val, 32, &value, 0, 32);
        value = 0x00810018;
        SHR_BITCOPY_RANGE(reg_above64_val, 64, &value, 0, 32);
        value = 0x60000004 | ((mdiv & 0xFF) << 3);  
        SHR_BITCOPY_RANGE(reg_above64_val, 96, &value, 0, 32);
        value = 0x1058041 | ((mdiv & 0xFF) << 7);  
        SHR_BITCOPY_RANGE(reg_above64_val, 128, &value, 0, 32);
        SOCDNX_IF_ERR_EXIT(WRITE_PLL_MISC_PLL_0_CONFIGr(unit, reg_above64_val));
        sal_usleep(10);

        SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
        value = 0x8460000 | (ndiv & 0x3F); 
        SHR_BITCOPY_RANGE(reg_above64_val, 0, &value, 0, 32);
        value = 0x20100000;
        SHR_BITCOPY_RANGE(reg_above64_val, 32, &value, 0, 32);
        value = 0x00810018;
        SHR_BITCOPY_RANGE(reg_above64_val, 64, &value, 0, 32);
        value = 0x60000004 | ((mdiv & 0xFF) << 3);  
        SHR_BITCOPY_RANGE(reg_above64_val, 96, &value, 0, 32);
        value = 0x3058041 | ((mdiv & 0xFF) << 7);  
        SHR_BITCOPY_RANGE(reg_above64_val, 128, &value, 0, 32);
        SOCDNX_IF_ERR_EXIT(WRITE_PLL_MISC_PLL_0_CONFIGr(unit, reg_above64_val));
        sal_usleep(10);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_pll_init(int unit)
{
    soc_jer_pll_config_t        pll;
    SOCDNX_INIT_FUNC_DEFS;

    pll = SOC_DPP_CONFIG(unit)->jer->pll;

    if (SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(flair_pll_init(unit, pll.ref_clk_fabric_in[0], pll.ref_clk_fabric_out[0]));
        SOC_EXIT;
    }

    if (SOC_IS_QUX(unit)) {
        if (pll.ref_clk_pml_in[0] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE && pll.ref_clk_pml_out[0] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(qux_pll_2_init(unit, QUX_PLL_TYPE_NIF_PML, pll.ref_clk_pml_in[0], pll.ref_clk_pml_out[0]));
        }
        if (pll.ref_clk_pmx_in != ARAD_INIT_SERDES_REF_CLOCK_DISABLE && pll.ref_clk_pmx_out != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(qux_pll_2_init(unit, QUX_PLL_TYPE_NIF_PMX, pll.ref_clk_pmx_in, pll.ref_clk_pmx_out));
        }
    } else {
        if (pll.ref_clk_pml_in[0] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE && pll.ref_clk_pml_out[0] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(jer_pll_2_3_init(unit, JER_PLL_TYPE_NIF_PML_0, pll.ref_clk_pml_in[0], pll.ref_clk_pml_out[0]));
        }
        if (pll.ref_clk_pml_in[1] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE && pll.ref_clk_pml_out[1] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(jer_pll_2_3_init(unit, JER_PLL_TYPE_NIF_PML_1, pll.ref_clk_pml_in[1], pll.ref_clk_pml_out[1]));
        }
        if (pll.ref_clk_pmh_in != ARAD_INIT_SERDES_REF_CLOCK_DISABLE && pll.ref_clk_pmh_out != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(jer_pll_2_3_init(unit, JER_PLL_TYPE_NIF_PMH, pll.ref_clk_pmh_in, pll.ref_clk_pmh_out));
        }
    }
    
    if (SOC_DPP_CONFIG(unit)->arad->init.pll.ts_clk_mode == 0x1) {
        SOCDNX_IF_ERR_EXIT(jer_pll_1_set(unit, JER_PLL_TYPE_TS));
    }
    if (!SOC_IS_QAX(unit)) {
        if (pll.ref_clk_fabric_in[0] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE) {
            SOCDNX_IF_ERR_EXIT(jer_pll_2_3_init(unit, JER_PLL_TYPE_FABRIC_0, pll.ref_clk_fabric_in[0], pll.ref_clk_fabric_out[0]));
        }
        if (pll.ref_clk_fabric_in[1] != ARAD_INIT_SERDES_REF_CLOCK_DISABLE)
        {
            SOCDNX_IF_ERR_EXIT(jer_pll_2_3_init(unit, JER_PLL_TYPE_FABRIC_1, pll.ref_clk_fabric_in[1], pll.ref_clk_fabric_out[1]));
        }
        
        if (SOC_DPP_CONFIG(unit)->arad->init.pll.bs_clk_mode == 0x1) {
            SOCDNX_IF_ERR_EXIT(jer_pll_1_set(unit, JER_PLL_TYPE_BS));
        }

    }else {
        
        if (SOC_DPP_CONFIG(unit)->arad->init.pll.bs_clk_mode == 0x1) {
            SOCDNX_IF_ERR_EXIT(jer_pll_4_set(unit, QAX_PLL_TYPE_BSPLL1));
            sal_usleep(30);
            SOCDNX_IF_ERR_EXIT(jer_pll_4_set(unit, QAX_PLL_TYPE_BSPLL0));

        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}
