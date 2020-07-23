/*
 * 

 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <sal/core/dpc.h>

#include <soc/error.h>
#include <soc/ipoll.h>
#include <soc/drv.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h> 


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR


int soc_fe1600_interrupt_deinit(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    sal_dpc_disable_and_wait(INT_TO_PTR(unit));

     
    if (soc_fe1600_interrupt_all_enable_set(unit, 0)) {
        LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "error disabling interrupts\n")));
    }

    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "error disconnecting polled interrupt mode\n")));
            }
        } else {
            
            if (soc_cm_interrupt_disconnect(unit) < 0) {
                LOG_ERROR(BSL_LS_SOC_INIT,
                          (BSL_META_U(unit,
                                      "could not disconnect interrupt line\n")));
            }
        }
    }



    if(!SAL_BOOT_NO_INTERRUPTS) 
    {
        fe1600_interrupts_dfe_control_data_deinit(unit);
        
        fe1600_interrupts_array_deinit(unit);       
    }

    SOCDNX_FUNC_RETURN;
}

int soc_fe1600_interrupt_init(int unit) 
{
    int rv;
    soc_control_t* soc;

    SOCDNX_INIT_FUNC_DEFS;
    
    sal_dpc_enable(INT_TO_PTR(unit));

    soc = SOC_CONTROL(unit);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        rv = fe1600_interrupts_array_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        rv = fe1600_interrupts_dfe_control_data_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);
    }
 

    
    

    soc_intr_block_lo_disable(unit, 0xFFFFFFFF);
    soc_intr_block_hi_disable(unit, 0xFFFFFFFF);

    if(!SAL_BOOT_NO_INTERRUPTS) {
        if (soc_property_get(unit, spn_POLLED_IRQ_MODE, 1)) {
            if (soc_ipoll_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("error initializing polled interrupt mode")));
            }
            soc->soc_flags |= SOC_F_POLLED;
        } else {
            
            if (soc_cm_interrupt_connect(unit, dfe_local_soc_intr, INT_TO_PTR(unit)) < 0) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("could not connect interrupt line")));
            }
        }
    }
exit:
    if (SOCDNX_FUNC_ERROR)
    {
        soc_fe1600_interrupt_deinit(unit);
    }
    SOCDNX_FUNC_RETURN;
}

