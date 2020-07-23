/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Register mnemonic address parsing routines.
 * See description for parse_symbolic_reference.
 * This module is used by command line shells.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/register.h>
#include <soc/util.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#endif
#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif

#ifdef SOC_COUNTER_TABLE_SUPPORT
#include <soc/mcm/driver.h>
#endif
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/port_sw_db.h>
#endif

/*
 * Symbol hash table entry with buckets represented by linked list.
 */

typedef struct symtab_entry_t {
    soc_reg_t             esw_reg;
    char                  *name;
    struct symtab_entry_t *next;
} symtab_entry_t;

static symtab_entry_t     *symtab_table;

/*
 * Statically allocated table with pointers
 * to globally allocated registers table.
 */

static symtab_entry_t     *hashtable[NUM_SOC_REG];

/*
 * Case-independent hash routine, where we store a symbol table
 * entry (front end) in the hash table. If we
 * hit on that entry, then we chain it at the
 * same base memory address.
 */

static int
hash_func(char *s)
{
    uint32 h = 0, c;

    while ((c = *s++) != 0)
    h = (h << 3) ^ h ^ (h >> 7) ^ (sal_islower(c) ? sal_toupper(c) : (c));
    return (h % NUM_SOC_REG);
}

/*
 * Initialize register symbol table. Hash all of the values in
 * the registers file into an address table we can lookup by
 * string mnemonic.
 */

void
_add_symbol(char *name, soc_reg_t reg, int ent, int *chained)
{
    int key;
    symtab_entry_t *table_entry;

    key = hash_func(name);

    /* Chain entry */
    table_entry = &symtab_table[ent];
    table_entry->name = name;
    table_entry->esw_reg = reg;
    
    table_entry->next = hashtable[key];
    if (hashtable[key]) {
        (*chained)++;
    }
    hashtable[key] = table_entry;
}


/*
 * Lookup a register in the symbol table.
 * Returns register number from matching entry
 * There may be both a register name and an alias that are
 * identical (AGE_TIMER, for example).  If one is a valid
 * register for this unit and another is invalid, the
 * valid register is returned.
 *
 * NOTE: hash_func is case-independent, so lookup_register is also.
 */

static symtab_entry_t*
lookup_register(int unit, char *regname)
{
    symtab_entry_t *loc, *floc;

    floc = NULL;
    for (loc = hashtable[hash_func(regname)]; loc; loc = loc->next) {
        if(!sal_strcasecmp(loc->name, regname)) {
            if (floc == NULL) {        /* save first match */
                floc = loc;
            }
#if defined(BCM_ESW_SUPPORT)
            if (SOC_REG_IS_VALID(unit, loc->esw_reg)) {
                return loc;
            }
#ifdef SOC_COUNTER_TABLE_SUPPORT
            if (SOC_REG_IS_COUNTER_TABLE(unit, loc->esw_reg)) {
                return loc;
            }
#endif /* SOC_COUNTER_TABLE_SUPPORT */
#endif
        }
    }

    return (floc != NULL) ? floc : NULL;
}


void
init_symtab()
{
    soc_reg_t      reg;
    int            c, d, lin, emp, lon, len, ent;
    symtab_entry_t *te;
#ifdef SOC_COUNTER_TABLE_SUPPORT
    
    soc_driver_t *drv = &soc_driver_bcm56980_a0;
#endif

    if (symtab_table != NULL) {
        return;
    }

    /* count how many table entries to allocate for ESW */
    ent = 0;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        ent++;
#if !defined(SOC_NO_ALIAS)    
        if (soc_reg_alias[reg] && *soc_reg_alias[reg]) {
            ent++;
        }
#endif /* !defined(SOC_NO_ALIAS) */
    }
#endif
#ifdef SOC_COUNTER_TABLE_SUPPORT
    ent += drv->ctr_tbl_info->num;
#endif

    symtab_table = sal_alloc(ent * sizeof(symtab_entry_t), "symtab_table");
    if (NULL == symtab_table) {
        return; /* how to signal failure? */
    }

    ent = 0;
    d = 1;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    /* Load the hashtable with each reserved word */
    /* ESW */
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
#if !defined(SOC_NO_NAMES)
        _add_symbol(soc_reg_name[reg], reg, ent, &d);
        ent += 1;
#endif /* !SOC_NO_NAMES */
#if !defined(SOC_NO_ALIAS)    
        if (soc_reg_alias[reg] && *soc_reg_alias[reg]) {
            _add_symbol(soc_reg_alias[reg], reg, ent, &d);
            ent += 1;
        }
#endif /* !defined(SOC_NO_ALIAS) */
    }

#ifdef SOC_COUNTER_TABLE_SUPPORT
#if !defined(SOC_NO_NAMES)
    {
        soc_field_t fld;

        for (reg = 0; reg < drv->ctr_tbl_info->num; reg++) {
            fld = drv->ctr_tbl_info->tables[reg].ctr_field;
            _add_symbol(soc_fieldnames[fld],
                (fld + SOC_COUNTER_TABLE_FIELD_START), ent, &d);
            ent += 1;
        }
    }
#endif /* !SOC_NO_NAMES */
#endif /* SOC_COUNTER_TABLE_SUPPORT */

#endif    

    c = ent + 1;
    lin = d-c > c-d ? d-c : c-d;
    emp = lon = 0;

    /* count number of empties and longest chain */
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        te = hashtable[reg];
        if (te == NULL) {
            emp += 1;
            continue;
        }
        len = 1;
        while (te->next != NULL) {
            len += 1;
            te = te->next;
        }
        if (len > lon) {
            lon = len;
        }
    }

    /*
     * this actually happens too early to print out
     * a printf here shows:
     * symtab: init 1028 regs, 1119 symbols, 674 linear, 445 chained
     *         345 empty, 5 longest chain
     */
    LOG_INFO(BSL_LS_APPL_SYMTAB,
             (BSL_META("symtab: init %d regs, %d symbols, %d linear, %d chained,\n"
                       "\t\t%d empty, %d longest chain\n"),
              NUM_SOC_REG, c, lin, d, emp, lon));

}

#ifdef SOC_COUNTER_TABLE_SUPPORT
static void
_handle_counter_table_refs(int unit, soc_regaddrlist_t *alist, soc_reg_t reg, soc_port_t port) {

    soc_regaddrinfo_t *a;

    if(!SOC_REG_IS_COUNTER_TABLE(unit, reg)) {
        return;
    }

    a = &alist->ainfo[alist->count++];
    a->valid = 1;
    a->idx = 0;
    a->reg = reg;
    a->port = port;

}
#endif /* SOC_COUNTER_TABLE_SUPPORT */

static void
_handle_array_refs(int unit, soc_regaddrlist_t *alist,
                   soc_reg_t reg, int block,
                   soc_cos_t cos, soc_port_t port,
                   int minidx, int maxidx)
{
#if defined(BCM_PETRA_SUPPORT)
    int rc = 0;
    uint32 offset = 0;
#endif
    int idx;
    soc_regaddrinfo_t *a;
    uint32 flags = SOC_REG_INFO(unit, reg).flags;

    bsl_log_add(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore, unit,
                "symtab: lookup reg %d blk %d cos %d port %d index %d:%d\n",
                reg, block, cos, port, minidx, maxidx);

    /* Ignore invalid ports, don't check if the port is really a instance id */
    if ((!(port & SOC_REG_ADDR_INSTANCE_MASK)) && port != REG_PORT_ANY && !SOC_PORT_VALID(unit, port) &&
        (SOC_REG_INFO(unit, reg).regtype != soc_ppportreg)) {
        return;
    }

    for (idx = minidx; idx <= maxidx; idx++) {
        if (flags & SOC_REG_FLAG_NO_DGNL) {
            /* Skip diagonals.  */
            if (idx == port) {
                continue;
            }
        }
        a = &alist->ainfo[alist->count++];
        a->valid = 1;
        a->idx = idx;
        a->reg = reg;
        a->block = block;
        a->port = port;
        a->field = INVALIDf;
        a->cos = cos;
        a->is_custom = 0;
        /* calculate address based on reg type */
        switch (SOC_REG_INFO(unit, reg).regtype) {
        case soc_customreg:
#if defined(BCM_PETRA_SUPPORT)
            if (SOC_IS_DPP(unit)) {
                rc = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
                if (rc < 0) {
                    return;
                }

                a->block = ((offset < 2) ? ILKN_PMH_BLOCK(unit) : (offset < 4 ? ILKN_PML_BLOCK(unit, 0) : ILKN_PML_BLOCK(unit, 1)));
            }
#endif
            a->is_custom = 1;
#if defined(BCM_DNX_SUPPORT)
            /** get register address */
            if (SOC_IS_DNX(unit)) {
                int blk;
                uint8 at;
                a->addr = soc_reg_addr_get(unit, reg, port, idx,
                                           SOC_REG_ADDR_OPTION_NONE,
                                           &blk, &at);
            }
#endif
            break;
        case soc_cosreg:
            idx = cos;
            /* FALLTHROUGH */
        case soc_cpureg:
        case soc_mcsreg:
        case soc_iprocreg:
        case soc_portreg:
        case soc_ppportreg:
        case soc_genreg:
        case soc_phy_reg:
        case soc_pipereg:
        case soc_xpereg:
        case soc_itmreg:
        case soc_ebreg:
        case soc_slicereg:
        case soc_layerreg:
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
                {
                    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
                        a->addr = soc_reg_addr(unit, reg, port, idx);
                    } else {
                        int blk;
                        uint8 at;
                        a->addr = soc_reg_addr_get(unit, reg, port, idx,
                                                   SOC_REG_ADDR_OPTION_NONE,
                                                   &blk, &at);
                    }
                }
#endif
            break;
        default:
            assert(0);
            a->addr = 0;
            break;
        }

        /*
         * Check if this register is actually implemented
         * in HW for the specified port/cos.
         */
        /* coverity[OVERRUN: FALSE] */
        if (reg_mask_subset(unit, a, NULL) &&
            (!SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_CCH))) {
            alist->count--;
        }
    }
}

/* Check string starting at name for [ and ] */
static int
_parse_array(char *name, char **out1, char **out2)
{
    char *idx1, *idx2, *tmpchr;
    int rv = 0;

    if ((idx1 = sal_strchr(name, '[')) != 0) {
        rv = 1;
        *idx1++ = 0;
        if ((tmpchr = sal_strchr(idx1, ']')) == 0) {
            LOG_WARN(BSL_LS_APPL_SYMTAB,
                     (BSL_META("Could not parse index in %s.\n"), name));
        } else {
            *tmpchr = 0;
        }
        if ((idx2 = sal_strchr(idx1, '-')) != 0) {
            *idx2++ = 0;
        }
    } else if ((idx1 = sal_strchr(name, '(')) != 0) {
        rv = 1;
        *idx1++ = 0;
        if ((tmpchr = sal_strchr(idx1, ')')) == 0) {
            LOG_WARN(BSL_LS_APPL_SYMTAB,
                     (BSL_META("Could not parse index in %s.\n"), name));
        } else {
            *tmpchr = 0;
        }
        if ((idx2 = sal_strchr(idx1, '-')) != 0) {
            *idx2++ = 0;
        }
    } else {
        idx2 = 0;
    }

    *out1 = idx1;
    *out2 = idx2;

    return rv;
}

/*
 * Parse a symbolic register reference.
 *
 * Fills in soc_regaddrlist_t, which is an array of soc_regaddrinfo_t.
 * Returns 0 on success, -1 on syntax error.
 *
 * Understands the following:
 *
 * regname            -Port register, all ports
 * regname.PBMP            -Port register, selected ports
 *
 * regname            -Generic register, all PICs or MMU
 * regname.BLK_RANGE        -Generic register, selected PICs
 *
 * regname            -COS register, all COS values, all PICs or MMU
 * regname.COS_RANGE        -COS register, selected COS's, all PICs
 * regname..BLK_RANGE        -COS register, all COS values, selected PICs
 * regname.COS_RANGE.BLK_RANGE    -COS register, selected COS's, selected PICs
 *
 * In addition, [index range] may be specified.  It can be specified for
 * register, but for non-array registers, it must be 0.  All array indices
 * are 0-based.
 *
 * If a leading '$' is present, it is ignored.
 *
 * See util.c:parse_pbmp() for PBMP format
 * See util.c:parse_num_range() for COS_RANGE format
 * See util.c:parse_block_range() for BLK_RANGE format
 *
 * Register addressing
 *    soc_reg_addr(unit, port, index)
 *    - port is REG_PORT_ANY for non-pic regs
 *    - index is the cos number for COS regs
 *
 * Prints a warning and returns -1 if unit does not support the
 * register.
 */

int
parse_symbolic_reference(int unit, soc_regaddrlist_t *alist, char *ref)
{
    char rname[80], *range1, *range2, *idx1, *idx2, *regtype_name;
    soc_port_t port, phy_port, dport;
    int legal_max, legal_min, range;
    soc_cos_t cos_min, cos_max, cos;
    int blk_min, blk_max, blk = 0;
    soc_block_t portblktype;
    soc_block_types_t regblktype, bmask = NULL;
    soc_reg_t reg;
    pbmp_t bm;
    soc_reg_info_t *reginfo;
    int maxidx, minidx, idxfound = 0;
    char pfmt[SOC_PBMP_FMT_LEN];
    symtab_entry_t  *et;
    int idx = 0, match = 0, max_block=0, port_num_blktype;
    soc_regtype_t regtype;
    
    LOG_INFO(BSL_LS_APPL_SYMTAB,
             (BSL_META_U(unit,
                         "parsing: %s\n"), ref));

    if (*ref == '$') {
        ref++;
    }

    sal_strncpy(rname, ref, 79);
    rname[79] = 0;

    if ((range1 = sal_strchr(rname, '.')) != 0) {
        *range1++ = '\0';
        idxfound = _parse_array(range1, &idx1, &idx2);

        if ((range2 = sal_strchr(range1, '.')) != 0) {
            *range2++ = '\0';
            if (*range2 == '\0') {
                range2 = 0;
            }
        }
        if (*range1 == '\0') {
            range1 = 0;
        }
    } else {
        range2 = 0;
    }

    if (!idxfound) {
        idxfound = _parse_array(rname, &idx1, &idx2);
    }

    et = lookup_register(unit, rname);
    if (NULL == et) {
        return -1;
    }

    reg = et->esw_reg;
    

    
    if (!SOC_REG_IS_VALID(unit, reg) && !SOC_REG_IS_COUNTER_TABLE(unit, reg)) {
        LOG_WARN(BSL_LS_APPL_SYMTAB,
                 (BSL_META_U(unit,
                             "Register %s %d is not valid for chips using driver %s\n"),
                             et->name, reg, SOC_UNIT_NAME(unit)));
        return -1;
    }

#ifdef SOC_COUNTER_TABLE_SUPPORT
    if(SOC_REG_IS_COUNTER_TABLE(unit, reg)) {
        if (range2) {
            return -1;
        }

        if (!range1) {
            SOC_PBMP_CLEAR(bm);
            SOC_PBMP_ASSIGN(bm, PBMP_CD_ALL(unit));
        } else if (parse_pbmp(unit, range1, &bm) < 0) {
            return -1;
        }

        SOC_PBMP_ITER(bm, port) {
            _handle_counter_table_refs(unit, alist, reg, port);
        }

        return (alist->count > 0)? 0: -1;
    }
#endif /* SOC_COUNTER_TABLE_SUPPORT */

    reginfo = &SOC_REG_INFO(unit, reg);
    if (!SOC_REG_IS_ENABLED(unit, reg)) {
#if defined(BCM_ESW_SUPPORT)
        LOG_WARN(BSL_LS_APPL_SYMTAB,
                 (BSL_META_U(unit,
                             "Register %s is not enabled for this configuration\n"),
                             SOC_REG_NAME(unit, reg)));
#endif
        return -1;
    }
    regblktype = reginfo->block;

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        /* Since TH3 has the dummy blocks CDMAC defined in the regsfile, we need
           to assign them to the CDPORT block */
        if (regblktype[0] == SOC_BLK_CDMAC) {
            regblktype[0] = SOC_BLK_CDPORT;
        }
    }
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    /* max_block = _soc_max_blocks_per_entry_get(); */
    /* _soc_max_blocks_per_entry_get() + 1 is also enough but for 
       future safe  purpose*/
    max_block = SOC_MAX_NUM_BLKS;
#endif

    bmask = sal_alloc(sizeof(uint32) * max_block, 
                      "parse_bmask");
    if (NULL == bmask) {
        return -1;
    }
    sal_memset(bmask, 0, sizeof(uint32) * max_block);

    if (diag_parse_range(idx1, idx2, &minidx, &maxidx,
                         (SOC_REG_IS_ARRAY(unit, reg) || SOC_REG_ARRAY(unit, reg)) ? SOC_REG_INFO(unit, reg).first_array_index : -1,
                         (SOC_REG_IS_ARRAY(unit, reg) || SOC_REG_ARRAY(unit, reg)) ? SOC_REG_NUMELS(unit,reg) + SOC_REG_INFO(unit, reg).first_array_index - 1 : -1) < 0) {
        LOG_WARN(BSL_LS_APPL_SYMTAB,
                 (BSL_META_U(unit,
                             "Register %s: bad index specification\n"), ref));
        goto _return_error;
    }

    bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit,
                  "symtab: index %d:%d type %d regblktype [ ",
                  minidx, maxidx, reginfo->regtype);
    while (regblktype[idx] != SOC_BLOCK_TYPE_INVALID) {
        bsl_log_add(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                    unit, "%s ",
                    soc_block_name_lookup_ext(regblktype[idx], unit));
        idx++;
    }
    bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                unit, "]\n");

    alist->count = 0;

    regtype = reginfo->regtype;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    if (soc_is_reg_flexe_core(unit, reg))
    {
        regtype = soc_genreg;
    }
#endif
    switch (regtype) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
    case soc_cpureg:
    case soc_mcsreg:
    case soc_iprocreg:
        if (range2) {
            goto _return_error;
        }

        /* parse block spec in range1 */
        if (parse_block_range(unit, regblktype, range1,
                              &blk_min, &blk_max, bmask) < 0) {
            goto _return_error;
        }
        idx = 0;
        while (bmask[idx] != 0) {
            if (SOC_BLOCK_IN_LIST(unit, regblktype, bmask[idx])) {
                break; 
            }
            idx++;
        }
        if ((idx > 0) && (match == 0)) {
            goto _return_error;
        }
        if (blk_min < 0 || blk_max > SOC_INFO(unit).block_num) {
            goto _return_error;
        }

        blk = CMIC_BLOCK(unit);
        if (blk_min > blk || blk_max < blk) {
            goto _return_error;
        }

        bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit, "");
        _handle_array_refs(unit, alist, reg,
                           blk,
                           -1,            /* cos */
                           REG_PORT_ANY,    /* port */
                           minidx, maxidx);
        bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                    unit, "");
        break;
#endif /* ESW ONLY */
    case soc_ppportreg:
        /* parse port spec in range1 */
        if (! range1) {
            SOC_PBMP_ASSIGN(bm, PBMP_PP_ALL(unit));
        } else if (parse_pp_pbmp(unit, range1, &bm) < 0) {
            goto _return_error;
        }
        LOG_VERBOSE(BSL_LS_APPL_SYMTAB,
                    (BSL_META_U(unit,
                                "symtab: ppport bitmap %s\n"),
                                SOC_PBMP_FMT(bm, pfmt)));
        switch (regblktype[0]) {
        case SOC_BLK_IPIPE:
            blk = IPIPE_BLOCK(unit);
            break;
        case SOC_BLK_EPIPE:
            blk = EPIPE_BLOCK(unit);
            break;
        default:
            goto _return_error;
        }
        bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit, "");
        SOC_PBMP_ITER(bm, port) {
            _handle_array_refs(unit, alist, reg,
                               blk,
                               -1,      /* cos */
                               port,
                               minidx, maxidx);
        }
        bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                    unit, "");
        break;
    case soc_customreg:
        if (range2) {
            goto _return_error;
        }
        portblktype = SOC_BLK_PORT;
        port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
                           SOC_DRIVER(unit)->port_num_blktype : 1 ;

        /* parse port spec in range1 */
        if (! range1) {
            BCM_PBMP_ASSIGN(bm, SOC_INFO(unit).custom_reg_access.custom_port_pbmp);
        } else if (parse_pbmp(unit, range1, &bm) < 0) {
            goto _return_error;
        }
        BCM_PBMP_ITER(bm, port) { /* update alist for every port in bm */
            _handle_array_refs(unit, alist, reg, blk, -1, port, minidx, maxidx);
        }
        LOG_VERBOSE(BSL_LS_APPL_SYMTAB,(BSL_META_U(unit, "symtab: port bitmap %s\n"),SOC_PBMP_FMT(bm, pfmt)));
        break;

    case soc_portreg:
        if (range2) {
            goto _return_error;
        }

        portblktype = SOC_BLK_PORT;
        port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
            SOC_DRIVER(unit)->port_num_blktype : 1;

        /* parse port spec in range1 */
        if (! range1) {
            if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {

#ifdef BCM_DNX_SUPPORT
                if (SOC_IS_DNX(unit)) {
                    int is_cdmac_block = 0;
                    int is_clmac_block = 0;
                    SOC_PBMP_CLEAR(bm);

                    /*
                     * Convert CDMAC to CDPORT to get SOC_BLOCK_BITMAP
                     */
                    if(regblktype[0] == SOC_BLK_CDMAC) {
                        is_cdmac_block = 1;
                        regblktype[0] = SOC_BLK_CDPORT;
                    }

                    if(regblktype[0] == SOC_BLK_CLMAC) {
                        is_clmac_block = 1;
                        regblktype[0] = SOC_BLK_CLPORT;
                    }

                    SOC_BLOCKS_ITER(unit, blk, regblktype) {
                        SOC_PBMP_OR(bm, SOC_BLOCK_BITMAP(unit, blk));
                    }

                    if(is_cdmac_block) {
                        regblktype[0] = SOC_BLK_CDMAC;
                    }
                    if(is_clmac_block) {
                        regblktype[0] = SOC_BLK_CLMAC;
                    }
                }
                else
#endif
                {
                    SOC_PBMP_CLEAR(bm);

                    SOC_BLOCKS_ITER(unit, blk, regblktype) {
                        SOC_PBMP_OR(bm, SOC_BLOCK_BITMAP(unit, blk));
                    }
                }


            } else {
                SOC_PBMP_ASSIGN(bm, PBMP_ALL(unit));
                if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                     SOC_INFO(unit).linkphy_enabled) ||
                    (soc_feature(unit, soc_feature_subtag_coe) &&
                    SOC_INFO(unit).subtag_enabled)) {
                    SOC_PBMP_CLEAR(bm);
                    SOC_PBMP_ASSIGN(bm, PBMP_PORT_ALL(unit));
                    SOC_PBMP_PORT_ADD(bm, CMIC_PORT(unit));
                    SOC_PBMP_PORT_ADD(bm, LB_PORT(unit));
                }
                SOC_PBMP_OR(bm, PBMP_MMU(unit));
            } 
#ifdef BCM_GREYHOUND_SUPPORT
            if ((SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) 
                && (regblktype[0] == SOC_BLK_XLPORT)) {
                int rv;
                rv = soc_greyhound_pgw_reg_blk_index_get(unit, reg, REG_PORT_ANY, 
                    &bm, NULL, NULL, 0);
                if ((rv < 0) && (rv != SOC_E_NOT_FOUND)){
                    goto _return_error;
                }
            }
#endif
        } else if (parse_pbmp(unit, range1, &bm) < 0) {
            goto _return_error;
        }
        LOG_VERBOSE(BSL_LS_APPL_SYMTAB,
                    (BSL_META_U(unit,
                                "symtab: port bitmap %s\n"),
                                SOC_PBMP_FMT(bm, pfmt)));

        bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit, "");
        /* Coverity
         * DPORT_SOC_PBMP_ITER checks that dport is valid.
         */
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(unit, bm, dport, port) {
            if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    phy_port = port;
                }
                for (idx = 0; idx < port_num_blktype; idx++) {
#ifdef BCM_KATANA2_SUPPORT
                    /* Override port blocks with Linkphy Blocks.. */
                    if(SOC_IS_KATANA2(unit) && (regblktype[0] == SOC_BLK_TXLP) ) {
                        soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                                                             SOC_BLK_TXLP, &blk, NULL);
                        break;
                    } else if(SOC_IS_KATANA2(unit) &&
                              (regblktype[0] == SOC_BLK_RXLP)) {
                        soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                                                             SOC_BLK_RXLP, &blk, NULL);
                        break;
                    }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
                    if ((SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) 
                        && (regblktype[0] == SOC_BLK_XLPORT)) {
                        if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port, 
                            NULL, &blk, NULL, 0) > 0){
                            break;
                        }
                    }
#endif
#ifdef BCM_METROLITE_SUPPORT
                    /* For metrolite, override port block with IECELL block */
                    if ((SOC_IS_METROLITE(unit)) &&
                               (regblktype[0] == SOC_BLK_IECELL)) {
                        soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                                                             SOC_BLK_IECELL, &blk,   NULL);
                        break;
                    } else
#endif
#if defined (BCM_SABER2_SUPPORT)
                    /* For saber2, override port block with IECELL block */
                    if ((SOC_IS_SABER2(unit)) &&
                               (regblktype[0] == SOC_BLK_IECELL)) {
                        soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                                                             SOC_BLK_IECELL, &blk,   NULL);
                        break;
                    }
#endif
#if defined (BCM_FIRELIGHT_SUPPORT)
                    /* For firelight, override port block with MACSEC block */
                    if (SOC_IS_FIRELIGHT(unit) &&
                        (regblktype[0] == SOC_BLK_MACSEC)) {
                        soc_fl_macsec_port_reg_blk_idx_get(unit, phy_port,
                                                             SOC_BLK_MACSEC, &blk,   NULL);
                        break;
                    }
#endif
                    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, idx);
                    if (blk < 0) {
                        break;
                    }
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_ESW_SUPPORT)
                    if(SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CPRI){
                      if (!((IS_CPRI_PORT(unit,port) || IS_RSVD4_PORT(unit,port))) &&
                         !(reg == CPRI_RST_CTRLr || reg == CPRI_CLK_CTRLr))   {
                          continue;
                      }
                    } 
#endif
                    if (SOC_IS_APACHE(unit) && ((phy_port == 17) || (phy_port == 53))) {
                        if ((SOC_INFO(unit).port_speed_max[port] >= 100000) && (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORT)) {
                            continue;
                        }
                    }

                    if (SOC_BLOCK_IN_LIST(unit, regblktype,
                                          SOC_BLOCK_TYPE(unit, blk))) {
                        break;
                    }

                    if (SOC_IS_DNX(unit)) {
                        if( ((regblktype[0] == SOC_BLK_CDMAC) && (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CDPORT)) ||
                             ((regblktype[0] == SOC_BLK_CLMAC) && (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLPORT))) {
                            /*
                             * DNX only: CDMAC case.
                             * SOC_PORT_IDX_BLOCK returns CDPORT, therefore check type of blk is CDPORT.
                             */
                            break;
                        }
                    }

                }
#ifdef BCM_GREYHOUND_SUPPORT
                if ((SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit))
                    && (regblktype[0] == SOC_BLK_XLPORT)) {
                    if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port,
                        NULL, &blk, NULL, 1) > 0){
                        continue;
                    }
                }
#endif
                if (blk < 0 || idx == port_num_blktype) {
#if defined(BCM_MONTEREY_SUPPORT) && defined(BCM_ESW_SUPPORT)
                    if ((regblktype[0] == SOC_BLK_CPRI) && 
                               !((IS_CPRI_PORT(unit,port)) ||(IS_RSVD4_PORT(unit,port))) &&  
                                !(reg == CPRI_RST_CTRLr || reg == CPRI_CLK_CTRLr  ))
                        
                    {   
                        continue;
                    }else 
#endif 
                    {  
                        goto _return_error;
                    }
                }
            } else {
                int instance = -1;
                /*  This big switch statement will go away soon */
                {
                    switch (regblktype[0]) {
                    case SOC_BLK_IPIPE:
                        blk = IPIPE_BLOCK(unit);
                        break;
                    case SOC_BLK_IPIPE_HI:
                        blk = IPIPE_HI_BLOCK(unit);
                        /* coverity[overrun-local] */
                        if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), port)) {
                            continue;
                        }
                        break;
                    case SOC_BLK_EPIPE:
                        blk = EPIPE_BLOCK(unit);
                        break;
                    case SOC_BLK_EPIPE_HI:
                        blk = EPIPE_HI_BLOCK(unit);
                        if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), port)) {
                            continue;
                        }
                        break;
                    case SOC_BLK_IGR:
                        blk = IGR_BLOCK(unit);
                        break;
                    case SOC_BLK_EGR:
                        blk = EGR_BLOCK(unit);
                        break;
                    case SOC_BLK_BSE: /* Aliased with IL */
                        if (SOC_IS_SHADOW(unit)) {
                            if (port == 9) {
                                blk = IL0_BLOCK(unit);
                            } else if (port == 13) {
                                blk = IL1_BLOCK(unit);
                            }
                        } else {
                            blk = BSE_BLOCK(unit);
                        }
                        break;
                    case SOC_BLK_CSE: /* Aliased with MS_ISEC */
                        if (SOC_IS_SHADOW(unit)) {
                            if (port >=1 && port <= 4) {
                                blk = MS_ISEC0_BLOCK(unit);
                            } else if (port >= 5 && port <= 8) {
                                blk = MS_ISEC1_BLOCK(unit);
                            }
                        } else {
                            blk = CSE_BLOCK(unit);
                        }
                        break;
                    case SOC_BLK_HSE: /* Aliased with MS_ESEC */
                        if (SOC_IS_SHADOW(unit)) {
                            if (port >= 1 && port <= 4) {
                                blk = MS_ESEC0_BLOCK(unit);
                            } else if (port >= 5 && port <= 8) {
                                blk = MS_ESEC1_BLOCK(unit);
                            }
                        } else {
                            blk = HSE_BLOCK(unit);
                        }
                        break;
                    case SOC_BLK_SYS: /* Aliased with CW */
                        if (SOC_IS_SHADOW(unit)) {
                            blk = CW_BLOCK(unit);
                        }
                        break;
                    case SOC_BLK_BSAFE:
                        blk = BSAFE_BLOCK(unit);
                        break;
                    case SOC_BLK_ESM:
                        blk = ESM_BLOCK(unit);
                        break;
                    case SOC_BLK_ARL:
                        blk = ARL_BLOCK(unit);
                        break;
                    case SOC_BLK_MMU:
                        blk = MMU_BLOCK(unit);
                        break;
                    case SOC_BLK_MCU:
                        blk = MCU_BLOCK(unit);
                        break;
                    case SOC_BLK_PGW_CL:
                        /* coverity[overrun-local] */
                        instance = SOC_INFO(unit).port_group[port];
                        blk = PGW_CL_BLOCK(unit, instance);
                        if (blk < 0) {
                            goto _return_error;
                        }
                        break;
#ifdef BCM_HURRICANE3_SUPPORT
                     case SOC_BLK_PGW_GE:                       
                        if (SOC_IS_HURRICANE3(unit)) {
                            blk = PGW_GE_BLOCK(unit, port);
                            if (blk < 0) {
                                goto _return_error;
                            }
                            port |= SOC_REG_ADDR_INSTANCE_MASK;
                        }
                        break;
#endif /* BCM_HURRICANE3_SUPPORT */
                    default:
                        /* search for a matching block... */
                        SOC_BLOCKS_ITER(unit, blk, regblktype) {
                            break;
                        }
                    }
                }
            }
            _handle_array_refs(unit, alist, reg,
                               blk,
                               -1,        /* cos */
                               port,
                               minidx, maxidx);
        }
        bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                    unit, "");
        break;
    case soc_cosreg:
    /* parse cos spec in range1 */
        if (parse_num_range(unit, range1, &cos_min, &cos_max, 0,
                            NUM_COS(unit) - 1) < 0) {
            goto _return_error;
        }
        /* parse block spec in range2 */
        if (parse_block_range(unit, regblktype, range2,
                              &blk_min, &blk_max, bmask) < 0) {
            goto _return_error;
        }
        idx = 0;
        while (bmask[idx] != 0) {
            if (SOC_BLOCK_IN_LIST(unit, regblktype, bmask[idx])) {
                break; 
            }
            idx++;
        }
        if ((idx > 0) && (match == 0)) {
            goto _return_error;
        }
        if (blk_min < 0 || blk_max > SOC_INFO(unit).block_num) {
            goto _return_error;
        }

        bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit, "");
        for (cos = cos_min; cos <= cos_max; cos++) {
            SOC_BLOCKS_ITER(unit, blk, regblktype) {
                if (blk < blk_min || blk > blk_max) {
                    continue;
                }
                port = SOC_BLOCK_PORT(unit, blk);
                _handle_array_refs(unit, alist, reg,
                                   blk,
                                   cos,
                                   port,
                                   -1, -1);
            }
        }
        bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                    unit, "");
        break;

    case soc_genreg:
        if (range2) {
            goto _return_error;
        }

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
        {
            /* parse block spec in range1 */
            if (parse_block_range(unit, regblktype, range1,
                  &blk_min, &blk_max, bmask) < 0) {
                goto _return_error;
            }
            idx = 0;
            while (bmask[idx] != 0) {
                if (SOC_BLOCK_IN_LIST(unit, regblktype, bmask[idx])) {
                    break; 
                }
                idx++;
            }
            if ((idx > 0) && (match == 0)) {
                goto _return_error;
            }
            if (blk_min < 0 || blk_max > SOC_INFO(unit).block_num) {
                goto _return_error;
            }

            bsl_log_start(BSL_APPL_SYMTAB, bslSeverityNormal, unit, "");
            SOC_BLOCKS_ITER(unit, blk, regblktype) {
                if (blk < blk_min || blk > blk_max) {
                    continue;
                }
                switch (regblktype[0]) {
                case SOC_BLK_PMQPORT:
                case SOC_BLK_QTGPORT:
                case SOC_BLK_PMQ:
                case SOC_BLK_PGW_CL:
                    port = SOC_REG_ADDR_INSTANCE_MASK | SOC_BLOCK_NUMBER(unit, blk);
                    break;
                default:
                    port = SOC_BLOCK_PORT(unit, blk);
                    break;
                }
#ifdef BCM_GREYHOUND_SUPPORT
                if ((SOC_IS_HURRICANE3(unit)) &&
                        (regblktype[0] == SOC_BLK_PGW_GE)) {
                
                        port = SOC_REG_ADDR_INSTANCE_MASK |
                            SOC_BLOCK_NUMBER(unit, blk);
                }
                if ((SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) &&
                        (regblktype[0] == SOC_BLK_PMQ)) {

                        port = SOC_REG_ADDR_INSTANCE_MASK |
                            SOC_BLOCK_NUMBER(unit, blk);
                }
                if ((SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) 
                    && ((regblktype[0] == SOC_BLK_XLPORT) ||
                    (regblktype[0] == SOC_BLK_GXPORT))) {
                    if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port, 
                                                NULL, NULL, NULL, 0) == SOC_E_NOT_FOUND){
                        /* port register not belong to pgw_gx and pgw_xl */
                        if (!PBMP_MEMBER(SOC_BLOCK_BITMAP(unit, blk), port)){
                            /* check the valid blk bitmap*/
                            continue;
                        }
                    } else if (regblktype[0] == SOC_BLK_XLPORT){
                        if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port, 
                            NULL, &blk, NULL, 1) > 0){
                            continue;
                        } 
                     }
                }
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
                if ((SOC_IS_TOMAHAWK2(unit)) &&
                        (regblktype[0] == SOC_BLK_OTPC)) {
                    if (blk_min == blk_max) {
                        port = SOC_REG_ADDR_INSTANCE_MASK |
                            SOC_BLOCK_NUMBER(unit, blk);
                    }
                }
#endif
                _handle_array_refs(unit, alist, reg,
                                   blk,
                                   -1,        /* cos */
                                   port,
                                   minidx, maxidx);
            }
            bsl_log_end(BSL_APPL_SYMTAB, bslSeverityNormal, bslSinkIgnore,
                        unit, "");
            if ((SOC_IS_TOMAHAWK3(unit)) && (regblktype[0] == SOC_BLK_CDMAC)) {
               
                port = REG_PORT_ANY; 
                _handle_array_refs(unit, alist, reg,
                                   blk,
                                   -1,        /* cos */
                                   port,
                                   minidx, maxidx);
            }
        }
#endif
        break;

    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
        legal_min = 0;
        legal_max = -1;
        regtype_name = NULL;
        switch (reginfo->regtype) {
        case soc_pipereg:
            legal_max = NUM_PIPE(unit) - 1;
            regtype_name = "pipe"; /* for debug print */
            break;
        case soc_xpereg:
            legal_max = NUM_XPE(unit) - 1;
            regtype_name = "xpe"; /* for debug print */
            break;
        case soc_itmreg:
            legal_max = NUM_ITM(unit) - 1;
            regtype_name = "itm"; /* for debug print */
            break;
        case soc_ebreg:
            legal_max = NUM_EB(unit) - 1;
            regtype_name = "eb"; /* for debug print */
            break;
        case soc_slicereg:
            legal_max = NUM_SLICE(unit) - 1;
            regtype_name = "slice"; /* for debug print */
            break;
        case soc_layerreg:
            legal_max = NUM_LAYER(unit) - 1;
            regtype_name = "layer"; /* for debug print */
            break;
        default:
            assert(0);
            break;
        }
        /* parse base_index number in range 1 */
        if (parse_num_range(unit, range1, &cos_min, &cos_max, legal_min,
                            legal_max) < 0) {
            goto _return_error;
        }
        /* parse block spec in range2 */
        if (parse_block_range(unit, regblktype, range2, &blk_min, &blk_max,
                              bmask) < 0) {
            goto _return_error;
        }
        if (blk_min < 0 || blk_max > SOC_INFO(unit).block_num) {
            goto _return_error;
        }
        idx = 0;
        while (bmask[idx] != 0) {
            if (!SOC_BLOCK_IN_LIST(unit, regblktype, bmask[idx])) {
                break; 
            }
            idx++;
        }
        if (bmask[idx] != 0) {
            goto _return_error;
        }
        LOG_VERBOSE(BSL_LS_APPL_SYMTAB,
                    (BSL_META_U(unit,
                                "symtab: %s %d:%d\n"),
                     regtype_name, cos_min, cos_max));
        for (range = cos_min; range <= cos_max; range++) {
            SOC_BLOCKS_ITER(unit, blk, regblktype) {
                if (blk < blk_min || blk > blk_max) {
                    continue;
                }
                _handle_array_refs(unit, alist, reg, blk, -1,
                                   SOC_REG_ADDR_INSTANCE_MASK | range,
                                   minidx, maxidx);
            }
        }
        break;
    default:
    assert(0);
        break;
    }

    if (bsl_check(bslLayerAppl, bslSourceSymtab, bslSeverityNormal, unit)) {
        int i;

        bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");
        for (i = 0; i < alist->count; i++) {
            char buf[80];
            sal_memset(buf, 0x0, 80);
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
            soc_reg_sprint_addr(unit,  buf, &alist->ainfo[i]);
#endif
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore,
                        unit, "symtab:  %d: %s %x\n",
                        i, buf, alist->ainfo[i].addr);
        }
        bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore,
                    unit, "");
    }

    if (alist->count == 0) {
        goto _return_error;
    }
    if (bmask) {
        sal_free(bmask);
    }
    return 0;

_return_error:
    sal_free(bmask);
    return -1;
}
