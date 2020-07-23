
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

/* 
 *  include  
 */ 
#include <shared/bsl.h>
#include <sal/core/time.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#include <soc/dfe/fe1600/fe1600_port.h>
#include <bcm_int/dfe_dispatch.h>
#include <appl/dfe/interrupts/interrupt_handler.h>
#include <appl/dfe/interrupts/interrupt_handler_cb_func.h>
#include <appl/dfe/interrupts/interrupt_handler_corr_act_func.h>
#endif /* BCM_DFE_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <appl/dpp/interrupts/interrupt_handler.h>
#include <appl/dpp/interrupts/interrupt_handler_cb_func.h>
#include <appl/dpp/interrupts/interrupt_handler_corr_act_func.h>
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_SAND_SUPPORT)
#include <soc/dcmn/dcmn_intr_handler.h>
#endif


#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>

/*
 *  Interrupt Data Semaphore time out defines
 *      the time out of the deinit interrupts is long because some call-back.
 */
#define APPL_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT 20000000
#define APPL_INTR_DATA_SEM_TAKE_TO_CB  10000

interrupt_handle_data_base_t interrupt_data_base[SOC_MAX_NUM_DEVICES];
interrupt_common_params_t interrupt_common_params[SOC_MAX_NUM_DEVICES];

int
interrupt_get_nof_block_instances(int unit, int inter_id, int *nof_block_instances)
{
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;
    soc_block_types_t block_types;

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        return SOC_E_FAIL;
    }

    /* verify interrupt_id */
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((inter_id > nof_interrupts) || inter_id < 0) {
        return SOC_E_FAIL;
    }

    inter = &(interrupts[inter_id]);
    if (!SOC_REG_IS_VALID(unit, inter->reg)) {
        return SOC_E_FAIL;
    }

    block_types = SOC_REG_INFO(unit, inter->reg).block;
    return interrupt_common_params[unit].interrupt_handle_block_instance(unit, block_types, nof_block_instances);
}

int
interrupt_add_interrupt_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    handle_interrupt_func inter_action,
    handle_interrupt_func inter_recurring_action)
{
    int nof_block_instances;
    int index, rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = interrupt_get_nof_block_instances(unit, en_inter, &nof_block_instances);
    if (rv != BCM_E_NONE) {
        BCM_EXIT;
    }

    for (index = 0; index < nof_block_instances; index++) {
        interrupt_data_base[unit].recurring_action_cycle_counting[en_inter].block_instance_data[index] = occurrences;
        interrupt_data_base[unit].recurring_action_cycle_time[en_inter].block_instance_data[index] = timeCycle;
    }

    interrupt_data_base[unit].func_arr[en_inter] = inter_action;
    interrupt_data_base[unit].func_arr_recurring_action[en_inter] = inter_recurring_action;

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_config_interrupt_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    handle_interrupt_func inter_action,
    handle_interrupt_func inter_recurring_action)
{
    int nof_block_instances;
    int index, rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv = interrupt_get_nof_block_instances(unit, en_inter, &nof_block_instances);
    if (rv != BCM_E_NONE) {
        BCM_EXIT;
    }

    if(-1 != occurrences) {
        for (index = 0; index < nof_block_instances; index++) {
            interrupt_data_base[unit].recurring_action_cycle_counting[en_inter].block_instance_data[index] = occurrences;
        }
    }

    if(-1 != timeCycle) {
        for (index = 0; index < nof_block_instances; index++) {
            interrupt_data_base[unit].recurring_action_cycle_time[en_inter].block_instance_data[index] = timeCycle;
        }
    }

    if(NULL != inter_action) {
        interrupt_data_base[unit].func_arr[en_inter] = inter_action;
    }
    if(NULL != inter_recurring_action) {
        interrupt_data_base[unit].func_arr_recurring_action[en_inter] = inter_recurring_action;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
handle_interrupt_recurring_detect(int unit, int block_instance, uint32 en_interrupt) 
{
    int currentTime;
    int cycleTime, *startCountingTime;
    int cycleCount, *counter;

    SOCDNX_INIT_FUNC_DEFS;

    currentTime = sal_time(); /*seconds*/

    cycleTime = (interrupt_data_base[unit].recurring_action_cycle_time[en_interrupt].block_instance_data[block_instance]);
    startCountingTime = &(interrupt_data_base[unit].recurring_action_time[en_interrupt].block_instance_data[block_instance]);

    cycleCount = (interrupt_data_base[unit].recurring_action_cycle_counting[en_interrupt].block_instance_data[block_instance]);
    counter  = &(interrupt_data_base[unit].recurring_action_counters[en_interrupt].block_instance_data[block_instance]);

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            "%s: en_interrupt=%d, currentTime=%d, cycleTime=%d, *startCountingTime=%d, cycleCount=%d, *counter=%d.\n"), FUNCTION_NAME(), en_interrupt, currentTime, cycleTime, *startCountingTime, cycleCount, *counter));

    if (((cycleTime) <= 0) || ((cycleCount) <=0)) {
        return 0;
    }

    if (1 == cycleCount) {
        return 1;
    }

    if (currentTime - *startCountingTime > cycleTime) {
        /*init recurring counting*/
        *startCountingTime = currentTime;
        *counter = 1;
        return 0;
    }

    (*counter)++;
    if (*counter >= cycleCount) {
        (*counter) = 0;
        return 1;
    }

    SOCDNX_FUNC_RETURN;
}

void
diag_switch_event_cb(
    int unit, 
    bcm_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata)
{
    int interrupt_id       = arg1;
    int block_instance  = arg2;
    soc_interrupt_db_t* interrupt;

    uint32 flags;

    int is_storm_count_period, is_storm_nominal;
    bcm_switch_event_control_t event_bcm_switch_event_mask, event_bcm_switch_event_clear; /*will be cfg after verifying interrupt_id*/
    int rc;
    int inter_get;

    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_REFERENCE(_rv);

    /*when arg3 isn't 0, means this is a reset user callback event generated by interrupt handler.*/
    if(arg3 != 0) {
        return;
    }

    /*handle only interrupts*/
    if(BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        return;
    }

    /*
     * Take the interrupt Data semaphore
     */ 
    if(interrupt_data_base[unit].interrupts_data_sem != NULL) {
        if( sal_sem_take(interrupt_data_base[unit].interrupts_data_sem, APPL_INTR_DATA_SEM_TAKE_TO_CB) != 0) {
            LOG_ERROR(BSL_LS_BCM_INTR,
                      (BSL_META_U(unit,
                                  "Failed to take the interrupt data Semaphore")));
                       return;
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_INTR,
                  (BSL_META_U(unit,
                              "Interrupt data is uninitialized")));
                   return;
    }

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOC_MSG("invalid unit")));
    }
    
    if(interrupt_id >= interrupt_common_params[unit].nof_interrupts || interrupt_id < 0){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);
    if(NULL == interrupt) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_BCM_MSG("No interrupts for device")));
    }

    /*set mask event*/
    event_bcm_switch_event_mask.action = bcmSwitchEventMask;
    event_bcm_switch_event_mask.event_id = interrupt_id;
    event_bcm_switch_event_mask.index = block_instance;
    /*mask*/
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_mask, 1); 
    SOCDNX_IF_ERR_EXIT(rc);

    /*storm detection*/
    rc = soc_interrupt_update_storm_detection(unit, block_instance, interrupt);
    SOCDNX_IF_ERR_EXIT(rc);
    rc = soc_interrupt_is_storm(unit, block_instance, interrupt, &is_storm_count_period, &is_storm_nominal);
    SOCDNX_IF_ERR_EXIT(rc);
    if (is_storm_count_period || is_storm_nominal) {
        /* Storm detected: keep the interrupt masked and return from CB */
        rc = dcmn_interrupt_print_info(unit, block_instance, interrupt_id, 0, DCMN_INT_CORR_ACT_NONE, "Storm Detected");
        SOCDNX_IF_ERR_EXIT(rc);
    }

    /* update statistics */
    rc = soc_interrupt_stat_cnt_increase(unit, block_instance, interrupt_id);
    SOCDNX_IF_ERR_EXIT(rc);

    /*interrupt_action*/
    if (handle_interrupt_recurring_detect(unit, block_instance, interrupt_id)) {
        /*recurring action*/
        rc = interrupt_data_base[unit].func_arr_recurring_action[interrupt_id](unit, block_instance, interrupt_id, NULL);
        SOCDNX_IF_ERR_EXIT(rc);
    } else {
        rc = interrupt_data_base[unit].func_arr[interrupt_id](unit, block_instance, interrupt_id, NULL);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    /*check if should unmask interrupt after handled*/
    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SOCDNX_IF_ERR_EXIT(rc);
    if (flags & SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE) {
        SOC_EXIT;
    }

    /*set clear event*/
    event_bcm_switch_event_clear.event_id = interrupt_id;
    event_bcm_switch_event_clear.index = block_instance;

    event_bcm_switch_event_clear.action = bcmSwitchEventClear;
    rc = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_clear, 1);
    if(rc!= SOC_E_UNAVAIL) {
        SOCDNX_IF_ERR_EXIT(rc);
    }


    rc = soc_interrupt_get(unit, block_instance, interrupt, &inter_get);
    SOCDNX_IF_ERR_EXIT(rc);

    if (inter_get == 0x0 || (flags & SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK)) {
         if (!(is_storm_count_period || is_storm_nominal)) {
             rc = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_mask, 0); 
             SOCDNX_IF_ERR_EXIT(rc);
         }
    } else {
        rc = dcmn_interrupt_print_info(unit, block_instance, interrupt_id, 0, DCMN_INT_CORR_ACT_NONE, "Interrupt clear Failed!!! Masked");
        SOCDNX_IF_ERR_EXIT(rc);
    }

exit:
    if(SOC_UNIT_VALID(unit)) {
        sal_sem_give(interrupt_data_base[unit].interrupts_data_sem);
    }
    return;
}

int
interrupt_handler_data_base_init(int unit)
{
    uint32 index;
    int cur_time;
    handle_interrupt_func interrupt_handle_genenric_none, interrupt_recurring_action_handle_generic_none;
    uint32 nof_interrupts;
    int rc;
    int nof_block_instances;
    interrupt_handle_block_data_t *recurring_action_counters;
    interrupt_handle_block_data_t *recurring_action_time;
    interrupt_handle_block_data_t *recurring_action_cycle_time;
    interrupt_handle_block_data_t *recurring_action_cycle_counting;

    SOCDNX_INIT_FUNC_DEFS;

    interrupt_data_base[unit].interrupts_data_sem = sal_sem_create("Interrupt appl data Sem", sal_sem_BINARY, 0);   
    if(interrupt_data_base[unit].interrupts_data_sem == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOC_MSG("Failed to create the Interrupt Data Semaphore")));
    }
 
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_FE1600(unit)) {
        interrupt_handle_genenric_none = fe1600_interrupt_handle_generic_none;
        interrupt_recurring_action_handle_generic_none = fe1600_interrupt_recurring_action_handle_generic_none;
    } else
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)){
        interrupt_handle_genenric_none = arad_interrupt_handle_generic_none;
        interrupt_recurring_action_handle_generic_none = arad_interrupt_recurring_action_handle_generic_none;
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    nof_interrupts = interrupt_common_params[unit].nof_interrupts;

    interrupt_data_base[unit].func_arr = sal_alloc( sizeof(handle_interrupt_func) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].func_arr_recurring_action =sal_alloc( sizeof(handle_interrupt_func) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_counters = sal_alloc( sizeof(interrupt_handle_block_data_t) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_time = sal_alloc( sizeof(interrupt_handle_block_data_t) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_cycle_time = sal_alloc( sizeof(interrupt_handle_block_data_t) * nof_interrupts, "interrupt_handler" );
    interrupt_data_base[unit].recurring_action_cycle_counting = sal_alloc( sizeof(interrupt_handle_block_data_t) * nof_interrupts, "interrupt_handler" );

    if ((interrupt_data_base[unit].func_arr == NULL) ||
        (interrupt_data_base[unit].func_arr_recurring_action == NULL) ||
        (interrupt_data_base[unit].recurring_action_counters == NULL) ||
        (interrupt_data_base[unit].recurring_action_time == NULL) ||
        (interrupt_data_base[unit].recurring_action_cycle_time == NULL) ||
        (interrupt_data_base[unit].recurring_action_cycle_counting == NULL)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to alloc interrupt_handler")));
    }

    cur_time = sal_time();
    
    /*init recurring_action_unit*/
    for (index = 0; index < nof_interrupts; index++) {
        interrupt_data_base[unit].func_arr[index] = interrupt_handle_genenric_none; /*do nothing by default*/
        interrupt_data_base[unit].func_arr_recurring_action[index] = interrupt_recurring_action_handle_generic_none; /*do nothing by default*/

        rc = interrupt_get_nof_block_instances(unit, index, &nof_block_instances);
        if (rc != BCM_E_NONE) {
            continue;
        }

        recurring_action_counters = &(interrupt_data_base[unit].recurring_action_counters[index]);
        if (recurring_action_counters != NULL) {
            recurring_action_counters->block_instance_data = sal_alloc( sizeof(int) * nof_block_instances, "interrupt_handler" );
            if (recurring_action_counters->block_instance_data == NULL) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to alloc interrupt_handler")));
            }

            sal_memset(recurring_action_counters->block_instance_data, 0, sizeof(int) * nof_block_instances);
        }

        recurring_action_time = &(interrupt_data_base[unit].recurring_action_time[index]);
        if (recurring_action_time != NULL) {
            recurring_action_time->block_instance_data = sal_alloc( sizeof(int) * nof_block_instances, "interrupt_handler" );
            if (recurring_action_time->block_instance_data == NULL) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to alloc interrupt_handler")));
            }

            sal_memset(recurring_action_time->block_instance_data, cur_time, sizeof(int) * nof_block_instances);
        }

        recurring_action_cycle_time = &(interrupt_data_base[unit].recurring_action_cycle_time[index]);
        if (recurring_action_cycle_time != NULL) {
            recurring_action_cycle_time->block_instance_data = sal_alloc( sizeof(int) * nof_block_instances, "interrupt_handler" );
            if (recurring_action_cycle_time->block_instance_data == NULL) {
               SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to alloc interrupt_handler"))); 
            }

            sal_memset(recurring_action_cycle_time->block_instance_data, -1, sizeof(int) * nof_block_instances);
        }

        recurring_action_cycle_counting = &(interrupt_data_base[unit].recurring_action_cycle_counting[index]);
        if (recurring_action_cycle_counting != NULL) {
            recurring_action_cycle_counting->block_instance_data = sal_alloc( sizeof(int) * nof_block_instances, "interrupt_handler" );
            if (recurring_action_cycle_counting->block_instance_data == NULL) {
               SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to alloc interrupt_handler"))); 
            }

            sal_memset(recurring_action_cycle_counting->block_instance_data, -1, sizeof(int) * nof_block_instances);
        }
    }
   
    interrupt_common_params[unit].interrupt_add_interrupt_handler_init(unit);  
    
    if(sal_sem_give(interrupt_data_base[unit].interrupts_data_sem)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to give the Interrupts Data Semaphore")));;
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_db_init(int unit)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    /* Create the common parameters according to Device type */
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_FE1600(unit)) {
        rc = fe1600_interrupt_handler_init_cmn_param(unit, &interrupt_common_params[unit]);
        SOCDNX_IF_ERR_EXIT(rc);
    } else
#endif /* BCM_DFE_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = arad_interrupt_handler_init_cmn_param(unit, &interrupt_common_params[unit]);
        SOCDNX_IF_ERR_EXIT(rc);
    } else 
#endif /* BCM_PETRA_SUPPORT */
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Interrupt Application not supported for this device"))); 
    }

    /* Init interrupt CB datad base */
    rc = interrupt_handler_data_base_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN;
}


int
interrupt_handler_appl_ref_cache_defaults_set(int unit)
{
     int rc;
     int memory_index;

    SOCDNX_INIT_FUNC_DEFS;

    /* attach cache memory */
    if (interrupt_common_params[unit].cached_mem != NULL) {
        for (memory_index=0; (interrupt_common_params[unit].cached_mem[memory_index]) != NUM_SOC_MEM; memory_index++) {
            rc = interrupt_attach_cache(unit, interrupt_common_params[unit].cached_mem[memory_index], COPYNO_ALL);
            SOCDNX_IF_ERR_EXIT(rc);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_log_defaults_set(int unit)
{
     int rv,i;
     bcm_switch_event_control_t event;

    SOCDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))  
#endif
    {
       event.action = bcmSwitchEventLog;
       event.index = 0;
       event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    
       rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
       BCMDNX_IF_ERR_EXIT(rv);

       for (i=0 ; interrupt_common_params[unit].int_disable_print_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) {
            event.event_id = interrupt_common_params[unit].int_disable_print_on_init[i];  
            event.index = 0;         
            rv=bcm_switch_event_control_set(unit,  BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0);
            BCMDNX_IF_ERR_EXIT(rv);
       }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_mask_defaults_set(int unit)
{
     int rv, i;
     soc_interrupt_db_t* interrupts;
     soc_block_types_t  block;
     soc_reg_t reg;
     int blk;
     bcm_switch_event_control_t event;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }
    
    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) 
#endif
    {
        for (i=0 ; interrupt_common_params[unit].int_disable_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) { 
            event.event_id = interrupt_common_params[unit].int_disable_on_init[i];
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,1);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        for (i=0 ; interrupt_common_params[unit].int_active_on_init[i] != interrupt_common_params[unit].nof_interrupts; i++) {
            /* ARAD_INT_IDR_MMUECC false alarm at arad_a0. should not be activated.*/
#ifdef BCM_PETRA_SUPPORT
            if (((interrupt_common_params[unit].int_active_on_init[i] == ARAD_INT_IDR_MMUECC_1BERRINT) || 
                 (interrupt_common_params[unit].int_active_on_init[i] == ARAD_INT_IDR_MMUECC_2BERRINT)) && SOC_IS_ARAD_A0(unit)) {
                continue;
            }
#endif /* BCM_PETRA_SUPPORT */
            event.event_id = interrupt_common_params[unit].int_active_on_init[i]; 
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_ref_defaults_set(int unit)
{
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

#if !defined(PLISIM)
	if ( ! SOC_IS_ARAD(unit))	/* Arad cache memory is initialized on init via proprietary function.  */
	{
		rc = interrupt_handler_appl_ref_cache_defaults_set(unit);
		SOCDNX_IF_ERR_EXIT(rc);
	}
#endif

    rc = interrupt_handler_appl_ref_log_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_ref_mask_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);



exit:
    SOCDNX_FUNC_RETURN;
}


int
interrupt_handler_appl_revent_register(int unit)
{
    int rv;

    SOCDNX_INIT_FUNC_DEFS;

    rv =bcm_switch_event_register(unit, diag_switch_event_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_switch_event_register(unit, interrupt_appl_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

int
interrupt_handler_appl_init(int unit)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    SOCDNX_INIT_FUNC_DEFS;
    if (SOC_IS_JERICHO(unit)) { 
        SOC_EXIT;
    }
 
    rc = interrupt_handler_appl_db_init(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_ref_defaults_set(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = interrupt_handler_appl_revent_register(unit);
    SOCDNX_IF_ERR_EXIT(rc);

    /* Enable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0); 
    SOCDNX_IF_ERR_EXIT(rc);
#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    dcmn_intr_handler_short_init(unit);
#endif
    
exit:
    SOCDNX_FUNC_RETURN;
}

static int interrupt_handler_appl_deinit_internal(int unit, int cache_upd_req)
{
    int memory_index;
    int rc;
    int index;
    uint32 nof_interrupts;
    int nof_block_instances;
    interrupt_handle_block_data_t *recurring_action_counters;
    interrupt_handle_block_data_t *recurring_action_time;
    interrupt_handle_block_data_t *recurring_action_cycle_time;
    interrupt_handle_block_data_t *recurring_action_cycle_counting;
    bcm_switch_event_control_t bcm_switch_event_control;

    SOCDNX_INIT_FUNC_DEFS;

    if (cache_upd_req) {
        /* Update cached memories to detect ECC and parity errors */
        rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
         SOCDNX_IF_ERR_CONT(rc); 
    }
 
    /* Disable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1); 
    SOCDNX_IF_ERR_CONT(rc); 
 
    if(interrupt_data_base[unit].interrupts_data_sem != NULL) {
        if( sal_sem_take(interrupt_data_base[unit].interrupts_data_sem, APPL_DCMN_INTR_DATA_SEM_TAKE_TO_DEINIT ) != 0) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOC_MSG("Failed to take the interrupt data Semaphor")));
        }
    } 
 
    bcm_switch_event_unregister(unit, interrupt_appl_cb, NULL);
    bcm_switch_event_unregister(unit, diag_switch_event_cb, NULL);

    nof_interrupts = interrupt_common_params[unit].nof_interrupts;
    for (index = 0; index < nof_interrupts; index++) {
        rc = interrupt_get_nof_block_instances(unit, index, &nof_block_instances);
        if (rc != BCM_E_NONE) {
            continue;
        }

        if (interrupt_data_base[unit].recurring_action_counters != NULL) {
            recurring_action_counters = &(interrupt_data_base[unit].recurring_action_counters[index]);
            if (recurring_action_counters->block_instance_data != NULL) {
                sal_free(recurring_action_counters->block_instance_data);
                recurring_action_counters->block_instance_data = NULL;
            }
        }

        if (interrupt_data_base[unit].recurring_action_time != NULL) {
            recurring_action_time = &(interrupt_data_base[unit].recurring_action_time[index]);
            if (recurring_action_time->block_instance_data != NULL) {
                sal_free(recurring_action_time->block_instance_data);
                recurring_action_time->block_instance_data = NULL;
            }
        }

        if (interrupt_data_base[unit].recurring_action_cycle_time != NULL) {
            recurring_action_cycle_time = &(interrupt_data_base[unit].recurring_action_cycle_time[index]);
            if (recurring_action_cycle_time->block_instance_data != NULL) {
                sal_free(recurring_action_cycle_time->block_instance_data);
                recurring_action_cycle_time->block_instance_data = NULL;
            }
        }

        if (interrupt_data_base[unit].recurring_action_cycle_counting != NULL) {
            recurring_action_cycle_counting = &(interrupt_data_base[unit].recurring_action_cycle_counting[index]);
            if (recurring_action_cycle_counting->block_instance_data != NULL) {
                sal_free(recurring_action_cycle_counting->block_instance_data);
                recurring_action_cycle_counting->block_instance_data = NULL;
            }
        }
    }

    if (interrupt_data_base[unit].func_arr != NULL) {
        sal_free(interrupt_data_base[unit].func_arr);
        interrupt_data_base[unit].func_arr = NULL;
    }
    if (interrupt_data_base[unit].func_arr_recurring_action != NULL) {
        sal_free(interrupt_data_base[unit].func_arr_recurring_action);
        interrupt_data_base[unit].func_arr_recurring_action = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_counters != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_counters);
        interrupt_data_base[unit].recurring_action_counters = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_time != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_time);
        interrupt_data_base[unit].recurring_action_time = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_cycle_time != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_cycle_time);
        interrupt_data_base[unit].recurring_action_cycle_time = NULL;
    }
    if (interrupt_data_base[unit].recurring_action_cycle_counting != NULL) {
        sal_free(interrupt_data_base[unit].recurring_action_cycle_counting);
        interrupt_data_base[unit].recurring_action_cycle_counting = NULL;
    }

    /* detach cache */
    if(interrupt_common_params[unit].cached_mem!= NULL) {
        for(memory_index=0; interrupt_common_params[unit].cached_mem[memory_index] != NUM_SOC_MEM; memory_index++) {
            rc = interrupt_detach_cache(unit, interrupt_common_params[unit].cached_mem[memory_index], COPYNO_ALL);
            SOCDNX_IF_ERR_CONT(rc);
        }
    }

    if (interrupt_data_base[unit].interrupts_data_sem != NULL) {
        sal_sem_destroy(interrupt_data_base[unit].interrupts_data_sem);
        interrupt_data_base[unit].interrupts_data_sem = NULL; 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

int interrupt_handler_appl_deinit(int unit)
{
    return (interrupt_handler_appl_deinit_internal(unit, 1));
}

int interrupt_handler_appl_nocache_deinit(int unit)
{
    return (interrupt_handler_appl_deinit_internal(unit, 0));
}


int
interrupt_attach_cache(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
    
    SOCDNX_INIT_FUNC_DEFS;
   
    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {

        SOC_MEM_ALIAS_TO_ORIG(unit,mem);
        assert(SOC_MEM_IS_VALID(unit, mem));

       rc = soc_mem_cache_set(unit, mem, block_instance, 1);
       SOCDNX_IF_ERR_EXIT(rc);

    } else {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
   
        
exit:
    SOCDNX_FUNC_RETURN;

}

int
interrupt_detach_cache(int unit, soc_reg_t mem, int block_instance)
{
    int rc;
  
    SOCDNX_INIT_FUNC_DEFS;
  
    if (soc_mem_is_valid(unit, mem) &&
            ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0)) {
	SOC_MEM_ALIAS_TO_ORIG(unit,mem);
	assert(SOC_MEM_IS_VALID(unit, mem));

        rc = soc_mem_cache_set(unit, mem, block_instance, 0);
        SOCDNX_IF_ERR_EXIT(rc);
    }
    else {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME


