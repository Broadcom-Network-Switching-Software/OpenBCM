/*! \file bcmcfg_read_device.c
 *
 * Device configuration handler. This uses the device configuration
 * schema to interpret the YAML event stream and populate the software
 * configuration data structures. The device configuration schema is
 * essentially the logical table definition.
 *
 * If the logical table is a "configuration table" (table_type:
 * config), then configuration data is saved to a High Availability
 * memory region, otherwise the data is saved in a temporary storage
 * area for BCMLTP (logical table playback).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_fmm.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_read_util.h>
#include <bcmcfg/bcmcfg_player.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/* Maximum number of elements allowed for a wide field. */
#define MAX_ELEMENTS 8

/* The PT values are from LTM - need to be made available for CFG. */

/*!
 * \brief Kind of table in descriptor.
 */
typedef enum bcmcfg_table_kind_e {
    /*! Unknown table. */
    BCMCFG_TBL_UNKNOWN,

    /*! Config logical table. */
    BCMCFG_TBL_CONFIG,

    /*! Standard Logical table. */
    BCMCFG_TBL_LOGICAL,

    /*! Physical table. */
    BCMCFG_TBL_PHYSICAL,

    /*! Config table as constraint. */
    BCMCFG_TBL_CONSTRAINT,

} bcmcfg_table_kind_t;

/*!
 * \brief Logical table parsing data.
 *
 * Logical table information that is tracked per-level.
 */
typedef struct bcmcfg_tbl_info_s {
    /*! device parser state */
    enum {
        /*! Unit - can transition to table or parent */
        UNIT,

        /*! Table - can transition to field or parent */
        TABLE,

        /*! Field - can transition to array or parent */
        FIELD,

        /*! Array - can transition to parent only */
        ARRAY,

        /*! Key range - can transition to parent only */
        KEY_RANGE
    } state;

    /*! Unit - true if unit in the set */
    bool unit_set[BCMDRD_CONFIG_MAX_UNITS];

    /*! Table kind. */
    bcmcfg_table_kind_t kind;

    /*! Logical Table representation. */
    const bcmltd_table_rep_t *lt;

    /*! Table Name. */
    const char *name;

    /*! True if SID is valid. */
    bool sid_valid;

    /*! Table ID. */
    uint32_t sid;

    /*! Field ID. */
    uint32_t fid;

    /*! Implicit field ID. */
    bool ifield;

    /*! Mapped Field ID. */
    uint32_t mfid;

    /*! Mapped Field value. */
    uint64_t mval;

    /*! Array index. */
    uint32_t aidx;

    /*! Physical table unit. */
    int pt_unit;

} bcmcfg_tbl_info_t;

/*!
 * \brief Range Information used for key range specification.
 */
typedef struct bcmcfg_key_range_s {
    /* Min key value. */
    uint32_t min;

    /* Max key value. */
    uint32_t max;

    /* Step value. */
    uint32_t step;

    /* Count. */
    uint32_t count;

    /* Next range. */
    struct bcmcfg_key_range_s *next;
} bcmcfg_key_range_t;

/*!
 * \brief Key range specification.
 */
typedef struct bcmcfg_key_range_spec_s {
    /* Field ID. */
    uint32_t fid;

    /* Total number of element. */
    uint32_t nelem;

    /* Total number of element. */
    uint32_t gap;

    /* Number of repeat. */
    uint32_t repeat;

    /* Key field range. */
    bcmcfg_key_range_t *ranges;

    /* Next field key specification. */
    struct bcmcfg_key_range_spec_s *next;
} bcmcfg_key_range_spec_t;

/*!
 * \brief Information for the list of key range specification.
 */
typedef struct bcmcfg_key_range_spec_list_s {
    /* Key range specification. */
    bcmcfg_key_range_spec_t *krs;

    /* Number of fields which key ranges are specified. */
    int num_krs;

    /* Total number of elements. */
    uint32_t num_entries;

    /* Pointer to base entry. */
    bcmcfg_tbl_list_t *lt;

    /* The flag that indicates if all relevant field parses are done.
     * If do_copy is set, base entry will be duplicated and overwrite
     * only key field value. */
    bool do_copy;
} bcmcfg_key_range_spec_list_t;

/*!
 * \brief Key field sequence parsing mode.
 *
 * Mode of parsing sequence event for key field value array.
 */
typedef enum bcmcfg_key_field_seq_mode_e {
    /*! Mode for single element. */
    BCMCFG_KEY_FIELD_SEQ_MODE_SINGLE_ELEMENT,

    /*! Mode for key field range specification. */
    BCMCFG_KEY_FIELD_SEQ_MODE_RANGE_SPEC,
} bcmcfg_key_field_seq_mode_t;

/*!
 * \brief Logical table parser user data.
 */
typedef struct bcmcfg_tbl_user_s {
    /*! Reader stack. */
    bcmcfg_tbl_info_t info[BCMCFG_MAX_LEVEL];

    /*! Table playback list head. */
    bcmcfg_playback_list_t *head;

    /*! Table playback list tail. */
    bcmcfg_playback_list_t *tail;

    /*! Append entries to table if true. */
    bool append;

    /*! Read context level to check
     *  the end of value field maps. */
    int level;
} bcmcfg_tbl_user_t;

/* User data passed to handler. */
static bcmcfg_tbl_user_t bcmcfg_tbl_user[BCMDRD_CONFIG_MAX_UNITS];
/* Current key field seq parsing mode. */
static bcmcfg_key_field_seq_mode_t
    bcmcfg_key_field_seq_mode = BCMCFG_KEY_FIELD_SEQ_MODE_SINGLE_ELEMENT;
/* Current key field range. */
static bcmcfg_key_range_t bcmcfg_key_range_current;
/* Key range data. */
static bcmcfg_key_range_spec_list_t bcmcfg_key_range_list_user;
/* Tail key range spec pointer */
static bcmcfg_key_range_spec_t *bcmcfg_key_range_spec_tail = NULL;

/* Parse information. */
static bcmcfg_parse_info_t pinfo = {
    .mode = PARSE_MODE_UNIT,
    .target_unit = BSL_UNIT_UNKNOWN
};

/*******************************************************************************
 * Private functions
 */
static int
bcmcfg_playback_free_tbl(bcmcfg_tbl_list_t *t_list);

/*!
 * \brief Delete all key range specification.
 *
 * Delete all key range specification.
 *
 * \param [in]     kr_list      Key range specification list.
 */
static void
bcmcfg_key_range_delete_all(bcmcfg_key_range_spec_list_t *kr_list)
{
    bcmcfg_key_range_spec_t *krs = kr_list->krs;
    bcmcfg_key_range_spec_t *next_krs = NULL;
    bcmcfg_key_range_t *ranges = NULL;
    bcmcfg_key_range_t *next_ranges = NULL;

    while (krs) {
        next_krs = krs->next;
        ranges = krs->ranges;
        while (ranges) {
            next_ranges = ranges->next;
            SHR_FREE(ranges);
            ranges = next_ranges;
        }
        SHR_FREE(krs);
        krs = next_krs;
    }

    return;
}

/*!
 * \brief Clear current range information
 *
 * Clear current range information.
 */
static void
bcmcfg_key_range_status_clear(void)
{
    bcmcfg_key_range_current.min = -1;
    bcmcfg_key_range_current.max = -1;
    bcmcfg_key_range_current.step = 1;
    bcmcfg_key_range_current.count = 0;

    bcmcfg_key_field_seq_mode = BCMCFG_KEY_FIELD_SEQ_MODE_SINGLE_ELEMENT;
    return;
}

/*!
 * \brief Initialize key range specification.
 *
 * Initialize key range specification.
 */
static void
bcmcfg_key_range_spec_init(void)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;

    bcmcfg_key_range_delete_all(kr_list);
    sal_memset(kr_list, 0, sizeof(*kr_list));
    kr_list->do_copy = false;
    kr_list->num_krs = 0;
    kr_list->num_entries = 1;
    bcmcfg_key_range_spec_tail = NULL;

    bcmcfg_key_range_status_clear();
    return;
}

/*!
 * \brief Calculate total number of entries.
 *
 * Calculate total number of entries.
 */
static void
bcmcfg_key_range_spec_entries(void)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    bcmcfg_key_range_spec_t *krs = kr_list->krs;

    while (krs) {
        kr_list->num_entries *= krs->nelem;
        krs = krs->next;
    }

    return;
}

/*!
 * \brief Key range validation.
 *
 * Validate key range specification.
 *
 * Accepts all key ranges; actual range checking will occur during
 * playback. This interface is kept so that a future range check
 * requirement has an implementation point.
 *
 * \param [in,out] info         Reader Data.
 * \param [in,out] tbl_info     Local LT data.
 */
static int
bcmcfg_key_range_spec_validate(bcmcfg_read_level_info_t *info,
                               bcmcfg_tbl_info_t *tbl_info)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add key range.
 *
 * Add key range into key range spec list.
 *
 * \param [in]    fid        Field ID.
 * \param [in]    min        Minimum value of the range.
 * \param [in]    max        Maximum value of the range.
 * \param [in]    step       Step size of the range.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_key_range_spec_add(uint32_t fid,
                          uint64_t min,
                          uint64_t max,
                          uint64_t step)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    bcmcfg_key_range_spec_t *krs = kr_list->krs;
    bcmcfg_key_range_t *range = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while (krs) {
        if (fid == krs->fid) {
            break;
        }
        krs = krs->next;
    }

    /* new field range */
    SHR_ALLOC(range, sizeof(*range), "bcmcfgRange");
    SHR_NULL_CHECK(range, SHR_E_MEMORY);
    range->min = min;
    range->max = max;
    range->step = step;
    range->next = NULL;

    /* new field range */
    if (krs == NULL) {
        bcmcfg_key_range_spec_t *new = NULL;
        SHR_ALLOC(new, sizeof(bcmcfg_key_range_spec_t), "bcmcfgKeyRangeSpec");
        SHR_NULL_CHECK(new, SHR_E_MEMORY);
        sal_memset(new, 0, sizeof(*new));
        new->fid = fid;
        new->nelem = (max - min)/step + 1;
        new->ranges = range;
        if (bcmcfg_key_range_spec_tail == NULL) {
            kr_list->krs = new;
        } else {
            bcmcfg_key_range_spec_tail->next = new;
        }
        bcmcfg_key_range_spec_tail = new;
        kr_list->num_krs++;
    } else {
        bcmcfg_key_range_t *cur = krs->ranges;
        krs->fid = fid;
        while (cur) {
            if (!cur->next) {
                break;
            }
            cur = cur->next;
        }
        SHR_NULL_CHECK(cur, SHR_E_INTERNAL);

        cur->next = range;
        krs->nelem += (max - min)/step + 1;
    }

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(range);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate gap and repeat for each key range field.
 *
 * Gap and Repeat.
 */
static void
bcmcfg_key_range_spec_info_calc(void)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    bcmcfg_key_range_spec_t *krs = kr_list->krs;
    /* accumulated number of element. */
    uint32_t acc_nelem = 1;

    while (krs) {
        krs->gap = acc_nelem * (krs->nelem - 1);
        krs->repeat = acc_nelem;

        acc_nelem *= krs->nelem;
        krs = krs->next;
    }

    return;
}

/*!
 * \brief Parse a string to a uint32.
 *
 * Parse a string up a uint64_t word.
 *
 * \param [in]     str          String.
 * \param [out]    valuep       Pointer to output value.
 * \param [in]     base         Numeric base.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_str_to_uint32(const char *str, uint32_t *valuep, int base)
{
    uint32_t value;
    char *end;
    int rv = SHR_E_NONE;

    value = sal_strtoul(str, &end, base);
    if (*str && *end) {
        /* Invalid conversion */
        rv = SHR_E_PARAM;
    } else {
        *valuep = value;
    }

    return rv;
}

/*!
 * \brief Parse a string to a uint64.
 *
 * Parse a string up a uint64_t word.
 *
 * \param [in]     str          String.
 * \param [out]    valuep       Pointer to output value.
 * \param [in]     base         Numeric base.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_str_to_uint64(const char *str, uint64_t *valuep, int base)
{
    uint64_t value;
    char *end;
    int rv = SHR_E_NONE;

    value = (uint64_t)sal_strtoull(str, &end, base);
    if ((*str && *end) || (str == end)) {
        /*
         * Invalid conversion: there is a non-empty string but the end
         * address isn't pointing to a trailing NUL character (0), or
         * the start address of the string is the same as the end
         * address, implying that no numeric conversion was possible.
         */
        rv = SHR_E_PARAM;
    } else {
        *valuep = value;
    }

    return rv;
}

/*!
 * \brief Parse a hex integer string.
 *
 * Parse a hex integer string up to 'size' number of uint64_t words.
 *
 * \param [in]     str          String.
 * \param [out]    fval         Pointer to output value array.
 * \param [in]     size         Size of output value array.
 *
 * TODO - eliminate strdup and free, handle conversion errors *
 * pointer count argument reversed from normal
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_parse_hex(const char *str, uint64_t *fval, size_t size, size_t *actual)
{
    size_t cnt = 0;
    char buf[17];       /* Conversion buffer. */
    int n;              /* Length of string to convert. */
    int m;              /* Offset of string to convert. */
    int len;            /* Number of characters to convert. */
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (sal_strncmp(str, "0x", 2)) {
        /* String must start with '0x' */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Ignore the leading 0x */
    str += 2;
    n = sal_strlen(str);
    m = 0;
    len = 16;

    if (n > len) {
        /*
         * Multiple pieces to convert, so start at the rightmost
         * 16 bytes.
         */
        m = n - len;
    } else {
        len = n;
    }

    while (n > 0 && cnt < size) {
        sal_memcpy(buf, str + m, len);
        buf[len] = 0;
        rv = bcmcfg_str_to_uint64(buf, fval + cnt, 16);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Invalid hex value %s\n"), str));
            SHR_ERR_EXIT(rv);
        }

        cnt++;
        n -= len; /* Reduce number of characters converted. */
        if (n < len) {
            len = n;
        }
        m -= len; /* Move offset to the left. */
    }

    if (cnt > 0) {
        *actual = cnt;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for physical table.
 *
 * Search for a physical table by name.
 *
 * \param [in,out] tbl_info     LT data.
 * \param [in]     key          Map key.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_pt_search(bcmcfg_tbl_info_t *tbl_info,
                      const char *key)
{
    int unit;
    uint32_t ptsid;
    bool assigned = false;
    int rv;
    const char *name;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!tbl_info->unit_set[unit]) {
            continue;
        }

        rv = bcmdrd_pt_name_to_sid(unit, key, &ptsid);

        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        if (!assigned) {
            tbl_info->pt_unit = unit;
            tbl_info->sid = ptsid;
            /* Lookup the name to avoid a memory allocation for only
               physical table names. */
            name = bcmdrd_pt_sid_to_name(unit, ptsid);
            if (name == NULL) {
                /* If the reverse lookup to name fails, then something
                 is seriously wrong. */
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            tbl_info->name = name;
            assigned = true;
        } else if (tbl_info->sid != ptsid) {
            /* PT sid inconsistent across units. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Physical table %s SID "
                                "is inconsistent across units\n"), key));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Search for a map key.
 *
 * Search for a map key in either the list of logical tables, or the
 * list of physical tables. If the key is found, set the table
 * kind and table SID corresponding to the found key. If the key is
 * not found, return SHR_E_NOT_FOUND.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     key          Map key.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_search(bcmcfg_tbl_info_t *tbl_info,
                          const char *key)
{
    int idx = SHR_E_NOT_FOUND;
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!tbl_info->unit_set[unit]) {
            continue;
        }
        idx = bcmlrd_table_name_to_idx(unit, key);
        break;
    }

    if (SHR_SUCCESS(idx)) {
        tbl_info->sid = (bcmltd_sid_t)idx;
        /* Won't know until playback if non-config table is mapped to
           this unit... */
        tbl_info->lt = bcmltd_table_get(tbl_info->sid);
        if (tbl_info->lt == NULL) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else {
            tbl_info->name = tbl_info->lt->name;
            tbl_info->kind =
                (tbl_info->lt->flags & BCMLTD_TABLE_F_CONFIG) ?
                BCMCFG_TBL_CONFIG : BCMCFG_TBL_LOGICAL;
            tbl_info->sid_valid = true;
        }
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            const bcmlrd_map_t *map;
            int rv;

            if (!tbl_info->unit_set[unit]) {
                continue;
            }

            rv = bcmlrd_map_get(unit, tbl_info->sid, &map);
            if (SHR_FAILURE(rv)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Table %s not mapped for unit %d\n"), key, unit));
            }
            break;
        }

    } else if (SHR_SUCCESS(bcmcfg_read_pt_search(tbl_info, key))) {
        tbl_info->kind = BCMCFG_TBL_PHYSICAL;
        tbl_info->sid_valid = true;
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup for table entries.
 *
 * \param [in,out] info         Reader Data.
 * \param [in]     level        Reader Level.
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_table_setup(bcmcfg_read_level_info_t *info,
                               int level,
                               bcmcfg_tbl_user_t *user,
                               bcmcfg_tbl_info_t *tbl_info)
{
    bcmcfg_tbl_list_t **list = NULL;
    bcmcfg_tbl_list_t *tbl = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (tbl_info->kind) {
    case BCMCFG_TBL_LOGICAL:
        list = &user->tail->lt;
        break;
    case BCMCFG_TBL_PHYSICAL:
        list = &user->tail->pt;
        break;
    case BCMCFG_TBL_CONFIG:
        /* No setup needed. */
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

    if (list) {
        tbl = sal_alloc(sizeof(*tbl), "bcmcfgReadDeviceTableSetup");
        if (tbl) {
            sal_memset(tbl, 0, sizeof(*tbl));
            tbl->sid = tbl_info->sid;
            tbl->name = tbl_info->name;
            tbl->line = info->line;
            tbl->column = info->column;
            sal_memcpy(tbl->unit_set,
                       tbl_info->unit_set,
                       sizeof(tbl->unit_set));
            tbl->next = *list;
            *list = tbl;
            user->append = true;
            user->level = level;
        } else {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if field is valid.
 *
 * Check if field is valid.
 *
 * \param [in]     unit         Unit number.
 * \param [in]     info         Read context.
 * \param [in]     sid          Logical Table ID.
 * \param [in]     fid          Field ID.
 * \param [in]     nfields      Number of fields in this LT.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_config_lt_field_check(int unit,
                                         bcmcfg_read_level_info_t *info,
                                         uint32_t sid,
                                         uint32_t fid,
                                         uint32_t nfields)
{
    bool is_unmapped;

    SHR_FUNC_ENTER(unit);

    if (fid >= nfields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Field ID is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_is_unmapped(unit, sid, fid, &is_unmapped));

    if (is_unmapped) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "%s:%d:%d: Unmapped field.\n"),
                   info->locus,
                   info->line,
                   info->column));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Test if field array.
 *
 * Return true if the current field is a field array.
 *
 * \param [in]     tbl_info     Local LT data.
 * \param [out]    tf           Boolean flag..
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_is_field_array(const bcmcfg_tbl_info_t *tbl_info, bool *tf)
{
    const bcmltd_field_rep_t *field;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    *tf = false;

    if (tbl_info->fid >= tbl_info->lt->fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field ID is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field = tbl_info->lt->field + tbl_info->fid;

    /* Is array if it has any depth. */
    if (field->depth > 0) {
        *tf = true;

        /*
         * By definition, the array field width has to be scalar,
         * and is enforced by tooling, so this is really an extra
         * consistency check.
         */
        if (field->width > BCM_FIELD_SIZE_BITS) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Field is too wide.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Test if key field.
 *
 * Return true if the current field is a key field.
 *
 * \param [in]     tbl_info     Local LT data.
 * \param [out]    is_key       Boolean flag..
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_is_key_field(const bcmcfg_tbl_info_t *tbl_info,
                                bool *is_key)
{
    const bcmltd_field_rep_t *field;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    *is_key = false;

    if (tbl_info->fid >= tbl_info->lt->fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field ID is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field = tbl_info->lt->field + tbl_info->fid;

    if (field->flags & BCMLTD_FIELD_F_KEY) {
        *is_key = true;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Test if field value is within the field range of table definition.
 *
 * \param [in]     tbl_info     Local LT data.
 * \param [in]     value        Field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_is_field_range_valid(const bcmcfg_tbl_info_t *tbl_info,
                                        uint64_t value)
{
    const bcmltd_field_rep_t *field;
    uint64_t max;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    field = tbl_info->lt->field + tbl_info->fid;

    /* Field width has to be scalar. */
    if (field->width > BCM_FIELD_SIZE_BITS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field is too wide.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Calculate max value based on the field width. */
    max = (field->width == BCM_FIELD_SIZE_BITS) ?
        ~0ULL : ((1ULL << field->width) - 1);
    if (value > max) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by number.
 *
 * Add a unit to the set of units with table assignments.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     unit         Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit(bcmcfg_tbl_info_t *tbl_info, int unit)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (pinfo.mode != PARSE_MODE_UNIT_EARLY &&
        pinfo.mode != PARSE_MODE_UNIT) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Add a unit */
    if (bcmdrd_dev_exists(unit)) {
        if (pinfo.target_unit == unit) {
            tbl_info->unit_set[unit] = true;
        } else {
        }
    } else {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add all units.
 *
 * Add all units that currently exist.
 *
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_all_units(bcmcfg_tbl_info_t *tbl_info)
{
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_unit(tbl_info, unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add unit by name.
 *
 * Add all units that correspond to the given name.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit name.
 * \param [out]    found        Name found flag.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_name(bcmcfg_tbl_info_t *tbl_info,
                                 const char *str,
                                 bool *found)
{
    int len;
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    len = sal_strlen(str);
    *found = false;
    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {

        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        if (!sal_strncmp(bcmdrd_dev_name(unit), str, len)) {
            SHR_IF_ERR_EXIT
                (bcmcfg_read_device_add_unit(tbl_info, unit));
            *found = true;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a unit by numeric string.
 *
 * Add a unit indicated by a numeric string.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit number string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_num(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    uint32_t value;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    rv = bcmcfg_str_to_uint32(str, &value, 0);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_unit(tbl_info, value));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid unit number %s.\n"), str));
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear table info.
 *
 * Clear table info.
 *
 * \param [in]     tbl_info      Table info.
 */
static void
bcmcfg_read_device_tbl_info_clear(bcmcfg_tbl_info_t *tbl_info)
{
    sal_memset(tbl_info, 0, sizeof(bcmcfg_tbl_info_t));
}

/*!
 * \brief Add a unit by string.
 *
 * Add a unit to the set of units by string.
 *
 * The string may be an integer or a device name.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Unit string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_unit_str(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    bool found = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!sal_strcmp(str, "*")) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_all_units(tbl_info));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_unit_name(tbl_info, str, &found));
        if (!found) {
            SHR_IF_ERR_EXIT
                (bcmcfg_read_device_add_unit_num(tbl_info, str));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup for config field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * Setup configuration table data.
 *
 * This must eventually be allocated from HA memory.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_setup_cfg_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_tbl_info_t *tbl_info)
{
    size_t i;
    bcmcfg_config_data_t *data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Allocate data for each configured unit. */
    for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
        int rv;

        rv = bcmcfg_tbl_user_config_get(i, tbl_info->sid, &data);
        if (SHR_FAILURE(rv)) {
            break;
        }

        if (!tbl_info->unit_set[i] ||
            data != NULL) {
            continue;
        }

        rv = bcmcfg_config_table_field_alloc(i, tbl_info->sid, &data);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmcfg_tbl_user_config_set(i, tbl_info->sid, data);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert LT enum to scalar value.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Enum string.
 * \param [out]    out          Enum value.
 *
 * Convert enum string to value. This value must be consistent across
 * the given set of units.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_str_to_enum(bcmcfg_tbl_info_t *tbl_info,
                   const char *str,
                   uint64_t *out)
{
    uint64_t value;
    uint64_t provisional;
    uint64_t *current = &value;
    bool has_value = false;
    bool unit_set = false;
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!tbl_info->unit_set[unit]) {
            continue;
        }
        unit_set = true;

        SHR_IF_ERR_EXIT
            (bcmlrd_resolve_enum(unit,
                                 tbl_info->sid,
                                 tbl_info->fid,
                                 str,
                                 current));
        if (!has_value) {
            current = &provisional;
            has_value = !has_value;
        } else if (value != provisional) {
            /* enum inconsistent across units */
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    if (!unit_set) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    *out = value;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle implicit key field.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          String.
 *
 * Handle a implicit key, where the key value is used as a YAML map key.
 *
 * Table must be an index table, and mfid must be within range of the
 * fields, and indicate a non-enum key with a width less than or equal
 * to 64 bits.
 * Setup configuration table data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_implicit_key(bcmcfg_tbl_info_t *tbl_info,
                                const char *str)
{
    const bcmltd_field_rep_t *field;
    int rv;
    uint64_t value;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Has to be an index table. */
    if ((tbl_info->lt->flags & BCMLTD_TABLE_F_TYPE_MASK) !=
        BCMLTD_TABLE_F_TYPE_INDEX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Not an index table.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Implicit field ID has to be in range. */
    if (tbl_info->mfid >= tbl_info->lt->fields) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field ID is out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    field = tbl_info->lt->field + tbl_info->mfid;

    /* Field width has to be scalar. */
    if (field->width > BCM_FIELD_SIZE_BITS) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field is too wide.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmcfg_str_to_uint64(str, &value, 0);
    if (SHR_SUCCESS(rv)) {
        if (field->flags & BCMLTD_FIELD_F_ENUM) {
            /* A digit number is used for enum field. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Table %s Field %s is enum field.\n"),
                       tbl_info->name, field->name));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        if (field->flags & BCMLTD_FIELD_F_ENUM) {
            /* If given string is non-digit,
             * try to resolve it as enum. */
            tbl_info->fid = tbl_info->mfid;
            rv = bcmcfg_str_to_enum(tbl_info, str, &value);
        }
    }

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid field name %s\n"), str));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Met all the requirements for implicit field assignment. */
    tbl_info->ifield = true;
    tbl_info->fid = tbl_info->mfid++;
    tbl_info->mval = value;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup logical table field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current table. If not found, assume
 * it may be a field value for an implicit key. If the key is found
 * for a config table, set up the config data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_lt_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_read_level_info_t *info,
                                   bcmcfg_tbl_info_t *tbl_info,
                                   const char *str)
{
    size_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (i = 0; i < tbl_info->lt->fields; i++) {
        if (!sal_strcmp(str, tbl_info->lt->field[i].name)) {
            tbl_info->fid = i;
            break;
        }
    }

    if (i == tbl_info->lt->fields) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcfg_read_device_implicit_key(tbl_info, str));
    } else if (tbl_info->kind == BCMCFG_TBL_CONFIG) {
        int iunit;

        for (iunit = 0; iunit < BCMDRD_CONFIG_MAX_UNITS; iunit++) {
            if (!tbl_info->unit_set[iunit]) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT(
                bcmcfg_read_device_config_lt_field_check(iunit,
                                                         info,
                                                         tbl_info->sid,
                                                         tbl_info->fid,
                                                         tbl_info->lt->fields));
        }
        SHR_IF_ERR_VERBOSE_EXIT(bcmcfg_read_device_setup_cfg_field(user,
                                                                   tbl_info));
    }
    /* Non-config fields do not require any setup. */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup physical table field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current physical table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_pt_field(bcmcfg_tbl_user_t *user,
                                   bcmcfg_tbl_info_t *tbl_info,
                                   const char *str)
{
    int unit;
    uint32_t ptfid = 0;
    bool assigned = false;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!sal_strcmp(str, "__INDEX")) {
        ptfid = BCMCFG_PT_FIELD_KEY_INDEX;
    } else if (!sal_strcmp(str, "__PORT")) {
        ptfid = BCMCFG_PT_FIELD_KEY_PORT;
    }

    if (ptfid == 0) {
        for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
            if (!tbl_info->unit_set[unit]) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_name_to_fid(unit,
                                             tbl_info->sid,
                                             str,
                                             &ptfid));
            if (!assigned) {
                tbl_info->fid = ptfid;
                assigned = true;
            } else if (tbl_info->fid != ptfid) {
                /* PT fid inconsistent across units. */
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Physical table FID %s "
                                    "is inconsistent across units\n"), str));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        tbl_info->fid = ptfid;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup field.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field string.
 *
 * Lookup a field string in the current table logical or physical
 * table.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_lookup_field(bcmcfg_tbl_user_t *user,
                                bcmcfg_read_level_info_t *info,
                                bcmcfg_tbl_info_t *tbl_info,
                                const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_lookup_lt_field(user, info, tbl_info, str));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_lookup_pt_field(user, tbl_info, str));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find field pointer in playback tbl data.
 *
 * \param [in,out] tbl          Playback table data.
 * \param [in]     fid          Field ID.
 * \param [in]     idx          Field array index.
 *
 * \retval field field pointer
 */
static shr_fmm_t *
bcmcfg_read_device_field_find(bcmcfg_tbl_list_t *tbl,
                              uint32_t fid,
                              uint32_t idx)
{
    shr_fmm_t *field = tbl->field;
    bool found = 0;

    while (field) {
        if (field->id == fid &&
            field->idx == idx) {
            found = 1;
            break;
        }
        field = field->next;
    }

    return (found) ? field : (NULL);
}

/*!
 * \brief Link field.
 *
 * \param [in,out] tbl          Playback table data.
 * \param [in]     fid          Field ID.
 * \param [in]     idx          Field array index.
 * \param [in]     value        Field value.
 *
 * Append a single field record to the current table playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_link_field(bcmcfg_tbl_list_t *tbl,
                              uint32_t fid,
                              uint32_t idx,
                              uint64_t value)
{
    shr_fmm_t *field;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (tbl == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Cannot find table entry data\n")));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    field = bcmcfg_read_device_field_find(tbl, fid, idx);
    if (field) {
        /* update value if field pointer already
         * exists in table data. */
        field->data = value;
    } else {
        field = sal_alloc(sizeof(*field), "bcmcfgReadDeviceLinkField");

        if (field && tbl) {
            sal_memset(field, 0, sizeof(*field));
            field->id = fid;
            field->data = value;
            field->idx = idx;
            field->next = tbl->field;
            tbl->field = field;
        } else {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Link fields.
 *
 * \param [in,out] tbl          Playback table data.
 * \param [in]     fid          Field ID.
 * \param [in]     count        Number of elements in field array.
 * \param [in]     value        Field array.
 *
 * Append an array of field values to the current table playback list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_link_fields(bcmcfg_tbl_list_t *tbl,
                               uint32_t fid,
                               size_t count,
                               const uint64_t *value)
{
    size_t idx;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (idx = 0; idx < count; idx++ ) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_link_field(tbl,
                                           fid,
                                           idx,
                                           value[idx]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set scalar fields.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     idx          Field index.
 * \param [in]     value        Field value.
 *
 * Set a scalar value of the table and field indicated in the local LT
 * data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_simple_field(bcmcfg_tbl_user_t *user,
                                    bcmcfg_read_level_info_t *info,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    uint32_t idx,
                                    uint64_t value)
{
    size_t i;
    uint32_t sid = tbl_info->sid;
    uint32_t fid = tbl_info->fid;
    bcmcfg_config_data_t *data;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
        for (i = 0; i < BCMDRD_CONFIG_MAX_UNITS; i++) {
            int rv;
            uint32_t field_index = 0;

            if (!tbl_info->unit_set[i]) {
                continue;
            }

            rv = bcmcfg_tbl_user_config_get(i, sid, &data);
            if (SHR_FAILURE(rv)) {
                break;
            }

            if (data == NULL) {
                /* The config table should have been set up by
                   table key processing. If it's not, then there
                   was some earlier problem, like a missing table
                   key. */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_EXIT(
                bcmcfg_tbl_user_config_field_index_get(i,
                                                       sid,
                                                       fid,
                                                       idx,
                                                       &field_index));
            /* Get config locus data and set. */
            data[field_index].value  = value;
            if (data[field_index].locus) {
                sal_free((void *)data[field_index].locus);
            }
            data[field_index].locus  = sal_strdup(info->locus);
            data[field_index].line   = info->line;
            data[field_index].column = info->column;
            data[field_index].config_src = BCMCFG_CONFIG_SRC_TYPE_READER;

            rv = bcmcfg_tbl_user_config_hook(i, sid, fid, value);
            if (SHR_FAILURE(rv)) {
                break;
            }

            rv = bcmcfg_tbl_user_config_set(i, sid, data);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }
        break;
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_link_field(user->tail->lt,
                                           fid,
                                           idx,
                                           value));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_link_field(user->tail->pt,
                                           fid,
                                           idx,
                                           value));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set scalar fields.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     idx          Field index.
 * \param [in]     str          Field value string.
 *
 * Convert the value string to an integer and set a scalar value of
 * the table and field indicated in the local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_simple_field_str(bcmcfg_tbl_user_t *user,
                                        bcmcfg_read_level_info_t *info,
                                        bcmcfg_tbl_info_t *tbl_info,
                                        uint32_t idx,
                                        const char *str)
{
    const bcmltd_field_rep_t *field = tbl_info->lt->field + tbl_info->fid;
    uint64_t value = 0;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (field->depth > 0 && idx >= field->depth) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Array index out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* check for enum */
    if (field->flags & BCMLTD_FIELD_F_ENUM) {
        SHR_IF_ERR_EXIT
            (bcmcfg_str_to_enum(tbl_info, str, &value));
    } else if (!sal_strcmp(str, "")) {
        /*
         * Handle an empty scalar value. The only context where
         * this is known to occur is after a complex key that has
         * no associated values. This is a degenerate but valid
         * case. Because the device schema otherwise requires
         * non-empty values, as all device schema values are
         * either numbers or enum symbols, the empty scalar only
         * serves to mark the end of the complex key clause. There
         * are no further actions needed for this particular case,
         * so the empty scalar may be otherwise safely ignored.
         */
        SHR_EXIT();
    } else {
        rv = bcmcfg_str_to_uint64(str, &value, 0);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Invalid field value %s\n"), str));
            SHR_ERR_EXIT(rv);
        }
    }

    /* Valid conversion */
    SHR_IF_ERR_EXIT
        (bcmcfg_read_device_set_simple_field(user,
                                             info,
                                             tbl_info,
                                             idx,
                                             value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert array index.
 *
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Index value string.
 *
 * Convert the value string to an integer and set the current array
 * index.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_aidx(bcmcfg_tbl_info_t *tbl_info, const char *str)
{
    const bcmltd_field_rep_t *field =
        tbl_info->lt->field + tbl_info->fid;
    uint32_t value;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    rv = bcmcfg_str_to_uint32(str, &value, 0);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Invalid array index %s\n"), str));
        SHR_ERR_EXIT(rv);
    }

    if ((value >= field->depth) || (value != 0 && field->depth == 0)) {
        /* Index out of range for array or != 0 if not array. */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Array index out of range.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl_info->aidx = value;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add implicit field to table.
 *
 * Add implicit field value to table.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_implicit_field_add(bcmcfg_tbl_user_t *user,
                                      bcmcfg_read_level_info_t *info,
                                      bcmcfg_tbl_info_t *tbl_info)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (bcmcfg_read_device_set_simple_field(user,
                                             info,
                                             tbl_info,
                                             0, tbl_info->mval));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set an LT field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value.
 *
 * Set a field value, dealing with enum, scalar and wide fields.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_lt_field_str(bcmcfg_tbl_user_t *user,
                                    bcmcfg_read_level_info_t *info,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    const char *str)
{
    const bcmltd_field_rep_t *field = tbl_info->lt->field + tbl_info->fid;
    uint32_t elements = bcmltd_field_idx_count_get(field->width);
    size_t count;
    uint64_t a_value[MAX_ELEMENTS];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (elements != 1 && tbl_info->kind == BCMCFG_TBL_CONFIG) {
        /* FLTG prevents this case. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (field->depth != 0 && tbl_info->kind == BCMCFG_TBL_CONFIG) {
        /* FLTG prevents this case. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Cannot set a read only field of a non-config table. */
    if (tbl_info->kind != BCMCFG_TBL_CONFIG &&
        field->flags & BCMLTD_FIELD_F_READ_ONLY) {
        /* Not supported at all. */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Field is read-only.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (elements == 1) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_simple_field_str(user,
                                                     info,
                                                     tbl_info,
                                                     0,
                                                     str));
    } else if (elements <= MAX_ELEMENTS) {
        sal_memset(a_value, 0, sizeof(a_value));
        SHR_IF_ERR_EXIT
            (bcmcfg_parse_hex(str, a_value, COUNTOF(a_value), &count));
        if (count <= elements) {
            SHR_IF_ERR_EXIT
                (bcmcfg_read_device_link_fields(user->tail->lt,
                                                tbl_info->fid,
                                                elements,
                                                a_value));
        } else {
            /* More words than elements. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Value is too wide.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Too many elements in table definition.\n")));
        /* Too many elements - not supported at all. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a PT field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value.
 *
 * Set a Physical table field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_pt_field_str(bcmcfg_tbl_user_t *user,
                                    bcmcfg_tbl_info_t *tbl_info,
                                    const char *str)
{
    uint32_t elements = 0;
    size_t count;
    uint64_t a_value[MAX_ELEMENTS];

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if ((tbl_info->fid == BCMCFG_PT_FIELD_KEY_INDEX) ||
        (tbl_info->fid == BCMCFG_PT_FIELD_KEY_PORT)) {
        elements = 1;
    } else {
        int minbit;
        int maxbit;

        minbit = bcmdrd_pt_field_minbit(tbl_info->pt_unit,
                                        tbl_info->sid,
                                        tbl_info->fid);

        if (minbit < 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        maxbit = bcmdrd_pt_field_maxbit(tbl_info->pt_unit,
                                        tbl_info->sid,
                                        tbl_info->fid);

        if (maxbit < 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        elements = bcmltd_field_idx_count_get(maxbit - minbit + 1);
    }

    if (elements <= MAX_ELEMENTS) {
        sal_memset(a_value, 0, sizeof(a_value));
        SHR_IF_ERR_EXIT
            (bcmcfg_parse_hex(str, a_value, COUNTOF(a_value), &count));
        if (count <= elements) {
            SHR_IF_ERR_EXIT
                (bcmcfg_read_device_link_fields(user->tail->pt,
                                                tbl_info->fid,
                                                elements,
                                                a_value));
        } else {
            /* More words than elements. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Value is too wide.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Too many elements in physical table.\n")));
        /* Too many elements - not supported at all. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Field value string.
 *
 * Set a field value for all table types.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_set_field(bcmcfg_tbl_user_t *user,
                             bcmcfg_read_level_info_t *info,
                             bcmcfg_tbl_info_t *tbl_info,
                             const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (tbl_info->kind) {
    case BCMCFG_TBL_CONFIG:
    case BCMCFG_TBL_LOGICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_lt_field_str(user,
                                                 info,
                                                 tbl_info,
                                                 str));
        break;
    case BCMCFG_TBL_PHYSICAL:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_pt_field_str(user,
                                                 tbl_info,
                                                 str));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Handle a field value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Read context.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     read_state   Reader state.
 * \param [in]     str          Field string.
 *
 * Handle a field key, which may be a field name, or a field value.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_handle_field(bcmcfg_tbl_user_t *user,
                                bcmcfg_read_level_info_t *info,
                                bcmcfg_tbl_info_t *tbl_info,
                                bcmcfg_read_state_t read_state,
                                const char *str)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (read_state) {
    case BCMCFG_READ_STATE_MAP:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_lookup_field(user, info, tbl_info, str));
        break;
    case BCMCFG_READ_STATE_SEQ:
    case BCMCFG_READ_STATE_MAP_V:
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_field(user, info, tbl_info, str));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an array value.
 *
 * Add a value to a field array.
 *
 * \param [in,out] info         Reader Data
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Value string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_array(bcmcfg_tbl_user_t *user,
                             bcmcfg_read_level_info_t *info,
                             bcmcfg_tbl_info_t *tbl_info,
                             const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    switch (info->state) {
    case BCMCFG_READ_STATE_SEQ:
        /* In a sequence: sequence ID is array index, str is value. */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_simple_field_str(user,
                                                     info,
                                                     tbl_info,
                                                     (int)info->seq_idx,
                                                     str));
        break;
    case BCMCFG_READ_STATE_MAP:
        /* In a map: str is array index. */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_aidx(tbl_info, str));
        break;
    case BCMCFG_READ_STATE_MAP_V:
        /* In a map: str is array value. */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_set_simple_field_str(user,
                                                     info,
                                                     tbl_info,
                                                     tbl_info->aidx,
                                                     str));
        break;
    default:
        /* Weird. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add key range information.
 *
 * Add key range information.
 *
 * \param [in,out] info         Reader Data
 * \param [in,out] user         Global LT data.
 * \param [in,out] tbl_info     Local LT data.
 * \param [in]     str          Value string.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_add_key_range(bcmcfg_tbl_user_t *user,
                                 bcmcfg_read_level_info_t *info,
                                 bcmcfg_tbl_info_t *tbl_info,
                                 const char *value)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    uint64_t ivalue = 0;
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        rv = bcmcfg_str_to_uint64(value, &ivalue, 0);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Invalid key range value %s\n"), value));
            SHR_ERR_EXIT(rv);
        }

        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_is_field_range_valid(tbl_info, ivalue));

        if (bcmcfg_key_field_seq_mode == BCMCFG_KEY_FIELD_SEQ_MODE_RANGE_SPEC) {
            switch (bcmcfg_key_range_current.count) {
            case 0:
                bcmcfg_key_range_current.min = ivalue;
                bcmcfg_key_range_current.max = ivalue;
                bcmcfg_key_range_current.step = 1;
                break;
            case 1:
                bcmcfg_key_range_current.max = ivalue;
                if (bcmcfg_key_range_current.max < bcmcfg_key_range_current.min) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("%s:%d:%d Min value is greater than max value\n"),
                              info->locus, info->line, info->column));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                break;
            case 2:
                if (ivalue < 1) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("%s:%d:%d: Step size must be greater than 1\n"),
                              info->locus, info->line, info->column));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                if (ivalue > (bcmcfg_key_range_current.max - bcmcfg_key_range_current.min)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("%s:%d:%d Step size is too large\n"),
                              info->locus, info->line, info->column));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                bcmcfg_key_range_current.step = ivalue;
                break;
            default:
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("%s:%d:%d Too many elements in key range specification\n"),
                          info->locus, info->line, info->column));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            bcmcfg_key_range_current.count++;
        } else {
            /* Single element mode. */
            SHR_IF_ERR_EXIT
                (bcmcfg_key_range_spec_add(tbl_info->fid, ivalue, ivalue, 1));
        }
        kr_list->lt = user->tail->lt;
        kr_list->do_copy = true;
    } while (0);

exit:
    SHR_FUNC_EXIT();
}

static bool
bcmcfg_is_target_unit(bcmcfg_tbl_info_t *tbl_info,
                      int unit)
{
    bool is_target = false;

    if (bcmdrd_dev_exists(unit) &&
        tbl_info->unit_set[unit]) {
        is_target = true;
    }

    return is_target;
}

static bool
bcmcfg_is_early_init(bcmcfg_parse_mode_t mode)
{
    return (mode == PARSE_MODE_UNIT_EARLY);
}

static bool
bcmcfg_is_lt_dev_config(const char *value)
{
    bool target = false;

    if (!sal_strcmp(value, "DEVICE_CONFIG")) {
        target = true;
    }

    return target;
}

/*
 * \brief Skip logical table parsing.
 *
 * In early init stage, skip all non-DEVICE_CONFIG LTs parsing.
 * In normal init stage, skip DEVICE_CONFIG LT parsing.
 */
static bool
bcmcfg_is_lt_skip(bcmcfg_parse_mode_t mode, const char *name)
{
    bool skip = false;

    if ( ( bcmcfg_is_early_init(mode) && !bcmcfg_is_lt_dev_config(name)) ||
         (!bcmcfg_is_early_init(mode) &&  bcmcfg_is_lt_dev_config(name)) ) {
        skip = true;
    }

    return skip;
}

/*!
 * \brief device scalar handler.
 *
 * Scalar events for the device schema occur for map keys, map values,
 * and sequences. Any other context is an error.
 *
 * Map keys:    Search for the key.
 * Map values:  Set the value.
 * Sequences:   Set the value.
 *
 * \param [in]     value        Scalar value.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_scalar(const char *value,
                          bcmcfg_read_context_t *context,
                          void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_tbl_info_t *tbl_info;
    int unit = pinfo.target_unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (bcmcfg_is_read_skip(context)) {
        SHR_EXIT();
    }

    user = user + unit;
    tbl_info = user->info + context->level;
    switch (tbl_info->state) {
    case UNIT:
        bcmcfg_read_device_tbl_info_clear(tbl_info);
        /* Add unit */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_unit_str(tbl_info, value));
        break;
    case TABLE:
        bcmcfg_read_skip_clear(context);
        if (bcmcfg_is_lt_skip(pinfo.mode, value)) {
            bcmcfg_read_skip_set(context);
            SHR_EXIT();
        }

        /* Table value */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_search(tbl_info, value));
        if (bcmcfg_is_target_unit(tbl_info, pinfo.target_unit)) {
            /* Start of table context, so stop appending fields. */
            user->append = false;
            user->level = context->level;
            /* Initialize the key range spec. */
            bcmcfg_key_range_spec_init();
        } else {
            bcmcfg_read_device_tbl_info_clear(tbl_info);
            bcmcfg_read_skip_set(context);
        }
        break;
    case FIELD:
        /* Field value */
        if (!user->append) {
            SHR_IF_ERR_EXIT
                (bcmcfg_read_device_table_setup(info,
                                                context->level,
                                                user,
                                                tbl_info));
        }
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_handle_field(user,
                                             info,
                                             tbl_info,
                                             info->state,
                                             value));
        break;
    case ARRAY:
        /* Array value */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_array(user,
                                          info,
                                          tbl_info,
                                          value));
        break;
    case KEY_RANGE:
        /* Key range */
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_add_key_range(user,
                                              info,
                                              tbl_info,
                                              value));
        break;
    default:
        /* Some error */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy LT data from previous level.
 *
 * If 'start' is true, copy previous level node data to the current
 * level.
 *
 * \param [in]     start        True if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in,out] user         Global LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_copy(bool start,
                        bcmcfg_read_context_t *context,
                        bcmcfg_tbl_user_t *user)
{
    bcmcfg_tbl_info_t *cur_tbl_info;
    bcmcfg_tbl_info_t *prev_tbl_info;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (start) {
        /* copy previous user data */
        cur_tbl_info = user->info + context->level;
        prev_tbl_info = user->info + context->level - 1;
        sal_memcpy(cur_tbl_info, prev_tbl_info, sizeof(bcmcfg_tbl_info_t));
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Manage table state transition.
 *
 * Table parsing states, except FIELD->ARRAY, change on a container
 * push.
 *
 * \param [in]     info         Reader data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static void
next_container_state(const bcmcfg_read_level_info_t *info,
                     bcmcfg_tbl_info_t *tbl_info)
{
    switch (tbl_info->state) {
    case UNIT:
        if (info->state != BCMCFG_READ_STATE_SEQ) {
            tbl_info->state = TABLE;
        }
        break;
    case TABLE:
        if (tbl_info->sid_valid) {
            tbl_info->state = FIELD;
        }
        break;
    default:
        /* Nothing to do. */
        break;
    }
}

/*!
 * \brief Table sequence handler.
 *
 * \param [in]     start        True if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_seq(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_level_info_t *prev = context->info + context->level - 1;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_tbl_info_t *tbl_info;
    int unit = pinfo.target_unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        if (bcmcfg_is_read_skip(context)) {
            break;
        }

        user = user + unit;
        tbl_info = user->info + context->level;
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_copy(start, context, user));

        if (start) {
            switch (prev->state) {
            case BCMCFG_READ_STATE_MAP:
                /* Initialize the key range data. */
                bcmcfg_key_range_spec_init();
                break;
            case BCMCFG_READ_STATE_MAP_V:
                if (tbl_info->state == FIELD) {
                    /* Sequences assigned to a field. */
                    bool array;
                    bool key;
                    SHR_IF_ERR_EXIT
                        (bcmcfg_read_device_is_field_array(tbl_info, &array));
                    if (array) {
                        tbl_info->state = ARRAY;
                    } else {
                        int rv;
                        rv = bcmcfg_read_device_is_key_field(tbl_info, &key);
                        if (key) {
                            tbl_info->state = KEY_RANGE;
                        } else {
                            SHR_IF_ERR_EXIT(rv);
                        }
                    }
                }
                break;
            case BCMCFG_READ_STATE_SEQ:
                if (tbl_info->state == UNIT) {
                    bcmcfg_read_device_add_unit(tbl_info, (int)info->seq_idx);
                } else if (tbl_info->state == FIELD) {
                    tbl_info->fid = info->seq_idx;
                } else if (tbl_info->state == KEY_RANGE) {
                    bcmcfg_key_field_seq_mode = BCMCFG_KEY_FIELD_SEQ_MODE_RANGE_SPEC;
                }
                break;
            default:
                break;
            }
            next_container_state(info, tbl_info);
        } else {
            switch (prev->state) {
            case BCMCFG_READ_STATE_SEQ:
                /* insert key range field entries when exiting seq. */
                SHR_IF_ERR_EXIT
                    (bcmcfg_key_range_spec_add(tbl_info->fid,
                                               bcmcfg_key_range_current.min,
                                               bcmcfg_key_range_current.max,
                                               bcmcfg_key_range_current.step));
                bcmcfg_key_range_status_clear();
                break;
            default:
                break;
            }
        }
    } while (0);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy table entry.
 *
 * Create new table list entry and copy all fields from given base table list.
 *
 * \param [in,out] list         Table list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_entry_copy(bcmcfg_tbl_list_t **list)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    bcmcfg_tbl_list_t *tbl = NULL;
    shr_fmm_t *field = kr_list->lt->field;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_ALLOC(tbl, sizeof(*tbl), "bcmcfgReadDeviceEntryPrep");
    SHR_NULL_CHECK(tbl, SHR_E_MEMORY);
    sal_memcpy(tbl, kr_list->lt, sizeof(*tbl));
    tbl->field = NULL;
    /* Copy fields from base table. */
    while (field) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_link_field(tbl,
                                           field->id,
                                           field->idx,
                                           field->data));
        field = field->next;
    }
    tbl->next = *list;
    *list = tbl;

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(tbl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get next table list pointer.
 *
 * Get next table list pointer.
 *
 * \param [in,out] tbl          Table list.
 * \param [in]     num          Number to proceed next.
 *
 * \retval tbl    Pointer to the table list.
 */
static bcmcfg_tbl_list_t *
bcmcfg_tbl_list_next_get(bcmcfg_tbl_list_t *tbl,
                         uint32_t num)
{
    uint32_t i;

    for (i = 0; i < num; i++) {
        if (tbl) {
            tbl = tbl->next;
        } else {
            tbl = NULL;
        }
    }

    return tbl;
}

/*!
 * \brief Add key field to the table entries.
 *
 * Add key field to the table entries.
 *
 * \param [in,out] list         Table list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_entry_key_field_add(bcmcfg_tbl_list_t **list)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    bcmcfg_key_range_spec_t *krs = kr_list->krs;
    shr_fmm_t key_field;
    bcmcfg_key_range_t *range = NULL;
    uint32_t cur_elem;
    uint32_t value;
    uint32_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while (krs) {
        sal_memset(&key_field, 0, sizeof(shr_fmm_t));
        key_field.id = krs->fid;
        key_field.idx = 0;

        cur_elem = 0;
        range = krs->ranges;
        while (range) {
            for (value = range->min; value <= range->max; value += range->step) {
                bcmcfg_tbl_list_t *tbl = *list;
                uint32_t offset = cur_elem * krs->repeat;
                tbl = bcmcfg_tbl_list_next_get(tbl, offset);

                /* Insert value into each entry */
                key_field.data = value;
                while (offset < kr_list->num_entries) {
                    for (i = 0; i < krs->repeat; i++) {
                        SHR_IF_ERR_EXIT(
                            bcmcfg_read_device_link_field(tbl,
                                                          key_field.id,
                                                          key_field.idx,
                                                          key_field.data));
                        tbl = bcmcfg_tbl_list_next_get(tbl, 1);
                        offset += 1;
                    }

                    tbl = bcmcfg_tbl_list_next_get(tbl, krs->gap);

                    offset += krs->gap;
                }
                cur_elem++;
            }
            range = range->next;
        }
        krs = krs->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create entries by iterating key range specifications.
 *
 * Create entries by iterating key range specifications.
 * - Duplicate table entries from base table entry.
 * - Calculate gap, repeat for each key field.
 * - Insert key fields to the base entries.
 *
 * Example,
 *       KEY1 : [[0, 2]]
 *       KEY2 : [[0, 1]]
 *       KEY3 : [[0, 2]]
 *
 *       | Gap | Rep | 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17 |
 *  KEY1 |  2  |  1  | 0,  1,  2,  0,  1,  2,  0,  1,  2,  0,  1,  2,  0,  1,  2,  0,  1,  2 |
 *  KEY2 |  3  |  3  | 0,  0,  0,  1,  1,  1,  0,  0,  0,  1,  1,  1,  0,  0,  0,  1,  1,  1 |
 *  KEY3 |  6  |  12 | 0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2 |
 *
 * - Duplicate 3 * 2 * 3 = 18 entries.
 * - Calculate gap, repeat information for all 3 key ranges.
 *   Gap    : number of entries between the same key values.
 *   Repeat : number of entries same key values are repeated.
 * - Insert key field to table entry list.
 *
 * \param [in,out] list         Table list.
 * \param [in,out] info         Reader Data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_entry_create(bcmcfg_tbl_list_t **list,
                                bcmcfg_read_level_info_t *info,
                                bcmcfg_tbl_info_t *tbl_info)
{
    bcmcfg_key_range_spec_list_t *kr_list = &bcmcfg_key_range_list_user;
    uint32_t i;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Total number of elements. */
    bcmcfg_key_range_spec_entries();

    /* Key range spec validation. */
    SHR_IF_ERR_EXIT
        (bcmcfg_key_range_spec_validate(info, tbl_info));

    /* Copy table entries for all key combinations.  */
    for (i = 0; i < (kr_list->num_entries -1); i++) {
        SHR_IF_ERR_EXIT
            (bcmcfg_read_device_entry_copy(list));
    }

    /* Calculate repeat, gap for each key field. */
    bcmcfg_key_range_spec_info_calc();

    /* Inserting key field for each table entries. */
    SHR_IF_ERR_EXIT
        (bcmcfg_read_device_entry_key_field_add(list));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Duplicate the lt entry with new key value.
 *
 * \param [in,out] user         Global LT data.
 * \param [in,out] info         Reader Data.
 * \param [in,out] tbl_info     Local LT data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_entry_duplicate(bcmcfg_tbl_user_t *user,
                                   bcmcfg_read_level_info_t *info,
                                   bcmcfg_tbl_info_t *tbl_info)
{
    bcmcfg_tbl_list_t **list = &user->tail->lt;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT(
        bcmcfg_read_device_entry_create(list, info, tbl_info));

    bcmcfg_key_range_spec_init();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML map event.
 *
 * When starting a map, set the sequence container and start_idx, and
 * calculate storage offset if within a sequence.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_map(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_read_level_info_t *prev = context->info + context->level - 1;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_tbl_info_t *tbl_info;
    int unit = pinfo.target_unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (bcmcfg_is_read_skip(context)) {
        SHR_EXIT();
    }

    user = user + unit;
    tbl_info = user->info + context->level;
    SHR_IF_ERR_EXIT
        (bcmcfg_read_device_copy(start, context, user));
    if (start) {

        /* Context.map */
        switch (prev->state) {
        case BCMCFG_READ_STATE_SEQ:
        case BCMCFG_READ_STATE_MAP:
            if (info->state == BCMCFG_READ_STATE_MAP &&
                user->level == context->level) {
                user->append = false;
            }
            break;
        case BCMCFG_READ_STATE_MAP_V:
            switch (tbl_info->state) {
            case TABLE:
                break;
            case FIELD:
                if (tbl_info->ifield) {
                    SHR_IF_ERR_EXIT
                        (bcmcfg_read_device_implicit_field_add(user,
                                                               info,
                                                               tbl_info));
                    tbl_info->ifield = false;
                } else {
                    bool array;
                    int rv;
                    rv = bcmcfg_read_device_is_field_array(tbl_info, &array);
                    if (array) {
                        tbl_info->state = ARRAY;
                    } else {
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        next_container_state(info, tbl_info);
    } else {
        if (prev->state == BCMCFG_READ_STATE_MAP_V) {
            if (bcmcfg_key_range_list_user.do_copy) {
                SHR_IF_ERR_EXIT
                    (bcmcfg_read_device_entry_duplicate(user, info, tbl_info));
            }
        }

        bcmcfg_read_skip_clear(context);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle YAML doc event.
 *
 * Clear the reader stack and prepare a playback list if there is not
 * one already.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in,out] context      Reader context.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_doc(bool start,
                       bcmcfg_read_context_t *context,
                       void *user_data)
{
    bcmcfg_read_level_info_t *info = context->info + context->level;
    bcmcfg_tbl_user_t *user = (bcmcfg_tbl_user_t *)user_data;
    bcmcfg_playback_list_t *list;
    int unit = pinfo.target_unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    COMPILER_REFERENCE(user);

    if (start) {
        user = user + unit;
        /* Clear info stack. */
        sal_memset(bcmcfg_tbl_user[unit].info, 0, sizeof(bcmcfg_tbl_user[unit].info));
        /* Clear append. */
        bcmcfg_tbl_user[unit].append = false;
        bcmcfg_tbl_user[unit].level = context->level;

        /* Only add a playback segment if there is no playback segment
           allocated, or the allocated playback segment is in
           use. This will avoid a chain of empty playback segments. */
        if (user->head == NULL || user->tail->pt || user->tail->lt) {
            list = sal_alloc(sizeof(*list), "bcmcfgReadDeviceDoc");
            if (list) {
                sal_memset(list, 0, sizeof(*list));
                list->locus = sal_strdup(info->locus);
                if (list->locus) {
                    if (user->head == NULL) {
                        /* Populate head. */
                        user->head = list;
                    }
                    if (user->tail) {
                        /* Link to tail. */
                        user->tail->next = list;
                    }
                    /* Set tail. */
                    user->tail = list;
                } else {
                    /* Out of memory for locus. */
                    sal_free(list);
                    SHR_ERR_EXIT(SHR_E_MEMORY);
                }
            } else {
                /* Out of memory for list. */
                SHR_ERR_EXIT(SHR_E_MEMORY);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Handle reader abort notification.
 *
 * Free any pending playback lists on an abort notification.
 *
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_abort(bcmcfg_read_context_t *context,
                         void *user_data)
{
    int unit = pinfo.target_unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    COMPILER_REFERENCE(context);
    COMPILER_REFERENCE(user_data);
    SHR_IF_ERR_EXIT
        (bcmcfg_playback_free(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup device reader.
 *
 * Device reader setup/cleanup. Called once with start=true, and
 * once with start=false.
 *
 * \param [in]     start        true if start, false if stop.
 * \param [in]     info         Parse info.
 * \param [in]     user_data    Handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_read_device_setup(bool start,
                         bcmcfg_parse_info_t *info,
                         void *user_data)
{
    int unit;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (info) {
        sal_memcpy(&pinfo, info, sizeof(bcmcfg_parse_info_t));
    }
    unit = pinfo.target_unit;
    if (start) {
        sal_memset(&bcmcfg_tbl_user[unit], 0, sizeof(bcmcfg_tbl_user[unit]));
    } else {
        if (unit >= 0 && unit < BCMDRD_CONFIG_MAX_UNITS) {
            SHR_IF_ERR_EXIT
                (bcmcfg_playback_free(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free playback table.
 *
 * Free playback table element.
 *
 * \param [in,out] t_list       Head of table list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_playback_free_tbl(bcmcfg_tbl_list_t *t_list)
{
    bcmcfg_tbl_list_t *t_ptr;
    shr_fmm_t *f_list;
    shr_fmm_t *f_ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while (t_list) {
        /* Free fields in table. */
        f_list = t_list->field;
        while (f_list) {
            f_ptr = f_list;
            f_list = f_list->next;
            sal_free(f_ptr);
        }
        /* Free table. */
        t_ptr = t_list;
        t_list = t_list->next;
        sal_free(t_ptr);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Free playback enum.
 *
 * Free playback enum element.
 *
 * \param [in,out] list         Head of enum list.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_playback_free_enum(bcmcfg_playback_enum_t *list)
{
    bcmcfg_playback_enum_t *ptr;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    while (list) {
        sal_free((void *)list->value);
        ptr = list;
        list = list->next;
        sal_free(ptr);
    }

    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * Get non-config table playback list.
 */
int
bcmcfg_playback_get(int unit, const bcmcfg_playback_list_t **list)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {
        if (!list) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (!bcmcfg_tbl_user[unit].head) {
            SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            /* No break here because static analysis says it is unreachable. */
        }

        if (!bcmcfg_tbl_user[unit].head->lt && !bcmcfg_tbl_user[unit].head->pt) {
            SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            /* No break here because static analysis says it is unreachable. */
        }

        *list = bcmcfg_tbl_user[unit].head;

    } while (0);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Free playback list.
 */
int
bcmcfg_playback_free(int unit)
{
    bcmcfg_playback_list_t *ptr;
    bcmcfg_playback_list_t *list;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    list = bcmcfg_tbl_user[unit].head;
    while (list) {
        bcmcfg_playback_free_tbl(list->lt);
        bcmcfg_playback_free_tbl(list->pt);
        bcmcfg_playback_free_enum(list->enum_head);
        if (list->locus) {
            sal_free((void *)list->locus);
        }
        ptr = list;
        list = list->next;
        sal_free(ptr);
    }

    bcmcfg_tbl_user[unit].head = bcmcfg_tbl_user[unit].tail = NULL;

    SHR_FUNC_EXIT();
}

/*
 * Device reader handler.
 */
const bcmcfg_read_handler_t bcmcfg_read_device = {
    .key       = "device",
    .setup     = bcmcfg_read_device_setup,
    .scalar    = bcmcfg_read_device_scalar,
    .map       = bcmcfg_read_device_map,
    .seq       = bcmcfg_read_device_seq,
    .doc       = bcmcfg_read_device_doc,
    .abort     = bcmcfg_read_device_abort,
    .user_data = &bcmcfg_tbl_user
};
