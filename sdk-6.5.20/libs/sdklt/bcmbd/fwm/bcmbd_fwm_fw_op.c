/*! \file bcmbd_fwm_fw_op.c
 *
 * Functions of firmware operation driver of Firmware Management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_util.h>
#include <bcmdrd_config.h>
#include <bcmbd/bcmbd_fwm.h>
#include "./bcmbd_fwm_internal.h"

#include <sal/sal_alloc.h>

/* Log source for this component. */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_FWM

/*******************************************************************************
 * Local definitions
 */
typedef enum default_op_idx_s{
    DEFAULT_OP_IDX_START = 0,
    DEFAULT_OP_IDX_STOP,
    DEFAULT_OP_IDX_RESET,
    DEFAULT_OP_IDX_COUNT
} default_op_idx_t;

#define DEFAULT_OP_START_NAME "start"
#define DEFAULT_OP_STOP_NAME  "stop"
#define DEFAULT_OP_RESET_NAME "reset"

#define DEFAULT_OP_START_HELP "Start the processor."
#define DEFAULT_OP_STOP_HELP  "Stop (or halt) the processor."
#define DEFAULT_OP_RESET_HELP "Reset the processor."

/* Firmware instance database. */
static bcmbd_fwm_fw_op_features_t *bcmbd_fwm_fw_op_db[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Internal functions
 */
/*
 * Get the processor name of a feature instance.
 *     [in] feature Feature type.
 *     [in] instance Feature instance number.
 *     [out] proc_name Processor name buffer.
 *     [in] size Size of the buffer.
 *
 * retval SHR_E_NONE No error.
 *        SHR_E_NOT_FOUND No info data found.
 */
static int
bcmbd_fwm_fw_op_proc_get(const char *feature, int instance,
                         char *proc_name, int size)
{
    int i;
    int unit = 0;
    int fw_inst_cnt;
    bcmbd_fwm_fwinst_t fw_inst;
    char feature_name[BCMBD_FWM_FWINST_FW_NAME_MAX];
    int match;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);
    SHR_NULL_CHECK(proc_name, SHR_E_PARAM);

    sal_snprintf(feature_name, BCMBD_FWM_FWINST_FW_NAME_MAX, "%s.%d",
                 feature, instance);

    /* Get the processor. */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_instance_count_get(unit, &fw_inst_cnt));
    match = -1;
    for (i = 0; i < fw_inst_cnt; i++) {
        SHR_IF_ERR_EXIT
            (bcmbd_fwm_fw_instance_get(unit, i, &fw_inst));

        if (!sal_strcasecmp(fw_inst.name, feature_name)) {
            sal_strlcpy(proc_name, fw_inst.processor, size);
            match = 1;
            break;
        }
    }

    if (match < 0) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Default function of processor start. If firmware feature code doesn't
 * register the customized processor start function, the function is applied.
 *     [in] unit Unit ID.
 *     [in] feature Feature type.
 *     [in] instance Feature instance number.
 *     [in] op Op name.
 *     [in] param Parameter of the op.
 *     [in] user_data User data.
 *
 * retval SHR_E_NONE No error.
 *        Other errors from proc drivers.
 */
static int
bcmbd_fwm_fw_op_default_start(int unit, const char *feature, int instance,
                              const char *op, int param, void *user_data)
{
    char proc_name[BCMBD_FWM_PROC_NAME_MAX];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_op_proc_get(feature, instance,
                                  proc_name, BCMBD_FWM_PROC_NAME_MAX));

    /* Call processor driver - start */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_control(unit, proc_name,
                                BCMBD_FWM_PROC_CONTROL_START));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Default function of processor stop. If firmware feature code doesn't
 * register the customized processor stop function, the function is applied.
 *     [in] unit Unit ID.
 *     [in] feature Feature type.
 *     [in] instance Feature instance number.
 *     [in] op Op name.
 *     [in] param Parameter of the op.
 *     [in] user_data User data.
 *
 * retval SHR_E_NONE No error.
 *        Other errors from proc drivers.
 */
static int
bcmbd_fwm_fw_op_default_stop(int unit, const char *feature, int instance,
                              const char *op, int param, void *user_data)
{
    char proc_name[BCMBD_FWM_PROC_NAME_MAX];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_op_proc_get(feature, instance,
                                  proc_name, BCMBD_FWM_PROC_NAME_MAX));

    /* Call processor driver - stop */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_control(unit, proc_name,
                                BCMBD_FWM_PROC_CONTROL_STOP));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Default function of processor reset. If firmware feature code doesn't
 * register the customized processor reset function, the function is applied.
 *     [in] unit Unit ID.
 *     [in] feature Feature type.
 *     [in] instance Feature instance number.
 *     [in] op Op name.
 *     [in] param Parameter of the op.
 *     [in] user_data User data.
 *
 * retval SHR_E_NONE No error.
 *        Other errors from proc drivers.
 */
static int
bcmbd_fwm_fw_op_default_reset(int unit, const char *feature, int instance,
                              const char *op, int param, void *user_data)
{
    char proc_name[BCMBD_FWM_PROC_NAME_MAX];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmbd_fwm_fw_op_proc_get(feature, instance,
                                  proc_name, BCMBD_FWM_PROC_NAME_MAX));

    /* Call processor driver - reset */
    SHR_IF_ERR_EXIT
        (bcmbd_fwm_proc_control(unit, proc_name,
                                BCMBD_FWM_PROC_CONTROL_RESET));

exit:
    SHR_FUNC_EXIT();
}

/* Initialize firmware operation driver. */
int
bcmbd_fwm_fw_op_init(int unit)
{
    int i;
    int j;
    bcmbd_fwm_fw_op_t *op;
    bcmbd_fwm_fw_op_features_t *fw_featues = NULL;
    bcmbd_fwm_fw_op_feature_t *feature;
    int idx = 0;

    SHR_FUNC_ENTER(unit);

    fw_featues = sal_alloc(sizeof(bcmbd_fwm_fw_op_features_t),
                           "bcmbdFwmOpFeatures");
    SHR_NULL_CHECK(fw_featues, SHR_E_MEMORY);

    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        sal_memset(&fw_featues->feature[i], 0,
                   sizeof(bcmbd_fwm_fw_op_feature_t));
        feature = &fw_featues->feature[i];
        feature->index = -1;

        op = feature->feature_op;

        /* Set default ops start, stop and reset for all entries. */
        idx = DEFAULT_OP_IDX_START;
        sal_snprintf(op[idx].op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                     DEFAULT_OP_START_NAME);
        sal_snprintf(op[idx].op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                     DEFAULT_OP_START_HELP);
        op[idx].op_cb = bcmbd_fwm_fw_op_default_start;

        idx = DEFAULT_OP_IDX_STOP;
        sal_snprintf(op[idx].op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                     DEFAULT_OP_STOP_NAME);
        sal_snprintf(op[idx].op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                     DEFAULT_OP_STOP_HELP);
        op[idx].op_cb = bcmbd_fwm_fw_op_default_stop;

        idx = DEFAULT_OP_IDX_RESET;
        sal_snprintf(op[idx].op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                     DEFAULT_OP_RESET_NAME);
        sal_snprintf(op[idx].op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                     DEFAULT_OP_RESET_HELP);
        op[idx].op_cb = bcmbd_fwm_fw_op_default_reset;

        /* Clean up the rest ops. */
        for (j = DEFAULT_OP_IDX_COUNT; j < BCMBD_FWM_FW_OP_NUM_MAX; j++) {
            sal_memset(op[j].op_name, 0,
                       sizeof(char) * BCMBD_FWM_FW_OP_NAME_MAX);
            sal_memset(op[j].op_help, 0,
                       sizeof(char) * BCMBD_FWM_FW_OP_HELP_MAX);
            op[j].op_cb = NULL;
        }
    }

    fw_featues->op_mutex = sal_mutex_create("bcmbdFwmOp");
    SHR_NULL_CHECK(fw_featues->op_mutex, SHR_E_MEMORY);

    bcmbd_fwm_fw_op_db[unit] = fw_featues;

exit:
    if (SHR_FUNC_ERR() && fw_featues) {
        if (fw_featues->op_mutex) {
            sal_mutex_destroy(fw_featues->op_mutex);
        }
        sal_free(fw_featues);
    }
    SHR_FUNC_EXIT();
}

/* Cleanup firmware operation driver. */
int
bcmbd_fwm_fw_op_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmbd_fwm_fw_op_db[unit]) {
        if (bcmbd_fwm_fw_op_db[unit]->op_mutex) {
            sal_mutex_destroy(bcmbd_fwm_fw_op_db[unit]->op_mutex);
        }
        sal_free(bcmbd_fwm_fw_op_db[unit]);
        bcmbd_fwm_fw_op_db[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

/* Register custom operation function. */
int
bcmbd_fwm_fw_op_register(int unit, const char *feature, bcmbd_fwm_fw_op_t *op)
{
    int empty_slot;
    int i;
    bcmbd_fwm_fw_op_t *op_entry;
    int match;
    int locked = 0;

    bcmbd_fwm_fw_op_features_t *fw_featues = NULL;
    bcmbd_fwm_fw_op_feature_t *op_set = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);
    SHR_NULL_CHECK(op, SHR_E_PARAM);

    fw_featues = bcmbd_fwm_fw_op_db[unit];
    SHR_NULL_CHECK(fw_featues, SHR_E_INIT);

    SHR_NULL_CHECK(fw_featues->op_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_featues->op_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Find the matched feature slot. */
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        bcmbd_fwm_fw_op_feature_t *op_tmp = &fw_featues->feature[i];
        if (!sal_strcasecmp(feature, op_tmp->feature_name)) {
            op_set = op_tmp;
            op_set->index = i;
            break;
        }
    }

    /* If not found, find an empty slot */
    if (op_set == NULL) {
        for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
            bcmbd_fwm_fw_op_feature_t *op_tmp = &fw_featues->feature[i];
            if (op_tmp->index < 0) {
                op_set = op_tmp;
                op_set->index = i;
                break;
            }
        }
    }

    /* Cannot register if there is no empty slot. */
    if (op_set == NULL) {
        SHR_IF_ERR_EXIT(SHR_E_FULL);
    }

    sal_strncpy(op_set->feature_name, feature,
                BCMBD_FWM_FWINFO_FW_TYPE_MAX - 1);
    op_set->feature_name[BCMBD_FWM_FWINFO_FW_TYPE_MAX - 1] = 0;

    /* Find empty op slot or overwrite it. */
    op_entry = op_set->feature_op;
    empty_slot = -1;
    match = -1;
    for (i = 0; i < BCMBD_FWM_FW_OP_NUM_MAX; i++) {
        if ((empty_slot < 0) && !sal_strcasecmp(op_entry[i].op_name, "")) {
            empty_slot = i;
        }
        if (!sal_strcasecmp(op_entry[i].op_name, op->op_name)) {
            /* Overwrite op */
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                        "Overwrite custom op(%s) on %s.\n"),
                        op->op_name, feature));
            sal_strncpy(op_entry[i].op_help, op->op_help,
                        BCMBD_FWM_FW_OP_HELP_MAX);
            op_entry[i].op_cb = op->op_cb;
            match = 1;
            break;
        }
    }

    /* Add new op */
    if (match < 0) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                    "Add custom op(%s) on %s.\n"), op->op_name, feature));
        sal_strncpy(op_entry[empty_slot].op_name, op->op_name,
                    BCMBD_FWM_FW_OP_NAME_MAX);
        sal_strncpy(op_entry[empty_slot].op_help, op->op_help,
                    BCMBD_FWM_FW_OP_HELP_MAX);
        op_entry[empty_slot].op_cb = op->op_cb;
    }
exit:
    if (locked && fw_featues->op_mutex) {
        sal_mutex_give(fw_featues->op_mutex);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/* Perform custom operations on the specified feature instance. */
int
bcmbd_fwm_fw_op_exec(int unit, const char *app, const char *op, int param)
{
    const char *feature_str = NULL;
    char *ret_str;
    char *save_str = NULL;
    int i_idx = -1;
    bcmbd_fwm_fw_op_t *op_entry;
    int match_drv_slot;
    int i;
    int locked = 0;

    bcmbd_fwm_fw_op_features_t *fw_featues = NULL;
    bcmbd_fwm_fw_op_feature_t *op_set = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(app, SHR_E_PARAM);
    SHR_NULL_CHECK(op, SHR_E_PARAM);

    fw_featues = bcmbd_fwm_fw_op_db[unit];
    SHR_NULL_CHECK(fw_featues, SHR_E_INIT);

    SHR_NULL_CHECK(fw_featues->op_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_featues->op_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    ret_str = sal_strchr(app, '.');
    if (ret_str == NULL) {
        /* No instance specified, assigned instance 0. */
        feature_str = app;
        i_idx = 0;
    } else {
        /* Firmware.Instance */
        feature_str = sal_strtok_r((char *)app, ".", &save_str);
        i_idx = sal_ctoi(sal_strtok_r(NULL, ".", &save_str), NULL);
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Do firmware op %s on %s.%d.\n"),
                            op, feature_str, i_idx));

    /* Find the matched feature slot. */
    match_drv_slot = -1;
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        op_set = &fw_featues->feature[i];
        if (!sal_strcasecmp(feature_str, op_set->feature_name)) {
            match_drv_slot = i;
            break;
        }
    }

    if (match_drv_slot < 0) {
        /* No matched custom op found. Call default ops(start, stop, reset). */
        if (!sal_strcasecmp(op, "start")) {
            SHR_IF_ERR_EXIT
                (bcmbd_fwm_fw_op_default_start(unit, feature_str, i_idx,
                                               op, 0, NULL));
        } else if (!sal_strcasecmp(op, "stop")) {
            SHR_IF_ERR_EXIT
                (bcmbd_fwm_fw_op_default_stop(unit, feature_str, i_idx,
                                              op, 0, NULL));
        } else if (!sal_strcasecmp(op, "reset")) {
            SHR_IF_ERR_EXIT
                (bcmbd_fwm_fw_op_default_reset(unit, feature_str, i_idx,
                                               op, 0, NULL));
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Firmware op %s is unrecognized.\n"), op));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        op_entry = op_set->feature_op;
        for (i = 0; i < BCMBD_FWM_FW_OP_NUM_MAX; i++) {
            if (!sal_strcasecmp(op_entry[i].op_name, op)) {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                            "Do custom op(%s) param(%d) on %s.%d.\n"),
                            op, param, feature_str, i_idx));
                SHR_IF_ERR_EXIT
                    (op_entry[i].op_cb(unit, feature_str, i_idx,
                                       op, param, NULL));
                break;
            }
        }

        if (i >= BCMBD_FWM_FW_OP_NUM_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Firmware op %s is not supported by firmware "
                                  "instance %s.%d.\n"),
                                  op, feature_str, i_idx));
            SHR_IF_ERR_EXIT(SHR_E_PARAM);
        }
    }

    /* Notify chip driver. */
    if (!sal_strcasecmp(op, "start")) {
        SHR_IF_ERR_EXIT
            (bcmbd_fwm_notif_op(unit,
                                feature_str,
                                i_idx,
                                BCMBD_FWM_FW_NOTIF_OPTION_STARTED,
                                NULL));
    }
    if (!sal_strcasecmp(op, "stop") || !sal_strcasecmp(op, "reset")) {
        SHR_IF_ERR_EXIT
            (bcmbd_fwm_notif_op(unit,
                                feature_str,
                                i_idx,
                                BCMBD_FWM_FW_NOTIF_OPTION_STOPPED,
                                NULL));
    }

exit:
    if (locked && fw_featues->op_mutex) {
        sal_mutex_give(fw_featues->op_mutex);
    }
    SHR_FUNC_EXIT();
}

/* Get the number of operations of specific firmware feature. */
int
bcmbd_fwm_fw_op_count_get(int unit, const char *feature, int *count)
{
    int op_cnt = 0;
    int match_drv_slot;
    int i;
    bcmbd_fwm_fw_op_t *op_entry;
    bcmbd_fwm_fw_op_features_t *fw_featues = NULL;
    bcmbd_fwm_fw_op_feature_t *op_set = NULL;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    fw_featues = bcmbd_fwm_fw_op_db[unit];
    SHR_NULL_CHECK(fw_featues, SHR_E_INIT);

    SHR_NULL_CHECK(fw_featues->op_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_featues->op_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Find the matched feature slot. */
    match_drv_slot = -1;
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        op_set = &fw_featues->feature[i];
        if (!sal_strcasecmp(feature, op_set->feature_name)) {
            match_drv_slot = i;
            break;
        }
    }

    if (match_drv_slot < 0) {
        /*
         * The FW feature doesn't register custom ops.
         * Return default supported ops(start, stop, reset).
         */
        op_cnt = DEFAULT_OP_IDX_COUNT;
    } else {
        op_entry = op_set->feature_op;
        for (i = 0; i < BCMBD_FWM_FW_OP_NUM_MAX; i++) {
            if (op_entry[i].op_cb) {
                op_cnt++;
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Firmware %s supported %d ops.\n"),
                            feature, op_cnt));
    *count = op_cnt;

exit:
    if (locked && fw_featues->op_mutex) {
        sal_mutex_give(fw_featues->op_mutex);
    }
    SHR_FUNC_EXIT();
}
/* Get the operation information. */
int
bcmbd_fwm_fw_op_get(int unit, const char *feature, int op_idx,
                    bcmbd_fwm_fw_op_t *op)
{
    int match_drv_slot;
    int i;
    bcmbd_fwm_fw_op_t *op_entry;
    bcmbd_fwm_fw_op_features_t *fw_featues = NULL;
    bcmbd_fwm_fw_op_feature_t *op_set = NULL;
    int max_op_cnt = 0;
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate parameters */
    SHR_NULL_CHECK(feature, SHR_E_PARAM);
    SHR_NULL_CHECK(op, SHR_E_PARAM);

    fw_featues = bcmbd_fwm_fw_op_db[unit];
    SHR_NULL_CHECK(fw_featues, SHR_E_INIT);

    SHR_NULL_CHECK(fw_featues->op_mutex, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (sal_mutex_take(fw_featues->op_mutex, SAL_MUTEX_FOREVER));
    locked = 1;

    /* Find the matched feature slot. */
    match_drv_slot = -1;
    for (i = 0; i < BCMBD_FWM_FW_FEATURE_NUM_MAX; i++) {
        op_set = &fw_featues->feature[i];
        if (!sal_strcasecmp(feature, op_set->feature_name)) {
            match_drv_slot = i;
            break;
        }
    }

    if (match_drv_slot < 0) {
        /*
         * The FW feature doesn't register custom ops.
         * Supported ops are defaults (start, stop, reset).
         */
        max_op_cnt = DEFAULT_OP_IDX_COUNT;
    } else {
        op_entry = op_set->feature_op;
        for (i = 0; i < BCMBD_FWM_FW_OP_NUM_MAX; i++) {
            if (op_entry[i].op_cb) {
                max_op_cnt++;
            }
        }
    }

    if ((op_idx < 0) || (op_idx > max_op_cnt)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (match_drv_slot < 0) {
        /*
         * The FW feature doesn't register custom ops.
         * Return default supported ops(start, stop, reset).
         */
        if (op_idx == DEFAULT_OP_IDX_START) {
            sal_snprintf(op->op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                         DEFAULT_OP_START_NAME);
            sal_snprintf(op->op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                         DEFAULT_OP_START_HELP);
            op->op_cb = bcmbd_fwm_fw_op_default_start;
        } else if (op_idx == DEFAULT_OP_IDX_STOP) {
            sal_snprintf(op->op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                         DEFAULT_OP_STOP_NAME);
            sal_snprintf(op->op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                         DEFAULT_OP_STOP_HELP);
            op->op_cb = bcmbd_fwm_fw_op_default_stop;
        } else {
            /* op_idx == DEFAULT_OP_IDX_RESET */
            sal_snprintf(op->op_name, BCMBD_FWM_FW_OP_NAME_MAX,
                         DEFAULT_OP_RESET_NAME);
            sal_snprintf(op->op_help, BCMBD_FWM_FW_OP_HELP_MAX,
                         DEFAULT_OP_RESET_HELP);
            op->op_cb = bcmbd_fwm_fw_op_default_reset;
        }
    } else {
        op_entry = op_set->feature_op;
        sal_memcpy(op, &op_entry[op_idx], sizeof(bcmbd_fwm_fw_op_t));
    }
exit:
    if (locked && fw_featues->op_mutex) {
        sal_mutex_give(fw_featues->op_mutex);
    }
    SHR_FUNC_EXIT();
}
