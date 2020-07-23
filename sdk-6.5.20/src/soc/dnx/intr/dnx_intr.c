/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement dnx interrupt.
 */


#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>
#include <soc/mcm/allenum.h>
#include <soc/dnx/intr/dnx_intr.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr_cb_func.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnxc/dnxc_intr.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR


int jr2_get_int_id_by_name(
    char *name);
#if !defined(NO_BCM_88800_A0)

int j2c_get_int_id_by_name(
    char *name);

#endif
int q2a_get_int_id_by_name(
    char *name);
int j2p_get_int_id_by_name(
    char *name);



static int
soc_dnx_ser_memory_mask_init(
    int unit)
{
    soc_reg_t reg;
    char *reg_name;
    int rc;
    int inst;
    int blk;
    soc_reg_above_64_val_t above_64;
    int array_index_max = 1;
    int array_index;

    SHR_FUNC_INIT_VARS(unit);

    
    SOC_REG_ABOVE_64_ALLONES(above_64);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }
        if (!SOC_REG_IS_ENABLED(unit, reg))
        {
            continue;
        }

        reg_name = SOC_REG_NAME(unit, reg);
        if ((sal_strstr(reg_name, "MEM_MASK")) == NULL || (sal_strstr(reg_name, "BRDC_") != NULL))
        {
            continue;
        }

        array_index_max = 1;
        if (SOC_REG_IS_ARRAY(unit, reg))
        {
            array_index_max = SOC_REG_NUMELS(unit, reg) + SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
        }
        SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block))
        {
            if (SOC_INFO(unit).block_valid[blk])
            {
                inst = SOC_BLOCK_NUMBER(unit, blk);
                for (array_index = SOC_REG_FIRST_ARRAY_INDEX(unit, reg); array_index < array_index_max; array_index++)
                {
                    rc = soc_reg_above_64_set(unit, reg, inst, array_index, above_64);
                    SHR_IF_ERR_EXIT(rc);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnx_int_name_to_id(
    int unit,
    char *name)
{
    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        return jr2_get_int_id_by_name(name);
    }
#if !defined(NO_BCM_88800_A0)

    else if (SOC_IS_J2C(unit))
    {
        return j2c_get_int_id_by_name(name);
    }

#endif
    else if (SOC_IS_Q2A(unit))
    {
        return q2a_get_int_id_by_name(name);
    }
    else if (SOC_IS_J2P(unit))
    {
        return j2p_get_int_id_by_name(name);
    }
    return -1;
}

int
soc_dnx_interrupts_array_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        SHR_IF_ERR_EXIT(jr2_interrupts_array_init(unit));
    }
#if !defined(NO_BCM_88800_A0)

    else if (SOC_IS_J2C(unit))
    {
        SHR_IF_ERR_EXIT(j2c_interrupts_array_init(unit));
    }

#endif
    else if (SOC_IS_Q2A(unit))
    {
        SHR_IF_ERR_EXIT(q2a_interrupts_array_init(unit));
    }
    else if (SOC_IS_J2P(unit))
    {
        SHR_IF_ERR_EXIT(j2p_interrupts_array_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_interrupts_array_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        jr2_interrupts_array_deinit(unit);
        SHR_EXIT();
    }
#if !defined(NO_BCM_88800_A0)

    else if (SOC_IS_J2C(unit))
    {
        j2c_interrupts_array_deinit(unit);
        SHR_EXIT();
    }

#endif
    else if (SOC_IS_Q2A(unit))
    {
        q2a_interrupts_array_deinit(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2P(unit))
    {
        j2p_interrupts_array_deinit(unit);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_interrupt_cb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        jr2_interrupt_cb_init(unit);
        SHR_EXIT();
    }
#if !defined(NO_BCM_88800_A0)

    else if (SOC_IS_J2C(unit))
    {
        j2c_interrupt_cb_init(unit);
        SHR_EXIT();
    }

#endif
    else if (SOC_IS_Q2A(unit))
    {
        q2a_interrupt_cb_init(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2P(unit))
    {
        j2p_interrupt_cb_init(unit);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnx_is_block_eci_intr_assert(
    int unit,
    int blk,
    soc_reg_above_64_val_t eci_interrupt)
{
    int rc = 0;
    soc_reg_t reg = ECI_ECI_INTERRUPTSr;
    soc_field_t intr_field;
    const dnx_data_intr_general_block_intr_field_mapping_t *intr_mapping;

    intr_mapping = dnx_data_intr.general.block_intr_field_mapping_get(unit, blk);
    intr_field = (soc_field_t) intr_mapping->intr;
    if (intr_field != INVALIDf)
    {
        rc = (int) soc_reg_above_64_field32_get(unit, reg, eci_interrupt, intr_field);
    }
    return rc;
}

int
soc_dnx_ser_init(
    int unit)
{
    const dnx_data_intr_ser_mem_mask_t *mem_mask;
    const dnxc_data_table_info_t *table_info;
    int mask_index;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx_ser_memory_mask_init(unit));

        table_info = dnx_data_intr.ser.mem_mask_info_get(unit);
        for (mask_index = 0; mask_index < table_info->key_size[0]; mask_index++)
        {
            mem_mask = dnx_data_intr.ser.mem_mask_get(unit, mask_index);
            if (mem_mask->reg != INVALIDr)
            {
                switch (mem_mask->mode)
                {
                    case dnxc_mem_mask_mode_none:
                    {
                        
                        break;
                    }
                    case dnxc_mem_mask_mode_zero:
                    {
                        SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 0));
                        break;
                    }
                    case dnxc_mem_mask_mode_one:
                    {
                        SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 1));
                        break;
                    }
                    default:
                    {
                        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Wrong memory mask mode %d, table index %d\n"),
                                                    mem_mask->mode, mask_index));
                        break;
                    }
                }
            }
        }

        
        if (!dnx_data_intr.ser.tcam_protection_issue_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_ser_tcam_protect_machine_enable(unit, 1));
        }

        
        SHR_IF_ERR_EXIT(dnx_ser_kaps_tcam_scan_machine_enable(unit, TRUE, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_intr_exceptional_dram_not_present(
    int unit,
    uint32 *is_exception)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_exception, _SHR_E_PARAM, "is_exception is NULL");
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        *is_exception = 1;
    }
    else
    {
        *is_exception = 0;
    }

exit:
    SHR_FUNC_EXIT;
}


int
soc_dnx_interrupt_enable(
    int unit,
    int intr_id,
    int block_instance,
    const soc_interrupt_db_t * inter)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_intr_exception_t *intr_exception;
    uint32 index, is_exception = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    table_info = dnx_data_intr.ser.intr_exception_info_get(unit);
    for (index = 0; index < table_info->key_size[0]; index++)
    {
        intr_exception = dnx_data_intr.ser.intr_exception_get(unit, index);
        if (intr_id == intr_exception->intr)
        {
            if (intr_exception->exception_get_cb != NULL)
            {
                SHR_IF_ERR_EXIT(intr_exception->exception_get_cb(unit, &is_exception));
            }
            else
            {
                is_exception = 1;
            }
            break;
        }
    }

    if (is_exception)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "intr_id(%d) is exceptional and can't be enabled!!\n", intr_id);
    }

    SHR_IF_ERR_EXIT(soc_interrupt_enable(unit, block_instance, inter));
exit:
    SHR_FUNC_EXIT;
}


int
soc_dnx_is_interrupt_support(
    int unit,
    int intr_id,
    int *is_support)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_intr_exception_t *intr_exception;
    uint32 index, is_exception = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_support, _SHR_E_PARAM, "is_support")
    
        table_info = dnx_data_intr.ser.intr_exception_info_get(unit);
    for (index = 0; index < table_info->key_size[0]; index++)
    {
        intr_exception = dnx_data_intr.ser.intr_exception_get(unit, index);
        if (intr_id == intr_exception->intr)
        {
            if (intr_exception->exception_get_cb != NULL)
            {
                SHR_IF_ERR_EXIT(intr_exception->exception_get_cb(unit, &is_exception));
            }
            else
            {
                is_exception = 1;
            }
            break;
        }
    }

    if (is_exception)
    {
        *is_support = 0;
    }
    else
    {
        *is_support = 1;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ser_kaps_tcam_scan_machine_enable(
    int unit,
    int is_init,
    int enable)
{
    soc_reg_t reg;
    int index, first_idx, end_idx;
    int blk, inst;
    soc_reg_above_64_val_t data;
    const dnx_data_intr_ser_kaps_tcam_scan_t *kaps_tcam_scan;
    int desc_dma_enabled = dnx_sbusdma_desc_is_enabled(unit, SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM);

    SHR_FUNC_INIT_VARS(unit);

    reg = KAPS_RPB_TCAM_SCAN_CFGSr;
    if (SOC_REG_IS_ARRAY(unit, reg))
    {
        first_idx = SOC_REG_INFO(unit, reg).first_array_index;
        end_idx = first_idx + SOC_REG_NUMELS(unit, reg) - 1;
    }
    else
    {
        first_idx = 0;
        end_idx = 0;
    }

    SOC_REG_ABOVE_64_CLEAR(data);

    if (is_init)
    {
        
        kaps_tcam_scan = dnx_data_intr.ser.kaps_tcam_scan_get(unit);

        
        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_SEC_PAUSE_Nf, 0);
        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_DEC_PAUSE_Nf, kaps_tcam_scan->scan_pause);
        
        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_SEC_ACTION_Nf, kaps_tcam_scan->correctable_action);
        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_DEC_ACTION_Nf,
                                     kaps_tcam_scan->uncorrectable_action);

        
        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_WINDOW_Nf, kaps_tcam_scan->scan_window);
        SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_on.set(unit, 0, data[0]));
        SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_on.set(unit, 1, data[1]));

        soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_WINDOW_Nf, 0x0);
        SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_paused.set(unit, 0, data[0]));
        SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_paused.set(unit, 1, data[1]));

        
        if (enable == TRUE)
        {
            soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_WINDOW_Nf, kaps_tcam_scan->scan_window);
        }
        else
        {
            soc_reg_above_64_field32_set(unit, reg, data, RPB_TCAM_SCAN_WINDOW_Nf, 0);
        }
    }
    else
    {
        
        if (enable == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_on.get(unit, 0, &data[0]));
            SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_on.get(unit, 1, &data[1]));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_paused.get(unit, 0, &data[0]));
            SHR_IF_ERR_EXIT(mdb_db_infos.kaps_scan_cfg_paused.get(unit, 1, &data[1]));
        }
    }

    
    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block))
    {
        if (SOC_INFO(unit).block_valid[blk])
        {
            inst = SOC_BLOCK_NUMBER(unit, blk);
            for (index = first_idx; index <= end_idx; index++)
            {
                if (desc_dma_enabled == TRUE)
                {
                    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_reg(unit, reg, inst, index, data));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, inst, index, data));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
