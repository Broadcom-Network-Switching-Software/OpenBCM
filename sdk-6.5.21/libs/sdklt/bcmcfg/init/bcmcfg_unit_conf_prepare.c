/*! \file bcmcfg_unit_conf_prepare.c
 *
 * Prepare an LRD per-unit mapping configuration for use.
 *
 * Table maps and fields may be made conditional based on BCMCFG
 * config tables.
 *
 * If the map has field conditions only, a copy of the map data
 * structure is made, and appropriate edits are made the the field
 * flags (by setting the UNMAPPED field flag), and any associated
 * mapping entries that correspond to that field (by setting the entry
 * type to IGNORE). Such field editing is not applied to a table with
 * table conditions and the table condition is not met.
 *
 * If the map has table conditions, then the map is set to NULL if the
 * table condition is not met.
 *
 * Because the map data structure has a number of pointers to other
 * structures, the data needed the map edits are allocated from the
 * heap, and freed when during LRD cleanup. Because many of the data
 * members are const, some care is taken to avoid "casting away
 * const-ness" by writing to intermediate data structures before
 * assembling the final map data structure.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <shr/shr_error.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/*!
 * \brief Memory tracker.
 *
 * Allocated memory header. Allocated data structure immediately
 * follows pointer.
 */
typedef struct bcmcfg_ucp_mem_s {
    /*! Pointer to next allocated memory chunk. */
    struct bcmcfg_ucp_mem_s *next;
} bcmcfg_ucp_mem_t;

/*!
 * \brief Writable map data.
 *
 * Pointers to most LRD map areas that will be written by map editing.
 * Map entries are allocated separately.
 */
typedef struct bcmcfg_ucp_map_data_s {
    /*! Outer map data structure. */
    bcmlrd_map_t *map;

    /*! Map group map data structure. */
    bcmlrd_map_group_t *group;

    /*! Map field_data structure. */
    bcmlrd_map_field_data_t *field_data;

    /*! Map field structure. */
    bcmlrd_field_data_t *field;
} bcmcfg_ucp_map_data_t;

/*!
 * \brief Tracked allocated memory.
 *
 * All allocated memory is tracked so it can be freed on cleanup.
 */
static bcmcfg_ucp_mem_t *bcmcfg_ucp_mem;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Tracking allocator.
 *
 * Allocate memory and add to tracker.
 *
 * \param [in]    size          Size to allocate.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static void *
bcmcfg_ucp_alloc(size_t size)
{
    size_t asize = size + sizeof(bcmcfg_ucp_mem_t);
    bcmcfg_ucp_mem_t *mem = sal_alloc(asize, "bcmcfgUcpAlloc");

    if (mem) {
        if (!bcmcfg_ucp_mem) {
            mem->next = NULL;
        } else {
            mem->next = bcmcfg_ucp_mem;
        }
        bcmcfg_ucp_mem = mem;
    }

    return mem ? (void *)(mem+1) : NULL;
}

/*!
 * \brief Tracking memory copier.
 *
 * Make a copy of 'in' using UCP allocator.
 *
 * \param [in]    in            Data to copy allocate.
 * \param [in]    size          Size to allocate.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static void *
bcmcfg_ucp_copy(const void *in, size_t size)
{
    void *p;

    p = bcmcfg_ucp_alloc(size);
    if (p) {
        sal_memcpy(p, in, size);
    }

    return p;
}

/*!
 * \brief Writable map conf.
 *
 * Make a writable copy of a map conf.
 *
 * \param [in]    in            Map configuration.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_map_conf_rep_t *
bcmcfg_ucp_copy_conf_rep(const bcmlrd_map_conf_rep_t *in)
{
    return bcmcfg_ucp_copy(in, sizeof(bcmlrd_map_conf_rep_t));
}

/*!
 * \brief Writable map field data.
 *
 * Make a writable copy of a map field data structure.
 *
 * \param [in]    in            Field data.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_map_field_data_t *
bcmcfg_ucp_copy_map_field_data(const bcmlrd_map_field_data_t *in)
{
    bcmlrd_map_field_data_t *out;

    out =  bcmcfg_ucp_copy(in, sizeof(bcmlrd_map_field_data_t));

    return out;
}

/*!
 * \brief Writable field data.
 *
 * Make a writable copy of a field array.
 *
 * \param [in]    in            Field array.
 * \param [in]    fields        Number of fields.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_field_data_t *
bcmcfg_ucp_copy_field_data(const bcmlrd_field_data_t *in, uint32_t fields)
{
    bcmlrd_field_data_t *out;

    out = bcmcfg_ucp_copy(in, sizeof(const bcmlrd_field_data_t) * fields);

    return out;
}

/*!
 * \brief Writable entry data.
 *
 * Make a writable copy of an entry array.
 *
 * \param [in]    in            Entry array.
 * \param [in]    entries       Number of entries.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_map_entry_t *
bcmcfg_ucp_copy_entry(const bcmlrd_map_entry_t *in, uint32_t entries)
{
    bcmlrd_map_entry_t *out;
    out = bcmcfg_ucp_copy(in,
                          sizeof(bcmlrd_map_entry_t) * entries);

    return out;
}

/*!
 * \brief Writable group data.
 *
 * Make a writable copy of a map group array.
 *
 * \param [in]    in            Group array.
 * \param [in]    groups        Number of groups.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_map_group_t *
bcmcfg_ucp_copy_group(const bcmlrd_map_group_t *in, uint32_t groups)
{
    bcmlrd_map_group_t *out;
    out = bcmcfg_ucp_copy(in, sizeof(bcmlrd_map_group_t) * groups);
    return out;
}

/*!
 * \brief Writable map.
 *
 * Make a writable copy of a map.
 *
 * \param [in]    in            Map.
 *
 * \retval !NULL  OK
 * \retval NULL   ERROR
 */
static bcmlrd_map_t *
bcmcfg_ucp_copy_map(const bcmlrd_map_t *in)
{
    bcmlrd_map_t *out = NULL;

    out = bcmcfg_ucp_copy(in, sizeof(bcmlrd_map_t));

    return out;
}

/*!
 * \brief Evaluate conditional expression.
 *
 * Evaluate a conditional expression for a given unit. Conditional
 * expressions read BCMCFG fields and a conditional operation. The
 * available operations are AND and OR. The result is true if all of
 * the ANDed expressions are true and at least one OR expression is
 * true.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  expr            Conditional expression.
 * \param [out] result          Evaluation result.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_eval_cond_expr(int unit,
                          const bcmlrd_cond_expr_t *expr,
                          bool *result)
{
    bool expr_and = false;
    bool expr_or = false;
    bool have_and = false;
    bool have_or = false;
    uint64_t config_value;
    size_t i;
    int rv = SHR_E_INTERNAL;

    for (i = 0; i < expr->num_cond; i++) {
        const bcmlrd_condition_t *cond = expr->cond + i;
        bool cond_result;
        uint32_t count;

        count = bcmlrd_field_idx_count_get(unit, cond->table, cond->field);
        if (count > 1) {
            continue;
        }

        rv = bcmcfg_field_get(unit, cond->table, cond->field, &config_value);
        if (SHR_FAILURE(rv)) {
            break;
        }
        cond_result = (cond->value == config_value);
        switch (cond->op) {
        case BCMLRD_CONDITION_AND:
            if (!have_and) {
                expr_and = cond_result;
                have_and = !have_and;
            } else {
                expr_and &= cond_result;
            }
            break;
        case BCMLRD_CONDITION_OR:
            if (!have_or) {
                expr_or = cond_result;
                have_or = !have_or;
            } else {
                expr_or |= cond_result;
            }
            break;

        default:
            /* Should be warning. */
            break;
        }
    }

    *result = (expr_and || expr_or);

    return rv;
}

/*!
 * \brief Edit a map entry.
 *
 * Edit the map entry according to the given edit list.
 *
 * The edit list is a sequence of implied 'instructions' that results
 * in nullifying entries associated with a conditional field. An edit
 * list is associated with a single field, and indicates with entries
 * within a group need to be nullified (i.e. unavailable to LTM for
 * use). Nullification is implemented by setting the entry type to
 * BCMLRD_MAP_ENTRY_IGNORE. This is easier than compacting entries,
 * and preserves the basic structure of the map as no counts need to
 * me modified.
 *
 * The edit list is structured as follows:
 *
 * #of groups
 * group#
 *   #of entries
 *   entry#
 *   entry#
 *   ...
 * group#
 *   ...
 * #of multi-field-transforms
 *   #of fields
 *     field_id
 *     field_id
 *     ...
 *   #of fields
 *   ...
 *
 * \param [in]  map_data        Writable map data.
 * \param [in]  edit_op         Edit operation list.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_edit_entry(bcmcfg_ucp_map_data_t *map_data, const uint32_t *edit_op)
{
    int rv = SHR_E_NONE;
    uint32_t num_group;
    uint32_t group_idx;
    uint32_t num_entries;
    uint32_t entry_idx;
    bcmlrd_map_t *map = map_data->map;
    bcmlrd_map_group_t *group = map_data->group;
    bcmlrd_map_entry_t *entry;

    num_group = *edit_op++;
    while (num_group != 0) {
        group_idx = *edit_op++;
        num_entries = *edit_op++;
        entry = bcmcfg_ucp_copy_entry(map->group[group_idx].entry,
                                      map->group[group_idx].entries);
        if (entry == NULL) {
            rv = SHR_E_MEMORY;
            break;
        }

        while (num_entries != 0) {
            entry_idx = *edit_op++;
            entry[entry_idx].entry_type = BCMLRD_MAP_ENTRY_IGNORE;
            num_entries--;
        }
        group[group_idx].entry = entry;
        num_group--;
    }

    return rv;
}

/*!
 * \brief Evaluate a table expression.
 *
 * Evaluate a table expression and remove a map from a mapping configuration
 * if the expression evaluates to false.
 *
 * \param [in]    unit          Unit number.
 * \param [inout] map           Pointer to pointer to map.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_apply_table_expr(int unit, bcmlrd_map_t **map)
{
    int rv;
    bool v;

    rv = bcmcfg_ucp_eval_cond_expr(unit, (*map)->cond->table_expr, &v);

    if (SHR_SUCCESS(rv) && v == false) {
        /* Remove map if condition is not met. */
        *map = NULL;
    }

    return rv;
}

/*!
 * \brief Copy map data.
 *
 * Make a writable copy of map data.
 *
 * \param [in]   map            Map to copy.
 * \param [out]  map_data       Writable map data.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_copy_map_data(const bcmlrd_map_t *map,
                         bcmcfg_ucp_map_data_t *map_data)
{
    int rv = SHR_E_INTERNAL;

    do {
        map_data->map = bcmcfg_ucp_copy_map(map);
        if (map_data->map == NULL) {
            rv = SHR_E_MEMORY;
            break;
        }

        /* Allocate group and field structures. */
        map_data->group = bcmcfg_ucp_copy_group(map->group, map->groups);
        if (map_data->group == NULL) {
            rv = SHR_E_MEMORY;
            break;
        }

        map_data->field_data = bcmcfg_ucp_copy_map_field_data(map->field_data);
        if (map_data->field_data == NULL) {
            rv = SHR_E_MEMORY;
            break;
        }

        map_data->field = bcmcfg_ucp_copy_field_data(map->field_data->field,
                                                     map->field_data->fields);
        if (map_data->field == NULL) {
            rv = SHR_E_MEMORY;
            break;
        }

        map_data->field_data->field = map_data->field;
        map_data->map->field_data = map_data->field_data;
        map_data->map->group = map_data->group;

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}

/*!
 * \brief Apply field expressions.
 *
 * Apply all field expressions in a map. If the field expression
 * evaluates to false, the field is unmapped, and any associated
 * map entries are nullified.
 *
 * \param [in]    unit          Unit number.
 * \param [inout] map           Pointer to pointer to map.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_apply_field_exprs(int unit, bcmlrd_map_t **map)
{
    size_t i;
    bcmcfg_ucp_map_data_t map_data;
    bool v;
    int rv = SHR_E_INTERNAL;

    sal_memset(&map_data, 0, sizeof(map_data));

    for (i = 0; i < (*map)->cond->num_field_cond; i++ ) {
        const bcmlrd_cond_field_t *cond = (*map)->cond->field_cond + i;
        rv = bcmcfg_ucp_eval_cond_expr(unit, cond->expr, &v);
        if (SHR_FAILURE(rv)) {
            break;
        }

        if (v == true) {
            /* No edits needed. Try next conditional, if any. */
            continue;
        }

        if (cond->edit == NULL) {
            /* Map needs edit, but there are no editing instructions. */
            rv = SHR_E_INTERNAL;
            break;
        }

        if (!map_data.map) {
            rv = bcmcfg_ucp_copy_map_data(*map, &map_data);
            if (SHR_FAILURE(rv)) {
                break;
            }
        }

        /* Mark the field as unmapped. */
        map_data.field[cond->field].flags |= BCMLRD_FIELD_F_UNMAPPED;

        /* Edit the map. */
        rv = bcmcfg_ucp_edit_entry(&map_data, cond->edit);

        if (SHR_FAILURE(rv)) {
            break;
        }

    }

    /* Use edited map if there is one. */
    if (SHR_SUCCESS(rv) && map_data.map) {
        *map = map_data.map;
    }

    return rv;
}

/*!
 * \brief Apply map expressions.
 *
 * Apply all table and field expressions. Field expressions are only
 * applied if there is no table expression, or the table expression
 * evaluates to false.
 *
 * \param [in]    unit          Unit number.
 * \param [inout] map           Pointer to pointer to map.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_apply_map(int unit, bcmlrd_map_t **map)
{
    int rv = SHR_E_NONE;

    if ((*map)->cond->table_expr) {
        rv = bcmcfg_ucp_apply_table_expr(unit, map);
    }

    if (*map && (*map)->cond->num_field_cond) {
        rv = bcmcfg_ucp_apply_field_exprs(unit, map);
    }

    return rv;
}

/*!
 * \brief Apply map expressions.
 *
 * Apply all table and field expressions in any maps against the given
 * map configuration.
 *
 * \param [in]    unit          Unit number.
 * \param [inout] rep           Map configuration.
 *
 * \retval SHR_E_NONE    OK
 * \retval !SHR_E_NONE   ERROR
 */
static int
bcmcfg_ucp_apply(int unit, bcmlrd_map_conf_rep_t *rep)
{
    int rv = SHR_E_NONE;
    size_t i;

    for ( i = 0; i < COUNTOF(rep->map); i++) {
        if (rep->map[i] == NULL) {
            /* No map, so nothing to do. */
            continue;
        }

        if (rep->map[i]->cond == NULL) {
            /* No conditions, so nothing to do. */
            continue;
        }

        rv = bcmcfg_ucp_apply_map(unit, (bcmlrd_map_t **)&rep->map[i]);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }

    return rv;
}

static void
bcmcfg_ucp_cleanup(void)
{
    bcmcfg_ucp_mem_t *p = bcmcfg_ucp_mem;
    bcmcfg_ucp_mem_t *q;

    /* Free memory used for map editing. */
    while (p) {
        q = p->next;
        sal_free(p);
        p = q;
    }
    bcmcfg_ucp_mem = NULL;
}

static int
bcmcfg_ucp_edit(int unit,
                const bcmlrd_map_conf_rep_t *in,
                const bcmlrd_map_conf_rep_t **out)
{
    int rv = SHR_E_MEMORY;
    bcmlrd_map_conf_rep_t *p;

    p = bcmcfg_ucp_copy_conf_rep(in);
    if (p) {
        rv = bcmcfg_ucp_apply(unit, p);
        if (SHR_SUCCESS(rv)) {
            *out = p;
        }
    }

    return rv;
}

/*******************************************************************************
 * Public functions
 */

void
bcmcfg_unit_conf_cleanup(void)
{
    bcmcfg_ucp_cleanup();
}

int
bcmcfg_unit_conf_edit(int unit,
                      bool warm)
{
    int rv = SHR_E_NONE;
    const bcmlrd_map_conf_rep_t *map_rep_in;
    const bcmlrd_map_conf_rep_t *map_rep_out = NULL;

    do {
        rv = bcmlrd_conf_get(unit, &map_rep_in);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmcfg_ucp_edit(unit,
                             map_rep_in,
                             &map_rep_out);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmlrd_conf_set(unit, map_rep_out);

    } while(0);

    return rv;
}
