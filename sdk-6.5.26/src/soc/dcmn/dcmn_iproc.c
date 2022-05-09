/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN IPROC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif 

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef BCM_IPROC_SUPPORT
    #include <soc/iproc.h>
#endif 

#include <soc/drv.h>
#include <soc/dcmn/dcmn_iproc.h>
#include <soc/dcmn/error.h>

#ifdef BCM_IPROC_SUPPORT


int soc_dcmn_iproc_config_paxb(int unit, int do_change_mem) 
{
    uint32 reg_val, orig_reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (do_change_mem && 
        soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "change_mem_mode", 1) != 0) {

        SOCDNX_IF_ERR_EXIT(READ_ICFG_PCIE_0_TL_TO_DL_BUF_MEM_CTRLr(unit, &reg_val));
        orig_reg_val = reg_val;
        soc_reg_field_set(unit, ICFG_PCIE_0_TL_TO_DL_BUF_MEM_CTRLr, &reg_val, PCIE_0_TL_TO_DL_BUF_PORTA_TM_OVERRIDEf, 1);
        soc_reg_field_set(unit, ICFG_PCIE_0_TL_TO_DL_BUF_MEM_CTRLr, &reg_val, PCIE_0_TL_TO_DL_BUF_PORTA_TMf, 4);
        if (orig_reg_val != reg_val) {
            SOCDNX_IF_ERR_EXIT(WRITE_ICFG_PCIE_0_TL_TO_DL_BUF_MEM_CTRLr(unit, reg_val));
        }

        SOCDNX_IF_ERR_EXIT(READ_ICFG_PCIE_0_DL_TO_TL_BUF_MEM_CTRLr(unit, &reg_val));
        orig_reg_val = reg_val;
        soc_reg_field_set(unit, ICFG_PCIE_0_DL_TO_TL_BUF_MEM_CTRLr, &reg_val, PCIE_0_DL_TO_TL_BUF_PORTA_TM_OVERRIDEf, 1);
        soc_reg_field_set(unit, ICFG_PCIE_0_DL_TO_TL_BUF_MEM_CTRLr, &reg_val, PCIE_0_DL_TO_TL_BUF_PORTA_TMf, 4);
        if (orig_reg_val != reg_val) {
            SOCDNX_IF_ERR_EXIT(WRITE_ICFG_PCIE_0_DL_TO_TL_BUF_MEM_CTRLr(unit, reg_val));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 
