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
#include <soc/intr.h>
#include <soc/ipoll.h>
#include <soc/drv.h>
#include <soc/register.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/mcm/allenum.h>
#ifdef BCM_88790
#include <soc/dnxf/ramon/ramon_intr_cb_func.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/intr/dnx_intr.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR


#define SOC_DNXC_INTR_DATA_SEM_TAKE_TO_CB  10000
#define SOC_DNXC_INTR_DATA_SEM_TAKE_TO_DEINIT 20000000




#ifdef BCM_DNXF_SUPPORT
int ramon_get_int_id_by_name(
    char *name);
#endif


int
dnxc_interrupt_memory_cached(
    int unit,
    soc_reg_t mem,
    int block_instance,
    int *cached_flag)
{
    int copyno;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cached_flag, _SHR_E_PARAM, "cached_flag");

    copyno = (block_instance == SOC_BLOCK_ALL) ? SOC_BLOCK_ALL : block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
    *cached_flag = soc_mem_cache_get(unit, mem, copyno);

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_intr_add_handler(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action)
{
    SHR_FUNC_INIT_VARS(unit);
    dnxc_intr_add_handler_ext(unit, en_inter, occurrences, timeCycle, inter_action, inter_recurring_action, NULL);
    SHR_FUNC_EXIT;
}

int
dnxc_intr_add_handler_ext(
    int unit,
    int en_inter,
    int occurrences,
    int timeCycle,
    soc_handle_interrupt_func inter_action,
    soc_handle_interrupt_func inter_recurring_action,
    char *msg)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].recurring_action_cycle_counting = occurrences;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].recurring_action_cycle_time = timeCycle;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].func_arr = inter_action;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].func_arr_recurring_action = inter_recurring_action;
    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].msg = msg;

    SHR_FUNC_EXIT;
}

int
dnxc_intr_add_clear_func(
    int unit,
    int en_inter,
    clear_func clear_action)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_inter].interrupt_clear = clear_action;

    SHR_FUNC_EXIT;
}

static int
dnxc_intr_interrupt_recurring_detect(
    int unit,
    int block_instance,
    uint32 en_interrupt)
{
    int currentTime;
    int cycleTime, *startCountingTime;
    int cycleCount, *counter;
    soc_handle_interrupt_func func_arr_recurring_action;
    SHR_FUNC_INIT_VARS(unit);

    currentTime = sal_time();   
    cycleTime = (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_cycle_time);
    startCountingTime = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_time);
    func_arr_recurring_action =
        (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].func_arr_recurring_action);

    cycleCount = (SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_cycle_counting);
    counter = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].recurring_action_counters);

    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "%s: en_interrupt=%d, currentTime=%d, cycleTime=%d, *startCountingTime=%d, cycleCount=%d, *counter=%d.\n"),
                 FUNCTION_NAME(), en_interrupt, currentTime, cycleTime, *startCountingTime, cycleCount, *counter));

    if (((cycleTime) <= 0) || ((cycleCount) <= 0) || (func_arr_recurring_action == NULL))
    {
        return 0;
    }

    if (1 == cycleCount)
    {
        return 1;
    }

    if (currentTime - *startCountingTime > cycleTime)
    {
        
        *startCountingTime = currentTime;
        *counter = 1;
        return 0;
    }

    (*counter)++;
    if (*counter >= cycleCount)
    {
        (*counter) = 0;
        return 1;
    }

    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_print_info_get(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *special_msg)
{
    int rc;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    soc_mem_t mem;
    char *memory_name;
    uint32 block;
    sand_memory_dc_t type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    cnt_reg = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].cnt_reg;
    if (!SOC_REG_IS_VALID(unit, cnt_reg))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unavail action for interrupt %d\n", en_interrupt);
    }
    type = sand_get_cnt_reg_type(unit, cnt_reg);

    if (type == SAND_INVALID_DC)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unavail action for interrupt %d\n", en_interrupt);
    }
    SHR_IF_ERR_EXIT(soc_get_reg_first_block_id(unit, cnt_reg, &block));

    rc = sand_get_cnt_reg_values(unit, type, cnt_reg, block_instance, &cntf, &cnt_overflowf, &addrf, &addr_validf);
    SHR_IF_ERR_EXIT(rc);

    if (addr_validf != 0)
    {
        mem = soc_addr_to_mem_extended(unit, block, 0xff, addrf);
        if (mem != INVALIDm)
        {
            SOC_MEM_ALIAS_TO_ORIG(unit, mem);
            memory_name = SOC_MEM_NAME(unit, mem);
        }
        else
        {
            memory_name = NULL;
        }

        switch (mem)
        {
            case INVALIDm:
                sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE,
                             "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory is not accessible",
                             cntf, cnt_overflowf, addrf);
                break;
            default:
                sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE,
                             "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s",
                             cntf, cnt_overflowf, addrf, memory_name);
                break;

        }

    }
    else
    {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE,
                     "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid", cntf,
                     cnt_overflowf, addrf);
    }

exit:
    SHR_FUNC_EXIT;
}
void
dnxc_intr_switch_event_cb(
    int unit,
    soc_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *userdata)
{
    int interrupt_id = arg1;
    int block_instance = arg2;
    soc_interrupt_db_t *interrupt;
    int nof_interrupts = 0;
    uint32 flags;
    int is_storm_count_period, is_storm_nominal;
    int rc;
    int inter_get;
    int intr_valid = 0;
    soc_handle_interrupt_func func;

    SHR_FUNC_INIT_VARS(unit);
    COMPILER_REFERENCE(_func_rv);

    
    if (SOC_SWITCH_EVENT_DEVICE_INTERRUPT != event)
    {
        return;
    }

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    
    if (SOC_CONTROL(unit)->interrupts_info->interrupt_sem != NULL)
    {
        if (sal_sem_take(SOC_CONTROL(unit)->interrupts_info->interrupt_sem, SOC_DNXC_INTR_DATA_SEM_TAKE_TO_CB) != 0)
        {
            LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Failed to take the interrupt data Semaphor")));
            return;
        }
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Interrupt data is uninitialiezed")));
        return;
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    SHR_IF_ERR_EXIT(rc);

    if (interrupt_id >= nof_interrupts || interrupt_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid interrupt");
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);
    intr_valid = 1;

    
    rc = soc_interrupt_disable(unit, block_instance, interrupt);
    SHR_IF_ERR_EXIT(rc);

    
    rc = soc_interrupt_update_storm_detection(unit, block_instance, interrupt);
    SHR_IF_ERR_EXIT(rc);
    rc = soc_interrupt_is_storm(unit, block_instance, interrupt, &is_storm_count_period, &is_storm_nominal);
    SHR_IF_ERR_EXIT(rc);
    if (is_storm_count_period || is_storm_nominal)
    {
        
        rc = dnxc_interrupt_print_info(unit, block_instance, interrupt_id, 0, DNXC_INT_CORR_ACT_NONE, "Storm Detected");
        SHR_IF_ERR_EXIT(rc);
        rc = dnxc_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, "Storm Detected");
        SHR_IF_ERR_EXIT(rc);
    }
    else
    {
        
        char *info = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].msg;
        if (dnxc_intr_interrupt_recurring_detect(unit, block_instance, interrupt_id))
        {
            
            func = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].func_arr_recurring_action;
            if (NULL != func)
            {
                rc = func(unit, block_instance, interrupt_id, info);
                SHR_IF_ERR_EXIT(rc);
            }
            else
            {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Interrupt recurr corrective action not found\n")));
                rc = dnxc_interrupt_print_info(unit, block_instance, interrupt_id, 0, DNXC_INT_CORR_ACT_NONE, info);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        else
        {
            func = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].func_arr;
            if (func != NULL)
            {
                rc = func(unit, block_instance, interrupt_id, info);
                SHR_IF_ERR_EXIT(rc);
            }
            else
            {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Interrupt corrective action not found\n")));
                rc = dnxc_interrupt_print_info(unit, block_instance, interrupt_id, 0, DNXC_INT_CORR_ACT_NONE, info);
                SHR_IF_ERR_EXIT(rc);
            }
        }
    }

    
    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SHR_IF_ERR_EXIT(rc);
    if (flags & SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE)
    {
        SHR_EXIT();
    }

    
    if (NULL == interrupt->interrupt_clear)
    {
        if (interrupt->vector_id == 0)
        {
            LOG_WARN(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit,
                                 "Warning: Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id\n")));
        }
        else
        {
            LOG_WARN(BSL_LS_BCM_SWITCH,
                     (BSL_META_U(unit, "Warning: call to interrupt clear for vector pointer, nothing done\n")));
        }
    }
    else
    {
        rc = interrupt->interrupt_clear(unit, block_instance, interrupt_id);
        if (rc != _SHR_E_UNAVAIL)
        {
            SHR_IF_ERR_EXIT(rc);
        }
    }

    rc = soc_interrupt_get(unit, block_instance, interrupt, &inter_get);
    SHR_IF_ERR_EXIT(rc);

    if (inter_get == 0x0 || (flags & SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK))
    {
         
        if (!(flags & (1 << SOC_INTERRUPT_DB_FLAGS_FORCE_MASK_BIT)) && !(is_storm_count_period || is_storm_nominal))
        {
            rc = soc_interrupt_enable(unit, block_instance, interrupt);
            SHR_IF_ERR_EXIT(rc);
        }
    }
    else
    {
        rc = dnxc_interrupt_print_info(unit, block_instance, interrupt_id, 0, DNXC_INT_CORR_ACT_NONE,
                                       "Interrupt clear Failed!!! Masked");
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    
    if (intr_valid)
    {
        rc = soc_interrupt_stat_cnt_increase(unit, block_instance, interrupt_id);
        SHR_IF_ERR_CONT(rc);
    }

    if (SOC_UNIT_VALID(unit))
    {
        sal_sem_give(SOC_CONTROL(unit)->interrupts_info->interrupt_sem);
    }
    return;
}

int
dnxc_intr_handler_deinit(
    int unit)
{
    sal_sem_t intr_sem;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_CONTROL(unit)->interrupts_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "interrupts wasn't initilized");
    }

    intr_sem = SOC_CONTROL(unit)->interrupts_info->interrupt_sem;
    if (intr_sem != NULL)
    {
        if (sal_sem_take(intr_sem, SOC_DNXC_INTR_DATA_SEM_TAKE_TO_DEINIT) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to take the interrupt data Semaphor");
        }
    }
#ifdef BCM_88790
    if (SOC_IS_RAMON(unit))
    {
        ramon_interrupts_array_deinit(unit);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        soc_dnx_interrupts_array_deinit(unit);
    }
#endif

    if (intr_sem != NULL)
    {
        sal_sem_destroy(intr_sem);
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_intr_handler_short_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sand_collect_blocks_control_info(unit));

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_intr_handler_init(
    int unit)
{
    sal_sem_t intr_sem;

    SHR_FUNC_INIT_VARS(unit);

    intr_sem = sal_sem_create("Interrupt appl data Sem", sal_sem_BINARY, 0);
    if (intr_sem == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Failed to create the Interrupt Data Semaphor");
    }
#ifdef BCM_88790
    if (SOC_IS_RAMON(unit))
    {
        SHR_IF_ERR_EXIT(ramon_interrupts_array_init(unit));
        
        ramon_interrupt_cb_init(unit);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx_interrupts_array_init(unit));
        SHR_IF_ERR_EXIT(soc_dnx_interrupt_cb_init(unit));
    }
#endif
    SHR_IF_ERR_EXIT(sand_collect_blocks_control_info(unit));

    SOC_CONTROL(unit)->interrupts_info->interrupt_sem = intr_sem;
    if (sal_sem_give(SOC_CONTROL(unit)->interrupts_info->interrupt_sem))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to give the Interrupts Data Semaphor");;
    }

exit:
    if (SOC_FAILURE(_func_rv))
    {
        sal_sem_destroy(intr_sem);
        SOC_CONTROL(unit)->interrupts_info->interrupt_sem = NULL;
    }
    
     
    SHR_FUNC_EXIT;
}

int
dnxc_get_ser_entry_from_cache(
    int unit,
    soc_mem_t mem,
    int copyno,
    int array_index,
    int index,
    uint32 *data_entry)
{
    int rc;
    uint8 orig_read_mode = SOC_MEM_FORCE_READ_THROUGH(unit);
    uint8 *vmap;
    int mem_array_vmap_offset = index;
    int entry_dw = 0;

    SHR_FUNC_INIT_VARS(unit);

    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    SOC_MEM_FORCE_READ_THROUGH_SET(unit, 0);    
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
    if (SOC_MEM_IS_ARRAY(unit, mem))
    {
        
        mem_array_vmap_offset = SOC_MEM_SIZE(unit, mem) * (array_index - SOC_MEM_FIRST_ARRAY_INDEX(unit, mem)) + index;
    }
    if (!CACHE_VMAP_TST(vmap, mem_array_vmap_offset) || SOC_MEM_TEST_SKIP_CACHE(unit))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Cache memory %s at entry %u is not available"),
                                    SOC_MEM_NAME(unit, mem), index));
        entry_dw = soc_mem_entry_words(unit, mem);
        sal_memset(data_entry, 0, (entry_dw * 4));
        rc = SOC_E_NONE;
    }
    else
    {
        rc = soc_mem_array_read(unit, mem, array_index, copyno, index, data_entry);
    }
    SOC_MEM_FORCE_READ_THROUGH_SET(unit, orig_read_mode);       

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}


int
dnxc_int_name_to_id(
    int unit,
    char *name)
{
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_RAMON(unit))
    {
        return ramon_get_int_id_by_name(name);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        return soc_dnx_int_name_to_id(unit, name);
    }
#endif

    return -1;
}

#undef BSL_LOG_MODULE
