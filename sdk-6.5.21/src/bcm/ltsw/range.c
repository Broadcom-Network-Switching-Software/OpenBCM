/*! \file range.c
 *
 * Range Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/range.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/range_int.h>
#include <bcm_int/ltsw/mbcm/range.h>
#include <bcm_int/ltsw/dev.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_RANGE

/*!
 * \brief One Range control entry for each SOC device containing Range module
 * information for that device.
 */
static bcmi_ltsw_range_db_t bcmi_ltsw_range_db[BCM_MAX_NUM_UNITS];

/*! Range mutex lock. */
#define RANGE_LOCK(unit) \
    do { \
        if (bcmi_ltsw_range_db[unit].lock) { \
            sal_mutex_take(bcmi_ltsw_range_db[unit].lock, SAL_MUTEX_FOREVER); \
        } \
    } while(0)

/*! Range mutex unlock. */
#define RANGE_UNLOCK(unit) \
    do { \
        if (bcmi_ltsw_range_db[unit].lock) { \
            sal_mutex_give(bcmi_ltsw_range_db[unit].lock); \
        } \
    } while(0)

static int
bcmint_range_control_get(int unit, bcmint_range_control_info_t **range_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_ctrl, SHR_E_PARAM);
    *range_ctrl = &(bcmi_ltsw_range_db[unit].control_db);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_info_get(int unit, bcmint_range_info_t **range_info)
{
    bcmint_range_control_info_t *range_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_control_get(unit, &range_ctrl));

    SHR_NULL_CHECK(range_ctrl, SHR_E_INIT);

    *range_info = &range_ctrl->range_info;

    SHR_NULL_CHECK(*range_info, SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_oper_mode_status_get(int unit,
        bcmint_range_tbls_info_t *tbls_info,
        bcm_range_oper_mode_t *mode)
{
    int dunit = 0;
    uint64_t oper_mode = 0x0;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);
    SHR_NULL_CHECK(tbls_info->oper_mode_sid, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(tbls_info->oper_mode_fid, SHR_E_UNAVAIL);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->oper_mode_sid,
                              &oper_mode_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive operation mode field from fetched entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(oper_mode_template,
                               tbls_info->oper_mode_fid,
                               &oper_mode));

    *mode = (oper_mode == 0) ? bcmRangeOperModeGlobal : bcmRangeOperModePipeLocal;

exit:
    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_range_ports_set(int unit,
    bcmlt_entry_handle_t range_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_pbmp_t ports)
{
    uint8_t pipe;
    int num_pipe;
    bcm_range_oper_mode_t oper_mode = bcmRangeOperModeCount;
    bcm_pbmp_t pbmp;
    int rv = 0;
    bcm_port_config_t  port_config;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);

    BCM_PBMP_ASSIGN(pbmp, ports);

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    /* Retrieve operational mode status */
    rv = bcmint_range_oper_mode_status_get(unit, tbls_info, &oper_mode);
    if (SHR_E_UNAVAIL == rv) {
        /* Nothing to be done */
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (BCM_PBMP_EQ(pbmp, port_config.all)) {
        if (oper_mode == bcmRangeOperModePipeLocal) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else {
        if (oper_mode != bcmRangeOperModePipeLocal) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* per-pipe pbm */
        num_pipe = bcmi_ltsw_dev_max_pipe_num(unit);
        for (pipe = 0; pipe < num_pipe; pipe++) {
            if (BCM_PBMP_EQ(pbmp, port_config.per_pp_pipe[pipe])) {
                break;
            }
        }

        if (pipe == num_pipe) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(range_lt,
                                   "PIPE", pipe));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_ports_get(int unit,
    bcmlt_entry_handle_t range_lt,
    bcmint_range_tbls_info_t *tbls_info,
    bcm_pbmp_t *ports)
{
    uint64_t pipe;
    bcm_range_oper_mode_t oper_mode = bcmRangeOperModeCount;
    int rv = 0;
    bcm_port_config_t  port_config;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);
    SHR_NULL_CHECK(ports, SHR_E_PARAM);

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    /* Retrieve operational mode status */
    rv = bcmint_range_oper_mode_status_get(unit, tbls_info, &oper_mode);
    if (SHR_E_UNAVAIL == rv) {
        /* Nothing to be done */
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    if (oper_mode == bcmRangeOperModeGlobal) {
        BCM_PBMP_ASSIGN(*ports, port_config.all);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(range_lt,
                                   "PIPE", &pipe));
        BCM_PBMP_ASSIGN(*ports, port_config.per_pipe[pipe]);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_create(int unit,
                      int flags,
                      bcm_range_config_t *range_config)
{
    uint64_t min_depth = 0;
    uint64_t max_depth = 0;
    int id = 0;
    int dunit = 0;
    bcmint_range_info_t *range_info = NULL;
    bcmint_range_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t range_check_lt = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    if (flags & BCM_RANGE_CREATE_WITH_ID) {
        if ((range_config->rid < min_depth) || (range_config->rid > max_depth)) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: Range ID : %d is not in range \n"), range_config->rid));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if (SHR_BITGET(range_info->range_id_bmp.w, range_config->rid)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    } else {
        for (id = min_depth; id <= max_depth; id++) {
            if (!SHR_BITGET(range_info->range_id_bmp.w, id)) {
                break;
            }
        }
        if (id > max_depth) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: All Range ID's are in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        range_config->rid = id;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE CHECK LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_sid,
                              &range_check_lt));

    /* Add PIPE if Pipe Local */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_ports_set(unit,
                                range_check_lt,
                                tbls_info,
                                range_config->ports));

    /* Add RANGE_CHECK_ID field to RANGE CHECK LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_check_lt,
                              tbls_info->range_key_fid,
                              range_config->rid));

    /* Add Max Value field to RANGE CHECK LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_check_lt,
                              tbls_info->range_max,
                              range_config->max));

    /* Add Min Value field to RANGE CHECK LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_check_lt,
                              tbls_info->range_min,
                              range_config->min));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_range_rtype_add(unit,
                                   range_check_lt,
                                   tbls_info,
                                   range_config));

    /* Insert RANGE CHECK LT with added config */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, range_check_lt,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

    SHR_BITSET(range_info->range_id_bmp.w, range_config->rid);

exit:
    if (BCMLT_INVALID_HDL != range_check_lt) {
        (void) bcmlt_entry_free(range_check_lt);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_range_oper_mode_get(
    int unit,
    bcm_range_oper_mode_t *mode)
{
    bcmint_range_info_t *range_info = NULL;
    bcmint_range_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_oper_mode_status_get(unit, tbls_info, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_range_oper_mode_get(
    int unit,
    bcm_range_oper_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_oper_mode_get(unit,
                                    mode));
exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * \brief bcm_ltsw_range_create
 *
 * Function to create RANGE checkers with given config
 *
 * Param: unit         - (IN) Unit number.
 *        flags        - (IN) Flags to Range allocator
 *        range_config - (IN) Range Config Structure
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *    !BCM_E_NONE      - Failure
 */
int
bcm_ltsw_range_create(int unit,
                      int flags,
                      bcm_range_config_t *range_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_create(unit, flags, range_config));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_range_config_get(int unit, bcm_range_config_t *range_config)
{
    int                            dunit = 0;
    uint64_t                       max_depth = 0, min_depth = 0;
    uint64_t                       range_id = 0;
    bcmint_range_info_t            *range_info = NULL;
    bcmint_range_tbls_info_t       *tbls_info = NULL;
    bcmlt_entry_handle_t           range_check_lt = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    if ((range_config->rid < min_depth) || (range_config->rid > max_depth)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!SHR_BITGET(range_info->range_id_bmp.w, range_config->rid)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE, (BSL_META_U(unit,
               "Error: Range ID : %d is not found\n"), range_config->rid));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE CHECK LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_sid,
                              &range_check_lt));

    /* Traverse All Range Checkers */
    while (SHR_E_NONE == (bcmi_lt_entry_commit(unit, range_check_lt,
                                               BCMLT_OPCODE_TRAVERSE,
                                               BCMLT_PRIORITY_NORMAL))) {
        /* Get Range Check ID */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(range_check_lt,
                                   tbls_info->range_key_fid,
                                   &range_id));
        if (range_id == range_config->rid)  {
            break;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_ports_get(unit,
                                range_check_lt,
                                tbls_info,
                                &(range_config->ports)));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_range_rtype_get(unit,
                                   range_check_lt,
                                   tbls_info,
                                   range_config));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(range_check_lt,
                                      tbls_info->range_min,
                                      (uint64_t *)&(range_config->min)));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(range_check_lt,
                                      tbls_info->range_max,
                                      (uint64_t *)&(range_config->max)));

exit:
    if (BCMLT_INVALID_HDL != range_check_lt) {
        (void) bcmlt_entry_free(range_check_lt);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief bcm_ltsw_range_get
 *
 * Function to get RANGE checkers config
 *
 * Param: unit         - (IN)  Unit number.
 *        range_config - (OUT) Range Config Structure
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *    !BCM_E_NONE      - Failure
 */
int
bcm_ltsw_range_get(int unit, bcm_range_config_t *range_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_config_get(unit, range_config));
exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_range_destroy(int unit, bcm_range_t range_id)
{
    int                            dunit = 0;
    uint64_t                       rid = 0;
    uint64_t                       max_depth = 0, min_depth = 0;
    bcmint_range_info_t            *range_info = NULL;
    bcmint_range_tbls_info_t       *tbls_info = NULL;
    bcmlt_entry_handle_t           range_check_lt = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    if ((range_id < min_depth) || (range_id > max_depth)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!SHR_BITGET(range_info->range_id_bmp.w, range_id)) {
        LOG_DEBUG(BSL_LS_BCM_RANGE, (BSL_META_U(unit,
                  "Error: Range ID : %d is not found\n"), range_id));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE CHECK LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_sid,
                              &range_check_lt));

    /* Traverse All Range Checkers */
    while (SHR_E_NONE == (bcmi_lt_entry_commit(unit, range_check_lt,
                                               BCMLT_OPCODE_TRAVERSE,
                                               BCMLT_PRIORITY_NORMAL))) {
        /* Get Range Check ID */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(range_check_lt,
                                   tbls_info->range_key_fid,
                                   &rid));
        if (range_id == rid)  {
            break;
        }
    }

    /* Destroy RANGE CHECK LT with ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, range_check_lt,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_BITCLR(range_info->range_id_bmp.w, range_id);

exit:
    if (BCMLT_INVALID_HDL != range_check_lt) {
        (void) bcmlt_entry_free(range_check_lt);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_range_destroy(int unit, bcm_range_t range_id)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_destroy(unit, range_id));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create protection mutex for Range module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_range_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_range_db[unit].lock == NULL) {
        bcmi_ltsw_range_db[unit].lock = sal_mutex_create("bcmFieldCntlMutex");
        SHR_NULL_CHECK(bcmi_ltsw_range_db[unit].lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for range module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_range_lock_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_range_db[unit].lock != NULL) {
        sal_mutex_destroy(bcmi_ltsw_range_db[unit].lock);
        bcmi_ltsw_range_db[unit].lock = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
bcmint_range_tbls_info_init(int unit,
                            bcmint_range_info_t *range_info)
{
    bcmint_range_tbls_info_t **tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_info, SHR_E_PARAM);

    tbls_info = &(range_info->tbls_info);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_range_tbls_info_init(unit,
                                        tbls_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_info_reinit(int unit,
        bcmint_range_tbls_info_t *tbls_info,
        bcmint_range_info_t *range_info)
{
    bcmlt_entry_handle_t           range_check_lt = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t           range_grp_lt = BCMLT_INVALID_HDL;
    uint64_t                       data = 0;
    int                            dunit = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbls_info, SHR_E_PARAM);
    SHR_NULL_CHECK(range_info, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE CHECK LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_sid,
                              &range_check_lt));

    /* Traverse All Range Checkers */
    while (SHR_E_NONE == (bcmi_lt_entry_commit(unit, range_check_lt,
                                               BCMLT_OPCODE_TRAVERSE,
                                               BCMLT_PRIORITY_NORMAL))) {
        /* Get Range Check ID */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(range_check_lt,
                                   tbls_info->range_key_fid,
                                   &data));
        /* Update Range Check ID bitmap in range_info */
        SHR_BITSET(range_info->range_id_bmp.w, data);
    }

    if (NULL != tbls_info->range_grp_sid) {
        /* Entry handle allocate for RANGE CHECK LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->range_grp_sid,
                                  &range_grp_lt));

        /* Traverse All Range Groups */
        while (SHR_E_NONE == (bcmi_lt_entry_commit(unit, range_grp_lt,
                        BCMLT_OPCODE_TRAVERSE,
                        BCMLT_PRIORITY_NORMAL))) {
            /* Get Range Group ID */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(range_grp_lt,
                                       tbls_info->range_key_fid,
                                       &data));
            /* Update Range Group ID bitmap in range_info */
            SHR_BITSET(range_info->range_grp_id_bmp.w, data);
        }
    }

exit:
    if (BCMLT_INVALID_HDL != range_check_lt) {
        (void) bcmlt_entry_free(range_check_lt);
    }
    if (BCMLT_INVALID_HDL != range_grp_lt) {
        (void) bcmlt_entry_free(range_grp_lt);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_range_info_init(int unit, bcmint_range_info_t *range_info)
{
    int warm = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_tbls_info_init(unit, range_info));

    warm = bcmi_warmboot_get(unit);
    if (warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_range_info_reinit(unit, range_info->tbls_info, range_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the range control information.
 *
 * This function initializes the range control information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_range_control_init(int unit)
{
    bcmint_range_control_info_t *range_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_control_get(unit, &range_ctrl));

    sal_memset(range_ctrl, 0, sizeof(bcmint_range_control_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_init(unit, &(range_ctrl->range_info)));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm_range_grp_id_generate(int unit,
        bcmint_range_tbls_info_t *tbls_info,
        bcmint_range_info_t *range_info,
        bcm_range_group_config_t *range_grp_config)
{
    uint64_t max_depth = 0, min_depth = 0;
    int id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_grp_config, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_grp_sid,
                                       tbls_info->range_grp_key_fid,
                                       &min_depth,
                                       &max_depth));

    if (range_grp_config->flags & BCM_RANGE_CREATE_WITH_ID) {
        if ((range_grp_config->range_group_id < min_depth) ||
            (range_grp_config->range_group_id > max_depth)) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: Range Group ID : %d is not in range \n"),
                range_grp_config->range_group_id));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if (SHR_BITGET(range_info->range_grp_id_bmp.w, range_grp_config->range_group_id)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    } else {
        for (id = min_depth; id <= max_depth; id++) {
            if (!SHR_BITGET(range_info->range_grp_id_bmp.w, id)) {
                break;
            }
        }
        if (id > max_depth) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: All Range ID's are in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        range_grp_config->range_group_id = id;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_group_set(int unit,
                          bool new,
                          bcm_range_group_config_t *range_grp_config)
{
    uint64_t min_depth = 0;
    uint64_t max_depth = 0;
    uint64_t range_bmp[_RANGE_ID_MAX] = {0};
    int id = 0;
    int dunit = 0;
    bcmint_range_info_t *range_info = NULL;
    bcmint_range_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t range_grp_lt = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_grp_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;

    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info->range_grp_sid, SHR_E_UNAVAIL);

    /* If "new" is set create a new Range Group else update */
    if (new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_range_grp_id_generate(unit,
                                       tbls_info,
                                       range_info,
                                       range_grp_config));
    } else {
        if (!SHR_BITGET(range_info->range_grp_id_bmp.w,
                                range_grp_config->range_group_id)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE GROUP LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_grp_sid,
                              &range_grp_lt));

    /* Add PIPE if Pipe Local */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_ports_set(unit,
                                range_grp_lt,
                                tbls_info,
                                range_grp_config->ports));

    /* Add RANGE_GROUP_ID field to RANGE GROUP LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_grp_lt,
                              tbls_info->range_grp_key_fid,
                              range_grp_config->range_group_id));

    /* Get depths of Range Checker IDs */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    for (id = min_depth; id <= max_depth; id++) {
        if (SHR_BITGET(range_grp_config->range_bmp.w, id)) {
            range_bmp[id] = TRUE;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(range_grp_lt,
                                     tbls_info->range_grp_key_type_fid,
                                     0,
                                     range_bmp,
                                     max_depth));

    if (new) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, range_grp_lt,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_BITSET(range_info->range_grp_id_bmp.w,
                            range_grp_config->range_group_id);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, range_grp_lt,
                                  BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (BCMLT_INVALID_HDL != range_grp_lt) {
        (void) bcmlt_entry_free(range_grp_lt);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_range_group_delete(int unit,
        bcm_range_group_config_t *range_grp_cfg)
{
    bcmint_range_info_t            *range_info = NULL;
    bcmint_range_tbls_info_t       *tbls_info = NULL;
    int                            dunit = 0;
    bcmlt_entry_handle_t           range_grp_lt = BCMLT_INVALID_HDL;


    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    if (!SHR_BITGET(range_info->range_grp_id_bmp.w,
                            range_grp_cfg->range_group_id)) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: Range Group ID : %d is not found\n"),
                range_grp_cfg->range_group_id));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    tbls_info = range_info->tbls_info;

    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info->range_grp_sid, SHR_E_UNAVAIL);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE GROUP LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_grp_sid,
                              &range_grp_lt));

    /* Add RANGE_GROUP_ID field to RANGE GROUP LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_grp_lt,
                              tbls_info->range_grp_key_fid,
                              range_grp_cfg->range_group_id));

    /* Destroy RANGE CHECK LT with ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, range_grp_lt,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_BITCLR(range_info->range_grp_id_bmp.w,
                        range_grp_cfg->range_group_id);

exit:
    if (BCMLT_INVALID_HDL != range_grp_lt) {
        (void) bcmlt_entry_free(range_grp_lt);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_range_group_config_get(int unit,
                bcm_range_group_config_t *range_grp_cfg)
{
    uint64_t min_depth = 0;
    uint64_t max_depth = 0;
    uint64_t range_bmp[_RANGE_ID_MAX] = {0};
    uint32_t num_elem = 0;
    bcmint_range_info_t            *range_info = NULL;
    bcmint_range_tbls_info_t       *tbls_info = NULL;
    int                            dunit = 0;
    int                            id = 0;
    bcmlt_entry_handle_t           range_grp_lt = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(range_grp_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    if (!SHR_BITGET(range_info->range_grp_id_bmp.w,
                           range_grp_cfg->range_group_id)) {
            LOG_DEBUG(BSL_LS_BCM_RANGE,
               (BSL_META_U(unit,
               "Error: Range Group ID : %d is not found\n"),
                range_grp_cfg->range_group_id));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    tbls_info = range_info->tbls_info;

    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info->range_grp_sid, SHR_E_UNAVAIL);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for RANGE GROUP LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->range_grp_sid,
                              &range_grp_lt));

    /* Add RANGE_GROUP_ID field to RANGE GROUP LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(range_grp_lt,
                              tbls_info->range_grp_key_fid,
                              range_grp_cfg->range_group_id));

    /* Lookup RANGE GROUP LT with ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, range_grp_lt,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_ports_get(unit,
                                range_grp_lt,
                                tbls_info,
                                &(range_grp_cfg->ports)));

    /* Get depths of Range Checker IDs */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(range_grp_lt,
                                     tbls_info->range_grp_key_type_fid,
                                     0, range_bmp,
                                     max_depth,
                                     &num_elem));

    for (id = min_depth; id <= num_elem; id++) {
        if (range_bmp[id]) {
            SHR_BITSET(range_grp_cfg->range_bmp.w, range_bmp[id]);
        }
    }

exit:
    if (BCMLT_INVALID_HDL != range_grp_lt) {
        (void) bcmlt_entry_free(range_grp_lt);
    }
    SHR_FUNC_EXIT();
}
/*
 * Current NPL Doesn't doesn't support Range Check Group.
 * But LT names are available.
 */
int
bcm_ltsw_range_group_create(
        int unit,
        bcm_range_group_config_t *range_group_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_group_set(unit, TRUE, range_group_config));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_range_group_delete(
    int unit, 
    bcm_range_group_config_t *range_group_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_group_delete(unit, range_group_config));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_range_group_update(
    int unit, 
    bcm_range_group_config_t *range_group_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_group_set(unit, FALSE, range_group_config));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_range_group_get(
    int unit, 
    bcm_range_group_config_t *range_group_config)
{
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_range_group_config_get(unit, range_group_config));
exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_range_control_detach(int unit)
{
    bcmint_range_control_info_t *range_ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_control_get(unit, &range_ctrl));

    sal_memset(range_ctrl, 0, sizeof(bcmint_range_control_info_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the range database.
 *
 * This function detach the range database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_range_db_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_control_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_clear(int unit)
{
    int warm = bcmi_warmboot_get(unit);
    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    /* Clear Range Check table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_range_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_range_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear range Logical tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_clear(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_db_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_range_detach(int unit)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);
    rv = (bcmint_range_detach(unit));
    RANGE_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_lock_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the range module.
 *
 * Initial range database data structure and
 * clear the range related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
bcmint_range_init(int unit)
{
    SHR_FUNC_ENTER(unit);
    RANGE_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_detach(unit));

    /* Range Module Database initialization. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_control_init(unit));

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_range_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_lock_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_range_traverse(
    int unit,
    bcm_range_traverse_cb callback,
    void *user_data)
{
    uint64_t                  max_depth = 0, min_depth = 0;
    bcmint_range_info_t       *range_info = NULL;
    bcmint_range_tbls_info_t  *tbls_info = NULL;
    int                       idx = 0, rv = 0;
    bcm_range_config_t        range_config;
    SHR_FUNC_ENTER(unit);

    RANGE_LOCK(unit);

    SHR_NULL_CHECK(callback, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_range_info_get(unit, &range_info));

    tbls_info = range_info->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       tbls_info->range_sid,
                                       tbls_info->range_key_fid,
                                       &min_depth,
                                       &max_depth));

    for (idx=min_depth; idx <= max_depth; idx++) {
       if(!SHR_BITGET(range_info->range_id_bmp.w, idx)) {
           continue;
       }
       memset (&range_config, 0, sizeof(range_config));
       range_config.rid = idx;
       SHR_IF_ERR_VERBOSE_EXIT(bcmint_range_config_get(unit, &range_config));
       rv = (callback)(unit, &range_config, user_data);
       if (SHR_FAILURE(rv)) {
           break;
       }
    }

exit:
    RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
