/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag register commands
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/counter.h>
#include <sal/appl/pci.h>
#include <soc/debug.h>
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif

#include <appl/diag/system.h>
#include <appl/diag/sysconf.h>
#include <ibde.h>

/* 
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */

static void
collect_comma_args(args_t *a, char *valstr, char *first)
{
    char           *s;

    sal_strcpy(valstr, first);

    while ((s = ARG_GET(a)) != 0) {
	sal_strcat(valstr, ",");
	sal_strcat(valstr, s);
    }
}

/* 
 * Utility routine to determine whether register type requires
 * block and access type information.
 */

static int 
reg_type_is_schan_reg(int reg_type)
{
    switch (reg_type) {
    case soc_schan_reg:
    case soc_genreg:
    case soc_portreg:
    case soc_ppportreg:
    case soc_cosreg:
        return 1;
    default:
        break;
    }
    return 0;
}

/* 
 * modify_reg_fields
 *
 *   Takes a soc_reg_t 'regno', pointer to current value 'val',
 *   and a string 'mod' containing a field replacement spec of the form
 *   "FIELD=value".   The string may contain more than one spec separated
 *   by commas.  Updates the field in the register value accordingly,
 *   leaving all other unmodified fields intact.  If the string CLEAR is
 *   present in the list, the current register value is zeroed.
 *   If mask is non-NULL, it receives a mask of all fields modified.
 *
 *   Examples with modreg:
 *        modreg fe_mac1 lback=1        (modifies only lback field)
 *        modreg config ip_cfg=2        (modifies only ip_cfg field)
 *        modreg config clear,ip_cfg=2,cpu_pri=4  (zeroes all other fields)
 *
 *   Note that if "clear" appears in the middle of the list, the
 *   values in the list before "clear" are ignored.
 *
 *   Returns CMD_FAIL on failure, CMD_OK on success.
 */

static int
modify_reg_fields(int unit, soc_reg_t regno,
		  uint64 *val, uint64 *mask /* out param */, char *mod)
{
    soc_field_info_t *fld;
    char           *fmod, *fval;
    char           *modstr;
    char           *tokstr;
    soc_reg_info_t *reg = &SOC_REG_INFO(unit, regno);
    uint64          fvalue;
    uint64          fldmask;
    uint64          tmask;

    if ((modstr = sal_alloc(ARGS_BUFFER, "modify_reg")) == NULL) {
        cli_out("modify_reg_fields: Out of memory\n");
        return CMD_FAIL;
    }

    sal_strlcpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;
    mod = modstr;

    if (mask) {
	COMPILER_64_ZERO(*mask);
    }

    while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0) {
	mod = NULL;		       /* Pass strtok NULL next time */
	fval = sal_strchr(fmod, '=');
	if (fval) {		       /* Point fval to arg, NULL if none */
	    *fval++ = 0;	       /* Now fmod holds only field name. */
	}
	if (fmod[0] == 0) {
	    cli_out("Null field name\n");
            sal_free(modstr);
	    return CMD_FAIL;
	}
	if (!sal_strcasecmp(fmod, "clear")) {
	    COMPILER_64_ZERO(*val);
	    if (mask) {
		COMPILER_64_ALLONES(*mask);
	    }
	    continue;
	}
	for (fld = &reg->fields[0]; fld < &reg->fields[reg->nFields]; fld++) {
	    if (!sal_strcasecmp(fmod, SOC_FIELD_NAME(unit, fld->field))) {
		break;
	    }
	}
	if (fld == &reg->fields[reg->nFields]) {
	    cli_out("No such field \"%s\" in register \"%s\".\n",
                    fmod, SOC_REG_NAME(unit, regno));
            sal_free(modstr);
	    return CMD_FAIL;
	}
	if (!fval) {
	    cli_out("Missing %d-bit value to assign to \"%s\" "
                    "field \"%s\".\n",
                    fld->len, SOC_REG_NAME(unit, regno), SOC_FIELD_NAME(unit, fld->field));
            sal_free(modstr);
	    return CMD_FAIL;
	}
	fvalue = parse_uint64(fval);

	/* Check that field value fits in field */
	COMPILER_64_MASK_CREATE(tmask, fld->len, 0);
	COMPILER_64_NOT(tmask);
	COMPILER_64_AND(tmask, fvalue);

	if (!COMPILER_64_IS_ZERO(tmask)) {
	    cli_out("Value \"%s\" too large for %d-bit field \"%s\".\n",
                    fval, fld->len, SOC_FIELD_NAME(unit, fld->field));
            sal_free(modstr);
	    return CMD_FAIL;
	}

	if (reg->flags & SOC_REG_FLAG_64_BITS) {
	    soc_reg64_field_set(unit, regno, val, fld->field, fvalue);
	} else {
	    uint32          tmp;
            uint32 ftmp;
	    COMPILER_64_TO_32_LO(tmp, *val);
	    COMPILER_64_TO_32_LO(ftmp, fvalue);
	    soc_reg_field_set(unit, regno, &tmp, fld->field, ftmp);
	    COMPILER_64_SET(*val, 0, tmp);
	    COMPILER_64_SET(fvalue, 0, ftmp);
	}

	COMPILER_64_MASK_CREATE(fldmask, fld->len, fld->bp);
	if (mask) {
	    COMPILER_64_OR(*mask, fldmask);
	}
    }

    sal_free(modstr);
    return CMD_OK;
}

#define PRINT_COUNT(str, len, wrap, prefix) \
    if ((wrap > 0) && (len > wrap)) { \
        cli_out("\n%s", prefix); \
        len = sal_strlen(prefix); \
    } \
    cli_out("%s", str); \
    len += sal_strlen(str)


/* 
 * Print a SOC internal register with fields broken out.
 */
void
reg_print(int unit, soc_regaddrinfo_t *ainfo, uint64 val, uint32 flags,
	  char *fld_sep, int wrap, char *field_names)
{
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, ainfo->reg);
    int             f;
    uint64          val64, resval, resfld;
    char            buf[80];
    char            line_buf[256];
    int		    linelen = 0;
    int		    nprint;
    int32           addr32 = ainfo->addr;
    char            temp_field_name[80];

    if (flags & REG_PRINT_HEX) {
	if (SOC_REG_IS_64(unit, ainfo->reg)) {
	    cli_out("%08x%08x\n",
                    COMPILER_64_HI(val),
                    COMPILER_64_LO(val));
	} else {
	    cli_out("%08x\n",
                    COMPILER_64_LO(val));
	}
	return;
    }

    if (flags & REG_PRINT_CHG) {
	SOC_REG_RST_VAL_GET(unit, ainfo->reg, resval);
	if (COMPILER_64_EQ(val, resval)) {	/* no changed fields */
	    return;
	}
    } else {
	COMPILER_64_ZERO(resval);
    }

    soc_reg_sprint_addr(unit, buf, ainfo);

    if (soc_feature(unit, soc_feature_new_sbus_format)) {
        sal_sprintf(line_buf, "%s[%d][0x%x]=", buf, 
                    SOC_BLOCK_INFO(unit, ainfo->block).cmic,
                    addr32);
    } else {
        sal_sprintf(line_buf, "%s[0x%x]=", buf, addr32);
    }
    PRINT_COUNT(line_buf, linelen, wrap, "   ");

    format_uint64(line_buf, val);
    PRINT_COUNT(line_buf, linelen, -1, "");

    if (flags & REG_PRINT_RAW) {
	cli_out("\n");
	return;
    }

    PRINT_COUNT(": <", linelen, wrap, "   ");

    nprint = 0;
    for (f = reginfo->nFields - 1; f >= 0; f--) {
	soc_field_info_t *fld = &reginfo->fields[f];
	val64 = soc_reg64_field_get(unit, ainfo->reg, val, fld->field);
        if (field_names != NULL) {
            sal_strlcpy(temp_field_name, ",", sizeof(temp_field_name));
            sal_strncat(temp_field_name, SOC_FIELD_NAME(unit, fld->field),
                        sizeof(temp_field_name) -
                        sal_strlen(temp_field_name) - 1);
            sal_strncat(temp_field_name, ",", 
                        sizeof(temp_field_name) -
                        sal_strlen(temp_field_name) - 1);
            if (sal_strstr(field_names, temp_field_name) == NULL) {
                continue;
            }
        }
	if (flags & REG_PRINT_CHG) {
	    resfld = soc_reg64_field_get(unit, ainfo->reg, resval, fld->field);
	    if (COMPILER_64_EQ(val64, resfld)) {
		continue;
	    }
	}

	if (nprint > 0) {
	    sal_sprintf(line_buf, "%s", fld_sep);
            PRINT_COUNT(line_buf, linelen, -1, "");
	}
	sal_sprintf(line_buf, "%s=", SOC_FIELD_NAME(unit, fld->field));

        PRINT_COUNT(line_buf, linelen, wrap, "   ");
	format_uint64(line_buf, val64);

        PRINT_COUNT(line_buf, linelen, -1, "");
	nprint += 1;
    }

    cli_out(">\n");
}

 /* 
  * modify_reg_above_64_fields
  *     
  * Exclusive function for modifying register having width >64b  
  * 
  * Returns -1 on failure, 0 on success
 */
 static int
 modify_reg_above_64_fields(int unit, 
                            soc_reg_t regno,
                            soc_reg_above_64_val_t val, 
                            soc_reg_above_64_val_t *mask,
                            char *mod)
 {
     soc_field_info_t *fld;
     char           *fmod, *fval;
     char           *modstr;
     soc_reg_info_t *reg = &SOC_REG_INFO(unit, regno);
     soc_reg_above_64_val_t  fvalue, tmask, fldmask;
     char *tokstr;
 
     if ((modstr = sal_alloc(ARGS_BUFFER, "modify_reg")) == NULL) {
         cli_out( "modify_reg_fields: Out of memory\n");
         return CMD_FAIL;
     }
 
     sal_strlcpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
     modstr[ARGS_BUFFER - 1] = 0;
     mod = modstr;
     if (mask) {
         SOC_REG_ABOVE_64_CLEAR(*mask);
     }
 
     while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0) {
         mod = NULL;            /* Pass strtok NULL next time */
         fval = strchr(fmod, '=');
         if (fval) {            /* Point fval to arg, NULL if none */
             *fval++ = 0;       /* Now fmod holds only field name. */
         }
         if (fmod[0] == 0) {
             cli_out( "Null field name\n");
             sal_free(modstr);
             return -1;
         }
         if (!sal_strcasecmp(fmod, "clear")) {
             SOC_REG_ABOVE_64_CLEAR(val);
             if (mask) {
                 SOC_REG_ABOVE_64_ALLONES(*mask);
             }
             continue;
         }
         for (fld = &reg->fields[0]; fld < &reg->fields[reg->nFields]; fld++) {
             if (!sal_strcasecmp(fmod, SOC_FIELD_NAME(unit, fld->field))) {
                 break;
             }
         }
         if (fld == &reg->fields[reg->nFields]) {
             cli_out( "No such field \"%s\" in register \"%s\".\n",
                    fmod, SOC_REG_NAME(unit, regno));
             sal_free(modstr);
             return -1;
         }
         if (!fval) {
             cli_out( "Missing %d-bit value to assign to \"%s\" field \"%s\".\n",
                    fld->len, SOC_REG_NAME(unit, regno),
                    SOC_FIELD_NAME(unit, fld->field));
             sal_free(modstr);
             return -1;
         }
         parse_long_integer(fvalue, SOC_REG_ABOVE_64_MAX_SIZE_U32, fval);
         /* Check that field value fits in field */
         SOC_REG_ABOVE_64_CREATE_MASK(tmask, fld->len, 0)
         SOC_REG_ABOVE_64_NOT(tmask)
         SOC_REG_ABOVE_64_AND(tmask, fvalue)
 
         if (!SOC_REG_ABOVE_64_IS_ZERO(tmask)) {
             cli_out( "Value \"%s\" too large for %d-bit field \"%s\".\n",
                    fval, fld->len, SOC_FIELD_NAME(unit, fld->field));
             sal_free(modstr);
             return -1;
         }
 
 
        soc_reg_above_64_field_set(unit, regno, val, fld->field, fvalue);
 
 
         SOC_REG_ABOVE_64_CREATE_MASK(fldmask, fld->len, fld->bp);
         if (mask) {
             SOC_REG_ABOVE_64_OR(*mask, fldmask);
         }
     }
     sal_free(modstr);
     return 0;
 }
 
 /* 
  * reg_above_64_print
  *
  * Print a SOC internal register with fields broken out.
  * This is a exclusive function for printing registers having width >64b  
 */
 void 
 reg_above_64_print(int unit, 
                    soc_regaddrinfo_t *ainfo, 
                    soc_reg_above_64_val_t val, 
                    uint32 flags,
                    char *fld_sep, 
                    int wrap)
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
         cli_out( "%s\n", line_buf);
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
         cli_out( "0x%02X 0x%04X: %s\n",SOC_BLOCK_INFO(unit, ainfo->block).schan,SOC_REG_INFO(unit,ainfo->reg).offset + skip, line_buf);
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
         cli_out( "\n");
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
         /*cli_out( "\n");*/
 
     }
 
     cli_out( ">\n");
 }
 
 
 

/* 
 * Reads and displays all SOC registers specified by alist structure.
 */
int
reg_print_all(int unit, soc_regaddrlist_t *alist, uint32 flags, 
              char *field_names)
{
    int             j;
    uint64          value;
    int             r, rv = 0;
    soc_regaddrinfo_t *ainfo;
    soc_reg_above_64_val_t value_above_64;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
#endif
    assert(alist);

    for (j = 0; j < alist->count; j++) {
	    ainfo = &alist->ainfo[j];

#ifdef CANCUN_SUPPORT
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit) &&
            SOC_REG_IS_CCH(unit, ainfo->reg) && !(soc_property_get(unit,
                    "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE) &&
                    SOC_BLOCK_IS(SOC_REG_INFO(unit, ainfo->reg).block,
                    SOC_BLK_CCH)) {
            if (SOC_REG_ARRAY(unit, ainfo->reg)) {
                if (alist->count > 1) {
                    /* Reg array access */
                    ainfo->reg += (j+1);
                } else if (alist->count == 1) {
                    /* Reg array single entry access */
                    ainfo->reg += ainfo->idx+1;
                }
            }
        }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* CANCUN_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
        /* FOR 56982 SKU, some of the PMs are not active in pipes.
           The below code determines which PM does it belong to
           based on the pipe_stage numbering of the IDB_CA/OBM
           registers
           Also, we dump all the valid PM instances in each pipe
           for the register when we try to dump the register
           without the pipe info in it. (e.g IDB_CA2_BUFFER_CONFIG)
        */
        soc_cm_get_id(unit, &dev_id, &rev_id);

        if (dev_id == BCM56982_DEVICE_ID) {

            soc_reg_t reg;
            soc_reg_info_t *regp;
            reg = ainfo->reg;
            regp = &(SOC_REG_INFO(unit, reg));
            if (regp->pipe_stage && *(regp->block) == SOC_BLK_EPIPE) {
                int acc_type;
                soc_info_t *si;
                int pipe_stage = 0;
                int pm_num = 0, pm = 0;
                pipe_stage = regp->pipe_stage;
                acc_type = SOC_REG_ACC_TYPE(unit, reg);
                si = &SOC_INFO(unit);
                if (pipe_stage > 14 && pipe_stage < 23) {
                    if (acc_type == 9) {
                        int pipe = 0;
                        soc_regaddrinfo_t temp_ainfo;
                        soc_reg_t base_reg;
                        base_reg = ainfo->reg;
                        sal_memcpy(&temp_ainfo, ainfo,
                                   sizeof(soc_regaddrinfo_t));

                        if (pipe_stage > 14 && pipe_stage < 19) {
                            pm_num = pipe_stage - 15;
                        }

                        if (pipe_stage > 18 && pipe_stage < 23) {
                            pm_num = pipe_stage - 19;
                        }

                        for (pipe = 0; pipe < 8; pipe++) {
                            pm = pm_num + (pipe * 4);
                            temp_ainfo.reg = ainfo->reg + 1 + pipe;
                            /* Check if the PM is active and only the
                               instance of the base register is being read
                             */
                            if ((si->active_pm_map[pm] == 1)
                                    && ((temp_ainfo.reg - base_reg) <= 8)){
                                if (SOC_REG_IS_ABOVE_64(unit, temp_ainfo.reg)) {
                                        SOC_IF_ERROR_RETURN(soc_reg_above_64_get(
                                                unit, temp_ainfo.reg,
                                                temp_ainfo.port,
                                                SOC_REG_IS_ARRAY(unit,
                                                temp_ainfo.reg) ?
                                                temp_ainfo.idx : 0,
                                                value_above_64));
                                        reg_above_64_print(unit, &temp_ainfo,
                                                           value_above_64,
                                                           flags, ",", 62 );
                                   continue;
                                }
                                if ((r = soc_anyreg_read(unit, &temp_ainfo,
                                                        &value)) < 0) {
                                    char            buf[80];
                                    soc_reg_sprint_addr(unit, buf, &temp_ainfo);
                                    cli_out("ERROR: read from "
                                            "register %s failed: %s\n",
                                            buf, soc_errmsg(r));
                                    return -1;
                                } else {
                                    reg_print(unit, &temp_ainfo, value,
                                              flags, ",", 62, field_names);
                                }
                            }
                        }
                        continue;
                    }
                    if (acc_type >=0 && acc_type < 8) {
                        if (pipe_stage > 14 && pipe_stage < 19) {
                            pm_num = pipe_stage - 15;
                        }

                        if (pipe_stage > 18 && pipe_stage < 23) {
                            pm_num = pipe_stage - 19;
                        }
                        pm = pm_num + (acc_type * 4);
                    }

                    if (si->active_pm_map[pm] != 1) {
                        char reg_name[80];
                        sal_sprintf(reg_name, "%s",
                                    SOC_REG_NAME(unit, ainfo->reg));
	                    cli_out("Register %s does not exist in TH3 56982 SKU\n",
                                    reg_name);
                        continue;
                    }
                }
            }
        }
#endif
        if (SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
            SOC_IF_ERROR_RETURN(soc_reg_above_64_get(
                                unit, ainfo->reg, ainfo->port, 
                                SOC_REG_IS_ARRAY(unit, ainfo->reg) ? 
                                ainfo->idx : 0, value_above_64));
            reg_above_64_print(unit, ainfo, value_above_64, flags, ",", 62 );
            continue;
        }
	if ((r = soc_anyreg_read(unit, ainfo, &value)) < 0) {
	    char            buf[80];
	    soc_reg_sprint_addr(unit, buf, ainfo);
	    cli_out("ERROR: read from register %s failed: %s\n",
                    buf, soc_errmsg(r));
	    rv = -1;
	} else {
	    reg_print(unit, ainfo, value, flags, ",", 62, field_names);
	}
    }

    return rv;
}

/*
 * Register Types - for getreg and dump commands
 */

static regtype_entry_t regtypes[] = {
 { "PCIC",	soc_pci_cfg_reg,"PCI Configuration space" },
 { "PCIM",	soc_cpureg,	"PCI Memory space (CMIC)" },
 { "MCS",	soc_mcsreg,	"MicroController Subsystem" },
 { "IPROC",     soc_iprocreg,   "iProc Subsystem" },
 { "SOC",	soc_schan_reg,	"SOC internal registers" },
 { "SCHAN",	soc_schan_reg,	"SOC internal registers" },
 { "PHY",	soc_phy_reg,	"PHY registers via MII (phyID<<8|phyADDR)" },
 { "MW",	soc_hostmem_w,	"Host Memory 32-bit" },
 { "MH",	soc_hostmem_h,	"Host Memory 16-bit" },
 { "MB",	soc_hostmem_b,	"Host Memory 8-bit" },
 { "MEM",	soc_hostmem_w,	"Host Memory 32-bit" },	/* Backward compat */
};

#define regtypes_count	COUNTOF(regtypes)

regtype_entry_t *regtype_lookup_name(char* str)
{
    int i;

    for (i = 0; i < regtypes_count; i++) {
	if (!sal_strcasecmp(str,regtypes[i].name)) {
	    return &regtypes[i];
        }
    }

    return 0;
}

void regtype_print_all(void)
{
    int i;

    cli_out("Register types supported by setreg, getreg, and dump:\n");

    for (i = 0; i < regtypes_count; i++)
	cli_out("\t%-10s -%s\n", regtypes[i].name, regtypes[i].help);
}

/* 
 * Get a register by type.
 *
 * doprint:  Boolean.  If set, display data.
 */
int
_reg_get_by_type(int unit, sal_vaddr_t vaddr, soc_block_t block, int acc_type,
         soc_regtype_t regtype, uint64 *outval, uint32 flags)
{
    int             rv = CMD_OK;
    int             r;
    uint16          phy_rd_data;
    uint32          regaddr = (uint32)vaddr;
    soc_regaddrinfo_t ainfo;
    int		    is64 = FALSE;
    int               blk, cmic, schan, i;

    switch (regtype) {
    case soc_pci_cfg_reg:
        if (regaddr & 3) {
            cli_out("ERROR: PCI config addr must be multiple of 4\n");
            rv = CMD_FAIL;
        } else {
            COMPILER_64_SET(*outval, 0, bde->pci_conf_read(unit, regaddr));
        }
        break;
    case soc_cpureg:
        if (regaddr & 3) {
            cli_out("ERROR: PCI memory addr must be multiple of 4\n");
            rv = CMD_FAIL;
        } else {
            COMPILER_64_SET(*outval, 0, soc_pci_read(unit, regaddr));
        }
        break;
#ifdef BCM_CMICM_SUPPORT
    case soc_mcsreg:
        if (regaddr & 3) {
            cli_out("ERROR: MCS memory addr must be multiple of 4\n");
            rv = CMD_FAIL;
        } else {
            COMPILER_64_SET(*outval, 0, soc_pci_mcs_read(unit, regaddr));
        }
        break;
#endif
#ifdef BCM_IPROC_SUPPORT
    case soc_iprocreg:
        if (regaddr & 3) {
            cli_out("ERROR: iProc memory addr must be multiple of 4\n");
            rv = CMD_FAIL;
        } else {
            COMPILER_64_SET(*outval, 0, soc_cm_iproc_read(unit, regaddr));
        }
        break;
#endif
    case soc_schan_reg:
    case soc_genreg:
    case soc_portreg:
	case soc_cosreg:
    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
        soc_regaddrinfo_extended_get (unit, &ainfo, block, acc_type, regaddr);
            if (ainfo.reg >= 0) {
            is64 = SOC_REG_IS_64(unit, ainfo.reg);
        }
        /* Defensive check to ensure that the supplied 'block' is valid */
        blk = -1;
        for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
            if (regtype == soc_schan_reg) {
                schan = SOC_BLOCK_INFO(unit, i).schan;
                if(schan == block) {
                    blk = i;
                }
            } else {
                cmic = SOC_BLOCK_INFO(unit, i).cmic;
                if(cmic == block) {
                    blk = i;
                }
            }
        }

        if(blk == -1) {
            cli_out("ERROR: Invalid block specified \n");
            rv = CMD_FAIL;
        }

        if(rv == CMD_OK) {
            soc_regaddrinfo_extended_get (unit, &ainfo, block, acc_type, regaddr);
            /* Defensive check to ensure that the register extracted is valid */
            if (ainfo.reg == -1) {
                cli_out("ERROR: Unable to resolve register with supplied data \n");
                rv = CMD_FAIL;
            } else if (ainfo.reg >= 0) {
                is64 = SOC_REG_IS_64(unit, ainfo.reg);
            }

            if(rv == CMD_OK) {
                r = soc_anyreg_read(unit, &ainfo, outval);
                if (r < 0) {
                    cli_out("ERROR: soc_reg32_read failed: %s\n", soc_errmsg(r));
                    rv = CMD_FAIL;
                }
            }
        }

        break;

    case soc_hostmem_w:
        COMPILER_64_SET(*outval, 0, *((uint32 *)INT_TO_PTR(vaddr)));
        break;

    case soc_hostmem_h:
        COMPILER_64_SET(*outval, 0, *((uint16 *)INT_TO_PTR(vaddr)));
        break;

    case soc_hostmem_b:
        COMPILER_64_SET(*outval, 0, *((uint8 *)INT_TO_PTR(vaddr)));
        break;

    case soc_phy_reg:
	/* Leave for MII debug reads */
	if ((r = soc_miim_read(unit,
			       (uint8) (regaddr >> 8 & 0xff),	/* Phy ID */
			       (uint8) (regaddr & 0xff),	/* Phy addr */
			       &phy_rd_data)) < 0) {
	    cli_out("ERROR: soc_miim_read failed: %s\n", soc_errmsg(r));
	    rv = CMD_FAIL;
	} else {
	    COMPILER_64_SET(*outval, 0, (uint32) phy_rd_data);
	}
	break;

    default:
	assert(0);
	rv = CMD_FAIL;
	break;
    }

    if ((rv == CMD_OK) && (flags & REG_PRINT_DO_PRINT)) {
	if (flags & REG_PRINT_HEX) {
	    if (is64) {
		cli_out("%08x%08x\n",
                        COMPILER_64_HI(*outval),
                        COMPILER_64_LO(*outval));
	    } else {
		cli_out("%08x\n",
                        COMPILER_64_LO(*outval));
	    }
	} else {
	    char buf[80];

	    format_uint64(buf, *outval);

	    cli_out("%s[%p] = %s\n",
                    soc_regtypenames[regtype], INT_TO_PTR(vaddr), buf);
	}
    }

    return rv;
}

/* 
 * Set a register by type.  For SOC registers, is64 is used to
 * indicate if this is a 64 bit register.  Otherwise, is64 is
 * ignored.
 */
int
_reg_set_by_type(int unit, sal_vaddr_t vaddr, soc_block_t block, int acc_type,
         soc_regtype_t regtype, uint64 regval)
{
    int             rv = CMD_OK, r;
    uint32          regaddr = (uint32)vaddr;
    uint32          val32;
    soc_regaddrinfo_t ainfo;
    int               blk, cmic, schan, i;

    COMPILER_64_TO_32_LO(val32, regval);

    switch (regtype) {
    case soc_pci_cfg_reg:
	bde->pci_conf_write(unit, regaddr, val32);
	break;

    case soc_cpureg:
	soc_pci_write(unit, regaddr, val32);
	break;

#ifdef BCM_CMICM_SUPPORT
    case soc_mcsreg:
        soc_pci_mcs_write(unit, regaddr, val32);
        break;
#endif
#ifdef BCM_IPROC_SUPPORT
    case soc_iprocreg:
        soc_cm_iproc_write(unit, regaddr, val32);
        break;
#endif
    case soc_schan_reg:
    case soc_genreg:
    case soc_portreg:
    case soc_cosreg:
    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
        /* Defensive check to ensure that the supplied 'block' is valid */
        blk = -1;
        for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
            if (regtype == soc_schan_reg) {
                schan = SOC_BLOCK_INFO(unit, i).schan;
                if(schan == block) {
                    blk = i;
                }
            } else {
                cmic = SOC_BLOCK_INFO(unit, i).cmic;
                if(cmic == block) {
                    blk = i;
                }
            }
        }

        if(blk == -1) {
            cli_out("ERROR: Invalid block specified \n");
            rv = CMD_FAIL;
        }

        if(rv == CMD_OK) {
            soc_regaddrinfo_extended_get(unit, &ainfo, block, acc_type, regaddr);

            /* Defensive check to ensure that the register extracted is valid */
            if (ainfo.reg == -1) {
                cli_out("ERROR: Unable to resolve register with supplied data \n");
                rv = CMD_FAIL;
            }
            else {
                r = soc_anyreg_write(unit, &ainfo, regval);
                if (r < 0) {
                    cli_out("ERROR: write reg failed: %s\n", soc_errmsg(r));
                    rv = CMD_FAIL;
                }
           }
       }

        break;

    case soc_hostmem_w:
        *((uint32 *)INT_TO_PTR(vaddr)) = val32;
        break;

    case soc_hostmem_h:
        *((uint16 *)INT_TO_PTR(vaddr)) = val32;
        break;

    case soc_hostmem_b:
        *((uint8 *)INT_TO_PTR(vaddr)) = val32;
        break;

    case soc_phy_reg:
	/* Leave for MII debug writes */
	if ((r = soc_miim_write(unit,
				(uint8) (regaddr >> 8 & 0xff),	/* Phy ID */
				(uint8) (regaddr & 0xff),	/* Phy addr */
				(uint16) val32)) < 0) {
	    cli_out("ERROR: write miim failed: %s\n", soc_errmsg(r));
	    rv = CMD_FAIL;
	}
	break;

    default:
	assert(0);
	rv = CMD_FAIL;
	break;
    }

    return rv;
}

/* 
 * Gets a memory value or register from the SOC.
 * Syntax: getreg [<regtype>] <offset|symbol>
 */

cmd_result_t
cmd_esw_reg_get(int unit, args_t *a)
{
    uint64          regval;
    uint32          regaddr = 0;
    sal_vaddr_t     vaddr;
    int             rv = CMD_OK, acc_type = 0;
    regtype_entry_t *rt;
    soc_regaddrlist_t alist;
    char           *name, *block, *access_type;
    uint32          flags = REG_PRINT_DO_PRINT;
    soc_block_t blk_num = 0;
    char *field_names = NULL, *temp_field_name = NULL;
    uint8 offset=0;

    if (0 == sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    /* 
     * If first arg is a register type, take it and use the next argument
     * as the name or address, otherwise default to register type "soc."
     */
    name = ARG_GET(a);

    for (;;) {
        if (name != NULL && !sal_strcasecmp(name, "raw")) {
            flags |= REG_PRINT_RAW;
            name = ARG_GET(a);
        } else if (name != NULL && !sal_strcasecmp(name, "hex")) {
            flags |= REG_PRINT_HEX;
            name = ARG_GET(a);
        } else if (name != NULL && !sal_strcasecmp(name, "chg")) {
            flags |= REG_PRINT_CHG;
            name = ARG_GET(a);
        } else {
            break;
        }
    }

    if (name == NULL) {
        return CMD_USAGE;
    }

    if ((rt = regtype_lookup_name(name)) != 0) {
        if ((name = ARG_GET(a)) == 0) {
            return CMD_USAGE;
        }
    } else {
        rt = regtype_lookup_name("schan");
    }
    if (0 == rt) {
        cli_out("Unknown register.\n");
        return (CMD_FAIL);
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return CMD_FAIL;
    }

    if (isint(name)) {
        /* Numerical address given */
        vaddr = parse_address(name);
        if (!reg_type_is_schan_reg(rt->type)) {
            blk_num = 0;
            acc_type = 0;
        } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
            if ((block = ARG_GET(a)) == 0) {
                cli_out("ERROR: This format of Getreg requires block-id to be specified.\n");
                return CMD_FAIL;
            } else {
                if (!isint(block)) {
                    cli_out("ERROR: block-id is not integer.\n");
                    return CMD_FAIL;
                }
                blk_num = parse_address(block);
            }
            if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
                if ((access_type = ARG_GET(a)) == 0) {
                    cli_out("ERROR: Access-type not specified.\n");
                    return CMD_FAIL;
                } else {
                    if (!isint(access_type)) {
                        cli_out("ERROR: Access-type is not integer.\n");
                        return CMD_FAIL;
                    }
                    acc_type = parse_address(access_type);
                }
            }
        }
        rv = _reg_get_by_type(unit, vaddr, blk_num, acc_type, rt->type, &regval, flags);
    } else {
        if (*name == '$') {
            name++;
        }
        field_names = NULL;
        while ((temp_field_name = ARG_GET(a)) != NULL) {
            if (field_names == NULL) {
                if ((field_names = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL) {
                     return (CMD_FAIL);
                }
                sal_strlcpy(field_names, ",", ARGS_BUFFER);
            }
            for (offset=0 ; offset < sal_strlen(temp_field_name);offset++) {
                 temp_field_name[offset] = toupper((int)
                                                   temp_field_name[offset]);
            } 
            sal_strncat(field_names, temp_field_name,
                        ARGS_BUFFER - sal_strlen(temp_field_name) - 1);
            sal_strncat(field_names, ",",
                        ARGS_BUFFER - sal_strlen(temp_field_name) - 1);
        }

        /* Symbolic name given, print all or some values ... */
        if (rt->type == soc_cpureg) {
            if (parse_cmic_regname(unit, name, &regaddr) < 0) {
                cli_out("ERROR: bad argument to GETREG PCIM: %s\n", name);
                rv = CMD_FAIL;
            } else {
                rv = _reg_get_by_type(unit, regaddr, -1, -1,
                                      rt->type, &regval, flags);
            }
        } else if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = CMD_FAIL;
        } else if (alist.ainfo->reg >= NUM_SOC_REG) {
            cli_out("Invalid register \"%s\"\n", name);
            rv = CMD_FAIL;
        } else if (reg_print_all(unit, &alist, flags, field_names) < 0) {
            rv = CMD_FAIL;
        }
    }
    soc_regaddrlist_free(&alist);
    if (field_names  != NULL) {
        sal_free(field_names);
    }
    return rv;
}

/* 
 * Auxilliary routine to handle setreg and modreg.
 *      mod should be 0 for setreg, which takes either a value or a
 *              list of <field>=<value>, and in the latter case, sets all
 *              non-specified fields to zero.
 *      mod should be 1 for modreg, which does a read-modify-write of
 *              the register and permits value to be specified by a list
 *              of <field>=<value>[,...] only.
 */

STATIC cmd_result_t
do_reg_set(int unit, args_t *a, int mod)
{
    uint64          regval;
    uint32          regaddr = 0;
    uint32          flags = 0;
    sal_vaddr_t     vaddr;
    int             rv = CMD_OK, i, acc_type = 0;
    regtype_entry_t *rt;
    soc_regaddrlist_t alist = {0, NULL};
    soc_regaddrinfo_t *ainfo;
    char           *name, *block, *access_type, *arg1;
    char           *s, *valstr = NULL;
    soc_block_t blk_num = 0;
    soc_reg_above_64_val_t value_above_64;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
#endif

    COMPILER_64_ALLONES(regval);

    if (0 == sh_check_attached(ARG_CMD(a), unit)) {
        return  CMD_FAIL;
    }

    arg1 = ARG_GET(a);
    for(;;) {
        if (arg1 != NULL && !sal_strcasecmp(arg1, "nocache")) {
            flags |= SOC_REG_DONT_USE_CACHE;
            arg1 = ARG_GET(a);
        } else {
            break;
        }
    }

    name = arg1;
    if (name == 0) {
        return  CMD_USAGE;
    }

    /* 
     * If first arg is an access type, take it and use the next argument
     * as the name, otherwise use default access type.
     * modreg command does not allow this and assumes soc.
     */

    if ((0 == mod) && (rt = regtype_lookup_name(name)) != 0) {
        if ((name = ARG_GET(a)) == 0) {
            return CMD_USAGE;
        }
    } else {
        rt = regtype_lookup_name("schan");
        if (0 == rt) {
            return CMD_FAIL;
        }
    }

    if (isint(name)) {
        if (!reg_type_is_schan_reg(rt->type)) {
            blk_num = 0;
            acc_type = 0;
        } else if (soc_feature(unit, soc_feature_new_sbus_format)) {
            if ((block = ARG_GET(a)) == 0) {
                cli_out("ERROR: This format of Setreg requires block-id to be specified.\n");
                return CMD_FAIL;
            } else {
                if (!isint(block)) {
                    cli_out("ERROR: block-id is not integer.\n");
                    return CMD_FAIL;
                }
                blk_num = parse_address(block);
            }
            if (soc_feature(unit, soc_feature_two_ingress_pipes)) {
                if ((access_type = ARG_GET(a)) == 0) {
                    cli_out("ERROR: Access-type not specified.\n");
                    return CMD_FAIL;
                } else {
                    if (!isint(access_type)) {
                        cli_out("ERROR: Access-type is not integer.\n");
                        return CMD_FAIL;
                    }
                    acc_type = parse_address(access_type);
                }
            }
        }
    }
    /* 
     * Parse the value field.  If there are more than one, string them
     * together with commas separating them (to make field-based setreg
     * inputs more convenient).
     */

    if ((s = ARG_GET(a)) == 0) {
        cli_out("Syntax error: missing value\n");
        return  CMD_USAGE;
    } 
 
    if ((valstr = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL) {
        cli_out("do_reg_set: Out of memory\n");
        return CMD_FAIL;
    }

    collect_comma_args(a, valstr, s);

    if (mod && isint(valstr)) {
        sal_free(valstr);
        return CMD_USAGE;
    }


    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        sal_free(valstr);
        return CMD_FAIL;
    }

    if (!mod && isint(name)) {
        /* Numerical address given */
        vaddr = parse_address(name);
        regval = parse_uint64(valstr);
        rv = _reg_set_by_type(unit, vaddr, blk_num, acc_type, rt->type, regval);
    } else {
        /* Symbolic name given, set all or some values ... */
        if (*name == '$') {
            name++;
        }
        if (rt->type == soc_cpureg) {
            if (parse_cmic_regname(unit, name, &regaddr) < 0) {
                cli_out("ERROR: bad argument to SETREG PCIM: %s\n", name);
                rv = CMD_FAIL;
            } else {
                regval = parse_uint64(valstr);
                rv = _reg_set_by_type(unit, regaddr, -1, -1, rt->type, regval);
            }
        } else if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            rv = CMD_FAIL;
        } else {
            if (isint(valstr)) { /* valstr is numeric */
                regval = parse_uint64(valstr);
                parse_long_integer(value_above_64, 
                                   SOC_REG_ABOVE_64_MAX_SIZE_U32, 
                                   valstr);
            }

            for (i = 0; i < alist.count; i++) {
                ainfo = &alist.ainfo[i];

                if (!isint(valstr)) { /* valstr is Field/value pairs */
                    /* 
                     * valstr must be a field replacement spec.
                     * In modreg mode, read the current register value,
                     * and modify it.  In setreg mode,
                     * assume a starting value of zero and modify it.
                     */
#ifdef BCM_TOMAHAWK3_SUPPORT
                /* FOR 56982 SKU, some of the PMs are not active in pipes.
                   The below code determines which PM does it belong to
                   based on the pipe_stage numbering of the IDB_CA/OBM
                   registers
                   Also, we program all the valid PM instances in all pipes
                   for the register when the regsiter name does not have
                   the pipe information in it.
                 */
                soc_cm_get_id(unit, &dev_id, &rev_id);

                if (dev_id == BCM56982_DEVICE_ID) {

                    soc_reg_info_t *regp;
                    soc_reg_t reg;
                    reg = ainfo->reg;
                    regp = &(SOC_REG_INFO(unit, reg));
                    if (regp->pipe_stage && *(regp->block) == SOC_BLK_EPIPE) {
                        int pipe_stage = 0;
                        int pm_num = 0, pm = 0;
                        soc_info_t *si;
                        acc_type = SOC_REG_ACC_TYPE(unit, reg);
                        si = &SOC_INFO(unit);
                        pipe_stage = regp->pipe_stage;

                        if (pipe_stage > 14 && pipe_stage < 23) {
                            if (acc_type == 9) {
                                int pipe = 0;
                                soc_regaddrinfo_t temp_ainfo;
                                soc_reg_t base_reg;
                                base_reg = ainfo->reg;

                                sal_memcpy(&temp_ainfo, ainfo, sizeof(soc_regaddrinfo_t));

                                if (pipe_stage > 14 && pipe_stage < 19) {
                                    pm_num = pipe_stage - 15;
                                }
 
                                if (pipe_stage > 18 && pipe_stage < 23) {
                                    pm_num = pipe_stage - 19;
                                }

                                for (pipe = 0; pipe < 8; pipe++) {
                                    pm = pm_num + (pipe * 4);
                                    temp_ainfo.reg = ainfo->reg + 1 + pipe;
                                    if ((si->active_pm_map[pm] == 1)
                                            && ((temp_ainfo.reg - base_reg) <= 8)){
                                        if (mod) { /* read-modify-write */
                                            if (SOC_REG_IS_ABOVE_64(unit,
                                                                ainfo->reg)) {
                                                rv = soc_reg_above_64_get(unit,
                                                         temp_ainfo.reg,
                                                         temp_ainfo.port,
                                                         SOC_REG_IS_ARRAY(unit,
                                                         temp_ainfo.reg) ?
                                                         ainfo->idx : 0,
                                                         value_above_64);
                                            } else {
                                               rv = soc_anyreg_read(unit,
                                                                    &temp_ainfo,
                                                                    &regval);
                                            }
                                            if (rv < 0) {
                                                char buf[80];
                                                soc_reg_sprint_addr(unit, buf,
                                                                    &temp_ainfo);
                                                cli_out("ERROR: read from "
                                                    "register %s failed: %s\n",
                                                    buf, soc_errmsg(rv));
                                            }
                                        } else {
                                            COMPILER_64_ZERO(regval);
                                            SOC_REG_ABOVE_64_CLEAR(value_above_64);
                                        }
                                        if (rv == CMD_OK) {
                                            if (SOC_REG_IS_ABOVE_64(unit,
                                                               ainfo->reg)) {
                                                rv = modify_reg_above_64_fields(
                                                       unit,
                                                       temp_ainfo.reg,
                                                       value_above_64,
                                                       (soc_reg_above_64_val_t *) 0,
                                                       valstr);
                                            } else {
                                                rv = modify_reg_fields(unit,
                                                        temp_ainfo.reg,
                                                        &regval, NULL, valstr);
                                            }

                                        }
                                        if (rv == CMD_OK) {
                                            if (SOC_REG_IS_ABOVE_64(unit,
                                                    ainfo->reg)) {
                                                rv = soc_reg_above_64_set( unit,
                                                      temp_ainfo.reg,
                                                      temp_ainfo.port,
                                                      SOC_REG_IS_ARRAY(unit,
                                                        temp_ainfo.reg) ?
                                                            temp_ainfo.idx : 0,
                                                            value_above_64);
                                                 if (rv == 0) {
                                                    continue;
                                                 }
                                                 break;
                                            }
                                            if (flags &
                                                    SOC_REG_DONT_USE_CACHE) {
                                                SOC_MEM_TEST_SKIP_CACHE_SET(
                                                                unit, TRUE);
                                            }
                                            rv = soc_anyreg_write(unit,
                                                                  &temp_ainfo,
                                                                  regval);
                                            if (flags &
                                                    SOC_REG_DONT_USE_CACHE) {
                                                SOC_MEM_TEST_SKIP_CACHE_SET(
                                                                    unit, FALSE);
                                            }
                                            if (rv < 0) {
                                                char buf[80];
                                                soc_reg_sprint_addr(unit,
                                                                    buf,
                                                                    &temp_ainfo);
                                                cli_out("ERROR: write to "
                                                        "register %s failed: %s\n",
                                                        buf, soc_errmsg(rv));
                                            }
                                        }
                                        if (rv != CMD_OK) {
                                            break;
                                        }

                                    }

                                   }
                                   continue;
                                }

                                if (acc_type >=0 && acc_type < 8) {
                                    if (pipe_stage > 14 && pipe_stage < 19) {
                                        pm_num = pipe_stage - 15;
                                    }

                                    if (pipe_stage > 18 && pipe_stage < 23) {
                                        pm_num = pipe_stage - 19;
                                    }

                                    pm = pm_num + (acc_type * 4);
                                }

                                if (si->active_pm_map[pm] != 1) {
                                    char reg_name[80];
                                    sal_sprintf(reg_name, "%s", SOC_REG_NAME(unit, ainfo->reg));
                                    cli_out("Register %s does not exist in TH3 56982 SKU\n",
                                            reg_name);
                                    continue;
                                }
                            }
                        }
                    }
#endif

                    if (mod) { /* read-modify-write */

#ifdef CANCUN_SUPPORT
#ifdef BCM_TRIDENT3_SUPPORT
                        if (SOC_IS_TRIDENT3X(unit) &&
                            SOC_REG_IS_CCH(unit, ainfo->reg) && !(soc_property_get(unit,
                                    "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE) &&
                                    SOC_BLOCK_IS(SOC_REG_INFO(unit, ainfo->reg).block,
                                    SOC_BLK_CCH)) {
                            if (SOC_REG_ARRAY(unit, ainfo->reg)) {
                                if (alist.count > 1) {
                                    /* Reg array access */
                                    ainfo->reg += (i+1);
                                } else if (alist.count == 1) {
                                    /* Reg array single entry access */
                                    ainfo->reg += alist.ainfo->idx+1;
                                }
                            }
                        }
#endif /* BCM_TRIDENT3_SUPPORT */
#endif /* CANCUN_SUPPORT */

                        if (SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
                            rv = soc_reg_above_64_get(unit, 
                                 ainfo->reg, ainfo->port,  
                                 SOC_REG_IS_ARRAY(unit, ainfo->reg) ? 
                                 ainfo->idx : 0, value_above_64);
                        } else {
                           rv = soc_anyreg_read(unit, ainfo, &regval);
                        }
                        if (rv < 0) {
                            char buf[80];
                            soc_reg_sprint_addr(unit, buf, ainfo);
                            cli_out("ERROR: read from register %s failed: %s\n",
                                    buf, soc_errmsg(rv));
                        }
                    } else {
                        COMPILER_64_ZERO(regval);
                        SOC_REG_ABOVE_64_CLEAR(value_above_64);
                    }
                    if (rv == CMD_OK) {
                        if (SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
                            rv = modify_reg_above_64_fields(unit, 
                                        ainfo->reg, value_above_64, 
                                        (soc_reg_above_64_val_t *) 0,valstr);
                        } else {
                            rv = modify_reg_fields(unit, 
                                        ainfo->reg, &regval, NULL, valstr);
                        }

                    }
                }
                if (rv == CMD_OK) {
                    if (SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
                         rv = soc_reg_above_64_set( unit, 
                                  ainfo->reg, ainfo->port, 
                                  SOC_REG_IS_ARRAY(unit, ainfo->reg) ? 
                                  ainfo->idx : 0, value_above_64);
                         if (rv == 0) {
                             continue;
                         }
                         break;
                    }
                    if (flags & SOC_REG_DONT_USE_CACHE) {
                        SOC_MEM_TEST_SKIP_CACHE_SET(unit, TRUE);
                    }
                    rv = soc_anyreg_write(unit, ainfo, regval);
                    if (flags & SOC_REG_DONT_USE_CACHE) {
                        SOC_MEM_TEST_SKIP_CACHE_SET(unit, FALSE);
                    }
                    if (rv < 0) {
                        char buf[80];
                        soc_reg_sprint_addr(unit, buf, ainfo);
                        cli_out("ERROR: write to register %s failed: %s\n",
                                buf, soc_errmsg(rv));
                    }
                }
                if (rv != CMD_OK) {
                    break;
                }
            }
        }
    }
    sal_free(valstr);
    soc_regaddrlist_free(&alist);
    return rv;
}

/* 
 * Sets a memory value or register on the SOC.
 * Syntax 1: setreg [<regtype>] <offset|symbol> <value>
 * Syntax 2: setreg [<regtype>] <offset|symbol> <field>=<value>[,...]
 */
cmd_result_t
cmd_esw_reg_set(int unit, args_t *a)
{
    return do_reg_set(unit, a, 0);
}

/* 
 * Read/modify/write a memory value or register on the SOC.
 * Syntax: modreg [<regtype>] <offset|symbol> <field>=<value>[,...]
 */
cmd_result_t
cmd_esw_reg_mod(int unit, args_t * a)
{
    return do_reg_set(unit, a, 1);
}

char regcmp_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: regcmp [args...]\n"
#else
    "Parameters: [-q] [<LOOPS>] <REG> [<VALUE>] [==|!=] <VALUE>\n\t"
    "If the optional <VALUE> on the left is given, starts by writing\n\t"
    "<VALUE> to <REG>.   Then loops up to <LOOPS> times reading <REG>,\n\t"
    "comparing if it is equal (==) or not equal (!=) to the <VALUE> on\n\t"
    "the right, and stopping if the compare fails.  If -q is specified, no\n\t"
    "output is displayed.  <LOOPS> defaults to 1 and may be * for\n\t"
    "indefinite.  If the compare fails, the command result is 1.  If the\n\t"
    "loops expire (compares succeed), the result is 0.  The result may be\n\t"
    "tested in an IF statement.  Also, each <VALUE> can consist of\n\t"
    "<FIELD>=<VALUE>[,...] to compare individual fields.  Examples:\n\t"
    "    if \"regcmp -q 1 rpkt.fe0 == 0\" \"echo RPKT IS ZERO\"\n\t"
    "    if \"regcmp -q config.e0 == fil_en=1\" \"echo FILTERING ENABLED\"\n"
#endif
    ;

cmd_result_t
reg_cmp(int unit, args_t *a)
{
#ifndef SOC_NO_NAMES
    soc_reg_t       reg;
#endif
    soc_regaddrlist_t alist;
    soc_regaddrinfo_t *ainfo;
    char           *name = NULL, *count_str;
    char           *read_str, *write_str, *op_str;
    uint64          read_val, read_mask, write_val, reg_val, tmp_val;
    int             equal, i, quiet, loop;
    int             are_equal;
    int rv = CMD_OK;

    if (!(count_str = ARG_GET(a))) {
        return (CMD_USAGE);
    }

    if (!sal_strcmp(count_str, "-q")) {
        quiet = TRUE;
        if (!(count_str = ARG_GET(a))) {
            return (CMD_USAGE);
        }
    } else {
        quiet = FALSE;
    }

    if (!sal_strcmp(count_str, "*")) {
        loop = -1;
    } else if (isint(count_str)) {
        if ((loop = parse_integer(count_str)) < 0) {
            cli_out("%s: Invalid loop count: %s\n", ARG_CMD(a), count_str);
            return (CMD_FAIL);
        }
    } else {
        name = count_str;
        loop = 1;
    }

    if (!name && !(name = ARG_GET(a))) {
        return (CMD_USAGE);
    }

    write_str = ARG_GET(a);
    op_str = ARG_GET(a);
    read_str = ARG_GET(a);

    /* Must have WRITE ==|!= READ or ==|!= READ */

    if (!read_str) {
        read_str = op_str;
        op_str = write_str;
        write_str = NULL;
    } else if (ARG_CNT(a)) {
        return (CMD_USAGE);
    }

    if (!read_str || !op_str) {
        return (CMD_USAGE);
    }

    if (!sal_strcmp(op_str, "==")) {
        equal = TRUE;
    } else if (!sal_strcmp(op_str, "!=")) {
        equal = FALSE;
    } else {
        return (CMD_USAGE);
    }

    if (*name == '$') {
        name++;
    }

    /* Symbolic name given, set all or some values ... */

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return CMD_FAIL;
    }

    if (parse_symbolic_reference(unit, &alist, name) < 0) {
        cli_out("%s: Syntax error parsing \"%s\"\n", ARG_CMD(a), name);
        soc_regaddrlist_free(&alist);
        return (CMD_FAIL);
    }

    ainfo = &alist.ainfo[0];
#ifndef SOC_NO_NAMES
    reg = ainfo->reg;
#endif

    COMPILER_64_ALLONES(read_mask);

    if (isint(read_str)) {
        read_val = parse_uint64(read_str);
    } else {
        COMPILER_64_ZERO(read_val);
        if (modify_reg_fields(unit, ainfo->reg, &read_val,
                              &read_mask, read_str) < 0) {
            cli_out("%s: Syntax error: %s\n", ARG_CMD(a), read_str);
            soc_regaddrlist_free(&alist);
            return (CMD_USAGE);
        }
    }

    if (write_str) {
        if (isint(write_str)) {
            write_val = parse_uint64(write_str);
        } else {
            COMPILER_64_ZERO(write_val);
            if (modify_reg_fields(unit, ainfo->reg, &write_val,
                                  (uint64 *) 0, write_str) < 0) {
                cli_out("%s: Syntax error: %s\n", ARG_CMD(a), write_str);
                soc_regaddrlist_free(&alist);
                return (CMD_USAGE);
            }
        }
    }

    do {
        for (i = 0; i < alist.count; i++) {
            int             r;

            ainfo = &alist.ainfo[i];
            if (write_str) {
                if ((r = soc_anyreg_write(unit, ainfo, write_val)) < 0) {
                    cli_out("%s: ERROR: Write register %s.%d failed: %s\n",
                            ARG_CMD(a), SOC_REG_NAME(unit, reg), i, soc_errmsg(r));
                    soc_regaddrlist_free(&alist);
                    return (CMD_FAIL);
                }
            }

            if ((r = soc_anyreg_read(unit, ainfo, &reg_val)) < 0) {
                cli_out("%s: ERROR: Read register %s.%d failed: %s\n",
                        ARG_CMD(a), SOC_REG_NAME(unit, reg), i, soc_errmsg(r));
                soc_regaddrlist_free(&alist);
                return (CMD_FAIL);
            }

            tmp_val = reg_val;
            COMPILER_64_AND(tmp_val, read_mask);
            COMPILER_64_XOR(tmp_val, read_val);
            are_equal = COMPILER_64_IS_ZERO(tmp_val);
            if ((!are_equal && equal) || (are_equal && !equal)) {
                if (!quiet) {
                    char buf1[80], buf2[80];
                    cli_out("%s: %s.%d ", ARG_CMD(a), SOC_REG_NAME(unit, reg), i);
                    format_uint64(buf1, read_val);
                    format_uint64(buf2, reg_val);
                    cli_out("%s %s %s\n", buf1, equal ? "!=" : "==", buf2);
                }
                soc_regaddrlist_free(&alist);
                return (1);
            }
        }

        if (loop > 0) {
            loop--;
        }
    } while (loop);

    soc_regaddrlist_free(&alist);
    return rv;
}

/* 
 * Lists registers containing a specific pattern
 *
 * If use_reset is true, ignores val and uses reset default value.
 */

static void
do_reg_list(int unit, soc_regaddrinfo_t *ainfo, int use_reset, uint64 regval)
{
    soc_reg_t       reg = ainfo->reg;
    soc_reg_info_t *reginfo = &SOC_REG_INFO(unit, reg);
    soc_field_info_t *fld;
    int             f;
    uint32          flags;
    uint64          mask, fldval, rval, rmsk;
    char            buf[80];
    char            rval_str[20], rmsk_str[20], dval_str[20];
    int		    i, copies, disabled;

    if (!SOC_REG_IS_VALID(unit, reg)) {
	    cli_out("Register is not valid for chip %s\n", SOC_UNIT_NAME(unit));
	    return;
    }

    flags = reginfo->flags;

    COMPILER_64_ALLONES(mask);

    SOC_REG_RST_VAL_GET(unit, reg, rval);
    SOC_REG_RST_MSK_GET(unit, reg, rmsk);
    format_uint64(rval_str, rval);
    format_uint64(rmsk_str, rmsk);
    if (use_reset) {
        regval = rval;
        mask = rmsk;
    } else {
	format_uint64(dval_str, regval);
    }

    soc_reg_sprint_addr(unit, buf, ainfo);

    cli_out("Register: %s", buf);
#if !defined(SOC_NO_ALIAS)
    if (soc_reg_alias[reg] && *soc_reg_alias[reg]) {
        cli_out(" alias %s", soc_reg_alias[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    cli_out(" %s register", soc_regtypenames[reginfo->regtype]);
    cli_out(" address 0x%08x\n", ainfo->addr);

    cli_out("Flags:");
    if (flags & SOC_REG_FLAG_64_BITS) {
        cli_out(" 64-bits");
    }
    if (flags & SOC_REG_FLAG_32_BITS) {
        cli_out(" 32-bits");
    }
    if (flags & SOC_REG_FLAG_ABOVE_64_BITS) {
        cli_out(" Above 64-bits");
    }

    if (flags & SOC_REG_FLAG_COUNTER) {
        cli_out(" counter");
    }
    if (flags & SOC_REG_FLAG_ARRAY) {
        cli_out(" array[%d-%d]", 0, reginfo->numels-1);
    }
    if (flags & SOC_REG_FLAG_NO_DGNL) {
        cli_out(" no-diagonals");
    }
    if (flags & SOC_REG_FLAG_RO) {
        cli_out(" read-only");
    }
    if (flags & SOC_REG_FLAG_WO) {
        cli_out(" write-only");
    }
    if (flags & SOC_REG_FLAG_ED_CNTR) {
        cli_out(" error/discard-counter");
    }
    if (flags & SOC_REG_FLAG_SPECIAL) {
        cli_out(" special");
    }
    if (flags & SOC_REG_FLAG_EMULATION) {
        cli_out(" emulation");
    }
    if (flags & SOC_REG_FLAG_VARIANT1) {
        cli_out(" variant1");
    }
    if (flags & SOC_REG_FLAG_VARIANT2) {
        cli_out(" variant2");
    }
    if (flags & SOC_REG_FLAG_VARIANT3) {
        cli_out(" variant3");
    }
    if (flags & SOC_REG_FLAG_VARIANT4) {
        cli_out(" variant4");
    }
    cli_out("\n");

    cli_out("Blocks:");
    copies = disabled = 0;
    for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
	/* if (SOC_BLOCK_INFO(unit, i).type & reginfo->block) { */
        /* if (SOC_BLOCK_IN_LIST(unit, reginfo->block, SOC_BLOCK_INFO(unit, i).type)) { */
        if (SOC_BLOCK_IS_TYPE(unit, i, reginfo->block)) {
	    if (SOC_INFO(unit).block_valid[i]) {
		cli_out(" %s", SOC_BLOCK_NAME(unit, i));
	    } else {
		cli_out(" [%s]", SOC_BLOCK_NAME(unit, i));
		disabled += 1;
	    }
	    copies += 1;
	}
    }
    cli_out(" (%d cop%s", copies, copies == 1 ? "y" : "ies");
    if (disabled) {
	cli_out(", %d disabled", disabled);
    }
    cli_out(")\n");

#if !defined(SOC_NO_DESC)
    if (soc_reg_desc[reg] && *soc_reg_desc[reg]) {
	cli_out("Description: %s\n", soc_reg_desc[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    cli_out("Displaying:");
    if (use_reset) {
	cli_out(" reset defaults");
    } else {
	cli_out(" value %s", dval_str);
    }
    cli_out(", reset value %s mask %s\n", rval_str, rmsk_str);

    for (f = reginfo->nFields - 1; f >= 0; f--) {
	fld = &reginfo->fields[f];
	cli_out("  %s<%d", SOC_FIELD_NAME(unit, fld->field),
                fld->bp + fld->len - 1);
	if (fld->len > 1) {
	    cli_out(":%d", fld->bp);
	}
	fldval = soc_reg64_field_get(unit, reg, mask, fld->field);
	if (use_reset && COMPILER_64_IS_ZERO(fldval)) {
	    cli_out("> = x");
	} else {
	    fldval = soc_reg64_field_get(unit, reg, regval, fld->field);
	    format_uint64(buf, fldval);
	    cli_out("> = %s", buf);
	}
	if (fld->flags & (SOCF_RO|SOCF_WO)) {
	    cli_out(" [");
	    i = 0;
	    if (fld->flags & SOCF_RO) {
		cli_out("%sRO", i++ ? "," : "");
	    }
	    if (fld->flags & SOCF_WO) {
		cli_out("%sWO", i++ ? "," : "");
	    }
	    cli_out("]");
	}
	cli_out("\n");
    }
}

#define	PFLAG_ALIAS	0x01
#define	PFLAG_SUMMARY	0x02

static void
_print_regname(int unit, soc_reg_t reg, int *col, int pflags)
{
    int             len;
    soc_reg_info_t *reginfo;

    reginfo = &SOC_REG_INFO(unit, reg);
    len = sal_strlen(SOC_REG_NAME(unit, reg)) + 1;

    if (pflags & PFLAG_SUMMARY) {
	char	tname, *dstr1, *dstr2, *bname;
	int	dlen, copies, i;
	char	nstr[128], bstr[64];

	switch (reginfo->regtype) {
	case soc_schan_reg:	tname = 's'; break;
	case soc_cpureg:	tname = 'c'; break;
	case soc_genreg:	tname = 'g'; break;
	case soc_portreg:	tname = 'p'; break;
	case soc_cosreg:	tname = 'o'; break;
        case soc_pipereg:       tname = 'p'; break;
        case soc_xpereg:        tname = 'x'; break;
        case soc_itmreg:        tname = 'i'; break;
        case soc_ebreg:        tname = 'e'; break;
        case soc_slicereg:      tname = 's'; break;
        case soc_layerreg:      tname = 'l'; break;
	case soc_hostmem_w:
	case soc_hostmem_h:
	case soc_hostmem_b:	tname = 'm'; break;
	case soc_phy_reg:	tname = 'f'; break;
	case soc_pci_cfg_reg:	tname = 'P'; break;
	default:		tname = '?'; break;
	}
#if !defined(SOC_NO_DESC)
	dstr2 = sal_strchr(soc_reg_desc[reg], '\n');
	if (dstr2 == NULL) {
	    dlen = sal_strlen(soc_reg_desc[reg]);
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
	    sal_sprintf(nstr, "%s[%d]", SOC_REG_NAME(unit, reg), reginfo->numels);
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
	cli_out(" %c%c%c%c%c  %-26s %-8.8s  %*.*s%s\n",
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
	cli_out("  ");
	*col = 2;
    }
    if (*col + len > ((pflags & PFLAG_ALIAS) ? 65 : 72)) {
	cli_out("\n  ");
	*col = 2;
    }
    cli_out("%s%s ", SOC_REG_NAME(unit, reg), SOC_REG_ARRAY(unit, reg) ? "[]" : "");
#if !defined(SOC_NO_ALIAS)
    if ((pflags & PFLAG_ALIAS) && soc_reg_alias[reg]) {
        len += sal_strlen(soc_reg_alias[reg]) + 8;
        cli_out("(aka %s) ", soc_reg_alias[reg]);
    }
#endif /* !defined(SOC_NO_ALIAS) */
    *col += len;
}

static void
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
    cli_out("\n");
}

cmd_result_t
cmd_esw_reg_list(int unit, args_t *a)
{
    char           *str;
    char           *val;
    uint64          value;
    soc_regaddrinfo_t ainfo;
    int             found;
    int             rv = CMD_OK;
    int             all_regs;
    soc_reg_t       reg;
    int             col;
    int		    pflag;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!soc_property_get(unit, spn_REGLIST_ENABLE, 1)) {
        return CMD_OK;
    }

    ainfo.reg = INVALIDr;
    pflag = 0;
    col = -1;

    /* Parse options */
    while (((str = ARG_GET(a)) != NULL) && (str[0] == '-')) {
	while (str[0] && str[0] == '-') {
	    str += 1;
	}
        if (sal_strcasecmp(str, "alias") == 0 ||
	    sal_strcasecmp(str, "a") == 0) {	/* list w/ alias */
            pflag |= PFLAG_ALIAS;
	    continue;
	}
        if (sal_strcasecmp(str, "summary") == 0 ||
	    sal_strcasecmp(str, "s") == 0) {	/* list w/ summary */
            pflag |= PFLAG_SUMMARY;
	    continue;
        }
	if (sal_strcasecmp(str, "counters") == 0 ||
	    sal_strcasecmp(str, "c") == 0) {	/* list counters */
            cli_out("unit %d counters\n", unit);
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                if (!SOC_REG_IS_VALID(unit, reg))
                    continue;
                if (!SOC_REG_IS_COUNTER(unit, reg))
                    continue;
                _print_regname(unit, reg, &col, pflag);
            }
            cli_out("\n\n");
            return CMD_OK;
        }
	if (sal_strcasecmp(str, "ed") == 0 ||
	    sal_strcasecmp(str, "e") == 0) {	/* error/discard */
            cli_out("unit %d error/discard counters\n", unit);
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                if (!SOC_REG_IS_VALID(unit, reg)) {
                    continue;
                }
                if (!(SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_ED_CNTR)) {
                    continue;
                }
                _print_regname(unit, reg, &col, pflag);
            }
            cli_out("\n\n");
            return CMD_OK;
        }
	if (sal_strcasecmp(str, "type") == 0 ||
	    sal_strcasecmp(str, "t") == 0) {	/* list by type */
            int	       i;
            soc_info_t *si = &SOC_INFO(unit);

	    for (i = 0; i < COUNTOF(si->has_block); i++) {
		if (!(si->has_block[i])) {
		    continue;
		}
		cli_out("unit %d %s registers\n",
                        unit,
                        soc_block_name_lookup_ext(si->has_block[i], unit));
		col = -1;
                _list_regs_by_type(unit, si->has_block[i], &col, pflag);
	    }
            cli_out("\n");
            return CMD_OK;
        }
	cli_out("ERROR: unrecognized option: %s\n", str);
	return CMD_FAIL;
    }

    if (!str) {
	return CMD_USAGE;
    }

    if ((val = ARG_GET(a)) != NULL) {
	value = parse_uint64(val);
    } else {
	COMPILER_64_ZERO(value);
    }


    if (isint(str)) {
	/* 
	 * Address given, look up SOC register.
	 */
	char            buf[80];
	uint32          addr;
	addr = parse_integer(str);
	soc_regaddrinfo_get(unit, &ainfo, addr);
	if (!ainfo.valid || (int)ainfo.reg < 0) {
	    cli_out("Unknown register address: 0x%x\n", addr);
	    rv = CMD_FAIL;
	} else {
	    soc_reg_sprint_addr(unit, buf, &ainfo);
	    cli_out("Address %s\n", buf);
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
	    cli_out("\n");
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
	    do_reg_list(unit, &ainfo, 0, value);
	} else {
            COMPILER_64_ZERO(value);
	    do_reg_list(unit, &ainfo, 1, value);
	}
    }

    return rv;
}

/* 
 * Editreg allows modifying register fields.
 * Works on fully qualified SOC registers only.
 */

cmd_result_t
cmd_esw_reg_edit(int unit, args_t *a)
{
    soc_reg_info_t *reginfo;
    soc_field_info_t *fld;
    soc_regaddrlist_t alist;
    soc_reg_t       reg;
    uint64	    v64;
    uint32          val, dfl, fv;
    char            ans[64], dfl_str[64];
    char           *name = ARG_GET(a);
    int             r, rv = CMD_FAIL;
    int             i, f;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return rv;
    }

    if (!name) {
	return CMD_USAGE;
    }

    if (*name == '$') {
	name++;
    }

    if (soc_regaddrlist_alloc(&alist) < 0) {
        cli_out("Could not allocate address list.  Memory error.\n");
        return CMD_FAIL;
    }

    if (parse_symbolic_reference(unit, &alist, name) < 0) {
        cli_out("Syntax error parsing \"%s\"\n", name);
        soc_regaddrlist_free(&alist);
        return (rv);
    }

    reg = alist.ainfo[0].reg;
    reginfo = &SOC_REG_INFO(unit, reg);

    /* 
     * If more than one register was specified, read the first one
     * and write the edited value to all of them.
     */

    if (soc_anyreg_read(unit, &alist.ainfo[0], &v64) < 0) {
        cli_out("ERROR: read reg failed\n");
        soc_regaddrlist_free(&alist);
        return (rv);
    }

    COMPILER_64_TO_32_LO(val, v64);

    cli_out("Current value: 0x%x\n", val);

    for (f = 0; f < (int)reginfo->nFields; f++) {
	fld = &reginfo->fields[f];
	dfl = soc_reg_field_get(unit, reg, val, fld->field);
	sal_sprintf(dfl_str, "0x%x", dfl);
	sal_sprintf(ans,		       /* Also use ans[] for prompt */
		"  %s<%d", SOC_FIELD_NAME(unit, fld->field), fld->bp + fld->len - 1);
	if (fld->len > 1) {
	    sal_sprintf(ans + sal_strlen(ans), ":%d", fld->bp);
	}
    /* coverity[secure_coding] */
	sal_strcat(ans, ">? ");
	if (sal_readline(ans, ans, sizeof(ans), dfl_str) == 0 || ans[0] == 0) {
	    cli_out("Aborted\n");
        soc_regaddrlist_free(&alist);
        return (rv);
	}
	fv = parse_integer(ans);
	if (fv & ~((1 << (fld->len - 1) << 1) - 1)) {
	    cli_out("Value too big for %d-bit field, try again.\n",
                    fld->len);
	    f--;
	} else {
	    soc_reg_field_set(unit, reg, &val, fld->field, fv);
	}
    }

    cli_out("Writing new value: 0x%x\n", val);

    for (i = 0; i < alist.count; i++) {
        COMPILER_64_SET(v64, 0, val);

	if ((r = soc_anyreg_write(unit, &alist.ainfo[i], v64)) < 0) {
	    cli_out("ERROR: write reg 0x%x failed: %s\n",
                    alist.ainfo[i].addr, soc_errmsg(r));
        soc_regaddrlist_free(&alist);
        return (rv);
	}
    }

    rv = CMD_OK;

    soc_regaddrlist_free(&alist);
    return rv;
}
void 
reg_watch_cb(int unit, soc_reg_t reg, soc_regaddrinfo_t *ainfo, uint32 flags, 
             uint32 data_hi, uint32 data_lo, void *user_data)
{   
    uint64 value;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return;
    }   
    COMPILER_64_SET(value,data_hi,data_lo);
    if (flags & SOC_REG_SNOOP_READ) {
#if !defined(SOC_NO_NAMES)
        cli_out("Unit=%d REGISTER=%s(Read) \t",unit,soc_reg_name[reg]);
#endif
        if (data_hi != 0) {
            cli_out("HighData=0x%08x \t",data_hi);
            cli_out("LowData=0x%08x \n",data_lo);
        } else {
            cli_out("Data=0x%08x \n",data_lo);
        }
        reg_print(unit, ainfo, value, REG_PRINT_DO_PRINT, ",", 62, NULL);
    } else {
        if (flags & SOC_REG_SNOOP_WRITE) {
#if !defined(SOC_NO_NAMES)
            cli_out("Unit=%d REGISTER=%s(WRITE) \t",unit,soc_reg_name[reg]);
#endif
            if (data_hi != 0) {
                cli_out("HighData=0x%08x \t",data_hi);
                cli_out("LowData=0x%08x \n",data_lo);
            } else {
                cli_out("Data=0x%08x \n",data_lo);
            }
            reg_print(unit, ainfo, value, REG_PRINT_DO_PRINT, ",", 62, NULL);
        }
    }
    return;
}

void reg_watch_snoop_all(int unit, uint32 flags, int regist)
{
    soc_reg_t       reg;

    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        if (regist) {
            soc_reg_snoop_register(unit, reg, flags, reg_watch_cb, NULL);
        } else {
            soc_reg_snoop_unregister(unit, reg);
        }
    }
    return;
}

cmd_result_t
reg_watch(int unit, args_t *a)
{
    soc_regaddrlist_t alist;
    soc_reg_t       reg = INVALIDr;
    /* uint64	    v64; */
    char           *name = ARG_GET(a);
    char           *c;
    int             rv = CMD_FAIL;
    int             allregs = 0;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return rv;
    }

    if (!name) {
	return CMD_USAGE;
    }

    if (sal_strcasecmp(name, "*") == 0) {
        allregs = 1;
    } else {
        if (sal_strcasecmp(name, "delta") == 0) {
            c = ARG_GET(a);
            soc_reg_watch_set(unit, (c && (sal_strcasecmp(c, "on") == 0)) ? 1 : 0);
            return CMD_OK;
        }

        if (soc_regaddrlist_alloc(&alist) < 0) {
            cli_out("Could not allocate address list.  Memory error.\n");
            return CMD_FAIL;
        }

        if (parse_symbolic_reference(unit, &alist, name) < 0) {
            cli_out("Syntax error parsing \"%s\"\n", name);
            soc_regaddrlist_free(&alist);
            return (rv);
        }
        reg = alist.ainfo[0].reg;
    }

    if (NULL == (c = ARG_GET(a))) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(c, "off") == 0) {
        /* unregister call back */
        if (allregs) {
            reg_watch_snoop_all(unit, 0, FALSE);
        } else {
            soc_reg_snoop_unregister(unit, reg);
        }
    } else if (sal_strcasecmp(c, "read") == 0) {
        /* register callback with read flag */
        if (allregs) {
            reg_watch_snoop_all(unit, SOC_REG_SNOOP_READ, TRUE);
        } else {
            soc_reg_snoop_register(unit, reg, SOC_REG_SNOOP_READ,
                                   reg_watch_cb, NULL);
        }
    } else if (sal_strcasecmp(c, "write") == 0) {
        /* register callback with write flag */
        if (allregs) {
            reg_watch_snoop_all(unit, SOC_REG_SNOOP_READ, TRUE);
        } else {
            soc_reg_snoop_register(unit, reg, SOC_REG_SNOOP_WRITE,
                                   reg_watch_cb, NULL);
        }
    } else {
        return CMD_USAGE;
    }
    if (bsl_check(bslLayerAppl, bslSourceTests, bslSeverityNormal, unit) == 0) {
        cli_out("** Also enable Tests debug by \"debug soc TESTS info\" \n");
    }
    return CMD_OK;
}
