/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/port.h>

#include <soc/types.h>
#include <soc/error.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

cprimod_interrupt_callback_entry_t cpri_intr_cbs[SOC_MAX_NUM_DEVICES][_shrCpriIntrCount]  = {{{NULL,NULL}}};
sal_mutex_t cprimod_lock[SOC_MAX_NUM_DEVICES] = {NULL};

int cprimod_port_cpm_type_get(int unit, int port,  cprimod_dispatch_type_t* type){

    SOC_INIT_FUNC_DEFS;
#ifdef CPRIMOD_CPRI_FALCON_SUPPORT
    *type = cprimodDispatchTypeCprif; 
#else
    *type = cprimodDispatchTypeCount;
#endif
    SOC_FUNC_RETURN;
}

int cprimod_init(int unit)
{
    SOC_INIT_FUNC_DEFS;

    cprimod_lock[unit] = sal_mutex_create("CPRIMOD_LOCK");

    if (cprimod_lock[unit] == NULL) {
        LOG_ERROR(BSL_LS_BCM_CPRI,
                  (BSL_META_U(unit,
                              "Failed to create mutex for CPRIMOD.\n")));
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE,(" "));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprimod_deinit(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (cprimod_lock[unit] == NULL) {
        sal_mutex_destroy(cprimod_lock[unit]);
    }
    SOC_FUNC_RETURN;
}

int cprimod_mutex_take(int unit)
{
    SOC_INIT_FUNC_DEFS;
    if (cprimod_lock[unit] != NULL){
        sal_mutex_take(cprimod_lock[unit], sal_mutex_FOREVER);
    }
    SOC_FUNC_RETURN;
}

int cprimod_mutex_give(int unit)
{
    SOC_INIT_FUNC_DEFS;
    if (cprimod_lock[unit] != NULL){
        sal_mutex_give(cprimod_lock[unit]);
    }
    SOC_FUNC_RETURN;
}

int cprimod_cpri_cpm_interrupt_data_init(int unit)
{
    int interrupt;
    SOC_INIT_FUNC_DEFS;

    for (interrupt = 0; interrupt < _shrCpriIntrCount; interrupt++){
       _SOC_IF_ERR_EXIT
            (cprimod_interrupt_callback_entry_t_init(&cpri_intr_cbs[unit][interrupt]));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprimod_cpri_port_interrupt_callback_register(int unit, cprimod_cpri_port_intr_type_t cpri_intr_type, cprimod_cpri_port_interrupt_callback_t callback, void* cb_data)
{
    SOC_INIT_FUNC_DEFS;

    if (cpri_intr_type >= _shrCpriIntrCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unknow interrupt type."));
    }

    if (cpri_intr_cbs[unit][cpri_intr_type].cb_func == NULL) {
        cpri_intr_cbs[unit][cpri_intr_type].cb_func = callback;
        cpri_intr_cbs[unit][cpri_intr_type].cb_data = cb_data;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Callback already registered"));
    }

exit:
    SOC_FUNC_RETURN;

}


int cprimod_cpri_port_interrupt_callback_unregister(int unit, cprimod_cpri_port_intr_type_t cpri_intr_type, cprimod_cpri_port_interrupt_callback_t callback)
{
    SOC_INIT_FUNC_DEFS;

    if (cpri_intr_type >= _shrCpriIntrCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unknow interrupt type."));
    }

    if (cpri_intr_cbs[unit][cpri_intr_type].cb_func != NULL) {
        cpri_intr_cbs[unit][cpri_intr_type].cb_func = NULL;
        cpri_intr_cbs[unit][cpri_intr_type].cb_data = NULL;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Callback not registered"));
    }

exit:
    SOC_FUNC_RETURN;

}



#undef _ERR_MSG_MODULE_NAME
