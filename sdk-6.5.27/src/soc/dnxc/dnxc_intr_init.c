
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement soc interrupt handler.
 */

#include <shared/bsl.h>
#include <shared/shr_thread_manager.h>

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
#include <soc/sand/shrextend/shrextend_debug.h>
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
#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/cmicx_gen2.h>
#include <soc/access/access_regmem_util.h>
#include <soc/access/auto_generated/common_enum.h>
#include <soc/access/access_intr.h>
#endif
#ifdef BCM_CMICX_GEN2_SUPPORT
#include <soc/access/intr_cmicx_gen2.h>
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
#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        access_runtime_info_t *runtime_info = access_runtime_info_get(unit);
        const access_device_type_info_t *device_info = runtime_info->device_type_info;
        access_local_regmem_id_t local_regmem = access_return_local_regmem_id(runtime_info, reg);
        const access_device_regmem_t *rm_info = device_info->local_regs + local_regmem;
        const access_device_block_t *blocks = device_info->blocks + rm_info->local_block_id;
        const uint8 *block_instances_flags = runtime_info->block_instance_flags + blocks->instances_start_index;

        for (inst = 0; inst < blocks->nof_instances; inst++)
        {
            if (block_instances_flags[inst] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
            {

                continue;
            }

            for (array_index = rm_info->first_array_index;
                 array_index < rm_info->nof_array_indices + rm_info->first_array_index; array_index++)
            {
                if (field != (int) ACCESS_INVALID_FIELD_ID)
                {
                    SHR_IF_ERR_EXIT(access_local_regmem
                                    (runtime_info, 0, local_regmem, inst, array_index, inst, regVal));

                    SHR_IF_ERR_EXIT(access_field_set(unit, field, regVal, fieldVal));
                }

                SHR_IF_ERR_EXIT(access_local_regmem(runtime_info,
                                                    FLAG_ACCESS_IS_WRITE, local_regmem, inst, array_index, 0, regVal));
            }
        }
        SHR_EXIT();
    }
#endif
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
    soc_interrupt_db_t *interrupts;
    int i;
    int nof_interrupts = 0;
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

#ifdef BCM_ACCESS_SUPPORT
    if ((!SAL_BOOT_NO_INTERRUPTS) && access_new_interrupt_available(unit))
    {
        access_runtime_info_t *runtime_info = access_runtime_info_get(unit);
        const access_device_type_info_t *device_info = runtime_info->device_type_info;
        access_local_block_id_t block_id, nof_blocks = device_info->nof_blocks;
        const access_device_regmem_t *rm_info;
        const access_device_block_t *block;
        const access_block_instance_t *block_instance;
        access_sbus_block_id_t sbus_block_id;
        access_regmem_id_t regmem;
        uint16 array_index;

        SOC_REG_ABOVE_64_CLEAR(data);

        for (block_id = 0; block_id < nof_blocks; block_id++)
        {
            block = ACCESS_DEV_BLK_INFO(runtime_info, block_id);
            block_instance = device_info->block_instances + block->instances_start_index;
            sbus_block_id = block_instance->sbus_info.sbus_block_id;

            regmem = SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info[sbus_block_id].int_mask_reg;
            if (ACCESS_INVALID_LOCAL_REGMEM_ID != regmem)
            {
                rm_info = device_info->local_regs + regmem;
                for (array_index = rm_info->first_array_index;
                     array_index < rm_info->first_array_index + rm_info->nof_array_indices; array_index++)
                {

                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem,
                                                        ACCESS_ALL_BLOCK_INSTANCES, array_index, 0, data));
                }
            }
        }

        SOC_REG_ABOVE_64_CLEAR(data);
        interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

        for (i = 0; i < nof_interrupts; i++)
        {
            regmem = interrupts[i].reg;
            if (ACCESS_INVALID_LOCAL_REGMEM_ID != regmem)
            {
                rm_info = device_info->local_regs + regmem;
                if (interrupts[i].vector_info)
                {
                    for (array_index = rm_info->first_array_index;
                         array_index < rm_info->first_array_index + rm_info->nof_array_indices; array_index++)
                    {

                        SHR_IF_ERR_EXIT(access_local_regmem(runtime_info, FLAG_ACCESS_IS_WRITE, regmem,
                                                            ACCESS_ALL_BLOCK_INSTANCES, array_index, 0, data));
                    }
                }
            }
        }
        SHR_EXIT();
    }
#endif

    if (!SAL_BOOT_NO_INTERRUPTS)
    {
        soc_block_types_t block;

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
    int interrupt_bit;
    soc_interrupt_event_control_t *evt_ctrl;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_CMICX_GEN2_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_20))
    {
        interrupt_bit = IPROC_IRQ_CHIP_INTR;
    }
    else
#endif
        interrupt_bit =
            soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_IF_ERR_EXIT(soc_dnxc_interrupt_all_enable_set(unit, 0));

#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        SHR_IF_ERR_EXIT(access_cmic_intr_disable(unit, interrupt_bit));
    }
    else
#endif
    {
        soc_cmic_intr_disable(unit, interrupt_bit);
    }

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
    int interrupt_bit;
    int nof_interrupts;
    soc_interrupt_event_control_t *evt_ctrl;

    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_CMICX_GEN2_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_20))
    {
        interrupt_bit = IPROC_IRQ_CHIP_INTR;
    }
    else
#endif
        interrupt_bit =
            soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;

#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        SHR_IF_ERR_EXIT(access_cmic_intr_disable(unit, interrupt_bit));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(soc_cmic_intr_disable(unit, interrupt_bit));
    }

    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;
    evt_ctrl->interrupts = sal_alloc(nof_interrupts * sizeof(soc_interrupt_cause_t), "soc_interrupt_cause_t");

    if (dnxc_intr_handler_init(unit) < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error initializing polled interrupt mode");
    }

    handle.num = interrupt_bit;
    handle.intr_fn = (void (*)(int, void *)) soc_dnxc_interrrupt_event_trigger;
    handle.intr_data = INT_TO_PTR(unit);
#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        SHR_IF_ERR_EXIT(access_cmic_intr_register(unit, (access_cmic_intr_handler_t *) & handle, 1));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(soc_cmic_intr_register(unit, &handle, 1));
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_IF_ERR_EXIT(soc_dnxc_interrupt_all_enable_set(unit, 0));

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    if (!SOC_WARM_BOOT(unit))
    {

        soc_dnxc_interrupts_disable(unit);
    }

#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        SHR_IF_ERR_EXIT(access_cmic_intr_enable(unit, interrupt_bit));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(soc_cmic_intr_enable(unit, interrupt_bit));
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_interrupt_all_enable_set(
    int unit,
    int enable)
{
    int ii;
    soc_reg_above_64_val_t eci_irq_enable;
    soc_reg_t reg[4] = { ICFG_CHIP_LP_INTR_ENABLE_REG0r,
        ICFG_CHIP_LP_INTR_ENABLE_REG1r,
        ICFG_CHIP_LP_INTR_ENABLE_REG2r,
        ICFG_CHIP_LP_INTR_ENABLE_REG3r
    };
    uint32 regVal;
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        SOC_REG_ABOVE_64_ALLONES(eci_irq_enable);
        regVal = 0xFFFFFFFF;
    }
    else
    {
        SOC_REG_ABOVE_64_CLEAR(eci_irq_enable);
        regVal = 0;
    }

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        if (!SOC_WARM_BOOT(unit))
        {

            SHR_IF_ERR_EXIT(access_regmem(unit,
                                          FLAG_ACCESS_IS_WRITE,
                                          rECI_ECI_INTERRUPTS_MASK, ACCESS_ALL_BLOCK_INSTANCES, 0, 0, eci_irq_enable));
        }

        SOC_CONTROL(unit)->interrupt_all_enable = enable;
        SHR_EXIT();
    }
#endif

    {
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable));
        }
    }

    for (ii = 0; ii < 4; ii++)
    {
        SHR_IF_ERR_EXIT(soc_cmic_or_iproc_setreg(unit, reg[ii], regVal));
    }

    SOC_CONTROL(unit)->interrupt_all_enable = enable;
exit:
    SHR_FUNC_EXIT;
}

int
soc_dnxc_interrupt_all_enable_get(
    int unit,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");

    *enable = SOC_CONTROL(unit)->interrupt_all_enable;
exit:
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
    shr_thread_manager_config_t interrupt_event_config_info;
    shr_thread_manager_handler_t interrupt_event_handler;
    int nof_interrupts;
    soc_interrupt_event_control_t *evt_ctrl;
    SHR_FUNC_INIT_VARS(unit);

    evt_ctrl = &SOC_CONTROL(unit)->interrupt_event_control;

    shr_thread_manager_config_t_init(&interrupt_event_config_info);

    interrupt_event_config_info.name = "INTR_EVT";
    interrupt_event_config_info.bsl_module = BSL_LOG_MODULE;
    interrupt_event_config_info.interval = sal_sem_FOREVER;
    interrupt_event_config_info.callback = soc_dnxc_interrupt_event_handler;
    interrupt_event_config_info.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    interrupt_event_config_info.interval = sal_sem_FOREVER;

    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    interrupt_event_config_info.user_data = INT_TO_PTR(nof_interrupts);
    interrupt_event_config_info.thread_priority = 50;

    SHR_IF_ERR_EXIT(shr_thread_manager_create(unit, &interrupt_event_config_info, &interrupt_event_handler));
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
    shr_thread_manager_handler_t interrupt_event_handler = NULL;
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
        SHR_IF_ERR_EXIT(shr_thread_manager_destroy(&interrupt_event_handler));
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
    int composite_unit = PTR_TO_INT(data);
    SHR_FUNC_INIT_VARS(unit);

    if ((NULL == SOC_CONTROL(composite_unit))
        || (0 == SOC_CONTROL(composite_unit)->interrupt_event_control.init_done)
        || (NULL == SOC_CONTROL(composite_unit)->interrupt_event_control.handler))
    {
        LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(composite_unit, "interrupt event handler not started\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(SOC_CONTROL(composite_unit)->interrupt_event_control.handler));
    LOG_DEBUG(BSL_LOG_MODULE, (BSL_META_U(composite_unit, "trigger interrupt event on unit %d\n"), unit));

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
    int soft_init_for_debug_purpose = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit) && dnx_data_intr.general.feature_get(unit, dnx_data_intr_general_soft_init_for_debug))
    {
        soft_init_for_debug_purpose = 1;
    }
    else
#endif
    {
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit) && dnxf_data_intr.general.feature_get(unit, dnxf_data_intr_general_soft_init_for_debug))
        {
            soft_init_for_debug_purpose = 1;
        }
#endif
    }

    if (soft_init_for_debug_purpose)
    {
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {

            reg_value[0] = 0;
            SHR_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE,
                                          rECI_SOFT_INIT_FOR_DEBUG_REG, ACCESS_ALL_BLOCK_INSTANCES, 0, 0, reg_value));

            reg_value[0] = 1;
            SHR_IF_ERR_EXIT(access_regmem(unit, FLAG_ACCESS_IS_WRITE,
                                          rECI_SOFT_INIT_FOR_DEBUG_REG, ACCESS_ALL_BLOCK_INSTANCES, 0, 0, reg_value));

        }
        else
#endif
        {

            soc_reg32_set(unit, ECI_SOFT_INIT_FOR_DEBUG_REGr, SOC_CORE_ALL, 0, 0);

            soc_reg32_set(unit, ECI_SOFT_INIT_FOR_DEBUG_REGr, SOC_CORE_ALL, 0, 1);
        }
        SHR_EXIT();
    }

#ifdef BCM_ACCESS_SUPPORT
    if (access_new_interrupt_available(unit))
    {
        access_runtime_info_t *runtime_info = access_runtime_info_get(unit);
        const access_device_type_info_t *device_info = runtime_info->device_type_info;
        access_local_regmem_id_t local_regmem;
        const access_device_regmem_t *rm_info = device_info->local_regs;
        const access_device_block_t *block;
        const uint8 *block_instances_flags;
        access_block_instance_num_t inst;

        uint16 array_index;

        for (local_regmem = 0; local_regmem < device_info->nof_regs; local_regmem++, rm_info++)
        {

            if (rm_info->flags & ACCESS_REGMEM_FLAG_WO)
            {
                continue;
            }
            if (!(rm_info->flags & (ACCESS_REGMEM_FLAG_COUNTER | ACCESS_REGMEM_FLAG_ERROR_FEILDS)))
            {
                continue;
            }
            if ((0x94 == rm_info->base_address) || (0x96 == rm_info->base_address) || (0x98 == rm_info->base_address))
            {

                continue;
            }

            block = device_info->blocks + rm_info->local_block_id;
            block_instances_flags = runtime_info->block_instance_flags + block->instances_start_index;;
            for (inst = 0; inst < block->nof_instances; inst++)
            {
                if (block_instances_flags[inst] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
                {

                    continue;
                }

                for (array_index = rm_info->first_array_index;
                     array_index < rm_info->first_array_index + rm_info->nof_array_indices; array_index++)
                {

                    SHR_IF_ERR_EXIT(access_local_regmem(runtime_info,
                                                        0, local_regmem, inst, array_index, 0, reg_value));
                }
            }
        }

        SHR_IF_ERR_EXIT(soc_interrupt_clear_all(unit, 1));

        SHR_EXIT();
    }
#endif

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

    SHR_IF_ERR_EXIT(soc_interrupt_clear_all(unit, 1));
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
