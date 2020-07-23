/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement Correction action functions for jr2_jericho interrupts.
 */


#include <shared/bsl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <sal/core/dpc.h>

#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/chip.h>

#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>
#include <soc/dnxc/dnxc_mem.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/sand/sand_ser_correction.h>
#include <soc/sand/sand_mem.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_cell.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/mdb_ser.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <bcm_int/dnx/sat/sat.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/cosq/flow_control/pfc_deadlock.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_hard_reset_access.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/switch/switch.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif
#endif


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

#define DNXC_XOR_MEM_DATA_MAX_SIZE       42


#ifdef BCM_DNX_SUPPORT
static soc_block_type_t
get_interrupt_block(
    int unit,
    uint32 en_interrupt)
{
    soc_reg_t reg;
    soc_reg_info_t *reginfo;

    reg = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].reg;
    reginfo = &SOC_REG_INFO(unit, reg);
    return reginfo->block[0];
}
#endif
static soc_mem_severe_t
get_mem_severity(
    int unit,
    soc_mem_t mem)
{
    int rc;
    soc_mem_severe_t severity;
    dnxc_xor_mem_info xor_mem_info;

    severity = SOC_MEM_SEVERITY(unit, mem);
    if (SOC_MEM_TYPE(unit, mem) == SOC_MEM_TYPE_XOR)
    {
        rc = dnxc_xor_mem_info_get(unit, mem, &xor_mem_info);
        if (SHR_SUCCESS(rc))
        {
            if (xor_mem_info.flags & DNXC_XOR_MEMORY_FORCE_SEVERITY_FLAGS)
            {
                severity = SOC_MEM_SEVERE_HIGH;
            }
        }
    }
    return severity;
}

int
dnxc_xor_mem_info_get(
    int unit,
    soc_mem_t mem,
    dnxc_xor_mem_info * xor_mem_info)
{
    int index;
    const dnxc_data_table_info_t *table_info;
#ifdef BCM_DNX_SUPPORT
    const dnx_data_intr_ser_xor_mem_t *dnx_xor_mem;
#endif
#ifdef BCM_DNXF_SUPPORT
    const dnxf_data_intr_ser_xor_mem_t *dnxf_xor_mem;
#endif

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(xor_mem_info, _SHR_E_PARAM, "xor_mem_info");

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        table_info = dnx_data_intr.ser.xor_mem_info_get(unit);
        for (index = 0; index < table_info->key_size[0]; index++)
        {
            dnx_xor_mem = dnx_data_intr.ser.xor_mem_get(unit, index);
            if (dnx_xor_mem->mem == mem)
            {
                xor_mem_info->sram_banks_bits = dnx_xor_mem->sram_banks_bits;
                xor_mem_info->entry_used_bits = dnx_xor_mem->entry_used_bits;
                xor_mem_info->flags = dnx_xor_mem->flags;
                SHR_EXIT();
            }
        }
        
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        table_info = dnxf_data_intr.ser.xor_mem_info_get(unit);
        for (index = 0; index < table_info->key_size[0]; index++)
        {
            dnxf_xor_mem = dnxf_data_intr.ser.xor_mem_get(unit, index);
            if (dnxf_xor_mem->mem == mem)
            {
                xor_mem_info->sram_banks_bits = dnxf_xor_mem->sram_banks_bits;
                xor_mem_info->entry_used_bits = dnxf_xor_mem->entry_used_bits;
                xor_mem_info->flags = 0;
                SHR_EXIT();
            }
        }
        
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_is_mdb_physical_mem(
    int unit,
    soc_mem_t mem)
{
    int rc = 0;
#ifdef BCM_DNX_SUPPORT
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_mdb_mem_map_t *mdb_mem_map;
    uint32 index;

    if (SOC_IS_DNX(unit))
    {
        table_info = dnx_data_intr.ser.mdb_mem_map_info_get(unit);
        for (index = 0; index < table_info->key_size[0]; index++)
        {
            mdb_mem_map = dnx_data_intr.ser.mdb_mem_map_get(unit, index);
            if (mem == mdb_mem_map->phy_mem)
            {
                rc = 1;
                break;
            }
        }
    }
#endif
    return rc;
}

static int
dnxc_mem_mdb_ser_alias(
    int unit,
    soc_mem_t * mem)
{
#ifdef BCM_DNX_SUPPORT
    soc_mem_t mem_tmp = *mem;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_mdb_mem_map_t *mdb_mem_map;
    uint32 index;

    if (SOC_IS_DNX(unit))
    {
        table_info = dnx_data_intr.ser.mdb_mem_map_info_get(unit);
        for (index = 0; index < table_info->key_size[0]; index++)
        {
            mdb_mem_map = dnx_data_intr.ser.mdb_mem_map_get(unit, index);
            if (mem_tmp == mdb_mem_map->acc_mem)
            {
                *mem = mdb_mem_map->phy_mem;
                break;
            }
        }
    }
#endif

    return SOC_E_NONE;
}


char *
dnxc_mem_ser_ecc_action_info_get(
    int unit,
    soc_mem_t mem,
    int isEcc1b)
{
    int rc, rc_ecc1b = 0, rc_ecc2b = 0;
    static char *str_ecc_action[] = {
        "No Action",    
        "ECC_1B_FIX",   
        "XOR_FIX",      
        "EM_SOFT_RECOVERY",     
        "SOFT_RESET",   
        "SHADOW if Cached, Else ECC_1B_FIX",    
        "SHADOW_AND_SOFT_RESET if Cached, Else HARD_RESET",     
        "SHADOW if Cached, Else HARD_RESET",    
        "HARD_RESET",   
        "XOR_FIX if Cached, Else No Action",    
        "MDB 1b Correct",       
        "MDB 2b Correct"        
    };

    soc_mem_severe_t severity = get_mem_severity(unit, mem);
    uint32 dynamic_mem, mem_is_accessible;

    dynamic_mem = sand_tbl_is_dynamic(unit, mem);
    severity = get_mem_severity(unit, mem);
    mem_is_accessible = !(soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem));
    {
        if (dnxc_is_mdb_physical_mem(unit, mem))
        {
            rc_ecc1b = 10;
            rc_ecc2b = 11;
        }
        else if (dynamic_mem)
        {
            rc_ecc1b = 0;       
            if (mem_is_accessible && SOC_IS_DNXF(unit))
            {
                rc_ecc1b = 1;   
            }
            if (severity == SOC_MEM_SEVERE_HIGH)
            {
                rc_ecc2b = 4;
            }
            else
            {
                rc_ecc2b = 0;
            }
        }
        else if (mem_is_accessible)
        {
            rc_ecc1b = 1;       
            if (SOC_MEM_TYPE(unit, mem) == SOC_MEM_TYPE_XOR)
            {
                rc_ecc1b = 2;   
            }

            if (severity == SOC_MEM_SEVERE_HIGH)
            {
                rc_ecc2b = 6;
            }
            else
            {
                rc_ecc2b = 7;
            }
        }
        else
        {
            rc_ecc1b = 0;
            rc_ecc2b = 8;
        }
    }
    if (isEcc1b)
    {
        rc = rc_ecc1b;
    }
    else
    {
        rc = rc_ecc2b;
    }
    return str_ecc_action[rc];
}


int
dnxc_mem_decide_corrective_action(
    int unit,
    sand_memory_dc_t type,
    soc_mem_t mem,
    int copyno,
    dnxc_int_corr_act_type * action_type,
    char *special_msg)
{
    
    soc_mem_severe_t severity = get_mem_severity(unit, mem);
    uint32 dynamic_mem;
    uint32 mem_is_accessible;
    int cache_enable;
    int rc;

    SHR_FUNC_INIT_VARS(unit);

    dynamic_mem = sand_tbl_is_dynamic(unit, mem);

    mem_is_accessible = !(soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem));

    if (dnxc_is_mdb_physical_mem(unit, mem))
    {
        if (type == SAND_ECC_ECC1B_DC)
        {
            *action_type = DNXC_INT_CORR_ACT_MDB_ECC_1B_FIX;
        }
        else if (type == SAND_ECC_ECC2B_DC)
        {
            *action_type = DNXC_INT_CORR_ACT_MDB_ECC_2B_FIX;
        }
        else
        {
            *action_type = DNXC_INT_CORR_ACT_NONE;
        }
        SHR_EXIT();
    }

    
    rc = dnxc_interrupt_memory_cached(unit, mem, copyno, &cache_enable);
    if (rc != _SHR_E_NONE)
    {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE, "mem %s failed to check cachebaility \n",
                     SOC_MEM_NAME(unit, mem));
    }
    SHR_IF_ERR_EXIT(rc);

    if (dynamic_mem)
    {
        switch (type)
        {
            case SAND_ECC_ECC1B_DC:
            case SAND_P_1_ECC_ECC1B_DC:
            case SAND_P_2_ECC_ECC1B_DC:
            case SAND_P_3_ECC_ECC1B_DC:
                if (mem_is_accessible && SOC_IS_DNXF(unit))
                {
                    *action_type = DNXC_INT_CORR_ACT_ECC_1B_FIX;
                }
                else
                {
                    *action_type = DNXC_INT_CORR_ACT_NONE;
                }
                SHR_EXIT();
            default:
                if (severity == SOC_MEM_SEVERE_HIGH)
                {
                    *action_type = DNXC_INT_CORR_ACT_SOFT_RESET;
                    SHR_EXIT();
                }
        }
        *action_type = DNXC_INT_CORR_ACT_NONE;
        SHR_EXIT();
    }

    if (mem_is_accessible)
    {
        switch (type)
        {
            case SAND_ECC_ECC1B_DC:
            case SAND_P_1_ECC_ECC1B_DC:
            case SAND_P_2_ECC_ECC1B_DC:
            case SAND_P_3_ECC_ECC1B_DC:
                if (SOC_MEM_TYPE(unit, mem) == SOC_MEM_TYPE_XOR)
                {
                    *action_type = DNXC_INT_CORR_ACT_XOR_ECC_1BIT_FIX;
                }
                else
                {
                    *action_type = DNXC_INT_CORR_ACT_ECC_1B_FIX;
                }
                SHR_EXIT();
            default:
                if (!cache_enable)
                {
                    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE, "mem %s is not cached \n",
                                 SOC_MEM_NAME(unit, mem));
                    *action_type = DNXC_INT_CORR_ACT_HARD_RESET;
                    SHR_EXIT();
                }
                switch (severity)
                {
                    case SOC_MEM_SEVERE_HIGH:
                        if (SOC_MEM_TYPE(unit, mem) == SOC_MEM_TYPE_XOR)
                        {
                            *action_type = DNXC_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET;
                        }
                        else
                        {
                            *action_type = DNXC_INT_CORR_ACT_SHADOW_AND_SOFT_RESET;
                        }
                        SHR_EXIT();
                    default:
                        if (SOC_MEM_TYPE(unit, mem) == SOC_MEM_TYPE_XOR)
                        {
                            *action_type = DNXC_INT_CORR_ACT_XOR_FIX;
                        }
                        else
                        {
                            *action_type = DNXC_INT_CORR_ACT_SHADOW;
                        }
                        SHR_EXIT();
                }
        }
    }
    else
    {
        switch (type)
        {
            case SAND_ECC_ECC1B_DC:
            case SAND_P_1_ECC_ECC1B_DC:
            case SAND_P_2_ECC_ECC1B_DC:
            case SAND_P_3_ECC_ECC1B_DC:
                *action_type = DNXC_INT_CORR_ACT_NONE;
                SHR_EXIT();
            default:
                *action_type = DNXC_INT_CORR_ACT_HARD_RESET;
                break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_data_collection_for_shadowing(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    char *special_msg,
    dnxc_int_corr_act_type * p_corrective_action,
    dnxc_interrupt_mem_err_info * shadow_correct_info)
{
    int rc;
    uint32 cntf, cnt_overflowf, addrf, addr_validf;
    soc_reg_t cnt_reg = INVALIDr;
    unsigned numels;
    int index;
    soc_mem_t mem;
    char *memory_name;
    uint32 block;
    sand_memory_dc_t type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(shadow_correct_info, _SHR_E_PARAM, "shadow_correct_info");

    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;
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
            SHR_IF_ERR_EXIT(dnxc_mem_mdb_ser_alias(unit, &mem));
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
                rc = soc_mem_addr_to_array_element_and_index(unit, mem, addrf, &numels, &index);
                SHR_IF_ERR_EXIT(rc);

                sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE,
                             "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x memory=%s, array element=%d, index=%d",
                             cntf, cnt_overflowf, addrf, memory_name, numels, index);

                
                if (mem == RTP_SLSCTm)
                {
                    *p_corrective_action = DNXC_INT_CORR_ACT_RTP_SLSCT;
                }
                else
                {
                    rc = dnxc_mem_decide_corrective_action(unit, type, mem, block_instance, p_corrective_action,
                                                           special_msg);
                }
                SHR_IF_ERR_EXIT(rc);

                shadow_correct_info->array_index = numels;
                shadow_correct_info->copyno = block_instance + SOC_MEM_BLOCK_MIN(unit, mem);
                shadow_correct_info->min_index = index;
                shadow_correct_info->max_index = index;
                shadow_correct_info->mem = mem;
        }
    }
    else
    {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE,
                     "nof_occurences=%04u, cnt_overflowf=0x%01x, memory address=0x%08x address is not valid",
                     cntf, cnt_overflowf, addrf);
    }

exit:
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action_shadow(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * shadow_correct_info_p,
    char *msg)
{
    int rc;
    uint32 *data_entry = 0;
    int entry_dw;
    int current_index;
    soc_mem_t mem = INVALIDm;
    int copyno;
    int mem_lock_flag = 0;
    soc_reg_above_64_val_t monitor_mask[4];
#ifdef BCM_DNX_SUPPORT
    uint32 eps_protect_flag[2] = { 0, 0 };
    soc_reg_above_64_val_t eps_shaper_set[2];
#endif

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(shadow_correct_info_p, _SHR_E_PARAM, "shadow_correct_info_p");

    mem = shadow_correct_info_p->mem;
    copyno =
        (shadow_correct_info_p->copyno == COPYNO_ALL) ? SOC_MEM_BLOCK_ANY(unit, mem) : shadow_correct_info_p->copyno;

    if (sand_tbl_is_dynamic(unit, mem))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    if (!soc_mem_cache_get(unit, mem, copyno))
    {
        SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
    }

    entry_dw = soc_mem_entry_words(unit, mem);
    data_entry = sal_alloc((entry_dw * 4), "Shadow data entry allocation");

    if (NULL == data_entry)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnx_mem_read_eps_protect_start(unit, mem, shadow_correct_info_p->copyno,
                                                   eps_protect_flag, eps_shaper_set));
#endif

    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, mem, block_instance, monitor_mask));
    MEM_LOCK(unit, mem);
    mem_lock_flag = 1;

    for (current_index = shadow_correct_info_p->min_index; current_index <= shadow_correct_info_p->max_index;
         current_index++)
    {
        rc = dnxc_get_ser_entry_from_cache(unit, mem, copyno, shadow_correct_info_p->array_index, current_index,
                                           data_entry);
        SHR_IF_ERR_EXIT(rc);

#ifdef BCM_DNX_SUPPORT
        if ((SOC_IS_DNX(unit)) && (mem == MDB_EEDB_ENTRY_BANKm))
        {
            SHR_IF_ERR_EXIT(mdb_ser_eedb_array_write(unit, shadow_correct_info_p->array_index,
                                                     shadow_correct_info_p->copyno, current_index, data_entry));

        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(soc_mem_array_write(unit, mem, shadow_correct_info_p->array_index,
                                                shadow_correct_info_p->copyno, current_index, data_entry));
        }
    }

exit:
    if (mem_lock_flag)
    {
        mem_lock_flag = 0;
        MEM_UNLOCK(unit, mem);
        SHR_IF_ERR_EXIT(sand_re_enable_block_mem_monitor(unit, interrupt_id, mem, block_instance, monitor_mask));
    }
#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_CONT(dnx_mem_read_eps_protect_end(unit, mem, shadow_correct_info_p->copyno,
                                                 eps_protect_flag, eps_shaper_set));
#endif

    if (data_entry != NULL)
    {
        sal_free(data_entry);
    }
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_corrected_data_entry(
    int unit,
    int block_instance,
    dnxc_interrupt_mem_err_info * ecc_1b_correct_info_p,
    int current_index,
    int xor_flag,
    uint32 *data_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ecc_1b_correct_info_p, _SHR_E_PARAM, "ecc_1b_correct_info_p");
    SHR_NULL_CHECK(data_entry, _SHR_E_PARAM, "data_entry");

#ifdef BCM_DNX_SUPPORT
    if ((SOC_IS_DNX(unit)) && (ecc_1b_correct_info_p->mem == MDB_EEDB_ENTRY_BANKm))
    {
        SHR_IF_ERR_EXIT(mdb_ser_eedb_array_read_flags(unit, ecc_1b_correct_info_p->array_index,
                                                      ecc_1b_correct_info_p->copyno, current_index, data_entry,
                                                      SOC_MEM_DONT_USE_CACHE));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(soc_mem_array_read_flags(unit, ecc_1b_correct_info_p->mem,
                                                 ecc_1b_correct_info_p->array_index,
                                                 ecc_1b_correct_info_p->copyno, current_index, data_entry,
                                                 SOC_MEM_DONT_USE_CACHE));
    }

    if (xor_flag == 0)
    {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit, "correction for  %s will be done by ecc 1 bit correction\n"),
                                    SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
    }

    
    if (SOC_MEM_FIELD_VALID(unit, ecc_1b_correct_info_p->mem, ECCf))
    {
        uint32 mem_row_bit_width;
        uint32 ecc_field[2] = { 0, 0 }, ecc_field_prev[2] =
        {
        0, 0};
        uint32 ecc_field_len;

        soc_mem_field_get(unit, ecc_1b_correct_info_p->mem, data_entry, ECCf, ecc_field);
        ecc_field_prev[0] = ecc_field[0];
        ecc_field_prev[1] = ecc_field[1];
        ecc_field_len = soc_mem_field_length(unit, ecc_1b_correct_info_p->mem, ECCf);
        mem_row_bit_width = soc_mem_entry_bits(unit, ecc_1b_correct_info_p->mem) - ecc_field_len;

        if (ecc_field_len < 32)
        {
            if ((1 << ecc_field_len) < mem_row_bit_width)
            {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Ecc len:%d for memory %s mem_row_bit_width:%d \n"),
                                            ecc_field_len, SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem),
                                            mem_row_bit_width));
                SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
            }
        }
        SHR_IF_ERR_EXIT(ecc_correction
                        (unit, ecc_1b_correct_info_p->mem, mem_row_bit_width, xor_flag, data_entry, ecc_field));
        if ((ecc_field_prev[0] != ecc_field[0]) || (ecc_field_prev[1] != ecc_field[1]))
        {
            soc_mem_field_set(unit, ecc_1b_correct_info_p->mem, data_entry, ECCf, ecc_field);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_handles_corrective_action_for_xor(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * xor_correct_info,
    int ecc1bit,
    char *msg)
{
    int rc;
    int sp_index, sp_banks_num;
    int current_index, correct_index, oper_index;
    int offset, cache_enable, entry_dw;
    uint32 *data_entry = NULL;
    dnxc_xor_mem_info xor_mem_info;
    uint32 eci_global_value;
    uint32 xor_correct_value[DNXC_XOR_MEM_DATA_MAX_SIZE] = { 0 };
    uint32 dump_xor_value[DNXC_XOR_MEM_DATA_MAX_SIZE] = { 0 };
    int mem_lock_flag = 0;
    soc_reg_above_64_val_t monitor_mask[4];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(xor_correct_info, _SHR_E_PARAM, "xor_correct_info");

    rc = dnxc_interrupt_memory_cached(unit, xor_correct_info->mem, block_instance, &cache_enable);
    if (rc != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Couldnt decide cache state for %s \n"),
                                    SOC_MEM_NAME(unit, xor_correct_info->mem)));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    entry_dw = soc_mem_entry_words(unit, xor_correct_info->mem);
    if (entry_dw > DNXC_XOR_MEM_DATA_MAX_SIZE)
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Memory %s wide large than %d\n"),
                                    SOC_MEM_NAME(unit, xor_correct_info->mem), DNXC_XOR_MEM_DATA_MAX_SIZE));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }
    data_entry = sal_alloc((entry_dw * 4), "Data entry allocation");
    if (NULL == data_entry)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    sal_memset(data_entry, 0, (entry_dw * 4));
    rc = dnxc_xor_mem_info_get(unit, xor_correct_info->mem, &xor_mem_info);
    SHR_IF_ERR_EXIT_WITH_LOG(rc, "The memory %s is not XOR memory.%s%s\n",
                             SOC_MEM_NAME(unit, xor_correct_info->mem), EMPTY, EMPTY);

    if (!ecc1bit && cache_enable)
    {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit, "correction for %s will be done by shadow\n"),
                                    SOC_MEM_NAME(unit, xor_correct_info->mem)));
    }
    else if (ecc1bit && SOC_MEM_FIELD_VALID(unit, xor_correct_info->mem, ECCf))
    {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit, "correction for  %s will be done by ecc 1 bit correction\n"),
                                    SOC_MEM_NAME(unit, xor_correct_info->mem)));
    }
    else
    {
        LOG_ERROR(BSL_LS_BCM_INTR, (BSL_META_U(unit, "The table %s can not be recovered.\n"),
                                    SOC_MEM_NAME(unit, xor_correct_info->mem)));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, xor_correct_info->mem, block_instance, monitor_mask));
    MEM_LOCK(unit, xor_correct_info->mem);
    mem_lock_flag = 1;
    sp_banks_num = 1 << xor_mem_info.sram_banks_bits;
    for (current_index = xor_correct_info->min_index; current_index <= xor_correct_info->max_index; current_index++)
    {
        sal_memset(xor_correct_value, 0, sizeof(xor_correct_value));
        offset = current_index & ~(sp_banks_num - 1);

        
        for (sp_index = 0; sp_index < sp_banks_num; sp_index++)
        {
            correct_index = offset + sp_index;

            if (ecc1bit)
            {
                SHR_IF_ERR_EXIT(dnxc_interrupt_corrected_data_entry(unit, block_instance, xor_correct_info,
                                                                    correct_index, 1, data_entry));
            }
            else
            {
                
                SHR_IF_ERR_EXIT(dnxc_get_ser_entry_from_cache(unit, xor_correct_info->mem, xor_correct_info->copyno,
                                                              xor_correct_info->array_index, correct_index,
                                                              data_entry));
            }
            SHR_IF_ERR_EXIT(soc_mem_array_write(unit, xor_correct_info->mem, xor_correct_info->array_index,
                                                xor_correct_info->copyno, correct_index, data_entry));

            if (sp_index == 0)
            {
                sal_memcpy(dump_xor_value, data_entry, entry_dw * sizeof(uint32));
            }

            for (oper_index = 0; oper_index < entry_dw; oper_index++)
            {
                xor_correct_value[oper_index] ^= data_entry[oper_index];
            }
        }

        if ((xor_mem_info.flags & DNXC_XOR_MEMORY_NOT_RECOVER_XOR_BANK_FLAGS) == 0)
        {
            
            SHR_IF_ERR_EXIT(READ_ECI_GLOBAL_MEM_OPTIONSr(unit, &eci_global_value));

            soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 1);
            SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));

            rc = soc_mem_array_write(unit, xor_correct_info->mem, xor_correct_info->array_index,
                                     xor_correct_info->copyno, offset, xor_correct_value);

            soc_reg_field_set(unit, ECI_GLOBAL_MEM_OPTIONSr, &eci_global_value, WRITE_TO_XORf, 0);
            SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_MEM_OPTIONSr(unit, eci_global_value));
            SHR_IF_ERR_EXIT(rc);

            
            SHR_IF_ERR_EXIT(soc_mem_array_write(unit, xor_correct_info->mem, xor_correct_info->array_index,
                                                xor_correct_info->copyno, offset, dump_xor_value));
            sal_memset(dump_xor_value, 0, DNXC_XOR_MEM_DATA_MAX_SIZE * sizeof(uint32));
        }
    }

exit:
    if (mem_lock_flag)
    {
        mem_lock_flag = 0;
        MEM_UNLOCK(unit, xor_correct_info->mem);
        SHR_IF_ERR_EXIT(sand_re_enable_block_mem_monitor
                        (unit, interrupt_id, xor_correct_info->mem, block_instance, monitor_mask));
    }

    if (data_entry != NULL)
    {
        sal_free(data_entry);
    }
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action_for_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * ecc_1b_correct_info_p,
    char *msg)
{
    int rc;
    uint32 *data_entry = 0;
    int entry_dw;
    int current_index;
    int copyno;
    int mem_lock_flag = 0;
    soc_reg_above_64_val_t monitor_mask[4];
#ifdef BCM_DNX_SUPPORT
    uint32 eps_protect_flag[2] = { 0, 0 };
    soc_reg_above_64_val_t eps_shaper_set[2];
#endif

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ecc_1b_correct_info_p, _SHR_E_PARAM, "ecc_1b_correct_info_p");

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_EXIT(dnx_mem_read_eps_protect_start(unit,
                                                   ecc_1b_correct_info_p->mem,
                                                   ecc_1b_correct_info_p->copyno, eps_protect_flag, eps_shaper_set));
#endif

    copyno = ecc_1b_correct_info_p->copyno - SOC_MEM_BLOCK_MIN(unit, ecc_1b_correct_info_p->mem);
    if (SOC_IS_DNX(unit) && sand_tbl_is_dynamic(unit, ecc_1b_correct_info_p->mem))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Interrupt will not be handled because %s is dynamic\n"),
                                    SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    entry_dw = soc_mem_entry_words(unit, ecc_1b_correct_info_p->mem);
    data_entry = sal_alloc((entry_dw * 4), "DNXC SER ecc 1 data entry allocation");
    if (NULL == data_entry)
    {
        SHR_IF_ERR_EXIT(_SHR_E_MEMORY);
    }

    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Before correction of %s \n"),
                                SOC_MEM_NAME(unit, ecc_1b_correct_info_p->mem)));

    
    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, ecc_1b_correct_info_p->mem, copyno, monitor_mask));
    MEM_LOCK(unit, ecc_1b_correct_info_p->mem);
    mem_lock_flag = 1;

    for (current_index = ecc_1b_correct_info_p->min_index; current_index <= ecc_1b_correct_info_p->max_index;
         current_index++)
    {
        rc = dnxc_interrupt_corrected_data_entry(unit, block_instance, ecc_1b_correct_info_p, current_index, 0,
                                                 data_entry);
        SHR_IF_ERR_EXIT(rc);

#ifdef BCM_DNX_SUPPORT
        if ((SOC_IS_DNX(unit)) && (ecc_1b_correct_info_p->mem == MDB_EEDB_ENTRY_BANKm))
        {
            SHR_IF_ERR_EXIT(mdb_ser_eedb_array_write(unit, ecc_1b_correct_info_p->array_index,
                                                     ecc_1b_correct_info_p->copyno, current_index, data_entry));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(soc_mem_array_write(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->array_index,
                                                ecc_1b_correct_info_p->copyno, current_index, data_entry));
        }
    }

exit:
    if (mem_lock_flag)
    {
        mem_lock_flag = 0;
        MEM_UNLOCK(unit, ecc_1b_correct_info_p->mem);
        SHR_IF_ERR_CONT(sand_re_enable_block_mem_monitor
                        (unit, interrupt_id, ecc_1b_correct_info_p->mem, copyno, monitor_mask));
    }

#ifdef BCM_DNX_SUPPORT
    SHR_IF_ERR_CONT(dnx_mem_read_eps_protect_end(unit, ecc_1b_correct_info_p->mem, ecc_1b_correct_info_p->copyno,
                                                 eps_protect_flag, eps_shaper_set));
#endif

    if (data_entry != NULL)
    {
        sal_free(data_entry);
    }
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action_soft_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    int rc = 0;
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        soc_block_type_t blktype = get_interrupt_block(unit, interrupt_id);
        int blk_is_fdrc = 0;

        switch (blktype)
        {
            case SOC_BLK_FCR:
            case SOC_BLK_FCT:
            case SOC_BLK_FDA:
            case SOC_BLK_FDR:
            case SOC_BLK_FDT:
            case SOC_BLK_FDTL:
            case SOC_BLK_FMAC:
            case SOC_BLK_FSRD:
            case SOC_BLK_MESH_TOPOLOGY:
            case SOC_BLK_RTP:
            {
                blk_is_fdrc = 1;
                break;
            }
            default:
            {
                blk_is_fdrc = 0;
                break;
            }
        }
        
        if (blk_is_fdrc)
        {
            SHR_IF_ERR_EXIT(dnx_redo_soft_reset_soft_init(unit, 0, 0, 0));
        }
        else if (blktype == SOC_BLK_ILE)
        {
            SHR_IF_ERR_EXIT(dnx_redo_soft_reset_soft_init(unit, 0, 1, 0));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_redo_soft_reset_soft_init(unit, 0, 1, 1));
        }
    }
    else
#endif
    {
        rc = soc_device_reset(unit, SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET, SOC_DNXC_RESET_ACTION_INOUT_RESET);
        SHR_IF_ERR_EXIT(rc);
    }
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
int
dnx_interrupt_hard_reset_call_register_func(
    int unit)
{
    bcm_switch_hard_reset_callback_t hard_reset_callback = NULL;
    dnx_hard_reset_callback_userdata_t callback_userdata;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_hard_reset_db.hard_reset_callback.callback.get(unit, &hard_reset_callback));
    SHR_IF_ERR_EXIT(dnx_hard_reset_db.hard_reset_callback.userdata.get(unit, &callback_userdata));
    if (hard_reset_callback != NULL)
    {
        hard_reset_callback(unit, 0, callback_userdata);
    }
    else
    {
        LOG_FATAL(BSL_LS_BCMDNX_DEVICE, (BSL_META("%d Device Hard Reset CB was not registed! \n"), unit));
    }

exit:
    SHR_FUNC_EXIT;
}
#endif


int
dnxc_interrupt_handles_corrective_action_hard_reset(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                                          "Decision: Hard reset is required! interrupt id: %d, block instance: %d \n"),
                               interrupt_id, block_instance));

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(dnx_interrupt_hard_reset_call_register_func(unit));
    }
#endif

#if defined(BCM_DNXF_SUPPORT) && defined(INCLUDE_INTR)
    if (SOC_IS_DNXF(unit))
    {
        SHR_IF_ERR_EXIT(soc_init(unit));
    }
#endif

#if defined(BCM_DNX_SUPPORT) || (defined(BCM_DNXF_SUPPORT) && defined(INCLUDE_INTR))
exit:
#endif
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

int
dnxc_interrupt_handles_corrective_action_for_mdb_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * correct_info,
    char *msg)
{
    int rc;
    int copyno, current_index;
    soc_mem_t mem = INVALIDm;
    soc_reg_above_64_val_t monitor_mask[4];
    int monitor_disabled = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(correct_info, _SHR_E_PARAM, "mdb_1b_correct_info_p");

    mem = correct_info->mem;
    copyno = correct_info->copyno;

    
    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, mem, block_instance, monitor_mask));
    monitor_disabled = 1;
    for (current_index = correct_info->min_index; current_index <= correct_info->max_index; current_index++)
    {
        rc = mdb_ser_ecc_1b_interrupt_data_correct(unit, copyno, mem, correct_info->array_index, current_index);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    if (monitor_disabled == 1)
    {
        SHR_IF_ERR_EXIT(sand_re_enable_block_mem_monitor(unit, interrupt_id, mem, block_instance, monitor_mask));
    }
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action_for_mdb_2b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * correct_info,
    char *msg)
{
    int rc;
    int copyno, current_index;
    soc_mem_t mem = INVALIDm;
    soc_reg_above_64_val_t monitor_mask[4];
    int monitor_disabled = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(correct_info, _SHR_E_PARAM, "mdb_1b_correct_info_p");

    mem = correct_info->mem;
    copyno = correct_info->copyno;

    
    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, mem, block_instance, monitor_mask));
    monitor_disabled = 1;
    for (current_index = correct_info->min_index; current_index <= correct_info->max_index; current_index++)
    {
        rc = mdb_ser_ecc_2b_interrupt_data_correct(unit, copyno, mem, correct_info->array_index, current_index);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    if (monitor_disabled == 1)
    {
        SHR_IF_ERR_EXIT(sand_re_enable_block_mem_monitor(unit, interrupt_id, mem, block_instance, monitor_mask));
    }
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_handles_corrective_action_for_Bdb_fifo_full(
    int unit,
    int block_instance,
    char *msg)
{
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_data_collection_for_tcam_protection_error(
    int unit,
    int block_instance,
    char *special_msg,
    sand_intr_tcam_location * location)
{
    int rc;
    uint32 reg_value;
    soc_reg_t protection_reg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");
    SHR_NULL_CHECK(location, _SHR_E_PARAM, "location");

    
    protection_reg = TCAM_TCAM_PROTECTION_ERRORr;
    rc = soc_reg32_get(unit, protection_reg, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);

    location->bank_id = soc_reg_field_get(unit, protection_reg, reg_value, TCAM_PROTECTION_ERROR_TCAM_BANKf);
    location->entry = soc_reg_field_get(unit, protection_reg, reg_value, TCAM_PROTECTION_ERROR_TCAM_ENTRY_ADDf);

    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "tcam bank_id=%04u, entry=%06u", location->bank_id, location->entry);
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(
    int unit,
    int block_instance,
    sand_intr_tcam_location * location)
{
    uint32 rc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(location, _SHR_E_PARAM, "location");

    rc = dnx_ser_tcam_access_rewrite_key_from_shadow(unit, block_instance, location->bank_id, location->entry);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_handles_corrective_action_CrpsCounterOvfError(
    int unit,
    int block_instance,
    uint32 en_interrupt)
{
    int rc;
    int index;
    uint32 reg_value;
    int CntOvfAddress;
    char print_str[DNXC_INTERRUPT_PRINT_MSG_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    
    index = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field;
    rc = soc_reg32_get(unit, CRPS_CRPS_ENGINE_ERRORr, block_instance, index, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    CntOvfAddress = soc_reg_field_get(unit, CRPS_CRPS_ENGINE_ERRORr, reg_value, ENGINE_N_CNT_OVF_ADDRESSf);

    sal_snprintf(print_str, DNXC_INTERRUPT_PRINT_MSG_SIZE - 1, "CRPS Counter overflow, block %d, address %d",
                 block_instance, CntOvfAddress);

    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), print_str));
exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_handles_corrective_action_tcam_move_eccfailure(
    int unit,
    int block_instance,
    char *msg)
{
    int rc;
    uint32 reg_value;
    uint32 valid, bank, line;
    uint32 ecc_1bit_error, ecc_2bit_error;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "msg");

    msg[0] = '\0';
    
    rc = soc_reg32_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, block_instance, 0, &reg_value);
    SHR_IF_ERR_EXIT(rc);
    valid = soc_reg_field_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, reg_value, TCAM_MOVE_ECC_FAILURE_VALIDf);

    bank = soc_reg_field_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, reg_value, TCAM_MOVE_ECC_BANKf);
    line = soc_reg_field_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, reg_value, TCAM_MOVE_ECC_LINEf);
    ecc_1bit_error = soc_reg_field_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, reg_value, TCAM_MOVE_1_BIT_ECCf);
    ecc_2bit_error = soc_reg_field_get(unit, TCAM_TCAM_MOVE_ECC_FAILUREr, reg_value, TCAM_MOVE_2_BIT_ECCf);

    sal_snprintf(msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "TcamMoveEccFailure, block %d, valid: %d, bank: %d, line: %d, reason: %s ",
                 block_instance, valid, bank, line,
                 valid ? (ecc_1bit_error ? "1bit ecc error" : (ecc_2bit_error ? "2bit ecc error" : "none")) : "none");
    
    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), msg));

exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_handles_corrective_action_McpEngDb_A_AccessErr(
    int unit,
    int block_instance,
    uint32 bank,
    char *msg)
{
    int rc;
    soc_reg_above_64_val_t reg_value;
    int EngineDbSource;
    soc_field_t field[] = { ENG_DB_A_BANK_0_SOURCEf, ENG_DB_A_BANK_1_SOURCEf };
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "special_msg");

    if (bank >= sizeof(field) / sizeof(soc_field_t))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported bank: %d\n", bank);
    }
    
    rc = soc_reg_above_64_get(unit, MCP_ENG_DB_BANK_SOURCEr, block_instance, 0, reg_value);
    SHR_IF_ERR_EXIT(rc);
    EngineDbSource = soc_reg_above_64_field32_get(unit, MCP_ENG_DB_BANK_SOURCEr, reg_value, field[bank]);

    sal_snprintf(msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "MCP_EngDb A AccessErrInt, block %d, Engine DB source is %d", block_instance, EngineDbSource);
    
    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), msg));

exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_handles_corrective_action_McpEngDb_C_AccessErr(
    int unit,
    int block_instance,
    int bank,
    char *msg)
{
    int rc;
    soc_reg_above_64_val_t reg_value;
    int EngineDbSource, EngineDbErrSource;
    soc_field_t field[] = {
        ENG_DB_C_BANK_0_SOURCEf, ENG_DB_C_BANK_1_SOURCEf, ENG_DB_C_BANK_2_SOURCEf, ENG_DB_C_BANK_3_SOURCEf,
        ENG_DB_C_BANK_4_SOURCEf, ENG_DB_C_BANK_5_SOURCEf, ENG_DB_C_BANK_6_SOURCEf, ENG_DB_C_BANK_7_SOURCEf,
        ENG_DB_C_BANK_8_SOURCEf, ENG_DB_C_BANK_9_SOURCEf, ENG_DB_C_BANK_10_SOURCEf, ENG_DB_C_BANK_11_SOURCEf,
        ENG_DB_C_BANK_12_SOURCEf, ENG_DB_C_BANK_13_SOURCEf, ENG_DB_C_BANK_14_SOURCEf, ENG_DB_C_BANK_15_SOURCEf,
        ENG_DB_C_BANK_16_SOURCEf, ENG_DB_C_BANK_17_SOURCEf, ENG_DB_C_BANK_18_SOURCEf, ENG_DB_C_BANK_19_SOURCEf,
        ENG_DB_C_BANK_20_SOURCEf, ENG_DB_C_BANK_21_SOURCEf, ENG_DB_C_BANK_22_SOURCEf, ENG_DB_C_BANK_23_SOURCEf,
        ENG_DB_C_BANK_24_SOURCEf, ENG_DB_C_BANK_25_SOURCEf, ENG_DB_C_BANK_26_SOURCEf, ENG_DB_C_BANK_27_SOURCEf,
        ENG_DB_C_BANK_28_SOURCEf, ENG_DB_C_BANK_29_SOURCEf, ENG_DB_C_BANK_30_SOURCEf, ENG_DB_C_BANK_31_SOURCEf,
        ENG_DB_C_BANK_32_SOURCEf, ENG_DB_C_BANK_33_SOURCEf, ENG_DB_C_BANK_34_SOURCEf, ENG_DB_C_BANK_35_SOURCEf
    };

    soc_field_t errfield[] = {
        ENG_DB_C_BANK_0_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_1_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_2_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_3_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_4_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_5_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_6_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_7_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_8_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_9_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_10_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_11_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_12_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_13_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_14_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_15_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_16_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_17_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_18_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_19_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_20_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_21_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_22_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_23_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_24_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_25_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_26_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_27_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_28_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_29_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_30_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_31_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_32_ACCESS_ERR_SOURCEf,
        ENG_DB_C_BANK_33_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_34_ACCESS_ERR_SOURCEf, ENG_DB_C_BANK_35_ACCESS_ERR_SOURCEf
    };
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "special_msg");

    if (bank >= sizeof(field) / sizeof(soc_field_t))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported bank: %d\n", bank);
    }
    
    rc = soc_reg_above_64_get(unit, MCP_ENG_DB_BANK_SOURCEr, block_instance, 0, reg_value);
    SHR_IF_ERR_EXIT(rc);
    EngineDbSource = soc_reg_above_64_field32_get(unit, MCP_ENG_DB_BANK_SOURCEr, reg_value, field[bank]);

    rc = soc_reg_above_64_get(unit, MCP_ENG_DB_BANK_ACCESS_ERR_SOURCEr, block_instance, 0, reg_value);
    SHR_IF_ERR_EXIT(rc);
    EngineDbErrSource =
        soc_reg_above_64_field32_get(unit, MCP_ENG_DB_BANK_ACCESS_ERR_SOURCEr, reg_value, errfield[bank]);

    sal_snprintf(msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "MCP_EngDb C AccessErrInt, block %d, Engine DB source is %d, unexpected source is %d",
                 block_instance, EngineDbSource, EngineDbErrSource);

    
    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), msg));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_interrupt_data_collection_for_kaps_tcam_error(
    int unit,
    int block_instance,
    int is_ecc_2bits,
    int is_rpb_a,
    int index,
    char *special_msg,
    sand_intr_tcam_location * location)
{
    uint32 reg_value;
    soc_reg_t reg;
    soc_field_t field;
    uint32 scan_idx, scan_pause;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");
    SHR_NULL_CHECK(location, _SHR_E_PARAM, "location");

    
    if (is_ecc_2bits)
    {
        reg = KAPS_RPB_TCAM_SCAN_INDEXr;
        if (is_rpb_a)
        {
            field = RPB_A_TCAM_SCAN_INDEX_Nf;
        }
        else
        {
            field = RPB_B_TCAM_SCAN_INDEX_Nf;
        }
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg, block_instance, index, &reg_value));
        scan_idx = soc_reg_field_get(unit, reg, reg_value, field);
        location->entry = scan_idx;

        
        if (is_rpb_a)
        {
            reg = KAPS_RPB_TCAM_A_SCAN_PAUSEr;
            field = RPB_A_TCAM_SCAN_PAUSE_Nf;
        }
        else
        {
            reg = KAPS_RPB_TCAM_B_SCAN_PAUSEr;
            field = RPB_B_TCAM_SCAN_PAUSE_Nf;
        }
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, reg, block_instance, index, &reg_value));
        scan_pause = soc_reg_field_get(unit, reg, reg_value, field);

        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                     "KAPS TECC: %s, %s, index=%d, entry=%d, pause=%d",
                     "ECC_2B", is_rpb_a ? "RPB_A" : "RPB_B", index, scan_idx, scan_pause);
    }
    else
    {
        sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                     "KAPS TECC: %s, %s, index=%d", "ECC_1B", is_rpb_a ? "RPB_A" : "RPB_B", index);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

int
dnxc_interrupt_handles_corrective_action_do_nothing(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNXF_SUPPORT

int
dnxc_interrupt_handles_corrective_action_for_rtp_slsct(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dnxc_interrupt_mem_err_info * shadow_correct_info_p,
    char *msg)
{
    uint32 slsct_val = 0, score_slsct;
    soc_field_t scrub_en;
    soc_reg_above_64_val_t reg_above_64;
    int current_index;
    int rc;
    int mem_lock_flag = 0;
    soc_reg_above_64_val_t monitor_mask[4];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(shadow_correct_info_p, _SHR_E_PARAM, "shadow_correct_info_p");

    SHR_IF_ERR_EXIT(sand_disable_block_mem_monitor(unit, shadow_correct_info_p->mem, block_instance, monitor_mask));
    mem_lock_flag = 1;
    {
        if (dnxf_data_fabric.topology.load_balancing_mode_get(unit) == soc_dnxf_load_balancing_mode_normal)
        {
            
            
            for (current_index = shadow_correct_info_p->min_index; current_index <= shadow_correct_info_p->max_index;
                 current_index++)
            {
                rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index,
                                         shadow_correct_info_p->copyno, current_index, &slsct_val);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        else if (dnxf_data_fabric.topology.load_balancing_mode_get(unit) == soc_dnxf_load_balancing_mode_balanced_input)
        {
            
            
            SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above_64));
            scrub_en =
                soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above_64,
                                             SCT_SCRUB_ENABLEf);
            soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above_64,
                                         SCT_SCRUB_ENABLEf, 0);
            SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above_64));

            SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above_64));
            soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above_64,
                                         SCT_SCRUB_ENABLEf, scrub_en);
            SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above_64));

        }
        else if (dnxf_data_fabric.topology.load_balancing_mode_get(unit) ==
                 soc_dnxf_load_balancing_mode_destination_unreachable)
        {
            
            
            score_slsct = dnxf_data_fabric.topology.max_link_score_get(unit);
            soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, SCORE_OF_LINKf, &score_slsct);

            for (current_index = shadow_correct_info_p->min_index; current_index <= shadow_correct_info_p->max_index;
                 current_index++)
            {
                rc = soc_mem_array_write(unit, shadow_correct_info_p->mem, shadow_correct_info_p->array_index,
                                         shadow_correct_info_p->copyno, current_index, &slsct_val);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        else
        {
            
            SHR_IF_ERR_EXIT(_SHR_E_UNAVAIL);
        }
    }
exit:
    if (mem_lock_flag)
    {
        mem_lock_flag = 0;
        SHR_IF_ERR_EXIT(sand_re_enable_block_mem_monitor
                        (unit, interrupt_id, shadow_correct_info_p->mem, block_instance, monitor_mask));
    }
    SHR_FUNC_EXIT;
}
#endif


int
dnxc_interrupt_print_info(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    int recurring_action,
    dnxc_int_corr_act_type corr_act,
    char *special_msg)
{
    int rc;
    uint32 flags = 1;
    soc_interrupt_db_t *interrupt;
    char cur_corr_act_msg[DNXC_INTERRUPT_COR_ACT_MSG_SIZE];
    char print_msg[DNXC_INTERRUPT_PRINT_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt]);
    if (special_msg == NULL)
    {
        special_msg = "None";
    }

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
    SHR_IF_ERR_EXIT(rc);

    
    if (((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0)
        && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE))
    {
        corr_act = DNXC_INT_CORR_ACT_NONE;
    }

    switch (corr_act)
    {
        case DNXC_INT_CORR_ACT_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset is Required");
            break;
        case DNXC_INT_CORR_ACT_NONE:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "None");
            break;
        case DNXC_INT_CORR_ACT_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Reset");
            break;
        case DNXC_INT_CORR_ACT_PRINT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Print");
            break;
        case DNXC_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Handle OAMP Event Fifo");
            break;
        case DNXC_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Handle OAMP Statistics Event Fifo");
            break;
        case DNXC_INT_CORR_ACT_SHADOW:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow");
            break;
        case DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown link");
            break;
        case DNXC_INT_CORR_ACT_CLEAR_CHECK:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Clear Check");
            break;
        case DNXC_INT_CORR_ACT_CONFIG_DRAM:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Config DRAM");
            break;
        case DNXC_INT_CORR_ACT_ECC_1B_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "ECC 1b Correct");
            break;
        case DNXC_INT_CORR_ACT_EPNI_EM_SOFT_RECOVERY:
        case DNXC_INT_CORR_ACT_IHB_EM_SOFT_RECOVERY:
        case DNXC_INT_CORR_ACT_IHP_EM_SOFT_RECOVERY:
        case DNXC_INT_CORR_ACT_OAMP_EM_SOFT_RECOVERY:
        case DNXC_INT_CORR_ACT_EM_SOFT_RECOVERY:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "EM Soft Recovery");
            break;
        case DNXC_INT_CORR_ACT_FORCE:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Force");
            break;
        case DNXC_INT_CORR_ACT_HANDLE_CRC_DEL_BUF_FIFO:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Handle CRC Delete Buffer FIFO");
            break;
        case DNXC_INT_CORR_ACT_HANDLE_MACT_EVENT_FIFO:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Handle MACT Event FIFO");
            break;
        case DNXC_INT_CORR_ACT_HARD_RESET_WITHOUT_FABRIC:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Hard Reset without Fabric");
            break;
        case DNXC_INT_CORR_ACT_INGRESS_HARD_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Ingress Hard Reset");
            break;
        case DNXC_INT_CORR_ACT_IPS_QDESC:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "IPS QDESC Clear Unused");
            break;
        case DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Reprogram Resource");
            break;
        case DNXC_INT_CORR_ACT_RTP_LINK_MASK_CHANGE:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "RTP Link Mask Change");
            break;
        case DNXC_INT_CORR_ACT_RX_LOS_HANDLE:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "RX LOS Handle");
            break;
        case DNXC_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Shadow and Soft Reset");
            break;
        case DNXC_INT_CORR_ACT_SHUTDOWN_UNREACH_DESTINATION:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown Unreachable Destination");
            break;
        case DNXC_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "TCAM Shadow from SW DB");
            break;
        case DNXC_INT_CORR_ACT_RTP_SLSCT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "RTP SLSCT");
            break;
        case DNXC_INT_CORR_ACT_SHUTDOWN_LINKS:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Shutdown links");
            break;
        case DNXC_INT_CORR_ACT_MC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "MC RTP Correct");
            break;
        case DNXC_INT_CORR_ACT_UC_RTP_CORRECT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "UC RTP Correct");
            break;
        case DNXC_INT_CORR_ACT_ALL_REACHABLE_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "All Rechable fix");
            break;
        case DNXC_INT_CORR_ACT_IPS_QSZ_CORRECT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "IPS QSZ Correct");
            break;
        case DNXC_INT_CORR_ACT_XOR_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "XOR Fix");
            break;
        case DNXC_INT_CORR_ACT_XOR_ECC_1BIT_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "XOR 1bit ecc Fix");
            break;
        case DNXC_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "XOR Fix and soft reset");
            break;
        case DNXC_INT_CORR_ACT_DYNAMIC_CALIBRATION:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Dynamic calibration");
            break;
        case DNXC_INT_CORR_ACT_MDB_ECC_1B_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "MDB 1b Correct");
            break;
        case DNXC_INT_CORR_ACT_MDB_ECC_2B_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "MDB 2b Correct");
            break;
        case DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_FULL:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Handled by BdbFifoNotEmpty interrupt");
            break;
        case DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Count Buffer as Quarantined");
            break;
        case DNXC_INT_CORR_ACT_HBC_ECC_1B_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "HBC 1b Correct");
            break;
        case DNXC_INT_CORR_ACT_HBC_ECC_2B_FIX:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "HBC 2b Correct");
            break;
        case DNXC_INT_PFC_DEADLOCK_BREAKING_MECHANISM:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "PFC deadlock recovery");
            break;
        case DNXC_INT_CORR_ACT_SOFT_INIT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Soft Init");
            break;
        case DNXC_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Log tcam query failure");
            break;
        case DNXC_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Log Counter overflow address");
            break;
        case DNXC_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Application specialized handling");
            break;
        case DNXC_INT_CORR_ACT_TCAM_MOVE_ECC_VALID:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Log tcam move ecc failure");
            break;
        case DNXC_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Log MCP DB A access error info");
            break;
        case DNXC_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Log MCP DB C access error info");
            break;
        case DNXC_INT_CORR_ACT_PON_RESET:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Power reset is required");
            break;
        case DNXC_INT_CORR_ACT_BTC_HEADER_SIZE_EXCEED:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "BTC Header size exceeded");
            break;
        case DNXC_INT_CORR_ACT_BTC_NOF_INSTRUCTIONS_EXCEED:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "BTC number of instructions exceeded");
            break;
        case DNXC_INT_CORR_ACT_BTC_HEADER_SIZE_NOT_BYTE_ALIGNED:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "BTC header size not byte aligned");
            break;
        case DNXC_INT_CORR_ACT_EPNI_ETPP_DEC_ABOVE_TH:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "ETPP bytes to strip above 144B");
            break;
        case DNXC_INT_CORR_ACT_EPNI_ETPP_SOP_DEC_ABOVE_TH:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "ETPP bytes to strip sop above 288B");
            break;
        case DNXC_INT_CORR_ACT_EPNI_ETPP_EOP_DEC_ABOVE_TH:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "ETPP bytes to strip eop above 288B");
            break;
        case DNXC_INT_CORR_ACT_EPNI_ETPP_EOP_AND_SOP_DEC_ABOVE_TH:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE,
                         "ETPP total bytes to remove from sop and eop above 288B");
            break;
        case DNXC_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "IPPD invalid destination passed");
            break;
        case DNXC_INT_CORR_ACT_MACSEC_SA_EXPIRY:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "MACSEC SA Expiry event");
            break;
        case DNXC_INT_CORR_ACT_KAPS_TECC:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Kaps TECC");
            break;
        default:
            sal_snprintf(cur_corr_act_msg, DNXC_INTERRUPT_COR_ACT_MSG_SIZE, "Unknown");
            break;
    }

    
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, DNXC_INTERRUPT_PRINT_MSG_SIZE,
                 "id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ", en_interrupt,
                 interrupt->reg_index, block_instance, unit, recurring_action, special_msg, cur_corr_act_msg);
#else
    sal_snprintf(print_msg, DNXC_INTERRUPT_PRINT_MSG_SIZE,
                 "name=%s, id=%d, index=%d, block=%d, unit=%d, recurring_action=%d | %s | %s\n ",
                 interrupt->name, en_interrupt, interrupt->reg_index, block_instance, unit, recurring_action,
                 special_msg, cur_corr_act_msg);
#endif

    
    if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s"), print_msg));
    }

exit:
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action_print(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg_print,
    char *msg)
{
    soc_interrupt_db_t *interrupt;
    char print_msg[DNXC_INTERRUPT_PRINT_MSG_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "invalid unit");
    }

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }
    interrupt = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    
#if defined(SOC_NO_NAMES)
    sal_snprintf(print_msg, DNXC_INTERRUPT_PRINT_MSG_SIZE, "id=%d, index=%d, block=%d, unit=%d | %s\n ",
                 interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#else
    sal_snprintf(print_msg, DNXC_INTERRUPT_PRINT_MSG_SIZE, "name=%s, id=%d, index=%d, block=%d, unit=%d | %s\n ",
                 interrupt->name, interrupt_id, interrupt->reg_index, block_instance, unit, msg_print);
#endif

    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s"), print_msg));
exit:
    SHR_FUNC_EXIT;
}

static int
dnxc_interrupt_fmac_link_get(
    int unit,
    int fmac_block_instance,
    int bit_in_field,
    soc_port_t * p_link)
{
    uint32 nof_instances_fmac = 0;
    uint32 nof_links_in_fmac = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_link, _SHR_E_PARAM, "p_link");

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_instances_fmac = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
        nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_instances_fmac = dnxf_data_device.blocks.nof_instances_fmac_get(unit);
        nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    }
#endif
    if (!((fmac_block_instance < nof_instances_fmac) && (bit_in_field < nof_links_in_fmac)))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    *p_link = fmac_block_instance * nof_links_in_fmac + bit_in_field;

exit:
    SHR_FUNC_EXIT;
}
static int
dnxc_interrupt_fsrd_link_get(
    int unit,
    int fsrd_block_instance,
    int bit_in_field,
    int reg_index,
    soc_port_t * p_link)
{
    uint32 nof_instances_fsrd = 0;
    uint32 nof_links_in_fsrd = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_link, _SHR_E_PARAM, "p_link");

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_instances_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
        nof_links_in_fsrd = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_instances_fsrd = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);
        nof_links_in_fsrd = dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);
    }
#endif
    if (!((fsrd_block_instance < nof_instances_fsrd) && (bit_in_field < nof_links_in_fsrd)))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    *p_link = fsrd_block_instance * nof_links_in_fsrd + bit_in_field;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_handles_corrective_action_shutdown_fbr_link(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg)
{
    int bit_in_field = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id].bit_in_field;
    soc_port_t port = 0;
    uint32 rc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_interrupt_fmac_link_get(unit, block_instance, bit_in_field, &port));
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_set, (unit, port, 0));
        SHR_IF_ERR_EXIT(rc);
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port += dnx_data_port.general.fabric_port_base_get(unit);
        rc = soc_dnxc_port_enable_set(unit, port, 0);
        SHR_IF_ERR_EXIT(rc);
    }
#endif
exit:
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_handles_corrective_action_reprogram_resource(
    int unit,
    int block_instance,
    int interrupt_id,
    void *param1,
    char *msg)
{
    SHR_FUNC_INIT_VARS(unit);;

    LOG_INFO(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Decision: reprogram resource should be made!\n")));
    SHR_FUNC_EXIT;
}

int
dnxc_interrupt_handles_corrective_action_tcam_query_failure(
    int unit,
    int block_instance,
    char *msg)
{
    int rc;
    soc_reg_above_64_val_t reg_value;
    int ii;
    int valid, reason;
    int pmfa_app_db[8];
    int pmfb_app_db[2];
    int flp_app_db[2];
    int oam_app_db[2];
    int vtt_app_db[2];
    int egq_app_db[2];
    char str[24];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "msg");

    msg[0] = '\0';
    
    rc = soc_reg_above_64_get(unit, TCAM_TCAM_QUERY_FAILUREr, block_instance, 0, reg_value);
    SHR_IF_ERR_EXIT(rc);
    valid = soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_VALID_FAILUREf);
    reason = soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_REASONf);

    pmfa_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_0f);
    pmfa_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_1f);
    pmfa_app_db[2] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_2f);
    pmfa_app_db[3] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_3f);
    pmfa_app_db[4] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_4f);
    pmfa_app_db[5] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_5f);
    pmfa_app_db[6] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_6f);
    pmfa_app_db[7] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFA_APP_DB_7f);

    pmfb_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFB_APP_DB_0f);
    pmfb_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_PMFB_APP_DB_1f);

    flp_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_FLP_APP_DB_0f);
    flp_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_FLP_APP_DB_1f);

    oam_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_OAM_APP_DB_0f);
    oam_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_OAM_APP_DB_1f);

    vtt_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_VTT_APP_DB_0f);
    vtt_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_VTT_APP_DB_1f);

    egq_app_db[0] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_EGQ_APP_DB_0f);
    egq_app_db[1] =
        soc_reg_above_64_field32_get(unit, TCAM_TCAM_QUERY_FAILUREr, reg_value, TCAM_QUERY_FAILURE_EGQ_APP_DB_1f);

    sal_snprintf(msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "TcamQueryFailure, block %d, valid: %d, reason: %d, ", block_instance, valid, reason);
    for (ii = 0; ii < 8; ii++)
    {
        sal_snprintf(str, 20, "pmfa[%d] %d ", ii, pmfa_app_db[ii]);
        sal_strncat(msg, str, 20);
    }
    for (ii = 0; ii < 2; ii++)
    {
        sal_snprintf(str, 20, "pmfb[%d] %d ", ii, pmfb_app_db[ii]);
        sal_strncat(msg, str, 20);
    }
    for (ii = 0; ii < 2; ii++)
    {
        sal_snprintf(str, 20, "flp[%d] %d ", ii, flp_app_db[ii]);
        sal_strncat(msg, str, 20);
    }
    for (ii = 0; ii < 2; ii++)
    {
        sal_snprintf(str, 20, "oam[%d] %d ", ii, oam_app_db[ii]);
        sal_strncat(msg, str, 20);
    }
    for (ii = 0; ii < 2; ii++)
    {
        sal_snprintf(str, 20, "vtt[%d] %d ", ii, vtt_app_db[ii]);
        sal_strncat(msg, str, 20);
    }
    for (ii = 0; ii < 2; ii++)
    {
        sal_snprintf(str, 20, "egq[%d] %d ", ii, egq_app_db[ii]);
        sal_strncat(msg, str, 20);
    }

    
    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), msg));

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fsrd_SyncStatusChanged(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    int rc;
    soc_port_t link = 0;
    soc_port_t port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    rc = dnxc_interrupt_fsrd_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].reg_index,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif
    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "FSRD sync status changed: link=%d, port=%d, need tune SerDes", link, port);
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fsrd_RxLockChanged(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    int rc;
    soc_port_t link = 0;
    soc_port_t port = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    rc = dnxc_interrupt_fsrd_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].reg_index,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif
    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "FSRD rx lock changed: link=%d, port=%d, need tune SerDes", link, port);
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fmac_tx_ilkn_underrun(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    int rc;
    soc_port_t link, port;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    
    rc = dnxc_interrupt_fmac_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif
    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "FMAC tx_ilkn_underrun: link=%d, port=%d, mask it",
                 link, port);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
    SHR_IF_ERR_EXIT(rc);
    flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
    rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
    SHR_IF_ERR_EXIT(rc);

    
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fmac_tx_ilkn_overflow(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    int rc;
    soc_port_t link, port;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    
    rc = dnxc_interrupt_fmac_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif
    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1, "FMAC tx_ilkn_overflow: link=%d, port=%d, mask it",
                 link, port);

    rc = soc_interrupt_flags_get(unit, en_interrupt, &flags);
    SHR_IF_ERR_EXIT(rc);
    flags |= SOC_INTERRUPT_DB_FLAGS_FORCE_MASK;
    rc = soc_interrupt_flags_set(unit, en_interrupt, flags);
    SHR_IF_ERR_EXIT(rc);

    
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fmac_print_link(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg,
    const char *msg)
{
    int rc;
    soc_port_t link, port;
    soc_interrupt_db_t *interrupts_arr;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(msg, _SHR_E_PARAM, "special_msg");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    
    rc = dnxc_interrupt_fmac_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif

    interrupts_arr = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt]);
    if (interrupts_arr == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Null parameter\n")));
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

#if !defined(SOC_NO_NAMES)

    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "%s: link=%d, port=%d %s", interrupts_arr->name, link, port, msg);

#else

    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "Interrupt %d: link=%d, port=%d %s", interrupts_arr->id, link, port, msg);

#endif

    
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}

int
dnxc_decide_interrupt_action_fmac_tuning_serdes(
    int unit,
    int block_instance,
    uint32 en_interrupt,
    dnxc_int_corr_act_type * p_corrective_action,
    char *special_msg)
{
    int rc;
    soc_port_t link, port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(p_corrective_action, _SHR_E_PARAM, "p_corrective_action");
    SHR_NULL_CHECK(special_msg, _SHR_E_PARAM, "special_msg");

    
    
    rc = dnxc_interrupt_fmac_link_get(unit, block_instance,
                                      SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[en_interrupt].bit_in_field,
                                      &link);
    SHR_IF_ERR_EXIT(rc);
    port = link;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        port = link + dnx_data_port.general.fabric_port_base_get(unit);
    }
#endif
    sal_snprintf(special_msg, DNXC_INTERRUPT_SPECIAL_MSG_SIZE - 1,
                 "link=%d, port=%d, Tune SerDes; If not resolved - shutdown link", link, port);

    
    *p_corrective_action = DNXC_INT_CORR_ACT_NONE;

exit:
    SHR_FUNC_EXIT;
}


int
dnxc_interrupt_handles_corrective_action(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    char *msg,
    dnxc_int_corr_act_type corr_act,
    void *param1,
    void *param2)
{
    int rc;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_INTR_IS_SUPPORTED(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No interrupts for device");
    }

    rc = soc_interrupt_flags_get(unit, interrupt_id, &flags);
    SHR_IF_ERR_EXIT(rc);

    
    if (((flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0)
        && SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE))
    {
        corr_act = DNXC_INT_CORR_ACT_NONE;
    }

    switch (corr_act)
    {
        case DNXC_INT_CORR_ACT_NONE:
        case DNXC_INT_CORR_ACT_APPLICATION_SPECIALIZED_HANDLING:
            rc = dnxc_interrupt_handles_corrective_action_do_nothing(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_SOFT_INIT:
        case DNXC_INT_CORR_ACT_SOFT_RESET:
            rc = dnxc_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_HARD_RESET:
        case DNXC_INT_CORR_ACT_PON_RESET:
            rc = dnxc_interrupt_handles_corrective_action_hard_reset(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_PRINT:
            rc = dnxc_interrupt_handles_corrective_action_print(unit, block_instance, interrupt_id, (char *) param1,
                                                                msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_SHADOW:
            rc = dnxc_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,
                                                                 (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_SHADOW_AND_SOFT_RESET:
            rc = dnxc_interrupt_handles_corrective_action_shadow(unit, block_instance, interrupt_id,
                                                                 (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);

            rc = dnxc_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_ECC_1B_FIX:
            rc = dnxc_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id,
                                                                     (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_SHUTDOWN_FBR_LINKS:
            rc = dnxc_interrupt_handles_corrective_action_shutdown_fbr_link(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_REPROGRAM_RESOURCE:
            rc = dnxc_interrupt_handles_corrective_action_reprogram_resource(unit, block_instance, interrupt_id, param1,
                                                                             msg);
            SHR_IF_ERR_EXIT(rc);
            break;

#ifdef BCM_DNXF_SUPPORT
        case DNXC_INT_CORR_ACT_RTP_SLSCT:
            rc = dnxc_interrupt_handles_corrective_action_for_rtp_slsct(unit, block_instance, interrupt_id,
                                                                        (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
#endif
        case DNXC_INT_CORR_ACT_XOR_FIX:
            rc = dnxc_interrupt_handles_corrective_action_for_xor(unit, block_instance, interrupt_id,
                                                                  (dnxc_interrupt_mem_err_info *) param1, 0, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_XOR_ECC_1BIT_FIX:
            rc = dnxc_interrupt_handles_corrective_action_for_xor(unit, block_instance, interrupt_id,
                                                                  (dnxc_interrupt_mem_err_info *) param1, 1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_XOR_FIX_AND_SOFT_RESET:
            rc = dnxc_interrupt_handles_corrective_action_for_xor(unit, block_instance, interrupt_id,
                                                                  (dnxc_interrupt_mem_err_info *) param1, 0, msg);
            SHR_IF_ERR_EXIT(rc);

            rc = dnxc_interrupt_handles_corrective_action_soft_reset(unit, block_instance, interrupt_id, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

#ifdef BCM_DNX_SUPPORT
        case DNXC_INT_CORR_ACT_MDB_ECC_1B_FIX:
            rc = dnxc_interrupt_handles_corrective_action_for_mdb_1b(unit, block_instance, interrupt_id,
                                                                     (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_MDB_ECC_2B_FIX:
            rc = dnxc_interrupt_handles_corrective_action_for_mdb_2b(unit, block_instance, interrupt_id,
                                                                     (dnxc_interrupt_mem_err_info *) param1, msg);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_HANDLE_OAMP_EVENT_FIFO:
            dnx_oam_oamp_event_handler(unit);
            break;

        case DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_NOT_EMPTY:
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_fifo_not_empty_interrupt_handler(unit, block_instance));
            break;

        case DNXC_INT_CORR_ACT_HBC_ECC_1B_FIX:
        case DNXC_INT_CORR_ACT_HBC_ECC_2B_FIX:
            SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_ecc_error_interrupt_handler(unit, block_instance));
            break;

        case DNXC_INT_CORR_ACT_DELETE_BDB_FIFO_FULL:
            rc = dnxc_interrupt_handles_corrective_action_for_Bdb_fifo_full(unit, block_instance, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_TCAM_SHADOW_FROM_SW_DB:
            rc = dnxc_interrupt_handles_corrective_action_tcam_shadow_from_sw_db(unit, block_instance,
                                                                                 (sand_intr_tcam_location *) param1);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_HANDLE_OAMP_STAT_EVENT_FIFO:
            dnx_oam_oamp_stat_event_handler(unit);
            break;

        case DNXC_INT_PFC_DEADLOCK_BREAKING_MECHANISM:
            rc = dnx_interrupt_handles_corrective_action_for_pfc_deadlock(unit, block_instance, (uint8 *) param1);
            SHR_IF_ERR_EXIT(rc);
            break;

        case DNXC_INT_CORR_ACT_ILKN_RX_PORT_STATUS_CHANGE:
            rc = dnx_port_ilkn_rx_status_change_interrupt_handler(unit, block_instance, (uint8 *) param1);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_CRPS_COUNTER_OVF_ERROR:
            rc = dnxc_interrupt_handles_corrective_action_CrpsCounterOvfError(unit, block_instance, interrupt_id);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_TCAM_MOVE_ECC_VALID:
            rc = dnxc_interrupt_handles_corrective_action_tcam_move_eccfailure(unit, block_instance, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_MCP_ENG_DB_A_ACCESS_ERR_INT:
            rc = dnxc_interrupt_handles_corrective_action_McpEngDb_A_AccessErr(unit, block_instance,
                                                                               *((uint32 *) param1), msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_MCP_ENG_DB_C_ACCESS_ERR_INT:
            rc = dnxc_interrupt_handles_corrective_action_McpEngDb_C_AccessErr(unit, block_instance,
                                                                               *((uint32 *) param1), msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        case DNXC_INT_CORR_ACT_IPPD_INVALID_DESTINATION_VALID:
            rc = dnx_switch_interrupt_handles_corrective_action_invalid_destination(unit, block_instance, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
#ifdef INCLUDE_XFLOW_MACSEC
        case DNXC_INT_CORR_ACT_MACSEC_SA_EXPIRY:
            rc = dnx_sec_interrupt_handles_corrective_action_sa_expiry(unit, block_instance, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
#endif
#endif
        case DNXC_INT_CORR_ACT_TCAM_QUERY_FAILURE_VALID:
            rc = dnxc_interrupt_handles_corrective_action_tcam_query_failure(unit, block_instance, msg);
            SHR_IF_ERR_EXIT(rc);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "UnKnown corrective action");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
