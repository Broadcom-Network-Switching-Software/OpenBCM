/*
 *  
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains a set of functions which can be used to parse an
 * SDK CLI command into a symbolic PHY operation.
 *
 * For example usage, please refer to e.g. $SDK/src/appl/diag/port.c.
 */

#include <appl/diag/parse.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/phymod/phymod_symop.h>

#include <phymod/phymod_reg.h>
#include <shared/bsl.h>

#include <soc/phy/phyctrl.h>
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod.h>
#endif

#if defined(PHYMOD_SUPPORT)

#define PHY_FIELD_NAME_MAX      80
#define PHY_FIELDS_MAX          32

typedef struct phy_field_s {
    char name[PHY_FIELD_NAME_MAX + 1];
        uint32 val;
} phy_field_t;

typedef struct phy_field_info_s {
    uint32 regval; /* raw value */
    int num_fields;
    phy_field_t field[PHY_FIELDS_MAX];
} phy_field_info_t;

typedef struct phy_iter_s {
    const char *name;
    const char *found_name;
#define PHY_SHELL_SYM_RAW       0x1     /* Do not decode fields */
#define PHY_SHELL_SYM_LIST      0x2     /* List symbol only */
#define PHY_SHELL_SYM_NZ        0x4     /* Skip if reg/field is zero  */
#define PHY_SHELL_SYM_RESET     0x8     /* Reset register */
    uint32 flags;
    uint32 addr;
    int lane;
    int pll_idx;
    int count;
    const phymod_symbols_t *symbols;
    phymod_phy_access_t *phy_access;
    phy_field_info_t *finfo;
    char tmpstr[64];
} phy_iter_t; 

STATIC int
_phymod_encode_field(const phymod_symbol_t *symbol, 
                     const char **fnames, 
                     const char *field, uint32 value, 
                     uint32 *and_masks, uint32 *or_masks)
{
#if PHYMOD_CONFIG_INCLUDE_FIELD_NAMES == 1
    phymod_field_info_t finfo; 
    uint32 zero_val; 

    PHYMOD_SYMBOL_FIELDS_ITER_BEGIN(symbol->fields, finfo, fnames) {

        if (sal_strcasecmp(finfo.name, field)) {
            continue; 
        }

        zero_val = 0;
        phymod_field_set(and_masks, finfo.minbit, finfo.maxbit, &zero_val);
        /* coverity[callee_ptr_arith] */
        phymod_field_set(or_masks, finfo.minbit, finfo.maxbit, &value);

        return 0;
    } PHYMOD_SYMBOL_FIELDS_ITER_END(); 

#endif /* PHYMOD_CONFIG_INCLUDE_FIELD_NAMES */

    return -1; 
}

#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
STATIC int
_phymod_print_str(const char *str)
{
    cli_out("%s", str); 
    return 0;
}
#endif

STATIC int
_phymod_sym_list(const phymod_symbol_t *symbol, uint32 flags, const char **fnames)
{
    uint32 reg_addr;
    uint32 num_copies;

    cli_out("Name:     %s", symbol->name); 
    if (symbol->alias != NULL) {
        cli_out(" (%s)", symbol->alias); 
    }
    cli_out("\n"); 
    reg_addr = symbol->addr & 0x00ffffff;
    switch (PHYMOD_REG_ACCESS_METHOD(symbol->addr)) {
    case PHYMOD_REG_ACC_AER_IBLK:
    case PHYMOD_REG_ACC_TSC_IBLK:
        cli_out("Address:  0x%"PRIx32"", reg_addr & 0xfffff); 
        num_copies = (reg_addr >> 20) & 0xf;
        if (num_copies == 1) {
            cli_out(" (1 copy only)"); 
        } else if (num_copies == 2) {
            cli_out(" (2 copies only)"); 
        }
        cli_out("\n"); 
        break;
    case PHYMOD_REG_ACC_RAW:
    default:
        cli_out("Address:  0x%"PRIx32"\n", symbol->addr); 
        break;
    }
    if (flags & PHY_SHELL_SYM_RAW) {
        cli_out("\n"); 
        return 0;
    }
#if PHYMOD_CONFIG_INCLUDE_RESET_VALUES == 1
    cli_out("Reset:    0x%"PRIx32" (%"PRIu32")\n",
            symbol->resetval, symbol->resetval);
#endif
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
    if (symbol->fields) {
        cli_out("Fields:   %"PRIu32"\n",
                phymod_field_info_count(symbol->fields));
        phymod_symbol_show_fields(symbol, fnames, NULL, 0,
                                  _phymod_print_str,
                                  NULL, NULL); 
    }
#endif
    return 0;
}

STATIC int
_phymod_sym_iter_count(const phymod_symbol_t *symbol, void *vptr)
{
    phy_iter_t *phy_iter = (phy_iter_t *)vptr;

    return ++(phy_iter->count);
}

STATIC int
_phymod_sym_find_hex(const phymod_symbol_t *symbol, void *vptr)
{
    phy_iter_t *phy_iter = (phy_iter_t *)vptr;

    if (phy_iter->symbols == NULL) {
        return -1;
    }

    if ((symbol->addr & 0xffff) == phy_iter->addr) {
        phy_iter->found_name = symbol->name;
    }
    return 0;
}

STATIC int
_phymod_sym_iter_op(const phymod_symbol_t *symbol, void *vptr)
{
    int rv;
    uint32 data;
    uint32 and_mask;
    uint32 or_mask;
    uint32 lane_mask;
    phymod_phy_access_t lpa, *pa;
    phy_iter_t *phy_iter = (phy_iter_t *)vptr;
    phy_field_t *fld;
    uint32 reg_addr;
    uint32 fdx;
    char lane_str[16];
    char pIdx_str[16];
    const char **fnames;

    if (phy_iter->symbols == NULL) {
        return -1;
    }
    fnames = phy_iter->symbols->field_names; 

    if (phy_iter->flags & PHY_SHELL_SYM_LIST) {
        _phymod_sym_list(symbol, phy_iter->flags, fnames);
        return 0;
    }

    pa = phy_iter->phy_access;
    /*There are only two PLL at most in a SerDes core.
      0 : PLL0
      1 : PLL1
    */
    if ((phy_iter->pll_idx >= 0) && (phy_iter->pll_idx <= 1)) {
        pa->access.pll_idx = phy_iter->pll_idx;
    }
    if (phy_iter->lane >= 0) {
        lane_mask = 1 << phy_iter->lane;
        if (lane_mask != pa->access.lane_mask) {
            sal_memcpy(&lpa, pa, sizeof(lpa));

            while ((lane_mask && (lane_mask & 0xFF)) == 0) {
                   lane_mask >>= 8;
                   lpa.access.addr += 8;
            }
            lpa.access.lane_mask = lane_mask;
            pa = &lpa;
        }
    }
    reg_addr = symbol->addr;

    if (phy_iter->flags & PHY_SHELL_SYM_RESET) {
#if PHYMOD_CONFIG_INCLUDE_RESET_VALUES == 1
        if (phymod_phy_reg_write(pa, reg_addr, symbol->resetval) != 0) {
            cli_out("Error resetting %s\n", symbol->name);
            return -1;
        }
#endif
        return 0;
    }

    if (phy_iter->finfo) {
        if (phy_iter->finfo->num_fields) {
            and_mask = ~0;
            or_mask = 0;
            for (fdx = 0; fdx < phy_iter->finfo->num_fields; fdx++) {
                fld = &phy_iter->finfo->field[fdx];
                /* coverity[callee_ptr_arith] */
                rv = _phymod_encode_field(symbol, fnames,
                                          fld->name, fld->val,
                                          &and_mask, &or_mask);
                if (rv < 0) {
                    cli_out("Invalid field: %s\n", fld->name);
                    return -1;
                }
            }
        } else {
            and_mask = 0;
            or_mask = phy_iter->finfo->regval;
        }

        /* Read, update and write PHY register */
        if (phymod_phy_reg_read(pa, reg_addr, &data) != 0) {
            cli_out("Error reading %s\n", symbol->name);
            return -1;
        }
        data &= and_mask;
        data |= or_mask;
        if (phymod_phy_reg_write(pa, reg_addr, data) != 0) {
            cli_out("Error writing %s\n", symbol->name);
            return -1;
        }
    }
    else {
        /* Decode PHY register */
        if (phymod_phy_reg_read(pa, reg_addr, &data) != 0) {
            cli_out("Error reading %s\n", symbol->name);
            return -1;
        }
        if (data == 0 && (phy_iter->flags & PHY_SHELL_SYM_NZ)) {
            return 0;
        }
        lane_str[0] = 0;
        pIdx_str[0] = 0;
        if (phy_iter->lane >= 0) {
            sal_sprintf(lane_str, ".%d", phy_iter->lane);
            if (phy_iter->pll_idx >= 0) {
                sal_sprintf(pIdx_str, ".%d", phy_iter->pll_idx);
            }
        }
        cli_out("%s%s%s [0x%08"PRIx32"] = 0x%04"PRIx32"\n", 
                symbol->name, lane_str, pIdx_str, reg_addr, data);
#if PHYMOD_CONFIG_INCLUDE_FIELD_INFO == 1
        if (phy_iter->flags & PHY_SHELL_SYM_RAW) {
            return 0;
        }
        if (symbol->fields) {
            /* coverity[callee_ptr_arith] */
            phymod_symbol_show_fields(symbol, fnames, &data,
                                      (phy_iter->flags & PHY_SHELL_SYM_NZ),
                                      _phymod_print_str,
                                      NULL, NULL); 
        }
#endif
    }
    return 0; 
}


#define MAX_PHYS_PER_PORT       6

/* Get PHYMod symbol table and access object from unit/port */
int
phymod_sym_info(int unit, int port, int intermediate, phymod_symbols_iter_t *iter,
                phymod_phy_access_t *pm_acc)
{
    phy_ctrl_t *pc;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int pm_type;
    int pport;
#endif

    if (pm_acc == NULL) {
        return -1;
    }
#if defined(PORTMOD_SUPPORT)
    if (SOC_USE_PORTCTRL(unit)) {
        int rv, num_phy;
        phymod_phy_access_t phy_acc[MAX_PHYS_PER_PORT];
        portmod_access_get_params_t params;
        int core_index;
        phy_iter_t *phy_iter =(phy_iter_t*) iter->vptr;

        SOC_IF_ERROR_RETURN
            (portmod_access_get_params_t_init(unit, &params));

        /* By default we get the access object(s) of outermost PHY */
        if (intermediate) {
            /* Request access object(s) of innermost PHY */
            params.phyn = 0;
        }

        rv = portmod_port_phy_lane_access_get(unit, port, &params,
                                              COUNTOF(phy_acc),
                                              phy_acc, &num_phy, NULL);

#ifdef BCM_TOMAHAWK3_SUPPORT
        pport = SOC_INFO(unit).port_l2p_mapping[port];
        rv = SOC_PORTCTRL_FUNCTIONS(unit)->soc_portctrl_pm_type_get(unit,
                                                    pport, &pm_type);

        if (SOC_FAILURE(rv)) {
            return rv;
        }
        if (pm_type == portmodDispatchTypePm8x50) {
            core_index = phy_iter->lane / 8;
            phy_iter->lane %= 8;
        } else
#endif
        {
            core_index = phy_iter->lane/4;
            phy_iter->lane %=4;
        }
        if (rv < 0 || core_index >= num_phy) {
            return -1;
        }
        /* Only a single core per port is currently supported */
        sal_memcpy(pm_acc, &phy_acc[core_index], sizeof(*pm_acc));
        return 0;
    }
#endif
    if (phy_port_info[unit] == NULL) {
        return -1;
    }
    /* Set up per-port info */
    pc = EXT_PHY_SW_STATE(unit, port);
    if (intermediate || pc == NULL) {
        pc = INT_PHY_SW_STATE(unit, port);
    }
    if (pc == NULL) {
        return -1;
    }
    if (pc->phymod_ctrl.phy[0] == NULL) {
        return -1;
    }
    sal_memcpy(pm_acc, &pc->phymod_ctrl.phy[0]->pm_phy, sizeof(*pm_acc));

    return 0;
}

/*
 * Function:
 *    phymod_symop_init
 * Purpose:
 *    Parse command line for symbolic PHY operation
 * Parameters:
 *    iter    - symbol table iterator structure
 *    a       - CLI argument list
 * Returns:
 *    CMD_OK if no error
 */
int
phymod_symop_init(phymod_symbols_iter_t *iter, args_t *a)
{
    phy_iter_t *phy_iter;
    phy_field_info_t *finfo = NULL;
    char *c;
    const char *name;
    uint32 flags;
    int fdx, lane, pll_idx;

    if (iter == NULL) {
        return CMD_FAIL;
    }

    phy_iter = sal_alloc(sizeof(phy_iter_t), "phy_iter");
    if (phy_iter == NULL) {
        cli_out("%s: Unable to allocate PHY iterator\n",
                ARG_CMD(a));
        return CMD_FAIL;
    }

    sal_memset(iter, 0, sizeof(*iter)); 
    sal_memset(phy_iter, 0, sizeof(*phy_iter)); 
    iter->vptr = phy_iter; 

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    name = c;

    /* Check for output flags */
    flags = 0;
    do {
        if (sal_strcmp(c, "list") == 0) {
            flags |= PHY_SHELL_SYM_LIST;
        } else if (sal_strcmp(c, "raw") == 0) {
            flags |= PHY_SHELL_SYM_RAW;
        } else if (sal_strcmp(c, "nz") == 0) {
            flags |= PHY_SHELL_SYM_NZ;
        } else if (sal_strcmp(c, "reset") == 0) {
            flags |= PHY_SHELL_SYM_RESET;
        } else {
            name = c;
            break;
        }
    } while ((c = ARG_GET(a)) != NULL);

    /* Check for lane-specification, i.e. <regname>.<lane> */
    lane = -1;
    pll_idx = -1;
    /* coverity[returned_pointer] */
    c = sal_strchr(name, '.');    
    if (c != NULL) {
        sal_strncpy(phy_iter->tmpstr, name, sizeof(phy_iter->tmpstr));
        phy_iter->tmpstr[sizeof(phy_iter->tmpstr)-1] = 0;
        name = phy_iter->tmpstr;
        c = sal_strchr(name, '.');    
        if (c != NULL) {
            *c++ = 0;
            lane = sal_ctoi(c, NULL);
            c = sal_strchr(c, '.');
            if (c != NULL) {
                *c++ = 0;
                pll_idx = sal_ctoi(c, NULL);
            }
        }
    }

    if ((c = ARG_GET(a)) != NULL) {
        finfo = sal_alloc(sizeof(phy_field_info_t), "field_info");
        if (finfo == NULL) {
            cli_out("%s: Unable to allocate field info\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        }
        sal_memset(finfo, 0, sizeof (*finfo));
        if (isint(c)) {
            /* Raw register value */
            finfo->regval = parse_integer(c);
            /* No further parameters expected */
            if (ARG_CNT(a) > 0) {
                cli_out("%s: Unexpected argument %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                sal_free(finfo);
                return CMD_USAGE;
            }
        } else {
            /* Individual fields */
            fdx = 0;
            do {
                if (finfo->num_fields >= PHY_FIELDS_MAX) {
                    cli_out("%s: Too many fields\n",
                            ARG_CMD(a));
                    sal_free(finfo);
                    return CMD_FAIL;
                }
                sal_strncpy(finfo->field[fdx].name, c,
                            PHY_FIELD_NAME_MAX);
                c = sal_strchr(finfo->field[fdx].name, '=');
                if (c == NULL) {
                    cli_out("%s: Invalid field assignment: %s\n",
                            ARG_CMD(a), finfo->field[fdx].name);
                    sal_free(finfo);
                    return CMD_FAIL;
                }
                *c++ = 0;
                finfo->field[fdx].val = parse_integer(c);
                fdx++;
            } while ((c = ARG_GET(a)) != NULL);
            finfo->num_fields = fdx;
        }
    }

    iter->name = name;
    iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_EXACT; 
    if (sal_strcmp(iter->name, "*") != 0) {
        switch (iter->name[0]) {
        case '^':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_START; 
            iter->name++;
            break;
        case '*':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_STRSTR; 
            iter->name++;
            break;
        case '@':
            iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_EXACT; 
            iter->name++;
            break;
        default:
            if (flags & PHY_SHELL_SYM_LIST) {
                iter->matching_mode = PHYMOD_SYMBOLS_ITER_MODE_STRSTR;
            }
            break;
        }
    }
    phy_iter->lane = lane;
    phy_iter->pll_idx = pll_idx;
    phy_iter->flags = flags; 
    phy_iter->finfo = finfo; 

    /* Save for symbol lookup based on register address */
    phy_iter->name = iter->name;
    phy_iter->addr = sal_ctoi(phy_iter->name, NULL); 

    return CMD_OK;
}

/*
 * Function:
 *    phymod_symop_exec
 * Purpose:
 *    Execute operation on one or more symbols
 * Parameters:
 *    iter    - symbol table iterator structure
 *    symbols - symbol table
 *    pm_acc  - PHY access structure
 *    hdr     - header string show for register read operations
 * Returns:
 *    CMD_OK if no error
 */
int
phymod_symop_exec(phymod_symbols_iter_t *iter, const phymod_symbols_t *symbols,
                  phymod_phy_access_t *pm_acc, char *hdr)
{
    phy_iter_t *phy_iter;

    if (iter == NULL || iter->vptr == NULL) {
        return CMD_FAIL;
    }
    phy_iter = (phy_iter_t *)iter->vptr;

    iter->symbols = symbols; 
    phy_iter->symbols = iter->symbols;
    phy_iter->phy_access = pm_acc;

    if (phy_iter->flags & PHY_SHELL_SYM_LIST) {
        /*
         * For symbol listings we force raw mode if multiple
         * matches are found.
         */
        phy_iter->count = 0;
        iter->function = _phymod_sym_iter_count; 
        if (phymod_symbols_iter(iter) > 1) {
            phy_iter->flags |= PHY_SHELL_SYM_RAW;
        }
    }

    iter->function = _phymod_sym_iter_op; 

    if (phy_iter->finfo == NULL && (phy_iter->flags & PHY_SHELL_SYM_RESET) == 0) {
        /* Show header only if not writing */
        cli_out("%s", hdr ? hdr : "");
    }
    /* If numerical value specified, then look up symbol name */
    if (phy_iter->name[0] >= '0' && phy_iter->name[0] <= '9') {
        iter->function = _phymod_sym_find_hex;
        iter->name = "*";
        phy_iter->found_name = NULL;
        /* Search for register address */
        if (phymod_symbols_iter(iter) == 0 || phy_iter->found_name == NULL) {
            cli_out("No matching address\n"); 
            return CMD_OK;
        }
        iter->name = phy_iter->found_name;
        /* Restore iterator function */
        iter->function = _phymod_sym_iter_op;
    }
    /* Iterate over all symbols in symbol table */
    if (phymod_symbols_iter(iter) == 0) {
        cli_out("No matching symbols\n"); 
    }
    return CMD_OK;
}

/*
 * Function:
 *    phymod_symop_cleanup
 * Purpose:
 *    Free resources allocated by phymod_symop_init
 * Parameters:
 *    iter    - symbol table iterator structure
 * Returns:
 *    CMD_OK if no error
 */
int
phymod_symop_cleanup(phymod_symbols_iter_t *iter)
{
    phy_iter_t *phy_iter;

    if (iter == NULL || iter->vptr == NULL) {
        return CMD_FAIL;
    }
    phy_iter = (phy_iter_t *)iter->vptr;

    if (phy_iter != NULL) {
        if (phy_iter->finfo != NULL) {
            sal_free(phy_iter->finfo);
        }
        sal_free(phy_iter);
    }
    sal_memset(iter, 0, sizeof(*iter)); 

    return CMD_OK;
}

#else
typedef int phymod_sym_access_not_empty; /* Make ISO compilers happy. */
#endif /* defined(PHYMOD_SUPPORT) */
