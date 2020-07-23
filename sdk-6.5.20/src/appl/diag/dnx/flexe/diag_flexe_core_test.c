/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/types.h>
#include <soc/defs.h>
#include <soc/chip.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include "diag_dnx_flexe.h"
#include "diag_flexe_adapt.h"

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/system.h>

#ifdef DIAG_FLEXE_CORE_DRIVER_AVALIABLE

/* Flexe core include */
#include "reg_operate.h"
#include "hal.h"
#include "b66sar_rx_tx.h"
#include "global_macro.h"
#include "reg_ram_test.h"

#define DIAG_FLEXE_CORE_MAX_FIELD_NUM   32

static int
_diag_flexe_reg_ram_is_readable(
    int unit,
    int is_ram,
    uint32 reg_ram_no)
{
    int rc = 0;
    char rw_atr[3];

    sal_memset(rw_atr, 0, sizeof(rw_atr));
    if (is_ram)
    {
        if (reg_ram_no < COUNTOF(all_ram))
        {
            sal_memcpy(rw_atr, all_ram[reg_ram_no].atr, sizeof(rw_atr));
        }
    }
    else
    {
        if (reg_ram_no < COUNTOF(all_reg))
        {
            sal_memcpy(rw_atr, all_reg[reg_ram_no].atr, sizeof(rw_atr));
        }
    }

    if (('R' == rw_atr[0]) || ('R' == rw_atr[1]))
    {
        rc = 1;
    }

    return rc;
}

static int
_diag_flexe_reg_ram_is_writeable(
    int unit,
    int is_ram,
    uint32 reg_ram_no)
{
    int rc = 0;
    char rw_atr[3];

    sal_memset(rw_atr, 0, sizeof(rw_atr));
    if (is_ram)
    {
        if (reg_ram_no < COUNTOF(all_ram))
        {
            sal_memcpy(rw_atr, all_ram[reg_ram_no].atr, sizeof(rw_atr));
        }
    }
    else
    {
        if (reg_ram_no < COUNTOF(all_reg))
        {
            sal_memcpy(rw_atr, all_reg[reg_ram_no].atr, sizeof(rw_atr));
        }
    }

    if (('W' == rw_atr[0]) || ('W' == rw_atr[1]))
    {
        rc = 1;
    }

    return rc;
}

shr_error_e
diag_flexe_test_reg_id_field_read(
    int unit,
    uint32 reg_no,
    uint32 index,
    uint32 field_no,
    uint32 *field_value)
{
    int reg_addr;
    uint32 start_bit, end_bit, nof_bits;
    uint32 reg_value;
    NEW_CHIP_REG *new_chip_reg;

    SHR_FUNC_INIT_VARS(unit);

    if (reg_no >= COUNTOF(all_reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (index >= all_reg[reg_no].instance_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (field_no >= all_reg[reg_no].field_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    SHR_NULL_CHECK(field_value, _SHR_E_PARAM, "field_value");

    new_chip_reg = all_reg[reg_no].reg_field + field_no;
    reg_addr = new_chip_reg->base_addr + new_chip_reg->offset_addr;
    reg_addr = reg_addr + index * all_reg[reg_no].instance_offset;
    start_bit = new_chip_reg->start_bit;
    end_bit = new_chip_reg->end_bit;
    nof_bits = end_bit - start_bit + 1;
    SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, reg_addr, &reg_value));

     /* coverity[ARRAY_VS_SINGLETON:FALSE]  */
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(&reg_value, start_bit, nof_bits, field_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_test_reg_id_all_fields_write(
    int unit,
    uint32 reg_no,
    uint32 index,
    uint32 *field_value)
{
    uint32 reg_num, reg_idx, ii, field_num;
    uint32 reg_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];
    NEW_CHIP_REG *new_chip_reg;
    uint32 start_bit, end_bit, nof_bits;
    int reg_addr;
    int dump_flag;

    SHR_FUNC_INIT_VARS(unit);

    if (reg_no >= COUNTOF(all_reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (index >= all_reg[reg_no].instance_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    SHR_NULL_CHECK(field_value, _SHR_E_PARAM, "field_value");

    field_num = all_reg[reg_no].field_num;
    reg_num = all_reg[reg_no].reg_field[field_num - 1].offset_addr - all_reg[reg_no].reg_field[0].offset_addr + 1;
    if (reg_num > FLEXE_CORE_REG_MEM_MAX_SIZE_U32)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_PARAM, "reg_num=%d reg_no=%d%s\n", reg_num, reg_no, EMPTY);
    }

    dump_flag = 0;
    sal_memset(reg_value, 0, sizeof(reg_value));
    for (ii = 0; ii < field_num; ii++)
    {
        new_chip_reg = all_reg[reg_no].reg_field + ii;
        reg_idx = all_reg[reg_no].reg_field[ii].offset_addr - all_reg[reg_no].reg_field[0].offset_addr;
        start_bit = new_chip_reg->start_bit;
        end_bit = new_chip_reg->end_bit;
        nof_bits = end_bit - start_bit + 1;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(reg_value + reg_idx, start_bit, nof_bits, field_value[ii]));

        if (field_value[ii] != 0)
        {
            dump_flag = 1;
        }
    }

    if ((index == 0) && (dump_flag))
    {
        cli_out("reg: %d(%s), field_num=%d\n", reg_no, all_reg[reg_no].reg_name, all_reg[reg_no].field_num);
        for (ii = 0; ii < field_num; ii++)
        {
            new_chip_reg = all_reg[reg_no].reg_field + ii;
            cli_out("%-4d0x%-8x0x%-6x%-2d %-2d    0x%-8X\n", ii,
                    new_chip_reg->base_addr, new_chip_reg->offset_addr,
                    new_chip_reg->start_bit, new_chip_reg->end_bit, field_value[ii]);
        }
        cli_out("reg_num=%d:    ", reg_num);
        for (ii = 0; ii < reg_num; ii++)
        {
            cli_out("%08X ", reg_value[ii]);
        }
        cli_out("\n\n");
    }

    reg_addr = all_reg[reg_no].reg_field[0].base_addr + all_reg[reg_no].reg_field[0].offset_addr;
    for (ii = 0; ii < reg_num; ii++)
    {
        SHR_IF_ERR_EXIT(diag_flexe_reg_write(unit, reg_addr + ii, reg_value[ii]));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_test_ram_id_read(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *ram_value)
{
    int ram_addr;
    SHR_FUNC_INIT_VARS(unit);

    if (ram_no >= COUNTOF(all_ram))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (!_diag_flexe_reg_ram_is_readable(unit, 1, ram_no))
    {
        SHR_IF_ERR_EXIT(_SHR_E_LIMIT);
    }
    if (array_idx >= all_ram[ram_no].instance_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (entry_idx >= all_ram[ram_no].depth)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    SHR_NULL_CHECK(ram_value, _SHR_E_PARAM, "ram_value");

    ram_addr = all_ram[ram_no].ram_name->ram_addr;
    ram_addr = ram_addr + array_idx * all_ram[ram_no].instance_offset;
    SHR_IF_ERR_EXIT(diag_flexe_mem_read(unit, ram_addr, entry_idx, all_ram[ram_no].ram_name->width, ram_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_flexe_test_ram_id_write(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *ram_value)
{
    int ram_addr;
    SHR_FUNC_INIT_VARS(unit);

    if (ram_no >= COUNTOF(all_ram))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (!_diag_flexe_reg_ram_is_writeable(unit, 1, ram_no))
    {
        cli_out("Error: this ram %d(%s) is not writeable!!\n", ram_no, all_ram[ram_no].ram_name_str);
        SHR_EXIT();
    }
    if (array_idx >= all_ram[ram_no].instance_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (entry_idx >= all_ram[ram_no].depth)
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    SHR_NULL_CHECK(ram_value, _SHR_E_PARAM, "ram_value");

    ram_addr = all_ram[ram_no].ram_name->ram_addr;
    ram_addr = ram_addr + array_idx * all_ram[ram_no].instance_offset;
    SHR_IF_ERR_EXIT(diag_flexe_mem_write(unit, ram_addr, entry_idx, all_ram[ram_no].ram_name->width, ram_value));

exit:
    SHR_FUNC_EXIT;
}

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

#if 0
static int
_diag_ram_read_field_and_check_default(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *ram_value,
    uint32 *match)
{
    char *default_value_str;
    uint32 default_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];

    SHR_FUNC_INIT_VARS(unit);

    if (ram_no >= COUNTOF(all_ram))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    SHR_NULL_CHECK(ram_value, _SHR_E_PARAM, "ram_value");
    SHR_NULL_CHECK(match, _SHR_E_PARAM, "match");

    SHR_IF_ERR_EXIT(diag_flexe_test_ram_id_read(unit, ram_no, array_idx, entry_idx, ram_value));

    default_value_str = all_ram[ram_no].default_value_str;
    sal_memset(default_value, 0, sizeof(default_value));
    parse_long_integer(default_value, COUNTOF(default_value), default_value_str);
    if (SHR_BITEQ_RANGE(ram_value, default_value, 0, all_ram[ram_no].ram_name->width))
    {
        *match = 1;
    }
    else
    {
        *match = 0;
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

#if 1
static void
_diag_flexe_reg_value_print_exp(
    int unit,
    uint32 error_num,
    uint32 reg_no,
    uint32 index,
    uint32 *field_value)
{

    uint32 ii;

    cli_out("%d @ %d @ %s @ %d @ %s @ index=%d @ @\n", error_num, reg_no,
            all_reg[reg_no].reg_name, all_reg[reg_no].field_num, all_reg[reg_no].atr, index);
    for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
    {
        cli_out(" @ %d @ %s @ 0x%06x+0x%02x @ %d - %d @ 0x%08X @ 0x%08X @",
                ii, all_reg[reg_no].reg_field[ii].field_name,
                all_reg[reg_no].reg_field[ii].base_addr, all_reg[reg_no].reg_field[ii].offset_addr,
                all_reg[reg_no].reg_field[ii].start_bit, all_reg[reg_no].reg_field[ii].end_bit,
                field_value[ii], all_reg[reg_no].reg_field[ii].default_value);
        if (all_reg[reg_no].reg_field[ii].default_value != field_value[ii])
        {
            cli_out("noMatch!!\n");
        }
        else
        {
            cli_out("\n");
        }
    }
}
#endif

static void
_diag_flexe_reg_value_print(
    int unit,
    uint32 reg_no,
    uint32 index,
    uint32 *field_value,
    uint32 dump_default)
{
    uint32 ii;
    cli_out("reg_no=%d, reg_name=%s[%d], field_num=%d, array info: %d/%d, %s\n",
            reg_no, all_reg[reg_no].reg_name, index, all_reg[reg_no].field_num,
            all_reg[reg_no].instance_num, all_reg[reg_no].instance_offset, all_reg[reg_no].atr);
    for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
    {
        cli_out("    %-6d%-30s0x%06x+0x%02x (%-2d : %-2d):   0x%08X",
                ii, all_reg[reg_no].reg_field[ii].field_name, all_reg[reg_no].reg_field[ii].base_addr,
                all_reg[reg_no].reg_field[ii].offset_addr, all_reg[reg_no].reg_field[ii].start_bit,
                all_reg[reg_no].reg_field[ii].end_bit, field_value[ii]);
        if (dump_default)
        {
            cli_out("(def: 0x%08X)", all_reg[reg_no].reg_field[ii].default_value);
            if (all_reg[reg_no].reg_field[ii].default_value != field_value[ii])
            {
                cli_out("    noMatch!!");
            }
            cli_out("\n");
        }
    }
    cli_out("    --------------------------------------------------");
    cli_out("--------------------------------------------------\n\n");
}

/**
 * \brief
 *   diag_flexe_test_reg_default: 
 *      Iter flexe core registers and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start register id.
 * \param [in] end_id - end register id.
 * \param [in] flags - flags.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_reg_default(
    int unit,
    uint32 start_id,
    uint32 end_id,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    uint32 ii, err_num = 0;
    uint32 reg_no, index, is_match;
    uint32 field_value[DIAG_FLEXE_CORE_MAX_FIELD_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (start_id > end_id)
    {
        start_id = end_id;
    }
    if (end_id >= COUNTOF(all_reg))
    {
        end_id = COUNTOF(all_reg) - 1;
    }

    for (reg_no = start_id; reg_no <= end_id; reg_no++)
    {
        if (!_diag_flexe_reg_ram_is_readable(unit, 0, reg_no))
        {
            continue;
        }

        if (flags & 0x1)
        {
            if (!_diag_flexe_reg_ram_is_writeable(unit, 0, reg_no))
            {
                continue;
            }
        }

        if (flags & 0x10)
        {
            PRT_TITLE_SET("FlexE core Reg(%d) %s %s", reg_no, all_reg[reg_no].reg_name, all_reg[reg_no].atr);
            PRT_COLUMN_ADD("Instance");
            PRT_COLUMN_ADD("Field_no");
            PRT_COLUMN_ADD("Field Name");
            PRT_COLUMN_ADD("Address");
            PRT_COLUMN_ADD("bits");
            PRT_COLUMN_ADD("value");
            PRT_COLUMN_ADD("default_value");
            PRT_COLUMN_ADD("CHanGed");
        }

        for (index = 0; index < all_reg[reg_no].instance_num; index++)
        {
            SHR_IF_ERR_EXIT(_diag_reg_read_field_and_check_default(unit, reg_no, index, field_value, &is_match));
            if (!is_match)
            {
                if (flags & 0x10)
                {
                    for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%d", index);
                        PRT_CELL_SET("%d", ii);
                        PRT_CELL_SET("%s", all_reg[reg_no].reg_field[ii].field_name);
                        PRT_CELL_SET("0x%x + 0x%x + 0x%x", all_reg[reg_no].reg_field[ii].base_addr,
                                     all_reg[reg_no].reg_field[ii].offset_addr,
                                     index * all_reg[reg_no].instance_offset);
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
                else
                {
                    if (flags & 0x100)
                    {
                        _diag_flexe_reg_value_print_exp(unit, err_num, reg_no, index, field_value);
                    }
                    else
                    {
                        _diag_flexe_reg_value_print(unit, reg_no, index, field_value, 1);
                    }
                }

                err_num++;
                break;
            }
        }

        if (flags & 0x10)
        {
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_test_reg_default_set: 
 *      Iter flexe core registers set all field with default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start register id.
 * \param [in] end_id - end register id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_reg_default_set(
    int unit,
    uint32 start_id,
    uint32 end_id)
{
    uint32 ii, reg_no, index;
    uint32 field_value[DIAG_FLEXE_CORE_MAX_FIELD_NUM];

    SHR_FUNC_INIT_VARS(unit);

    if (start_id > end_id)
    {
        start_id = end_id;
    }
    if (end_id >= COUNTOF(all_reg))
    {
        end_id = COUNTOF(all_reg) - 1;
    }

    for (reg_no = start_id; reg_no <= end_id; reg_no++)
    {
        if ((!_diag_flexe_reg_ram_is_readable(unit, 0, reg_no)) || (!_diag_flexe_reg_ram_is_writeable(unit, 0, reg_no)))
        {
            continue;
        }

        sal_memset(field_value, 0, sizeof(field_value));
        for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
        {
            field_value[ii] = all_reg[reg_no].reg_field[ii].default_value;
        }

        for (index = 0; index < all_reg[reg_no].instance_num; index++)
        {
            SHR_IF_ERR_EXIT(diag_flexe_test_reg_id_all_fields_write(unit, reg_no, index, field_value));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_test_ram_default: 
 *      Iter flexe core ram and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start_ram_id.
 * \param [in] end_id - end_ram_id.
 * \param [in] flags - flags.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_ram_default(
    int unit,
    uint32 start_id,
    uint32 end_id,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    uint32 err_num = 0;
    uint32 ram_no;
    uint32 array_idx, entry_idx;
    uint32 ram_depth, ram_width, width_8;
    uint32 default_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];
    uint32 read_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];
    char *default_value_str;
    char read_value_str[64];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (start_id > end_id)
    {
        start_id = end_id;
    }
    if (end_id >= COUNTOF(all_ram))
    {
        end_id = COUNTOF(all_ram) - 1;
    }

    PRT_TITLE_SET("FlexE RAM default check");
    PRT_COLUMN_ADD("Index");
    PRT_COLUMN_ADD("Ram No");
    PRT_COLUMN_ADD("Ram Name");
    PRT_COLUMN_ADD("array/entry");
    PRT_COLUMN_ADD("read value");
    PRT_COLUMN_ADD("default_value");
    for (ram_no = start_id; ram_no <= end_id; ram_no++)
    {
        if (!_diag_flexe_reg_ram_is_readable(unit, 1, ram_no))
        {
            continue;
        }

        if (flags & 0x1)
        {
            if (!_diag_flexe_reg_ram_is_writeable(unit, 1, ram_no))
            {
                continue;
            }
        }

        ram_depth = all_ram[ram_no].depth;
        ram_width = all_ram[ram_no].ram_name->width;
        width_8 = BITS2BYTES(ram_width);

        default_value_str = all_ram[ram_no].default_value_str;
        sal_memset(default_value, 0, sizeof(default_value));
        parse_long_integer(default_value, COUNTOF(default_value), default_value_str);

        for (array_idx = 0; array_idx < all_ram[ram_no].instance_num; array_idx++)
        {
            for (entry_idx = 0; entry_idx < ram_depth; entry_idx++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_test_ram_id_read(unit, ram_no, array_idx, entry_idx, read_value));

                if (!SHR_BITEQ_RANGE(read_value, default_value, 0, all_ram[ram_no].ram_name->width))
                {
                    _shr_format_long_integer(read_value_str, read_value, width_8);
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%d", err_num);
                    PRT_CELL_SET("%d %s", ram_no, all_ram[ram_no].atr);
                    PRT_CELL_SET("%s", all_ram[ram_no].ram_name_str);
                    PRT_CELL_SET("%d/%d", array_idx, entry_idx);
                    PRT_CELL_SET("%s", read_value_str);
                    PRT_CELL_SET("%s", default_value_str);

                    err_num++;
                    break;
                }
            }
        }
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_test_ram_default_set: 
 *      Iter flexe core ram write with default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start_ram_id.
 * \param [in] end_id - end_ram_id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_ram_default_set(
    int unit,
    uint32 start_id,
    uint32 end_id)
{
    uint32 ram_no;
    uint32 array_idx, entry_idx;
    uint32 ram_depth;
    uint32 default_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];
    char *default_value_str;

    SHR_FUNC_INIT_VARS(unit);

    if (start_id > end_id)
    {
        start_id = end_id;
    }
    if (end_id >= COUNTOF(all_ram))
    {
        end_id = COUNTOF(all_ram) - 1;
    }

    for (ram_no = start_id; ram_no <= end_id; ram_no++)
    {
        if ((!_diag_flexe_reg_ram_is_readable(unit, 1, ram_no)) || (!_diag_flexe_reg_ram_is_writeable(unit, 1, ram_no)))
        {
            continue;
        }

        ram_depth = all_ram[ram_no].depth;

        default_value_str = all_ram[ram_no].default_value_str;
        sal_memset(default_value, 0, sizeof(default_value));
        parse_long_integer(default_value, COUNTOF(default_value), default_value_str);

        cli_out("ram_no=%d, name=%s\n", ram_no, all_ram[ram_no].ram_name_str);
        cli_out("    default_value_str =  %s\n", default_value_str);
        cli_out("    default_value     =  %08X %08X %08X %08X\n\n",
                default_value[0], default_value[1], default_value[2], default_value[3]);

        for (array_idx = 0; array_idx < all_ram[ram_no].instance_num; array_idx++)
        {
            for (entry_idx = 0; entry_idx < ram_depth; entry_idx++)
            {
                SHR_IF_ERR_EXIT(diag_flexe_test_ram_id_write(unit, ram_no, array_idx, entry_idx, default_value));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if 0
/**
 * \brief
 *   diag_flexe_test_reg_write: 
 *      Write register with specfic value and read&check if success
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_reg_write(
    int unit,
    uint32 mode)
{
    uint32 err_num = 0;
    uint32 reg_no, field_no;
    uint32 wr_data[4] = { 0x0, 0xffffffff, 0x55555555, 0xaaaaaaaa };
    uint32 write_value, read_value;
    int reg_addr, reg_addr_tmp;

    if (mode >= COUNTOF(wr_data))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    write_value = wr_data[mode];

    for (reg_no = 0; reg_no < COUNTOF(all_reg); reg_no++)
    {
        if (!_diag_flexe_reg_ram_is_writeable(unit, 0, reg_no))
        {
            continue;
        }

        reg_addr = -1;
        for (field_no = 0; field_no < all_reg[reg_no].field_num; field_no++)
        {
            reg_addr_tmp = all_reg[reg_no].reg_field[field_no].base_addr +
                all_reg[reg_no].reg_field[field_no].offset_addr;
            if (reg_addr != reg_addr_tmp)
            {
                reg_addr = reg_addr_tmp;
                SHR_IF_ERR_EXIT(diag_flexe_reg_write(unit, reg_addr, write_value));
                SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, reg_addr, &read_value));
                if (read_value != write_value)
                {
                    err_num++;
                    cli_out("    reg (%s),  field(%s): write_value=0x%x, read_value=0x%x\n",
                            all_reg[reg_no].reg_name, all_reg[reg_no].reg_field[field_no].field_name,
                            write_value, read_value);
                    break;
                }
            }
        }
    }

    if (err_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_test_ram_write: 
 *      Write ram with specfic value and read&check if success
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_ram_write(
    int unit,
    uint32 mode)
{
    uint32 err_num = 0;
    uint32 reg_no, field_no;
    uint32 wr_data[4] = { 0x0, 0xffffffff, 0x55555555, 0xaaaaaaaa };
    uint32 write_data, read_value;
    int reg_addr, reg_addr_tmp;

    if (mode >= COUNTOF(wr_data))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    write_data = wr_data[mode];

    for (reg_no = 0; reg_no < COUNTOF(all_reg); reg_no++)
    {
        if (!_diag_flexe_reg_ram_is_writeable(unit, 0, reg_no))
        {
            continue;
        }

        reg_addr = -1;
        for (field_no = 0; field_no < all_reg[reg_no].field_num; field_no++)
        {
            reg_addr_tmp = all_reg[reg_no].reg_field[field_no].base_addr +
                all_reg[reg_no].reg_field[field_no].offset_addr;
            if (reg_addr != reg_addr_tmp)
            {
                reg_addr = reg_addr_tmp;
                SHR_IF_ERR_EXIT(diag_flexe_reg_write(unit, reg_addr, write_value));
                SHR_IF_ERR_EXIT(diag_flexe_reg_read(unit, reg_addr, &read_value));
                if (read_value != write_value)
                {
                    err_num++;
                    cli_out("    reg (%s),  field(%s): write_value=0x%x, read_value=0x%x\n",
                            all_reg[reg_no].reg_name, all_reg[reg_no].reg_field[field_no].field_name,
                            write_value, read_value);
                    break;
                }
            }
        }
    }

    if (err_num)
    {
        SHR_IF_ERR_EXIT(_SHR_E_CONFIG);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief
 *   diag_flexe_reg_id_dump: 
 *      dump all fields of a single reg by reg_no
 * \param [in] unit - The unit number.
 * \param [in] reg_no - reg_no.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_reg_id_dump(
    int unit,
    uint32 reg_no,
    sh_sand_control_t * sand_control)
{
    uint32 ii, index, is_match;
    uint32 field_value[DIAG_FLEXE_CORE_MAX_FIELD_NUM];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (reg_no >= COUNTOF(all_reg))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

    if (!_diag_flexe_reg_ram_is_readable(unit, 0, reg_no))
    {
        cli_out("Error: this reg(%s) is not readable!!\n", all_reg[reg_no].reg_name);
        SHR_EXIT();
    }

    PRT_TITLE_SET("FlexE core Reg(%d) %s %s", reg_no, all_reg[reg_no].reg_name, all_reg[reg_no].atr);
    PRT_COLUMN_ADD("Instance");
    PRT_COLUMN_ADD("Field_no");
    PRT_COLUMN_ADD("Field Name");
    PRT_COLUMN_ADD("Address");
    PRT_COLUMN_ADD("bits");
    PRT_COLUMN_ADD("value");

    for (index = 0; index < all_reg[reg_no].instance_num; index++)
    {
        SHR_IF_ERR_EXIT(_diag_reg_read_field_and_check_default(unit, reg_no, index, field_value, &is_match));

        /*_diag_flexe_reg_value_print(unit, reg_no, index, field_value, 1);*/
        for (ii = 0; ii < all_reg[reg_no].field_num; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", index);
            PRT_CELL_SET("%d", ii);
            PRT_CELL_SET("%s", all_reg[reg_no].reg_field[ii].field_name);
            PRT_CELL_SET("0x%x + 0x%x + 0x%x", all_reg[reg_no].reg_field[ii].base_addr,
                         all_reg[reg_no].reg_field[ii].offset_addr, index * all_reg[reg_no].instance_offset);
            PRT_CELL_SET("%d~%d", all_reg[reg_no].reg_field[ii].start_bit, all_reg[reg_no].reg_field[ii].end_bit);
            PRT_CELL_SET("0x%X", field_value[ii]);
        }

    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_ram_id_dump: 
 *      dump range entry of ram
 * \param [in] unit - The unit number.
 * \param [in] ram_no - ram_no.
 * \param [in] array_idx - array_idx.
 * \param [in] start_entry - start_entry.
 * \param [in] end_entry - end_entry.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_ram_id_dump(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 start_entry,
    uint32 end_entry,
    sh_sand_control_t * sand_control)
{
    uint32 entry_index;
    uint32 ram_width, width_8;
    uint32 read_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];
    char read_value_str[64];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (ram_no >= COUNTOF(all_ram))
    {
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
    if (start_entry > end_entry)
    {
        start_entry = end_entry;
    }
    if (end_entry >= all_ram[ram_no].depth)
    {
        end_entry = all_ram[ram_no].depth - 1;
    }

    if (!_diag_flexe_reg_ram_is_readable(unit, 1, ram_no))
    {
        cli_out("Error: this ram(%s) is not readable!!\n", all_ram[ram_no].ram_name_str);
        SHR_EXIT();
    }

    ram_width = all_ram[ram_no].ram_name->width;
    width_8 = BITS2BYTES(ram_width);

    PRT_TITLE_SET("FlexE RAM=%d %s %s", ram_no, all_ram[ram_no].ram_name_str, all_ram[ram_no].atr);
    PRT_COLUMN_ADD("[array][entry]");
    PRT_COLUMN_ADD("value (default_value=%s)", all_ram[ram_no].default_value_str);
    for (entry_index = start_entry; entry_index <= end_entry; entry_index++)
    {
        SHR_IF_ERR_EXIT(diag_flexe_test_ram_id_read(unit, ram_no, array_idx, entry_index, read_value));
        _shr_format_long_integer(read_value_str, read_value, width_8);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("[%d][%d]", array_idx, entry_index);
        PRT_CELL_SET("%s", read_value_str);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_reg_ram_list: 
 *      Iter flexe core registers/ram and list required id
 * \param [in] unit - The unit number.
 * \param [in] is_ram - if list ram.
 * \param [in] list_string - list_string.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_reg_ram_list(
    int unit,
    int is_ram,
    char *list_string,
    sh_sand_control_t * sand_control)
{
    uint32 id;
    char *default_value_str;
    uint32 default_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (is_ram)
    {
        PRT_TITLE_SET("FlexE core RAM list--[%s]", (list_string ? list_string : "*"));
        PRT_COLUMN_ADD("RAM ID");
        PRT_COLUMN_ADD("RAM Name");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("Array");
        PRT_COLUMN_ADD("Depth");
        PRT_COLUMN_ADD("Width");
        PRT_COLUMN_ADD("rw_atr");
        PRT_COLUMN_ADD("default_value");

        for (id = 0; id < COUNTOF(all_ram); id++)
        {
            if (list_string != NULL)
            {
                if ((sal_strcasestr(all_ram[id].ram_name_str, list_string)) == NULL)
                {
                    continue;
                }
            }

            default_value_str = all_ram[id].default_value_str;
            sal_memset(default_value, 0, sizeof(default_value));
            parse_long_integer(default_value, COUNTOF(default_value), default_value_str);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", id);
            PRT_CELL_SET("%s", all_ram[id].ram_name_str);
            PRT_CELL_SET("0x%x", all_ram[id].ram_name->ram_addr);
            PRT_CELL_SET("%d/%d", all_ram[id].instance_num, all_ram[id].instance_offset);
            PRT_CELL_SET("%d", all_ram[id].depth);
            PRT_CELL_SET("%d", all_ram[id].ram_name->width);
            PRT_CELL_SET("%s", all_ram[id].atr);
            PRT_CELL_SET("%s %08X %08X", all_ram[id].default_value_str, default_value[0], default_value[1]);
        }
    }
    else
    {
        PRT_TITLE_SET("FlexE core Reg list--[%s]", (list_string ? list_string : "*"));
        PRT_COLUMN_ADD("Reg ID");
        PRT_COLUMN_ADD("Reg Name");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("Array");
        PRT_COLUMN_ADD("field_num");
        PRT_COLUMN_ADD("rw_atr");

        for (id = 0; id < COUNTOF(all_reg); id++)
        {
            if (list_string != NULL)
            {
                if ((sal_strcasestr(all_reg[id].reg_name, list_string)) == NULL)
                {
                    continue;
                }
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", id);
            PRT_CELL_SET("%s", all_reg[id].reg_name);
            PRT_CELL_SET("0x%x+%d", all_reg[id].reg_field[0].base_addr, all_reg[id].reg_field[0].offset_addr);
            PRT_CELL_SET("%d/%d", all_reg[id].instance_num, all_reg[id].instance_offset);
            PRT_CELL_SET("%d", all_reg[id].field_num);
            PRT_CELL_SET("%s", all_reg[id].atr);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   diag_flexe_reg_ram_list: 
 *      Iter flexe core registers/ram and list required id
 * \param [in] unit - The unit number.
 * \param [in] is_ram - if list ram.
 * \param [in] start_id - start_id.
 * \param [in] end_id - end_id.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_reg_ram_id_list(
    int unit,
    int is_ram,
    uint32 start_id,
    uint32 end_id,
    sh_sand_control_t * sand_control)
{
    uint32 id;
    char *default_value_str;
    uint32 default_value[FLEXE_CORE_REG_MEM_MAX_SIZE_U32];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (start_id > end_id)
    {
        start_id = end_id;
    }

    if (is_ram)
    {
        if (end_id >= COUNTOF(all_ram))
        {
            end_id = COUNTOF(all_ram) - 1;
        }

        PRT_TITLE_SET("FlexE core RAM list--[%d - %d]", start_id, end_id);
        PRT_COLUMN_ADD("RAM ID");
        PRT_COLUMN_ADD("RAM Name");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("Array");
        PRT_COLUMN_ADD("Depth");
        PRT_COLUMN_ADD("Width");
        PRT_COLUMN_ADD("rw_atr");
        PRT_COLUMN_ADD("default_value");

        for (id = start_id; id <= end_id; id++)
        {
            default_value_str = all_ram[id].default_value_str;
            sal_memset(default_value, 0, sizeof(default_value));
            parse_long_integer(default_value, COUNTOF(default_value), default_value_str);

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", id);
            PRT_CELL_SET("%s", all_ram[id].ram_name_str);
            PRT_CELL_SET("0x%x", all_ram[id].ram_name->ram_addr);
            PRT_CELL_SET("%d/%d", all_ram[id].instance_num, all_ram[id].instance_offset);
            PRT_CELL_SET("%d", all_ram[id].depth);
            PRT_CELL_SET("%d", all_ram[id].ram_name->width);
            PRT_CELL_SET("%s", all_ram[id].atr);
            PRT_CELL_SET("%s %08X %08X", all_ram[id].default_value_str, default_value[0], default_value[1]);
        }
    }
    else
    {
        if (end_id >= COUNTOF(all_reg))
        {
            end_id = COUNTOF(all_reg) - 1;
        }

        PRT_TITLE_SET("FlexE core Reg list--[%d-%d]", start_id, end_id);
        PRT_COLUMN_ADD("Reg ID");
        PRT_COLUMN_ADD("Reg Name");
        PRT_COLUMN_ADD("Address");
        PRT_COLUMN_ADD("Array");
        PRT_COLUMN_ADD("field_num");
        PRT_COLUMN_ADD("rw_atr");

        for (id = start_id; id <= end_id; id++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", id);
            PRT_CELL_SET("%s", all_reg[id].reg_name);
            PRT_CELL_SET("0x%x+%d", all_reg[id].reg_field[0].base_addr, all_reg[id].reg_field[0].offset_addr);
            PRT_CELL_SET("%d/%d", all_reg[id].instance_num, all_reg[id].instance_offset);
            PRT_CELL_SET("%d", all_reg[id].field_num);
            PRT_CELL_SET("%s", all_reg[id].atr);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
#else
shr_error_e
diag_flexe_reg_id_dump(
    int unit,
    uint32 reg_no,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_ram_id_dump(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 start_entry,
    uint32 end_entry,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_test_ram_id_write(
    int unit,
    uint32 ram_no,
    uint32 array_idx,
    uint32 entry_idx,
    uint32 *ram_value)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_reg_ram_list(
    int unit,
    int is_ram,
    char *list_string,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_reg_ram_id_list(
    int unit,
    int is_ram,
    uint32 start_id,
    uint32 end_id,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

shr_error_e
diag_flexe_test_reg_default(
    int unit,
    uint32 start_id,
    uint32 end_id,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   diag_flexe_test_reg_default_set: 
 *      Iter flexe core registers set all field with default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start register id.
 * \param [in] end_id - end register id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_reg_default_set(
    int unit,
    uint32 start_id,
    uint32 end_id)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   diag_flexe_test_ram_default: 
 *      Iter flexe core ram and check if be default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start_ram_id.
 * \param [in] end_id - end_ram_id.
 * \param [in] flags - flags.
 * \param [in] sand_control - sand_control.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_ram_default(
    int unit,
    uint32 start_id,
    uint32 end_id,
    uint32 flags,
    sh_sand_control_t * sand_control)
{
    return _SHR_E_UNAVAIL;
}

/**
 * \brief
 *   diag_flexe_test_ram_default_set: 
 *      Iter flexe core ram write with default value
 * \param [in] unit - The unit number.
 * \param [in] start_id - start_ram_id.
 * \param [in] end_id - end_ram_id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
diag_flexe_test_ram_default_set(
    int unit,
    uint32 start_id,
    uint32 end_id)
{
    return _SHR_E_UNAVAIL;
}

#endif
