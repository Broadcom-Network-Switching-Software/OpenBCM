/*! \file bcmcfg_table_set_cached_config.c
 *
 * Create cached table configuration until bcmcfg is ready
 * to apply them.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmltd/str/bcmltd_common_str.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_DEV

/*! Cached config by ID. */
typedef struct bcmcfg_cached_config_id_s {
    /*! Logical table ID. */
    bcmlrd_sid_t sid;
    /*! Logical field ID. */
    bcmlrd_fid_t fid;
    /*! Logical field index. */
    uint32_t fidx;
    /*! Logical field value. */
    uint64_t     value;
} bcmcfg_cached_config_id_t;

/*! Cached config by string. */
typedef struct bcmcfg_cached_config_str_s {
    /*! Logical table name. */
    const char *tname;
    /*! Logical field name. */
    const char *fname;
    /*! Logical field index. */
    uint32_t fidx;
    /*! Logical field value name. */
    const char *vname;
    /*! Logical field value. */
    uint64_t    value;
} bcmcfg_cached_config_str_t;

/*! Config cache data. */
typedef struct bcmcfg_cached_config_s {
    /*! Config data by ID. */
    bcmcfg_cached_config_id_t  *cid;
    /*! Config data by string. */
    bcmcfg_cached_config_str_t *cstr;
    /*! Next pointer. */
    struct bcmcfg_cached_config_s *next;
} bcmcfg_cached_config_t;

/*! Head of cached_config linked list. */
static bcmcfg_cached_config_t *list_head[BCMDRD_CONFIG_MAX_UNITS] = { NULL };
/*! Tail of cached_config linked list. */
static bcmcfg_cached_config_t *list_tail[BCMDRD_CONFIG_MAX_UNITS] = { NULL };

/*******************************************************************************
 * Private functions
 */

static void
bcmcfg_cached_config_data_add(int unit, bcmcfg_cached_config_t *list)
{
    if (list_head[unit] == NULL) {
        list_head[unit] = list;
    }
    if (list_tail[unit]) {
        list_tail[unit]->next = list;
    }
    list_tail[unit] = list;

    return;
}

static void
bcmcfg_table_cached_config_cleanup(int unit)
{
    bcmcfg_cached_config_t *list = list_head[unit];
    bcmcfg_cached_config_t *next = NULL;

    while (list) {
        next = list->next;

        SHR_FREE(list->cid);
        if (list->cstr) {
            SHR_FREE(list->cstr->tname);
            SHR_FREE(list->cstr->fname);
            SHR_FREE(list->cstr->vname);
        }
        SHR_FREE(list->cstr);
        SHR_FREE(list);

        list = next;
    }

    list_head[unit] = NULL;
    list_tail[unit] = NULL;
    return;
}

static int
bcmcfg_cached_lt_skip_cid(bcmcfg_init_stage_t stage, bcmlrd_sid_t sid)
{
    bool skip = false;

    if ((stage == INIT_STAGE_EARLY && sid != DEVICE_CONFIGt) ||
        (stage == INIT_STAGE_NORMAL && sid == DEVICE_CONFIGt) ) {
        skip = true;
    }

    return skip;
}

static int
bcmcfg_cached_lt_skip_cstr(bcmcfg_init_stage_t stage, const char *name)
{
    bool skip = false;
    bool is_device_config = !sal_strcmp(name, DEVICE_CONFIGs);

    if ((stage == INIT_STAGE_EARLY && !is_device_config) ||
        (stage == INIT_STAGE_NORMAL && is_device_config) ) {
        skip = true;
    }

    return skip;
}

/*******************************************************************************
 * Public functions
 */

int
bcmcfg_table_set_cached_config_value_by_id(int unit,
                                           bcmltd_sid_t sid,
                                           bcmltd_sid_t fid,
                                           uint32_t fidx,
                                           uint64_t value)
{
    bcmcfg_cached_config_t *list = NULL;
    bcmcfg_cached_config_id_t *data = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_ALLOC(list, sizeof(*list), "bcmcfgTableSetRequestId");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    SHR_ALLOC(data, sizeof(*data), "bcmcfgTableCacheId");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, sizeof(*data));

    data->sid = sid;
    data->fid = fid;
    data->fidx = fidx;
    data->value = value;
    list->cid = data;

    bcmcfg_cached_config_data_add(unit, list);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(data);
        SHR_FREE(list);
    }
    SHR_FUNC_EXIT();
}


int
bcmcfg_table_set_cached_config_string_by_name(int unit,
                                              const char *table_name,
                                              const char *field_name,
                                              uint32_t field_index,
                                              const char *field_value)
{
    bcmcfg_cached_config_t *list = NULL;
    bcmcfg_cached_config_str_t *data = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_ALLOC(list, sizeof(*list), "bcmcfgTableSetRequestId");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    SHR_ALLOC(data, sizeof(*data), "bcmcfgTableCacheId");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, sizeof(*data));

    data->tname = sal_strdup(table_name);
    data->fname = sal_strdup(field_name);
    data->vname = sal_strdup(field_value);
    data->fidx = field_index;
    list->cstr = data;

    bcmcfg_cached_config_data_add(unit, list);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(data);
        SHR_FREE(list);
    }
    SHR_FUNC_EXIT();
}

int
bcmcfg_table_set_cached_config_value_by_name(int unit,
                                             const char *table_name,
                                             const char *field_name,
                                             uint32_t field_index,
                                             uint64_t field_value)
{
    bcmcfg_cached_config_t *list = NULL;
    bcmcfg_cached_config_str_t *data = NULL;

    SHR_FUNC_ENTER(unit);

    if (!bcmdrd_dev_exists(unit)) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_ALLOC(list, sizeof(*list), "bcmcfgTableSetRequestId");
    SHR_NULL_CHECK(list, SHR_E_MEMORY);
    sal_memset(list, 0, sizeof(*list));

    SHR_ALLOC(data, sizeof(*data), "bcmcfgTableCacheId");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, sizeof(*data));

    data->tname = sal_strdup(table_name);
    data->fname = sal_strdup(field_name);
    data->fidx = field_index;
    data->value = field_value;
    list->cstr = data;

    bcmcfg_cached_config_data_add(unit, list);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(data);
        SHR_FREE(list);
    }
    SHR_FUNC_EXIT();
}

int
bcmcfg_table_cached_config_apply(int unit, bool warm,
                                 bcmcfg_init_stage_t stage)
{
    bcmcfg_cached_config_t *list = list_head[unit];
    bcmcfg_cached_config_id_t *cid = NULL;
    bcmcfg_cached_config_str_t *cstr = NULL;
    bcmltd_sid_t sid;
    bcmltd_fid_t fid;
    uint32_t value;
    uint64_t value64;

    SHR_FUNC_ENTER(unit);

    if (warm && list) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META("Ignoring config set in warm boot\n")));
    }
    while (list) {
        cid = list->cid;
        cstr = list->cstr;

        if (cid) {
            if (warm) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META("table %u, field %u, value %" PRIu64 "\n"),
                         cid->sid, cid->fid, cid->value));
            } else if (!bcmcfg_cached_lt_skip_cid(stage, cid->sid)) {
                SHR_IF_ERR_EXIT(
                    bcmcfg_table_set_internal(unit,
                                              cid->sid,
                                              cid->fid,
                                              cid->fidx, 1,
                                              &cid->value, NULL));
            }
        } else if (cstr && cstr->vname) {
            if (warm) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META("table %s, field %s, value %s\n"),
                         cstr->tname, cstr->fname, cstr->vname));

            } else if (!bcmcfg_cached_lt_skip_cstr(stage, cstr->tname)) {
                SHR_IF_ERR_EXIT(
                    bcmlrd_table_field_name_to_idx(unit,
                                                   cstr->tname,
                                                   cstr->fname,
                                                   &sid, &fid));

                SHR_IF_ERR_EXIT(
                    bcmlrd_field_symbol_to_value(unit,
                                                 sid,
                                                 fid,
                                                 cstr->vname,
                                                 &value));

                value64 = value;

                SHR_IF_ERR_EXIT(
                    bcmcfg_table_set_internal(unit,
                                              sid,
                                              fid,
                                              cstr->fidx, 1,
                                              &value64, NULL));
            }
        } else if (cstr && !cstr->vname) {
            if (warm) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META("table %s, field %s, value %" PRIu64 "\n"),
                         cstr->tname, cstr->fname, cstr->value));
            } else if (!bcmcfg_cached_lt_skip_cstr(stage, cstr->tname)) {
                SHR_IF_ERR_EXIT(
                    bcmlrd_table_field_name_to_idx(unit,
                                                   cstr->tname,
                                                   cstr->fname,
                                                   &sid, &fid));

                SHR_IF_ERR_EXIT(
                    bcmcfg_table_set_internal(unit,
                                              sid,
                                              fid,
                                              cstr->fidx, 1,
                                              &cstr->value, NULL));
            }
        }
        list = list->next;
    }

exit:
    if (SHR_FUNC_ERR() ||
        stage == INIT_STAGE_NORMAL) {
        bcmcfg_table_cached_config_cleanup(unit);
    }

    SHR_FUNC_EXIT();
}

