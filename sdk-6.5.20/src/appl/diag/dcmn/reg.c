/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        reg.c
 * Purpose:     Diag shell register commands for DPP
 */



#include <shared/bsl.h>

#include <appl/diag/system.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <appl/dpp/regs_filter.h>
#endif 
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#endif
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif 

#if (defined(LINUX) || defined(UNIX)) && (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
extern int _cpu_write(int d, uint32 addr, uint32 *buf);
extern int _cpu_read(int d, uint32 addr, uint32 *buf);
#endif

#define PFLAG_ALIAS      0x01
#define PFLAG_SUMMARY    0x02


/*
 * Register Types - for getreg and dump commands
 */
static regtype_entry_t dpp_regtypes[] = {
    { "SOC",                         soc_genreg,
      "SOC registers" },
    { "CMIC_SBUS_RING_MAP_0_7",      soc_cpureg,
      "CMIC_SBUS_RING_MAP_0_7 registers" },
    { "CMIC_SBUS_RING_MAP_8_15",     soc_cpureg,
      "CMIC_SBUS_RING_MAP_8_15 registers" },
    { "CMIC_SBUS_RING_MAP_16_23",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_16_23 registers" },
    { "CMIC_SBUS_RING_MAP_24_31",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_24_31 registers" },
    { "CMIC_SBUS_RING_MAP_32_39",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_32_39 registers" },
    { "CMIC_SBUS_RING_MAP_40_47",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_40_47 registers" },
    { "CMIC_SBUS_RING_MAP_48_55",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_48_55 registers" },
    { "CMIC_SBUS_RING_MAP_56_63",    soc_cpureg,
      "CMIC_SBUS_RING_MAP_56_63 registers" },
};

#define regtypes_count  COUNTOF(dpp_regtypes)


#define PRINT_COUNT(str, len, wrap, prefix) \
    if ((wrap > 0) && (len > wrap)) { \
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"\n%s", prefix); \
        len = sal_strlen(prefix); \
    } \
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s", str); \
    len += strlen(str)


/*
 * Given an address, fill in regaddrinfo structure
 * This can only be called for SOC registers, not for CPU registers
 */
STATIC void
dpp_soc_regaddrinfo_get(int unit, soc_regaddrinfo_t *ainfo, uint32 addr,
                        uint32 cmic_blk)
{
    int                     reg;
    uint32                  lkup_addr;
    int idx = 0, schan_blk; 

    if (ainfo == NULL) {
        return;
    }

    ainfo->valid = 0;
    ainfo->reg = INVALIDr;
    
    ainfo->block = SOC_BLK_NONE;
    lkup_addr =  addr;
    schan_blk = cmic_blk;

    while(SOC_BLOCK_INFO(unit, idx).type != -1) {
        if (SOC_BLOCK_INFO(unit, idx).schan ==  schan_blk) {
            ainfo->block  = SOC_BLOCK_INFO(unit, idx).type;
            ainfo->idx = SOC_BLOCK_INFO(unit, idx).number;
            break;
        }
        idx++;
    }

    if (SOC_BLK_NONE == ainfo->block) {
        ainfo->valid = 0;
        ainfo->reg = INVALIDr;
        return;
    }
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        if (lkup_addr == (SOC_REG_INFO(unit, reg).offset & (~0x80000))) {
            soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
            
            if (SOC_BLOCK_IN_LIST(unit, regblktype, ainfo->block)) {
                ainfo->valid = 1;
                ainfo->reg = reg;
                ainfo->field = INVALIDf;
                ainfo->cos = -1;
                ainfo->port = -1;
                break;
            }
        }
    }
}


/* 
 * Lists registers containing a specific pattern
 *
 * If use_reset is true, ignores val and uses reset default value.
 */
STATIC void
dpp_do_reg_list(int unit, soc_regaddrinfo_t *ainfo, int use_reset, 
                soc_reg_above_64_val_t regval)
{
    soc_reg_t       reg = ainfo->reg;
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, reg);
    soc_field_info_t *fld;
    int                 f;
    uint32              flags;
    soc_reg_above_64_val_t mask, rval, rmsk;
    char                buf[256];
    char                rval_str[256], rmsk_str[256], dval_str[256];
    int                    i, copies, disabled;
    soc_reg_above_64_val_t  fldval;


    if (!SOC_REG_IS_VALID(unit, reg)) {
        cli_out("Register %s is not valid for chip %s\n",
                SOC_REG_NAME(unit, reg), SOC_UNIT_NAME(unit));
        return;
    }
    flags = reginfo->flags;

    SOC_REG_ABOVE_64_ALLONES(mask);

    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, rval);
    SOC_REG_ABOVE_64_RST_MSK_GET(unit, reg, rmsk);
    if (use_reset) {
        SOC_REG_ABOVE_64_COPY(regval, rval);
        SOC_REG_ABOVE_64_COPY(mask, rmsk);
    }

    soc_reg_sprint_addr(unit, buf, ainfo);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Register: %s", buf);
#if !defined(SOC_NO_ALIAS)
    if (soc_reg_alias[reg] && *soc_reg_alias[reg]) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," alias %s", soc_reg_alias[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " %s register", soc_regtypenames[reginfo->regtype]);
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " address 0x%08x\n", ainfo->addr);

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"Flags:");
    if(flags & SOC_REG_FLAG_ABOVE_64_BITS) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit," above 64-bits");
    } else if (flags & SOC_REG_FLAG_64_BITS) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " 64-bits");
    } else {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " 32-bits");
    }
    if (flags & SOC_REG_FLAG_COUNTER) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " counter");
    }
    if (flags & SOC_REG_FLAG_ARRAY) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " array[%d-%d]", 0, reginfo->numels-1);
    }
    if (flags & SOC_REG_FLAG_NO_DGNL) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " no-diagonals");
    }
    if (flags & SOC_REG_FLAG_RO) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " read-only");
    }
    if (flags & SOC_REG_FLAG_WO) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " write-only");
    }
    if (flags & SOC_REG_FLAG_ED_CNTR) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " error/discard-counter");
    }
    if (flags & SOC_REG_FLAG_SPECIAL) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " special");
    }
    if (flags & SOC_REG_FLAG_EMULATION) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " emulation");
    }
    if (flags & SOC_REG_FLAG_VARIANT1) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " variant1");
    }
    if (flags & SOC_REG_FLAG_VARIANT2) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " variant2");
    }
    if (flags & SOC_REG_FLAG_VARIANT3) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " variant3");
    }
    if (flags & SOC_REG_FLAG_VARIANT4) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " variant4");
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Blocks:");
    copies = disabled = 0;
    for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
        /*if (SOC_BLOCK_INFO(unit, i).type & reginfo->block) {*/
        if (SOC_BLOCK_IS_TYPE(unit, i, reginfo->block)) {
            if (SOC_INFO(unit).block_valid[i]) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " %s", SOC_BLOCK_NAME(unit, i));
            } else {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " [%s]", SOC_BLOCK_NAME(unit, i));
                disabled += 1;
            }
            copies += 1;
        }
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " (%d cop%s", copies, copies == 1 ? "y" : "ies");
    if (disabled) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", %d disabled", disabled);
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ")\n");

#if !defined(SOC_NO_DESC)
    /* coverity[overrun-local: FALSE] */
    if (soc_reg_desc[reg] && *soc_reg_desc[reg]) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Description: %s\n", soc_reg_desc[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Displaying:");
    if (use_reset) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " reset defaults");
    } else {
        if (SOC_REG_IS_ABOVE_64(unit, reg)) {
            format_long_integer(dval_str, regval, SOC_REG_ABOVE_64_INFO(unit, reg).size);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " value %s", dval_str);
        } else {
            format_long_integer(dval_str, regval, 2);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " value %s", dval_str);
        }
    }
    
    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        format_long_integer(rval_str, rval, SOC_REG_ABOVE_64_INFO(unit, reg).size);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\nreset value %s",rval_str);
        format_long_integer(rmsk_str, rmsk, SOC_REG_ABOVE_64_INFO(unit, reg).size);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\nmask %s\n",rmsk_str);

    } else {

        uint64 rval64, rmsk64;
        SOC_REG_RST_VAL_GET(unit, reg, rval64);
        SOC_REG_RST_MSK_GET(unit, reg, rmsk64);
        format_uint64(rval_str, rval64);
        format_uint64(rmsk_str, rmsk64);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ", reset value %s mask %s\n", rval_str, rmsk_str);

    }

    for (f = reginfo->nFields - 1; f >= 0; f--) {
    fld = &reginfo->fields[f];
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  %s<%d", SOC_FIELD_NAME(unit, fld->field),
           fld->bp + fld->len - 1);
    if (fld->len > 1) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ":%d", fld->bp);
    }

    soc_reg_above_64_field_get(unit, reg, mask, fld->field,fldval);
    if (use_reset && SOC_REG_ABOVE_64_IS_ZERO(fldval)) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "> = x");
    } else {
        soc_reg_above_64_field_get(unit, reg, regval, fld->field,fldval);
        format_long_integer(buf, fldval, ((fld->len+31)/32));
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "> = %s", buf);
    }
    if (fld->flags & (SOCF_RO|SOCF_WO)) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " [");
        i = 0;
        if (fld->flags & SOCF_RO) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "RO");
            i++;
        }
        if (fld->flags & SOCF_WO) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%sWO", i++ ? "," : "");
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "]");
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");

    }
}

STATIC void
_print_regname(int unit, soc_reg_t reg, int *col, int pflags)
{
    int             len;
    soc_reg_info_t *reginfo;

    reginfo = &SOC_REG_INFO(unit, reg);
    len = strlen(SOC_REG_NAME(unit, reg)) + 1;

    if (pflags & PFLAG_SUMMARY) {
        char    tname, *dstr1, *dstr2, *bname;
        int     dlen, copies, i;
        char    nstr[128], bstr[64];

        switch (reginfo->regtype) {
        case soc_schan_reg:
            tname = 's';
            break;
        case soc_cpureg:
            tname = 'c';
            break;
        case soc_genreg:
            tname = 'g';
            break;
        case soc_portreg:
            tname = 'p';
            break;
        case soc_cosreg:
            tname = 'o';
            break;
        case soc_hostmem_w:
        case soc_hostmem_h:
        case soc_hostmem_b:
            tname = 'm';
            break;
        case soc_phy_reg:
            tname = 'f';
            break;
        case soc_pci_cfg_reg:
            tname = 'P';
            break;
        default:
            tname = '?';
            break;
        }
#if !defined(SOC_NO_DESC)
        dstr2 = strchr(soc_reg_desc[reg], '\n');
        if (dstr2 == NULL) {
            dlen = strlen(soc_reg_desc[reg]);
        } else {
            dlen = dstr2 - soc_reg_desc[reg];
        }
        if (dlen > 30) {
            dlen = 30;
            dstr2 = "...";
        } else {
            dstr2 = "";
        }
        dstr1 = soc_reg_desc[reg];
#else /* defined(SOC_NO_DESC) */
        dlen = 1;
        dstr1 = "";
        dstr2 = "";
#endif /* defined(SOC_NO_DESC) */
        if (reginfo->flags & SOC_REG_FLAG_ARRAY) {
            sal_sprintf(nstr, "%s[%d]",
                        SOC_REG_NAME(unit, reg), reginfo->numels);
        } else {
            sal_sprintf(nstr, "%s", SOC_REG_NAME(unit, reg));
        }

        copies = 0;
        bname = NULL;
        for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
            /*if (SOC_BLOCK_INFO(unit, i).type & reginfo->block) {*/
            if (SOC_BLOCK_IS_TYPE(unit, i, reginfo->block)) {
                if (bname == NULL) {
                    bname = SOC_BLOCK_NAME(unit, i);
                }
                copies += 1;
            }
        }
        if (copies > 1) {
            sal_sprintf(bstr, "%d/%s", copies, bname);
        } else if (copies == 1) {
            sal_sprintf(bstr, "%s", bname);
        } else {
            sal_sprintf(bstr, "none");
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, " %c%c%c%c%c  %-26s %-8.8s  %*.*s%s\n",
               tname,
               (reginfo->flags & SOC_REG_FLAG_64_BITS) ? '6' : '3',
               (reginfo->flags & SOC_REG_FLAG_COUNTER) ? 'c' : '-',
               (reginfo->flags & SOC_REG_FLAG_ED_CNTR) ? 'e' : '-',
               (reginfo->flags & SOC_REG_FLAG_RO) ? 'r' :
               (reginfo->flags & SOC_REG_FLAG_WO) ? 'w' : '-',
               nstr,
               bstr,
               dlen, dlen, dstr1, dstr2);
        return;
    }
    if (*col < 0) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "  ");
        *col = 2;
    }
    if (*col + len > ((pflags & PFLAG_ALIAS) ? 65 : 72)) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n  ");
        *col = 2;
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s%s ",
           SOC_REG_NAME(unit, reg), SOC_REG_ARRAY(unit, reg) ? "[]" : "");
#if !defined(SOC_NO_ALIAS)
    if ((pflags & PFLAG_ALIAS) && soc_reg_alias[reg]) {
        len += strlen(soc_reg_alias[reg]) + 8;
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "(aka %s) ", soc_reg_alias[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    *col += len;
}

STATIC void
_list_regs_by_type(int unit, soc_block_t blk, int *col, int pflag)
{
    soc_reg_t       reg;

    *col = -1;
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        /*if (SOC_REG_INFO(unit, reg).block & blk) {*/
        if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, blk)) {
            _print_regname(unit, reg, col, pflag);
        }
    }
    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
}

char cmd_dpp_reg_list_usage[] =
"Usage: listreg [options] regname [value]\n"
"Options:\n"
"    -alias/-a       display aliases\n"
"    -summary/-s     display single line summary for each reg\n"
"    -counters/-c    display counters\n"
"    -ed/-e          display error/discard counters\n"
"    -type/-t        display registers grouped by block type\n"
"If regname is '*' or does not match a register name, a substring\n"
"match is performed.  [] indicates register array.\n"
"If regname is a numeric address, the register that resides at that\n"
"address is displayed.\n";

cmd_result_t
cmd_dpp_reg_list(int unit, args_t *a)
{
    char           *str;
    char           *val;
    soc_reg_above_64_val_t   value;
    soc_regaddrinfo_t ainfo;
    int             found;
    int             rv = CMD_OK;
    int             all_regs;
    soc_reg_t       reg;
    int             col;
    int             pflag;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_property_get(unit, spn_REGLIST_ENABLE, 1)) {
        return CMD_OK;
    }
    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    ainfo.reg = INVALIDr;
    pflag = 0;
    col = -1;

    /* Parse options */
    while (((str = ARG_GET(a)) != NULL) && (str[0] == '-')) {
        while (str[0] && str[0] == '-') {
            str += 1;
        }
        if (sal_strcasecmp(str, "alias") == 0 ||
            sal_strcasecmp(str, "a") == 0) {    /* list w/ alias */
            pflag |= PFLAG_ALIAS;
            continue;
        }
        if (sal_strcasecmp(str, "summary") == 0 ||
            sal_strcasecmp(str, "s") == 0) {    /* list w/ summary */
            pflag |= PFLAG_SUMMARY;
            continue;
        }
        if (sal_strcasecmp(str, "counters") == 0 ||
            sal_strcasecmp(str, "c") == 0) {    /* list counters */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "unit %d counters\n", unit);
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                if (!SOC_REG_IS_VALID(unit, reg))
                    continue;
                if (!SOC_REG_IS_COUNTER(unit, reg))
                    continue;
                _print_regname(unit, reg, &col, pflag);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n\n");
            return CMD_OK;
        }
        if (sal_strcasecmp(str, "ed") == 0 ||
            sal_strcasecmp(str, "e") == 0) {    /* error/discard */
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "unit %d error/discard counters\n", unit);
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                if (!SOC_REG_IS_VALID(unit, reg)) {
                    continue;
                }
                if (!(SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_ED_CNTR)) {
                    continue;
                }
                _print_regname(unit, reg, &col, pflag);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n\n");
            return CMD_OK;
        }
        if (sal_strcasecmp(str, "type") == 0 ||
            sal_strcasecmp(str, "t") == 0) {    /* list by type */
            int    i;
            
            /*
              soc_block_t    blk;
              
              for (i = 0; i < (sizeof(SOC_INFO(unit).has_block) * 8); i++) {
              blk = (1 << i);
              if ( !(blk & SOC_INFO(unit).has_block)) {
              continue;
              }*/
            
            for (i = 0; i < COUNTOF(SOC_INFO(unit).has_block); i++) {
                if (!(SOC_INFO(unit).has_block[i])) {
                    continue;
                }
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "unit %d %s registers\n",
                       unit,
                       /*soc_block_name_lookup_ext(blk, unit));*/
                       soc_block_name_lookup_ext
                       (SOC_INFO(unit).has_block[i], unit));
                col = -1;
                /*_list_regs_by_type(unit, blk, &col, pflag);*/
                _list_regs_by_type(unit, SOC_INFO(unit).has_block[i],
                                   &col, pflag);
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
            return CMD_OK;
        }
        cli_out("ERROR: unrecognized option: %s\n", str);
        return CMD_FAIL;
    }

    if (!str) {
        return CMD_USAGE;
    }

    if ((val = ARG_GET(a)) != NULL) {
        parse_long_integer(value, SOC_REG_ABOVE_64_MAX_SIZE_U32, val);

    } else {
        SOC_REG_ABOVE_64_CLEAR(value);

    }


    if (isint(str)) {
        /* 
         * Address given, look up SOC register.
         */
        char            buf[80];
        uint32          addr;
        addr = parse_integer(str);
        dpp_soc_regaddrinfo_get(unit, &ainfo, addr,0);
        if (!ainfo.valid || (int)ainfo.reg < 0) {
            cli_out("Unknown register address: 0x%x\n", addr);
            rv = CMD_FAIL;
        } else {
            soc_reg_sprint_addr(unit, buf, &ainfo);
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Address %s\n", buf);
        }
    } else {
        soc_regaddrlist_t alist;
        
        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return CMD_FAIL;
        }

        /* 
         * Symbolic name.
         * First check if the register is there as exact match.
         * If not, list all substring matches.
         */

        all_regs = 0;
        if (*str == '$') {
            str++;
        } else if (*str == '*') {
            str++;
            all_regs = 1;
        }

        if (all_regs || parse_symbolic_reference(unit, &alist, str) < 0) {
            found = 0;
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                if (!SOC_REG_IS_VALID(unit, reg)) {
                    continue;
                }
                if (strcaseindex(SOC_REG_NAME(unit, reg), str) != 0) {
                    if (!found && !all_regs) {
                        cli_out("Unknown register; possible matches are:\n");
                    }
                    _print_regname(unit, reg, &col, pflag);
                    found = 1;
                }
            }
            if (!found) {
                cli_out("No matching register found");
            }
            bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
            rv = CMD_FAIL;
        } else {
            ainfo = alist.ainfo[0];
        }

        soc_regaddrlist_free(&alist);
    }

    /* 
     * Now have ainfo -- if reg is no longer INVALIDr
     */

    if (ainfo.reg != INVALIDr) {
        if (val) {
            dpp_do_reg_list(unit, &ainfo, 0, value);
        } else {
            SOC_REG_ABOVE_64_CLEAR(value);
            dpp_do_reg_list(unit, &ainfo, 1, value);
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

    return rv;
}


/* 
 * Print a SOC internal register with fields broken out.
 */
STATIC  void
dpp_reg_print(int unit, soc_regaddrinfo_t *ainfo, soc_reg_above_64_val_t val, uint32 flags,
      char *fld_sep, int wrap)
{
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, ainfo->reg);
    int             f;
    char            buf[80];
    char            line_buf[256];
    int             linelen = 0;
    int             nprint;
    int             rlen;
    int             skip;
    soc_reg_above_64_val_t          resval, fieldval, resfld;

    if (flags & REG_PRINT_HEX) {
        format_long_integer(line_buf, val, SOC_REG_ABOVE_64_MAX_SIZE_U32);
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s\n", line_buf);
        return;
    }
   
    if (flags & REG_PRINT_ADDR ) {
        if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
            rlen = SOC_REG_ABOVE_64_INFO(unit, ainfo->reg).size;
        } else if (SOC_REG_IS_64(unit, ainfo->reg)){
            rlen = 2;
        } else {
            rlen = 1; 
        }
        format_spec_long_integer(line_buf, val, rlen);

        skip = 0;
        if(SOC_REG_IS_ARRAY(unit, ainfo->reg)) {
            skip = ainfo->idx*SOC_REG_ARRAY_INFO(unit, ainfo->reg).element_skip;
        } else if (SOC_REG_ARRAY(unit, ainfo->reg)) {
            if (SOC_REG_ARRAY2(unit, ainfo->reg)) {
                skip = ainfo->idx*2;
            } else {
                skip = ainfo->idx;
            }
        }
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit,"0x%02X 0x%04X: %s\n",SOC_BLOCK_INFO(unit, ainfo->block).schan,SOC_REG_INFO(unit,ainfo->reg).offset + skip, line_buf);
        return;
    }

    if (flags & REG_PRINT_CHG) {
        SOC_REG_ABOVE_64_RST_VAL_GET(unit, ainfo->reg, resval);
        if (SOC_REG_ABOVE_64_IS_EQUAL(val, resval)) {    /* no changed fields */
            return;
        }
    } else {
        SOC_REG_ABOVE_64_CLEAR(resval);
    }

    soc_reg_sprint_addr(unit, buf, ainfo);

    sal_sprintf(line_buf, "%s[0x%x]=", buf, ainfo->addr);
    PRINT_COUNT(line_buf, linelen, wrap, "   ");

    format_long_integer(line_buf, val, SOC_REG_ABOVE_64_MAX_SIZE_U32);
    PRINT_COUNT(line_buf, linelen, -1, "");

    if (flags & REG_PRINT_RAW) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");
        return;
    }

    PRINT_COUNT(": <", linelen, wrap, "   ");

    nprint = 0;
    for (f = reginfo->nFields - 1; f >= 0; f--) {
        soc_field_info_t *fld = &reginfo->fields[f];
        soc_reg_above_64_field_get(unit, ainfo->reg, val, fld->field, fieldval);
        if (flags & REG_PRINT_CHG) {
            soc_reg_above_64_field_get(unit, ainfo->reg, resval, fld->field, resfld);
            if (SOC_REG_ABOVE_64_IS_EQUAL(fieldval, resfld)) {
                continue;
            }
        }
         {
            if (nprint > 0) 
            {
                sal_sprintf(line_buf, "%s", fld_sep);
                PRINT_COUNT(line_buf, linelen, -1, "");
            }
            sal_sprintf(line_buf, "%s=", SOC_FIELD_NAME(unit, fld->field));

            PRINT_COUNT(line_buf, linelen, wrap, "   ");
            format_long_integer(line_buf, fieldval, (fld->len+31)/32);
            PRINT_COUNT(line_buf, linelen, -1, "");

        }

        nprint += 1;
        /*bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");*/

    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, ">\n");
}



int ignore_reg_fe3200(int unit, char *name)
{
#ifdef BCM_88950_SUPPORT
  if (SOC_IS_FE3200(unit)) {
    char *regs[] = {
        "CCS_INDIRECT_COMMAND",
        "CCS_INDIRECT_COMMAND_ADDRESS",
        "CCS_INDIRECT_COMMAND_RD_DATA",
        "CCS_INDIRECT_COMMAND_WR_DATA",
        "CCS_RESERVED_PCMI_0_S",
        "CCS_RESERVED_PCMI_0_T",
        "CCS_RESERVED_PCMI_1_S",
        "CCS_RESERVED_PCMI_1_T",
        "CCS_RESERVED_PCMI_2_S",
        "CCS_RESERVED_PCMI_2_T",
        "CCS_RESERVED_PCMI_3_S",
        "CCS_RESERVED_PCMI_3_T",
        "CCS_RESERVED_PCMI_4_S",
        "CCS_RESERVED_PCMI_4_T",
        "DCH_INDIRECT_COMMAND",
        "DCH_INDIRECT_COMMAND_ADDRESS",
        "DCH_INDIRECT_COMMAND_RD_DATA",
        "DCH_INDIRECT_COMMAND_WR_DATA",
        "DCH_RESERVED_PCMI_0_S",
        "DCH_RESERVED_PCMI_0_T",
        "DCH_RESERVED_PCMI_1_S",
        "DCH_RESERVED_PCMI_1_T",
        "DCH_RESERVED_PCMI_3_S",
        "DCH_RESERVED_PCMI_3_T",
        "DCL_RESERVED_PCMI_2_S",
        "DCL_RESERVED_PCMI_2_T",
        "DCL_RESERVED_PCMI_3_S",
        "DCL_RESERVED_PCMI_3_T",
        "DCL_RESERVED_PCMI_4_S",
        "DCL_RESERVED_PCMI_4_T",
        "DCMC_INDIRECT_COMMAND",
        "DCMC_INDIRECT_COMMAND_ADDRESS",
        "DCMC_INDIRECT_COMMAND_RD_DATA",
        "DCMC_INDIRECT_COMMAND_WR_DATA",
        "DCMC_RESERVED_PCMI_0_S",
        "DCMC_RESERVED_PCMI_0_T",
        "DCMC_RESERVED_PCMI_1_S",
        "DCMC_RESERVED_PCMI_1_T",
        "DCMC_RESERVED_PCMI_2_S",
        "DCMC_RESERVED_PCMI_2_T",
        "DCMC_RESERVED_PCMI_3_S",
        "DCMC_RESERVED_PCMI_3_T",
        "DCMC_RESERVED_PCMI_4_S",
        "DCMC_RESERVED_PCMI_4_T",
        "DCM_INDIRECT_COMMAND",
        "DCM_INDIRECT_COMMAND_ADDRESS",
        "DCM_INDIRECT_COMMAND_RD_DATA",
        "DCM_INDIRECT_COMMAND_WR_DATA",
        "DCM_RESERVED_PCMI_3_S",
        "DCM_RESERVED_PCMI_3_T",
        "FMAC_INDIRECT_COMMAND",
        "FMAC_INDIRECT_COMMAND_ADDRESS",
        "FMAC_INDIRECT_COMMAND_RD_DATA",
        "FMAC_INDIRECT_COMMAND_WR_DATA",
        "FMAC_RESERVED_PCMI_0_S",
        "FMAC_RESERVED_PCMI_0_T",
        "FMAC_RESERVED_PCMI_1_S",
        "FMAC_RESERVED_PCMI_1_T",
        "FMAC_RESERVED_PCMI_2_S",
        "FMAC_RESERVED_PCMI_2_T",
        "FMAC_RESERVED_PCMI_3_S",
        "FMAC_RESERVED_PCMI_3_T",
        "FMAC_RESERVED_PCMI_4_S",
        "FMAC_RESERVED_PCMI_4_T",
        "FSRD_RESERVED_PCMI_3_S",
        "FSRD_RESERVED_PCMI_3_T",
        "MESH_TOPOLOGY_RESERVED_PCMI_0_S",
        "MESH_TOPOLOGY_RESERVED_PCMI_0_T",
        "MESH_TOPOLOGY_RESERVED_PCMI_1_S",
        "MESH_TOPOLOGY_RESERVED_PCMI_1_T",
        "MESH_TOPOLOGY_RESERVED_PCMI_2_S",
        "MESH_TOPOLOGY_RESERVED_PCMI_2_T",
        "MESH_TOPOLOGY_RESERVED_PCMI_3_S",
        "MESH_TOPOLOGY_RESERVED_PCMI_3_T",
        "MESH_TOPOLOGY_RESERVED_PCMI_4_S",
        "MESH_TOPOLOGY_RESERVED_PCMI_4_T",
        "OCCG_INDIRECT_COMMAND",
        "OCCG_INDIRECT_COMMAND_ADDRESS",
        "OCCG_INDIRECT_COMMAND_RD_DATA",
        "OCCG_INDIRECT_COMMAND_WR_DATA",
        "OCCG_RESERVED_PCMI_0_S",
        "OCCG_RESERVED_PCMI_0_T",
        "OCCG_RESERVED_PCMI_1_S",
        "OCCG_RESERVED_PCMI_1_T",
        "OCCG_RESERVED_PCMI_2_S",
        "OCCG_RESERVED_PCMI_2_T",
        "OCCG_RESERVED_PCMI_3_S",
        "OCCG_RESERVED_PCMI_3_T",
        "OCCG_RESERVED_PCMI_4_S",
        "OCCG_RESERVED_PCMI_4_T",
        "RTP_RESERVED_PCMI_3_S",
        "RTP_RESERVED_PCMI_3_T",
        "CMIC_BROADSYNC_REF_CLK_GEN_CTRL",
        "CMIC_TIMESYNC_BROADSYNC0_CLK_COUNT_CTRL",
        "CMIC_TIMESYNC_BROADSYNC1_CLK_COUNT_CTRL",
        "CMIC_TIMESYNC_CAPTURE_STATUS_1",
        "CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1",
        "CMIC_TIMESYNC_INTERRUPT_ENABLE",
        "CMIC_TIMESYNC_INTERRUPT_STATUS",
        "CMIC_TIMESYNC_INTERRUPT_CLR",
        "CMIC_TIMESYNC_TM",
        "CMIC_MMU_COSLC_COUNT_ADDR",
        "CMIC_MMU_COSLC_COUNT_DATA",
        "CMIC_CMC0_FSCHAN_DATA32",
        "CMIC_CMC0_FSCHAN_DATA64_LO",
        "CMIC_FSCHAN_DATA32",
        "CMIC_FSCHAN_DATA64_LO",
        "CMIC_CMC1_FSCHAN_DATA32",
        "CMIC_CMC1_FSCHAN_DATA64_LO",
        "CMIC_CMC2_FSCHAN_DATA32",
        "CMIC_CMC2_FSCHAN_DATA64_LO",
        "CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWER",
        "CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWER",
        "CMIC_TIMESYNC_COUNTER_CONFIG_SELECT",
        "CMIC_TIMESYNC_TS0_COUNTER_ENABLE",
        "CMIC_TIMESYNC_TS0_FREQ_CTRL_FRAC",
        "CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPER",
        "CMIC_TIMESYNC_TS1_COUNTER_ENABLE",
        "CMIC_TIMESYNC_TS1_FREQ_CTRL_FRAC",
        "CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPER",
        "CMIC_TIMESYNC_TIME_CAPTURE_MODE",
        "CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWER",
        "CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPER",
        "CMIC_TIMESYNC_FIFO_STATUS",
        "CMIC_TIMESYNC_GPIO_0_CTRL",
        "CMIC_TIMESYNC_GPIO_0_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_0_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_0_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_0_INPUT_DIVISOR",
        "CMIC_TIMESYNC_GPIO_1_CTRL",
        "CMIC_TIMESYNC_GPIO_1_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_1_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_1_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_1_INPUT_DIVISOR",
        "CMIC_TIMESYNC_GPIO_2_CTRL",
        "CMIC_TIMESYNC_GPIO_2_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_2_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_2_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_2_INPUT_DIVISOR",
        "CMIC_TIMESYNC_GPIO_3_CTRL",
        "CMIC_TIMESYNC_GPIO_3_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_3_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_3_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_3_INPUT_DIVISOR",
        "CMIC_TIMESYNC_GPIO_4_CTRL",
        "CMIC_TIMESYNC_GPIO_4_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_4_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_4_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_4_INPUT_DIVISOR",
        "CMIC_TIMESYNC_GPIO_5_CTRL",
        "CMIC_TIMESYNC_GPIO_5_OUTPUT_ENABLE",
        "CMIC_TIMESYNC_GPIO_5_UP_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_5_DOWN_EVENT_CTRL",
        "CMIC_TIMESYNC_GPIO_5_PHASE_ADJUST_LOWER",
        "CMIC_TIMESYNC_GPIO_5_PHASE_ADJUST_UPPER",
        "CMIC_TIMESYNC_GPIO_5_INPUT_DIVISOR",
        "CMIC_TIMESYNC_SYNCE_CLK1_COUNT_CTRL",
        "CMIC_TIMESYNC_SYNCE_CLK2_COUNT_CTRL",
        "CMIC_TIMESYNC_SYNCE_CLK3_COUNT_CTRL",
        "CMIC_TIMESYNC_SYNCE_CLK4_COUNT_CTRL",
        "CMIC_TIMESYNC_SYNCE_CLK5_COUNT_CTRL",
        "CMIC_TIMESYNC_TIME_CAPTURE_CONTROL",
    };
    int i;

    for (i=0; i<sizeof(regs)/sizeof(char *); i++) {
        if (sal_strcasecmp(name, regs[i])==0) {
            return 1;
        }
    }
  }
#endif /*BCM_88950_SUPPORT*/

  return 0;
}


/* 
 * Reads and displays all SOC registers specified by alist structure.
 */
STATIC int
dpp_reg_print_all(int unit, soc_regaddrlist_t *alist, uint32 flags)
{
    int                 j;
    soc_reg_above_64_val_t value; 
    int                 r, rv = 0, array_index, blk;
    uint32              val32 = 0, addr;
    uint8               acc_type;
    soc_regaddrinfo_t   *ainfo;
    
    if (alist == NULL) {
        return BCM_E_INTERNAL;
    }
    for (j = 0; j < alist->count; j++) {
        ainfo = &alist->ainfo[j];
        array_index = SOC_REG_IS_ARRAY(unit, ainfo->reg) ? ainfo->idx : 0;

        if(SOC_REG_IS_WRITE_ONLY(unit, ainfo->reg)) {
            continue;
        }

        if (ignore_reg_fe3200(unit, SOC_REG_NAME(unit, ainfo->reg))) {
            continue;
        }

        if(soc_cpureg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
            addr = soc_reg_addr_get(unit, ainfo->reg, ainfo->block, ainfo->idx, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            val32 = soc_pci_read(unit, addr);
            SOC_REG_ABOVE_64_CLEAR(value);
            value[0] = val32;
            dpp_reg_print(unit, ainfo, value, flags, ",", 62);
        }
#ifdef BCM_IPROC_SUPPORT
        else if (soc_iprocreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
            addr = soc_reg_addr_get(unit, ainfo->reg, ainfo->block, ainfo->idx, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            soc_iproc_getreg(unit, addr, &val32);
            SOC_REG_ABOVE_64_CLEAR(value);
            value[0] = val32;
            dpp_reg_print(unit, ainfo, value, flags, ",", 62);
        }
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
        else if (soc_mcsreg == SOC_REG_INFO(unit,ainfo->reg).regtype) {
            addr = soc_reg_addr_get(unit, ainfo->reg, ainfo->block, ainfo->idx, SOC_REG_ADDR_OPTION_NONE, &blk, &acc_type);
            val32 = soc_pci_mcs_read(unit, addr);
            if(flags&REG_PRINT_ADDR) {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%02X 0x%04X: 0x%08x\n",SOC_BLOCK_INFO(unit, ainfo->block).schan,addr,val32);  
            } else {
                bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "%s[0x%x] = 0x%x\n",SOC_REG_NAME (unit, ainfo->reg),addr,val32);
            }

        }
#endif
        else if (ainfo->is_custom) {
            r = soc_custom_reg_above_64_get(unit, ainfo->reg, ainfo->port, array_index, value);
            if (r < 0) {
                char            buf[80];
                soc_reg_sprint_addr(unit, buf, ainfo);
                cli_out("ERROR: read from register %s failed: %s\n", buf, soc_errmsg(r));
                rv = -1;
            }
            dpp_reg_print(unit, ainfo, value, flags, ",", 62);
        } else if ((r = soc_reg_above_64_get(unit, ainfo->reg, ainfo->port, array_index, value)) < 0) {
            char            buf[80];
            soc_reg_sprint_addr(unit, buf, ainfo);
            cli_out("ERROR: read from register %s failed: %s\n",
                    buf, soc_errmsg(r));
            rv = -1;
        } else {
            dpp_reg_print(unit, ainfo, value, flags, ",", 62);
        }
        /*bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "\n");*/

    }
    return rv;
}


regtype_entry_t *
dpp_regtype_lookup_name(char* str)
{
    int i;

    for (i = 0; i < regtypes_count; i++) {
        if (!sal_strcasecmp(str,dpp_regtypes[i].name)) {
            return &dpp_regtypes[i];
        }
    }

    return 0;
}


int 
filter_regs(int unit , soc_reg_t reg){
    
     /* CMIC regs filter */
    switch(reg) {
        /*Sarath Kumar Immadisetty : These registers are absolete.*/
        case CMIC_MMU_COSLC_COUNT_ADDRr          :
        case CMIC_MMU_COSLC_COUNT_DATAr          :

        /* Nir Shaknay 
         * These registers do not ack the CPU until the FSCHAN read cycle is completed.
         * Meaning you must activate a proper FSCHAN read command in order to read these registers.*/
        case CMIC_CMC0_FSCHAN_DATA32r            :
        case CMIC_CMC0_FSCHAN_DATA64_LOr         :
        case CMIC_FSCHAN_DATA32r                 :
        case CMIC_FSCHAN_DATA64_LOr              :
        case CMIC_CMC1_FSCHAN_DATA32r            :
        case CMIC_CMC1_FSCHAN_DATA64_LOr         :
        case CMIC_CMC2_FSCHAN_DATA32r            :
        case CMIC_CMC2_FSCHAN_DATA64_LOr         :
        case CMIC_TS_FREQ_CTRL_LOWERr            :
        case CMIC_TS_FREQ_CTRL_UPPERr            :
        case CMIC_BROADSYNC_REF_CLK_GEN_CTRLr    :
        case CMIC_TIMESYNC_BROADSYNC0_CLK_COUNT_CTRLr:
        case CMIC_TIMESYNC_BROADSYNC1_CLK_COUNT_CTRLr:
        case CMIC_TIMESYNC_CAPTURE_STATUS_1r     :
        case CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r :
        case CMIC_TIMESYNC_INTERRUPT_ENABLEr     :
        case CMIC_TIMESYNC_INTERRUPT_STATUSr     :
        case CMIC_TIMESYNC_INTERRUPT_CLRr        :
        case CMIC_TIMESYNC_TMr                   :
        case CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr  :
        case CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr  :
        case CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr:
        case CMIC_TIMESYNC_TS0_COUNTER_ENABLEr   :
        case CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr   :
        case CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr  :
        case CMIC_TIMESYNC_TS1_COUNTER_ENABLEr   :
        case CMIC_TIMESYNC_TS1_FREQ_CTRL_FRACr   :
        case CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr  :
        case CMIC_TIMESYNC_TIME_CAPTURE_MODEr    :
        case CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr:
        case CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr:
        case CMIC_TIMESYNC_FIFO_STATUSr          :
        case CMIC_TIMESYNC_GPIO_0_CTRLr          :
        case CMIC_TIMESYNC_GPIO_0_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_0_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_0_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_0_INPUT_DIVISORr :
        case CMIC_TIMESYNC_GPIO_1_CTRLr          :
        case CMIC_TIMESYNC_GPIO_1_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_1_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_1_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_1_INPUT_DIVISORr :
        case CMIC_TIMESYNC_GPIO_2_CTRLr          :
        case CMIC_TIMESYNC_GPIO_2_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_2_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_2_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_2_INPUT_DIVISORr :
        case CMIC_TIMESYNC_GPIO_3_CTRLr          :
        case CMIC_TIMESYNC_GPIO_3_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_3_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_3_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_3_INPUT_DIVISORr :
        case CMIC_TIMESYNC_GPIO_4_CTRLr          :
        case CMIC_TIMESYNC_GPIO_4_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_4_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_4_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_4_INPUT_DIVISORr :
        case CMIC_TIMESYNC_GPIO_5_CTRLr          :
        case CMIC_TIMESYNC_GPIO_5_OUTPUT_ENABLEr :
        case CMIC_TIMESYNC_GPIO_5_UP_EVENT_CTRLr :
        case CMIC_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr:
        case CMIC_TIMESYNC_GPIO_5_PHASE_ADJUST_LOWERr:
        case CMIC_TIMESYNC_GPIO_5_PHASE_ADJUST_UPPERr:
        case CMIC_TIMESYNC_GPIO_5_INPUT_DIVISORr :
        case CMIC_TIMESYNC_SYNCE_CLK1_COUNT_CTRLr:
        case CMIC_TIMESYNC_SYNCE_CLK2_COUNT_CTRLr:
        case CMIC_TIMESYNC_SYNCE_CLK3_COUNT_CTRLr:
        case CMIC_TIMESYNC_SYNCE_CLK4_COUNT_CTRLr:
        case CMIC_TIMESYNC_SYNCE_CLK5_COUNT_CTRLr:
        case CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr :
        case CMICTXCOSMASKr                      :
        case CMIC_BS_CLK_CTRLr                   :
        case CMIC_BS_CONFIGr                     :
        case CMIC_BS_HEARTBEAT_CTRLr             :
        case CMIC_BS_HEARTBEAT_DOWN_DURATIONr    :
        case CMIC_BS_HEARTBEAT_UP_DURATIONr      :
        case CMIC_BS_INITIAL_CRCr                :
        case CMIC_BS_INPUT_TIME_0r               :
        case CMIC_BS_INPUT_TIME_1r               :
        case CMIC_BS_INPUT_TIME_2r               :
        case CMIC_BS_OFFSET_ADJUST_0r            :
        case CMIC_BS_OFFSET_ADJUST_1r            :
        case CMIC_BS_OUTPUT_TIME_0r              :   
        case CMIC_BS_OUTPUT_TIME_1r              :   
        case CMIC_BS_OUTPUT_TIME_2r              :   
        case CMIC_BS_REF_CLK_GEN_CTRLr           :
        case CMIC_CMC0_FIFO_CH2_RD_DMA_HOSTMEM_START_ADDRESSr :
        case CMIC_TS_CAPTURE_STATUSr             :
        case CMIC_TS_CAPTURE_STATUS_CLRr         :
        case CMIC_TS_FIFO_STATUSr                :
        case CMIC_TS_INPUT_TIME_FIFO_IDr         :
        case CMIC_TS_INPUT_TIME_FIFO_TSr         :
        case CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr :
        case CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr :
        case CMIC_TS_LCPLL_CLK_COUNT_CTRLr       :
        case CMIC_TS_TIME_CAPTURE_CTRLr          :
        case CMIC_TS_GPIO_1_CTRLr                :
        case CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_1_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_1_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_1_UP_EVENT_CTRLr       :
        case CMIC_TS_GPIO_2_CTRLr                :
        case CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_2_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_2_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_2_UP_EVENT_CTRLr       :
        case CMIC_TS_GPIO_3_CTRLr                :
        case CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_3_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_3_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_3_UP_EVENT_CTRLr       :
        case CMIC_TS_GPIO_4_CTRLr                :
        case CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_4_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_4_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_4_UP_EVENT_CTRLr       :
        case CMIC_TS_GPIO_5_CTRLr                :
        case CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_5_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_5_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_5_UP_EVENT_CTRLr       :
        case CMIC_TS_GPIO_6_CTRLr                :
        case CMIC_TS_GPIO_6_DOWN_EVENT_CTRLr     :
        case CMIC_TS_GPIO_6_OUTPUT_ENABLEr       :
        case CMIC_TS_GPIO_6_PHASE_ADJUSTr        :
        case CMIC_TS_GPIO_6_UP_EVENT_CTRLr       :

            return 1;
    }


    if(SOC_IS_JERICHO_AND_BELOW(unit)) {
            /*
            * we skip on the following common regs after checking with noam halevi from the following reasons
            * 1. XXX_ECC_INTERRUPT_REGISTER_TEST because that the test mask is vary from block to block biut since the register
            * taken from common we cant know the actual size
            *
            * 2. XXX_INDIRECT_COMMAND   since INDIRECT_COMMAND_COUNT change by the HW we cant count on what we wrote
            *
            * 3. XXX_INDIRECT_COMMAND_WR_DATA since length vary from block to block and its width is like the widest mem in a block  
            *  we cant actually know how match bits is realy active for this register while its definition came from common
            *  and its 640 bits
 */
#ifdef BCM_PETRA_SUPPORT
        uint32 disallowed_fields[] = {INDIRECT_COMMAND_COUNTf,INDIRECT_COMMAND_WR_DATAf, INTERRUPT_REGISTER_TESTf,ECC_INTERRUPT_REGISTER_TESTf,NUM_SOC_FIELD};
        if (reg_contain_one_of_the_fields(unit,reg,disallowed_fields)) {
            return 1;
        }
#endif
        switch(reg) {
            /* these global registers vary from block to block, making the expected value wrong */
            /* Following regs are not included in all blocks */
            case NBIH_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case NBIL_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case NBIH_INDIRECT_COMMAND_ADDRESSr:
            case NBIL_INDIRECT_COMMAND_ADDRESSr:
            case NBIH_INDIRECT_COMMAND_DATA_INCREMENTr:
            case NBIL_INDIRECT_COMMAND_DATA_INCREMENTr:
            case NBIH_INDIRECT_COMMAND_WIDE_MEMr:
            case NBIL_INDIRECT_COMMAND_WIDE_MEMr:
            case NBIH_INDIRECT_FORCE_BUBBLEr:
            case NBIL_INDIRECT_FORCE_BUBBLEr:

            /* Unknown errors */
            case CLPORT_SGNDET_EARLYCRSr:
            case EDB_PAR_ERR_INITIATEr:
            case MACSEC_PROG_TX_CRCr:
            case MAC_PFC_CTRLr:
            case MAC_PFC_REFRESH_CTRLr:
            case SFD_OFFSETr:

            /* additional regs r/w test at tr 3 failed on emulation*/
            case IHB_INTERRUPT_MASK_REGISTERr:
            case IHP_VTT_PROGRAM_ENCOUNTEREDr:
            case ILKN_PMH_ECC_INTERRUPT_REGISTER_TESTr:
            case ILKN_PMH_INDIRECT_COMMAND_WR_DATAr:
            case ILKN_PMH_INDIRECT_COMMANDr:
            case ILKN_PMH_INTERRUPT_REGISTER_TESTr:
            case ILKN_PML_ECC_INTERRUPT_REGISTER_TESTr:
            case ILKN_PML_INDIRECT_COMMAND_WR_DATAr:
            case ILKN_PML_INDIRECT_COMMANDr:
            case ILKN_PML_INTERRUPT_REGISTER_TESTr:

            case CFC_INTERRUPT_MASK_REGISTERr: /*tr3 emulation  reg=CFC_INTERRUPT_MASK_REGISTERr,value=0x55501,expected0x55555*/ 

            case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
            case MESH_TOPOLOGY_RESERVED_MTCPr:


            /*Dana Ziman (Block Owner) : "The NBIL/NBIH in Jericho has no indirect access."  */
            case NBIH_INDIRECT_COMMAND_RD_DATAr:
            case NBIL_INDIRECT_COMMAND_RD_DATAr:                                                

            /*Keren Yaar(Block Owner) : "Should not be readable"*/

                return 1;
            }
        }
    return 0;
}

int 
dpp_all_reg_get(int unit, int is_debug)
{
    int rc, blk, port,j;
#ifdef BCM_PETRA_SUPPORT
	int is_master, master_port;
#endif
    soc_reg_t reg;
    soc_regaddrlist_t alist;
    int count = 0;
    uint32 flags = REG_PRINT_RAW; /*all regs should be printed in raw mode*/
    uint32 phy_port;

    if(is_debug) {
        flags |= REG_PRINT_ADDR;
    }
    bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "");

    /*count relevant regs*/
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }

        if(filter_regs(unit , reg)){
            continue;
        }

        if(soc_genreg == SOC_REG_INFO(unit,reg).regtype) {
            SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit,reg).block) {         
               count += SOC_REG_INFO(unit,reg).numels;
            }
        }

        if(soc_portreg == SOC_REG_INFO(unit,reg).regtype) {
            PBMP_ITER(PBMP_PORT_ALL(unit), port){
#ifdef BCM_PETRA_SUPPORT
				if (SOC_IS_ARAD(unit) && !IS_SFI_PORT(unit, port))
				{
					rc = soc_port_sw_db_master_channel_get(unit, port, &master_port);
					if (rc < 0)
					{
                        return CMD_FAIL;
					}
					is_master = (port == master_port? 1:0);
					if (!is_master) {
						continue;
					} 
				}
#endif
                if(SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_PORT_TYPE(unit, port))) {
                    count += SOC_REG_INFO(unit,reg).numels;
                }
            }
        }

        if(soc_cpureg == SOC_REG_INFO(unit,reg).regtype) {
            count += SOC_REG_INFO(unit,reg).numels;
        }

        if (soc_customreg == SOC_REG_INFO(unit,reg).regtype) {
            PBMP_ITER(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port){
                count += SOC_REG_INFO(unit,reg).numels;
            }
        }
    }

    bsl_log_add(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "Collected %d registers\n",count);

    /*allocate alist*/
    alist.count = 0;
    alist.ainfo = sal_alloc(sizeof(soc_regaddrinfo_t)*count, "alist.ainfo");
    if(NULL == alist.ainfo) {
        cli_out("Failed to allocate alist.ainfo\n");
        return CMD_FAIL;
    }

    /*collect data*/
    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }

        if(filter_regs(unit , reg)){
            continue;
        }

        if(soc_genreg == SOC_REG_INFO(unit,reg).regtype || soc_cpureg == SOC_REG_INFO(unit,reg).regtype) {
            SOC_BLOCKS_ITER(unit, blk, SOC_REG_INFO(unit,reg).block) {  
                if(SOC_REG_INFO(unit,reg).numels > 0) {
                    for(j=0 ;j<SOC_REG_INFO(unit,reg).numels ; j++) {
                        alist.ainfo[alist.count].addr = SOC_REG_INFO(unit,reg).offset;
                        alist.ainfo[alist.count].valid = 1;
                        alist.ainfo[alist.count].reg = reg;
                        alist.ainfo[alist.count].idx = j;
                        alist.ainfo[alist.count].block = blk;
                        alist.ainfo[alist.count].port = SOC_BLOCK_PORT(unit, blk);
                        alist.ainfo[alist.count].cos = -1;
                        alist.ainfo[alist.count].field = INVALIDf;
			alist.ainfo[alist.count].is_custom = 0;
                        alist.count++;
                    }
                } else {
                    alist.ainfo[alist.count].addr = SOC_REG_INFO(unit,reg).offset;
                    alist.ainfo[alist.count].valid = 1;
                    alist.ainfo[alist.count].reg = reg;
                    alist.ainfo[alist.count].idx = 0;
                    alist.ainfo[alist.count].block = blk;
                    alist.ainfo[alist.count].port = SOC_BLOCK_PORT(unit, blk);
                    alist.ainfo[alist.count].cos = -1;
                    alist.ainfo[alist.count].field = INVALIDf;
	            alist.ainfo[alist.count].is_custom = 0;	
                    alist.count++;
                }
            }
        }

        if(soc_portreg == SOC_REG_INFO(unit,reg).regtype) {
            PBMP_ITER(PBMP_PORT_ALL(unit), port){
#ifdef BCM_PETRA_SUPPORT
                if (SOC_IS_ARAD(unit) && !IS_SFI_PORT(unit, port))
                {
                    rc = soc_port_sw_db_master_channel_get(unit, port, &master_port);
                    if (rc < 0)
                    {
                        goto exit;
                    }
                    is_master = (port == master_port? 1:0);
                    if (!is_master) {
                        continue;
                    }
                }
#endif
                if(SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_PORT_TYPE(unit, port))) {
                    alist.ainfo[alist.count].addr = SOC_REG_INFO(unit,reg).offset;
                    alist.ainfo[alist.count].valid = 1;
                    alist.ainfo[alist.count].reg = reg;
                    alist.ainfo[alist.count].idx = 0;
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    alist.ainfo[alist.count].block = SOC_PORT_BLOCK(unit, phy_port);
                    alist.ainfo[alist.count].port = port;
                    alist.ainfo[alist.count].cos = -1;
                    alist.ainfo[alist.count].field = INVALIDf;
                    alist.ainfo[alist.count].is_custom = 0;
                    alist.count++;
                }
            }
        }

        if (soc_customreg == SOC_REG_INFO(unit,reg).regtype) {
            PBMP_ITER(SOC_INFO(unit).custom_reg_access.custom_port_pbmp, port){
                    int block = 0;

#ifdef BCM_PETRA_SUPPORT
                    uint32 offset = 0;
                    rc = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
                    if (rc < 0) {
                        goto exit;
                    }

                    block = ((offset < 2) ? ILKN_PMH_BLOCK(unit) : 
                        (offset < 4 ? ILKN_PML_BLOCK(unit, 0) : ILKN_PML_BLOCK(unit, 1)));
#endif

                    alist.ainfo[alist.count].addr = SOC_REG_INFO(unit,reg).offset;
                    alist.ainfo[alist.count].valid = 1;
                    alist.ainfo[alist.count].reg = reg;
                    alist.ainfo[alist.count].idx = -1;
                    alist.ainfo[alist.count].block = block;
                    alist.ainfo[alist.count].port = port;
                    alist.ainfo[alist.count].cos = -1;
                    alist.ainfo[alist.count].field = INVALIDf;
                    alist.ainfo[alist.count].is_custom = 1;
                    alist.count++;
            }
        }
    }

    /*print*/
    rc = dpp_reg_print_all(unit, &alist, flags);
    bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "");

#ifdef BCM_PETRA_SUPPORT
exit:
#endif
    sal_free(alist.ainfo);
    if(rc<0) {
        return CMD_FAIL;
    }

    return CMD_OK;
}


#if defined(BCM_88270_A0)  
/* Pedantic compiler allows only 509 chars*/
char cmd_dpp_pem_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    " 1. pem get blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row>\n\t"
    " 2. pem set blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row> data1...data10\n" ;
#else
    " 1. pem get blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row>\n\t"
    " 2. pem set blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row> data1...data10\n" ;
#endif


/* 
 * Gets parameters to carry out debug operations on PEM memory.
 * Syntax:
 *   pem get blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row>
 *
 *   pem set blk <blk number> addr <offset within block> row <row beyond address> num_bits <number of bits in one row> data1...data10
 * 
 * See:
 *   cmd_dpp_pem_usage
 */
cmd_result_t
cmd_dpp_pem(int unit, args_t *a)
{
  int ret, num_longs, ii, do_get ;
  char *name ;
  char *proc_name ;
  phy_mem_t mem ;
  unsigned long data[10] ;

  proc_name = "cmd_dpp_pem" ;
  ret = CMD_OK ;
  do_get = 0 ;
  if (0 == sh_check_attached(ARG_CMD(a), unit))
  {
    return CMD_FAIL ;
  }
  bsl_log_start(BSL_APPL_SHELL, bslSeverityNormal, unit, "") ;
  name = ARG_GET(a) ;
  if (!sal_strcasecmp(name,"get"))
  {
    /* 
     * Enter if first arg is 'get'
     */
    do_get = 1 ;
  }
  else if (sal_strcasecmp(name,"set"))
  {
    /* 
     * Enter if first arg is neither 'get' nor 'set
     */
    cli_out("%s(): Second param MUST be 'blk'. (Detected '%s')\r\n",proc_name,name) ;
    ret = CMD_USAGE ;
    goto exit ;
  }
  /*
   * At this point, 'do_get' is either non-zero (get operation) or zero (set operation)
   */
  {
    name = ARG_GET(a) ;
    /*
     * The next parameter MUST be 'blk'.
     */
    if (sal_strcasecmp(name,"blk"))
    {
      cli_out("%s(): Next param MUST be 'blk'. (Detected '%s')\r\n",proc_name,name) ;
      ret = CMD_USAGE ;
      goto exit ;
    }
    /*
     * Now expect numerical block identifier.
     */
    name = ARG_GET(a) ;
    if (isint(name))
    {
      mem.block_identifier = parse_integer(name) ;
    }
    else
    {
      cli_out("%s(): Bad format for block identifier\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
    /*
     * The next parameter MUST be 'addr'.
     */
    name = ARG_GET(a) ;
    if (sal_strcasecmp(name,"addr"))
    {
      cli_out("%s(): Next param MUST be 'addr'. (Detected '%s')\r\n",proc_name,name) ;
      ret = CMD_USAGE ;
      goto exit ;
    }
    /*
     * Now expect numerical value of 'address'.
     */
    name = ARG_GET(a) ;
    if (isint(name))
    {
      mem.mem_address = parse_integer(name) ;
    }
    else
    {
      cli_out("%s(): Bad format for 'address' within block\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
    /*
     * The next parameter MUST be 'row'.
     */
    name = ARG_GET(a) ;
    if (sal_strcasecmp(name,"row"))
    {
      cli_out("%s(): Next param MUST be 'row'. (Detected '%s')\r\n",proc_name,name) ;
      ret = CMD_USAGE ;
      goto exit ;
    }
    /*
     * Now expect numerical value of 'row'.
     */
    name = ARG_GET(a) ;
    if (isint(name))
    {
      mem.row_index = parse_integer(name) ;
    }
    else
    {
      cli_out("%s(): Bad format for row offset from 'address'\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
    /*
     * The next parameter MUST be 'num_bits' (in one row).
     */
    name = ARG_GET(a) ;
    if (sal_strcasecmp(name,"num_bits"))
    {
      cli_out("%s(): Next param MUST be 'num_bits'. (Detected '%s')\r\n",proc_name,name) ;
      ret = CMD_USAGE ;
      goto exit ;
    }
    /*
     * Now expect numerical value of 'num_bits'.
     */
    name = ARG_GET(a) ;
    if (isint(name))
    {
      mem.mem_width_in_bits = parse_integer(name) ;
    }
    else
    {
      cli_out("%s(): Bad format for 'num_bits' in one row\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
  }
  if (do_get)
  {
    /*
     * Get operation. No more parameters are required. Display data read from target.
     */
    ret = phy_mem_read(unit, &mem, data) ;
    if (ret != SOC_E_NONE)
    {
      cli_out("%s(): Fail reported by phy_mem_read()\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
    num_longs = BITS2WORDS(mem.mem_width_in_bits) ;
    cli_out("%s(): Contents of %d words:\r\n",proc_name,num_longs) ;
    for (ii = 0 ; ii < num_longs ; ii++)
    {
      cli_out("0x%08lX ",data[ii]) ;
    }
    cli_out("\r\n") ;
  }
  else
  {
    /*
     * Set operation. Data parameters are required.
     */
    num_longs = BITS2WORDS(mem.mem_width_in_bits) ;
    cli_out("%s(): Expecting %d words  --  ",proc_name,num_longs) ;

    for (ii = 0 ; ii < num_longs ; ii++)
    {
      name = ARG_GET(a) ;
      if (isint(name))
      {
        data[ii] = parse_integer(name) ;
        cli_out("%d  ",ii + 1) ;
      }
      else
      {
        cli_out("\r\n") ;
        cli_out("%s(): Bad format for 'data' on word no. %d\r\n",proc_name,ii) ;
        ret = CMD_FAIL ;
        goto exit ;
      }
    }
    cli_out("\r\n") ;
    ret = phy_mem_write(unit, &mem, data) ;
    if (ret != SOC_E_NONE)
    {
      cli_out("%s(): Fail reported by phy_mem_write()\r\n",proc_name) ;
      ret = CMD_FAIL ;
      goto exit ;
    }
    else
    {
      cli_out("%s(): Pem was successfully updated\r\n",proc_name) ;
    }
    cli_out("\r\n") ;
  }
exit:
  bsl_log_end(BSL_APPL_SHELL, bslSeverityNormal, bslSinkIgnore, unit, "") ;
  return (ret) ;
}
#endif /* #if defined(BCM_88470_A0) */


