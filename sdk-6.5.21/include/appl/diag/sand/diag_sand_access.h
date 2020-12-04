/*
 * ! \file diag_sand_access.h Purpose: shell registers commands for Dune Devices 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_SAND_ACCESS_H_INCLUDED
#define DIAG_SAND_ACCESS_H_INCLUDED

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#define MAX_FIELDS_NUM  512
#define SAND_DCL_CMD(_f)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char     _f##_usage[];

extern sh_sand_man_t sh_sand_access_man;
extern sh_sand_cmd_t sh_sand_access_cmds[];

extern const char cmd_sand_access_usage[];
extern const char cmd_sand_access_desc[];

cmd_result_t cmd_sand_access(
    int unit,
    args_t * a);

/**
 * \brief - read register and return its value and address
 *
 * \param [in] unit - unit number
 * \param [in] reg - register to read
 * \param [in] block_id - block id
 * \param [in] port - port index
 * \param [in] reg_index - register index in array register
 * \param [out] value - register value
 * \param [out] addr_p - register address
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e access_reg_read(
    int unit,
    soc_reg_t reg,
    soc_block_t block_id,
    int port,
    int reg_index,
    uint32 *value,
    uint32 *addr_p);

/**
 * \brief - print register in a tabular format - format should contain the following fields:
 *          Object, Block, Index, Address, Value and Property in this order
 *
 * \param [in] prt_ctr - pointer to print infra control structure
 * \param [in] unit - unit number
 * \param [in] reg - register to print
 * \param [in] block_name - block name
 * \param [in] reg_index - register index in array register
 * \param [in] address - register address
 * \param [in] reg_value - register value
 * \param [in] field_flag - if true - print register fields as well
 * \param [in] changed_flag - if true print only changed registers
 * \param [in] fld_match_list - allows filtering which fields to print and which not according to order.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e access_reg_print(
    prt_control_t * prt_ctr,
    int unit,
    soc_reg_t reg,
    char *block_name,
    int reg_index,
    uint32 address,
    soc_reg_above_64_val_t reg_value,
    int field_flag,
    int changed_flag,
    int *fld_match_list);

SAND_DCL_CMD(cmd_sand_reg_list)
SAND_DCL_CMD(cmd_sand_reg_get) SAND_DCL_CMD(cmd_sand_reg_set) SAND_DCL_CMD(cmd_sand_reg_modify)
/*
 * The parameter name is set when you want to dump a specific block.
 */
     int
     diag_sand_reg_get_all(
    int unit,
    int is_debug,
    char *name);

/*
 * List the tables, or fields of a table entry
 */
SAND_DCL_CMD(cmd_sand_mem_list)
SAND_DCL_CMD(cmd_sand_mem_get)
SAND_DCL_CMD(cmd_sand_mem_write)
SAND_DCL_CMD(cmd_sand_mem_modify) SAND_DCL_CMD(cmd_sand_dma) SAND_DCL_CMD(cmd_sand_intr)
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)
    SAND_DCL_CMD(cmd_sand_cpu_i2c) SAND_DCL_CMD(cmd_sand_cpu_regs)
#endif
    SAND_DCL_CMD(cmd_sand_pcie_reg) SAND_DCL_CMD(cmd_sand_pcic_access)
#endif /* DIAG_SAND_ACCESS_H_INCLUDED */
