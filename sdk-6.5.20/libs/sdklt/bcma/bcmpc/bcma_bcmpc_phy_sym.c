/*! \file bcma_bcmpc_phy_sym.c
 *
 * Internal PHY registers access functions through PHY symbols.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_error.h>

#include <bcmpc/bcmpc_util.h>
#include <bcmpc/bcmpc_phy.h>

#include <phymod/phymod.h>
#include <phymod/phymod_reg.h>
#include <phymod/phymod_diag.h>

#include <bcma/cli/bcma_cli_token.h>

#include <bcma/bcmpc/bcma_bcmpc.h>

#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

typedef struct sym_info_s {
    int unit;
    uint32_t flags;
    int port;
    phymod_symbols_t *symbols;
    bcma_bcmpc_parse_desc_t *desc;
    int ctoks_num;
    bcma_cli_tokens_t *ctoks;
} sym_info_t;

static int
print_str(const char *str)
{
    return cli_out("%s", str);
}

static void
print_sym_addr(const phymod_symbol_t *symbol)
{
    uint32_t reg_addr;
    uint32_t serdes_blk;
    uint32_t data;

    reg_addr = symbol->addr & 0x00ffffff;
    serdes_blk = reg_addr >> 8;
    if (serdes_blk && (serdes_blk & 0xf) == 0) {
        serdes_blk >>= 4;
    }
    switch (PHYMOD_REG_ACCESS_METHOD(symbol->addr)) {
    case PHYMOD_REG_ACC_BRCM_SHADOW:
        if ((reg_addr & 0x1f) == 0x15) {
            cli_out("Address:  0x%"PRIx32" (expansion)\n", reg_addr >> 8);
            break;
        }
        cli_out("Address:  0x%"PRIx32"\n", reg_addr & 0x1f);
        cli_out("Shadow:   0x%"PRIx32"\n", reg_addr >> 8);
        break;
    case PHYMOD_REG_ACC_BRCM_1000X:
        cli_out("Address:  0x%"PRIx32" (fiber)\n", reg_addr & 0x1f);
        break;
    case PHYMOD_REG_ACC_XAUI_IBLK:
        if (symbol->addr & PHYMOD_REG_ACC_XAUI_IBLK_CL22) {
            cli_out("Block:    0x%"PRIx32"\n", serdes_blk);
            cli_out("Address:  0x%"PRIx32" (clause 22)\n", reg_addr & 0x1f);
            break;
        }
        /* fall through */
    case PHYMOD_REG_ACC_XGS_IBLK:
        cli_out("Block:    0x%"PRIx32"\n", serdes_blk);
        cli_out("Address:  0x%"PRIx32"\n", reg_addr & 0x1f);
        break;
    case PHYMOD_REG_ACC_AER_IBLK:
    case PHYMOD_REG_ACC_TSC_IBLK:
        cli_out("Address:  0x%"PRIx32"", reg_addr & 0xfffff);
        data = (reg_addr >> 20) & 0xf;
        if (data == 1) {
            cli_out(" (1 copy only)");
        } else if (data == 2) {
            cli_out(" (2 copies only)");
        }
        cli_out("\n");
        break;
    case PHYMOD_REG_ACC_RAW:
    default:
        cli_out("Address:  0x%"PRIx32"\n", symbol->addr);
        break;
    }
}

static int
print_sym(const phymod_symbol_t *symbol, void *vptr)
{
    sym_info_t *si = (sym_info_t *)vptr;
    int unit = si->unit;
    int port = si->port;
    uint32_t flags = si->flags;
    int ctoks_num = si->ctoks_num;
    bcma_cli_tokens_t *ctoks = si->ctoks;
    phymod_symbols_t *symbols = si->symbols;
    bcma_bcmpc_parse_desc_t *desc = si->desc;
    const char **fnames = symbols->field_names;
    int rv;
    uint32_t reg_addr;
    uint32_t and_mask, or_mask, data;
    bcmpc_phy_acc_ovrr_t acc_ovrr, *ovrr = &acc_ovrr;

    if (flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST) {
        cli_out("Name:     %s\n", symbol->name);
        print_sym_addr(symbol);
        if (flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_RAW) {
            cli_out("\n");
            return 0;
        }
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
        if (symbol->fields) {
            cli_out("Fields:   %"PRIu32"\n",
                    phymod_field_info_count(symbol->fields));
            phymod_symbol_show_fields(symbol, fnames, NULL, 0,
                                      print_str, NULL, NULL);
            cli_out("\n");
        }
#endif
        return 0;
    }

    reg_addr = symbol->addr;
    bcma_bcmpc_desc2ovrr(desc, ovrr);

    if (ctoks != NULL) {
        /* These csts contain the data and/or field assignments */
        bcma_bcmpc_encode_fields_from_tokens(symbol, fnames, ctoks_num, ctoks,
                                             1, &and_mask, &or_mask);

        /* Read, update and write PHY register */
        rv = bcmpc_phy_reg_read(unit, port, reg_addr, &data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to read %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
            return -1;
        }
        data &= and_mask;
        data |= or_mask;
        rv = bcmpc_phy_reg_write(unit, port, reg_addr, data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to write %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
            return -1;
        }
    } else {
        /* Decode PHY register */
        rv = bcmpc_phy_reg_read(unit, port, reg_addr, &data, ovrr);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFail to read %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, symbol->name);
            return -1;
        }
        cli_out("%s [0x%08"PRIx32"] = 0x%04"PRIx32"\n",
                symbol->name, reg_addr, data);
        if (flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_RAW) {
            return 0;
        }
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
        if (symbol->fields) {
            /* coverity[callee_ptr_arith] */
            phymod_symbol_show_fields(symbol, symbols->field_names,
                                      &data, 0, print_str, NULL, NULL);
        }
#endif
    }

    return 0;
}

static int
phy_sym_op(int unit, int port, phymod_symbols_t *symbols, const char *name,
           uint32_t flags, bcma_bcmpc_parse_desc_t *desc,
           bcma_cli_tokens_t *ctoks, int ctoks_num)
{
    phymod_symbols_iter_t phy_iter, *iter = &phy_iter;
    sym_info_t sym_info, *si = &sym_info;

    sal_memset(iter, 0, sizeof(*iter));
    sal_memset(si, 0, sizeof(*si));

    si->unit = unit;
    si->flags = flags;
    si->port = port;
    si->symbols = symbols;
    si->desc = desc;
    si->ctoks_num = ctoks_num;
    si->ctoks = ctoks;

    iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_EXACT;
    if (sal_strcmp(name, "*") != 0) {
        switch (name[0]) {
        case '^':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_START;
            name++;
            break;
        case '*':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_STRSTR;
            name++;
            break;
        case '@':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_EXACT;
            name++;
            break;
        default:
            if (flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST) {
                iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_STRSTR;
            }
            break;
        }
    }

    /* Interate over all matching symbols */
    iter->name = name;
    iter->symbols = symbols;
    iter->function = print_sym;
    iter->vptr = si;

    if (phymod_symbols_iter(iter) <= 0) {
        cli_out("no matching symbols\n");
        return -1;
    }

    return 0;
}

static phymod_symbols_t *
phy_sym_get(int unit, int port)
{
    int rv;
    bcmpc_phy_access_data_t acc_data;
    phymod_phy_access_t phy_access, *pa = &phy_access;
    phymod_symbols_t *symbols = NULL;

    BCMPC_PHY_ACCESS_DATA_SET(&acc_data, unit, port);
    rv = bcmpc_phymod_phy_access_t_init(unit, port, 0, &acc_data, pa);
    if (rv < 0) {
        cli_out("%sInvalid port number\n", BCMA_CLI_CONFIG_ERROR_STR);
        return NULL;
    }

    if (phymod_diag_symbols_table_get(pa, &symbols) < 0) {
        cli_out("%sNo symbol table\n", BCMA_CLI_CONFIG_ERROR_STR);
        return NULL;
    }
    return symbols;
}

#endif /* PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS */

int
bcma_bcmpc_phy_sym(int unit, int port, bcma_bcmpc_parse_desc_t *desc,
                   uint32_t flags, bcma_cli_tokens_t *ctoks, int ctoks_num)
{
#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return SHR_E_UNAVAIL;
#else
    int rv;
    phymod_symbols_t *symbols;

    symbols = phy_sym_get(unit, port);
    if (symbols == NULL) {
        return SHR_E_NOT_FOUND;
    }

    if ((flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST) != 0) {
        return SHR_E_FAIL;
    }

    rv = phy_sym_op(unit, port, symbols, desc->name, flags, desc,
                    ctoks, ctoks_num);

    return (rv < 0) ? SHR_E_FAIL : SHR_E_NONE;
#endif /* PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}

int
bcma_bcmpc_phy_sym_list(int unit, int port, const char *name, uint32_t flags)
{
#if PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS == 0
    return SHR_E_UNAVAIL;
#else
    phymod_symbols_t *symbols;

    symbols = phy_sym_get(unit, port);
    if (symbols == NULL) {
        return SHR_E_NOT_FOUND;
    }

    if ((flags & BCMA_BCMPC_PHY_CMD_FLAG_SYM_LIST) == 0) {
        return SHR_E_FAIL;
    }

    if (name == NULL) {
        bcma_bcmpc_parse_error("symbol", NULL);
        return SHR_E_FAIL;
    }

    if (phy_sym_op(unit, port, symbols, name, flags, NULL, NULL, 0) < 0) {
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
#endif /* PHYMOD_CONFIG_INCLUDE_CHIP_SYMBOLS */
}
