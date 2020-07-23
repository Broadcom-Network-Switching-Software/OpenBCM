
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
#include <shared/periodic_event.h>

#include <soc/intr.h>
#include <soc/ipoll.h>
#include <soc/iproc.h>
#include <soc/intr_cmicx.h>
#include <soc/dnxc/intr.h>
#include <soc/dnxc/dnxc_intr.h>

#include <soc/dnxc/dnxc_intr_handler.h>
#include <soc/dnxc/error.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/sand/sand_intr_corr_act_func.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/intr/dnx_intr.h>
#endif
#include <soc/dnxc/dnxc_ha.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxf/cmn/dnxf_intr.h>
#endif


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR







int
soc_dnxc_set_mem_mask(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    int all_one)
{
    int rc;
    int inst = 0;
    int blk;
    soc_reg_above_64_val_t regVal;
    soc_reg_above_64_val_t fieldVal;
    int array_index_min = 0;
    int array_index_max = 1;
    int array_index;
    SHR_FUNC_INIT_VARS(unit);

    
    if (all_one)
    {
        SOC_REG_ABOVE_64_ALLONES(fieldVal);
        SOC_REG_ABOVE_64_ALLONES(regVal);
    }
    else
    {
        SOC_REG_ABOVE_64_CLEAR(fieldVal);
        SOC_REG_ABOVE_64_CLEAR(regVal);
    }

    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block))
    {
        if (SOC_INFO(unit).block_valid[blk])
        {
            if (SOC_REG_IS_ARRAY(unit, reg))
            {
                array_index_max = SOC_REG_NUMELS(unit, reg) + SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
                array_index_min = SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
            }

            for (array_index = array_index_min; array_index < array_index_max; array_index++)
            {
                if (field != INVALIDf)
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, inst, array_index, regVal));
                    soc_reg_above_64_field_set(unit, reg, regVal, field, fieldVal);
                }
                rc = soc_reg_above_64_set(unit, reg, inst, array_index, regVal);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        inst++;
    }
exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_ser_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx_ser_init(unit));
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnxf_ser_init(unit));
    }
#endif
exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrupts_disable(
    int unit)
{
    int rc = _SHR_E_INTERNAL;
    int copy_no;
    soc_reg_t reg;
    int blk;
    soc_reg_above_64_val_t data;
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    soc_block_types_t block;
    soc_interrupt_db_t *interrupts;
    int i;
    int nof_interrupts = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_interrupts = dnxf_data_intr.general.nof_interrupts_get(unit);
    }
#endif

    if (!SAL_BOOT_NO_INTERRUPTS)
    {

        
        SOC_REG_ABOVE_64_CLEAR(data);
        for (blk = 0; ((SOC_BLOCK_TYPE(unit, blk) >= 0) && (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_EMPTY)); blk++)
        {
            
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }
            reg = SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[SOC_BLOCK_INFO(unit, blk).cmic].int_mask_reg;
            if (SOC_REG_IS_VALID(unit, reg))
            {
                copy_no = SOC_BLOCK_NUMBER(unit, blk);
                rc = soc_reg_above_64_set(unit, reg, copy_no, 0, data);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
        for (i = 0; i < nof_interrupts; i++)
        {
            reg = interrupts[i].reg;
            
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }
            block = SOC_REG_INFO(unit, reg).block;
            SOC_REG_ABOVE_64_CLEAR(data);
            SOC_BLOCKS_ITER(unit, blk, block)
            {
                
                if (!SOC_INFO(unit).block_valid[blk])
                {
                    continue;
                }
                copy_no = SOC_BLOCK_NUMBER(unit, blk);
                if (interrupts[i].vector_info)
                {
                    rc = soc_reg_above_64_set(unit, interrupts[i].vector_info->int_mask_reg, copy_no, 0, data);
                    SHR_IF_ERR_EXIT(rc);
                }
            }
        }
#endif
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrupts_deinit(
    int unit)
{
    int ii;
    soc_reg_t reg[4] = { ICFG_CHIP_LP_INTR_ENABLE_REG0r,
        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
        ICFG_CHIP_LP_INTR_ENABLE_REG3r
    };
    int interrupt_bit;
    soc_interrupt_event_control_t *evt_ctrl;
#ifdef BCM_DNX_SUPPORT
    soc_reg_above_64_val_t eci_irq_enable;
#endif

    SHR_FUNC_INIT_VARS(unit);

    interrupt_bit = soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SOC_REG_ABOVE_64_CLEAR(eci_irq_enable);
        (void) soc_reg_above_64_set(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable);
    }
#endif

    for (ii = 0; ii < 4; ii++)
    {
        (void) soc_cmic_or_iproc_setreg(unit, reg[ii], 0);
    }

    
    soc_cmic_intr_disable(unit, interrupt_bit);

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    if (dnxc_intr_handler_deinit(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error at interrupt db deinitialization");
    }

    
    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;
    if (NULL == evt_ctrl->interrupts)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error at interrupt cause deinitialization");
    }
    sal_free((void *) evt_ctrl->interrupts);
    evt_ctrl->interrupts = NULL;

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrupts_init(
    int unit)
{
    soc_cmic_intr_handler_t handle;
    int ii;
    soc_reg_t reg[4] = { ICFG_CHIP_LP_INTR_ENABLE_REG0r,
        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
        ICFG_CHIP_LP_INTR_ENABLE_REG3r
    };
    int interrupt_bit;
    int nof_interrupts;
    soc_interrupt_event_control_t *evt_ctrl;
#ifdef BCM_DNX_SUPPORT
    soc_reg_above_64_val_t eci_irq_enable;
#endif

    SHR_FUNC_INIT_VARS(unit);

    interrupt_bit = soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;

    
    soc_cmic_intr_disable(unit, interrupt_bit);

    
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;
    evt_ctrl->interrupts = sal_alloc(nof_interrupts * sizeof(soc_interrupt_cause_t), "soc_interrupt_cause_t");

    
    if (dnxc_intr_handler_init(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
    }

    
    handle.num = interrupt_bit;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        handle.intr_fn = (void (*)(int, void *)) soc_dnxc_interrrupt_event_trigger;
    }
    else
#endif
    {
        handle.intr_fn = soc_dnxc_intr_handler;
    }
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);
    
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SOC_REG_ABOVE_64_ALLONES(eci_irq_enable);
        (void) soc_reg_above_64_set(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable);
    }
#endif
    for (ii = 0; ii < 4; ii++)
    {
        (void) soc_cmic_or_iproc_setreg(unit, reg[ii], 0xFFFFFFFF);
    }

    if (!SOC_WARM_BOOT(unit))
    {
        
        soc_dnxc_interrupts_disable(unit);
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_interrupt_all_enable_set(
    int unit,
    int enable)
{
    int interrupt_bit;
    SHR_FUNC_INIT_VARS(unit);

    interrupt_bit = soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;
    
    if (enable)
    {
        soc_cmic_intr_enable(unit, interrupt_bit);
    }
    else
    {
        soc_cmic_intr_disable(unit, interrupt_bit);
    }
#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        uint32 mask, irqMask;
        uint32 addr;
        int s;

        mask = 0x01 << (interrupt_bit % (sizeof(uint32) * 8));
        addr = soc_reg_addr(unit, INTC_INTR_ENABLE_REG3r, REG_PORT_ANY, 0);
        s = sal_splhi();
        irqMask = soc_pci_read(unit, addr);
        if (enable)
        {
            irqMask |= mask;
        }
        else
        {
            irqMask &= ~mask;
        }
        soc_pci_write(unit, addr, irqMask);
        sal_spl(s);
    }
#endif
    SHR_FUNC_EXIT;
}


static int
soc_dnxc_interrupt_event_handler(
    int unit,
    void *userdata)
{
    SHR_FUNC_INIT_VARS(unit);

    
    soc_dnxc_ser_intr_handler(INT_TO_PTR(unit), userdata, NULL, NULL, NULL);

    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrupt_event_init(
    int unit)
{
    periodic_event_config_t interrupt_event_config_info;
    periodic_event_handler_t interrupt_event_handler;
    int nof_interrupts;
    soc_interrupt_event_control_t *evt_ctrl;
    char name[24];
    SHR_FUNC_INIT_VARS(unit);

    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;
    
    periodic_event_config_t_init(&interrupt_event_config_info);
    (void) sal_snprintf(name, sizeof(name), "INTR_EVT.%d", unit);
    interrupt_event_config_info.name = name;

    interrupt_event_config_info.bsl_module = BSL_LOG_MODULE;
    interrupt_event_config_info.interval = sal_sem_FOREVER;
    interrupt_event_config_info.callback = soc_dnxc_interrupt_event_handler;

    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    interrupt_event_config_info.user_data = INT_TO_PTR(nof_interrupts);
    interrupt_event_config_info.thread_priority = 50; 

    SHR_IF_ERR_EXIT(periodic_event_create(unit, &interrupt_event_config_info, &interrupt_event_handler));
    evt_ctrl->handler = interrupt_event_handler;
    evt_ctrl->init_done = 1;

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrupt_event_deinit(
    int unit)
{
    uint8 is_init = 0;
    periodic_event_handler_t interrupt_event_handler = NULL;
    soc_interrupt_event_control_t *evt_ctrl;
    SHR_FUNC_INIT_VARS(unit);

    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;
    is_init = evt_ctrl->init_done;
    
    
    if (is_init == TRUE)
    {
        
        interrupt_event_handler = evt_ctrl->handler;
    }

    
    if (interrupt_event_handler != NULL)
    {
        SHR_IF_ERR_EXIT(periodic_event_destroy(&interrupt_event_handler));
    }
    evt_ctrl->init_done = 0;
exit:
    SHR_FUNC_EXIT;
}


int
soc_dnxc_interrrupt_event_trigger(
    int unit,
    void *data)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((NULL == SOC_CONTROL(unit))
        || (0 == SOC_CONTROL(unit)->interrupt_event_control.init_done)
        || (NULL == SOC_CONTROL(unit)->interrupt_event_control.handler))
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "interrupt event handler not started\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(periodic_event_trigger(SOC_CONTROL(unit)->interrupt_event_control.handler));
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(unit, "trigger interrupt event\n")));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnxc_interrupt_counter_clear(
    int unit)
{
    int i_bl, i_ind, first_array_ind;
    int numels;
    soc_reg_t reg;
    soc_reg_info_t *reginfo;
    soc_reg_above_64_val_t reg_value;
    SHR_FUNC_INIT_VARS(unit);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }
        reginfo = &SOC_REG_INFO(unit, reg);

        if (!SOC_REG_IS_ARRAY(unit, reg) || SOC_REG_NUMELS(unit, reg) == 0)
        {
            
            numels = 1;
            first_array_ind = 0;
        }
        else
        {
            numels = reginfo->numels;
            first_array_ind = reginfo->first_array_index;
        }
        
        if (reginfo->flags & SOC_REG_FLAG_WO)
        {
            continue;
        }
        if (!SOC_REG_HAS_COUNTER_FIELDS(unit, reg) && !SOC_REG_HAS_ERROR_FIELDS(unit, reg))
        {
            continue;
        }
        
        if (sand_is_ser_intr_cnt_reg(unit, reg))
        {
            continue;
        }
        
        for (i_bl = 0; SOC_BLOCK_INFO(unit, i_bl).type >= 0; ++i_bl)
        {
            
            if (!SOC_BLOCK_IS_TYPE(unit, i_bl, reginfo->block))
            {
                continue;
            }

            if (!SOC_INFO(unit).block_valid[i_bl])
            {
                continue;
            }

            for (i_ind = first_array_ind; i_ind < first_array_ind + numels; i_ind++)
            {
                (void) soc_reg_above_64_get(unit, reg, SOC_BLOCK_PORT(unit, i_bl), i_ind, reg_value);
            }
        }
    }

    
    SHR_IF_ERR_EXIT(soc_interrupt_clear_all(unit));
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
