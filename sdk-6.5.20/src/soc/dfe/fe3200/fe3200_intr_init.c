
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement soc interrupt handler.
 */


#include <shared/bsl.h>

#include <sal/core/dpc.h>

#include <soc/intr.h>
#include <soc/ipoll.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>

#include <soc/dfe/fe3200/fe3200_intr_cb_func.h>
#include <soc/dfe/fe3200/fe3200_intr_corr_act_func.h>
#include <soc/dfe/fe3200/fe3200_intr.h>
#include <soc/dfe/fe3200/fe3200_defs.h>

#include <soc/dcmn/dcmn_intr_handler.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/drv.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR


static 
soc_reg_t soc_fe3200_interrupt_monitor_mem_reg[] = {
    BRDC_CCS_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_DCH_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_DCH_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_DCL_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_DCL_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_DCM_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_DCM_ECC_ERR_2B_MONITOR_MEM_MASKr,
    ECI_ECC_ERR_1B_MONITOR_MEM_MASKr,
    ECI_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_FSRD_ECC_ERR_1B_MONITOR_MEM_MASKr,
    BRDC_FSRD_ECC_ERR_2B_MONITOR_MEM_MASKr,
    BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr,
    RTP_ECC_ERR_1B_MONITOR_MEM_MASKr,
    RTP_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_DRHPA_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_DRHPB_ECC_ERR_2B_MONITOR_MEM_MASKr,
    RTP_DRHPA_ECC_ERR_1B_MONITOR_MEM_MASKr,
    RTP_DRHPB_ECC_ERR_1B_MONITOR_MEM_MASKr,
    INVALIDr
};



int 
soc_fe3200_interrupt_all_enable_set(int unit, int enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (enable) {
        soc_cmicm_intr3_enable(unit, 0xffffffff);
        soc_cmicm_intr4_enable(unit, 0xffffffff);
        soc_cmicm_intr5_enable(unit, 0xffffffff);
    } else {
        soc_cmicm_intr3_disable(unit, 0xffffffff);
        soc_cmicm_intr4_disable(unit, 0xffffffff);
        soc_cmicm_intr5_disable(unit, 0xffffffff);
    }

    SOCDNX_FUNC_RETURN;
}

int 
soc_fe3200_interrupt_all_enable_get(int unit, int *enable)
{
    int rc;
    int mask;
    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_interrupt_is_all_mask(unit, &mask);
    SOCDNX_IF_ERR_EXIT(rc);

    *enable = mask ? 0 : 1;
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_fe3200_ser_init(int unit)
{
    int idx, array_index, array_index_max = 1;
    int rc;
    soc_reg_above_64_val_t above_64;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_REG_ABOVE_64_ALLONES(above_64);
    for(idx=0; soc_fe3200_interrupt_monitor_mem_reg[idx] != INVALIDr; idx++) {

        if (soc_fe3200_interrupt_monitor_mem_reg[idx] == BRDC_FSRD_ECC_ERR_1B_MONITOR_MEM_MASKr ||
            soc_fe3200_interrupt_monitor_mem_reg[idx] == BRDC_FSRD_ECC_ERR_2B_MONITOR_MEM_MASKr ||
            soc_fe3200_interrupt_monitor_mem_reg[idx] == BRDC_FSRD_WC_UC_MEM_MASK_BITMAPr)
        {
            continue;
        }

        if (SOC_REG_IS_ARRAY(unit, soc_fe3200_interrupt_monitor_mem_reg[idx])) {
             array_index_max = SOC_REG_NUMELS(unit, soc_fe3200_interrupt_monitor_mem_reg[idx]);
        }

        for (array_index = 0; array_index < array_index_max; array_index++) {
            rc = soc_reg_above_64_set(unit, soc_fe3200_interrupt_monitor_mem_reg[idx], 0, array_index, above_64);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_fe3200_interrupts_disable(int unit)
{
    int rc;
    int i, copy_no;
    soc_interrupt_db_t* interrupts;
    soc_block_types_t  block;
    soc_reg_t reg;
    int blk;
    int nof_interrupts;
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    rc = soc_fe3200_nof_interrupts(unit, &nof_interrupts);
    SOCDNX_IF_ERR_EXIT(rc);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        
        SOC_REG_ABOVE_64_CLEAR(data);
        for (i = 0; i < SOC_FE3200_NOF_BLK; i++) {
            if (SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg != INVALIDr) {
                rc = soc_reg_above_64_set(unit, SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[i].int_mask_reg, 0,  0, data); 
                SOCDNX_IF_ERR_EXIT(rc);
            }
        }
        
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i=0 ; i < nof_interrupts; i++) { 
            reg = interrupts[i].reg;
            
            if (!SOC_REG_IS_VALID(unit, reg))
            {
               continue;
            }
            block = SOC_REG_INFO(unit, reg).block;
            SOC_REG_ABOVE_64_CLEAR(data);
            SOC_BLOCKS_ITER(unit, blk, block) {
                copy_no = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                if (interrupts[i].vector_info) {
                    rc = soc_reg_above_64_set(unit, interrupts[i].vector_info->int_mask_reg, copy_no,  0, data);
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_fe3200_interrupts_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

     
    soc_fe3200_interrupt_all_enable_set(unit, 0);

    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        if (soc_ipoll_disconnect(unit) < 0) {
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U(unit,
                                  "error disconnecting polled interrupt mode\n")));
        }
        SOC_CONTROL(unit)->soc_flags &= ~SOC_F_POLLED;
    }

    
    if (soc_cm_interrupt_disconnect(unit) < 0) {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit,
                                    "could not disconnect interrupt line\n")));
    }

    if (dcmn_intr_handler_deinit(unit) < 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error at interrupt db deinitialization")));
    }
           
exit:               
    SOCDNX_FUNC_RETURN;
}

int soc_fe3200_interrupts_init(int unit)
{
    int cmc;
    int rc;

    SOCDNX_INIT_FUNC_DEFS;
    
    sal_dpc_enable(INT_TO_PTR(unit));

    cmc = SOC_PCI_CMC(unit);
     
     
    soc_fe3200_interrupt_all_enable_set(unit, 0);

    if (!SAL_BOOT_NO_INTERRUPTS) {

        
        if (dcmn_intr_handler_init(unit) < 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
        }
     
             
        if (!SOC_WARM_BOOT(unit))
        {
            rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_STORM_TIMED_PERIOD, 0);
            SOCDNX_IF_ERR_EXIT(rc);
            rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_STORM_TIMED_COUNT, 0);
            SOCDNX_IF_ERR_EXIT(rc);
            rc = SOC_DFE_WARM_BOOT_ARR_MEMSET(unit, INTR_FLAGS, 0);
            SOCDNX_IF_ERR_EXIT(rc);
        }
             
        
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            SOC_CONTROL(unit)->soc_flags |= SOC_F_POLLED;
        } else {
            if (soc_cm_interrupt_connect(unit, soc_cmicm_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }    

        if (!SOC_WARM_BOOT(unit)) {
            uint32 rval;
            uint32 enable_msi = 1;

            if (soc_feature(unit, soc_feature_iproc) &&
                 (soc_cm_get_bus_type(unit) & SOC_DEV_BUS_MSI) == 0) {
               
                enable_msi = 0;
            }

            rval = soc_pci_read(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc));
            soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr, &rval, ENABLE_MSIf, enable_msi);
            soc_pci_write(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc), rval);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
