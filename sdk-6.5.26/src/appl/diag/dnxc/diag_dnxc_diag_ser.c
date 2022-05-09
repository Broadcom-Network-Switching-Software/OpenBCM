/** \file diag_dnxc_diag_ser.c
 *
 * diagnostic pack for dnxc mdb ser diag command
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/sand/shrextend/shrextend_debug.h>

/*appl*/
#include <appl/diag/diag.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#include <soc/sand/sand_ser_correction.h>
#include <soc/dnxc/dnxc_intr_corr_act_func.h>
#include <soc/dnxc/intr.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/mdb_ser.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#endif

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/access/sbus.h>
#include <soc/access/pci.h>
#include <soc/access/auto_generated/common_enum.h>
#include <soc/access/test/test_access.h>
#include <soc/dnxc/dnxc_access_fifodma.h>
#include <soc/access/access_regmem_util.h>
#include <soc/access/access_intr.h>
#include <soc/dnxc/dnxc_access_intr.h>
#endif

/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>

/*sal*/
#include <sal/appl/sal.h>

/*************
 * TYPE DEFS *
 *************/
#define DIAG_DNX_SER_INFO_MAX_LEN   256
#define SER_INTR_INSTANCE_BMP_WIDTH32   2
#define SER_INTR_INSTANCE_BMP_WIDTH8    8
typedef enum
{
    diag_ser_intr_type_all,
    diag_ser_intr_type_yes,
    diag_ser_intr_type_no,
} diag_ser_intr_type_e;

#ifdef BCM_ACCESS_SUPPORT
static shr_error_e
dnxc_ser_memory_get_single_mem_info_access(
    access_runtime_info_t * runtime_info,
    access_regmem_id_t regmem,
    char *pEntryInfo,
    char *pMemInfo)
{
    int rc, is_mem_alias;
    char str_mem_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    char srt_entry_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    int entry_cnt, entry_bits;
    dnxc_xor_mem_info xor_mem_info;

    access_regmem_id_t regmem_alias;
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** Constant access information for the device type */
    const access_device_regmem_t *rm_info;
    access_local_regmem_id_t local_regmem;

    SHR_FUNC_INIT_VARS(runtime_info->unit);

    SHR_NULL_CHECK(pEntryInfo, _SHR_E_PARAM, "pEntryInfo");
    SHR_NULL_CHECK(pMemInfo, _SHR_E_PARAM, "pMemInfo");

    SHR_IF_ERR_EXIT(access_regmem_global2local(runtime_info, regmem, &local_regmem)); /** Convert global to local regmem ID */
    rm_info = device_info->local_regs + local_regmem;
    is_mem_alias = 0;
    regmem_alias = regmem;
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_IS_ALIAS))
    {
        regmem_alias = rm_info->global_regmem_id;
        is_mem_alias = 1;
    }

    /*
     * if (diag_dnxc_is_mdb_physical_mem(unit, regmem)) { is_mem_alias = 0; mem_alias = mem; } 
     */

    /*
     * memory info 
     */
    if (is_mem_alias)
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Alias->%s/",
                     ACCESS_REGMEM_NAME(regmem_alias));
    }
    /*
     * Dynamic 
     */
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_ASIC_CHANGABLE))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Dynamic/");
    }
    /*
     * ReadOnly 
     */
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_RO))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "ReadOnly/");
    }
    /*
     * WriteOnly 
     */
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_WO))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "WritedOnly/");
    }

    /*
     * Cacheable 
     */
    if (access_local_regmem_cachable(runtime_info, local_regmem))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Cacheable/");
    }
    if (access_local_regmem_cache_get(runtime_info, local_regmem))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Shadowed/");
    }
    /*
     * Protection 
     */
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_HAS_PARITY))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Parity/");
    }
    if ((rm_info->flags & ACCESS_REGMEM_FLAG_HAS_ECC))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "ECC/");
    }

    if (ACCESS_REGMEM_IS_ARRAY(rm_info))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Array/");
    }

    if ((rm_info->flags & ACCESS_REGMEM_FLAG_MEMORY_TYPE_MASK) == ACCESS_REGMEM_FLAG_MEMORY_TYPE_XOR)
    {
        rc = dnxc_xor_mem_info_get(runtime_info->unit, regmem, &xor_mem_info);
        if (rc == _SHR_E_NONE)
        {
            sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                         sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "XOR(%d/%d)",
                         xor_mem_info.sram_banks_bits, xor_mem_info.entry_used_bits);
        }
        else
        {
            sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                         sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "XOR(null)");
        }
    }

    entry_cnt = rm_info->u.mem.mem_nof_elements;
    entry_bits = rm_info->width_in_bits;
    if (ACCESS_REGMEM_IS_ARRAY(rm_info))
    {
        sal_snprintf(srt_entry_info, sizeof(srt_entry_info), "%d/%d/%d",
                     rm_info->nof_array_indices, entry_cnt, entry_bits);
    }
    else
    {
        sal_snprintf(srt_entry_info, sizeof(srt_entry_info), "%d/%d", entry_cnt, entry_bits);
    }

    sal_strncpy_s(pEntryInfo, srt_entry_info, DIAG_DNX_SER_INFO_MAX_LEN);
    sal_strncpy_s(pMemInfo, str_mem_info, DIAG_DNX_SER_INFO_MAX_LEN);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_ser_memory_list_cmd_access(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ii, dynamic_flag, static_flag, alias_flag, xor_flag;
    char *severity_flag;
    char *str_block, *access_type, *protect_type;
    uint32 max_entry_num = SAL_UINT32_MAX, min_entry_num = 0;
    uint32 max_entry_bit = SAL_UINT32_MAX, min_entry_bit = 0;
    int entry_cnt, entry_bits;
    int is_alias_present;
    int number;

    access_runtime_info_t *runtime_info;
    const access_device_type_info_t *device_info;
    access_regmem_id_t regmem;
    access_block_id_t global_blk_id;
    access_local_block_id_t local_blk_id, start_id, end_id;
    const access_device_block_t *block;
    const access_device_regmem_t *rm_info;
    char str_severity[32];
    char str_mem_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    char str_entry_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    runtime_info = access_runtime_info_get(unit);
    device_info = runtime_info->device_type_info;

    SH_SAND_IS_PRESENT("AliAS", is_alias_present);
    SH_SAND_GET_STR("block", str_block);
    SH_SAND_GET_STR("ACCess", access_type);
    SH_SAND_GET_STR("SeVeRity", severity_flag);
    SH_SAND_GET_BOOL("DYNAMIC", dynamic_flag);
    SH_SAND_GET_BOOL("STatiC", static_flag);
    SH_SAND_GET_BOOL("AliAS", alias_flag);
    SH_SAND_GET_UINT32_RANGE("Num_ENTries", min_entry_num, max_entry_num);
    SH_SAND_GET_UINT32_RANGE("Num_BIts", min_entry_bit, max_entry_bit);
    SH_SAND_GET_STR("PROTEct", protect_type);
    SH_SAND_GET_BOOL("XOR", xor_flag);

    global_blk_id = access_block_type_name_match(unit, str_block);
    if (ACCESS_BLOCK_IS_VALID(runtime_info, global_blk_id))
    {
        start_id = device_info->blocks_global2local_map[global_blk_id];
        end_id = start_id + 1;
    }
    else
    {
        start_id = 0;
        end_id = device_info->nof_blocks;
    }

    PRT_TITLE_SET("SER LIST Access MEMORIES");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("BLOCK");
    PRT_COLUMN_ADD("offset");
    PRT_COLUMN_ADD("memory-name");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Severity");
    PRT_COLUMN_ADD("entry-info");
    PRT_COLUMN_ADD("memory-info");
    number = 0;
    for (local_blk_id = start_id; local_blk_id < end_id; local_blk_id++)
    {
        block = ACCESS_DEV_BLK_INFO(runtime_info, local_blk_id);
        global_blk_id = block->block_type;
        for (ii = 0; ii <= block->nof_memories; ii++)
        {
            rm_info = device_info->local_regs + block->memories_start + ii;
            regmem = rm_info->global_regmem_id;

            /*
             * Fill memories Severity
             */
            if (ACCESS_REGMEM_IS_LOW_SEVERITY(rm_info))
            {
                if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                    && (sal_strncasecmp(severity_flag, "low", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
                {
                    continue;
                }
                sal_snprintf(str_severity, sizeof(str_severity), "low");

            }
            else if (ACCESS_REGMEM_IS_MEDIUM_SEVERITY(rm_info))
            {
                if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                    && (sal_strncasecmp(severity_flag, "medium", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
                {
                    continue;
                }
                sal_snprintf(str_severity, sizeof(str_severity), "medium");

            }
            else if (ACCESS_REGMEM_IS_HIGH_SEVERITY(rm_info))
            {
                if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                    && (sal_strncasecmp(severity_flag, "high", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
                {
                    continue;
                }
                sal_snprintf(str_severity, sizeof(str_severity), "high");

            }
            else
            {
                if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                    && (sal_strncasecmp(severity_flag, "miss", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
                {
                    continue;
                }
                sal_snprintf(str_severity, sizeof(str_severity), "miss");
            }

            /*
             * Fill Alias memories 
             */
            if (ACCESS_REGMEM_IS_ALIAS(rm_info))
            {
                if (is_alias_present && !alias_flag)
                {
                    continue;
                }
            }
            else
            {
                if (is_alias_present && alias_flag)
                {
                    continue;
                }
            }
            /*
             * if (diag_dnxc_is_mdb_physical_mem(unit, mem)) { mem_alias = mem; }
             */

            /*
             * Fill dynamic/static flag 
             */
            if (ACCESS_REGMEM_IS_DYNAMIC(rm_info))
            {
                if (!dynamic_flag)
                {
                    continue;
                }
            }
            else
            {
                if (!static_flag)
                {
                    continue;
                }
            }

            /*
             * Fill accessable type: RW/RO/WO/NA 
             */
            if (sal_strncasecmp(access_type, "RW", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (ACCESS_REGMEM_IS_RO(rm_info) || ACCESS_REGMEM_IS_RO(rm_info))
                {
                    continue;
                }
            }
            else if (sal_strncasecmp(access_type, "RO", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (!ACCESS_REGMEM_IS_RO(rm_info))
                {
                    continue;
                }
            }
            else if (sal_strncasecmp(access_type, "WO", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (!ACCESS_REGMEM_IS_WO(rm_info))
                {
                    continue;
                }
            }
            else if (sal_strncasecmp(access_type, "NA", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (!ACCESS_REGMEM_IS_RO(rm_info) || !ACCESS_REGMEM_IS_WO(rm_info))
                {
                    continue;
                }
            }
            /*
             * Fill XOR memories 
             */
            if (xor_flag)
            {
                if (!ACCESS_REGMEM_IS_XOR(rm_info))
                {
                    continue;
                }
            }

            entry_cnt = rm_info->u.mem.mem_nof_elements;
            entry_bits = rm_info->width_in_bits;
            if ((entry_cnt < min_entry_num) || (entry_cnt > max_entry_num))
            {
                continue;
            }
            if ((entry_bits < min_entry_bit) || (entry_bits > max_entry_bit))
            {
                continue;
            }

            if (sal_strncasecmp(protect_type, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
            }
            else if (sal_strncasecmp(protect_type, "ecc", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (!ACCESS_REGMEM_IS_ECC(rm_info))
                {
                    continue;
                }
            }
            else if (sal_strncasecmp(protect_type, "parity", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (!ACCESS_REGMEM_IS_PARITY(rm_info))
                {
                    continue;
                }
            }
            else if (sal_strncasecmp(protect_type, "none", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                if (ACCESS_REGMEM_IS_ECC(rm_info) || ACCESS_REGMEM_IS_PARITY(rm_info))
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnxc_ser_memory_get_single_mem_info_access
                            (runtime_info, regmem, str_entry_info, str_mem_info));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", number);
            PRT_CELL_SET("%s", ACCESS_BLOCK_NAME(global_blk_id));
            PRT_CELL_SET("%d", ii);
            PRT_CELL_SET("%s", ACCESS_REGMEM_NAME(regmem));
            PRT_CELL_SET("%X", rm_info->base_address);
            PRT_CELL_SET("%s", str_severity);
            PRT_CELL_SET("%s", str_entry_info);
            PRT_CELL_SET("%s", str_mem_info);
            number++;
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_ser_intr_info_print_access(
    int unit,
    int intr_id,
    int cdsp,
    char *str_block,
    diag_ser_intr_type_e vector_type,
    diag_ser_intr_type_e handler_type,
    diag_ser_intr_type_e enable_type,
    diag_ser_intr_type_e assert_type,
    sh_sand_control_t * sand_control)
{
    int intr, nof_interrupts;
    int intr_assert, intr_enable;
    int intr_has_assert, intr_has_enable;
    uint32 flags, nof_instances, instance_num, statistics_count;
    uint32 enable_bmp[SER_INTR_INSTANCE_BMP_WIDTH32];
    uint32 assert_bmp[SER_INTR_INSTANCE_BMP_WIDTH32];
    char str_flags[256];
    int intr_print;
    access_local_regmem_id_t local_regmem;
    uint8 *block_instance_flags;
    access_block_id_t blk_type;
    access_runtime_info_t *runtime_info = SOC_CONTROL(unit)->device_access_runtime_info; /** Access information for the device */
    const access_device_type_info_t *device_info = runtime_info->device_type_info; /** constant access information for the device type */
    const access_device_regmem_t *rm_info;
    const access_device_block_t *local_block;

    soc_interrupt_db_t *interrupts;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_CLI_EXIT(_SHR_E_UNIT, "Not supported for unit %d \n", unit);
    }

    PRT_TITLE_SET("Interrupt Dump access");
    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Intr Name(instance)");
    PRT_COLUMN_ADD("Root");
    PRT_COLUMN_ADD("Flags");
    PRT_COLUMN_ADD("Action");
    PRT_COLUMN_ADD("En");
    PRT_COLUMN_ADD("On");
    PRT_COLUMN_ADD("Counter");

    blk_type = access_block_type_name_match(unit, str_block);
    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    for (intr = 0; intr < nof_interrupts; intr++)
    {
        local_regmem = (access_local_regmem_id_t) interrupts[intr].reg;
        if (local_regmem == ACCESS_INVALID_LOCAL_REGMEM_ID)
        {
            continue;
        }
        rm_info = device_info->local_regs + local_regmem;
        local_block = device_info->blocks + rm_info->local_block_id; /** The local block information of regmem */

        if (blk_type < BLOCK_NOF_BLOCKS)
        {
            if (local_block->block_type != blk_type)
            {
                continue;
            }
        }

        if (vector_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump root interrupt 
             */
            if (interrupts[intr].vector_id == 0)
            {
                continue;
            }
        }
        else if (vector_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (interrupts[intr].vector_id == 1)
            {
                continue;
            }
        }
        if (handler_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump interrupt with handler
             */
            if (interrupts[intr].func_arr == NULL)
            {
                continue;
            }
        }
        else if (handler_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump interrupt without handler
             */
            if (interrupts[intr].func_arr != NULL)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(soc_interrupt_flags_get(unit, intr, &flags));
        sal_memset(str_flags, 0, sizeof(str_flags));
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sizeof(str_flags) - sal_strlen(str_flags), "LOG");
        }
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB_BIT))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_BCM_AND_USR_CB");
        }
        else if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_OVERRIDE");
        }
        else
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_BCM_ONLY");
        }
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " FORCE_UNMASK");
        }

        instance_num = 0;
        statistics_count = 0;
        sal_memset(enable_bmp, 0, sizeof(enable_bmp));
        sal_memset(assert_bmp, 0, sizeof(assert_bmp));
        intr_has_assert = 0;
        intr_has_enable = 0;
        block_instance_flags = runtime_info->block_instance_flags + local_block->instances_start_index;
        nof_instances = local_block->nof_instances;
        for (instance_num = 0; instance_num < nof_instances; instance_num++)
        {
            if (block_instance_flags[instance_num] & ACCESS_RUNTIME_BLOCK_INSTANCE_FLAG_DISABLED)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(access_interrupt_get(unit, instance_num, &(interrupts[intr]), &intr_assert));
            SHR_IF_ERR_EXIT(access_interrupt_is_enabled(unit, instance_num, &(interrupts[intr]), &intr_enable));
            if (intr_assert)
            {
                intr_has_assert = 1;
                SHR_BITSET(assert_bmp, instance_num);
            }
            if (intr_enable)
            {
                intr_has_enable = 1;
                SHR_BITSET(enable_bmp, instance_num);
            }
            statistics_count += interrupts[intr].statistics_count[instance_num];

            if (cdsp)
            {
                if (enable_type == diag_ser_intr_type_yes)
                {
                    /*
                     * Only dump enable interrupt 
                     */
                    if (intr_enable == 0)
                    {
                        continue;
                    }
                }
                else if (enable_type == diag_ser_intr_type_no)
                {
                    /*
                     * Only dump disable interrupt 
                     */
                    if (intr_enable == 1)
                    {
                        continue;
                    }
                }

                if (assert_type == diag_ser_intr_type_yes)
                {
                    /*
                     * Only dump asserted interrupt 
                     */
                    if (intr_assert == 0)
                    {
                        continue;
                    }
                }
                else if (assert_type == diag_ser_intr_type_no)
                {
                    /*
                     * Only dump not asserted interrupt 
                     */
                    if (intr_assert == 1)
                    {
                        continue;
                    }
                }

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("-");
                PRT_CELL_SET("        [%d]", instance_num);
                PRT_CELL_SKIP(3);       /* Vector & Flag & Action */
                PRT_CELL_SET("%d", intr_enable);
                PRT_CELL_SET("%d", intr_assert);
                PRT_CELL_SET("%d", interrupts[intr].statistics_count[instance_num]);
            }
        }

        intr_print = 1;
        if (enable_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump enable interrupt 
             */
            if (intr_has_enable == 0)
            {
                intr_print = 0;
            }
        }
        else if (enable_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (intr_has_enable == 1)
            {
                intr_print = 0;
            }
        }

        if (assert_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump root interrupt 
             */
            if (intr_has_assert == 0)
            {
                intr_print = 0;
            }
        }
        else if (assert_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (intr_has_assert == 1)
            {
                intr_print = 0;
            }
        }

        if (intr_print)
        {
            char tmp[SER_INTR_INSTANCE_BMP_WIDTH8 + 3];

            if (cdsp)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
            PRT_CELL_SET("%d", intr);
#if !defined(SOC_NO_NAMES)
            PRT_CELL_SET("%s[%d]", interrupts[intr].name, instance_num);
#else
            PRT_CELL_SET("%s[%d]", "--", instance_num);
#endif
            PRT_CELL_SET("%d", interrupts[intr].vector_id);
            PRT_CELL_SET("%s", str_flags);

            if (interrupts[intr].func_arr != NULL)
            {
                PRT_CELL_SET("Set");
            }
            else
            {
                PRT_CELL_SET("NULL");
            }
            _shr_format_long_integer(tmp, enable_bmp, BITS2BYTES(instance_num));
            PRT_CELL_SET("%s", tmp);
            _shr_format_long_integer(tmp, assert_bmp, BITS2BYTES(instance_num));
            PRT_CELL_SET("%s", tmp);
            PRT_CELL_SET("%d", statistics_count);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#endif

#ifdef BCM_DNX_SUPPORT

static shr_error_e
dnxc_diag_parse_macro_type(
    int unit,
    char *str_macro_type,
    mdb_macro_types_e * macro_type,
    uint32 *macro_index)
{
    shr_error_e rc = _SHR_E_PARAM;
    char *str_macro_idx;
    SHR_FUNC_INIT_VARS(unit);

    if ((str_macro_type[0] == 'a') || (str_macro_type[0] == 'A'))
    {
        *macro_type = MDB_MACRO_A;
        rc = _SHR_E_NONE;
    }
    if ((str_macro_type[0] == 'b') || (str_macro_type[0] == 'B'))
    {
        *macro_type = MDB_MACRO_B;
        rc = _SHR_E_NONE;
    }

    if (rc == _SHR_E_NONE)
    {
        rc = _SHR_E_PARAM;
        str_macro_idx = sal_strstr(str_macro_type, ":");
        if (str_macro_idx == NULL)
        {
            str_macro_idx = sal_strstr(str_macro_type, "-");
        }
        if (str_macro_idx != NULL)
        {
            str_macro_idx++;
            *macro_index = parse_integer(str_macro_idx);
            rc = _SHR_E_NONE;
        }
    }
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_dnxc_diag_parse_mdb_physical_mem(
    int unit,
    char *mem_name,
    uint32 copyno,
    uint32 *phy_mem,
    int *blk)
{
    shr_error_e rc = _SHR_E_PARAM;
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_mdb_mem_map_t *mdb_mem_map;
    const char *mem_phy_name;
    uint32 index, n;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(phy_mem, _SHR_E_PARAM, "phy_mem");

    /** Get size of table default size */
    table_info = dnx_data_intr.ser.mdb_mem_map_info_get(unit);
    for (index = 0; index < table_info->key_size[0]; index++)
    {
        mdb_mem_map = dnx_data_intr.ser.mdb_mem_map_get(unit, index);

#if defined(BCM_ACCESS_SUPPORT)
        if (soc_dnxc_access_available(unit))
        {
            mem_phy_name = ACCESS_REGMEM_NAME(mdb_mem_map->phy_mem);
        }
        else
#endif /* BCM_ACCESS_SUPPORT */
        {
            mem_phy_name = SOC_MEM_NAME(unit, mdb_mem_map->phy_mem);
        }
        n = sal_strnlen(mem_phy_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (sal_strncasecmp(mem_name, mem_phy_name, n) == 0)
        {
            *phy_mem = mdb_mem_map->phy_mem;
#if defined(BCM_ACCESS_SUPPORT)
            if (soc_dnxc_access_available(unit))
            {
                *blk = copyno;
                rc = _SHR_E_NONE;
            }
            else
#endif
            {
                *blk = copyno + SOC_MEM_BLOCK_MIN(unit, *phy_mem);
                if (*blk <= SOC_MEM_BLOCK_MAX(unit, *phy_mem))
                {
                    rc = _SHR_E_NONE;
                }
            }
            break;
        }
    }
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_diag_ser_mdb_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *info_name;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", info_name);

    if (info_name)
    {
        if (sal_strncasecmp(info_name, "table", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(mdb_phy_table_db_info_dump(unit, 0));
        }
        else if (sal_strncasecmp(info_name, "map", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(mdb_macro_map_info_dump(unit));
        }
        else if (sal_strncasecmp(info_name, "table-detail", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(mdb_phy_table_db_info_dump(unit, 1));
        }
        else
        {
            cli_out("Invalid info option, using:\n");
            cli_out("    ser mdb info table\n");
            cli_out("    ser mdb info map\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_diag_ser_mdb_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dump_opt = 0;
    uint32 entry_data[MDB_SER_ENTRY_SIZE_IN_U32];
    uint32 ecc;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("type", dump_opt);

    if (dump_opt == 0)
    {
        /*
         * dump mdb entry by macro_type/macro_index/cluster_index/entry_index 
         */
        char *str_macro;
        mdb_macro_types_e macro_type;
        uint32 macro_index, cluster_index, entry_index;
        int macro_present, cluster_present, entry_present;

        SH_SAND_IS_PRESENT("MaCRo", macro_present);
        SH_SAND_IS_PRESENT("CLUSTER", cluster_present);
        SH_SAND_IS_PRESENT("ENTry", entry_present);
        if (!macro_present || !cluster_present || !entry_present)
        {
            cli_out("parameter of macro/cluster/entry must be present!!\n");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
        SH_SAND_GET_STR("MaCRo", str_macro);
        SH_SAND_GET_UINT32("CLUSTER", cluster_index);
        SH_SAND_GET_UINT32("ENTry", entry_index);

        SHR_IF_ERR_EXIT(dnxc_diag_parse_macro_type(unit, str_macro, &macro_type, &macro_index));

         /* coverity[callee_ptr_arith:FALSE]  */
        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_entry_get
                        (unit, macro_type, macro_index, cluster_index, entry_index, entry_data, &ecc));

        cli_out("macro=MDB_%s-%d, cluster=%d, entry=%d\n",
                dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->name, macro_index, cluster_index, entry_index);

        SHR_IF_ERR_EXIT(mdb_ser_print_cluster_entry(unit, entry_data, ecc, "    "));
    }
    else if (dump_opt == 1)
    {
        /*
         * dump mdb entry by mdb_phy_name/block/array_index/entry_index 
         */
        char *phy_mem_name;
        uint32 phy_mem;
        uint32 block, array_index, entry_index;
        int blk;
        int memory_present, block_present, array_present, entry_present;
        const char *mem_name;

        SH_SAND_IS_PRESENT("MEMory", memory_present);
        SH_SAND_IS_PRESENT("BLock", block_present);
        SH_SAND_IS_PRESENT("ARray", array_present);
        SH_SAND_IS_PRESENT("ENTry", entry_present);
        if (!memory_present || !block_present || !array_present || !entry_present)
        {
            cli_out("parameter of memory/block/array/entry must be present!!\n");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }

        SH_SAND_GET_STR("MEMory", phy_mem_name);
        SH_SAND_GET_UINT32("BLock", block);
        SH_SAND_GET_UINT32("ARray", array_index);
        SH_SAND_GET_UINT32("ENTry", entry_index);

        SHR_IF_ERR_EXIT(_dnxc_diag_parse_mdb_physical_mem(unit, phy_mem_name, block, &phy_mem, &blk));

        SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_get(unit, phy_mem, blk, array_index, entry_index, entry_data));

#if defined(BCM_ACCESS_SUPPORT)
        if (soc_dnxc_access_available(unit))
        {
            mem_name = ACCESS_REGMEM_NAME(phy_mem);
        }
        else
#endif /* BCM_ACCESS_SUPPORT */
        {
            mem_name = SOC_MEM_NAME(unit, phy_mem);
        }

        cli_out("physical memory=%s, block=%d, blk=%d, array=%d, entry=%d\n",
                mem_name, block, blk, array_index, entry_index);

        SHR_IF_ERR_EXIT(mdb_ser_print_cluster_entry(unit, entry_data, 0x100, "    "));
    }
    else if (dump_opt == 2)
    {
        /*
         * dump changed row for an mdb dh cluster by macro_type/macro_index/cluster_index/entry_index 
         */
        char *str_macro, *str_table;
        mdb_macro_types_e macro_type;
        uint32 macro_index, cluster_index;
        int macro_present, cluster_present, table_present;

        SH_SAND_IS_PRESENT("MaCRo", macro_present);
        SH_SAND_IS_PRESENT("CLUSTER", cluster_present);
        SH_SAND_IS_PRESENT("Table", table_present);

        if (table_present)
        {
            mdb_physical_table_e mdb_phy_table_id;
            SH_SAND_GET_STR("Table", str_table);

            SHR_IF_ERR_EXIT(mdb_physical_table_string_to_id(unit, str_table, &mdb_phy_table_id));

            SHR_IF_ERR_EXIT(mdb_phy_table_cluster_chg_dump(unit, mdb_phy_table_id));
        }
        else if (macro_present)
        {
            if (!cluster_present)
            {
                cli_out("parameter of cluster must be present!!\n");
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }

            SH_SAND_GET_STR("MaCRo", str_macro);
            SH_SAND_GET_UINT32("CLUSTER", cluster_index);

            SHR_IF_ERR_EXIT(dnxc_diag_parse_macro_type(unit, str_macro, &macro_type, &macro_index));

            SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_chg_dump(unit, macro_type, macro_index, cluster_index));
        }
        else
        {
            cli_out("parameter of macro/cluster or table must be present!!\n");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }
    }
    else if (dump_opt == 3)
    {
        /*
         * direct dh entry cache consistency check 
         */
        char *str_macro, *str_table;
        mdb_macro_types_e macro_type;
        uint32 macro_index, cluster_index;
        int macro_present, cluster_present, table_present;

        SH_SAND_IS_PRESENT("MaCRo", macro_present);
        SH_SAND_IS_PRESENT("CLUSTER", cluster_present);
        SH_SAND_IS_PRESENT("Table", table_present);

        if (table_present)
        {
            mdb_physical_table_e mdb_phy_table_id;
            SH_SAND_GET_STR("Table", str_table);

            SHR_IF_ERR_EXIT(mdb_physical_table_string_to_id(unit, str_table, &mdb_phy_table_id));

            SHR_IF_ERR_EXIT(mdb_phy_table_cluster_cache_check(unit, mdb_phy_table_id));
        }
        else if (macro_present)
        {
            if (!cluster_present)
            {
                cli_out("parameter of cluster must be present!!\n");
                SHR_IF_ERR_EXIT(_SHR_E_PARAM);
            }

            SH_SAND_GET_STR("MaCRo", str_macro);
            SH_SAND_GET_UINT32("CLUSTER", cluster_index);

            SHR_IF_ERR_EXIT(dnxc_diag_parse_macro_type(unit, str_macro, &macro_type, &macro_index));

            SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_cache_check(unit, macro_type, macro_index, cluster_index));
        }
        else
        {
            cli_out("parameter of macro/cluster or table must be present!!\n");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }

    }
    else
    {
        cli_out("Invalid input\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_diag_ser_mdb_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 dump_opt = 0;
    uint32 entry_data[4];
    uint32 *array_uint32;
    uint32 is_ecc_present, ecc;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("type", dump_opt);
    SH_SAND_GET_ARRAY32("DaTa", array_uint32);

    SH_SAND_IS_PRESENT("ECC", is_ecc_present);
    if (is_ecc_present)
    {
        SH_SAND_GET_UINT32("ECC", ecc);
    }
    else
    {
         /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
        calc_ecc(unit, MDB_SER_ENTRY_DATA_SIZE_IN_BITS, array_uint32, &ecc);
    }

    /*
     * Proper treatment of sign extension 
     */
    array_uint32[MDB_SER_ENTRY_DATA_SIZE_IN_BITS / 32] &= 0xFFFFFF;

    if (dump_opt == 0)
    {
        /*
         * dump mdb entry by macro_type/macro_index/cluster_index/entry_index 
         */
        char *str_macro_type;
        mdb_macro_types_e macro_type;
        uint32 macro_index, cluster_index, entry_index;
        int macro_present, cluster_present, entry_present;

        SH_SAND_IS_PRESENT("MaCRo", macro_present);
        SH_SAND_IS_PRESENT("CLUSTER", cluster_present);
        SH_SAND_IS_PRESENT("ENTry", entry_present);
        if (!macro_present || !cluster_present || !entry_present)
        {
            cli_out("parameter of macro/cluster/entry must be present!!");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }

        SH_SAND_GET_STR("MaCRo", str_macro_type);
        SH_SAND_GET_UINT32("CLUSTER", cluster_index);
        SH_SAND_GET_UINT32("ENTry", entry_index);

        SHR_IF_ERR_EXIT(dnxc_diag_parse_macro_type(unit, str_macro_type, &macro_type, &macro_index));

        SHR_IF_ERR_EXIT(mdb_dh_macro_cluster_entry_set
                        (unit, macro_type, macro_index, cluster_index, entry_index, array_uint32, ecc));
    }
    else if (dump_opt == 1)
    {
        /*
         * dump mdb entry by mdb_phy_name/block/array_index/entry_index 
         */
        char *phy_mem_name;
        uint32 phy_mem;
        uint32 block, array_index, entry_index;
        int blk;
        int memory_present, block_present, array_present, entry_present;

        SH_SAND_IS_PRESENT("MEMory", memory_present);
        SH_SAND_IS_PRESENT("BLock", block_present);
        SH_SAND_IS_PRESENT("ARray", array_present);
        SH_SAND_IS_PRESENT("ENTry", entry_present);
        if (!memory_present || !block_present || !array_present || !entry_present)
        {
            cli_out("parameter of memory/block/array/entry must be present!!\n");
            SHR_IF_ERR_EXIT(_SHR_E_PARAM);
        }

        SH_SAND_GET_STR("MEMory", phy_mem_name);
        SH_SAND_GET_UINT32("BLock", block);
        SH_SAND_GET_UINT32("ARray", array_index);
        SH_SAND_GET_UINT32("ENTry", entry_index);

        SHR_IF_ERR_EXIT(_dnxc_diag_parse_mdb_physical_mem(unit, phy_mem_name, block, &phy_mem, &blk));

#if defined(BCM_ACCESS_SUPPORT)
        if (soc_dnxc_access_available(unit))
        {
            access_runtime_info_t *runtime_info;
            runtime_info = access_runtime_info_get(unit);
            SHR_IF_ERR_EXIT(access_field_set(unit, mDDHA_MACRO_0_PHYSICAL_ENTRY_BANK_fDATA, entry_data, array_uint32));
            SHR_IF_ERR_EXIT(access_field32_set(runtime_info, mDDHA_MACRO_0_PHYSICAL_ENTRY_BANK_fECC, entry_data, ecc));
        }
        else
#endif /* BCM_ACCESS_SUPPORT */
        {
            soc_mem_field_set(unit, phy_mem, entry_data, DATAf, array_uint32);
            soc_mem_field32_set(unit, phy_mem, entry_data, ECCf, ecc);
        }
        SHR_IF_ERR_EXIT(mdb_dh_physical_mem_entry_set(unit, phy_mem, blk, array_index, entry_index, entry_data));
    }
    else
    {
        cli_out("Invalid input\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t Dnxc_diag_ser_mdb_info_man = {
    .brief = "SER MDB info commands \n",
    .full = "ser mdb info <info-opt>\n",
    .synopsis = " [info-opt=<table | map>]",
    .examples = "table\n" "map"
};

static sh_sand_man_t Dnxc_diag_ser_mdb_read_man = {
    .brief = "SER MDB dump commands \n",
    .full = "SER MDB dump commands \n",
    .examples = "macro=a-1 cluster=7 entry=0\n"
        "1 mem=DDHA_MACRO_0_PHYSICAL_ENTRY_BANK block=0 array=7 entry=0\n" "2 table=INLIF_1"
};

static sh_sand_man_t Dnxc_diag_ser_mdb_write_man = {
    .brief = "SER MDB dump commands \n",
    .full = "SER MDB dump commands \n",
    .examples = "macro=a-1 cluster=7 entry=0 data=0x12345678abcdef\n"
        "1 mem=DDHA_MACRO_0_PHYSICAL_ENTRY_BANK block=0 array=7 entry=0 data=0x12345678abcdef ecc=7"
};

static sh_sand_option_t Dnxc_diag_ser_mdb_info_options[] = {
    {"type", SAL_FIELD_TYPE_STR, "Configure mdb ser info option<table or map>", "", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_option_t Dnxc_diag_ser_mdb_read_options[] = {
    {"type", SAL_FIELD_TYPE_UINT32, "Configure mdb ser read option<0: read by macro, 1:read by physical memory>",
     "0", NULL, NULL, SH_SAND_ARG_FREE},
    {"MaCRo", SAL_FIELD_TYPE_STR, "Configure mdb ser read DH macro", "a:1", NULL},
    {"CLUSTER", SAL_FIELD_TYPE_UINT32, "Configure mdb ser read DH cluster index", "0", NULL},
    {"MEMory", SAL_FIELD_TYPE_STR, "Configure mdb ser read memory name", "", NULL},
    {"BLock", SAL_FIELD_TYPE_UINT32, "Configure mdb ser read memory block", "0", NULL},
    {"ARray", SAL_FIELD_TYPE_UINT32, "Configure mdb ser read memory array index", "0", NULL},
    {"ENTry", SAL_FIELD_TYPE_UINT32, "Configure mdb ser read memory entry index", "0", NULL},
    {"Table", SAL_FIELD_TYPE_STR, "MDB physical table name", "", NULL},
    {NULL}
};

static sh_sand_option_t Dnxc_diag_ser_mdb_write_options[] = {
    {"type", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write option", "0", NULL, NULL, SH_SAND_ARG_FREE},
    {"MaCRo", SAL_FIELD_TYPE_STR, "Configure mdb ser write DH macro", "", NULL},
    {"CLUSTER", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write DH cluster index", "0", NULL},
    {"MEMory", SAL_FIELD_TYPE_STR, "Configure mdb ser write memory name", "", NULL},
    {"BLock", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write memory block", "0", NULL},
    {"ARray", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write memory array index", "0", NULL},
    {"ENTry", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write memory entry index", "0", NULL},
    {"DaTa", SAL_FIELD_TYPE_ARRAY32, "Up to 120bits of the memory entry data", "0", NULL},
    {"ECC", SAL_FIELD_TYPE_UINT32, "Configure mdb ser write memory entry ecc", "0", NULL, "0-255"},
    {NULL}
};

sh_sand_cmd_t dnxc_diag_mdb_ser_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"Info", dnxc_diag_ser_mdb_info_cmd, NULL, Dnxc_diag_ser_mdb_info_options, &Dnxc_diag_ser_mdb_info_man},
    {"Read", dnxc_diag_ser_mdb_read_cmd, NULL, Dnxc_diag_ser_mdb_read_options, &Dnxc_diag_ser_mdb_read_man},
    {"Write", dnxc_diag_ser_mdb_write_cmd, NULL, Dnxc_diag_ser_mdb_write_options, &Dnxc_diag_ser_mdb_write_man},
    {NULL}
};

static sh_sand_man_t dnxc_diag_ser_mdb_man = {
    .brief = "SER MDB commands \n",
    .full = "SER MDB commands\n",
};

static shr_error_e
dnxc_diag_ser_tcam_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *str_mode;
    int mode_present, core_id;
    uint32 bank_id;
    uint32 dump_mode = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("BaNK", bank_id);
    SH_SAND_GET_INT32("CORE", core_id);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    dump_mode = 0;
    SH_SAND_IS_PRESENT("mode", mode_present);
    if (mode_present)
    {
        SH_SAND_GET_STR("mode", str_mode);
        if (str_mode)
        {
            if (sal_strncasecmp(str_mode, "shadow", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                dump_mode = 1;
            }
            else if (sal_strncasecmp(str_mode, "check", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                dump_mode = 2;
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_ser_tcam_access_entry_key_dump(unit, core_id, bank_id, dump_mode));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_diag_ser_tcam_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 bank_id, entry_id, valid, key_mode;
    uint32 *array_key, *array_mask;
    int core_id;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("BaNK", bank_id);
    SH_SAND_GET_UINT32("ENTry", entry_id);
    SH_SAND_GET_UINT32("ValiD", valid);
    SH_SAND_GET_UINT32("MoDe", key_mode);
    SH_SAND_GET_ARRAY32("KEY", array_key);
    SH_SAND_GET_ARRAY32("MAsK", array_mask);
    SH_SAND_GET_INT32("CORE", core_id);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_ser_tcam_access_write_hw_entry
                    (unit, core_id, bank_id, entry_id, valid, key_mode, array_key, array_mask));

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnxc_diag_ser_tcam_dump_man = {
    .brief = "SER TCAM dump commands \n",
    .full = "Dump tcam valid entry for specific bank\n"
        "       ser tcam dump bank=<bank_id> hw:  Dump tcam entry from HW\n"
        "       ser tcam dump bank=<bank_id> shadow:  Dump tcam entry from SHADOW\n"
        "       ser tcam dump bank=<bank_id> check:  Check if shadow match hw value\n",
    .examples = "core=0 bank=0\n" "core=0 bank=1 shadow\n" "core=0 bank=2 hw\n" "core=0 bank=12 check",
};

static sh_sand_man_t dnxc_diag_ser_tcam_write_man = {
    .brief = "SER TCAM write commands \n",
    .full = "Write a tcam entry at specific offset of one bank\n"
        "Parameters:   [BaNK=<bank-id>]:  valid range is 0-15\n"
        "              [ENTry=<offset>]:  0-2047 for big bank, and 0-255 for small bank\n"
        "              [KEY=<key vlaue>]:  vlaue of tcam key entry\n"
        "              [MAsK=<key vlaue>]:  vlaue of tcam mask entry\n"
        "              [ValiD=<0/1/2/3>]:  2 valid bits represent 2 half entry\n"
        "              [MoDe=<0/1/2/3>]:  0-HALF; 1-Single; 3-Double; 2-Invalid\n",
    .examples = "core=0 bank=0 entry=2 key=0x0123456789 mask=0x9876543210 valid=1 mode=0",

};

static sh_sand_option_t dnxc_diag_ser_tcam_dump_options[] = {
    {"mode", SAL_FIELD_TYPE_STR, "Configure tcam dump mode<hw|shadow|check>", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"CORE", SAL_FIELD_TYPE_INT32, "Configure core id of TCAM", "0", NULL, "0-1"},
    {"BaNK", SAL_FIELD_TYPE_UINT32, "Configure tcam bank ID", "0", NULL, "0-15"},

    {NULL}
};

static sh_sand_option_t dnxc_diag_ser_tcam_write_options[] = {
    {"CORE", SAL_FIELD_TYPE_INT32, "Configure core id of TCAM", "0", NULL, "0-1"},
    {"BaNK", SAL_FIELD_TYPE_UINT32, "Configure tcam bank ID", "0", NULL, "0-15"},
    {"ENTry", SAL_FIELD_TYPE_UINT32, "Configure tcam entry offset", "0", NULL},
    {"KEY", SAL_FIELD_TYPE_ARRAY32, "Up to 160bits of the TCAM key data", "0", NULL},
    {"MAsK", SAL_FIELD_TYPE_ARRAY32, "Up to 160bits of the TCAM mask data", "0", NULL},
    {"ValiD", SAL_FIELD_TYPE_UINT32, "Configure tcam entry valid bit", "0", NULL, "0-3"},
    {"MoDe", SAL_FIELD_TYPE_UINT32, "Configure tcam entry key mode", "0", NULL, "0-3"},
    {NULL}
};

sh_sand_cmd_t dnxc_diag_ser_tcam_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"DuMP", dnxc_diag_ser_tcam_dump_cmd, NULL, dnxc_diag_ser_tcam_dump_options, &dnxc_diag_ser_tcam_dump_man},
    {"Write", dnxc_diag_ser_tcam_write_cmd, NULL, dnxc_diag_ser_tcam_write_options, &dnxc_diag_ser_tcam_write_man},
    {NULL}
};

static sh_sand_man_t dnxc_diag_ser_tcam_man = {
    .brief = "SER TCAM commands \n",
    .full = "SER TCAM commands\n",
};

#endif

static int
diag_dnxc_is_mdb_physical_mem(
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

static shr_error_e
dnxc_ser_memory_get_single_mem_info(
    int unit,
    soc_mem_t mem,
    char *pBlockInfo,
    uint32 *pAddress,
    char *pEntryInfo,
    char *pMemInfo)
{
    uint16 block;
    int rc, is_mem_alias, blk_type;
    char str_mem_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    char srt_entry_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    soc_mem_t mem_alias;
    int copyno;
    uint8 acc_type;
    uint32 address, element_skip;
    int index_cnt, numels, entry_bytes;
    dnxc_xor_mem_info xor_mem_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pBlockInfo, _SHR_E_PARAM, "pBlockInfo");
    SHR_NULL_CHECK(pAddress, _SHR_E_PARAM, "pAddress");
    SHR_NULL_CHECK(pEntryInfo, _SHR_E_PARAM, "pEntryInfo");
    SHR_NULL_CHECK(pMemInfo, _SHR_E_PARAM, "pMemInfo");
    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    is_mem_alias = 0;
    mem_alias = SOC_MEM_ALIAS_MAP(unit, mem);
    if (SOC_MEM_IS_VALID(unit, mem_alias))
    {
        is_mem_alias = 1;
    }
    else
    {
        mem_alias = mem;
    }
    if (diag_dnxc_is_mdb_physical_mem(unit, mem))
    {
        is_mem_alias = 0;
        mem_alias = mem;
    }

    /*
     * Block 
     */
    /*
     * SOC_BLOCK_TYPE ?? 
     */
    block = SOC_MEM_BLOCK_MIN(unit, mem_alias);
    blk_type = SOC_BLOCK_TYPE(unit, block);
    copyno = SOC_MEM_BLOCK_ANY(unit, mem_alias);
    address = soc_mem_addr_get(unit, mem_alias, 0, copyno, 0, &acc_type);

    /*
     * memory info 
     */
    if (is_mem_alias)
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Alias->%s/",
                     SOC_MEM_NAME(unit, mem_alias));
    }
    /*
     * Dynamic 
     */
    if (soc_mem_is_signal(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Dynamic/");
    }
    /*
     * ReadOnly 
     */
    if (soc_mem_is_readonly(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "ReadOnly/");
    }
    /*
     * WriteOnly 
     */
    if (soc_mem_is_writeonly(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "WritedOnly/");
    }
    /*
     * Internal 
     */
    if (SOC_MEM_IS_INTERNAL(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Internal/");
    }
    /*
     * Cacheable 
     */
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Cacheable/");
    }
    if (soc_mem_cache_get(unit, mem_alias, SOC_BLOCK_ALL))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Shadowed/");
    }
    /*
     * Protection 
     */
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Parity/");
    }
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "ECC/");
    }

    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                     sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Array/");
    }

    if (SOC_MEM_TYPE(unit, mem_alias) == SOC_MEM_TYPE_XOR)
    {
        rc = dnxc_xor_mem_info_get(unit, mem, &xor_mem_info);
        if (rc == _SHR_E_NONE)
        {
            sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                         sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "XOR(%d/%d)",
                         xor_mem_info.sram_banks_bits, xor_mem_info.entry_used_bits);
        }
        else
        {
            sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)),
                         sizeof(str_mem_info) - sal_strnlen(str_mem_info, sizeof(str_mem_info)), "XOR(null)");
        }
    }

    entry_bytes = soc_mem_entry_bytes(unit, mem_alias);
    index_cnt = soc_mem_index_count(unit, mem_alias);
    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        numels = SOC_MEM_NUMELS(unit, mem_alias);
        element_skip = SOC_MEM_ELEM_SKIP(unit, mem_alias);
        sal_snprintf(srt_entry_info, sizeof(srt_entry_info), "%d/%d/%d/%d",
                     index_cnt, numels, entry_bytes, element_skip);
    }
    else
    {
        sal_snprintf(srt_entry_info, sizeof(srt_entry_info), "%d/%d", index_cnt, entry_bytes);
    }

    sal_strncpy(pBlockInfo, soc_block_name_lookup_ext(blk_type, unit), DIAG_DNX_SER_INFO_MAX_LEN);
    sal_strncpy_s(pEntryInfo, srt_entry_info, DIAG_DNX_SER_INFO_MAX_LEN);
    sal_strncpy_s(pMemInfo, str_mem_info, DIAG_DNX_SER_INFO_MAX_LEN);
    *pAddress = address;

exit:
    SHR_FUNC_EXIT;
}

STATIC int
dnxc_ser_table_info_single_export(
    int unit,
    soc_mem_t mem,
    char *delimiters)
{
    int rv = SOC_E_NONE;
    uint16 block;
    int blk_type;
    int protect;
    char *str_protect[] = { "none", "PARITY", "ECC" };
    soc_mem_severe_t severity;
    int numels, entry_dw, index_cnt, cache_size;
    int copyno;
    uint8 acc_type;
    uint32 address, end_address;
    soc_mem_t mem_alias;
    int bit_length, is_mem_alias;
    char *ser_action;

    if (!SOC_MEM_IS_VALID(unit, mem))
    {
        return rv;
    }

    is_mem_alias = 0;
    mem_alias = SOC_MEM_ALIAS_MAP(unit, mem);
    if (SOC_MEM_IS_VALID(unit, mem_alias))
    {
        is_mem_alias = 1;
    }
    else
    {
        mem_alias = mem;
    }

    /*
     * Memory Name 
     */
    cli_out("%s", SOC_MEM_NAME(unit, mem));
    cli_out("%s", delimiters);

    /*
     * Block 
     */
    /*
     * SOC_BLOCK_TYPE ?? 
     */
    block = SOC_MEM_BLOCK_MIN(unit, mem_alias);
    blk_type = SOC_BLOCK_TYPE(unit, block);
    cli_out("%s", soc_block_name_lookup_ext(blk_type, unit));
    cli_out("%s", delimiters);

    /*
     * ReadOnly 
     */
    if (soc_mem_is_readonly(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /*
     * WriteOnly 
     */
    if (soc_mem_is_writeonly(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /*
     * Internal 
     */
    if (SOC_MEM_IS_INTERNAL(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /*
     * Dynamic 
     */
    if ((SOC_MEM_INFO(unit, mem_alias).flags & SOC_MEM_FLAG_SIGNAL))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /*
     * Cacheable 
     */
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }

    /*
     * Use FF & Protection 
     */
    protect = 0;
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf) ||
        (SOC_IS_ARADPLUS_AND_BELOW(unit) && (mem_alias == SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm)))
    {
        protect = 1;
    }
    if (SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf))
    {
        protect = 2;
    }
    /*
     * Use FF 
     */
    if (protect == 0)
    {
        cli_out("TRUE%s", delimiters);
    }
    else
    {
        cli_out("FALSE%s", delimiters);
    }
    /*
     * Protection 
     */
    cli_out("%s%s", str_protect[protect], delimiters);

    /*
     * Type: Config/Data/Control ???? 
     */
    cli_out("%s", delimiters);

    /*
     * Severity 
     */
    severity = SOC_MEM_SEVERITY(unit, mem_alias);
    if (severity == SOC_MEM_SEVERE_LOW)
    {
        cli_out("Low%s", delimiters);
    }
    else if (severity == SOC_MEM_SEVERE_MEDIUM)
    {
        cli_out("Medium%s", delimiters);
    }
    else if (severity == SOC_MEM_SEVERE_HIGH)
    {
        cli_out("Severe%s", delimiters);
    }
    else
    {
        cli_out("Other%s", delimiters);
    }

    /*
     * Alias
     */
    if (is_mem_alias)
    {
        cli_out("%s", SOC_MEM_NAME(unit, mem_alias));
        cli_out("%s", delimiters);
    }
    else
    {
        cli_out("%s", delimiters);
    }

    /*
     * 
     */
    bit_length = soc_mem_entry_bits(unit, mem_alias);
    numels = 1;
    if (SOC_MEM_IS_ARRAY(unit, mem_alias))
    {
        numels = SOC_MEM_ARRAY_INFO(unit, mem_alias).numels;
    }
    entry_dw = soc_mem_entry_words(unit, mem_alias);
    index_cnt = soc_mem_index_count(unit, mem_alias);

    /*
     * Size(bits) 
     */
    cli_out("%d%s", bit_length * numels * index_cnt, delimiters);

    /*
     * CacheSize(bits) 
     */
    cache_size = 0;
    if (soc_mem_is_cachable(unit, mem_alias))
    {
        cache_size = numels * index_cnt * entry_dw * 4 * 8;
    }
    cli_out("%d%s", cache_size, delimiters);

    /*
     * soc_mem_info_t* meminfo;
     */
    /*
     * Address 
     */
    copyno = SOC_MEM_BLOCK_ANY(unit, mem_alias);
    address = soc_mem_addr_get(unit, mem_alias, 0, copyno, 0, &acc_type);
    cli_out("0x%x%s", address, delimiters);

    /*
     * End Address 
     */
    end_address = address + index_cnt - 1;
    cli_out("0x%x%s", end_address, delimiters);

    /*
     * Lines 
     */
    cli_out("%d%s", index_cnt, delimiters);

    /*
     * Length(bits) 
     */
    bit_length = soc_mem_entry_bits(unit, mem_alias);
    cli_out("%d%s", bit_length, delimiters);

    /*
     * SER Action -- See dcmn_mem_decide_corrective_action 
     */
    if (is_mem_alias)
    {
        cli_out("Alias, No Action%s%s%s%s", delimiters, delimiters, delimiters, delimiters);
    }
    else
    {
        if (protect == 0)
        {
            cli_out("Un-protected RegArray, No Action%s%s%s%s", delimiters, delimiters, delimiters, delimiters);
        }
        else if (protect == 1)
        {
            /*
             * PARITY 
             */
            cli_out("PARITY protected%s", delimiters);
            ser_action = dnxc_mem_ser_ecc_action_info_get(unit, mem, 0);
            cli_out("%s%s%s%s", ser_action, delimiters, delimiters, delimiters);
        }
        else
        {
            /*
             * ECC 
             */
            cli_out("ECC protected%s%s", delimiters, delimiters);
            ser_action = dnxc_mem_ser_ecc_action_info_get(unit, mem, 1);
            cli_out("%s%s", ser_action, delimiters);
            ser_action = dnxc_mem_ser_ecc_action_info_get(unit, mem, 0);
            cli_out("%s%s", ser_action, delimiters);
        }
    }

    cli_out("\n");
    return rv;
}

static shr_error_e
dnxc_ser_memory_list_cmd_normal(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *delimiters = "&";
    char *block_n, *access_type, *protect_type;
    char block_str[PRT_COLUMN_WIDTH_BIG];
    int dynamic_flag, static_flag, alias_flag, export_flag, xor_flag;
    soc_mem_t mem, mem_alias;
    uint32 number;
    uint32 max_entry_num = SAL_UINT32_MAX, min_entry_num = 0;
    uint32 max_entry_bit = SAL_UINT32_MAX, min_entry_bit = 0;
    int entry_cnt, entry_bits;
    int is_alias_present;
    char str_mem_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    char str_entry_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    char str_block_info[DIAG_DNX_SER_INFO_MAX_LEN] = { 0 };
    uint32 address = 0;
    char *severity_flag;
    char str_severity[32];
    soc_mem_severe_t mem_severity;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("AliAS", is_alias_present);
    SH_SAND_GET_STR("block", block_n);
    SH_SAND_GET_STR("ACCess", access_type);
    SH_SAND_GET_STR("SeVeRity", severity_flag);
    SH_SAND_GET_BOOL("DYNAMIC", dynamic_flag);
    SH_SAND_GET_BOOL("STatiC", static_flag);
    SH_SAND_GET_BOOL("AliAS", alias_flag);
    SH_SAND_GET_UINT32_RANGE("Num_ENTries", min_entry_num, max_entry_num);
    SH_SAND_GET_UINT32_RANGE("Num_BIts", min_entry_bit, max_entry_bit);
    SH_SAND_GET_STR("PROTEct", protect_type);
    SH_SAND_GET_BOOL("EXPort", export_flag);
    SH_SAND_GET_BOOL("XOR", xor_flag);

    if (export_flag)
    {
        for (mem = 0; mem < NUM_SOC_MEM; mem++)
        {
            if (SOC_MEM_IS_VALID(unit, mem))
            {
                SHR_IF_ERR_EXIT(dnxc_ser_table_info_single_export(unit, mem, delimiters));
            }
        }
        SHR_EXIT();
    }

    cli_out("%-16s: 0x%-8x --->> 0x%-8x\n", "Num_ENTries", min_entry_num, max_entry_num);
    cli_out("%-16s: 0x%-8x --->> 0x%-8x\n", "Num_BIts", min_entry_bit, max_entry_bit);

    PRT_TITLE_SET("SER LIST MEMORIES");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("memory-name");
    PRT_COLUMN_ADD("BLOCK");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("Severity");
    PRT_COLUMN_ADD("entry-info");
    PRT_COLUMN_ADD("memory-info");
    number = 0;
    for (mem = 0; mem < NUM_SOC_MEM; mem++)
    {
        if (!SOC_MEM_IS_VALID(unit, mem))
        {
            continue;
        }

        mem_severity = SOC_MEM_SEVERITY(unit, mem);
        if (mem_severity == SOC_MEM_SEVERE_LOW)
        {
            if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                && (sal_strncasecmp(severity_flag, "low", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
            {
                continue;
            }
            sal_snprintf(str_severity, sizeof(str_severity), "low");

        }
        else if (mem_severity == SOC_MEM_SEVERE_MEDIUM)
        {
            if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                && (sal_strncasecmp(severity_flag, "medium", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
            {
                continue;
            }
            sal_snprintf(str_severity, sizeof(str_severity), "medium");

        }
        else if (mem_severity == SOC_MEM_SEVERE_HIGH)
        {
            if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                && (sal_strncasecmp(severity_flag, "high", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
            {
                continue;
            }
            sal_snprintf(str_severity, sizeof(str_severity), "high");

        }
        else if (mem_severity == SOC_MEM_SEVERE_MISSING)
        {
            if ((sal_strncasecmp(severity_flag, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)
                && (sal_strncasecmp(severity_flag, "miss", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0))
            {
                continue;
            }
            sal_snprintf(str_severity, sizeof(str_severity), "miss");

        }
        else
        {
            sal_snprintf(str_severity, sizeof(str_severity), "Err-%d", mem_severity);
        }

        /*
         * Fill Alias memories 
         */
        mem_alias = SOC_MEM_ALIAS_MAP(unit, mem);
        if (!SOC_MEM_IS_VALID(unit, mem_alias))
        {
            mem_alias = mem;
            if (is_alias_present && alias_flag)
            {
                continue;
            }
        }
        else
        {
            if (is_alias_present && !alias_flag)
            {
                continue;
            }
        }
        if (diag_dnxc_is_mdb_physical_mem(unit, mem))
        {
            mem_alias = mem;
        }

        /*
         * Fill blocks
         */
        if (diag_sand_mem_blocks_get(unit, mem, block_str, block_n) != _SHR_E_NONE)
        {
            continue;
        }

        /*
         * Fill dynamic/static flag 
         */
        if (soc_mem_is_signal(unit, mem_alias))
        {
            if (!dynamic_flag)
            {
                continue;
            }
        }
        else
        {
            if (!static_flag)
            {
                continue;
            }
        }

        /*
         * Fill accessable type: RW/RO/WO/NA 
         */
        if (sal_strncasecmp(access_type, "RW", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if ((soc_mem_is_readonly(unit, mem_alias)) || (soc_mem_is_writeonly(unit, mem_alias)))
            {
                continue;
            }
        }
        else if (sal_strncasecmp(access_type, "RO", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (!soc_mem_is_readonly(unit, mem_alias))
            {
                continue;
            }
        }
        else if (sal_strncasecmp(access_type, "WO", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (!soc_mem_is_writeonly(unit, mem_alias))
            {
                continue;
            }
        }
        else if (sal_strncasecmp(access_type, "NA", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (!(soc_mem_is_readonly(unit, mem_alias)) || !(soc_mem_is_writeonly(unit, mem_alias)))
            {
                continue;
            }
        }

        /*
         * Fill XOR memories 
         */
        if (xor_flag)
        {
            if (SOC_MEM_TYPE(unit, mem) != SOC_MEM_TYPE_XOR)
            {
                continue;
            }
        }

        entry_cnt = soc_mem_index_count(unit, mem_alias);
        entry_bits = soc_mem_entry_bytes(unit, mem_alias);
        if ((entry_cnt < min_entry_num) || (entry_cnt > max_entry_num))
        {
            continue;
        }
        if ((entry_bits < min_entry_bit) || (entry_bits > max_entry_bit))
        {
            continue;
        }

        if (sal_strncasecmp(protect_type, "all", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
        }
        else if (sal_strncasecmp(protect_type, "ecc", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (!SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf))
            {
                continue;
            }
        }
        else if (sal_strncasecmp(protect_type, "parity", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if (!SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf))
            {
                continue;
            }
        }
        else if (sal_strncasecmp(protect_type, "none", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            if ((SOC_MEM_FIELD_VALID(unit, mem_alias, ECCf)) || (SOC_MEM_FIELD_VALID(unit, mem_alias, PARITYf)))
            {
                continue;
            }
        }
        else
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnxc_ser_memory_get_single_mem_info(unit, mem, str_block_info, &address,
                                                            str_entry_info, str_mem_info));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", number);
        PRT_CELL_SET("%s", SOC_MEM_NAME(unit, mem));
        PRT_CELL_SET("%s", str_block_info);
        PRT_CELL_SET("%X", address);
        PRT_CELL_SET("%s", str_severity);
        PRT_CELL_SET("%s", str_entry_info);
        PRT_CELL_SET("%s", str_mem_info);
        number++;
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_ser_memory_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_ACCESS_SUPPORT
    if (soc_dnxc_access_available(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_ser_memory_list_cmd_access(unit, args, sand_control));
    }
    else
#endif
    {
        SHR_IF_ERR_EXIT(dnxc_ser_memory_list_cmd_normal(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t sand_diag_num_entry_bits_enum[] = {
    {"max", SAL_UINT32_MAX, "Max value for Num_ENTries/Num_BIts", NULL},
    {NULL}
};

static sh_sand_option_t dnxc_ser_memory_list_options[] = {
    {"BLock", SAL_FIELD_TYPE_STR, "Block name or any substring of it to filter memories", ""},
    {"DYNAMIC", SAL_FIELD_TYPE_BOOL, "If dynamic memories is listed", "Yes"},
    {"STatiC", SAL_FIELD_TYPE_BOOL, "If static memories is listed", "Yes"},
    {"AliAS", SAL_FIELD_TYPE_BOOL, "If only alias memories is listed", "No"},
    {"Num_ENTries", SAL_FIELD_TYPE_UINT32, "The range of entry numbers of list memories", "0-max",
     (void *) sand_diag_num_entry_bits_enum, "0-max"},
    {"Num_BIts", SAL_FIELD_TYPE_UINT32, "The range of entry bit length of list memories", "0-max",
     (void *) sand_diag_num_entry_bits_enum, "0-max"},
    {"PROTEct", SAL_FIELD_TYPE_STR, "The protect type of list memories, ECC/Parity/none", "all"},
    {"EXPort", SAL_FIELD_TYPE_BOOL, "List all memory with the format of export", "No"},
    {"XOR", SAL_FIELD_TYPE_BOOL, "List only XOR memories", "No"},
    {"ACCess", SAL_FIELD_TYPE_STR, "The access property of list memories, RW/RO/WO/NA", ""},
    {"SeVeRity", SAL_FIELD_TYPE_STR, "The memory severity info, miss/low/medium/high/all", "all"},
    {NULL}
};

static sh_sand_man_t dnxc_ser_memory_list_man = {
    .brief = "SER MEMory LIST commands\n",
    .full = "List memories based on input condition\n",
    .examples = "Num_ENTries=0-32 Num_BIts=8-16\n"
        "BLock=sch\n" "AliAS=yes\n" "PROTEct=ecc ACCess=RW STatiC=yes\n"
        "SeVeRity=high\n" "PROTEct=ecc ACCess=RW DYNAMIC=yes\n" "EXPort\n" "XOR"
};

sh_sand_cmd_t dnxc_ser_memory_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"LIST", dnxc_ser_memory_list_cmd, NULL, dnxc_ser_memory_list_options, &dnxc_ser_memory_list_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY},
    {NULL}
};

static sh_sand_man_t dnxc_ser_memory_man = {
    .brief = "SER MEMory commands \n",
    .full = "SER MEMory commands\n",
};

int soc_dnx_intr_config_dump(
    int unit,
    int intr);

static int
diag_blktype_in_list(
    int unit,
    int blktype,
    int *list)
{
    int _bidx = 0;
    while (list[_bidx] != SOC_BLOCK_TYPE_INVALID)
    {
        if (blktype == list[_bidx])
        {
            return TRUE;
        }
        _bidx++;
    }
    return FALSE;
}

static shr_error_e
dnxc_ser_intr_info_print(
    int unit,
    int cdsp,
    soc_block_t blk_type,
    diag_ser_intr_type_e vector_type,
    diag_ser_intr_type_e handler_type,
    diag_ser_intr_type_e enable_type,
    diag_ser_intr_type_e assert_type,
    sh_sand_control_t * sand_control)
{
    int intr, nof_interrupts, blk, port;
    int *blk_list;
    int intr_assert, intr_enable;
    int intr_has_assert, intr_has_enable;
    uint32 flags, instance_num, statistics_count;
    uint32 enable_bmp[SER_INTR_INSTANCE_BMP_WIDTH32];
    uint32 assert_bmp[SER_INTR_INSTANCE_BMP_WIDTH32];
    char str_flags[256];
    int intr_print;

    soc_interrupt_db_t *interrupts;
    soc_block_info_t *bi = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (NULL == interrupts)
    {
        SHR_CLI_EXIT(_SHR_E_UNIT, "Not supported for unit %d \n", unit);
    }

    PRT_TITLE_SET("                      Interrupt");
    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Intr Name(instance)");
    PRT_COLUMN_ADD("Root");
    PRT_COLUMN_ADD("Flags");
    PRT_COLUMN_ADD("Action");
    PRT_COLUMN_ADD("En");
    PRT_COLUMN_ADD("On");
    PRT_COLUMN_ADD("Counter");

    SHR_IF_ERR_EXIT(soc_nof_interrupts(unit, &nof_interrupts));
    for (intr = 0; intr < nof_interrupts; intr++)
    {
        if (!SOC_REG_IS_VALID(unit, interrupts[intr].reg))
        {
            continue;
        }

        blk_list = SOC_REG_INFO(unit, interrupts[intr].reg).block;
        if (blk_type != SOC_BLK_NONE)
        {
            if (!diag_blktype_in_list(unit, blk_type, blk_list))
            {
                continue;
            }
        }

        if (vector_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump root interrupt 
             */
            if (interrupts[intr].vector_id == 0)
            {
                continue;
            }
        }
        else if (vector_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (interrupts[intr].vector_id == 1)
            {
                continue;
            }
        }
        if (handler_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump interrupt with handler
             */
            if (interrupts[intr].func_arr == NULL)
            {
                continue;
            }
        }
        else if (handler_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump interrupt without handler
             */
            if (interrupts[intr].func_arr != NULL)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(soc_interrupt_flags_get(unit, intr, &flags));
        sal_memset(str_flags, 0, sizeof(str_flags));
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sizeof(str_flags) - sal_strlen(str_flags), "LOG");
        }
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB_BIT))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_BCM_AND_USR_CB");
        }
        else if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_OVERRIDE");
        }
        else
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " ACT_BCM_ONLY");
        }
        if (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS))
        {
            sal_snprintf(str_flags + sal_strlen(str_flags), sal_strlen(str_flags) - sal_strlen(str_flags),
                         " FORCE_UNMASK");
        }

        instance_num = 0;
        statistics_count = 0;
        sal_memset(enable_bmp, 0, sizeof(enable_bmp));
        sal_memset(assert_bmp, 0, sizeof(assert_bmp));
        intr_has_assert = 0;
        intr_has_enable = 0;
        SOC_BLOCKS_ITER(unit, blk, blk_list)
        {
            if (!SOC_INFO(unit).block_valid[blk])
            {
                continue;
            }

            instance_num++;
            bi = &(SOC_BLOCK_INFO(unit, blk));
            if (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP)
            {
                port = SOC_BLOCK_PORT(unit, blk);
            }
            else
            {
                port = bi->number;
            }
            SHR_IF_ERR_EXIT(soc_interrupt_get(unit, port, &(interrupts[intr]), &intr_assert));
            SHR_IF_ERR_EXIT(soc_interrupt_is_enabled(unit, port, &(interrupts[intr]), &intr_enable));
            if (intr_assert)
            {
                intr_has_assert = 1;
                SHR_BITSET(assert_bmp, bi->number);
            }
            if (intr_enable)
            {
                intr_has_enable = 1;
                SHR_BITSET(enable_bmp, bi->number);
            }

            statistics_count += interrupts[intr].statistics_count[bi->number];

            if (cdsp)
            {
                if (enable_type == diag_ser_intr_type_yes)
                {
                    /*
                     * Only dump enable interrupt 
                     */
                    if (intr_enable == 0)
                    {
                        continue;
                    }
                }
                else if (enable_type == diag_ser_intr_type_no)
                {
                    /*
                     * Only dump disable interrupt 
                     */
                    if (intr_enable == 1)
                    {
                        continue;
                    }
                }

                if (assert_type == diag_ser_intr_type_yes)
                {
                    /*
                     * Only dump asserted interrupt 
                     */
                    if (intr_assert == 0)
                    {
                        continue;
                    }
                }
                else if (assert_type == diag_ser_intr_type_no)
                {
                    /*
                     * Only dump not asserted interrupt 
                     */
                    if (intr_assert == 1)
                    {
                        continue;
                    }
                }

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("-");
                PRT_CELL_SET("        [%d]", bi->number);
                PRT_CELL_SKIP(3);       /* Vector & Flag & Action */
                PRT_CELL_SET("%d", intr_enable);
                PRT_CELL_SET("%d", intr_assert);
                PRT_CELL_SET("%d", interrupts[intr].statistics_count[bi->number]);
            }
        }

        intr_print = 1;
        if (enable_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump enable interrupt 
             */
            if (intr_has_enable == 0)
            {
                intr_print = 0;
            }
        }
        else if (enable_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (intr_has_enable == 1)
            {
                intr_print = 0;
            }
        }

        if (assert_type == diag_ser_intr_type_yes)
        {
            /*
             * Only dump root interrupt 
             */
            if (intr_has_assert == 0)
            {
                intr_print = 0;
            }
        }
        else if (assert_type == diag_ser_intr_type_no)
        {
            /*
             * Only dump real interrupt 
             */
            if (intr_has_assert == 1)
            {
                intr_print = 0;
            }
        }

        if (intr_print)
        {
            char tmp[SER_INTR_INSTANCE_BMP_WIDTH8 + 3];

            if (cdsp)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }
            PRT_CELL_SET("%d", intr);
#if !defined(SOC_NO_NAMES)
            PRT_CELL_SET("%s[%d]", interrupts[intr].name, instance_num);
#else
            PRT_CELL_SET("%s[%d]", "--", instance_num);
#endif
            PRT_CELL_SET("%d", interrupts[intr].vector_id);
            PRT_CELL_SET("%s", str_flags);

            if (interrupts[intr].func_arr != NULL)
            {
                PRT_CELL_SET("Set");
            }
            else
            {
                PRT_CELL_SET("NULL");
            }
            _shr_format_long_integer(tmp, enable_bmp, BITS2BYTES(instance_num));
            PRT_CELL_SET("%s", tmp);
            _shr_format_long_integer(tmp, assert_bmp, BITS2BYTES(instance_num));
            PRT_CELL_SET("%s", tmp);
            PRT_CELL_SET("%d", statistics_count);
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_ser_intr_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_ser_intr_type_e vector_type, enable_type;
    diag_ser_intr_type_e handler_type, assert_type;
    int cdsp, intr;
    char *str_block;
    soc_block_t blk_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("VECTor", vector_type);
    SH_SAND_GET_ENUM("ENAble", enable_type);
    SH_SAND_GET_ENUM("ON", assert_type);
    SH_SAND_GET_ENUM("HanDLeR", handler_type);
    SH_SAND_GET_BOOL("CoreDiSPlay", cdsp);
    SH_SAND_GET_STR("block", str_block);
    SH_SAND_GET_INT32("ID", intr);

#ifdef BCM_ACCESS_SUPPORT
    if (SOC_IS_NEW_ACCESS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(dnxc_ser_intr_info_print_access(unit, intr, cdsp, str_block, vector_type, handler_type,
                                                        enable_type, assert_type, sand_control));
        SHR_EXIT();
    }
#endif

    if (intr >= 0)
    {
        SHR_IF_ERR_EXIT(sand_single_interrupt_info_dump(unit, intr));
        SHR_EXIT();
    }
    blk_type = soc_block_name_match(unit, str_block);
    SHR_IF_ERR_EXIT(dnxc_ser_intr_info_print(unit, cdsp, blk_type, vector_type, handler_type,
                                             enable_type, assert_type, sand_control));
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t dnxc_ser_intr_type_enum[] = {
    {"all", diag_ser_intr_type_all, "interrupt all"},
    {"yes", diag_ser_intr_type_yes, "interrupt yes only"},
    {"no", diag_ser_intr_type_no, "interrupt no only"},
    {NULL}
};

static sh_sand_option_t dnxc_ser_intr_dump_options[] = {
    {"VECTor", SAL_FIELD_TYPE_ENUM, "Interrupt vector filter", "all", (void *) dnxc_ser_intr_type_enum},
    {"ENAble", SAL_FIELD_TYPE_ENUM, "Interrupt enable filter", "all", (void *) dnxc_ser_intr_type_enum},
    {"ON", SAL_FIELD_TYPE_ENUM, "Interrupt assert filter", "all", (void *) dnxc_ser_intr_type_enum},
    {"HanDLeR", SAL_FIELD_TYPE_ENUM, "Interrupt handler filter", "all", (void *) dnxc_ser_intr_type_enum},
    {"CoreDiSPlay", SAL_FIELD_TYPE_BOOL, "display interrupt on instance", "false", NULL},
    {"BLock", SAL_FIELD_TYPE_STR, "Block type name", ""},
    {"ID", SAL_FIELD_TYPE_INT32, "Interrupt ID", "-1"},
    {NULL}
};

static sh_sand_man_t dnxc_ser_intr_dump_man = {
    .brief = "SER INTeRrupt commands\n",
    .full = "Dump INTeRrupt based on input condition\n",
    .examples = "enable=yes ON=yes\n" "vector=no HanDLeR=no\n" "vector=yes ENAble=no CoreDiSPlay\n" "BLock=eci\n" "ID=0"
};

sh_sand_cmd_t dnxc_ser_intr_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"DuMP", dnxc_ser_intr_dump_cmd, NULL, dnxc_ser_intr_dump_options, &dnxc_ser_intr_dump_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY},

    {NULL}
};

static sh_sand_man_t dnxc_ser_intr_man = {
    .brief = "SER INTeRrupt commands",
};

#ifdef BCM_ACCESS_SUPPORT
/** see header file */
shr_error_e
sh_cmd_is_access_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (soc_dnxc_access_available(unit))
    {
        SHR_EXIT();
    }
    else
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t dnxc_new_access_man = {
    .brief = "SER ACCess commands",
};

static shr_error_e
_access_parse_regmem_name_list(
    int unit,
    char *str_regmem_names,
    access_regmem_id_t * regmem_list,
    uint32 *regmem_num)
{
    access_regmem_id_t regmem;
    uint32 num = 0, ii;
    char **item_list = NULL;
    uint32 num_of_items = 0;

    SHR_FUNC_INIT_VARS(unit);

    num = 0;
    if (sal_strlen(str_regmem_names) == 0)
    {
        SHR_EXIT();
    }

    if (sal_strstr(str_regmem_names, "|") != NULL)
    {
        if ((item_list =
             utilex_str_split(str_regmem_names, "|", _ACCESS_SBUSDMA_DESC_MAX_MEM_NUM, &num_of_items)) == NULL)
        {
            SHR_EXIT();
        }
        for (ii = 0; ii < num_of_items; ii++)
        {
            regmem = access_parse_regmem_by_name(unit, item_list[ii]);
            if (regmem != ACCESS_INVALID_REGMEM_ID)
            {
                regmem_list[num] = regmem;
                num++;
            }
        }
    }
    else if (sal_strstr(str_regmem_names, ",") != NULL)
    {
        if ((item_list =
             utilex_str_split(str_regmem_names, ",", _ACCESS_SBUSDMA_DESC_MAX_MEM_NUM, &num_of_items)) == NULL)
        {
            SHR_EXIT();
        }
        for (ii = 0; ii < num_of_items; ii++)
        {
            regmem = access_parse_regmem_by_name(unit, item_list[ii]);
            if (regmem != ACCESS_INVALID_REGMEM_ID)
            {
                regmem_list[num] = regmem;
                num++;
            }
        }
    }
    else
    {
        regmem = access_parse_regmem_by_name(unit, str_regmem_names);
        if (regmem != ACCESS_INVALID_REGMEM_ID)
        {
            regmem_list[num] = regmem;
            num++;
        }
    }

exit:
    *regmem_num = num;
    if (item_list != NULL)
    {
        utilex_str_split_free(item_list, num_of_items);
        item_list = NULL;
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_access_fifodma_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        cli_out("The diag is not supported on SIM\n");
        SHR_EXIT();
    }
#endif

    SHR_IF_ERR_EXIT(dnxc_access_fifodma_dump(unit));
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_access_fifodma_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *str_regmem_name;
    soc_access_fifodma_src_t source;
    uint32 block, array_idx, max_entries;
    access_regmem_id_t regmem;
    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        cli_out("The diag is not supported on SIM\n");
        SHR_EXIT();
    }
#endif

    SH_SAND_GET_ENUM("SouRCe", source);
    SH_SAND_GET_STR("NaMe", str_regmem_name);
    SH_SAND_GET_UINT32("BLock", block);
    SH_SAND_GET_UINT32("ARray", array_idx);
    SH_SAND_GET_UINT32("SiZe", max_entries);
    if (source == soc_access_fifodma_src_number)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    regmem = access_parse_regmem_by_name(unit, str_regmem_name);
    if (regmem == ACCESS_INVALID_REGMEM_ID)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(test_access_fifodma_start(unit, source, regmem, block, array_idx, max_entries, 0, 0));
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_access_fifodma_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    soc_access_fifodma_src_t source;
    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        cli_out("The diag is not supported on SIM\n");
        SHR_EXIT();
    }
#endif

    SH_SAND_GET_ENUM("SouRCe", source);
    if (source == soc_access_fifodma_src_number)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(test_access_fifodma_stop(unit, source));
exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static sh_sand_enum_t dnxc_access_fifidma_src_enum[] = {
    {"crps_hi0", soc_access_fifodma_src_crps_hi0, "crps_hi0"},
    {"crps_lo0", soc_access_fifodma_src_crps_lo0, "crps_lo0"},
    {"crps_hi1", soc_access_fifodma_src_crps_hi1, "crps_hi1"},
    {"crps_lo1", soc_access_fifodma_src_crps_lo1, "crps_lo1"},
    {"crps_hi2", soc_access_fifodma_src_crps_hi2, "crps_hi2"},
    {"crps_lo2", soc_access_fifodma_src_crps_lo2, "crps_lo2"},
    {"crps_hi3", soc_access_fifodma_src_crps_hi3, "crps_hi3"},
    {"crps_lo3", soc_access_fifodma_src_crps_lo3, "interrupt yes only"},
    {"crpsxl_hi", soc_access_fifodma_src_crpsxl_hi, "crpsxl_hi"},
    {"crpsxl_lo", soc_access_fifodma_src_crpsxl_lo, "crpsxl_lo"},
    {"oamp0", soc_access_fifodma_src_oamp_fifodma0, "oamp_fifodma0"},
    {"oamp1", soc_access_fifodma_src_oamp_fifodma1, "oamp_fifodma1"},
    {"oamp2", soc_access_fifodma_src_oamp_fifodma2, "oamp_fifodma2"},
    {"olp_lem1", soc_access_fifodma_src_olp_lem1_fifodma, "olp_lem1_fifodma"},
    {"olp_lem2", soc_access_fifodma_src_olp_lem2_fifodma, "olp_lem2_fifodma"},
    {"mact_lem_1_0", soc_access_fifodma_src_mact_lem_1_fifodma0, "mact_lem_1_fifodma0"},
    {"mact_lem_1_1", soc_access_fifodma_src_mact_lem_1_fifodma1, "mact_lem_1_fifodma1"},
    {"mact_lem_2_0", soc_access_fifodma_src_mact_lem_2_fifodma0, "mact_lem_2_fifodma0"},
    {"mact_lem_2_1", soc_access_fifodma_src_mact_lem_2_fifodma1, "mact_lem_2_fifodma1"},
    {"eventor", soc_access_fifodma_src_eventor_fifodma, "eventor_fifodma"},
    {"core0_ipmf2", soc_access_fifodma_src_core0_ipmf2_fifodma, "core0_ipmf2_fifodma"},
    {"core1_ipmf2", soc_access_fifodma_src_core1_ipmf2_fifodma, "core1_ipmf2_fifodma"},
    {"core2_ipmf2", soc_access_fifodma_src_core2_ipmf2_fifodma, "core2_ipmf2_fifodma"},
    {"core3_ipmf2", soc_access_fifodma_src_core3_ipmf2_fifodma, "core3_ipmf2_fifodma"},
    {"invalid", soc_access_fifodma_src_number, "invalid fifodma source"},
    {NULL}
};

static sh_sand_option_t dnxc_access_fifodma_start_options[] = {
    {"SouRCe", SAL_FIELD_TYPE_ENUM, "access fifodma source", "invalid", (void *) dnxc_access_fifidma_src_enum},
    {"NaMe", SAL_FIELD_TYPE_STR, "regmem names string for FIFODMA", ""},
    {"BLock", SAL_FIELD_TYPE_UINT32, "regmem block instance number", "0", NULL},
    {"ARray", SAL_FIELD_TYPE_UINT32, "regmem array index", "0", NULL},
    {"SiZe", SAL_FIELD_TYPE_UINT32, "Max entry size of fifodma", "0", NULL},

    {NULL}
};

static sh_sand_option_t dnxc_access_fifodma_stop_options[] = {
    {"SouRCe", SAL_FIELD_TYPE_ENUM, "access fifodma source", "invalid", (void *) dnxc_access_fifidma_src_enum},
    {NULL}
};

static sh_sand_option_t dnxc_access_fifodma_dump_options[] = {
    {NULL}
};

static sh_sand_man_t dnxc_access_fifodma_dump_man = {
    .brief = "SER access fifodma dump diag\n",
    .full = "SER access fifodma dump diag\n",
    .examples = ""
};

static sh_sand_man_t dnxc_access_fifodma_start_man = {
    .brief = "SER access fifodma start diag\n",
    .full = "SER access fifodma start diag\n",
    .examples = "SouRCe=oamp0 NaMe=OAMP_INTERRUPT_MESSAGE ARray=0 BLock=0 SiZe=16384"
};

static sh_sand_man_t dnxc_access_fifodma_stop_man = {
    .brief = "SER access fifodma stop diag\n",
    .full = "SER access fifodma stop diag\n",
    .examples = "SouRCe=oamp0"
};

static sh_sand_man_t dnxc_access_fifodma_man = {
    .brief = "SER access fifodma\n",
    .full = "SER access fifodma\n",
};

sh_sand_cmd_t dnxc_access_fifodma_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"DuMP", dnxc_access_fifodma_dump_cmd, NULL, dnxc_access_fifodma_dump_options,
     &dnxc_access_fifodma_dump_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},

    {"START", dnxc_access_fifodma_start_cmd, NULL, dnxc_access_fifodma_start_options,
     &dnxc_access_fifodma_start_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},

    {"STOP", dnxc_access_fifodma_stop_cmd, NULL, dnxc_access_fifodma_stop_options,
     &dnxc_access_fifodma_stop_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};

static shr_error_e
dnxc_access_sbusdma_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *str_regmem_names;
    uint32 block, array_idx, flags, patten;
    access_regmem_id_t regmem, regmem_list[_ACCESS_SBUSDMA_DESC_MAX_NUM];
    uint32 ii, regmem_num;

    SHR_FUNC_INIT_VARS(unit);

#ifdef PLISIM
    if (SAL_BOOT_PLISIM)
    {
        cli_out("The diag is not supported on SIM\n");
        SHR_EXIT();
    }
#endif

    SH_SAND_GET_STR("NaMe", str_regmem_names);
    SH_SAND_GET_UINT32("BLock", block);
    SH_SAND_GET_UINT32("ARray", array_idx);
    SH_SAND_GET_UINT32("PatTeRN", patten);
    SH_SAND_GET_UINT32("FLaG", flags);

    SHR_IF_ERR_EXIT(_access_parse_regmem_name_list(unit, str_regmem_names, regmem_list, &regmem_num));

    cli_out("flags=0x%x, patten=0x%x, block=%d, array_idx=%d\n", flags, patten, block, array_idx);
    cli_out("str_regmem_names=%s, regmem_num=%d\n", str_regmem_names, regmem_num);
    for (ii = 0; ii < regmem_num; ii++)
    {
        regmem = regmem_list[ii];
        if (access_is_regmem_a_valid_memory(unit, regmem))
        {
            cli_out("    %-4d%-10s -- %s\n", ii, "Memory", ACCESS_REGMEM_NAME(regmem));
        }
        else if (access_is_regmem_a_valid_register(unit, regmem))
        {
            cli_out("    %-4d%-10s -- %s\n", ii, "Register", ACCESS_REGMEM_NAME(regmem));
        }
        else
        {
            cli_out("    %-4d%-10s -- %d\n", ii, "Invalid", (int) regmem);
        }
    }

    SHR_IF_ERR_EXIT(test_access_regmem_sbusdma_desc(unit, flags, patten, regmem_list, regmem_num, block, array_idx));

exit:
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnxc_access_sbusdma_options[] = {
    {"NaMe", SAL_FIELD_TYPE_STR, "comma-separated set of regmem names string", ""},
    {"BLock", SAL_FIELD_TYPE_UINT32, "Memory/Register block instance number", "0", NULL},
    {"ARray", SAL_FIELD_TYPE_UINT32, "Memory/Register array index", "0", NULL},
    {"PatTeRN", SAL_FIELD_TYPE_UINT32, "Memory write patten", "0", NULL},
    {"FLaG", SAL_FIELD_TYPE_UINT32, "Sbus DMA access Flags", "0", NULL},
    {NULL}
};

static sh_sand_man_t dnxc_access_sbusdma_man = {
    .brief = "SER sbus dma diag\n",
    .full = "SER sbus dma diag\n",
    .examples =
        "ARray=0 BLock=0 FLaG=0x100000 pattern=0x3 name=IPPA_VLAN_MEMBERSHIP_TABLE_1,IPPA_INTERRUPT_MASK_REGISTER"
};

sh_sand_cmd_t dnxc_new_access_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"SbusDMA", dnxc_access_sbusdma_cmd, NULL, dnxc_access_sbusdma_options,
     &dnxc_access_sbusdma_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},

    {"FifoDMA", NULL, dnxc_access_fifodma_cmds, NULL, &dnxc_access_fifodma_man},

    {NULL}
};

#endif

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
sh_sand_man_t sh_dnxc_diag_ser_man = {
    .brief = "SER commands",
    .full = "The dispatcher for the different SER diagnostics commands\n"
};

/**
 * \brief DNXC SER diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for MBIST diagnostic commands
 */
#ifdef BCM_DNXF_SUPPORT
sh_sand_cmd_t sh_dnxf_diag_ser_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"MEMory", NULL, dnxc_ser_memory_cmds, NULL, &dnxc_ser_memory_man},
    {"INTeRrupt", NULL, dnxc_ser_intr_cmds, NULL, &dnxc_ser_intr_man},

#ifdef BCM_ACCESS_SUPPORT
    {"ACCess", NULL, dnxc_new_access_cmds, NULL, &dnxc_new_access_man,
     NULL, NULL, SH_CMD_CONDITIONAL * SH_CMD_NOT_RELEASE, sh_cmd_is_access_available},
#endif
    {NULL}
};
#endif

#ifdef BCM_DNX_SUPPORT
sh_sand_cmd_t sh_dnx_diag_ser_cmds[] = {
    /*
     * Name | Leaf Action | Junction Array | Options for Leaf | Usage 
     */
    {"MDB", NULL, dnxc_diag_mdb_ser_cmds, NULL, &dnxc_diag_ser_mdb_man},
    {"TCam", NULL, dnxc_diag_ser_tcam_cmds, NULL, &dnxc_diag_ser_tcam_man},
    {"MEMory", NULL, dnxc_ser_memory_cmds, NULL, &dnxc_ser_memory_man},
    {"INTeRrupt", NULL, dnxc_ser_intr_cmds, NULL, &dnxc_ser_intr_man},

#ifdef BCM_ACCESS_SUPPORT
    {"ACCess", NULL, dnxc_new_access_cmds, NULL, &dnxc_new_access_man,
     NULL, NULL, SH_CMD_CONDITIONAL * SH_CMD_NOT_RELEASE, sh_cmd_is_access_available},
#endif
    {NULL}
};

#endif
