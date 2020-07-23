/*! \file bcma_bcmltcmd_counter.c
 *
 * Command to show or clear the collected information from
 * counter logical tables in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/bcmlt/bcma_bcmltcmd_counter.h>

/* Counter types */
#define CTR_TYPE_ERRDISC   (1 << 0)
#define CTR_TYPE_TM        (1 << 1)

#define CTR_TYPES_NUM       2

/* CLI parse enum for counter types */
static bcma_cli_parse_enum_t ctr_types_enum[] = {
    { "ErrDisc", CTR_TYPE_ERRDISC },
    { "TM",      CTR_TYPE_TM      },
    { NULL,      0                }
};

/* Counter show flags */
#define CTR_SHOW_CHANGED   (1 << 0)
#define CTR_SHOW_SAME      (1 << 1)
#define CTR_SHOW_ZERO      (1 << 2)
#define CTR_SHOW_NONZERO   (1 << 3)
#define CTR_SHOW_ALL \
    (CTR_SHOW_CHANGED | CTR_SHOW_SAME | CTR_SHOW_ZERO | CTR_SHOW_NONZERO)

/* CLI parse enum for counter show flags */
static bcma_cli_parse_enum_t ctr_show_flags_enum[] = {
    { "Changed",    CTR_SHOW_CHANGED   },
    { "Same",       CTR_SHOW_SAME      },
    { "Zero"   ,    CTR_SHOW_ZERO      },
    { "NonZero",    CTR_SHOW_NONZERO   },
    { "All",        CTR_SHOW_ALL       },
    { NULL,         0                  }
};

/* List for default counters, used when no counters are specified */
static bcma_cli_parse_enum_t def_counters_enum[] = {
    { "CTR_MAC",                 0                              },
    { "CTR_MAC_ERR",             CTR_TYPE_ERRDISC               },
    { "CTR_EGR_MC_Q",            0                              },
    { "CTR_EGR_UC_Q",            0                              },
    { "CTR_EGR_TM_PORT_DROP",    CTR_TYPE_ERRDISC               },
    { "CTR_ING_TM_PORT_UC_DROP", CTR_TYPE_ERRDISC               },
    { "CTR_ING_TM_THD_HEADROOM_POOL_LOSSLESS_UC_DROP",
                                 CTR_TYPE_ERRDISC | CTR_TYPE_TM },
    { "CTR_TM_UC_Q_DROP",        CTR_TYPE_ERRDISC | CTR_TYPE_TM },
    { "CTR_TM_MC_Q_DROP",        CTR_TYPE_ERRDISC | CTR_TYPE_TM },
    { "CTR_TM_REPL_Q_DROP",      CTR_TYPE_ERRDISC | CTR_TYPE_TM },
    { "CTR_TM_OBM_PORT_DROP",    CTR_TYPE_ERRDISC | CTR_TYPE_TM },
    { "CTR_TM_OBM_PORT_USAGE",   CTR_TYPE_TM                    },
    { NULL,                      0                              }
};

/* User data for the callback function for counter show command */
typedef struct ctr_show_data_s {
    /* SHR print buffer */
    shr_pb_t *pb;
    /* Counter show flags (CTR_SHOW_xxx) */
    uint32_t flags;
} ctr_show_data_t;

/*
 * The key field information in ctr_info, used for the key fields other than
 * the PORT_ID key
 */
typedef struct ctr_info_key_field_s {
    /* Name */
    const char *name;
    /* Maximum number of key values */
    int num;
    /*
     * Entry offset for each adjacent key value in cache, might be greater
     * than 1 for multiple keys LT
     */
    int offset;
} ctr_info_key_field_t;

/* Cache field attribute in ctr_info */
#define CTR_INFO_CFA_VALID  (1 << 0)

/* Maximum number of miscellaneous key fields (other than the PORT_ID key) */
#define MAX_NUM_MISC_KEYS 2

/* Counter information for each cached counter LT */
typedef struct ctr_info_s {
    /* Counter LT entry information */
    bcma_bcmlt_entry_info_t *ei;
    /* Default field for PORT_ID key field (if any) */
    bcmlt_field_def_t *port_id_key;
    /* Miscellaneous key fields information (if any) */
    ctr_info_key_field_t misc_keys[MAX_NUM_MISC_KEYS];
    /* Maximum number of entries indexed by the miscellaneous keys fields */
    int num_misc_key_entries;
    /* Fields attributes for the entry in cache */
    uint32_t *cache_fields_attr;
    /* Cache to store the counter value */
    uint64_t *cache;
    /* Type of the counter LT (CTR_TYPE_xxx) */
    uint32_t types;
    /* Set to true if this counter is interesting for the counter command */
    bool marked;
    /* Pointer to the next counter information */
    struct ctr_info_s *next;
} ctr_info_t;

/* List head for the counter LTs which are used to be referenced */
static ctr_info_t *ctr_info_head[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*
 * Recursive function to calculate the maximum number of entries
 * indexed by the specified key level
 */
static int
entries_count(ctr_info_key_field_t *misc_keys, int idx)
{
    if (idx >= (MAX_NUM_MISC_KEYS) || misc_keys[idx].name == NULL) {
        return 1;
    } else {
        return (misc_keys[idx].num) * entries_count(misc_keys, idx + 1);
    }
}

/* Find the default field information from a specified field name */
static bcmlt_field_def_t *
entry_def_field_find(bcma_bcmlt_entry_info_t *ei, const char *name)
{
    int idx;
    bcmlt_field_def_t *df;

    for (idx = 0; idx < ei->num_def_fields; idx++) {
        df = &ei->def_fields[idx];
        if (sal_strcmp(df->name, name) == 0) {
            return df;
        }
    }
    return NULL;
}

/* Cleanup the resource allocated by struct ctr_info_t */
static void
ctr_info_destroy(ctr_info_t *ci)
{
    if (ci) {
        if (ci->ei) {
            bcma_bcmlt_entry_info_destroy(ci->ei);
        }
        if (ci->cache_fields_attr) {
            sal_free(ci->cache_fields_attr);
        }
        if (ci->cache) {
            sal_free(ci->cache);
        }
        sal_free(ci);
    }
}

/* Create counter information ctr_info_t for a specified counter LT name */
static ctr_info_t *
ctr_info_create(int unit, const char *ctr_lt)
{
    ctr_info_t *ci;
    bcma_bcmlt_entry_info_t *ei;
    bcmlt_field_def_t *df;
    ctr_info_key_field_t *key;
    int cache_size;
    int idx, num_entries;
    int kidx = 0;
    bcma_cli_parse_enum_t *e;
    int match;

    /* First check if the specified LT name is valid */
    match = bcma_bcmlt_tables_search(unit, ctr_lt, BCMA_BCMLT_MATCH_EXACT,
                                     BCMA_BCMLT_SCOPE_F_LTBL, NULL, NULL);
    if (!match) {
        return NULL;
    }

    ci = sal_alloc(sizeof(ctr_info_t), "bcmaCtrInfo");
    if (ci == NULL) {
        return NULL;
    }
    sal_memset(ci, 0, sizeof(ctr_info_t));

    /* Create entry information for this LT */
    ei = bcma_bcmlt_entry_info_create(unit, ctr_lt, 0, 0, NULL, NULL, 0);
    if (ei == NULL) {
        ctr_info_destroy(ci);
        return NULL;
    }
    ci->ei = ei;

    /* At least one key field should be associated with the LT */
    if (ei->num_key_fields == 0) {
        ctr_info_destroy(ci);
        return NULL;
    }

    /*
     * Only support the port-based counter LTs associate with the
     * OPERATIONAL_STATE symbol field
     */
    df = entry_def_field_find(ei, "PORT_ID");
    if (df && bcma_bcmlt_field_flags(df) == BCMA_BCMLT_FIELD_F_KEY) {
        ci->port_id_key = df;
        df = entry_def_field_find(ei, "OPERATIONAL_STATE");
        if (!df || bcma_bcmlt_field_flags(df) != BCMA_BCMLT_FIELD_F_SYMBOL) {
            ctr_info_destroy(ci);
            return NULL;
        }
    }

    /* Find if there is any other key fields */
    for (idx = ei->num_def_fields -1; idx >= 0; idx--) {
        df = &ei->def_fields[idx];
        if (!df->key || df == ci->port_id_key) {
            continue;
        }
        if (kidx >= MAX_NUM_MISC_KEYS) {
            ctr_info_destroy(ci);
            return NULL;
        }
        key = &ci->misc_keys[kidx++];
        key->name = df->name;
        key->num = df->max + 1;
    }
    /* Calculate the offset of the key entry */
    for (kidx = 0; kidx < MAX_NUM_MISC_KEYS; kidx++) {
        key = &ci->misc_keys[kidx];
        if (key->name == NULL) {
            break;
        }
        key->offset = entries_count(ci->misc_keys, kidx + 1);
    }

    /* Allocate and set the fields attribute per entry */
    ci->cache_fields_attr = sal_alloc(ei->num_def_fields * sizeof(uint32_t),
                                      "bcmaCtrInfoCFAttr");
    if (ci->cache_fields_attr == NULL) {
        ctr_info_destroy(ci);
        return NULL;
    }
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        df = &ei->def_fields[idx];
        ci->cache_fields_attr[idx] = 0;
        /* Only the 64-bit scalar non-key R/W fields will be cached */
        if (bcma_bcmlt_field_flags(df) == 0 &&
            df->access != BCMLT_FIELD_ACCESS_READ_ONLY) {
            ci->cache_fields_attr[idx] |= CTR_INFO_CFA_VALID;
        }
    }

    /* Calculate the maximum number of entries */
    ci->num_misc_key_entries = 1;
    key = &ci->misc_keys[0];
    if (key->name) {
        ci->num_misc_key_entries = key->num * key->offset;
    }
    num_entries = ci->num_misc_key_entries;
    if (ci->port_id_key) {
        num_entries *= (ci->port_id_key->max + 1);
    }
    cache_size = num_entries * ei->num_def_fields;

    /* Create the cache for storing the previous counter value */
    ci->cache = sal_alloc(cache_size * sizeof(uint64_t), "bcmaCtrInfoCache");
    if (ci->cache == NULL) {
        ctr_info_destroy(ci);
        return NULL;
    }
    sal_memset(ci->cache, 0, cache_size * sizeof(uint64_t));

    /* Set the counter type if this is a pre-defined default counter */
    e = &def_counters_enum[0];
    while (e->name) {
        if (sal_strcmp(e->name, ei->name) == 0) {
            ci->types = e->val;
            break;
        }
        e++;
    }

    return ci;
}

/*
 * Get counter information for a specified LT from the list. If can not find
 * the LT in the list, create the counter information and add it to the list.
 */
static ctr_info_t *
ctr_info_get(int unit, const char *ctr_lt)
{
    ctr_info_t *ci = ctr_info_head[unit];

    /* Check if the counter information has been created */
    while (ci) {
        if (sal_strcasecmp(ci->ei->name, ctr_lt) == 0) {
            return ci;
        }
        ci = ci->next;
    }

    /* Create the counter information and add that to the tail of the list */
    ci = ctr_info_create(unit, ctr_lt);
    if (ci) {
        if (ctr_info_head[unit] == NULL) {
            ctr_info_head[unit] = ci;
        } else {
            ctr_info_t *tail = ctr_info_head[unit];

            while (tail->next) {
                tail = tail->next;
            }
            tail->next = ci;
        }
    } else {
        LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                    (BSL_META_U(unit,
                                "Failed to create ctr_info for %s\n"), ctr_lt));
    }
    return ci;
}

/* Check if the specified field is valid in the cached entry */
static bool
entry_field_is_cached(ctr_info_t *ci, int field_idx)
{
    uint32_t attr = CTR_INFO_CFA_VALID;

    return ((ci->cache_fields_attr[field_idx] & attr) != 0);
}

/* Calculate the cache entry base for the specified key indices */
static int
cache_entry_index(ctr_info_t *ci, int port, int offset)
{
    if (port >= 0) {
        offset += port * ci->num_misc_key_entries;
    }
    return offset * ci->ei->num_def_fields;
}

/* Callback function to show counter value for each valid entry */
static int
ctr_entry_show(void *cookie, ctr_info_t *ci, int port, int offset)
{
    ctr_show_data_t *usr_data = (ctr_show_data_t *)cookie;
    shr_pb_t *pb = usr_data->pb;
    uint32_t flags = usr_data->flags;
    bcma_bcmlt_entry_info_t *ei = ci->ei;
    int rv, idx, entry_base, cidx, kidx;
    bcmlt_field_def_t *df;
    ctr_info_key_field_t *key;
    uint64_t val, old_val;
    bool changed;

    entry_base = cache_entry_index(ci, port, offset);
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        if (entry_field_is_cached(ci, idx)) {
            df = &ei->def_fields[idx];
            rv = bcmlt_entry_field_get(ei->eh, df->name, &val);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            cidx = entry_base + idx;
            old_val = ci->cache[cidx];
            ci->cache[cidx] = val;
            if (old_val > val) {
                old_val = 0;
            }
            changed = (val != old_val);
            /* Show values according to the show flags */
            if ((((flags & CTR_SHOW_CHANGED) && changed) ||
                 ((flags & CTR_SHOW_SAME) && !changed)) &&
                (((flags & CTR_SHOW_ZERO) && val == 0) ||
                 ((flags & CTR_SHOW_NONZERO) && val != 0))) {
                shr_pb_printf(pb, "%s.%s", ei->name, df->name);
                for (kidx = 0; kidx < MAX_NUM_MISC_KEYS; kidx++) {
                    key = &ci->misc_keys[kidx];
                    if (key->name == NULL) {
                        break;
                    }
                    shr_pb_printf(pb, ".%s(%d)",
                                  key->name, (offset / key->offset) % key->num);
                }
                if (port >= 0) {
                    shr_pb_printf(pb, ".%d", port);
                }
                shr_pb_printf(pb, " %"PRIu64" +%"PRIu64"\n",
                              val, val - old_val);
            }
        }
    }

    return 0;
}

/* Callback function to clear counter value for each valid entry */
static int
ctr_entry_clear(void *cookie, ctr_info_t *ci, int port, int offset)
{
    int rv, idx, entry_base;
    bcma_bcmlt_entry_info_t *ei = ci->ei;
    bcmlt_field_def_t *df;

    /* Only update the cached fields */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        df = &ei->def_fields[idx];
        if (entry_field_is_cached(ci, idx)) {
            rv = bcmlt_entry_field_add(ei->eh, df->name, 0);
            if (SHR_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                            (BSL_META_U(ei->unit,
                                        "%s.%s: bcmlt_entry_field_add failed: "
                                        BCMA_BCMLT_ERRMSG_FMT"\n"),
                             ei->name, df->name, BCMA_BCMLT_ERRMSG(rv)));
            }
        } else if (!df->key) {
            bcmlt_entry_field_unset(ei->eh, df->name);
        }
    }
    rv = bcmlt_entry_commit(ei->eh,
                            BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_SUCCESS(rv)) {
        entry_base = cache_entry_index(ci, port, offset);
        for (idx = 0; idx < ei->num_def_fields; idx++) {
            if (entry_field_is_cached(ci, idx)) {
                ci->cache[entry_base + idx] = 0;
            }
        }
    } else {
        LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                    (BSL_META_U(ei->unit,
                                "%s: update operation failed: "
                                BCMA_BCMLT_ERRMSG_FMT"\n"),
                     ei->name, BCMA_BCMLT_ERRMSG(rv)));
    }

    return 0;
}

/*
 * Function to find all valid entries indexed by a specified port ID (port-based
 * LT), or all valid entries indexed by all keys fields (non-port-based LT)
 */
static int
ctr_lt_valid_entries_do(int unit, ctr_info_t *ci, int port,
                        int (*cb)(void *, ctr_info_t *, int, int), void *cookie)
{
    int rv, idx, kidx = 0;
    ctr_info_key_field_t *key;
    uint64_t key_val;
    const char *opstr;
    bcma_bcmlt_entry_info_t *ei = ci->ei;

    /* Add port to PORT_ID field for port-based LT */
    if (ci->port_id_key) {
        rv = bcmlt_entry_field_add(ei->eh, ci->port_id_key->name, port);
        if (SHR_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                        (BSL_META_U(ei->unit,
                                    "%s.%s: bcmlt_entry_field_add failed: "
                                    BCMA_BCMLT_ERRMSG_FMT"\n"),
                         ei->name, ci->port_id_key->name,
                         BCMA_BCMLT_ERRMSG(rv)));
        }
    }
    /* Iterate for all entries indexed by other keys */
    for (idx = 0; idx < ci->num_misc_key_entries; idx++) {
        for (kidx = 0; kidx < MAX_NUM_MISC_KEYS; kidx++) {
            key = &ci->misc_keys[kidx];
            if (key->name == NULL) {
                break;
            }
            key_val = (idx / key->offset) % key->num;
            rv = bcmlt_entry_field_add(ei->eh, key->name, key_val);
            if (SHR_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                            (BSL_META_U(ei->unit,
                                        "%s.%s: bcmlt_entry_field_add failed: "
                                        BCMA_BCMLT_ERRMSG_FMT"\n"),
                             ei->name, key->name, BCMA_BCMLT_ERRMSG(rv)));
            }
        }
        rv = bcmlt_entry_commit(ei->eh,
                                BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL);
        if (SHR_FAILURE(rv)) {
            if (rv != SHR_E_NOT_FOUND) {
                LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                            (BSL_META_U(ei->unit,
                                        "%s.%d: lookup operation failed: "
                                        BCMA_BCMLT_ERRMSG_FMT"\n"),
                             ei->name, port, BCMA_BCMLT_ERRMSG(rv)));
            }
            continue;
        }
        /* Check if the entry is valid for port-based counter table */
        if (ci->port_id_key) {
            rv = bcmlt_entry_field_symbol_get(ei->eh, "OPERATIONAL_STATE",
                                              &opstr);
            if (SHR_FAILURE(rv) || sal_strcmp(opstr, "VALID") != 0) {
                continue;
            }
        }
        if (cb) {
            cb(cookie, ci, port, idx);
        }
    }

    return 0;
}

/* Iterate for the marked counters in the list */
static int
counters_iterate(int unit, bcmdrd_pbmp_t *pbmp,
                int (*cb)(void *, ctr_info_t *, int, int), void *cookie)
{
    int port;
    ctr_info_t *ci;

    /* Port-based counter LTs */
    BCMDRD_PBMP_ITER(*pbmp, port) {
        ci = ctr_info_head[unit];
        while (ci) {
            if (ci->marked && ci->port_id_key) {
                ctr_lt_valid_entries_do(unit, ci, port, cb, cookie);
            }
            ci = ci->next;
        }
    }

    /* Non port-based counter LTs */
    ci = ctr_info_head[unit];
    while (ci) {
        if (ci->marked && ci->port_id_key == NULL) {
            ctr_lt_valid_entries_do(unit, ci, -1, cb, cookie);
        }
        ci = ci->next;
    }

    return 0;
}

/* Counter show process for the specified ports */
static int
counters_show(int unit, bcmdrd_pbmp_t *pbmp, uint32_t flags)
{
    ctr_show_data_t usr_data;
    shr_pb_t *pb;

    /* Set default flags if none are specified */
    if ((flags & (CTR_SHOW_CHANGED | CTR_SHOW_SAME)) == 0) {
        flags |= CTR_SHOW_CHANGED;
    }
    if ((flags & (CTR_SHOW_ZERO | CTR_SHOW_NONZERO)) == 0) {
        flags |= CTR_SHOW_NONZERO;
    }

    pb = shr_pb_create();

    usr_data.pb = pb;
    usr_data.flags = flags;

    counters_iterate(unit, pbmp, ctr_entry_show, &usr_data);

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return 0;
}

/* Counter clear process for the specified ports */
static int
counters_clear(int unit, bcmdrd_pbmp_t *pbmp)
{
    counters_iterate(unit, pbmp, ctr_entry_clear, NULL);

    return 0;
}

/* Clear the marked flag for all counters in the list */
static void
ctr_info_list_clear_marks(int unit)
{
    ctr_info_t *ci = ctr_info_head[unit];

    while (ci) {
        ci->marked = false;
        ci = ci->next;
    }
}

/*
 * If no counters are specified from the CLI, make sure the default counters
 * (which matches the specified types) are added to the list and marked as
 * target counters
 */
static int
ctr_info_list_mark_def_tbls(int unit, int *ctr_type)
{
    int idx, cnt = 0;
    bool skip;
    uint32_t type;
    ctr_info_t *ci;
    bcma_cli_parse_enum_t *e = &def_counters_enum[0];

    while (e->name) {
        ci = ctr_info_get(unit, e->name);
        if (ci) {
            skip = false;
            /* Check whether the counter types are specified and matched */
            for (idx = 0; idx < CTR_TYPES_NUM; idx++) {
                type = 1 << idx;
                if (ctr_type[idx] > 0 && (ci->types & type) == 0) {
                    skip = true;
                } else if (ctr_type[idx] < 0 && (ci->types & type) != 0) {
                    skip = true;
                }
            }
            if (!skip) {
                ci->marked = true;
                cnt++;
            }
        }
        e++;
    }
    return cnt;
}

/* Parse the counter type argument (eg. ed, !ed, tm, !tm) */
static int
bcmltcmd_arg_parse_ctr_type(const char *arg, int *ctr_types)
{
    int idx, op_not = FALSE;
    bcma_cli_parse_enum_t *e;;

    /* Process for boolean operator NOT */
    while (*arg == '!') {
        arg++;
        op_not = !op_not;
    }

    for (idx = 0;; idx++) {
        e = &ctr_types_enum[idx];
        if (e->name == NULL) {
            break;
        }
        if (bcma_cli_parse_cmp(e->name, arg, 0)) {
            break;
        }
    }

    if (idx < CTR_TYPES_NUM) {
        ctr_types[idx] = op_not ? -1 : 1;
        return 0;
    }
    return -1;
}

/* Command function for 'counter show' */
static int
bcmltcmd_counter_show(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    bcma_cli_parse_enum_t *e;
    uint32_t flags = 0;
    bcmdrd_pbmp_t pbmp;
    bool pbmp_valid = false;
    ctr_info_t *ci;
    int num_ctr = 0;
    int ctr_type[CTR_TYPES_NUM] = {0};

    ctr_info_list_clear_marks(unit);

    while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        /* Check for the counter show flags */
        e = &ctr_show_flags_enum[0];
        while (e->name) {
            if (bcma_cli_parse_cmp(e->name, arg, 0)) {
                flags |= e->val;
                break;
            }
            e++;
        }
        if (e->name) {
            continue;
        }
        /* Check for counter type */
        if (bcmltcmd_arg_parse_ctr_type(arg, ctr_type) == 0) {
            continue;
        }
        if (bcmdrd_pbmp_parse(arg, &pbmp) == 0) {
            /* Port list */
            pbmp_valid = true;
            continue;
        }
        /* Specified LT name */
        ci = ctr_info_get(unit, arg);
        if (ci) {
            ci->marked = true;
            num_ctr++;
        } else {
            cli_out("%s%s: Unsupported counter LT\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    /* Use default LTs if no LTs are specified. */
    if (num_ctr == 0) {
        num_ctr = ctr_info_list_mark_def_tbls(unit, ctr_type);
        if (num_ctr == 0) {
            cli_out("%sNon of the default counter LTs are supported\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    if (!pbmp_valid) {
        bcmdrd_dev_logic_pbmp(unit, &pbmp);
    }

    counters_show(unit, &pbmp, flags);

    return BCMA_CLI_CMD_OK;
}

/* Command function for 'counter clear' */
static int
bcmltcmd_counter_clear(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    const char *arg;
    bcmdrd_pbmp_t pbmp;
    bool pbmp_valid = false;
    ctr_info_t *ci;
    int num_ctr = 0;
    int ctr_type[CTR_TYPES_NUM] = {0};

    ctr_info_list_clear_marks(unit);

    while ((arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        /* Check for counter type */
        if (bcmltcmd_arg_parse_ctr_type(arg, ctr_type) == 0) {
            continue;
        }
        if (bcmdrd_pbmp_parse(arg, &pbmp) == 0) {
            /* Port list */
            pbmp_valid = true;
            continue;
        }
        /* Specified LT name */
        ci = ctr_info_get(unit, arg);
        if (ci) {
            ci->marked = true;
            num_ctr++;
        } else {
            cli_out("%s%s: Unsupported counter LT\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    /* Use default LTs if no LTs are specified. */
    if (num_ctr == 0) {
        num_ctr = ctr_info_list_mark_def_tbls(unit, ctr_type);
        if (num_ctr == 0) {
            cli_out("%sNon of the default counter LTs are supported\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    if (!pbmp_valid) {
        bcmdrd_dev_logic_pbmp(unit, &pbmp);
    }

    counters_clear(unit, &pbmp);

    return BCMA_CLI_CMD_OK;
}

/* Sub-commands and dispatch functions for the CLI counter command */
static bcma_cli_command_t counter_cmds[] = {
    { "Show",  bcmltcmd_counter_show },
    { "Clear", bcmltcmd_counter_clear }
};

int
bcma_bcmltcmd_counter_cleanup(bcma_cli_t *cli)
{
    int unit;
    ctr_info_t *ci;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        while (ctr_info_head[unit]) {
            ci = ctr_info_head[unit];
            ctr_info_head[unit] = ci->next;
            ctr_info_destroy(ci);
        }
    }

    return 0;
}

int
bcma_bcmltcmd_counter(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int idx;
    const char *arg;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    for (idx = 0; idx < COUNTOF(counter_cmds); idx++) {
        if (bcma_cli_parse_cmp(counter_cmds[idx].name, arg, 0)) {
            return (*counter_cmds[idx].func)(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
