/*! \file bcma_testutil_phy.c
 *
 * PHY operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmpc/bcmpc_util.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_phy.h>
#include <phymod/phymod_diag.h>
#include <bcma/test/util/bcma_testutil_phy.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

int
bcma_testutil_phy_set(int unit, int lport, const char *reg_name,
                      const char *field_name, uint32_t val)
{
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_phy_access_data_t acc_data;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    phymod_symbols_t *symbols_table = NULL;
    phymod_symbol_t symbol;
    uint32_t reg_addr, reg_data;
    phymod_field_info_t finfo;

    SHR_FUNC_ENTER(unit);

    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* get symbols table */
    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, pport);

    SHR_IF_ERR_EXIT
        (bcmpc_phymod_phy_access_t_init(unit, pport, 0, &acc_data, pa));

    if (phymod_diag_symbols_table_get(pa, &symbols_table) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* find the symbol according to the input reg name */
    if (phymod_symbols_find(reg_name, symbols_table, &symbol) < 0) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    reg_addr = symbol.addr;

    /* get original data */
    SHR_IF_ERR_EXIT
        (bcmpc_phy_reg_read(unit, pport, reg_addr, &reg_data, NULL));

    /* set field data */
    PHYMOD_SYMBOL_FIELDS_ITER_BEGIN
        (symbol.fields, finfo, symbols_table->field_names)
    {
        if (sal_strcasecmp(finfo.name, field_name)) {
            continue;
        }
        if (finfo.minbit >= 32 || finfo.maxbit >= 32) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }
        phymod_field_set(&reg_data, finfo.minbit, finfo.maxbit, &val);
    } PHYMOD_SYMBOL_FIELDS_ITER_END();

    /* write data back */
    SHR_IF_ERR_EXIT
        (bcmpc_phy_reg_write(unit, pport, reg_addr, reg_data, NULL));

exit:
    SHR_FUNC_EXIT();
}
