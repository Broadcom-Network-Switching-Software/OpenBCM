/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag memory commands
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/mem.h>
#if defined (BCM_FIRELIGHT_SUPPORT)
#include <soc/soc_mem_bulk.h>
#endif
#include <soc/debug.h>
#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/l2x.h>
#include <soc/l3x.h>
#include <soc/soc_ser_log.h>
#ifdef BCM_ISM_SUPPORT
#include <soc/ism.h>
#endif
#include <appl/diag/system.h>
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/esw/flow_db.h>
#include <soc/esw/flow_db_core.h>
#endif
#ifdef ALPM_ENABLE
#include <soc/alpm.h>
#endif
#ifdef __KERNEL__
#define atoi _shr_ctoi
#endif

#ifdef INCLUDE_MEM_SCAN
static sal_usecs_t prev_memscan_interval[SOC_MAX_NUM_DEVICES];
static int prev_memscan_rate[SOC_MAX_NUM_DEVICES];
#endif
#ifdef BCM_SRAM_SCAN_SUPPORT
static sal_usecs_t prev_sramscan_interval[SOC_MAX_NUM_DEVICES];
static int prev_sramscan_rate[SOC_MAX_NUM_DEVICES];
#endif

/* Memory counter array */
static mem_count_t *mem_count[SOC_MAX_NUM_DEVICES];

/* local defines */
#define FIELD_ID_ARRAY_SIZE  40
#define VIEW_MULTI_ID_MAX    2

/*
 * Utility routine to concatenate the first argument ("first"), with
 * the remaining arguments, with commas separating them.
 */

static void
collect_comma_args(args_t *a, char *valstr, char *first)
{
    char *s;

    sal_strcpy(valstr, first);

    while ((s = ARG_GET(a)) != 0) {
	sal_strcat(valstr, ",");
	sal_strcat(valstr, s);
    }
}

static void
check_global(int unit, soc_mem_t mem, char *s, int *is_global)
{
    soc_field_info_t	*fld;
    soc_mem_info_t	*m = &SOC_MEM_INFO(unit, mem);
    char                *eqpos;
    
    eqpos = strchr(s, '=');
    if (eqpos != NULL) {
        *eqpos++ = 0;
    }
    for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
        if (!sal_strcasecmp(s, SOC_FIELD_NAME(unit, fld->field)) &&
            (fld->flags & SOCF_GLOBAL)) {
    	    break;
        }
    }
    if (fld == &m->fields[m->nFields]) {
        *is_global = 0;
    } else {
        *is_global = 1;
    }
}

static int
collect_comma_args_with_view(args_t *a, char *valstr, char *first, 
                             char *view, int unit, soc_mem_t mem)
{
    char *s, *s_copy = NULL, *f_copy = NULL;
    int is_global, rv = 0;

    if ((f_copy = sal_alloc(sal_strlen(first) + 1, "first")) == NULL) {
        cli_out("cmd_esw_mem_write : Out of memory\n");
        rv = -1;
        goto done;
    }
    sal_memset(f_copy, 0, sal_strlen(first) + 1);
    sal_strcpy(f_copy, first);

    /* Check if field is global before applying view prefix */
    check_global(unit, mem, f_copy, &is_global);
    if (!is_global) {
        sal_strcpy(valstr, view);
        sal_strcat(valstr, first);
    } else {
        sal_strcpy(valstr, first);
    }

    while ((s = ARG_GET(a)) != 0) {
        if ((s_copy = sal_alloc(sal_strlen(s) + 1, "s_copy")) == NULL) {
            cli_out("cmd_esw_mem_write : Out of memory\n");
            rv = -1;
            goto done;
        }
        sal_memset(s_copy, 0, sal_strlen(s) + 1);
        sal_strcpy(s_copy, s);
        check_global(unit, mem, s_copy, &is_global);
        sal_free(s_copy);
        sal_strcat(valstr, ",");
        if (!is_global) {
            sal_strcat(valstr, view);
            sal_strcat(valstr, s);
        } else {
            sal_strcat(valstr, s);
        }
    }
done:
    if (f_copy != NULL) {
        sal_free(f_copy);
    }
    return rv;
}

static int
_modify_flex_mem_fields(int unit, soc_mem_t mem, uint32 *entry,
		  uint32 *mask, char *mod, int incr)
{
#ifdef BCM_TRIDENT3_SUPPORT
    int rv = CMD_NFND;
    soc_field_info_t	*fld;
    char		*fmod, *fval, *s;
    char		*modstr = NULL;
    char		*tokstr = NULL;
    uint32		fvalue[SOC_MAX_MEM_FIELD_WORDS];
    uint32		fincr[SOC_MAX_MEM_FIELD_WORDS];
    int			i, entry_dw;
    soc_mem_info_t	*m = &SOC_MEM_INFO(unit, mem);
    char *mod_ptr = NULL;
    soc_field_t key_type_fid = 0;
    soc_field_t data_type_fid = 0;
    int key_type = 0;
    int data_type = 0; 
    int flex_view = FALSE;
    soc_mem_t mem_view_id[VIEW_MULTI_ID_MAX];
    soc_flow_db_mem_view_field_info_t view_field;
    int field_type;
    uint32 cnt;
    int ix = 0;
    uint32 field_id[FIELD_ID_ARRAY_SIZE];
    int field_found;
    soc_field_info_t soc_field;
    char *field_name;
    int iy;

    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return CMD_NFND;
    }
    /* check if flow_db is initialized */
    rv = soc_flow_db_status_get(unit);

    if (SOC_FAILURE(rv)) {
        return CMD_NFND;
    }

    entry_dw = BYTES2WORDS(m->bytes);

    if ((modstr = sal_alloc(ARGS_BUFFER, "modify_mem")) == NULL) {
        cli_out("modify_mem_fields: Out of memory\n");
        return CMD_FAIL;
    }

    sal_strncpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;

    /* For flex view entry modification, user should always specify the 
     * key_type if applicable and data_type.
     */
    mod_ptr = modstr;
    while ((fmod = sal_strtok_r(mod_ptr, ",", &tokstr)) != 0) {
        mod_ptr = NULL;                 /* Pass strtok NULL next time */
        fval = strchr(fmod, '=');
        if (fval != NULL) {       /* Point fval to arg, NULL if none */
            *fval++ = 0;          /* Now fmod holds only field name. */

            if (!sal_strcasecmp(fmod, "KEY_TYPE0")) {
                key_type_fid = KEY_TYPE0f;
                parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
                key_type = fvalue[0];
            } else if (!sal_strcasecmp(fmod, "KEY_TYPE")) {
                key_type_fid = KEY_TYPEf;
                parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
                key_type = fvalue[0];
            } else if (!sal_strcasecmp(fmod, "DATA_TYPE0")) {
                data_type_fid = DATA_TYPE0f;
                parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
                data_type = fvalue[0];
            } else if (!sal_strcasecmp(fmod, "DATA_TYPE")) {
                data_type_fid = DATA_TYPEf;
                parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
                data_type = fvalue[0];
            }
        }  
    }
    if (key_type_fid || data_type_fid) {
        if (!key_type) {
            rv = soc_flow_db_mem_to_view_id_multi_get (unit, mem,
                       SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                       0, NULL, VIEW_MULTI_ID_MAX, (uint32 *)mem_view_id);
        } else {
            rv = soc_flow_db_mem_to_view_id_multi_get (unit, mem, 
                    key_type, data_type, 0, NULL, VIEW_MULTI_ID_MAX,
                    (uint32 *)mem_view_id);
        }
        /* if error, fall thru to try non-flex action */
        if (rv == SOC_E_NONE) {
            flex_view = TRUE;
        }
    }

    if (flex_view == FALSE) {
        sal_free(modstr);
        return CMD_NFND;
    }

    /* restore back modstr */
    sal_strncpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;
    mod = modstr;

    if (mask) {
	memset(mask, 0, entry_dw * 4);
    }

    while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0) {
	mod = NULL;			/* Pass strtok NULL next time */
	fval = strchr(fmod, '=');
	if (fval != NULL) {		/* Point fval to arg, NULL if none */
	    *fval++ = 0;		/* Now fmod holds only field name. */
	}
	if (fmod[0] == 0) {
	    cli_out("Null field name\n");
            sal_free(modstr);
	    return CMD_FAIL;
	}
	if (!sal_strcasecmp(fmod, "clear")) {
	    sal_memset(entry, 0, entry_dw * sizeof (*entry));
	    if (mask) {
		memset(mask, 0xff, entry_dw * sizeof (*entry));
	    }
	    continue;
	}
        field_found= FALSE;
        for (iy = 0; iy < VIEW_MULTI_ID_MAX; iy++) {
            if (!soc_flow_db_mem_view_is_valid(unit, mem_view_id[iy])) {
                continue;
            }

            for (field_type = SOC_FLOW_DB_FIELD_TYPE_CONTROL;
                    field_type < SOC_FLOW_DB_FIELD_TYPE_MAX; field_type++) {
                rv = soc_flow_db_mem_view_field_list_get(unit, mem_view_id[iy],
                                 field_type, FIELD_ID_ARRAY_SIZE, field_id, &cnt);
                if (SOC_SUCCESS(rv)) {
                    for (ix = 0; ix < cnt; ix++) {
                        if (field_type == SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY ||
                            field_type == 
                               SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA  ||
                            field_type == 
                               SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT) {
                            (void) soc_flow_db_mem_view_field_name_get(unit, 
                                       mem_view_id[iy], field_id[ix], &field_name);
                        }
                        else {
                            field_name = SOC_FIELD_NAME(unit,field_id[ix]);
                        }

                        if (!sal_strcasecmp(fmod, field_name)) {
                            field_found=TRUE;
                            break;
                        }
                    }
                }

                if (field_found == TRUE) {
                    break;
                }
            }
            if (field_found == TRUE) {
                break;
            }
        }
        if (field_found == TRUE) {
            rv = soc_flow_db_mem_view_field_info_get(unit,mem_view_id[iy],
                   field_id[ix], &view_field);
        }
        if ((field_found == FALSE) || SOC_FAILURE(rv)) {
            sal_free(modstr);
            return CMD_NFND;
        }
        soc_field.field = field_id[ix];
        soc_field.len = view_field.width;
        soc_field.bp = view_field.offset;
        soc_field.flags = view_field.flags;
        fld = &soc_field;
        mem = mem_view_id[iy];

	if (!fval) {
	    cli_out("Missing %d-bit value to assign to \"%s\" field \"%s\".\n",
                    fld->len,
                    SOC_MEM_UFNAME(unit, mem), field_name);
            sal_free(modstr);
	    return -1;
	}
	s = strchr(fval, '+');
	if (s == NULL) {
	    s = strchr(fval, '-');
	}
	if (s == fval) {
	    s = NULL;
	}
	if (incr) {
	    if (s != NULL) {
		parse_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS,
				   s[1] ? &s[1] : "1");
		if (*s == '-') {
		    neg_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS);
		}
		if (fld->len & 31) {
		    /* Proper treatment of sign extension */
		    fincr[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
		}
		soc_mem_field_get(unit, mem, entry, fld->field, fvalue);
		add_long_integer(fvalue, fincr, SOC_MAX_MEM_FIELD_WORDS);
		if (fld->len & 31) {
		    /* Proper treatment of sign extension */
		    fvalue[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
		}
		soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
	    }
	} else {
	    if (s != NULL) {
		*s = 0;
	    }
	    parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
	    for (i = fld->len; i < SOC_MAX_MEM_FIELD_BITS; i++) {
		if (fvalue[i / 32] & 1 << (i & 31)) {
		    cli_out("Value \"%s\" too large for "
                            "%d-bit field \"%s\".\n",
                            fval, fld->len, field_name);
                    sal_free(modstr);
		    return CMD_FAIL;
		}
	    }
	    soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
	}
	if (mask) {
	    sal_memset(fvalue, 0, sizeof (fvalue));
	    for (i = 0; i < fld->len; i++) {
		fvalue[i / 32] |= 1 << (i & 31);
	    }
	    soc_mem_field_set(unit, mem, mask, fld->field, fvalue);
	}
    }

    sal_free(modstr);
    return CMD_OK;
#else
    return CMD_NFND;
#endif
}
/*
 * modify_mem_fields
 *
 *   Verify similar to modify_reg_fields (see reg.c) but works on
 *   memory table entries instead of register values.  Handles fields
 *   of any length.
 *
 *   If mask is non-NULL, it receives an entry which is a mask of all
 *   fields modified.
 *
 *   Values may be specified with optional increment or decrement
 *   amounts; for example, a MAC address could be 0x1234+2 or 0x1234-1
 *   to specify an increment of +2 or -1, respectively.
 *
 *   If incr is FALSE, the increment is ignored and the plain value is
 *   stored in the field (e.g. 0x1234).
 *
 *   If incr is TRUE, the value portion is ignored.  Instead, the
 *   increment value is added to the existing value of the field.  The
 *   field value wraps around on overflow.
 *
 *   Returns -1 on failure, 0 on success.
 */

static int
_modify_mem_fields(int unit, soc_mem_t mem, uint32 *entry,
		  uint32 *mask, char *mod, int incr)
{
    soc_field_info_t	*fld;
    char		*fmod, *fval, *s;
    char		*modstr = NULL;
    char		*tokstr = NULL;
    uint32		fvalue[SOC_MAX_MEM_FIELD_WORDS];
    uint32		fincr[SOC_MAX_MEM_FIELD_WORDS];
    int			i, entry_dw;
    soc_mem_info_t	*m = &SOC_MEM_INFO(unit, mem);

    entry_dw = BYTES2WORDS(m->bytes);

    if ((modstr = sal_alloc(ARGS_BUFFER, "modify_mem")) == NULL) {
        cli_out("modify_mem_fields: Out of memory\n");
        return CMD_FAIL;
    }

    sal_strncpy(modstr, mod, ARGS_BUFFER);/* Don't destroy input string */
    modstr[ARGS_BUFFER - 1] = 0;
    mod = modstr;

    if (mask) {
	memset(mask, 0, entry_dw * 4);
    }

    while ((fmod = sal_strtok_r(mod, ",", &tokstr)) != 0) {
	mod = NULL;			/* Pass strtok NULL next time */
	fval = strchr(fmod, '=');
	if (fval != NULL) {		/* Point fval to arg, NULL if none */
	    *fval++ = 0;		/* Now fmod holds only field name. */
	}
	if (fmod[0] == 0) {
	    cli_out("Null field name\n");
            sal_free(modstr);
	    return -1;
	}
	if (!sal_strcasecmp(fmod, "clear")) {
	    sal_memset(entry, 0, entry_dw * sizeof (*entry));
	    if (mask) {
		memset(mask, 0xff, entry_dw * sizeof (*entry));
	    }
	    continue;
	}
	for (fld = &m->fields[0]; fld < &m->fields[m->nFields]; fld++) {
	    if (!sal_strcasecmp(fmod, SOC_FIELD_NAME(unit, fld->field))) {
		break;
	    }
	}
	if (fld == &m->fields[m->nFields]) {
	    cli_out("No such field \"%s\" in memory \"%s\".\n",
                    fmod, SOC_MEM_UFNAME(unit, mem));
            sal_free(modstr);
	    return -1;
	}
	if (!fval) {
	    cli_out("Missing %d-bit value to assign to \"%s\" field \"%s\".\n",
                    fld->len,
                    SOC_MEM_UFNAME(unit, mem), SOC_FIELD_NAME(unit, fld->field));
            sal_free(modstr);
	    return -1;
	}
	s = strchr(fval, '+');
	if (s == NULL) {
	    s = strchr(fval, '-');
	}
	if (s == fval) {
	    s = NULL;
	}
	if (incr) {
	    if (s != NULL) {
		parse_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS,
				   s[1] ? &s[1] : "1");
		if (*s == '-') {
		    neg_long_integer(fincr, SOC_MAX_MEM_FIELD_WORDS);
		}
		if (fld->len & 31) {
		    /* Proper treatment of sign extension */
		    fincr[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
		}
		soc_mem_field_get(unit, mem, entry, fld->field, fvalue);
		add_long_integer(fvalue, fincr, SOC_MAX_MEM_FIELD_WORDS);
		if (fld->len & 31) {
		    /* Proper treatment of sign extension */
		    fvalue[fld->len / 32] &= ~(0xffffffff << (fld->len & 31));
		}
		soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
	    }
	} else {
	    if (s != NULL) {
		*s = 0;
	    }
	    parse_long_integer(fvalue, SOC_MAX_MEM_FIELD_WORDS, fval);
	    for (i = fld->len; i < SOC_MAX_MEM_FIELD_BITS; i++) {
		if (fvalue[i / 32] & 1 << (i & 31)) {
		    cli_out("Value \"%s\" too large for "
                            "%d-bit field \"%s\".\n",
                            fval, fld->len, SOC_FIELD_NAME(unit, fld->field));
                    sal_free(modstr);
		    return -1;
		}
	    }
	    soc_mem_field_set(unit, mem, entry, fld->field, fvalue);
	}
	if (mask) {
	    sal_memset(fvalue, 0, sizeof (fvalue));
	    for (i = 0; i < fld->len; i++) {
		fvalue[i / 32] |= 1 << (i & 31);
	    }
	    soc_mem_field_set(unit, mem, mask, fld->field, fvalue);
	}
    }

    sal_free(modstr);
    return 0;
}

static int
modify_mem_fields(int unit, soc_mem_t mem, uint32 *entry,
		  uint32 *mask, char *mod, int incr)
{
    int rv;

    rv = _modify_flex_mem_fields(unit,mem,entry,mask,mod,incr);
    if (rv == CMD_NFND) {
        return _modify_mem_fields(unit,mem,entry,mask,mod,incr);
    } else {
        return rv;
    }
}

/*
 * mmudebug command
 *    No argument: print current state
 *    Argument is 1: enable
 *    Argument is 0: disable
 */

char mmudebug_usage[] =
    "Parameters: [on | off]\n\t"
    "Puts the MMU in debug mode (on) or normal mode (off).\n\t"
    "With no parameter, displays the current mode.\n\t"
    "MMU must be in debug mode to access CBP memories.\n";

cmd_result_t
mem_mmudebug(int unit, args_t *a)
{
    char *s = ARG_GET(a);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (s) {
	if (!sal_strcasecmp(s, "on")) {
	    cli_out("Entering debug mode ...\n");
	    if (soc_mem_debug_set(unit, 1) < 0) {
                return CMD_FAIL;
            }
	} else if (!sal_strcasecmp(s, "off")) {
	    cli_out("Leaving debug mode ...\n");
	    if (soc_mem_debug_set(unit, 0) < 0) {
                return CMD_FAIL;
            }
	} else
	    return CMD_USAGE;
    } else {
	int enable = 0;
	soc_mem_debug_get(unit, &enable);
	cli_out("MMU debug mode is %s\n", enable ? "on" : "off");
    }

    return CMD_OK;
}

/*
 * Initialize Cell Free Address pool.
 */

char cfapinit_usage[] =
    "Parameters: none\n\t"
    "Run diags on CFAP pool and initialize CFAP with good entries only\n";

cmd_result_t
mem_cfapinit(int unit, args_t *a)
{
    int r;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((r = soc_mem_cfap_init(unit)) < 0) {
	cli_out("NOTICE: error initializing CFAP: %s\n", soc_errmsg(r));
	return CMD_FAIL;
    }

    return CMD_OK;
}

#ifdef BCM_HERCULES_SUPPORT
/*
 * Initialize Cell Free Address pool.
 */

char llainit_usage[] =
    "Parameters: [force]\n\t"
    "Run diags on LLA and PP and initialize LLA with good entries only\n";

cmd_result_t
mem_llainit(int unit, args_t *a)
{
    char *argforce;
    int force = FALSE;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((argforce = ARG_GET(a)) != NULL) {
        if (!sal_strcasecmp(argforce, "force")) {
            force = TRUE;
        } else {
            return CMD_USAGE;
        }
    }

    if (force) {
        if (SOC_CONTROL(unit)->lla_map != NULL) {
            int i;
	    PBMP_PORT_ITER(unit, i) {
                if (SOC_CONTROL(unit)->lla_map[i] != NULL) {
                    sal_free(SOC_CONTROL(unit)->lla_map[i]);
                    SOC_CONTROL(unit)->lla_map[i] = NULL;
                }
            }
        }
    }

    return CMD_OK;
}
#endif

static int
parse_dwords(int count, uint32 *dw, args_t *a)
{
    char	*s;
    int		i;

    for (i = 0; i < count; i++) {
	if ((s = ARG_GET(a)) == NULL) {
	    cli_out("Not enough data values (have %d, need %d)\n",
                    i, count);
	    return -1;
	}

	dw[i] = parse_integer(s);
    }

    if (ARG_CNT(a) > 0) {
	cli_out("Ignoring extra data on command line "
                "(only %d words needed)\n",
                count);
    }

    return 0;
}

cmd_result_t
cmd_esw_mem_write(int unit, args_t *a)
{
    int			i, index, start, count, copyno;
    char		*tab, *idx, *cnt, *s, *memname, *slam_buf = NULL;
    soc_mem_t		mem;
    uint32		entry[SOC_MAX_MEM_WORDS];
    int			entry_dw, view_len, entry_bytes;
    char		copyno_str[8];
    int			r, update;
    int			rv = CMD_FAIL;
    char		*valstr = NULL, *view = NULL;
    int			no_cache = 0;
    int                 use_slam = 0;
    int			acc_type = 0;
    int			specific_pipe = 0;
#ifdef BCM_KATANA2_SUPPORT
    int port_index[15]=
    {1,5,9,13,17,21,25,26,27,28,30,33,37,39,41};
    int matched = 0;
#endif /*BCM_KATANA2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
    int         enable_mem_mode_check = 1;
#endif /* BCM_TOMAHAWK_SUPPORT */
    int         idx_phy = 0;
    uint32      flags = SOC_MEM_NO_FLAGS;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    tab = ARG_GET(a);
    if (tab != NULL && sal_strcasecmp(tab, "idx_phy") == 0) {
        idx_phy = 1;
        tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "nocache") == 0) {
	no_cache = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe_x") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe_y") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe0") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_0;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe1") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_1;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe2") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_2;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe3") == 0) {
	acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_3;
        specific_pipe = 1;
	tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe4") == 0) {
    acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_4;
        specific_pipe = 1;
    tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe5") == 0) {
    acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_5;
        specific_pipe = 1;
    tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe6") == 0) {
    acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_6;
        specific_pipe = 1;
    tab = ARG_GET(a);
    }
    if (tab != NULL && sal_strcasecmp(tab, "pipe7") == 0) {
    acc_type = _SOC_MEM_ADDR_ACC_TYPE_PIPE_7;
        specific_pipe = 1;
    tab = ARG_GET(a);
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (tab != NULL && sal_strcasecmp(tab, "nocheck_mem_mode") == 0) {
        enable_mem_mode_check = 0;
	tab = ARG_GET(a);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /* you will need at least one value and all the args .. */
    if (!tab || !idx || !cnt || !s || !isint(cnt)) {
	return CMD_USAGE;
    }

    /* Deal with VIEW:MEMORY if applicable */
    memname = sal_strstr(tab, ":");
    view_len = 0;
    if (memname != NULL) {
        memname++;
        view_len = memname - tab;
    } else {
        memname = tab;
    }

    if (parse_memory_name(unit, &mem, memname, &copyno, 0) < 0) {
	cli_out("ERROR: unknown table \"%s\"\n",tab);
	goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && (soc_mem_index_max(unit, mem) <= 0)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    if (soc_mem_is_readonly(unit, mem)) {
        cli_out("Error: Table %s is read-only\n",
                SOC_MEM_UFNAME(unit, mem));
        goto done;
    }

    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

    if (copyno == COPYNO_ALL) {
	copyno_str[0] = 0;
    } else {
	sal_sprintf(copyno_str, ".%d", copyno);
    }

    entry_dw = soc_mem_entry_words(unit, mem);

    if ((valstr = sal_alloc(ARGS_BUFFER, "reg_set")) == NULL) {
        cli_out("cmd_esw_mem_write : Out of memory\n");
        goto done;
    }

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */

    if (!isint(s)) {
	/* List of fields */

	if (view_len == 0) {
            collect_comma_args(a, valstr, s);
        } else {
            if ((view = sal_alloc(view_len + 1, "view_name")) == NULL) {
                cli_out("cmd_esw_mem_write : Out of memory\n");
                goto done;
            }
            sal_memset(view, 0, view_len + 1);
            sal_memcpy(view, tab, view_len);
            if (collect_comma_args_with_view(a, valstr, s, view, unit, mem) < 0) {
                cli_out("Out of memory: aborted\n");
                goto done;
            }
        }

	memset(entry, 0, sizeof (entry));

	if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
	    cli_out("Syntax error: aborted\n");
	    goto done;
	}

	update = TRUE;
    } else {
	/* List of numeric values */

	ARG_PREV(a);

	if (parse_dwords(entry_dw, entry, a) < 0) {
	    goto done;
	}

	update = FALSE;
    }

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
	cli_out("WRITE[%s%s], DATA:", SOC_MEM_UFNAME(unit, mem), copyno_str);
	for (i = 0; i < entry_dw; i++) {
	    cli_out(" 0x%x", entry[i]);
	}
	cli_out("\n");
    }

    /*
     * Created entry, now write it
     */

    use_slam = soc_property_get(unit, spn_DIAG_SHELL_USE_SLAM, 0);
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        if (enable_mem_mode_check &&
            soc_th_mem_is_dual_mode(unit, mem, NULL, NULL)) {
            use_slam = 0; /* simplify */
            LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                "Write slam DISABLED for mem %s\n"),
                SOC_MEM_NAME(unit, mem)));
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    if (use_slam && count > 1) {
        entry_bytes = WORDS2BYTES(soc_mem_entry_words(unit, mem));
        slam_buf = soc_cm_salloc(unit, count * entry_bytes, "slam_entry");
        if (slam_buf == NULL) {
            cli_out("cmd_esw_mem_write : Out of memory\n");
            goto done;
        }
        for (i = 0; i < count; i++) {
            sal_memcpy(slam_buf + i * entry_bytes, entry, entry_bytes);
        }
        r = soc_mem_write_range(unit, mem, copyno, start, start + count - 1, slam_buf);
        soc_cm_sfree(unit, slam_buf);
        if (r < 0) {
            cli_out("Slam ERROR: table %s.%s: %s\n",
                    SOC_MEM_UFNAME(unit, mem), copyno_str, soc_errmsg(r));
            goto done;
        }
        for (index = start; index < start + count; index++) {
	    if (update) {
	        modify_mem_fields(unit, mem, entry, NULL, valstr, TRUE);
	    }
        }
    } else {
        for (index = start; index < start + count; index++) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                /* In case of SP memories, out of SP0, SP1, SP2 & SP3                 
                   only SP0 is valid. i.e only inidices 0,4,8. This check 
                   is to avoid processing invalid indices. Issue is handled
                   for THDI memories alone. Has to be handled
                   for other SP memories in future */

                if ((mem == THDIEMA_THDI_PORT_SP_CONFIGm ||
                            mem == THDIEXT_THDI_PORT_SP_CONFIGm ||
                            mem == THDIQEN_THDI_PORT_SP_CONFIGm ||
                            mem == THDIRQE_THDI_PORT_SP_CONFIGm ||
                            mem == THDI_PORT_SP_CONFIGm) && (index % 4)) {

                    continue;
                }
                if(!SOC_IS_SABER2(unit)) {   
                    matched = 0;
                    if ((mem == THDIEMA_THDI_PORT_PG_CONFIGm ||
                         mem == THDIEXT_THDI_PORT_PG_CONFIGm ||
                         mem == THDIQEN_THDI_PORT_PG_CONFIGm ||
                         mem == THDIRQE_THDI_PORT_PG_CONFIGm ||
                         mem == THDI_PORT_PG_CONFIGm )) {
                         for (i = 0; i < 15 ; i ++) {
                             if (port_index[i] == (index/8)) { 
                                matched = 1; 
                                break; 
                             }
                         }

                         if (!matched && (index % 8)) {
                             continue;
                         }
                    }

                }
            }

#endif /*BCM_KATANA2_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
            if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
                if (enable_mem_mode_check) {
                    if (!soc_th_dual_tcam_index_valid(unit, mem, index)) {
                        LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                            "Write IGNORED for mem %s, index %d\n"),
                            SOC_MEM_NAME(unit, mem), index));
                        continue;
                    }
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */

            if (idx_phy) {
                flags |= SOC_MEM_DONT_MAP_INDEX;
            }
            if (no_cache) {
                flags |= SOC_MEM_DONT_USE_CACHE;
            }
            if (specific_pipe) {
                if ((r = soc_mem_pipe_select_write(unit,
                                                   flags,
                                                   mem, copyno, acc_type, index,
                                                   entry)) < 0) {
                    cli_out("Write ERROR: table %s(Y).%s[%d]: %s\n",
                            SOC_MEM_UFNAME(unit, mem), copyno_str,
                            index, soc_errmsg(r));
                    goto done;
                }
            } else if ((r = soc_mem_write_extended(unit,
                                                   flags,
                                                   mem,
                                                   copyno, index, entry)) < 0) {
	        cli_out("Write ERROR: table %s.%s[%d]: %s\n",
                        SOC_MEM_UFNAME(unit, mem), copyno_str,
                        index, soc_errmsg(r));
	        goto done;
	    }

	    if (update) {
	        modify_mem_fields(unit, mem, entry, NULL, valstr, TRUE);
	    }
        }
    }
    rv = CMD_OK;

 done:
    if (valstr != NULL) {
       sal_free(valstr);
    }
    if (view != NULL) {
       sal_free(view);
    }
    return rv;
}

#if defined (BCM_FIRELIGHT_SUPPORT)
cmd_result_t
cmd_esw_coupled_mem_write(int unit, args_t *a)
{
    int rv = CMD_FAIL;
    int index, start, count;
    int copynotable1, copynotable2;
    char *table1_tab, *table2_tab, *idx, *cnt, *s_table1, *s_table2;
    char *table1memname, *table2memname;
    soc_mem_t table1mem, table2mem;
    uint32 entry_table1[SOC_MAX_MEM_WORDS];
    uint32 entry_table2[SOC_MAX_MEM_WORDS];
    int phy_index[2] = {0, 0};
    int copyno[2];
    uint32 *entry_data[2];
    static soc_mem_t bulk_mem[2];

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    table1_tab = ARG_GET(a);
    table2_tab = ARG_GET(a);
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s_table1 = ARG_GET(a);
    s_table2 = ARG_GET(a);

    /* you will need at least one dword and all the args .. */
    if (!table1_tab || !table2_tab || !idx || !cnt || !s_table1 || !s_table2 ||
        !isint(cnt)) {
        return CMD_USAGE;
    }

    table1memname = table1_tab;
    table2memname = table2_tab;

    if (parse_memory_name(unit, &table1mem, table1memname,
                          &copynotable1, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", table1_tab);
        return rv;
    }

    if (parse_memory_name(unit, &table2mem, table2memname,
                          &copynotable2, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", table2_tab);
        return rv;
    }

    /* Currently, only support Firelight
       table1 = ISEC_SP_TCAM_KEY and table2 = ISEC_SP_TCAM_MASK */
    if (!SOC_IS_FIRELIGHT(unit) ||
        !(table1mem == ISEC_SP_TCAM_KEYm && table2mem == ISEC_SP_TCAM_MASKm)) {
            cli_out("Error: Table %s and %s not valid for this command.\n",
                    SOC_MEM_UFNAME(unit, table1mem),
                    SOC_MEM_UFNAME(unit, table2mem));
            return rv;
    }

    if (!SOC_MEM_IS_VALID(unit, table1mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, table1mem), SOC_UNIT_NAME(unit));
        return rv;
    } else if (!SOC_MEM_IS_VALID(unit, table2mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, table2mem), SOC_UNIT_NAME(unit));
        return rv;
    }

    if (soc_mem_is_readonly(unit, table1mem)) {
        cli_out("ERROR: Table %s is read-only\n",
                SOC_MEM_UFNAME(unit, table1mem));
        return rv;
    } else if (soc_mem_is_readonly(unit, table2mem)) {
        cli_out("ERROR: Table %s is read-only\n",
                SOC_MEM_UFNAME(unit, table2mem));
        return rv;
    }

    bulk_mem[0] = table1mem;
    bulk_mem[1] = table2mem;

    memset(entry_table1, 0, sizeof (entry_table1));
    memset(entry_table2, 0, sizeof (entry_table2));

    modify_mem_fields(unit, table1mem, entry_table1, NULL, s_table1, FALSE);
    modify_mem_fields(unit, table2mem, entry_table2, NULL, s_table2, FALSE);

    start = parse_memory_index(unit, table1mem, idx);
    count = parse_integer(cnt);

    /*
     * Take lock to ensure atomic modification of memory.
     */
    soc_mem_lock(unit, table1mem);
    soc_mem_lock(unit, table2mem);

    rv = CMD_OK;
    copyno[0] = copynotable1;
    copyno[1] = copynotable2;
    for (index = start; index < start + count; index++) {
        if (copyno[0] != COPYNO_ALL && copyno[1] != COPYNO_ALL) {
            continue;
        }

        phy_index[0] = index;
        phy_index[1] = index;

        entry_data[0] = entry_table1;
        entry_data[1] = entry_table2;
        soc_mem_bulk_write(unit, bulk_mem, phy_index, copyno, entry_data, 2);
    }
    soc_mem_unlock(unit, table1mem);
    soc_mem_unlock(unit, table2mem);

    return rv;
}
#endif

/*
 * Modify the fields of a table entry
 */
cmd_result_t
cmd_esw_mem_modify(int unit, args_t *a)
{
    int			index, start, count, copyno, i, view_len;
    char		*tab, *idx, *cnt, *s, *memname;
    soc_mem_t		mem;
    uint32		entry[SOC_MAX_MEM_WORDS], mask[SOC_MAX_MEM_WORDS];
    uint32		changed[SOC_MAX_MEM_WORDS];
    char		*valstr = NULL, *view = NULL;
    int			r, rv = CMD_FAIL;
    int			blk;
#ifdef BCM_TRIDENT2_SUPPORT
    int                 check_view = FALSE;
    soc_mem_t           mview = INVALIDm;
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    int port_index[15]=
    {1,5,9,13,17,21,25,26,27,28,30,33,37,39,41};
    int matched = 0;
#endif /*BCM_KATANA2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
    int         enable_mem_mode_check = 1;
    int       check_lpm_view = FALSE;
    soc_mem_t lpm_view = INVALIDm;
    int       pair_lpm = 0;
#endif /* BCM_TOMAHAWK_SUPPORT */
    uint32      flags = SOC_MEM_NO_FLAGS;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, & rev_id);
#endif

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    tab = ARG_GET(a);
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (tab != NULL && sal_strcasecmp(tab, "nocheck_mem_mode") == 0) {
        enable_mem_mode_check = 0;
        tab = ARG_GET(a);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
    if (tab != NULL && sal_strcasecmp(tab, "idx_phy") == 0) {
        flags = SOC_MEM_DONT_MAP_INDEX;
        tab = ARG_GET(a);
    }
    idx = ARG_GET(a);
    cnt = ARG_GET(a);
    s = ARG_GET(a);

    /* you will need at least one dword and all the args .. */
    if (!tab || !idx || !cnt || !s || !isint(cnt)) {
	return CMD_USAGE;
    }

    if ((valstr = sal_alloc(ARGS_BUFFER, "mem_modify")) == NULL) {
        cli_out("cmd_esw_mem_modify : Out of memory\n");
        goto done;
    }

    memname = sal_strstr(tab, ":"); 
    view_len = 0; 
    if (memname != NULL) { 
        memname++; 
        view_len = memname - tab; 
    } else { 
        memname = tab; 
    } 

    if (parse_memory_name(unit, &mem, memname, &copyno, 0) < 0) {
	cli_out("ERROR: unknown table \"%s\"\n",tab);
    goto done;
    }

    if (view_len == 0) {
        collect_comma_args(a, valstr, s);
    } else {
        if ((view = sal_alloc(view_len + 1, "view_name")) == NULL) {
            cli_out("cmd_esw_mem_modify : Out of memory\n");
            goto done;
        }

        sal_memcpy(view, tab, view_len);
        view[view_len] = 0;

        if (collect_comma_args_with_view(a, valstr, s, view, unit, mem) < 0) {
            cli_out("Out of memory: aborted\n");
            goto done;
        }
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && (soc_mem_index_max(unit, mem) <= 0)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    if (soc_mem_is_readonly(unit, mem)) {
	cli_out("ERROR: Table %s is read-only\n", SOC_MEM_UFNAME(unit, mem));
        goto done;
    }

    sal_memset(changed, 0, sizeof (changed));

    if (modify_mem_fields(unit, mem, changed, mask, valstr, FALSE) < 0) {
	cli_out("Syntax error: aborted\n");
        goto done;
    }

    start = parse_memory_index(unit, mem, idx);
    count = parse_integer(cnt);

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) &&
        (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
         mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
         mem == L3_DEFIP_ALPM_IPV6_128m)) {
        check_view = TRUE;
        mview = mem;
    }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && (flags & SOC_MEM_DONT_MAP_INDEX)) {
        if (soc_feature(unit, soc_feature_l3_defip_map) &&
            (mem == L3_DEFIPm ||
#ifdef BCM_TOMAHAWK3_SUPPORT
             mem == L3_DEFIP_LEVEL1m ||
#endif
             mem == L3_DEFIP_ONLYm ||
             mem == L3_DEFIP_PAIR_128m ||
#ifdef BCM_TOMAHAWK3_SUPPORT
             mem == L3_DEFIP_PAIR_LEVEL1m ||
#endif
             mem == L3_DEFIP_PAIR_128_ONLYm)) {
            check_lpm_view = TRUE;
            lpm_view = mem;
            pair_lpm = (mem == L3_DEFIP_PAIR_128m ||
#ifdef BCM_TOMAHAWK3_SUPPORT
                        mem == L3_DEFIP_PAIR_LEVEL1m ||
#endif
                        mem == L3_DEFIP_PAIR_128_ONLYm) ? 1 : 0;
        }
    }
#endif
    /*
     * Take lock to ensure atomic modification of memory.
     */

    soc_mem_lock(unit, mem);

    rv = CMD_OK;

    for (index = start; index < start + count; index++) {

        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
	    if (copyno != COPYNO_ALL && copyno != blk) {
		continue;
	    }

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (enable_mem_mode_check) {
                if (!soc_th_dual_tcam_index_valid(unit, mem, index)) {
                    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
                        "Modify IGNORED for mem %s, index %d\n"),
                        SOC_MEM_NAME(unit, mem), index));
                    continue;
                }
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
            if (check_view) {
                if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
                    mview = _soc_trident2_alpm_bkt_view_get(unit, index);
                }
#if defined(ALPM_ENABLE)
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                else {
                    mview = soc_alpm_cmn_bkt_view_get(unit, index);
                }
#endif
#endif
                if (!(mview == INVALIDm || mem == mview)) {
                    continue;
                }
            }
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (check_lpm_view) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                lpm_view = _soc_th3_lpm_view_get(unit, index, pair_lpm);
            } else
#endif
            if (SOC_IS_TOMAHAWKX(unit)) {
                lpm_view = _soc_th_lpm_view_get(unit, index, pair_lpm);
            }
            if (mem != lpm_view) {
                continue;
            }
        }
#endif
	    /*
	     * Retrieve the current entry, set masked fields to changed
	     * values, and write back.
	     */

            r = soc_mem_read_physical_index(unit, flags, mem, blk, index, entry);

#ifdef BCM_TOMAHAWK3_SUPPORT
        if ( r == BCM_E_UNAVAIL && dev_id == BCM56982_DEVICE_ID) {
            soc_mem_info_t *memp;
            memp = &SOC_MEM_INFO(unit, mem);
            if ((memp->pipe_stage > 14 && memp->pipe_stage < 23) &&
                    (SOC_BLOCK_IS_CMP(unit, SOC_MEM_BLOCK_MIN(unit, mem),
                                            SOC_BLK_EPIPE))) {
                continue;
            }
        }
#endif
	    if (r < 0) {
		cli_out("ERROR: read from %s table copy %d failed: %s\n",
                        SOC_MEM_UFNAME(unit, mem), blk, soc_errmsg(r));
		rv = CMD_FAIL;
		break;
	    }

	    for (i = 0; i < SOC_MAX_MEM_WORDS; i++) {
		entry[i] = (entry[i] & ~mask[i]) | changed[i];
	    }

#ifdef BCM_KATANA2_SUPPORT

        if (SOC_IS_KATANA2(unit)) {
           /* In case of SP memories, out of SP0, SP1, SP2 & SP3                 
              only SP0 is valid i.e. only indices 0,4,8.This check
              is avoid processing invalid indices. Issue is handled
              for THDI memories alone. Has to be handled
              for other SP memories in future */

              if ((mem == THDIEMA_THDI_PORT_SP_CONFIGm ||
                   mem == THDIEXT_THDI_PORT_SP_CONFIGm ||
                   mem == THDIQEN_THDI_PORT_SP_CONFIGm ||
                   mem == THDIRQE_THDI_PORT_SP_CONFIGm ||
                   mem == THDI_PORT_SP_CONFIGm) && (index % 4)) { 
                   continue;
              }
              if (!SOC_IS_SABER2(unit)) {
                  matched = 0;
                   if ((mem == THDIEMA_THDI_PORT_PG_CONFIGm ||
                         mem == THDIEXT_THDI_PORT_PG_CONFIGm ||
                         mem == THDIQEN_THDI_PORT_PG_CONFIGm ||
                         mem == THDIRQE_THDI_PORT_PG_CONFIGm ||
                         mem == THDI_PORT_PG_CONFIGm )) {
                       for (i = 0; i < 15 ; i ++) {
                            if (port_index[i] == (index/8)) { 
                                matched = 1; 
                                break; 
                            }
                       }

                       if (!matched && (index % 8)) {
                           continue;
                       }
                   }
              }
        }

#endif /*BCM_KATANA2_SUPPORT */
            
        r = soc_mem_write_extended(unit, flags, mem, blk, index, entry);

	    if (r < 0) {
		    cli_out("ERROR: write to %s table copy %d failed: %s\n",
                    SOC_MEM_UFNAME(unit, mem), blk, soc_errmsg(r));
		    rv = CMD_FAIL;
		    break;
	    }
	}

	if (rv != CMD_OK) {
	    break;
	}
	modify_mem_fields(unit, mem, changed, NULL, valstr, TRUE);
    }

    soc_mem_unlock(unit, mem);

 done:
    if (view != NULL) {
        sal_free(view);
    }
    sal_free(valstr);
    return rv;
}

/*
 * Auxiliary routine to perform either insert or delete
 */

#define TBL_OP_INSERT		0
#define TBL_OP_DELETE		1
#define TBL_OP_LOOKUP		2

static cmd_result_t
do_ins_del_lkup(int unit, args_t *a, int tbl_op)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    uint32		entry[SOC_MAX_MEM_WORDS];
    uint32		result[SOC_MAX_MEM_WORDS];
    int			entry_dw = 0, index;
    char		*ufname, *s;
    int			rv = CMD_FAIL;
    char		valstr[1024];
    soc_mem_t		mem;
    int			copyno, update, view_len = 0;
    int			swarl = 0;	/* Indicates operating on
					   software ARL/L2 table */
    char		*tab, *memname, *view = NULL;
    int			count = 1;
    soc_mem_t		arl_mem = INVALIDm;
    shr_avl_compare_fn	arl_cmp = NULL;
    int			quiet = 0;
    int32               banks = 0;
    int			i=0;
    int8                banknum = SOC_MEM_HASH_BANK_ALL;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    for (;;) {
	if ((tab = ARG_GET(a)) == NULL) {
	    return CMD_USAGE;
	}

	if (isint(tab)) {
	    count = parse_integer(tab);
	    continue;
	}

	if (sal_strcasecmp(tab, "quiet") == 0) {
	    quiet = 1;
	    continue;
	}
        
        if (soc_feature(unit, soc_feature_ism_memory) ||
            soc_feature(unit, soc_feature_shared_hash_mem)) {
            char *bnum;
            int8 num = -1;
            bnum = sal_strstr(tab, "bank");
            if (bnum != NULL) {
                num = sal_atoi(bnum+4);
                if (num >= 0 && num < 20) { 
                    banknum = num;
                }
	        continue;
            }
            
        } else {
	    if (sal_strcasecmp(tab, "bank0") == 0) {
	        banks = SOC_MEM_HASH_BANK0_ONLY;
	        continue;
	    }

	    if (sal_strcasecmp(tab, "bank1") == 0) {
	        banks = SOC_MEM_HASH_BANK1_ONLY;
	        continue;
	    }
        }  

	break;
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
	arl_mem = L2Xm;
	arl_cmp = soc_l2x_entry_compare_key;
    }
#endif

    if (!sal_strcasecmp(tab, "sa") || !sal_strcasecmp(tab, "sarl")) {
	if (soc->arlShadow == NULL) {
	    cli_out("ERROR: No software ARL table\n");
	    goto done;
	}
	mem = arl_mem;
        if (mem != INVALIDm) {
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
	swarl = 1;
    } else {
        memname = sal_strstr(tab, ":");
  	if (memname != NULL) {
  	    memname++;
  	    view_len = memname - tab;
  	} else {
  	    memname = tab;
  	}
        if (parse_memory_name(unit, &mem, memname, &copyno, 0) < 0) {
	    cli_out("ERROR: unknown table \"%s\"\n", tab);
	    goto done;
        }
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                (mem != INVALIDm) ? SOC_MEM_UFNAME(unit, mem) : "INVALID",
                SOC_UNIT_NAME(unit));
        goto done;
    }

    if (!soc_mem_is_sorted(unit, mem) &&
	!soc_mem_is_hashed(unit, mem) &&
        !soc_mem_is_cam(unit, mem) &&
        !soc_mem_is_cmd(unit, mem)) {
	cli_out("ERROR: %s table is not sorted, hashed, CAM, or command\n",
                SOC_MEM_UFNAME(unit, mem));
	goto done;
    }

    if ((soc_feature(unit, soc_feature_ism_memory) && soc_mem_is_mview(unit, mem)) ||
        (soc_feature(unit, soc_feature_shared_hash_mem) && ((mem == L2Xm) ||
         (mem == L3_ENTRY_ONLYm) || (mem == L3_ENTRY_IPV4_UNICASTm) || 
         (mem == L3_ENTRY_IPV4_MULTICASTm) || (mem == L3_ENTRY_IPV6_UNICASTm) || 
         (mem == L3_ENTRY_IPV6_MULTICASTm)))) {
        if (banknum != SOC_MEM_HASH_BANK_ALL) {
            banks |= ((int32)1 << banknum);
        } else if (banks == 0) {
            banks = SOC_MEM_HASH_BANK_ALL;
        } 
    }
    /* All hash tables have multiple banks from TRX onwards only */
#ifndef BCM_TRX_SUPPORT
    if (soc_feature(unit, soc_feature_dual_hash) && !SOC_IS_TRX(unit)) {
        if (banks != 0) {
            switch (mem) {
            case L2Xm:
            case MPLS_ENTRYm:
            case VLAN_XLATEm:
            case EGR_VLAN_XLATEm: 
            case L3_ENTRY_IPV4_UNICASTm:
            case L3_ENTRY_IPV4_MULTICASTm:
            case L3_ENTRY_IPV6_UNICASTm:
            case L3_ENTRY_IPV6_MULTICASTm:
                 break;
            default:
                cli_out("ERROR: %s table does not have multiple banks\n",
                        SOC_MEM_UFNAME(unit, mem));
                goto done;
            }
        }
    }
#endif

    entry_dw = soc_mem_entry_words(unit, mem);
    ufname = (swarl ? "SARL" : SOC_MEM_UFNAME(unit, mem));

    if (tbl_op == TBL_OP_LOOKUP && copyno == COPYNO_ALL) {
	copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    /*
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */

    if ((s = ARG_GET(a)) == 0) {
	cli_out("ERROR: missing data for entry to %s\n",
                tbl_op == TBL_OP_INSERT ? "insert" :
                tbl_op == TBL_OP_DELETE ? "delete" : "lookup");
	goto done;
    }

    if (!isint(s)) {
	/* List of fields */
        sal_memset(valstr, 0, 1024);
        if (view_len == 0) {
            collect_comma_args(a, valstr, s);
	} else {
  	    if ((view = sal_alloc(view_len + 1, "view_name")) == NULL) {
  	        cli_out("cmd_esw_mem_modify : Out of memory\n");
  	        goto done;
  	    }
  	    sal_memset(view, 0, view_len + 1);
  	    sal_memcpy(view, tab, view_len);
  	    if (collect_comma_args_with_view(a, valstr, s, view, unit, mem) < 0) {
  	        cli_out("Out of memory: aborted\n");
  	        goto done;
  	    }
  	}

	memset(entry, 0, sizeof (entry));

	if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
	    cli_out("Syntax error in field specification\n");
	    goto done;
	}

	update = TRUE;
    } else {
	/* List of numeric values */

	ARG_PREV(a);

	if (parse_dwords(entry_dw, entry, a) < 0) {
	    goto done;
	}

	update = FALSE;
    }

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
	cli_out("%s[%s], DATA:",
                tbl_op == TBL_OP_INSERT ? "INSERT" :
                tbl_op == TBL_OP_DELETE ? "DELETE" : "LOOKUP", ufname);
	for (i = 0; i < entry_dw; i++) {
	    cli_out(" 0x%x", entry[i]);
	}
	cli_out("\n");
    }

    /*
     * Have entry data, now insert, delete, or lookup.
     * For delete and lookup, all fields except the key are ignored.
     * Software ARL/L2 table requires special processing.
     */

    while (count--) {
	switch (tbl_op) {
	case TBL_OP_INSERT:
	    if (swarl) {
		if (soc->arlShadow != NULL) {
		    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
		    i = shr_avl_insert(soc->arlShadow,
				       arl_cmp,
				       (shr_avl_datum_t *) entry);
		    sal_mutex_give(soc->arlShadowMutex);
		} else {
		    i = SOC_E_NONE;
		}
#ifdef BCM_TRIUMPH_SUPPORT
            } else if (mem == EXT_L2_ENTRYm) {
                i = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ANY, 0,
                                           entry, NULL, 0);
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_ISM_SUPPORT
            } else if (soc_feature(unit, soc_feature_ism_memory) && soc_mem_is_mview(unit, mem)) {
                if (banks == SOC_MEM_HASH_BANK_ALL) {
                    /* coverity[stack_use_callee] */
                    /* coverity[stack_use_overflow] */
                    i = soc_mem_insert(unit, mem, copyno, entry);
                } else {
                    i = soc_mem_bank_insert(unit, mem, banks,
                                            copyno, (void *)entry, NULL);
                }
#endif
#ifdef BCM_FIREBOLT2_SUPPORT
            } else if (banks != 0) {
                switch (mem) {
                case L2Xm:
                    if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                        i = soc_mem_bank_insert(unit, mem, banks,
                                                copyno, (void *)entry, NULL);
                    } else {
                        i = soc_fb_l2x_bank_insert(unit, banks,
                                                   (void *)entry);
                    }
                    break;
#if defined(INCLUDE_L3)
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    if (soc_feature(unit, soc_feature_shared_hash_mem)) {
                        i = soc_mem_bank_insert(unit, mem, banks,
                                                copyno, (void *)entry, NULL);
                    } else {
                        i = soc_fb_l3x_bank_insert(unit, banks,
                                   (void *)entry);
                    }
                    break;
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
                case MPLS_ENTRYm:
#endif
                case VLAN_XLATEm:
                case VLAN_MACm:
                case AXP_WRX_WCDm:
                case AXP_WRX_SVP_ASSIGNMENTm:
#if defined(BCM_TRIUMPH3_SUPPORT)
                case FT_SESSIONm:
                case FT_SESSION_IPV6m:
#endif
                    i = soc_mem_bank_insert(unit, mem, banks,
                                            copyno, (void *)entry, NULL);
                    break;
                default:
                    i = SOC_E_INTERNAL;
                    goto done;
                }
#endif /* BCM_FIREBOLT2_SUPPORT */
	    } else {
		i = soc_mem_insert(unit, mem, copyno, entry);
	    }

            if (i == SOC_E_EXISTS) {
                i = SOC_E_NONE;
            }

	    if (quiet && i == SOC_E_FULL) {
		i = SOC_E_NONE;
	    }

	    if (i < 0) {
		cli_out("Insert ERROR: %s table insert failed: %s\n",
                        ufname, soc_errmsg(i));
		goto done;
	    }

	    break;

	case TBL_OP_DELETE:
	    if (swarl) {
		if (soc->arlShadow != NULL) {
		    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
		    i = shr_avl_delete(soc->arlShadow,
				       arl_cmp,
				       (shr_avl_datum_t *) entry);
		    sal_mutex_give(soc->arlShadowMutex);
		} else {
		    i = SOC_E_NONE;
		}
#ifdef BCM_TRX_SUPPORT
            } else if (soc_feature(unit, soc_feature_generic_table_ops)) {
                i = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY,
                                           banks, entry, NULL, 0);
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            } else if (banks != 0) {
                switch (mem) {
                case L2Xm:
                    i = soc_fb_l2x_bank_delete(unit, banks,
                                               (void *)entry);
                    break;
#if defined(INCLUDE_L3)
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    i = soc_fb_l3x_bank_delete(unit, banks,
                               (void *)entry);
                    break;
#endif
                default:
                    i = SOC_E_INTERNAL;
                    goto done;
                }
#endif
	    } else {
		i = soc_mem_delete(unit, mem, copyno, entry);
	    }

	    if (quiet && i == SOC_E_NOT_FOUND) {
		i = SOC_E_NONE;
	    }

	    if (i < 0) {
		cli_out("Delete ERROR: %s table delete failed: %s\n",
                        ufname, soc_errmsg(i));
		goto done;
	    }

	    break;

	case TBL_OP_LOOKUP:
	    if (swarl) {
		if (soc->arlShadow != NULL) {
		    sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
		    i = shr_avl_lookup(soc->arlShadow,
				       arl_cmp,
				       (shr_avl_datum_t *) entry);
		    sal_mutex_give(soc->arlShadowMutex);
                    if (i) {
                        i = SOC_E_NONE;
                        index = -1;
                        sal_memcpy(result, entry,
                               soc_mem_entry_words(unit, mem) * 4);
                    } else {
                        i = SOC_E_NOT_FOUND;
                    }
		} else {
		    i = SOC_E_NONE;
                    goto done;
		}
#ifdef BCM_TRX_SUPPORT
            } else if (soc_feature(unit, soc_feature_generic_table_ops)) {
                i = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY,
                                           banks, entry, result, &index);
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT2_SUPPORT
            } else if (banks != 0) {
                switch (mem) {
                case L2Xm:
                    i = soc_fb_l2x_bank_lookup(unit, banks,
                                               (void *)entry,
                                               (void *)result,
                                               &index);
                    break;
#if defined(INCLUDE_L3)
                case L3_ENTRY_IPV4_UNICASTm:
                case L3_ENTRY_IPV4_MULTICASTm:
                case L3_ENTRY_IPV6_UNICASTm:
                case L3_ENTRY_IPV6_MULTICASTm:
                    i = soc_fb_l3x_bank_lookup(unit, banks,
                                               (void *)entry,
                                               (void *)result,
                                               &index);
                    break;
#endif
                default:
                    i = SOC_E_INTERNAL;
                    goto done;
                }
#endif /* BCM_FIREBOLT2_SUPPORT */
	    } else {
		i = soc_mem_search(unit, mem, copyno,
                                   &index, entry, result, 0);
	    }

	    if (i < 0) {		/* Error not fatal for lookup */
                if (i == SOC_E_NOT_FOUND) {
                    cli_out("Lookup: No matching entry found\n");
                } else {
                    cli_out("Lookup ERROR: read error during search: %s\n",
                            soc_errmsg(i));
                }
	    } else { /* Entry found */
		if (index < 0) {
		    cli_out("Found in %s.%s: ",
                            ufname, SOC_BLOCK_NAME(unit, copyno));
		} else {
		    cli_out("Found %s.%s[%d]: ",
                            ufname, SOC_BLOCK_NAME(unit, copyno), index);
		}
		soc_mem_entry_dump(unit, mem, result, BSL_LSS_CLI);
		cli_out("\n");
            }

	    break;

	default:
	    assert(0);
	    break;
	}

	if (update) {
	    modify_mem_fields(unit, mem, entry, NULL, valstr, TRUE);
	}
    }

    rv = CMD_OK;

 done:
    if (view != NULL) {
        sal_free(view);
    }
    return rv;
}

/*
 * Insert an entry by key into a sorted table
 */

char insert_usage[] =
    "\nParameters 1: [quiet] [<COUNT>] [bank#] <TABLE> <DW0> .. <DWN>\n"
    "Parameters 2: [quiet] [<COUNT>] <TABLE> <FIELD>=<VALUE> ...\n\t"
    "Number of <DW> must be appropriate to table entry size.\n\t"
    "Entry is inserted in ascending sorted order by key field.\n\t"
    "The quiet option indicates not to complain on table/bucket full.\n\t"
    "Some tables allow restricting the insert to a single bank.\n";

cmd_result_t
mem_insert(int unit, args_t *a)
{
    return do_ins_del_lkup(unit, a, TBL_OP_INSERT);
}


/*
 * Delete entries by key from a sorted table
 */

char delete_usage[] =
    "\nParameters 1: [quiet] [<COUNT>] [bank#] <TABLE> <DW0> .. <DWN>\n"
    "Parameters 2: [quiet] [<COUNT>] <TABLE> <FIELD>=<VALUE> ...\n\t"
    "Number of <DW> must be appropriate to table entry size.\n\t"
    "The entry is deleted by key.\n\t"
    "All fields other than the key field(s) are ignored.\n\t"
    "The quiet option indicates not to complain on entry not found.\n\t"
    "Some tables allow restricting the delete to a single bank.\n";

cmd_result_t
mem_delete(int unit, args_t *a)
{
    return do_ins_del_lkup(unit, a, TBL_OP_DELETE);
}

/*
 * Lookup entry an key in a sorted table
 */

char lookup_usage[] =
    "\nParameters 1: [<COUNT>] [bank#] <TABLE> <DW0> .. <DWN>\n"
    "Parameters 2: [<COUNT>] <TABLE> <FIELD>=<VALUE> ...\n\t"
    "Number of <DW> must be appropriate to table entry size.\n\t"
    "The entry is looked up by key.\n\t"
    "All fields other than the key field(s) are ignored.\n\t"
    "Some tables allow restricting the lookup to a single bank.\n";

cmd_result_t
mem_lookup(int unit, args_t *a)
{
    return do_ins_del_lkup(unit, a, TBL_OP_LOOKUP);
}

/*
 * Remove (delete) entries by index from a sorted table
 */

char remove_usage[] =
    "Parameters: <TABLE> <INDEX> [COUNT]\n\t"
    "Deletes an entry from a table by index.\n\t"
    "(For the ARL, reads the table entry and deletes by key).\n";

cmd_result_t
mem_remove(int unit, args_t *a)
{
    char		*tab, *ind, *cnt, *ufname;
    soc_mem_t		mem;
    int			copyno;
    int			r, rv = CMD_FAIL;
    int			index, count;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    tab = ARG_GET(a);
    ind = ARG_GET(a);
    cnt = ARG_GET(a);

    if (!tab || !ind) {
	return CMD_USAGE;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
	cli_out("ERROR: unknown table \"%s\"\n", tab);
	goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    ufname = SOC_MEM_UFNAME(unit, mem);

    if (!soc_mem_is_sorted(unit, mem) &&
	!soc_mem_is_hashed(unit, mem) &&
	!soc_mem_is_cam(unit, mem)) {
	cli_out("ERROR: %s table is not sorted, hashed, or CAM\n",
                ufname);
	goto done;
    }

    count = cnt ? parse_integer(cnt) : 1;

    index = parse_memory_index(unit, mem, ind);

    while (count-- > 0)
	if ((r = soc_mem_delete_index(unit, mem, copyno, index)) < 0) {
	    cli_out("ERROR: delete %s table index 0x%x failed: %s\n",
                    ufname, index, soc_errmsg(r));
	    goto done;
	}

    rv = CMD_OK;

 done:
    return rv;
}

/*  
 * Pop an entry from a FIFO
 */

char pop_usage[] =
    "\nParameters: [quiet] [<COUNT>] <TABLE>\n"
    "The quiet option indicates not to complain on FIFO empty.\n";

cmd_result_t
mem_pop(int unit, args_t *a)
{   
    uint32              result[SOC_MAX_MEM_WORDS];
    char                *ufname;
    int                 rv = CMD_FAIL;
    soc_mem_t           mem;
    int                 copyno;
    char                *tab; 
    int                 count = 1;
    int                 quiet = 0;
    int                 i;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_mem_push_pop)) {
        /* feature not supported */
        return CMD_FAIL;
    }

    for (;;) {
        if ((tab = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        if (isint(tab)) {
            count = parse_integer(tab);
            continue;
        }

        if (sal_strcasecmp(tab, "quiet") == 0) {
            quiet = 1;
            continue;
        }
        break;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", tab);
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    switch (mem) {
#if defined (BCM_TRIUMPH_SUPPORT)
        case ING_IPFIX_EXPORT_FIFOm:
        case EGR_IPFIX_EXPORT_FIFOm:
        case EXT_L2_MOD_FIFOm:
#endif /* BCM_TRIUMPH_SUPPORT */
        case L2_MOD_FIFOm:
#ifdef BCM_TRIUMPH3_SUPPORT
        case ING_SER_FIFOm:
        case ING_SER_FIFO_Xm:
        case ING_SER_FIFO_Ym:
        case EGR_SER_FIFOm:
        case EGR_SER_FIFO_Xm:
        case EGR_SER_FIFO_Ym:
        case ISM_SER_FIFOm:
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        case ING_SER_FIFO_PIPE0m:
        case ING_SER_FIFO_PIPE1m:
        case ING_SER_FIFO_PIPE2m:
        case ING_SER_FIFO_PIPE3m:
        case EGR_SER_FIFO_PIPE0m:
        case EGR_SER_FIFO_PIPE1m:
        case EGR_SER_FIFO_PIPE2m:
        case EGR_SER_FIFO_PIPE3m:
        case L2_MGMT_SER_FIFOm:
        case MMU_GCFG_MEM_FAIL_ADDR_64m:
        case MMU_XCFG_MEM_FAIL_ADDR_64m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE1m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE2m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE3m:
        case MMU_SCFG_MEM_FAIL_ADDR_64m:
        case MMU_SCFG_MEM_FAIL_ADDR_64_SC0m:
        case MMU_SCFG_MEM_FAIL_ADDR_64_SC1m:
        case CENTRAL_CTR_EVICTION_FIFOm:
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
        case MMU_SEDCFG_MEM_FAIL_ADDR_64m:
        case MMU_SEDCFG_MEM_FAIL_ADDR_64_SC0m:
        case MMU_SEDCFG_MEM_FAIL_ADDR_64_SC1m:
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_56980_A0
        case ING_SER_FIFO_PIPE4m:
        case ING_SER_FIFO_PIPE5m:
        case ING_SER_FIFO_PIPE6m:
        case ING_SER_FIFO_PIPE7m:
        case EGR_SER_FIFO_PIPE4m:
        case EGR_SER_FIFO_PIPE5m:
        case EGR_SER_FIFO_PIPE6m:
        case EGR_SER_FIFO_PIPE7m:
        case EDB_SER_FIFOm:
        case EDB_SER_FIFO_PIPE0m:
        case EDB_SER_FIFO_PIPE1m:
        case EDB_SER_FIFO_PIPE2m:
        case EDB_SER_FIFO_PIPE3m:
        case EDB_SER_FIFO_PIPE4m:
        case EDB_SER_FIFO_PIPE5m:
        case EDB_SER_FIFO_PIPE6m:
        case EDB_SER_FIFO_PIPE7m:
        case EGR_SER_FIFO_2m:
        case EGR_SER_FIFO_2_PIPE0m:
        case EGR_SER_FIFO_2_PIPE1m:
        case EGR_SER_FIFO_2_PIPE2m:
        case EGR_SER_FIFO_2_PIPE3m:
        case EGR_SER_FIFO_2_PIPE4m:
        case EGR_SER_FIFO_2_PIPE5m:
        case EGR_SER_FIFO_2_PIPE6m:
        case EGR_SER_FIFO_2_PIPE7m:
        case MMU_GLBCFG_MEM_FAIL_ADDR_64m:
        case MMU_ITMCFG_MEM_FAIL_ADDR_64_ITM0m:
        case MMU_ITMCFG_MEM_FAIL_ADDR_64_ITM1m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE0m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE1m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE2m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE3m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE4m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE5m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE6m:
        case MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE7m:
        case DLB_ECMP_SER_FIFOm:
#endif
            break;
        default:
            cli_out("ERROR: %s table does not support FIFO push/pop\n",
                    SOC_MEM_UFNAME(unit, mem));
            goto done;
    }

    ufname = SOC_MEM_UFNAME(unit, mem);

    if (copyno == COPYNO_ALL) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        cli_out("POP[%s]", ufname);
        cli_out("\n");
    }

    while (count--) {
        i = soc_mem_pop(unit, mem, copyno, result);

        if (i < 0) {            /* Error not fatal for lookup */
            if (i != SOC_E_NOT_FOUND) {
                cli_out("Pop ERROR: read error during pop: %s\n",
                        soc_errmsg(i));
            } else if (!quiet) {
                cli_out("Pop: Fifo empty\n");
            }
        } else { /* Entry popped */
            cli_out("Popped in %s.%s: ",
                    ufname, SOC_BLOCK_NAME(unit, copyno));
            soc_mem_entry_dump(unit, mem, result, BSL_LSS_CLI);
            cli_out("\n");
        }
    }

    rv = CMD_OK;

 done:
    return rv;
}

/*
 * Push an entry onto a FIFO
 */

char push_usage[] =
    "\nParameters: [quiet] [<COUNT>] <TABLE>\n"
    "The quiet option indicates not to complain on FIFO full.\n";

cmd_result_t
mem_push(int unit, args_t *a)
{
    uint32              entry[SOC_MAX_MEM_WORDS];
    int                 entry_dw;
    char                *ufname, *s;
    int                 rv = CMD_FAIL;
    char                valstr[1024];
    soc_mem_t           mem;
    int                 copyno;
    char                *tab;
    int                 count = 1;
    int                 quiet = 0;
    int                 i;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_mem_push_pop)) {
        /* feature not supported */
        return CMD_FAIL;
    }

    for (;;) {
        if ((tab = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        if (isint(tab)) {
            count = parse_integer(tab);
            continue;
        }

        if (sal_strcasecmp(tab, "quiet") == 0) {
            quiet = 1;
            continue;
        }
        break;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
        cli_out("ERROR: unknown table \"%s\"\n", tab);
        goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    switch (mem) { 
#if defined (BCM_TRIUMPH_SUPPORT)
        case ING_IPFIX_EXPORT_FIFOm:
        case EGR_IPFIX_EXPORT_FIFOm:
        case EXT_L2_MOD_FIFOm:
#endif /* BCM_TRIUMPH_SUPPORT */
        case L2_MOD_FIFOm:
            break;
        default:
            cli_out("ERROR: %s table does not support FIFO push/pop\n",
                    SOC_MEM_UFNAME(unit, mem));
            goto done;
    }
    
    entry_dw = soc_mem_entry_words(unit, mem);
    ufname = SOC_MEM_UFNAME(unit, mem);
    
    if (copyno == COPYNO_ALL) { 
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    /* 
     * If a list of fields were specified, generate the entry from them.
     * Otherwise, generate it by reading raw dwords from command line.
     */

    if ((s = ARG_GET(a)) == 0) { 
        cli_out("ERROR: missing data for entry to push\n");
        goto done;
    }

    if (!isint(s)) {
        /* List of fields */

        collect_comma_args(a, valstr, s);

        sal_memset(entry, 0, sizeof (entry));

        if (modify_mem_fields(unit, mem, entry, NULL, valstr, FALSE) < 0) {
            cli_out("Syntax error in field specification\n");
            goto done;
        }
    } else {
        /* List of numeric values */

        ARG_PREV(a);

        if (parse_dwords(entry_dw, entry, a) < 0) {
            goto done;
        }
    }

    if (bsl_check(bslLayerSoc, bslSourceSocmem, bslSeverityNormal, unit)) {
        cli_out("PUSH[%s], DATA:", ufname);
        for (i = 0; i < entry_dw; i++) {
            cli_out(" 0x%x", entry[i]);
        }
        cli_out("\n");
    }

    /*
     * Have entry data, push entry.
     */

    while (count--) {
        i = soc_mem_push(unit, mem, copyno, entry);
        if (quiet && i == SOC_E_FULL) {
            i = SOC_E_NONE;
        }

        if (i < 0) {
            cli_out("Push ERROR: %s table push failed: %s\n",
                    ufname, soc_errmsg(i));
            goto done;
        }
    }

    rv = CMD_OK;

 done:
    return rv;
}

/*
 * do_search_entry
 *
 *  Auxiliary routine for command_search()
 *  Returns byte position within entry where pattern is found.
 *  Returns -1 if not found.
 *  Finds pattern in either big- or little-endian order.
 */

static int
do_search_entry(uint8 *entry, int entry_len,
		uint8 *pat, int pat_len)
{
    int start, i;

    for (start = 0; start <= entry_len - pat_len; start++) {
	for (i = 0; i < pat_len; i++) {
	    if (entry[start + i] != pat[i]) {
		break;
	    }
	}

	if (i == pat_len) {
	    return start;
	}

	for (i = 0; i < pat_len; i++) {
	    if (entry[start + i] != pat[pat_len - 1 - i]) {
		break;
	    }
	}

	if (i == pat_len) {
	    return start;
	}
    }

    return -1;
}

/*
 * Search a table for a byte pattern
 */

#define SRCH_MAX_PAT		16

char search_usage[] =
    "1. Parameters: [ALL | BIN | FBIN] <TABLE>[.<COPY>] 0x<PATTERN>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tDumps table entries containing the specified byte pattern\n\t"
    "anywhere in the entry in big- or little-endian order.\n\t"
    "Example: search arl 0x112233445566\n"
#endif
    "2. Parameters: [ALL | BIN | FBIN] <TABLE>[.<COPY>]"
         "<FIELD>=<VALUE>[,...]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tDumps table entries where the specified fields are equal\n\t"
    "to the specified values.\n\t"
    "Example: search l3 ip_addr=0xf4000001,pnum=10\n\t"
    "The ALL flag indicates to search sorted tables in their\n\t"
    "entirety instead of only entries known to contain data.\n\t"
    "The BIN flag does a binary search of sorted tables in\n\t"
    "the portion known to contain data.\n\t"
    "The FBIN flag does a binary search, and guarantees if there\n\t"
    "are duplicates the lowest matching index will be returned.\n"
#endif
    ;

cmd_result_t
mem_search(int unit, args_t *a)
{
    int			t, rv = CMD_FAIL;
    char		*tab, *patstr;
    soc_mem_t		mem;
    uint32		pat_entry[SOC_MAX_MEM_WORDS];
    uint32		pat_mask[SOC_MAX_MEM_WORDS];
    uint32		entry[SOC_MAX_MEM_WORDS];
    char		valstr[1024];
    uint8		pat[SRCH_MAX_PAT];
    char		*ufname;
    int			patsize = 0, found_count = 0, entry_bytes, entry_dw;
    int			copyno, index, min, max;
    int			byte_search;
    int			all_flag = 0, bin_flag = 0, lowest_match = 0;
#ifdef BCM_TRIDENT2_SUPPORT
    int       check_view = FALSE;
    soc_mem_t view = INVALIDm;
#endif /* BCM_TRIDENT2_SUPPORT */

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    tab = ARG_GET(a);

    while (tab) {
	if (!sal_strcasecmp(tab, "all")) {
	    all_flag = 1;
	} else if (!sal_strcasecmp(tab, "bin")) {
	    bin_flag = 1;
	} else if (!sal_strcasecmp(tab, "fbin")) {
	    bin_flag = 1;
	    lowest_match = 1;
	} else {
	    break;
	}
	tab = ARG_GET(a);
    }

    patstr = ARG_GET(a);

    if (!tab || !patstr) {
	return CMD_USAGE;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
	cli_out("ERROR: unknown table \"%s\"\n", tab);
	goto done;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
        cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
        goto done;
    }

    if (copyno == COPYNO_ALL) {
	copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    }

    entry_bytes = soc_mem_entry_bytes(unit, mem);
    entry_dw = BYTES2WORDS(entry_bytes);
    min = soc_mem_index_min(unit, mem);
    max = soc_mem_index_max(unit, mem);
    ufname = SOC_MEM_UFNAME(unit, mem);

    byte_search = isint(patstr);

    if (byte_search) {
	char *s;

	/*
	 * Search by pattern string.
	 * Parse pattern string (long hex number) into array of bytes.
	 */

	if (patstr[0] != '0' ||
	    (patstr[1] != 'x' && patstr[1] != 'X') || patstr[2] == 0) {
	    cli_out("ERROR: illegal search pattern, need hex constant\n");
	    goto done;
	}

	patstr += 2;

	for (s = patstr; *s; s++) {
	    if (!isxdigit((unsigned) *s)) {
		cli_out("ERROR: invalid hex digit in search pattern\n");
		goto done;
	    }
	}

	while (s > patstr + 1) {
	    s -= 2;
	    pat[patsize++] = xdigit2i(s[0]) << 4 | xdigit2i(s[1]);
	}

	if (s > patstr) {
	    s--;
	    pat[patsize++] = xdigit2i(s[0]);
	}

#if 0
	cli_out("PATTERN: ");
	for (index = 0; index < patsize; index++) {
	    cli_out("%02x ", pat[index]);
	}
	cli_out("\n");
#endif
    } else {
	/*
	 * Search by pat_entry and pat_mask.
	 * Collect list of fields
	 */

	collect_comma_args(a, valstr, patstr);

	memset(pat_entry, 0, sizeof (pat_entry));

	if (modify_mem_fields(unit, mem,
			      pat_entry, pat_mask,
			      valstr, FALSE) < 0) {
	    cli_out("Syntax error: aborted\n");
	    goto done;
	}
    }

    if (bin_flag) {
	if (!soc_mem_is_sorted(unit, mem)) {
	    cli_out("ERROR: can only binary search sorted tables\n");
	    goto done;
	}

	if (byte_search) {
	    cli_out("ERROR: can only binary search for "
                    "<FIELD>=<VALUE>[,...]\n");
	    goto done;
	}

	cli_out("Searching %s...\n", ufname);

	t = soc_mem_search(unit, mem, copyno, &index,
				pat_entry, entry, lowest_match);
        if ((t < 0) && (t != SOC_E_NOT_FOUND)) {
	    cli_out("Read ERROR: table[%s]: %s\n", ufname, soc_errmsg(t));
	    goto done;
	}

	if (t == SOC_E_NONE) {
	    cli_out("%s[%d]: ", ufname, index);
	    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
	    cli_out("\n");
	} else {
	    cli_out("Nothing found\n");
        }

	rv = CMD_OK;
	goto done;
    }

    if (!all_flag && soc_mem_is_sorted(unit, mem)) {
	max = soc_mem_index_last(unit, mem, copyno);
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) &&
        (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
         mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
         mem == L3_DEFIP_ALPM_IPV6_128m)) {
        check_view = TRUE;
        view = mem;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    cli_out("Searching %s table indexes 0x%x through 0x%x...\n",
            ufname, min, max);

    for (index = min; index <= max; index++) {
	int match;

#ifdef BCM_TRIDENT2_SUPPORT
        if (check_view) {
            if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
                view = _soc_trident2_alpm_bkt_view_get(unit, index);
            }
#if defined(ALPM_ENABLE)
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            else {
                view = soc_alpm_cmn_bkt_view_get(unit, index);
            }
#endif
#endif
            if (!(view == INVALIDm || mem == view)) {
                continue;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

	if ((t = soc_mem_read(unit, mem, copyno, index, entry)) < 0) {
	    cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                    ufname, SOC_BLOCK_NAME(unit, copyno), index, soc_errmsg(t));
	    goto done;
	}

#ifdef BCM_XGS_SWITCH_SUPPORT
	if (!all_flag) {
#ifdef BCM_XGS12_SWITCH_SUPPORT
            if SOC_IS_XGS12_SWITCH(unit) {
	        if (mem == L2Xm &&
		    !soc_L2Xm_field32_get(unit, entry, VALID_BITf)) {
		    continue;
	        }

	        if (mem == L3Xm &&
		    !soc_L3Xm_field32_get(unit, entry, L3_VALIDf)) {
		    continue;
	        }
            }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_FIREBOLT_SUPPORT
            if (SOC_IS_FBX(unit) || SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) { 
                switch (mem) {
                    case L2Xm:
                        {
                            soc_field_t vld_fld;
                            vld_fld =
                                (SOC_IS_TOMAHAWK3(unit) || SOC_IS_TRIDENT3X(unit)) ? BASE_VALIDf : VALIDf;
                            if (!soc_L2Xm_field32_get(unit, entry, vld_fld)) {
                                continue;
                            }
                        }
                        break; 
                    case L2_USER_ENTRYm:
                        if (!soc_L2_USER_ENTRYm_field32_get(unit, 
                               entry, VALIDf)) {
                            continue;
                        }
                        break;
                    case L3_ENTRY_IPV4_UNICASTm:
                        if (!soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, 
                               entry, VALIDf)) {
                            continue;
                        }
                        break;
                    case L3_ENTRY_IPV4_MULTICASTm:
                        if (soc_mem_field_valid(unit, mem, VALIDf)) {
                            if (!soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                   entry, VALIDf)) {
                                continue;
                            }
                        }

                        if (soc_mem_field_valid(unit, mem, VALID_0f) &&
                            soc_mem_field_valid(unit, mem, VALID_1f)) {
                            if (!(soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                   entry, VALID_0f) &&
                                  soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                   entry, VALID_1f))) {
                                continue;
                            }
                        }
                        break;
                    case L3_ENTRY_IPV6_UNICASTm:
                        if (!(soc_L3_ENTRY_IPV6_UNICASTm_field32_get(unit,
                                entry, VALID_0f) && 
                              soc_L3_ENTRY_IPV6_UNICASTm_field32_get(unit,
                                entry, VALID_1f))) {
                            continue;
                        }
                        break;
                    case L3_ENTRY_IPV6_MULTICASTm:
                        if (!(soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                entry, VALID_0f) &&
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                entry, VALID_1f) &&
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                entry, VALID_2f) &&
                              soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, 
                                entry, VALID_3f))) {
                            continue;
                        }
                        break;
                    default:
                        break;
                }
            }
#endif /* BCM_FIREBOLT_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
	}
#endif /* BCM_XGS_SWITCH_SUPPORT */

	if (byte_search) {
	    match = (do_search_entry((uint8 *) entry, entry_bytes,
				     pat, patsize) >= 0);
	} else {
	    int i;

	    for (i = 0; i < entry_dw; i++)
		if ((entry[i] & pat_mask[i]) != pat_entry[i])
		    break;

	    match = (i == entry_dw);
	}

	if (match) {
	    cli_out("%s.%s[%d]: ",
                    ufname, SOC_BLOCK_NAME(unit, copyno), index);
	    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
	    cli_out("\n");
	    found_count++;
	}
    }

    if (found_count == 0) {
	cli_out("Nothing found\n");
    }

    rv = CMD_OK;

 done:
    return rv;
}

/*
 * Print a one line summary for matching memories
 * If substr_match is NULL, match all memories.
 * If substr_match is non-NULL, match any memories whose name
 * or user-friendly name contains that substring.
 */

static void
mem_list_summary(int unit, char *substr_match)
{
    soc_mem_t		mem;
    int			i, copies, dlen;
    int			found = 0;
    char		*dstr;
    int         print_tr = 0;
    const char  *soctr = "soc_tr";

    if(substr_match) {
        if(sal_strcmp(substr_match,soctr) == 0){
            print_tr = 1;
            substr_match = NULL;
        }
    }

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!soc_mem_is_valid(unit, mem)) {
            continue;
        }

#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit) && (soc_mem_index_max(unit, mem) <= 0)) {
            continue;
        }
#endif /* BCM_HAWKEYE_SUPPORT */

        if (substr_match != NULL &&
            strcaseindex(SOC_MEM_NAME(unit, mem), substr_match) == NULL &&
            strcaseindex(SOC_MEM_UFNAME(unit, mem), substr_match) == NULL) {
            continue;
        }

        copies = 0;
        SOC_MEM_BLOCK_ITER(unit, mem, i) {
            copies += 1;
        }

        dlen = sal_strlen(SOC_MEM_DESC(unit, mem));
        if (dlen > 30) {
            dlen = 26;
            dstr = "...";
        } else {
            dstr = "";
        }

        if(print_tr) {
            if(!soc_mem_is_readonly(unit, mem)) {
                if(soc_mem_index_count(unit,mem) > 3) {
                    cli_out("local mem '%s';\n", SOC_MEM_UFNAME(unit,mem));
                    cli_out("$name; $tr50; $tr51; $tr52;\n");
                } else if(soc_mem_index_count(unit,mem) > 0) {
                    cli_out("local mem '%s';\n", SOC_MEM_UFNAME(unit,mem));
                    cli_out("$name; $tr50;\n");
                }
            } else {
                cli_out("#local mem '%s' Read Only Memory - Not tested\n",
                         SOC_MEM_UFNAME(unit,mem));
            }
        } else {
            if (!found) {
                cli_out(" %-6s %1s %-28s%6s/%-4s %s\n",
                        "Flags", "C", "Name", "Entry",
                        "Copy", "Description");
                found = 1;
            }

            cli_out(" %c%c%c%c%c%c %1s %-28s%6d",
                    soc_mem_is_readonly(unit, mem) ? 'r' : '-',
                    soc_mem_is_debug(unit, mem) ? 'd' : '-',
                    soc_mem_is_sorted(unit, mem) ? 's' :
                    soc_mem_is_hashed(unit, mem) ? 'h' :
                    soc_mem_is_cam(unit, mem) ? 'A' : '-',
                    soc_mem_is_cbp(unit, mem) ? 'c' : '-',
                    (soc_mem_is_bistepic(unit, mem) ||
                    soc_mem_is_bistffp(unit, mem) ||
                    soc_mem_is_bistcbp(unit, mem)) ? 'b' : '-',
                    soc_mem_is_cachable(unit, mem) ? 'C' : '-',
                    soc_mem_cache_get(unit, mem, SOC_BLOCK_ANY) ? "*" : "",
                    SOC_MEM_UFNAME(unit, mem),
                    soc_mem_index_count(unit, mem));
            if (copies == 1) {
                cli_out("%5s %*.*s%s\n",
                        "",
                        dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
            } else {
                cli_out("/%-4d %*.*s%s\n",
                        copies,
                        dlen, dlen, SOC_MEM_DESC(unit, mem), dstr);
            }
        }
    }

    if (found) {
	cli_out("Flags: (r)eadonly, (d)ebug, (s)orted, (h)ashed\n"
                "       C(A)M, (c)bp, (b)ist-able, (C)achable\n");
    cli_out("    C: Cache state, * means cache is on\n");
    } else if (substr_match != NULL) {
	cli_out("No memory found with the substring '%s' in its name.\n",
                substr_match);
    }
}

/*
 * List the tables, or fields of a table entry
 */

cmd_result_t
cmd_esw_mem_list(int unit, args_t *a)
{
    soc_mem_info_t	*m;
    soc_field_info_t	*fld;
    char		*tab, *s;
    soc_mem_t		mem;
    uint32		entry[SOC_MAX_MEM_WORDS];
    uint32		mask[SOC_MAX_MEM_WORDS];
    int			have_entry, i, dw, copyno;
    int			copies, disabled, dmaable, slamable;
    char		*dmastr, *slamstr;
    uint32		flags;
    int			minidx, maxidx;
    uint8               at;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!soc_property_get(unit, spn_MEMLIST_ENABLE, 1)) {
        return CMD_OK;
    }
    tab = ARG_GET(a);

    if (!tab) {
	mem_list_summary(unit, NULL);
	return CMD_OK;
    }

    if (parse_memory_name(unit, &mem, tab, &copyno, 0) < 0) {
	if ((s = strchr(tab, '.')) != NULL) {
	    *s = 0;
	}
	mem_list_summary(unit, tab);
	return CMD_OK;
    }

    if (!SOC_MEM_IS_VALID(unit, mem)) {
	cli_out("ERROR: Memory \"%s\" not valid for this unit\n", tab);
	return CMD_FAIL;
    }

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && (soc_mem_index_max(unit, mem) <= 0)) {
        cli_out("ERROR: Memory \"%s\" not valid for this unit\n", tab);
        return CMD_FAIL;
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    if (copyno < 0) {
	copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        if (copyno < 0) {
            cli_out("ERROR: Memory \"%s\" has no instance on this unit\n", tab);
            return CMD_FAIL;
        }
    } else if (!SOC_MEM_BLOCK_VALID(unit, mem, copyno)) {
	cli_out("ERROR: Invalid copy number %d for memory %s\n", copyno, tab);
	return CMD_FAIL;
    }

    m = &SOC_MEM_INFO(unit, mem);
    flags = m->flags;

    dw = BYTES2WORDS(m->bytes);

    if ((s = ARG_GET(a)) == 0) {
	have_entry = 0;
    } else {
	for (i = 0; i < dw; i++) {
	    if (s == 0) {
		cli_out("Not enough data specified (%d words needed)\n", dw);
		return CMD_FAIL;
	    }
	    entry[i] = parse_integer(s);
	    s = ARG_GET(a);
	}
	if (s) {
	    cli_out("Extra data specified (ignored)\n");
	}
	have_entry = 1;
    }

    cli_out("Memory: %s.%s",
            SOC_MEM_NAME(unit, mem),
            SOC_BLOCK_NAME(unit, copyno));
    s = SOC_MEM_UFNAME(unit, mem);
    if (s && *s && sal_strcmp(SOC_MEM_NAME(unit, mem), s) != 0) {
	    cli_out(" aka %s", s);
    }
    s = SOC_MEM_UFALIAS(unit, mem);
    if (s && *s && sal_strcmp(SOC_MEM_UFNAME(unit, mem), s) != 0) {
	    cli_out(" alias %s", s);
    }
    cli_out(" address 0x%08x\n", soc_mem_addr_get(unit, mem, 0, copyno, 0, &at));

    cli_out("Flags:");
    if (flags & SOC_MEM_FLAG_READONLY) {
	cli_out(" read-only");
    }
    if (flags & SOC_MEM_FLAG_VALID) {
	cli_out(" valid");
    }
    if (flags & SOC_MEM_FLAG_DEBUG) {
	cli_out(" debug");
    }
    if (flags & SOC_MEM_FLAG_SORTED) {
	cli_out(" sorted");
    }
    if (flags & SOC_MEM_FLAG_CBP) {
	cli_out(" cbp");
    }
    if (flags & SOC_MEM_FLAG_CACHABLE) {
    	cli_out(" cachable");
        if (soc_mem_cache_get(unit, mem, SOC_BLOCK_ANY)) {
            cli_out("(on)");
        } else {
            cli_out("(off)");
        }
    }
    if (flags & SOC_MEM_FLAG_BISTCBP) {
	cli_out(" bist-cbp");
    }
    if (flags & SOC_MEM_FLAG_BISTEPIC) {
	cli_out(" bist-epic");
    }
    if (flags & SOC_MEM_FLAG_BISTFFP) {
	cli_out(" bist-ffp");
    }
    if (flags & SOC_MEM_FLAG_UNIFIED) {
	cli_out(" unified");
    }
    if (flags & SOC_MEM_FLAG_HASHED) {
	cli_out(" hashed");
    }
    if (flags & SOC_MEM_FLAG_WORDADR) {
	cli_out(" word-addressed");
    }
    if (flags & SOC_MEM_FLAG_BE) {
	cli_out(" big-endian");
    }
    if (flags & SOC_MEM_FLAG_MULTIVIEW) {
        cli_out(" multiview");
    }
    cli_out("\n");

    cli_out("Blocks: ");
    copies = disabled = dmaable = slamable = 0;
    SOC_MEM_BLOCK_ITER(unit, mem, i) {
	if (SOC_INFO(unit).block_valid[i]) {
	    dmastr = "";
	    slamstr = "";
#ifdef BCM_XGS_SWITCH_SUPPORT
	    if (soc_mem_dmaable(unit, mem, i)) {
		dmastr = "/dma";
		dmaable += 1;
	    }
	    if (soc_mem_slamable(unit, mem, i)) {
		slamstr = "/slam";
		slamable += 1;
	    }
#endif
	    cli_out(" %s%s%s", SOC_BLOCK_NAME(unit, i), dmastr, slamstr);
	} else {
	    cli_out(" [%s]", SOC_BLOCK_NAME(unit, i));
	    disabled += 1;
	}
	copies += 1;
    }
    cli_out(" (%d cop%s", copies, copies == 1 ? "y" : "ies");
    if (disabled) {
	cli_out(", %d disabled", disabled);
    }
#ifdef BCM_XGS_SWITCH_SUPPORT
    if (dmaable) {
	cli_out(", %d dmaable", dmaable);
    }
    if (slamable) {
	cli_out(", %d slamable", slamable);
    }
#endif
    cli_out(")\n");

    minidx = soc_mem_index_min(unit, mem);
    maxidx = soc_mem_index_max(unit, mem);
    cli_out("Entries: %d with indices %d-%d (0x%x-0x%x)",
            maxidx - minidx + 1,
            minidx,
            maxidx,
            minidx,
            maxidx);
    cli_out(", each %d bytes %d words\n", m->bytes, dw);

    cli_out("Entry mask:");
    soc_mem_datamask_get(unit, mem, mask);
    for (i = 0; i < dw; i++) {
	if (mask[i] == 0xffffffff) {
	    cli_out(" -1");
	} else if (mask[i] == 0) {
	    cli_out(" 0");
	} else {
	    cli_out(" 0x%08x", mask[i]);
	}
    }
    cli_out("\n");

#ifdef BCM_ISM_SUPPORT
    if (soc_feature(unit, soc_feature_ism_memory)) {
        uint8 banks[_SOC_ISM_MAX_BANKS] = {0};
        uint32 bank_size[_SOC_ISM_MAX_BANKS];
        uint8 count;
        if (soc_ism_get_banks_for_mem(unit, mem, banks, bank_size, &count) 
            == SOC_E_NONE) {
            if (count) {
                uint8 bc;
                uint32 bmask = 0;
                for (bc = 0; bc < count; bc++) {
                    bmask |= 1 << banks[bc];
                }
                cli_out("ISM bank mask: 0x%x\n", bmask);
            }
        }
    }
#endif

    s = SOC_MEM_DESC(unit, mem);
    if (s && *s) {
	cli_out("Description: %s\n", s);
    }

    for (fld = &m->fields[m->nFields - 1]; fld >= &m->fields[0]; fld--) {
	cli_out("  %s<%d", SOC_FIELD_NAME(unit, fld->field), fld->bp + fld->len - 1);
	if (fld->len > 1) {
	    cli_out(":%d", fld->bp);
	}
	if (have_entry) {
	    uint32 fval[SOC_MAX_MEM_FIELD_WORDS];
	    char tmp[132];

	    sal_memset(fval, 0, sizeof (fval));
	    soc_mem_field_get(unit, mem, entry, fld->field, fval);
	    format_long_integer(tmp, fval, SOC_MAX_MEM_FIELD_WORDS);
	    cli_out("> = %s\n", tmp);
	} else {
	    cli_out(">\n");
	}
    }

    return CMD_OK;
}

/*
 * Turn on/off software caching of hardware tables
 */

cmd_result_t
mem_esw_cache(int unit, args_t *a)
{
    soc_mem_t		mem;
    int			copyno;
    char		*c;
    int			enable, r;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
	for (enable = 0; enable < 2; enable++) {
	    cli_out("Caching is %s for:\n", enable ? "on" : "off");

	    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
		int any_printed;

		if ((!SOC_MEM_IS_VALID(unit, mem)) ||
                    (!soc_mem_is_cachable(unit, mem))) {
		    continue;
                }

		any_printed = 0;

		SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
		    if ((!enable) ==
			(!soc_mem_cache_get(unit, mem, copyno))) {
			if (!any_printed) {
			    cli_out("    ");
			}
			cli_out(" %s.%s",
                                SOC_MEM_UFNAME(unit, mem),
                                SOC_BLOCK_NAME(unit, copyno));
			any_printed = 1;
		    }
		}

		if (any_printed) {
		    cli_out("\n");
		}
	    }
	}

	return CMD_OK;
    }

    while ((c = ARG_GET(a)) != 0) {
	switch (*c) {
	case '+':
	    enable = 1;
	    c++;
	    break;
	case '-':
	    enable = 0;
	    c++;
	    break;
	default:
	    enable = 1;
	    break;
	}

	if (parse_memory_name(unit, &mem, c, &copyno, 0) < 0) {
	    cli_out("%s: Unknown table \"%s\"\n", ARG_CMD(a), c);
	    return CMD_FAIL;
	}

        if (!SOC_MEM_IS_VALID(unit, mem)) {
            cli_out("Error: Memory %s not valid for chip %s.\n",
                    SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
            continue;
        }

	if (!soc_mem_is_cachable(unit, mem)) {
	    cli_out("%s: Memory %s is not cachable\n",
                    ARG_CMD(a), SOC_MEM_UFNAME(unit, mem));
	    return CMD_FAIL;
	}

	if ((r = soc_mem_cache_set(unit, mem, copyno, enable)) < 0) {
	    cli_out("%s: Error setting cachability for %s: %s\n",
                    ARG_CMD(a), SOC_MEM_UFNAME(unit, mem), soc_errmsg(r));
	    return CMD_FAIL;
	}
    }

    return CMD_OK;
}

#ifdef INCLUDE_MEM_SCAN

/*
 * Turn on/off memory scan thread
 */

char memscan_usage[] =
    "Parameters: [Interval=<USEC>] [Rate=<ENTRIES>] [on] [off]\n\t"
    "Interval specifies how often to run (0 to stop)\n\t"
    "Rate specifies the number of entries to process per interval\n\t"
    "Any memories that are cached will be scanned (see 'cache' command)\n";

cmd_result_t
mem_scan(int unit, args_t *a)
{
    parse_table_t pt;
    int running, rv;
    sal_usecs_t interval = 0;
    int rate = SOC_MEMSCAN_RATE_DEFAULT;
    char *c;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((running = soc_mem_scan_running(unit, &rate, &interval)) < 0) {
        cli_out("soc_mem_scan_running %d: ERROR: %s\n",
                unit, soc_errmsg(running));
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        cli_out("%s: %s on unit %d\n",
                ARG_CMD(a), running ? "Running" : "Not running", unit);
        cli_out("%s:   Interval: %d usec\n",
                ARG_CMD(a), interval);
        cli_out("%s:   Rate: %d\n",
                ARG_CMD(a), rate);

        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_INT | PQ_DFL, 0, &interval, 0);
    parse_table_add(&pt, "Rate", PQ_INT | PQ_DFL, 0, &rate, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);

    if ((c = ARG_GET(a)) != NULL) {
        if (sal_strcasecmp(c, "off") == 0) {
            interval = 0;
            rate = 0;
        } else if (sal_strcasecmp(c, "on") == 0) {
            interval = prev_memscan_interval[unit];
            rate = prev_memscan_rate[unit];
        } else {
            return CMD_USAGE;
        }
    }

    if (interval == 0) {
        sal_usecs_t prev_interval;
        int prev_rate;

        rv = soc_mem_scan_running(unit, &prev_rate, &prev_interval);
        if (rv < 0 || prev_rate <= 0) {
            prev_rate = SOC_MEMSCAN_RATE_DEFAULT;
        }
        if (rv < 0 || prev_interval <= 0) {
            prev_interval = SOC_MEMSCAN_INTERVAL_DEFAULT;
        }
        prev_memscan_rate[unit] = prev_rate;
        prev_memscan_interval[unit] = prev_interval;

        if ((rv = soc_mem_scan_stop(unit)) < 0) {
            cli_out("soc_mem_scan_stop %d: ERROR: %s\n",
                    unit, soc_errmsg(rv));
            return CMD_FAIL;
        }

        cli_out("%s: Stopped on unit %d\n", ARG_CMD(a), unit);
    } else {
        if (0 == rate) {
            cli_out("%s: Start fails since rate 0 is not supported!\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        if ((rv = soc_mem_scan_start(unit, rate, interval)) < 0) {
            cli_out("soc_mem_scan_start %d: ERROR: %s\n",
                    unit, soc_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out("%s: Started on unit %d\n", ARG_CMD(a), unit);
    }

    return CMD_OK;
}

#endif /* INCLUDE_MEM_SCAN */

#ifdef BCM_SRAM_SCAN_SUPPORT

/*
 * Turn on/off sram scan thread
 */

char sramscan_usage[] =
    "Parameters: [Interval=<USEC>] [Rate=<ENTRIES>] [on] [off]\n\t"
    "Interval specifies how often to run (0 to stop)\n\t"
    "Rate specifies the number of entries to process per interval\n\t"
    "Any srams that are cached will be scanned (see 'cache' command)\n";

cmd_result_t
sram_scan(int unit, args_t *a)
{
    parse_table_t pt;
    int    running, rv;
    sal_usecs_t interval = 0;
    int    rate = SOC_SRAMSCAN_RATE_DEFAULT;
    char   *c;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((running = soc_sram_scan_running(unit, &rate, &interval)) < 0) {
        cli_out("soc_sram_scan_running %d: ERROR: %s\n",
                unit, soc_errmsg(running));
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        cli_out("%s: %s on unit %d\n",
                ARG_CMD(a), running ? "Running" : "Not running", unit);
        cli_out("%s:   Interval: %d usec\n",
                ARG_CMD(a), interval);
        cli_out("%s:   Rate: %d\n",
                ARG_CMD(a), rate);

        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_INT | PQ_DFL, 0, &interval, 0);
    parse_table_add(&pt, "Rate", PQ_INT | PQ_DFL, 0, &rate, 0);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    parse_arg_eq_done(&pt);

    if ((c = ARG_GET(a)) != NULL) {
        if (sal_strcasecmp(c, "off") == 0) {
            interval = 0;
            rate = 0;
        } else if (sal_strcasecmp(c, "on") == 0) {
            interval = prev_sramscan_interval[unit];
            rate = prev_sramscan_rate[unit];
        } else {
            return CMD_USAGE;
        }
    }

    if (interval == 0) {
        sal_usecs_t prev_interval;
        int prev_rate;

        rv = soc_sram_scan_running(unit, &prev_rate, &prev_interval);
        if (rv < 0 || prev_rate <= 0) {
            prev_rate = SOC_SRAMSCAN_RATE_DEFAULT;
        }
        if (rv < 0 || prev_interval <= 0) {
            prev_interval = SOC_SRAMSCAN_INTERVAL_DEFAULT;
        }
        prev_sramscan_rate[unit] = prev_rate;
        prev_sramscan_interval[unit] = prev_interval;

        if ((rv = soc_sram_scan_stop(unit)) < 0) {
            cli_out("soc_sram_scan_stop %d: ERROR: %s\n",
                    unit, soc_errmsg(rv));
            return CMD_FAIL;
        }

        cli_out("%s: Stopped on unit %d\n", ARG_CMD(a), unit);
    } else {
        if (0 == rate) {
            cli_out("%s: Start fails since rate 0 is not supported!\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        if ((rv = soc_sram_scan_start(unit, rate, interval)) < 0) {
            cli_out("soc_sram_scan_start %d: ERROR: %s\n",
                    unit, soc_errmsg(rv));
            return CMD_FAIL;
        }
        cli_out("%s: Started on unit %d\n", ARG_CMD(a), unit);
    }
    return CMD_OK;
}

#endif /* BCM_SRAM_SCAN_SUPPORT */


#if defined(SER_TR_TEST_SUPPORT)
char cmd_esw_ser_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "SER [options]\t"
#else
    " SER INFO   (NOT YET IMPLEMENTED)\n"
    " SER SHOW   (NOT YET IMPLEMENTED)\n"
    " SER LOG [ID=<id>]\n"
    "\tid argument is used to show specified SER log\n"
    " SER INJECT memory=<memID> [Index=<Index>] [Pipe=pipe_x|pipe_y|"
    "pipe0|pipe1|pipe2|pipe3|pipe4|pipe5|pipe6|pipe7] [Xor_bank=0|1]\n"
    "\tpipe argument is allowed only in multi-pipe devices\n"
    "\tXor_bank argument is used to insert error into XOR ram of XOR hash tables.\n"
    " SER INJECT register=<regID> port=<portNum> [Index=<Index>]"
    " [Pipe=pipe_x|pipe_y|pipe0|pipe1|pipe2|pipe3](NOT YET IMPLEMENTED\n"
    "\tWith no optional parameters, Index will default to 0, and Pipe to"
    " pipe_x(pipe0)\n"
#endif
    ;

/*
 * Function:
 *      cmd_esw_ser
 * Purpose:
 *      Entry point and argument parser for SER diag shell utilities
 * Parameters:
 *      unit    - (IN) Device Number
 *      a       - (IN) A string of user input.  Only the first parameter will
 *                     be used by this function.
 * Returns:
 *      CMD_OK if injection succeeds, CMD_USAGE if there is a problem with user
 *      input, and CMD_FAIL if preconditions are not met or injection fails for
 *      a reason not related to incorrect input.
 */
cmd_result_t
esw_ser_inject(int unit, args_t *a) {
    cmd_result_t rv = CMD_USAGE;
    char *mem_name = "", *pipe_name = "";
    int index, tcam_mask, i, idx_phy, xor_bank = 0;
    uint32  flags = 0;
    soc_block_t block = MEM_BLOCK_ANY;
    soc_mem_t mem;
    soc_pipe_select_t pipe = 0;
    parse_table_t pt;
    const char *allowed_pipe_name[] = {
        "pipe_x",
        "pipe_y",
        "pipe0",
        "pipe1",
        "pipe2",
        "pipe3",
        "pipe4",
        "pipe5",
        "pipe6",
        "pipe7",
    };
    const int pipe_num[] = {
        SOC_PIPE_SELECT_X_COMMON,
        SOC_PIPE_SELECT_Y_COMMON,
        SOC_PIPE_SELECT_0,
        SOC_PIPE_SELECT_1,
        SOC_PIPE_SELECT_2,
        SOC_PIPE_SELECT_3,
        SOC_PIPE_SELECT_4,
        SOC_PIPE_SELECT_5,
        SOC_PIPE_SELECT_6,
        SOC_PIPE_SELECT_7,
    };
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Memory", PQ_STRING,
                    "INVALIDm", &(mem_name), NULL);
    parse_table_add(&pt, "Index", PQ_INT,
                    (void *)0, &index, NULL);
    parse_table_add(&pt, "Pipe", PQ_STRING,
                    "pipe_x", &(pipe_name), NULL);
    parse_table_add(&pt, "TCAM", PQ_BOOL,
                    FALSE, &(tcam_mask), NULL);
    parse_table_add(&pt, "idx_phy", PQ_BOOL,
                    FALSE, &(idx_phy), NULL);
    parse_table_add(&pt, "Xor_bank", PQ_BOOL,
                    FALSE, &(xor_bank), NULL);

    if(parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return rv;
    }

    if (!(parse_memory_name(unit,&(mem),mem_name,&block,0) >=0)) {
        cli_out("Invalid memory selected.\n");
        parse_arg_eq_done(&pt);
        return rv;
    }

    for (i = 0; i < COUNTOF(pipe_num); i++) {
        if (!sal_strcasecmp(pipe_name, allowed_pipe_name[i])) {
            pipe = pipe_num[i];
            break;
        }
    }

    if (i == COUNTOF(pipe_num)) {
        cli_out("Invalid pipe selected. Valid entries are:"
                " pipe_x, pipe_y, pipe0, pipe1, pipe2, pipe3"
                " pipe4, pipe5, pipe6, pipe7\n");
        parse_arg_eq_done(&pt);
        return rv;
    }
    if (!SOC_SUCCESS(soc_ser_inject_support(unit, mem, pipe))){
        if (soc_ser_inject_support(unit,mem,pipe) == SOC_E_FAIL) {
            cli_out("Injection is not supported because miscellaneous"
                    " initializations have not yet been performed.\n"
                    "Type 'init misc' to do so and try again.\n");
        } else {
            cli_out("The selected memory: %s is valid, but SER correction "
                    "for it is not currently supported.\n", mem_name);
        }
        rv = CMD_FAIL;
        parse_arg_eq_done(&pt);
        return rv;
    }
    if (index < 0) {
        cli_out("Invalid index selected");
        parse_arg_eq_done(&pt);
        return rv;
    }
    if(tcam_mask) {
        flags |= SOC_INJECT_ERROR_TCAM_FLAG;
    }
    if (idx_phy) {
        flags |= SOC_INJECT_ERROR_DONT_MAP_INDEX;
    }
    if (xor_bank) {
        flags |= SOC_INJECT_ERROR_XOR_BANK;
    }
    if (SOC_FAILURE(soc_ser_inject_error(unit, flags, mem, pipe, block, index))) {
        cli_out("Injection failed on %s at index %d %s.\n", mem_name, index,
                pipe_name);
        rv = CMD_FAIL;
    } else {
        cli_out("Error injected on %s at index %d %s\n", mem_name, index,
                pipe_name);
        rv = CMD_OK;
    }

    parse_arg_eq_done(&pt);
    return rv;
}

/*
 * Function:
 *      cmd_esw_ser
 * Purpose:
 *      Entry point and argument parser for SER diag shell utilities
 * Parameters:
 *      unit    - (IN) Device Number
 *      a       - (IN) A string of user input.  Only the first parameter will
 *                     be used by this function.
 * Returns:
 *      CMD_OK if the command succeeds, CMD_USAGE if there is a problem with the
 *       input from the user.  Other returns  of type cmd_result_t are possible
 *       depending on the implementation of each utility.
 */
cmd_result_t
cmd_esw_ser(int unit, args_t *a){
    char* arg1;
    int i, log_id = 1;
    cmd_result_t rv = CMD_USAGE;
    parse_table_t pt;
    /* pipe argument is allowed only in multi-pipe devices */
    for (i = 0; i < a->a_argc; i++) {
         if (NUM_PIPE(unit) == 1) {
             if (sal_strstr(a->a_argv[i], "pipe") != NULL) {
                 return rv;
             }
         }
    }
    arg1 = ARG_GET(a);
    if (arg1 != NULL && !sal_strcasecmp(arg1, "inject")) {
        rv = esw_ser_inject(unit, a);
    } else if (arg1 != NULL && !sal_strcasecmp(arg1, "info")) {
        cli_out("Info option not yet implemented\n");
        rv = CMD_NOTIMPL;
    } else if (arg1 != NULL && !sal_strcasecmp(arg1, "show")) {
        cli_out("Show option not yet implemented\n");
        rv = CMD_NOTIMPL;
    } else if (arg1 != NULL && !sal_strcasecmp(arg1, "log")) {
        if (ARG_CNT(a) == 0) {
            rv = soc_ser_log_print_all(unit);
            if (rv != 0) {
                cli_out("Error: getting SER log failed %d!\n", rv);
                return CMD_USAGE;
            }
        } else {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "ID", PQ_INT, (void *)0, &log_id, NULL);

            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            if (log_id < 0) {
                cli_out("Invalid id selected!\n");
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            parse_arg_eq_done(&pt);

            rv = soc_ser_log_print_one(unit, log_id);
            if (rv != 0) {
                cli_out("Error: getting SER log failed %d!\n", rv);
                return CMD_USAGE;
            }
        }
    } else {
        cli_out("Invalid mode selected.\n");
    }
    return rv;
}

#endif /*defined(SER_TR_TEST_SUPPORT)*/

void mem_watch_counter_init(int unit);
/*
 * Function:
 *     mem_watch_cb
 * Purpose:
 *     Call back function for memory snooping
 * Parameters:
 *     unit         - (IN)  BCM device number
 *     mem          - (IN)  memory for snooping
 *     flags        - (IN)  SOC_MEM_SNOOP_XXX flags
 *     copyno       - (IN)  Memory block to read/write
 *     index_min    - (IN)  First memory index to read/write
 *     index_max    - (IN)  Last memory index to read/write
 *     data_buffer  - (IN)  buffer of the entry to read/write
 *     user_data    - (IN)  registered snooping user data
 * Returns:
 *     No return value
 */
static void
mem_watch_cb(int unit, soc_mem_t mem, uint32 flags, int copyno, int index_min,
             int index_max, void *data_buffer, void *user_data)
{
    int i;

    if (INVALIDm == mem) {
        cli_out("Invalid memory....\n");
        return;
    }

    if (index_max < index_min) {
        cli_out("Wrong indexes ....\n");
        return;
    }
    if (mem_count[unit] == NULL) {
        mem_watch_counter_init(unit);
    }

    if (SOC_MEM_SNOOP_WRITE_COUNT & flags) {
        mem_count[unit][mem].wr_count_cur += (index_max - index_min + 1);
    }

    if (SOC_MEM_SNOOP_READ_COUNT & flags) {
        mem_count[unit][mem].rd_count_cur += (index_max - index_min + 1);
    }

    if ((SOC_MEM_SNOOP_WRITE & flags)
        || (SOC_MEM_SNOOP_READ & flags)) {

        if (NULL == data_buffer) {
            cli_out("Buffer is NULL .... \n");
            return;
        }

        for (i = 0; i <= (index_max - index_min); i++) {
            cli_out("Unit = %d, mem = %s (%d), copyno = %d index = %d, Entry - ",
                    unit, SOC_MEM_NAME(unit, mem), mem, copyno, i + index_min);
            soc_mem_entry_dump(unit,mem, (char *)data_buffer + i, BSL_LSS_CLI);
        }
        cli_out("\n");
    }

    return;
}

/*
 * Function:
 *     mem_watch_counter_init
 * Purpose:
 *     Allocates space and initializes memory counters
 * Parameters:
 *     unit - (IN)  BCM device number
 * Returns:
 *     No return value
 */
void
mem_watch_counter_init(int unit)
{
    soc_mem_t mem;

    mem_count[unit] = (mem_count_t*) sal_alloc(sizeof(mem_count_t) * NUM_SOC_MEM, "mem_count");
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        mem_count[unit][mem].rd_count_old = 0;
        mem_count[unit][mem].rd_count_cur = 0;
        mem_count[unit][mem].wr_count_old = 0;
        mem_count[unit][mem].wr_count_cur = 0;
    }
    return;
}

/*
 * Function:
 *     mem_watch_snoop_all
 * Purpose:
 *     Registers or unregisters a snooping call back for all memories
 *     with the specific SOC_MEM_SNOOP_* flags
 * Parameters:
 *     unit   - (IN) BCM device number
 *     flags  - (IN) SOC_MEM_SNOOP_* flags
 *     regist - (IN) register or unregister
 * Returns:
 *     No return value
 */
void
mem_watch_snoop_all(int unit, uint32 flags, int regist)
{
    soc_mem_t mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        if (regist) {
            soc_mem_snoop_register(unit, mem, flags, mem_watch_cb, NULL);
        } else {
            soc_mem_snoop_unregister(unit, mem, flags);
        }
    }
    return;
}

/*
 * Function:
 *     mem_watch_start
 * Purpose:
 *     Registers a snooping call back for all memories with
 *     SOC_MEM_SNOOP_READ/WRITE_ALL flags
 * Parameters:
 *     unit - (IN)  BCM device number
 * Returns:
 *     No return value
 */
void
mem_watch_start(int unit)
{
    soc_mem_t mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        soc_mem_snoop_register(unit, mem, SOC_MEM_SNOOP_WRITE_COUNT
                               | SOC_MEM_SNOOP_READ_COUNT,
                               mem_watch_cb, NULL);
    }
    return;
}

/*
 * Function:
 *    mem_watch_stop
 * Purpose:
 *     Reset snooping flags SOC_MEM_SNOOP_READ/WRITE_ALL for all memories
 *
 * Parameters:
 *     unit  - (IN)  BCM device number
 *     clear - (IN)  if TRUE, clear counters
 * Returns:
 *     No return value
 */
static void
mem_watch_stop(int unit, uint8 clear) {
    soc_mem_t mem;

    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }
        soc_mem_snoop_unregister(unit, mem, SOC_MEM_SNOOP_WRITE_COUNT
                                 | SOC_MEM_SNOOP_READ_COUNT);
        if (clear == TRUE) {
            mem_count[unit][mem].rd_count_old = 0;
            mem_count[unit][mem].rd_count_cur = 0;
            mem_count[unit][mem].wr_count_old = 0;
            mem_count[unit][mem].wr_count_cur = 0;
        }
    }
    return;
}

/*
 * Function:
 *     mem_watch_show
 * Purpose:
 *     Show memories that have been read/written
 * Parameters:
 *     unit      - (IN)  BCM device number
 *     show_type - (IN)  config for which type should be showed:
 *                          SOC_MEM_WATCH_SHOW_READ
 *                          SOC_MEM_WATCH_SHOW_WRITE
 *                          SOC_MEM_WATCH_SHOW_READ_WRITE
 *     show_all  - (IN)  if TRUE: also show the memories where the delta of
 *                       read/writes is 0 between 2 invocations of the
 *                       command
 * Returns:
 *     No return value
 * Notes:
 *     mem_watch_start() should be called before calling this function
 */
static void
mem_watch_show(int unit, uint32 show_type, uint8 show_all)
{
    soc_mem_t mem;
    uint32 rd_delta, wr_delta;

    mem = -1;
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!SOC_MEM_IS_VALID(unit, mem)) {
            continue;
        }

        if (!(SOC_MEM_SNOOP_WRITE_COUNT
              & SOC_MEM_INFO(unit, mem).snoop_flags)
            || !(SOC_MEM_SNOOP_READ_COUNT
                 & SOC_MEM_INFO(unit, mem).snoop_flags)) {
            cli_out("WARNING: Memory watch not started on %s\n",
                    SOC_MEM_NAME(unit, mem));
            continue;
        }

        rd_delta = mem_count[unit][mem].rd_count_cur
                   - mem_count[unit][mem].rd_count_old;

        wr_delta = mem_count[unit][mem].wr_count_cur
                   - mem_count[unit][mem].wr_count_old;

        /* Only show momories that have counter update unless "show all" */
        if (show_all == TRUE
            || ((SOC_MEM_WATCH_SHOW_READ & show_type) && (rd_delta != 0))
            || ((SOC_MEM_WATCH_SHOW_WRITE & show_type) && (wr_delta != 0)))
        {
            cli_out("%s ", SOC_MEM_NAME(unit, mem));
        }

        if (SOC_MEM_WATCH_SHOW_READ & show_type) {
            if (rd_delta == 0) {
                if (show_all == TRUE) {
                    cli_out("Read count: %u 0 ",
                            mem_count[unit][mem].rd_count_cur);
                }
            } else {
                cli_out("Read count: %u +%u ",
                        mem_count[unit][mem].rd_count_cur, rd_delta);
                if (mem_count[unit][mem].rd_count_cur
                    < mem_count[unit][mem].rd_count_old) {
                    cli_out("(wrap) ");
                }
            }

            if (!(rd_delta == 0 && show_all == FALSE)) {
                mem_count[unit][mem].rd_count_old
                = mem_count[unit][mem].rd_count_cur;
            }
        }

        if (SOC_MEM_WATCH_SHOW_WRITE & show_type) {
            if (wr_delta == 0) {
                if (show_all == TRUE) {
                    cli_out("Write count: %u 0 ",
                            mem_count[unit][mem].wr_count_cur);
                }
            } else {
                cli_out("Write count: %u +%u ",
                        mem_count[unit][mem].wr_count_cur, wr_delta);
                if (mem_count[unit][mem].wr_count_cur
                    < mem_count[unit][mem].wr_count_old) {
                    cli_out("(wrap) ");
                }
            }

            if (!(wr_delta == 0 && show_all == FALSE)) {
                mem_count[unit][mem].wr_count_old
                = mem_count[unit][mem].wr_count_cur;
            }
        }

        if (show_all == TRUE
            || ((SOC_MEM_WATCH_SHOW_READ & show_type) && (rd_delta != 0))
            || ((SOC_MEM_WATCH_SHOW_WRITE & show_type) && (wr_delta != 0)))
        {
            cli_out("\n");
        }
    }
    return;
}

static soc_mem_t mem_last=0;
static 
int mem_skip(int unit, soc_mem_t mem)
{
    if (!soc_mem_is_valid(unit, mem)) {
        return 1;
    }
#ifdef SABER2_DEBUG
    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_READONLY) != 0) {
        /*cli_out("skipping read only memory %s\n", SOC_MEM_NAME(unit,mem));*/
         return 1; 
    }
    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_WRITEONLY) != 0) {
        /*cli_out("skipping write only memory %s\n", SOC_MEM_NAME(unit,mem));*/
         return 1; 
    }
    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG) != 0) {
        /*cli_out("skipping debug only memory %s\n", SOC_MEM_NAME(unit,mem));*/
         return 1; 
    }
    if ((soc_mem_flags(unit, mem) & SOC_MEM_FLAG_SIGNAL) != 0) {
        /*cli_out("skipping signal only memory %s\n", SOC_MEM_NAME(unit,mem));*/
         return 1; 
    }
#endif
    /* People  can add chip specifi memory here */
    if (SOC_IS_SABER2(unit)) {
#ifdef SABER2_DEBUG     
        switch(mem) {
        case IARB_ING_PHYSICAL_PORTm:
             return 1;
        default:
             break;
        }
#endif
    }
    return 0;
}
cmd_result_t
mem_first(int unit, args_t *a)
{
    int count=0;
    soc_mem_t mem;
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (mem_skip(unit, mem)) {
            continue;
        }
        /* cli_out("First Memory: %s \n", SOC_MEM_NAME(unit, mem)); */
        var_set("mem_name", SOC_MEM_NAME(unit,mem), FALSE, FALSE);
        var_set(SOC_MEM_NAME(unit,mem),"1", TRUE, FALSE);
        mem_last = mem;
        count++;
        break;
    }
    for (mem = mem + 1 ; mem < NUM_SOC_MEM; mem++) {
        if (mem_skip(unit, mem)) {
            continue;
        }
        /* if Any left !! */
        var_unset(SOC_MEM_NAME(unit,mem),TRUE, FALSE,FALSE);
        count++;
    }
    var_set_integer("mem_count", count, FALSE, FALSE);
    return CMD_OK;
}
cmd_result_t
mem_next(int unit, args_t *a)
{
    soc_mem_t mem;

    var_unset(SOC_MEM_NAME(unit,mem_last),TRUE, FALSE,FALSE);
    for (mem = mem_last + 1; mem < NUM_SOC_MEM; mem++) {
        if (mem_skip(unit, mem)) {
            continue;
        }
        /* cli_out("Next Memory: %s \n", SOC_MEM_NAME(unit, mem)); */
        var_set("mem_name", SOC_MEM_NAME(unit,mem), FALSE, FALSE);
        var_set(SOC_MEM_NAME(unit,mem),"1", TRUE, FALSE);
        mem_last = mem;
        return CMD_OK;
    }
    /* Actuall Not Required but better to have it for boundary check */
    var_set("mem_name", "LAST_INVALID_MEMORY", FALSE, FALSE);
    return CMD_OK;
}

cmd_result_t
mem_watch(int unit, args_t *a)
{
    soc_mem_t       mem;
    char            *c, *memname, *subcmd;
    int             copyno;
    int             allmems = 0;
    uint32          show_type;
    uint8           show_all;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        return CMD_USAGE;
    }

    if (NULL == (c = ARG_GET(a))) {
        return CMD_USAGE;
    }

    subcmd = c;

    if (sal_strcasecmp(subcmd, "start") == 0) {
        mem_watch_start(unit);
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "stop") == 0) {
        if (NULL == (subcmd = ARG_GET(a))) {
            mem_watch_stop(unit, FALSE);
        } else if (sal_strcasecmp(subcmd, "clear") == 0) {
            mem_watch_stop(unit, TRUE);
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        if (NULL == (subcmd = ARG_GET(a))) {
            /* memwatch show */
            show_type = SOC_MEM_WATCH_SHOW_READ_WRITE;
            show_all = FALSE;
        } else if (sal_strcasecmp(subcmd, "read") == 0) {
            if (NULL == (subcmd = ARG_GET(a))) {
                /* memwatch show read */
                show_type = SOC_MEM_WATCH_SHOW_READ;
                show_all = FALSE;
            } else if (sal_strcasecmp(subcmd, "all") == 0) {
                /* memwatch show read all */
                show_type = SOC_MEM_WATCH_SHOW_READ;
                show_all = TRUE;
            } else {
                return CMD_FAIL;
            }
        } else if (sal_strcasecmp(subcmd, "write") == 0) {
            if (NULL == (subcmd = ARG_GET(a))) {
                /* memwatch show write */
                show_type = SOC_MEM_WATCH_SHOW_WRITE;
                show_all = FALSE;
            } else if (sal_strcasecmp(subcmd, "all") == 0) {
                /* memwatch show write all */
                show_type = SOC_MEM_WATCH_SHOW_WRITE;
                show_all = TRUE;
            } else {
                return CMD_FAIL;
            }
        } else if (sal_strcasecmp(subcmd, "all") == 0) {
            /* memwatch show all */
            show_type = SOC_MEM_WATCH_SHOW_READ_WRITE;
            show_all = TRUE;
        } else {
            return CMD_FAIL;
        }

        mem_watch_show(unit, show_type, show_all);
        return CMD_OK;
    }

    if (sal_strcasecmp(c, "*") == 0) {
        allmems = 1;
    } else {
        /* Deal with VIEW:MEMORY if applicable */
        memname = sal_strstr(c, ":");
        if (memname != NULL) {
            memname++;
        } else {
            memname = c;
        }

        if (sal_strcasecmp(memname, "delta") == 0) {
            c = ARG_GET(a);
            soc_mem_watch_set(unit, (c && (sal_strcasecmp(c, "on") == 0)) ? 1 : 0);
            return CMD_OK;
        }

        if (parse_memory_name(unit, &mem, memname, &copyno, 0) < 0) {
            cli_out("ERROR: unknown table \"%s\"\n", c);
            return CMD_FAIL;
        }
    }

    if (NULL == (c = ARG_GET(a))) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(c, "off") == 0) {
        /* unregister single read/write call back */
        if (allmems) {
            mem_watch_snoop_all(unit, SOC_MEM_SNOOP_READ | SOC_MEM_SNOOP_WRITE,
                                FALSE);
        } else {
            soc_mem_snoop_unregister(unit, mem, SOC_MEM_SNOOP_READ
                                     | SOC_MEM_SNOOP_WRITE);
        }
    } else if (sal_strcasecmp(c, "read") == 0) {
        /* register callback with read flag */
        if (allmems) {
            mem_watch_snoop_all(unit, SOC_MEM_SNOOP_READ, TRUE);
        } else {
            soc_mem_snoop_register(unit, mem, SOC_MEM_SNOOP_READ,
                                   mem_watch_cb, NULL);
        }
    } else if (sal_strcasecmp(c, "write") == 0) {
        /* register callback with write flag */
        if (allmems) {
            mem_watch_snoop_all(unit, SOC_MEM_SNOOP_WRITE, TRUE);
        } else {
            soc_mem_snoop_register(unit, mem, SOC_MEM_SNOOP_WRITE,
                                   mem_watch_cb, NULL);
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

