/**
 * \file diag_framer_regram_test.c Contains all of the framer testrun commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_SYMTAB

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/error.h>
#include <appl/diag/shell.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#ifndef NO_FRAMER_LIB_BUILD
#include "diag_framer_regram_test.h"
#include "j2x_uas_framer_reg_operate.h"
#include "global_define.h"
#include "j2x_sal_framer_info.h"

typedef int (
    *framer_module_regram_cb_f) (
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data);

static void
diag_framer_get_regram_test_value(
    int unit,
    uint32 width,
    uint32 patten,
    uint32 *test_val)
{
    uint32 ii, idx;
    if (width == 32)
    {
        *test_val = patten;
    }
    else
    {
        for (ii = 0; ii < width; ii++)
        {
            idx = ii % 32;
            if (SHR_IS_BITSET(&patten, idx))
            {
                SHR_BITSET(test_val, ii);
            }
            else
            {
                SHR_BITCLR(test_val, ii);
            }
        }
    }

    return;
}

#if 0
static shr_error_e
diag_framer_set_ram_test_value(
    int unit,
    uint32 width,
    uint32 mode,
    uint32 *default_val)
{
    uint32 ii, idx;
    uint32 mode_list[] = { 0, 0xFFFFFFFF, 0x5D5D5D5D };
    uint32 mode_value[1];

    if (mode >= COUNTOF(mode_list))
    {
        mode = 0;
    }
    mode_value[0] = mode_list[mode];
    for (ii = 0; ii < width; ii++)
    {
        idx = ii % 32;
        if (SHR_IS_BITSET(mode_value, idx))
        {
            SHR_BITSET(default_val, ii);
        }
        else
        {
            SHR_BITCLR(default_val, ii);
        }
    }
    return _SHR_E_NONE;
}

static shr_error_e
diag_framer_set_reg_test_value(
    int unit,
    uint32 mode,
    uint32 *reg_val)
{
    switch (mode)
    {
        case 0:
            break;
        case 1:
            *reg_val = 0;
            break;
        case 2:
            *reg_val = 0xffffffff;
            break;
        case 3:
            *reg_val = 0x5d5d5d5d;
            break;
        default:
            break;
    }
    return _SHR_E_NONE;
}
#endif

static char *
_diag_frame_reg_type_str(
    J2X_U8 reg_type)
{
    static char *str_reg_type[] = {
        "WO",
        "WOP",
        "RW",
        "RWP",
        "RWUS",
        "RWUR",
        "RST",
        "RST_ROOT_F",
        "RST_ROOT_L",
        "RST_GLB_F",
        "RST_GLB_L",
        "RST_CTRL",
        "PLS",
        "CCR",
        "CCW",
        "CVR",
        "CVW",
        "CCWK",
        "CVWK",
        "CCRK",
        "CVRK",
        "RO",
        "ROP",
        "ALM",
        "INTS",
        "INTP",
        "INTL",
        "INTR",
        "INTS_YUSHUI",
        "INTST",
        "INTPT",
        "INTLT",
        "TRO",
        "TWO",
        "TRW",
        "ERROR"
    };
    if (reg_type >= COUNTOF(str_reg_type))
    {
        reg_type = COUNTOF(str_reg_type) - 1;
    }

    return str_reg_type[reg_type];
}
static int
_diag_framer_is_reg_type_mem(
    J2X_U8 reg_type)
{
    int rc = 0;
    if ((reg_type == REG_TYPE_TRW) || (reg_type == REG_TYPE_TRO) || (reg_type == REG_TYPE_TWO))
    {
        rc = 1;
    }
    return rc;
}

static int
_diag_framer_is_regmem_ro(
    J2X_U8 reg_type)
{
    int rc = 0;
    if ((reg_type == REG_TYPE_RO) || (reg_type == REG_TYPE_ROP) || (reg_type == REG_TYPE_TRO))
    {
        rc = 1;
    }
    return rc;
}

static int
_diag_framer_is_regmem_wo(
    J2X_U8 reg_type)
{
    int rc = 0;
    if ((reg_type == REG_TYPE_WO) || (reg_type == REG_TYPE_WOP) || (reg_type == REG_TYPE_TWO))
    {
        rc = 1;
    }
    return rc;
}

static int
_diag_frame_regram_need_skip(
    J2X_MODULE_INST_S * module_inst,
    REG_INFO_S * reg_info)
{
    int rc = 0;

    switch (reg_info->reg_type)
    {
        case REG_TYPE_RO:
        case REG_TYPE_ROP:
        case REG_TYPE_TRO:
        case REG_TYPE_INTS_YUSHUI:
        case REG_TYPE_INTR:
        case REG_TYPE_ALM:
        case REG_TYPE_CCR:
        case REG_TYPE_CCW:
        case REG_TYPE_CVR:
        case REG_TYPE_CVW:
        case REG_TYPE_CCWK:
        case REG_TYPE_CVWK:
        case REG_TYPE_CCRK:
        case REG_TYPE_CVRK:
            rc = 1;
            break;
        default:
            break;
    }

    /*
     * Module: PPS_TOD_GLOBAL_TS 
     */
    if (module_inst->module_addr == 0xb0600)
    {
        rc = 1;
    }

    /*
     * ADAPT_FLEXE_DEMUX_0__CALENB_FLEXE_DEMUX.FDMX0 
     */
    if ((module_inst->module_addr == 0x20400) && (reg_info->offset_addr == 0xb))
    {
        rc = 1;
    }
    /*
     * ADAPT_FLEXE_DEMUX_0__CALENB_FLEXE_DEMUX.FDMX1 
     */
    else if ((module_inst->module_addr == 0x24400) && (reg_info->offset_addr == 0xb))
    {
        rc = 1;
    }
    /*
     * ADAPT_INSTANCE_TX__CALENB_INSTANCE_TX.FFOB 
     */
    else if ((module_inst->module_addr == 0x1c400) && (reg_info->offset_addr == 0xb))
    {
        rc = 1;
    }

    return rc;
}

static int
_diag_framer_name_match(
    char *name1,
    char *name2)
{
    int rc = 0;
    size_t max_len = SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH;
    if ((name1 == NULL) || (name2 == NULL))
    {
        rc = 1;
    }
    else if (sal_strncasecmp(name1, name2, sal_strnlen(name1, max_len)) == 0)
    {
        rc = 1;
    }

    return rc;
}

static int
_diag_framer_module_regram_iter(
    int unit,
    uint32 flags,
    char *module_name,
    char *regram_name,
    framer_module_regram_cb_f cb,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    int ii, jj;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cb, _SHR_E_FAIL, "cb is NULL!");

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT(cb(unit, flags, -1, -1, prt_ctr, user_data));
    for (ii = 0; ii < sys_info->reg_module_num; ii++)
    {
        module_inst = sys_info->module_inst + ii;
        if (!_diag_framer_name_match(module_name, module_inst->inst_name))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(cb(unit, flags, ii, -1, prt_ctr, user_data));
        for (jj = 0; jj < module_inst->module->reg_num; jj++)
        {
            reg_info = module_inst->module->regs + jj;

            if (!_diag_framer_name_match(regram_name, reg_info->reg_name))
            {
                continue;
            }
            if (_diag_framer_is_reg_type_mem(reg_info->reg_type))
            {
                if ((flags & DIAG_FRAMER_TEST_FLAG_MEM) == 0)
                {
                    continue;
                }
            }
            else
            {
                if ((flags & DIAG_FRAMER_TEST_FLAG_REG) == 0)
                {
                    continue;
                }
            }
            if (flags & DIAG_FRAMER_TEST_FLAG_SKIP)
            {
                if (_diag_frame_regram_need_skip(module_inst, reg_info))
                {
                    continue;
                }
            }
            SHR_IF_ERR_EXIT(cb(unit, flags, ii, jj, prt_ctr, user_data));
        }
    }
    SHR_IF_ERR_EXIT(cb(unit, flags, -2, -1, prt_ctr, user_data));

exit:

    SHR_FUNC_EXIT;
}

#if 0
shr_error_e
diag_framer_test_list_module(
    int unit,
    char *module_name,
    sh_sand_control_t * sand_control)
{
    int rv = 0;
    int ii, jj, module_match;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_S *module_info;
    REG_INFO_S *reg_info;
    uint32 depth, width, max_width = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    PRT_TITLE_SET("Framer module info");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Module name");
    PRT_COLUMN_ADD("module_addr");
    PRT_COLUMN_ADD("reg_info");
    for (ii = 0; ii < sys_info->reg_module_num; ii++)
    {
        module_match = 0;
        if (module_name == NULL)
        {
            module_match = 1;
        }
        else if (sal_strncasecmp(module_name, sys_info->module_inst[ii].inst_name,
                                 sal_strnlen(module_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0)
        {
            module_match = 1;
        }

        if (module_match)
        {
            /*
             * PRT_ROW_ADD(PRT_ROW_SEP_NONE); PRT_CELL_SET("%d", ii); PRT_CELL_SET("%s",
             * sys_info->module_inst[ii].inst_name); PRT_CELL_SET("0x%x", sys_info->module_inst[ii].module_addr);
             * PRT_CELL_SET("");
             */

            module_info = sys_info->module_inst[ii].module;
            reg_info = module_info->regs;
            for (jj = 0; jj < module_info->reg_num; jj++)
            {
                if (_diag_framer_is_reg_type_mem(reg_info[jj].reg_type))
                {
                    depth = reg_info[jj].ext & MASK_20_BIT;
                    width = ((reg_info[jj].ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
                    if (width > max_width)
                    {
                        max_width = width;
                    }
                    if (width > 64)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%d", ii);
                        PRT_CELL_SET("%s", sys_info->module_inst[ii].inst_name);
                        PRT_CELL_SET("0x%x", sys_info->module_inst[ii].module_addr);
                        PRT_CELL_SET("Ram-%s[%d - %d]%d/%d", reg_info[jj].reg_name,
                                     depth, width, reg_info[jj].bits_num, max_width);
                    }
                }
                else
                {
                    if (reg_info[jj].bits_num > 32)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%d", ii);
                        PRT_CELL_SET("%s", sys_info->module_inst[ii].inst_name);
                        PRT_CELL_SET("0x%x", sys_info->module_inst[ii].module_addr);
                        PRT_CELL_SET("Reg-%s-%d", reg_info[jj].reg_name, reg_info[jj].bits_num);
                    }
                }
            }
        }
        /*
         * sal_snprintf(str_mem_info + sal_strnlen(str_mem_info, sizeof(str_mem_info)), sizeof(str_mem_info) -
         * sal_strnlen(str_mem_info, sizeof(str_mem_info)), "Cacheable/"); if ((sal_strncasecmp(severity_flag, "all",
         * SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0) && (sal_strncasecmp(severity_flag, "low",
         * SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) != 0)) 
         */
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_framer_test_reg_default: 
 *      Iter framer core registers and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] module_name - module_name.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_framer_test_reg_default(
    int unit,
    char *module_name,
    int changed,
    sh_sand_control_t * sand_control)
{
    int rv = 0;
    int ii, jj, module_match, module_first;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_S *module_info;
    REG_INFO_S *reg_info;
    uint32 reg_data;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    PRT_TITLE_SET("Framer module info");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("BaseAddr");
    PRT_COLUMN_ADD("regName");
    PRT_COLUMN_ADD("value");
    PRT_COLUMN_ADD("default");
    for (ii = 0; ii < sys_info->reg_module_num; ii++)
    {
        module_match = 0;
        if (module_name == NULL)
        {
            module_match = 1;
        }
        else if (sal_strncasecmp(module_name, sys_info->module_inst[ii].inst_name,
                                 sal_strnlen(module_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0)
        {
            module_match = 1;
        }

        if (module_match)
        {
            module_first = 1;
            module_info = sys_info->module_inst[ii].module;
            reg_info = module_info->regs;
            for (jj = 0; jj < module_info->reg_num; jj++)
            {
                if (_diag_framer_is_reg_type_mem(reg_info[jj].reg_type))
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(framer_regs_read(unit,
                                                 sys_info->module_inst[ii].module_addr,
                                                 reg_info[jj].offset_addr, &reg_data));
                if ((!changed) || (reg_data != reg_info[jj].default_value))
                {
                    if (module_first)
                    {
                        module_first = 0;
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", sys_info->module_inst[ii].inst_name);
                        PRT_CELL_SET("0x%x", sys_info->module_inst[ii].module_addr);
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("");
                        PRT_CELL_SET("");
                    }
                    PRT_CELL_SET("%s", reg_info[jj].reg_name);
                    PRT_CELL_SET("0x%x", reg_data);
                    PRT_CELL_SET("0x%x", reg_info[jj].default_value);
                }

            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_framer_test_ram_default(
    int unit,
    char *module_name,
    int mode,
    sh_sand_control_t * sand_control)
{
    int rv = 0;
    int ii, jj, kk, module_match, module_first;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_S *module_info;
    REG_INFO_S *reg_info;
    uint32 *ram_data = NULL, *default_val = NULL;
    uint32 depth, width, width32, width8;
    char ram_data_str[128];
    char def_data_str[128];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    PRT_TITLE_SET("Framer module info");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("BaseAddr");
    PRT_COLUMN_ADD("RamName");
    PRT_COLUMN_ADD("Entry");
    PRT_COLUMN_ADD("value");
    for (ii = 0; ii < sys_info->reg_module_num; ii++)
    {
        module_match = 0;
        if (module_name == NULL)
        {
            module_match = 1;
        }
        else if (sal_strncasecmp(module_name, sys_info->module_inst[ii].inst_name,
                                 sal_strnlen(module_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)) == 0)
        {
            module_match = 1;
        }

        if (module_match)
        {
            module_first = 1;
            module_info = sys_info->module_inst[ii].module;
            reg_info = module_info->regs;
            for (jj = 0; jj < module_info->reg_num; jj++)
            {
                if (!_diag_framer_is_reg_type_mem(reg_info[jj].reg_type))
                {
                    continue;
                }
                depth = reg_info[jj].ext & MASK_20_BIT;
                width = ((reg_info[jj].ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
                width32 = (width + 31) / 32;
                width8 = (width + 7) / 8;
                ram_data = sal_alloc(sizeof(uint32) * width32, "ram_data");
                SHR_NULL_CHECK(ram_data, _SHR_E_MEMORY, "data is NULL!");
                default_val = sal_alloc(sizeof(uint32) * width32, "default_val");
                SHR_NULL_CHECK(default_val, _SHR_E_MEMORY, "data is NULL!");
                sal_memset(default_val, 0, sizeof(uint32) * width32);
                SHR_IF_ERR_EXIT(diag_framer_set_ram_test_value(unit, width, mode, default_val));
                _shr_format_long_integer(def_data_str, default_val, width8);
                for (kk = 0; kk < depth; kk++)
                {
                    sal_memset(ram_data, 0, sizeof(uint32) * width32);
                    SHR_IF_ERR_EXIT(framer_rams_read(unit,
                                                     sys_info->module_inst[ii].module_addr,
                                                     reg_info[jj].offset_addr, kk, width, ram_data));

                    if (!SHR_BITEQ_RANGE(ram_data, default_val, 0, width))
                    {
                        _shr_format_long_integer(ram_data_str, ram_data, width8);
                        if (module_first)
                        {
                            module_first = 0;
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", sys_info->module_inst[ii].inst_name);
                            PRT_CELL_SET("0x%x", sys_info->module_inst[ii].module_addr);
                        }
                        else
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("");
                            PRT_CELL_SET("");
                        }
                        PRT_CELL_SET("%s", reg_info[jj].reg_name);
                        PRT_CELL_SET("%d", kk);
                        PRT_CELL_SET("%s", ram_data_str);
                        PRT_CELL_SET("%s", def_data_str);
                    }

                }
            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif

static int
framer_ramreg_list_cb(
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;
    uint32 depth, width;

    SHR_FUNC_INIT_VARS(unit);

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    if (module_idx < 0)
    {
        SHR_EXIT();
    }

    module_inst = sys_info->module_inst + module_idx;
    if (regram_idx < 0)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", module_idx);
        PRT_CELL_SET("%s", module_inst->inst_name);
        PRT_CELL_SET("0x%x", module_inst->module_addr);
        PRT_CELL_SET("");
        PRT_CELL_SET("%d", module_inst->module->reg_num);
    }
    else
    {
        reg_info = module_inst->module->regs + regram_idx;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("");
        PRT_CELL_SET("");
        PRT_CELL_SET("");
        if (_diag_framer_is_reg_type_mem(reg_info->reg_type))
        {
            depth = reg_info->ext & MASK_20_BIT;
            width = ((reg_info->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
            PRT_CELL_SET("%s", _diag_frame_reg_type_str(reg_info->reg_type));
            PRT_CELL_SET("%s[%d-%d]", reg_info->reg_name, depth, width);
        }
        else
        {
            PRT_CELL_SET("%s", _diag_frame_reg_type_str(reg_info->reg_type));
            PRT_CELL_SET("%s", reg_info->reg_name);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

static int
framer_reg_dump_chg_cb(
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;
    uint32 reg_data, expect_data;
    uint32 patten;
    static int new_module = 0;
    static int check_reg_num = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (module_idx == -1)
    {
        check_reg_num = 0;
        SHR_EXIT();
    }
    else if (module_idx < -1)
    {
        if (flags & DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT)
        {
            cli_out("check_reg_num = %d\n", check_reg_num);
        }
        SHR_EXIT();
    }
    if (regram_idx < 0)
    {
        if (flags & DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT)
        {
            cli_out(".");
        }
        new_module = 1;
        SHR_EXIT();
    }

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    module_inst = sys_info->module_inst + module_idx;
    reg_info = module_inst->module->regs + regram_idx;

    if (_diag_framer_is_reg_type_mem(reg_info->reg_type))
    {
        SHR_EXIT();
    }
    if ((_diag_framer_is_regmem_ro(reg_info->reg_type)) || (_diag_framer_is_regmem_wo(reg_info->reg_type)))
    {
        SHR_EXIT();
    }
    check_reg_num++;

    reg_data = 0;
    patten = *((uint32 *) user_data);
    if ((flags & DIAG_FRAMER_TEST_FLAG_SET_PATTEN) == 0)
    {
        patten = reg_info->default_value;
    }
    diag_framer_get_regram_test_value(unit, 32, patten, &expect_data);
    rv = framer_regp_read(unit, module_inst->module_addr, reg_info->offset_addr, &reg_data);
    if ((reg_data != expect_data) | (rv != 0))
    {
        if (new_module)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", module_inst->inst_name);
            PRT_CELL_SET("0x%x", module_inst->module_addr);
            new_module = 0;
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("");
            PRT_CELL_SET("");
        }
        PRT_CELL_SET("%s", reg_info->reg_name);
        PRT_CELL_SET("0x%x %s", reg_info->offset_addr, _diag_frame_reg_type_str(reg_info->reg_type));
        PRT_CELL_SET("0x%x", reg_data);
        if (rv == 0)
        {
            PRT_CELL_SET("0x%x", expect_data);
        }
        else
        {
            PRT_CELL_SET("Read Fail--%d", rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
framer_reg_set_cb(
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;
    uint32 reg_data;
    uint32 patten;

    SHR_FUNC_INIT_VARS(unit);

    if (module_idx < 0)
    {
        SHR_EXIT();
    }
    if (regram_idx < 0)
    {
        SHR_EXIT();
    }

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    module_inst = sys_info->module_inst + module_idx;
    reg_info = module_inst->module->regs + regram_idx;

    if (_diag_framer_is_reg_type_mem(reg_info->reg_type))
    {
        SHR_EXIT();
    }
    if ((_diag_framer_is_regmem_ro(reg_info->reg_type)) || (_diag_framer_is_regmem_wo(reg_info->reg_type)))
    {
        SHR_EXIT();
    }

    reg_data = reg_info->default_value;
    patten = *((uint32 *) user_data);
    if ((flags & DIAG_FRAMER_TEST_FLAG_SET_PATTEN) == 0)
    {
        patten = reg_info->default_value;
    }
    diag_framer_get_regram_test_value(unit, 32, patten, &reg_data);
    rv = framer_regp_write(unit, module_inst->module_addr, reg_info->offset_addr, reg_data);
    if (rv != 0)
    {
        cli_out("framer_reg_set_cb fail %s---%s\n", module_inst->inst_name, reg_info->reg_name);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
framer_ram_dump_chg_cb(
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;
    uint32 *ram_data = NULL, *default_val = NULL;
    uint32 ii, depth, width, width32, width8;
    char ram_data_str[128];
    char def_data_str[128];
    uint32 patten;
    static int new_module = 0;
    static int check_ram_num = 0;

    SHR_FUNC_INIT_VARS(unit);
    if (module_idx == -1)
    {
        check_ram_num = 0;
        SHR_EXIT();
    }
    else if (module_idx < -1)
    {
        if (flags & DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT)
        {
            cli_out("check_ram_num = %d\n", check_ram_num);
        }
        SHR_EXIT();
    }
    if (regram_idx < 0)
    {
        if (flags & DIAG_FRAMER_TEST_FLAG_DEBUG_PRINT)
        {
            cli_out(".");
        }
        new_module = 1;
        SHR_EXIT();
    }

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    module_inst = sys_info->module_inst + module_idx;
    reg_info = module_inst->module->regs + regram_idx;

    if (!_diag_framer_is_reg_type_mem(reg_info->reg_type))
    {
        SHR_EXIT();
    }
    if ((_diag_framer_is_regmem_ro(reg_info->reg_type)) || (_diag_framer_is_regmem_wo(reg_info->reg_type)))
    {
        SHR_EXIT();
    }
    check_ram_num++;

    depth = reg_info->ext & MASK_20_BIT;
    width = ((reg_info->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
    width32 = (width + 31) / 32;
    width8 = (width + 7) / 8;
    ram_data = sal_alloc(sizeof(uint32) * width32, "ram_data");
    SHR_NULL_CHECK(ram_data, _SHR_E_MEMORY, "data is NULL!");
    default_val = sal_alloc(sizeof(uint32) * width32, "default_val");
    SHR_NULL_CHECK(default_val, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(default_val, 0, sizeof(uint32) * width32);
    patten = *((uint32 *) user_data);
    if ((flags & DIAG_FRAMER_TEST_FLAG_SET_PATTEN) == 0)
    {
        patten = 0;
    }
    diag_framer_get_regram_test_value(unit, width, patten, default_val);
    _shr_format_long_integer(def_data_str, default_val, width8);
    for (ii = 0; ii < depth; ii++)
    {
        sal_memset(ram_data, 0, sizeof(uint32) * width32);
        rv = (framer_ramp_read(unit, module_inst->module_addr, reg_info->offset_addr, ii, width, ram_data));

        if ((rv != 0) || (!SHR_BITEQ_RANGE(ram_data, default_val, 0, width)))
        {
            _shr_format_long_integer(ram_data_str, ram_data, width8);
            if (new_module)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", module_inst->inst_name);
                PRT_CELL_SET("0x%x", module_inst->module_addr);
                new_module = 0;
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("");
                PRT_CELL_SET("");
            }
            PRT_CELL_SET("%s[%d - %d] %s", reg_info->reg_name, depth, width,
                         _diag_frame_reg_type_str(reg_info->reg_type));
            PRT_CELL_SET("%d", ii);
            if (rv == 0)
            {
                PRT_CELL_SET("%s", ram_data_str);
                PRT_CELL_SET("%s", def_data_str);
            }
            else
            {
                PRT_CELL_SET("Read fail-%d", rv);
                PRT_CELL_SET("%s", def_data_str);
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static int
framer_ram_set_cb(
    int unit,
    uint32 flags,
    int module_idx,
    int regram_idx,
    prt_control_t * prt_ctr,
    void *user_data)
{
    int rv;
    J2X_CHIP_S *sys_info = NULL;
    J2X_MODULE_INST_S *module_inst;
    REG_INFO_S *reg_info;
    uint32 *ram_data = NULL;
    uint32 ii, depth, width, width32;
    uint32 patten = 0;

    SHR_FUNC_INIT_VARS(unit);
    if (module_idx < 0)
    {
        SHR_EXIT();
    }
    if (regram_idx < 0)
    {
        SHR_EXIT();
    }

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    module_inst = sys_info->module_inst + module_idx;
    reg_info = module_inst->module->regs + regram_idx;

    if (!_diag_framer_is_reg_type_mem(reg_info->reg_type))
    {
        SHR_EXIT();
    }
    if ((_diag_framer_is_regmem_ro(reg_info->reg_type)) || (_diag_framer_is_regmem_wo(reg_info->reg_type)))
    {
        SHR_EXIT();
    }

    depth = reg_info->ext & MASK_20_BIT;
    width = ((reg_info->ext >> SHIFT_BIT_20) & MASK_10_BIT) + 1;
    width32 = (width + 31) / 32;
    ram_data = sal_alloc(sizeof(uint32) * width32, "ram_data");
    SHR_NULL_CHECK(ram_data, _SHR_E_MEMORY, "data is NULL!");
    sal_memset(ram_data, 0, sizeof(uint32) * width32);
    patten = *((uint32 *) user_data);
    if ((flags & DIAG_FRAMER_TEST_FLAG_SET_PATTEN) == 0)
    {
        patten = 0;
    }
    diag_framer_get_regram_test_value(unit, width, patten, ram_data);

    for (ii = 0; ii < depth; ii++)
    {
        rv = framer_ramp_write(unit, module_inst->module_addr, reg_info->offset_addr, ii, width, ram_data);
        if (rv != 0)
        {
            cli_out("framer_ram_set_cb fail %s---%s, %d\n", module_inst->inst_name, reg_info->reg_name, rv);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_framer_test_reg_default: 
 *      Iter framer core registers and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] flags - flags.
 * \param [in] module_name - module_name.
 * \param [in] patten - pattern
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_framer_test_reg_default(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & DIAG_FRAMER_TEST_FLAG_SET)
    {
        SHR_IF_ERR_EXIT(_diag_framer_module_regram_iter(unit, flags, module_name,
                                                        NULL, framer_reg_set_cb, prt_ctr, &patten));
    }

    PRT_TITLE_SET("Framer register info");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("BaseAddr");
    PRT_COLUMN_ADD("regName");
    PRT_COLUMN_ADD("regAddr");
    PRT_COLUMN_ADD("value");
    PRT_COLUMN_ADD("default");
    SHR_IF_ERR_EXIT(_diag_framer_module_regram_iter(unit, flags, module_name,
                                                    NULL, framer_reg_dump_chg_cb, prt_ctr, &patten));
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_framer_test_ram_default(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (flags & DIAG_FRAMER_TEST_FLAG_SET)
    {
        SHR_IF_ERR_EXIT(_diag_framer_module_regram_iter(unit, flags, module_name,
                                                        NULL, framer_ram_set_cb, prt_ctr, &patten));
    }

    PRT_TITLE_SET("Framer Ram info");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("BaseAddr");
    PRT_COLUMN_ADD("RamName");
    PRT_COLUMN_ADD("Entry");
    PRT_COLUMN_ADD("value");
    PRT_COLUMN_ADD("default");
    SHR_IF_ERR_EXIT(_diag_framer_module_regram_iter(unit, flags, module_name,
                                                    NULL, framer_ram_dump_chg_cb, prt_ctr, &patten));
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_framer_test_list_module(
    int unit,
    uint32 flags,
    char *module_name,
    uint32 patten,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Framer module info");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Module name");
    PRT_COLUMN_ADD("module_addr");
    PRT_COLUMN_ADD("regType");
    PRT_COLUMN_ADD("regInfo");
    SHR_IF_ERR_EXIT(_diag_framer_module_regram_iter(unit, flags, module_name,
                                                    NULL, framer_ramreg_list_cb, prt_ctr, &patten));
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#if 0
static int
_diag_reg_read_field_and_check_default(
    int unit,
    uint32 reg_no,
    uint32 index,
    uint32 *field_value,
    uint32 *match)
{
    uint32 ii;
    uint32 is_match;

    SHR_FUNC_INIT_VARS(unit);

    if (reg_no >= COUNTOF(all_reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    SHR_NULL_CHECK(field_value, _SHR_E_PARAM, "field_value");
    SHR_NULL_CHECK(match, _SHR_E_PARAM, "match");

    is_match = 1;
    for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
    {
        SHR_IF_ERR_EXIT(diag_flexe_test_reg_id_field_read(unit, reg_no, index, ii, field_value + ii));
        if (all_reg[reg_no].reg_field[ii].default_value != field_value[ii])
        {
            is_match = 0;
        }
    }
    *match = is_match;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_test_reg_default: 
 *      Iter flexe core registers and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start register id.
 * \param [in] end_id - end register id.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_framer_test_list_modu(
    int unit,
    sh_sand_control_t * sand_control)
{
    int rv = 0;
    J2X_CHIP_S *sys_info = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    rv = j2x_sal_framer_get_sys_info(unit, &sys_info);
    if (rv != 0)
    {
        SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
    }

    for (reg_no = start_id; reg_no <= end_id; reg_no++)
    {
        if (!_diag_flexe_reg_ram_is_readable(unit, 0, reg_no))
        {
            continue;
        }

        PRT_TITLE_SET("FlexE core Reg(%d) %s CHanGed entry", reg_no, all_reg[reg_no].reg_name);
        PRT_COLUMN_ADD("Instance");
        PRT_COLUMN_ADD("Field_no");
        PRT_COLUMN_ADD("Field Name");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("bits");
        PRT_COLUMN_ADD("value");
        PRT_COLUMN_ADD("default_value");
        PRT_COLUMN_ADD("CHanGed");

        for (index = 0; index < all_reg[reg_no].instance_num; index++)
        {
            SHR_IF_ERR_EXIT(_diag_reg_read_field_and_check_default(unit, reg_no, index, field_value, &is_match));
            if (!is_match)
            {
                /*_diag_flexe_reg_value_print(unit, reg_no, index, field_value, 1);*/
                for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%d", index);
                    PRT_CELL_SET("%d", ii);
                    PRT_CELL_SET("%s", all_reg[reg_no].reg_field[ii].field_name);
                    PRT_CELL_SET("0x%x + 0x%x + 0x%x", all_reg[reg_no].reg_field[ii].base_addr,
                                 all_reg[reg_no].reg_field[ii].offset_addr, index * all_reg[reg_no].instance_offset);
                    PRT_CELL_SET("%d~%d", all_reg[reg_no].reg_field[ii].start_bit,
                                 all_reg[reg_no].reg_field[ii].end_bit);
                    PRT_CELL_SET("0x%X", field_value[ii]);
                    PRT_CELL_SET("0x%X", all_reg[reg_no].reg_field[ii].default_value);
                    if (all_reg[reg_no].reg_field[ii].default_value != field_value[ii])
                    {
                        PRT_CELL_SET("Yes");
                    }
                    else
                    {
                        PRT_CELL_SET("No");
                    }
                }
            }
        }
    }
    PRT_COMMITX;

    if (err_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_CONFIG);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#endif

#endif /* NO_FRAMER_LIB_BUILD */
