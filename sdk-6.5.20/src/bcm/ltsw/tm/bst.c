/* \file bst.c
 *
 * TM BST Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw/mbcm/bst.h>

#include <bcm_int/ltsw/tm/bst.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/switch.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define NUM_CPU_QUEUES              48    /* CPU port has 48 MC Queues */
#define NUM_GP_QUEUES               12    /* GPs includingLB/mgmt has 12 Queues */
#define MMU_NUM_PG                  8
#define MMU_NUM_POOL                4
#define MMU_NUM_INT_PRI             16
#define MMU_NUM_RQE_QUEUES          9
#define DEFAULT_PROFILE_ID          0
#define MAX_ITM_PER_DEV             2

#define COMPILER_64_ZERO(dst)           ((dst) = 0)
#define COMPILER_64_ADD_32(dst, src)    ((dst) += (src))

#define LTSW_BST_OP_GET             0x1
#define LTSW_BST_OP_SET             0x2

typedef struct cosq_bst_res_index_s {

    /* port. */
    int port;

    /* spid or pg or cosq. */
    int index;
} cosq_bst_res_index_t;

/* bst event notification info. */
typedef struct cosq_bst_event_notif_s {
    /* Buffer pool (ITM) */
    int buffer_pool;

    /* Class for which event was triggered. */
    const char *event_class;

    /* Current state due to the triggered event. */
    const char *state;
} cosq_bst_event_notif_t;

/* Struct of BST priority group threshold profile.*/
typedef struct cosq_bst_thd_pri_grp_profile_s {

    /* Shared threshold in cells. */
    uint32_t  share_cells;

    /* Headroom threshold in cells. */
    uint32_t  headroom_cells;
} cosq_bst_thd_pri_grp_profile_t;

/* Struct of BST egress queue threshold profile.*/
typedef struct cosq_bst_thd_queue_profile_s {

    /* UC Queue Threshold in cells. */
    uint32_t  uc_queue_cells;

    /* MC Queue Threshold in cells. */
    uint32_t  mc_queue_cells;

    /* CPU Queue Threshold in cells. */
    uint32_t  cpu_queue_cells;
} cosq_bst_thd_queue_profile_t;

/*
 * \brief BST profile operation function.
 *
 * \param [in] unit Unit Number.
 * \param [in] res_info cosq_bst_handler_t.
 * \param [in] local_port local_port.
 * \param [in] index index.
 * \param [in] flags flags.
 * \param [in] cells cells.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
typedef int (*profile_op_f)(int unit, void *res_info, int local_port,
                            int index, int flags, uint32_t *cells);

/* Struct of BST threshold. */
typedef struct bst_threshold_s {

    /* Logical table name. */
    const char *thd_lt;

    /* port. */
    const char *key;

    /* Key 1. */
    const char *key1;

    /* buffer id. */
    const char *key2;

    /* Threshold field. */
    bcmi_lt_field_t field;

    /* Threshold max value. */
    int max_cells;

    /* Profile table. */
    const char *profile_lt;

    /* Profile field. */
    const char *profile_field;

    /* Profile Table handle. */
    bcmi_ltsw_profile_hdl_t profile_hdl;

    /* BST profile operation function. */
    profile_op_f profile_op;

    /* Callback function to compare profile set. */
    bcmi_ltsw_profile_cmp_cb cmp_func;

    /* threshold value of itm0 and itm1 in sw. */
    uint32_t *p_threshold[MAX_ITM_PER_DEV];
} bst_threshold_t;

/* Struct of BST stat.*/
typedef struct bst_stat_s {

    /* Logical table name. */
    const char *stat_lt;

    /* port. */
    const char *key;

    /* spid or pg or cosq. */
    const char *key1;

    /* itm. */
    const char *key2;

    /* stat field. */
    bcmi_lt_field_t field;

    /* Stat value of itm0 and itm1 in sw. */
    uint32_t *p_stat[MAX_ITM_PER_DEV];
} bst_stat_t;

typedef struct cosq_bst_handler_s {

    /* Struct of BST threshold. */
    bst_threshold_t  thd_hdl;

    /* Struct of BST stat. */
    bst_stat_t  stat_hdl;

    /* Resource class for an event. */
    const char *bst_module;
} cosq_bst_handler_t;

typedef struct cosq_bst_info_s {

    /* Track mode. */
    int track_mode;

    /* Snapshot mode. */
    int snapshot_mode;

    /* BST tracking enable. */
    int bst_tracking_enable;

    cosq_bst_handler_t bst_handler[bcmBstStatIdMaxCount];

    /* Mutex lock for bst stat sw copy.*/
    sal_mutex_t mutex;
} cosq_bst_info_t;

/* Software bookkeeping for BST information. */
static cosq_bst_info_t cosq_bst_info[BCM_MAX_NUM_UNITS];

/* COSQ BST info. */
#define COSQ_BST_INFO(_u_) (&cosq_bst_info[_u_])

#define COSQ_BST_RESOURCE(u,r) (&((cosq_bst_info[(u)]).bst_handler[(r)]))

/* Take BST lock */
#define COSQ_BST_LOCK(u) \
    sal_mutex_take(cosq_bst_info[u].mutex, SAL_MUTEX_FOREVER)

/* Release BST lock*/
#define COSQ_BST_UNLOCK(u) \
    sal_mutex_give(cosq_bst_info[u].mutex)

static tm_ltsw_bst_chip_driver_t *tm_bst_chip_driver_cb[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

static int
cosq_bst_thd_port_pool_profile_cmp_cb(int unit, void *entries,
                                      int entries_per_set,
                                      int index, int *cmp);

static int
cosq_bst_thd_port_pool_uc_profile_cmp_cb(int unit, void *entries,
                                         int entries_per_set,
                                         int index, int *cmp);

static int
cosq_bst_thd_port_pool_mc_profile_cmp_cb(int unit, void *entries,
                                         int entries_per_set,
                                         int index, int *cmp);

/*
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          PORT ID.
 *
 * \retval TRUE               Port is cpu port.
 * \retval FALSE              Port is non cpu port.
 */
static int
is_cpu_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_CPU);
}

/*
 * \brief Add an entry into LT.
 *
 * This function is used to add an entry into LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key_val              key field.
 * \param [in]  key_value            key value.
 * \param [in]  key_val              key 2 field.
 * \param [in]  key_value            key 2 value.
 * \param [in]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_table_set(int unit,
                   const char *lt_val,
                   const char *key0_val,
                   int key0_value,
                   const char *key1_val,
                   int key1_value,
                   const char *key2_val,
                   int key2_value,
                   bcmi_lt_field_t *field)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key0_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key0_val,
                                   key0_value));
    }

    if (key1_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key1_val,
                                   key1_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl,
                               field->fld_name,
                               field->u.val));

    LOG_INFO(BSL_LOG_MODULE,
    (BSL_META_U(unit, "lt %s update %s=%d %s=%d %s=%d %s=%d\n"),
                       lt_val, key0_val ? key0_val : "invalid", key0_value,
                       key1_val ? key1_val : "invalid", key1_value,
                       key2_val ? key2_val : "invalid", key2_value,
                       field->fld_name, (int)field->u.val));

    rv = bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL);

    if (rv != SHR_E_NONE) {
        LOG_ERROR(BSL_LOG_MODULE,
        (BSL_META_U(unit, "lt %s update %s=%d %s=%d %s=%d %s=%d\n"),
                           lt_val, key0_val ? key0_val : "invalid", key0_value,
                           key1_val ? key1_val : "invalid", key1_value,
                           key2_val ? key2_val : "invalid", key2_value,
                           field->fld_name, (int)field->u.val));
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entry from LT.
 *
 * This function is used to get an entry from LT.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  lt_val               Logic table name.
 * \param [in]  key_val              key field.
 * \param [in]  key_value            key value.
 * \param [in]  key_val              key 2 field.
 * \param [in]  key_value            key 2 value.
 * \param [out]  field                bcmi_lt_field_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_table_get(int unit,
                   const char *lt_val,
                   const char *key0_val,
                   int key0_value,
                   const char *key1_val,
                   int key1_value,
                   const char *key2_val,
                   int key2_value,
                   bcmi_lt_field_t *field)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_val, &entry_hdl));

    if (key0_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key0_val,
                                   key0_value));
    }

    if (key1_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key1_val,
                                   key1_value));
    }

    if (key2_val != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, key2_val,
                                   key2_value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, field->fld_name,
                               &(field->u.val)));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  cells                cells.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_entry_add(int unit, cosq_bst_handler_t *res_info,
                                         uint32_t cells, int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, res_info->thd_hdl.profile_hdl,
                                          &cells, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, res_info->thd_hdl.profile_lt, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, res_info->thd_hdl.field.fld_name,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, res_info->thd_hdl.profile_field, cells));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d\n"),
            res_info->thd_hdl.profile_lt,
            res_info->thd_hdl.field.fld_name, *profile_idx,
            res_info->thd_hdl.profile_field, cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_entry_get(int unit, cosq_bst_handler_t *res_info,
                                    int profile_idx, uint32_t *cells)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } value;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, res_info->thd_hdl.profile_lt,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, res_info->thd_hdl.field.fld_name,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, res_info->thd_hdl.profile_field,
                               &(value.val)));
    *cells = value.val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from port pool profile.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] res_info             cosq_bst_handler_t.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
cosq_bst_thd_port_pool_profile_entry_delete(int unit,
                                            cosq_bst_handler_t *res_info,
                                            int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, res_info->thd_hdl.profile_hdl,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, res_info->thd_hdl.profile_lt,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, res_info->thd_hdl.field.fld_name,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_hash_cb(int unit, void *entries,
                                       int entries_per_set,
                                       uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries, entries_per_set * sizeof(uint32_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_thd_port_pool_default_profile_add(int unit, cosq_bst_handler_t *res_info)
{
    int cells = res_info->thd_hdl.max_cells;
    int profile_idx;
    uint32_t ref_count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    /* Add default profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_entry_add(unit, res_info, cells,
                                                  &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, res_info->thd_hdl.thd_lt, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, res_info->thd_hdl.profile_hdl,
                                              1, profile_idx, ref_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register the port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_register(int unit, cosq_bst_handler_t *res_info)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = res_info->thd_hdl.profile_hdl;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, res_info->thd_hdl.profile_lt,
                                       res_info->thd_hdl.field.fld_name,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_bst_thd_port_pool_profile_hash_cb,
                                    res_info->thd_hdl.cmp_func));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of the port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_recover(int unit, cosq_bst_handler_t *res_info,
                                       int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = res_info->thd_hdl.profile_hdl;
    uint32_t cells = 0;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_port_pool_profile_entry_get(unit, res_info,
                                                      profile_id, &cells));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &cells,
                                           entries_per_set, profile_id));

        if (profile_id == DEFAULT_PROFILE_ID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, 1));
        }
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_recover(int unit, cosq_bst_handler_t *res_info)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, res_info->thd_hdl.thd_lt, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl,
                                   res_info->thd_hdl.field.fld_name,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_port_pool_profile_recover(unit, res_info, index));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_init(int unit, cosq_bst_handler_t *res_info)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = res_info->thd_hdl.profile_hdl;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_register(unit, res_info));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, res_info->thd_hdl.profile_lt));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_port_pool_default_profile_add(unit, res_info));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_port_pool_recover(unit, res_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              profile.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_entry_add(int unit,
                                       cosq_bst_thd_pri_grp_profile_t profile,
                                       int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_BST_THD_PRI_GRP,
                                          &profile, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_ING_BST_THD_PRI_GRP_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_ING_BST_THD_PRI_GRP_PROFILE_IDs,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, SHARED_CELLSs, profile.share_cells));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, HEADROOM_CELLSs, profile.headroom_cells));

    LOG_INFO(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "lt %s update %s=%d %s=%d %s=%d\n"),
            TM_ING_BST_THD_PRI_GRP_PROFILEs,
            TM_ING_BST_THD_PRI_GRP_PROFILE_IDs, *profile_idx,
            SHARED_CELLSs, profile.share_cells,
            HEADROOM_CELLSs, profile.headroom_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              profile.
 * \param [out] cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_entry_get(int unit, int profile_idx,
                                       cosq_bst_thd_pri_grp_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } value;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_ING_BST_THD_PRI_GRP_PROFILEs,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_ING_BST_THD_PRI_GRP_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, SHARED_CELLSs,
                               &(value.val)));
    profile->share_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, HEADROOM_CELLSs,
                               &(value.val)));
    profile->headroom_cells = value.val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
cosq_bst_thd_pri_grp_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_BST_THD_PRI_GRP,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_ING_BST_THD_PRI_GRP_PROFILEs,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_ING_BST_THD_PRI_GRP_PROFILE_IDs,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_hash_cb(int unit, void *entries,
                                     int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(cosq_bst_thd_pri_grp_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_cmp_cb(int unit, void *entries,
                                    int entries_per_set, int index, int *cmp)
{
    cosq_bst_thd_pri_grp_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(cosq_bst_thd_pri_grp_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_pri_grp_profile_entry_get(unit, index, &profile));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(cosq_bst_thd_pri_grp_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default profile.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  res_info      cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_thd_pri_grp_default_profile_add(int unit)
{
    cosq_bst_thd_pri_grp_profile_t profile;
    int profile_idx;
    cosq_bst_handler_t *res_info;
    uint32_t ref_count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdPriGroupShared);

    /* Add default profile. */
    profile.headroom_cells =
                    COSQ_BST_RESOURCE(unit, bcmBstStatIdPriGroupHeadroom)->thd_hdl.max_cells;

    profile.share_cells = res_info->thd_hdl.max_cells;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_pri_grp_profile_entry_add(unit, profile, &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, res_info->thd_hdl.thd_lt, &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, res_info->thd_hdl.profile_hdl,
                                              1, profile_idx, ref_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_PRI_GRP;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_ING_BST_THD_PRI_GRP_PROFILEs,
                                       TM_ING_BST_THD_PRI_GRP_PROFILE_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_bst_thd_pri_grp_profile_hash_cb,
                                    cosq_bst_thd_pri_grp_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_PRI_GRP;
    cosq_bst_thd_pri_grp_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_pri_grp_profile_entry_get(unit, profile_id, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &profile,
                                           entries_per_set, profile_id));

        if (profile_id == DEFAULT_PROFILE_ID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, 1));
        }
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_recover(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_ING_THD_PORT_PRI_GRPs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl,
                                   TM_ING_BST_THD_PRI_GRP_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_pri_grp_profile_recover(unit, index));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize LT TM_ING_BST_THD_PRI_GRP_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_pri_grp_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_PRI_GRP;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_pri_grp_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_ING_BST_THD_PRI_GRP_PROFILEs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_pri_grp_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_pri_grp_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Add an entry into LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              profile.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_entry_add(int unit,
                                     cosq_bst_thd_queue_profile_t profile,
                                     int *profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int entries_per_set = 1;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_BST_THD_QUEUE,
                                          &profile, 0,
                                          entries_per_set, profile_idx);
    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Add an entry into LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EGR_BST_THD_Q_PROFILEs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EGR_BST_THD_Q_PROFILE_IDs,
                               *profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, UC_Q_CELLSs, profile.uc_queue_cells));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, MC_Q_CELLSs, profile.mc_queue_cells));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, CPU_Q_CELLSs, profile.cpu_queue_cells));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Get a entry from LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              profile.
 * \param [out] cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_entry_get(int unit, int profile_idx,
                                     cosq_bst_thd_queue_profile_t *profile)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } value;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EGR_BST_THD_Q_PROFILEs,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EGR_BST_THD_Q_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, UC_Q_CELLSs,
                               &(value.val)));
    profile->uc_queue_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, MC_Q_CELLSs,
                               &(value.val)));
    profile->mc_queue_cells = value.val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(entry_hdl, CPU_Q_CELLSs,
                               &(value.val)));
    profile->cpu_queue_cells = value.val;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Delete a entry from LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
cosq_bst_thd_queue_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_BST_THD_QUEUE,
                                      profile_idx);
    if (rv == SHR_E_BUSY) {
        /* Profile index is still used by others and return directly */
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_EGR_BST_THD_Q_PROFILEs,
                              &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(entry_hdl, TM_EGR_BST_THD_Q_PROFILE_IDs,
                               profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Calculate hash signature for LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_hash_cb(int unit, void *entries,
                                   int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(cosq_bst_thd_queue_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_cmp_cb(int unit, void *entries,
                                  int entries_per_set, int index, int *cmp)
{
    cosq_bst_thd_queue_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&profile, 0, sizeof(cosq_bst_thd_queue_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_queue_profile_entry_get(unit, index, &profile));

    *cmp = sal_memcmp(entries, (void *)&profile,
                      sizeof(cosq_bst_thd_queue_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default profile.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_thd_queue_default_profile_add(int unit)
{
    cosq_bst_thd_queue_profile_t profile;
    int profile_idx;
    cosq_bst_handler_t *res_info;
    uint32_t ref_count = 0;
    uint32_t entry_num = 0;

    SHR_FUNC_ENTER(unit);

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdUcast);

    /* Add default profile. */
    profile.uc_queue_cells = COSQ_BST_RESOURCE(unit, bcmBstStatIdUcast)->thd_hdl.max_cells;
    profile.mc_queue_cells = COSQ_BST_RESOURCE(unit, bcmBstStatIdMcast)->thd_hdl.max_cells;
    /*
     * For current chips, the max THD value of CPU queue
     * is always same as MC queue
     */
    profile.cpu_queue_cells = COSQ_BST_RESOURCE(unit, bcmBstStatIdMcast)->thd_hdl.max_cells;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_queue_profile_entry_add(unit, profile, &profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, res_info->thd_hdl.thd_lt,
                              &entry_num));
    ref_count += entry_num;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, COSQ_BST_RESOURCE(unit, bcmBstStatIdMcast)->thd_hdl.thd_lt,
                              &entry_num));
    ref_count += entry_num;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, res_info->thd_hdl.profile_hdl,
                                              1, profile_idx, ref_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_QUEUE;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, TM_EGR_BST_THD_Q_PROFILEs,
                                       TM_EGR_BST_THD_Q_PROFILE_IDs,
                                       &idx_min, &idx_max));
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    cosq_bst_thd_queue_profile_hash_cb,
                                    cosq_bst_thd_queue_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_QUEUE;
    cosq_bst_thd_queue_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    int profile_id;

    SHR_FUNC_ENTER(unit);

    profile_id = index * entries_per_set;

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, profile_id,
                                         &ref_count));

    if (ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_queue_profile_entry_get(unit, profile_id, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, &profile,
                                           entries_per_set, profile_id));

        if (profile_id == DEFAULT_PROFILE_ID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase
                    (unit, profile_hdl, entries_per_set, profile_id, 1));
        }
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, profile_id, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_recover(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv, dunit, index;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_UC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl,
                                   TM_EGR_BST_THD_Q_PROFILE_IDs,
                                   &data));

        index = data;

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_queue_profile_recover(unit, index));
    }

    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_free(entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_THD_MC_Qs, &entry_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl,
                                   TM_EGR_BST_THD_Q_PROFILE_IDs,
                                   &data));

        index = data;
        if (index != DEFAULT_PROFILE_ID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_queue_profile_recover(unit, index));
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize LT TM_EGR_BST_THD_Q_PROFILEs.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_queue_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_BST_THD_QUEUE;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_profile_unregister(unit, profile_hdl), SHR_E_NOT_FOUND);

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_queue_profile_register(unit));

    if(!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, TM_EGR_BST_THD_Q_PROFILEs));

        /* Add default profile. */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_queue_default_profile_add(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_thd_queue_recover(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST stat field operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  key0_val             key 0 value.
 * \param [in]  key1_val             key 1 value.
 * \param [in]  key2_val             key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  value                value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_stat_field_op(int unit, cosq_bst_handler_t *res_info,
                       int key0_val, int key1_val, int key2_val,
                       int flags, uint32_t *value)
{
    bcmi_lt_field_t field;
    const char *tbl_name;
    const char *key0 = NULL;
    const char *key1 = NULL;
    const char *key2 = NULL;

    SHR_FUNC_ENTER(unit);

    tbl_name = res_info->stat_hdl.stat_lt;
    field.fld_name = res_info->stat_hdl.field.fld_name;
    key0 = res_info->stat_hdl.key;
    key1 = res_info->stat_hdl.key1;
    key2 = res_info->stat_hdl.key2;

    if (flags == LTSW_BST_OP_SET) {
        field.u.val = *value;
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_table_set(unit, tbl_name, key0, key0_val,
                                key1, key1_val, key2, key2_val, &field));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_table_get(unit, tbl_name, key0, key0_val,
                                key1, key1_val, key2, key2_val, &field));
        *value = field.u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST threshold field operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  res_info             cosq_bst_handler_t.
 * \param [in]  key0_val             key 0 value.
 * \param [in]  key1_val             key 1 value.
 * \param [in]  flags                flags.
 * \param [in]  value                value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_threshold_field_op(int unit, cosq_bst_handler_t *res_info,
                            int key0_val, int key1_val, int key2_val,
                            int flags, uint32_t *value)
{
    bcmi_lt_field_t field;
    const char *tbl_name;
    const char *key0 = NULL;
    const char *key1 = NULL;
    const char *key2 = NULL;

    SHR_FUNC_ENTER(unit);

    tbl_name = res_info->thd_hdl.thd_lt;
    field.fld_name = res_info->thd_hdl.field.fld_name;
    key0 = res_info->thd_hdl.key;
    key1 = res_info->thd_hdl.key1;
    key2 = res_info->thd_hdl.key2;

    if (flags == LTSW_BST_OP_SET) {
        field.u.val = *value;
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_table_set(unit, tbl_name, key0, key0_val,
                                key1, key1_val, key2, key2_val, &field));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_table_get(unit, tbl_name, key0, key0_val,
                                key1, key1_val, key2, key2_val, &field));
        *value = field.u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST port pool threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_bst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  pool_id              key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_port_pool_threshold_op(int unit, void *info,
                                int local_port, int pool_id, int flags,
                                uint32_t *cells)
{
    int old_profile_id, profile_id;
    cosq_bst_handler_t *res_info = (cosq_bst_handler_t *)info;
    uint32_t old_cells = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = cosq_bst_threshold_field_op(unit, res_info, local_port, pool_id,
                                     -1, LTSW_BST_OP_GET,
                                     (uint32_t *)&old_profile_id);
    if (rv == SHR_E_NOT_FOUND) {
        old_profile_id = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_entry_get(unit, res_info, old_profile_id,
                                                  &old_cells));

    if (flags == LTSW_BST_OP_SET) {
        if (old_cells != *cells) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_port_pool_profile_entry_add(unit, res_info, *cells,
                                                          &profile_id));
            SHR_IF_ERR_CONT
                (cosq_bst_threshold_field_op(unit, res_info, local_port, pool_id,
                                             -1, LTSW_BST_OP_SET,
                                             (uint32_t *)&profile_id));
            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_port_pool_profile_entry_delete(unit, res_info,
                                                                 profile_id));
                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_port_pool_profile_entry_delete(unit, res_info,
                                                             old_profile_id));
        }
    } else {
        *cells = old_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST pg shared threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_bst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  pg                   key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_pg_shared_threshold_op(int unit, void *info,
                                int local_port, int pg,
                                int flags, uint32_t *cells)
{
    int old_profile_id, profile_id;
    cosq_bst_handler_t *res_info = (cosq_bst_handler_t *)info;
    cosq_bst_thd_pri_grp_profile_t profile;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = cosq_bst_threshold_field_op(unit, res_info, local_port, pg,
                                     -1, LTSW_BST_OP_GET,
                                     (uint32_t *)&old_profile_id);
    if (rv == SHR_E_NOT_FOUND) {
        old_profile_id = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_pri_grp_profile_entry_get(unit, old_profile_id, &profile));

    if (flags == LTSW_BST_OP_SET) {
        if (profile.share_cells != *cells) {
            profile.share_cells = *cells;
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_pri_grp_profile_entry_add(unit, profile,
                                                        &profile_id));
            SHR_IF_ERR_CONT
                (cosq_bst_threshold_field_op(unit, res_info, local_port, pg,
                                             -1, LTSW_BST_OP_SET,
                                             (uint32_t *)&profile_id));

            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_pri_grp_profile_entry_delete(unit, profile_id));

                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_pri_grp_profile_entry_delete(unit, old_profile_id));
        }
    } else {
        *cells = profile.share_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST pg headroom threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_bst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  pg                   key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_pg_headroom_threshold_op(int unit, void *info,
                                  int local_port, int pg,
                                  int flags, uint32_t *cells)
{
    int old_profile_id, profile_id;
    cosq_bst_handler_t *res_info = (cosq_bst_handler_t *)info;
    cosq_bst_thd_pri_grp_profile_t profile;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = cosq_bst_threshold_field_op(unit, res_info, local_port, pg,
                                     -1, LTSW_BST_OP_GET,
                                     (uint32_t *)&old_profile_id);
    if (rv == SHR_E_NOT_FOUND) {
        old_profile_id = 0;
        rv = SHR_E_NONE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_pri_grp_profile_entry_get(unit, old_profile_id, &profile));

    if (flags == LTSW_BST_OP_SET) {
        if (profile.headroom_cells != *cells) {
            profile.headroom_cells = *cells;
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_pri_grp_profile_entry_add(unit, profile,
                                                        &profile_id));
            SHR_IF_ERR_CONT
                (cosq_bst_threshold_field_op(unit, res_info, local_port, pg,
                                             -1, LTSW_BST_OP_SET,
                                             (uint32_t *)&profile_id));
            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_pri_grp_profile_entry_delete(unit, profile_id));

                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_pri_grp_profile_entry_delete(unit, old_profile_id));
        }
    } else {
        *cells = profile.headroom_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST ucast threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_bst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  queue                key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_ucast_threshold_op(int unit, void *info,
                            int local_port, int queue,
                            int flags, uint32_t *cells)
{
    int old_profile_id, profile_id;
    cosq_bst_handler_t *res_info = (cosq_bst_handler_t *)info;
    cosq_bst_thd_queue_profile_t profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_threshold_field_op(unit, res_info, local_port, queue,
                                     -1, LTSW_BST_OP_GET,
                                     (uint32_t *)&old_profile_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_queue_profile_entry_get(unit, old_profile_id, &profile));

    if (flags == LTSW_BST_OP_SET) {
        if (profile.uc_queue_cells != *cells) {
            profile.uc_queue_cells = *cells;

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_queue_profile_entry_add(unit, profile,
                                                      &profile_id));
            SHR_IF_ERR_CONT
                (cosq_bst_threshold_field_op(unit, res_info, local_port, queue,
                                             -1, LTSW_BST_OP_SET,
                                             (uint32_t *)&profile_id));
            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_queue_profile_entry_delete(unit, profile_id));

                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_queue_profile_entry_delete(unit, old_profile_id));
        }
    } else {
        *cells = profile.uc_queue_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief BST mcast threshold operation function.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  info                 cosq_bst_handler_t.
 * \param [in]  local_port           key 1 value.
 * \param [in]  queue                key 2 value.
 * \param [in]  flags                flags.
 * \param [in]  cells                cells.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_mcast_threshold_op(int unit, void *info,
                            int local_port, int queue, int flags,
                            uint32_t *cells)
{
    int old_profile_id, profile_id;
    cosq_bst_handler_t *res_info = (cosq_bst_handler_t *)info;
    cosq_bst_thd_queue_profile_t profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_threshold_field_op(unit, res_info, local_port, queue,
                                     -1, LTSW_BST_OP_GET,
                                     (uint32_t *)&old_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_queue_profile_entry_get(unit, old_profile_id, &profile));

    if (flags == LTSW_BST_OP_SET) {
        profile_id = old_profile_id;
        if (is_cpu_port(unit, local_port)) {
            if (profile.cpu_queue_cells != *cells) {
                profile.cpu_queue_cells = *cells;
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_queue_profile_entry_add(unit, profile,
                                                          &profile_id));
            }
        } else {
            if (profile.mc_queue_cells != *cells) {
                profile.mc_queue_cells = *cells;
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_queue_profile_entry_add(unit, profile,
                                                          &profile_id));
            }
        }

        if (profile_id != old_profile_id) {
            SHR_IF_ERR_CONT
                (cosq_bst_threshold_field_op(unit, res_info, local_port, queue,
                                             -1, LTSW_BST_OP_SET,
                                             (uint32_t *)&profile_id));
            if (SHR_FUNC_ERR()) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_thd_queue_profile_entry_delete(unit, profile_id));

                SHR_EXIT();
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_thd_queue_profile_entry_delete(unit, old_profile_id));
        }

    } else {
        if (is_cpu_port(unit, local_port)) {
            *cells = profile.cpu_queue_cells;
        } else {
            *cells = profile.mc_queue_cells;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set BST control config.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  module               Class.
 * \param [in]  track_mode           track mode.
 * \param [in]  cor                  clear on read value.
 * \param [in]  snapshot             snapshot.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_control_set(int unit, const char *module, int track_mode,
                     int cor, int snapshot)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TM_BST_CONTROLs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, CLASSs, module));

    if (track_mode != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(entry_hdl, TRACKING_MODEs,
            ((track_mode) ? HIGH_WATERMARK_TRACKs : INSTANTANEOUS_TRACKs)));
    }

    if (cor != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, CLEAR_ON_READs,
                                   ((cor) ? true : false)));
    }

    if (snapshot != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(entry_hdl, SNAPSHOTs,
                                   ((snapshot) ? true : false)));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, entry_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Get BST control config.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  module               Class.
 * \param [out] track_mode           track mode.
 * \param [out] cor                  clear on read value.
 * \param [out] snapshot             snapshot.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_control_get(int unit, const char *module, int *track_mode,
                     int *cor, int *snapshot)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        {CLASSs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        {TRACKING_MODEs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        {CLEAR_ON_READs, BCMI_LT_FIELD_F_GET, 0, {0}},
        {SNAPSHOTs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.sym_val = module;

    rv = bcmi_lt_entry_get(unit, TM_BST_CONTROLs,
                           &lt_entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        if (track_mode != NULL) {
            *track_mode = 0;
        }

        if (cor != NULL) {
            *cor = 0;
        }

        if (snapshot != NULL) {
            *snapshot = 0;
        }
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (track_mode != NULL) {
        if (sal_strcasecmp(fields[1].u.sym_val, INSTANTANEOUS_TRACKs) == 0) {
            *track_mode = 0;
        } else if (sal_strcasecmp(fields[1].u.sym_val, HIGH_WATERMARK_TRACKs) == 0) {
            *track_mode = 1;
        } else {
            *track_mode = 0;
        }
    }

    if (cor != NULL) {
        *cor = fields[2].u.val;
    }

    if (snapshot != NULL) {
        *snapshot = fields[3].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LT TM_BST_EVENT_STATE_CONTROLs handler.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  buffer               buffer id.
 * \param [in]  module               module.
 * \param [in]  arg                  enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_event_state_control_set(int unit, int buffer, const char *module,
                                 int arg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {BUFFER_POOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {CLASSs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2 */ {STATEs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer;
    fields[1].u.sym_val = module;

    if (arg == 0) {
        fields[2].u.sym_val = OFFs;
    } else if (arg == 1) {
        fields[2].u.sym_val = ARMEDs;
    } else if (arg == 2) {
        fields[2].u.sym_val = ARMED_PASSIVEs;
    } else if (arg == 3) {
        fields[2].u.sym_val = TRIGGEREDs;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_BST_EVENT_STATE_CONTROLs,
                           &lt_entry, NULL));
    LOG_INFO(BSL_LOG_MODULE,
    (BSL_META_U(unit, "lt %s update BUFFER_POOL=%d CLASS=%s STATE=%s\n"),
                       TM_BST_EVENT_STATE_CONTROLs,
                       (int)fields[0].u.val,
                       fields[1].u.sym_val,
                       fields[2].u.sym_val));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LT TM_BST_EVENT_STATE_CONTROLs handler.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  buffer               buffer id.
 * \param [in]  module               module.
 * \param [in]  arg                  enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_event_state_control_get(int unit, int buffer, const char *module,
                                 int *arg)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {BUFFER_POOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {CLASSs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2 */ {STATEs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer;
    fields[1].u.sym_val = module;

    rv = bcmi_lt_entry_get(unit, TM_BST_EVENT_STATE_CONTROLs,
                           &lt_entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        if (arg != NULL) {
            *arg = 0;
        }

        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (arg != NULL) {
        if (sal_strcasecmp(fields[2].u.sym_val, OFFs) == 0) {
            *arg = 0;
        } else if (sal_strcasecmp(fields[2].u.sym_val, ARMEDs) == 0) {
            *arg = 1;
        } else if (sal_strcasecmp(fields[2].u.sym_val, ARMED_PASSIVEs) == 0) {
            *arg = 2;
        } else if (sal_strcasecmp(fields[2].u.sym_val, TRIGGEREDs) == 0) {
            *arg = 3;
        }
    }
    LOG_INFO(BSL_LOG_MODULE,
    (BSL_META_U(unit, "lt %s lookup BUFFER_POOL=%d CLASS=%s STATE=%s\n"),
                       TM_BST_EVENT_STATE_CONTROLs,
                       (int)fields[0].u.val,
                       fields[1].u.sym_val,
                       fields[2].u.sym_val));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LT TM_BST_EVENT_STATE_CONTROLs handler.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  buffer               buffer id.
 * \param [in]  module               module.
 * \param [in]  arg                  enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_event_state_get(int unit, int buffer, const char *module,
                                 int *arg)
{
    int rv;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {BUFFER_POOLs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {CLASSs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2 */ {STATEs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = buffer;
    fields[1].u.sym_val = module;

    rv = bcmi_lt_entry_get(unit, TM_BST_EVENT_STATEs,
                           &lt_entry, NULL, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        if (arg != NULL) {
            *arg = 0;
        }

        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (arg != NULL) {
        if (sal_strcasecmp(fields[2].u.sym_val, OFFs) == 0) {
            *arg = 0;
        } else if (sal_strcasecmp(fields[2].u.sym_val, ARMEDs) == 0) {
            *arg = 1;
        } else if (sal_strcasecmp(fields[2].u.sym_val, ARMED_PASSIVEs) == 0) {
            *arg = 2;
        } else if (sal_strcasecmp(fields[2].u.sym_val, TRIGGEREDs) == 0) {
            *arg = 3;
        }
    }
    LOG_INFO(BSL_LOG_MODULE,
    (BSL_META_U(unit, "lt %s lookup BUFFER_POOL=%d CLASS=%s STATE=%s\n"),
                       TM_BST_EVENT_STATE_CONTROLs,
                       (int)fields[0].u.val,
                       fields[1].u.sym_val,
                       fields[2].u.sym_val));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Retrieve BST tracking status and disable bst.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  module               module.
 * \param [out] sync_val             enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_pre_sync(int unit, const char *module, int *sync_val)
{
    int itm = 0;
    int arg = 0;
    int snapshot_en = 0;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_get(unit, module, NULL, NULL, &snapshot_en));

    for (itm = 0; itm < device_info.num_itm; itm++ ) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_event_state_get(unit, itm, module, &arg));
        sync_val[itm] = arg;

        if (snapshot_en) {
            if (sync_val[itm] == 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_event_state_control_set(unit, itm, module, 0));
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_event_state_control_set(unit, itm, module, 0));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Restore BST tracking status.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  module               module.
 * \param [in]  sync_val             enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_post_sync(int unit, const char *module, int *sync_val)
{
    int itm;
    bcmi_ltsw_cosq_device_info_t device_info;
    int snapshot_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_get(unit, module, NULL, NULL, &snapshot_en));

    for (itm = 0; itm < device_info.num_itm; itm++ ) {
        if (snapshot_en) {
            if (sync_val[itm] == 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_event_state_control_set(unit, itm, module, sync_val[itm]));
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_event_state_control_set(unit, itm, module, sync_val[itm]));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Enable/Disable BST tracking.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  buffer               buffer id.
 * \param [in]  module               module.
 * \param [in]  arg                  enable value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_tracking_enable_set(int unit, int buffer, const char *module, int enable)
{
    int itm, from_itm, to_itm;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));


    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_event_state_control_set(unit, itm, module, enable));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
cosq_bst_tracking_enable_get(int unit, int buffer, const char *module, int *enable)
{
    int itm = 0;
    int arg = 0;

    SHR_FUNC_ENTER(unit);

    itm = buffer;
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_event_state_control_get(unit, itm, module, &arg));

    *enable = arg ? 1 : 0;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Init bst handler.
 */
static void
cosq_bst_handler_init(int unit)
{
    bcm_bst_stat_id_t bid;
    int alloc_size = 0;
    static cosq_bst_handler_t bst_handler[] =
    {
        /* bcmBstStatIdDevice */{{TM_BST_DEVICE_THDs,       /* Threshold LT */
                                NULL,                      /* Key0 */
                                NULL,                      /* Key1 */
                                BUFFER_POOLs,              /* Key2 */
                                {CELLSs},                  /* Threshold Field */
                                0x7fff,                    /* Field default value */
                                },
                                {CTR_TM_BST_DEVICEs,       /* BST Stat LT */
                                NULL,                      /* Key0 */
                                NULL,                      /* Key1 */
                                BUFFER_POOLs,              /* Key2 */
                                {CELLSs},                  /* Stat Field */
                                },
                                DEVICEs,                   /* Resource class */
                                },
        /* bcmBstStatIdEgrPool */ {{TM_BST_SERVICE_POOL_THDs,
                                NULL,
                                TM_BST_SERVICE_POOL_THD_IDs,
                                NULL,
                                {EGR_SERVICE_POOL_UC_CELLSs},
                                0x7ffff,
                                },
                                {CTR_EGR_TM_BST_SERVICE_POOLs,
                                NULL,
                                TM_EGR_SERVICE_POOL_IDs,
                                BUFFER_POOLs,
                                {UC_CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdEgrMCastPool */ {{TM_BST_SERVICE_POOL_THDs,
                                NULL,
                                TM_BST_SERVICE_POOL_THD_IDs,
                                NULL,
                                {EGR_SERVICE_POOL_MC_CELLSs},
                                0x7ffff,
                                },
                                {CTR_EGR_TM_BST_SERVICE_POOLs,
                                NULL,
                                TM_EGR_SERVICE_POOL_IDs,
                                BUFFER_POOLs,
                                {MC_CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdIngPool */ {{TM_BST_SERVICE_POOL_THDs,
                                NULL,
                                TM_BST_SERVICE_POOL_THD_IDs,
                                NULL,
                                {ING_SERVICE_POOL_CELLSs},
                                0x7ffff,
                                },
                                {CTR_ING_TM_BST_SERVICE_POOLs,
                                NULL,
                                TM_ING_SERVICE_POOL_INDEXs,
                                BUFFER_POOLs,
                                {SHARED_CELLSs},
                                },
                                INGs,
                                },
        /* bcmBstStatIdPortPool */ {{TM_ING_THD_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_ING_SERVICE_POOL_IDs,
                                NULL,
                                {TM_ING_BST_THD_PORT_SERVICE_POOL_PROFILE_IDs},
                                0x7ffff,
                                TM_ING_BST_THD_PORT_SERVICE_POOL_PROFILEs,
                                SHARED_CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_PORT_SERVICE_POOL,
                                cosq_bst_port_pool_threshold_op,
                                cosq_bst_thd_port_pool_profile_cmp_cb,
                                },
                                {CTR_ING_TM_BST_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_ING_SERVICE_POOL_IDs,
                                NULL,
                                {CELLSs},
                                },
                                INGs,
                                },
        /* bcmBstStatIdPriGroupShared */ {{TM_ING_THD_PORT_PRI_GRPs,
                                PORT_IDs,
                                TM_PRI_GRP_IDs,
                                NULL,
                                {TM_ING_BST_THD_PRI_GRP_PROFILE_IDs},
                                0x7ffff,
                                TM_ING_BST_THD_PRI_GRP_PROFILEs,
                                SHARED_CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_PRI_GRP,
                                cosq_bst_pg_shared_threshold_op,
                                },
                                {CTR_ING_TM_BST_PORT_PRI_GRPs,
                                PORT_IDs,
                                TM_PRI_GRP_IDs,
                                NULL,
                                {SHARED_CELLSs},
                                },
                                INGs,
                                },
        /* bcmBstStatIdPriGroupHeadroom */ {{TM_ING_THD_PORT_PRI_GRPs,
                                PORT_IDs,
                                TM_PRI_GRP_IDs,
                                NULL,
                                {TM_ING_BST_THD_PRI_GRP_PROFILE_IDs},
                                0x7ffff,
                                TM_ING_BST_THD_PRI_GRP_PROFILEs,
                                HEADROOM_CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_PRI_GRP,
                                cosq_bst_pg_headroom_threshold_op,
                                },
                                {CTR_ING_TM_BST_PORT_PRI_GRPs,
                                PORT_IDs,
                                TM_PRI_GRP_IDs,
                                NULL,
                                {HEADROOM_CELLSs},
                                },
                                INGs,
                                },
        /* bcmBstStatIdUcast */ {{TM_THD_UC_Qs,
                                PORT_IDs,
                                TM_UC_Q_IDs,
                                NULL,
                                {TM_EGR_BST_THD_Q_PROFILE_IDs},
                                0x7ffff,
                                TM_EGR_BST_THD_Q_PROFILEs,
                                UC_Q_CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_QUEUE,
                                cosq_bst_ucast_threshold_op,
                                },
                                {CTR_EGR_TM_BST_UC_Qs,
                                PORT_IDs,
                                TM_UC_Q_IDs,
                                BUFFER_POOLs,
                                {CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdMcast */ {{TM_THD_MC_Qs,
                                PORT_IDs,
                                TM_MC_Q_IDs,
                                NULL,
                                {TM_EGR_BST_THD_Q_PROFILE_IDs},
                                0x7ffff,
                                TM_EGR_BST_THD_Q_PROFILEs,
                                MC_Q_CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_QUEUE,
                                cosq_bst_mcast_threshold_op,
                                },
                                {CTR_EGR_TM_BST_MC_Qs,
                                PORT_IDs,
                                TM_MC_Q_IDs,
                                BUFFER_POOLs,
                                {CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdHeadroomPool */ {{TM_BST_SERVICE_POOL_THDs,
                                NULL,
                                TM_BST_SERVICE_POOL_THD_IDs,
                                NULL,
                                {ING_HEADROOM_POOL_CELLSs},
                                0x7ffff,
                                },
                                {CTR_ING_TM_BST_SERVICE_POOLs,
                                NULL,
                                TM_ING_SERVICE_POOL_INDEXs,
                                BUFFER_POOLs,
                                {HEADROOM_CELLSs},
                                },
                                INGs,
                                },
        /* bcmBstStatIdEgrPortPoolSharedUcast */ {{TM_EGR_THD_UC_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_EGR_SERVICE_POOL_IDs,
                                NULL,
                                {TM_EGR_BST_THD_UC_PORT_SERVICE_POOL_PROFILE_IDs},
                                0x7ffff,
                                TM_EGR_BST_THD_UC_PORT_SERVICE_POOL_PROFILEs,
                                CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_UC_PORT_SERVICE_POOL,
                                cosq_bst_port_pool_threshold_op,
                                cosq_bst_thd_port_pool_uc_profile_cmp_cb,
                                },
                                {CTR_EGR_TM_BST_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_EGR_SERVICE_POOL_IDs,
                                BUFFER_POOLs,
                                {UC_CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdEgrPortPoolSharedMcast */ {{TM_EGR_THD_MC_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_EGR_SERVICE_POOL_IDs,
                                NULL,
                                {TM_EGR_BST_THD_MC_PORT_SERVICE_POOL_PROFILE_IDs},
                                0x7ffff,
                                TM_EGR_BST_THD_MC_PORT_SERVICE_POOL_PROFILEs,
                                CELLSs,
                                BCMI_LTSW_PROFILE_BST_THD_MC_PORT_SERVICE_POOL,
                                cosq_bst_port_pool_threshold_op,
                                cosq_bst_thd_port_pool_mc_profile_cmp_cb,
                                },
                                {CTR_EGR_TM_BST_PORT_SERVICE_POOLs,
                                PORT_IDs,
                                TM_EGR_SERVICE_POOL_IDs,
                                BUFFER_POOLs,
                                {MC_CELLSs},
                                },
                                EGRs,
                                },
        /* bcmBstStatIdQueueTotal */ {{NULL,
                                NULL,
                                NULL,
                                NULL,
                                {NULL},
                                0,
                                },
                                {NULL,
                                NULL,
                                NULL,
                                NULL,
                                {NULL},
                                },
                                NULL,
                                },
        /* bcmBstStatIdRQEQueue */ {{TM_BST_REPL_Q_PRI_QUEUE_THDs,
                                NULL,
                                REPL_Q_NUMs,
                                BUFFER_POOLs,
                                {CELLSs},
                                0x1fff,
                                },
                                {CTR_TM_BST_REPL_Q_PRI_QUEUEs,
                                NULL,
                                REPL_Q_NUMs,
                                BUFFER_POOLs,
                                {CELLSs},
                                },
                                REPL_Qs,
                                },
        /* bcmBstStatIdRQEPool */ {{TM_BST_REPL_Q_SERVICE_POOL_THDs,
                                NULL,
                                NULL,
                                BUFFER_POOLs,
                                {CELLSs},
                                0x1fff,
                                },
                                {CTR_TM_BST_REPL_Q_GLOBALs,
                                NULL,
                                NULL,
                                BUFFER_POOLs,
                                {CELLSs},
                                },
                                REPL_Qs,
                                },
    };

    alloc_size = sizeof(cosq_bst_handler_t);

    sal_memset(COSQ_BST_RESOURCE(unit, 0), 0, alloc_size * bcmBstStatIdMaxCount);

    for (bid = bcmBstStatIdDevice; bid <= bcmBstStatIdRQEPool; bid++) {
        sal_memcpy(COSQ_BST_RESOURCE(unit, bid), &bst_handler[bid], alloc_size);
    }
}

/*
 * \brief Compare profile set of port pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_profile_cmp_cb(int unit, void *entries,
                                      int entries_per_set,
                                      int index, int *cmp)
{
    uint32_t cells = 0;
    cosq_bst_handler_t *res_info;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdPortPool);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_entry_get(unit, res_info, index, &cells));

     if (cells == *(int*)(entries)) {
        *cmp = 0;
    } else {
        *cmp = 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of port pool uc profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_uc_profile_cmp_cb(int unit, void *entries,
                                         int entries_per_set,
                                         int index, int *cmp)
{
    uint32_t cells = 0;
    cosq_bst_handler_t *res_info;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedUcast);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_entry_get(unit, res_info, index, &cells));

     if (cells == *(int*)(entries)) {
        *cmp = 0;
    } else {
        *cmp = 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of port mc pool profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
cosq_bst_thd_port_pool_mc_profile_cmp_cb(int unit, void *entries,
                                         int entries_per_set,
                                         int index, int *cmp)
{
    uint32_t cells = 0;
    cosq_bst_handler_t *res_info;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedMcast);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_thd_port_pool_profile_entry_get(unit, res_info, index, &cells));

     if (cells == *(int*)(entries)) {
        *cmp = 0;
    } else {
        *cmp = 1;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [in]   cells        cells.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_bst_stat_id_t bid, uint32_t cells)
{
    bcm_port_t local_port;
    int index, start_index, end_index, idx;
    int itm, from_itm = 0, to_itm = 0;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    tm_ltsw_bst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    if (cells > res_info->thd_hdl.max_cells) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_index_resolve(unit, gport, cosq, bid, &local_port,
                                &start_index, &end_index));
    if (res_info->thd_hdl.key2 != NULL) {
        from_itm = 0;
        to_itm = 1;
    } else {
        from_itm = to_itm = 0;
    }

    for (index = start_index; index <= end_index; index++ ) {
        if (res_info->thd_hdl.profile_op != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (res_info->thd_hdl.profile_op(unit, res_info, local_port, index,
                                              LTSW_BST_OP_SET, &cells));
        } else {
            for (itm = from_itm; itm <= to_itm; itm++ ) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_threshold_field_op(unit, res_info, local_port, index,
                                                 itm, LTSW_BST_OP_SET, &cells));
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &idx));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "BST threshold set bid %d idx %d.\n"),
                  bid, idx));

        res_info->thd_hdl.p_threshold[0][idx] = cells;
        res_info->thd_hdl.p_threshold[1][idx] = cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  cells        cells.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_bst_stat_id_t bid, uint32_t *cells)
{
    bcm_port_t local_port;
    int index, start_index, end_index;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_index_resolve(unit, gport, cosq, bid, &local_port,
                                &start_index, &end_index));

    for (index = start_index; index <= end_index; index++) {
        if (res_info->thd_hdl.profile_op != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (res_info->thd_hdl.profile_op(unit, res_info, local_port, index,
                                             LTSW_BST_OP_GET, cells));
        } else {
            rv = cosq_bst_threshold_field_op(unit, res_info, local_port, index,
                                             0, LTSW_BST_OP_GET, cells);
            if (rv == SHR_E_NOT_FOUND) {
                *cells = res_info->thd_hdl.max_cells;
                rv = SHR_E_NONE;
            }
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the threshold for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   port         PORT ID.
 * \param [in]   index        index.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  cells        cells.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
*/
static int
cosq_bst_threshold_retrieve(int unit, int port, int index,
                            bcm_bst_stat_id_t bid, uint32_t *threshold)
{
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (res_info->thd_hdl.profile_op != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (res_info->thd_hdl.profile_op(unit, res_info, port, index,
                                         LTSW_BST_OP_GET, threshold));
    } else {
        rv = cosq_bst_threshold_field_op(unit, res_info, port, index,
                                         0, LTSW_BST_OP_GET, threshold);
        if (rv == SHR_E_NOT_FOUND) {
            *threshold = res_info->thd_hdl.max_cells;
            rv = SHR_E_NONE;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear the BST stat table.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   res_info     res info.
 * \param [in]   opcode       opcode.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_hw_clear(int unit, cosq_bst_handler_t *res_info,
                       bcm_bst_stat_id_t bid)
{
    const char *tbl_name;

    SHR_FUNC_ENTER(unit);

    tbl_name = res_info->stat_hdl.stat_lt;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, tbl_name));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the BST stat table through transaction.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   res_info     res info.
 * \param [in]   opcode       opcode.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_hw_init(int unit, cosq_bst_handler_t *res_info,
                      bcm_bst_stat_id_t bid)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    const char *tbl_name;
    const char *port_key = NULL;
    const char *index_key = NULL;
    const char *buffer_key = NULL;
    int port, max_port;
    bool cpu_valid = FALSE;
    bcmlt_field_def_t field_def;
    int index, max_index, itm, max_itm;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_cosq_device_info_t device_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    tbl_name = res_info->stat_hdl.stat_lt;
    port_key = res_info->stat_hdl.key;
    index_key = res_info->stat_hdl.key1;
    buffer_key = res_info->stat_hdl.key2;

    if (port_key != NULL) {
        if ((bid == bcmBstStatIdMcast) ||
            (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
            cpu_valid = TRUE;
        }
        max_port = bcmi_ltsw_dev_logic_port_num(unit);
    } else {
        max_port = 1;
    }

    max_index = 0;
    if (index_key != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, tbl_name,
                                   index_key, &field_def));
        max_index = field_def.max;
    }

    max_itm = 1;
    if (buffer_key != NULL) {
        max_itm = device_info.num_itm;
    }

    for (port = 0; port < max_port; port++) {
        if (is_cpu_port(unit, port) && !cpu_valid && (port_key != NULL)) {
            continue;
        }
        for (itm = 0; itm < max_itm; itm++) {
            for (index = 0; index <= max_index; index++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, tbl_name, &entry_hdl));

                if (port_key != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(entry_hdl, port_key,
                                               port));
                }

                if (index_key != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(entry_hdl, index_key,
                                               index));
                }

                if (buffer_key != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(entry_hdl, buffer_key,
                                               itm));
                }

                rv = bcmi_lt_entry_commit(unit, entry_hdl, BCMLT_OPCODE_INSERT,
                                          BCMLT_PRIORITY_NORMAL);
                if (rv == SHR_E_EXISTS) {
                    SHR_EXIT();
                }
                SHR_IF_ERR_VERBOSE_EXIT(rv);

                if (BCMLT_INVALID_HDL != entry_hdl) {
                    (void) bcmlt_entry_free(entry_hdl);
                }
            }
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Reset all stat counters for give bid.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_clear_all(int unit, cosq_bst_handler_t *res_info,
                        bcm_cosq_buffer_id_t buffer,
                        bcm_bst_stat_id_t bid)
{
    bcm_port_t local_port;
    int port;
    uint32_t port_type = 0;
    bcm_pbmp_t pbmp;
    int index, start_index, end_index, idx;
    int itm, from_itm, to_itm;
    uint32_t cells = 0;
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        switch (bid) {
            case bcmBstStatIdDevice:
            case bcmBstStatIdRQEPool:
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_stat_field_op(unit, res_info, -1, -1, itm,
                                            LTSW_BST_OP_SET, &cells));
                SHR_IF_ERR_VERBOSE_EXIT
                    (drv->cosq_bst_resolve_index(unit, bid, -1, -1, &idx));

                res_info->stat_hdl.p_stat[itm][idx] = cells;
                break;
            case bcmBstStatIdRQEQueue:
                for (index = 0; index < MMU_NUM_RQE_QUEUES; index++ ) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_bst_stat_field_op(unit, res_info, -1, index, itm,
                                                LTSW_BST_OP_SET, &cells));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (drv->cosq_bst_resolve_index(unit, bid, -1, index, &idx));

                    res_info->stat_hdl.p_stat[itm][idx] = cells;
                }
                break;
            case bcmBstStatIdEgrPool:
            case bcmBstStatIdEgrMCastPool:
            case bcmBstStatIdIngPool:
            case bcmBstStatIdHeadroomPool:
                for (index = 0; index < MMU_NUM_POOL; index++ ) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_bst_stat_field_op(unit, res_info, -1, index, itm,
                                                LTSW_BST_OP_SET, &cells));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (drv->cosq_bst_resolve_index(unit, bid, -1, index, &idx));

                    res_info->stat_hdl.p_stat[itm][idx] = cells;
                }
                break;
            case bcmBstStatIdMcast:
            /*
             * In the following line of code, Coverity thinks the
             * case for value bcmBstStatIdEgrPortPoolSharedMcast
             * is not terminated by a 'break' statement.
             * This is expected.
             */
            /* coverity[unterminated_case] */
            case bcmBstStatIdEgrPortPoolSharedMcast:
                port_type = BCMI_LTSW_PORT_TYPE_CPU;
            case bcmBstStatIdUcast:
            case bcmBstStatIdEgrPortPoolSharedUcast:
            case bcmBstStatIdPriGroupShared:
            case bcmBstStatIdPriGroupHeadroom:
            case bcmBstStatIdPortPool:
                port_type |= BCMI_LTSW_PORT_TYPE_PORT;

                BCM_PBMP_CLEAR(pbmp);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));

                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (drv->cosq_bst_index_resolve(unit, port, -1, bid, &local_port,
                                                     &start_index, &end_index));
                    if (res_info->stat_hdl.key2 == NULL) {
                        itm = BCM_COSQ_BUFFER_ID_INVALID;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
                        from_itm = to_itm = itm;
                    }
                    for (index = start_index; index <= end_index; index++ ) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (cosq_bst_stat_field_op(unit, res_info, local_port,
                                                    index, itm,
                                                    LTSW_BST_OP_SET, &cells));
                        SHR_IF_ERR_VERBOSE_EXIT
                            (drv->cosq_bst_resolve_index(unit, bid, local_port,
                                                         index, &idx));

                        res_info->stat_hdl.p_stat[itm][idx] = cells;
                    }
                }
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get the BST stat sw size.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 * \param [out]  size         size.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_sw_size_get(
    int unit,
    cosq_bst_handler_t *res_info,
    bcm_bst_stat_id_t bid,
    int *size)
{
    int alloc_size = 0;
    int local_port = 0;
    int index = 0;
    uint64_t idx_max, idx_min;
    tm_ltsw_bst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);

    if (res_info->stat_hdl.key) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, res_info->stat_hdl.stat_lt,
                                           res_info->stat_hdl.key,
                                           &idx_min, &idx_max));
        local_port = (int)idx_max;
    }

    if (res_info->stat_hdl.key1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, res_info->stat_hdl.stat_lt,
                                           res_info->stat_hdl.key1,
                                           &idx_min, &idx_max));
        index = (int)idx_max;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &alloc_size));

    *size = alloc_size + 1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the BST stat sw info.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_sw_init(int unit, cosq_bst_handler_t *res_info, bcm_bst_stat_id_t bid)
{
    int alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_stat_sw_size_get(unit, res_info, bid, &alloc_size));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST stat sw size bid %d alloc_size %d.\n"),
              bid, alloc_size));

    SHR_ALLOC(res_info->stat_hdl.p_stat[0], alloc_size * sizeof(uint32_t),
              "bstStat");

    SHR_NULL_CHECK(res_info->stat_hdl.p_stat[0], SHR_E_MEMORY);

    sal_memset(res_info->stat_hdl.p_stat[0], 0, alloc_size * sizeof(uint32_t));

    SHR_ALLOC(res_info->stat_hdl.p_stat[1], alloc_size * sizeof(uint32_t),
              "bstStat1");

    SHR_NULL_CHECK(res_info->stat_hdl.p_stat[1], SHR_E_MEMORY);

    sal_memset(res_info->stat_hdl.p_stat[1], 0, alloc_size * sizeof(uint32_t));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the BST stat table.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_init(int unit, bool sync_hw)
{
    bcm_bst_stat_id_t bid;
    cosq_bst_handler_t *res_info;

    SHR_FUNC_ENTER(unit);

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        res_info = COSQ_BST_RESOURCE(unit, bid);

        if (res_info->stat_hdl.stat_lt == NULL) {
            continue;
        }

        if (res_info->stat_hdl.p_stat[0]) {
            break;
        }

        if (sync_hw) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_stat_hw_init(unit, res_info, bid));
        }

        SHR_IF_ERR_EXIT
            (cosq_bst_stat_sw_init(unit, res_info, bid));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief get the BST threshold sw size.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 * \param [out]  size         size.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_sw_size_get(
    int unit,
    cosq_bst_handler_t *res_info,
    bcm_bst_stat_id_t bid,
    int *size)
{
    int alloc_size = 0;
    int local_port = 0;
    int index = 0;
    uint64_t idx_max, idx_min;
    tm_ltsw_bst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);

    if (res_info->thd_hdl.key) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, res_info->thd_hdl.thd_lt,
                                           res_info->thd_hdl.key,
                                           &idx_min, &idx_max));
        local_port = (int)idx_max;
    }

    if (res_info->thd_hdl.key1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, res_info->thd_hdl.thd_lt,
                                           res_info->thd_hdl.key1,
                                           &idx_min, &idx_max));
        index = (int)idx_max;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &alloc_size));

    *size = alloc_size + 1;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the BST threshold sw info.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_sw_init(int unit, cosq_bst_handler_t *res_info, bcm_bst_stat_id_t bid)
{
    int alloc_size = 0, i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_threshold_sw_size_get(unit, res_info, bid, &alloc_size));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST threshold sw size bid %d alloc_size %d.\n"),
              bid, alloc_size));

    SHR_ALLOC(res_info->thd_hdl.p_threshold[0], alloc_size * sizeof(uint32_t),
              "bstThreshold");

    SHR_NULL_CHECK(res_info->thd_hdl.p_threshold[0], SHR_E_MEMORY);

    sal_memset(res_info->thd_hdl.p_threshold[0], res_info->thd_hdl.max_cells,
               alloc_size * sizeof(uint32_t));

    SHR_ALLOC(res_info->thd_hdl.p_threshold[1], alloc_size * sizeof(uint32_t),
              "bstThreshold1");

    SHR_NULL_CHECK(res_info->thd_hdl.p_threshold[1], SHR_E_MEMORY);

    sal_memset(res_info->thd_hdl.p_threshold[1], res_info->thd_hdl.max_cells,
              alloc_size * sizeof(uint32_t));

    for (i = 0; i < alloc_size; i++) {
        res_info->thd_hdl.p_threshold[0][i] = res_info->thd_hdl.max_cells;
        res_info->thd_hdl.p_threshold[1][i] = res_info->thd_hdl.max_cells;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Sync speficied BST stat.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_sw_set(int unit, bcm_bst_stat_id_t bid, int port, int index)
{
    int idx;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    uint32_t threshold = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);

    SHR_NULL_CHECK(res_info->thd_hdl.thd_lt, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_resolve_index(unit, bid, port, index, &idx));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_threshold_retrieve(unit, port, index, bid, &threshold));

    res_info->thd_hdl.p_threshold[0][idx] = threshold;
    res_info->thd_hdl.p_threshold[1][idx] = threshold;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST threshold restore bid %d port %d cosq %d.\n"
                         "threshold %d res_info->thd_hdl.p_threshold[idx] %d"
                         " idx=%d\n"),
              bid, port, index, threshold,
              res_info->thd_hdl.p_threshold[0][idx], idx));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the BST threshold sw info.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_threshold_sw_restore(int unit, cosq_bst_handler_t *res_info, bcm_bst_stat_id_t bid)
{
    bcm_port_t local_port;
    int port;
    uint32_t port_type = 0;
    bcm_pbmp_t pbmp;
    int index, start_index, end_index;
    tm_ltsw_bst_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);
    SHR_NULL_CHECK(res_info->thd_hdl.thd_lt, SHR_E_PARAM);

    switch (bid) {
        case bcmBstStatIdDevice:
        case bcmBstStatIdRQEPool:
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_threshold_sw_set(unit, bid, -1, -1));
            break;
        case bcmBstStatIdRQEQueue:
            for (index = 0; index < MMU_NUM_RQE_QUEUES; index++ ) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_threshold_sw_set(unit, bid, -1, index));
            }
            break;
        case bcmBstStatIdEgrPool:
        case bcmBstStatIdEgrMCastPool:
        case bcmBstStatIdIngPool:
        case bcmBstStatIdHeadroomPool:
            for (index = 0; index < MMU_NUM_POOL; index++ ) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_threshold_sw_set(unit, bid, -1, index));
            }
            break;
        case bcmBstStatIdMcast:
        /*
         * In the following line of code, Coverity thinks the
         * case for value bcmBstStatIdEgrPortPoolSharedMcast
         * is not terminated by a 'break' statement.
         * This is expected.
         */
        /* coverity[unterminated_case] */
        case bcmBstStatIdEgrPortPoolSharedMcast:
            port_type = BCMI_LTSW_PORT_TYPE_CPU;
        case bcmBstStatIdUcast:
        case bcmBstStatIdEgrPortPoolSharedUcast:
        case bcmBstStatIdPriGroupShared:
        case bcmBstStatIdPriGroupHeadroom:
        case bcmBstStatIdPortPool:
            port_type |= BCMI_LTSW_PORT_TYPE_PORT;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));

            BCM_PBMP_ITER(pbmp, port) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (drv->cosq_bst_index_resolve(unit, port, -1, bid, &local_port,
                                            &start_index, &end_index));
                for (index = start_index; index <= end_index; index++ ) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_bst_threshold_sw_set(unit, bid, local_port,
                                                   index));
                }
            }
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear the BST threshold and profile and stat table.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_table_hw_clear(int unit)
{
    bcm_bst_stat_id_t bid;
    cosq_bst_handler_t *res_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, TM_BST_EVENT_STATE_CONTROLs));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, TM_BST_CONTROLs));

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        res_info = COSQ_BST_RESOURCE(unit, bid);

        if (res_info->stat_hdl.stat_lt == NULL) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_stat_hw_clear(unit, res_info, bid));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear the BST threshold and stat sw info.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_table_sw_clear(int unit)
{
    bcm_bst_stat_id_t bid;
    cosq_bst_handler_t *res_info;

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        res_info = COSQ_BST_RESOURCE(unit, bid);

        if (res_info->stat_hdl.stat_lt == NULL) {
            continue;
        }

        SHR_FREE(res_info->stat_hdl.p_stat[0]);

        SHR_FREE(res_info->stat_hdl.p_stat[1]);

        SHR_FREE(res_info->thd_hdl.p_threshold[0]);

        SHR_FREE(res_info->thd_hdl.p_threshold[1]);
    }

    return SHR_E_NONE;
}

/*
 * \brief Get the BST profile for CosQ objects.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  pvalue       BST stat count.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                  bcm_cosq_buffer_id_t buffer, bcm_bst_stat_id_t bid,
                  uint64_t *pvalue)
{
    bcm_port_t local_port;
    int itm = 0, from_itm, to_itm, idx;
    uint32_t temp_stat , max_stat;
    int index, start_index, end_index;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_index_resolve(unit, gport, cosq, bid, &local_port,
                                &start_index, &end_index));

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (res_info->stat_hdl.key2 == NULL) {
        itm = BCM_COSQ_BUFFER_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
        from_itm = to_itm = itm;
    }

    COMPILER_64_ZERO(*pvalue);

    if (res_info->stat_hdl.p_stat[0] == NULL) {
        SHR_EXIT();
    }

    /*
     * When a resource has more than one instance, find the max value
     * of all applicable instances and return the Max value.
     */
    for (index = start_index; index <= end_index; index++ ) {

        temp_stat = 0;
        max_stat = 0;

        for (itm = from_itm; itm <= to_itm; itm++ ) {
            SHR_IF_ERR_VERBOSE_EXIT
                (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &idx));

            temp_stat = res_info->stat_hdl.p_stat[itm][idx];
            if (temp_stat > max_stat) {
                max_stat = temp_stat;
            }
        }

        COMPILER_64_ADD_32(*pvalue, max_stat);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear speficied BST stat.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                    bcm_cosq_buffer_id_t buffer, bcm_bst_stat_id_t bid)
{
    bcm_port_t local_port;
    int itm = 0, from_itm = 0, to_itm = 0, idx;
    int index, start_index, end_index;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    uint32_t temp_stat = 0;
    int sync_val[MAX_ITM_PER_DEV] = {0};
    int sync = 0;
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));


    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    SHR_NULL_CHECK(res_info->stat_hdl.stat_lt, SHR_E_PARAM);

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (res_info->stat_hdl.p_stat[0] == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_pre_sync(unit, res_info->bst_module, sync_val));
    sync = 1;

    if (gport == -1) {
        /* Clear all stat counters for give bid */
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_stat_clear_all(unit, res_info, buffer, bid));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_bst_index_resolve(unit, gport, cosq, bid, &local_port,
                                    &start_index, &end_index));
        if (res_info->stat_hdl.key2 == NULL) {
            itm = BCM_COSQ_BUFFER_ID_INVALID;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
            from_itm = to_itm = itm;
        }
        /*
         * When a resource has more than one instance, clear all
         * applicable instances.
         */
        for (index = start_index; index <= end_index; index++ ) {
            for (itm = from_itm; itm <= to_itm; itm++ ) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_stat_field_op(unit, res_info, local_port, index,
                                            itm, LTSW_BST_OP_SET, &temp_stat));
                SHR_IF_ERR_VERBOSE_EXIT
                    (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &idx));

                res_info->stat_hdl.p_stat[itm][idx] = temp_stat;
            }
        }
    }

exit:
    if (sync == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_post_sync(unit, res_info->bst_module, sync_val));
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Clear speficied BST stat.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_sw_stat_clear(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_buffer_id_t buffer, bcm_bst_stat_id_t bid)
{
    bcm_port_t local_port;
    int itm = 0, from_itm, to_itm, idx;
    int index, start_index, end_index;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    uint32_t temp_stat = 0;
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    SHR_NULL_CHECK(res_info->stat_hdl.stat_lt, SHR_E_PARAM);

    if (buffer == BCM_COSQ_BUFFER_ID_INVALID) {
        from_itm = 0;
        to_itm = device_info.num_itm - 1;
    } else if ((buffer >=  0) && (buffer <  device_info.num_itm)) {
        from_itm = to_itm = buffer;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (res_info->stat_hdl.p_stat[0] == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_index_resolve(unit, gport, cosq, bid, &local_port,
                                &start_index, &end_index));

    if (res_info->stat_hdl.key2 == NULL) {
        itm = BCM_COSQ_BUFFER_ID_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
        from_itm = to_itm = itm;
    }

    /*
     * When a resource has more than one instance, clear all
     * applicable instances.
     */
    for (index = start_index; index <= end_index; index++ ) {
        for (itm = from_itm; itm <= to_itm; itm++ ) {
            SHR_IF_ERR_VERBOSE_EXIT
                (drv->cosq_bst_resolve_index(unit, bid, local_port, index, &idx));

            res_info->stat_hdl.p_stat[itm][idx] = temp_stat;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Retrieve gport/cosq from BST SW index for a resource.
 *        Used for decoding BST trigger information for notifying users.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          BST stat id.
 * \param [in]   port         Unused (retained to maintain consistency).
 * \param [in]   index        SW index for which stat > 0.
 * \param [in]   gport        gport corresponding to input VOQ.
 * \param [in]   cosq         cosq corresponding to input VOQ.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_map_resource_to_gport_cos(int unit, bcm_bst_stat_id_t bid, int port,
                                   int index, int buffer,
                                   bcm_gport_t *gport, bcm_cos_t *cosq)
{
    bcmi_ltsw_cosq_port_map_info_t port_info;

    SHR_FUNC_ENTER(unit);

    switch (bid) {
        case bcmBstStatIdDevice:
            *gport = -1;
            *cosq = BCM_COS_INVALID;
            break;
        case bcmBstStatIdRQEPool:
            *gport = -1;
            *cosq = buffer;
            break;
        case bcmBstStatIdIngPool:
        case bcmBstStatIdEgrPool:
        case bcmBstStatIdHeadroomPool:
        case bcmBstStatIdEgrMCastPool:
        case bcmBstStatIdRQEQueue:
            *gport = -1;
            *cosq = index;
            break;
        case bcmBstStatIdPortPool:
        case bcmBstStatIdPriGroupHeadroom:
        case bcmBstStatIdPriGroupShared:
        case bcmBstStatIdEgrPortPoolSharedUcast:
        case bcmBstStatIdEgrPortPoolSharedMcast:
        case bcmBstStatIdUcast:
            *gport = port;
            *cosq = index;
            break;
        case bcmBstStatIdMcast:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_port_map_info_get(unit, port,
                                                  &port_info));
            *gport = port;
            *cosq = index + port_info.num_uc_q;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Sync speficied BST stat.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   buffer       buffer id.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_sync(int unit, bcm_bst_stat_id_t bid, int port,
                   int index, int buffer)
{
    int hwm_mode;
    uint32_t temp_stat;
    int idx;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    cosq_bst_info_t *bst_info = COSQ_BST_INFO(unit);
    bcm_gport_t gport = BCM_GPORT_INVALID;
    bcm_cos_queue_t cosq = BCM_COS_INVALID;
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    uint32_t threshold = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_resolve_index, SHR_E_INIT);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST stat sync bid %d port %d cosq %d itm %d.\n"),
              bid, port, index, buffer));

    SHR_NULL_CHECK(res_info->thd_hdl.thd_lt, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_stat_field_op(unit, res_info, port, index, buffer,
                                LTSW_BST_OP_GET, &temp_stat));

    SHR_IF_ERR_VERBOSE_EXIT
        (drv->cosq_bst_resolve_index(unit, bid, port, index, &idx));

    hwm_mode = bst_info->track_mode;
    if (hwm_mode) {
        /* BST tracking is in Peak mode.
         * Hence store the watermark value only.
         */
        if (res_info->stat_hdl.p_stat[buffer][idx] < temp_stat) {
            res_info->stat_hdl.p_stat[buffer][idx] = temp_stat;
        }
    } else {
        res_info->stat_hdl.p_stat[buffer][idx] = temp_stat;
    }

    threshold = res_info->thd_hdl.p_threshold[buffer][idx];

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST stat sync bid %d port %d cosq %d itm %d.\n"
                         "threshold %d res_info->stat_hdl.p_stat[buffer][idx] %d"
                         " idx=%d\n"),
              bid, port, index, buffer, threshold,
              res_info->stat_hdl.p_stat[buffer][idx], idx));

    if (res_info->stat_hdl.p_stat[buffer][idx] >= threshold) {

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_map_resource_to_gport_cos(unit, bid, port, index, buffer,
                                                &gport, &cosq));
        /* Generate EVENT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_switch_event_generate(unit, BCM_SWITCH_EVENT_MMU_BST_TRIGGER,
                                             bid, gport, cosq));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief SW snapshot and sync.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          bcm_bst_stat_id_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_stat_sync_snapshot(int unit, bcm_bst_stat_id_t bid)
{
    bcm_port_t local_port;
    int port;
    uint32_t port_type = 0;
    bcm_pbmp_t pbmp;
    int index, start_index, end_index;
    int itm, from_itm, to_itm;
    int sync_val[MAX_ITM_PER_DEV] = {0};
    int sync = 0;
    cosq_bst_handler_t *res_info = COSQ_BST_RESOURCE(unit, bid);
    tm_ltsw_bst_chip_driver_t *drv = NULL;
    bcmi_ltsw_cosq_device_info_t device_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));


    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_bst_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_bst_index_resolve, SHR_E_INIT);

    SHR_NULL_CHECK(res_info->thd_hdl.thd_lt, SHR_E_PARAM);

    if (res_info->stat_hdl.p_stat[0] == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_pre_sync(unit, res_info->bst_module, sync_val));
    sync = 1;
    from_itm = 0;
    to_itm = device_info.num_itm - 1;

    for (itm = from_itm; itm <= to_itm; itm++ ) {
        switch (bid) {
            case bcmBstStatIdDevice:
            case bcmBstStatIdRQEPool:
                SHR_IF_ERR_VERBOSE_EXIT
                    (cosq_bst_stat_sync(unit, bid, -1, -1, itm));
                break;
            case bcmBstStatIdRQEQueue:
                for (index = 0; index < MMU_NUM_RQE_QUEUES; index++ ) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_bst_stat_sync(unit, bid, -1, index, itm));
                }
                break;
            case bcmBstStatIdEgrPool:
            case bcmBstStatIdEgrMCastPool:
            case bcmBstStatIdIngPool:
            case bcmBstStatIdHeadroomPool:
                for (index = 0; index < MMU_NUM_POOL; index++ ) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (cosq_bst_stat_sync(unit, bid, -1, index, itm));
                }
                break;
            case bcmBstStatIdMcast:
            /*
             * In the following line of code, Coverity thinks the
             * case for value bcmBstStatIdEgrPortPoolSharedMcast
             * is not terminated by a 'break' statement.
             * This is expected.
             */
            /* coverity[unterminated_case] */
            case bcmBstStatIdEgrPortPoolSharedMcast:
                port_type = BCMI_LTSW_PORT_TYPE_CPU;
            case bcmBstStatIdUcast:
            case bcmBstStatIdEgrPortPoolSharedUcast:
            case bcmBstStatIdPriGroupShared:
            case bcmBstStatIdPriGroupHeadroom:
            case bcmBstStatIdPortPool:
                port_type |= BCMI_LTSW_PORT_TYPE_PORT;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));

                BCM_PBMP_ITER(pbmp, port) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (drv->cosq_bst_index_resolve(unit, port, -1, bid, &local_port,
                                                &start_index, &end_index));
                    if (res_info->stat_hdl.key2 == NULL) {
                        itm = BCM_COSQ_BUFFER_ID_INVALID;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_ltsw_cosq_port_itm_validate(unit, local_port, &itm));
                        from_itm = to_itm = itm;
                    }
                    for (index = start_index; index <= end_index; index++ ) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (cosq_bst_stat_sync(unit, bid, local_port,
                                                index, itm));
                    }
                }
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    if (sync == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_post_sync(unit, res_info->bst_module, sync_val));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief The callback function to handle bst event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_bst_event_ingress(int unit, int itm)
{
    int rv = SHR_E_NONE;
    bcm_bst_stat_id_t bid;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t value = 0;
    cosq_bst_res_index_t res_index[bcmBstStatIdMaxCount] = {{-1}};
    int res_ct = 0;
    int bid_detect[bcmBstStatIdMaxCount] = {0};
    int dunit;
    int pipe;
    bcmi_ltsw_cosq_device_info_t device_info;

    rv = bcmi_ltsw_cosq_device_info_get(unit, &device_info);
    if (SHR_FAILURE(rv)) {
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING.\n"),
                 unit));

    dunit = bcmi_ltsw_dev_dunit(unit);

    for (pipe = 0; pipe < device_info.num_pipe; pipe++ ) {
        rv = bcmlt_entry_allocate(dunit, TM_BST_EVENT_SOURCE_INGs, &entry_hdl);
        if (SHR_FAILURE(rv)) {
            return;
        }

        rv = bcmlt_entry_field_add(entry_hdl, TM_PIPEs, pipe);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        rv = bcmi_lt_entry_commit(unit, entry_hdl,
                                  BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Failed to read bst event source ing table rv %d.\n"),
                                 rv));

            bcmlt_entry_free(entry_hdl);
            return;
        }

        rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOLs, &val);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        if (val) {
            rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOL_INDEXs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdIngPool].port = -1;
            res_index[bcmBstStatIdIngPool].index = value;

            bid_detect[bcmBstStatIdIngPool] = 1;
            res_ct++;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING itm = %d.\n"
                            "bid = bcmBstStatIdIngPool port=%d pg = %d.\n"),
                 unit, itm, res_index[bcmBstStatIdIngPool].port,
                 res_index[bcmBstStatIdIngPool].index));
        }

        rv = bcmlt_entry_field_get(entry_hdl, HEADROOM_POOLs, &val);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        if (val) {
            rv = bcmlt_entry_field_get(entry_hdl, HEADROOM_POOL_INDEXs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdHeadroomPool].port = -1;
            res_index[bcmBstStatIdHeadroomPool].index = value;

            bid_detect[bcmBstStatIdHeadroomPool] = 1;
            res_ct++;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING itm = %d.\n"
                            "bid = bcmBstStatIdHeadroomPool port=%d pg = %d.\n"),
                 unit, itm, res_index[bcmBstStatIdHeadroomPool].port,
                 res_index[bcmBstStatIdHeadroomPool].index));
        }

        rv = bcmlt_entry_field_get(entry_hdl, PRI_GRPs, &val);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        if (val) {
            rv = bcmlt_entry_field_get(entry_hdl, PRI_GRP_PORTs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPriGroupShared].port = value;

            rv = bcmlt_entry_field_get(entry_hdl, PRI_GRP_INDEXs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPriGroupShared].index = value;

            bid_detect[bcmBstStatIdPriGroupShared] = 1;
            res_ct++;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING itm = %d.\n"
                            "bid = bcmBstStatIdPriGroupShared port=%d pg = %d.\n"),
                 unit, itm, res_index[bcmBstStatIdPriGroupShared].port,
                 res_index[bcmBstStatIdPriGroupShared].index));
        }

        rv = bcmlt_entry_field_get(entry_hdl, PRI_GRP_HEADROOMs, &val);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        if (val) {
            rv = bcmlt_entry_field_get(entry_hdl, PRI_GRP_HEADROOM_PORTs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPriGroupHeadroom].port = value;

            rv = bcmlt_entry_field_get(entry_hdl, PRI_GRP_HEADROOM_INDEXs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPriGroupHeadroom].index = value;

            bid_detect[bcmBstStatIdPriGroupHeadroom] = 1;
            res_ct++;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING itm = %d.\n"
                            "bid = bcmBstStatIdPriGroupHeadroom port=%d pg = %d.\n"),
                 unit, itm, res_index[bcmBstStatIdPriGroupHeadroom].port,
                 res_index[bcmBstStatIdPriGroupHeadroom].index));
        }

        rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOLs, &val);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }

        if (val) {
            rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_PORTs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPortPool].port = value;

            rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_INDEXs, &value);
            if (SHR_FAILURE(rv)) {
                bcmlt_entry_free(entry_hdl);
                return;
            }
            res_index[bcmBstStatIdPortPool].index = value;

            bid_detect[bcmBstStatIdPortPool] = 1;
            res_ct++;
            LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=ING itm = %d.\n"
                            "bid = bcmBstStatIdPortPool port=%d pg = %d.\n"),
                 unit, itm, res_index[bcmBstStatIdPortPool].port,
                 res_index[bcmBstStatIdPortPool].index));
        }

        bcmlt_entry_free(entry_hdl);
    }

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        if (bid_detect[bid] == 0) {
            continue;
        }

        rv = cosq_bst_stat_sync(unit, bid, res_index[bid].port,
                                res_index[bid].index, itm);
    }

    return;
}

/*!
 * \brief The callback function to handle bst event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_bst_event_egress(int unit, int itm)
{
    int rv = SHR_E_NONE;
    bcm_bst_stat_id_t bid;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t value = 0;
    cosq_bst_res_index_t res_index[bcmBstStatIdMaxCount] = {{-1}};
    int res_ct = 0;
    int bid_detect[bcmBstStatIdMaxCount] = {0};
    int dunit;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=EGR itm = %d.\n"),
                 unit, itm));

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_entry_allocate(dunit, TM_BST_EVENT_SOURCE_EGRs, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        return;
    }

    rv = bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to read bst event source egr table rv %d.\n"),
                             rv));

        bcmlt_entry_free(entry_hdl);
        return;
    }

    rv = bcmlt_entry_field_get(entry_hdl, UC_Qs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, UC_Q_PORTs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdUcast].port = value;

        rv = bcmlt_entry_field_get(entry_hdl, TM_UC_Q_IDs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdUcast].index = value;

        bid_detect[bcmBstStatIdUcast] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdUcast port=%d queue = %d.\n"),
             unit, itm, res_index[bcmBstStatIdUcast].port,
             res_index[bcmBstStatIdUcast].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, MC_Qs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, MC_Q_PORTs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdMcast].port = value;

        rv = bcmlt_entry_field_get(entry_hdl, TM_MC_Q_IDs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdMcast].index = value;

        bid_detect[bcmBstStatIdMcast] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdMcast port=%d queue = %d.\n"),
             unit, itm, res_index[bcmBstStatIdMcast].port,
             res_index[bcmBstStatIdMcast].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOL_UCs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOL_UC_INDEXs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrPool].port = -1;
        res_index[bcmBstStatIdEgrPool].index = value;

        bid_detect[bcmBstStatIdEgrPool] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdEgrPool pool = %d.\n"),
             unit, itm, res_index[bcmBstStatIdEgrPool].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOL_MCs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOL_MC_INDEXs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrMCastPool].port = -1;
        res_index[bcmBstStatIdEgrMCastPool].index = value;

        bid_detect[bcmBstStatIdEgrMCastPool] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdEgrMCastPool pool = %d.\n"),
             unit, itm, res_index[bcmBstStatIdEgrMCastPool].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_UCs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_UC_PORTs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrPortPoolSharedUcast].port = value;

        rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_UC_INDEXs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrPortPoolSharedUcast].index = value;

        bid_detect[bcmBstStatIdEgrPortPoolSharedUcast] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdEgrPortPoolSharedUcast port=%d queue = %d.\n"),
             unit, itm, res_index[bcmBstStatIdEgrPortPoolSharedUcast].port,
             res_index[bcmBstStatIdEgrPortPoolSharedUcast].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_MCs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_MC_PORTs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrPortPoolSharedMcast].port = value;

        rv = bcmlt_entry_field_get(entry_hdl, PORT_SERVICE_POOL_MC_INDEXs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdEgrPortPoolSharedMcast].index = value;

        bid_detect[bcmBstStatIdEgrPortPoolSharedMcast] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=EGR itm = %d.\n"
                        "bid = bcmBstStatIdEgrPortPoolSharedMcast port=%d queue = %d.\n"),
             unit, itm, res_index[bcmBstStatIdEgrPortPoolSharedMcast].port,
             res_index[bcmBstStatIdEgrPortPoolSharedMcast].index));
    }

    bcmlt_entry_free(entry_hdl);

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        if (bid_detect[bid] == 0) {
            continue;
        }

        rv = cosq_bst_stat_sync(unit, bid, res_index[bid].port,
                                res_index[bid].index, itm);
    }

    return;
}

/*!
 * \brief The callback function to handle bst event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_bst_event_repl_queue(int unit, int itm)
{
    int rv = SHR_E_NONE;
    bcm_bst_stat_id_t bid;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val = 0;
    uint64_t value = 0;
    cosq_bst_res_index_t res_index[bcmBstStatIdMaxCount] = {{-1}};
    int res_ct = 0;
    int bid_detect[bcmBstStatIdMaxCount] = {0};
    int dunit;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=REPL.\n"),
                 unit));

    dunit = bcmi_ltsw_dev_dunit(unit);
    rv = bcmlt_entry_allocate(dunit, TM_BST_EVENT_SOURCE_REPL_Qs, &entry_hdl);
    if (SHR_FAILURE(rv)) {
        return;
    }

    rv = bcmlt_entry_field_add(entry_hdl, BUFFER_POOLs, itm);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    rv = bcmi_lt_entry_commit(unit, entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to read bst event source repl table rv %d.\n"),
                             rv));

        bcmlt_entry_free(entry_hdl);
        return;
    }

    rv = bcmlt_entry_field_get(entry_hdl, PRI_Qs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        rv = bcmlt_entry_field_get(entry_hdl, REPL_Q_NUMs, &value);
        if (SHR_FAILURE(rv)) {
            bcmlt_entry_free(entry_hdl);
            return;
        }
        res_index[bcmBstStatIdRQEQueue].port = -1;
        res_index[bcmBstStatIdRQEQueue].index = value;

        bid_detect[bcmBstStatIdRQEQueue] = 1;
        res_ct++;
        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                        "BST interrupt handler: unit=%d class=REPL itm = %d.\n"
                        "bid = bcmBstStatIdRQEQueue port=%d pg = %d.\n"),
             unit, itm, res_index[bcmBstStatIdRQEQueue].port,
             res_index[bcmBstStatIdRQEQueue].index));
    }

    rv = bcmlt_entry_field_get(entry_hdl, SERVICE_POOLs, &val);
    if (SHR_FAILURE(rv)) {
        bcmlt_entry_free(entry_hdl);
        return;
    }

    if (val) {
        res_index[bcmBstStatIdRQEPool].port = -1;
        res_index[bcmBstStatIdRQEPool].index = -1;
        bid_detect[bcmBstStatIdRQEPool] = 1;
        res_ct++;
    }

    bcmlt_entry_free(entry_hdl);

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        if (bid_detect[bid] == 0) {
            continue;
        }

        rv = cosq_bst_stat_sync(unit, bid, res_index[bid].port,
                                res_index[bid].index, itm);
    }

    return;
}

/*!
 * \brief The callback function to handle bst event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_bst_event_cfap(int unit, int itm)
{
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST interrupt handler: unit=%d class=DEVICE.\n"),
                 unit));

    (void)cosq_bst_stat_sync(unit, bcmBstStatIdDevice, -1, -1, itm);

    return;
}

/*!
 * \brief The callback function to handle bst event notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   bst event info.
 * \param [in]   user_data    User data.
 */
static void
cosq_bst_notification_cb(int unit, const char *event, void *notif_info,
                         void *user_data)
{
    cosq_bst_event_notif_t *bst_event = (cosq_bst_event_notif_t *)notif_info;
    int itm = 0;

    if (bst_event == NULL) {
        return;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "BST notification: unit=%d state =%s class=%s.\n"),
                 unit, bst_event->state, bst_event->event_class));

    if (!sal_strcmp(bst_event->state, TRIGGEREDs)) {
        itm = bst_event->buffer_pool;

        COSQ_BST_LOCK(unit);
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "BST notification: unit=%d class=%s itm = %d.\n"),
                     unit, bst_event->event_class, itm));

        if (!sal_strcmp(bst_event->event_class, INGs)) {
            cosq_bst_event_ingress(unit, itm);
        } else if (!sal_strcmp(bst_event->event_class, EGRs)) {
            cosq_bst_event_egress(unit, itm);
        } else if (!sal_strcmp(bst_event->event_class, REPL_Qs)) {
            cosq_bst_event_repl_queue(unit, itm);
        } else if (!sal_strcmp(bst_event->event_class, DEVICEs)) {
            cosq_bst_event_cfap(unit, itm);
        }

        COSQ_BST_UNLOCK(unit);
    }

    return;
}

/*!
 * \brief Parse bst event table entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       Table name.
 * \param [in]  eh            LT entry handle.
 * \param [out] notif_info    Pointer to notification info.
 * \param [out] status        Parser callback returned status, NULL for not care.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static void
cosq_bst_event_parse(int unit, const char *lt_name,
                     uint32_t entry_hdl, void *notif_info,
                     bcmi_ltsw_event_status_t *status)
{
    bcmlt_entry_handle_t eh = entry_hdl;
    cosq_bst_event_notif_t *bst_event = (cosq_bst_event_notif_t *)notif_info;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } fval;
    int rv;

    rv = bcmlt_entry_field_get(eh, BUFFER_POOLs, &(fval.val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, BUFFER_POOLs, rv));
        return;
    }
    bst_event->buffer_pool = fval.val;

    rv = bcmlt_entry_field_symbol_get(eh, CLASSs, &(fval.sym_val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, CLASSs, rv));
        return;
    }
    bst_event->event_class = fval.sym_val;

    rv = bcmlt_entry_field_symbol_get(eh, STATEs, &(fval.sym_val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, STATEs, rv));
        return;
    }
    bst_event->state = fval.sym_val;
}

/*!
 * \brief Subscribe for updates of TM_BST_EVENT_STATEs table.
 *
 * \param [in] unit Unit number.
 * \param [in] cb Callback to be invoked.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
cosq_bst_moniter_enable(int unit, bool enable)
{
    SHR_FUNC_ENTER(unit);

    if (enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_parser_set(unit, TM_BST_EVENT_STATEs,
                                      cosq_bst_event_parse,
                                      sizeof(cosq_bst_event_notif_t)));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_attrib_set(unit, TM_BST_EVENT_STATEs,
                                      BCMI_LTSW_EVENT_HIGH_PRIORITY));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_subscribe(unit, TM_BST_EVENT_STATEs,
                                     cosq_bst_notification_cb, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_unsubscribe(unit, TM_BST_EVENT_STATEs,
                                       cosq_bst_notification_cb));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief restore bst info during warmboot.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_info_restore(int unit)
{
    int i;
    int enable;
    cosq_bst_info_t *bst_info;
    const char *module[] = {EGRs, INGs, DEVICEs, REPL_Qs};

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_tracking_enable_get(unit, 0, DEVICEs,
                                      &enable));

    bst_info->bst_tracking_enable = enable;

    if (bst_info->bst_tracking_enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_moniter_enable(unit, TRUE));

        SHR_IF_ERR_EXIT
            (cosq_bst_stat_init(unit, FALSE));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_get(unit, DEVICEs, &enable, NULL, NULL));

    bst_info->track_mode = enable;

    bst_info->snapshot_mode = 0;

    for (i = 0; i < COUNTOF(module); i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_control_get(unit, module[i], NULL, NULL, &enable));

        if (enable) {
            bst_info->snapshot_mode |= (0x1 << i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init the max BST threshold value for each BST ID.
 *
 * \param [in]   unit         Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_max_thd_init(int unit)
{
    bcm_bst_stat_id_t bid;
    uint64_t idx_max, idx_min;
    cosq_bst_handler_t *res_info;
    const char *tbl_name;
    const char *fld_name;

    SHR_FUNC_ENTER(unit);

    for (bid = bcmBstStatIdDevice; bid < bcmBstStatIdMaxCount; bid++) {
        res_info = COSQ_BST_RESOURCE(unit, bid);

        if (res_info->thd_hdl.thd_lt == NULL) {
            continue;
        }

        if (res_info->thd_hdl.profile_lt == NULL) {
            tbl_name = res_info->thd_hdl.thd_lt;
            fld_name = res_info->thd_hdl.field.fld_name;
        } else {
            tbl_name = res_info->thd_hdl.profile_lt;
            fld_name = res_info->thd_hdl.profile_field;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, tbl_name,
                                           fld_name,
                                           &idx_min, &idx_max));
        res_info->thd_hdl.max_cells = (int)idx_max;

        SHR_IF_ERR_EXIT
            (cosq_bst_threshold_sw_init(unit, res_info, bid));

        if (bcmi_warmboot_get(unit)) {
            SHR_IF_ERR_EXIT
                (cosq_bst_threshold_sw_restore(unit, res_info, bid));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Validate input parameters for input BST.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_bst_bid_gport_cosq_param_check(int unit, bcm_gport_t gport,
                                    bcm_cos_queue_t cosq,
                                    bcm_bst_stat_id_t bid)
{
    bcmi_ltsw_cosq_port_map_info_t port_info;
    int port_num_ucq, port;
    cosq_bst_handler_t *res_info = NULL;

    SHR_FUNC_ENTER(unit);

    res_info = COSQ_BST_RESOURCE(unit, bid);

    SHR_NULL_CHECK(res_info->thd_hdl.thd_lt, SHR_E_PARAM);

    if (bid == bcmBstStatIdDevice ||
        bid == bcmBstStatIdRQEPool) {
        /* port,queue based param checks invalid for Device */
        SHR_EXIT();
    }

    if (cosq < -1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (bid == bcmBstStatIdRQEQueue) {
        if (cosq >= MMU_NUM_RQE_QUEUES) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        SHR_EXIT();
    }

    if ((bid == bcmBstStatIdHeadroomPool) ||
        (bid == bcmBstStatIdIngPool) ||
        (bid == bcmBstStatIdPortPool) ||
        (bid == bcmBstStatIdPriGroupShared) ||
        (bid == bcmBstStatIdPriGroupHeadroom)) {
        if (cosq >= MMU_NUM_PG) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        /* Passed cosq is multicast. Cannot be used for unicast Bids */
        if ((bid == bcmBstStatIdUcast) ||
            (bid == bcmBstStatIdEgrPool) ||
            (bid == bcmBstStatIdEgrPortPoolSharedUcast)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        /* Passed cosq is unicast. Cannot be used for multicast Bids */
        if ((bid == bcmBstStatIdMcast) ||
            (bid == bcmBstStatIdEgrMCastPool) ||
            (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        if (gport == -1) {
            SHR_EXIT();
        }
        /* GPORT is of local port type */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_localport_resolve(unit, gport, &port));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_cosq_port_map_info_get(unit, port, &port_info));

        if (cosq == BCM_COS_INVALID) {
            SHR_EXIT();
        }
        if (is_cpu_port(unit, port)) {
            port_num_ucq = 0;
            if (cosq >= NUM_CPU_QUEUES) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            port_num_ucq = port_info.num_uc_q;
            if (cosq >= NUM_GP_QUEUES) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
        if (cosq < port_num_ucq) {
            /* Passed cosq is unicast. Cannot be used for multicast Bids */
            if ((bid == bcmBstStatIdMcast) ||
                (bid == bcmBstStatIdEgrMCastPool) ||
                (bid == bcmBstStatIdEgrPortPoolSharedMcast)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            /* Passed cosq is multicast. Cannot be used for unicast Bids */
             if ((bid == bcmBstStatIdUcast) ||
                 (bid == bcmBstStatIdEgrPool) ||
                 (bid == bcmBstStatIdEgrPortPoolSharedUcast)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

int
tm_ltsw_bst_chip_driver_register(int unit, tm_ltsw_bst_chip_driver_t *drv)
{
    tm_bst_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_bst_chip_driver_deregister(int unit)
{
    tm_bst_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_bst_chip_driver_get(int unit, tm_ltsw_bst_chip_driver_t **drv)
{
    *drv = tm_bst_chip_driver_cb[unit];

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_bst_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile)
{
    uint32_t cells = 0;
    int bytes = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_bid_gport_cosq_param_check(unit, gport, cosq, bid));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_threshold_get(unit, gport, cosq, bid, &cells));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_cosq_thd_cell_to_byte(unit, cells, &bytes));

    profile->byte = bytes;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile)
{
    int cells = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_bid_gport_cosq_param_check(unit, gport, cosq, bid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_byte_to_cell(unit, profile->byte, &cells));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_threshold_set(unit, gport, cosq, bid, cells));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid,
    uint32_t options,
    uint64_t *pvalue)
{
    cosq_bst_handler_t *res_info;
    int track_mode, cor;
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_bid_gport_cosq_param_check(unit, gport, cosq, bid));

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_stat_get(unit, gport, cosq, buffer, bid, pvalue));

    res_info = COSQ_BST_RESOURCE(unit, bid);
    if (options & BCM_COSQ_STAT_CLEAR) {
        COSQ_BST_LOCK(unit);
        locked = TRUE;

        SHR_IF_ERR_VERBOSE_EXIT
           (cosq_bst_control_get(unit, res_info->bst_module,
                                 &track_mode, &cor, NULL));

        if (!(track_mode == 1 && cor == 1)) {
            SHR_IF_ERR_VERBOSE_EXIT
               (cosq_bst_stat_clear(unit, gport, cosq, buffer, bid));
        }

        SHR_IF_ERR_VERBOSE_EXIT
           (cosq_bst_sw_stat_clear(unit, gport, cosq, buffer, bid));
    }

exit:
    if (locked) {
        COSQ_BST_UNLOCK(unit);
    }

    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_stat_clear(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid)
{
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_bid_gport_cosq_param_check(unit, gport, cosq, bid));

    COSQ_BST_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_stat_clear(unit, gport, cosq, buffer, bid));
exit:
    if (locked) {
        COSQ_BST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_stat_sync(
    int unit,
    bcm_bst_stat_id_t bid)
{
    bool locked = FALSE;

    SHR_FUNC_ENTER(unit);

    COSQ_BST_LOCK(unit);
    locked = true;

    if ((bid < 0) || (bid >= bcmBstStatIdMaxCount)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (cosq_bst_stat_sync_snapshot(unit, bid));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "BST stat sync bid %d .\n"),
              bid));

exit:
    if (locked) {
        COSQ_BST_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_init(int unit)
{
    cosq_bst_handler_t *res_info;
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);
    bst_info->bst_tracking_enable = 0;

    cosq_bst_handler_init(unit);

    SHR_IF_ERR_EXIT
        (cosq_bst_max_thd_init(unit));

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdPortPool);
    SHR_IF_ERR_EXIT
        (cosq_bst_thd_port_pool_profile_init(unit, res_info));

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedUcast);
    SHR_IF_ERR_EXIT
        (cosq_bst_thd_port_pool_profile_init(unit, res_info));

    res_info = COSQ_BST_RESOURCE(unit, bcmBstStatIdEgrPortPoolSharedMcast);
    SHR_IF_ERR_EXIT
        (cosq_bst_thd_port_pool_profile_init(unit, res_info));

    SHR_IF_ERR_EXIT
        (cosq_bst_thd_pri_grp_profile_init(unit));

    SHR_IF_ERR_EXIT
        (cosq_bst_thd_queue_profile_init(unit));

    if (bst_info->mutex == NULL) {
        bst_info->mutex = sal_mutex_create("cosBstMutex");
        SHR_NULL_CHECK(bst_info->mutex, SHR_E_MEMORY);
    }

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT
            (cosq_bst_info_restore(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_detach(int unit)
{
    cosq_bst_info_t *bst_info;
    bst_info = COSQ_BST_INFO(unit);

    (void)cosq_bst_table_sw_clear(unit);

    if (bst_info->bst_tracking_enable) {
        (void)cosq_bst_moniter_enable(unit, FALSE);
    }

    if (!bcmi_warmboot_get(unit)) {
        (void)cosq_bst_table_hw_clear(unit);
    }

    if (bst_info->mutex) {
        sal_mutex_destroy(bst_info->mutex);
        bst_info->mutex = NULL;
    }

    sal_memset(bst_info, 0, sizeof(cosq_bst_info_t));

    return SHR_E_NONE;
}

int
tm_ltsw_cosq_bst_enable_set(
    int unit,
    int arg)
{
    int enable = 0;
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    enable = arg ? 1 : 0;

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_stat_init(unit, true));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_tracking_enable_set(unit, BCM_COSQ_BUFFER_ID_INVALID,
                                      DEVICEs, enable));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_tracking_enable_set(unit, BCM_COSQ_BUFFER_ID_INVALID,
                                      INGs, enable));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_tracking_enable_set(unit, BCM_COSQ_BUFFER_ID_INVALID,
                                      EGRs, enable));
    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_tracking_enable_set(unit, BCM_COSQ_BUFFER_ID_INVALID,
                                      REPL_Qs, enable));

    if (enable) {
        if (!bst_info->bst_tracking_enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_moniter_enable(unit, TRUE));
        }
    } else {
        if (bst_info->bst_tracking_enable) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_moniter_enable(unit, FALSE));
        }
    }

    bst_info->bst_tracking_enable = arg;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_enable_get(
    int unit,
    int *arg)
{
    int itm = 0, i;
    int enable = 0;
    bcmi_ltsw_cosq_device_info_t device_info;
    const char *module[] = {EGRs, INGs, DEVICEs, REPL_Qs};
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    *arg = bst_info->bst_tracking_enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    for (i = 0; i < COUNTOF(module); i++) {
        for (itm = 0; itm < device_info.num_itm; itm++ ) {
            SHR_IF_ERR_VERBOSE_EXIT
                (cosq_bst_event_state_get(unit, itm, module[i], &enable));
            if (enable != 1) {
                *arg = 0;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_tracking_mode_set(
    int unit,
    int arg)
{
    int hwm_mode = 0;
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    hwm_mode = arg ? 1 : 0;

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    bst_info->track_mode = arg;

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_set(unit, DEVICEs, hwm_mode, -1, -1));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_set(unit, INGs, hwm_mode, -1, -1));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_set(unit, EGRs, hwm_mode, -1, -1));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_bst_control_set(unit, REPL_Qs, hwm_mode, -1, -1));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_tracking_mode_get(
    int unit,
    int *arg)
{
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    *arg = bst_info->track_mode;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_snapshot_enable_set(
    int unit,
    int arg)
{
    int i, enable = 0;
    cosq_bst_info_t *bst_info;
    const char *module[] = {EGRs, INGs, DEVICEs, REPL_Qs};

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    bst_info->snapshot_mode = arg;

    for (i = 0; i < COUNTOF(module); i++) {
        enable = 0;
        if (arg & (0x1 << i)) {
            enable = 0x1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_bst_control_set(unit, module[i], -1, -1, enable));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_cosq_bst_snapshot_enable_get(
    int unit,
    int *arg)
{
    cosq_bst_info_t *bst_info;

    SHR_FUNC_ENTER(unit);

    bst_info = COSQ_BST_INFO(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_NULL_CHECK(bst_info, SHR_E_PARAM);

    *arg = bst_info->snapshot_mode;

exit:
    SHR_FUNC_EXIT();
}

